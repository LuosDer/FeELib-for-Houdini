
#pragma once

#ifndef __GFE_PointInMesh_h__
#define __GFE_PointInMesh_h__

//#include "GFE/GFE_PointInMesh.h"

#include "GA/GA_Detail.h"
#include "GA/GA_PageHandle.h"
#include "GA/GA_PageIterator.h"


//#include "GFE/GFE_Type.h
#include "GFE/GFE_GroupParse.h"
#include "GFE/GFE_WindingNumber.h"
#include "GFE/GFE_AttributeCopy.h"


class GFE_PointInMesh {

public:
    GFE_PointInMesh(
        GA_Detail* const geoPoint,
        const GA_Detail* const geoRefMesh = nullptr,
        const SOP_NodeVerb::CookParms* const cookparms = nullptr,
        GFE_WindingNumber_Cache* const sopcache = nullptr,
        //const GA_PointGroup * geoPointGroup = nullptr,
        //const GA_PrimitiveGroup* geoRefMeshGroup = nullptr,
        const exint subscribeRatio = 64,
        const exint minGrainSize = 64
        //,const bool outWN = false,
        //const UT_StringHolder & wnName = "__topo_windingNumber",
        //const GA_Storage wnStorage = GA_STORE_REAL64,
        //const GFE_WindingNumberType wnType = GFE_WNType_XYZ,
        //const bool fullAccuracy = false,
        //const fpreal accuracyScale = 2.0,
        //const bool asSolidAngle = false,
        //const bool negate = false
    )
        : geoPoint(geoPoint)
        , geoRefMesh(geoRefMesh)
        , cookparms(cookparms)
        , sopcache(sopcache)
        //, geoPointGroup(geoPointGroup)
        //, geoRefMeshGroup(geoRefMeshGroup)
        , subscribeRatio(subscribeRatio)
        , minGrainSize(minGrainSize)
        //, outWN(outWN)
        //, wnName(wnName)
        //, wnStorage(wnStorage)
        //, wnType(wnType)
        //, fullAccuracy(fullAccuracy)
        //, accuracyScale(accuracyScale)
        //, asSolidAngle(asSolidAngle)
        //, negate(negate)
    {
        UT_ASSERT_MSG(geoPoint, "do not find geo");
        if (!geoRefMesh)
            this->geoRefMesh = geoPoint;
    }

    GFE_PointInMesh(
        const SOP_NodeVerb::CookParms& cookparms,
        GA_Detail* const geoPoint,
        const GA_Detail* const geoRefMesh = nullptr,
        GFE_WindingNumber_Cache* const sopcache = nullptr,
        const exint subscribeRatio = 64,
        const exint minGrainSize = 64
    )
        : geoPoint(geoPoint)
        , geoRefMesh(geoRefMesh)
        , cookparms(&cookparms)
        , sopcache(sopcache)
        , subscribeRatio(subscribeRatio)
        , minGrainSize(minGrainSize)
    {
        UT_ASSERT_MSG(geoPoint, "do not find geo");
        if (!geoRefMesh)
            this->geoRefMesh = geoPoint;
    }



    ~GFE_PointInMesh()
    {
        if (!outWN && wnAttribPtr)
        {
            geoPoint->getAttributes().destroyAttribute(wnAttribPtr);
        }
        if (!outGroup && pointInMeshGroup)
        {
            geoPoint->destroyGroup(pointInMeshGroup);
        }
        if (!outNumericAttrib && numericAttrib)
        {
            geoPoint->getAttributes().destroyAttribute(numericAttrib);
        }
    }


    void
        setInGroup()
    {
#if 1
        hasCalcGroup = true;

        groupParse.setParm(cookparms);

        groupParse.setParm(geoPoint);
        groupParse.setParm(geoPointGroupName);
        geoPointGroup = groupParse.parsePoint();

        groupParse.setParm(geoRefMesh);
        groupParse.setParm(geoRefMeshGroupName);
        geoRefMeshGroup = groupParse.parsePrimitive();

#else
        if (cookparms)
        {
            hasCalcGroup = true;
            GOP_Manager gop;
            geoPointGroup = GFE_GroupParse_Namespace::findOrParsePointGroupDetached(*cookparms, geoPoint, geoPointGroupName, gop);
            geoRefMeshGroup = GFE_GroupParse_Namespace::findOrParsePrimitiveGroupDetached(*cookparms, geoRefMesh, geoRefMeshGroupName, gop);
        }
#endif

    }

