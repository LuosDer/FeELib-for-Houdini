
#pragma once

#ifndef __GFE_Fuse_h__
#define __GFE_Fuse_h__

#include "GFE/GFE_Fuse.h"

#include "GFE/GFE_GeoFilter.h"



#include "GU/GU_Snap.h"
#include "SOP/SOP_Fuse-2.0.proto.h"

class GFE_Fuse : public GFE_AttribCreateFilterWithRef0 {

//#define __TEMP_GFE_Fuse_GroupName      "__TEMP_GFE_Fuse_Group"
//#define __TEMP_GFE_Fuse_SnapAttribName "__TEMP_GFE_Fuse_SnapTo"
//#define __TEMP_GFE_Fuse_SnapGroupName  "__TEMP_GFE_Fuse_Snapped"

    
public:
    
enum Method
{
    Point,
    Grid,
    Specified,
};
    
    //using GFE_AttribFilter::GFE_AttribFilter;
    
    GFE_Fuse(
        GA_Detail& geo,
        const GA_Detail* const geoRef,
        const SOP_NodeVerb::CookParms* const cookparms = nullptr
    )
        : GFE_AttribCreateFilterWithRef0(geo, geoRef, cookparms)
        , groupParserFuseRef(geoRef ? *geoRef : geo, groupParser.getGOPRef(), cookparms)
        , keepEdgeGroupArray(geo, cookparms)
    {
    }

    void
        setComputeParm(
            const Method method = Method::Point,
            const bool modifyTarget = false,
            const bool consolidate = false,
            const bool deleteConsolidated = true,
            const bool delUnusedPoint = false
        )
    {
        setHasComputed();
        
        this->method = method;
        this->delUnusedPoint = delUnusedPoint;
        comSnapParm.myModifyBothQueryAndTarget = modifyTarget;
        comSnapParm.myConsolidate = consolidate;
        comSnapParm.myDeleteConsolidated = deleteConsolidated;
        
        pointSnapParm.myModifyBothQueryAndTarget = modifyTarget;
        pointSnapParm.myConsolidate = consolidate;
        pointSnapParm.myDeleteConsolidated = deleteConsolidated;
        
        gridSnapParm.myModifyBothQueryAndTarget = modifyTarget;
        gridSnapParm.myConsolidate = consolidate;
        gridSnapParm.myDeleteConsolidated = deleteConsolidated;
        
        specifiedSnapParm.myModifyBothQueryAndTarget = modifyTarget;
        specifiedSnapParm.myConsolidate = consolidate;
        specifiedSnapParm.myDeleteConsolidated = deleteConsolidated;
    }
    
    void
        setPointComputeParm(
            const fpreal distance = 1e-05,
            const GU_Snap::PointSnapParms::SnapAlgorithm algorithm = GU_Snap::PointSnapParms::SnapAlgorithm::ALGORITHM_LOWEST_POINT,
	        const bool mismatch = false
        )
    {
        setHasComputed();
        
        pointSnapParm.myAlgorithm = algorithm;
        pointSnapParm.myDistance = distance;
        pointSnapParm.myMismatch = mismatch;
    }
    
    void
        setGridComputeParm(
            const int gridSnapType,
            const float myXLines,
            const float myYLines,
            const float myZLines,
            const float myXOff,
            const float myYOff,
            const float myZOff,
            const float tol
        )
    {
        setHasComputed();
        
        gridSnapParm.myGridSnapType = gridSnapType;
        gridSnapParm.myXLines = myXLines;
        gridSnapParm.myYLines = myYLines;
        gridSnapParm.myZLines = myZLines;
        gridSnapParm.myXOff   = myXOff;
        gridSnapParm.myYOff   = myYOff;
        gridSnapParm.myZOff   = myZOff;
        gridSnapParm.myTol    = tol;
    }
    
    SYS_FORCE_INLINE void
        setGridComputeParm(
            const int gridSnapType,
            const UT_Vector3D lines,
            const UT_Vector3D offset,
            const float tol
        )
    {
        setGridComputeParm(gridSnapType, lines[0], lines[1], lines[2], offset[0], offset[1], offset[2], tol);
    }
    
