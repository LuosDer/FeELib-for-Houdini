
#pragma once

#ifndef __GA_FeE_Adjacency_h__
#define __GA_FeE_Adjacency_h__

//#include <GA_FeE/GA_FeE_Adjacency.h>

#include <GEO/GEO_Detail.h>

#include <GA_FeE/GA_FeE_VertexNextEquiv.h>

namespace GA_FeE_Adjacency {








    //Get Vertex Destination Point
    static void
        pointPointEdge(
            const GA_Detail* geo,
            const GA_Offset ptoff,
            UT_ValArray<GA_Offset>& ptoffArray
        )
    {
        //exint cap = 0;
        ptoffArray.clear();

        GA_Offset pt_next;
        for (GA_Offset vtxoff_next = geo->pointVertex(ptoff); vtxoff_next != GA_INVALID_OFFSET; vtxoff_next = geo->vertexToNextVertex(vtxoff_next))
        {
            const GA_Offset primoff = geo->vertexPrimitive(vtxoff_next);
            const GA_Size numvtx = geo->getPrimitiveVertexCount(primoff);
            const GA_Size vtxpnum = GA_FeE_TopologyReference::vertexPrimIndex(geo, primoff, vtxoff_next);

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

            const GA_Size vtxpnum_next = vtxpnum + 1;
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
            const GA_ROHandleT<GA_Size>& vtxpnumAttribHandle,
            const GA_PointGroup* geoGroup = nullptr,
            const GA_PointGroup* seamGroup = nullptr,
            const exint subscribeRatio = 64,
            const exint minGrainSize = 64
        )
    {
        const GA_SplittableRange geo0SplittableRange0(geo->getPointRange(geoGroup));
        UTparallelFor(geo0SplittableRange0, [&geo, &attribHandle, &vtxpnumAttribHandle, &seamGroup](const GA_SplittableRange& r)
        {
            UT_ValArray<GA_Offset> ptoffArray;
            GA_Offset start, end;
            for (GA_Iterator it(r); it.blockAdvance(start, end); )
            {
                for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                {
                    ptoffArray.clear();

                    GA_Offset pt_next;
                    for (GA_Offset vtxoff_next = geo->pointVertex(elemoff); vtxoff_next != GA_INVALID_OFFSET; vtxoff_next = geo->vertexToNextVertex(vtxoff_next))
                    {
                        const GA_Offset primoff = geo->vertexPrimitive(vtxoff_next);
                        const GA_Size numvtx = geo->getPrimitiveVertexCount(primoff);
                        const GA_Size vtxpnum = vtxpnumAttribHandle.get(vtxoff_next);

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

                        const GA_Size vtxpnum_next = vtxpnum + 1;
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
                    attribHandle.set(elemoff, ptoffArray);
                }
            }
        }, subscribeRatio, minGrainSize);
    }


    //Get all prims neighbours prims with adjacent by edge
    static void
    pointPointEdge(
        const GA_Detail* geo,
        const GA_RWHandleT<UT_ValArray<GA_Offset>>& attribHandle,
        const GA_PointGroup* geoGroup = nullptr,
        const GA_PointGroup* seamGroup = nullptr,
        const exint subscribeRatio = 128,
        const exint minGrainSize = 64
    )
    {
        const GA_SplittableRange geo0SplittableRange0(geo->getPointRange(geoGroup));
        UTparallelFor(geo0SplittableRange0, [&geo, &attribHandle, &seamGroup](const GA_SplittableRange& r)
        {
            UT_ValArray<GA_Offset> adjElems;
            GA_Offset start, end;
            for (GA_Iterator it(r); it.blockAdvance(start, end); )
            {
                for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                {
                    pointPointEdge(geo, elemoff, adjElems);
                    attribHandle.set(elemoff, adjElems);
                }
            }
        }, subscribeRatio, minGrainSize);
    }



    //Get Vertex Destination Point
    static void
    pointPointEdge(
        GA_Detail* geo,
        const GA_RWHandleT<UT_ValArray<GA_Offset>>& attribHandle,
        const GA_ROHandleT<GA_Offset>& vtxPrevH,
        const GA_ROHandleT<GA_Offset>& vtxNextH,
        const GA_PointGroup* geoGroup = nullptr,
        const GA_PointGroup* seamGroup = nullptr,
        const exint subscribeRatio = 128,
        const exint minGrainSize = 64
    )
    {
        GA_Topology& topo = geo->getTopology();
        topo.makeFull();
        //topo.makeVertexRef();
        const GA_ATITopology* vtxPointRef = topo.getPointRef();
        const GA_ATITopology* pointVtxRef = topo.getVertexRef();
        //const GA_ATITopology* vtxPrevRef = topo.getVertexPrevRef();
        const GA_ATITopology* vtxNextRef = topo.getVertexNextRef();
        const GA_SplittableRange geo0SplittableRange0(geo->getPointRange(geoGroup));

        UTparallelFor(geo0SplittableRange0, [&geo, &attribHandle, &vtxPrevH, &vtxNextH, &pointVtxRef, &vtxNextRef, &vtxPointRef, &seamGroup](const GA_SplittableRange& r)
        {
            UT_ValArray<GA_Offset> ptoffArray(32);
            GA_Offset start, end;
            for (GA_Iterator it(r); it.blockAdvance(start, end); )
            {
                for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                {
                    ptoffArray.clear();
                    for (GA_Offset vtxoff_next = pointVtxRef->getLink(elemoff); vtxoff_next != GA_INVALID_OFFSET; vtxoff_next = vtxNextRef->getLink(vtxoff_next))
                    {
                        const GA_Offset vtxPrev = vtxPrevH.get(vtxoff_next);
                        if (vtxPrev != GA_INVALID_OFFSET)
                        {
                            const GA_Offset pt_next = vtxPointRef->getLink(vtxPrev);
                            //ptoffArray.uniqueSortedInsert(pt_next);
                            if (ptoffArray.uniqueSortedFindAscending(pt_next) == -1)
                            {
                                ptoffArray.emplace_back(pt_next);
                                ptoffArray.sort();
                            }
                        }
                        const GA_Offset vtxNext = vtxNextH.get(vtxoff_next);
                        if (vtxNext != GA_INVALID_OFFSET)
                        {
                            const GA_Offset pt_next = vtxPointRef->getLink(vtxNext);
                            //ptoffArray.uniqueSortedInsert(pt_next);
                            if (ptoffArray.uniqueSortedFindAscending(pt_next) == -1)
                            {
                                ptoffArray.emplace_back(pt_next);
                                ptoffArray.sort();
                            }
                        }
                    }
                    attribHandle.set(elemoff, ptoffArray);
                }
            }
        }, subscribeRatio, minGrainSize);
    }




    //Get all prims neighbours prims with adjacent by edge
    static void
        primPrimEdge(
            GA_Detail* geo,
            const GA_RWHandleT<UT_ValArray<GA_Offset>>& attribHandle,
            const GA_ROHandleT<GA_Offset>& nextEquivAttribHandle,
            const GA_PrimitiveGroup* geoGroup = nullptr,
            const GA_VertexGroup* seamGroup = nullptr,
            const exint subscribeRatio = 128,
            const exint minGrainSize = 64
        )
    {
        GA_Topology& topo = geo->getTopology();
        topo.makePrimitiveRef();
        const GA_ATITopology* vtxPrimRef = topo.getPrimitiveRef();

        const GA_SplittableRange geo0SplittableRange0(geo->getPrimitiveRange(geoGroup));
        UTparallelFor(geo0SplittableRange0, [&geo, &attribHandle, &nextEquivAttribHandle, &seamGroup, &vtxPrimRef](const GA_SplittableRange& r)
        {
            UT_ValArray<GA_Offset> adjElems;
            GA_Offset vtxoff_next;
            GA_Offset start, end;
            if (seamGroup)
            {
                for (GA_Iterator it(r); it.blockAdvance(start, end); )
                {
                    for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                    {
                        adjElems.clear();
                        const GA_OffsetListRef& vertices = geo->getPrimitiveVertexList(elemoff);
                        const GA_Size numvtx = vertices.size();
                        //for (GA_Offset vtxoff : vertices)
                        for (GA_Size i = 0; i < numvtx; ++i)
                        {
                            const GA_Offset vtxoff_start = vertices[i];
                            if (!seamGroup->contains(vtxoff_start))
                                continue;
                            GA_Offset vtxoff_next = nextEquivAttribHandle.get(vtxoff_start);
                            if (vtxoff_next < 0)
                                continue;
                            for (; vtxoff_next != vtxoff_start; vtxoff_next = nextEquivAttribHandle.get(vtxoff_next))
                            {
                                if (adjElems.find(vtxPrimRef->getLink(vtxoff_next)) != -1)
                                    continue;
                                adjElems.emplace_back(vtxPrimRef->getLink(vtxoff_next));
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
                        adjElems.clear();
                        const GA_OffsetListRef& vertices = geo->getPrimitiveVertexList(elemoff);
                        const GA_Size numvtx = vertices.size();
                        //for (GA_Offset vtxoff : vertices)
                        for (GA_Size i = 0; i < numvtx; ++i)
                        {
                            const GA_Offset vtxoff_start = vertices[i];
                            GA_Offset vtxoff_next = nextEquivAttribHandle.get(vtxoff_start);
                            if (vtxoff_next < 0)
                                continue;
                            for (; vtxoff_next != vtxoff_start; vtxoff_next = nextEquivAttribHandle.get(vtxoff_next))
                            {
                                if (adjElems.find(vtxPrimRef->getLink(vtxoff_next)) != -1)
                                    continue;
                                adjElems.emplace_back(vtxPrimRef->getLink(vtxoff_next));
                            }
                        }
                        attribHandle.set(elemoff, adjElems);
                    }
                }
            }
        }, subscribeRatio, minGrainSize);
    }



    //Get all prims neighbours prims with adjacent by edge
    static void
        primPrimEdge1(
            const GA_Detail* geo,
            const GA_RWHandleT<UT_ValArray<GA_Offset>>& attribHandle,
            const GA_ROHandleT<GA_Offset>& dstptAttribHandle,
            const GA_PrimitiveGroup* geoGroup = nullptr,
            const GA_VertexGroup* seamGroup = nullptr,
            const exint subscribeRatio = 128,
            const exint minGrainSize = 64
        )
    {
        const GA_SplittableRange geoSplittableRange(geo->getPrimitiveRange(geoGroup));
        UTparallelFor(geoSplittableRange, [&geo, &attribHandle, &dstptAttribHandle, &seamGroup](const GA_SplittableRange& r)
        {
            UT_ValArray<GA_Offset> adjElems(32);
            GA_Offset vtxoff_next;
            GA_Offset start, end;
            if (seamGroup)
            {
                for (GA_Iterator it(r); it.blockAdvance(start, end); )
                {
                    for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                    {
                        adjElems.clear();
                        const GA_OffsetListRef& vertices = geo->getPrimitiveVertexList(elemoff);
                        const GA_Size numvtx = vertices.size();
                        for (GA_Size i = 0; i < numvtx; ++i)
                        {
                            const GA_Offset dstpt = dstptAttribHandle.get(vertices[i]);
                            if (dstpt < 0)
                                continue;
                            const GA_Offset ptoff = geo->vertexPoint(vertices[i]);
                            for (vtxoff_next = geo->pointVertex(ptoff); vtxoff_next != GA_INVALID_OFFSET; vtxoff_next = geo->vertexToNextVertex(vtxoff_next))
                            {
                                if (vtxoff_next == vertices[i])
                                    continue;
                                if (dstptAttribHandle.get(vtxoff_next) != dstpt)
                                    continue;
                                if (adjElems.find(geo->vertexPrimitive(vtxoff_next)) != -1)
                                    continue;
                                adjElems.emplace_back(geo->vertexPrimitive(vtxoff_next));
                            }

                            for (vtxoff_next = geo->pointVertex(dstpt); vtxoff_next != GA_INVALID_OFFSET; vtxoff_next = geo->vertexToNextVertex(vtxoff_next))
                            {
                                if (dstptAttribHandle.get(vtxoff_next) != ptoff)
                                    continue;
                                if (adjElems.find(geo->vertexPrimitive(vtxoff_next)) != -1)
                                    continue;
                                adjElems.emplace_back(geo->vertexPrimitive(vtxoff_next));
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
                        adjElems.clear();
                        const GA_OffsetListRef& vertices = geo->getPrimitiveVertexList(elemoff);
                        const GA_Size numvtx = vertices.size();
                        for (GA_Size i = 0; i < numvtx; ++i)
                        {
                            const GA_Offset dstpt = dstptAttribHandle.get(vertices[i]);
                            if (dstpt < 0)
                                continue;
                            const GA_Offset ptoff = geo->vertexPoint(vertices[i]);
                            for (vtxoff_next = geo->pointVertex(ptoff); vtxoff_next != GA_INVALID_OFFSET; vtxoff_next = geo->vertexToNextVertex(vtxoff_next))
                            {
                                if (vtxoff_next == vertices[i])
                                    continue;
                                if (dstptAttribHandle.get(vtxoff_next) != dstpt)
                                    continue;
                                if (adjElems.find(geo->vertexPrimitive(vtxoff_next)) >= 0)
                                    continue;
                                adjElems.emplace_back(geo->vertexPrimitive(vtxoff_next));
                            }

                            for (vtxoff_next = geo->pointVertex(dstpt); vtxoff_next != GA_INVALID_OFFSET; vtxoff_next = geo->vertexToNextVertex(vtxoff_next))
                            {
                                if (dstptAttribHandle.get(vtxoff_next) != ptoff)
                                    continue;
                                if (adjElems.find(geo->vertexPrimitive(vtxoff_next)) >= 0)
                                    continue;
                                adjElems.emplace_back(geo->vertexPrimitive(vtxoff_next));
                            }
                        }
                        attribHandle.set(elemoff, adjElems);
                    }
                }
            }
        }, subscribeRatio, minGrainSize);
    }


    //Get all prims neighbours prims with adjacent by edge
    static void
        primPrimEdge2(
            const GA_Detail* geo,
            const GA_RWHandleT<UT_ValArray<GA_Offset>>& attribHandle,
            const GA_ROHandleT<GA_Offset>& dstptAttribHandle,
            const GA_PrimitiveGroup* geoGroup = nullptr,
            const GA_VertexGroup* seamGroup = nullptr,
            const exint subscribeRatio = 128,
            const exint minGrainSize = 64
        )
    {
        const GA_SplittableRange geo0SplittableRange0(geo->getPrimitiveRange(geoGroup));
        UTparallelFor(geo0SplittableRange0, [&geo, &attribHandle, &dstptAttribHandle, &seamGroup](const GA_SplittableRange& r)
        {
            //UT_SmallArray<GA_Offset> adjElems;
            //UT_ValArray<GA_Offset> adjElems;
            GA_Offset vtxoff_next;
            GA_Offset start, end;
            if (seamGroup)
            {
                for (GA_Iterator it(r); it.blockAdvance(start, end); )
                {
                    for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                    {
                        UT_ValArray<GA_Offset> adjElems;
                        const GA_OffsetListRef& vertices = geo->getPrimitiveVertexList(elemoff);
                        const GA_Size numvtx = vertices.size();
                        for (GA_Size i = 0; i < numvtx; ++i)
                        {
                            const GA_Offset dstpt = dstptAttribHandle.get(vertices[i]);
                            if (dstpt < 0)
                                continue;
                            const GA_Offset ptoff = geo->vertexPoint(vertices[i]);
                            for (vtxoff_next = geo->pointVertex(ptoff); vtxoff_next != GA_INVALID_OFFSET; vtxoff_next = geo->vertexToNextVertex(vtxoff_next))
                            {
                                if (vtxoff_next == vertices[i])
                                    continue;
                                if (dstptAttribHandle.get(vtxoff_next) != dstpt)
                                    continue;
                                if (adjElems.find(geo->vertexPrimitive(vtxoff_next)) != -1)
                                    continue;
                                adjElems.emplace_back(geo->vertexPrimitive(vtxoff_next));
                            }

                            for (vtxoff_next = geo->pointVertex(dstpt); vtxoff_next != GA_INVALID_OFFSET; vtxoff_next = geo->vertexToNextVertex(vtxoff_next))
                            {
                                if (dstptAttribHandle.get(vtxoff_next) != ptoff)
                                    continue;
                                if (adjElems.find(geo->vertexPrimitive(vtxoff_next)) != -1)
                                    continue;
                                adjElems.emplace_back(geo->vertexPrimitive(vtxoff_next));
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
                        UT_ValArray<GA_Offset> adjElems;
                        const GA_OffsetListRef& vertices = geo->getPrimitiveVertexList(elemoff);
                        const GA_Size numvtx = vertices.size();
                        for (GA_Size i = 0; i < numvtx; ++i)
                        {
                            const GA_Offset dstpt = dstptAttribHandle.get(vertices[i]);
                            if (dstpt < 0)
                                continue;
                            const GA_Offset ptoff = geo->vertexPoint(vertices[i]);
                            for (vtxoff_next = geo->pointVertex(ptoff); vtxoff_next != GA_INVALID_OFFSET; vtxoff_next = geo->vertexToNextVertex(vtxoff_next))
                            {
                                if (vtxoff_next == vertices[i])
                                    continue;
                                if (dstptAttribHandle.get(vtxoff_next) != dstpt)
                                    continue;
                                if (adjElems.find(geo->vertexPrimitive(vtxoff_next)) >= 0)
                                    continue;
                                adjElems.emplace_back(geo->vertexPrimitive(vtxoff_next));
                            }

                            for (vtxoff_next = geo->pointVertex(dstpt); vtxoff_next != GA_INVALID_OFFSET; vtxoff_next = geo->vertexToNextVertex(vtxoff_next))
                            {
                                if (dstptAttribHandle.get(vtxoff_next) != ptoff)
                                    continue;
                                if (adjElems.find(geo->vertexPrimitive(vtxoff_next)) >= 0)
                                    continue;
                                adjElems.emplace_back(geo->vertexPrimitive(vtxoff_next));
                            }
                        }
                        attribHandle.set(elemoff, adjElems);
                    }
                }
            }
        }, subscribeRatio, minGrainSize);
    }



    //Get all prims neighbours prims with adjacent by edge
    static void
        primPrimEdge3(
            const GA_Detail* geo,
            const GA_RWHandleT<UT_ValArray<GA_Offset>>& attribHandle,
            const GA_ROHandleT<GA_Offset>& dstptAttribHandle,
            const GA_PrimitiveGroup* geoGroup = nullptr,
            const GA_VertexGroup* seamGroup = nullptr,
            const exint subscribeRatio = 128,
            const exint minGrainSize = 64
        )
    {
        const GA_SplittableRange geo0SplittableRange0(geo->getPrimitiveRange(geoGroup));
        UTparallelFor(geo0SplittableRange0, [&geo, &attribHandle, &dstptAttribHandle, &seamGroup](const GA_SplittableRange& r)
        {
            //UT_SmallArray<GA_Offset> adjElems;
            //UT_ValArray<GA_Offset> adjElems;
            GA_Offset vtxoff_next;
            GA_Offset start, end;
            if (seamGroup)
            {
                for (GA_Iterator it(r); it.blockAdvance(start, end); )
                {
                    for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                    {
                        UT_ValArray<GA_Offset> adjElems;
                        const GA_OffsetListRef& vertices = geo->getPrimitiveVertexList(elemoff);
                        const GA_Size numvtx = vertices.size();
                        for (GA_Size i = 0; i < numvtx; ++i)
                        {
                            const GA_Offset dstpt = dstptAttribHandle.get(vertices[i]);
                            if (dstpt < 0)
                                continue;
                            const GA_Offset ptoff = geo->vertexPoint(vertices[i]);
                            for (vtxoff_next = geo->pointVertex(ptoff); vtxoff_next != GA_INVALID_OFFSET; vtxoff_next = geo->vertexToNextVertex(vtxoff_next))
                            {
                                if (vtxoff_next == vertices[i])
                                    continue;
                                if (dstptAttribHandle.get(vtxoff_next) != dstpt)
                                    continue;
                                if (adjElems.find(geo->vertexPrimitive(vtxoff_next)) != -1)
                                    continue;
                                adjElems.emplace_back(geo->vertexPrimitive(vtxoff_next));
                            }

                            for (vtxoff_next = geo->pointVertex(dstpt); vtxoff_next != GA_INVALID_OFFSET; vtxoff_next = geo->vertexToNextVertex(vtxoff_next))
                            {
                                if (dstptAttribHandle.get(vtxoff_next) != ptoff)
                                    continue;
                                if (adjElems.find(geo->vertexPrimitive(vtxoff_next)) != -1)
                                    continue;
                                adjElems.emplace_back(geo->vertexPrimitive(vtxoff_next));
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
                        UT_ValArray<GA_Offset> adjElems;
                        const GA_OffsetListRef& vertices = geo->getPrimitiveVertexList(elemoff);
                        const GA_Size numvtx = vertices.size();
                        for (GA_Size i = 0; i < numvtx; ++i)
                        {
                            const GA_Offset dstpt = dstptAttribHandle.get(vertices[i]);
                            if (dstpt < 0)
                                continue;
                            const GA_Offset ptoff = geo->vertexPoint(vertices[i]);
                            for (vtxoff_next = geo->pointVertex(ptoff); vtxoff_next != GA_INVALID_OFFSET; vtxoff_next = geo->vertexToNextVertex(vtxoff_next))
                            {
                                if (vtxoff_next == vertices[i])
                                    continue;
                                if (dstptAttribHandle.get(vtxoff_next) != dstpt)
                                    continue;
                                if (adjElems.find(geo->vertexPrimitive(vtxoff_next)) >= 0)
                                    continue;
                                adjElems.emplace_back(geo->vertexPrimitive(vtxoff_next));
                            }

                            for (vtxoff_next = geo->pointVertex(dstpt); vtxoff_next != GA_INVALID_OFFSET; vtxoff_next = geo->vertexToNextVertex(vtxoff_next))
                            {
                                if (dstptAttribHandle.get(vtxoff_next) != ptoff)
                                    continue;
                                if (adjElems.find(geo->vertexPrimitive(vtxoff_next)) >= 0)
                                    continue;
                                adjElems.emplace_back(geo->vertexPrimitive(vtxoff_next));
                            }
                        }
                        attribHandle.set(elemoff, adjElems);
                    }
                }
            }
        }, subscribeRatio, minGrainSize);
    }


    //Get all prims neighbours prims with adjacent by edge
    static void
    primPrimEdge4(
        const GEO_Detail* geo,
        const GA_RWHandleT<UT_ValArray<GA_Offset>>& attribHandle,
        const GA_PrimitiveGroup* geoGroup = nullptr,
        const GA_VertexGroup* seamGroup = nullptr,
        const exint subscribeRatio = 128,
        const exint minGrainSize = 64
    )
    {
        const GA_SplittableRange geo0SplittableRange0(geo->getPrimitiveRange(geoGroup));
        UTparallelFor(geo0SplittableRange0, [&geo, &attribHandle, &seamGroup](const GA_SplittableRange& r)
        {
            GEO_Detail::GEO_EdgeAdjArray adjElemStruct;
            UT_ValArray<GA_Offset> adjElems;
            GA_Offset start, end;
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
                            adjElems[i] = adjElemStruct[i].myAdjacentPolygon;
                        }
                        attribHandle.set(elemoff, adjElems);
                    }
                }
            }
        }, subscribeRatio, minGrainSize);
    }





    //Get all prims neighbours prims with adjacent by edge
    static void
    primPrimPoint(
        GA_Detail* geo,
        const GA_RWHandleT<UT_ValArray<GA_Offset>>& attribHandle,
        const GA_PrimitiveGroup* geoGroup = nullptr,
        const GA_PointGroup* seamGroup = nullptr,
        const exint subscribeRatio = 128,
        const exint minGrainSize = 64
    )
    {
        GA_Topology& topo = geo->getTopology();
        //topo.makeFull();
        //topo.makePrimitiveRef();
        //topo.makeVertexRef();
        const GA_ATITopology* vtxPrimRef = topo.getPrimitiveRef();
        const GA_ATITopology* vtxPointRef = topo.getPointRef();
        const GA_ATITopology* pointVtxRef = topo.getVertexRef();
        const GA_ATITopology* vtxNextRef = topo.getVertexNextRef();


        const GA_SplittableRange geo0SplittableRange0(geo->getPrimitiveRange(geoGroup));
        UTparallelFor(geo0SplittableRange0, [&geo, &attribHandle, &seamGroup, &vtxNextRef, &vtxPointRef, &pointVtxRef, &vtxPrimRef](const GA_SplittableRange& r)
        {
            UT_ValArray<GA_Offset> adjElems(64);
            GA_Offset vtxoff_next;
            GA_Offset start, end;
            if (seamGroup)
            {
                for (GA_Iterator it(r); it.blockAdvance(start, end); )
                {
                    for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                    {
                        adjElems.clear();
                        const GA_OffsetListRef& vertices = geo->getPrimitiveVertexList(elemoff);
                        const GA_Size numvtx = vertices.size();
                        for (GA_Size i = 0; i < numvtx; ++i)
                        {
                            const GA_Offset ptoff = vtxPointRef->getLink(vertices[i]);
                            if (seamGroup->contains(ptoff))
                                continue;
                            for (vtxoff_next = pointVtxRef->getLink(ptoff); vtxoff_next != GA_INVALID_OFFSET; vtxoff_next = vtxNextRef->getLink(vtxoff_next))
                            {
                                if (vtxoff_next == vertices[i])
                                    continue;
                                if (adjElems.find(vtxPrimRef->getLink(vtxoff_next)) != -1)
                                    continue;
                                adjElems.emplace_back(vtxPrimRef->getLink(vtxoff_next));
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
                        adjElems.clear();
                        const GA_OffsetListRef& vertices = geo->getPrimitiveVertexList(elemoff);
                        const GA_Size numvtx = vertices.size();
                        for (GA_Size i = 0; i < numvtx; ++i)
                        {
                            const GA_Offset ptoff = vtxPointRef->getLink(vertices[i]);
                            for (vtxoff_next = pointVtxRef->getLink(ptoff); vtxoff_next != GA_INVALID_OFFSET; vtxoff_next = vtxNextRef->getLink(vtxoff_next))
                            {
                                if (vtxoff_next == vertices[i])
                                    continue;
                                if (adjElems.find(vtxPrimRef->getLink(vtxoff_next)) != -1)
                                    continue;
                                adjElems.emplace_back(vtxPrimRef->getLink(vtxoff_next));
                            }
                        }
                        attribHandle.set(elemoff, adjElems);
                    }
                }
            }
        }, subscribeRatio, minGrainSize);
    }

















    //GA_FeE_Adjacency::addAttribPointPointEdge(geo, name, geoGroup, seamGroup, storage, nullptr, nullptr, GA_ReuseStrategy(), subscribeRatio, minGrainSize);
    static GA_Attribute*
        addAttribPointPointEdge(
            GEO_Detail* geo,
            const GA_PointGroup* geoGroup = nullptr,
            const GA_PointGroup* seamGroup = nullptr,
            const GA_Storage storage = GA_STORE_INT32,
            const UT_StringHolder& name = "__topo_nebs",
            const UT_Options* creation_args = nullptr,
            const GA_AttributeOptions* attribute_options = nullptr,
            const GA_ReuseStrategy& reuse = GA_ReuseStrategy(),
            const exint subscribeRatio = 128,
            const exint minGrainSize = 64
        )
    {
        GA_Attribute* attribPtr = geo->findPointAttribute(GA_FEE_TOPO_SCOPE, name);
        if (attribPtr)
            return attribPtr;
        GA_Attribute* vtxPrevAttrib = nullptr;
        GA_Attribute* vtxNextAttrib = nullptr;
        GA_FeE_TopologyReference::addAttribVertexVertexPrim(geo, vtxPrevAttrib, vtxNextAttrib, nullptr, storage);

        //attribPtr = geo->addIntTuple(GA_ATTRIB_POINT, name, 1, defaults, creation_args, attribute_options, storage, reuse);
        attribPtr = geo->addIntArray(GA_ATTRIB_POINT, GA_FEE_TOPO_SCOPE, name, 1, creation_args, attribute_options, storage, reuse);

        pointPointEdge(geo, attribPtr, vtxPrevAttrib, vtxNextAttrib, geoGroup, seamGroup, subscribeRatio, minGrainSize);
        //UT_ASSERT_P(vtxPrevAttrib);
        //UT_ASSERT_P(vtxNextAttrib);
        //GA_ROHandleT<GA_Offset> vtxPrevH(vtxPrevAttrib);
        //GA_ROHandleT<GA_Offset> vtxNextH(vtxNextAttrib);
        //pointPointEdge(geo, attribPtr, vtxPrevH, vtxNextH, geoGroup, seamGroup, subscribeRatio, minGrainSize);
        return attribPtr;
    }





    //GA_FeE_Adjacency::addAttribPrimPrimEdge(geo, name, geoGroup, seamGroup, storage, nullptr, nullptr, GA_ReuseStrategy(), subscribeRatio, minGrainSize);
    
    static GA_Attribute*
        addAttribPrimPrimEdge(
            GEO_Detail* geo,
            const GA_PrimitiveGroup* geoGroup = nullptr,
            const GA_VertexGroup* seamGroup = nullptr,
            const GA_Storage storage = GA_STORE_INT32,
            const UT_StringHolder& name = "__topo_nebs",
            const UT_Options* creation_args = nullptr,
            const GA_AttributeOptions* attribute_options = nullptr,
            const GA_ReuseStrategy& reuse = GA_ReuseStrategy(),
            const exint subscribeRatio = 128,
            const exint minGrainSize = 64
        )
    {
        GA_Attribute* attribPtr = geo->findPrimitiveAttribute(GA_FEE_TOPO_SCOPE, name);
        if (attribPtr)
            return attribPtr;

        //GA_Attribute* dstptAttrib = GA_FeE_TopologyReference::addAttribVertexPointDst(geo, "dstpt", nullptr, GA_Defaults(-1), storage, subscribeRatio, minGrainSize);
        //GA_RWHandleT<GA_Offset> dstptAttribHandle(dstptAttrib);
        //GA_Attribute* vtxPrimNextAttrib = addAttribVertexVertexPrimNext(geo, "vtxPrimNext", nullptr, GA_Defaults(-1), storage, subscribeRatio, minGrainSize);

        GA_Attribute* vtxNextEquivAttrib = GA_FeE_VertexNextEquiv::addAttribVertexNextEquiv(geo, nullptr, storage);

        attribPtr = geo->addIntArray(GA_ATTRIB_PRIMITIVE, GA_FEE_TOPO_SCOPE, name, 1, creation_args, attribute_options, storage, reuse);
        //GA_ROHandleT<UT_ValArray<GA_Offset>> attribHandle(attribPtr);
        primPrimEdge(geo, attribPtr, vtxNextEquivAttrib, geoGroup, seamGroup, subscribeRatio, minGrainSize);
        return attribPtr;
    }




    //GA_FeE_Adjacency::addAttribAdjacency(geo, groupType, connectivityGroupType);

    static GA_Attribute*
        addAttribAdjacency(
            GEO_Detail* geo,
            const GA_GroupType groupType,
            const GA_GroupType connectivityGroupType,
            const GA_Group* geoGroup = nullptr,
            const GA_Group* seamGroup = nullptr,
            const GA_Storage storage = GA_STORE_INT32,
            const UT_StringHolder& name = "__topo_nebs",
            const UT_Options* creation_args = nullptr,
            const GA_AttributeOptions* attribute_options = nullptr,
            const GA_ReuseStrategy& reuse = GA_ReuseStrategy(),
            const exint subscribeRatio = 128,
            const exint minGrainSize = 64
        )
    {
        switch (groupType)
        {
        case GA_GROUP_PRIMITIVE:
            switch (connectivityGroupType)
            {
            case GA_GROUP_PRIMITIVE:
                //return GA_FeE_Adjacency::addAttribPrimPrimPrim(geo,
                //    UTverify_cast<const GA_PrimitiveGroup*>(geoGroup),
                //    UTverify_cast<const GA_PrimitiveGroup*>(seamGroup),
                //    storage, name, creation_args, attribute_options, reuse, subscribeRatio, minGrainSize);
                break;
            case GA_GROUP_POINT:
                //return GA_FeE_Adjacency::addAttribPrimPrimPoint(geo,
                //    UTverify_cast<const GA_PrimitiveGroup*>(geoGroup),
                //    UTverify_cast<const GA_PointGroup*>(seamGroup),
                //    storage, name, creation_args, attribute_options, reuse, subscribeRatio, minGrainSize);
                break;
            case GA_GROUP_VERTEX:
                //return GA_FeE_Adjacency::addAttribPrimPrimVertex(geo,
                //    UTverify_cast<const GA_PrimitiveGroup*>(geoGroup),
                //    UTverify_cast<const GA_VertexGroup*>(seamGroup),
                //    storage, name, creation_args, attribute_options, reuse, subscribeRatio, minGrainSize);
                break;
            case GA_GROUP_EDGE:
                return GA_FeE_Adjacency::addAttribPrimPrimEdge(geo,
                    UTverify_cast<const GA_PrimitiveGroup*>(geoGroup),
                    UTverify_cast<const GA_VertexGroup*>(seamGroup),
                    storage, name, creation_args, attribute_options, reuse, subscribeRatio, minGrainSize);
                break;
            }

            break;
        case GA_GROUP_POINT:
            switch (connectivityGroupType)
            {
            case GA_GROUP_PRIMITIVE:
                //return GA_FeE_Adjacency::addAttribPointPointPrim(geo,
                //    UTverify_cast<const GA_PointGroup*>(geoGroup),
                //    UTverify_cast<const GA_PrimitiveGroup*>(seamGroup),
                //    storage, name, creation_args, attribute_options, reuse, subscribeRatio, minGrainSize);
                break;
            case GA_GROUP_POINT:
                //return GA_FeE_Adjacency::addAttribPointPointPoint(geo,
                //    UTverify_cast<const GA_PointGroup*>(geoGroup),
                //    UTverify_cast<const GA_PointGroup*>(seamGroup),
                //    storage, name, creation_args, attribute_options, reuse, subscribeRatio, minGrainSize);
                break;
            case GA_GROUP_VERTEX:
                //return GA_FeE_Adjacency::addAttribPointPointVertex(geo,
                //    UTverify_cast<const GA_PointGroup*>(geoGroup),
                //    UTverify_cast<const GA_VertexGroup*>(seamGroup),
                //    storage, name, creation_args, attribute_options, reuse, subscribeRatio, minGrainSize);
                break;
            case GA_GROUP_EDGE:
                return GA_FeE_Adjacency::addAttribPointPointEdge(geo,
                    UTverify_cast<const GA_PointGroup*>(geoGroup),
                    UTverify_cast<const GA_PointGroup*>(seamGroup),
                    storage, name, creation_args, attribute_options, reuse, subscribeRatio, minGrainSize);
                break;
            }

            break;
        case GA_GROUP_VERTEX:
            switch (connectivityGroupType)
            {
            case GA_GROUP_PRIMITIVE:
                //return GA_FeE_Adjacency::addAttribVertexVertexPrim(geo,
                //    UTverify_cast<const GA_VertexGroup*>(geoGroup),
                //    UTverify_cast<const GA_PrimitiveGroup*>(seamGroup),
                //    storage, name, creation_args, attribute_options, reuse, subscribeRatio, minGrainSize);
                break;
            case GA_GROUP_POINT:
                //return GA_FeE_Adjacency::addAttribVertexVertexPoint(geo,
                //    UTverify_cast<const GA_VertexGroup*>(geoGroup),
                //    UTverify_cast<const GA_PointGroup*>(seamGroup),
                //    storage, name, creation_args, attribute_options, reuse, subscribeRatio, minGrainSize);
                break;
            case GA_GROUP_VERTEX:
                //return GA_FeE_Adjacency::addAttribVertexVertexVertex(geo,
                //    UTverify_cast<const GA_VertexGroup*>(geoGroup),
                //    UTverify_cast<const GA_VertexGroup*>(seamGroup),
                //    storage, name, creation_args, attribute_options, reuse, subscribeRatio, minGrainSize);
                break;
            case GA_GROUP_EDGE:
                //return GA_FeE_Adjacency::addAttribVertexVertexEdge(geo,
                //    UTverify_cast<const GA_VertexGroup*>(geoGroup),
                //    UTverify_cast<const GA_PointGroup*>(seamGroup),
                //    storage, name, creation_args, attribute_options, reuse, subscribeRatio, minGrainSize);
                break;
            }
            break;
        }
        return nullptr;
    }

    




} // End of namespace GA_FeE_Adjacency

#endif
