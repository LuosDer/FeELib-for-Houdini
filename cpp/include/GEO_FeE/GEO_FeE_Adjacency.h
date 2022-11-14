
#pragma once

#ifndef __GEO_FeE_Adjacency_h__
#define __GEO_FeE_Adjacency_h__

//#include <GEO_FeE/GEO_FeE_Adjacency.h>

#include <GEO/GEO_Detail.h>
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

#include <GA/GA_Primitive.h>
#include <GA/GA_Defaults.h>



namespace GEO_FeE_Adjacency {

#if 1
    typedef GA_ROHandleT<GA_Offset> GA_Offset_ROHandle;
    typedef GA_ROHandleT<GA_Size> GA_Size_ROHandle;
#else
    typedef GA_RWHandleT<GA_Offset> GA_Offset_ROHandle;
    typedef GA_RWHandleT<GA_Size> GA_Size_ROHandle;
#endif



    //Get Vertex Prim Index
    static GA_Size
        vertexPrimIndex(
            const GA_Detail* geo,
            const GA_Offset& vtxoff
        )
    {
        return geo->getPrimitiveVertexList(geo->vertexPrimitive(vtxoff)).find(vtxoff);
    }

    static GA_Size
        vertexPrimIndex(
            const GA_Detail* geo,
            const GA_Offset& primoff,
            const GA_Offset& vtxoff
        )
    {
        return geo->getPrimitiveVertexList(primoff).find(vtxoff);
    }




    //Get Vertex Destination Point
    //This is Faster than use linear vertex offset
    static GA_Offset
        vertexVertexDst(
            const GA_Detail* geo,
            const GA_Offset& primoff,
            const GA_Size& vtxpnum
        )
    {
        GA_Size vtxpnum_next = vtxpnum + 1;
        if (vtxpnum_next == geo->getPrimitiveVertexCount(primoff)) {
            if (geo->getPrimitiveClosedFlag(primoff))
            {
                return geo->getPrimitiveVertexOffset(primoff, 0);
            }
            else
            {
                return -1;
            }
        }
        else
        {
            return geo->getPrimitiveVertexOffset(primoff, vtxpnum_next);
        }
    }


    //Get Vertex Destination Point
    static GA_Offset
        vertexVertexDst(
            const GA_Detail* geo,
            const GA_Offset& vtxoff
        )
    {
        GA_Offset primoff = geo->vertexPrimitive(vtxoff);
        return vertexVertexDst(geo, primoff, vertexPrimIndex(geo, primoff, vtxoff));
    }



    //Get Vertex Destination Point
#if 1
    //This is Faster than use linear vertex offset
    static GA_Offset
        vertexPointDst(
            const GA_Detail* geo,
            const GA_Offset& primoff,
            const GA_Size& vtxpnum
        )
    {
        GA_Size vtxpnum_next = vtxpnum + 1;
        if (vtxpnum_next == geo->getPrimitiveVertexCount(primoff)) {
            if (geo->getPrimitiveClosedFlag(primoff))
            {
                return geo->vertexPoint(geo->getPrimitiveVertexOffset(primoff, 0));
            }
            else
            {
                return -1;
            }
        }
        else
        {
            return geo->vertexPoint(geo->getPrimitiveVertexOffset(primoff, vtxpnum_next));
        }
    }

    static GA_Offset
    vertexPointDst(
        const GA_Detail* geo,
        const GA_Offset& vtxoff
    )
    {
        GA_Offset primoff = geo->vertexPrimitive(vtxoff);
        return vertexPointDst(geo, primoff, vertexPrimIndex(geo, primoff, vtxoff));
    }
#else
    static GA_Offset
        vertexPointDst(
            const GA_Detail* geo,
            const GA_Offset& vtxoff
        )
    {
        GA_Offset vertexVertexDst = vertexPointDst(geo, vtxoff);
        if (vertexVertexDst == -1)
        {
            return -1;
        }
        else
        {
            return geo->vertexPoint(vertexVertexDst);
        }
    }
    static GA_Offset
        vertexPointDst(
            const GA_Detail* geo,
            const GA_Offset& primoff,
            const GA_Size& vtxpnum
        )
    {
        GA_Offset vertexVertexDst = vertexPointDst(geo, primoff, vtxpnum);
        if (vertexVertexDst == -1)
        {
            return -1;
        }
        else
        {
            return geo->vertexPoint(vertexVertexDst);
        }
    }
#endif




