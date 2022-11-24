
#pragma once

#ifndef __GA_FeE_VertexNextEquiv_h__
#define __GA_FeE_VertexNextEquiv_h__

//#include <GA_FeE/GA_FeE_VertexNextEquiv.h>

#include <GEO/GEO_Detail.h>

#include <GA_FeE/GA_FeE_TopologyReference.h>

namespace GA_FeE_VertexNextEquiv {


    //Get Vertex Next Equiv Vertex
    static GA_Offset
        vertexNextEquivNoLoop(
            const GA_Detail* geo,
            const GA_Offset& vtxoff,
            const GA_Offset& dstpt
        )
    {
        if (dstpt < 0)
            return GA_INVALID_OFFSET;

        GA_Offset vtxoff_next;
        GA_Offset nextVtxDstpt;
        for (vtxoff_next = geo->vertexToNextVertex(vtxoff); vtxoff_next != GA_INVALID_OFFSET; vtxoff_next = geo->vertexToNextVertex(vtxoff_next))
        {
            nextVtxDstpt = GA_FeE_TopologyReference::vertexPointDst(geo, vtxoff_next);
            if (nextVtxDstpt == dstpt)
                return vtxoff_next;
        }

        const GA_Offset& ptnum = geo->vertexPoint(vtxoff);
        for (vtxoff_next = geo->pointVertex(dstpt); vtxoff_next != GA_INVALID_OFFSET; vtxoff_next = geo->vertexToNextVertex(vtxoff_next))
        {
            nextVtxDstpt = GA_FeE_TopologyReference::vertexPointDst(geo, vtxoff_next);
            if (nextVtxDstpt == ptnum)
            {
                if (dstpt < ptnum)
                    return GA_INVALID_OFFSET;
                return vtxoff_next;
            }
        }
        return GA_INVALID_OFFSET;
    }

    static GA_Offset
        vertexNextEquivNoLoop(
            const GA_Detail* geo,
            const GA_Offset& vtxoff,
            const GA_Offset& primoff,
            const GA_Size& vtxpnum
        )
    {
        return vertexNextEquivNoLoop(geo, vtxoff, GA_FeE_TopologyReference::vertexPointDst(geo, primoff, vtxpnum));
    }

    static GA_Offset
        vertexNextEquivNoLoop(
            const GA_Detail * geo,
            const GA_Offset & vtxoff
        )
    {
        const GA_Offset& dstpt = GA_FeE_TopologyReference::vertexPointDst(geo, vtxoff);
        return vertexNextEquivNoLoop(geo, vtxoff, dstpt);
    }







    //Get Vertex Next Equiv Vertex
    static GA_Offset
        vertexNextEquiv(
            const GA_Detail* geo,
            const GA_Offset& vtxoff,
            const GA_Offset& dstpt
        )
    {
        if (dstpt < 0)
            return GA_INVALID_OFFSET;

        GA_Offset vtxoff_next;
        GA_Offset nextVtxDstpt;
        for (vtxoff_next = geo->vertexToNextVertex(vtxoff); vtxoff_next != GA_INVALID_OFFSET; vtxoff_next = geo->vertexToNextVertex(vtxoff_next))
        {
            nextVtxDstpt = GA_FeE_TopologyReference::vertexPointDst(geo, vtxoff_next);
            if (nextVtxDstpt == dstpt)
                return vtxoff_next;
        }

        const GA_Offset& ptnum = geo->vertexPoint(vtxoff);
        for (vtxoff_next = geo->pointVertex(dstpt); vtxoff_next != GA_INVALID_OFFSET; vtxoff_next = geo->vertexToNextVertex(vtxoff_next))
        {
            nextVtxDstpt = GA_FeE_TopologyReference::vertexPointDst(geo, vtxoff_next);
            if (nextVtxDstpt == ptnum)
            {
                return vtxoff_next;
            }
        }
        return GA_INVALID_OFFSET;
    }

    static GA_Offset
        vertexNextEquiv(
            const GA_Detail* geo,
            const GA_Offset& vtxoff,
            const GA_Offset& primoff,
            const GA_Size& vtxpnum
        )
    {
        return vertexNextEquiv(geo, vtxoff, GA_FeE_TopologyReference::vertexPointDst(geo, primoff, vtxpnum));
    }

    static GA_Offset
        vertexNextEquiv(
            const GA_Detail* geo,
            const GA_Offset& vtxoff
        )
    {
        const GA_Offset& dstpt = GA_FeE_TopologyReference::vertexPointDst(geo, vtxoff);
        return vertexNextEquiv(geo, vtxoff, dstpt);
    }

















