
#pragma once

#ifndef __GA_FeE_Measure_h__
#define __GA_FeE_Measure_h__

//#include "GA_FeE/GA_FeE_Measure.h"

#include "GA/GA_Detail.h"
#include "GA_FeE/GA_FeE_Attribute.h"


namespace GA_FeE_Measure {








template<typename T>
static T
heronsFormula(
    const UT_Vector3T<T>& pos0,
    const UT_Vector3T<T>& pos1,
    const UT_Vector3T<T>& pos2
)
{
    //Heron's formula
    const T length0 = pos0.distance(pos1);
    const T length1 = pos1.distance(pos2);
    const T length2 = pos2.distance(pos0);
    const T halfP = (length0 + length1 + length2) * 0.5;
    return sqrt(halfP * (halfP - length0) * (halfP - length1) * (halfP - length2));
}

//using triangleArea = heronsFormula;
//typedef heronsFormula triangleArea;



template<typename T>
static T
bretschneidersFormula0(
    const UT_Vector3T<T>& pos0,
    const UT_Vector3T<T>& pos1,
    const UT_Vector3T<T>& pos2,
    const UT_Vector3T<T>& pos3
)
{
    //Bretschneider's formula 0
    //https://en.wikipedia.org/wiki/Bretschneider's_formula
    //https://zhuanlan.zhihu.com/p/101219058
    //https://zhuanlan.zhihu.com/p/110025234
    const T length0 = pos0.distance2(pos1);
    const T length1 = pos1.distance2(pos2);
    const T length2 = pos2.distance2(pos3);
    const T length3 = pos3.distance2(pos0);
    T e = pos0.distance2(pos2);
    T f = pos1.distance2(pos3);
    e *= f;//e == (ef)
    f = length1 + length3 - length0 - length2;
    f *= f;

    return sqrt(4 * e - f) * 0.25;
}


//typedef bretschneidersFormula0 quadrangleArea;


template<typename T>
static T
bretschneidersFormula1(
    const UT_Vector3T<T>& pos0,
    const UT_Vector3T<T>& pos1,
    const UT_Vector3T<T>& pos2,
    const UT_Vector3T<T>& pos3
)
{
    //Bretschneider's formula 1
    const T length0 = pos0.distance(pos1);
    const T length1 = pos1.distance(pos2);
    const T length2 = pos2.distance(pos3);
    const T length3 = pos3.distance(pos0);

    T length4 = pos0.distance(pos2);
    T length5 = pos1.distance(pos3);

    length5 *= length4;
    length4 = length0 * length2 + length1 * length3;

    const T halfP = (length0 + length1 + length2 + length3) * 0.5;
#if 1
    const T area = (halfP - length0) * (halfP - length1) * (halfP - length2) * (halfP - length3)
                  - (length4 + length5) * (length4 - length5) * 0.25;
#else
    const T area = sqrt((halfP - length0) * (halfP - length1) * (halfP - length2) * (halfP - length3)
                  - length0 * length1 * length2 * length3 * (cos(angle) + 1) * 0.5);
#endif
    return sqrt(area);
}




//GU_Detail::compute3DArea(const GA_Offset primoff)


//static fpreal32
//primArea32(
//    const GA_Detail* const geo,
//    const GA_Offset primoff
//)
//{
//    const GA_OffsetListRef vertices = geo->getPrimitiveVertexList(primoff);
//
//    const bool closed = vertices.getExtraFlag();
//    if (!closed)
//        return 0.0;
//
//    const GA_Size numvtx = vertices.size();
//    if (numvtx < 3)
//        return 0.0;
//
//
//
//    switch (geo->getPrimitiveTypeId(primoff))
//    {
//    case GA_PRIMPOLY:
//    {
//    }
//    break;
//    default:
//        return 0.0;
//    }
//
//
//
//    UT_Vector3T<fpreal32> pos0 = geo->getPos3(geo->vertexPoint(vertices[0]));
//    UT_Vector3T<fpreal32> pos1 = geo->getPos3(geo->vertexPoint(vertices[1]));
//    const UT_Vector3T<fpreal32> pos2 = geo->getPos3(geo->vertexPoint(vertices[2]));
//
//    //GA_Offset ptoff0 = geo->vertexPoint(geo->getPrimitiveVertexOffset(primoff, 0));
//
//    switch (numvtx)
//    {
//    case 3:
//    {
//        return heronsFormula(pos0, pos1, pos2);
//    }
//    break;
//    case 4:
//    {
//        const UT_Vector3T<fpreal32> pos3 = geo->getPos3(geo->vertexPoint(vertices[3]));
//        return bretschneidersFormula0(pos0, pos1, pos2, pos3);
//    }
//    break;
//    //default:
//    }
//
//
//
//    const UT_Vector3T<fpreal32> dir0 = pos1 - pos0;
//    const UT_Vector3T<fpreal32> dir1 = pos2 - pos1;
//    const UT_Vector3T<fpreal32> crossdir0B = cross(dir0, dir1);
//    fpreal32 lengthdir0B = crossdir0B.length();
//    lengthdir0B = 1.0 / lengthdir0B;
//    const fpreal32 cosnx = ((pos1[1] - pos0[1]) * (pos2[2] - pos0[2]) - (pos2[1] - pos0[1]) * (pos1[2] - pos0[2])) * lengthdir0B;
//    const fpreal32 cosny = ((pos2[0] - pos0[0]) * (pos1[2] - pos0[2]) - (pos1[0] - pos0[0]) * (pos2[2] - pos0[2])) * lengthdir0B;
//    const fpreal32 cosnz = ((pos1[0] - pos0[0]) * (pos2[1] - pos0[1]) - (pos2[0] - pos0[0]) * (pos1[1] - pos0[1])) * lengthdir0B;
//
//    fpreal32 areaSum = 0.0;
//    pos0 = geo->getPos3(geo->vertexPoint(vertices[numvtx - 1]));
//    for (GA_Size i(0); i < numvtx; ++i)
//    {
//        pos1 = geo->getPos3(geo->vertexPoint(vertices[i]));
//        areaSum += cosnz * (pos0[0] * pos1[1] - pos1[0] * pos0[1])
//            + cosnx * (pos0[1] * pos1[2] - pos1[1] * pos0[2])
//            + cosny * (pos0[2] * pos1[0] - pos1[2] * pos0[0]);
//        pos0 = pos1;
//    }
//    return abs(0.5 * areaSum);
//}



//GU_Detail::compute3DArea(const GA_Offset primoff)


#define primAreaMacro(T)                                                                                                                                \
static fpreal##T                                                                                                                                        \
primArea##T(                                                                                                                                            \
    const GA_Detail* const geo,                                                                                                                         \
    const GA_Offset primoff                                                                                                                             \
)                                                                                                                                                       \
{                                                                                                                                                       \
    const GA_OffsetListRef vertices = geo->getPrimitiveVertexList(primoff);                                                                             \
                                                                                                                                                        \
    const bool closed = vertices.getExtraFlag();                                                                                                       \
    if (!closed)                                                                                                                                        \
    return 0.0;                                                                                                                                         \
                                                                                                                                                        \
    const GA_Size numvtx = vertices.size();                                                                                                            \
    if (numvtx < 3)                                                                                                                                     \
        return 0.0;                                                                                                                                     \
                                                                                                                                                        \
                                                                                                                                                        \
                                                                                                                                                        \
    switch (geo->getPrimitiveTypeId(primoff))                                                                                                           \
    {                                                                                                                                                   \
    case GA_PRIMPOLY:                                                                                                                                   \
    {                                                                                                                                                   \
    }                                                                                                                                                   \
    break;                                                                                                                                              \
    default:                                                                                                                                            \
        return 0.0;                                                                                                                                     \
    }                                                                                                                                                   \
                                                                                                                                                        \
                                                                                                                                                        \
                                                                                                                                                        \
    UT_Vector3T<fpreal##T> pos0 = geo->getPos3(geo->vertexPoint(vertices[0]));                                                                           \
    UT_Vector3T<fpreal##T> pos1 = geo->getPos3(geo->vertexPoint(vertices[1]));                                                                           \
    const UT_Vector3T<fpreal##T> pos2 = geo->getPos3(geo->vertexPoint(vertices[2]));                                                                     \
                                                                                                                                                        \
    /*GA_Offset ptoff0 = geo->vertexPoint(geo->getPrimitiveVertexOffset(primoff, 0));*/                                                                   \
                                                                                                                                                        \
    switch (numvtx)                                                                                                                                     \
    {                                                                                                                                                   \
    case 3:                                                                                                                                             \
    {                                                                                                                                                   \
        return heronsFormula(pos0, pos1, pos2);                                                                                                         \
    }                                                                                                                                                   \
    break;                                                                                                                                              \
    case 4:                                                                                                                                             \
    {                                                                                                                                                   \
        const UT_Vector3T<fpreal##T> pos3 = geo->getPos3(geo->vertexPoint(vertices[3]));                                                                 \
        return bretschneidersFormula0(pos0, pos1, pos2, pos3);                                                                                          \
    }                                                                                                                                                   \
    break;                                                                                                                                              \
    default:                                                                                                                                          \
        UT_ASSERT_MSG(0, "unsupport numvtx");                                                                                                            \
        break;                                                                                                                                           \
    }                                                                                                                                                   \
                                                                                                                                                        \
                                                                                                                                                        \
                                                                                                                                                        \
    const UT_Vector3T<fpreal##T> dir0 = pos1 - pos0;                                                                                                     \
    const UT_Vector3T<fpreal##T> dir1 = pos2 - pos1;                                                                                                     \
    const UT_Vector3T<fpreal##T> crossdir0B = cross(dir0, dir1);                                                                                         \
    fpreal##T lengthdir0B = crossdir0B.length();                                                                                                         \
    lengthdir0B = 1.0 / lengthdir0B;                                                                                                                    \
    const fpreal##T cosnx = ((pos1[1] - pos0[1]) * (pos2[2] - pos0[2]) - (pos2[1] - pos0[1]) * (pos1[2] - pos0[2])) * lengthdir0B;                       \
    const fpreal##T cosny = ((pos2[0] - pos0[0]) * (pos1[2] - pos0[2]) - (pos1[0] - pos0[0]) * (pos2[2] - pos0[2])) * lengthdir0B;                       \
    const fpreal##T cosnz = ((pos1[0] - pos0[0]) * (pos2[1] - pos0[1]) - (pos2[0] - pos0[0]) * (pos1[1] - pos0[1])) * lengthdir0B;                       \
                                                                                                                                                        \
    fpreal##T areaSum = 0.0;                                                                                                                             \
    pos0 = geo->getPos3(geo->vertexPoint(vertices[numvtx - 1]));                                                                                        \
    for (GA_Size i(0); i < numvtx; ++i)                                                                                                                 \
    {                                                                                                                                                   \
        pos1 = geo->getPos3(geo->vertexPoint(vertices[i]));                                                                                             \
        areaSum += cosnz * (pos0[0] * pos1[1] - pos1[0] * pos0[1])                                                                                      \
            + cosnx * (pos0[1] * pos1[2] - pos1[1] * pos0[2])                                                                                           \
            + cosny * (pos0[2] * pos1[0] - pos1[2] * pos0[0]);                                                                                          \
        pos0 = pos1;                                                                                                                                    \
    }                                                                                                                                                   \
    return abs(0.5 * areaSum);                                                                                                                          \
}

