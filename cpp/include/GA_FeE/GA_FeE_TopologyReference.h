
#pragma once

#ifndef __GA_FeE_TopologyReference_h__
#define __GA_FeE_TopologyReference_h__

//#include <GA_FeE/GA_FeE_TopologyReference.h>

#include <GA/GA_Detail.h>
#include <GA/GA_SplittableRange.h>
#include <GA/GA_PageHandle.h>
#include <GA/GA_PageIterator.h>

namespace GA_FeE_TopologyReference {



    //Get Vertex Prim Index
    static GA_Size
        vertexPrimIndex(
            const GA_Detail* geo,
            const GA_Offset vtxoff
        )
    {
        return geo->getPrimitiveVertexList(geo->vertexPrimitive(vtxoff)).find(vtxoff);
    }

    static GA_Size
        vertexPrimIndex(
            const GA_Detail* geo,
            const GA_Offset primoff,
            const GA_Offset vtxoff
        )
    {
        return geo->getPrimitiveVertexList(primoff).find(vtxoff);
    }




    //Get Vertex Destination Point
    //This is Faster than use linear vertex offset
    static GA_Offset
        vertexVertexDst(
            const GA_Detail* geo,
            const GA_Offset primoff,
            const GA_Size vtxpnum
        )
    {
        const GA_Size vtxpnum_next = vtxpnum + 1;
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
            const GA_Offset vtxoff
        )
    {
        const GA_Offset primoff = geo->vertexPrimitive(vtxoff);
        return vertexVertexDst(geo, primoff, vertexPrimIndex(geo, primoff, vtxoff));
    }



