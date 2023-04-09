
#pragma once

#ifndef __GFE_Adjacency_h__
#define __GFE_Adjacency_h__

//#include "GFE/GFE_Adjacency.h"


#include "GA/GA_PageHandle.h"
#include "GA/GA_PageIterator.h"



#include "GFE/GFE_GeoFilter.h"

#include "GFE/GFE_VertexNextEquiv.h"



class GFE_Adjacency : public GFE_AttribFilter {

public:
    //using GFE_AttribFilter::GFE_AttribFilter;

    GFE_Adjacency(
        GA_Detail* const geo,
        const SOP_NodeVerb::CookParms* const cookparms = nullptr
    )
        : GFE_AttribFilter(geo, cookparms)
        , pointSeamGroup(geo, groupParser.getGOP(), cookparms)
        , vertexEdgeSeamGroup(geo, groupParser.getGOP(), cookparms)
        , edgeSeamGroup(geo, groupParser.getGOP(), cookparms)
    {
    }

    
    void
        setComputeParm(
            const bool asOffset = true,
            const exint subscribeRatio = 64,
            const exint minGrainSize = 1024
        )
    {
        setHasComputed();
        this->asOffset = asOffset;
        this->subscribeRatio = subscribeRatio;
        this->minGrainSize = minGrainSize;
    }
    
    // void
    // setComputeParm(
    //         const bool outVertexPrimIndex = false,
    //         const bool outVertexVertexPrim = false,
    //         const bool outVertexPointDst = false,
    //         const bool outVertexNextEquiv = false,
    //         const bool outVertexNextEquivNoLoop = false,
    //         const bool outPointPointEdge = false,
    //         const bool outPointPointPrim = false,
    //         const bool outPrimPrimEdge = false,
    //         const bool outPrimPrimPoint = false
    //     )
    // {
    //     setHasComputed();
    //     this->outVertexPrimIndex = outVertexPrimIndex;
    //     this->outVertexVertexPrim = outVertexVertexPrim;
    //     this->outVertexPointDst = outVertexPointDst;
    //     this->outVertexNextEquiv = outVertexNextEquiv;
    //     this->outVertexNextEquivNoLoop = outVertexNextEquivNoLoop;
    //     
    //     this->outPointPointEdge = outPointPointEdge;
    //     this->outPointPointPrim = outPointPointPrim;
    //     
    //     this->outPrimPrimEdge = outPrimPrimEdge;
    //     this->outPrimPrimPoint = outPrimPrimPoint;
    // }

    inline
    void
        setKernel(
            const int kernel
        )
    {
        setHasComputed();
        this->kernel = kernel;
    }



    inline
        void
        setVertexVertexPrim()
    {
        calVertexVertexPrim = false;
    }

    void
        setVertexVertexPrim(
            const bool detached,
            const UT_StringHolder& attribName0 = "",
            const UT_StringHolder& attribName1 = "",
            const GA_Storage storage = GA_STORE_INVALID
        )
    {
        calVertexVertexPrim = true;
        vertexVertexPrimPrevAttrib = getOutAttribArray().findOrCreateTuple(detached,
            GA_ATTRIB_VERTEX, GA_STORECLASS_INT, storage, attribName0, 1, GA_Defaults(-1));
        vertexVertexPrimNextAttrib = getOutAttribArray().findOrCreateTuple(detached,
            GA_ATTRIB_VERTEX, GA_STORECLASS_INT, storage, attribName1, 1, GA_Defaults(-1));
    }

    void
        setVertexVertexPrim(
            GA_Attribute* const attribPtr0,
            GA_Attribute* const attribPtr1
        )
    {
        if (attribPtr0 && attribPtr1)
        {
            calVertexVertexPrim = true;
            vertexVertexPrimPrevAttrib = attribPtr0;
            vertexVertexPrimNextAttrib = attribPtr1;
        }
        else
        {
            calVertexVertexPrim = false;
        }
    }


#if 1

#define GFE_SETATTRIB_SPECIALIZATION(ATTRIB_NAME_UPPER, ATTRIB_NAME_LOWNER, ATTRIB_OWNER)           \
    SYS_FORCE_INLINE                                                                                \
    void                                                                                            \
        set##ATTRIB_NAME_UPPER()                                                                    \
    {                                                                                               \
        cal##ATTRIB_NAME_UPPER = false;                                                             \
    }                                                                                               \
                                                                                                    \
    void                                                                                            \
        set##ATTRIB_NAME_UPPER(                                                                     \
            const bool detached,                                                                    \
            const UT_StringHolder& attribName = "",                                                 \
            const GA_Storage storage = GA_STORE_INVALID                                             \
        )                                                                                           \
    {                                                                                               \
        cal##ATTRIB_NAME_UPPER = true;                                                              \
        ATTRIB_NAME_LOWNER##Attrib = getOutAttribArray().findOrCreateTuple(detached,                \
            ATTRIB_OWNER, GA_STORECLASS_INT, storage, attribName, 1, GA_Defaults(-1));              \
    }                                                                                               \
                                                                                                    \
    void                                                                                            \
    set##ATTRIB_NAME_UPPER(                                                                         \
            GA_Attribute* const attribPtr                                                           \
        )                                                                                           \
    {                                                                                               \
        if(attribPtr)                                                                               \
        {                                                                                           \
            cal##ATTRIB_NAME_UPPER = true;                                                          \
            ATTRIB_NAME_LOWNER##Attrib = attribPtr;                                                 \
        }                                                                                           \
        else                                                                                        \
        {                                                                                           \
            cal##ATTRIB_NAME_UPPER = false;                                                         \
        }                                                                                           \
    }                                                                                               \


    GFE_SETATTRIB_SPECIALIZATION(VertexPrimIndex,       vertexPrimIndex,       GA_ATTRIB_VERTEX)
    GFE_SETATTRIB_SPECIALIZATION(VertexPointDst,        vertexPointDst,        GA_ATTRIB_VERTEX)
    GFE_SETATTRIB_SPECIALIZATION(VertexNextEquiv,       vertexNextEquiv,       GA_ATTRIB_VERTEX)
    GFE_SETATTRIB_SPECIALIZATION(VertexNextEquivNoLoop, vertexNextEquivNoLoop, GA_ATTRIB_VERTEX)

#undef GFE_SETATTRIB_SPECIALIZATION
    

        