    //Get Vertex Next Equiv Vertex
    static GA_Offset
        vertexVertexNextEquiv(
            const GA_Detail* geo,
            const GA_Offset& vtxoff,
            const GA_Offset& dstpt
        )
    {
        if (dstpt < 0)
            return -1;

        GA_Offset vtxoff_next;
        GA_Offset nextVtxDstpt;
        for (vtxoff_next = geo->vertexToNextVertex(vtxoff); vtxoff_next != GA_INVALID_OFFSET; vtxoff_next = geo->vertexToNextVertex(vtxoff_next))
        {
            nextVtxDstpt = vertexPointDst(geo, vtxoff_next);
            if (nextVtxDstpt == dstpt)
                return vtxoff_next;
        }

        GA_Offset ptnum = geo->vertexPoint(vtxoff);
        for (vtxoff_next = geo->pointVertex(dstpt); vtxoff_next != GA_INVALID_OFFSET; vtxoff_next = geo->vertexToNextVertex(vtxoff_next))
        {
            nextVtxDstpt = vertexPointDst(geo, vtxoff_next);
            if (nextVtxDstpt == ptnum)
            {
                if (dstpt < ptnum)
                    return -1;
                return vtxoff_next;
            }
        }
        return -1;
    }

    static GA_Offset
        vertexVertexNextEquiv(
            const GA_Detail* geo,
            const GA_Offset& vtxoff,
            const GA_Offset& primoff,
            const GA_Size& vtxpnum
        )
    {
        return vertexVertexNextEquiv(geo, vtxoff, vertexPointDst(geo, primoff, vtxpnum));
    }

    static GA_Offset
        vertexVertexNextEquiv(
            const GA_Detail * geo,
            const GA_Offset & vtxoff
        )
    {
        GA_Offset dstpt = vertexPointDst(geo, vtxoff);
        return vertexVertexNextEquiv(geo, vtxoff, dstpt);
    }
















#if 1
    //Get all vertices NextEquiv Vertex
    //template<typename T>
    static void
        vertexPrimIndex(
            const GA_Detail* geo,
            const GA_RWHandleT<GA_Size>& attribHandle,
            const GA_VertexGroup* geoGroup = nullptr,
            const exint& subscribeRatio = 32,
            const exint& minGrainSize = 1024
        )
    {
        UT_AutoInterrupt boss("vertexPrimIndex");
        if (geoGroup)
        {
#if 1
            const GA_PrimitiveGroupUPtr promotedGroupUPtr = geo->createDetachedPrimitiveGroup();
            GA_PrimitiveGroup* promotedGroup = promotedGroupUPtr.get();
            promotedGroup->combine(geoGroup);
#else
            GOP_Manager gop;
            const GA_PrimitiveGroup* promotedGroup = nullptr;
            GA_PrimitiveGroupUPtr groupDeleter;
            GA_PrimitiveGroup* newDetachedGroup = new GA_PrimitiveGroup(*geo);
            groupDeleter.reset(newDetachedGroup);
            newDetachedGroup->combine(promotedGroup);
            promotedGroup = newDetachedGroup;
#endif
            const GA_SplittableRange geo0SplittableRange0(geo->getPrimitiveRange(promotedGroup));
            UTparallelFor(geo0SplittableRange0, [&boss, &geo, &attribHandle, &geoGroup](const GA_SplittableRange& r)
            {
                if (boss.wasInterrupted())
                    return;
                GA_Offset start;
                GA_Offset end;
                for (GA_Iterator it(r); it.blockAdvance(start, end); )
                {
                    for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                    {
                        GA_OffsetListRef vertices = geo->getPrimitiveVertexList(elemoff);
                        GA_Size size = vertices.size();
                        for (GA_Size vtxpnum = 0; vtxpnum < size; ++vtxpnum)
                        {
                            GA_Offset vtxoff = vertices[vtxpnum];
                            if (!geoGroup->contains(vtxoff))
                                continue;
                            attribHandle.set(vtxoff, vtxpnum);
                        }
                    }
                }
            }, subscribeRatio, minGrainSize);
        }
        else
        {
            const GA_SplittableRange geo0SplittableRange0(geo->getPrimitiveRange(nullptr));
            UTparallelFor(geo0SplittableRange0, [&geo, &attribHandle, &boss](const GA_SplittableRange& r)
            {
                if (boss.wasInterrupted())
                    return;
                GA_Offset start;
                GA_Offset end;
                for (GA_Iterator it(r); it.blockAdvance(start, end); )
                {
                    for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                    {
                        GA_OffsetListRef vertices = geo->getPrimitiveVertexList(elemoff);
                        GA_Size size = vertices.size();
                        for (GA_Size vtxpnum = 0; vtxpnum < size; ++vtxpnum)
                        {
                            attribHandle.set(vertices[vtxpnum], vtxpnum);
                        }
                    }
                }
            }, subscribeRatio, minGrainSize);
        }
        //attribHandle->bumpDataId();
    }
#else
    //Get all vertices NextEquiv Vertex
    //template<typename T>
    static void
        vertexPrimIndex(
            const GA_Detail* geo,
            const GA_RWHandleT<GA_Size>& attribHandle,
            const GA_VertexGroup* geoGroup = nullptr,
            const exint& subscribeRatio = 16,
            const exint& minGrainSize = 1024
        )
    {
        UT_AutoInterrupt boss("vertexPrimIndex");

        const GA_SplittableRange geo0SplittableRange0(geo->getVertexRange(geoGroup));
        UTparallelFor(geo0SplittableRange0, [&geo, &attribHandle, &boss](const GA_SplittableRange& r)
        {
            if (boss.wasInterrupted())
                return;
            GA_Offset start;
            GA_Offset end;
            for (GA_Iterator it(r); it.blockAdvance(start, end); )
            {
                for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                {
                    attribHandle.set(elemoff, vertexPrimIndex(geo, elemoff));
                }
            }
        }, subscribeRatio, minGrainSize);

        //attribHandle->bumpDataId();
    }
#endif



