
#pragma once

#ifndef __GFE_LeaveElement_h__
#define __GFE_LeaveElement_h__



//#include "GFE/GFE_LeaveElement.h"

#include "GA/GA_Detail.h"

#include "GFE/GFE_Group.h"




namespace GFE_LeaveElement {









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

















} // End of namespace GFE_LeaveElement

#endif