primAreaMacro(32);
primAreaMacro(64);



template<typename T>
static T
primArea(
    const GA_Detail* const geo,
    const GA_Offset primoff
)
{
    if constexpr (std::is_same<T, float>::value)
        return primArea32(geo, primoff);
    else
        return primArea64(geo, primoff);
}




template<typename T>
static T
//GU_Detail::compute3DArea(const GA_Offset primoff)
//primArea(const GU_Detail* geo, const GA_Offset primoff, GA_ROHandleT<UT_Vector3T<T>> posAttrib_h, GA_AttributeOwner attribOwner)
primArea(
    const GA_Detail* const geo,
    const GA_Offset primoff,
    const GA_ROHandleT<UT_Vector3T<T>>& posAttrib_h
)
{
    const GA_OffsetListRef vertices = geo->getPrimitiveVertexList(primoff);

    const bool closed = vertices.getExtraFlag();
    if (!closed)
        return 0.0;

    const GA_Size numvtx = vertices.size();
    if (numvtx < 3)
        return 0.0;



    switch (geo->getPrimitiveTypeId(primoff))
    {
    case GA_PRIMPOLY:
    {
    }
    break;
    default:
        return 0.0;
    }


    const GA_AttributeOwner attribOwner = posAttrib_h.getAttribute()->getOwner();

    UT_Vector3T<T> pos0, pos1, pos2;

    switch (attribOwner)
    {
    case GA_ATTRIB_VERTEX:
    {
        pos0 = posAttrib_h.get(vertices[0]);
        pos1 = posAttrib_h.get(vertices[1]);
        pos2 = posAttrib_h.get(vertices[2]);
    }
    break;
    case GA_ATTRIB_POINT:
    {
        pos0 = posAttrib_h.get(geo->vertexPoint(vertices[0]));
        pos1 = posAttrib_h.get(geo->vertexPoint(vertices[1]));
        pos2 = posAttrib_h.get(geo->vertexPoint(vertices[2]));
    }
    break;
    default:
        return 0.0;
    }
    //UT_Vector3T<T> pos1 = geo->getPos3(geo->vertexPoint(vertices[0]));
    //UT_Vector3T<T> pos1 = geo->getPos3(geo->vertexPoint(vertices[1]));
    //UT_Vector3T<T> pos2 = geo->getPos3(geo->vertexPoint(vertices[2]));

    //GA_Offset ptoff0 = geo->vertexPoint(geo->getPrimitiveVertexOffset(primoff, 0));

    switch (numvtx)
    {
    case 3:
    {
        return heronsFormula(pos0, pos1, pos2);
    }
        break;
    case 4:
    {
        //UT_Vector3T<T> pos3 = geo->getPos3(geo->vertexPoint(vertices[3]));
        UT_Vector3T<T> pos3;
        switch (attribOwner)
        {
        case GA_ATTRIB_VERTEX:
        {
            pos3 = posAttrib_h.get(vertices[3]);
        }
        break;
        case GA_ATTRIB_POINT:
        {
            pos3 = posAttrib_h.get(geo->vertexPoint(vertices[3]));
        }
        break;
        default:
            return 0.0;
        }
        return bretschneidersFormula0(pos0, pos1, pos2, pos3);
    }
        break;
    default:
        UT_ASSERT_MSG(0, "unhandled numvtx");
        break;
    }



    const UT_Vector3T<T> dir0 = pos1 - pos0;
    const UT_Vector3T<T> dir1 = pos2 - pos1;
    const UT_Vector3T<T> crossdir0B = cross(dir0, dir1);
    T lengthdir0B = crossdir0B.length();
    lengthdir0B = 1.0 / lengthdir0B;
    const T cosnx = ((pos1[1] - pos0[1]) * (pos2[2] - pos0[2]) - (pos2[1] - pos0[1]) * (pos1[2] - pos0[2])) * lengthdir0B;
    const T cosny = ((pos2[0] - pos0[0]) * (pos1[2] - pos0[2]) - (pos1[0] - pos0[0]) * (pos2[2] - pos0[2])) * lengthdir0B;
    const T cosnz = ((pos1[0] - pos0[0]) * (pos2[1] - pos0[1]) - (pos2[0] - pos0[0]) * (pos1[1] - pos0[1])) * lengthdir0B;

    T areaSum = 0.0;
    //pos0 = geo->getPos3(geo->vertexPoint(vertices[numvtx - 1]));
    switch (attribOwner)
    {
    case GA_ATTRIB_VERTEX:
    {
        pos0 = posAttrib_h.get(vertices[numvtx - 1]);
        for (GA_Size vtxpnum(0); vtxpnum < numvtx; ++vtxpnum)
        {
            //pos1 = geo->getPos3(geo->vertexPoint(vertices[vtxpnum]));
            pos1 = posAttrib_h.get(vertices[vtxpnum]);
            areaSum += cosnz * (pos0[0] * pos1[1] - pos1[0] * pos0[1])
                     + cosnx * (pos0[1] * pos1[2] - pos1[1] * pos0[2])
                     + cosny * (pos0[2] * pos1[0] - pos1[2] * pos0[0]);
            pos0 = pos1;
        }
    }
    break;
    case GA_ATTRIB_POINT:
    {
        pos0 = posAttrib_h.get(geo->vertexPoint(vertices[numvtx - 1]));
        for (GA_Size vtxpnum(0); vtxpnum < numvtx; ++vtxpnum)
        {
            //pos1 = geo->getPos3(geo->vertexPoint(vertices[vtxpnum]));
            pos1 = posAttrib_h.get(geo->vertexPoint(vertices[vtxpnum]));
            areaSum += cosnz * (pos0[0] * pos1[1] - pos1[0] * pos0[1])
                     + cosnx * (pos0[1] * pos1[2] - pos1[1] * pos0[2])
                     + cosny * (pos0[2] * pos1[0] - pos1[2] * pos0[0]);
            pos0 = pos1;
        }
    }
    break;
    default:
        return 0.0;
    }

    return abs(0.5 * areaSum);
}



