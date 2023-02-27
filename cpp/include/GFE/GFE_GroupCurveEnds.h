
#pragma once

#ifndef __GFE_GroupCurveEnds_h__
#define __GFE_GroupCurveEnds_h__

//#include "GFE/GFE_GroupCurveEnds.h"

#include "GA/GA_Detail.h"


namespace GFE_GroupCurveEnds {



SYS_FORCE_INLINE
    static void
    groupCurveEnds(
        const GA_Detail* const geo,
        const GA_PrimitiveGroup* const geoPrimGroup
    )
{
    UT_ASSERT_P(geo);
    if (geoPrimGroup && geoPrimGroup->isEmpty())
        return;

}





SYS_FORCE_INLINE
static void
groupCurveEnds(
    const SOP_NodeVerb::CookParms& cookparms,
    const GA_Detail* const geo,
    const GA_GroupType groupType,
    const GA_Group* const groupName
)
{
    GOP_Manager gop;
    const GA_Group* geo0Group = GFE_Group::findOrParseGroupDetached(cookparms, geo, groupType, groupName, gop);

}















} // End of namespace GFE_GroupCurveEnds

#endif