    //Get all vertices NextEquiv Vertex
    //template<typename T>
    static void
        vertexVertexDst(
            const GA_Detail* geo,
            const GA_RWHandleT<GA_Offset>& attribHandle,
            const GA_Size_ROHandle& vtxpnumAttribHandle,
            const GA_VertexGroup* geoGroup = nullptr,
            const exint& subscribeRatio = 16,
            const exint& minGrainSize = 1024
        )
    {
        UT_AutoInterrupt boss("vertexVertexDst");

        const GA_SplittableRange geo0SplittableRange0(geo->getVertexRange(geoGroup));
        UTparallelFor(geo0SplittableRange0, [&geo, &attribHandle, &vtxpnumAttribHandle, &boss](const GA_SplittableRange& r)
        {
            if (boss.wasInterrupted())
                return;
            GA_Offset start;
            GA_Offset end;
            for (GA_Iterator it(r); it.blockAdvance(start, end); )
            {
                for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                {
                    attribHandle.set(elemoff, vertexVertexDst(geo, geo->vertexPrimitive(elemoff), vtxpnumAttribHandle.get(elemoff)));
                }
            }
        }, subscribeRatio, minGrainSize);

        //attribHandle->bumpDataId();
    }



    //Get all vertices NextEquiv Vertex
    //template<typename T>
    static void
        vertexPointDst(
            const GA_Detail* geo,
            const GA_RWHandleT<GA_Offset>& attribHandle,
            const GA_Size_ROHandle& vtxpnumAttribHandle,
            const GA_VertexGroup* geoGroup = nullptr,
            const exint& subscribeRatio = 16,
            const exint& minGrainSize = 1024
        )
    {
        UT_AutoInterrupt boss("vertexPointDst");

        const GA_SplittableRange geo0SplittableRange0(geo->getVertexRange(geoGroup));
        UTparallelFor(geo0SplittableRange0, [&geo, &attribHandle, &vtxpnumAttribHandle, &boss](const GA_SplittableRange& r)
        {
            if (boss.wasInterrupted())
                return;
            GA_Offset start;
            GA_Offset end;
            for (GA_Iterator it(r); it.blockAdvance(start, end); )
            {
                for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                {
                    attribHandle.set(elemoff, vertexPointDst(geo, geo->vertexPrimitive(elemoff), vtxpnumAttribHandle.get(elemoff)));
                }
            }
        }, subscribeRatio, minGrainSize);

        //attribHandle->bumpDataId();
    }




    //Get all vertices NextEquiv Vertex
    //template<typename T>
    static void
        vertexPointDst(
            const GA_Detail* geo,
            const GA_RWHandleT<GA_Offset>& attribHandle,
            const GA_VertexGroup* geoGroup = nullptr,
            const exint& subscribeRatio = 16,
            const exint& minGrainSize = 1024
        )
    {
        UT_AutoInterrupt boss("vertexPointDst");

        const GA_SplittableRange geo0SplittableRange0(geo->getVertexRange(geoGroup));
        UTparallelFor(geo0SplittableRange0, [&geo, &attribHandle, &boss](const GA_SplittableRange& r)
        {
            if (boss.wasInterrupted())
                return;
            GA_Offset start;
            GA_Offset end;
            for (GA_Iterator it(r); it.blockAdvance(start, end); )
            {
                for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                {
                    attribHandle.set(elemoff, vertexPointDst(geo, elemoff));
                }
            }
        }, subscribeRatio, minGrainSize);

        //attribHandle->bumpDataId();
    }