template<typename T, typename T1>
static void
primArea(
    const GA_Detail* const geo,
    const GA_RWHandleT<T>& areaAttrib_h,
    const GA_ROHandleT<UT_Vector3T<T1>>& posAttrib_h,
    const GA_PrimitiveGroup* const geoPrimGroup = nullptr,
    const exint subscribeRatio = 16,
    const exint minGrainSize = 1024
)
{
    //GU_Measure::computeArea(*geo, areaAttrib_h, geoPrimGroup);
    const GA_SplittableRange geoSplittableRange0(geo->getPrimitiveRange(geoPrimGroup));
    UTparallelFor(geoSplittableRange0, [geo, &areaAttrib_h, &posAttrib_h](const GA_SplittableRange& r)
    {
        GA_Offset start, end;
        for (GA_Iterator it(r); it.blockAdvance(start, end); )
        {
            for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
            {
                const T area = primArea(geo, elemoff, posAttrib_h);
                areaAttrib_h.set(elemoff, area);
            }
        }
    }, subscribeRatio, minGrainSize);
}



template<typename T>
static void
primArea(
    const GA_Detail* const geo,
    const GA_RWHandleT<T>& areaAttrib_h,
    const GA_PrimitiveGroup* const geoPrimGroup = nullptr,
    const exint subscribeRatio = 16,
    const exint minGrainSize = 1024
)
{
    //GU_Measure::computeArea(*geo, areaAttrib_h, geoPrimGroup);poly
    const GA_SplittableRange geoSplittableRange0(geo->getPrimitiveRange(geoPrimGroup));
    UTparallelFor(geoSplittableRange0, [geo, &areaAttrib_h](const GA_SplittableRange& r)
    {
        GA_Offset start, end;
        for (GA_Iterator it(r); it.blockAdvance(start, end); )
        {
            for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
            {
                const T area = primArea<T>(geo, elemoff);
                areaAttrib_h.set(elemoff, area);
            }
        }
    }, subscribeRatio, minGrainSize);
}

SYS_FORCE_INLINE
static void
primArea(
    const GA_Detail* const geo,
    GA_Attribute* const p_h,
    const GA_PrimitiveGroup* const geoPrimGroup = nullptr,
    const exint subscribeRatio = 16,
    const exint minGrainSize = 1024
)
{
    switch (p_h->getAIFTuple()->getStorage(p_h))
    {
    case GA_STORE_REAL16:
        primArea<fpreal16>(geo, p_h, geoPrimGroup, subscribeRatio, minGrainSize);
        break;
    case GA_STORE_REAL32:
        primArea<fpreal32>(geo, p_h, geoPrimGroup, subscribeRatio, minGrainSize);
        break;
    case GA_STORE_REAL64:
        primArea<fpreal64>(geo, p_h, geoPrimGroup, subscribeRatio, minGrainSize);
        break;
    default:
        UT_ASSERT_MSG(0, "unhandled storage type");
        break;
    }
}