#define GFE_SETATTRIB_ARRAY_SPECIALIZATION(ATTRIB_NAME_UPPER, ATTRIB_NAME_LOWNER, ATTRIB_OWNER)     \
    SYS_FORCE_INLINE                                                                                \
    void                                                                                            \
        set##ATTRIB_NAME_UPPER()                                                                    \
    {                                                                                               \
        cal##ATTRIB_NAME_UPPER = false;                                                             \
    }                                                                                               \
                                                                                                    \
    void                                                                                            \
        set##ATTRIB_NAME_UPPER(                                                                     \
            const bool detached,                                                                    \
            const UT_StringHolder& attribName = "",                                                 \
            const GA_Storage storage = GA_STORE_INVALID                                             \
        )                                                                                           \
    {                                                                                               \
        cal##ATTRIB_NAME_UPPER = true;                                                              \
        ATTRIB_NAME_LOWNER##Attrib = getOutAttribArray().findOrCreateArray(detached,                \
            ATTRIB_OWNER, GA_STORECLASS_INT, storage, attribName);                                  \
    }                                                                                               \
                                                                                                    \
    void                                                                                            \
    set##ATTRIB_NAME_UPPER(                                                                         \
            GA_Attribute* const attribPtr                                                           \
        )                                                                                           \
    {                                                                                               \
        if(attribPtr)                                                                               \
        {                                                                                           \
            cal##ATTRIB_NAME_UPPER = true;                                                          \
            ATTRIB_NAME_LOWNER##Attrib = attribPtr;                                                 \
        }                                                                                           \
        else                                                                                        \
        {                                                                                           \
            cal##ATTRIB_NAME_UPPER = false;                                                         \
        }                                                                                           \
    }                                                                                               \



    GFE_SETATTRIB_ARRAY_SPECIALIZATION(PointPointEdge, pointPointEdge, GA_ATTRIB_POINT)
    GFE_SETATTRIB_ARRAY_SPECIALIZATION(PointPointPrim, pointPointPrim, GA_ATTRIB_POINT)
    GFE_SETATTRIB_ARRAY_SPECIALIZATION(PrimPrimEdge,   primPrimEdge,   GA_ATTRIB_PRIMITIVE)
    GFE_SETATTRIB_ARRAY_SPECIALIZATION(PrimPrimPoint,  primPrimPoint,  GA_ATTRIB_PRIMITIVE)



#undef GFE_SETATTRIB_ARRAY_SPECIALIZATION

#else


inline
void
setVertexPrimIndex()
{
    calVertexPrimIndex = false;
}

void
setVertexPrimIndex(
    const bool detached,
    const UT_StringHolder& attribName = "",
    const GA_Storage storage = GA_STORE_INVALID
)
{
    calVertexPrimIndex = true;
    vertexPrimIndexAttrib = getOutAttribArray().findOrCreateTuple(detached,
        GA_ATTRIB_VERTEX, GA_STORECLASS_INT, storage, attribName, 1, GA_Defaults(-1));
}

void
setVertexPrimIndex(
    GA_Attribute* const attribPtr
)
{
    if (attribPtr)
    {
        calVertexPrimIndex = true;
        vertexPrimIndexAttrib = attribPtr;
    }
    else
    {
        calVertexPrimIndex = false;
    }
}




inline
void
setVertexPointDst()
{
    calVertexPointDst = false;
}

void
setVertexPointDst(
    //const bool asOffset,
    const bool detached,
    const UT_StringHolder& attribName = "",
    const GA_Storage storage = GA_STORE_INVALID
)
{
    calVertexPointDst = true;
    //vertexPointDst_asOffset = asOffset;
    vertexPointDstAttrib = getOutAttribArray().findOrCreateTuple(detached,
        GA_ATTRIB_VERTEX, GA_STORECLASS_INT, storage, attribName, 1, GA_Defaults(-1));
}

void
setVertexPointDst(
    //const bool asOffset,
    GA_Attribute* const attribPtr
)
{
    if (attribPtr)
    {
        calVertexPointDst = true;
        vertexPointDstAttrib = attribPtr;
        //vertexPointDst_asOffset = asOffset;
    }
    else
    {
        calVertexPointDst = false;
    }
}


inline
void
setVertexNextEquiv()
{
    calVertexNextEquiv = false;
}

void
setVertexNextEquiv(
    const bool detached,
    const UT_StringHolder& attribName = "",
    const GA_Storage storage = GA_STORE_INVALID
)
{
    calVertexNextEquiv = true;
    vertexNextEquivAttrib = getOutAttribArray().findOrCreateTuple(detached,
        GA_ATTRIB_VERTEX, GA_STORECLASS_INT, storage, attribName, 1, GA_Defaults(-1));
}

void
setVertexNextEquiv(
    GA_Attribute* const attribPtr
)
{
    if (attribPtr)
    {
        calVertexNextEquiv = true;
        vertexNextEquivAttrib = attribPtr;
    }
    else
    {
        calVertexNextEquiv = false;
    }
}




inline
void
setVertexNextEquivNoLoop()
{
    calVertexNextEquivNoLoop = false;
}

void
setVertexNextEquivNoLoop(
    const bool detached,
    const UT_StringHolder& attribName = "",
    const GA_Storage storage = GA_STORE_INVALID
)
{
    calVertexNextEquivNoLoop = true;
    vertexNextEquivNoLoopAttrib = getOutAttribArray().findOrCreateTuple(detached,
        GA_ATTRIB_VERTEX, GA_STORECLASS_INT, storage, attribName, 1, GA_Defaults(-1));
}

void
setVertexNextEquivNoLoop(
    GA_Attribute* const attribPtr
)
{
    if (attribPtr)
    {
        calVertexNextEquivNoLoop = true;
        vertexNextEquivNoLoopAttrib = attribPtr;
    }
    else
    {
        calVertexNextEquivNoLoop = false;
    }
}


inline
void
setPointPointEdge()
{
    calPointPointEdge = false;
}

void
setPointPointEdge(
    const bool detached,
    const UT_StringHolder& attribName = "",
    const GA_Storage storage = GA_STORE_INVALID
)
{
    calPointPointEdge = true;
    pointPointEdgeAttrib = getOutAttribArray().findOrCreateArray(detached,
        GA_ATTRIB_POINT, GA_STORECLASS_INT, storage, attribName);
}

void
setPointPointEdge(
    GA_Attribute* const attribPtr
)
{
    if (attribPtr)
    {
        calPointPointEdge = true;
        pointPointEdgeAttrib = attribPtr;
    }
    else
    {
        calPointPointEdge = false;
    }
}




