
#ifndef __GU_FeE_measure_h__
#define __GU_FeE_measure_h__


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


#pragma once



namespace GU_FeE_measure {


static attribPrecisonF
    //GU_Detail::compute3DArea(const GA_Offset primoff)
    compute3DArea(const GU_Detail* geo, const GA_Offset primoff)
{
    int primtype = geo->getPrimitiveTypeId(primoff);
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
    TAttribTypeV pos2 = geo->getPos3(geo->vertexPoint(vertices(2)));

    //GA_Offset ptoff0 = geo->vertexPoint(geo->getPrimitiveVertexOffset(primoff, 0));

    switch (numvtx)
    {
    case 3:
    {
        attribPrecisonF length0 = pos0.distance(pos1);
        attribPrecisonF length1 = pos1.distance(pos2);
        attribPrecisonF length2 = pos2.distance(pos0);
        attribPrecisonF halfP = (length0 + length1 + length2) * 0.5;
        areaSum = sqrt(halfP * (halfP - length0) * (halfP - length1) * (halfP - length2));
        return areaSum;
    }
    break;
    case 4:
    {
        //Bretschneider's formula

        TAttribTypeV pos3 = geo->getPos3(geo->vertexPoint(vertices(3)));
        attribPrecisonF length0 = pos0.distance(pos1);
        attribPrecisonF length1 = pos1.distance(pos2);
        attribPrecisonF length2 = pos2.distance(pos3);
        attribPrecisonF length3 = pos3.distance(pos0);

        attribPrecisonF length4 = pos0.distance(pos2);
        attribPrecisonF length5 = pos1.distance(pos3);

        length5 *= length4;
        length4 = length0 * length2 + length1 * length3;

        attribPrecisonF halfP = (length0 + length1 + length2 + length3) * 0.5;
        //areaSum = sqrt((halfP - length0) * (halfP - length1) * (halfP - length2) * (halfP - length3)
        //        - length0 * length1 * length2 * length3 * (cos(angle) + 1) * 0.5);
        areaSum = (halfP - length0) * (halfP - length1) * (halfP - length2) * (halfP - length3)
            - (length4 + length5) * (length4 - length5) * 0.25;
        areaSum = sqrt(areaSum);
        return areaSum;
    }
    break;
    //default:
    }



    TAttribTypeV dir0 = pos1 - pos0;
    TAttribTypeV dir1 = pos2 - pos1;
    TAttribTypeV crossdir0B = cross(dir0, dir1);
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



/*

static attribPrecisonF
//GU_Detail::compute3DArea(const GA_Offset primoff)
compute3DArea1(const GU_Detail* geo, GA_RWHandleT<attribPrecisonF>, const GA_Offset primoff)
{
    int primtype = geo->getPrimitiveTypeId(primoff);
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

    geo->get
    TAttribTypeV pos0 = geo->get(geo->vertexPoint(vertices(0)));
    TAttribTypeV pos1 = geo->getPos3(geo->vertexPoint(vertices(1)));
    TAttribTypeV pos2 = geo->getPos3(geo->vertexPoint(vertices(2)));

    //GA_Offset ptoff0 = geo->vertexPoint(geo->getPrimitiveVertexOffset(primoff, 0));

    switch (numvtx)
    {
    case 3:
    {
        attribPrecisonF length0 = pos0.distance(pos1);
        attribPrecisonF length1 = pos1.distance(pos2);
        attribPrecisonF length2 = pos2.distance(pos0);
        attribPrecisonF halfP = (length0 + length1 + length2) * 0.5;
        areaSum = sqrt(halfP * (halfP - length0) * (halfP - length1) * (halfP - length2));
        return areaSum;
    }
    break;
    case 4:
    {
        //Bretschneider's formula

        TAttribTypeV pos3 = geo->getPos3(geo->vertexPoint(vertices(3)));
        attribPrecisonF length0 = pos0.distance(pos1);
        attribPrecisonF length1 = pos1.distance(pos2);
        attribPrecisonF length2 = pos2.distance(pos3);
        attribPrecisonF length3 = pos3.distance(pos0);

        attribPrecisonF length4 = pos0.distance(pos2);
        attribPrecisonF length5 = pos1.distance(pos3);

        length5 *= length4;
        length4 = length0 * length2 + length1 * length3;

        attribPrecisonF halfP = (length0 + length1 + length2 + length3) * 0.5;
        //areaSum = sqrt((halfP - length0) * (halfP - length1) * (halfP - length2) * (halfP - length3)
        //        - length0 * length1 * length2 * length3 * (cos(angle) + 1) * 0.5);
        areaSum = (halfP - length0) * (halfP - length1) * (halfP - length2) * (halfP - length3)
            - (length4 + length5) * (length4 - length5) * 0.25;
        areaSum = sqrt(areaSum);
        return areaSum;
    }
    break;
    //default:
    }



    TAttribTypeV dir0 = pos1 - pos0;
    TAttribTypeV dir1 = pos2 - pos1;
    TAttribTypeV crossdir0B = cross(dir0, dir1);
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


*/

} // End of namespace GU_FeE_measure

#endif
