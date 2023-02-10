
#pragma once

#ifndef __GA_FeE_Carve_h__
#define __GA_FeE_Carve_h__



//#include "GA_FeE/GA_FeE_Carve.h"

#include "GA/GA_Detail.h"
//#include "GA/GA_Types.h"

#include "GA_FeE/GA_FeE_Type.h"
#include "GA_FeE/GA_FeE_TopologyReference.h"

#include "GEO/GEO_PrimPoly.h"


enum GA_FeE_CarveType
{
    GA_FeE_CarveType_AUTO,
    GA_FeE_CarveType_OPEN,
    GA_FeE_CarveType_CLOSE,
};

namespace GA_FeE_Carve {



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
    const GA_FeE_CarveType polyType = GA_FeE_CarveType_AUTO,

    const GA_Storage storage = GA_STORE_INVALID,
    //const bool createSrcPrimAttrib = false,
    const UT_StringHolder& srcPrimAttribName = "",
    //const bool createSrcPointAttrib = false,
    const UT_StringHolder& srcPointAttribName = "",
    const bool delInputPointGroup = false
)
{
    GOP_Manager gop;
    GA_PointGroup* const cutPointGroup = GA_FeE_Group::findOrParsePointGroupDetached(cookparms, geoPoint, cutPointGroupName, gop);
    if (cutPointGroup && cutPointGroup->isEmpty())
    {
        geoPoint->replaceWith(*geoFull);
        return;
    }

    const GA_PrimitiveGroup* const primGroup = GA_FeE_Group::findOrParsePrimitiveGroupDetached(cookparms, geoFull, primGroupName, gop);
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






} // End of namespace GA_FeE_Carve

#endif
