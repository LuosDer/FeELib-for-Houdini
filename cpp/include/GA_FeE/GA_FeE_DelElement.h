
#pragma once

#ifndef __GA_FeE_DelElement_h__
#define __GA_FeE_DelElement_h__


//#include "GA_FeE/GA_FeE_DelElement.h"

#include "GA/GA_Detail.h"

#include "GA_FeE/GA_FeE_Group.h"




namespace GA_FeE_DelElement {





static void
    delElement(
        GA_Detail* const geo,
        GA_Group*& group,
        const bool reverse = false,
        const bool delGroup = true,
        const bool delWithPoint = true,
        const GA_Detail::GA_DestroyPointMode delPointMode = GA_Detail::GA_DestroyPointMode::GA_DESTROY_DEGENERATE_INCOMPATIBLE,
        const bool guaranteeNoVertexReference = false
    )
{
    UT_VERIFY_P(geo);
    if (!group)
    {
        if (reverse)
        {
            geo->destroyPointOffsets(GA_Range(geo->getPointMap(), nullptr), delPointMode, guaranteeNoVertexReference);
            //geo->clearTopologyAttributes();
            //geo->createTopologyAttributes();
        }
        return;
    }
    switch (group->classType())
    {
    case GA_GROUP_PRIMITIVE:
    {
        const GA_Range&& range = GA_Range(geo->getPrimitiveMap(), UTverify_cast<GA_PrimitiveGroup*>(group), reverse);
        geo->destroyPrimitives(range, delWithPoint);
    }
    break;
    case GA_GROUP_POINT:
        geo->destroyPointOffsets(GA_Range(geo->getPointMap(), UTverify_cast<GA_PointGroup*>(group), reverse), delPointMode, guaranteeNoVertexReference);
        break;
    case GA_GROUP_VERTEX:
        geo->destroyVertexOffsets(GA_Range(geo->getVertexMap(), UTverify_cast<GA_VertexGroup*>(group), reverse));
        break;
    case GA_GROUP_EDGE:
        UT_ASSERT_MSG(0, "not possible");
        break;
    default:
        UT_ASSERT_MSG(0, "not possible");
        break;
    }
    if (delGroup)
    {
        geo->destroyGroup(group);
        group = nullptr;
    }
}



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

    delElement(geo, geoGroup, reverse, delGroup && !geoGroup->isDetached(), delWithPoint, delPointMode, guaranteeNoVertexReference);
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

    delElement(geo, geoGroup, reverse, delGroup && !geoGroup->isDetached(), delWithPoint, delPointMode, guaranteeNoVertexReference);

    if (doDelElement)
    {
        geo->bumpDataIdsForAddOrRemove(1, 1, 1);
    }
}












} // End of namespace GA_FeE_DelElement

#endif