    //Get all vertices NextEquiv Vertex
    //template<typename T>
    static void
        vertexNextEquivNoLoop(
            const GA_Detail* geo,
            const GA_RWHandleT<GA_Offset>& attribHandle,
            const GA_VertexGroup* geoGroup = nullptr,
            const exint& subscribeRatio = 64,
            const exint& minGrainSize = 64
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
                    attribHandle.set(elemoff, vertexNextEquivNoLoop(geo, elemoff));
                }
            }
        }, subscribeRatio, minGrainSize);
    }




    //Get all vertices NextEquiv Vertex
    //template<typename T>
    static void
        vertexNextEquiv(
            const GA_Detail* geo,
            const GA_RWHandleT<GA_Offset>& attribHandle,
            const GA_VertexGroup* geoGroup = nullptr,
            const exint& subscribeRatio = 64,
            const exint& minGrainSize = 64
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
                        attribHandle.set(elemoff, vertexNextEquiv(geo, elemoff));
                    }
                }
            }, subscribeRatio, minGrainSize);
    }







    //Get all vertices NextEquiv Vertex
    static void
        vertexNextEquivNoLoop(
            const GA_Detail* geo,
            const GA_RWHandleT<GA_Offset>& attribHandle,
            GA_VertexGroup* validGroup,
            const GA_ROHandleT<GA_Offset>& dstptAttribHandle,
            const GA_VertexGroup* geoGroup = nullptr,
            const exint& subscribeRatio = 64,
            const exint& minGrainSize = 64
        )
    {
        const GA_SplittableRange geo0SplittableRange0(geo->getVertexRange(geoGroup));
        UTparallelFor(geo0SplittableRange0, [&geo, &attribHandle, &dstptAttribHandle, &validGroup](const GA_SplittableRange& r)
        {
            GA_Offset vtxoff_next;
            GA_Offset start, end;
            for (GA_Iterator it(r); it.blockAdvance(start, end); )
            {
                for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                {
                    GA_Offset dstpt = dstptAttribHandle.get(elemoff);
                    if (dstpt < 0)
                    {
                        attribHandle.set(elemoff, GA_INVALID_OFFSET);
                        validGroup->setElement(elemoff, true);
                        continue;
                    }

                    for (vtxoff_next = geo->vertexToNextVertex(elemoff); vtxoff_next != GA_INVALID_OFFSET; vtxoff_next = geo->vertexToNextVertex(vtxoff_next))
                    {
                        if (dstptAttribHandle.get(vtxoff_next) != dstpt)
                            continue;
                        dstpt = GA_INVALID_OFFSET;
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
                        dstpt = GA_INVALID_OFFSET;
                        attribHandle.set(elemoff, vtxoff_next);
                    }
                    if (dstpt > 0)
                    {
                        attribHandle.set(elemoff, GA_INVALID_OFFSET);
                        validGroup->setElement(elemoff, true);
                    }
                }
            }
        }, subscribeRatio, minGrainSize);
        validGroup->invalidateGroupEntries();
    }



    //Get all vertices NextEquiv Vertex
    static void
        vertexNextEquivNoLoop(
            const GA_Detail* geo,
            const GA_RWHandleT<GA_Offset>& attribHandle,
            const GA_ROHandleT<GA_Offset>& dstptAttribHandle,
            const GA_VertexGroup* geoGroup = nullptr,
            const exint& subscribeRatio = 64,
            const exint& minGrainSize = 64
        )
    {
        const GA_SplittableRange geo0SplittableRange0(geo->getVertexRange(geoGroup));
        UTparallelFor(geo0SplittableRange0, [&geo, &attribHandle, &dstptAttribHandle](const GA_SplittableRange& r)
        {
            GA_Offset vtxoff_next;
            GA_Offset start, end;
            for (GA_Iterator it(r); it.blockAdvance(start, end); )
            {
                for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                {
                    GA_Offset dstpt = dstptAttribHandle.get(elemoff);
                    if (dstpt < 0)
                    {
                        attribHandle.set(elemoff, GA_INVALID_OFFSET);
                        continue;
                    }

                    for (vtxoff_next = geo->vertexToNextVertex(elemoff); vtxoff_next != GA_INVALID_OFFSET; vtxoff_next = geo->vertexToNextVertex(vtxoff_next))
                    {
                        if (dstptAttribHandle.get(vtxoff_next) != dstpt)
                            continue;
                        dstpt = GA_INVALID_OFFSET;
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
                        dstpt = GA_INVALID_OFFSET;
                        attribHandle.set(elemoff, vtxoff_next);
                    }
                    if (dstpt > 0)
                        attribHandle.set(elemoff, GA_INVALID_OFFSET);
                }
            }
        }, subscribeRatio, minGrainSize);
    }



    //Get all vertices NextEquiv Vertex
    static void
        vertexNextEquivNoLoop(
            const GA_Detail* geo,
            GA_VertexGroup* validGroup,
            const GA_ROHandleT<GA_Offset>& dstptAttribHandle,
            const GA_VertexGroup* geoGroup = nullptr,
            const exint& subscribeRatio = 64,
            const exint& minGrainSize = 64
        )
    {
        const GA_SplittableRange geo0SplittableRange0(geo->getVertexRange(geoGroup));
        UTparallelFor(geo0SplittableRange0, [&geo, &dstptAttribHandle, &validGroup](const GA_SplittableRange& r)
        {
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
                        dstpt = GA_INVALID_OFFSET;
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
                        dstpt = GA_INVALID_OFFSET;
                    }
                    if (dstpt > 0)
                        validGroup->setElement(elemoff, true);
                }
            }
        }, subscribeRatio, minGrainSize);

        validGroup->invalidateGroupEntries();
    }







    //Get all vertices NextEquiv Vertex
    static void
        vertexNextEquiv(
            const GA_Detail* geo,
            const GA_RWHandleT<GA_Offset>& attribHandle,
            GA_VertexGroup* validGroup,
            const GA_ROHandleT<GA_Offset>& dstptAttribHandle,
            const GA_VertexGroup* geoGroup = nullptr,
            const exint& subscribeRatio = 64,
            const exint& minGrainSize = 64
        )
    {
        const GA_SplittableRange geo0SplittableRange0(geo->getVertexRange(geoGroup));
        UTparallelFor(geo0SplittableRange0, [&geo, &attribHandle, &dstptAttribHandle, &validGroup](const GA_SplittableRange& r)
        {
            GA_Offset vtxoff_next;
            GA_Offset start, end;
            for (GA_Iterator it(r); it.blockAdvance(start, end); )
            {
                for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                {
                    GA_Offset dstpt = dstptAttribHandle.get(elemoff);
                    if (dstpt < 0)
                    {
                        attribHandle.set(elemoff, GA_INVALID_OFFSET);
                        validGroup->setElement(elemoff, true);
                        continue;
                    }

                    for (vtxoff_next = geo->vertexToNextVertex(elemoff); vtxoff_next != GA_INVALID_OFFSET; vtxoff_next = geo->vertexToNextVertex(vtxoff_next))
                    {
                        if (dstptAttribHandle.get(vtxoff_next) != dstpt)
                            continue;
                        dstpt = GA_INVALID_OFFSET;
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
                        dstpt = GA_INVALID_OFFSET;
                        attribHandle.set(elemoff, vtxoff_next);
                    }
                    if (dstpt > 0)
                    {
                        attribHandle.set(elemoff, GA_INVALID_OFFSET);
                        validGroup->setElement(elemoff, true);
                    }
                }
            }
        }, subscribeRatio, minGrainSize);
        validGroup->invalidateGroupEntries();
    }



    //Get all vertices NextEquiv Vertex
    static void
        vertexNextEquiv(
            const GA_Detail* geo,
            const GA_RWHandleT<GA_Offset>& attribHandle,
            const GA_ROHandleT<GA_Offset>& dstptAttribHandle,
            const GA_VertexGroup* geoGroup = nullptr,
            const exint& subscribeRatio = 64,
            const exint& minGrainSize = 64
        )
    {
        const GA_SplittableRange geo0SplittableRange0(geo->getVertexRange(geoGroup));
        UTparallelFor(geo0SplittableRange0, [&geo, &attribHandle, &dstptAttribHandle](const GA_SplittableRange& r)
        {
            GA_Offset vtxoff_next;
            GA_Offset start, end;
            for (GA_Iterator it(r); it.blockAdvance(start, end); )
            {
                for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                {
                    GA_Offset dstpt = dstptAttribHandle.get(elemoff);
                    if (dstpt < 0)
                    {
                        attribHandle.set(elemoff, GA_INVALID_OFFSET);
                        continue;
                    }

                    for (vtxoff_next = geo->vertexToNextVertex(elemoff); vtxoff_next != GA_INVALID_OFFSET; vtxoff_next = geo->vertexToNextVertex(vtxoff_next))
                    {
                        if (dstptAttribHandle.get(vtxoff_next) != dstpt)
                            continue;
                        dstpt = GA_INVALID_OFFSET;
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
                        dstpt = GA_INVALID_OFFSET;
                        attribHandle.set(elemoff, vtxoff_next);
                    }
                    if (dstpt > 0)
                        attribHandle.set(elemoff, GA_INVALID_OFFSET);
                }
            }
        }, subscribeRatio, minGrainSize);
    }



    //Get all vertices NextEquiv Vertex
    static void
        vertexNextEquiv(
            const GA_Detail* geo,
            GA_VertexGroup* validGroup,
            const GA_ROHandleT<GA_Offset>& dstptAttribHandle,
            const GA_VertexGroup* geoGroup = nullptr,
            const exint& subscribeRatio = 64,
            const exint& minGrainSize = 64
        )
    {
        const GA_SplittableRange geo0SplittableRange0(geo->getVertexRange(geoGroup));
        UTparallelFor(geo0SplittableRange0, [&geo, &dstptAttribHandle, &validGroup](const GA_SplittableRange& r)
        {
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
                        dstpt = GA_INVALID_OFFSET;
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
                        dstpt = GA_INVALID_OFFSET;
                    }
                    if (dstpt > 0)
                        validGroup->setElement(elemoff, true);
                }
            }
        }, subscribeRatio, minGrainSize);

        validGroup->invalidateGroupEntries();
    }












    //GA_FeE_VertexNextEquiv::addAttribVertexNextEquivNoLoop(geo, name, groupName, name, geoGroup, defaults, creation_args, attribute_options, storage, reuse, subscribeRatio, minGrainSize);

    static GA_Attribute*
        addAttribVertexNextEquivNoLoop(
            GEO_Detail* geo,
            GA_VertexGroup* validGroup,
            const UT_StringHolder& name = "nextEquivNoLoop",
            const UT_StringHolder& groupName = "nextEquivValid",
            const GA_VertexGroup* geoGroup = nullptr,
            const GA_Defaults& defaults = GA_Defaults(-1),
            const UT_Options* creation_args = 0,
            const GA_AttributeOptions* attribute_options = 0,
            const GA_Storage& storage = GA_STORE_INT64,
            const GA_ReuseStrategy& reuse = GA_ReuseStrategy(),
            const exint& subscribeRatio = 32,
            const exint& minGrainSize = 1024
        )
    {
        GA_Attribute* attribPtr = geo->findVertexAttribute(name);
        if (attribPtr && geo->findVertexGroup(name))
            return attribPtr;

        attribPtr = geo->addIntTuple(GA_ATTRIB_VERTEX, name, 1, defaults, creation_args, attribute_options, storage, reuse);

        validGroup = geo->newVertexGroup(groupName);
        GA_RWHandleT<GA_Size> refAttribHandle = GA_FeE_TopologyReference::addAttribVertexPointDst(geo, "dstpt", geoGroup, GA_Defaults(-1), storage, reuse);
        vertexNextEquivNoLoop(geo, attribPtr, validGroup, refAttribHandle, geoGroup, subscribeRatio, minGrainSize);
        return attribPtr;
    }

    //GA_FeE_VertexNextEquiv::addAttribVertexNextEquivNoLoop(geo, validGroup, name, groupName, posAttribHandle, name, geoGroup, defaults, storage, reuse, subscribeRatio, minGrainSize);

    SYS_FORCE_INLINE
        static GA_Attribute*
        addAttribVertexNextEquivNoLoop(
            GEO_Detail* geo,
            GA_VertexGroup* validGroup,
            const UT_StringHolder& name = "nextEquivNoLoop",
            const UT_StringHolder& groupName = "nextEquivValid",
            const GA_VertexGroup* geoGroup = nullptr,
            const GA_Defaults& defaults = GA_Defaults(-1),
            const GA_Storage& storage = GA_STORE_INT64,
            const GA_ReuseStrategy& reuse = GA_ReuseStrategy(),
            const exint& subscribeRatio = 32,
            const exint& minGrainSize = 1024
        )
    {
        return addAttribVertexNextEquivNoLoop(geo, validGroup, name, groupName, geoGroup, defaults, 0, 0, storage, reuse, subscribeRatio, minGrainSize);
    }

    //GA_FeE_VertexNextEquiv::addAttribVertexNextEquivNoLoop(geo, validGroup, name, groupName, posAttribHandle, name, geoGroup, defaults, storage, subscribeRatio, minGrainSize);

    SYS_FORCE_INLINE
        static GA_Attribute*
        addAttribVertexNextEquivNoLoop(
            GEO_Detail* geo,
            GA_VertexGroup* validGroup,
            const UT_StringHolder& name = "nextEquivNoLoop",
            const UT_StringHolder& groupName = "nextEquivValid",
            const GA_VertexGroup* geoGroup = nullptr,
            const GA_Defaults& defaults = GA_Defaults(-1),
            const GA_Storage& storage = GA_STORE_INT64,
            const exint& subscribeRatio = 32,
            const exint& minGrainSize = 1024
        )
    {
        return addAttribVertexNextEquivNoLoop(geo, validGroup, name, groupName, geoGroup, defaults, 0, 0, storage, GA_ReuseStrategy(), subscribeRatio, minGrainSize);
    }




    //GA_FeE_VertexNextEquiv::addAttribVertexNextEquivNoLoop(geo, name, geoGroup, storage, subscribeRatio, minGrainSize);

    static GA_VertexGroup*
        addAttribVertexNextEquivNoLoop(
            GEO_Detail* geo,
            const UT_StringHolder& name = "nextEquivValid",
            const GA_VertexGroup* geoGroup = nullptr,
            const GA_Storage& storage = GA_STORE_INT64,
            const exint& subscribeRatio = 32,
            const exint& minGrainSize = 1024
        )
    {
        GA_VertexGroup* groupPtr = geo->findVertexGroup(name);
        if (groupPtr)
            return groupPtr;

        groupPtr = geo->newVertexGroup(name);
        GA_RWHandleT<GA_Size> refAttribHandle = GA_FeE_TopologyReference::addAttribVertexPointDst(geo, "dstpt", geoGroup, GA_Defaults(-1), storage, GA_ReuseStrategy());
        vertexNextEquivNoLoop(geo, groupPtr, refAttribHandle, geoGroup, subscribeRatio, minGrainSize);
        return groupPtr;
    }






    //GA_FeE_VertexNextEquiv::addAttribVertexNextEquiv(geo, name, geoGroup, defaults, creation_args, attribute_options, storage, reuse, subscribeRatio, minGrainSize);

    static GA_Attribute*
        addAttribVertexNextEquivNoLoop(
            GEO_Detail* geo,
            const UT_StringHolder& name = "nextEquivNoLoop",
            const GA_VertexGroup* geoGroup = nullptr,
            const GA_Defaults& defaults = GA_Defaults(-1),
            const UT_Options* creation_args = 0,
            const GA_AttributeOptions* attribute_options = 0,
            const GA_Storage& storage = GA_STORE_INT64,
            const GA_ReuseStrategy& reuse = GA_ReuseStrategy(),
            const exint& subscribeRatio = 32,
            const exint& minGrainSize = 1024
        )
    {
        GA_Attribute* attribPtr = geo->findVertexAttribute(name);
        if (attribPtr)
            return attribPtr;
        attribPtr = geo->addIntTuple(GA_ATTRIB_VERTEX, name, 1, defaults, creation_args, attribute_options, storage, reuse);
        GA_RWHandleT<GA_Size> refAttribHandle = GA_FeE_TopologyReference::addAttribVertexPointDst(geo, "dstpt", geoGroup, GA_Defaults(-1), storage, reuse);
        vertexNextEquivNoLoop(geo, attribPtr, refAttribHandle, geoGroup, subscribeRatio, minGrainSize);
        return attribPtr;
    }

    //GA_FeE_VertexNextEquiv::addAttribVertexNextEquiv(geo, posAttribHandle, name, geoGroup, defaults, storage, reuse, subscribeRatio, minGrainSize);

    SYS_FORCE_INLINE
        static GA_Attribute*
        addAttribVertexNextEquivNoLoop(
            GEO_Detail* geo,
            const UT_StringHolder& name = "nextEquivNoLoop",
            const GA_VertexGroup* geoGroup = nullptr,
            const GA_Defaults& defaults = GA_Defaults(-1),
            const GA_Storage& storage = GA_STORE_INT64,
            const GA_ReuseStrategy& reuse = GA_ReuseStrategy(),
            const exint& subscribeRatio = 32,
            const exint& minGrainSize = 1024
        )
    {
        return addAttribVertexNextEquivNoLoop(geo, name, geoGroup, defaults, 0, 0, storage, reuse, subscribeRatio, minGrainSize);
    }

    //GA_FeE_VertexNextEquiv::addAttribVertexNextEquiv(geo, posAttribHandle, name, geoGroup, defaults, storage, subscribeRatio, minGrainSize);

    SYS_FORCE_INLINE
        static GA_Attribute*
        addAttribVertexNextEquivNoLoop(
            GEO_Detail* geo,
            const UT_StringHolder& name = "nextEquivNoLoop",
            const GA_VertexGroup* geoGroup = nullptr,
            const GA_Defaults& defaults = GA_Defaults(-1),
            const GA_Storage& storage = GA_STORE_INT64,
            const exint& subscribeRatio = 32,
            const exint& minGrainSize = 1024
        )
    {
        return addAttribVertexNextEquivNoLoop(geo, name, geoGroup, defaults, 0, 0, storage, GA_ReuseStrategy(), subscribeRatio, minGrainSize);
    }








    //GA_FeE_VertexNextEquiv::addAttribVertexNextEquiv(geo, name, geoGroup, defaults, creation_args, attribute_options, storage, reuse, subscribeRatio, minGrainSize);

    static GA_Attribute*
        addAttribVertexNextEquiv(
            GEO_Detail* geo,
            const UT_StringHolder& name = "nextEquiv",
            const GA_VertexGroup* geoGroup = nullptr,
            const GA_Defaults& defaults = GA_Defaults(-1),
            const UT_Options* creation_args = 0,
            const GA_AttributeOptions* attribute_options = 0,
            const GA_Storage& storage = GA_STORE_INT64,
            const GA_ReuseStrategy& reuse = GA_ReuseStrategy(),
            const exint& subscribeRatio = 32,
            const exint& minGrainSize = 1024
        )
    {
        GA_Attribute* attribPtr = geo->findVertexAttribute(name);
        if (attribPtr)
            return attribPtr;
        attribPtr = geo->addIntTuple(GA_ATTRIB_VERTEX, name, 1, defaults, creation_args, attribute_options, storage, reuse);
        GA_RWHandleT<GA_Size> refAttribHandle = GA_FeE_TopologyReference::addAttribVertexPointDst(geo, "dstpt", geoGroup, GA_Defaults(-1), storage, reuse);
        vertexNextEquiv(geo, attribPtr, refAttribHandle, geoGroup, subscribeRatio, minGrainSize);
        return attribPtr;
    }

    //GA_FeE_VertexNextEquiv::addAttribVertexNextEquiv(geo, posAttribHandle, name, geoGroup, defaults, storage, reuse, subscribeRatio, minGrainSize);

    SYS_FORCE_INLINE
        static GA_Attribute*
        addAttribVertexNextEquiv(
            GEO_Detail* geo,
            const UT_StringHolder& name = "nextEquiv",
            const GA_VertexGroup* geoGroup = nullptr,
            const GA_Defaults& defaults = GA_Defaults(-1),
            const GA_Storage& storage = GA_STORE_INT64,
            const GA_ReuseStrategy& reuse = GA_ReuseStrategy(),
            const exint& subscribeRatio = 32,
            const exint& minGrainSize = 1024
        )
    {
        return addAttribVertexNextEquiv(geo, name, geoGroup, defaults, 0, 0, storage, reuse, subscribeRatio, minGrainSize);
    }

    //GA_FeE_VertexNextEquiv::addAttribVertexNextEquiv(geo, posAttribHandle, name, geoGroup, defaults, storage, subscribeRatio, minGrainSize);

    SYS_FORCE_INLINE
        static GA_Attribute*
        addAttribVertexNextEquiv(
            GEO_Detail* geo,
            const UT_StringHolder& name = "nextEquiv",
            const GA_VertexGroup* geoGroup = nullptr,
            const GA_Defaults& defaults = GA_Defaults(-1),
            const GA_Storage& storage = GA_STORE_INT64,
            const exint& subscribeRatio = 32,
            const exint& minGrainSize = 1024
        )
    {
        return addAttribVertexNextEquiv(geo, name, geoGroup, defaults, 0, 0, storage, GA_ReuseStrategy(), subscribeRatio, minGrainSize);
    }

    




} // End of namespace GA_FeE_VertexNextEquiv

#endif
