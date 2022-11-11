
#pragma once

#ifndef __GU_FeE_Measure_h__
#define __GU_FeE_Measure_h__

//#include <GU_FeE/GU_FeE_Measure.h>

#include <GU/GU_Detail.h>
#include <GEO/GEO_PrimPoly.h>
#include <OP/OP_Operator.h>
#include <OP/OP_OperatorTable.h>
#include <PRM/PRM_Include.h>
#include <PRM/PRM_TemplateBuilder.h>
#include <UT/UT_DSOVersion.h>
#include <UT/UT_Interrupt.h>
#include <UT/UT_StringHolder.h>
#include <SYS/SYS_Math.h>
#include <limits.h>



namespace GU_FeE_Measure {



using attribPrecisonF = fpreal32;
using TAttribTypeV = UT_Vector3T<attribPrecisonF>;






static attribPrecisonF
heronsFormula(
    const TAttribTypeV& pos0,
    const TAttribTypeV& pos1,
    const TAttribTypeV& pos2
)
{
    //Heron's formula
    attribPrecisonF length0 = pos0.distance(pos1);
    attribPrecisonF length1 = pos1.distance(pos2);
    attribPrecisonF length2 = pos2.distance(pos0);
    attribPrecisonF halfP = (length0 + length1 + length2) * 0.5;
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
    T length0 = pos0.distance2(pos1);
    T length1 = pos1.distance2(pos2);
    T length2 = pos2.distance2(pos3);
    T length3 = pos3.distance2(pos0);
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
    T area;
    T length0 = pos0.distance(pos1);
    T length1 = pos1.distance(pos2);
    T length2 = pos2.distance(pos3);
    T length3 = pos3.distance(pos0);

    T length4 = pos0.distance(pos2);
    T length5 = pos1.distance(pos3);

    length5 *= length4;
    length4 = length0 * length2 + length1 * length3;

    T halfP = (length0 + length1 + length2 + length3) * 0.5;
    //areaSum = sqrt((halfP - length0) * (halfP - length1) * (halfP - length2) * (halfP - length3)
    //        - length0 * length1 * length2 * length3 * (cos(angle) + 1) * 0.5);
    area = (halfP - length0) * (halfP - length1) * (halfP - length2) * (halfP - length3)
        - (length4 + length5) * (length4 - length5) * 0.25;
    return sqrt(area);
}







static attribPrecisonF
//GU_Detail::compute3DArea(const GA_Offset primoff)
polyArea(
    const GU_Detail* geo,
    const GA_Offset& primoff
)
{
    const int primtype = geo->getPrimitiveTypeId(primoff);
    if (primtype != GA_PRIMPOLY)
        return 0.0;

    const GA_OffsetListRef vertices = geo->getPrimitiveVertexList(primoff);
    const bool closed = vertices.getExtraFlag();

    //if (!geo->getPrimitiveClosedFlag(primoff))
    if (!closed)
        return 0.0;

    const GA_Size numvtx = vertices.size();


    if (numvtx < 3)
        return 0.0;

    attribPrecisonF areaSum = 0.0;


    TAttribTypeV pos0 = geo->getPos3(geo->vertexPoint(vertices(0)));
    TAttribTypeV pos1 = geo->getPos3(geo->vertexPoint(vertices(1)));
    const TAttribTypeV pos2 = geo->getPos3(geo->vertexPoint(vertices(2)));

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
        //Bretschneider's formula

        const TAttribTypeV pos3 = geo->getPos3(geo->vertexPoint(vertices(3)));
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

    pos0 = geo->getPos3(geo->vertexPoint(vertices(numvtx - 1)));
    for (GA_Size i(0); i < numvtx; ++i)
    {
        pos1 = geo->getPos3(geo->vertexPoint(vertices(i)));
        areaSum += cosnz * (pos0[0] * pos1[1] - pos1[0] * pos0[1])
            + cosnx * (pos0[1] * pos1[2] - pos1[1] * pos0[2])
            + cosny * (pos0[2] * pos1[0] - pos1[2] * pos0[0]);
        pos0 = pos1;
    }
    return abs(0.5 * areaSum);
}






static attribPrecisonF
//GU_Detail::compute3DArea(const GA_Offset primoff)
//polyArea(const GU_Detail* geo, const GA_Offset primoff, GA_ROHandleT<TAttribTypeV> posAttribHandle, GA_AttributeOwner attribOwner)
polyArea(
    const GU_Detail* geo,
    const GA_Offset& primoff,
    const GA_ROHandleT<TAttribTypeV>& posAttribHandle)
{
    const int primtype = geo->getPrimitiveTypeId(primoff);
    if (primtype != GA_PRIMPOLY)
        return 0.0;

    const GA_OffsetListRef vertices = geo->getPrimitiveVertexList(primoff);
    const bool closed = vertices.getExtraFlag();

    //if (!geo->getPrimitiveClosedFlag(primoff))
    if (!closed)
        return 0.0;

    const GA_Size numvtx = vertices.size();


    if (numvtx < 3)
        return 0.0;

    const GA_AttributeOwner attribOwner = posAttribHandle.getAttribute()->getOwner();
    attribPrecisonF areaSum = 0.0;

    TAttribTypeV pos0, pos1, pos2;

    switch (attribOwner)
    {
    case GA_ATTRIB_VERTEX:
    {
        pos0 = posAttribHandle.get(vertices(0));
        pos1 = posAttribHandle.get(vertices(1));
        pos2 = posAttribHandle.get(vertices(2));
    }
    break;
    case GA_ATTRIB_POINT:
    {
        pos0 = posAttribHandle.get(geo->vertexPoint(vertices(0)));
        pos1 = posAttribHandle.get(geo->vertexPoint(vertices(1)));
        pos2 = posAttribHandle.get(geo->vertexPoint(vertices(2)));
    }
    break;
    default:
        return 0.0;
    }
    //TAttribTypeV pos1 = geo->getPos3(geo->vertexPoint(vertices(0)));
    //TAttribTypeV pos1 = geo->getPos3(geo->vertexPoint(vertices(1)));
    //TAttribTypeV pos2 = geo->getPos3(geo->vertexPoint(vertices(2)));

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
        //TAttribTypeV pos3 = geo->getPos3(geo->vertexPoint(vertices(3)));
        TAttribTypeV pos3;
        switch (attribOwner)
        {
        case GA_ATTRIB_VERTEX:
        {
            pos3 = posAttribHandle.get(vertices(3));
        }
        break;
        case GA_ATTRIB_POINT:
        {
            pos3 = posAttribHandle.get(geo->vertexPoint(vertices(3)));
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

    //pos0 = geo->getPos3(geo->vertexPoint(vertices(numvtx - 1)));
    switch (attribOwner)
    {
    case GA_ATTRIB_VERTEX:
    {
        pos0 = posAttribHandle.get(vertices(numvtx - 1));
        for (GA_Size vtxpnum(0); vtxpnum < numvtx; ++vtxpnum)
        {
            //pos1 = geo->getPos3(geo->vertexPoint(vertices(vtxpnum)));
            pos1 = posAttribHandle.get(vertices(vtxpnum));
            areaSum += cosnz * (pos0[0] * pos1[1] - pos1[0] * pos0[1])
                     + cosnx * (pos0[1] * pos1[2] - pos1[1] * pos0[2])
                     + cosny * (pos0[2] * pos1[0] - pos1[2] * pos0[0]);
            pos0 = pos1;
        }
    }
    break;
    case GA_ATTRIB_POINT:
    {
        pos0 = posAttribHandle.get(geo->vertexPoint(vertices(numvtx - 1)));
        for (GA_Size vtxpnum(0); vtxpnum < numvtx; ++vtxpnum)
        {
            //pos1 = geo->getPos3(geo->vertexPoint(vertices(vtxpnum)));
            pos1 = posAttribHandle.get(geo->vertexPoint(vertices(vtxpnum)));
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
polyArea(
    const GU_Detail* geo,
    const GA_RWHandleF& areaAttribHandle,
    const GA_ROHandleT<TAttribTypeV>& posAttribHandle,
    const GA_PrimitiveGroup* geoPrimGroup = nullptr,
    const exint& subscribeRatio = 16,
    const exint& minGrainSize = 1024
)
{
    //GU_Measure::computeArea(*geo, areaAttribHandle, geoPrimGroup);

    UT_AutoInterrupt boss("polyArea");
    const GA_SplittableRange geo0SplittableRange0(geo->getPrimitiveRange(geoPrimGroup));
    UTparallelFor(geo0SplittableRange0, [&geo, &areaAttribHandle, &posAttribHandle, &boss](const GA_SplittableRange& r)
    {
        if (boss.wasInterrupted())
            return;
        GA_Offset start;
        GA_Offset end;
        for (GA_Iterator it(r); it.blockAdvance(start, end); )
        {
            for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
            {
                //attribPrecisonF attribValue = GU_FeE_Measure::polyArea(outGeo0, elemoff, attribHandle, geo0AttribClassFinal);
                attribPrecisonF attribValue = polyArea(geo, elemoff, posAttribHandle);
                areaAttribHandle.set(elemoff, attribValue);
            }
        }
    }, subscribeRatio, minGrainSize);

    areaAttribHandle->bumpDataId();
}




static void
polyArea(
    const GU_Detail* geo,
    const GA_RWHandleF& areaAttribHandle,
    const GA_PrimitiveGroup* geoPrimGroup = nullptr,
    const exint& subscribeRatio = 16,
    const exint& minGrainSize = 1024
)
{
    //GU_Measure::computeArea(*geo, areaAttribHandle, geoPrimGroup);

    UT_AutoInterrupt boss("polyArea");
    const GA_SplittableRange geo0SplittableRange0(geo->getPrimitiveRange(geoPrimGroup));
    UTparallelFor(geo0SplittableRange0, [&geo, &areaAttribHandle, &boss](const GA_SplittableRange& r)
    {
        if (boss.wasInterrupted())
            return;
        GA_Offset start;
        GA_Offset end;
        for (GA_Iterator it(r); it.blockAdvance(start, end); )
        {
            for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
            {
                //attribPrecisonF attribValue = GU_FeE_Measure::polyArea(outGeo0, elemoff, attribHandle, geo0AttribClassFinal);
                attribPrecisonF attribValue = polyArea(geo, elemoff);
                areaAttribHandle.set(elemoff, attribValue);
            }
        }
    }, subscribeRatio, minGrainSize);

    areaAttribHandle->bumpDataId();
}




} // End of namespace GU_FeE_Measure

#endif
