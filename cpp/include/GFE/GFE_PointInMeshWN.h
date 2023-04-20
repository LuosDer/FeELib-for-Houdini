
#pragma once

#ifndef __GFE_PointInMeshWN_h__
#define __GFE_PointInMeshWN_h__

//#include "GFE/GFE_PointInMeshWN.h"

#include "GA/GA_Detail.h"
#include "GA/GA_PageHandle.h"
#include "GA/GA_PageIterator.h"


#include "GFE/GFE_GeoFilter.h"
#include "GFE/GFE_WindingNumber.h"
#include "GFE/GFE_AttributeCopy.h"



#if 1

class GFE_PointInMeshWN : public GFE_WindingNumber {

public:

    using GFE_WindingNumber::GFE_WindingNumber; 


    void
        setOutGroup(
            const bool outGroupDetached = false,
            const UT_StringHolder& groupName = "pointInMesh_wn",
            const bool groupInGeoPoint = true,
            const bool groupOnGeoPoint = true,
            const fpreal threshold = 1e-05,
            const bool reverseGroup = false,
            const GFE_GroupMergeType groupMergeType = GFE_GroupMerge_Replace
        )
    {
        hasParm_outGroup = true;
        this->outGroupDetached = outGroupDetached;
        this->groupName = groupName;
        this->groupInGeoPoint = groupInGeoPoint;
        this->groupOnGeoPoint = groupOnGeoPoint;
        this->threshold = threshold;
        this->reverseGroup = reverseGroup;
        this->groupMergeType = groupMergeType;
    }

protected:

    template<typename FLOAT_T>
    void
        computeOutGroup()
    {
        const GA_SplittableRange geoPointRange(geoPoint->getPointRange(geoPointGroup));
        UTparallelFor(geoPointRange, [this](const GA_SplittableRange& r)
        {
            GA_PageHandleT<FLOAT_T, FLOAT_T, true, false, const GA_Attribute, const GA_ATINumeric, const GA_Detail> attrib_ph(gfeWN.getAttrib());
            for (GA_PageIterator pit = r.beginPages(); !pit.atEnd(); ++pit)
            {
                GA_Offset start, end;
                for (GA_Iterator it(pit.begin()); it.blockAdvance(start, end); )
                {
                    attrib_ph.setPage(start);
                    for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                    {
                        FLOAT_T windingNumber = attrib_ph.value(elemoff);
                        windingNumber = abs(windingNumber);

                        if (gfeWN.asSolidAngle) {
                            windingNumber /= PI * 2.0;
                        }

                        bool outval;
                        if (groupInGeoPoint && groupOnGeoPoint)
                        {
                            outval = windingNumber > 0.5 - threshold;
                        }
                        else
                        {
                            if (groupInGeoPoint)
                            {
                                outval = windingNumber > 0.5 + threshold;
                            }
                            if (groupOnGeoPoint)
                            {
                                outval = windingNumber > (0.5 - threshold) && windingNumber < (0.5 + threshold);
                            }
                        }

                        outval ^= reverseGroup;

                        switch (groupMergeType)
                        {
                        case GFE_GroupMerge_Replace:
                            break;
                        case GFE_GroupMerge_Union:
                            outval = outval || pointInMeshGroup->contains(elemoff);
                            break;
                        case GFE_GroupMerge_Intersect:
                            outval = outval && pointInMeshGroup->contains(elemoff);
                            break;
                        case GFE_GroupMerge_Subtract:
                            outval = !outval && pointInMeshGroup->contains(elemoff);
                            break;
                        default:
                            break;
                        }

                        pointInMeshGroup->setElement(elemoff, outval);

                        //if (outIntAttrib && intAttrib) {
                        //    intAttrib_ph.value() = outval;
                        //}
                    }
                }
            }
        }, subscribeRatio, minGrainSize);
    }

public:


