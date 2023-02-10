
#pragma once

#ifndef __GA_FeE_OnPoly_h__
#define __GA_FeE_OnPoly_h__

#ifndef MAXLOOPCOUNT
#define MAXLOOPCOUNT 1000000000
#endif


//#include "GA_FeE/GA_FeE_OnPoly.h"

#include "GA/GA_Detail.h"
//#include "GA/GA_Types.h"

#include "GA_FeE/GA_FeE_Type.h"
#include "GA_FeE/GA_FeE_TopologyReference.h"
#include "GA_FeE/GA_FeE_Group.h"


namespace GA_FeE_OnPoly {



static void
    onPoly(
        GA_Detail* const geo,
        const bool keepOrder = false,
        const bool keepLoop = true,
        const bool closeLoop = false,
        const GA_PointGroup* const stopPointGroup = nullptr,
        GA_Attribute* const srcPrimsAttrib = nullptr
    )
{
}



} // End of namespace GA_FeE_OnPoly

#endif
