
#pragma once

#ifndef __GFE_Carve_h__
#define __GFE_Carve_h__



//#include "GFE/GFE_Carve.h"

#include "GA/GA_Detail.h"
//#include "GA/GA_Types.h"

#include "GFE/GFE_Type.h"
#include "GFE/GFE_TopologyReference.h"

#include "GEO/GEO_PrimPoly.h"


enum GFE_CarveType
{
    GFE_CarveType_AUTO,
    GFE_CarveType_OPEN,
    GFE_CarveType_CLOSE,
};

namespace GFE_Carve {



// can not use in parallel unless for each GA_Detail
static void
carve(
    const SOP_NodeVerb::CookParms& cookparms,
    GA_Detail* const geoPoint,
    const GA_Detail* const geoFull,
    const UT_StringHolder& cutPointGroupName = "",
    const UT_StringHolder& primGroupName = "",
    const bool cutPoint = false,
    const bool mergePrimEndsIfClosed = true,
    const GFE_CarveType polyType = GFE_CarveType_AUTO,

    const GA_Storage storage = GA_STORE_INVALID,
    //const bool createSrcPrimAttrib = false,
    const UT_StringHolder& srcPrimAttribName = "",
    //const bool createSrcPointAttrib = false,
    const UT_StringHolder& srcPointAttribName = "",
    const bool delInputPointGroup = false
)
{
    GOP_Manager gop;
    GA_PointGroup* const cutPointGroup = GFE_Group::findOrParsePointGroupDetached(cookparms, geoPoint, cutPointGroupName, gop);
    if (cutPointGroup && cutPointGroup->isEmpty())
    {
        geoPoint->replaceWith(*geoFull);
        return;
    }

    const GA_PrimitiveGroup* const primGroup = GFE_Group::findOrParsePrimitiveGroupDetached(cookparms, geoFull, primGroupName, gop);
    if (primGroup && primGroup->isEmpty())
    {
        geoPoint->replaceWith(*geoFull);
        return;
    }

    polyCut(geoPoint, geoFull,
        srcPrimAttribName, srcPointAttribName, cutPointGroup, 
        storage, primGroup,
        cutPoint, mergePrimEndsIfClosed, polyType);

    if (delInputPointGroup && cutPointGroup && !cutPointGroup->isDetached())
    {
        geoPoint->destroyGroup(cutPointGroup);
    }
}






} // End of namespace GFE_Carve

#endif