//#define DEF_FUNC_AddAttrib(DEF_FUNC_Parm_FuncName, DEF_FUNC_Parm_ParmPack, DEF_FUNC_Parm_AttribName, DEF_FUNC_Parm_Defaults, DEF_FUNC_Parm_Storage, DEF_FUNC_Parm_SubscribeRatio, DEF_FUNC_Parm_MinGrainSize)     \
//        SYS_FORCE_INLINE                                                                                                                                                                                          \
//        static GA_Attribute*                                                                                                                                                                                      \
//        DEF_FUNC_Parm_FuncName(                                                                                                                                                                                   \
//            GA_Detail* const geo,                                                                                                                                                                                      \
//            DEF_FUNC_Parm_ParmPack                                                                                                                                                                                \
//            const UT_StringHolder& name = #DEF_FUNC_Parm_AttribName,                                                                                                                                              \
//            const GA_PrimitiveGroup* const geoPrimGroup = nullptr,                                                                                                                                                      \
//            const GA_Defaults& defaults = GA_Defaults(DEF_FUNC_Parm_Defaults),                                                                                                                                    \
//            const GA_Storage storage = DEF_FUNC_Parm_Storage,                                                                                                                                                    \
//            const GA_ReuseStrategy& reuse = GA_ReuseStrategy(),                                                                                                                                                   \
//            const exint subscribeRatio = DEF_FUNC_Parm_SubscribeRatio,                                                                                                                                           \
//            const exint minGrainSize = DEF_FUNC_Parm_MinGrainSize                                                                                                                                                \
//        )                                                                                                                                                                                                         \
//        {                                                                                                                                                                                                         \
//            return DEF_FUNC_Parm_FuncName(geo, name, geoPrimGroup, defaults, 0, 0, storage, reuse, subscribeRatio, minGrainSize);                                                                                 \
//        }                                                                                                                                                                                                         \
//        SYS_FORCE_INLINE                                                                                                                                                                                          \
//        static GA_Attribute*                                                                                                                                                                                      \
//        DEF_FUNC_Parm_FuncName(                                                                                                                                                                                   \
//            GA_Detail* const geo,                                                                                                                                                                                      \
//            DEF_FUNC_Parm_ParmPack                                                                                                                                                                                \
//            const UT_StringHolder& name = #DEF_FUNC_Parm_AttribName,                                                                                                                                              \
//            const GA_PrimitiveGroup* const geoPrimGroup = nullptr,                                                                                                                                                      \
//            const GA_Defaults& defaults = GA_Defaults(DEF_FUNC_Parm_Defaults),                                                                                                                                    \
//            const GA_Storage storage = DEF_FUNC_Parm_Storage,                                                                                                                                                    \
//            const exint subscribeRatio = DEF_FUNC_Parm_SubscribeRatio,                                                                                                                                           \
//            const exint minGrainSize = DEF_FUNC_Parm_MinGrainSize                                                                                                                                                \
//        )                                                                                                                                                                                                         \
//        {                                                                                                                                                                                                         \
//            return DEF_FUNC_Parm_FuncName(geo, name, geoPrimGroup, defaults, 0, 0, storage, GA_ReuseStrategy(), subscribeRatio, minGrainSize);                                                                    \
//        }                                                                                                                                                                                                         \
//
//
//
//
//DEF_FUNC_AddAttrib(addAttribPrimArea,          , area, -1.0, GA_STORE_INVALID, 16, 1024)


//using parmPack1 = const GA_ROHandleT<UT_Vector3T<T>>& posAttrib_h;
//DEF_FUNC_AddAttrib(addAttribPrimArea, parmPack1, area, -1.0, GA_STORE_INVALID, 16, 1024)

//DEF_FUNC_AddAttrib(addAttribPrimPerimeter, , perimeter, -1.0, GA_STORE_INVALID, 16, 1024)

//GA_FeE_Measure::addAttribPrimArea(geo, name, geoPrimGroup, defaults, storage, reuse, subscribeRatio, minGrainSize);





//GA_FeE_Measure::addAttribPrimArea(geo, name, geoPrimGroup, defaults, creation_args, attribute_options, storage, reuse, subscribeRatio, minGrainSize);

static GA_Attribute*
addAttribPrimArea(
    GA_Detail* const geo,
    const GA_PrimitiveGroup* const geoPrimGroup = nullptr,
    const GA_Storage storage = GA_STORE_INVALID,
    const UT_StringHolder& name = "area",
    const GA_Defaults& defaults = GA_Defaults(-1.0),
    const UT_Options* const creation_args = nullptr,
    const GA_AttributeOptions* const attribute_options = nullptr,
    const GA_ReuseStrategy& reuse = GA_ReuseStrategy(),
    const exint subscribeRatio = 16,
    const exint minGrainSize = 1024
)
{
    const GA_Storage finalStorage = storage == GA_STORE_INVALID ? geo->getP()->getAIFTuple()->getStorage(geo->getP()) : storage;
    GA_Attribute* const attribPtr = geo->getAttributes().createTupleAttribute(GA_ATTRIB_PRIMITIVE, GA_FEE_TOPO_SCOPE, name, finalStorage, 1, defaults, creation_args, attribute_options, reuse);
    //GA_Attribute* const attribPtr = geo->addFloatTuple(GA_ATTRIB_PRIMITIVE, name, 1, defaults, creation_args, attribute_options, finalStorage, reuse);
    switch (finalStorage)
    {
    case GA_STORE_REAL16:
        primArea<fpreal16>(geo, attribPtr, geoPrimGroup, subscribeRatio, minGrainSize);
        break;
    case GA_STORE_REAL32:
        primArea<fpreal32>(geo, attribPtr, geoPrimGroup, subscribeRatio, minGrainSize);
        break;
    case GA_STORE_REAL64:
        primArea<fpreal64>(geo, attribPtr, geoPrimGroup, subscribeRatio, minGrainSize);
        break;
    default:
        UT_ASSERT_MSG(0, "unhandled numvtx");
        break;
    }
    return attribPtr;
}


