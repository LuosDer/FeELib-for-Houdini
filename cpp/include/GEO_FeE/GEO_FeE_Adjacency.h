
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

        GA_Offset nextVtx = geo->vertexToNextVertex(vtxoff);
        GA_Offset nextVtxDstpt;
        while (nextVtx != GA_INVALID_OFFSET)
        {
            nextVtxDstpt = vertexPointDst(geo, nextVtx);
            if (nextVtxDstpt == dstpt)
                return nextVtx;
            nextVtx = geo->vertexToNextVertex(nextVtx);
        }

        GA_Offset ptnum = geo->vertexPoint(vtxoff);
        nextVtx = geo->pointVertex(dstpt);
        while (nextVtx != GA_INVALID_OFFSET)
        {
            nextVtxDstpt = vertexPointDst(geo, nextVtx);
            if (nextVtxDstpt == ptnum)
            {
                if (dstpt < ptnum)
                    return -1;
                return nextVtx;
            }
            nextVtx = geo->vertexToNextVertex(nextVtx);
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

        GA_Offset vtxoff = geo->pointVertex(ptoff);
        GA_Offset primoff;
        GA_Offset pt_next;
        GA_Size vtxpnum;
        GA_Size vtxpnum_next;
        GA_Size numvtx;
        while (vtxoff != GA_INVALID_OFFSET)
        {
            primoff = geo->vertexPrimitive(vtxoff);
            numvtx = geo->getPrimitiveVertexCount(primoff);
            vtxpnum = vertexPrimIndex(geo, primoff, vtxoff);

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
            vtxoff = geo->vertexToNextVertex(vtxoff);
        }
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
        UT_AutoInterrupt boss("primPrimEdge");

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

        attribHandle->bumpDataId();
    }




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

    attribHandle->bumpDataId();

}

#if 1
    //Get all vertices NextEquiv Vertex
    template<typename T>
    static void
        vertexPrimIndex(
        const GA_Detail* geo,
        const GA_RWHandleT<T>& attribHandle,
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
        attribHandle->bumpDataId();
    }
#else
    //Get all vertices NextEquiv Vertex
    template<typename T>
    static void
        vertexPrimIndex(
            const GA_Detail* geo,
            const GA_RWHandleT<T>& attribHandle,
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

        attribHandle->bumpDataId();
    }