    GA_PointGroup*
        addOutGroup()
    {
        if (!hasParm_inGroup)
            setInGroup();

        if (!gfeWN.getHasComputed())
            gfeWN.compute();

        if (!hasParm_outGroup)
            setOutGroup();

        if (outGroupDetached)
        {
            pointInMeshGroupUPtr = geoPoint->createDetachedPointGroup();
            pointInMeshGroup = pointInMeshGroupUPtr.get();
        }
        else
        {
            pointInMeshGroup = geoPoint->newPointGroup(groupName);
        }

        switch (gfeWN.getAttrib()->getAIFTuple()->getStorage(gfeWN.getAttrib()))
        {
        case GA_STORE_REAL16:
            computeOutGroup<fpreal16>();
            break;
        case GA_STORE_REAL32:
            computeOutGroup<fpreal32>();
            break;
        case GA_STORE_REAL64:
            computeOutGroup<fpreal64>();
            break;
        default:
            break;
        }

        return pointInMeshGroup;
    }



    void
        setNumericAttrib(
            const bool outNumericDetached = false,
            const GA_Storage numericStorage = GA_STORE_INVALID,
            const UT_StringHolder& numericAttribName = "pointInMesh_wn"
        )
    {
        hasParm_outNumeric = true;
        this->outNumericDetached = outNumericDetached;
        this->numericStorage = numericStorage;
        this->numericAttribName = numericAttribName;
    }

    GA_Attribute*
        addNumericAttrib()
    {
        if (!pointInMeshGroup)
            addOutGroup();

        if (!hasParm_outNumeric)
            setNumericAttrib();

        if (outNumericDetached)
        {
            numericAttribUPtr = geoPoint->createDetachedTupleAttribute(GA_ATTRIB_POINT, numericStorage, 1, GA_Defaults(0));
            numericAttrib = numericAttribUPtr.get();
        }
        else
        {
            numericAttrib = geoPoint->getAttributes().createTupleAttribute(GA_ATTRIB_POINT, numericAttribName, numericStorage, 1, GA_Defaults(0));
        }

        GFE_AttributeCopy::copyAttribute(numericAttrib, pointInMeshGroup);

        return numericAttrib;
    }




    void
        compute()
    {
        addNumericAttrib();
        addOutGroup();
    }

    void
        bumpDataId()
    {
        gfeWN.bumpDataId();

        if (pointInMeshGroup && !outGroupDetached)
        {
            pointInMeshGroup->bumpDataId();
        }
        if (numericAttrib && !outNumericDetached)
        {
            numericAttrib->bumpDataId();
        }
    }

    void
        visualizeGroup() const
    {
        if (!outGroupDetached && cookparms && pointInMeshGroup)
        {
            cookparms->getNode()->setHighlight(true);
            cookparms->select(*pointInMeshGroup);
        }
    }


private:

    bool hasParm_outGroup = false;
    bool outGroupDetached = false;
    //UT_StringHolder groupName;
    bool groupInGeoPoint = true;
    bool groupOnGeoPoint = true;
    fpreal threshold;
    bool reverseGroup;
    GFE_GroupMergeType groupMergeType;


    bool hasParm_outNumeric = false;
    bool outNumericDetached = false;
    GA_AttributeUPtr numericAttribUPtr;
    GA_Attribute* numericAttrib = nullptr;
    GA_Storage numericStorage;
    UT_StringHolder numericAttribName;




}; // End of class GFE_PointInMeshWN


#else

class GFE_PointInMeshWN : public GFE_AttribFilter, public GFE_GeoFilterRef {

public:

    GFE_PointInMeshWN(
        GA_Detail* const geoPoint,
        const GA_Detail* const geoRefMesh = nullptr,
        GFE_WindingNumber_Cache* const sopcache = nullptr,
        const SOP_NodeVerb::CookParms* const cookparms = nullptr,
    )
        : geoPoint(geoPoint)
        , geoRefMesh(geoRefMesh ? geoRefMesh : geoPoint)
        , cookparms(cookparms)
        , gfeWN(geoPoint, this->geoRefMesh, nullptr, cookparms)
    {
        UT_ASSERT_MSG(geoPoint, "do not find geo");
    }




    ~GFE_PointInMeshWN()
    {
    }


    void
        setInGroup(
            const GA_PointGroup* const geoPointGroup = nullptr,
            const GA_PrimitiveGroup* const geoRefMeshGroup = nullptr
        )
    {
        hasParm_inGroup = true;
        this->geoPointGroup = geoPointGroup;
        this->geoRefMeshGroup = geoRefMeshGroup;
        gfeWN.setInGroup(geoPointGroup, geoRefMeshGroup);
    }