    //Get all vertices NextEquiv Vertex
    //template<typename T>
    static void
        vertexVertexNextEquiv(
            const GA_Detail* geo,
            const GA_RWHandleT<GA_Offset>& attribHandle,
            const GA_VertexGroup* geoGroup = nullptr,
            const exint& subscribeRatio = 16,
            const exint& minGrainSize = 1024
        )
    {
        UT_AutoInterrupt boss("vertexVertexNextEquiv");

        const GA_SplittableRange geo0SplittableRange0(geo->getVertexRange(geoGroup));
        UTparallelFor(geo0SplittableRange0, [&geo, &attribHandle, &boss](const GA_SplittableRange& r)
        {
            if (boss.wasInterrupted())
                return;
            GA_Offset start;
            GA_Offset end;
            for (GA_Iterator it(r); it.blockAdvance(start, end); )
            {
                for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                {
                    attribHandle.set(elemoff, vertexVertexNextEquiv(geo, elemoff));
                }
            }
        }, subscribeRatio, minGrainSize);
        //attribHandle->bumpDataId();
    }





    //Get all vertices NextEquiv Vertex
    static void
        vertexVertexNextEquiv(
            const GA_Detail* geo,
            const GA_RWHandleT<GA_Offset>& attribHandle,
            GA_VertexGroup* validGroup,
            const GA_Offset_ROHandle& dstptAttribHandle,
            const GA_VertexGroup* geoGroup = nullptr,
            const exint& subscribeRatio = 16,
            const exint& minGrainSize = 1024
        )
    {
        UT_AutoInterrupt boss("vertexVertexNextEquiv");

        const GA_SplittableRange geo0SplittableRange0(geo->getVertexRange(geoGroup));
        UTparallelFor(geo0SplittableRange0, [&geo, &attribHandle, &dstptAttribHandle, &validGroup, &boss](const GA_SplittableRange& r)
        {
            if (boss.wasInterrupted())
                return;

            GA_Offset vtxoff_next;
            GA_Offset start;
            GA_Offset end;
            for (GA_Iterator it(r); it.blockAdvance(start, end); )
            {
                for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                {
                    GA_Offset dstpt = dstptAttribHandle.get(elemoff);
                    if (dstpt < 0)
                    {
                        attribHandle.set(elemoff, -1);
                        validGroup->setElement(elemoff, true);
                        continue;
                    }

                    for (vtxoff_next = geo->vertexToNextVertex(elemoff); vtxoff_next != GA_INVALID_OFFSET; vtxoff_next = geo->vertexToNextVertex(vtxoff_next))
                    {
                        if (dstptAttribHandle.get(vtxoff_next) != dstpt)
                            continue;
                        dstpt = -1;
                        attribHandle.set(elemoff, vtxoff_next);
                        validGroup->setElement(elemoff, true);
                        break;
                    }

                    if (dstpt < 0)
                        continue;

                    GA_Offset ptnum = geo->vertexPoint(elemoff);
                    for (vtxoff_next = geo->pointVertex(dstpt); vtxoff_next != GA_INVALID_OFFSET; vtxoff_next = geo->vertexToNextVertex(vtxoff_next))
                    {
                        if (dstptAttribHandle.get(vtxoff_next) != ptnum)
                            continue;
                        if (dstpt < ptnum)
                            break;
                        dstpt = -1;
                        attribHandle.set(elemoff, vtxoff_next);
                    }
                    if (dstpt > 0)
                    {
                        attribHandle.set(elemoff, -1);
                        validGroup->setElement(elemoff, true);
                    }
                }
            }
        }, subscribeRatio, minGrainSize);

        //attribHandle->bumpDataId();
        validGroup->invalidateGroupEntries();
    }