static GA_AttributeUPtr
addDetachedAttribPrimArea(
    const GA_Detail* const geo,
    const GA_PrimitiveGroup* const geoPrimGroup = nullptr,
    const GA_Storage storage = GA_STORE_INVALID,
    const GA_Defaults& defaults = GA_Defaults(-1.0),
    const GA_AttributeOptions* const attribute_options = nullptr,
    const exint subscribeRatio = 16,
    const exint minGrainSize = 1024
)
{
    const GA_Storage finalStorage = storage == GA_STORE_INVALID ? geo->getP()->getAIFTuple()->getStorage(geo->getP()) : storage;
    GA_AttributeUPtr attribUPtr = geo->getAttributes().createDetachedTupleAttribute(GA_ATTRIB_PRIMITIVE, finalStorage, 1, defaults, attribute_options);
    GA_Attribute* const attribPtr = attribUPtr.get();
    switch (finalStorage)
    {
    case GA_STORE_REAL16:
        primArea<fpreal16>(geo, attribPtr, geoPrimGroup, subscribeRatio, minGrainSize);
        break;
    case GA_STORE_REAL32:
        primArea<fpreal32>(geo, attribPtr, geoPrimGroup, subscribeRatio, minGrainSize);
        break;
    case GA_STORE_REAL64:
        primArea<fpreal64>(geo, attribPtr, geoPrimGroup, subscribeRatio, minGrainSize);
        break;
    default:
        UT_ASSERT_MSG(0, "unhandled numvtx");
        break;
    }
    return attribUPtr;
}












//GA_FeE_Measure::addAttribPrimArea(geo, posAttrib_h, name, geoPrimGroup, defaults, creation_args, attribute_options, storage, reuse, subscribeRatio, minGrainSize);

template<typename T>
static GA_Attribute*
addAttribPrimArea(
    GA_Detail* const geo,
    const GA_ROHandleT<UT_Vector3T<T>>& posAttrib_h,
    const GA_PrimitiveGroup* const geoPrimGroup = nullptr,
    const GA_Storage storage = GA_STORE_INVALID,
    const UT_StringHolder& name = "area",
    const GA_Defaults& defaults = GA_Defaults(-1.0),
    const UT_Options* const creation_args = nullptr,
    const GA_AttributeOptions* const attribute_options = nullptr,
    const GA_ReuseStrategy& reuse = GA_ReuseStrategy(),
    const exint subscribeRatio = 16,
    const exint minGrainSize = 1024
)
{
    const GA_Storage finalStorage = storage == GA_STORE_INVALID ? posAttrib_h->getAIFTuple()->getStorage(posAttrib_h.getAttribute()) : storage;
    GA_Attribute* const attribPtr = geo->getAttributes().createTupleAttribute(GA_ATTRIB_PRIMITIVE, GA_FEE_TOPO_SCOPE, name, finalStorage, 1, defaults, creation_args, attribute_options, reuse);
    //GA_Attribute* const attribPtr = geo->addFloatTuple(GA_ATTRIB_PRIMITIVE, name, 1, defaults, creation_args, attribute_options, finalStorage, reuse);
    //GA_RWHandleT<T> attribHandle(attribPtr);
    primArea<T>(geo, attribPtr, posAttrib_h, geoPrimGroup, subscribeRatio, minGrainSize);
    return attribPtr;
}


template<typename T>
static GA_AttributeUPtr
addDetachedAttribPrimArea(
    const GA_Detail* const geo,
    const GA_ROHandleT<UT_Vector3T<T>>& posAttrib_h,
    const GA_PrimitiveGroup* const geoPrimGroup = nullptr,
    const GA_Storage storage = GA_STORE_INVALID,
    const GA_Defaults& defaults = GA_Defaults(-1.0),
    const GA_AttributeOptions* const attribute_options = nullptr,
    const exint subscribeRatio = 16,
    const exint minGrainSize = 1024
)
{
    const GA_Storage finalStorage = storage == GA_STORE_INVALID ? posAttrib_h->getAIFTuple()->getStorage(posAttrib_h.getAttribute()) : storage;
    GA_AttributeUPtr attribUPtr = geo->getAttributes().createDetachedTupleAttribute(GA_ATTRIB_PRIMITIVE, finalStorage, 1, defaults, attribute_options);
    GA_Attribute* const attribPtr = attribUPtr.get();
    primArea<T>(geo, attribPtr, posAttrib_h, geoPrimGroup, subscribeRatio, minGrainSize);
    return attribUPtr;
}





static GA_Attribute*
addAttribPrimArea(
    GA_Detail* const geo,
    const GA_Attribute* const posAttrib,
    const GA_PrimitiveGroup* const geoPrimGroup = nullptr,
    const GA_Storage storage = GA_STORE_INVALID,
    const UT_StringHolder& name = "area",
    const GA_Defaults& defaults = GA_Defaults(-1.0),
    const UT_Options* const creation_args = nullptr,
    const GA_AttributeOptions* const attribute_options = nullptr,
    const GA_ReuseStrategy& reuse = GA_ReuseStrategy(),
    const exint subscribeRatio = 16,
    const exint minGrainSize = 1024
)
{
    const GA_Storage finalStorage = storage == GA_STORE_INVALID ? posAttrib->getAIFTuple()->getStorage(posAttrib) : storage;
    switch (finalStorage)
    {
    case GA_STORE_REAL16:
        return addAttribPrimArea<fpreal16>(geo, posAttrib, geoPrimGroup, storage, name, defaults, creation_args, attribute_options, reuse, subscribeRatio, minGrainSize);
        break;
    case GA_STORE_REAL32:
        return addAttribPrimArea<fpreal32>(geo, posAttrib, geoPrimGroup, storage, name, defaults, creation_args, attribute_options, reuse, subscribeRatio, minGrainSize);
        break;
    case GA_STORE_REAL64:
        return addAttribPrimArea<fpreal64>(geo, posAttrib, geoPrimGroup, storage, name, defaults, creation_args, attribute_options, reuse, subscribeRatio, minGrainSize);
        break;
    default:
        UT_ASSERT_MSG(0, "unhandled storage type");
        break;
    }
    return nullptr;
}


static GA_AttributeUPtr
addDetachedAttribPrimArea(
    const GA_Detail* const geo,
    const GA_Attribute* const posAttrib,
    const GA_PrimitiveGroup* const geoPrimGroup = nullptr,
    const GA_Storage storage = GA_STORE_INVALID,
    const GA_Defaults& defaults = GA_Defaults(-1.0),
    const GA_AttributeOptions* const attribute_options = nullptr,
    const exint subscribeRatio = 16,
    const exint minGrainSize = 1024
)
{
    const GA_Storage finalStorage = storage == GA_STORE_INVALID ? posAttrib->getAIFTuple()->getStorage(posAttrib) : storage;
    switch (finalStorage)
    {
    case GA_STORE_REAL16:
        return addDetachedAttribPrimArea<fpreal16>(geo, posAttrib, geoPrimGroup, storage, defaults, attribute_options, subscribeRatio, minGrainSize);
        break;
    case GA_STORE_REAL32:
        return addDetachedAttribPrimArea<fpreal32>(geo, posAttrib, geoPrimGroup, storage, defaults, attribute_options, subscribeRatio, minGrainSize);
        break;
    case GA_STORE_REAL64:
        return addDetachedAttribPrimArea<fpreal64>(geo, posAttrib, geoPrimGroup, storage, defaults, attribute_options, subscribeRatio, minGrainSize);
        break;
    default:
        UT_ASSERT_MSG(0, "unhandled storage type");
        break;
    }
    return GA_AttributeUPtr();
}






















