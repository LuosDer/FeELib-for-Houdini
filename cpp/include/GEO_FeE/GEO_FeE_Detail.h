
#pragma once

#ifndef __GEO_FeE_Detail_h__
#define __GEO_FeE_Detail_h__

#ifndef MAXLOOPCOUNT
#define MAXLOOPCOUNT 1000000000
#endif


//#include "GA_FeE/GEO_FeE_Detail.h"

#include "GA/GA_Detail.h"
//#include "GA/GA_Types.h"

#include "GA_FeE/GA_FeE_Type.h"
#include "GA_FeE/GA_FeE_Attribute.h"
#include "GA_FeE/GA_FeE_Detail.h"
#include "GA_FeE/GA_FeE_Group.h"




namespace GEO_FeE_Detail {


//GEO_FeE_Detail::delByGroup(cookparms, geo, groupType, groupName, reverse, delGroup, delWithPoint, delPointMode, guaranteeNoVertexReference);
static void
    delElement(
        const SOP_NodeVerb::CookParms& cookparms,
        GA_Detail* const geo,
        const GA_GroupType groupType,
        const UT_StringHolder& groupName,
        const bool reverse = false,
        const bool delGroup = true,
        const bool delWithPoint = true,
        const GA_Detail::GA_DestroyPointMode delPointMode = GA_Detail::GA_DestroyPointMode::GA_DESTROY_DEGENERATE_INCOMPATIBLE,
        const bool guaranteeNoVertexReference = false
    )
{
    GOP_Manager gop;
    GA_Group* geoGroup = GA_FeE_Group::findOrParseGroupDetached(cookparms, geo, groupType, groupName, gop);

    GA_FeE_Detail::delElement(geo, geoGroup, reverse, delGroup && !geoGroup->isDetached(), delWithPoint, delPointMode, guaranteeNoVertexReference);
}

static void
    delElement(
        const bool doDelElement,
        const SOP_NodeVerb::CookParms& cookparms,
        GA_Detail* const geo,
        const GA_GroupType groupType,
        const UT_StringHolder& groupName,
        const bool reverse = false,
        const bool delGroup = true,
        const bool delWithPoint = true,
        const GA_Detail::GA_DestroyPointMode delPointMode = GA_Detail::GA_DestroyPointMode::GA_DESTROY_DEGENERATE_INCOMPATIBLE,
        const bool guaranteeNoVertexReference = false
    )
{
    GOP_Manager gop;
    GA_Group* geoGroup = GA_FeE_Group::findOrParseGroupDetached(cookparms, geo, groupType, groupName, gop);

    if (!doDelElement)
    {
        if (geoGroup)
        {
            cookparms.getNode()->setHighlight(true);
            cookparms.select(*geoGroup);
        }
        return;
    }

    GA_FeE_Detail::delElement(geo, geoGroup, reverse, delGroup && !geoGroup->isDetached(), delWithPoint, delPointMode, guaranteeNoVertexReference);
}












static void
leaveElement(
    const SOP_NodeVerb::CookParms& cookparms,
    GA_Detail* const geo,
    const GA_Detail* const srcGeo,
    const GA_GroupType groupType,
    const UT_StringHolder& groupName,
    const bool reverse = false,
    const bool delGroup = true
)
{
    GOP_Manager gop;
    GA_Group* geoGroup = GA_FeE_Group::findOrParseGroupDetached(cookparms, geo, groupType, groupName, gop);

    GA_FeE_Detail::leaveElement(geo, srcGeo, geoGroup, reverse, delGroup && !geoGroup->isDetached());
}




static void
leaveElement(
    const bool doLeaveElement,
    const SOP_NodeVerb::CookParms& cookparms,
    GA_Detail* const geo,
    const GA_Detail* const srcGeo,
    const GA_GroupType groupType,
    const UT_StringHolder& groupName,
    const bool reverse = false,
    const bool delGroup = true
)
{
    GOP_Manager gop;
    GA_Group* geoGroup = GA_FeE_Group::findOrParseGroupDetached(cookparms, geo, groupType, groupName, gop);

    if (!doLeaveElement)
    {
        if (geoGroup)
        {
            cookparms.getNode()->setHighlight(true);
            cookparms.select(*geoGroup);
        }
        return;
    }

    GA_FeE_Detail::leaveElement(geo, srcGeo, geoGroup, reverse, delGroup && !geoGroup->isDetached());
}






//extractPoint(geo, groupName, reverseGroup, delGroup);
static void
extractPoint(
    GA_Detail* const geo,
    GA_PointGroup*& group,
    const UT_StringHolder& delPrimGroup,
    const UT_StringHolder& delPointGroup,
    const UT_StringHolder& delVertexGroup,
    const UT_StringHolder& delEdgeGroup,
    const bool reverseGroup = false,
    const bool delInputGroup = true
)
{
    UT_VERIFY_P(geo);
    if (group)
    {
        GA_FeE_Group::delStdGroup(geo, delPrimGroup, "", delVertexGroup, delEdgeGroup);
        GA_GroupTable* groupTable = geo->getGroupTable(GA_GROUP_POINT);
        for (GA_GroupTable::iterator<GA_Group> it = groupTable->beginTraverse(); !it.atEnd(); ++it)
        {
            GA_PointGroup* const geoGroup = static_cast<GA_PointGroup*>(it.group());
            if (geoGroup->isDetached())
                continue;
            //if (geoGroup->getName() == group->getName())
            if (geoGroup == group)
                continue;
            if (geoGroup->getName().match(delPointGroup))
                continue;
            groupTable->destroy(geoGroup);
        }
    }
    else
    {
        GA_FeE_Group::delStdGroup(geo, delPrimGroup, delPointGroup, delVertexGroup, delEdgeGroup);
    }

    GA_FeE_Detail::extractPoint(geo, group,
        reverseGroup, delInputGroup);
}


//GA_FeE_Detail::extractPoint(cookparms, outGeo0, inGeo0, groupName,
//    delPrimGroup, delPointGroup, delVertexGroup, delEdgeGroup,
//    sopparms.getReverseGroup(), sopparms.getDelInputGroup());
static void
extractPoint(
    const SOP_NodeVerb::CookParms& cookparms,
    GA_Detail* const geo,
    const UT_StringHolder& groupName,
    const UT_StringHolder& delPrimGroup,
    const UT_StringHolder& delPointGroup,
    const UT_StringHolder& delVertexGroup,
    const UT_StringHolder& delEdgeGroup,
    const bool reverseGroup = false,
    const bool delInputGroup = true
)
{
    UT_VERIFY_P(geo);

    GOP_Manager gop;
    GA_PointGroup* group = GA_FeE_Group::findOrParsePointGroupDetached(cookparms, geo, groupName, gop);
    extractPoint(geo, group,
        delPrimGroup, delPointGroup, delVertexGroup, delEdgeGroup,
        reverseGroup, delInputGroup);
}




//extractPoint(geo, groupName, reverseGroup, delGroup);
SYS_FORCE_INLINE
static void
extractPoint(
    GA_Detail* const geo,
    const GA_Detail* const srcGeo,
    const GA_PointGroup* const group,
    const UT_StringHolder& delPrimAttrib,
    const UT_StringHolder& delPointAttrib,
    const UT_StringHolder& delVertexAttrib,
    const UT_StringHolder& delDetailAttrib,
    const UT_StringHolder& delPrimGroup,
    const UT_StringHolder& delPointGroup,
    const UT_StringHolder& delVertexGroup,
    const UT_StringHolder& delEdgeGroup,
    const bool reverseGroup = false,
    const bool delInputGroup = true
)
{
    UT_VERIFY_P(geo);
    UT_VERIFY_P(srcGeo);
    geo->replaceWithPoints(*srcGeo);

    GA_FeE_Attribute::delStdAttribute(geo, delPrimAttrib, delPointAttrib, delVertexAttrib, delDetailAttrib);
    GA_FeE_Group::delStdGroup(geo, delPrimGroup, delPointGroup, delVertexGroup, delEdgeGroup);

    GA_FeE_Detail::extractPoint(geo, srcGeo, group,
        reverseGroup, delInputGroup);

    GA_FeE_Group::delStdGroup(geo, GA_GROUP_POINT, delPointGroup);
}


//GA_FeE_Detail::extractPoint(cookparms, outGeo0, inGeo0, groupName,
//    delPrimGroup, delPointGroup, delVertexGroup, delEdgeGroup,
//    sopparms.getReverseGroup(), sopparms.getDelInputGroup());
SYS_FORCE_INLINE
static void
extractPoint(
    const SOP_NodeVerb::CookParms& cookparms,
    GA_Detail* const geo,
    const GA_Detail* const srcGeo,
    const UT_StringHolder& groupName,
    const UT_StringHolder& delPrimAttrib,
    const UT_StringHolder& delPointAttrib,
    const UT_StringHolder& delVertexAttrib,
    const UT_StringHolder& delDetailAttrib,
    const UT_StringHolder& delPrimGroup,
    const UT_StringHolder& delPointGroup,
    const UT_StringHolder& delVertexGroup,
    const UT_StringHolder& delEdgeGroup,
    const bool reverseGroup = false,
    const bool delInputGroup = true
)
{
    UT_VERIFY_P(geo);

    GOP_Manager gop;
    const GA_PointGroup* const group = GA_FeE_Group::findOrParsePointGroupDetached(cookparms, srcGeo, groupName, gop);
    extractPoint(geo, srcGeo, group,
        delPrimAttrib, delPointAttrib, delVertexAttrib, delDetailAttrib,
        delPrimGroup, delPointGroup, delVertexGroup, delEdgeGroup,
        reverseGroup, delInputGroup);
}




} // End of namespace GEO_FeE_Detail

#endif
