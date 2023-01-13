
#pragma once

#ifndef __GA_FeE_PolyReduce2D_h__
#define __GA_FeE_PolyReduce2D_h__

//#include "GA_FeE/GA_FeE_PolyReduce2D.h"

//#include "GA/GA_Detail.h"
#include "GEO/GEO_Detail.h"
#include "GA/GA_SplittableRange.h"


#include "GEO/GEO_SplitPoints.h"


//#include "GA_FeE/GA_FeE_Type.h
#include "GA_FeE/GA_FeE_Math.h"
#include "GA_FeE/GA_FeE_Detail.h"
#include "GA_FeE/GA_FeE_Group.h"

#include "GA_FeE/GA_FeE_PrimInlinePoint.h"



namespace GA_FeE_PolyReduce2D {


#define ReverseROC 1












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
            GA_FeE_PrimInlinePoint::delPrimInlinePoint_fast(geo, geoGroup,
                threshold_inlineAngleRadians, reverseGroup);
        }
        else
        {
            GA_PointGroupUPtr inlinePtGroupUPtr = GA_FeE_PrimInlinePoint::groupDetachedPrimInlinePoint_fast(geo, geoGroup,
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

    GA_Attribute* polyReduce2DPtGroup;

    int nebs[] = neighbours(0, @ptnum);

    int nebcount = len(nebs);

    if (nebcount > 2) {
        error('not support neighbour count > 2');
        error('暂不支持大于2数量的邻居');
        error(itoa(@ptnum));
    }

    if (nebcount == 0) {
#if ReverseROC
        f@ndot = chi("../geoPropertyType") ? SYS_FP32_MAX : 2;
#else
        if (chi("../geoPropertyType") == 0) {
            f@ndot = 2;
        }
        else if (chi("../geoPropertyType") == 1) {
            f@ndot = SYS_FP32_MAX;
        }
        else {
            f@ndot = 0;
        }
#endif

        v@ndir = 0;
    }
    else if (nebcount == 1) {
#if ReverseROC
        f@ndot = chi("../geoPropertyType") ? SYS_FP32_MAX : 2;
#else
        if (chi("../geoPropertyType") == 0) {
            f@ndot = 2;
        }
        else if (chi("../geoPropertyType") == 1) {
            f@ndot = SYS_FP32_MAX;
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
            // f@ndot = GA_FeE_Math::distToLine(v@P, nebpos0, nebpos1);
            v@ndir = nebpos0 - v@P;
            f@ndot = GA_FeE_Math::distToLine(v@ndir, nebpos1 - v@P, nebpos0, nebpos1);
        }
        else {
            vector nebpos0 = point(0, 'P', nebs[0]);
            vector nebpos1 = point(0, 'P', nebs[1]);
            f@ndot = GA_FeE_Math::circleRadius3Point(nebpos0, v@P, nebpos1);
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






} // End of namespace GA_FeE_PolyReduce2D

#endif