//GA_FeE_Measure::addAttribPrimArea(geo, posAttribOwner, posAttribName, name, geoPrimGroup, defaults, creation_args, attribute_options, storage, reuse, subscribeRatio, minGrainSize);

static GA_Attribute*
addAttribPrimArea(
    GA_Detail* const geo,
    //const GA_AttributeOwner posAttribOwner = GA_ATTRIB_PRIMITIVE,
    const UT_StringHolder& posAttribName = "P",
    const GA_PrimitiveGroup* const geoPrimGroup = nullptr,
    const GA_Storage storage = GA_STORE_INVALID,
    const UT_StringHolder& name = "area",
    const GA_Defaults& defaults = GA_Defaults(-1.0),
    const UT_Options* const creation_args = nullptr,
    const GA_AttributeOptions* const attribute_options = nullptr,
    const GA_ReuseStrategy& reuse = GA_ReuseStrategy(),
    const exint subscribeRatio = 16,
    const exint minGrainSize = 1024
)
{
    if (posAttribName == "P")
    {
        return addAttribPrimArea(geo, geoPrimGroup, storage, name, defaults, creation_args, attribute_options, reuse, subscribeRatio, minGrainSize);
    }
    else
    {
        const GA_AttributeOwner search_order[] = { GA_ATTRIB_POINT, GA_ATTRIB_VERTEX };
        const GA_Attribute* const attribPtr = geo->findAttribute(posAttribName, search_order, 2);
        //GA_AttributeOwner geo0AttribClassFinal;
        //const GA_Attribute* const attribPtr = GA_FeE_Attribute::findFloatTuplePointVertex(geo, posAttribOwner, posAttribName, geo0AttribClassFinal);
        if (!attribPtr)
            return nullptr;
        return addAttribPrimArea(geo, attribPtr, geoPrimGroup, storage, name, defaults, creation_args, attribute_options, reuse, subscribeRatio, minGrainSize);
    }
}




//
//
//static fpreal32
//primPerimeter32(
//    const GA_Detail* const geo,
//    const GA_Offset primoff
//)
//{
//    const GA_OffsetListRef vertices = geo->getPrimitiveVertexList(primoff);
//    const GA_Size numvtx = vertices.size();
//    if (numvtx < 2)
//        return 0.0;
//
//    const bool closed = vertices.getExtraFlag();
//    fpreal32 pSum = 0.0;
//
//    switch (geo->getPrimitiveTypeId(primoff))
//    {
//    case GA_PRIMPOLY:
//    {
//    }
//    break;
//    default:
//        return 0.0;
//    }
//
//    UT_Vector3T<fpreal32> pos0 = geo->getPos3(geo->vertexPoint(vertices[closed ? numvtx - 1 : 0]));
//    for (GA_Size i(!closed); i < numvtx; ++i)
//    {
//        UT_Vector3T<fpreal32> pos1 = geo->getPos3(geo->vertexPoint(vertices[i]));
//        pSum += (pos1 - pos0).length();
//        pos0 = pos1;
//    }
//
//    return pSum;
//}
//
//static fpreal64
//primPerimeter64(
//    const GA_Detail* const geo,
//    const GA_Offset primoff
//)
//{
//    const GA_OffsetListRef vertices = geo->getPrimitiveVertexList(primoff);
//    const GA_Size numvtx = vertices.size();
//    if (numvtx < 2)
//        return 0.0;
//
//    const bool closed = vertices.getExtraFlag();
//    fpreal64 pSum = 0.0;
//
//    switch (geo->getPrimitiveTypeId(primoff))
//    {
//    case GA_PRIMPOLY:
//    {
//    }
//    break;
//    default:
//        return 0.0;
//    }
//
//    UT_Vector3T<fpreal64> pos0 = geo->getPos3D(geo->vertexPoint(vertices[closed ? numvtx - 1 : 0]));
//    for (GA_Size i(!closed); i < numvtx; ++i)
//    {
//        UT_Vector3T<fpreal64> pos1 = geo->getPos3D(geo->vertexPoint(vertices[i]));
//        pSum += (pos1 - pos0).length();
//        pos0 = pos1;
//    }
//
//    return pSum;
//}


//GU_Detail::compute3DArea(const GA_Offset primoff)

#define primPerimeterMacro(T)                                                                                        \
static fpreal##T                                                                                                     \
primPerimeter##T(                                                                                                    \
    const GA_Detail* const geo,                                                                                      \
    const GA_Offset primoff                                                                                          \
)                                                                                                                    \
{                                                                                                                    \
    const GA_OffsetListRef vertices = geo->getPrimitiveVertexList(primoff);                                          \
    const GA_Size numvtx = vertices.size();                                                                          \
    if (numvtx < 2)                                                                                                  \
        return 0.0;                                                                                                  \
                                                                                                                     \
    const bool closed = vertices.getExtraFlag();                                                                     \
    fpreal##T pSum = 0.0;                                                                                            \
                                                                                                                     \
    switch (geo->getPrimitiveTypeId(primoff))                                                                        \
    {                                                                                                                \
    case GA_PRIMPOLY:                                                                                                \
    {                                                                                                                \
    }                                                                                                                \
    break;                                                                                                           \
    default:                                                                                                         \
        return 0.0;                                                                                                  \
    }                                                                                                                \
                                                                                                                     \
    UT_Vector3T<fpreal##T> pos0 = geo->getPos3T<fpreal##T>(geo->vertexPoint(vertices[closed ? numvtx-1 : 0]));       \
    for (GA_Size i(!closed); i < numvtx; ++i)                                                                        \
    {                                                                                                                \
        UT_Vector3T<fpreal##T> pos1 = geo->getPos3T<fpreal##T>(geo->vertexPoint(vertices[i]));                       \
        pSum += (pos1 - pos0).length();                                                                              \
        pos0 = pos1;                                                                                                 \
    }                                                                                                                \
                                                                                                                     \
    return pSum;                                                                                                     \
}

primPerimeterMacro(32);
primPerimeterMacro(64);



template<typename T>
static T
primPerimeter(
    const GA_Detail* const geo,
    const GA_Offset primoff
)
{
    if constexpr (std::is_same<T, float>::value)
        return primPerimeter32(geo, primoff);
    else
        return primPerimeter64(geo, primoff);
}





