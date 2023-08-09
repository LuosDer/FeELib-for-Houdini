
#pragma once

#ifndef __GFE_Math_h__
#define __GFE_Math_h__

#include <GFE/GFE_Math.h>



#include <GFE/GFE_Type.h>

#include <GA/GA_Detail.h>

#include <variant>

namespace GFE_Math {

    namespace detail
    {
        template<typename FLOAT_T>
        SYS_FORCE_INLINE static FLOAT_T numDistance2(const FLOAT_T val0, const FLOAT_T val1)
        { return pow(val0 > val1 ? val0 - val1 : val1 - val0, 2); }

        template<typename VECTOR_T>
        SYS_FORCE_INLINE static typename VECTOR_T::value_type vectorDistance2(const VECTOR_T& val0, const VECTOR_T& val1)
        { return val0.distance(val1); }

        
        //template<typename _Ty>
        //SYS_FORCE_INLINE static GFE_Type::get_value_type_t<_Ty> lengthVec(const _Ty& val)
        //{
        //    using value_type = typename GFE_Type::get_value_type_t<_Ty>;
        //    if constexpr (GFE_Type::isVector<_Ty>)
        //        return val.length();
        //    else
        //        return val;
        //}
        //
        //template<typename _Ty>
        //SYS_FORCE_INLINE static _Ty lengthScalar(const _Ty val)
        //{
        //    if constexpr (GFE_Type::isScalar<_Ty>)
        //        return val;
        //    else
        //        return val;
        //}

    } // End of Namespace detail
    


    
    template<typename _Ty>
    SYS_FORCE_INLINE static GFE_Type::get_value_type_t<_Ty> length(const _Ty& val)
    {
        using value_type = typename GFE_Type::get_value_type_t<_Ty>;
        if constexpr (GFE_Type::isVector<_Ty>)
            return val.length();
            //return detail::lengthVec(val);
        //else if constexpr (GFE_Type::isMatrix<_Ty>)
        //    return detail::lengthMtx(val);
        else if constexpr (GFE_Type::isScalar<_Ty>)
            return value_type(val);
            
            //return detail::lengthScalar(val);
    }

    
    template<typename _Ty>
    SYS_FORCE_INLINE static GFE_Type::get_value_type_t<_Ty> length2(const _Ty& val)
    {
        using value_type = typename GFE_Type::get_value_type_t<_Ty>;
        if constexpr (GFE_Type::isVector<_Ty>)
            return val.length2();
        else if constexpr (GFE_Type::isScalar<_Ty>)
            return value_type(val) * value_type(val);
            
        //return detail::lengthScalar(val);
    }
    
    template<typename _Ty>
    SYS_FORCE_INLINE static _Ty lerp(const _Ty& v0, const _Ty& v1, const fpreal bias)
    {
        std::
        return v0 * (1.0 - bias) + v1 * bias;
    }
    
    template<typename _TScalar>
    static int8 minidx(const UT_Vector3T<_TScalar>& arg)
    {
        int8 idx = 0;
        _TScalar val = arg[0];
        for (int8 i = 1; i < 3; ++i)
        {
            if (arg[i] >= val)
                continue;
            idx = i;
            val = arg[idx];
        }
        return idx;
    }

    template<typename _TScalar>
    static int8 mididx(const UT_Vector3T<_TScalar>& arg)
    {
        if ( arg[1] > arg[0] )
        {
            if ( arg[2] > arg[1] )
                return 1;
            else if ( arg[2] > arg[0] )
                return 2;
            else
                return 0;
        }
        else
        {
            if ( arg[2] > arg[0] )
                return 0;
            else if ( arg[2] > arg[1] )
                return 2;
            else
                return 1;
        }
    }

