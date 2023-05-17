
#pragma once

#ifndef __GFE_DelElement_h__
#define __GFE_DelElement_h__


//#include "GFE/GFE_DelElement.h"

#include "GA/GA_Detail.h"

#include "GFE/GFE_GroupParse.h"




namespace GFE_DelElement {







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
    GA_Group* geoGroup = GFE_GroupParse_Namespace::findOrParseGroupDetached(cookparms, geo, groupType, groupName, gop);

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












} // End of namespace GFE_DelElement

#endif