//GU_Detail::compute3DArea(const GA_Offset primoff)

template<typename T>
static T
primPerimeter(
    const GA_Detail* const geo,
    const GA_Offset primoff,
    const GA_ROHandleT<UT_Vector3T<T>>& posAttrib_h
)
{
    const GA_OffsetListRef vertices = geo->getPrimitiveVertexList(primoff);
    const GA_Size numvtx = vertices.size();
    if (numvtx < 2)
        return 0.0;

    const bool closed = vertices.getExtraFlag();
    T pSum = 0.0;

    switch (geo->getPrimitiveTypeId(primoff))
    {
    case GA_PRIMPOLY:
    {
    }
    break;
    default:
        return 0.0;
    }


    const GA_AttributeOwner attribOwner = posAttrib_h.getAttribute()->getOwner();

    UT_Vector3T<T> pos0, pos1;
    switch (attribOwner)
    {
    case GA_ATTRIB_VERTEX:
    {
        pos0 = posAttrib_h.get(vertices[closed ? numvtx - 1 : 0]);
        for (GA_Size i(!closed); i < numvtx; ++i)
        {
            pos1 = posAttrib_h.get(vertices[i]);
            pSum += (pos1 - pos0).length();
            pos0 = pos1;
        }
    }
    break;
    case GA_ATTRIB_POINT:
    {
        pos0 = posAttrib_h.get(geo->vertexPoint(vertices[closed ? numvtx - 1 : 0]));
        for (GA_Size i(!closed); i < numvtx; ++i)
        {
            pos1 = posAttrib_h.get(geo->vertexPoint(vertices[i]));
            pSum += (pos1 - pos0).length();
            pos0 = pos1;
        }
    }
    break;
    default:
        return 0.0;
    }

    return pSum;
}



template<typename T>
static void
primPerimeter(
    const GA_Detail* const geo,
    const GA_RWHandleT<T>& p_h,
    const GA_PrimitiveGroup* const geoPrimGroup = nullptr,
    const exint subscribeRatio = 16,
    const exint minGrainSize = 1024
)
{
    //GU_Measure::computePerimeter(*geo, p_h, geoPrimGroup);
    const GA_SplittableRange geoSplittableRange0(geo->getPrimitiveRange(geoPrimGroup));
    UTparallelFor(geoSplittableRange0, [geo, &p_h](const GA_SplittableRange& r)
    {
        GA_Offset start, end;
        for (GA_Iterator it(r); it.blockAdvance(start, end); )
        {
            for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
            {
                //T attribValue = GA_FeE_Measure::primPerimeter(outGeo0, elemoff, attribHandle, geo0AttribClassFinal);
                p_h.set(elemoff, primPerimeter<T>(geo, elemoff));
            }
        }
    }, subscribeRatio, minGrainSize);
}

SYS_FORCE_INLINE
static void
primPerimeter(
    const GA_Detail* const geo,
    GA_Attribute* const pAttrib,
    const GA_PrimitiveGroup* const geoPrimGroup = nullptr,
    const exint subscribeRatio = 16,
    const exint minGrainSize = 1024
)
{
    switch (pAttrib->getAIFTuple()->getStorage(pAttrib))
    {
    case GA_STORE_REAL16:
        primPerimeter<fpreal16>(geo, pAttrib, geoPrimGroup, subscribeRatio, minGrainSize);
        break;
    case GA_STORE_REAL32:
        primPerimeter<fpreal32>(geo, pAttrib, geoPrimGroup, subscribeRatio, minGrainSize);
        break;
    case GA_STORE_REAL64:
        primPerimeter<fpreal64>(geo, pAttrib, geoPrimGroup, subscribeRatio, minGrainSize);
        break;
    default:
        UT_ASSERT_MSG(0, "unhandled storage type");
        break;
    }
}



template<typename T, typename T1>
static void
primPerimeter(
    const GA_Detail* const geo,
    const GA_RWHandleT<T>& p_h,
    const GA_ROHandleT<UT_Vector3T<T1>>& posAttrib_h,
    const GA_PrimitiveGroup* const geoPrimGroup = nullptr,
    const exint subscribeRatio = 16,
    const exint minGrainSize = 1024
)
{
    //GU_Measure::computePerimeter(*geo, p_h, geoPrimGroup);
    const GA_SplittableRange geoSplittableRange0(geo->getPrimitiveRange(geoPrimGroup));
    UTparallelFor(geoSplittableRange0, [geo, &p_h, &posAttrib_h](const GA_SplittableRange& r)
    {
        GA_Offset start, end;
        for (GA_Iterator it(r); it.blockAdvance(start, end); )
        {
            for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
            {
                T attribValue = primPerimeter(geo, elemoff, posAttrib_h);
                p_h.set(elemoff, attribValue);
            }
        }
    }, subscribeRatio, minGrainSize);
}


template<typename T>
SYS_FORCE_INLINE
static void
primPerimeter(
    const GA_Detail* const geo,
    GA_Attribute* const pAttrib,
    const GA_ROHandleT<UT_Vector3T<T>>& posAttrib_h,
    const GA_PrimitiveGroup* const geoPrimGroup = nullptr,
    const exint subscribeRatio = 16,
    const exint minGrainSize = 1024
)
{
    switch (pAttrib->getAIFTuple()->getStorage(pAttrib))
    {
    case GA_STORE_REAL16:
        primPerimeter<fpreal16>(geo, pAttrib, posAttrib_h, geoPrimGroup, subscribeRatio, minGrainSize);
        break;
    case GA_STORE_REAL32:
        primPerimeter<fpreal32>(geo, pAttrib, posAttrib_h, geoPrimGroup, subscribeRatio, minGrainSize);
        break;
    case GA_STORE_REAL64:
        primPerimeter<fpreal64>(geo, pAttrib, posAttrib_h, geoPrimGroup, subscribeRatio, minGrainSize);
        break;
    default:
        UT_ASSERT_MSG(0, "unhandled storage type");
        break;
    }
}




//GA_FeE_Measure::addAttribPrimArea(geo, name, geoPrimGroup, defaults, creation_args, attribute_options, storage, reuse, subscribeRatio, minGrainSize);

