
#pragma once

#ifndef __GA_FeE_Measure_h__
#define __GA_FeE_Measure_h__

//#include "GA_FeE/GA_FeE_Measure.h"

#include "GA/GA_Detail.h"
#include "GA_FeE/GA_FeE_Attribute.h"


namespace GA_FeE_Measure {



using attribPrecisonF = fpreal32;
using TAttribTypeV = UT_Vector3T<attribPrecisonF>;







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







static attribPrecisonF
//GU_Detail::compute3DArea(const GA_Offset primoff)
primArea(
    const GA_Detail* const geo,
    const GA_Offset primoff
)
{
    const GA_OffsetListRef vertices = geo->getPrimitiveVertexList(primoff);

    const bool& closed = vertices.getExtraFlag();
    if (!closed)
        return 0.0;

    const GA_Size& numvtx = vertices.size();
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



    TAttribTypeV pos0 = geo->getPos3(geo->vertexPoint(vertices[0]));
    TAttribTypeV pos1 = geo->getPos3(geo->vertexPoint(vertices[1]));
    const TAttribTypeV pos2 = geo->getPos3(geo->vertexPoint(vertices[2]));

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
        const TAttribTypeV pos3 = geo->getPos3(geo->vertexPoint(vertices[3]));
        return bretschneidersFormula0(pos0, pos1, pos2, pos3);
    }
    break;
    //default:
    }



    const TAttribTypeV dir0 = pos1 - pos0;
    const TAttribTypeV dir1 = pos2 - pos1;
    const TAttribTypeV crossdir0B = cross(dir0, dir1);
    attribPrecisonF lengthdir0B = crossdir0B.length();
    lengthdir0B = 1.0 / lengthdir0B;
    const attribPrecisonF cosnx = ((pos1[1] - pos0[1]) * (pos2[2] - pos0[2]) - (pos2[1] - pos0[1]) * (pos1[2] - pos0[2])) * lengthdir0B;
    const attribPrecisonF cosny = ((pos2[0] - pos0[0]) * (pos1[2] - pos0[2]) - (pos1[0] - pos0[0]) * (pos2[2] - pos0[2])) * lengthdir0B;
    const attribPrecisonF cosnz = ((pos1[0] - pos0[0]) * (pos2[1] - pos0[1]) - (pos2[0] - pos0[0]) * (pos1[1] - pos0[1])) * lengthdir0B;

    attribPrecisonF areaSum = 0.0;
    pos0 = geo->getPos3(geo->vertexPoint(vertices[numvtx - 1]));
    for (GA_Size i(0); i < numvtx; ++i)
    {
        pos1 = geo->getPos3(geo->vertexPoint(vertices[i]));
        areaSum += cosnz * (pos0[0] * pos1[1] - pos1[0] * pos0[1])
            + cosnx * (pos0[1] * pos1[2] - pos1[1] * pos0[2])
            + cosny * (pos0[2] * pos1[0] - pos1[2] * pos0[0]);
        pos0 = pos1;
    }
    return abs(0.5 * areaSum);
}