    void
        setInGroup(
            const UT_StringHolder& geoPointGroupName,
            const UT_StringHolder& geoRefMeshGroupName
        )
    {
#if 1
        hasParm_inGroup = true;

        groupParse.setParm(cookparms);

        groupParse.setParm(geoPoint);
        groupParse.setParm(geoPointGroupName);
        geoPointGroup = groupParse.parsePointConst();

        groupParse.setParm(geoRefMesh);
        groupParse.setParm(geoRefMeshGroupName);
        geoRefMeshGroup = groupParse.parsePrimitiveConst();
#else
        if (cookparms)
        {
            hasParm_inGroup = true;
            GOP_Manager gop;
            geoPointGroup = GFE_GroupParse_Namespace::findOrParsePointGroupDetached(*cookparms, geoPoint, geoPointGroupName, gop);
            geoRefMeshGroup = GFE_GroupParse_Namespace::findOrParsePrimitiveGroupDetached(*cookparms, geoRefMesh, geoRefMeshGroupName, gop);
        }
#endif
        gfeWN.setInGroup(geoPointGroup, geoRefMeshGroup);
    }

    
    GFE_WindingNumber&
        getGFEWN()
    {
        return gfeWN;
    }


    //void
    //    setWNAttrib(
    //        const bool outWN = false,
    //        const GA_Storage wnStorage = GA_STORE_REAL64,
    //        const UT_StringHolder& wnName = "__topo_windingNumber",
    //        const GFE_WindingNumberType wnType = GFE_WNType_XYZ,
    //        const bool fullAccuracy = false,
    //        const fpreal accuracyScale = 2.0,
    //        const bool asSolidAngle = false,
    //        const bool negate = false
    //    )
    //{
    //    hasInitialParm_wn = true;
    //    this->outWN = outWN;
    //    this->wnName = wnName;
    //    this->wnStorage = wnStorage;
    //    this->wnType = wnType;
    //    this->fullAccuracy = fullAccuracy;
    //    this->accuracyScale = accuracyScale;
    //    this->asSolidAngle = asSolidAngle;
    //    this->negate = negate;
    //}

    void
        computeWNAttrib()
    {
        if (!hasParm_inGroup)
            setInGroup();

        gfeWN.compute();
    }

    //void
    //    setOutWN(bool outWN)
    //{
    //    this->outWN = outWN;
    //}

    void
        setOutGroup(
            const bool outGroupDetached = false,
            const UT_StringHolder& groupName = "pointInMesh_wn",
            const bool groupInGeoPoint = true,
            const bool groupOnGeoPoint = true,
            const fpreal threshold = 1e-05,
            const bool reverseGroup = false,
            const GFE_GroupMergeType groupMergeType = GFE_GroupMerge_Replace
        )
    {
        hasParm_outGroup = true;
        this->outGroupDetached = outGroupDetached;
        this->groupName = groupName;
        this->groupInGeoPoint = groupInGeoPoint;
        this->groupOnGeoPoint = groupOnGeoPoint;
        this->threshold = threshold;
        this->reverseGroup = reverseGroup;
        this->groupMergeType = groupMergeType;
    }

protected:

    template<typename FLOAT_T>
    void
        computeOutGroup()
    {
        const GA_SplittableRange geoPointRange(geoPoint->getPointRange(geoPointGroup));
        UTparallelFor(geoPointRange, [this](const GA_SplittableRange& r)
        {
            GA_PageHandleT<FLOAT_T, FLOAT_T, true, false, const GA_Attribute, const GA_ATINumeric, const GA_Detail> attrib_ph(gfeWN.getAttrib());
            for (GA_PageIterator pit = r.beginPages(); !pit.atEnd(); ++pit)
            {
                GA_Offset start, end;
                for (GA_Iterator it(pit.begin()); it.blockAdvance(start, end); )
                {
                    attrib_ph.setPage(start);
                    for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                    {
                        FLOAT_T windingNumber = attrib_ph.value(elemoff);
                        windingNumber = abs(windingNumber);

                        if (gfeWN.asSolidAngle) {
                            windingNumber /= PI * 2.0;
                        }

                        bool outval;
                        if (groupInGeoPoint && groupOnGeoPoint)
                        {
                            outval = windingNumber > 0.5 - threshold;
                        }
                        else
                        {
                            if (groupInGeoPoint)
                            {
                                outval = windingNumber > 0.5 + threshold;
                            }
                            if (groupOnGeoPoint)
                            {
                                outval = windingNumber > (0.5 - threshold) && windingNumber < (0.5 + threshold);
                            }
                        }

                        outval ^= reverseGroup;

                        switch (groupMergeType)
                        {
                        case GFE_GroupMerge_Replace:
                            break;
                        case GFE_GroupMerge_Union:
                            outval = outval || pointInMeshGroup->contains(elemoff);
                            break;
                        case GFE_GroupMerge_Intersect:
                            outval = outval && pointInMeshGroup->contains(elemoff);
                            break;
                        case GFE_GroupMerge_Subtract:
                            outval = !outval && pointInMeshGroup->contains(elemoff);
                            break;
                        default:
                            break;
                        }

                        pointInMeshGroup->setElement(elemoff, outval);

                        //if (outIntAttrib && intAttrib) {
                        //    intAttrib_ph.value() = outval;
                        //}
                    }
                }
            }
        }, subscribeRatio, minGrainSize);
    }

public:


    GA_PointGroup*
        addOutGroup()
    {
        if (!hasParm_inGroup)
            setInGroup();

        if (!gfeWN.getHasComputed())
            gfeWN.compute();

        if (!hasParm_outGroup)
            setOutGroup();

        if (outGroupDetached)
        {
            pointInMeshGroupUPtr = geoPoint->createDetachedPointGroup();
            pointInMeshGroup = pointInMeshGroupUPtr.get();
        }
        else
        {
            pointInMeshGroup = geoPoint->newPointGroup(groupName);
        }

        switch (gfeWN.getAttrib()->getAIFTuple()->getStorage(gfeWN.getAttrib()))
        {
        case GA_STORE_REAL16:
            computeOutGroup<fpreal16>();
            break;
        case GA_STORE_REAL32:
            computeOutGroup<fpreal32>();
            break;
        case GA_STORE_REAL64:
            computeOutGroup<fpreal64>();
            break;
        default:
            break;
        }
        
        return pointInMeshGroup;
    }



    void
        setNumericAttrib(
            const bool outNumericDetached = false,
            const GA_Storage numericStorage = GA_STORE_INVALID,
            const UT_StringHolder& numericAttribName = "pointInMesh_wn"
        )
    {
        hasParm_outNumeric = true;
        this->outNumericDetached = outNumericDetached;
        this->numericStorage = numericStorage;
        this->numericAttribName = numericAttribName;
    }

    GA_Attribute*
        addNumericAttrib()
    {
        if (!pointInMeshGroup)
            addOutGroup();

        if (!hasParm_outNumeric)
            setNumericAttrib();

        if (outNumericDetached)
        {
            numericAttribUPtr = geoPoint->createDetachedTupleAttribute(GA_ATTRIB_POINT, numericStorage, 1, GA_Defaults(0));
            numericAttrib = numericAttribUPtr.get();
        }
        else
        {
            numericAttrib = geoPoint->getAttributes().createTupleAttribute(GA_ATTRIB_POINT, numericAttribName, numericStorage, 1, GA_Defaults(0));
        }

        GFE_AttributeCopy::copyAttribute(numericAttrib, pointInMeshGroup);

        return numericAttrib;
    }




    void
        compute()
    {
        addNumericAttrib();
        addOutGroup();
    }

    void
        bumpDataId()
    {
        gfeWN.bumpDataId();
        
        if (pointInMeshGroup && !outGroupDetached)
        {
            pointInMeshGroup->bumpDataId();
        }
        if (numericAttrib && !outNumericDetached)
        {
            numericAttrib->bumpDataId();
        }
    }

    void
        visualizeGroup() const
    {
        if (!outGroupDetached && cookparms && pointInMeshGroup)
        {
            cookparms->getNode()->setHighlight(true);
            cookparms->select(*pointInMeshGroup);
        }
    }


private:
    GA_Detail* geoPoint;
    const GA_Detail* geoRefMesh;
    GFE_WindingNumber_Cache* sopcache;