static GA_Attribute*
addAttribPrimPerimeter(
    GA_Detail* const geo,
    const GA_PrimitiveGroup* const geoPrimGroup = nullptr,
    const GA_Storage storage = GA_STORE_INVALID,
    const UT_StringHolder& name = "perimeter",
    const GA_Defaults& defaults = GA_Defaults(-1.0),
    const UT_Options* const creation_args = nullptr,
    const GA_AttributeOptions* const attribute_options = nullptr,
    const GA_ReuseStrategy& reuse = GA_ReuseStrategy(),
    const exint subscribeRatio = 16,
    const exint minGrainSize = 1024
)
{
    const GA_Storage finalStorage = storage == GA_STORE_INVALID ? geo->getP()->getAIFTuple()->getStorage(geo->getP()) : storage;
    GA_Attribute* const attribPtr = geo->getAttributes().createTupleAttribute(GA_ATTRIB_PRIMITIVE, GA_FEE_TOPO_SCOPE, name, finalStorage, 1, defaults, creation_args, attribute_options, reuse);
    //GA_Attribute* const attribPtr = geo->addFloatTuple(GA_ATTRIB_PRIMITIVE, name, 1, defaults, creation_args, attribute_options, finalStorage, reuse);
    primPerimeter(geo, attribPtr, geoPrimGroup, subscribeRatio, minGrainSize);
    return attribPtr;
}
















//GA_FeE_Measure::addAttribPrimArea(geo, posAttrib_h, name, geoPrimGroup, defaults, creation_args, attribute_options, storage, reuse, subscribeRatio, minGrainSize);

template<typename T>
static GA_Attribute*
addAttribPrimPerimeter(
    GA_Detail* const geo,
    const GA_ROHandleT<UT_Vector3T<T>>& posAttrib_h,
    const GA_PrimitiveGroup* const geoPrimGroup = nullptr,
    const GA_Storage storage = GA_STORE_INVALID,
    const UT_StringHolder& name = "perimeter",
    const GA_Defaults& defaults = GA_Defaults(-1.0),
    const UT_Options* const creation_args = nullptr,
    const GA_AttributeOptions* const attribute_options = nullptr,
    const GA_ReuseStrategy& reuse = GA_ReuseStrategy(),
    const exint subscribeRatio = 16,
    const exint minGrainSize = 1024
)
{
    const GA_Storage finalStorage = storage == GA_STORE_INVALID ? posAttrib_h->getAIFTuple()->getStorage(posAttrib_h.getAttribute()) : storage;
    GA_Attribute* const attribPtr = geo->getAttributes().createTupleAttribute(GA_ATTRIB_PRIMITIVE, GA_FEE_TOPO_SCOPE, name, finalStorage, 1, defaults, creation_args, attribute_options, reuse);
    //GA_Attribute* const attribPtr = geo->addFloatTuple(GA_ATTRIB_PRIMITIVE, name, 1, defaults, creation_args, attribute_options, finalStorage, reuse);
    primPerimeter(geo, attribPtr, posAttrib_h, geoPrimGroup, subscribeRatio, minGrainSize);
    return attribPtr;
}


static GA_Attribute*
addAttribPrimPerimeter(
    GA_Detail* const geo,
    const GA_Attribute* const posAttrib,
    const GA_PrimitiveGroup* const geoPrimGroup = nullptr,
    const GA_Storage storage = GA_STORE_INVALID,
    const UT_StringHolder& name = "perimeter",
    const GA_Defaults& defaults = GA_Defaults(-1.0),
    const UT_Options* const creation_args = nullptr,
    const GA_AttributeOptions* const attribute_options = nullptr,
    const GA_ReuseStrategy& reuse = GA_ReuseStrategy(),
    const exint subscribeRatio = 16,
    const exint minGrainSize = 1024
)
{
    const GA_Storage finalStorage = storage == GA_STORE_INVALID ? posAttrib->getAIFTuple()->getStorage(posAttrib) : storage;
    GA_Attribute* const attribPtr = geo->getAttributes().createTupleAttribute(GA_ATTRIB_PRIMITIVE, GA_FEE_TOPO_SCOPE, name, finalStorage, 1, defaults, creation_args, attribute_options, reuse);
    //GA_Attribute* const attribPtr = geo->addFloatTuple(GA_ATTRIB_PRIMITIVE, name, 1, defaults, creation_args, attribute_options, finalStorage, reuse);
    switch (finalStorage)
    {
    case GA_STORE_REAL16:
        return addAttribPrimPerimeter<fpreal16>(geo, attribPtr, geoPrimGroup, storage, name, defaults, creation_args, attribute_options, reuse, subscribeRatio, minGrainSize);
        break;
    case GA_STORE_REAL32:
        return addAttribPrimPerimeter<fpreal32>(geo, attribPtr, geoPrimGroup, storage, name, defaults, creation_args, attribute_options, reuse, subscribeRatio, minGrainSize);
        break;
    case GA_STORE_REAL64:
        return addAttribPrimPerimeter<fpreal64>(geo, attribPtr, geoPrimGroup, storage, name, defaults, creation_args, attribute_options, reuse, subscribeRatio, minGrainSize);
        break;
    default:
        UT_ASSERT_MSG(0, "unhandled storage type");
        break;
    }
    return nullptr;
}





//GA_FeE_Measure::addAttribPrimPerimeter(geo, posAttribOwner, posAttribName, name, geoPrimGroup, defaults, creation_args, attribute_options, storage, reuse, subscribeRatio, minGrainSize);

static GA_Attribute*
addAttribPrimPerimeter(
    GA_Detail* const geo,
    //const GA_AttributeOwner posAttribOwner = GA_ATTRIB_PRIMITIVE,
    const UT_StringHolder& posAttribName = "P",
    const GA_PrimitiveGroup* const geoPrimGroup = nullptr,
    const GA_Storage storage = GA_STORE_INVALID,
    const UT_StringHolder& name = "perimeter",
    const GA_Defaults& defaults = GA_Defaults(-1.0),
    const UT_Options* const creation_args = nullptr,
    const GA_AttributeOptions* const attribute_options = nullptr,
    const GA_ReuseStrategy& reuse = GA_ReuseStrategy(),
    const exint subscribeRatio = 16,
    const exint minGrainSize = 1024
)
{
    if (posAttribName == "P")
    {
        return addAttribPrimPerimeter(geo, geoPrimGroup, storage, name, defaults, creation_args, attribute_options, reuse, subscribeRatio, minGrainSize);
    }
    else
    {
        const GA_AttributeOwner search_order[] = { GA_ATTRIB_POINT, GA_ATTRIB_VERTEX };
        const GA_Attribute* const attribPtr = geo->findAttribute(posAttribName, search_order, 2);
        //GA_AttributeOwner geo0AttribClassFinal;
        //const GA_Attribute* const attribPtr = GA_FeE_Attribute::findFloatTuplePointVertex(geo, posAttribOwner, posAttribName, geo0AttribClassFinal);
        if (!attribPtr)
            return nullptr;
        return addAttribPrimPerimeter(geo, attribPtr, geoPrimGroup, storage, name, defaults, creation_args, attribute_options, reuse, subscribeRatio, minGrainSize);
    }
}








} // End of namespace GA_FeE_Measure

#endif
