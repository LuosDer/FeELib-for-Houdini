
#pragma once

#ifndef __GA_FeE_Adjacency_h__
#define __GA_FeE_Adjacency_h__

//#include <GEO_FeE/GA_FeE_Adjacency.h>

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

#include <GA/GA_PageHandle.h>
#include <GA/GA_PageIterator.h>


namespace GA_FeE_Adjacency {



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
        const GA_Size& vtxpnum_next = vtxpnum + 1;
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
        const GA_Offset& primoff = geo->vertexPrimitive(vtxoff);
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
        const GA_Size& vtxpnum_next = vtxpnum + 1;
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
        const GA_Offset& primoff = geo->vertexPrimitive(vtxoff);
        return vertexPointDst(geo, primoff, vertexPrimIndex(geo, primoff, vtxoff));
    }
#else
    static GA_Offset
        vertexPointDst(
            const GA_Detail* geo,
            const GA_Offset& vtxoff
        )
    {
        const GA_Offset& vertexVertexDst = vertexPointDst(geo, vtxoff);
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
        const GA_Offset& vertexVertexDst = vertexPointDst(geo, primoff, vtxpnum);
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

        const GA_Offset& ptnum = geo->vertexPoint(vtxoff);
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
        const GA_Offset& dstpt = vertexPointDst(geo, vtxoff);
        return vertexVertexNextEquiv(geo, vtxoff, dstpt);
    }














    //Get Vertex Destination Point
    static void
        vertexVertexPrim(
            const GA_Detail* geo,
            const GA_RWHandleT<GA_Size>& attribHandle_prev,
            const GA_RWHandleT<GA_Size>& attribHandle_next,
            const GA_VertexGroup* geoGroup = nullptr,
            const exint& subscribeRatio = 32,
            const exint& minGrainSize = 1024
        )
    {
        if (geoGroup)
        {
            const GA_PrimitiveGroupUPtr promotedGroupUPtr = geo->createDetachedPrimitiveGroup();
            GA_PrimitiveGroup* promotedGroup = promotedGroupUPtr.get();
            promotedGroup->combine(geoGroup);

            const GA_SplittableRange geo0SplittableRange0(geo->getPrimitiveRange(promotedGroup));
            UTparallelFor(geo0SplittableRange0, [&geo, &attribHandle_prev, &attribHandle_next, &geoGroup](const GA_SplittableRange& r)
            {
                GA_Offset start, end;
                for (GA_Iterator it(r); it.blockAdvance(start, end); )
                {
                    for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                    {
                        const GA_OffsetListRef& vertices = geo->getPrimitiveVertexList(elemoff);
                        const GA_Size& size = vertices.size();
                        if (geo->getPrimitiveClosedFlag(elemoff))
                        {
                            attribHandle_prev.set(vertices[0], vertices[size-1]);
                            attribHandle_next.set(vertices[size-1], vertices[0]);
                        }
                        else
                        {
                            attribHandle_prev.set(vertices[0], -1);
                            attribHandle_next.set(vertices[size-1], -1);
                        }
                        GA_Offset vtxoff_prev = vertices[0];
                        GA_Offset vtxoff_next;
                        for (GA_Size vtxpnum = 1; vtxpnum < size; ++vtxpnum)
                        {
                            //if (!geoGroup->contains(vtxoff))
                            //    continue;
                            vtxoff_next = vertices[vtxpnum];
                            attribHandle_next.set(vtxoff_prev, vtxoff_next);
                            attribHandle_prev.set(vtxoff_next, vtxoff_prev);
                            vtxoff_prev = vtxoff_next;
                        }
                    }
                }
            }, subscribeRatio, minGrainSize);
        }
        else
        {
            const GA_SplittableRange geo0SplittableRange0(geo->getPrimitiveRange(nullptr));
            UTparallelFor(geo0SplittableRange0, [&geo, &attribHandle_prev, &attribHandle_next](const GA_SplittableRange& r)
            {
                GA_Offset start,  end;
                for (GA_Iterator it(r); it.blockAdvance(start, end); )
                {
                    for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                    {
                        const GA_OffsetListRef& vertices = geo->getPrimitiveVertexList(elemoff);
                        const GA_Size& size = vertices.size();
                        if (geo->getPrimitiveClosedFlag(elemoff))
                        {
                            attribHandle_prev.set(vertices[0], vertices[size - 1]);
                            attribHandle_next.set(vertices[size - 1], vertices[0]);
                        }
                        else
                        {
                            attribHandle_prev.set(vertices[0], -1);
                            attribHandle_next.set(vertices[size - 1], -1);
                        }
                        GA_Offset vtxoff_prev = vertices[0];
                        GA_Offset vtxoff_next;
                        for (GA_Size vtxpnum = 1; vtxpnum < size; ++vtxpnum)
                        {
                            //if (!geoGroup->contains(vtxoff))
                            //    continue;
                            vtxoff_next = vertices[vtxpnum];
                            attribHandle_next.set(vtxoff_prev, vtxoff_next);
                            attribHandle_prev.set(vtxoff_next, vtxoff_prev);
                            vtxoff_prev = vtxoff_next;
                        }
                    }
                }
            }, subscribeRatio, minGrainSize);
        }
    }


    //Get Vertex Destination Point
    static void
        vertexVertexPrim1(
            GA_Detail* geo,
            const GA_RWHandleT<GA_Size>& attribHandle_prev,
            const GA_RWHandleT<GA_Size>& attribHandle_next,
            const GA_VertexGroup* geoGroup = nullptr,
            const exint& subscribeRatio = 32,
            const exint& minGrainSize = 1024
        )
    {
        GA_Topology& topo = geo->getTopology();
        const GA_SplittableRange geo0SplittableRange0(geo->getVertexRange(geoGroup));
        UTparallelFor(geo0SplittableRange0, [&geo, &topo, &attribHandle_prev, &attribHandle_next, &geoGroup](const GA_SplittableRange& r)
        {
            GA_Offset start, end;
            GA_Offset vtxoff_prev, vtxoff_next;
            for (GA_Iterator it(r); it.blockAdvance(start, end); )
            {
                for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                {
                    topo.getAdjacentBoundaryVertices(elemoff, vtxoff_prev, vtxoff_next);
                    attribHandle_prev.set(elemoff, vtxoff_prev);
                    attribHandle_next.set(elemoff, vtxoff_next);
                }
            }
        }, subscribeRatio, minGrainSize);
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
        if (geoGroup)
        {
            const GA_PrimitiveGroupUPtr promotedGroupUPtr = geo->createDetachedPrimitiveGroup();
            GA_PrimitiveGroup* promotedGroup = promotedGroupUPtr.get();
            promotedGroup->combine(geoGroup);

            const GA_SplittableRange geo0SplittableRange0(geo->getPrimitiveRange(promotedGroup));
            UTparallelFor(geo0SplittableRange0, [&geo, &attribHandle, &geoGroup](const GA_SplittableRange& r)
            {
                GA_Offset start, end;
                for (GA_Iterator it(r); it.blockAdvance(start, end); )
                {
                    for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                    {
                        const GA_OffsetListRef& vertices = geo->getPrimitiveVertexList(elemoff);
                        const GA_Size& size = vertices.size();
                        for (GA_Size vtxpnum = 0; vtxpnum < size; ++vtxpnum)
                        {
                            if (!geoGroup->contains(vertices[vtxpnum]))
                                continue;
                            attribHandle.set(vertices[vtxpnum], vtxpnum);
                        }
                    }
                }
            }, subscribeRatio, minGrainSize);
        }
        else
        {
            const GA_SplittableRange geo0SplittableRange0(geo->getPrimitiveRange(nullptr));
            UTparallelFor(geo0SplittableRange0, [&geo, &attribHandle](const GA_SplittableRange& r)
            {
                GA_Offset start, end;
                for (GA_Iterator it(r); it.blockAdvance(start, end); )
                {
                    for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                    {
                        const GA_OffsetListRef& vertices = geo->getPrimitiveVertexList(elemoff);
                        const GA_Size& size = vertices.size();
                        for (GA_Size vtxpnum = 0; vtxpnum < size; ++vtxpnum)
                        {
                            attribHandle.set(vertices[vtxpnum], vtxpnum);
                        }
                    }
                }
            }, subscribeRatio, minGrainSize);
        }
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
        const GA_SplittableRange geo0SplittableRange0(geo->getVertexRange(geoGroup));
        UTparallelFor(geo0SplittableRange0, [&geo, &attribHandle](const GA_SplittableRange& r)
        {
            GA_Offset start, end;
            for (GA_Iterator it(r); it.blockAdvance(start, end); )
            {
                for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                {
                    attribHandle.set(elemoff, vertexPrimIndex(geo, elemoff));
                }
            }
        }, subscribeRatio, minGrainSize);
    }