    void
        setInputGroup(
            const GA_PointGroup* const geoPointGroup,
            const GA_PrimitiveGroup* const geoRefMeshGroup
        )
    {
        hasCalcGroup = true;
        this->geoPointGroup = geoPointGroup;
        this->geoRefMeshGroup = geoRefMeshGroup;
    }

    void
        setWNAttribPtr(
            GA_Attribute* const wnAttribPtr
        )
    {
        this->wnAttribPtr = wnAttribPtr;
    }

    void
        setInGroup(
            const UT_StringHolder& geoPointGroupName,
            const UT_StringHolder& geoRefMeshGroupName
        )
    {
        if (cookparms)
        {
            this->geoPointGroupName = geoPointGroupName;
            this->geoRefMeshGroupName = geoRefMeshGroupName;
            setInGroup();
        }
    }


    void
        setWNAttrib(
            const bool outWN = false,
            const UT_StringHolder& wnName = "__topo_windingNumber",
            const GA_Storage wnStorage = GA_STORE_REAL64,
            const GFE_WindingNumberType wnType = GFE_WNType_XYZ,
            const bool fullAccuracy = false,
            const fpreal accuracyScale = 2.0,
            const bool asSolidAngle = false,
            const bool negate = false
        )
    {
        hasInitialParm_wn = true;
        this->outWN = outWN;
        this->wnName = wnName;
        this->wnStorage = wnStorage;
        this->wnType = wnType;
        this->fullAccuracy = fullAccuracy;
        this->accuracyScale = accuracyScale;
        this->asSolidAngle = asSolidAngle;
        this->negate = negate;
    }

    GA_Attribute*
        addWNAttrib()
    {
        if (!hasCalcGroup)
            setInGroup();

        if (!hasInitialParm_wn)
            setWNAttrib();

        wnAttribPtr = GFE_WindingNumber::addAttribWindingNumber(
            geoPoint, geoRefMesh, sopcache,
            geoPointGroup, geoRefMeshGroup,
            wnStorage, wnName,
            wnType, fullAccuracy, accuracyScale, asSolidAngle, negate);

        return wnAttribPtr;
    }



    void
        setOutGroup(
            const bool outGroup = false,
            const UT_StringHolder& groupName = "pointInMesh_wn",
            const bool inGeo = true,
            const bool onGeo = true,
            const fpreal threshold = 1e-05,
            const bool reverseGroup = false,
            const GFE_GroupMergeType groupMergeType = GFE_GroupMerge_Replace
        )
    {
        hasInitialParm_group = true;
        this->outGroup = outGroup;
        this->groupName = groupName;
        this->inGeo = inGeo;
        this->onGeo = onGeo;
        this->threshold = threshold;
        this->reverseGroup = reverseGroup;
        this->groupMergeType = groupMergeType;
    }


