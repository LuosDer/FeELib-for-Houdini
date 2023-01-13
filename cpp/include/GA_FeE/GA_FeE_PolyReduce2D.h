
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
#include "GA_FeE/GA_FeE_Adjacency.h"
#include "GA_FeE/GA_FeE_GroupPromote.h"

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

    const int geoPropertyType = 0,
    const bool delInLinePoint = true,
    const fpreal threshold_inlineAngleRadians = 1e-05,
    const bool reverseGroup = false,
    const bool delPoint = false,
    const exint subscribeRatio = 64,
    const exint minGrainSize = 16
)
{
    const GA_Precision finalPreferredPrecision = geo->getPreferredPrecision();
    const GA_Storage inStorageI = GA_FeE_Type::getPreferredStorageI(finalPreferredPrecision);
    const GA_Storage inStorageF = GA_FeE_Type::getPreferredStorageF(finalPreferredPrecision);


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



    const GA_ATINumericUPtr weightUPtr = geo->getAttributes().createDetachedTupleAttribute(GA_ATTRIB_POINT, inStorageF, 1);
    GA_Attribute* const weightPtr = weightUPtr.get();
    const GA_RWHandleT<fpreal> weight_h(weightPtr);


    const GA_ATINumericUPtr ndirUPtr = geo->getAttributes().createDetachedTupleAttribute(GA_ATTRIB_POINT, inStorageF, 3);
    GA_Attribute* const ndirPtr = ndirUPtr.get();
    const GA_RWHandleT<UT_VectorT<fpreal>> ndir_h(ndirPtr);


    const GA_Attribute* const nebsAttribPtr = GA_FeE_Adjacency::addAttribPointPointEdge(geo);
    const GA_ROHandleT<UT_ValArray<GA_Offset>> nebsAttrib_h(nebsAttribPtr);
    


    const GA_PointGroup* const geoPointGroup = GA_FeE_GroupPromote::groupPromotePointDetached(geo, geoGroup);
    UT_UniquePtr<const GA_PointGroup> geoPointGroupUPtr = UTmakeUnique<const GA_PointGroup>(geoPointGroup);
    {
        const GA_SplittableRange geoSplittableRange0(geo->getPointRange(geoPointGroup));
        UTparallelFor(geoSplittableRange0, [geo, weight_h, ndir_h, nebsAttrib_h, geoPropertyType](const GA_SplittableRange& r)
        {
            fpreal weight;
            UT_VectorT<fpreal> ndir();

            GA_Offset start, end;
            for (GA_Iterator it(r); it.blockAdvance(start, end); )
            {
                for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                {
                    UT_ValArray<GA_Offset> nebsArr;
                    nebsAttrib_h.get(elemoff, nebsArr);

                    UT_ASSERT_MSG(nebsArr.size() <= 2, "nost support neighbour count > 2");

                    if (nebsArr.size() == 0)
                    {
#if ReverseROC
                        weight = geoPropertyType ? SYS_FP32_MAX : 2;
#else
                        switch (geoPropertyType)
                        {
                        case 0: weight = 2;            break;
                        case 1: weight = SYS_FP32_MAX; break;
                        case 2: weight = 0;            break;
                        default: break;
                        }
#endif
                        ndir = 0;
                    }
                    else if (nebsArr.size() == 1) 
                    {
#if ReverseROC
                        weight = geoPropertyType ? SYS_FP32_MAX : 2;
#else
                        if (geoPropertyType == 0) {
                            weight = 2;
                        }
                        else if (geoPropertyType == 1) {
                            weight = SYS_FP32_MAX;
                        }
                        else {
                            weight = 0;
                        }
#endif

                        ndir = point(0, 'P', nebs[0]);
                        ndir = ndir - v@P;
                        if (geoPropertyType == 0) {
                            ndir = normalize(ndir);
                        }
                    }
                    else {
                        if (geoPropertyType == 0) {
                            ndir = point(0, 'P', nebs[0]);
                            ndir = normalize(ndir - v@P);

                            vector ndir1 = point(0, 'P', nebs[1]);
                            ndir1 = normalize(ndir1 - v@P);

                            weight = dot(ndir, ndir1);
                        }
                        else if (geoPropertyType == 1) {
                            vector nebpos0 = point(0, 'P', nebs[0]);
                            vector nebpos1 = point(0, 'P', nebs[1]);
                            // weight = GA_FeE_Math::distToLine(v@P, nebpos0, nebpos1);
                            ndir = nebpos0 - v@P;
                            weight = GA_FeE_Math::distToLine(ndir, nebpos1 - v@P, nebpos0, nebpos1);
                        }
                        else {
                            vector nebpos0 = point(0, 'P', nebs[0]);
                            vector nebpos1 = point(0, 'P', nebs[1]);
                            weight = GA_FeE_Math::circleRadius3Point(nebpos0, v@P, nebpos1);
                            // #if ReverseROC
                            //     weight = 1 / weight;
                            // #endif
                        }
                    }
                    weight_h.set(elemoff, weight);
                    ndir_h.set(elemoff, ndir);
                }
            }
        }, subscribeRatio, minGrainSize);
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
