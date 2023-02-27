
#pragma once

#ifndef __GFE_Math_h__
#define __GFE_Math_h__

//#include "GFE/GFE_Math.h"

#include "GA/GA_Detail.h"
//#include "GA/GA_SplittableRange.h"



//#include "GFE/GFE_Type.h


namespace GFE_Math {




#define ReverseROC 1


template<typename T>
SYS_FORCE_INLINE
static T
    distToLine(
        const UT_Vector3T<T>& pos,
        const UT_Vector3T<T>& pos0,
        const UT_Vector3T<T>& pos1
    ) {
    return sqrt(length2(cross(pos0 - pos, pos1 - pos)) / distance2(pos0, pos1));
}

//SYS_FORCE_INLINE
//static fpreal
//    distToLine(
//        const UT_Vector3T<fpreal>& pos,
//        const UT_Vector3T<fpreal>& pos0,
//        const UT_Vector3T<fpreal>& pos1
//    ) {
//    return sqrt(length2(cross(pos0 - pos, pos1 - pos)) / distance2(pos0, pos1));
//}



template<typename T>
SYS_FORCE_INLINE
static T
distToLine(
    const UT_Vector3T<T>& dir0,
    const UT_Vector3T<T>& dir1,
    const UT_Vector3T<T>& pos0,
    const UT_Vector3T<T>& pos1
) {
    return sqrt(length2(cross(dir0, dir1)) / distance2(pos0, pos1));
}


template<typename T>
static T
circleRadius3Point(
    const UT_Vector3T<T>& pos0,
    const UT_Vector3T<T>& pos1,
    const UT_Vector3T<T>& pos2
) {
    const T x1 = pos0[0],
        x2 = pos1[0],
        x3 = pos2[0],
        y1 = pos0[1],
        y2 = pos1[1],
        y3 = pos2[1],
        z1 = pos0[2],
        z2 = pos1[2],
        z3 = pos2[2];

    const T a1 = (y1 * z2 - y2 * z1 - y1 * z3 + y3 * z1 + y2 * z3 - y3 * z2),
        b1 = -(x1 * z2 - x2 * z1 - x1 * z3 + x3 * z1 + x2 * z3 - x3 * z2),
        c1 = (x1 * y2 - x2 * y1 - x1 * y3 + x3 * y1 + x2 * y3 - x3 * y2),
        d1 = -(x1 * y2 * z3 - x1 * y3 * z2 - x2 * y1 * z3 + x2 * y3 * z1 + x3 * y1 * z2 - x3 * y2 * z1);

    const T a2 = 2 * (x2 - x1),
        b2 = 2 * (y2 - y1),
        c2 = 2 * (z2 - z1),
        d2 = x1 * x1 + y1 * y1 + z1 * z1 - x2 * x2 - y2 * y2 - z2 * z2;

    const T a3 = 2 * (x3 - x1),
        b3 = 2 * (y3 - y1),
        c3 = 2 * (z3 - z1),
        d3 = x1 * x1 + y1 * y1 + z1 * z1 - x3 * x3 - y3 * y3 - z3 * z3;

    UT_Vector3T<T> circleCenter;
    circleCenter[0] = -(b1 * c2 * d3 - b1 * c3 * d2 - b2 * c1 * d3 + b2 * c3 * d1 + b3 * c1 * d2 - b3 * c2 * d1)
        / (a1 * b2 * c3 - a1 * b3 * c2 - a2 * b1 * c3 + a2 * b3 * c1 + a3 * b1 * c2 - a3 * b2 * c1);
    circleCenter[1] = (a1 * c2 * d3 - a1 * c3 * d2 - a2 * c1 * d3 + a2 * c3 * d1 + a3 * c1 * d2 - a3 * c2 * d1)
        / (a1 * b2 * c3 - a1 * b3 * c2 - a2 * b1 * c3 + a2 * b3 * c1 + a3 * b1 * c2 - a3 * b2 * c1);
    circleCenter[2] = -(a1 * b2 * d3 - a1 * b3 * d2 - a2 * b1 * d3 + a2 * b3 * d1 + a3 * b1 * d2 - a3 * b2 * d1)
        / (a1 * b2 * c3 - a1 * b3 * c2 - a2 * b1 * c3 + a2 * b3 * c1 + a3 * b1 * c2 - a3 * b2 * c1);

    // addpoint(0, circleCenter);

    // return distance(circleCenter, pos0);

    // #if ReverseROC
    //     return 1.0 / distance(circleCenter, pos0);
    // #else
    //     return distance(circleCenter, pos0);
    // #endif
#if ReverseROC
    return sqrt(distance2(pos0, pos2) / distance2(circleCenter, pos0));
#else
    return sqrt(distance2(circleCenter, pos0) / distance2(pos0, pos2));
#endif
}







} // End of namespace GFE_Math

#endif