    template<typename _TScalar>
    static int8 maxidx(const UT_Vector3T<_TScalar>& arg)
    {
        int8 idx = 0;
        _TScalar val = arg[0];
        for (int i = 1; i < 3; ++i)
        {
            if (arg[i] <= val)
                continue;
            idx = i;
            val = arg[idx];
        }
        return idx;
    }



    
template<typename _Ty>
SYS_FORCE_INLINE static _Ty radians(const _Ty degrees)
{ return degrees * PI / 180; }

template<typename _Ty>
SYS_FORCE_INLINE static _Ty degrees(const _Ty radians)
{ return radians * 180 / PI; }


template<typename _Ty>
SYS_FORCE_INLINE static typename _Ty::value_type tupleDistance2(const _Ty& val0, const _Ty& val1)
{
    if constexpr(::std::is_same_v<_Ty, int8> ||
                 ::std::is_same_v<_Ty, int16> ||
                 ::std::is_same_v<_Ty, int32> ||
                 ::std::is_same_v<_Ty, int64> ||
                 ::std::is_same_v<_Ty, fpreal16> ||
                 ::std::is_same_v<_Ty, fpreal32> ||
                 ::std::is_same_v<_Ty, fpreal64> )
    {
        return detail::numDistance2(val0, val1);
    }
    else
        return detail::vectorDistance2(val0, val1);
}
    
#define __GFE_Math_FUNC_tupleDistance2(TYPE)                                   \
SYS_FORCE_INLINE static TYPE tupleDistance2(const TYPE val0, const TYPE val1)  \
{ return detail::numDistance2(val0, val1); }                                   \

__GFE_Math_FUNC_tupleDistance2(int8);
__GFE_Math_FUNC_tupleDistance2(int16);
__GFE_Math_FUNC_tupleDistance2(int32);
__GFE_Math_FUNC_tupleDistance2(int64);
__GFE_Math_FUNC_tupleDistance2(fpreal16);
__GFE_Math_FUNC_tupleDistance2(fpreal32);
__GFE_Math_FUNC_tupleDistance2(fpreal64);
    
#undef __GFE_Math_FUNC_tupleDistance2
    
    
template<typename FLOAT_T>
static UT_Vector3T<FLOAT_T> blendDir(const UT_Vector3T<FLOAT_T>& dir0, const UT_Vector3T<FLOAT_T>& dir1, const FLOAT_T blend)
{
    UT_QuaternionT<FLOAT_T> q;
    q.updateFromVectors(dir0, dir1);
    q = UT_QuaternionT<FLOAT_T>(0,0,0,1).interpolate(q, blend);
    return q.rotate(dir0);
}

template<typename VECTOR_T, typename T_value_type>
SYS_FORCE_INLINE static VECTOR_T vlerp(const VECTOR_T a, const VECTOR_T b, const T_value_type t)
{ return lerp(a, b, t); }

template<>
SYS_FORCE_INLINE static int16 vlerp<int16, fpreal16>(const int16 a, const int16 b, const fpreal16 t)
{ return a + (b - a) * t; }

template<>
SYS_FORCE_INLINE static int vlerp<int, fpreal32>(const int a, const int b, const fpreal32 t)
{ return a + (b - a) * t; }

template<>
SYS_FORCE_INLINE static int64 vlerp<int64, fpreal64>(const int64 a, const int64 b, const fpreal64 t)
{ return a + (b - a) * t; }

template<>
SYS_FORCE_INLINE static fpreal16 vlerp<fpreal16, fpreal16>(const fpreal16 a, const fpreal16 b, const fpreal16 t)
{ return a + (b - a) * t; }

template<>
SYS_FORCE_INLINE static fpreal32 vlerp<fpreal32, fpreal32>(const fpreal32 a, const fpreal32 b, const fpreal32 t)
{ return SYSlerp(a, b, t); }

template<>
SYS_FORCE_INLINE static fpreal vlerp<fpreal, fpreal>(const fpreal a, const fpreal b, const fpreal t)
{ return SYSlerp(a, b, t); }



#define GFE_MATH_ReverseROC 1


template<typename _Ty>
SYS_FORCE_INLINE static _Ty distToLine(
        const UT_Vector3T<_Ty>& pos,
        const UT_Vector3T<_Ty>& pos0,
        const UT_Vector3T<_Ty>& pos1)
{ return sqrt(length2(cross(pos0 - pos, pos1 - pos)) / distance2(pos0, pos1)); }



//SYS_FORCE_INLINE static fpreal distToLine(
//     const UT_Vector3T<fpreal>& pos,
//     const UT_Vector3T<fpreal>& pos0,
//     const UT_Vector3T<fpreal>& pos1)
// { return sqrt(length2(cross(pos0 - pos, pos1 - pos)) / distance2(pos0, pos1)); }



template<typename _Ty>
SYS_FORCE_INLINE static _Ty distToLine(
    const UT_Vector3T<_Ty>& dir0,
    const UT_Vector3T<_Ty>& dir1,
    const UT_Vector3T<_Ty>& pos0,
    const UT_Vector3T<_Ty>& pos1)
{ return sqrt(length2(cross(dir0, dir1)) / distance2(pos0, pos1)); }


template<typename _Ty>
static _Ty circleRadius3Point(
    const UT_Vector3T<_Ty>& pos0,
    const UT_Vector3T<_Ty>& pos1,
    const UT_Vector3T<_Ty>& pos2
)
{
    const _Ty x1 = pos0[0],
            x2 = pos1[0],
            x3 = pos2[0],
            y1 = pos0[1],
            y2 = pos1[1],
            y3 = pos2[1],
            z1 = pos0[2],
            z2 = pos1[2],
            z3 = pos2[2];

    const _Ty a1 =  (y1 * z2 - y2 * z1 - y1 * z3 + y3 * z1 + y2 * z3 - y3 * z2),
            b1 = -(x1 * z2 - x2 * z1 - x1 * z3 + x3 * z1 + x2 * z3 - x3 * z2),
            c1 =  (x1 * y2 - x2 * y1 - x1 * y3 + x3 * y1 + x2 * y3 - x3 * y2),
            d1 = -(x1 * y2 * z3 - x1 * y3 * z2 - x2 * y1 * z3 + x2 * y3 * z1 + x3 * y1 * z2 - x3 * y2 * z1);

    const _Ty a2 = 2 * (x2 - x1),
            b2 = 2 * (y2 - y1),
            c2 = 2 * (z2 - z1),
            d2 = x1 * x1 + y1 * y1 + z1 * z1 - x2 * x2 - y2 * y2 - z2 * z2;

    const _Ty a3 = 2 * (x3 - x1),
            b3 = 2 * (y3 - y1),
            c3 = 2 * (z3 - z1),
            d3 = x1 * x1 + y1 * y1 + z1 * z1 - x3 * x3 - y3 * y3 - z3 * z3;

    UT_Vector3T<_Ty> circleCenter;
    circleCenter[0] = -(b1 * c2 * d3 - b1 * c3 * d2 - b2 * c1 * d3 + b2 * c3 * d1 + b3 * c1 * d2 - b3 * c2 * d1)
                     / (a1 * b2 * c3 - a1 * b3 * c2 - a2 * b1 * c3 + a2 * b3 * c1 + a3 * b1 * c2 - a3 * b2 * c1);
    circleCenter[1] = (a1 * c2 * d3 - a1 * c3 * d2 - a2 * c1 * d3 + a2 * c3 * d1 + a3 * c1 * d2 - a3 * c2 * d1)
                     / (a1 * b2 * c3 - a1 * b3 * c2 - a2 * b1 * c3 + a2 * b3 * c1 + a3 * b1 * c2 - a3 * b2 * c1);
    circleCenter[2] = -(a1 * b2 * d3 - a1 * b3 * d2 - a2 * b1 * d3 + a2 * b3 * d1 + a3 * b1 * d2 - a3 * b2 * d1)
                     / (a1 * b2 * c3 - a1 * b3 * c2 - a2 * b1 * c3 + a2 * b3 * c1 + a3 * b1 * c2 - a3 * b2 * c1);

    // addpoint(0, circleCenter);

    // return distance(circleCenter, pos0);

    // #if GFE_MATH_ReverseROC
    //     return 1.0 / distance(circleCenter, pos0);
    // #else
    //     return distance(circleCenter, pos0);
    // #endif
#if GFE_MATH_ReverseROC
    return sqrt(distance2(pos0, pos2) / distance2(circleCenter, pos0));
#else
    return sqrt(distance2(circleCenter, pos0) / distance2(pos0, pos2));
#endif
}


#undef GFE_MATH_ReverseROC





    
template<typename FLOAT_T>
static bool pointInTriangleT(
    const UT_Vector3T<FLOAT_T>& p,
    const UT_Vector3T<FLOAT_T>& a,
    const UT_Vector3T<FLOAT_T>& b,
    const UT_Vector3T<FLOAT_T>& c
)
{
    const UT_Vector3T<FLOAT_T> ba = b - a;
    const UT_Vector3T<FLOAT_T> cb = c - b;
    const UT_Vector3T<FLOAT_T> ac = a - c;
    const UT_Vector3T<FLOAT_T> n = cross(ac, ba);  // Same as n = ba X ca

    UT_Vector3T<FLOAT_T> px = p - a;
    UT_Vector3T<FLOAT_T> nx = cross(ba, px);
    if (nx.dot(n) < 0) return false;
    
    px = p - b;
    nx = cross(cb, px);
    if (nx.dot(n) < 0) return false;
    
    px = p - c;
    nx = cross(ac, px);
    if (nx.dot(n) < 0) return false;
    
    return true;
}

/*
template<typename FLOAT_T>
static bool pointInTriangleT1(
    const UT_Vector3T<FLOAT_T>& pointPos,
    const UT_Vector3T<FLOAT_T>& triPosCenter,
    const UT_Vector3T<FLOAT_T>& triPosLeft,
    const UT_Vector3T<FLOAT_T>& triPosRight
)
{
    const UT_Vector3T<FLOAT_T> cl = triPosLeft   - triPosCenter;
    const UT_Vector3T<FLOAT_T> cr = triPosRight  - triPosCenter;
    const UT_Vector3T<FLOAT_T> cp = pointPos     - triPosCenter;
    const UT_Vector3T<FLOAT_T> lr = triPosRight  - triPosLeft;
    const UT_Vector3T<FLOAT_T> lc = triPosLeft   - triPosCenter;
    const UT_Vector3T<FLOAT_T> lp = triPosLeft   - pointPos;
    const UT_Vector3T<FLOAT_T> rl = triPosLeft   - triPosRight;
    const UT_Vector3T<FLOAT_T> rc = triPosCenter - triPosRight;
    const UT_Vector3T<FLOAT_T> rp = pointPos     - triPosRight;
    
    UT_Vector3T<FLOAT_T> dir0 = cross(cl, cr);
    UT_Vector3T<FLOAT_T> dir1 = cross(cl, cp);
    UT_Vector3T<FLOAT_T> dir2 = cross(lr, lc);
    UT_Vector3T<FLOAT_T> dir3 = cross(lr, lp);
    UT_Vector3T<FLOAT_T> dir4 = cross(rc, rl);
    UT_Vector3T<FLOAT_T> dir5 = cross(rc, rp);
    dir0.normalize();
    dir1.normalize();
    dir2.normalize();
    dir3.normalize();
    dir4.normalize();
    dir5.normalize();
    return dot(dir0, dir1) > 1e-05 &&
           dot(dir2, dir3) > 1e-05 &&
           dot(dir4, dir5) > 1e-05;
}
*/

// typedef pointInTriangleT<fpreal16>   pointInTriangleH;
// typedef pointInTriangleT<fpreal32>   pointInTriangleF;
// typedef pointInTriangleT<fpreal64>   pointInTriangleD;
// typedef pointInTriangleT<fpreal>     pointInTriangleR;

#define pointInTriangleH pointInTriangleT<fpreal16>
#define pointInTriangleF pointInTriangleT<fpreal32>
#define pointInTriangleD pointInTriangleT<fpreal64>
#define pointInTriangleR pointInTriangleT<fpreal>
    
} // End of Namespace GFE_Math



#endif