    //Get all vertices NextEquiv Vertex
    static void
        vertexVertexNextEquiv(
            const GA_Detail* geo,
            const GA_RWHandleT<GA_Offset>& attribHandle,
            const GA_Offset_ROHandle& dstptAttribHandle,
            const GA_VertexGroup* geoGroup = nullptr,
            const exint& subscribeRatio = 16,
            const exint& minGrainSize = 1024
        )
    {
        UT_AutoInterrupt boss("vertexVertexNextEquiv");

        const GA_SplittableRange geo0SplittableRange0(geo->getVertexRange(geoGroup));
        UTparallelFor(geo0SplittableRange0, [&geo, &attribHandle, &dstptAttribHandle, &boss](const GA_SplittableRange& r)
        {
            if (boss.wasInterrupted())
                return;

            GA_Offset vtxoff_next;
            GA_Offset start;
            GA_Offset end;
            for (GA_Iterator it(r); it.blockAdvance(start, end); )
            {
                for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                {
                    GA_Offset dstpt = dstptAttribHandle.get(elemoff);
                    if (dstpt < 0)
                    {
                        attribHandle.set(elemoff, -1);
                        continue;
                    }

                    for (vtxoff_next = geo->vertexToNextVertex(elemoff); vtxoff_next != GA_INVALID_OFFSET; vtxoff_next = geo->vertexToNextVertex(vtxoff_next))
                    {
                        if (dstptAttribHandle.get(vtxoff_next) != dstpt)
                            continue;
                        dstpt = -1;
                        attribHandle.set(elemoff, vtxoff_next);
                        break;
                    }

                    if (dstpt < 0)
                        continue;

                    GA_Offset ptnum = geo->vertexPoint(elemoff);
                    for (vtxoff_next = geo->pointVertex(dstpt); vtxoff_next != GA_INVALID_OFFSET; vtxoff_next = geo->vertexToNextVertex(vtxoff_next))
                    {
                        if (dstptAttribHandle.get(vtxoff_next) != ptnum)
                            continue;
                        if (dstpt < ptnum)
                            break;
                        dstpt = -1;
                        attribHandle.set(elemoff, vtxoff_next);
                    }
                    if (dstpt > 0)
                        attribHandle.set(elemoff, -1);
                }
            }
        }, subscribeRatio, minGrainSize);

        //attribHandle->bumpDataId();
    }



    //Get all vertices NextEquiv Vertex
    static void
        vertexVertexNextEquiv(
            const GA_Detail* geo,
            GA_VertexGroup* validGroup,
            const GA_Offset_ROHandle& dstptAttribHandle,
            const GA_VertexGroup* geoGroup = nullptr,
            const exint& subscribeRatio = 16,
            const exint& minGrainSize = 1024
        )
    {
        UT_AutoInterrupt boss("vertexVertexNextEquiv");

        const GA_SplittableRange geo0SplittableRange0(geo->getVertexRange(geoGroup));
        UTparallelFor(geo0SplittableRange0, [&geo, &dstptAttribHandle, &validGroup, &boss](const GA_SplittableRange& r)
        {
            if (boss.wasInterrupted())
                return;

            GA_Offset vtxoff_next;
            GA_Offset start;
            GA_Offset end;
            for (GA_Iterator it(r); it.blockAdvance(start, end); )
            {
                for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                {
                    GA_Offset dstpt = dstptAttribHandle.get(elemoff);
                    if (dstpt < 0)
                    {
                        validGroup->setElement(elemoff, true);
                        continue;
                    }

                    for (vtxoff_next = geo->vertexToNextVertex(elemoff); vtxoff_next != GA_INVALID_OFFSET; vtxoff_next = geo->vertexToNextVertex(vtxoff_next))
                    {
                        if (dstptAttribHandle.get(vtxoff_next) != dstpt)
                            continue;
                        dstpt = -1;
                        validGroup->setElement(elemoff, true);
                        break;
                    }

                    if (dstpt < 0)
                        continue;

                    GA_Offset ptnum = geo->vertexPoint(elemoff);
                    for (vtxoff_next = geo->pointVertex(dstpt); vtxoff_next != GA_INVALID_OFFSET; vtxoff_next = geo->vertexToNextVertex(vtxoff_next))
                    {
                        if (dstptAttribHandle.get(vtxoff_next) == ptnum)
                            continue;
                        if (dstpt < ptnum)
                            break;
                        dstpt = -1;
                    }
                    if (dstpt > 0)
                        validGroup->setElement(elemoff, true);
                }
            }
        }, subscribeRatio, minGrainSize);

        validGroup->invalidateGroupEntries();
    }