    GA_PointGroup*
        addOutGroup()
    {
        if (!hasCalcGroup)
            setInGroup();

        if (!wnAttribPtr)
            addWNAttrib();

        if (!hasInitialParm_group)
            setOutGroup();

        pointInMeshGroup = geoPoint->newPointGroup(groupName);

        //GA_Attribute* wnAttribPtrTmp = wnAttribPtr;
        //GA_PointGroup* pointInMeshGroupTmp = pointInMeshGroup;
        //const fpreal thresholdTmp = this->threshold;

        const GA_SplittableRange geoPointRange(geoPoint->getPointRange(geoPointGroup));
        UTparallelFor(geoPointRange, [this](const GA_SplittableRange& r)
        {
            GA_PageHandleT<fpreal, fpreal, true, false, const GA_Attribute, const GA_ATINumeric, const GA_Detail> attrib_ph(wnAttribPtr);
            for (GA_PageIterator pit = r.beginPages(); !pit.atEnd(); ++pit)
            {
                GA_Offset start, end;
                for (GA_Iterator it(pit.begin()); it.blockAdvance(start, end); )
                {
                    attrib_ph.setPage(start);
                    for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                    {
                        fpreal windingNumber = attrib_ph.value(elemoff);
                        windingNumber = abs(windingNumber);

                        if (asSolidAngle) {
                            windingNumber /= PI * 2.0;
                        }
                        
                        bool outval;
                        if (inGeo && onGeo)
                        {
                            outval = windingNumber > 0.5 - threshold;
                        }
                        else
                        {
                            if (inGeo)
                            {
                                outval = windingNumber > 0.5 + threshold;
                            }
                            if (onGeo)
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

        
        return pointInMeshGroup;
    }



    void
        setNumericAttrib(
            const bool outNumericAttrib = false,
            const GA_Storage numericStorage = GA_STORE_INVALID,
            const UT_StringHolder& numericAttribName = "pointInMesh_wn"
        )
    {
        hasInitialParm_numeric = true;
        this->outNumericAttrib = outNumericAttrib;
        this->numericStorage = numericStorage;
        this->numericAttribName = numericAttribName;
    }

    GA_Attribute*
        addNumericAttrib()
    {
        if (!hasCalcGroup)
            setInGroup();

        if (!pointInMeshGroup)
            addOutGroup();

        if (!hasInitialParm_numeric)
            setNumericAttrib();

        numericAttrib = geoPoint->getAttributes().createTupleAttribute(GA_ATTRIB_POINT, numericAttribName, numericStorage, 1, GA_Defaults(0));

        GFE_AttributeCopy::copyAttribute(numericAttrib, pointInMeshGroup);

        return numericAttrib;
    }




    void
        compute()
    {
        if (outNumericAttrib)
        {
            addNumericAttrib();
        }
        if (outGroup)
        {
            addOutGroup();
        }
        if (outWN)
        {
            addWNAttrib();
        }
    }

    void
        bumpDataId()
    {
        if (outWN && wnAttribPtr)
        {
            wnAttribPtr->bumpDataId();
        }
        if (outGroup && pointInMeshGroup)
        {
            pointInMeshGroup->bumpDataId();
        }
        if (outNumericAttrib && numericAttrib)
        {
            numericAttrib->bumpDataId();
        }
    }

    void
        visualizeGroup()
    {
        if (outGroup && cookparms && pointInMeshGroup)
        {
            cookparms->getNode()->setHighlight(true);
            cookparms->select(*pointInMeshGroup);
        }
    }


private:
    GA_Detail* geoPoint;
    const GA_Detail* geoRefMesh;
    GFE_WindingNumber_Cache* sopcache;


    bool hasCalcGroup = false;
    const SOP_NodeVerb::CookParms* cookparms;
    GFE_GroupParse groupParse;
    UT_StringHolder geoPointGroupName;
    UT_StringHolder geoRefMeshGroupName;
    const GA_PointGroup* geoPointGroup = nullptr;
    const GA_PrimitiveGroup* geoRefMeshGroup = nullptr;



    bool outWN;
    bool hasInitialParm_wn = false;
    GA_Attribute* wnAttribPtr = nullptr;
    UT_StringHolder wnName;
    GA_Storage wnStorage;
    GFE_WindingNumberType wnType;
    bool fullAccuracy;
    fpreal accuracyScale;
    bool asSolidAngle;
    bool negate;



    bool outGroup;
    bool hasInitialParm_group = false;
    GA_PointGroup* pointInMeshGroup = nullptr;
    UT_StringHolder groupName;
    bool inGeo;
    bool onGeo;
    fpreal threshold;
    bool reverseGroup;
    GFE_GroupMergeType groupMergeType;


    const exint subscribeRatio;
    const exint minGrainSize;


    bool outNumericAttrib = false;
    bool hasInitialParm_numeric = false;
    GA_Attribute* numericAttrib = nullptr;
    GA_Storage numericStorage;
    UT_StringHolder numericAttribName;




}; // End of class GFE_PointInMesh




























#if 0

namespace GFE_PointInMesh_Namespace {




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
            const bool inGeo = true,
            const bool onGeo = true,
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
            const bool inGeo = true,
            const bool onGeo = true,
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
            const bool inGeo = true,
            const bool onGeo = true,
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
        const bool inGeo = true,
        const bool onGeo = true,
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
            groupMergeType, inGeo, onGeo, threshold,
            wnStorage, wnType,
            fullAccuracy, accuracyScale,
            asSolidAngle, negate);

        pointInMesh_wn_groupPtr->bumpDataId();

        return pointInMesh_wn_groupPtr;
    }


} // End of namespace GFE_PointInMesh

#endif

#endif