inline
void
setPointPointPrim()
{
    calPointPointPrim = false;
}

void
setPointPointPrim(
    const bool detached,
    const UT_StringHolder& attribName = "",
    const GA_Storage storage = GA_STORE_INVALID
)
{
    calPointPointPrim = true;
    pointPointPrimAttrib = getOutAttribArray().findOrCreateArray(detached,
        GA_ATTRIB_POINT, GA_STORECLASS_INT, storage, attribName);
}

void
setPointPointPrim(
    GA_Attribute* const attribPtr
)
{
    if (attribPtr)
    {
        calPointPointPrim = true;
        pointPointPrimAttrib = attribPtr;
    }
    else
    {
        calPointPointPrim = false;
    }
}



inline
void
setPrimPrimEdge()
{
    calPrimPrimEdge = false;
}

void
setPrimPrimEdge(
    const bool detached,
    const UT_StringHolder& attribName = "",
    const GA_Storage storage = GA_STORE_INVALID
)
{
    calPrimPrimEdge = true;
    primPrimEdgeAttrib = getOutAttribArray().findOrCreateArray(detached,
        GA_ATTRIB_PRIMITIVE, GA_STORECLASS_INT, storage, attribName);
}

void
setPrimPrimEdge(
    GA_Attribute* const attribPtr
)
{
    if (attribPtr)
    {
        calPrimPrimEdge = true;
        primPrimEdgeAttrib = attribPtr;
    }
    else
    {
        calPrimPrimEdge = false;
    }
}




inline
void
setPrimPrimPoint()
{
    calPrimPrimPoint = false;
}

void
setPrimPrimPoint(
    const bool detached,
    const UT_StringHolder& attribName = "",
    const GA_Storage storage = GA_STORE_INVALID
)
{
    calPrimPrimPoint = true;
    primPrimPointAttrib = getOutAttribArray().findOrCreateArray(detached,
        GA_ATTRIB_PRIMITIVE, GA_STORECLASS_INT, storage, attribName);
}

void
setPrimPrimPoint(
    GA_Attribute* const attribPtr
)
{
    if (attribPtr)
    {
        calPrimPrimPoint = true;
        primPrimPointAttrib = attribPtr;
    }
    else
    {
        calPrimPrimPoint = false;
    }
}

#endif


#define GFE_GETATTRIB_SPECIALIZATION(ATTRIB_NAME_UPPER, ATTRIB_NAME_LOWNER)                         \
    SYS_FORCE_INLINE                                                                                \
    GA_Attribute*                                                                                   \
        get##ATTRIB_NAME_UPPER() const                                                              \
    {                                                                                               \
        return ATTRIB_NAME_LOWNER##Attrib;                                                          \
    }                                                                                               \



    GFE_GETATTRIB_SPECIALIZATION(VertexPrimIndex,       vertexPrimIndex)
    GFE_GETATTRIB_SPECIALIZATION(VertexPointDst,        vertexPointDst)
    GFE_GETATTRIB_SPECIALIZATION(VertexNextEquiv,       vertexNextEquiv)
    GFE_GETATTRIB_SPECIALIZATION(VertexNextEquivNoLoop, vertexNextEquivNoLoop)
    GFE_GETATTRIB_SPECIALIZATION(VertexVertexPrimPrev,  vertexVertexPrimPrev)
    GFE_GETATTRIB_SPECIALIZATION(VertexVertexPrimNext,  vertexVertexPrimNext)
    GFE_GETATTRIB_SPECIALIZATION(PointPointEdge,        pointPointEdge)
    GFE_GETATTRIB_SPECIALIZATION(PointPointPrim,        pointPointPrim)
    GFE_GETATTRIB_SPECIALIZATION(PrimPrimEdge,          primPrimEdge)
    GFE_GETATTRIB_SPECIALIZATION(PrimPrimPoint,         primPrimPoint)



