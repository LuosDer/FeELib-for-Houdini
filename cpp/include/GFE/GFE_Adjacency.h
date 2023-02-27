
#pragma once

#ifndef __GFE_Adjacency_h__
#define __GFE_Adjacency_h__

//#include "GFE/GFE_Adjacency.h"

#include "GA/GA_Detail.h"

#include "GFE/GFE_VertexNextEquiv.h"

namespace GFE_Adjacency {








    //Get Vertex Destination Point
    static void
        pointPointEdge(
            const GA_Detail* const geo,
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
            const GA_Size vtxpnum = GFE_TopologyReference::vertexPrimIndex(geo, primoff, vtxoff_next);

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
            const GA_Detail* const geo,
            const GA_RWHandleT<UT_ValArray<GA_Offset>>& attrib_h,
            const GA_ROHandleT<GA_Size>& vtxpnumAttrib_h,
            const GA_PointGroup* const geoGroup = nullptr,
            const GA_PointGroup* const seamGroup = nullptr,
            const exint subscribeRatio = 64,
            const exint minGrainSize = 64
        )
    {
        const GA_SplittableRange geo0SplittableRange0(geo->getPointRange(geoGroup));
        UTparallelFor(geo0SplittableRange0, [geo, &attrib_h, &vtxpnumAttrib_h, seamGroup](const GA_SplittableRange& r)
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
                        const GA_Size vtxpnum = vtxpnumAttrib_h.get(vtxoff_next);

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
                    attrib_h.set(elemoff, ptoffArray);
                }
            }
        }, subscribeRatio, minGrainSize);
    }


    //Get all prims neighbours prims with adjacent by edge
    static void
    pointPointEdge(
        const GA_Detail* const geo,
        const GA_RWHandleT<UT_ValArray<GA_Offset>>& attrib_h,
        const GA_PointGroup* const geoGroup = nullptr,
        const GA_PointGroup* const seamGroup = nullptr,
        const exint subscribeRatio = 128,
        const exint minGrainSize = 64
    )
    {

        const GA_SplittableRange geo0SplittableRange0(geo->getPointRange(geoGroup));
        UTparallelFor(geo0SplittableRange0, [geo, &attrib_h, seamGroup](const GA_SplittableRange& r)
        {
            UT_ValArray<GA_Offset> adjElems;
            GA_Offset start, end;
            for (GA_Iterator it(r); it.blockAdvance(start, end); )
            {
                for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                {
                    pointPointEdge(geo, elemoff, adjElems);
                    attrib_h.set(elemoff, adjElems);
                }
            }
        }, subscribeRatio, minGrainSize);
    }



    //Get Vertex Destination Point
    static void
    pointPointEdge(
        GA_Detail* const geo,
        const GA_RWHandleT<UT_ValArray<GA_Offset>>& attrib_h,
        const GA_ROHandleT<GA_Offset>& vtxPrev_h,
        const GA_ROHandleT<GA_Offset>& vtxNext_h,
        const GA_PointGroup* const geoGroup = nullptr,
        const GA_PointGroup* const seamGroup = nullptr,
        const exint subscribeRatio = 128,
        const exint minGrainSize = 64
    )
    {
        GA_Topology& topo = geo->getTopology();
        topo.makeFull();
        //topo.makeVertexRef();
        const GA_ATITopology* const vtxPointRef = topo.getPointRef();
        const GA_ATITopology* const pointVtxRef = topo.getVertexRef();
        //const GA_ATITopology* const vtxPrevRef = topo.getVertexPrevRef();
        const GA_ATITopology* const vtxNextRef = topo.getVertexNextRef();
        const GA_SplittableRange geo0SplittableRange0(geo->getPointRange(geoGroup));

        UTparallelFor(geo0SplittableRange0, [geo, &attrib_h, &vtxPrev_h, &vtxNext_h, pointVtxRef, vtxNextRef, vtxPointRef, seamGroup](const GA_SplittableRange& r)
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
                        const GA_Offset vtxPrev = vtxPrev_h.get(vtxoff_next);
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
                        const GA_Offset vtxNext = vtxNext_h.get(vtxoff_next);
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
                    attrib_h.set(elemoff, ptoffArray);
                }
            }
        }, subscribeRatio, minGrainSize);
    }




    //Get all prims neighbours prims with adjacent by edge
    static void
        primPrimEdge(
            GA_Detail* const geo,
            const GA_RWHandleT<UT_ValArray<GA_Offset>>& attrib_h,
            const GA_ROHandleT<GA_Offset>& nextEquivAttrib_h,
            const GA_PrimitiveGroup* const geoGroup = nullptr,
            const GA_VertexGroup* const seamGroup = nullptr,
            const exint subscribeRatio = 128,
            const exint minGrainSize = 64
        )
    {
        GA_Topology& topo = geo->getTopology();
        topo.makePrimitiveRef();
        const GA_ATITopology* const vtxPrimRef = topo.getPrimitiveRef();

        const GA_SplittableRange geo0SplittableRange0(geo->getPrimitiveRange(geoGroup));
        UTparallelFor(geo0SplittableRange0, [geo, &attrib_h, &nextEquivAttrib_h, seamGroup, vtxPrimRef](const GA_SplittableRange& r)
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
                            GA_Offset vtxoff_next = nextEquivAttrib_h.get(vtxoff_start);
                            if (vtxoff_next < 0)
                                continue;
                            for (; vtxoff_next != vtxoff_start; vtxoff_next = nextEquivAttrib_h.get(vtxoff_next))
                            {
                                if (adjElems.find(vtxPrimRef->getLink(vtxoff_next)) != -1)
                                    continue;
                                adjElems.emplace_back(vtxPrimRef->getLink(vtxoff_next));
                            }
                        }
                        attrib_h.set(elemoff, adjElems);
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
                            GA_Offset vtxoff_next = nextEquivAttrib_h.get(vtxoff_start);
                            if (vtxoff_next < 0)
                                continue;
                            for (; vtxoff_next != vtxoff_start; vtxoff_next = nextEquivAttrib_h.get(vtxoff_next))
                            {
                                if (adjElems.find(vtxPrimRef->getLink(vtxoff_next)) != -1)
                                    continue;
                                adjElems.emplace_back(vtxPrimRef->getLink(vtxoff_next));
                            }
                        }
                        attrib_h.set(elemoff, adjElems);
                    }
                }
            }
        }, subscribeRatio, minGrainSize);
    }



    //Get all prims neighbours prims with adjacent by edge
    static void
        primPrimEdge1(
            const GA_Detail* const geo,
            const GA_RWHandleT<UT_ValArray<GA_Offset>>& attrib_h,
            const GA_ROHandleT<GA_Offset>& dstptAttrib_h,
            const GA_PrimitiveGroup* const geoGroup = nullptr,
            const GA_VertexGroup*    const seamGroup = nullptr,
            const exint subscribeRatio = 128,
            const exint minGrainSize = 64
        )
    {
        const GA_SplittableRange geoSplittableRange(geo->getPrimitiveRange(geoGroup));
        UTparallelFor(geoSplittableRange, [geo, &attrib_h, &dstptAttrib_h, seamGroup](const GA_SplittableRange& r)
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
                            const GA_Offset dstpt = dstptAttrib_h.get(vertices[i]);
                            if (dstpt < 0)
                                continue;
                            const GA_Offset ptoff = geo->vertexPoint(vertices[i]);
                            for (vtxoff_next = geo->pointVertex(ptoff); vtxoff_next != GA_INVALID_OFFSET; vtxoff_next = geo->vertexToNextVertex(vtxoff_next))
                            {
                                if (vtxoff_next == vertices[i])
                                    continue;
                                if (dstptAttrib_h.get(vtxoff_next) != dstpt)
                                    continue;
                                if (adjElems.find(geo->vertexPrimitive(vtxoff_next)) != -1)
                                    continue;
                                adjElems.emplace_back(geo->vertexPrimitive(vtxoff_next));
                            }

                            for (vtxoff_next = geo->pointVertex(dstpt); vtxoff_next != GA_INVALID_OFFSET; vtxoff_next = geo->vertexToNextVertex(vtxoff_next))
                            {
                                if (dstptAttrib_h.get(vtxoff_next) != ptoff)
                                    continue;
                                if (adjElems.find(geo->vertexPrimitive(vtxoff_next)) != -1)
                                    continue;
                                adjElems.emplace_back(geo->vertexPrimitive(vtxoff_next));
                            }
                        }
                        attrib_h.set(elemoff, adjElems);
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
                            const GA_Offset dstpt = dstptAttrib_h.get(vertices[i]);
                            if (dstpt < 0)
                                continue;
                            const GA_Offset ptoff = geo->vertexPoint(vertices[i]);
                            for (vtxoff_next = geo->pointVertex(ptoff); vtxoff_next != GA_INVALID_OFFSET; vtxoff_next = geo->vertexToNextVertex(vtxoff_next))
                            {
                                if (vtxoff_next == vertices[i])
                                    continue;
                                if (dstptAttrib_h.get(vtxoff_next) != dstpt)
                                    continue;
                                if (adjElems.find(geo->vertexPrimitive(vtxoff_next)) >= 0)
                                    continue;
                                adjElems.emplace_back(geo->vertexPrimitive(vtxoff_next));
                            }

                            for (vtxoff_next = geo->pointVertex(dstpt); vtxoff_next != GA_INVALID_OFFSET; vtxoff_next = geo->vertexToNextVertex(vtxoff_next))
                            {
                                if (dstptAttrib_h.get(vtxoff_next) != ptoff)
                                    continue;
                                if (adjElems.find(geo->vertexPrimitive(vtxoff_next)) >= 0)
                                    continue;
                                adjElems.emplace_back(geo->vertexPrimitive(vtxoff_next));
                            }
                        }
                        attrib_h.set(elemoff, adjElems);
                    }
                }
            }
        }, subscribeRatio, minGrainSize);
    }


    //Get all prims neighbours prims with adjacent by edge
    static void
        primPrimEdge2(
            const GA_Detail* const geo,
            const GA_RWHandleT<UT_ValArray<GA_Offset>>& attrib_h,
            const GA_ROHandleT<GA_Offset>& dstptAttrib_h,
            const GA_PrimitiveGroup* const geoGroup = nullptr,
            const GA_VertexGroup*    const seamGroup = nullptr,
            const exint subscribeRatio = 128,
            const exint minGrainSize = 64
        )
    {
        const GA_SplittableRange geo0SplittableRange0(geo->getPrimitiveRange(geoGroup));
        UTparallelFor(geo0SplittableRange0, [geo, &attrib_h, &dstptAttrib_h, seamGroup](const GA_SplittableRange& r)
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
                            const GA_Offset dstpt = dstptAttrib_h.get(vertices[i]);
                            if (dstpt < 0)
                                continue;
                            const GA_Offset ptoff = geo->vertexPoint(vertices[i]);
                            for (vtxoff_next = geo->pointVertex(ptoff); vtxoff_next != GA_INVALID_OFFSET; vtxoff_next = geo->vertexToNextVertex(vtxoff_next))
                            {
                                if (vtxoff_next == vertices[i])
                                    continue;
                                if (dstptAttrib_h.get(vtxoff_next) != dstpt)
                                    continue;
                                if (adjElems.find(geo->vertexPrimitive(vtxoff_next)) != -1)
                                    continue;
                                adjElems.emplace_back(geo->vertexPrimitive(vtxoff_next));
                            }

                            for (vtxoff_next = geo->pointVertex(dstpt); vtxoff_next != GA_INVALID_OFFSET; vtxoff_next = geo->vertexToNextVertex(vtxoff_next))
                            {
                                if (dstptAttrib_h.get(vtxoff_next) != ptoff)
                                    continue;
                                if (adjElems.find(geo->vertexPrimitive(vtxoff_next)) != -1)
                                    continue;
                                adjElems.emplace_back(geo->vertexPrimitive(vtxoff_next));
                            }
                        }
                        attrib_h.set(elemoff, adjElems);
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
                            const GA_Offset dstpt = dstptAttrib_h.get(vertices[i]);
                            if (dstpt < 0)
                                continue;
                            const GA_Offset ptoff = geo->vertexPoint(vertices[i]);
                            for (vtxoff_next = geo->pointVertex(ptoff); vtxoff_next != GA_INVALID_OFFSET; vtxoff_next = geo->vertexToNextVertex(vtxoff_next))
                            {
                                if (vtxoff_next == vertices[i])
                                    continue;
                                if (dstptAttrib_h.get(vtxoff_next) != dstpt)
                                    continue;
                                if (adjElems.find(geo->vertexPrimitive(vtxoff_next)) >= 0)
                                    continue;
                                adjElems.emplace_back(geo->vertexPrimitive(vtxoff_next));
                            }

                            for (vtxoff_next = geo->pointVertex(dstpt); vtxoff_next != GA_INVALID_OFFSET; vtxoff_next = geo->vertexToNextVertex(vtxoff_next))
                            {
                                if (dstptAttrib_h.get(vtxoff_next) != ptoff)
                                    continue;
                                if (adjElems.find(geo->vertexPrimitive(vtxoff_next)) >= 0)
                                    continue;
                                adjElems.emplace_back(geo->vertexPrimitive(vtxoff_next));
                            }
                        }
                        attrib_h.set(elemoff, adjElems);
                    }
                }
            }
        }, subscribeRatio, minGrainSize);
    }



    //Get all prims neighbours prims with adjacent by edge
    static void
        primPrimEdge3(
            const GA_Detail* const geo,
            const GA_RWHandleT<UT_ValArray<GA_Offset>>& attrib_h,
            const GA_ROHandleT<GA_Offset>& dstptAttrib_h,
            const GA_PrimitiveGroup* const geoGroup = nullptr,
            const GA_VertexGroup*    const seamGroup = nullptr,
            const exint subscribeRatio = 128,
            const exint minGrainSize = 64
        )
    {
        const GA_SplittableRange geo0SplittableRange0(geo->getPrimitiveRange(geoGroup));
        UTparallelFor(geo0SplittableRange0, [geo, &attrib_h, &dstptAttrib_h, seamGroup](const GA_SplittableRange& r)
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
                            const GA_Offset dstpt = dstptAttrib_h.get(vertices[i]);
                            if (dstpt < 0)
                                continue;
                            const GA_Offset ptoff = geo->vertexPoint(vertices[i]);
                            for (vtxoff_next = geo->pointVertex(ptoff); vtxoff_next != GA_INVALID_OFFSET; vtxoff_next = geo->vertexToNextVertex(vtxoff_next))
                            {
                                if (vtxoff_next == vertices[i])
                                    continue;
                                if (dstptAttrib_h.get(vtxoff_next) != dstpt)
                                    continue;
                                if (adjElems.find(geo->vertexPrimitive(vtxoff_next)) != -1)
                                    continue;
                                adjElems.emplace_back(geo->vertexPrimitive(vtxoff_next));
                            }

                            for (vtxoff_next = geo->pointVertex(dstpt); vtxoff_next != GA_INVALID_OFFSET; vtxoff_next = geo->vertexToNextVertex(vtxoff_next))
                            {
                                if (dstptAttrib_h.get(vtxoff_next) != ptoff)
                                    continue;
                                if (adjElems.find(geo->vertexPrimitive(vtxoff_next)) != -1)
                                    continue;
                                adjElems.emplace_back(geo->vertexPrimitive(vtxoff_next));
                            }
                        }
                        attrib_h.set(elemoff, adjElems);
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
                            const GA_Offset dstpt = dstptAttrib_h.get(vertices[i]);
                            if (dstpt < 0)
                                continue;
                            const GA_Offset ptoff = geo->vertexPoint(vertices[i]);
                            for (vtxoff_next = geo->pointVertex(ptoff); vtxoff_next != GA_INVALID_OFFSET; vtxoff_next = geo->vertexToNextVertex(vtxoff_next))
                            {
                                if (vtxoff_next == vertices[i])
                                    continue;
                                if (dstptAttrib_h.get(vtxoff_next) != dstpt)
                                    continue;
                                if (adjElems.find(geo->vertexPrimitive(vtxoff_next)) >= 0)
                                    continue;
                                adjElems.emplace_back(geo->vertexPrimitive(vtxoff_next));
                            }

                            for (vtxoff_next = geo->pointVertex(dstpt); vtxoff_next != GA_INVALID_OFFSET; vtxoff_next = geo->vertexToNextVertex(vtxoff_next))
                            {
                                if (dstptAttrib_h.get(vtxoff_next) != ptoff)
                                    continue;
                                if (adjElems.find(geo->vertexPrimitive(vtxoff_next)) >= 0)
                                    continue;
                                adjElems.emplace_back(geo->vertexPrimitive(vtxoff_next));
                            }
                        }
                        attrib_h.set(elemoff, adjElems);
                    }
                }
            }
        }, subscribeRatio, minGrainSize);
    }


    //Get all prims neighbours prims with adjacent by edge
    static void
    primPrimEdge4(
        const GA_Detail* const geo,
        const GA_RWHandleT<UT_ValArray<GA_Offset>>& attrib_h,
        const GA_PrimitiveGroup* const geoGroup = nullptr,
        const GA_VertexGroup*    const seamGroup = nullptr,
        const exint subscribeRatio = 128,
        const exint minGrainSize = 64
    )
    {
        const GA_SplittableRange geo0SplittableRange0(geo->getPrimitiveRange(geoGroup));
        UTparallelFor(geo0SplittableRange0, [geo, &attrib_h, seamGroup](const GA_SplittableRange& r)
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
                        GA_Size numAdj = static_cast<const GEO_Detail*>(geo)->getEdgeAdjacentPolygons(adjElemStruct, elemoff);
                        adjElems.setSizeNoInit(numAdj);
                        for (GA_Size i = 0; i < numAdj; ++i)
                        {
                            adjElems[i] = adjElemStruct[i].myAdjacentPolygon;
                        }
                        attrib_h.set(elemoff, adjElems);
                    }
                }
                else
                {
                    for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                    {
                        GA_Size numAdj = static_cast<const GEO_Detail*>(geo)->getEdgeAdjacentPolygons(adjElemStruct, elemoff);
                        adjElems.setSizeNoInit(numAdj);
                        for (GA_Size i = 0; i < numAdj; ++i)
                        {
                            adjElems[i] = adjElemStruct[i].myAdjacentPolygon;
                        }
                        attrib_h.set(elemoff, adjElems);
                    }
                }
            }
        }, subscribeRatio, minGrainSize);
    }





    //Get all prims neighbours prims with adjacent by edge
    static void
    primPrimPoint(
        GA_Detail* const geo,
        const GA_RWHandleT<UT_ValArray<GA_Offset>>& attrib_h,
        const GA_PrimitiveGroup* const geoGroup  = nullptr,
        const GA_PointGroup*     const seamGroup = nullptr,
        const exint subscribeRatio = 128,
        const exint minGrainSize = 64
    )
    {
        GA_Topology& topo = geo->getTopology();
        //topo.makeFull();
        //topo.makePrimitiveRef();
        //topo.makeVertexRef();
        const GA_ATITopology* const vtxPrimRef = topo.getPrimitiveRef();
        const GA_ATITopology* const vtxPointRef = topo.getPointRef();
        const GA_ATITopology* const pointVtxRef = topo.getVertexRef();
        const GA_ATITopology* const vtxNextRef = topo.getVertexNextRef();


        const GA_SplittableRange geo0SplittableRange0(geo->getPrimitiveRange(geoGroup));
        UTparallelFor(geo0SplittableRange0, [geo, &attrib_h, seamGroup, vtxNextRef, vtxPointRef, pointVtxRef, vtxPrimRef](const GA_SplittableRange& r)
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
                        attrib_h.set(elemoff, adjElems);
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
                        attrib_h.set(elemoff, adjElems);
                    }
                }
            }
        }, subscribeRatio, minGrainSize);
    }

















    //GFE_Adjacency::addAttribPointPointEdge(geo, name, geoGroup, seamGroup, storage, nullptr, nullptr, GA_ReuseStrategy(), subscribeRatio, minGrainSize);
    static GA_Attribute*
        addAttribPointPointEdge(
            GA_Detail* const geo,
            const GA_PointGroup* const geoGroup = nullptr,
            const GA_PointGroup* const seamGroup = nullptr,
            const GA_Storage storage = GA_STORE_INVALID,
            const UT_StringHolder& name = "__topo_nebs",
            const UT_Options* const creation_args = nullptr,
            const GA_AttributeOptions* const attribute_options = nullptr,
            const GA_ReuseStrategy& reuse = GA_ReuseStrategy(),
            const exint subscribeRatio = 128,
            const exint minGrainSize = 64
        )
    {
        GA_Attribute* attribPtr = geo->findPointAttribute(GFE_TOPO_SCOPE, name);
        if (attribPtr)
            return attribPtr;
        GA_Attribute* vtxPrevAttrib = nullptr;
        GA_Attribute* vtxNextAttrib = nullptr;

        const GA_Storage finalStorage = storage == GA_STORE_INVALID ? GFE_Type::getPreferredStorageI(geo) : storage;

        GFE_TopologyReference::addAttribVertexVertexPrim(geo, vtxPrevAttrib, vtxNextAttrib, nullptr, finalStorage);

        attribPtr = geo->getAttributes().createArrayAttribute(GA_ATTRIB_POINT, GFE_TOPO_SCOPE, name, finalStorage, 1, creation_args, attribute_options, "arraydata", reuse);
        //attribPtr = geo->addIntTuple(GA_ATTRIB_POINT, name, 1, defaults, creation_args, attribute_options, finalStorage, reuse);
        //attribPtr = geo->addIntArray(GA_ATTRIB_POINT, GFE_TOPO_SCOPE, name, 1, creation_args, attribute_options, finalStorage, reuse);

        pointPointEdge(geo, attribPtr, vtxPrevAttrib, vtxNextAttrib, geoGroup, seamGroup, subscribeRatio, minGrainSize);
        //UT_ASSERT_P(vtxPrevAttrib);
        //UT_ASSERT_P(vtxNextAttrib);
        //GA_ROHandleT<GA_Offset> vtxPrev_h(vtxPrevAttrib);
        //GA_ROHandleT<GA_Offset> vtxNext_h(vtxNextAttrib);
        //pointPointEdge(geo, attribPtr, vtxPrev_h, vtxNext_h, geoGroup, seamGroup, subscribeRatio, minGrainSize);
        return attribPtr;
    }





    //GFE_Adjacency::addAttribPrimPrimEdge(geo, name, geoGroup, seamGroup, storage, nullptr, nullptr, GA_ReuseStrategy(), subscribeRatio, minGrainSize);
    
    static GA_Attribute*
        addAttribPrimPrimEdge(
            GA_Detail* const geo,
            const GA_PrimitiveGroup* const geoGroup = nullptr,
            const GA_VertexGroup* const seamGroup = nullptr,
            const GA_Storage storage = GA_STORE_INVALID,
            const UT_StringHolder& name = "__topo_nebs",
            const UT_Options* creation_args = nullptr,
            const GA_AttributeOptions* const attribute_options = nullptr,
            const GA_ReuseStrategy& reuse = GA_ReuseStrategy(),
            const exint subscribeRatio = 128,
            const exint minGrainSize = 64
        )
    {
        GA_Attribute* attribPtr = geo->findPrimitiveAttribute(GFE_TOPO_SCOPE, name);
        if (attribPtr)
            return attribPtr;

        const GA_Storage finalStorage = storage == GA_STORE_INVALID ? GFE_Type::getPreferredStorageI(geo) : storage;
        //GA_Attribute* dstptAttrib = GFE_TopologyReference::addAttribVertexPointDst(geo, "dstpt", nullptr, GA_Defaults(-1), finalStorage, subscribeRatio, minGrainSize);
        //GA_RWHandleT<GA_Offset> dstptAttrib_h(dstptAttrib);
        //GA_Attribute* vtxPrimNextAttrib = addAttribVertexVertexPrimNext(geo, "vtxPrimNext", nullptr, GA_Defaults(-1), finalStorage, subscribeRatio, minGrainSize);

        GA_Attribute* vtxNextEquivAttrib = GFE_VertexNextEquiv::addAttribVertexNextEquiv(geo, nullptr, finalStorage);

        attribPtr = geo->getAttributes().createArrayAttribute(GA_ATTRIB_PRIMITIVE, GFE_TOPO_SCOPE, name, finalStorage, 1, creation_args, attribute_options, "arraydata", reuse);
        //attribPtr = geo->addIntArray(GA_ATTRIB_PRIMITIVE, GFE_TOPO_SCOPE, name, 1, creation_args, attribute_options, finalStorage, reuse);
        //GA_ROHandleT<UT_ValArray<GA_Offset>> attrib_h(attribPtr);
        primPrimEdge(geo, attribPtr, vtxNextEquivAttrib, geoGroup, seamGroup, subscribeRatio, minGrainSize);
        return attribPtr;
    }




    //GFE_Adjacency::addAttribAdjacency(geo, groupType, connectivityGroupType);

    static GA_Attribute*
        addAttribAdjacency(
            GA_Detail* const geo,
            const GA_GroupType groupType,
            const GA_GroupType connectivityGroupType,
            const GA_Group* const geoGroup = nullptr,
            const GA_Group* const seamGroup = nullptr,
            const GA_Storage storage = GA_STORE_INVALID,
            const UT_StringHolder& name = "__topo_nebs",
            const UT_Options* const creation_args = nullptr,
            const GA_AttributeOptions* const attribute_options = nullptr,
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
                //return GFE_Adjacency::addAttribPrimPrimPrim(geo,
                //    UTverify_cast<const GA_PrimitiveGroup*>(geoGroup),
                //    UTverify_cast<const GA_PrimitiveGroup*>(seamGroup),
                //    storage, name, creation_args, attribute_options, reuse, subscribeRatio, minGrainSize);
                break;
            case GA_GROUP_POINT:
                //return GFE_Adjacency::addAttribPrimPrimPoint(geo,
                //    UTverify_cast<const GA_PrimitiveGroup*>(geoGroup),
                //    UTverify_cast<const GA_PointGroup*>(seamGroup),
                //    storage, name, creation_args, attribute_options, reuse, subscribeRatio, minGrainSize);
                break;
            case GA_GROUP_VERTEX:
                //return GFE_Adjacency::addAttribPrimPrimVertex(geo,
                //    UTverify_cast<const GA_PrimitiveGroup*>(geoGroup),
                //    UTverify_cast<const GA_VertexGroup*>(seamGroup),
                //    storage, name, creation_args, attribute_options, reuse, subscribeRatio, minGrainSize);
                break;
            case GA_GROUP_EDGE:
                return GFE_Adjacency::addAttribPrimPrimEdge(geo,
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
                //return GFE_Adjacency::addAttribPointPointPrim(geo,
                //    UTverify_cast<const GA_PointGroup*>(geoGroup),
                //    UTverify_cast<const GA_PrimitiveGroup*>(seamGroup),
                //    storage, name, creation_args, attribute_options, reuse, subscribeRatio, minGrainSize);
                break;
            case GA_GROUP_POINT:
                //return GFE_Adjacency::addAttribPointPointPoint(geo,
                //    UTverify_cast<const GA_PointGroup*>(geoGroup),
                //    UTverify_cast<const GA_PointGroup*>(seamGroup),
                //    storage, name, creation_args, attribute_options, reuse, subscribeRatio, minGrainSize);
                break;
            case GA_GROUP_VERTEX:
                //return GFE_Adjacency::addAttribPointPointVertex(geo,
                //    UTverify_cast<const GA_PointGroup*>(geoGroup),
                //    UTverify_cast<const GA_VertexGroup*>(seamGroup),
                //    storage, name, creation_args, attribute_options, reuse, subscribeRatio, minGrainSize);
                break;
            case GA_GROUP_EDGE:
                return GFE_Adjacency::addAttribPointPointEdge(geo,
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
                //return GFE_Adjacency::addAttribVertexVertexPrim(geo,
                //    UTverify_cast<const GA_VertexGroup*>(geoGroup),
                //    UTverify_cast<const GA_PrimitiveGroup*>(seamGroup),
                //    storage, name, creation_args, attribute_options, reuse, subscribeRatio, minGrainSize);
                break;
            case GA_GROUP_POINT:
                //return GFE_Adjacency::addAttribVertexVertexPoint(geo,
                //    UTverify_cast<const GA_VertexGroup*>(geoGroup),
                //    UTverify_cast<const GA_PointGroup*>(seamGroup),
                //    storage, name, creation_args, attribute_options, reuse, subscribeRatio, minGrainSize);
                break;
            case GA_GROUP_VERTEX:
                //return GFE_Adjacency::addAttribVertexVertexVertex(geo,
                //    UTverify_cast<const GA_VertexGroup*>(geoGroup),
                //    UTverify_cast<const GA_VertexGroup*>(seamGroup),
                //    storage, name, creation_args, attribute_options, reuse, subscribeRatio, minGrainSize);
                break;
            case GA_GROUP_EDGE:
                //return GFE_Adjacency::addAttribVertexVertexEdge(geo,
                //    UTverify_cast<const GA_VertexGroup*>(geoGroup),
                //    UTverify_cast<const GA_PointGroup*>(seamGroup),
                //    storage, name, creation_args, attribute_options, reuse, subscribeRatio, minGrainSize);
                break;
            }
            break;
        }
        return nullptr;
    }

    




} // End of namespace GFE_Adjacency

#endif