#endif

    template<typename T>
    //Get all vertices NextEquiv Vertex
    static void
        vertexPointDst(
            const GA_Detail* geo,
            const GA_RWHandleT<T>& attribHandle,
            const GA_VertexGroup* geoGroup = nullptr,
            const bool& outAsOffset = true,
            const exint& subscribeRatio = 16,
            const exint& minGrainSize = 1024
        )
    {
        UT_AutoInterrupt boss("vertexPointDst");

        const GA_SplittableRange geo0SplittableRange0(geo->getVertexRange(geoGroup));
        UTparallelFor(geo0SplittableRange0, [&geo, &attribHandle, &outAsOffset, &boss](const GA_SplittableRange& r)
            {
                if (boss.wasInterrupted())
                    return;
                GA_Offset start;
                GA_Offset end;
                for (GA_Iterator it(r); it.blockAdvance(start, end); )
                {
                    for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                    {
                        if (outAsOffset)
                        {
                            attribHandle.set(elemoff, vertexPointDst(geo, elemoff));
                        }
                        else
                        {
                            GA_Offset ptoff = vertexPointDst(geo, elemoff);
                            if (ptoff == -1)
                            {
                                attribHandle.set(elemoff, -1);
                            }
                            else
                            {
                                attribHandle.set(elemoff, geo->pointIndex(ptoff));
                            }
                        }
                    }
                }
            }, subscribeRatio, minGrainSize);

        attribHandle->bumpDataId();

    }

    template<typename T>
    //Get all vertices NextEquiv Vertex
    static void
        vertexPointDst(
            const GA_Detail* geo,
            const GA_RWHandleT<T>& attribHandle,
            const GA_ROHandleT<T>& vtxpnumAttribHandle,
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

        attribHandle->bumpDataId();

    }






    //Get all vertices NextEquiv Vertex
    template<typename T>
    static void
        vertexVertexNextEquiv(
            const GA_Detail* geo,
            const GA_RWHandleT<T>& attribHandle,
            const GA_VertexGroup* geoGroup = nullptr,
            const bool& outAsOffset = true,
            const exint& subscribeRatio = 16,
            const exint& minGrainSize = 1024
        )
    {
        UT_AutoInterrupt boss("vertexVertexNextEquiv");

        const GA_SplittableRange geo0SplittableRange0(geo->getVertexRange(geoGroup));
        UTparallelFor(geo0SplittableRange0, [&geo, &attribHandle, &outAsOffset, &boss](const GA_SplittableRange& r)
            {
                if (boss.wasInterrupted())
                    return;
                GA_Offset start;
                GA_Offset end;
                for (GA_Iterator it(r); it.blockAdvance(start, end); )
                {
                    for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                    {
                        if (outAsOffset)
                        {
                            attribHandle.set(elemoff, vertexVertexNextEquiv(geo, elemoff));
                        }
                        else
                        {
                            GA_Offset vtxoff = vertexVertexNextEquiv(geo, elemoff);
                            if (vtxoff == -1)
                            {
                                attribHandle.set(elemoff, -1);
                            }
                            else
                            {
                                attribHandle.set(elemoff, geo->vertexIndex(vtxoff));
                            }
                        }
                    }
                }
            }, subscribeRatio, minGrainSize);
        attribHandle->bumpDataId();
    }




    //Get all vertices NextEquiv Vertex
    static void
        vertexVertexNextEquiv(
            const GA_Detail* geo,
            const GA_RWHandleT<GA_Offset>& attribHandle,
            const GA_ROHandleT<GA_Offset>& dstptAttribHandle,
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


                        GA_Offset nextVtx = geo->vertexToNextVertex(elemoff);
                        while (nextVtx != GA_INVALID_OFFSET)
                        {
                            if (dstptAttribHandle.get(nextVtx) == dstpt)
                            {
                                dstpt = -1;
                                break;
                            }
                            nextVtx = geo->vertexToNextVertex(nextVtx);
                        }

                        if (dstpt < 0)
                        {
                            attribHandle.set(elemoff, -1);
                            continue;
                        }

                        nextVtx = geo->pointVertex(dstpt);
                        GA_Offset ptnum = geo->vertexPoint(elemoff);
                        while (nextVtx != GA_INVALID_OFFSET)
                        {
                            if (dstptAttribHandle.get(nextVtx) == ptnum)
                            {
                                if (dstpt < ptnum)
                                {
                                    break;
                                }
                                attribHandle.set(elemoff, nextVtx);
                            }
                            nextVtx = geo->vertexToNextVertex(nextVtx);
                        }
                        attribHandle.set(elemoff, -1);
                    }
                }
            }, subscribeRatio, minGrainSize);

        attribHandle->bumpDataId();
    }



    //Get all vertices NextEquiv Vertex
    static void
        vertexVertexNextEquiv(
            const GA_Detail* geo,
            const GA_RWHandleT<GA_Offset>& attribHandle,
            const GA_ROHandleT<GA_Offset>& dstptAttribHandle,
            GA_VertexGroup* validGroup,
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


                    GA_Offset nextVtx = geo->vertexToNextVertex(elemoff);
                    while (nextVtx != GA_INVALID_OFFSET)
                    {
                        if (dstptAttribHandle.get(nextVtx) == dstpt)
                        {
                            dstpt = -1;
                            break;
                        }
                        nextVtx = geo->vertexToNextVertex(nextVtx);
                    }

                    if (dstpt < 0)
                    {
                        attribHandle.set(elemoff, -1);
                        validGroup->setElement(elemoff, true);
                        continue;
                    }

                    nextVtx = geo->pointVertex(dstpt);
                    GA_Offset ptnum = geo->vertexPoint(elemoff);
                    while (nextVtx != GA_INVALID_OFFSET)
                    {
                        if (dstptAttribHandle.get(nextVtx) == ptnum)
                        {
                            if (dstpt < ptnum)
                            {
                                break;
                            }
                            attribHandle.set(elemoff, nextVtx);
                        }
                        nextVtx = geo->vertexToNextVertex(nextVtx);
                    }
                    attribHandle.set(elemoff, -1);
                    validGroup->setElement(elemoff, true);
                }
            }
        }, subscribeRatio, minGrainSize);

        attribHandle->bumpDataId();
        validGroup->invalidateGroupEntries();
    }





} // End of namespace GEO_FeE_Adjacency

#endif