#undef GFE_GETATTRIB_SPECIALIZATION






   
    
   



    
private:

    virtual bool
        computeCore() override
    {
        // calVertexPrimIndex = outVertexPrimIndex;
        // calVertexVertexPrim = outVertexVertexPrim;
        // calVertexPointDst = outVertexPointDst;
        // calVertexNextEquiv = outVertexNextEquiv;
        // calVertexNextEquivNoLoop = outVertexNextEquivNoLoop;
        // calPointPointEdge = outPointPointEdge;
        // calPointPointPrim = outPointPointPrim;
        // calPrimPrimEdge = outPrimPrimEdge;
        // calPrimPrimPoint = outPrimPrimPoint;
        
        if (!calVertexPrimIndex &&
            !calVertexVertexPrim &&
            !calVertexPointDst &&
            !calVertexNextEquiv &&
            !calVertexNextEquivNoLoop &&
            !calPointPointEdge &&
            !calPointPointPrim &&
            !calPrimPrimEdge &&
            !calPrimPrimPoint)
                return false;

        if (groupParser.isEmpty())
            return true;
        
        //if (vertexEdgeSeamGroup)
        //    GFE_GroupUnion::groupUnion(vertexEdgeSeamGroup, edgeSeamGroup);

        
        calVertexPrimIndex = calVertexPrimIndex || calVertexPointDst || calVertexNextEquiv || calVertexNextEquivNoLoop || calPointPointEdge || calPrimPrimEdge;
        calVertexPointDst = calVertexPointDst || calVertexNextEquiv || calVertexNextEquivNoLoop || calPrimPrimEdge;
        calVertexVertexPrim = calVertexVertexPrim || calPointPointEdge;
        calVertexNextEquiv = calVertexNextEquiv || calPrimPrimEdge;
        if (calVertexPrimIndex)
        {
            vertexPrimIndex(vertexPrimIndexAttrib);
        }
        if (calVertexPointDst)
        {
            switch ((calVertexNextEquiv || calVertexNextEquivNoLoop || calPrimPrimEdge) ? 0 : kernel)
            {
            case 0:
                vertexPointDstByVtxpnum(vertexPointDstAttrib, vertexPrimIndexAttrib);
                break;
            case 1:
                vertexPointDst(vertexPointDstAttrib, vertexPrimIndexAttrib);
                break;
            default:
                vertexPointDstByVtxpnum(vertexPointDstAttrib, vertexPrimIndexAttrib);
                break;
            }
        }
        if (calVertexVertexPrim)
        {
            switch (calPointPointEdge ? 0 : kernel)
            {
            case 0:
                vertexVertexPrim(vertexVertexPrimPrevAttrib, vertexVertexPrimNextAttrib);
                break;
            case 1:
                vertexVertexPrim1(vertexVertexPrimPrevAttrib, vertexVertexPrimNextAttrib);
                break;
            default:
                vertexVertexPrim(vertexVertexPrimPrevAttrib, vertexVertexPrimNextAttrib);
                break;
            }
        }
        if (calVertexNextEquiv || calPrimPrimEdge)
        {
            vertexNextEquiv(vertexNextEquivAttrib, vertexPointDstAttrib);
        }
        if (calVertexNextEquivNoLoop)
        {
            vertexNextEquivNoLoop(vertexNextEquivNoLoopAttrib, vertexPointDstAttrib);
        }
        if (calPointPointEdge)
        {
            switch (kernel)
            {
            case 0:
                pointPointEdge(pointPointEdgeAttrib, vertexPrimIndexAttrib);
                break;
            case 1:
                pointPointEdge(pointPointEdgeAttrib, vertexVertexPrimPrevAttrib, vertexVertexPrimNextAttrib);
                break;
            case 2:
                pointPointEdge(pointPointEdgeAttrib);
                break;
            default:
                pointPointEdge(pointPointEdgeAttrib, vertexPrimIndexAttrib);
                break;
            }
        }
        if (calPointPointPrim)
        {
            //GFE_Adjacency_Namespace::pointPointPrim(geo, attribHandle,
            //    groupParser.getPointGroup(), nullptr,
            //    subscribeRatio, minGrainSize);
        }
        if (calPrimPrimEdge)
        {
            switch (kernel)
            {
            case 0:
                primPrimEdge(primPrimEdgeAttrib, vertexNextEquivAttrib);
                break;
            case 1:
                primPrimEdge1(primPrimEdgeAttrib, vertexPointDstAttrib);
                break;
            case 2:
                primPrimEdge2(primPrimEdgeAttrib, vertexPointDstAttrib);
                break;
            case 3:
                primPrimEdge3(primPrimEdgeAttrib, vertexPointDstAttrib);
                break;
            case 4:
                primPrimEdge4(primPrimEdgeAttrib);
                break;
            default:
                break;
            }
        }
        if (calPrimPrimPoint)
        {
            primPrimPoint(primPrimPointAttrib);
        }



        return true;
    }




    //Get all vertices NextEquiv Vertex
    //template<typename T>
    void
        vertexPrimIndex(
            const GA_RWHandleT<GA_Size>& attrib_h
        )
    {
        const GA_Detail* const geo = this->geo;
        if (groupParser.isFull())
        {
            //const GA_SplittableRange geoSplittableRange0(geo->getPrimitiveRange(nullptr));
            UTparallelFor(groupParser.getPrimitiveSplittableRange(), [geo, &attrib_h](const GA_SplittableRange& r)
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
                            attrib_h.set(vertices[vtxpnum], vtxpnum);
                        }
                    }
                }
            }, subscribeRatio, minGrainSize);
        }
        else
        {
            const GA_VertexGroup* const geoGroup = groupParser.getVertexGroup();
            //const GA_SplittableRange geoSplittableRange0(groupParser.getPrimitiveRange());
            UTparallelFor(groupParser.getPrimitiveSplittableRange(), [geo, &attrib_h, geoGroup](const GA_SplittableRange& r)
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
                            attrib_h.set(vertices[vtxpnum], vtxpnum);
                        }
                    }
                }
            }, subscribeRatio, minGrainSize);
        }
    }