#endif



    //Get all vertices NextEquiv Vertex
    //template<typename T>
    static void
        vertexVertexDst(
            const GA_Detail* geo,
            const GA_RWHandleT<GA_Offset>& attribHandle,
            const GA_ROHandleT<GA_Size>& vtxpnumAttribHandle,
            const GA_VertexGroup* geoGroup = nullptr,
            const exint& subscribeRatio = 16,
            const exint& minGrainSize = 1024
        )
    {
        const GA_SplittableRange geo0SplittableRange0(geo->getVertexRange(geoGroup));
        UTparallelFor(geo0SplittableRange0, [&geo, &attribHandle, &vtxpnumAttribHandle](const GA_SplittableRange& r)
        {
            GA_Offset start, end;
            for (GA_Iterator it(r); it.blockAdvance(start, end); )
            {
                for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                {
                    attribHandle.set(elemoff, vertexVertexDst(geo, geo->vertexPrimitive(elemoff), vtxpnumAttribHandle.get(elemoff)));
                }
            }
        }, subscribeRatio, minGrainSize);
    }





    //Get all vertices NextEquiv Vertex
    //template<typename T>
    static void
        vertexPointDst(
            const GA_Detail* geo,
            GA_Attribute* attribPtr,
            const GA_Attribute* vtxpnumAttribPtr,
            const GA_VertexGroup* geoGroup = nullptr,
            const exint& subscribeRatio = 16,
            const exint& minGrainSize = 1024
        )
    {
        const GA_SplittableRange geo0SplittableRange0(geo->getVertexRange(geoGroup));
        UTparallelFor(geo0SplittableRange0, [&geo, &attribPtr, &vtxpnumAttribPtr](const GA_SplittableRange& r)
        {
            GA_PageHandleScalar<GA_Offset>::RWType attrib_ph(attribPtr);
            GA_PageHandleScalar<GA_Size>::ROType vtxpnumAttrib_ph(vtxpnumAttribPtr);
            for (GA_PageIterator pit = r.beginPages(); !pit.atEnd(); ++pit)
            {
                GA_Offset start, end;
                for (GA_Iterator it(pit.begin()); it.blockAdvance(start, end); )
                {
                    attrib_ph.setPage(start);
                    vtxpnumAttrib_ph.setPage(start);
                    for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                    {
                        attrib_ph.set(elemoff, vertexPointDst(geo, geo->vertexPrimitive(elemoff), vtxpnumAttrib_ph.value(elemoff)));
                    }
                }
            }
        }, subscribeRatio, minGrainSize);
    }


    //Get all vertices NextEquiv Vertex
    //template<typename T>
    static void
        vertexPointDst(
            const GA_Detail* geo,
            const GA_RWHandleT<GA_Offset>& attribHandle,
            const GA_ROHandleT<GA_Size>& vtxpnumAttribHandle,
            const GA_VertexGroup* geoGroup = nullptr,
            const exint& subscribeRatio = 16,
            const exint& minGrainSize = 1024
        )
    {
        const GA_SplittableRange geo0SplittableRange0(geo->getVertexRange(geoGroup));
        UTparallelFor(geo0SplittableRange0, [&geo, &attribHandle, &vtxpnumAttribHandle](const GA_SplittableRange& r)
        {
            GA_Offset start, end;
            for (GA_Iterator it(r); it.blockAdvance(start, end); )
            {
                for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                {
                    attribHandle.set(elemoff, vertexPointDst(geo, geo->vertexPrimitive(elemoff), vtxpnumAttribHandle.get(elemoff)));
                }
            }
        }, subscribeRatio, minGrainSize);
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
        const GA_SplittableRange geo0SplittableRange0(geo->getVertexRange(geoGroup));
        UTparallelFor(geo0SplittableRange0, [&geo, &attribHandle](const GA_SplittableRange& r)
        {
            GA_Offset start, end;
            for (GA_Iterator it(r); it.blockAdvance(start, end); )
            {
                for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                {
                    attribHandle.set(elemoff, vertexPointDst(geo, elemoff));
                }
            }
        }, subscribeRatio, minGrainSize);
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
        const GA_SplittableRange geo0SplittableRange0(geo->getVertexRange(geoGroup));
        UTparallelFor(geo0SplittableRange0, [&geo, &attribHandle](const GA_SplittableRange& r)
        {
            GA_Offset start, end;
            for (GA_Iterator it(r); it.blockAdvance(start, end); )
            {
                for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                {
                    attribHandle.set(elemoff, vertexVertexNextEquiv(geo, elemoff));
                }
            }
        }, subscribeRatio, minGrainSize);
    }





    //Get all vertices NextEquiv Vertex
    static void
        vertexVertexNextEquiv(
            const GA_Detail* geo,
            const GA_RWHandleT<GA_Offset>& attribHandle,
            GA_VertexGroup* validGroup,
            const GA_ROHandleT<GA_Offset>& dstptAttribHandle,
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
            GA_Offset start, end;
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

                    const GA_Offset& ptnum = geo->vertexPoint(elemoff);
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
        validGroup->invalidateGroupEntries();
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

            GA_Offset vtxoff_next;
            GA_Offset start, end;
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

                    const GA_Offset& ptnum = geo->vertexPoint(elemoff);
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
    }



    //Get all vertices NextEquiv Vertex
    static void
        vertexVertexNextEquiv(
            const GA_Detail* geo,
            GA_VertexGroup* validGroup,
            const GA_ROHandleT<GA_Offset>& dstptAttribHandle,
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
            GA_Offset start, end;
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

                    const GA_Offset& ptnum = geo->vertexPoint(elemoff);
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
        ptoffArray.clear();

        GA_Offset pt_next;
        for (GA_Offset vtxoff_next = geo->pointVertex(ptoff); vtxoff_next != GA_INVALID_OFFSET; vtxoff_next = geo->vertexToNextVertex(vtxoff_next))
        {
            const GA_Offset& primoff = geo->vertexPrimitive(vtxoff_next);
            const GA_Size& numvtx = geo->getPrimitiveVertexCount(primoff);
            const GA_Size& vtxpnum = vertexPrimIndex(geo, primoff, vtxoff_next);

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

            const GA_Size& vtxpnum_next = vtxpnum + 1;
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
            const exint& subscribeRatio = 16,
            const exint& minGrainSize = 1024
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
                        const GA_Offset& primoff = geo->vertexPrimitive(vtxoff_next);
                        const GA_Size& numvtx = geo->getPrimitiveVertexCount(primoff);
                        const GA_Size& vtxpnum = vtxpnumAttribHandle.get(vtxoff_next);

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

                        const GA_Size& vtxpnum_next = vtxpnum + 1;
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
        const exint& subscribeRatio = 16,
        const exint& minGrainSize = 1024
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


    //Get all prims neighbours prims with adjacent by edge
    static void
        primPrimEdge(
            const GA_Detail* geo,
            const GA_RWHandleT<UT_ValArray<GA_Offset>>& attribHandle,
            const GA_ROHandleT<GA_Offset>& dstptAttribHandle,
            const GA_PrimitiveGroup* geoGroup = nullptr,
            const GA_VertexGroup* seamGroup = nullptr,
            const exint& subscribeRatio = 16,
            const exint& minGrainSize = 1024
        )
    {
        const GA_SplittableRange geo0SplittableRange0(geo->getPrimitiveRange(geoGroup));
        UTparallelFor(geo0SplittableRange0, [&geo, &attribHandle, &dstptAttribHandle, &seamGroup](const GA_SplittableRange& r)
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
                        const GA_Size& numvtx = vertices.size();
                        for (GA_Size i = 0; i < numvtx; ++i)
                        {
                            const GA_Offset& dstpt = dstptAttribHandle.get(vertices[i]);
                            if (dstpt < 0)
                                continue;
                            const GA_Offset& ptoff = geo->vertexPoint(vertices[i]);
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
                        const GA_Size& numvtx = vertices.size();
                        for (GA_Size i = 0; i < numvtx; ++i)
                        {
                            const GA_Offset& dstpt = dstptAttribHandle.get(vertices[i]);
                            if (dstpt < 0)
                                continue;
                            const GA_Offset& ptoff = geo->vertexPoint(vertices[i]);
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
        primPrimEdge1(
            const GA_Detail* geo,
            const GA_RWHandleT<UT_ValArray<GA_Offset>>& attribHandle,
            const GA_ROHandleT<GA_Offset>& dstptAttribHandle,
            const GA_PrimitiveGroup* geoGroup = nullptr,
            const GA_VertexGroup* seamGroup = nullptr,
            const exint& subscribeRatio = 16,
            const exint& minGrainSize = 1024
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
                        const GA_Size& numvtx = vertices.size();
                        for (GA_Size i = 0; i < numvtx; ++i)
                        {
                            const GA_Offset& dstpt = dstptAttribHandle.get(vertices[i]);
                            if (dstpt < 0)
                                continue;
                            const GA_Offset& ptoff = geo->vertexPoint(vertices[i]);
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
                        const GA_Size& numvtx = vertices.size();
                        for (GA_Size i = 0; i < numvtx; ++i)
                        {
                            const GA_Offset& dstpt = dstptAttribHandle.get(vertices[i]);
                            if (dstpt < 0)
                                continue;
                            const GA_Offset& ptoff = geo->vertexPoint(vertices[i]);
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
            const exint& subscribeRatio = 16,
            const exint& minGrainSize = 1024
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
                        const GA_Size& numvtx = vertices.size();
                        for (GA_Size i = 0; i < numvtx; ++i)
                        {
                            const GA_Offset& dstpt = dstptAttribHandle.get(vertices[i]);
                            if (dstpt < 0)
                                continue;
                            const GA_Offset& ptoff = geo->vertexPoint(vertices[i]);
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
                        const GA_Size& numvtx = vertices.size();
                        for (GA_Size i = 0; i < numvtx; ++i)
                        {
                            const GA_Offset& dstpt = dstptAttribHandle.get(vertices[i]);
                            if (dstpt < 0)
                                continue;
                            const GA_Offset& ptoff = geo->vertexPoint(vertices[i]);
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
        const GEO_Detail* geo,
        const GA_RWHandleT<UT_ValArray<GA_Offset>>& attribHandle,
        const GA_PrimitiveGroup* geoGroup = nullptr,
        const GA_VertexGroup* seamGroup = nullptr,
        const exint& subscribeRatio = 16,
        const exint& minGrainSize = 1024
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
        const GA_Detail* geo,
        const GA_RWHandleT<UT_ValArray<GA_Offset>>& attribHandle,
        const GA_PrimitiveGroup* geoGroup = nullptr,
        const GA_PointGroup* seamGroup = nullptr,
        const exint& subscribeRatio = 16,
        const exint& minGrainSize = 1024
    )
    {
        const GA_SplittableRange geo0SplittableRange0(geo->getPrimitiveRange(geoGroup));
        UTparallelFor(geo0SplittableRange0, [&geo, &attribHandle, &seamGroup](const GA_SplittableRange& r)
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
                        const GA_Size& numvtx = vertices.size();
                        for (GA_Size i = 0; i < numvtx; ++i)
                        {
                            const GA_Offset& ptoff = geo->vertexPoint(vertices[i]);
                            if (seamGroup->contains(ptoff))
                                continue;
                            for (vtxoff_next = geo->pointVertex(ptoff); vtxoff_next != GA_INVALID_OFFSET; vtxoff_next = geo->vertexToNextVertex(vtxoff_next))
                            {
                                if (vtxoff_next == vertices[i])
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
                        const GA_Size& numvtx = vertices.size();
                        for (GA_Size i = 0; i < numvtx; ++i)
                        {
                            const GA_Offset& ptoff = geo->vertexPoint(vertices[i]);
                            for (vtxoff_next = geo->pointVertex(ptoff); vtxoff_next != GA_INVALID_OFFSET; vtxoff_next = geo->vertexToNextVertex(vtxoff_next))
                            {
                                if (vtxoff_next == vertices[i])
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
        }, subscribeRatio, minGrainSize);
    }









} // End of namespace GA_FeE_Adjacency

#endif