    //Get Vertex Destination Point
#if 1
    //This is Faster than use linear vertex offset
    static GA_Offset
        vertexPointDst(
            const GA_Detail* geo,
            const GA_Offset primoff,
            const GA_Size vtxpnum
        )
    {
        const GA_Size vtxpnum_next = vtxpnum + 1;
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
        const GA_Offset vtxoff
    )
    {
        const GA_Offset primoff = geo->vertexPrimitive(vtxoff);
        return vertexPointDst(geo, primoff, vertexPrimIndex(geo, primoff, vtxoff));
    }
#else
    static GA_Offset
        vertexPointDst(
            const GA_Detail* geo,
            const GA_Offset vtxoff
        )
    {
        const GA_Offset vertexVertexDst = vertexPointDst(geo, vtxoff);
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
            const GA_Offset primoff,
            const GA_Size vtxpnum
        )
    {
        const GA_Offset vertexVertexDst = vertexPointDst(geo, primoff, vtxpnum);
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














    //Get Vertex neb Vertex in same Prim
    static void
        vertexVertexPrim(
            const GA_Detail* geo,
            const GA_RWHandleT<GA_Offset>& attribHandle_prev,
            const GA_RWHandleT<GA_Offset>& attribHandle_next,
            const GA_VertexGroup* geoGroup = nullptr,
            const exint subscribeRatio = 64,
            const exint minGrainSize = 128
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
                        const GA_Size numvtx = vertices.size();
                        if (geo->getPrimitiveClosedFlag(elemoff))
                        {
                            attribHandle_prev.set(vertices[0], vertices[numvtx -1]);
                            attribHandle_next.set(vertices[numvtx -1], vertices[0]);
                        }
                        else
                        {
                            attribHandle_prev.set(vertices[0], -1);
                            attribHandle_next.set(vertices[numvtx -1], -1);
                        }
                        GA_Offset vtxoff_prev = vertices[0];
                        GA_Offset vtxoff_next;
                        for (GA_Size vtxpnum = 1; vtxpnum < numvtx; ++vtxpnum)
                        {
                            vtxoff_next = vertices[vtxpnum];
                            if (!geoGroup->contains(vtxoff_next))
                                continue;
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
            const GA_SplittableRange geo0SplittableRange0(geo->getPrimitiveRange());
            UTparallelFor(geo0SplittableRange0, [&geo, &attribHandle_prev, &attribHandle_next](const GA_SplittableRange& r)
            {
                GA_Offset start,  end;
                for (GA_Iterator it(r); it.blockAdvance(start, end); )
                {
                    for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                    {
                        const GA_OffsetListRef& vertices = geo->getPrimitiveVertexList(elemoff);
                        const GA_Size numvtx = vertices.size();
                        if (geo->getPrimitiveClosedFlag(elemoff))
                        {
                            attribHandle_prev.set(vertices[0], vertices[numvtx - 1]);
                            attribHandle_next.set(vertices[numvtx - 1], vertices[0]);
                        }
                        else
                        {
                            attribHandle_prev.set(vertices[0], -1);
                            attribHandle_next.set(vertices[numvtx - 1], -1);
                        }
                        GA_Offset vtxoff_prev = vertices[0];
                        GA_Offset vtxoff_next;
                        for (GA_Size vtxpnum = 1; vtxpnum < numvtx; ++vtxpnum)
                        {
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


    



    //Get Vertex Destination Vertex
    static void
        vertexVertexPrimNext(
        const GA_Detail* geo,
        const GA_RWHandleT<GA_Offset>& attribHandle_next,
        const GA_VertexGroup* geoGroup = nullptr,
        const exint subscribeRatio = 64,
        const exint minGrainSize = 256
    )
    {
        if (geoGroup)
        {
            const GA_PrimitiveGroupUPtr promotedGroupUPtr = geo->createDetachedPrimitiveGroup();
            GA_PrimitiveGroup* promotedGroup = promotedGroupUPtr.get();
            promotedGroup->combine(geoGroup);

            const GA_SplittableRange geo0SplittableRange0(geo->getPrimitiveRange(promotedGroup));
            UTparallelFor(geo0SplittableRange0, [&geo, &attribHandle_next, &geoGroup](const GA_SplittableRange& r)
            {
                GA_Offset start, end;
                for (GA_Iterator it(r); it.blockAdvance(start, end); )
                {
                    for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                    {
                        const GA_OffsetListRef& vertices = geo->getPrimitiveVertexList(elemoff);
                        const GA_Size numvtx = vertices.size();
                        if (geo->getPrimitiveClosedFlag(elemoff))
                        {
                            //attribHandle_prev.set(vertices[0], vertices[numvtx - 1]);
                            attribHandle_next.set(vertices[numvtx - 1], vertices[0]);
                        }
                        else
                        {
                            //attribHandle_prev.set(vertices[0], -1);
                            attribHandle_next.set(vertices[numvtx - 1], -1);
                        }
                        GA_Offset vtxoff_prev = vertices[0];
                        GA_Offset vtxoff_next;
                        for (GA_Size vtxpnum = 1; vtxpnum < numvtx; ++vtxpnum)
                        {
                            vtxoff_next = vertices[vtxpnum];
                            if (!geoGroup->contains(vtxoff_next))
                                continue;
                            attribHandle_next.set(vtxoff_prev, vtxoff_next);
                            //attribHandle_prev.set(vtxoff_next, vtxoff_prev);
                            vtxoff_prev = vtxoff_next;
                        }
                    }
                }
            }, subscribeRatio, minGrainSize);
        }
        else
        {
            const GA_SplittableRange geo0SplittableRange0(geo->getPrimitiveRange());
            UTparallelFor(geo0SplittableRange0, [&geo, &attribHandle_next](const GA_SplittableRange& r)
            {
                GA_Offset start, end;
                for (GA_Iterator it(r); it.blockAdvance(start, end); )
                {
                    for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                    {
                        const GA_OffsetListRef& vertices = geo->getPrimitiveVertexList(elemoff);
                        const GA_Size numvtx = vertices.size();
                        if (geo->getPrimitiveClosedFlag(elemoff))
                        {
                            //attribHandle_prev.set(vertices[0], vertices[numvtx - 1]);
                            attribHandle_next.set(vertices[numvtx - 1], vertices[0]);
                        }
                        else
                        {
                            //attribHandle_prev.set(vertices[0], -1);
                            attribHandle_next.set(vertices[numvtx - 1], -1);
                        }
                        GA_Offset vtxoff_prev = vertices[0];
                        GA_Offset vtxoff_next;
                        for (GA_Size vtxpnum = 1; vtxpnum < numvtx; ++vtxpnum)
                        {
                            vtxoff_next = vertices[vtxpnum];
                            attribHandle_next.set(vtxoff_prev, vtxoff_next);
                            //attribHandle_prev.set(vtxoff_next, vtxoff_prev);
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
            const exint subscribeRatio = 64,
            const exint minGrainSize = 128
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







    //Get all vertices NextEquiv Vertex
    //template<typename T>
    static void
        vertexPrimIndex(
            const GA_Detail* geo,
            const GA_RWHandleT<GA_Size>& attribHandle,
            const GA_VertexGroup* geoGroup = nullptr,
            const exint subscribeRatio = 64,
            const exint minGrainSize = 16
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
                        const GA_Size numvtx = vertices.size();
                        for (GA_Size vtxpnum = 0; vtxpnum < numvtx; ++vtxpnum)
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
                        const GA_Size numvtx = vertices.size();
                        for (GA_Size vtxpnum = 0; vtxpnum < numvtx; ++vtxpnum)
                        {
                            attribHandle.set(vertices[vtxpnum], vtxpnum);
                        }
                    }
                }
            }, subscribeRatio, minGrainSize);
        }
    }





    //Get all vertices NextEquiv Vertex
    //template<typename T>
    static void
        vertexPointDstByVtxpnum(
            GA_Detail* geo,
            const GA_RWHandleT<GA_Offset>& attribHandle,
            const GA_ROHandleT<GA_Size>& vtxpnumAttribHandle,
            const GA_VertexGroup* geoGroup = nullptr,
            const exint subscribeRatio = 64,
            const exint minGrainSize = 64
        )
    {
        GA_Topology& topo = geo->getTopology();
        topo.makePrimitiveRef();
        const GA_ATITopology* vtxPrimRef = topo.getPrimitiveRef();

        const GA_SplittableRange geo0SplittableRange0(geo->getVertexRange(geoGroup));
        UTparallelFor(geo0SplittableRange0, [&geo, &attribHandle, &vtxpnumAttribHandle, &vtxPrimRef](const GA_SplittableRange& r)
        {
            GA_Offset start, end;
            for (GA_Iterator it(r); it.blockAdvance(start, end); )
            {
                for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                {
                    attribHandle.set(elemoff, vertexPointDst(geo, vtxPrimRef->getLink(elemoff), vtxpnumAttribHandle.get(elemoff)));
                }
            }
        }, subscribeRatio, minGrainSize);
    }


    //Get all vertices NextEquiv Vertex
    //template<typename T>
    static void
        vertexPointDst(
            GA_Detail* geo,
            GA_Attribute* attrib_next,
            const GA_Attribute* vtxPrimNextAttrib,
            const GA_VertexGroup* geoGroup = nullptr,
            const exint subscribeRatio = 64,
            const exint minGrainSize = 64
        )
    {
        GA_Topology& topo = geo->getTopology();
        const GA_ATITopology* vtxPointRef = topo.getPointRef();

        const GA_SplittableRange geo0SplittableRange0(geo->getVertexRange(geoGroup));
        UTparallelFor(geo0SplittableRange0, [&geo, &attrib_next, &vtxPrimNextAttrib, &vtxPointRef](const GA_SplittableRange& r)
        {
            GA_PageHandleScalar<GA_Offset>::RWType dstpt_aph(attrib_next);
            GA_PageHandleScalar<GA_Offset>::ROType vtxPrimNext_aph(vtxPrimNextAttrib);
            for (GA_PageIterator pit = r.beginPages(); !pit.atEnd(); ++pit)
            {
                GA_Offset start, end;
                for (GA_Iterator it(pit.begin()); it.blockAdvance(start, end); )
                {
                    dstpt_aph.setPage(start);
                    vtxPrimNext_aph.setPage(start);
                    for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                    {
                        if (vtxPrimNext_aph.value(elemoff) < 0)
                        {
                            dstpt_aph.value(elemoff) = GA_INVALID_OFFSET;
                        }
                        else
                        {
                            dstpt_aph.value(elemoff) = vtxPointRef->getLink(vtxPrimNext_aph.value(elemoff));
                        }
                    }
                }
            }
        }, subscribeRatio, minGrainSize);
    }

    
    

    //Get all vertices NextEquiv Vertex
    //template<typename T>
    static void
        vertexPointDst(
            GA_Detail* geo,
            const GA_RWHandleT<GA_Offset>& attribHandle_next,
            const GA_VertexGroup* geoGroup = nullptr,
            const exint subscribeRatio = 64,
            const exint minGrainSize = 64
        )
    {
        GA_Topology& topo = geo->getTopology();
        const GA_ATITopology* vtxPointRef = topo.getPointRef();
        if (geoGroup)
        {
            const GA_PrimitiveGroupUPtr promotedGroupUPtr = geo->createDetachedPrimitiveGroup();
            GA_PrimitiveGroup* promotedGroup = promotedGroupUPtr.get();
            promotedGroup->combine(geoGroup);

            const GA_SplittableRange geo0SplittableRange0(geo->getPrimitiveRange(promotedGroup));
            UTparallelFor(geo0SplittableRange0, [&geo, &attribHandle_next, &geoGroup, &vtxPointRef](const GA_SplittableRange& r)
            {
                GA_Offset start, end;
                for (GA_Iterator it(r); it.blockAdvance(start, end); )
                {
                    for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                    {
                        const GA_OffsetListRef& vertices = geo->getPrimitiveVertexList(elemoff);
                        const GA_Size numvtx = vertices.size();
                        if (geo->getPrimitiveClosedFlag(elemoff))
                        {
                            //attribHandle_prev.set(vertices[0], vtxPointRef->getLink(vertices[numvtx - 1]));
                            attribHandle_next.set(vertices[numvtx - 1], vtxPointRef->getLink(vertices[0]));
                        }
                        else
                        {
                            //attribHandle_prev.set(vertices[0], -1);
                            attribHandle_next.set(vertices[numvtx - 1], -1);
                        }
                        GA_Offset vtxoff_prev = vertices[0];
                        GA_Offset vtxoff_next;
                        for (GA_Size vtxpnum = 1; vtxpnum < numvtx; ++vtxpnum)
                        {
                            vtxoff_next = vertices[vtxpnum];
                            if (!geoGroup->contains(vtxoff_next))
                                continue;
                            attribHandle_next.set(vtxoff_prev, vtxPointRef->getLink(vtxoff_next));
                            //attribHandle_prev.set(vtxoff_next, vtxPointRef->getLink(vtxoff_prev));
                            vtxoff_prev = vtxoff_next;
                        }
                    }
                }
            }, subscribeRatio, minGrainSize);
        }
        else
        {
            const GA_SplittableRange geo0SplittableRange0(geo->getPrimitiveRange());
            UTparallelFor(geo0SplittableRange0, [&geo, &attribHandle_next, &vtxPointRef](const GA_SplittableRange& r)
            {
                GA_Offset start, end;
                for (GA_Iterator it(r); it.blockAdvance(start, end); )
                {
                    for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                    {
                        const GA_OffsetListRef& vertices = geo->getPrimitiveVertexList(elemoff);
                        const GA_Size numvtx = vertices.size();
                        if (geo->getPrimitiveClosedFlag(elemoff))
                        {
                            //attribHandle_prev.set(vertices[0], vtxPointRef->getLink(vertices[numvtx - 1]));
                            attribHandle_next.set(vertices[numvtx - 1], vtxPointRef->getLink(vertices[0]));
                        }
                        else
                        {
                            //attribHandle_prev.set(vertices[0], -1);
                            attribHandle_next.set(vertices[numvtx - 1], -1);
                        }
                        GA_Offset vtxoff_prev = vertices[0];
                        GA_Offset vtxoff_next;
                        for (GA_Size vtxpnum = 1; vtxpnum < numvtx; ++vtxpnum)
                        {
                            vtxoff_next = vertices[vtxpnum];
                            attribHandle_next.set(vtxoff_prev, vtxPointRef->getLink(vtxoff_next));
                            //attribHandle_prev.set(vtxoff_next, vtxPointRef->getLink(vtxoff_prev));
                            vtxoff_prev = vtxoff_next;
                        }
                    }
                }
            }, subscribeRatio, minGrainSize);
        }
    }
















    //GA_FeE_Adjacency::addAttribVertexPrimIndex(geo, name, geoGroup, defaults, creation_args, attribute_options, storage, reuse, subscribeRatio, minGrainSize);

    static GA_Attribute*
        addAttribVertexPrimIndex(
            GEO_Detail* geo,
            const UT_StringHolder& name = "__topo_vtxpnum",
            const GA_VertexGroup* geoGroup = nullptr,
            const GA_Defaults& defaults = GA_Defaults(-1),
            const GA_Storage storage = GA_STORE_INT32,
            const UT_Options* creation_args = nullptr,
            const GA_AttributeOptions* attribute_options = nullptr,
            const GA_ReuseStrategy& reuse = GA_ReuseStrategy(),
            const exint subscribeRatio = 64,
            const exint minGrainSize = 16
        )
    {
        GA_Attribute* attribPtr = geo->findVertexAttribute(GA_SCOPE_PRIVATE, name);
        if (attribPtr)
            return attribPtr;
        attribPtr = geo->addIntTuple(GA_ATTRIB_VERTEX, GA_SCOPE_PRIVATE, name, 1, defaults, creation_args, attribute_options, storage, reuse);
        vertexPrimIndex(geo, attribPtr, geoGroup, subscribeRatio, minGrainSize);
        return attribPtr;
    }

    //GA_FeE_Adjacency::addAttribVertexPrimIndex(geo, name, geoGroup, defaults, storage, subscribeRatio, minGrainSize);

    SYS_FORCE_INLINE
        static GA_Attribute*
        addAttribVertexPrimIndex(
            GEO_Detail* geo,
            const UT_StringHolder& name = "__topo_vtxpnum",
            const GA_VertexGroup* geoGroup = nullptr,
            const GA_Defaults& defaults = GA_Defaults(-1),
            const GA_Storage storage = GA_STORE_INT32,
            const exint subscribeRatio = 64,
            const exint minGrainSize = 16
        )
    {
        return addAttribVertexPrimIndex(geo, name, geoGroup, defaults, storage, nullptr, nullptr, GA_ReuseStrategy(), subscribeRatio, minGrainSize);
    }












    //GA_FeE_Adjacency::addAttribVertexVertexPrim(geo, name, geoGroup, defaults, creation_args, attribute_options, storage, reuse, subscribeRatio, minGrainSize);

    static bool
        addAttribVertexVertexPrim(
            GEO_Detail* geo,
            GA_Attribute*& attribPtr_prev,
            GA_Attribute*& attribPtr_next,
            const UT_StringHolder& namePrev = "__topo_vtxPrimPrev",
            const UT_StringHolder& nameNext = "__topo_vtxPrimNext",
            const GA_VertexGroup* geoGroup = nullptr,
            const GA_Defaults& defaults = GA_Defaults(-1),
            const GA_Storage storage = GA_STORE_INT32,
            const UT_Options* creation_args = nullptr,
            const GA_AttributeOptions* attribute_options = nullptr,
            const GA_ReuseStrategy& reuse = GA_ReuseStrategy(),
            const exint subscribeRatio = 64,
            const exint minGrainSize = 128
        )
    {
        attribPtr_prev = geo->findVertexAttribute(GA_SCOPE_PRIVATE, namePrev);
        attribPtr_next = geo->findVertexAttribute(GA_SCOPE_PRIVATE, nameNext);
        if (attribPtr_prev && attribPtr_next)
            return false;
        attribPtr_prev = geo->addIntTuple(GA_ATTRIB_VERTEX, GA_SCOPE_PRIVATE, namePrev, 1, defaults, creation_args, attribute_options, storage, reuse);
        attribPtr_next = geo->addIntTuple(GA_ATTRIB_VERTEX, GA_SCOPE_PRIVATE, nameNext, 1, defaults, creation_args, attribute_options, storage, reuse);
        vertexVertexPrim(geo, attribPtr_prev, attribPtr_next, geoGroup, subscribeRatio, minGrainSize);
        return true;
    }


    //GA_FeE_Adjacency::addAttribVertexVertexPrim(geo, attribPtr_prev, attribPtr_next, namePrev, nameNext, geoGroup, defaults, storage, subscribeRatio, minGrainSize);

    SYS_FORCE_INLINE
        static bool
        addAttribVertexVertexPrim(
            GEO_Detail* geo,
            GA_Attribute*& attribPtr_prev,
            GA_Attribute*& attribPtr_next,
            const UT_StringHolder& namePrev = "__topo_vtxPrimPrev",
            const UT_StringHolder& nameNext = "__topo_vtxPrimNext",
            const GA_VertexGroup* geoGroup = nullptr,
            const GA_Defaults& defaults = GA_Defaults(-1),
            const GA_Storage storage = GA_STORE_INT32,
            const exint subscribeRatio = 64,
            const exint minGrainSize = 128
        )
    {
        return addAttribVertexVertexPrim(geo, attribPtr_prev, attribPtr_next, namePrev, nameNext, geoGroup, defaults, storage, nullptr, nullptr, GA_ReuseStrategy(), subscribeRatio, minGrainSize);
    }










    //GA_FeE_Adjacency::addAttribVertexVertexPrimNext(geo, name, geoGroup, defaults, creation_args, attribute_options, storage, reuse, subscribeRatio, minGrainSize);

    static GA_Attribute*
        addAttribVertexVertexPrimNext(
            GEO_Detail* geo,
            const UT_StringHolder& nameNext = "__topo_vtxPrimNext",
            const GA_VertexGroup* geoGroup = nullptr,
            const GA_Defaults& defaults = GA_Defaults(-1),
            const GA_Storage storage = GA_STORE_INT32,
            const UT_Options* creation_args = nullptr,
            const GA_AttributeOptions* attribute_options = nullptr,
            const GA_ReuseStrategy& reuse = GA_ReuseStrategy(),
            const exint subscribeRatio = 64,
            const exint minGrainSize = 256
        )
    {
        GA_Attribute* attribPtr_next = geo->findVertexAttribute(GA_SCOPE_PRIVATE, nameNext);
        if (attribPtr_next)
            return attribPtr_next;
        attribPtr_next = geo->addIntTuple(GA_ATTRIB_VERTEX, GA_SCOPE_PRIVATE, nameNext, 1, defaults, creation_args, attribute_options, storage, reuse);
        vertexVertexPrimNext(geo, attribPtr_next, geoGroup, subscribeRatio, minGrainSize);
        return attribPtr_next;
    }

    //GA_FeE_Adjacency::addAttribVertexVertexPrimNext(geo, attribPtr_next, nameNext, geoGroup, defaults, storage, subscribeRatio, minGrainSize);

    SYS_FORCE_INLINE
        static GA_Attribute*
        addAttribVertexVertexPrimNext(
            GEO_Detail* geo,
            const UT_StringHolder& nameNext = "__topo_vtxPrimNext",
            const GA_VertexGroup* geoGroup = nullptr,
            const GA_Defaults& defaults = GA_Defaults(-1),
            const GA_Storage storage = GA_STORE_INT32,
            const exint subscribeRatio = 64,
            const exint minGrainSize = 256
        )
    {
        return addAttribVertexVertexPrimNext(geo, nameNext, geoGroup, defaults, storage, nullptr, nullptr, GA_ReuseStrategy(), subscribeRatio, minGrainSize);
    }








    //GA_FeE_Adjacency::addAttribVertexPointDst(geo, name, geoGroup, defaults, creation_args, attribute_options, storage, reuse, subscribeRatio, minGrainSize);

    static GA_Attribute*
        addAttribVertexPointDst(
            GEO_Detail* geo,
            const UT_StringHolder& name = "__topo_dstpt",
            const GA_VertexGroup* geoGroup = nullptr,
            const GA_Defaults& defaults = GA_Defaults(-1),
            const GA_Storage storage = GA_STORE_INT32,
            const UT_Options* creation_args = nullptr,
            const GA_AttributeOptions* attribute_options = nullptr,
            const GA_ReuseStrategy& reuse = GA_ReuseStrategy(),
            const exint subscribeRatio = 64,
            const exint minGrainSize = 64
        )
    {
        GA_Attribute* attribPtr = geo->findVertexAttribute(GA_SCOPE_PRIVATE, name);
        if (attribPtr)
            return attribPtr;
        attribPtr = geo->addIntTuple(GA_ATTRIB_VERTEX, GA_SCOPE_PRIVATE, name, 1, defaults, creation_args, attribute_options, storage, reuse);

        GA_Attribute* refAttrib = geo->findVertexAttribute(GA_SCOPE_PRIVATE, "__topo_vtxPrimNext");
        if (refAttrib)
        {
            //GA_Attribute* vtxPrimNextAttrib = addAttribVertexVertexPrimNext(geo, "__topo_vtxPrimNext", geoGroup, GA_Defaults(-1), storage, nullptr);
            vertexPointDst(geo, attribPtr, refAttrib, geoGroup, subscribeRatio, minGrainSize);
            return attribPtr;
        }

        refAttrib = geo->findVertexAttribute(GA_SCOPE_PRIVATE, "__topo_vtxpnum");
        if (refAttrib)
        {
            //GA_Attribute* vtxPrimNextAttrib = addAttribVertexPrimIndex(geo, "__topo_vtxPrimNext", geoGroup, GA_Defaults(-1), storage, nullptr);
            vertexPointDstByVtxpnum(geo, attribPtr, refAttrib, geoGroup, subscribeRatio, minGrainSize);
            return attribPtr;
        }

        vertexPointDst(geo, attribPtr, geoGroup, subscribeRatio, minGrainSize);
        return attribPtr;
    }


    //GA_FeE_Adjacency::addAttribVertexPointDst(geo, name, geoGroup, defaults, storage, subscribeRatio, minGrainSize);

    SYS_FORCE_INLINE
        static GA_Attribute*
        addAttribVertexPointDst(
            GEO_Detail* geo,
            const UT_StringHolder& name = "__topo_dstpt",
            const GA_VertexGroup* geoGroup = nullptr,
            const GA_Defaults& defaults = GA_Defaults(-1),
            const GA_Storage storage = GA_STORE_INT32,
            const exint subscribeRatio = 64,
            const exint minGrainSize = 64
        )
    {
        return addAttribVertexPointDst(geo, name, geoGroup, defaults, storage, nullptr, nullptr, GA_ReuseStrategy(), subscribeRatio, minGrainSize);
    }






} // End of namespace GA_FeE_TopologyReference

#endif
