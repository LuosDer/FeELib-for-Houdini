
#pragma once

#ifndef __GA_FeE_LeaveElement_h__
#define __GA_FeE_LeaveElement_h__



//#include "GA_FeE/GA_FeE_LeaveElement.h"

#include "GA/GA_Detail.h"

#include "GA_FeE/GA_FeE_Group.h"




namespace GA_FeE_LeaveElement {









static void
    leaveElement(
        GA_Detail* const geo,
        const GA_Detail* const srcGeo,
        GA_Group*& group,
        const bool reverse = false,
        const bool delGroup = true
    )
{
    UT_VERIFY_P(geo);
    geo->clear();
    if (!group)
    {
        if (reverse)
        {
            geo->replaceWith(*srcGeo);
        }
        return;
        //geo->destroyPointOffsets(GA_Range(geo->getPointMap(), nullptr));
        //geo->clearTopologyAttributes();
        //geo->createTopologyAttributes();
    }
    switch (group->classType())
    {
    case GA_GROUP_PRIMITIVE:
    {
        static_cast<GEO_Detail*>(geo)->merge(*static_cast<const GEO_Detail*>(srcGeo), static_cast<const GA_PrimitiveGroup*>(group));
    }
    break;
    case GA_GROUP_POINT:
        static_cast<GEO_Detail*>(geo)->mergePoints(*static_cast<const GEO_Detail*>(srcGeo), static_cast<const GA_PointGroup*>(group));
        break;
    case GA_GROUP_VERTEX:
        UT_ASSERT_MSG(0, "not possible");
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

    leaveElement(geo, srcGeo, geoGroup, reverse, delGroup && !geoGroup->isDetached());
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

    leaveElement(geo, srcGeo, geoGroup, reverse, delGroup && !geoGroup->isDetached());
}









} // End of namespace GA_FeE_LeaveElement

#endif
