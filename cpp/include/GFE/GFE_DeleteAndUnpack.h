
#pragma once

#ifndef __GFE_DelAndUnpack_h__
#define __GFE_DelAndUnpack_h__


//#include "GFE/GFE_DelAndUnpack.h"

#include "GA/GA_Detail.h"





namespace GFE_DelAndUnpack {





static void
    delAndUnpack(
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
}






} // End of namespace GFE_DelAndUnpack

#endif