static attribPrecisonF
//GU_Detail::compute3DArea(const GA_Offset primoff)
//primArea(const GU_Detail* geo, const GA_Offset primoff, GA_ROHandleT<TAttribTypeV> posAttribHandle, GA_AttributeOwner attribOwner)
primArea(
    const GA_Detail* const geo,
    const GA_Offset primoff,
    const GA_ROHandleT<TAttribTypeV>& posAttribHandle
)
{
    const GA_OffsetListRef vertices = geo->getPrimitiveVertexList(primoff);

    const bool& closed = vertices.getExtraFlag();
    if (!closed)
        return 0.0;

    const GA_Size& numvtx = vertices.size();
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


    const GA_AttributeOwner attribOwner = posAttribHandle.getAttribute()->getOwner();

    TAttribTypeV pos0, pos1, pos2;

    switch (attribOwner)
    {
    case GA_ATTRIB_VERTEX:
    {
        pos0 = posAttribHandle.get(vertices[0]);
        pos1 = posAttribHandle.get(vertices[1]);
        pos2 = posAttribHandle.get(vertices[2]);
    }
    break;
    case GA_ATTRIB_POINT:
    {
        pos0 = posAttribHandle.get(geo->vertexPoint(vertices[0]));
        pos1 = posAttribHandle.get(geo->vertexPoint(vertices[1]));
        pos2 = posAttribHandle.get(geo->vertexPoint(vertices[2]));
    }
    break;
    default:
        return 0.0;
    }
    //TAttribTypeV pos1 = geo->getPos3(geo->vertexPoint(vertices[0]));
    //TAttribTypeV pos1 = geo->getPos3(geo->vertexPoint(vertices[1]));
    //TAttribTypeV pos2 = geo->getPos3(geo->vertexPoint(vertices[2]));

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
        //TAttribTypeV pos3 = geo->getPos3(geo->vertexPoint(vertices[3]));
        TAttribTypeV pos3;
        switch (attribOwner)
        {
        case GA_ATTRIB_VERTEX:
        {
            pos3 = posAttribHandle.get(vertices[3]);
        }
        break;
        case GA_ATTRIB_POINT:
        {
            pos3 = posAttribHandle.get(geo->vertexPoint(vertices[3]));
        }
        break;
        default:
            return 0.0;
        }
        return bretschneidersFormula0(pos0, pos1, pos2, pos3);
    }
    break;
    //default:
    }



    const TAttribTypeV dir0 = pos1 - pos0;
    const TAttribTypeV dir1 = pos2 - pos1;
    const TAttribTypeV crossdir0B = cross(dir0, dir1);
    attribPrecisonF lengthdir0B = crossdir0B.length();
    lengthdir0B = 1.0 / lengthdir0B;
    const attribPrecisonF cosnx = ((pos1[1] - pos0[1]) * (pos2[2] - pos0[2]) - (pos2[1] - pos0[1]) * (pos1[2] - pos0[2])) * lengthdir0B;
    const attribPrecisonF cosny = ((pos2[0] - pos0[0]) * (pos1[2] - pos0[2]) - (pos1[0] - pos0[0]) * (pos2[2] - pos0[2])) * lengthdir0B;
    const attribPrecisonF cosnz = ((pos1[0] - pos0[0]) * (pos2[1] - pos0[1]) - (pos2[0] - pos0[0]) * (pos1[1] - pos0[1])) * lengthdir0B;

    attribPrecisonF areaSum = 0.0;
    //pos0 = geo->getPos3(geo->vertexPoint(vertices[numvtx - 1]));
    switch (attribOwner)
    {
    case GA_ATTRIB_VERTEX:
    {
        pos0 = posAttribHandle.get(vertices[numvtx - 1]);
        for (GA_Size vtxpnum(0); vtxpnum < numvtx; ++vtxpnum)
        {
            //pos1 = geo->getPos3(geo->vertexPoint(vertices[vtxpnum]));
            pos1 = posAttribHandle.get(vertices[vtxpnum]);
            areaSum += cosnz * (pos0[0] * pos1[1] - pos1[0] * pos0[1])
                     + cosnx * (pos0[1] * pos1[2] - pos1[1] * pos0[2])
                     + cosny * (pos0[2] * pos1[0] - pos1[2] * pos0[0]);
            pos0 = pos1;
        }
    }
    break;
    case GA_ATTRIB_POINT:
    {
        pos0 = posAttribHandle.get(geo->vertexPoint(vertices[numvtx - 1]));
        for (GA_Size vtxpnum(0); vtxpnum < numvtx; ++vtxpnum)
        {
            //pos1 = geo->getPos3(geo->vertexPoint(vertices[vtxpnum]));
            pos1 = posAttribHandle.get(geo->vertexPoint(vertices[vtxpnum]));
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




static void
primArea(
    const GA_Detail* const geo,
    const GA_RWHandleF& areaAttribHandle,
    const GA_ROHandleT<TAttribTypeV>& posAttribHandle,
    const GA_PrimitiveGroup* const geoPrimGroup = nullptr,
    const exint subscribeRatio = 16,
    const exint minGrainSize = 1024
)
{
    //GU_Measure::computeArea(*geo, areaAttribHandle, geoPrimGroup);
    const GA_SplittableRange geo0SplittableRange0(geo->getPrimitiveRange(geoPrimGroup));
    UTparallelFor(geo0SplittableRange0, [&geo, &areaAttribHandle, &posAttribHandle](const GA_SplittableRange& r)
    {
        GA_Offset start, end;
        for (GA_Iterator it(r); it.blockAdvance(start, end); )
        {
            for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
            {
                //attribPrecisonF attribValue = GA_FeE_Measure::primArea(outGeo0, elemoff, attribHandle, geo0AttribClassFinal);
                areaAttribHandle.set(elemoff, primArea(geo, elemoff, posAttribHandle));
            }
        }
    }, subscribeRatio, minGrainSize);
}




static void
primArea(
    const GA_Detail* const geo,
    const GA_RWHandleF& areaAttribHandle,
    const GA_PrimitiveGroup* const geoPrimGroup = nullptr,
    const exint subscribeRatio = 16,
    const exint minGrainSize = 1024
)
{
    //GU_Measure::computeArea(*geo, areaAttribHandle, geoPrimGroup);poly
    const GA_SplittableRange geo0SplittableRange0(geo->getPrimitiveRange(geoPrimGroup));
    UTparallelFor(geo0SplittableRange0, [&geo, &areaAttribHandle](const GA_SplittableRange& r)
    {
        GA_Offset start, end;
        for (GA_Iterator it(r); it.blockAdvance(start, end); )
        {
            for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
            {
                //attribPrecisonF attribValue = GA_FeE_Measure::primArea(outGeo0, elemoff, attribHandle, geo0AttribClassFinal);
                areaAttribHandle.set(elemoff, primArea(geo, elemoff));
            }
        }
    }, subscribeRatio, minGrainSize);
}







//#define DEF_FUNC_AddAttrib(DEF_FUNC_Parm_FuncName, DEF_FUNC_Parm_ParmPack, DEF_FUNC_Parm_AttribName, DEF_FUNC_Parm_Defaults, DEF_FUNC_Parm_Storage, DEF_FUNC_Parm_SubscribeRatio, DEF_FUNC_Parm_MinGrainSize)     \
//        SYS_FORCE_INLINE                                                                                                                                                                                          \
//        static GA_Attribute*                                                                                                                                                                                      \
//        DEF_FUNC_Parm_FuncName(                                                                                                                                                                                   \
//            GEO_Detail* const geo,                                                                                                                                                                                      \
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
//            GEO_Detail* const geo,                                                                                                                                                                                      \
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
//DEF_FUNC_AddAttrib(addAttribPrimArea,          , area, -1.0, GA_STORE_REAL32, 16, 1024)


//using parmPack1 = const GA_ROHandleT<TAttribTypeV>& posAttribHandle;
//DEF_FUNC_AddAttrib(addAttribPrimArea, parmPack1, area, -1.0, GA_STORE_REAL32, 16, 1024)

//DEF_FUNC_AddAttrib(addAttribPrimPerimeter, , perimeter, -1.0, GA_STORE_REAL32, 16, 1024)

//GA_FeE_Measure::addAttribPrimArea(geo, name, geoPrimGroup, defaults, storage, reuse, subscribeRatio, minGrainSize);





//GA_FeE_Measure::addAttribPrimArea(geo, name, geoPrimGroup, defaults, creation_args, attribute_options, storage, reuse, subscribeRatio, minGrainSize);

static GA_Attribute*
addAttribPrimArea(
    GEO_Detail* const geo,
    const GA_PrimitiveGroup* const geoPrimGroup = nullptr,
    const GA_Storage storage = GA_STORE_REAL32,
    const UT_StringHolder& name = "area",
    const GA_Defaults& defaults = GA_Defaults(-1.0),
    const UT_Options* const creation_args = nullptr,
    const GA_AttributeOptions* const attribute_options = nullptr,
    const GA_ReuseStrategy& reuse = GA_ReuseStrategy(),
    const exint subscribeRatio = 16,
    const exint minGrainSize = 1024
)
{
    const GA_Storage finalStorage = storage == GA_STORE_REAL32 ? geo->getP()->getAIFTuple()->getStorage(geo->getP()) : storage;
    GA_Attribute* attribPtr = geo->addFloatTuple(GA_ATTRIB_PRIMITIVE, name, 1, defaults, creation_args, attribute_options, finalStorage, reuse);
    GA_RWHandleT<attribPrecisonF> attribHandle(attribPtr);
    primArea(geo, attribHandle, geoPrimGroup, subscribeRatio, minGrainSize);
    return attribPtr;
}












//GA_FeE_Measure::addAttribPrimArea(geo, posAttribHandle, name, geoPrimGroup, defaults, creation_args, attribute_options, storage, reuse, subscribeRatio, minGrainSize);

static GA_Attribute*
addAttribPrimArea(
    GEO_Detail* const geo,
    const GA_ROHandleT<TAttribTypeV>& posAttribHandle,
    const GA_PrimitiveGroup* const geoPrimGroup = nullptr,
    const GA_Storage storage = GA_STORE_REAL32,
    const UT_StringHolder& name = "area",
    const GA_Defaults& defaults = GA_Defaults(-1.0),
    const UT_Options* const creation_args = nullptr,
    const GA_AttributeOptions* const attribute_options = nullptr,
    const GA_ReuseStrategy& reuse = GA_ReuseStrategy(),
    const exint subscribeRatio = 16,
    const exint minGrainSize = 1024
)
{
    const GA_Storage finalStorage = storage == GA_STORE_REAL32 ? posAttribHandle->getAIFTuple()->getStorage(posAttribHandle.getAttribute()) : storage;
    GA_Attribute* attribPtr = geo->addFloatTuple(GA_ATTRIB_PRIMITIVE, name, 1, defaults, creation_args, attribute_options, finalStorage, reuse);
    GA_RWHandleT<attribPrecisonF> attribHandle(attribPtr);
    primArea(geo, attribHandle, posAttribHandle, geoPrimGroup, subscribeRatio, minGrainSize);
    return attribPtr;
}






















//GA_FeE_Measure::addAttribPrimArea(geo, posAttribOwner, posAttribName, name, geoPrimGroup, defaults, creation_args, attribute_options, storage, reuse, subscribeRatio, minGrainSize);

static GA_Attribute*
addAttribPrimArea(
    GEO_Detail* const geo,
    const GA_AttributeOwner posAttribOwner = GA_ATTRIB_PRIMITIVE,
    const UT_StringHolder& posAttribName = "P",
    const GA_PrimitiveGroup* const geoPrimGroup = nullptr,
    const GA_Storage storage = GA_STORE_REAL32,
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
        GA_AttributeOwner geo0AttribClassFinal;
        const GA_Attribute* attribPtr = GA_FeE_Attribute::findFloatTuplePointVertex(geo, posAttribOwner, posAttribName, geo0AttribClassFinal);
        if (!attribPtr)
            return nullptr;
        GA_ROHandleT<TAttribTypeV> posAttribHandle(attribPtr);
        return addAttribPrimArea(geo, posAttribHandle, geoPrimGroup, storage, name, defaults, creation_args, attribute_options, reuse, subscribeRatio, minGrainSize);
    }
}









static attribPrecisonF
//GU_Detail::compute3DArea(const GA_Offset primoff)
primPerimeter(
    const GA_Detail* const geo,
    const GA_Offset primoff
)
{
    const GA_OffsetListRef vertices = geo->getPrimitiveVertexList(primoff);
    const GA_Size& numvtx = vertices.size();
    if (numvtx < 2)
        return 0.0;

    const bool& closed = vertices.getExtraFlag();
    attribPrecisonF pSum = 0.0;

    switch (geo->getPrimitiveTypeId(primoff))
    {
    case GA_PRIMPOLY:
    {
    }
    break;
    default:
        return 0.0;
    }

    TAttribTypeV pos0 = geo->getPos3(geo->vertexPoint(vertices[closed ? numvtx-1 : 0]));
    for (GA_Size i(!closed); i < numvtx; ++i)
    {
        TAttribTypeV pos1 = geo->getPos3(geo->vertexPoint(vertices[i]));
        pSum += (pos1 - pos0).length();
        pos0 = pos1;
    }

    return pSum;
}


static attribPrecisonF
//GU_Detail::compute3DArea(const GA_Offset primoff)
primPerimeter(
    const GA_Detail* const geo,
    const GA_Offset primoff,
    const GA_ROHandleT<TAttribTypeV>& posAttribHandle
)
{
    const GA_OffsetListRef vertices = geo->getPrimitiveVertexList(primoff);
    const GA_Size& numvtx = vertices.size();
    if (numvtx < 2)
        return 0.0;

    const bool& closed = vertices.getExtraFlag();
    attribPrecisonF pSum = 0.0;

    switch (geo->getPrimitiveTypeId(primoff))
    {
    case GA_PRIMPOLY:
    {
    }
    break;
    default:
        return 0.0;
    }


    const GA_AttributeOwner attribOwner = posAttribHandle.getAttribute()->getOwner();

    TAttribTypeV pos0, pos1;
    switch (attribOwner)
    {
    case GA_ATTRIB_VERTEX:
    {
        pos0 = posAttribHandle.get(vertices[closed ? numvtx - 1 : 0]);
        for (GA_Size i(!closed); i < numvtx; ++i)
        {
            pos1 = posAttribHandle.get(vertices[i]);
            pSum += (pos1 - pos0).length();
            pos0 = pos1;
        }
    }
    break;
    case GA_ATTRIB_POINT:
    {
        pos0 = posAttribHandle.get(geo->vertexPoint(vertices[closed ? numvtx - 1 : 0]));
        for (GA_Size i(!closed); i < numvtx; ++i)
        {
            pos1 = posAttribHandle.get(geo->vertexPoint(vertices[i]));
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




static void
primPerimeter(
    const GA_Detail* const geo,
    const GA_RWHandleF& pAttribHandle,
    const GA_PrimitiveGroup* const geoPrimGroup = nullptr,
    const exint subscribeRatio = 16,
    const exint minGrainSize = 1024
)
{
    //GU_Measure::computePerimeter(*geo, pAttribHandle, geoPrimGroup);
    const GA_SplittableRange geo0SplittableRange0(geo->getPrimitiveRange(geoPrimGroup));
    UTparallelFor(geo0SplittableRange0, [&geo, &pAttribHandle](const GA_SplittableRange& r)
    {
        GA_Offset start, end;
        for (GA_Iterator it(r); it.blockAdvance(start, end); )
        {
            for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
            {
                //attribPrecisonF attribValue = GA_FeE_Measure::primPerimeter(outGeo0, elemoff, attribHandle, geo0AttribClassFinal);
                pAttribHandle.set(elemoff, primPerimeter(geo, elemoff));
            }
        }
    }, subscribeRatio, minGrainSize);
}

static void
primPerimeter(
    const GA_Detail* const geo,
    const GA_RWHandleF& pAttribHandle,
    const GA_ROHandleT<TAttribTypeV>& posAttribHandle,
    const GA_PrimitiveGroup* const geoPrimGroup = nullptr,
    const exint subscribeRatio = 16,
    const exint minGrainSize = 1024
)
{
    //GU_Measure::computePerimeter(*geo, pAttribHandle, geoPrimGroup);
    const GA_SplittableRange geo0SplittableRange0(geo->getPrimitiveRange(geoPrimGroup));
    UTparallelFor(geo0SplittableRange0, [&geo, &pAttribHandle, &posAttribHandle](const GA_SplittableRange& r)
    {
        GA_Offset start, end;
        for (GA_Iterator it(r); it.blockAdvance(start, end); )
        {
            for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
            {
                //attribPrecisonF attribValue = GA_FeE_Measure::primPerimeter(outGeo0, elemoff, attribHandle, geo0AttribClassFinal);
                pAttribHandle.set(elemoff, primPerimeter(geo, elemoff, posAttribHandle));
            }
        }
    }, subscribeRatio, minGrainSize);
}







//GA_FeE_Measure::addAttribPrimArea(geo, name, geoPrimGroup, defaults, creation_args, attribute_options, storage, reuse, subscribeRatio, minGrainSize);

static GA_Attribute*
addAttribPrimPerimeter(
    GEO_Detail* const geo,
    const GA_PrimitiveGroup* const geoPrimGroup = nullptr,
    const GA_Storage storage = GA_STORE_REAL32,
    const UT_StringHolder& name = "perimeter",
    const GA_Defaults& defaults = GA_Defaults(-1.0),
    const UT_Options* const creation_args = nullptr,
    const GA_AttributeOptions* const attribute_options = nullptr,
    const GA_ReuseStrategy& reuse = GA_ReuseStrategy(),
    const exint subscribeRatio = 16,
    const exint minGrainSize = 1024
)
{
    const GA_Storage finalStorage = storage == GA_STORE_REAL32 ? geo->getP()->getAIFTuple()->getStorage(geo->getP()) : storage;
    GA_Attribute* attribPtr = geo->addFloatTuple(GA_ATTRIB_PRIMITIVE, name, 1, defaults, creation_args, attribute_options, finalStorage, reuse);
    GA_RWHandleT<attribPrecisonF> attribHandle(attribPtr);
    primPerimeter(geo, attribPtr, geoPrimGroup, subscribeRatio, minGrainSize);
    return attribPtr;
}
















//GA_FeE_Measure::addAttribPrimArea(geo, posAttribHandle, name, geoPrimGroup, defaults, creation_args, attribute_options, storage, reuse, subscribeRatio, minGrainSize);

static GA_Attribute*
addAttribPrimPerimeter(
    GEO_Detail* const geo,
    const GA_ROHandleT<TAttribTypeV>& posAttribHandle,
    const GA_PrimitiveGroup* const geoPrimGroup = nullptr,
    const GA_Storage storage = GA_STORE_REAL32,
    const UT_StringHolder& name = "perimeter",
    const GA_Defaults& defaults = GA_Defaults(-1.0),
    const UT_Options* const creation_args = nullptr,
    const GA_AttributeOptions* const attribute_options = nullptr,
    const GA_ReuseStrategy& reuse = GA_ReuseStrategy(),
    const exint subscribeRatio = 16,
    const exint minGrainSize = 1024
)
{
    const GA_Storage finalStorage = storage == GA_STORE_REAL32 ? posAttribHandle->getAIFTuple()->getStorage(posAttribHandle.getAttribute()) : storage;
    GA_Attribute* attribPtr = geo->addFloatTuple(GA_ATTRIB_PRIMITIVE, name, 1, defaults, creation_args, attribute_options, finalStorage, reuse);
    GA_RWHandleT<attribPrecisonF> attribHandle(attribPtr);
    primPerimeter(geo, attribPtr, posAttribHandle, geoPrimGroup, subscribeRatio, minGrainSize);
    return attribPtr;
}






//GA_FeE_Measure::addAttribPrimPerimeter(geo, posAttribOwner, posAttribName, name, geoPrimGroup, defaults, creation_args, attribute_options, storage, reuse, subscribeRatio, minGrainSize);

static GA_Attribute*
addAttribPrimPerimeter(
    GEO_Detail* const geo,
    const GA_AttributeOwner posAttribOwner = GA_ATTRIB_PRIMITIVE,
    const UT_StringHolder& posAttribName = "P",
    const GA_PrimitiveGroup* const geoPrimGroup = nullptr,
    const GA_Storage storage = GA_STORE_REAL32,
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
        GA_AttributeOwner geo0AttribClassFinal;
        const GA_Attribute* attribPtr = GA_FeE_Attribute::findFloatTuplePointVertex(geo, posAttribOwner, posAttribName, geo0AttribClassFinal);
        if (!attribPtr)
            return nullptr;
        GA_ROHandleT<TAttribTypeV> posAttribHandle(attribPtr);
        return addAttribPrimPerimeter(geo, posAttribHandle, geoPrimGroup, storage, name, defaults, creation_args, attribute_options, reuse, subscribeRatio, minGrainSize);
    }
}








} // End of namespace GA_FeE_Measure

#endif
