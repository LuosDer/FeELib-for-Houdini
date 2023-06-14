
#pragma once

#ifndef __GFE_Bound_h__
#define __GFE_Bound_h__

#include "GFE/GFE_Bound.h"




#include "UT/UT_BoundingBox.h"


namespace GFE_Bound {

    template<typename FLOAT_T>
    SYS_FORCE_INLINE static UT_BoundingBoxT<FLOAT_T> stdBoundingBoxT()
    { return UT_BoundingBoxT<FLOAT_T>(SYS_FP32_MAX, SYS_FP32_MAX, SYS_FP32_MAX, SYS_FP32_MIN, SYS_FP32_MIN, SYS_FP32_MIN); }

    template<typename FLOAT_T, typename FLOAT_T2>
    static void expandBounds(UT_BoundingBoxT<FLOAT_T>& bound, const UT_BoundingBoxT<FLOAT_T2>& boundRef)
    {
        bound(0,0) += boundRef(0,0);
        bound(0,1) += boundRef(0,1);
        bound(1,0) += boundRef(1,0);
        bound(1,1) += boundRef(1,1);
        bound(2,0) += boundRef(2,0);
        bound(2,1) += boundRef(2,1);
    }

    template<typename FLOAT_T>
    SYS_FORCE_INLINE static void setStd(UT_BoundingBoxT<FLOAT_T>& bbox)
    { bbox.setBounds(SYS_FP32_MAX, SYS_FP32_MAX, SYS_FP32_MAX, SYS_FP32_MIN, SYS_FP32_MIN, SYS_FP32_MIN); }

    


} // End of namespace GFE_Bound

#endif
