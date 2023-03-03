
#pragma once

#ifndef __GFE_OnPoly_h__
#define __GFE_OnPoly_h__

#ifndef MAXLOOPCOUNT
#define MAXLOOPCOUNT 1000000000
#endif


//#include "GFE/GFE_OnPoly.h"

#include "GA/GA_Detail.h"
//#include "GA/GA_Types.h"

#include <GU/GU_RayIntersect.h>

#include "GFE/GFE_Type.h"
#include "GFE/GFE_TopologyReference.h"
#include "GFE/GFE_GroupParse.h"


namespace GFE_OnPoly {



static void
    onPoly(
        GA_Detail* const geo,
        const GA_Detail* const geoRef,
        const GA_PointGroup* const geoGroup,
        const GA_Storage storage = GA_STORE_INVALID,
    )
{
    UT_ASSERT_P(geo);
    UT_ASSERT_P(geoRef);

    const GA_Precision precision = geo->getPreferredPrecision();
    const GA_Storage inStorageI = GFE_Type::getPreferredStorageI(precision);
    const GA_Storage inStorageF = GFE_Type::getPreferredStorageF(precision);
    geo->project
}


static void
    onPoly(
        const SOP_NodeVerb::CookParms& cookparms,
        GA_Detail* const geo,
        const GA_Detail* const geoRef,
        const UT_StringHolder& groupName,
        const GA_Storage storage = GA_STORE_INVALID,
    )
{
    UT_ASSERT_P(geo);
    UT_ASSERT_P(geoRef);

    GOP_Manager gop;
    const GA_Group* const geoGroup = GFE_GroupParse_Namespace::findOrParsePointGroupDetached(cookparms, geo, groupName, gop);
    if (!geoGroup)
        return;

    if (!geoGroup || geoGroup->isEmpty())
    //if (!geoGroup || GFE_Group::groupIsEmpty(geoGroup))
        return;

    onPoly(geo, geoRef, geoGroup);

}


} // End of namespace GFE_OnPoly

#endif
