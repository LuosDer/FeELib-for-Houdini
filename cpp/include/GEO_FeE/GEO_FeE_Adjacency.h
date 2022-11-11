
#pragma once

#ifndef __GEO_FeE_Adjacency_h__
#define __GEO_FeE_Adjacency_h__

//#include <GEO_FeE/GEO_FeE_Adjacency.h>

#include <GU/GU_Detail.h>
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


    //Get all prims neighbours prims with adjacent by edge
    static void
    primPrimEdge(
        const GU_Detail* geo,
        const GA_RWHandleT<UT_ValArray<GA_Offset>>& attribHandle,
        const GA_PrimitiveGroup* geoGroup = nullptr,
        const GA_VertexGroup* vertexEdgeSeamGroup = nullptr,
        const exint& subscribeRatio = 16,
        const exint& minGrainSize = 1024
    )
    {
    UT_AutoInterrupt boss("primPrimEdge");

    const GA_SplittableRange geo0SplittableRange0(geo->getPrimitiveRange(geoGroup));
    UTparallelFor(geo0SplittableRange0, [&geo, &attribHandle, &vertexEdgeSeamGroup, &boss](const GA_SplittableRange& r)
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
            if (vertexEdgeSeamGroup)
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



    //Get Vertex Destination Point
    static GA_Size
        vertexPrimOffset(
            const GU_Detail* geo,
            const GA_Offset& vtxoff
        )
    {
        return geo->getPrimitiveVertexList(geo->vertexPrimitive(vtxoff)).find(vtxoff);
    }

    //Get Vertex Destination Point
    static GA_Size
        vertexPrimOffset(
            const GU_Detail* geo,
            const GA_Offset& primoff,
            const GA_Offset& vtxoff
        )
    {
        return geo->getPrimitiveVertexList(primoff).find(vtxoff);
    }

    //Get Vertex Destination Point
    static GA_Offset
    vertexPointDst(
        const GU_Detail* geo,
        const GA_Offset& vtxoff
    )
    {
        GA_Offset primoff = geo->vertexPrimitive(vtxoff);
        GA_Size vtxpnum = vertexPrimOffset(geo, primoff, vtxoff) + 1;
        if (vtxpnum == geo->getPrimitiveVertexCount(primoff)) {
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
            return geo->vertexPoint(geo->getPrimitiveVertexOffset(primoff, vtxpnum));
        }
    }



    //Get Vertex Next Equiv Vertex
    static GA_Offset
    vertexVertexNextEquiv(
        const GU_Detail* geo,
        const GA_Offset& vtxoff
    )
    {
        GA_Offset dstpt = vertexPointDst(geo, vtxoff);
        if (dstpt == -1)
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

        nextVtx = geo->pointVertex(dstpt);
        GA_Offset ptnum = geo->vertexPoint(vtxoff);
        while (nextVtx != GA_INVALID_OFFSET)
        {
            nextVtxDstpt = vertexPointDst(geo, nextVtx);
            if (nextVtxDstpt == ptnum)
                return nextVtx;
            nextVtx = geo->vertexToNextVertex(nextVtx);
        }
        return -1;  
    }








    //Get all vertices NextEquiv Vertex
    template<typename T>
    static void
        vertexPrimOffset(
            const GU_Detail* geo,
            const GA_RWHandleT<T>& attribHandle,
            const GA_VertexGroup* geoGroup = nullptr,
            const exint& subscribeRatio = 16,
            const exint& minGrainSize = 1024
        )
    {
        UT_AutoInterrupt boss("vertexPrimOffset");

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
                    attribHandle.set(elemoff, vertexPrimOffset(geo, elemoff));
                }
            }
        }, subscribeRatio, minGrainSize);

        attribHandle->bumpDataId();

    }

    template<typename T>
    //Get all vertices NextEquiv Vertex
    static void
        vertexPointDst(
            const GU_Detail* geo,
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

    //Get all vertices NextEquiv Vertex
    template<typename T>
    static void
        vertexVertexNextEquiv(
            const GU_Detail* geo,
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





} // End of namespace GEO_FeE_Adjacency

#endif