#if 1
    //This is Faster than use linear vertex offset
    GA_Offset
        vertexPointDst(
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
    
    GA_Size
        vertexPrimIndex(
            const GA_Offset primoff,
            const GA_Offset vtxoff
        )
    {
        return geo->getPrimitiveVertexList(primoff).find(vtxoff);
    }

    GA_Offset
    vertexPointDst(
        const GA_Offset vtxoff
    )
    {
        const GA_Offset primoff = geo->vertexPrimitive(vtxoff);
        return vertexPointDst(primoff, vertexPrimIndex(primoff, vtxoff));
    }
#else
    GA_Offset
        vertexPointDst(
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
    
    GA_Offset
        vertexPointDst(
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







    void
        vertexPointDstByVtxpnum(
            const GA_RWHandleT<GA_Offset>& attrib_h,
            const GA_ROHandleT<GA_Size>& vtxpnum_h
        )
    {
        const GA_Topology& topo = geo->getTopology();
        //topo.makePrimitiveRef();
        const GA_ATITopology* const vtxPrimRef = topo.getPrimitiveRef();

        UTparallelFor(groupParser.getVertexSplittableRange(), [&attrib_h, &vtxpnum_h, vtxPrimRef, this](const GA_SplittableRange& r)
        {
            GA_Offset start, end;
            for (GA_Iterator it(r); it.blockAdvance(start, end); )
            {
                for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                {
                    attrib_h.set(elemoff, vertexPointDst(vtxPrimRef->getLink(elemoff), vtxpnum_h.get(elemoff)));
                }
            }
        }, subscribeRatio, minGrainSize);
    }

    void
        vertexPointDst(
            GA_Attribute* const attrib_next,
            const GA_Attribute* const vtxPrimNextAttrib
        )
    {
        const GA_Detail* const geo = this->geo;
        const GA_Topology& topo = geo->getTopology();
        const GA_ATITopology* const vtxPointRef = topo.getPointRef();

        //const GA_VertexGroup* const geoGroup = groupParser.getVertexGroup();
        //const GA_SplittableRange geoSplittableRange0(geo->getVertexRange(geoGroup));
        UTparallelFor(groupParser.getVertexSplittableRange(), [geo, attrib_next, vtxPrimNextAttrib, vtxPointRef](const GA_SplittableRange& r)
        {
            GA_PageHandleScalar<GA_Offset>::RWType dstpt_ph(attrib_next);
            GA_PageHandleScalar<GA_Offset>::ROType vtxPrimNext_ph(vtxPrimNextAttrib);
            for (GA_PageIterator pit = r.beginPages(); !pit.atEnd(); ++pit)
            {
                GA_Offset start, end;
                for (GA_Iterator it(pit.begin()); it.blockAdvance(start, end); )
                {
                    dstpt_ph.setPage(start);
                    vtxPrimNext_ph.setPage(start);
                    for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                    {
                        if (vtxPrimNext_ph.value(elemoff) < 0)
                        {
                            dstpt_ph.value(elemoff) = GA_INVALID_OFFSET;
                        }
                        else
                        {
                            dstpt_ph.value(elemoff) = vtxPointRef->getLink(vtxPrimNext_ph.value(elemoff));
                        }
                    }
                }
            }
        }, subscribeRatio, minGrainSize);
    }


    void
        vertexVertexPrim(
            const GA_RWHandleT<GA_Offset>& attribPrev_h,
            const GA_RWHandleT<GA_Offset>& attribNext_h
        )
    {
        const GA_Detail* const geo = this->geo;
        if (groupParser.isFull())
        {
            const GA_SplittableRange geoSplittableRange0(geo->getPrimitiveRange());
            UTparallelFor(geoSplittableRange0, [geo, &attribPrev_h, &attribNext_h](const GA_SplittableRange& r)
            {
                GA_Offset start, end;
                for (GA_Iterator it(r); it.blockAdvance(start, end); )
                {
                    for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                    {
                        const GA_OffsetListRef& vertices = geo->getPrimitiveVertexList(elemoff);
                        const GA_Size lastIndex = vertices.size() - 1;
                        if (geo->getPrimitiveClosedFlag(elemoff))
                        {
                            attribPrev_h.set(vertices[0], vertices[lastIndex]);
                            attribNext_h.set(vertices[lastIndex], vertices[0]);
                        }
                        else
                        {
                            attribPrev_h.set(vertices[0], -1);
                            attribNext_h.set(vertices[lastIndex], -1);
                        }
                        GA_Offset vtxoff_prev = vertices[0];
                        GA_Offset vtxoff_next;
                        for (GA_Size vtxpnum = 1; vtxpnum <= lastIndex; ++vtxpnum)
                        {
                            vtxoff_next = vertices[vtxpnum];
                            attribNext_h.set(vtxoff_prev, vtxoff_next);
                            attribPrev_h.set(vtxoff_next, vtxoff_prev);
                            vtxoff_prev = vtxoff_next;
                        }
                    }
                }
            }, subscribeRatio, minGrainSize);
        }
        else
        {
            const GA_VertexGroup* const geoGroup = groupParser.getVertexGroup();
            UTparallelFor(groupParser.getPrimitiveSplittableRange(), [geo, &attribPrev_h, &attribNext_h, geoGroup](const GA_SplittableRange& r)
            {
                GA_Offset start, end;
                for (GA_Iterator it(r); it.blockAdvance(start, end); )
                {
                    for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                    {
                        const GA_OffsetListRef& vertices = geo->getPrimitiveVertexList(elemoff);
                        const GA_Size lastIndex = vertices.size() - 1;
                        if (geo->getPrimitiveClosedFlag(elemoff))
                        {
                            attribPrev_h.set(vertices[0], vertices[lastIndex]);
                            attribNext_h.set(vertices[lastIndex], vertices[0]);
                        }
                        else
                        {
                            attribPrev_h.set(vertices[0], -1);
                            attribNext_h.set(vertices[lastIndex], -1);
                        }
                        GA_Offset vtxoff_prev = vertices[0];
                        GA_Offset vtxoff_next;
                        for (GA_Size vtxpnum = 1; vtxpnum <= lastIndex; ++vtxpnum)
                        {
                            vtxoff_next = vertices[vtxpnum];
                            if (!geoGroup->contains(vtxoff_next))
                                continue;
                            attribNext_h.set(vtxoff_prev, vtxoff_next);
                            attribPrev_h.set(vtxoff_next, vtxoff_prev);
                            vtxoff_prev = vtxoff_next;
                        }
                    }
                }
            }, subscribeRatio, minGrainSize);
        }
    }

    void
        vertexVertexPrim1(
            const GA_RWHandleT<GA_Size>& attribPrev_h,
            const GA_RWHandleT<GA_Size>& attribNext_h
        )
    {
        const GA_Topology& topo = geo->getTopology();
        UTparallelFor(groupParser.getVertexSplittableRange(), [&topo, &attribPrev_h, &attribNext_h](const GA_SplittableRange& r)
        {
            GA_Offset start, end;
            GA_Offset vtxoff_prev, vtxoff_next;
            for (GA_Iterator it(r); it.blockAdvance(start, end); )
            {
                for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                {
                    topo.getAdjacentBoundaryVertices(elemoff, vtxoff_prev, vtxoff_next);
                    attribPrev_h.set(elemoff, vtxoff_prev);
                    attribNext_h.set(elemoff, vtxoff_next);
                }
            }
        }, subscribeRatio, minGrainSize);
    }



    

    //Get all vertices NextEquiv Vertex
    void
        vertexNextEquiv(
            const GA_RWHandleT<GA_Offset>& attrib_h,
            const GA_ROHandleT<GA_Offset>& dstptAttrib_h
        )
    {
        const GA_Topology& topo = geo->getTopology();
        //topo.makeVertexRef();
        const GA_ATITopology* const vtxPointRef = topo.getPointRef();
        const GA_ATITopology* const pointVtxRef = topo.getVertexRef();
        const GA_ATITopology* const vtxNextRef = topo.getVertexNextRef();

        UTparallelFor(groupParser.getVertexSplittableRange(),
            [&attrib_h, &dstptAttrib_h, vtxPointRef, pointVtxRef, vtxNextRef](const GA_SplittableRange& r)
        {
            GA_Offset vtxoff_next, dstpt, ptnum;
            GA_Offset start, end;
            for (GA_Iterator it(r); it.blockAdvance(start, end); )
            {
                for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                {
                    dstpt = dstptAttrib_h.get(elemoff);
                    if (dstpt < 0)
                    {
                        attrib_h.set(elemoff, GA_INVALID_OFFSET);
                        continue;
                    }

                    for (vtxoff_next = vtxNextRef->getLink(elemoff); vtxoff_next != GA_INVALID_OFFSET; vtxoff_next = vtxNextRef->getLink(vtxoff_next))
                    {
                        if (dstptAttrib_h.get(vtxoff_next) != dstpt)
                            continue;
                        dstpt = GA_INVALID_OFFSET;
                        attrib_h.set(elemoff, vtxoff_next);
                        break;
                    }

                    if (dstpt < 0)
                        continue;

                    ptnum = vtxPointRef->getLink(elemoff);
                    for (vtxoff_next = pointVtxRef->getLink(dstpt); vtxoff_next != GA_INVALID_OFFSET; vtxoff_next = vtxNextRef->getLink(vtxoff_next))
                    {
                        if (dstptAttrib_h.get(vtxoff_next) != ptnum)
                            continue;
                        dstpt = GA_INVALID_OFFSET;
                        attrib_h.set(elemoff, vtxoff_next);
                        break;
                    }
                    if (dstpt >= 0)
                    {
                        attrib_h.set(elemoff, GA_INVALID_OFFSET);
                    }
                }
            }
        }, subscribeRatio, minGrainSize);
    }


    
    void
        vertexNextEquivNoLoop(
            const GA_RWHandleT<GA_Offset>& attrib_h,
            const GA_ROHandleT<GA_Offset>& dstptAttrib_h
        )
    {
        const GA_Topology& topo = geo->getTopology();
        //topo.makeVertexRef();
        const GA_ATITopology* const vtxPointRef = topo.getPointRef();
        const GA_ATITopology* const pointVtxRef = topo.getVertexRef();
        const GA_ATITopology* const vtxNextRef = topo.getVertexNextRef();

        UTparallelFor(groupParser.getVertexSplittableRange(),
            [&attrib_h, &dstptAttrib_h, vtxPointRef, pointVtxRef, vtxNextRef](const GA_SplittableRange& r)
        {
            GA_Offset vtxoff_next, dstpt, ptnum;
            GA_Offset start, end;
            for (GA_Iterator it(r); it.blockAdvance(start, end); )
            {
                for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                {
                    dstpt = dstptAttrib_h.get(elemoff);
                    if (dstpt < 0)
                    {
                        attrib_h.set(elemoff, GA_INVALID_OFFSET);
                        continue;
                    }

                    for (vtxoff_next = vtxNextRef->getLink(elemoff); vtxoff_next != GA_INVALID_OFFSET; vtxoff_next = vtxNextRef->getLink(vtxoff_next))
                    {
                        if (dstptAttrib_h.get(vtxoff_next) != dstpt)
                            continue;
                        dstpt = GA_INVALID_OFFSET;
                        attrib_h.set(elemoff, vtxoff_next);
                        break;
                    }

                    if (dstpt < 0)
                        continue;

                    ptnum = vtxPointRef->getLink(elemoff);
                    for (vtxoff_next = pointVtxRef->getLink(dstpt); vtxoff_next != GA_INVALID_OFFSET; vtxoff_next = vtxNextRef->getLink(vtxoff_next))
                    {
                        if (dstptAttrib_h.get(vtxoff_next) != ptnum)
                            continue;
                        if (dstpt > ptnum)
                        {
                            dstpt = GA_INVALID_OFFSET;
                            attrib_h.set(elemoff, vtxoff_next);
                        }
                        break;
                    }
                    if (dstpt >= 0)
                    {
                        attrib_h.set(elemoff, GA_INVALID_OFFSET);
                    }
                }
            }
        }, subscribeRatio, minGrainSize);
    }




    //Get all prims neighbours prims with adjacent by edge
    void
        pointPointEdge(
            const GA_RWHandleT<UT_ValArray<GA_Offset>>& attrib_h,
            const GA_ROHandleT<GA_Size>& vtxpnumAttrib_h
        )
    {
        const GA_Detail* const geo = this->geo;
        const GA_PointGroup* const seamGroup = pointSeamGroup.getPointGroup();
        UTparallelFor(groupParser.getPointSplittableRange(), [geo, &attrib_h, &vtxpnumAttrib_h, seamGroup](const GA_SplittableRange& r)
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

    
    //Get Vertex Destination Point
    void
    pointPointEdge(
        const GA_RWHandleT<UT_ValArray<GA_Offset>>& attrib_h,
        const GA_ROHandleT<GA_Offset>& vtxPrev_h,
        const GA_ROHandleT<GA_Offset>& vtxNext_h
    )
    {
        const GA_PointGroup* const seamGroup = pointSeamGroup.getPointGroup();
        GA_Topology& topo = geo->getTopology();
        topo.makeFull();
        //topo.makeVertexRef();
        const GA_ATITopology* const vtxPointRef = topo.getPointRef();
        const GA_ATITopology* const pointVtxRef = topo.getVertexRef();
        //const GA_ATITopology* const vtxPrevRef = topo.getVertexPrevRef();
        const GA_ATITopology* const vtxNextRef = topo.getVertexNextRef();

        UTparallelFor(groupParser.getPointSplittableRange(), [&attrib_h, &vtxPrev_h, &vtxNext_h, pointVtxRef, vtxNextRef, vtxPointRef, seamGroup](const GA_SplittableRange& r)
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


    //Get Vertex Destination Point
    void
        pointPointEdge(
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
            const GA_Size vtxpnum = GFE_TopologyReference_Namespace::vertexPrimIndex(geo, primoff, vtxoff_next);

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
    void
    pointPointEdge(
        const GA_RWHandleT<UT_ValArray<GA_Offset>>& attrib_h
    )
    {
        UTparallelFor(groupParser.getPointSplittableRange(), [&attrib_h, this](const GA_SplittableRange& r)
        {
            UT_ValArray<GA_Offset> adjElems;
            GA_Offset start, end;
            for (GA_Iterator it(r); it.blockAdvance(start, end); )
            {
                for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                {
                    pointPointEdge(elemoff, adjElems);
                    attrib_h.set(elemoff, adjElems);
                }
            }
        }, subscribeRatio, minGrainSize);
    }


    //Get all prims neighbours prims with adjacent by edge
    void
        primPrimEdge(
            const GA_RWHandleT<UT_ValArray<GA_Offset>>& attrib_h,
            const GA_ROHandleT<GA_Offset>& nextEquivAttrib_h
        )
    {
        const GA_VertexGroup* const seamGroup = vertexEdgeSeamGroup.getVertexGroup();
        GA_Topology& topo = geo->getTopology();
        topo.makePrimitiveRef();
        const GA_ATITopology* const vtxPrimRef = topo.getPrimitiveRef();

        const GA_Detail* const geo = this->geo;
        UTparallelFor(groupParser.getPrimitiveSplittableRange(), [geo, &attrib_h, &nextEquivAttrib_h, seamGroup, vtxPrimRef](const GA_SplittableRange& r)
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
    void
        primPrimEdge1(
            const GA_RWHandleT<UT_ValArray<GA_Offset>>& attrib_h,
            const GA_ROHandleT<GA_Offset>& dstptAttrib_h
        )
    {
        const GA_Detail* const geo = this->geo;
        const GA_VertexGroup* const seamGroup = vertexEdgeSeamGroup.getVertexGroup();
        UTparallelFor(groupParser.getPrimitiveSplittableRange(), [geo, &attrib_h, &dstptAttrib_h, seamGroup](const GA_SplittableRange& r)
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
    void
        primPrimEdge2(
            const GA_RWHandleT<UT_ValArray<GA_Offset>>& attrib_h,
            const GA_ROHandleT<GA_Offset>& dstptAttrib_h
        )
    {
        const GA_Detail* const geo = this->geo;
        const GA_VertexGroup* const seamGroup = vertexEdgeSeamGroup.getVertexGroup();
        UTparallelFor(groupParser.getPrimitiveSplittableRange(), [geo, &attrib_h, &dstptAttrib_h, seamGroup](const GA_SplittableRange& r)
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
    void
        primPrimEdge3(
            const GA_RWHandleT<UT_ValArray<GA_Offset>>& attrib_h,
            const GA_ROHandleT<GA_Offset>& dstptAttrib_h
        )
    {
        const GA_Detail* const geo = this->geo;
        const GA_VertexGroup* const seamGroup = vertexEdgeSeamGroup.getVertexGroup();
        UTparallelFor(groupParser.getPrimitiveSplittableRange(), [geo, &attrib_h, &dstptAttrib_h, seamGroup](const GA_SplittableRange& r)
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
    void
    primPrimEdge4(
        const GA_RWHandleT<UT_ValArray<GA_Offset>>& attrib_h
    )
    {
        const GA_Detail* const geo = this->geo;
        const GA_VertexGroup* const seamGroup = vertexEdgeSeamGroup.getVertexGroup();
        UTparallelFor(groupParser.getPrimitiveSplittableRange(), [geo, &attrib_h, seamGroup](const GA_SplittableRange& r)
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

    void
        primPrimPoint(
        const GA_RWHandleT<UT_ValArray<GA_Offset>>& attrib_h
    )
    {
        const GA_PointGroup* const seamGroup = pointSeamGroup.getPointGroup();
        
        GA_Topology& topo = geo->getTopology();
        //topo.makeFull();
        //topo.makePrimitiveRef();
        //topo.makeVertexRef();
        const GA_ATITopology* const vtxPrimRef = topo.getPrimitiveRef();
        const GA_ATITopology* const vtxPointRef = topo.getPointRef();
        const GA_ATITopology* const pointVtxRef = topo.getVertexRef();
        const GA_ATITopology* const vtxNextRef = topo.getVertexNextRef();


        const GA_Detail* const geo = this->geo;
        UTparallelFor(groupParser.getPrimitiveSplittableRange(), [geo, &attrib_h, seamGroup, vtxNextRef, vtxPointRef, pointVtxRef, vtxPrimRef](const GA_SplittableRange& r)
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











    
public:
     
    // bool outVertexPrimIndex       = false;
    // bool outVertexVertexPrim      = false;
    // bool outVertexPointDst        = false;
    // bool outVertexNextEquiv       = false;
    // bool outVertexNextEquivNoLoop = false;
    // bool outPointPointEdge        = false;
    // bool outPointPointPrim        = false;
    // bool outPrimPrimEdge          = false;
    // bool outPrimPrimPoint         = false;

    bool asOffset = true;
    // bool vertexPrimIndex_asOffset;
    // bool vertexVertexPrimPrev_asOffset;
    // bool vertexVertexPrimNext_asOffset;
    // bool vertexPointDst_asOffset;
    // bool vertexNextEquiv_asOffset;
    // bool vertexNextEquivNoLoop_asOffset;
    // bool pointPointEdge_asOffset;
    // bool pointPointPrim_asOffset;
    // bool primPrimEdge_asOffset;
    // bool primPrimPoint_asOffset;

    // UT_StringHolder vertexPrimIndexAttribName;
    // UT_StringHolder vertexVertexPrimPrevAttribName;
    // UT_StringHolder vertexVertexPrimNextAttribName;
    // UT_StringHolder vertexPointDstAttribName;
    // UT_StringHolder vertexNextEquivAttribName;
    // UT_StringHolder vertexNextEquivNoLoopAttribName;
    // UT_StringHolder pointPointEdgeAttribName;
    // UT_StringHolder pointPointPrimAttribName;
    // UT_StringHolder primPrimEdgeAttribName;
    // UT_StringHolder primPrimPointAttribName;

    
private:
    GA_Attribute* vertexPrimIndexAttrib       ;
    GA_Attribute* vertexVertexPrimPrevAttrib  ;
    GA_Attribute* vertexVertexPrimNextAttrib  ;
    GA_Attribute* vertexPointDstAttrib        ;
    GA_Attribute* vertexNextEquivAttrib       ;
    GA_Attribute* vertexNextEquivNoLoopAttrib ;
    GA_Attribute* pointPointEdgeAttrib        ;
    GA_Attribute* pointPointPrimAttrib        ;
    GA_Attribute* primPrimEdgeAttrib          ;
    GA_Attribute* primPrimPointAttrib         ;
    // GA_Attribute* vertexPrimIndexAttrib       = nullptr;
    // GA_Attribute* vertexVertexPrimPrevAttrib  = nullptr;
    // GA_Attribute* vertexVertexPrimNextAttrib  = nullptr;
    // GA_Attribute* vertexPointDstAttrib        = nullptr;
    // GA_Attribute* vertexNextEquivAttrib       = nullptr;
    // GA_Attribute* vertexNextEquivNoLoopAttrib = nullptr;
    // GA_Attribute* pointPointEdgeAttrib        = nullptr;
    // GA_Attribute* pointPointPrimAttrib        = nullptr;
    // GA_Attribute* primPrimEdgeAttrib          = nullptr;
    // GA_Attribute* primPrimPointAttrib         = nullptr;

    
    GFE_GroupParser pointSeamGroup;
    GFE_GroupParser vertexEdgeSeamGroup;
    GFE_GroupParser edgeSeamGroup;

    bool calVertexPrimIndex       = false;
    bool calVertexVertexPrim      = false;
    bool calVertexPointDst        = false;
    bool calVertexNextEquiv       = false;
    bool calVertexNextEquivNoLoop = false;
    bool calPointPointEdge        = false;
    bool calPointPointPrim        = false;
    bool calPrimPrimEdge          = false;
    bool calPrimPrimPoint         = false;
    
    int kernel = 0;
    
    int subscribeRatio = 64;
    int minGrainSize = 64;

}; // End of class GFE_Adjacency







































































namespace GFE_Adjacency_Namespace {



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
            const GA_Size vtxpnum = GFE_TopologyReference_Namespace::vertexPrimIndex(geo, primoff, vtxoff_next);

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

















    //GFE_Adjacency_Namespace::addAttribPointPointEdge(geo, name, geoGroup, seamGroup, storage, nullptr, nullptr, GA_ReuseStrategy(), subscribeRatio, minGrainSize);
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

        GFE_TopologyReference_Namespace::addAttribVertexVertexPrim(geo, vtxPrevAttrib, vtxNextAttrib, nullptr, finalStorage);

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





    //GFE_Adjacency_Namespace::addAttribPrimPrimEdge(geo, name, geoGroup, seamGroup, storage, nullptr, nullptr, GA_ReuseStrategy(), subscribeRatio, minGrainSize);
    
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
        //GA_Attribute* dstptAttrib = GFE_TopologyReference_Namespace::addAttribVertexPointDst(geo, "dstpt", nullptr, GA_Defaults(-1), finalStorage, subscribeRatio, minGrainSize);
        //GA_RWHandleT<GA_Offset> dstptAttrib_h(dstptAttrib);
        //GA_Attribute* vtxPrimNextAttrib = addAttribVertexVertexPrimNext(geo, "vtxPrimNext", nullptr, GA_Defaults(-1), finalStorage, subscribeRatio, minGrainSize);

        GA_Attribute* vtxNextEquivAttrib = GFE_VertexNextEquiv_Namespace::addAttribVertexNextEquiv(geo, nullptr, finalStorage);

        attribPtr = geo->getAttributes().createArrayAttribute(GA_ATTRIB_PRIMITIVE, GFE_TOPO_SCOPE, name, finalStorage, 1, creation_args, attribute_options, "arraydata", reuse);
        //attribPtr = geo->addIntArray(GA_ATTRIB_PRIMITIVE, GFE_TOPO_SCOPE, name, 1, creation_args, attribute_options, finalStorage, reuse);
        //GA_ROHandleT<UT_ValArray<GA_Offset>> attrib_h(attribPtr);
        primPrimEdge(geo, attribPtr, vtxNextEquivAttrib, geoGroup, seamGroup, subscribeRatio, minGrainSize);
        return attribPtr;
    }




    //GFE_Adjacency_Namespace::addAttribAdjacency(geo, groupType, connectivityGroupType);

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
                //return GFE_Adjacency_Namespace::addAttribPrimPrimPrim(geo,
                //    UTverify_cast<const GA_PrimitiveGroup*>(geoGroup),
                //    UTverify_cast<const GA_PrimitiveGroup*>(seamGroup),
                //    storage, name, creation_args, attribute_options, reuse, subscribeRatio, minGrainSize);
                break;
            case GA_GROUP_POINT:
                //return GFE_Adjacency_Namespace::addAttribPrimPrimPoint(geo,
                //    UTverify_cast<const GA_PrimitiveGroup*>(geoGroup),
                //    UTverify_cast<const GA_PointGroup*>(seamGroup),
                //    storage, name, creation_args, attribute_options, reuse, subscribeRatio, minGrainSize);
                break;
            case GA_GROUP_VERTEX:
                //return GFE_Adjacency_Namespace::addAttribPrimPrimVertex(geo,
                //    UTverify_cast<const GA_PrimitiveGroup*>(geoGroup),
                //    UTverify_cast<const GA_VertexGroup*>(seamGroup),
                //    storage, name, creation_args, attribute_options, reuse, subscribeRatio, minGrainSize);
                break;
            case GA_GROUP_EDGE:
                return addAttribPrimPrimEdge(geo,
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
                //return GFE_Adjacency_Namespace::addAttribPointPointPrim(geo,
                //    UTverify_cast<const GA_PointGroup*>(geoGroup),
                //    UTverify_cast<const GA_PrimitiveGroup*>(seamGroup),
                //    storage, name, creation_args, attribute_options, reuse, subscribeRatio, minGrainSize);
                break;
            case GA_GROUP_POINT:
                //return GFE_Adjacency_Namespace::addAttribPointPointPoint(geo,
                //    UTverify_cast<const GA_PointGroup*>(geoGroup),
                //    UTverify_cast<const GA_PointGroup*>(seamGroup),
                //    storage, name, creation_args, attribute_options, reuse, subscribeRatio, minGrainSize);
                break;
            case GA_GROUP_VERTEX:
                //return GFE_Adjacency_Namespace::addAttribPointPointVertex(geo,
                //    UTverify_cast<const GA_PointGroup*>(geoGroup),
                //    UTverify_cast<const GA_VertexGroup*>(seamGroup),
                //    storage, name, creation_args, attribute_options, reuse, subscribeRatio, minGrainSize);
                break;
            case GA_GROUP_EDGE:
                return GFE_Adjacency_Namespace::addAttribPointPointEdge(geo,
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
                //return GFE_Adjacency_Namespace::addAttribVertexVertexPrim(geo,
                //    UTverify_cast<const GA_VertexGroup*>(geoGroup),
                //    UTverify_cast<const GA_PrimitiveGroup*>(seamGroup),
                //    storage, name, creation_args, attribute_options, reuse, subscribeRatio, minGrainSize);
                break;
            case GA_GROUP_POINT:
                //return GFE_Adjacency_Namespace::addAttribVertexVertexPoint(geo,
                //    UTverify_cast<const GA_VertexGroup*>(geoGroup),
                //    UTverify_cast<const GA_PointGroup*>(seamGroup),
                //    storage, name, creation_args, attribute_options, reuse, subscribeRatio, minGrainSize);
                break;
            case GA_GROUP_VERTEX:
                //return GFE_Adjacency_Namespace::addAttribVertexVertexVertex(geo,
                //    UTverify_cast<const GA_VertexGroup*>(geoGroup),
                //    UTverify_cast<const GA_VertexGroup*>(seamGroup),
                //    storage, name, creation_args, attribute_options, reuse, subscribeRatio, minGrainSize);
                break;
            case GA_GROUP_EDGE:
                //return GFE_Adjacency_Namespace::addAttribVertexVertexEdge(geo,
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
