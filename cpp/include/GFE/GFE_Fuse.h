
#pragma once

#ifndef __GFE_Fuse_h__
#define __GFE_Fuse_h__

#include "GFE/GFE_Fuse.h"

#include "GFE/GFE_GeoFilter.h"



#include "GU/GU_Snap.h"
#include "SOP/SOP_Fuse-2.0.proto.h"

class GFE_Fuse : public GFE_AttribFilterWithRef0 {


#define __TEMP_GFE_Fuse_GroupName      "__TEMP_GFE_Fuse_Group"
#define __TEMP_GFE_Fuse_SnapAttribName "__TEMP_GFE_Fuse_SnapTo"
#define __TEMP_GFE_Fuse_SnapGroupName  "__TEMP_GFE_Fuse_Snapped"

    
public:
    //using GFE_AttribFilter::GFE_AttribFilter;
    
    GFE_Fuse(
        GA_Detail& geo,
        const GA_Detail* const geoRef,
        const SOP_NodeVerb::CookParms* const cookparms = nullptr
    )
        : GFE_AttribFilterWithRef0(geo, geoRef, cookparms)
        , groupParserFuseRef(geoRef ? *geoRef : geo, groupParser.getGOPRef(), cookparms)
        , keepEdgeGroupArray(geo, cookparms)
    {
    }

    void
        setComputeParm(
            const fpreal dist = 0,
            const bool extendStart       = true,
            const bool extendEnd         = true,
            const bool ignoreTwoNebPoint = false,
            const bool tryFindNextPointIfOverlap = false,
            const exint subscribeRatio = 64,
            const exint minGrainSize   = 64
        )
    {
        setHasComputed();
        this->dist = dist;
        this->extendStart       = extendStart;
        this->extendEnd         = extendEnd;
        this->ignoreTwoNebPoint = ignoreTwoNebPoint;
        this->tryFindNextPointIfOverlap = tryFindNextPointIfOverlap;
        this->subscribeRatio = subscribeRatio;
        this->minGrainSize   = minGrainSize;
    }

    
private:

virtual bool
    computeCore() override
{
    if (!extendStart && !extendEnd || dist == 0)
        return true;
        
    if (groupParser.isEmpty())
        return true;
    
    if (GFE_Type::isInvalidPosAttrib(posAttrib))
        posAttrib = geo->getP();

    const bool keepEdgeGroup = !keepEdgeGroupArray.isEmpty();
    const GA_Group* const geoGroup = groupParser.getGroup();
    GU_Detail& geoGU = *geo->asGU_Detail();


    
    comSnapParm.myQGroup = groupParser.getPointGroup();
    comSnapParm.myTGroup = groupParserFuseRef.getPointGroup();
    
    comSnapParm.myConsolidate = g;

    
    if (keepEdgeGroup)
    {
        
    }
    else
    {
        
        GU_Snap::snapPoints(geoGU, geoRef0, pointSnapParm);

        GU_Snap::snapGrid(geoGU, gridSnapParm);
        
        GU_Snap::snapByAttrib(geoGU, geoRef0, attribSnapParm);
        
    }

    
    if (geoGroup)
    {
        fuseParms.setQuerygroup(geoGroup->isDetached());
    }
        
    fuseParms.setPosAttrib(posAttrib->getName());

    SOP_Fuse_2_0Parms fuseParms1(fuseParms);
    fuseParms1.setQuerygroup(__TEMP_GFE_Fuse_SnapGroupName);

    using Snaptype = SOP_Fuse_2_0Enums::Snaptype;
    using Algorithm = SOP_Fuse_2_0Enums::Algorithm;
    using TargetClass = SOP_Fuse_2_0Enums::TargetClass;
    using PositionSnapMethod = SOP_Fuse_2_0Enums::PositionSnapMethod;
    using Matchtype = SOP_Fuse_2_0Enums::Matchtype;
    using Gridtype = SOP_Fuse_2_0Enums::Gridtype;
    using Gridround = SOP_Fuse_2_0Enums::Gridround;
    using Attribsnapmethod = SOP_Fuse_2_0Enums::Attribsnapmethod;
    using Grouppropagation = SOP_Fuse_2_0Enums::Grouppropagation;


    
    pointGroup = groupParser.classType() == GA_GROUP_PRIMITIVE ? nullptr : groupParser.getPointGroup();
    switch (posAttribNonConst->getAIFTuple()->getStorage(posAttribNonConst))
    {
    case GA_STORE_REAL16: extendCurveStraight<fpreal16>(); break;
    case GA_STORE_REAL32: extendCurveStraight<fpreal32>(); break;
    case GA_STORE_REAL64: extendCurveStraight<fpreal64>(); break;
    default: UT_ASSERT_MSG(0, "not possible storage"); break;
    }
    
    return true;
}


public:
    
    GU_Snap::PointSnapParms  pointSnapParm;
    GU_Snap::GridSnapParms   gridSnapParm;
    GU_Snap::AttribSnapParms attribSnapParm;
    GU_Snap::CommonSnapParms comSnapParm;
    
    GFE_GroupParser groupParserFuseRef;
    GFE_GroupArray keepEdgeGroupArray;
    SOP_Fuse_2_0Parms fuseParms;
    
    fpreal dist = 0;
    bool extendStart = true;
    bool extendEnd = true;
    bool ignoreTwoNebPoint = false;
    bool tryFindNextPointIfOverlap = false;

private:
    const GA_PointGroup* pointGroup;
    
    exint subscribeRatio = 64;
    exint minGrainSize   = 1024;

private:
    
    GU_DetailHandle destgdh;
    UT_Array<GU_ConstDetailHandle> inputgdh;
    const SOP_NodeVerb* const fuseVerb = SOP_NodeVerb::lookupVerb("fuse::2.0");


#undef __TEMP_GFE_Fuse_GroupName
#undef __TEMP_GFE_Fuse_SnapAttribName
#undef __TEMP_GFE_Fuse_SnapGroupName

}; // End of class GFE_Fuse







#endif