    SYS_FORCE_INLINE void emplaceMergeAttribs(GU_Snap::AttributeMergeData && attribMergeData)
    {
        comSnapParm.myMergeAttribs.emplace_back(attribMergeData);
        
        pointSnapParm.    myMergeAttribs.emplace_back(attribMergeData);
        gridSnapParm.     myMergeAttribs.emplace_back(attribMergeData);
        specifiedSnapParm.myMergeAttribs.emplace_back(attribMergeData);
    }

    SYS_FORCE_INLINE void setPosMergeMethod()
    { mergePos = false; }

    SYS_FORCE_INLINE void setPosMergeMethod(const GU_Snap::AttributeMergeMethod mergeMethod)
    { mergePos = true; posMergeMethod = mergeMethod; }



    
    const GA_Attribute* setTargetElemAttrib(const UT_StringRef& name = "")
    {
        setHasComputed();
        
        const GA_Attribute* attrib;
        if (geoRef0)
            attrib = getRef0AttribArray().append(GA_ATTRIB_POINT, name);
        else
            attrib = getInAttribArray().append(GA_ATTRIB_POINT, name);
        
        if (attrib && !attrib->getAIFTuple())
            attrib = nullptr;
        specifiedSnapParm.myTargetElemAttrib = attrib;
        specifiedSnapParm.myTargetOwner = attrib->getOwner();
        return attrib;
    }
    
    void setTargetElemAttrib(const GA_Attribute* const attrib)
    {
        setHasComputed();
        specifiedSnapParm.myTargetElemAttrib = attrib;
        specifiedSnapParm.myTargetOwner = attrib->getOwner();
    }
    
    GA_PointGroup* setSnappedGroup(const bool detached = true, const UT_StringRef& name = "")
    {
        GA_PointGroup* const group = findOrCreatePointGroup(detached, name);
        comSnapParm.myOutputGroup = group;
        
        pointSnapParm.    myOutputGroup = group;
        gridSnapParm.     myOutputGroup = group;
        specifiedSnapParm.myOutputGroup = group;
        return group;
    }

    GA_Attribute* setSnapAttrib(const bool detached = true, const UT_StringRef& name = "")
    {
        GA_Attribute* const attrib = getOutAttribArray().findOrCreateTuple(detached, GA_ATTRIB_POINT, GA_STORECLASS_INT, GA_STORE_INVALID, name);
        comSnapParm.myOutputAttribH = attrib;
        
        pointSnapParm    .myOutputAttribH = attrib;
        gridSnapParm     .myOutputAttribH = attrib;
        specifiedSnapParm.myOutputAttribH = attrib;
        return attrib;
    }



    
    GA_Attribute* setQRadiusAttrib(const UT_StringRef& name)
    {
        GA_Attribute* radiusAttrib = getInAttribArray().append(GA_ATTRIB_POINT, name);
        if (radiusAttrib && !radiusAttrib->getAIFTuple())
            radiusAttrib = nullptr;
        pointSnapParm.myQPscaleH = radiusAttrib;
        return radiusAttrib;
    }

    const GA_Attribute* setTRadiusAttrib(const UT_StringRef& name)
    {
        const GA_Attribute* radiusAttrib;
        if (geoRef0)
            radiusAttrib = getRef0AttribArray().append(GA_ATTRIB_POINT, name);
        else
            radiusAttrib = getInAttribArray().append(GA_ATTRIB_POINT, name);
        
        if (radiusAttrib && !radiusAttrib->getAIFTuple())
            radiusAttrib = nullptr;
        pointSnapParm.myTPscaleH = radiusAttrib;
        return radiusAttrib;
    }
    
