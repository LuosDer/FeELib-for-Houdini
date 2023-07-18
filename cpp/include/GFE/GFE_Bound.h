
#pragma once

#ifndef __GFE_Bound_h__
#define __GFE_Bound_h__

#include "GFE/GFE_Bound.h"




#include "UT/UT_BoundingBox.h"


namespace GFE_Bound {

    template<typename _TScalar>
    SYS_FORCE_INLINE static UT_BoundingBoxT<_TScalar> stdBoundingBoxT()
    { return UT_BoundingBoxT<_TScalar>(SYS_FP32_MAX, SYS_FP32_MAX, SYS_FP32_MAX, SYS_FP32_MIN, SYS_FP32_MIN, SYS_FP32_MIN); }

    //using stdBoundingBoxH = stdBoundingBoxT<fpreal16>;
    //using stdBoundingBoxF = stdBoundingBoxT<fpreal32>;
    //using stdBoundingBoxD = stdBoundingBoxT<fpreal64>;
    //using stdBoundingBoxR = stdBoundingBoxT<fpreal>  ;
    
    //typedef stdBoundingBoxT<fpreal16> stdBoundingBoxH;
    //typedef stdBoundingBoxT<fpreal32> stdBoundingBoxF;
    //typedef stdBoundingBoxT<fpreal64> stdBoundingBoxD;
    //typedef stdBoundingBoxT<fpreal>   stdBoundingBoxR;


    
    template<typename _TScalar, typename _TScalar1>
    static void expandBounds(UT_BoundingBoxT<_TScalar>& bound, const UT_BoundingBoxT<_TScalar1>& boundRef)
    {
        bound(0,0) += boundRef(0,0);
        bound(0,1) += boundRef(0,1);
        bound(1,0) += boundRef(1,0);
        bound(1,1) += boundRef(1,1);
        bound(2,0) += boundRef(2,0);
        bound(2,1) += boundRef(2,1);
    }

    template<typename _TScalar>
    SYS_FORCE_INLINE static void setStd(UT_BoundingBoxT<_TScalar>& bbox)
    { bbox.setBounds(SYS_FP32_MAX, SYS_FP32_MAX, SYS_FP32_MAX, SYS_FP32_MIN, SYS_FP32_MIN, SYS_FP32_MIN); }

    
    template<typename _TScalar>
    static UT_BoundingBoxT<_TScalar> getBBox(const GA_Detail& geo, const GA_Range& pointRange, const GA_Attribute* posAttrib = nullptr)
    {
        if (GFE_Type::isInvalidPosAttrib(posAttrib))
            posAttrib = geo.getP();
        
        auto bbox = stdBoundingBoxT<fpreal32>();
        geo.enlargeBoundingBox(bbox, pointRange, posAttrib);
        if constexpr (std::is_same_v<_TScalar, float>)
            return bbox;
        else
            return UT_BoundingBoxT<_TScalar>(bbox);
    }
    

} // End of namespace GFE_Bound

#endif