    bool hasParm_inGroup = false;
    const SOP_NodeVerb::CookParms* cookparms;
    GFE_GroupParse groupParse;
    //UT_StringHolder geoPointGroupName;
    //UT_StringHolder geoRefMeshGroupName;
    const GA_PointGroup* geoPointGroup = nullptr;
    const GA_PrimitiveGroup* geoRefMeshGroup = nullptr;



    GFE_WindingNumber gfeWN;


    //GA_Attribute* wnAttribPtr = nullptr;
    //UT_StringHolder wnName;
    //GA_Storage wnStorage;
    //GFE_WindingNumberType wnType;
    //bool fullAccuracy;
    //fpreal accuracyScale;
    //bool asSolidAngle;
    //bool negate;

    bool hasParm_outGroup = false;
    bool outGroupDetached = false;
    GA_PointGroupUPtr pointInMeshGroupUPtr;
    GA_PointGroup* pointInMeshGroup = nullptr;
    UT_StringHolder groupName;
    bool groupInGeoPoint;
    bool groupOnGeoPoint;
    fpreal threshold;
    bool reverseGroup;
    GFE_GroupMergeType groupMergeType;


    exint subscribeRatio;
    exint minGrainSize;


    bool hasParm_outNumeric = false;
    bool outNumericDetached = false;
    GA_AttributeUPtr numericAttribUPtr;
    GA_Attribute* numericAttrib = nullptr;
    GA_Storage numericStorage;
    UT_StringHolder numericAttribName;




}; // End of class GFE_PointInMeshWN

#endif


























#if 0

namespace GFE_PointInMeshWN_Namespace {




    template<typename FLOAT_T>
    static void
        pointInMesh_wn(
            GA_Detail* const geoPoint,
            const GA_Detail* const geoRefMesh,
            const GA_ROHandleT<FLOAT_T>& wnAttribPtr,
            GA_PointGroup* const group,

            GFE_WindingNumber_Cache* const sopcache = nullptr,
            const GA_PointGroup* const geoPointGroup = nullptr,
            const GA_PrimitiveGroup* const geoRefMeshGroup = nullptr,


            const GFE_GroupMergeType groupMergeType = GFE_GroupMerge_Replace,
            const bool groupInGeoPoint = true,
            const bool groupOnGeoPoint = true,
            const fpreal threshold = 1e-05

        ) {
        UT_ASSERT_P(geoPoint);
        UT_ASSERT_P(geoRefMesh);
        UT_ASSERT_P(group);
    }

    template<typename FLOAT_T>
    static void
        pointInMesh_wn(
            GA_Detail* const geoPoint,
            const GA_Detail* const geoRefMesh,
            const GA_RWHandleT<FLOAT_T>& wnAttribPtr,
            GA_PointGroup* const group,

            GFE_WindingNumber_Cache* const sopcache = nullptr,
            const GA_PointGroup* const geoPointGroup = nullptr,
            const GA_PrimitiveGroup* const geoRefMeshGroup = nullptr,


            const GFE_GroupMergeType groupMergeType = GFE_GroupMerge_Replace,
            const bool groupInGeoPoint = true,
            const bool groupOnGeoPoint = true,
            const fpreal threshold = 1e-05,

            const GFE_WindingNumberType wnType = GFE_WNType_XYZ,
            const bool fullAccuracy = false,
            const fpreal accuracyScale = 2.0,
            const bool asSolidAngle = false,
            const bool negate = false

        ) {
        UT_ASSERT_P(geoPoint);
        UT_ASSERT_P(geoRefMesh);

        if (!outWN && !outGroup && !outIntAttrib)
            return;

        if (!geoRefMesh)
            geoRefMesh = geoPoint;

        UT_Matrix4T<T> mtx;
    }