    SYS_FORCE_INLINE void setRadiusAttrib(const UT_StringRef& name)
    {
        setQRadiusAttrib(name);
        setTRadiusAttrib(name);
    }




    
    GA_Attribute* setQMatchAttrib(const UT_StringRef& name)
    {
        GA_Attribute* matchAttrib = getInAttribArray().append(GA_ATTRIB_POINT, name);
        if (!matchAttrib)
        {
            pointSnapParm.myUseMatchAttrib = false;
            return matchAttrib;
        }
        const GA_AIFTuple* const aifTuple = matchAttrib->getAIFTuple();
        if (aifTuple)
        {
            switch (matchAttrib->getStorageClass())
            {
            case GA_STORECLASS_INT:   pointSnapParm.myQMatchIntH = matchAttrib; break;
            case GA_STORECLASS_FLOAT: pointSnapParm.myQMatchFltH = matchAttrib; break;
            }
        }
        else
        {
            const GA_AIFStringTuple* const aifStrTuple = matchAttrib->getAIFStringTuple();
            if (aifStrTuple)
            {
                pointSnapParm.myQMatchStrH = matchAttrib;
            }
            else
            {
                matchAttrib = nullptr;
            }
        }
        pointSnapParm.myUseMatchAttrib = matchAttrib;
        return matchAttrib;
    }
    
    const GA_Attribute* setTMatchAttrib(const UT_StringRef& name)
    {
        const GA_Attribute* matchAttrib = geoRef0 ? getRef0AttribArray().append(GA_ATTRIB_POINT, name) : getInAttribArray().append(GA_ATTRIB_POINT, name);
        if (!matchAttrib)
        {
            pointSnapParm.myUseMatchAttrib = false;
            return matchAttrib;
        }
        const GA_AIFTuple* const aifTuple = matchAttrib->getAIFTuple();
        if (aifTuple)
        {
            switch (matchAttrib->getStorageClass())
            {
            case GA_STORECLASS_INT:   pointSnapParm.myTMatchIntH = matchAttrib; break;
            case GA_STORECLASS_FLOAT: pointSnapParm.myTMatchFltH = matchAttrib; break;
            }
        }
        else
        {
            const GA_AIFStringTuple* const aifStrTuple = matchAttrib->getAIFStringTuple();
            if (aifStrTuple)
            {
                pointSnapParm.myTMatchStrH = matchAttrib;
            }
            else
            {
                matchAttrib = nullptr;
            }
        }
        pointSnapParm.myUseMatchAttrib = matchAttrib;
        return matchAttrib;
    }
    
    SYS_FORCE_INLINE void setMatchAttrib(const UT_StringRef& name)
    {
        setQMatchAttrib(name);
        setTMatchAttrib(name);
        pointSnapParm.myUseMatchAttrib &= bool(pointSnapParm.myQMatchStrH.getAttribute());
    }
    
