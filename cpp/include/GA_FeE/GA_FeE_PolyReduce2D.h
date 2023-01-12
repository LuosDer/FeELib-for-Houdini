
#pragma once

#ifndef __GA_FeE_GeoProperty_h__
#define __GA_FeE_GeoProperty_h__

//#include "GA_FeE/GA_FeE_GeoProperty.h"

//#include "GA/GA_Detail.h"
#include "GEO/GEO_Detail.h"
#include "GA/GA_SplittableRange.h"


#include "GEO/GEO_SplitPoints.h"


//#include "GA_FeE/GA_FeE_Type.h
#include "GA_FeE/GA_FeE_Detail.h"
#include "GA_FeE/GA_FeE_Group.h"


namespace GA_FeE_GeoProperty {



//polyReduce2D(geo, groupName, reverseGroup, delGroup);
static void
polyReduce2D(
    GEO_Detail* const geo,
    const GA_PrimitiveGroup* const group,
    const bool reverseGroup = false
)
{
    GEOsplitPoints(geo, group);

}



//polyReduce2D(geo, groupName, reverseGroup, delGroup);
static void
polyReduce2D(
    const SOP_NodeVerb::CookParms& cookparms,
    GEO_Detail* const geo,
    const GA_PrimitiveGroup* const geoGroup,
    const UT_StringHolder& outGroupName,
    
    const bool delInLinePoint = true,
    const fpreal threshold_inlineAngleRadians = 1e-05,
    const bool reverseGroup = false,
    const bool delPoint = false,
    const exint subscribeRatio = 64,
    const exint minGrainSize = 16
)
{
    GEOsplitPoints(geo, geoGroup);

    if (delInLinePoint)
    {
        if (delPoint)
        {
            delPrimInlinePoint_fast(geo, geoGroup,
                threshold_inlineAngleRadians, reverseGroup);
        }
        else
        {
            GA_PointGroupUPtr inlinePtGroupUPtr = groupDetachedPrimInlinePoint_fast(geo, geoGroup,
                threshold_inlineAngleRadians, reverseGroup);
            GA_PointGroup* const inlinePtGroup = inlinePtGroupUPtr.get();
        }
    }

    GA_PointGroupUPtr polyReduce2DPtGroupUPtr;
    GA_PointGroup* polyReduce2DPtGroup;
    if (delPoint)
    {
        polyReduce2DPtGroupUPtr = geo->createDetachedPointGroup();
        polyReduce2DPtGroup = polyReduce2DPtGroupUPtr.get();
    }
    else
    {
        polyReduce2DPtGroup = static_cast<GA_PointGroup*>(geo->pointGroups().newGroup(outGroupName));
    }






#define MAXF32 1e10
#define ReverseROC 1

    function
        float distToLine(
            vector pos, pos0, pos1;
    ) {
        return sqrt(length2(cross(pos0 - pos, pos1 - pos)) / distance2(pos0, pos1));
    }

    function
        float distToLine(
            vector dir0, dir1, pos0, pos1;
    ) {
        return sqrt(length2(cross(dir0, dir1)) / distance2(pos0, pos1));
    }


    function
        float circleRadius3Point(
            vector pos0, pos1, pos2;
    ) {
        float x1 = pos0.x,
            x2 = pos1.x,
            x3 = pos2.x,
            y1 = pos0.y,
            y2 = pos1.y,
            y3 = pos2.y,
            z1 = pos0.z,
            z2 = pos1.z,
            z3 = pos2.z;

        float a1 = (y1 * z2 - y2 * z1 - y1 * z3 + y3 * z1 + y2 * z3 - y3 * z2),
            b1 = -(x1 * z2 - x2 * z1 - x1 * z3 + x3 * z1 + x2 * z3 - x3 * z2),
            c1 = (x1 * y2 - x2 * y1 - x1 * y3 + x3 * y1 + x2 * y3 - x3 * y2),
            d1 = -(x1 * y2 * z3 - x1 * y3 * z2 - x2 * y1 * z3 + x2 * y3 * z1 + x3 * y1 * z2 - x3 * y2 * z1);

        float a2 = 2 * (x2 - x1),
            b2 = 2 * (y2 - y1),
            c2 = 2 * (z2 - z1),
            d2 = x1 * x1 + y1 * y1 + z1 * z1 - x2 * x2 - y2 * y2 - z2 * z2;

        float a3 = 2 * (x3 - x1),
            b3 = 2 * (y3 - y1),
            c3 = 2 * (z3 - z1),
            d3 = x1 * x1 + y1 * y1 + z1 * z1 - x3 * x3 - y3 * y3 - z3 * z3;

        vector circleCenter;
        circleCenter.x = -(b1 * c2 * d3 - b1 * c3 * d2 - b2 * c1 * d3 + b2 * c3 * d1 + b3 * c1 * d2 - b3 * c2 * d1)
            / (a1 * b2 * c3 - a1 * b3 * c2 - a2 * b1 * c3 + a2 * b3 * c1 + a3 * b1 * c2 - a3 * b2 * c1);
        circleCenter.y = (a1 * c2 * d3 - a1 * c3 * d2 - a2 * c1 * d3 + a2 * c3 * d1 + a3 * c1 * d2 - a3 * c2 * d1)
            / (a1 * b2 * c3 - a1 * b3 * c2 - a2 * b1 * c3 + a2 * b3 * c1 + a3 * b1 * c2 - a3 * b2 * c1);
        circleCenter.z = -(a1 * b2 * d3 - a1 * b3 * d2 - a2 * b1 * d3 + a2 * b3 * d1 + a3 * b1 * d2 - a3 * b2 * d1)
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

    // if ( @numvtx <= 2 ) return;

    // int vtxpnum = vertexprimindex(0, @vtxnum);

    // if ( vtxpnum == 0 ) {
    //     return;
    // }

    // int vtxpnum_next = vtxpnum + 1;
    // if ( vtxpnum_next == @numvtx ) {
    //     return;
    // }



    // int neb = primpoint(0, @primnum, vtxpnum - 1);
    // vector nebpos0 = point(0, 'P', neb);


    // neb = primpoint(0, @primnum, vtxpnum_next);
    // vector nebpos1 = point(0, 'P', neb);




    // if ( chi("../limitMinPoints") && primvertexcount(0, @primnum) <= chi("../minPoints") ) return;

    int nebs[] = neighbours(0, @ptnum);

    int nebcount = len(nebs);

    if (nebcount > 2) {
        error('not support neighbour count > 2');
        error('暂不支持大于2数量的邻居');
        error(itoa(@ptnum));
    }

    if (nebcount == 0) {
#if ReverseROC
        f@ndot = chi("../geoPropertyType") ? MAXF32 : 2;
#else
        if (chi("../geoPropertyType") == 0) {
            f@ndot = 2;
        }
        else if (chi("../geoPropertyType") == 1) {
            f@ndot = MAXF32;
        }
        else {
            f@ndot = 0;
        }
#endif

        v@ndir = 0;
    }
    else if (nebcount == 1) {
#if ReverseROC
        f@ndot = chi("../geoPropertyType") ? MAXF32 : 2;
#else
        if (chi("../geoPropertyType") == 0) {
            f@ndot = 2;
        }
        else if (chi("../geoPropertyType") == 1) {
            f@ndot = MAXF32;
        }
        else {
            f@ndot = 0;
        }
#endif

        v@ndir = point(0, 'P', nebs[0]);
        v@ndir = v@ndir - v@P;
        if (chi("../geoPropertyType") == 0) {
            v@ndir = normalize(v@ndir);
        }
    }
    else {
        if (chi("../geoPropertyType") == 0) {
            v@ndir = point(0, 'P', nebs[0]);
            v@ndir = normalize(v@ndir - v@P);

            vector ndir1 = point(0, 'P', nebs[1]);
            ndir1 = normalize(ndir1 - v@P);

            f@ndot = dot(v@ndir, ndir1);
        }
        else if (chi("../geoPropertyType") == 1) {
            vector nebpos0 = point(0, 'P', nebs[0]);
            vector nebpos1 = point(0, 'P', nebs[1]);
            // f@ndot = distToLine(v@P, nebpos0, nebpos1);
            v@ndir = nebpos0 - v@P;
            f@ndot = distToLine(v@ndir, nebpos1 - v@P, nebpos0, nebpos1);
        }
        else {
            vector nebpos0 = point(0, 'P', nebs[0]);
            vector nebpos1 = point(0, 'P', nebs[1]);
            f@ndot = circleRadius3Point(nebpos0, v@P, nebpos1);
            // #if ReverseROC
            //     f@ndot = 1 / f@ndot;
            // #endif
        }
    }









    if (delPoint)
    {
        geo->bumpDataIdsForAddOrRemove(1, 1, 1);
    }
    else
    {
        cookparms.getNode()->setHighlight(true);
        cookparms.select(*polyReduce2DPtGroup);
        polyReduce2DPtGroup->bumpDataId();
    }
}










} // End of namespace GA_FeE_GeoProperty

#endif