    //Get Vertex Destination Point
    static void
        pointPointEdge(
            const GA_Detail* geo,
            const GA_Offset& ptoff,
            UT_ValArray<GA_Offset>& ptoffArray
        )
    {
        //exint cap = 0;
        ptoffArray.setSizeNoInit(0);

        GA_Offset vtxoff_next = geo->pointVertex(ptoff);
        GA_Offset primoff;
        GA_Offset pt_next;
        GA_Size vtxpnum;
        GA_Size vtxpnum_next;
        GA_Size numvtx;
        for (vtxoff_next = geo->pointVertex(ptoff); vtxoff_next != GA_INVALID_OFFSET; vtxoff_next = geo->vertexToNextVertex(vtxoff_next))
        {
            primoff = geo->vertexPrimitive(vtxoff_next);
            numvtx = geo->getPrimitiveVertexCount(primoff);
            vtxpnum = vertexPrimIndex(geo, primoff, vtxoff_next);

            if (vtxpnum == 0)
            {
                if (geo->getPrimitiveClosedFlag(primoff))
                {
                    pt_next = geo->vertexPoint(geo->getPrimitiveVertexOffset(primoff, numvtx-1));
                    if (ptoffArray.find(pt_next) == -1)
                        ptoffArray.emplace_back(pt_next);
                }
            }
            else
            {
                pt_next = geo->vertexPoint(geo->getPrimitiveVertexOffset(primoff, vtxpnum-1));
                if (ptoffArray.find(pt_next) == -1)
                    ptoffArray.emplace_back(pt_next);
            }

            vtxpnum_next = vtxpnum + 1;
            if (vtxpnum_next == numvtx) {
                if (geo->getPrimitiveClosedFlag(primoff))
                {
                    pt_next = geo->vertexPoint(geo->getPrimitiveVertexOffset(primoff, 0));
                    if (ptoffArray.find(pt_next) == -1)
                        ptoffArray.emplace_back(pt_next);
                }
            }
            else
            {
                pt_next = geo->vertexPoint(geo->getPrimitiveVertexOffset(primoff, vtxpnum_next));
                if (ptoffArray.find(pt_next) == -1)
                    ptoffArray.emplace_back(pt_next);
            }
        }
    }


    //Get all prims neighbours prims with adjacent by edge
    static void
        pointPointEdge(
            const GA_Detail* geo,
            const GA_RWHandleT<UT_ValArray<GA_Offset>>& attribHandle,
            const GA_Size_ROHandle& vtxpnumAttribHandle,
            const GA_PointGroup* geoGroup = nullptr,
            const GA_PointGroup* seamGroup = nullptr,
            const exint& subscribeRatio = 16,
            const exint& minGrainSize = 1024
        )
    {
        UT_AutoInterrupt boss("pointPointEdge");

        const GA_SplittableRange geo0SplittableRange0(geo->getPointRange(geoGroup));
        UTparallelFor(geo0SplittableRange0, [&geo, &attribHandle, &vtxpnumAttribHandle , &seamGroup, &boss](const GA_SplittableRange& r)
        {
            if (boss.wasInterrupted())
                return;
            UT_ValArray<GA_Offset> ptoffArray;
            GA_Offset start;
            GA_Offset end;
            for (GA_Iterator it(r); it.blockAdvance(start, end); )
            {
                for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                {
                    ptoffArray.setSizeNoInit(0);

                    GA_Offset vtxoff_next = geo->pointVertex(elemoff);
                    GA_Offset primoff;
                    GA_Offset pt_next;
                    GA_Size vtxpnum;
                    GA_Size vtxpnum_next;
                    GA_Size numvtx;
                    for (vtxoff_next = geo->pointVertex(elemoff); vtxoff_next != GA_INVALID_OFFSET; vtxoff_next = geo->vertexToNextVertex(vtxoff_next))
                    {
                        primoff = geo->vertexPrimitive(vtxoff_next);
                        numvtx = geo->getPrimitiveVertexCount(primoff);
                        vtxpnum = vtxpnumAttribHandle.get(vtxoff_next);

                        if (vtxpnum == 0)
                        {
                            if (geo->getPrimitiveClosedFlag(primoff))
                            {
                                pt_next = geo->vertexPoint(geo->getPrimitiveVertexOffset(primoff, numvtx - 1));
                                if (ptoffArray.find(pt_next) == -1)
                                    ptoffArray.emplace_back(pt_next);
                            }
                        }
                        else
                        {
                            pt_next = geo->vertexPoint(geo->getPrimitiveVertexOffset(primoff, vtxpnum - 1));
                            if (ptoffArray.find(pt_next) == -1)
                                ptoffArray.emplace_back(pt_next);
                        }

                        vtxpnum_next = vtxpnum + 1;
                        if (vtxpnum_next == numvtx) {
                            if (geo->getPrimitiveClosedFlag(primoff))
                            {
                                pt_next = geo->vertexPoint(geo->getPrimitiveVertexOffset(primoff, 0));
                                if (ptoffArray.find(pt_next) == -1)
                                    ptoffArray.emplace_back(pt_next);
                            }
                        }
                        else
                        {
                            pt_next = geo->vertexPoint(geo->getPrimitiveVertexOffset(primoff, vtxpnum_next));
                            if (ptoffArray.find(pt_next) == -1)
                                ptoffArray.emplace_back(pt_next);
                        }
                    }
                }
            }
        }, subscribeRatio, minGrainSize);

        //attribHandle->bumpDataId();
    }