    static GA_PointGroup*
        pointInMesh_wn(
            GA_Detail* const geoPoint,
            const GA_Detail* const geoRefMesh,
            GFE_WindingNumber_Cache* const sopcache = nullptr,
            const GA_PointGroup* const geoPointGroup = nullptr,
            const GA_PrimitiveGroup* const geoRefMeshGroup = nullptr,

            const bool outWN = false,
            const UT_StringHolder& wnName = "__topo_windingNumber",
            const bool outGroup = true,
            const UT_StringHolder& groupName = "pointInMesh_wn",
            const bool outIntAttrib = false,
            const GA_Storage intStorage = GA_STORE_INVALID,
            const UT_StringHolder& intAttribName = "pointInMesh_wn",

            const GFE_GroupMergeType groupMergeType = GFE_GroupMerge_Replace,
            const bool groupInGeoPoint = true,
            const bool groupOnGeoPoint = true,
            const fpreal threshold = 1e-05,

            const GA_Storage wnStorage = GA_STORE_INVALID,
            const GFE_WindingNumberType wnType = GFE_WNType_XYZ,
            const bool fullAccuracy = false,
            const fpreal accuracyScale = 2.0,
            const bool asSolidAngle = false,
            const bool negate = false
        ) {
        UT_ASSERT_P(geoPoint);
        UT_ASSERT_P(geoRefMesh);

        if (!outWN && !outGroup && !outIntAttrib)
            return nullptr;

        const GA_Attribute* const wnAttribPtr = GFE_WindingNumber::addAttribWindingNumber(
            geoPoint, geoRefMesh, nullptr,
            geoPointGroup, geoRefMeshGroup,
            wnStorage, wnName,
            wnType, fullAccuracy, accuracyScale, asSolidAngle, negate);

        //pointInMesh_wn<fpreal>(geoPoint, geoRefMesh, restGeo, attribToXformPattern, posRestAttribPtr, posRefAttribPtr);

    }

    static GA_PointGroup*
    addOutGroup(
        const SOP_NodeVerb::CookParms& cookparms,
        GA_Detail* const geoPoint,
        const GA_Detail* const geoRefMesh,
        const UT_StringHolder& geoPointGroupName,
        const UT_StringHolder& geoRefMeshGroupName,
        GFE_WindingNumber_Cache* const sopcache = nullptr,

        const bool outWN = false,
        const UT_StringHolder& wnName = "__topo_windingNumber",
        const bool outGroup = true,
        const UT_StringHolder& groupName = "pointInMesh_wn",
        const bool outIntAttrib = false,
        const GA_Storage intStorage = GA_STORE_INVALID,
        const UT_StringHolder& intAttribName = "pointInMesh_wn",

        const GFE_GroupMergeType groupMergeType = GFE_GroupMerge_Replace,
        const bool groupInGeoPoint = true,
        const bool groupOnGeoPoint = true,
        const fpreal threshold = 1e-05,

        const GA_Storage wnStorage = GA_STORE_INVALID,
        const GFE_WindingNumberType wnType = GFE_WNType_XYZ,
        const bool fullAccuracy = false,
        const fpreal accuracyScale = 2.0,
        const bool asSolidAngle = false,
        const bool negate = false
        )
    {
        UT_ASSERT_P(geoPoint);
        UT_ASSERT_P(geoRefMesh);

        if (!outWN && !outGroup && !outIntAttrib)
            return;

        GOP_Manager gop;
        const GA_PointGroup* const geoPointGroup = GFE_GroupParse_Namespace::findOrParsePointGroupDetached(cookparms, geoPoint, geoPointGroupName, gop);
        const GA_PrimitiveGroup* const geoRefMeshGroup = GFE_GroupParse_Namespace::findOrParsePrimitiveGroupDetached(cookparms, geoRefMesh, geoRefMeshGroupName, gop);

        GA_PointGroup* pointInMesh_wn_groupPtr = pointInMesh_wn(
            geoPoint, geoRefMesh, sopcache,
            geoPointGroup, geoRefMeshGroup,
            outWN, wnName,
            outGroup, groupName,
            outIntAttrib, intStorage, intAttribName,
            groupMergeType, groupInGeoPoint, groupOnGeoPoint, threshold,
            wnStorage, wnType,
            fullAccuracy, accuracyScale,
            asSolidAngle, negate);

        pointInMesh_wn_groupPtr->bumpDataId();

        return pointInMesh_wn_groupPtr;
    }


} // End of namespace GFE_PointInMeshWN

#endif

#endif