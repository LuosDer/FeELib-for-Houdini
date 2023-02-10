
#pragma once

#ifndef __GA_FeE_PolyReduce2D_h__
#define __GA_FeE_PolyReduce2D_h__

//#include "GA_FeE/GA_FeE_PolyReduce2D.h"

//#include "GA/GA_Detail.h"
#include "GA/GA_Detail.h"
#include "GA/GA_SplittableRange.h"


#include "GEO/GEO_SplitPoints.h"


#include "GA_FeE/GA_FeE_Type.h"
#include "GA_FeE/GA_FeE_Array.h"
#include "GA_FeE/GA_FeE_Math.h"
#include "GA_FeE/GA_FeE_Detail.h"
#include "GA_FeE/GA_FeE_Group.h"
#include "GA_FeE/GA_FeE_Adjacency.h"
#include "GA_FeE/GA_FeE_GroupPromote.h"

#include "GA_FeE/GA_FeE_PrimInlinePoint.h"



enum PolyReduce2D_GeoPropertyType
{
    PolyReduce2D_ANGLE,
    PolyReduce2D_DIST,
    PolyReduce2D_ROC,
};


namespace GA_FeE_PolyReduce2D {


#define ReverseROC 1

#define GA_FeE_PolyReduce2D_UseStd 1

#define GA_FeE_PolyReduce2D_UseDetachedAttrib_ForRelease 1





using fpreal_polyReduce2D = fpreal;
using UT_Vector3F_polyReduce2D = UT_Vector3T<fpreal_polyReduce2D>;




//polyReduce2D(geo, groupName, reverseGroup, delGroup);
static void
polyReduce2D(
    GA_Detail* const geo,
    const GA_PrimitiveGroup* const geoGroup,
    GA_PointGroup* const polyReduce2DPtGroup,

    const bool delInLinePoint = true,
    const fpreal threshold_inlineAngleRadians = 1e-05,

    const bool limitByGeoProperty = true,
    const PolyReduce2D_GeoPropertyType geoPropertyType = PolyReduce2D_ANGLE,
    fpreal threshold_maxAngle = GA_FeE_Type::radians(150),
    const fpreal threshold_maxDist = 1e-04,

    const bool limitMinPoint = false,
    const exint minPoint = 10,

    const bool coverSourcePoly = false,
    const bool reverseGroup = false, 
    const exint subscribeRatio = 64,
    const exint minGrainSize = 16
)
{
    threshold_maxAngle = threshold_maxAngle > 0 ? cos(threshold_maxAngle) : 0;

    const GA_Precision finalPreferredPrecision = geo->getPreferredPrecision();
    const GA_Storage inStorageI = GA_FeE_Type::getPreferredStorageI(finalPreferredPrecision);
    const GA_Storage inStorageF = GA_FeE_Type::getPreferredStorageF(finalPreferredPrecision);

#if GA_FeE_PolyReduce2D_UseDetachedAttrib_ForRelease
    const GA_AttributeUPtr weightUPtr = geo->getAttributes().createDetachedTupleAttribute(GA_ATTRIB_POINT, inStorageF, 1);
    const GA_AttributeUPtr ndirUPtr   = geo->getAttributes().createDetachedTupleAttribute(GA_ATTRIB_POINT, inStorageF, 3);
    GA_Attribute* const weightPtr = weightUPtr.get();
    GA_Attribute* const ndirPtr   = ndirUPtr.get();
#else
    GA_Attribute* const weightPtr = geo->getAttributes().createTupleAttribute(GA_ATTRIB_POINT, "weight", inStorageF, 1, GA_Defaults(0));
    GA_Attribute* const ndirPtr   = geo->getAttributes().createTupleAttribute(GA_ATTRIB_POINT, "ndir", inStorageF, 3, GA_Defaults(0));
#endif

    const GA_RWHandleT<fpreal_polyReduce2D> weight_h(weightPtr);
    const GA_RWHandleT<UT_Vector3F_polyReduce2D> ndir_h(ndirPtr);


    const GA_Attribute* const nebsAttribPtr = GA_FeE_Adjacency::addAttribPointPointEdge(geo);
    const GA_ROHandleT<UT_ValArray<GA_Offset>> nebsAttrib_h(nebsAttribPtr);


    const GA_ROHandleT<UT_Vector3F_polyReduce2D> pos_h(geo->getP());
    

    {
        const GA_PointGroupUPtr geoPointGroupUPtr = GA_FeE_GroupPromote::groupPromotePointDetached(geo, geoGroup);
        const GA_PointGroup* const geoPointGroup = geoPointGroupUPtr.get();
        const GA_SplittableRange geoSplittableRange0(geo->getPointRange(geoPointGroup));
        UTparallelFor(geoSplittableRange0, [geo, &pos_h, &ndir_h, &weight_h, &nebsAttrib_h,
            geoPropertyType](const GA_SplittableRange& r)
        {
            fpreal_polyReduce2D weight;
            UT_Vector3F_polyReduce2D pos, ndir, ndir1, posneb0, posneb1;
            UT_ValArray<GA_Offset> nebsArr;

            GA_Offset start, end;
            for (GA_Iterator it(r); it.blockAdvance(start, end); )
            {
                for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                {
                    pos = pos_h.get(elemoff);
                    nebsAttrib_h.get(elemoff, nebsArr);

                    UT_ASSERT_MSG(nebsArr.size() <= 2, "not support neighbour count > 2");

                    switch (nebsArr.size())
                    {
                    case 0:
                        ndir = 0;
#if ReverseROC
                        weight = geoPropertyType ? SYS_FP32_MAX : 2;
#else
                        switch (geoPropertyType)
                        {
                        case PolyReduce2D_ANGLE: weight = 2;            break;
                        case PolyReduce2D_DIST:  weight = SYS_FP32_MAX; break;
                        case PolyReduce2D_ROC:   weight = 0;            break;
                        default:                 weight = 2;            break;
                        }
#endif
                        break;
                    case 1:
                        ndir = pos_h.get(nebsArr[0]) - pos;
                        if (geoPropertyType == PolyReduce2D_ANGLE) {
                            ndir.normalize();
                        }
#if ReverseROC
                        weight = geoPropertyType ? SYS_FP32_MAX : 2;
#else
                        switch (geoPropertyType)
                        {
                        case PolyReduce2D_ANGLE: weight = 2;            break;
                        case PolyReduce2D_DIST:  weight = SYS_FP32_MAX; break;
                        case PolyReduce2D_ROC:   weight = 0;            break;
                        default:                 weight = 2;            break;
                        }
#endif
                        break;
                    case 2:
                        posneb0 = pos_h.get(nebsArr[0]);
                        posneb1 = pos_h.get(nebsArr[1]);
                        switch (geoPropertyType)
                        {
                        case PolyReduce2D_ANGLE:
                            ndir = posneb1 - pos;
                            ndir.normalize();

                            ndir1 = posneb1 - pos;
                            ndir1.normalize();

                            weight = dot(ndir, ndir1);
                            break;
                        case PolyReduce2D_DIST:
                            // weight = GA_FeE_Math::distToLine(pos, posneb0, posneb1);
                            ndir = posneb0 - pos;
                            weight = GA_FeE_Math::distToLine(ndir, posneb1 - pos, posneb0, posneb1);
                            break;
                        case PolyReduce2D_ROC:
                            weight = GA_FeE_Math::circleRadius3Point(posneb0, pos, posneb1);
                            ndir = 0;
                            // #if ReverseROC
                            //     weight = 1 / weight;
                            // #endif
                            break;
                        default:
                            weight = 0;
                            ndir = 0;
                            UT_ASSERT_MSG(0, "not support geoPropertyType");
                            break;
                        }
                        break;
                    default:
                        weight = 0;
                        ndir = 0;
                        UT_ASSERT_MSG(0, "not support neighbour count > 2");
                        break;
                    }
                    weight_h.set(elemoff, weight);
                    ndir_h.set(elemoff, ndir);
                }
            }
        }, subscribeRatio, minGrainSize);
    }








    const GA_Topology& topo = geo->getTopology();
    const GA_ATITopology* const vtxPointRef = topo.getPointRef();

    const GA_SplittableRange geoSplittableRange1(geo->getPrimitiveRange(geoGroup));
    UTparallelFor(geoSplittableRange1, [geo, vtxPointRef, polyReduce2DPtGroup,
        &pos_h, &ndir_h, &weight_h,
        limitByGeoProperty, geoPropertyType, threshold_maxAngle, threshold_maxDist,
        limitMinPoint, minPoint, coverSourcePoly](const GA_SplittableRange& r)
    {
        GA_OffsetListRef vertices;
        //fpreal_polyReduce2D weight;
        //UT_Vector3F_polyReduce2D pos, ndir, ndir1, posneb0, posneb1;
        //UT_ValArray<GA_Offset> nebsArr;
        UT_Vector3F_polyReduce2D ndir;

#if GA_FeE_PolyReduce2D_UseStd
        std::vector<GA_Size> idx_prev, idx_next, scaleIdx_prev, scaleIdx_next;
        std::vector<int> argsort;
        std::vector<fpreal_polyReduce2D> weights;
#else
        UT_ValArray<GA_Size> idx_prev, idx_next, scaleIdx_prev, scaleIdx_next;
        UT_ValArray<int> argsort;
        UT_ValArray<fpreal_polyReduce2D> weights;
#endif
        UT_ValArray<UT_Vector3F_polyReduce2D> poses, dirs;


        GA_Offset start, end;
        for (GA_Iterator it(r); it.blockAdvance(start, end); )
        {
            for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
            {
                idx_prev.clear();
                idx_next.clear();
                scaleIdx_prev.clear();
                scaleIdx_next.clear();
                poses.clear();
                dirs.clear();
                weights.clear();
                argsort.clear();

                vertices = geo->getPrimitiveVertexList(elemoff);
                const GA_Size numvtx = vertices.size();
                GA_Size lastIndex = numvtx - 1;

                bool isClosed;
                {
                    GA_Offset primpoint0 = vtxPointRef->getLink(vertices[0]);
                    GA_Offset primpoint1 = vtxPointRef->getLink(vertices[lastIndex]);
                    // isClosed = geo->getPrimitiveClosedFlag(elemoff) || primpoint0 == primpoint1;
                    if (primpoint0 == primpoint1) {
                        --lastIndex;
                        isClosed = 1;
                    } else if (geo->getPrimitiveClosedFlag(elemoff)) {
                        isClosed = 1;
                    } else {
                        isClosed = 0;
                    }
                }

                GA_Size primpointscount = lastIndex + 1;
                GA_Size lastIndex_prev = lastIndex - 1;



                //////////////////// initial ////////////////////////

#if GA_FeE_PolyReduce2D_UseStd
                //if (idx_prev.capacity() < primpointscount)
                //{
                //    idx_prev.setCapacity(primpointscount);
                //    idx_next.setCapacity(primpointscount);
                //    scaleIdx_prev.setCapacity(primpointscount);
                //    scaleIdx_next.setCapacity(primpointscount);
                //    poses.setCapacity(primpointscount);
                //    dirs.setCapacity(primpointscount);
                //    weights.setCapacity(primpointscount);
                //    argsort.setCapacity(primpointscount);
                //}
                if (idx_prev.size() < primpointscount)
                {
                    idx_prev.resize(primpointscount);
                    idx_next.resize(primpointscount);
                    scaleIdx_prev.resize(primpointscount);
                    scaleIdx_next.resize(primpointscount);
                    poses.setSize(primpointscount);
                    dirs.setSize(primpointscount);
                    weights.resize(primpointscount);
                    argsort.resize(primpointscount);
                }
#else
                //if (idx_prev.capacity() < primpointscount)
                //{
                //    idx_prev.setCapacity(primpointscount);
                //    idx_next.setCapacity(primpointscount);
                //    scaleIdx_prev.setCapacity(primpointscount);
                //    scaleIdx_next.setCapacity(primpointscount);
                //    poses.setCapacity(primpointscount);
                //    dirs.setCapacity(primpointscount);
                //    weights.setCapacity(primpointscount);
                //    argsort.setCapacity(primpointscount);
                //}
                if (idx_prev.size() < primpointscount)
                {
                    idx_prev.setSize(primpointscount);
                    idx_next.setSize(primpointscount);
                    scaleIdx_prev.setSize(primpointscount);
                    scaleIdx_next.setSize(primpointscount);
                    poses.setSize(primpointscount);
                    dirs.setSize(primpointscount);
                    weights.setSize(primpointscount);
                    argsort.setSize(primpointscount);
                }
#endif


                for (GA_Size vtxpnum = 0; vtxpnum < primpointscount; ++vtxpnum) {
                    GA_Offset primpoint = vtxPointRef->getLink(vertices[vtxpnum]);
                    
                    poses[vtxpnum] = pos_h.get(primpoint);
                    dirs[vtxpnum] = ndir_h.get(primpoint);
                    weights[vtxpnum] = weight_h.get(primpoint);
                }

                if (geo->getPrimitiveClosedFlag(elemoff)) {
                    dirs[lastIndex] = poses[lastIndex_prev] - poses[lastIndex];
                    if (geoPropertyType == PolyReduce2D_ANGLE) {
                        dirs[lastIndex].normalize();
                    }
                }
                else if (isClosed) {
                    dirs[0] = poses[lastIndex] - poses[0];
                    if (geoPropertyType == PolyReduce2D_ANGLE) {
                        dirs[0].normalize();
                    }
                }
                else {
                    dirs[0] *= -1;
                }


                const GA_Size NONFoundINT = primpointscount + 1;// the name can be replace by lastIndex (not value)


                // cout(isClosed);
                if (isClosed)
                {
                    idx_prev[0] = lastIndex;
                    idx_next[lastIndex] = 0;
                }
                else
                {
                    idx_prev[0] = NONFoundINT;
                    idx_next[lastIndex] = NONFoundINT;
                }
                idx_next[0] = 1;
                idx_prev[lastIndex] = lastIndex_prev;



                GA_Size minidx;
                {
#if GA_FeE_PolyReduce2D_UseStd
                    argsort = GA_FeE_Array::argsort(weights);
#else
                    argsort = GA_FeE_Array::argsort_UT_Array(weights);
#endif

                    minidx = argsort[lastIndex];
                    scaleIdx_prev[minidx] = argsort[lastIndex_prev];
                    scaleIdx_next[minidx] = NONFoundINT;

                    minidx = argsort[0];
                    scaleIdx_prev[minidx] = NONFoundINT;
                    scaleIdx_next[minidx] = argsort[1];

                    for (GA_Size i = 1, i0 = 0, i1 = 2; i < lastIndex; ++i, ++i0, ++i1) {
                        idx_prev[i] = i0;
                        idx_next[i] = i1;

                        scaleIdx_prev[argsort[i]] = argsort[i0];
                        scaleIdx_next[argsort[i]] = argsort[i1];
                    }
                }


                //if (chi("../blastGroup") && primpointscount < 2) {
                //    removeprim(0, @primnum, 1);
                //    continue;
                //}


                if (primpointscount < 2) {
                    continue;
                }



                // GA_Size loopCount = 0;
                // for (GA_Size i = 0; i < @Frame-1 && primpointscount > 0; ++i) {
                while (primpointscount > 2) {
                    // ++loopCount;

                    if (limitByGeoProperty)
                    {
                        if (geoPropertyType == PolyReduce2D_ANGLE) {
                            if (weights[minidx] >= threshold_maxAngle) {
                                fpreal weight = weights[minidx];
                                break;
                            }
                        }
                        else if (geoPropertyType == PolyReduce2D_DIST) {
                            if (weights[minidx] >= threshold_maxDist) {
                                fpreal weight = weights[minidx];
                                break;
                            }
                        }
                        else {
#if ReverseROC
                            if (weights[minidx] >= threshold_maxDist) {
                                fpreal weight = weights[minidx];
                                break;
                            }
#else
                            if (weights[minidx] <= threshold_maxDist) {
                                fpreal weight = weights[minidx];
                                break;
                            }
#endif
                        }
                    }

                    if (limitMinPoint && primpointscount <= minPoint) {
                        break;
                    }

                    GA_Size previdx = idx_prev[minidx];
                    GA_Size nextidx = idx_next[minidx];
                    if (nextidx != NONFoundINT) {
                        // if ( i == @Frame-1 ) {
                        //     printf("\n %d %d", previdx, nextidx);
                        // }

                        switch (geoPropertyType)
                        {
                        case PolyReduce2D_ANGLE:
                            dirs[nextidx] = poses[previdx] - poses[nextidx];
                            dirs[nextidx].normalize();
                            break;
                        case PolyReduce2D_DIST:
                            dirs[nextidx] = poses[previdx] - poses[nextidx];
                            break;
                        case PolyReduce2D_ROC:
                            break;
                        default:
                            dirs[nextidx] = poses[previdx] - poses[nextidx];
                            dirs[nextidx].normalize();
                            break;
                        }

                        GA_Size next_nextidx = idx_next[nextidx];
                        if (next_nextidx != NONFoundINT) {

                            fpreal_polyReduce2D scale_min;
                            switch (geoPropertyType)
                            {
                            case PolyReduce2D_ANGLE: scale_min = -dot(dirs[nextidx], dirs[next_nextidx]);                                                         break;
                            case PolyReduce2D_DIST:  scale_min = GA_FeE_Math::distToLine(dirs[nextidx], dirs[next_nextidx], poses[previdx], poses[next_nextidx]); break;
                            case PolyReduce2D_ROC:   scale_min = GA_FeE_Math::circleRadius3Point(poses[previdx], poses[nextidx], poses[next_nextidx]);            break;
                            default:                 scale_min = -dot(dirs[nextidx], dirs[next_nextidx]);                                                         break;
                            }

                            weights[nextidx] = scale_min;

                            GA_Size prev = scaleIdx_prev[nextidx];
                            GA_Size next = scaleIdx_next[nextidx];
                            if (next != NONFoundINT) scaleIdx_prev[next] = prev;
                            if (prev != NONFoundINT) scaleIdx_next[prev] = next;

                            // GA_Size j = 0;
                            prev = minidx;
                            for (next = scaleIdx_next[prev]; weights[next] < scale_min; next = scaleIdx_next[prev]) {
                                if (next == NONFoundINT) break;
                                prev = next;
                                // if ( j < prev ) ++j;
                            }
                            scaleIdx_next[nextidx] = next;
                            if (next != NONFoundINT) scaleIdx_prev[next] = nextidx;
                            scaleIdx_prev[nextidx] = prev;
                            scaleIdx_next[prev] = nextidx;
                        }
                    }

                    if (previdx != NONFoundINT) {
                        GA_Size next_nextidx = idx_prev[previdx];
                        if (next_nextidx != NONFoundINT) {
                            fpreal_polyReduce2D scale_min;

                            switch (geoPropertyType)
                            {
                            case PolyReduce2D_ANGLE: scale_min = -dot(dirs[previdx], dirs[nextidx]);                                                         break;
                            case PolyReduce2D_DIST:  scale_min = GA_FeE_Math::distToLine(dirs[previdx], dirs[nextidx], poses[next_nextidx], poses[nextidx]); break;
                            case PolyReduce2D_ROC:   scale_min = GA_FeE_Math::circleRadius3Point(poses[next_nextidx], poses[previdx], poses[nextidx]);       break;
                            default:                 scale_min = -dot(dirs[previdx], dirs[nextidx]);                                                         break;
                            }

                            weights[previdx] = scale_min;

                            GA_Size prev = scaleIdx_prev[previdx];
                            GA_Size next = scaleIdx_next[previdx];
                            if (next != NONFoundINT) scaleIdx_prev[next] = prev;
                            if (prev != NONFoundINT) scaleIdx_next[prev] = next;

                            // GA_Size j = 0;
                            prev = minidx;
                            for (next = scaleIdx_next[prev]; weights[next] < scale_min; next = scaleIdx_next[prev]) {
                                if (next == NONFoundINT) break;
                                prev = next;
                                // if ( j < prev ) ++j;
                            }
                            scaleIdx_next[previdx] = next;
                            if (next != NONFoundINT) scaleIdx_prev[next] = previdx;
                            scaleIdx_prev[previdx] = prev;
                            scaleIdx_next[prev] = previdx;
                        }
                    }
                    idx_prev[nextidx] = previdx;
                    idx_next[previdx] = nextidx;

                    --primpointscount;

                    minidx = scaleIdx_next[minidx];
                }


                if (coverSourcePoly)
                {
                    // GA_Size primvertices[];
                    // resize(primvertices, primpointscount);
                    // resize(i[]@primpoints_out, primpointscount);

                    GA_Size i = 0;
                    for (GA_Size k = minidx; k != NONFoundINT; k = scaleIdx_next[k]) {
                        GA_Offset primpoint = vtxPointRef->getLink(vertices[k]);
                        polyReduce2DPtGroup->setElement(primpoint, true);


                        switch (geoPropertyType)
                        {
                        case PolyReduce2D_ANGLE:
                            ndir_h.set(primpoint, dirs[k]);
                            break;
                        case PolyReduce2D_DIST:
                            ndir = dirs[k];
                            ndir.normalize();
                            ndir_h.set(primpoint, ndir);
                            break;
                        case PolyReduce2D_ROC:
                            break;
                        default:
                            ndir_h.set(primpoint, dirs[k]);
                            break;
                        }
                        // weight_h.set(primpoint, weights[k]);

                        // i[]@primpoints_out[i] = primpoint;
                        // primvertices[i++] = k;
                    }
                    // GA_Size argsort[] = argsort(primvertices);
                    // i[]@primpoints_out = reorder(i[]@primpoints_out, argsort);
                }
                else
                {
                    for (GA_Size k = minidx; k != NONFoundINT; k = scaleIdx_next[k]) {
                        GA_Offset primpoint = vtxPointRef->getLink(vertices[k]);
                        polyReduce2DPtGroup->setElement(primpoint, true);
                        //ndir_h.set(primpoint, dirs[k]);
                        //weight_h.set(primpoint, weights[k]);
                    }
                }

            } // end of for each primitive
        }
    }, subscribeRatio, minGrainSize);

    polyReduce2DPtGroup->invalidateGroupEntries();

    if (reverseGroup)
    {
        polyReduce2DPtGroup->toggleAll(geo->getNumPoints());
    }


    if (!coverSourcePoly)
        return;

    ///////////////////////// Cover Source Poly /////////////////////////////////


}




//polyReduce2D(geo, groupName, reverseGroup, delGroup);
static void
polyReduce2D(
    GA_Detail* const geo,
    const GA_PrimitiveGroup* const geoGroup,
    GA_PointGroup* const polyReduce2DPtGroup,

    const bool delInLinePoint = true,
    const fpreal threshold_inlineAngleRadians = 1e-05,

    const bool limitByGeoProperty = true,
    const PolyReduce2D_GeoPropertyType geoPropertyType = PolyReduce2D_ANGLE,
    fpreal threshold_maxAngle = GA_FeE_Type::radians(150),
    const fpreal threshold_maxDist = 1e-04,

    const bool limitMinPoint = false,
    const exint minPoint = 10,

    const bool coverSourcePoly = false,
    const bool reverseGroup = false,
    const bool delPoint = false,
    const exint subscribeRatio = 64,
    const exint minGrainSize = 16
)
{
    polyReduce2D(geo, geoGroup, polyReduce2DPtGroup,
        delInLinePoint, threshold_inlineAngleRadians,
        limitByGeoProperty, geoPropertyType, threshold_maxAngle, threshold_maxDist,
        limitMinPoint, minPoint,
        coverSourcePoly, reverseGroup,
        subscribeRatio, minGrainSize);

    if (delPoint)
    {
        geo->destroyPointOffsets(GA_Range(geo->getPointMap(), polyReduce2DPtGroup, true), GA_Detail::GA_DestroyPointMode::GA_DESTROY_DEGENERATE_INCOMPATIBLE);
    }
}


//polyReduce2D(geo, groupName, reverseGroup, delGroup);
static GA_PointGroup*
polyReduce2D(
    GA_Detail* const geo,
    const GA_PrimitiveGroup* const geoGroup,
    const UT_StringHolder& outGroupName,

    const bool delInLinePoint = true,
    const fpreal threshold_inlineAngleRadians = 1e-05,

    const bool limitByGeoProperty = true,
    const PolyReduce2D_GeoPropertyType geoPropertyType = PolyReduce2D_ANGLE,
    fpreal threshold_maxAngle = GA_FeE_Type::radians(150),
    const fpreal threshold_maxDist = 1e-04,

    const bool limitMinPoint = false,
    const exint minPoint = 10,

    const bool coverSourcePoly = false,
    const bool reverseGroup = false,
    const bool delPoint = false,
    const exint subscribeRatio = 64,
    const exint minGrainSize = 16
)
{
    GEOsplitPoints(static_cast<GEO_Detail*>(geo), geoGroup);

#if 1
    if (delInLinePoint)
    {
        GA_FeE_PrimInlinePoint::delPrimInlinePoint_fast(geo, geoGroup,
            threshold_inlineAngleRadians, reverseGroup);
    }
#else
    GA_PointGroupUPtr inlinePtGroupUPtr;
    GA_PointGroup* inlinePtGroup = nullptr;
    if (delInLinePoint)
    {
        if (delPoint)
        {
            GA_FeE_PrimInlinePoint::delPrimInlinePoint_fast(geo, geoGroup,
                threshold_inlineAngleRadians, reverseGroup);
        }
        else
        {
            inlinePtGroupUPtr = GA_FeE_PrimInlinePoint::groupDetachedPrimInlinePoint_fast(geo, geoGroup,
                threshold_inlineAngleRadians, reverseGroup);
            inlinePtGroup = inlinePtGroupUPtr.get();
        }
    }
#endif

    GA_PointGroupUPtr polyReduce2DPtGroupUPtr;
    GA_PointGroup* polyReduce2DPtGroup = nullptr;
    if (delPoint)
    {
        polyReduce2DPtGroupUPtr = geo->createDetachedPointGroup();
        polyReduce2DPtGroup = polyReduce2DPtGroupUPtr.get();
    }
    else
    {
        if (outGroupName.length() == 0 || !outGroupName.isstring())
            return nullptr;
        polyReduce2DPtGroup = static_cast<GA_PointGroup*>(geo->pointGroups().newGroup(outGroupName));
    }

    polyReduce2D(geo, geoGroup, polyReduce2DPtGroup,
        delInLinePoint, threshold_inlineAngleRadians,
        limitByGeoProperty, geoPropertyType, threshold_maxAngle, threshold_maxDist,
        limitMinPoint, minPoint,
        coverSourcePoly, reverseGroup, delPoint,
        subscribeRatio, minGrainSize);

    if (delPoint)
    {
        return nullptr;
    }

    return polyReduce2DPtGroup;
}



//polyReduce2D(geo, groupName, reverseGroup, delGroup);
//static GA_PointGroup*
//polyReduce2D(
//    GA_Detail* const geo,
//    const GA_PrimitiveGroup* const geoGroup,
//    const UT_StringHolder& outGroupName,
//
//    const PolyReduce2D_GeoPropertyType geoPropertyType = PolyReduce2D_ANGLE,
//    const bool delInLinePoint = true,
//
//    const fpreal threshold_inlineAngleRadians = 1e-05,
//    const bool reverseGroup = false,
//    const bool delPoint = false,
//    const bool coverSourcePoly = false,
//    const exint subscribeRatio = 64,
//    const exint minGrainSize = 16
//)
//{
//    polyReduce2D(geo, geoGroup, outGroupName,
//        geoPropertyType, delInLinePoint, threshold_inlineAngleRadians,
//        reverseGroup, delPoint, coverSourcePoly,
//        subscribeRatio, minGrainSize);
//}


//polyReduce2D(geo, groupName, reverseGroup, delGroup);
static GA_PointGroup*
polyReduce2D(
    const SOP_NodeVerb::CookParms& cookparms,
    GA_Detail* const geo,
    const UT_StringHolder& inGroupName,
    const UT_StringHolder& outGroupName,

    const bool delInLinePoint = true,
    const fpreal threshold_inlineAngleRadians = 1e-05,

    const bool limitByGeoProperty = true,
    const PolyReduce2D_GeoPropertyType geoPropertyType = PolyReduce2D_ANGLE,
    fpreal threshold_maxAngle = GA_FeE_Type::radians(150),
    const fpreal threshold_maxDist = 1e-04,

    const bool limitMinPoint = false,
    const exint minPoint = 10,

    const bool coverSourcePoly = false,
    const bool reverseGroup = false,
    const bool delPoint = false,
    const exint subscribeRatio = 64,
    const exint minGrainSize = 16
)
{
    GOP_Manager gop;
    const GA_PrimitiveGroup* const geoPrimGroup = GA_FeE_Group::findOrParsePrimitiveGroupDetached(cookparms, geo, inGroupName, gop);
    if (geoPrimGroup && geoPrimGroup->isEmpty())
        return nullptr;

    GA_PointGroup* const polyReduce2DPtGroup = polyReduce2D(geo, geoPrimGroup, outGroupName,
        delInLinePoint, threshold_inlineAngleRadians,
        limitByGeoProperty, geoPropertyType, threshold_maxAngle, threshold_maxDist,
        limitMinPoint, minPoint,
        coverSourcePoly, reverseGroup, delPoint,
        subscribeRatio, minGrainSize);

    return polyReduce2DPtGroup;
}






} // End of namespace GA_FeE_PolyReduce2D

#endif