    //Get all prims neighbours prims with adjacent by edge
    static void
    pointPointEdge(
        const GA_Detail* geo,
        const GA_RWHandleT<UT_ValArray<GA_Offset>>& attribHandle,
        const GA_PointGroup* geoGroup = nullptr,
        const GA_PointGroup* seamGroup = nullptr,
        const exint& subscribeRatio = 16,
        const exint& minGrainSize = 1024
    )
    {
        UT_AutoInterrupt boss("pointPointEdge");

        const GA_SplittableRange geo0SplittableRange0(geo->getPointRange(geoGroup));
        UTparallelFor(geo0SplittableRange0, [&geo, &attribHandle, &seamGroup, &boss](const GA_SplittableRange& r)
        {
            if (boss.wasInterrupted())
                return;
            UT_ValArray<GA_Offset> adjElems;
            GA_Offset start;
            GA_Offset end;
            for (GA_Iterator it(r); it.blockAdvance(start, end); )
            {
                for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                {
                    pointPointEdge(geo, elemoff, adjElems);
                    attribHandle.set(elemoff, adjElems);
                }
            }
        }, subscribeRatio, minGrainSize);

        //attribHandle->bumpDataId();
    }



#if 1
    //Get all prims neighbours prims with adjacent by edge
    static void
        primPrimEdge(
            const GA_Detail* geo,
            const GA_RWHandleT<UT_ValArray<GA_Offset>>& attribHandle,
            const GA_Offset_ROHandle& dstptAttribHandle,
            const GA_PrimitiveGroup* geoGroup = nullptr,
            const GA_VertexGroup* seamGroup = nullptr,
            const exint& subscribeRatio = 16,
            const exint& minGrainSize = 1024
        )
    {
        UT_AutoInterrupt boss("primPrimEdge");

        const GA_SplittableRange geo0SplittableRange0(geo->getPrimitiveRange(geoGroup));
        UTparallelFor(geo0SplittableRange0, [&geo, &attribHandle, &dstptAttribHandle, &seamGroup, &boss](const GA_SplittableRange& r)
        {
            if (boss.wasInterrupted())
                return;
            //UT_SmallArray<GA_Offset> adjElems;
            UT_ValArray<GA_Offset> adjElems;
            GA_Offset primoff;
            GA_Offset ptoff;
            GA_Offset dstpt;
            GA_Offset vtxoff_next;
            GA_Size numvtx;
            GA_OffsetListRef vertices;
            GA_Offset start;
            GA_Offset end;
            if (seamGroup)
            {
                for (GA_Iterator it(r); it.blockAdvance(start, end); )
                {
                    for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                    {
                        //adjElems.setSizeNoInit(0);
                        adjElems.clear();
                        vertices = geo->getPrimitiveVertexList(elemoff);
                        numvtx = vertices.size();
                        for (GA_Size i = 0; i < numvtx; ++i)
                        {
                            dstpt = dstptAttribHandle.get(vertices[i]);
                            ptoff = geo->vertexPoint(vertices[i]);
                            vtxoff_next = geo->pointVertex(ptoff);
                            for (vtxoff_next = geo->pointVertex(ptoff); vtxoff_next != GA_INVALID_OFFSET; vtxoff_next = geo->vertexToNextVertex(vtxoff_next))
                            {
                                if (vtxoff_next == vertices[i])
                                    continue;
                                if (dstptAttribHandle.get(vtxoff_next) != dstpt)
                                    continue;
                                primoff = geo->vertexPrimitive(vtxoff_next);
                                if (adjElems.find(primoff) != -1)
                                    continue;
                                //adjElems.emplace_back(primoff);
                                adjElems.append(primoff);
                            }

                            for (vtxoff_next = geo->pointVertex(dstpt); vtxoff_next != GA_INVALID_OFFSET; vtxoff_next = geo->vertexToNextVertex(vtxoff_next))
                            {
                                if (dstptAttribHandle.get(vtxoff_next) != ptoff)
                                    continue;
                                primoff = geo->vertexPrimitive(vtxoff_next);
                                if (adjElems.find(primoff) != -1)
                                    continue;
                                //adjElems.emplace_back(primoff);
                                adjElems.append(primoff);
                            }
                        }
                        attribHandle.set(elemoff, adjElems);
                    }
                }
            }
            else
            {
                for (GA_Iterator it(r); it.blockAdvance(start, end); )
                {
                    for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                    {
                        vertices = geo->getPrimitiveVertexList(elemoff);
                        numvtx = vertices.size();
                        adjElems.setSizeNoInit(numvtx);
                        for (GA_Size i = 0; i < numvtx; ++i)
                        {
                            dstpt = dstptAttribHandle.get(vertices[i]);
                            if (dstpt < 0)
                                continue;
                            ptoff = geo->vertexPoint(vertices[i]);
                            vtxoff_next = geo->pointVertex(ptoff);
                            for (vtxoff_next = geo->pointVertex(ptoff); vtxoff_next != GA_INVALID_OFFSET; vtxoff_next = geo->vertexToNextVertex(vtxoff_next))
                            {
                                if (vtxoff_next == vertices[i])
                                    continue;
                                if (dstptAttribHandle.get(vtxoff_next) != dstpt)
                                    continue;
                                primoff = geo->vertexPrimitive(vtxoff_next);
                                if (adjElems.find(primoff) >= 0)
                                    continue;
                                adjElems.emplace_back(primoff);
                            }

                            for (vtxoff_next = geo->pointVertex(dstpt); vtxoff_next != GA_INVALID_OFFSET; vtxoff_next = geo->vertexToNextVertex(vtxoff_next))
                            {
                                if (dstptAttribHandle.get(vtxoff_next) != ptoff)
                                    continue;
                                primoff = geo->vertexPrimitive(vtxoff_next);
                                if (adjElems.find(primoff) >= 0)
                                    continue;
                                adjElems.emplace_back(primoff);
                            }
                        }
                        attribHandle.set(elemoff, adjElems);
                    }
                }
            }
        }, subscribeRatio, minGrainSize);

        //attribHandle->bumpDataId();
    }
#else
    //Get all prims neighbours prims with adjacent by edge
    static void
        primPrimEdge(
            const GEO_Detail* geo,
            const GA_RWHandleT<UT_ValArray<GA_Offset>>& attribHandle,
            const GA_PrimitiveGroup* geoGroup = nullptr,
            const GA_VertexGroup* seamGroup = nullptr,
            const exint& subscribeRatio = 16,
            const exint& minGrainSize = 1024
        )
    {
        UT_AutoInterrupt boss("primPrimEdge");

        const GA_SplittableRange geo0SplittableRange0(geo->getPrimitiveRange(geoGroup));
        UTparallelFor(geo0SplittableRange0, [&geo, &attribHandle, &seamGroup, &boss](const GA_SplittableRange& r)
        {
            if (boss.wasInterrupted())
                return;
            GEO_Detail::GEO_EdgeAdjArray adjElemStruct;
            //UT_SmallArray<GA_Offset> adjElems;
            UT_ValArray<GA_Offset> adjElems;
            GA_Offset start;
            GA_Offset end;
            for (GA_Iterator it(r); it.blockAdvance(start, end); )
            {
                if (seamGroup)
                {
                    for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                    {
                        GA_Size numAdj = geo->getEdgeAdjacentPolygons(adjElemStruct, elemoff);
                        adjElems.setSizeNoInit(numAdj);
                        for (GA_Size i = 0; i < numAdj; ++i)
                        {
                            //if (attribValue != UNREACHED_NUMBER)
                            //    continue;
                            adjElems[i] = adjElemStruct[i].myAdjacentPolygon;
                        }
                        attribHandle.set(elemoff, adjElems);
                    }
                }
                else
                {
                    for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                    {
                        GA_Size numAdj = geo->getEdgeAdjacentPolygons(adjElemStruct, elemoff);
                        adjElems.setSizeNoInit(numAdj);
                        for (GA_Size i = 0; i < numAdj; ++i)
                        {
                            //if (attribValue != UNREACHED_NUMBER)
                            //    continue;
                            adjElems[i] = adjElemStruct[i].myAdjacentPolygon;
                        }
                        attribHandle.set(elemoff, adjElems);
                    }
                }
            }
        }, subscribeRatio, minGrainSize);

        //attribHandle->bumpDataId();
    }
#endif












} // End of namespace GEO_FeE_Adjacency

#endif
