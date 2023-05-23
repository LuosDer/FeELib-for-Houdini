
#pragma once

#ifndef __GFE_Bound_h__
#define __GFE_Bound_h__

#include "GFE/GFE_Bound.h"




#include "UT/UT_BoundingBox.h"





namespace GFE_Bound {

    template<typename FLOAT_T>
    SYS_FORCE_INLINE static UT_BoundingBoxT<FLOAT_T> stdBoundingBox()
    { return UT_BoundingBoxT<FLOAT_T>(SYS_FP32_MAX, SYS_FP32_MAX, SYS_FP32_MAX, SYS_FP32_MIN, SYS_FP32_MIN, SYS_FP32_MIN); }




} // End of namespace GFE_Bound

#endif
