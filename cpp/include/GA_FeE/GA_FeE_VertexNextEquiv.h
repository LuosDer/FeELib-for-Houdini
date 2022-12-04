
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
            GA_Detail* geo,
            const GA_Offset vtxoff,
            const GA_Offset dstpt
        )
    {
        GA_Topology& topo = geo->getTopology();
        const GA_ATITopology* vtxPointRef = topo.getPointRef();
        const GA_ATITopology* pointVtxRef = topo.getVertexRef();
        const GA_ATITopology* vtxNextRef = topo.getVertexNextRef();

        GA_Offset vtxoff_next, ptnum;
        if (dstpt < 0)
        {
            return GA_INVALID_OFFSET;
        }

        for (vtxoff_next = vtxNextRef->getLink(vtxoff); vtxoff_next != GA_INVALID_OFFSET; vtxoff_next = vtxNextRef->getLink(vtxoff_next))
        {
            if (GA_FeE_TopologyReference::vertexPointDst(geo, vtxoff_next) != dstpt)
                continue;
            return vtxoff_next;
        }

        ptnum = vtxPointRef->getLink(vtxoff);
        for (vtxoff_next = pointVtxRef->getLink(dstpt); vtxoff_next != GA_INVALID_OFFSET; vtxoff_next = vtxNextRef->getLink(vtxoff_next))
        {
            if (GA_FeE_TopologyReference::vertexPointDst(geo, vtxoff_next) != ptnum)
                continue;
            if (dstpt > ptnum)
            {
                return vtxoff_next;
            }
            break;
        }
        return GA_INVALID_OFFSET;
    }


    static GA_Offset
        vertexNextEquivNoLoop(
            GA_Detail* geo,
            const GA_Offset vtxoff,
            const GA_Offset primoff,
            const GA_Size vtxpnum
        )
    {
        return vertexNextEquivNoLoop(geo, vtxoff, GA_FeE_TopologyReference::vertexPointDst(geo, primoff, vtxpnum));
    }

    static GA_Offset
        vertexNextEquivNoLoop(
            GA_Detail * geo,
            const GA_Offset vtxoff
        )
    {
        return vertexNextEquivNoLoop(geo, vtxoff, GA_FeE_TopologyReference::vertexPointDst(geo, vtxoff));
    }







    //Get Vertex Next Equiv Vertex
    static GA_Offset
        vertexNextEquiv(
            GA_Detail* geo,
            const GA_Offset vtxoff,
            const GA_Offset dstpt
        )
    {
        GA_Topology& topo = geo->getTopology();
        const GA_ATITopology* vtxPointRef = topo.getPointRef();
        const GA_ATITopology* pointVtxRef = topo.getVertexRef();
        const GA_ATITopology* vtxNextRef = topo.getVertexNextRef();

        GA_Offset vtxoff_next, ptnum;
        if (dstpt < 0)
        {
            return GA_INVALID_OFFSET;
        }

        for (vtxoff_next = vtxNextRef->getLink(vtxoff); vtxoff_next != GA_INVALID_OFFSET; vtxoff_next = vtxNextRef->getLink(vtxoff_next))
        {
            if (GA_FeE_TopologyReference::vertexPointDst(geo, vtxoff_next) != dstpt)
                continue;
            return vtxoff_next;
        }

        ptnum = vtxPointRef->getLink(vtxoff);
        for (vtxoff_next = pointVtxRef->getLink(dstpt); vtxoff_next != GA_INVALID_OFFSET; vtxoff_next = vtxNextRef->getLink(vtxoff_next))
        {
            if (GA_FeE_TopologyReference::vertexPointDst(geo, vtxoff_next) != ptnum)
                continue;
            return vtxoff_next;
        }
        return GA_INVALID_OFFSET;
    }





    static GA_Offset
        vertexNextEquiv(
            GA_Detail* geo,
            const GA_Offset vtxoff,
            const GA_Offset primoff,
            const GA_Size vtxpnum
        )
    {
        return vertexNextEquiv(geo, vtxoff, GA_FeE_TopologyReference::vertexPointDst(geo, primoff, vtxpnum));
    }

    static GA_Offset
        vertexNextEquiv(
            GA_Detail* geo,
            const GA_Offset vtxoff
        )
    {
        return vertexNextEquiv(geo, vtxoff, GA_FeE_TopologyReference::vertexPointDst(geo, vtxoff));
    }

















    //Get all vertices NextEquiv Vertex
    //template<typename T>
    static void
        vertexNextEquivNoLoop(
            GA_Detail* geo,
            const GA_RWHandleT<GA_Offset>& attribHandle,
            const GA_VertexGroup* geoGroup = nullptr,
            const exint subscribeRatio = 64,
            const exint minGrainSize = 64
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
            GA_Detail* geo,
            const GA_RWHandleT<GA_Offset>& attribHandle,
            const GA_VertexGroup* geoGroup = nullptr,
            const exint subscribeRatio = 64,
            const exint minGrainSize = 64
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
            GA_Detail* geo,
            const GA_RWHandleT<GA_Offset>& attribHandle,
            GA_VertexGroup* validGroup,
            const GA_ROHandleT<GA_Offset>& dstptAttribHandle,
            const GA_VertexGroup* geoGroup = nullptr,
            const exint subscribeRatio = 64,
            const exint minGrainSize = 64
        )
    {
        GA_Topology& topo = geo->getTopology();
        topo.makeVertexRef();
        const GA_ATITopology* vtxPointRef = topo.getPointRef();
        const GA_ATITopology* pointVtxRef = topo.getVertexRef();
        const GA_ATITopology* vtxNextRef = topo.getVertexNextRef();

        const GA_SplittableRange geo0SplittableRange0(geo->getVertexRange(geoGroup));
        UTparallelFor(geo0SplittableRange0, 
            [&geo, &attribHandle, &dstptAttribHandle, &validGroup,
            &vtxPointRef, &pointVtxRef, &vtxNextRef]
            (const GA_SplittableRange& r)
            {
                GA_Offset vtxoff_next, dstpt, ptnum;
                GA_Offset start, end;
                for (GA_Iterator it(r); it.blockAdvance(start, end); )
                {
                    for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                    {
                        dstpt = dstptAttribHandle.get(elemoff);
                        if (dstpt < 0)
                        {
                            attribHandle.set(elemoff, GA_INVALID_OFFSET);
                            //validGroup->setElement(elemoff, true);
                            continue;
                        }

                        for (vtxoff_next = vtxNextRef->getLink(elemoff); vtxoff_next != GA_INVALID_OFFSET; vtxoff_next = vtxNextRef->getLink(vtxoff_next))
                        {
                            if (dstptAttribHandle.get(vtxoff_next) != dstpt)
                                continue;
                            dstpt = GA_INVALID_OFFSET;
                            attribHandle.set(elemoff, vtxoff_next);
                            break;
                        }

                        if (dstpt < 0)
                            continue;

                        ptnum = vtxPointRef->getLink(elemoff);
                        for (vtxoff_next = pointVtxRef->getLink(dstpt); vtxoff_next != GA_INVALID_OFFSET; vtxoff_next = vtxNextRef->getLink(vtxoff_next))
                        {
                            if (dstptAttribHandle.get(vtxoff_next) != ptnum)
                                continue;
                            if (dstpt > ptnum)
                            {
                                dstpt = GA_INVALID_OFFSET;
                                attribHandle.set(elemoff, vtxoff_next);
                            }
                            break;
                        }
                        if (dstpt >= 0)
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
            GA_Detail* geo,
            GA_VertexGroup* validGroup,
            const GA_ROHandleT<GA_Offset>& dstptAttribHandle,
            const GA_VertexGroup* geoGroup = nullptr,
            const exint subscribeRatio = 64,
            const exint minGrainSize = 64
        )
    {
        GA_Topology& topo = geo->getTopology();
        topo.makeVertexRef();
        const GA_ATITopology* vtxPointRef = topo.getPointRef();
        const GA_ATITopology* pointVtxRef = topo.getVertexRef();
        const GA_ATITopology* vtxNextRef = topo.getVertexNextRef();

        const GA_SplittableRange geo0SplittableRange0(geo->getVertexRange(geoGroup));
        UTparallelFor(geo0SplittableRange0,
            [&geo, &dstptAttribHandle, &validGroup,
            &vtxPointRef, &pointVtxRef, &vtxNextRef]
        (const GA_SplittableRange& r)
        {
            GA_Offset vtxoff_next, dstpt, ptnum;
            GA_Offset start, end;
            for (GA_Iterator it(r); it.blockAdvance(start, end); )
            {
                for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                {
                    dstpt = dstptAttribHandle.get(elemoff);
                    if (dstpt < 0)
                    {
                        //validGroup->setElement(elemoff, true);
                        continue;
                    }

                    for (vtxoff_next = vtxNextRef->getLink(elemoff); vtxoff_next != GA_INVALID_OFFSET; vtxoff_next = vtxNextRef->getLink(vtxoff_next))
                    {
                        if (dstptAttribHandle.get(vtxoff_next) != dstpt)
                            continue;
                        dstpt = GA_INVALID_OFFSET;
                        break;
                    }

                    if (dstpt < 0)
                        continue;

                    ptnum = vtxPointRef->getLink(elemoff);
                    for (vtxoff_next = pointVtxRef->getLink(dstpt); vtxoff_next != GA_INVALID_OFFSET; vtxoff_next = vtxNextRef->getLink(vtxoff_next))
                    {
                        if (dstptAttribHandle.get(vtxoff_next) != ptnum)
                            continue;
                        if (dstpt > ptnum)
                        {
                            dstpt = GA_INVALID_OFFSET;
                        }
                        break;
                    }
                    if (dstpt >= 0)
                    {
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
            GA_Detail* geo,
            const GA_RWHandleT<GA_Offset>& attribHandle,
            const GA_ROHandleT<GA_Offset>& dstptAttribHandle,
            const GA_VertexGroup* geoGroup = nullptr,
            const exint subscribeRatio = 64,
            const exint minGrainSize = 64
        )
    {
        GA_Topology& topo = geo->getTopology();
        topo.makeVertexRef();
        const GA_ATITopology* vtxPointRef = topo.getPointRef();
        const GA_ATITopology* pointVtxRef = topo.getVertexRef();
        const GA_ATITopology* vtxNextRef = topo.getVertexNextRef();

        const GA_SplittableRange geo0SplittableRange0(geo->getVertexRange(geoGroup));
        UTparallelFor(geo0SplittableRange0,
            [&geo, &attribHandle, &dstptAttribHandle,
            &vtxPointRef, &pointVtxRef, &vtxNextRef]
        (const GA_SplittableRange& r)
        {
            GA_Offset vtxoff_next, dstpt, ptnum;
            GA_Offset start, end;
            for (GA_Iterator it(r); it.blockAdvance(start, end); )
            {
                for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                {
                    dstpt = dstptAttribHandle.get(elemoff);
                    if (dstpt < 0)
                    {
                        attribHandle.set(elemoff, GA_INVALID_OFFSET);
                        continue;
                    }

                    for (vtxoff_next = vtxNextRef->getLink(elemoff); vtxoff_next != GA_INVALID_OFFSET; vtxoff_next = vtxNextRef->getLink(vtxoff_next))
                    {
                        if (dstptAttribHandle.get(vtxoff_next) != dstpt)
                            continue;
                        dstpt = GA_INVALID_OFFSET;
                        attribHandle.set(elemoff, vtxoff_next);
                        break;
                    }

                    if (dstpt < 0)
                        continue;

                    ptnum = vtxPointRef->getLink(elemoff);
                    for (vtxoff_next = pointVtxRef->getLink(dstpt); vtxoff_next != GA_INVALID_OFFSET; vtxoff_next = vtxNextRef->getLink(vtxoff_next))
                    {
                        if (dstptAttribHandle.get(vtxoff_next) != ptnum)
                            continue;
                        if (dstpt > ptnum)
                        {
                            dstpt = GA_INVALID_OFFSET;
                            attribHandle.set(elemoff, vtxoff_next);
                        }
                        break;
                    }
                    if (dstpt >= 0)
                    {
                        attribHandle.set(elemoff, GA_INVALID_OFFSET);
                    }
                }
            }
        }, subscribeRatio, minGrainSize);
    }







    //Get all vertices NextEquiv Vertex
    static void
        vertexNextEquiv(
            GA_Detail* geo,
            const GA_RWHandleT<GA_Offset>& attribHandle,
            GA_VertexGroup* unsharedGroup,
            const GA_ROHandleT<GA_Offset>& dstptAttribHandle,
            const GA_VertexGroup* geoGroup = nullptr,
            const exint subscribeRatio = 64,
            const exint minGrainSize = 64
        )
    {
        GA_Topology& topo = geo->getTopology();
        topo.makeVertexRef();
        const GA_ATITopology* vtxPointRef = topo.getPointRef();
        const GA_ATITopology* pointVtxRef = topo.getVertexRef();
        const GA_ATITopology* vtxNextRef = topo.getVertexNextRef();

        const GA_SplittableRange geo0SplittableRange0(geo->getVertexRange(geoGroup));
        UTparallelFor(geo0SplittableRange0,
            [&geo, &attribHandle, &unsharedGroup, &dstptAttribHandle,
            &vtxPointRef, &pointVtxRef, &vtxNextRef]
            (const GA_SplittableRange& r)
        {
            GA_Offset vtxoff_next, dstpt, ptnum;
            GA_Offset start, end;
            for (GA_Iterator it(r); it.blockAdvance(start, end); )
            {
                for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                {
                    dstpt = dstptAttribHandle.get(elemoff);
                    if (dstpt < 0)
                    {
                        attribHandle.set(elemoff, GA_INVALID_OFFSET);
                        continue;
                    }

                    for (vtxoff_next = vtxNextRef->getLink(elemoff); vtxoff_next != GA_INVALID_OFFSET; vtxoff_next = vtxNextRef->getLink(vtxoff_next))
                    {
                        if (dstptAttribHandle.get(vtxoff_next) != dstpt)
                            continue;
                        dstpt = GA_INVALID_OFFSET;
                        attribHandle.set(elemoff, vtxoff_next);
                        break;
                    }

                    if (dstpt < 0)
                        continue;

                    ptnum = vtxPointRef->getLink(elemoff);
                    for (vtxoff_next = pointVtxRef->getLink(dstpt); vtxoff_next != GA_INVALID_OFFSET; vtxoff_next = vtxNextRef->getLink(vtxoff_next))
                    {
                        if (dstptAttribHandle.get(vtxoff_next) != ptnum)
                            continue;
                        dstpt = GA_INVALID_OFFSET;
                        attribHandle.set(elemoff, vtxoff_next);
                        break;
                    }
                    if (dstpt >= 0)
                    {
                        attribHandle.set(elemoff, GA_INVALID_OFFSET);
                        unsharedGroup->setElement(elemoff, true);
                    }
                }
            }
        }, subscribeRatio, minGrainSize);
    }


    //Get all vertices NextEquiv Vertex
    static void
        vertexNextEquiv(
            GA_Detail* geo,
            const GA_RWHandleT<GA_Offset>& attribHandle,
            const GA_ROHandleT<GA_Offset>& dstptAttribHandle,
            const GA_VertexGroup* geoGroup = nullptr,
            const exint subscribeRatio = 64,
            const exint minGrainSize = 64
        )
    {
        GA_Topology& topo = geo->getTopology();
        topo.makeVertexRef();
        const GA_ATITopology* vtxPointRef = topo.getPointRef();
        const GA_ATITopology* pointVtxRef = topo.getVertexRef();
        const GA_ATITopology* vtxNextRef = topo.getVertexNextRef();

        const GA_SplittableRange geo0SplittableRange0(geo->getVertexRange(geoGroup));
        UTparallelFor(geo0SplittableRange0,
            [&geo, &attribHandle, &dstptAttribHandle,
            &vtxPointRef, &pointVtxRef, &vtxNextRef]
        (const GA_SplittableRange& r)
        {
            GA_Offset vtxoff_next, dstpt, ptnum;
            GA_Offset start, end;
            for (GA_Iterator it(r); it.blockAdvance(start, end); )
            {
                for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                {
                    dstpt = dstptAttribHandle.get(elemoff);
                    if (dstpt < 0)
                    {
                        attribHandle.set(elemoff, GA_INVALID_OFFSET);
                        continue;
                    }

                    for (vtxoff_next = vtxNextRef->getLink(elemoff); vtxoff_next != GA_INVALID_OFFSET; vtxoff_next = vtxNextRef->getLink(vtxoff_next))
                    {
                        if (dstptAttribHandle.get(vtxoff_next) != dstpt)
                            continue;
                        dstpt = GA_INVALID_OFFSET;
                        attribHandle.set(elemoff, vtxoff_next);
                        break;
                    }

                    if (dstpt < 0)
                        continue;

                    ptnum = vtxPointRef->getLink(elemoff);
                    for (vtxoff_next = pointVtxRef->getLink(dstpt); vtxoff_next != GA_INVALID_OFFSET; vtxoff_next = vtxNextRef->getLink(vtxoff_next))
                    {
                        if (dstptAttribHandle.get(vtxoff_next) != ptnum)
                            continue;
                        dstpt = GA_INVALID_OFFSET;
                        attribHandle.set(elemoff, vtxoff_next);
                        break;
                    }
                    if (dstpt >= 0)
                    {
                        attribHandle.set(elemoff, GA_INVALID_OFFSET);
                    }
                }
            }
        }, subscribeRatio, minGrainSize);
    }




    //Get all vertices NextEquiv Vertex
    static void
        vertexNextEquiv(
            GA_Detail* geo,
            GA_VertexGroup* unsharedGroup,
            const GA_ROHandleT<GA_Offset>& dstptAttribHandle,
            const GA_VertexGroup* geoGroup = nullptr,
            const exint subscribeRatio = 64,
            const exint minGrainSize = 64
        )
    {
        GA_Topology& topo = geo->getTopology();
        topo.makeVertexRef();
        const GA_ATITopology* vtxPointRef = topo.getPointRef();
        const GA_ATITopology* pointVtxRef = topo.getVertexRef();
        const GA_ATITopology* vtxNextRef = topo.getVertexNextRef();

        const GA_SplittableRange geo0SplittableRange0(geo->getVertexRange(geoGroup));
        UTparallelFor(geo0SplittableRange0,
            [&geo, &unsharedGroup, &dstptAttribHandle,
            &vtxPointRef, &pointVtxRef, &vtxNextRef]
        (const GA_SplittableRange& r)
        {
            GA_Offset vtxoff_next, dstpt, ptnum;
            GA_Offset start, end;
            for (GA_Iterator it(r); it.blockAdvance(start, end); )
            {
                for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                {
                    dstpt = dstptAttribHandle.get(elemoff);
                    if (dstpt < 0)
                    {
                        continue;
                    }

                    for (vtxoff_next = vtxNextRef->getLink(elemoff); vtxoff_next != GA_INVALID_OFFSET; vtxoff_next = vtxNextRef->getLink(vtxoff_next))
                    {
                        if (dstptAttribHandle.get(vtxoff_next) != dstpt)
                            continue;
                        dstpt = GA_INVALID_OFFSET;
                        break;
                    }

                    if (dstpt < 0)
                        continue;

                    ptnum = vtxPointRef->getLink(elemoff);
                    for (vtxoff_next = pointVtxRef->getLink(dstpt); vtxoff_next != GA_INVALID_OFFSET; vtxoff_next = vtxNextRef->getLink(vtxoff_next))
                    {
                        if (dstptAttribHandle.get(vtxoff_next) != ptnum)
                            continue;
                        dstpt = GA_INVALID_OFFSET;
                        break;
                    }
                    if (dstpt >= 0)
                    {
                        unsharedGroup->setElement(elemoff, true);
                    }
                }
            }
        }, subscribeRatio, minGrainSize);
    }











    //GA_FeE_VertexNextEquiv::addAttribVertexNextEquivNoLoop(geo, name, groupName, name, geoGroup, defaults, creation_args, attribute_options, storage, reuse, subscribeRatio, minGrainSize);

    static GA_Attribute*
        addAttribVertexNextEquivNoLoop(
            GEO_Detail* geo,
            GA_VertexGroup*& validGroup,
            const UT_StringHolder& name = "__topo_nextEquivNoLoop",
            const UT_StringHolder& groupName = "__topo_nextEquivValid",
            const GA_VertexGroup* geoGroup = nullptr,
            const GA_Defaults& defaults = GA_Defaults(-1),
            const GA_Storage storage = GA_STORE_INT32,
            const UT_Options* creation_args = nullptr,
            const GA_AttributeOptions* attribute_options = nullptr,
            const GA_ReuseStrategy& reuse = GA_ReuseStrategy(),
            const exint subscribeRatio = 32,
            const exint minGrainSize = 1024
        )
    {
        GA_Attribute* attribPtr = geo->findVertexAttribute(GA_FEE_TOPO_SCOPE, name);
        if (attribPtr && geo->findVertexGroup(name))
            return attribPtr;
        attribPtr = geo->addIntTuple(GA_ATTRIB_VERTEX, GA_FEE_TOPO_SCOPE, name, 1, defaults, creation_args, attribute_options, storage, reuse);

        validGroup = geo->newVertexGroup(groupName);
        const GA_Attribute* refAttrib = GA_FeE_TopologyReference::addAttribVertexPointDst(geo, geoGroup, storage);
        vertexNextEquivNoLoop(geo, attribPtr, validGroup, refAttrib, geoGroup, subscribeRatio, minGrainSize);
        return attribPtr;
    }







    //GA_FeE_VertexNextEquiv::addGroupVertexNextEquivNoLoop(geo, name, geoGroup, subscribeRatio, minGrainSize);

    static GA_VertexGroup*
        addGroupVertexNextEquivNoLoop(
            GEO_Detail* geo,
            const GA_VertexGroup* geoGroup = nullptr,
            const GA_Storage storage = GA_STORE_INT32,
            const UT_StringHolder& name = "__topo_nextEquivValid",
            const exint subscribeRatio = 32,
            const exint minGrainSize = 1024
        )
    {
        GA_VertexGroup* groupPtr = geo->findVertexGroup(name);
        if (groupPtr)
            return groupPtr;

        groupPtr = geo->newVertexGroup(name);
        const GA_Attribute* refAttrib = GA_FeE_TopologyReference::addAttribVertexPointDst(geo, geoGroup, storage);
        vertexNextEquivNoLoop(geo, groupPtr, refAttrib, geoGroup, subscribeRatio, minGrainSize);
        return groupPtr;
    }






    //GA_FeE_VertexNextEquiv::addAttribVertexNextEquiv(geo, name, geoGroup, defaults, creation_args, attribute_options, storage, reuse, subscribeRatio, minGrainSize);

    static GA_Attribute*
        addAttribVertexNextEquivNoLoop(
            GEO_Detail* geo,
            const GA_VertexGroup* geoGroup = nullptr,
            const GA_Storage storage = GA_STORE_INT32,
            const UT_StringHolder& name = "__topo_nextEquivNoLoop",
            const GA_Defaults& defaults = GA_Defaults(-1),
            const UT_Options* creation_args = nullptr,
            const GA_AttributeOptions* attribute_options = nullptr,
            const GA_ReuseStrategy& reuse = GA_ReuseStrategy(),
            const exint subscribeRatio = 32,
            const exint minGrainSize = 1024
        )
    {
        GA_Attribute* attribPtr = geo->findVertexAttribute(name);
        if (attribPtr)
            return attribPtr;
        attribPtr = geo->addIntTuple(GA_ATTRIB_VERTEX, GA_FEE_TOPO_SCOPE, name, 1, defaults, creation_args, attribute_options, storage, reuse);
        const GA_Attribute* refAttrib = GA_FeE_TopologyReference::addAttribVertexPointDst(geo, geoGroup, storage);
        vertexNextEquivNoLoop(geo, attribPtr, refAttrib, geoGroup, subscribeRatio, minGrainSize);
        return attribPtr;
    }












    //GA_FeE_VertexNextEquiv::addAttribVertexNextEquiv(geo, groupPtr, name, geoGroup, defaults, storage, creation_args, attribute_options, reuse, subscribeRatio, minGrainSize);

    static GA_Attribute*
        addAttribVertexNextEquiv(
            GEO_Detail* geo,
            GA_VertexGroup*& groupPtr,
            const GA_VertexGroup* geoGroup = nullptr,
            const GA_Storage storage = GA_STORE_INT32,
            const UT_StringHolder& name = "__topo_nextEquiv",
            const UT_StringHolder& groupName = "__topo_unshared",
            const GA_Defaults& defaults = GA_Defaults(-1),
            const UT_Options* creation_args = nullptr,
            const GA_AttributeOptions* attribute_options = nullptr,
            const GA_ReuseStrategy& reuse = GA_ReuseStrategy(),
            const exint subscribeRatio = 32,
            const exint minGrainSize = 1024
        )
    {
        GA_Attribute* attribPtr = geo->findVertexAttribute(name);
        if (attribPtr)
            return attribPtr;
        const GA_Attribute* refAttrib = GA_FeE_TopologyReference::addAttribVertexPointDst(geo, geoGroup, storage);
        attribPtr = geo->addIntTuple(GA_ATTRIB_VERTEX, GA_FEE_TOPO_SCOPE, name, 1, defaults, creation_args, attribute_options, storage, reuse);
        groupPtr = geo->newVertexGroup(groupName);
        vertexNextEquiv(geo, attribPtr, groupPtr, refAttrib, geoGroup, subscribeRatio, minGrainSize);
        return attribPtr;
    }





    //GA_FeE_VertexNextEquiv::addAttribVertexNextEquiv(geo, name, geoGroup, defaults, storage, nullptr, nullptr, GA_ReuseStrategy(), subscribeRatio, minGrainSize);
    
    static GA_Attribute*
        addAttribVertexNextEquiv(
            GEO_Detail* geo,
            const GA_VertexGroup* geoGroup = nullptr,
            const GA_Storage storage = GA_STORE_INT32,
            const UT_StringHolder& name = "__topo_nextEquiv",
            const GA_Defaults& defaults = GA_Defaults(-1),
            const UT_Options* creation_args = nullptr,
            const GA_AttributeOptions* attribute_options = nullptr,
            const GA_ReuseStrategy& reuse = GA_ReuseStrategy(),
            const exint subscribeRatio = 32,
            const exint minGrainSize = 1024
        )
    {
        GA_Attribute* attribPtr = geo->findVertexAttribute(name);
        if (attribPtr)
            return attribPtr;
        attribPtr = geo->addIntTuple(GA_ATTRIB_VERTEX, GA_FEE_TOPO_SCOPE, name, 1, defaults, creation_args, attribute_options, storage, reuse);
        const GA_Attribute* refAttrib = GA_FeE_TopologyReference::addAttribVertexPointDst(geo, geoGroup, storage);
        vertexNextEquiv(geo, attribPtr, refAttrib, geoGroup, subscribeRatio, minGrainSize);
        return attribPtr;
    }



    //GA_FeE_VertexNextEquiv::addAttribVertexNextEquiv(geo, name, geoGroup, subscribeRatio, minGrainSize);

    static GA_VertexGroup*
    addGroupVertexNextEquiv(
        GEO_Detail* geo,
        const GA_VertexGroup* geoGroup = nullptr,
        const GA_Storage storage = GA_STORE_INT32,
        const UT_StringHolder& name = "__topo_unshared",
        const exint subscribeRatio = 32,
        const exint minGrainSize = 1024
    )
    {
        GA_VertexGroup* groupPtr = geo->findVertexGroup(name);
        if (groupPtr)
            return groupPtr;
        const GA_Attribute* refAttrib = GA_FeE_TopologyReference::addAttribVertexPointDst(geo, geoGroup, storage);
        groupPtr = geo->newVertexGroup(name);
        vertexNextEquiv(geo, groupPtr, refAttrib, geoGroup, subscribeRatio, minGrainSize);
        return groupPtr;
    }



} // End of namespace GA_FeE_VertexNextEquiv

#endif