    SYS_FORCE_INLINE void setMatchTol(const fpreal tol = 0)
    { pointSnapParm.myMatchTol = tol; }
    
    
private:

virtual bool
    computeCore() override
{
    if (groupParser.isEmpty())
        return true;
    
    if (GFE_Type::isInvalidPosAttrib(posAttrib) || GFE_Type::isInvalidPosAttrib(posAttribNonConst))
    {
        posAttribNonConst = geo->getP();
        posAttrib = posAttribNonConst;
    }

    if (GFE_Type::isInvalidPosAttrib(posRef0Attrib))
        posRef0Attrib = geoRef0 ? geoRef0->getP() : nullptr;

    GA_AttributeUPtr posRestUPtr;
    GA_Attribute* posRest = posAttribNonConst;
    if (!geoRef0 && mergePos)
    {
        //posRestUPtr = GFE_Attribute::clone(*geo, *posAttrib);
        //posRest = posRestUPtr.get();
        
        posRest = GFE_Attribute::clone(*geo, *posAttrib, "abc");
    }
    
    const bool keepEdgeGroup = !keepEdgeGroupArray.isEmpty();
    //const GA_Group* const geoGroup = groupParser.getGroup();
    GU_Detail& geoGU = *geo->asGU_Detail();

    comSnapParm.myQGroup = groupParser.getPointGroup();
    comSnapParm.myTGroup = groupParserFuseRef.getPointGroup();
    
    pointSnapParm    .myQGroup = groupParser.getPointGroup();
    gridSnapParm     .myQGroup = groupParser.getPointGroup();
    specifiedSnapParm.myQGroup = groupParser.getPointGroup();
    
    pointSnapParm    .myTGroup = groupParserFuseRef.getPointGroup();
    gridSnapParm     .myTGroup = groupParserFuseRef.getPointGroup();
    specifiedSnapParm.myTGroup = groupParserFuseRef.getPointGroup();
    
    if (keepEdgeGroup)
    {
        
    }
    else
    {
        switch (method)
        {
        case Method::Point:
            if (mergePos)
            {
                emplaceMergeAttribs(GU_Snap::AttributeMergeData(posMergeMethod, geoRef0 ? posRef0Attrib : posAttribNonConst, posAttribNonConst));
                pointSnapParm.myQPosH = geoRef0 ? posAttribNonConst : posRest;
            }
            else
            {
                pointSnapParm.myQPosH = posAttribNonConst;
                
            }
            //pointSnapParm.myQPosH = mergePos ? posAttribNonConst : nullptr;
            pointSnapParm.myTPosH = geoRef0 ? posRef0Attrib : posRest;
            GU_Snap::snapPoints(geoGU, geoRef0->asGU_Detail(), pointSnapParm);
        break;
            
            
        case Method::Grid:
            gridSnapParm.myPosH = posAttribNonConst;
            GU_Snap::snapGrid(geoGU, gridSnapParm);
        break;
            
            
        case Method::Specified:
            specifiedSnapParm.myTargetOwner = GA_ATTRIB_POINT;
            GU_Snap::snapByAttrib(geoGU, geoRef0->asGU_Detail(), specifiedSnapParm);
        break;

            
        default: UT_ASSERT_MSG(0, "Unhandled Fuse Method");                             
        break;
        }
    }
    
    posAttribNonConst->bumpDataId();
    
    if (delUnusedPoint)
        geo->destroyUnusedPoints();
    
    //if (geoGroup)
    //    fuseParms.setQuerygroup(geoGroup->isDetached());
    
        
    //fuseParms.setPosAttrib(posAttrib->getName());

    //SOP_Fuse_2_0Parms fuseParms1(fuseParms);
    //fuseParms1.setQuerygroup(__TEMP_GFE_Fuse_SnapGroupName);
    
    //pointGroup = groupParser.classType() == GA_GROUP_PRIMITIVE ? nullptr : groupParser.getPointGroup();
    //switch (posAttribNonConst->getAIFTuple()->getStorage(posAttribNonConst))
    //{
    //case GA_STORE_REAL16: extendCurveStraight<fpreal16>(); break;
    //case GA_STORE_REAL32: extendCurveStraight<fpreal32>(); break;
    //case GA_STORE_REAL64: extendCurveStraight<fpreal64>(); break;
    //default: UT_ASSERT_MSG(0, "not possible storage"); break;
    //}

    return true;
}


public:
    
    GFE_GroupParser groupParserFuseRef;
    GFE_GroupArray keepEdgeGroupArray;
    //SOP_Fuse_2_0Parms fuseParms;

    
    Method method = Method::Point;
    bool delUnusedPoint = false;
    
    bool mergePos = false;
    GU_Snap::AttributeMergeMethod posMergeMethod = GU_Snap::AttributeMergeMethod::MERGE_ATTRIBUTE_MEAN;

private:
    GU_Snap::CommonSnapParms comSnapParm;
    GU_Snap::PointSnapParms  pointSnapParm;
    GU_Snap::GridSnapParms   gridSnapParm;
    GU_Snap::AttribSnapParms specifiedSnapParm;
    
    const GA_PointGroup* pointGroup;
    
    //exint subscribeRatio = 64;
    //exint minGrainSize   = 1024;

private:
    
    //GU_DetailHandle destgdh;
    //UT_Array<GU_ConstDetailHandle> inputgdh;
    //const SOP_NodeVerb* const fuseVerb = SOP_NodeVerb::lookupVerb("fuse::2.0");
    
// #undef __TEMP_GFE_Fuse_GroupName
// #undef __TEMP_GFE_Fuse_SnapAttribName
// #undef __TEMP_GFE_Fuse_SnapGroupName

}; // End of class GFE_Fuse







#endif
