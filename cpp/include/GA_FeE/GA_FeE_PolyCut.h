
#pragma once

#ifndef __GA_FeE_PolyCut_h__
#define __GA_FeE_PolyCut_h__



//#include "GA_FeE/GA_FeE_PolyCut.h"

#include "GA/GA_Detail.h"
//#include "GA/GA_Types.h"

#include "GA_FeE/GA_FeE_Type.h"
#include "GA_FeE/GA_FeE_TopologyReference.h"

#include "GEO/GEO_PrimPoly.h"



namespace GA_FeE_PolyCut {



static void
    polyCutPoint(
        GA_Detail* const geo,
        const GA_PointGroup* const cutPointGroup,
        const bool mergePrimEndsIfClosed = true,
        GA_Attribute* const srcPrimsAttrib = nullptr,
        GA_Attribute* const srcPointsAttrib = nullptr
    )
{
    UT_ASSERT_P(geo);
    if (srcPrimsAttrib)
    {

    }
    if (srcPointsAttrib)
    {

    }

}

static void
polyCutPrim(
    GA_Detail* const geo,
    const GA_PointGroup* const cutPointGroup,
    const bool mergePrimEndsIfClosed = true,
    GA_Attribute* const srcPrimsAttrib = nullptr
)
{
    UT_ASSERT_P(geo);

    if (srcPrimsAttrib)
    {

    }

}


} // End of namespace GA_FeE_PolyCut

#endif
