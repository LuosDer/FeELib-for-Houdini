
#pragma once

#ifndef __GFE_MeshTopology_h__
#define __GFE_MeshTopology_h__

#include "GFE/GFE_MeshTopology.h"



#include "GFE/GFE_GeoFilter.h"
#include "GFE/GFE_OffsetAttributeToIndex.h"



enum class GFE_MeshTopologyType {
    VertexPrimIndex,
    VertexVertexPrim,
    VertexVertexPrimPrev,
    VertexVertexPrimNext,
    VertexNextEquiv,
    VertexNextEquivNoLoop,
    VertexPointDst,
    PointPointEdge,
    PointPointPrim,
    PrimPrimEdge,
    PrimPrimPoint,
};



class GFE_MeshTopology : public GFE_AttribFilter {

public:
    //using GFE_AttribFilter::GFE_AttribFilter;

    GFE_MeshTopology(
        GFE_Detail* const geo,
        const SOP_NodeVerb::CookParms* const cookparms = nullptr
    )
        : GFE_AttribFilter(geo, cookparms)
        , pointSeamGroup(geo, groupParser.getGOPRef(), cookparms)
        , vertexEdgeSeamGroup(geo, groupParser.getGOPRef(), cookparms)
        , edgeSeamGroup(geo, groupParser.getGOPRef(), cookparms)
    {
    }

    GFE_MeshTopology(
        GA_Detail& geo,
        const SOP_NodeVerb::CookParms* const cookparms = nullptr
    )
        : GFE_AttribFilter(geo, cookparms)
        , pointSeamGroup(geo, groupParser.getGOPRef(), cookparms)
        , vertexEdgeSeamGroup(geo, groupParser.getGOPRef(), cookparms)
        , edgeSeamGroup(geo, groupParser.getGOPRef(), cookparms)
    {
    }

    GFE_MeshTopology(
        GA_Detail* const geo,
        const SOP_NodeVerb::CookParms* const cookparms = nullptr
    )
        : GFE_AttribFilter(geo, cookparms)
        , pointSeamGroup(geo, groupParser.getGOPRef(), cookparms)
        , vertexEdgeSeamGroup(geo, groupParser.getGOPRef(), cookparms)
        , edgeSeamGroup(geo, groupParser.getGOPRef(), cookparms)
    {
    }

    void clear()
    {
        vertexPrimIndexAttrib       = nullptr;
        vertexVertexPrimPrevAttrib  = nullptr;
        vertexVertexPrimNextAttrib  = nullptr;
        vertexPointDstAttrib        = nullptr;
        vertexNextEquivAttrib       = nullptr;
        vertexNextEquivNoLoopAttrib = nullptr;
        pointPointEdgeAttrib        = nullptr;
        pointPointPrimAttrib        = nullptr;
        primPrimEdgeAttrib          = nullptr;
        primPrimPointAttrib         = nullptr;
        vertexNextEquivGroup        = nullptr;
        vertexNextEquivNoLoopGroup  = nullptr;
        unsharedEdgeGroup           = nullptr;
    }
    
    virtual void reset(GA_Detail& geo, const SOP_NodeVerb::CookParms* const cookparms = nullptr) override
    {
        GFE_AttribFilter::reset(geo, cookparms);
        pointSeamGroup.reset(geo, cookparms);
        vertexEdgeSeamGroup.reset(geo, cookparms);
        edgeSeamGroup.reset(geo, cookparms);
        clear();
    }

    SYS_FORCE_INLINE virtual void reset(GA_Detail* const geo, const SOP_NodeVerb::CookParms* const cookparms = nullptr) override
    { reset(*geo, cookparms); }


    
    void setComputeParm(
        const bool outAsOffset = true,
        const exint subscribeRatio = 64,
        const exint minGrainSize = 1024
    )
    {
        setHasComputed();
        this->outAsOffset = outAsOffset;
        this->subscribeRatio = subscribeRatio;
        this->minGrainSize = minGrainSize;
    }

    SYS_FORCE_INLINE void setKernel(const int kernel)
    { this->kernel = kernel; }



    SYS_FORCE_INLINE GA_EdgeGroup* setUnsharedEdgeGroup(const bool detached = false, const UT_StringRef& groupName = "__topo_unshared")
    {
        unsharedEdgeGroup = getOutGroupArray().findOrCreateEdge(detached, groupName);
        return unsharedEdgeGroup;
    }
    SYS_FORCE_INLINE void setUnsharedEdgeGroup(GA_EdgeGroup* const groupPtr)
    { unsharedEdgeGroup = groupPtr; }
    
    SYS_FORCE_INLINE GA_EdgeGroup* getUnsharedEdgeGroup() const
    { return unsharedEdgeGroup; }
    


    
    SYS_FORCE_INLINE void setVertexVertexPrim(
        const bool detached = false,
        const UT_StringRef& attribName0 = "__topo_vertexVertexPrimPrev",
        const UT_StringRef& attribName1 = "__topo_vertexVertexPrimNext",
        const GA_Storage storage = GA_STORE_INVALID
    )
    {
        vertexVertexPrimPrevAttrib = getOutAttribArray().findOrCreateTuple(detached,
            GA_ATTRIB_VERTEX, GA_STORECLASS_INT, storage, attribName0, 1, GA_Defaults(GFE_INVALID_OFFSET));
        vertexVertexPrimNextAttrib = getOutAttribArray().findOrCreateTuple(detached,
            GA_ATTRIB_VERTEX, GA_STORECLASS_INT, storage, attribName1, 1, GA_Defaults(GFE_INVALID_OFFSET));
    }
    SYS_FORCE_INLINE void setVertexVertexPrim(GA_Attribute* const attribPtr0, GA_Attribute* const attribPtr1)
    {
        vertexVertexPrimPrevAttrib = attribPtr0;
        vertexVertexPrimNextAttrib = attribPtr1;
    }



#define GFE_SETATTRIB_SPECIALIZATION(ATTRIB_NAME_UPPER, ATTRIB_NAME_LOWNER, ATTRIB_OWNER)              \
    SYS_FORCE_INLINE GA_Attribute* set##ATTRIB_NAME_UPPER(                                             \
        const bool detached = false,                                                                   \
        const UT_StringRef& attribName = "__topo_"#ATTRIB_NAME_LOWNER,                                 \
        const GA_Storage storage = GA_STORE_INVALID                                                    \
    )                                                                                                  \
    {                                                                                                  \
        ATTRIB_NAME_LOWNER##Attrib = getOutAttribArray().findOrCreateTuple(detached,                   \
            ATTRIB_OWNER, GA_STORECLASS_INT, storage, attribName, 1, GA_Defaults(GFE_INVALID_OFFSET)); \
        return ATTRIB_NAME_LOWNER##Attrib;                                                             \
    }                                                                                                  \
    SYS_FORCE_INLINE void set##ATTRIB_NAME_UPPER(GA_Attribute* const attribPtr)                        \
    { ATTRIB_NAME_LOWNER##Attrib = attribPtr; }                                                        \
    
    GFE_SETATTRIB_SPECIALIZATION(VertexPrimIndex,       vertexPrimIndex,       GA_ATTRIB_VERTEX)
    GFE_SETATTRIB_SPECIALIZATION(VertexPointDst,        vertexPointDst,        GA_ATTRIB_VERTEX)
    GFE_SETATTRIB_SPECIALIZATION(VertexNextEquiv,       vertexNextEquiv,       GA_ATTRIB_VERTEX)
    GFE_SETATTRIB_SPECIALIZATION(VertexNextEquivNoLoop, vertexNextEquivNoLoop, GA_ATTRIB_VERTEX)

#undef GFE_SETATTRIB_SPECIALIZATION
    

        



#define GFE_SETATTRIB_ARRAY_SPECIALIZATION(ATTRIB_NAME_UPPER, ATTRIB_NAME_LOWNER, ATTRIB_OWNER)  \
    SYS_FORCE_INLINE GA_Attribute* set##ATTRIB_NAME_UPPER(                                       \
        const bool detached = false,                                                             \
        const UT_StringRef& attribName = "__topo_"#ATTRIB_NAME_LOWNER,                           \
        const GA_Storage storage = GA_STORE_INVALID                                              \
    )                                                                                            \
    {                                                                                            \
        ATTRIB_NAME_LOWNER##Attrib = getOutAttribArray().findOrCreateArray(detached,             \
            ATTRIB_OWNER, GA_STORECLASS_INT, storage, attribName);                               \
        return ATTRIB_NAME_LOWNER##Attrib;                                                       \
    }                                                                                            \
    SYS_FORCE_INLINE void set##ATTRIB_NAME_UPPER(GA_Attribute* const attribPtr)                  \
    { ATTRIB_NAME_LOWNER##Attrib = attribPtr; }                                                  \

    GFE_SETATTRIB_ARRAY_SPECIALIZATION(PointPointEdge, pointPointEdge, GA_ATTRIB_POINT)
    GFE_SETATTRIB_ARRAY_SPECIALIZATION(PointPointPrim, pointPointPrim, GA_ATTRIB_POINT)
    GFE_SETATTRIB_ARRAY_SPECIALIZATION(PrimPrimEdge,   primPrimEdge,   GA_ATTRIB_PRIMITIVE)
    GFE_SETATTRIB_ARRAY_SPECIALIZATION(PrimPrimPoint,  primPrimPoint,  GA_ATTRIB_PRIMITIVE)

#undef GFE_SETATTRIB_ARRAY_SPECIALIZATION



    

#define GFE_SETGROUP_SPECIALIZATION(ATTRIB_NAME_UPPER, ATTRIB_NAME_LOWNER, ATTRIB_OWNER)     \
    SYS_FORCE_INLINE GA_VertexGroup* set##ATTRIB_NAME_UPPER##Group(                          \
        const bool detached = false,                                                         \
        const UT_StringRef& groupName = "__topo_"#ATTRIB_NAME_LOWNER                         \
    )                                                                                        \
    {                                                                                        \
        ATTRIB_NAME_LOWNER##Group = static_cast<GA_VertexGroup*>(                            \
            getOutGroupArray().findOrCreate(detached, ATTRIB_OWNER, groupName));             \
        return ATTRIB_NAME_LOWNER##Group;                                                    \
    }                                                                                        \
    SYS_FORCE_INLINE void set##ATTRIB_NAME_UPPER(GA_VertexGroup* const groupPtr)             \
    { ATTRIB_NAME_LOWNER##Group = groupPtr; }                                                \
    
    GFE_SETGROUP_SPECIALIZATION(VertexNextEquiv,        vertexNextEquiv,        GA_GROUP_VERTEX)
    GFE_SETGROUP_SPECIALIZATION(VertexNextEquivNoLoop,  vertexNextEquivNoLoop,  GA_GROUP_VERTEX)
    
#undef GFE_SETGROUP_SPECIALIZATION

    

#define GFE_GETATTRIB_SPECIALIZATION(ATTRIB_NAME_UPPER, ATTRIB_NAME_LOWNER)   \
    SYS_FORCE_INLINE GA_Attribute* get##ATTRIB_NAME_UPPER() const             \
    { return ATTRIB_NAME_LOWNER##Attrib; }                                    \

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

        

#define GFE_GETGROUP_SPECIALIZATION(ATTRIB_NAME_UPPER, ATTRIB_NAME_LOWNER)     \
SYS_FORCE_INLINE GA_VertexGroup* get##ATTRIB_NAME_UPPER##Group() const         \
{ return ATTRIB_NAME_LOWNER##Group; }                                          \

GFE_GETGROUP_SPECIALIZATION(VertexNextEquiv,       vertexNextEquiv)
GFE_GETGROUP_SPECIALIZATION(VertexNextEquivNoLoop, vertexNextEquivNoLoop)

#undef GFE_GETGROUP_SPECIALIZATION



    

    GA_Attribute* setAdjacency(
        const bool detached = false,
        const GA_AttributeOwner attribOwner = GA_ATTRIB_POINT,
        const bool largeConnectivity = true,
        const UT_StringRef& attribName = "__topo_adjacency",
        const GA_Storage storage = GA_STORE_INVALID
    )
    {
        switch (attribOwner)
        {
        case GA_ATTRIB_PRIMITIVE:
            return largeConnectivity ? setPrimPrimPoint (detached, attribName, storage) : setPrimPrimEdge  (detached, attribName, storage);
            break;
        case GA_ATTRIB_POINT:
            return largeConnectivity ? setPointPointPrim(detached, attribName, storage) : setPointPointEdge(detached, attribName, storage);
            break;
        case GA_ATTRIB_VERTEX:
            break;
        }
        return nullptr;
    }
   
    
    SYS_FORCE_INLINE GA_Attribute* setAdjacency(
        const bool detached = false,
        const GA_GroupType groupType = GA_GROUP_POINT,
        const bool largeConnectivity = true,
        const UT_StringRef& attribName = "__topo_adjacency",
        const GA_Storage storage = GA_STORE_INVALID
    )
    { return setAdjacency(detached, GFE_Type::attributeOwner_groupType(groupType), largeConnectivity, attribName, storage); }
   



    
private:

    virtual bool
        computeCore() override
    {
        // calVertexPrimIndex       = outVertexPrimIndex;
        // calVertexVertexPrim      = outVertexVertexPrim;
        // calVertexPointDst        = outVertexPointDst;
        // calVertexNextEquiv       = outVertexNextEquiv;
        // calVertexNextEquivNoLoop = outVertexNextEquivNoLoop;
        // calPointPointEdge        = outPointPointEdge;
        // calPointPointPrim        = outPointPointPrim;
        // calPrimPrimEdge          = outPrimPrimEdge;
        // calPrimPrimPoint         = outPrimPrimPoint;
        
        if (!vertexPrimIndexAttrib &&
            !vertexVertexPrimPrevAttrib &&
            !vertexVertexPrimNextAttrib &&
            !vertexPointDstAttrib &&
            !vertexNextEquivAttrib &&
            !vertexNextEquivNoLoopAttrib &&
            !pointPointEdgeAttrib &&
            !pointPointPrimAttrib &&
            !primPrimEdgeAttrib &&
            !primPrimPointAttrib)
                return false;

        if (groupParser.isEmpty())
            return true;
        
        //if (vertexEdgeSeamGroup)
        //    GFE_GroupUnion::groupUnion(vertexEdgeSeamGroup, edgeSeamGroup);

        const bool calVertexNextEquivNoLoop =
                   vertexNextEquivNoLoopAttrib ||
                   vertexNextEquivNoLoopGroup;
        
        const bool calVertexNextEquiv =
                   vertexNextEquivAttrib ||
                   vertexNextEquivGroup  ||
                   primPrimEdgeAttrib;
        
        const bool calVertexVertexPrim =
                   vertexVertexPrimPrevAttrib  ||
                   vertexVertexPrimNextAttrib  ||
                   pointPointEdgeAttrib;
        
        const bool calVertexPointDst =
                   vertexPointDstAttrib        ||
                   calVertexNextEquiv          ||
                   calVertexNextEquivNoLoop;
        
        const bool calVertexPrimIndex =
                   vertexPrimIndexAttrib       ||
                   calVertexPointDst           ||
                   pointPointEdgeAttrib;
        
        
        
        if (calVertexPrimIndex)
        {
            vertexPrimIndex();
        }
        if (calVertexPointDst)
        {
            switch ((calVertexNextEquiv || calVertexNextEquivNoLoop) ? 0 : kernel)
            {
            default:
            case 0: vertexPointDstByVtxpnum(); break;
            case 1: vertexPointDst(); break;
            }
        }
        if (calVertexVertexPrim)
        {
            switch (pointPointEdgeAttrib ? 0 : kernel)
            {
            default:
            case 0: vertexVertexPrim();  break;
            case 1: vertexVertexPrim1(); break;
            }
        }
        if (calVertexNextEquiv)
        {
            vertexNextEquiv();
        }
        if (calVertexNextEquivNoLoop)
        {
            vertexNextEquivNoLoop();
        }
        if (pointPointEdgeAttrib)
        {
            switch (kernel)
            {
            default:
            case 0:  pointPointEdge();  break;
            case 1:  pointPointEdge1(); break;
            case 2:  pointPointEdge2(); break;
            }
        }
        if (pointPointPrimAttrib)
        {
            //GFE_MeshTopology_Namespace::pointPointPrim(geo, attribHandle,
            //    groupParser.getPointGroup(), nullptr,
            //    subscribeRatio, minGrainSize);
        }
        if (primPrimEdgeAttrib)
        {
            switch (kernel)
            {
            default:
            case 0: primPrimEdge();  break;
            case 1: primPrimEdge1(); break;
            case 2: primPrimEdge2(); break;
            case 3: primPrimEdge3(); break;
            case 4: primPrimEdge4(); break;
            }
        }
        if (primPrimPointAttrib)
        {
            primPrimPoint();
            //primPrimPoint(primPrimPointAttrib);
        }


        if(outAsOffset)
            return true;

        GFE_OffsetAttribToIndex offsetAttribToIndex(geo);
        //offsetAttribToIndex.offsetToIndex = true;
        
        // offsetAttribToIndex.getOutAttribArray().append(vertexPrimIndexAttrib);
        offsetAttribToIndex.getOutAttribArray().append(vertexVertexPrimPrevAttrib);
        offsetAttribToIndex.getOutAttribArray().append(vertexVertexPrimNextAttrib);
        offsetAttribToIndex.getOutAttribArray().append(vertexPointDstAttrib);
        offsetAttribToIndex.getOutAttribArray().append(vertexNextEquivAttrib);
        offsetAttribToIndex.getOutAttribArray().append(vertexNextEquivNoLoopAttrib);
        offsetAttribToIndex.getOutAttribArray().append(pointPointEdgeAttrib);
        offsetAttribToIndex.getOutAttribArray().append(pointPointPrimAttrib);
        offsetAttribToIndex.getOutAttribArray().append(primPrimEdgeAttrib);
        offsetAttribToIndex.getOutAttribArray().append(primPrimPointAttrib);

        offsetAttribToIndex.compute();
        return true;
    }




    //Get all vertices NextEquiv Vertex
    //template<typename T>
    void vertexPrimIndex()
    {
        if(!vertexPrimIndexAttrib)
            setVertexPrimIndex(!outIntermediateAttrib);
        
        int_wh.bind(vertexPrimIndexAttrib);
        //const GA_Detail* const geo = this->geo;
        if (groupParser.isFull())
        {
            //const GA_SplittableRange geoSplittableRange0(geo->getPrimitiveRange(nullptr));
            UTparallelFor(groupParser.getPrimitiveSplittableRange(), [this](const GA_SplittableRange& r)
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
                            int_wh.set(vertices[vtxpnum], vtxpnum);
                        }
                    }
                }
            }, subscribeRatio, minGrainSize);
        }
        else
        {
            const GA_VertexGroup* const geoGroup = groupParser.getVertexGroup();
            //const GA_SplittableRange geoSplittableRange0(groupParser.getPrimitiveRange());
            UTparallelFor(groupParser.getPrimitiveSplittableRange(), [this, geoGroup](const GA_SplittableRange& r)
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
                            int_wh.set(vertices[vtxpnum], vtxpnum);
                        }
                    }
                }
            }, subscribeRatio, minGrainSize);
        }
    }







    void vertexPointDstByVtxpnum()
    {
        if(!vertexPointDstAttrib)
            setVertexPointDst(!outIntermediateAttrib);
        
        int_wh.bind(vertexPointDstAttrib);
        int_oh.bind(vertexPrimIndexAttrib);
        
        const GA_Topology& topo = geo->getTopology();
        //topo.makePrimitiveRef();
        const GA_ATITopology* const vtxPrimRef = topo.getPrimitiveRef();

        UTparallelFor(groupParser.getVertexSplittableRange(), [this, vtxPrimRef](const GA_SplittableRange& r)
        {
            GA_Offset start, end;
            for (GA_Iterator it(r); it.blockAdvance(start, end); )
            {
                for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                {
                    int_wh.set(elemoff, geo->vertexPointDst(vtxPrimRef->getLink(elemoff), int_oh.get(elemoff)));
                }
            }
        }, subscribeRatio, minGrainSize);
    }




    void vertexVertexPrim()
    {
        if(!vertexVertexPrimPrevAttrib && !vertexVertexPrimNextAttrib)
            setVertexVertexPrim(!outIntermediateAttrib);
        
        int_wh.bind(vertexVertexPrimPrevAttrib);
        int1_wh.bind(vertexVertexPrimNextAttrib);
        
        //const GA_Detail* const geo = this->geo;
        if (groupParser.isFull())
        {
            const GA_SplittableRange geoSplittableRange0(geo->getPrimitiveRange());
            UTparallelFor(geoSplittableRange0, [this](const GA_SplittableRange& r)
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
                            if (vertexVertexPrimPrevAttrib)
                                int_wh.set(vertices[0], vertices[lastIndex]);
                            if (vertexVertexPrimNextAttrib)
                                int1_wh.set(vertices[lastIndex], vertices[0]);
                        }
                        else
                        {
                            if (vertexVertexPrimPrevAttrib)
                                int_wh.set(vertices[0], GFE_INVALID_OFFSET);
                            if (vertexVertexPrimNextAttrib)
                                int1_wh.set(vertices[lastIndex], GFE_INVALID_OFFSET);
                        }
                        GA_Offset vtxoff_prev = vertices[0];
                        GA_Offset vtxoff_next;
                        for (GA_Size vtxpnum = 1; vtxpnum <= lastIndex; ++vtxpnum)
                        {
                            vtxoff_next = vertices[vtxpnum];
                            if (vertexVertexPrimPrevAttrib)
                                int_wh.set(vtxoff_next, vtxoff_prev);
                            if (vertexVertexPrimNextAttrib)
                                int1_wh.set(vtxoff_prev, vtxoff_next);
                            vtxoff_prev = vtxoff_next;
                        }
                    }
                }
            }, subscribeRatio, minGrainSize);
        }
        else
        {
            const GA_VertexGroup* const geoGroup = groupParser.getVertexGroup();
            UTparallelFor(groupParser.getPrimitiveSplittableRange(), [this, geoGroup](const GA_SplittableRange& r)
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
                            if (vertexVertexPrimPrevAttrib)
                                int_wh.set(vertices[0], vertices[lastIndex]);
                            if (vertexVertexPrimNextAttrib)
                                int1_wh.set(vertices[lastIndex], vertices[0]);
                        }
                        else
                        {
                            if (vertexVertexPrimPrevAttrib)
                                int_wh.set(vertices[0], GFE_INVALID_OFFSET);
                            if (vertexVertexPrimNextAttrib)
                                int1_wh.set(vertices[lastIndex], GFE_INVALID_OFFSET);
                        }
                        GA_Offset vtxoff_prev = vertices[0];
                        GA_Offset vtxoff_next;
                        for (GA_Size vtxpnum = 1; vtxpnum <= lastIndex; ++vtxpnum)
                        {
                            vtxoff_next = vertices[vtxpnum];
                            if (!geoGroup->contains(vtxoff_next))
                                continue;
                            if (vertexVertexPrimPrevAttrib)
                                int_wh.set(vtxoff_next, vtxoff_prev);
                            if (vertexVertexPrimNextAttrib)
                                int1_wh.set(vtxoff_prev, vtxoff_next);
                            vtxoff_prev = vtxoff_next;
                        }
                    }
                }
            }, subscribeRatio, minGrainSize);
        }
    }

    void
        vertexVertexPrim1()
    {
        if(!vertexVertexPrimPrevAttrib && !vertexVertexPrimNextAttrib)
            setVertexVertexPrim(!outIntermediateAttrib);
        
        int_wh.bind(vertexVertexPrimPrevAttrib);
        int1_wh.bind(vertexVertexPrimNextAttrib);
        
        const GA_Topology& topo = geo->getTopology();
        UTparallelFor(groupParser.getVertexSplittableRange(), [&topo, this](const GA_SplittableRange& r)
        {
            GA_Offset start, end;
            GA_Offset vtxoff_prev, vtxoff_next;
            for (GA_Iterator it(r); it.blockAdvance(start, end); )
            {
                for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                {
                    topo.getAdjacentBoundaryVertices(elemoff, vtxoff_prev, vtxoff_next);
                    if (vertexVertexPrimPrevAttrib)
                        int_wh.set(elemoff, vtxoff_prev);
                    if (vertexVertexPrimNextAttrib)
                        int1_wh.set(elemoff, vtxoff_next);
                }
            }
        }, subscribeRatio, minGrainSize);
    }



    
    void vertexPointDst()
    {
        if(!vertexPointDstAttrib)
            setVertexPointDst(!outIntermediateAttrib);
        
        //const GA_Detail* const geo = this->geo;
        const GA_Topology& topo = geo->getTopology();
        const GA_ATITopology* const vtxPointRef = topo.getPointRef();

        //const GA_VertexGroup* const geoGroup = groupParser.getVertexGroup();
        //const GA_SplittableRange geoSplittableRange0(geo->getVertexRange(geoGroup));
        UTparallelFor(groupParser.getVertexSplittableRange(), [this, vtxPointRef](const GA_SplittableRange& r)
        {
            GA_PageHandleScalar<GA_Offset>::RWType dstpt_ph(vertexPointDstAttrib);
            GA_PageHandleScalar<GA_Offset>::ROType vtxPrimNext_ph(vertexVertexPrimNextAttrib);
            for (GA_PageIterator pit = r.beginPages(); !pit.atEnd(); ++pit)
            {
                GA_Offset start, end;
                for (GA_Iterator it(pit.begin()); it.blockAdvance(start, end); )
                {
                    dstpt_ph.setPage(start);
                    vtxPrimNext_ph.setPage(start);
                    for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                    {
                        if (vtxPrimNext_ph.value(elemoff) == GFE_INVALID_OFFSET)
                        {
                            dstpt_ph.value(elemoff) = GFE_INVALID_OFFSET;
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




    
    

    //Get all vertices NextEquiv Vertex
    void
        vertexNextEquiv()
    {
        if(!vertexNextEquivAttrib && !vertexNextEquivGroup)
            setVertexNextEquiv(!outIntermediateAttrib);
        
        int_wh.bind(vertexNextEquivAttrib);
        int_oh.bind(vertexPointDstAttrib);
        
        const GA_Topology& topo = geo->getTopology();
        //topo.makeVertexRef();
        const GA_ATITopology* const vtxPointRef = topo.getPointRef();
        const GA_ATITopology* const pointVtxRef = topo.getVertexRef();
        const GA_ATITopology* const vtxNextRef = topo.getVertexNextRef();

        UTparallelFor(groupParser.getVertexSplittableRange(),
            [this, vtxPointRef, pointVtxRef, vtxNextRef](const GA_SplittableRange& r)
        {
            GA_Offset vtxoff_next, dstpt, ptnum;
            GA_Offset start, end;
            for (GA_Iterator it(r); it.blockAdvance(start, end); )
            {
                for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                {
                    dstpt = int_oh.get(elemoff);
                    if (dstpt == GFE_INVALID_OFFSET)
                    {
                        if (vertexNextEquivAttrib)
                            int_wh.set(elemoff, GFE_INVALID_OFFSET);
                        continue;
                    }

                    for (vtxoff_next = vtxNextRef->getLink(elemoff); vtxoff_next != GFE_INVALID_OFFSET; vtxoff_next = vtxNextRef->getLink(vtxoff_next))
                    {
                        if (int_oh.get(vtxoff_next) != dstpt)
                            continue;
                        dstpt = GFE_INVALID_OFFSET;
                        if (vertexNextEquivAttrib)
                            int_wh.set(elemoff, vtxoff_next);
                        break;
                    }

                    if (dstpt == GFE_INVALID_OFFSET)
                        continue;

                    ptnum = vtxPointRef->getLink(elemoff);
                    for (vtxoff_next = pointVtxRef->getLink(dstpt); vtxoff_next != GFE_INVALID_OFFSET; vtxoff_next = vtxNextRef->getLink(vtxoff_next))
                    {
                        if (int_oh.get(vtxoff_next) != ptnum)
                            continue;
                        dstpt = GFE_INVALID_OFFSET;
                        if (vertexNextEquivAttrib)
                            int_wh.set(elemoff, vtxoff_next);
                        break;
                    }
                    if (dstpt != GFE_INVALID_OFFSET)
                    {
                        if (vertexNextEquivGroup)
                            vertexNextEquivGroup->setElement(elemoff, true);
                        if (vertexNextEquivAttrib)
                            int_wh.set(elemoff, GFE_INVALID_OFFSET);
                    }
                }
            }
        }, subscribeRatio, minGrainSize);
        
        if (vertexNextEquivGroup)
            vertexNextEquivGroup->invalidateGroupEntries();
    }


    
    void
        vertexNextEquivNoLoop()
    {
        if(!vertexNextEquivNoLoopAttrib && !vertexNextEquivNoLoopGroup)
            setVertexNextEquivNoLoop(!outIntermediateAttrib);
        
        int_wh.bind(vertexNextEquivNoLoopAttrib);
        int_oh.bind(vertexPointDstAttrib);
        
        const GA_Topology& topo = geo->getTopology();
        //topo.makeVertexRef();
        const GA_ATITopology* const vtxPointRef = topo.getPointRef();
        const GA_ATITopology* const pointVtxRef = topo.getVertexRef();
        const GA_ATITopology* const vtxNextRef = topo.getVertexNextRef();

        UTparallelFor(groupParser.getVertexSplittableRange(),
            [this, vtxPointRef, pointVtxRef, vtxNextRef](const GA_SplittableRange& r)
        {
            GA_Offset vtxoff_next, dstpt, ptnum;
            GA_Offset start, end;
            for (GA_Iterator it(r); it.blockAdvance(start, end); )
            {
                for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                {
                    dstpt = int_oh.get(elemoff);
                    if (dstpt == GFE_INVALID_OFFSET)
                    {
                        if (vertexNextEquivNoLoopAttrib)
                            int_wh.set(elemoff, GFE_INVALID_OFFSET);
                        continue;
                    }

                    for (vtxoff_next = vtxNextRef->getLink(elemoff); vtxoff_next != GFE_INVALID_OFFSET; vtxoff_next = vtxNextRef->getLink(vtxoff_next))
                    {
                        if (int_oh.get(vtxoff_next) != dstpt)
                            continue;
                        dstpt = GFE_INVALID_OFFSET;
                        if (vertexNextEquivNoLoopAttrib)
                            int_wh.set(elemoff, vtxoff_next);
                        break;
                    }

                    if (dstpt == GFE_INVALID_OFFSET)
                        continue;

                    ptnum = vtxPointRef->getLink(elemoff);
                    for (vtxoff_next = pointVtxRef->getLink(dstpt); vtxoff_next != GFE_INVALID_OFFSET; vtxoff_next = vtxNextRef->getLink(vtxoff_next))
                    {
                        if (int_oh.get(vtxoff_next) != ptnum)
                            continue;
                        if (dstpt > ptnum)
                        {
                            dstpt = GFE_INVALID_OFFSET;
                            if (vertexNextEquivNoLoopAttrib)
                                int_wh.set(elemoff, vtxoff_next);
                        }
                        break;
                    }
                    if (dstpt != GFE_INVALID_OFFSET)
                    {
                        if (vertexNextEquivNoLoopGroup)
                            vertexNextEquivNoLoopGroup->setElement(elemoff, true);
                        if (vertexNextEquivNoLoopAttrib)
                            int_wh.set(elemoff, GFE_INVALID_OFFSET);
                    }
                }
            }
        }, subscribeRatio, minGrainSize);
        
        if (vertexNextEquivNoLoopGroup)
            vertexNextEquivNoLoopGroup->invalidateGroupEntries();
    }




    
    void unsharedEdge()
    {
        if(!unsharedEdgeGroup)
            setUnsharedEdgeGroup(!outIntermediateAttrib);
        
        int_wh.bind(vertexNextEquivNoLoopAttrib);
        int_oh.bind(vertexPointDstAttrib);
        
        const GA_Topology& topo = geo->getTopology();
        //topo.makeVertexRef();
        const GA_ATITopology* const vtxPointRef = topo.getPointRef();
        const GA_ATITopology* const pointVtxRef = topo.getVertexRef();
        const GA_ATITopology* const vtxNextRef = topo.getVertexNextRef();

        UTparallelFor(groupParser.getVertexSplittableRange(),
            [this, vtxPointRef, pointVtxRef, vtxNextRef](const GA_SplittableRange& r)
        {
            GA_Offset vtxoff_next, dstpt, ptnum;
            GA_Offset start, end;
            for (GA_Iterator it(r); it.blockAdvance(start, end); )
            {
                for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                {
                    dstpt = int_oh.get(elemoff);
                    if (dstpt == GFE_INVALID_OFFSET)
                    {
                        if (vertexNextEquivNoLoopAttrib)
                            int_wh.set(elemoff, GFE_INVALID_OFFSET);
                        continue;
                    }

                    for (vtxoff_next = vtxNextRef->getLink(elemoff); vtxoff_next != GFE_INVALID_OFFSET; vtxoff_next = vtxNextRef->getLink(vtxoff_next))
                    {
                        if (int_oh.get(vtxoff_next) != dstpt)
                            continue;
                        dstpt = GFE_INVALID_OFFSET;
                        if (vertexNextEquivNoLoopAttrib)
                            int_wh.set(elemoff, vtxoff_next);
                        break;
                    }

                    if (dstpt == GFE_INVALID_OFFSET)
                        continue;

                    ptnum = vtxPointRef->getLink(elemoff);
                    for (vtxoff_next = pointVtxRef->getLink(dstpt); vtxoff_next != GFE_INVALID_OFFSET; vtxoff_next = vtxNextRef->getLink(vtxoff_next))
                    {
                        if (int_oh.get(vtxoff_next) != ptnum)
                            continue;
                        if (dstpt > ptnum)
                        {
                            dstpt = GFE_INVALID_OFFSET;
                            if (vertexNextEquivNoLoopAttrib)
                                int_wh.set(elemoff, vtxoff_next);
                        }
                        break;
                    }
                    if (dstpt != GFE_INVALID_OFFSET)
                    {
                        if (vertexNextEquivNoLoopGroup)
                            vertexNextEquivNoLoopGroup->setElement(elemoff, true);
                        if (vertexNextEquivNoLoopAttrib)
                            int_wh.set(elemoff, GFE_INVALID_OFFSET);
                    }
                }
            }
        }, subscribeRatio, minGrainSize);
        
        if (vertexNextEquivNoLoopGroup)
            vertexNextEquivNoLoopGroup->invalidateGroupEntries();
    }

    


    

    //Get all prims neighbours prims with adjacent by edge
    void pointPointEdge()
    {
        if(!pointPointEdgeAttrib)
            setPointPointEdge(!outIntermediateAttrib);
        
        intArray_wh.bind(pointPointEdgeAttrib);
        int_oh.bind(vertexPrimIndexAttrib);
        
        const GA_PointGroup* const seamGroup = pointSeamGroup.getPointGroup();
        UTparallelFor(groupParser.getPointSplittableRange(), [seamGroup, this](const GA_SplittableRange& r)
        {
            UT_ValArray<GA_Offset> ptoffArray;
            GA_Offset start, end;
            for (GA_Iterator it(r); it.blockAdvance(start, end); )
            {
                for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                {
                    ptoffArray.clear();

                    GA_Offset pt_next;
                    for (GA_Offset vtxoff_next = geo->pointVertex(elemoff); vtxoff_next != GFE_INVALID_OFFSET; vtxoff_next = geo->vertexToNextVertex(vtxoff_next))
                    {
                        const GA_Offset primoff = geo->vertexPrimitive(vtxoff_next);
                        const GA_Size numvtx = geo->getPrimitiveVertexCount(primoff);
                        const GA_Size vtxpnum = int_oh.get(vtxoff_next);

                        if (vtxpnum == 0)
                        {
                            if (geo->getPrimitiveClosedFlag(primoff))
                            {
                                pt_next = geo->vertexPoint(geo->getPrimitiveVertexOffset(primoff, numvtx - 1));
                                if (ptoffArray.find(pt_next) == GFE_FIND_INVALID_INDEX)
                                    ptoffArray.emplace_back(pt_next);
                            }
                        }
                        else
                        {
                            pt_next = geo->vertexPoint(geo->getPrimitiveVertexOffset(primoff, vtxpnum - 1));
                            if (ptoffArray.find(pt_next) == GFE_FIND_INVALID_INDEX)
                                ptoffArray.emplace_back(pt_next);
                        }

                        const GA_Size vtxpnum_next = vtxpnum + 1;
                        if (vtxpnum_next == numvtx) {
                            if (geo->getPrimitiveClosedFlag(primoff))
                            {
                                pt_next = geo->vertexPoint(geo->getPrimitiveVertexOffset(primoff, 0));
                                if (ptoffArray.find(pt_next) == GFE_FIND_INVALID_INDEX)
                                    ptoffArray.emplace_back(pt_next);
                            }
                        }
                        else
                        {
                            pt_next = geo->vertexPoint(geo->getPrimitiveVertexOffset(primoff, vtxpnum_next));
                            if (ptoffArray.find(pt_next) == GFE_FIND_INVALID_INDEX)
                                ptoffArray.emplace_back(pt_next);
                        }
                    }
                    intArray_wh.set(elemoff, ptoffArray);
                }
            }
        }, subscribeRatio, minGrainSize);
    }



    
    //Get Vertex Destination Point
    void pointPointEdge1()
    {
        if(!pointPointEdgeAttrib)
            setPointPointEdge(!outIntermediateAttrib);
        
        intArray_wh.bind(pointPointEdgeAttrib);
        int_oh.bind(vertexVertexPrimPrevAttrib);
        const GA_ROHandleT<GA_Offset> vtxNext_h(vertexVertexPrimNextAttrib);
        
        const GA_PointGroup* const seamGroup = pointSeamGroup.getPointGroup();
        
        const GA_Topology& topo = geo->getTopology();
        //topo.makeFull();
        //topo.makeVertexRef();
        const GA_ATITopology* const vtxPointRef = topo.getPointRef();
        const GA_ATITopology* const pointVtxRef = topo.getVertexRef();
        //const GA_ATITopology* const vtxPrevRef = topo.getVertexPrevRef();
        const GA_ATITopology* const vtxNextRef = topo.getVertexNextRef();

        UTparallelFor(groupParser.getPointSplittableRange(), [this, &vtxNext_h, pointVtxRef, vtxNextRef, vtxPointRef, seamGroup](const GA_SplittableRange& r)
        {
            UT_ValArray<GA_Offset> ptoffArray(32);
            GA_Offset start, end;
            for (GA_Iterator it(r); it.blockAdvance(start, end); )
            {
                for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                {
                    ptoffArray.clear();
                    for (GA_Offset vtxoff_next = pointVtxRef->getLink(elemoff); vtxoff_next != GFE_INVALID_OFFSET; vtxoff_next = vtxNextRef->getLink(vtxoff_next))
                    {
                        const GA_Offset vtxPrev = int_oh.get(vtxoff_next);
                        if (vtxPrev != GFE_INVALID_OFFSET)
                        {
                            const GA_Offset pt_next = vtxPointRef->getLink(vtxPrev);
                            //ptoffArray.uniqueSortedInsert(pt_next);
                            if (ptoffArray.uniqueSortedFindAscending(pt_next) == GFE_FIND_INVALID_INDEX)
                            {
                                ptoffArray.emplace_back(pt_next);
                                ptoffArray.sort();
                            }
                        }
                        const GA_Offset vtxNext = vtxNext_h.get(vtxoff_next);
                        if (vtxNext != GFE_INVALID_OFFSET)
                        {
                            const GA_Offset pt_next = vtxPointRef->getLink(vtxNext);
                            //ptoffArray.uniqueSortedInsert(pt_next);
                            if (ptoffArray.uniqueSortedFindAscending(pt_next) == GFE_FIND_INVALID_INDEX)
                            {
                                ptoffArray.emplace_back(pt_next);
                                ptoffArray.sort();
                            }
                        }
                    }
                    intArray_wh.set(elemoff, ptoffArray);
                }
            }
        }, subscribeRatio, minGrainSize);
    }


    //Get Vertex Destination Point
    void
        pointPointEdge2(
            const GA_Offset ptoff,
            UT_ValArray<GA_Offset>& ptoffArray
        )
    {
        //exint cap = 0;
        ptoffArray.clear();

        GA_Offset pt_next;
        for (GA_Offset vtxoff_next = geo->pointVertex(ptoff); vtxoff_next != GFE_INVALID_OFFSET; vtxoff_next = geo->vertexToNextVertex(vtxoff_next))
        {
            const GA_Offset primoff = geo->vertexPrimitive(vtxoff_next);
            const GA_Size numvtx = geo->getPrimitiveVertexCount(primoff);
            const GA_Size vtxpnum = geo->vertexPrimIndex(primoff, vtxoff_next);

            if (vtxpnum == 0)
            {
                if (geo->getPrimitiveClosedFlag(primoff))
                {
                    pt_next = geo->vertexPoint(geo->getPrimitiveVertexOffset(primoff, numvtx-1));
                    if (ptoffArray.find(pt_next) == GFE_FIND_INVALID_INDEX)
                        ptoffArray.emplace_back(pt_next);
                }
            }
            else
            {
                pt_next = geo->vertexPoint(geo->getPrimitiveVertexOffset(primoff, vtxpnum-1));
                if (ptoffArray.find(pt_next) == GFE_FIND_INVALID_INDEX)
                    ptoffArray.emplace_back(pt_next);
            }

            const GA_Size vtxpnum_next = vtxpnum + 1;
            if (vtxpnum_next == numvtx) {
                if (geo->getPrimitiveClosedFlag(primoff))
                {
                    pt_next = geo->vertexPoint(geo->getPrimitiveVertexOffset(primoff, 0));
                    if (ptoffArray.find(pt_next) == GFE_FIND_INVALID_INDEX)
                        ptoffArray.emplace_back(pt_next);
                }
            }
            else
            {
                pt_next = geo->vertexPoint(geo->getPrimitiveVertexOffset(primoff, vtxpnum_next));
                if (ptoffArray.find(pt_next) == GFE_FIND_INVALID_INDEX)
                    ptoffArray.emplace_back(pt_next);
            }
        }
    }

    //Get all prims neighbours prims with adjacent by edge
    void
        pointPointEdge2()
    {
        if(!pointPointEdgeAttrib)
            setPointPointEdge(!outIntermediateAttrib);
        
        intArray_wh.bind(pointPointEdgeAttrib);
        
        UTparallelFor(groupParser.getPointSplittableRange(), [this](const GA_SplittableRange& r)
        {
            UT_ValArray<GA_Offset> adjElems;
            GA_Offset start, end;
            for (GA_Iterator it(r); it.blockAdvance(start, end); )
            {
                for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                {
                    pointPointEdge2(elemoff, adjElems);
                    intArray_wh.set(elemoff, adjElems);
                }
            }
        }, subscribeRatio, minGrainSize);
    }


    //Get all prims neighbours prims with adjacent by edge
    void
        primPrimEdge()
    {
        if(!primPrimEdgeAttrib)
            setPrimPrimEdge(!outIntermediateAttrib);
        
        intArray_wh.bind(primPrimEdgeAttrib);
        int_oh.bind(vertexNextEquivAttrib);
        
        const GA_VertexGroup* const seamGroup = vertexEdgeSeamGroup.getVertexGroup();
        const GA_Topology& topo = geo->getTopology();
        //topo.makePrimitiveRef();
        const GA_ATITopology* const vtxPrimRef = topo.getPrimitiveRef();

        //const GA_Detail* const geo = this->geo;
        UTparallelFor(groupParser.getPrimitiveSplittableRange(), [this, seamGroup, vtxPrimRef](const GA_SplittableRange& r)
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
                            GA_Offset vtxoff_next = int_oh.get(vtxoff_start);
                            if (vtxoff_next == GFE_INVALID_OFFSET)
                                continue;
                            for (; vtxoff_next != vtxoff_start; vtxoff_next = int_oh.get(vtxoff_next))
                            {
                                if (adjElems.find(vtxPrimRef->getLink(vtxoff_next)) != GFE_FIND_INVALID_INDEX)
                                    continue;
                                adjElems.emplace_back(vtxPrimRef->getLink(vtxoff_next));
                            }
                        }
                        intArray_wh.set(elemoff, adjElems);
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
                            GA_Offset vtxoff_next = int_oh.get(vtxoff_start);
                            if (vtxoff_next == GFE_INVALID_OFFSET)
                                continue;
                            for (; vtxoff_next != vtxoff_start; vtxoff_next = int_oh.get(vtxoff_next))
                            {
                                if (adjElems.find(vtxPrimRef->getLink(vtxoff_next)) != GFE_FIND_INVALID_INDEX)
                                    continue;
                                adjElems.emplace_back(vtxPrimRef->getLink(vtxoff_next));
                            }
                        }
                        intArray_wh.set(elemoff, adjElems);
                    }
                }
            }
        }, subscribeRatio, minGrainSize);
    }

    


    //Get all prims neighbours prims with adjacent by edge
    void
        primPrimEdge1()
    {
        if(!primPrimEdgeAttrib)
            setPrimPrimEdge(!outIntermediateAttrib);
        
        intArray_wh.bind(primPrimEdgeAttrib);
        int_oh.bind(vertexPointDstAttrib);
        
        //const GA_Detail* const geo = this->geo;
        const GA_VertexGroup* const seamGroup = vertexEdgeSeamGroup.getVertexGroup();
        UTparallelFor(groupParser.getPrimitiveSplittableRange(), [this, seamGroup](const GA_SplittableRange& r)
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
                            const GA_Offset dstpt = int_oh.get(vertices[i]);
                            if (dstpt == GFE_INVALID_OFFSET)
                                continue;
                            const GA_Offset ptoff = geo->vertexPoint(vertices[i]);
                            for (vtxoff_next = geo->pointVertex(ptoff); vtxoff_next != GFE_INVALID_OFFSET; vtxoff_next = geo->vertexToNextVertex(vtxoff_next))
                            {
                                if (vtxoff_next == vertices[i])
                                    continue;
                                if (int_oh.get(vtxoff_next) != dstpt)
                                    continue;
                                if (adjElems.find(geo->vertexPrimitive(vtxoff_next)) != GFE_FIND_INVALID_INDEX)
                                    continue;
                                adjElems.emplace_back(geo->vertexPrimitive(vtxoff_next));
                            }

                            for (vtxoff_next = geo->pointVertex(dstpt); vtxoff_next != GFE_INVALID_OFFSET; vtxoff_next = geo->vertexToNextVertex(vtxoff_next))
                            {
                                if (int_oh.get(vtxoff_next) != ptoff)
                                    continue;
                                if (adjElems.find(geo->vertexPrimitive(vtxoff_next)) != GFE_FIND_INVALID_INDEX)
                                    continue;
                                adjElems.emplace_back(geo->vertexPrimitive(vtxoff_next));
                            }
                        }
                        intArray_wh.set(elemoff, adjElems);
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
                            const GA_Offset dstpt = int_oh.get(vertices[i]);
                            if (dstpt == GFE_INVALID_OFFSET)
                                continue;
                            const GA_Offset ptoff = geo->vertexPoint(vertices[i]);
                            for (vtxoff_next = geo->pointVertex(ptoff); vtxoff_next != GFE_INVALID_OFFSET; vtxoff_next = geo->vertexToNextVertex(vtxoff_next))
                            {
                                if (vtxoff_next == vertices[i])
                                    continue;
                                if (int_oh.get(vtxoff_next) != dstpt)
                                    continue;
                                if (adjElems.find(geo->vertexPrimitive(vtxoff_next)) != GFE_INVALID_OFFSET)
                                    continue;
                                adjElems.emplace_back(geo->vertexPrimitive(vtxoff_next));
                            }

                            for (vtxoff_next = geo->pointVertex(dstpt); vtxoff_next != GFE_INVALID_OFFSET; vtxoff_next = geo->vertexToNextVertex(vtxoff_next))
                            {
                                if (int_oh.get(vtxoff_next) != ptoff)
                                    continue;
                                if (adjElems.find(geo->vertexPrimitive(vtxoff_next)) != GFE_INVALID_OFFSET)
                                    continue;
                                adjElems.emplace_back(geo->vertexPrimitive(vtxoff_next));
                            }
                        }
                        intArray_wh.set(elemoff, adjElems);
                    }
                }
            }
        }, subscribeRatio, minGrainSize);
    }


    //Get all prims neighbours prims with adjacent by edge
    void
        primPrimEdge2()
    {
        if(!primPrimEdgeAttrib)
            setPrimPrimEdge(!outIntermediateAttrib);
        
        intArray_wh.bind(primPrimEdgeAttrib);
        int_oh.bind(vertexPointDstAttrib);

        const GA_VertexGroup* const seamGroup = vertexEdgeSeamGroup.getVertexGroup();
        UTparallelFor(groupParser.getPrimitiveSplittableRange(), [this, seamGroup](const GA_SplittableRange& r)
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
                            const GA_Offset dstpt = int_oh.get(vertices[i]);
                            if (dstpt == GFE_INVALID_OFFSET)
                                continue;
                            const GA_Offset ptoff = geo->vertexPoint(vertices[i]);
                            for (vtxoff_next = geo->pointVertex(ptoff); vtxoff_next != GFE_INVALID_OFFSET; vtxoff_next = geo->vertexToNextVertex(vtxoff_next))
                            {
                                if (vtxoff_next == vertices[i])
                                    continue;
                                if (int_oh.get(vtxoff_next) != dstpt)
                                    continue;
                                if (adjElems.find(geo->vertexPrimitive(vtxoff_next)) != GFE_FIND_INVALID_INDEX)
                                    continue;
                                adjElems.emplace_back(geo->vertexPrimitive(vtxoff_next));
                            }

                            for (vtxoff_next = geo->pointVertex(dstpt); vtxoff_next != GFE_INVALID_OFFSET; vtxoff_next = geo->vertexToNextVertex(vtxoff_next))
                            {
                                if (int_oh.get(vtxoff_next) != ptoff)
                                    continue;
                                if (adjElems.find(geo->vertexPrimitive(vtxoff_next)) != GFE_FIND_INVALID_INDEX)
                                    continue;
                                adjElems.emplace_back(geo->vertexPrimitive(vtxoff_next));
                            }
                        }
                        intArray_wh.set(elemoff, adjElems);
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
                            const GA_Offset dstpt = int_oh.get(vertices[i]);
                            if (dstpt == GFE_INVALID_OFFSET)
                                continue;
                            const GA_Offset ptoff = geo->vertexPoint(vertices[i]);
                            for (vtxoff_next = geo->pointVertex(ptoff); vtxoff_next != GFE_INVALID_OFFSET; vtxoff_next = geo->vertexToNextVertex(vtxoff_next))
                            {
                                if (vtxoff_next == vertices[i])
                                    continue;
                                if (int_oh.get(vtxoff_next) != dstpt)
                                    continue;
                                if (adjElems.find(geo->vertexPrimitive(vtxoff_next)) != GFE_INVALID_OFFSET)
                                    continue;
                                adjElems.emplace_back(geo->vertexPrimitive(vtxoff_next));
                            }

                            for (vtxoff_next = geo->pointVertex(dstpt); vtxoff_next != GFE_INVALID_OFFSET; vtxoff_next = geo->vertexToNextVertex(vtxoff_next))
                            {
                                if (int_oh.get(vtxoff_next) != ptoff)
                                    continue;
                                if (adjElems.find(geo->vertexPrimitive(vtxoff_next)) != GFE_INVALID_OFFSET)
                                    continue;
                                adjElems.emplace_back(geo->vertexPrimitive(vtxoff_next));
                            }
                        }
                        intArray_wh.set(elemoff, adjElems);
                    }
                }
            }
        }, subscribeRatio, minGrainSize);
    }



    //Get all prims neighbours prims with adjacent by edge
    void
        primPrimEdge3()
    {
        if(!primPrimEdgeAttrib)
            setPrimPrimEdge(outTopoAttrib);
        
        intArray_wh.bind(primPrimEdgeAttrib);
        int_oh.bind(vertexPointDstAttrib);
        
        //const GA_Detail* const geo = this->geo;
        const GA_VertexGroup* const seamGroup = vertexEdgeSeamGroup.getVertexGroup();
        UTparallelFor(groupParser.getPrimitiveSplittableRange(), [this, seamGroup](const GA_SplittableRange& r)
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
                            const GA_Offset dstpt = int_oh.get(vertices[i]);
                            if (dstpt == GFE_INVALID_OFFSET)
                                continue;
                            const GA_Offset ptoff = geo->vertexPoint(vertices[i]);
                            for (vtxoff_next = geo->pointVertex(ptoff); vtxoff_next != GFE_INVALID_OFFSET; vtxoff_next = geo->vertexToNextVertex(vtxoff_next))
                            {
                                if (vtxoff_next == vertices[i])
                                    continue;
                                if (int_oh.get(vtxoff_next) != dstpt)
                                    continue;
                                if (adjElems.find(geo->vertexPrimitive(vtxoff_next)) != GFE_FIND_INVALID_INDEX)
                                    continue;
                                adjElems.emplace_back(geo->vertexPrimitive(vtxoff_next));
                            }

                            for (vtxoff_next = geo->pointVertex(dstpt); vtxoff_next != GFE_INVALID_OFFSET; vtxoff_next = geo->vertexToNextVertex(vtxoff_next))
                            {
                                if (int_oh.get(vtxoff_next) != ptoff)
                                    continue;
                                if (adjElems.find(geo->vertexPrimitive(vtxoff_next)) != GFE_FIND_INVALID_INDEX)
                                    continue;
                                adjElems.emplace_back(geo->vertexPrimitive(vtxoff_next));
                            }
                        }
                        intArray_wh.set(elemoff, adjElems);
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
                            const GA_Offset dstpt = int_oh.get(vertices[i]);
                            if (dstpt == GFE_INVALID_OFFSET)
                                continue;
                            const GA_Offset ptoff = geo->vertexPoint(vertices[i]);
                            for (vtxoff_next = geo->pointVertex(ptoff); vtxoff_next != GFE_INVALID_OFFSET; vtxoff_next = geo->vertexToNextVertex(vtxoff_next))
                            {
                                if (vtxoff_next == vertices[i])
                                    continue;
                                if (int_oh.get(vtxoff_next) != dstpt)
                                    continue;
                                if (adjElems.find(geo->vertexPrimitive(vtxoff_next)) != GFE_INVALID_OFFSET)
                                    continue;
                                adjElems.emplace_back(geo->vertexPrimitive(vtxoff_next));
                            }

                            for (vtxoff_next = geo->pointVertex(dstpt); vtxoff_next != GFE_INVALID_OFFSET; vtxoff_next = geo->vertexToNextVertex(vtxoff_next))
                            {
                                if (int_oh.get(vtxoff_next) != ptoff)
                                    continue;
                                if (adjElems.find(geo->vertexPrimitive(vtxoff_next)) != GFE_INVALID_OFFSET)
                                    continue;
                                adjElems.emplace_back(geo->vertexPrimitive(vtxoff_next));
                            }
                        }
                        intArray_wh.set(elemoff, adjElems);
                    }
                }
            }
        }, subscribeRatio, minGrainSize);
    }


    //Get all prims neighbours prims with adjacent by edge
    void
        primPrimEdge4()
    {
        if(!primPrimEdgeAttrib)
            setPrimPrimEdge(!outIntermediateAttrib);
        
        intArray_wh.bind(primPrimEdgeAttrib);
        
        const GEO_Detail* geo_GEO = static_cast<const GEO_Detail*>(static_cast<const GA_Detail*>(geo));
        const GA_VertexGroup* const seamGroup = vertexEdgeSeamGroup.getVertexGroup();
        UTparallelFor(groupParser.getPrimitiveSplittableRange(), [this, geo_GEO, seamGroup](const GA_SplittableRange& r)
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
                        const GA_Size numAdj = geo_GEO->getEdgeAdjacentPolygons(adjElemStruct, elemoff);
                        adjElems.setSizeNoInit(numAdj);
                        for (GA_Size i = 0; i < numAdj; ++i)
                        {
                            adjElems[i] = adjElemStruct[i].myAdjacentPolygon;
                        }
                        intArray_wh.set(elemoff, adjElems);
                    }
                }
                else
                {
                    for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                    {
                        const GA_Size numAdj = geo_GEO->getEdgeAdjacentPolygons(adjElemStruct, elemoff);
                        adjElems.setSizeNoInit(numAdj);
                        for (GA_Size i = 0; i < numAdj; ++i)
                        {
                            adjElems[i] = adjElemStruct[i].myAdjacentPolygon;
                        }
                        intArray_wh.set(elemoff, adjElems);
                    }
                }
            }
        }, subscribeRatio, minGrainSize);
    }

    void
        primPrimPoint()
    {
        if(!primPrimPointAttrib)
            setPrimPrimPoint(!outIntermediateAttrib);
            
        intArray_wh.bind(primPrimPointAttrib);
        
        const GA_PointGroup* const seamGroup = pointSeamGroup.getPointGroup();
        
        const GA_Topology& topo = geo->getTopology();
        //topo.makeFull();
        //topo.makePrimitiveRef();
        //topo.makeVertexRef();
        const GA_ATITopology* const vtxPrimRef = topo.getPrimitiveRef();
        const GA_ATITopology* const vtxPointRef = topo.getPointRef();
        const GA_ATITopology* const pointVtxRef = topo.getVertexRef();
        const GA_ATITopology* const vtxNextRef = topo.getVertexNextRef();
        
        //const GA_Detail* const geo = this->geo;
        UTparallelFor(groupParser.getPrimitiveSplittableRange(), [this, seamGroup, vtxNextRef, vtxPointRef, pointVtxRef, vtxPrimRef](const GA_SplittableRange& r)
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
                            for (vtxoff_next = pointVtxRef->getLink(ptoff); vtxoff_next != GFE_INVALID_OFFSET; vtxoff_next = vtxNextRef->getLink(vtxoff_next))
                            {
                                if (vtxoff_next == vertices[i])
                                    continue;
                                if (adjElems.find(vtxPrimRef->getLink(vtxoff_next)) != GFE_FIND_INVALID_INDEX)
                                    continue;
                                adjElems.emplace_back(vtxPrimRef->getLink(vtxoff_next));
                            }
                        }
                        intArray_wh.set(elemoff, adjElems);
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
                            for (vtxoff_next = pointVtxRef->getLink(ptoff); vtxoff_next != GFE_INVALID_OFFSET; vtxoff_next = vtxNextRef->getLink(vtxoff_next))
                            {
                                if (vtxoff_next == vertices[i])
                                    continue;
                                if (adjElems.find(vtxPrimRef->getLink(vtxoff_next)) != GFE_FIND_INVALID_INDEX)
                                    continue;
                                adjElems.emplace_back(vtxPrimRef->getLink(vtxoff_next));
                            }
                        }
                        intArray_wh.set(elemoff, adjElems);
                    }
                }
            }
        }, subscribeRatio, minGrainSize);
    }




    
public:
    bool outAsOffset = true;
    bool outIntermediateAttrib = true;

private:
    GA_Attribute*   vertexPrimIndexAttrib       = nullptr;
    GA_Attribute*   vertexVertexPrimPrevAttrib  = nullptr;
    GA_Attribute*   vertexVertexPrimNextAttrib  = nullptr;
    GA_Attribute*   vertexPointDstAttrib        = nullptr;
    GA_Attribute*   vertexNextEquivAttrib       = nullptr;
    GA_Attribute*   vertexNextEquivNoLoopAttrib = nullptr;
    GA_Attribute*   pointPointEdgeAttrib        = nullptr;
    GA_Attribute*   pointPointPrimAttrib        = nullptr;
    GA_Attribute*   primPrimEdgeAttrib          = nullptr;
    GA_Attribute*   primPrimPointAttrib         = nullptr;
    GA_VertexGroup* vertexNextEquivGroup        = nullptr; // unshared vertex
    GA_VertexGroup* vertexNextEquivNoLoopGroup  = nullptr;
    GA_EdgeGroup*   unsharedEdgeGroup           = nullptr;
    
        
    GA_RWHandleT<GA_Size> int_wh;
    GA_RWHandleT<GA_Size> int1_wh;
        
    GA_ROHandleT<GA_Size> int_oh;
    GA_RWHandleT<UT_ValArray<GA_Offset>> intArray_wh;

        
    GFE_GroupParser pointSeamGroup;
    GFE_GroupParser vertexEdgeSeamGroup;
    GFE_GroupParser edgeSeamGroup;

    int kernel = 0;
    exint subscribeRatio = 64;
    exint minGrainSize = 64;

}; // End of class GFE_MeshTopology




















































































































/*

namespace GFE_MeshTopology_Namespace {



    //Get Vertex Destination Point
    static void
        pointPointEdge(
            const GA_Detail& geo,
            const GA_Offset ptoff,
            UT_ValArray<GA_Offset>& ptoffArray
        )
    {
        //exint cap = 0;
        ptoffArray.clear();

        GA_Offset pt_next;
        for (GA_Offset vtxoff_next = geo.pointVertex(ptoff); vtxoff_next != GFE_INVALID_OFFSET; vtxoff_next = geo.vertexToNextVertex(vtxoff_next))
        {
            const GA_Offset primoff = geo.vertexPrimitive(vtxoff_next);
            const GA_Size numvtx = geo.getPrimitiveVertexCount(primoff);
            const GA_Size vtxpnum = GFE_TopologyReference_Namespace::vertexPrimIndex(geo, primoff, vtxoff_next);

            if (vtxpnum == 0)
            {
                if (geo.getPrimitiveClosedFlag(primoff))
                {
                    pt_next = geo.vertexPoint(geo.getPrimitiveVertexOffset(primoff, numvtx-1));
                    if (ptoffArray.find(pt_next) == -1)
                        ptoffArray.emplace_back(pt_next);
                }
            }
            else
            {
                pt_next = geo.vertexPoint(geo.getPrimitiveVertexOffset(primoff, vtxpnum-1));
                if (ptoffArray.find(pt_next) == -1)
                    ptoffArray.emplace_back(pt_next);
            }

            const GA_Size vtxpnum_next = vtxpnum + 1;
            if (vtxpnum_next == numvtx) {
                if (geo.getPrimitiveClosedFlag(primoff))
                {
                    pt_next = geo.vertexPoint(geo.getPrimitiveVertexOffset(primoff, 0));
                    if (ptoffArray.find(pt_next) == -1)
                        ptoffArray.emplace_back(pt_next);
                }
            }
            else
            {
                pt_next = geo.vertexPoint(geo.getPrimitiveVertexOffset(primoff, vtxpnum_next));
                if (ptoffArray.find(pt_next) == -1)
                    ptoffArray.emplace_back(pt_next);
            }
        }
    }




    //Get all prims neighbours prims with adjacent by edge
    static void
        pointPointEdge(
            const GA_Detail& geo,
            const GA_RWHandleT<UT_ValArray<GA_Offset>>& attrib_h,
            const GA_ROHandleT<GA_Size>& vtxpnumAttrib_h,
            const GA_PointGroup* const geoGroup = nullptr,
            const GA_PointGroup* const seamGroup = nullptr,
            const exint subscribeRatio = 64,
            const exint minGrainSize = 64
        )
    {
        const GA_SplittableRange geo0SplittableRange0(geo.getPointRange(geoGroup));
        UTparallelFor(geo0SplittableRange0, [&geo, &attrib_h, &vtxpnumAttrib_h, seamGroup](const GA_SplittableRange& r)
        {
            UT_ValArray<GA_Offset> ptoffArray;
            GA_Offset start, end;
            for (GA_Iterator it(r); it.blockAdvance(start, end); )
            {
                for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                {
                    ptoffArray.clear();

                    GA_Offset pt_next;
                    for (GA_Offset vtxoff_next = geo.pointVertex(elemoff); vtxoff_next != GFE_INVALID_OFFSET; vtxoff_next = geo.vertexToNextVertex(vtxoff_next))
                    {
                        const GA_Offset primoff = geo.vertexPrimitive(vtxoff_next);
                        const GA_Size numvtx = geo.getPrimitiveVertexCount(primoff);
                        const GA_Size vtxpnum = vtxpnumAttrib_h.get(vtxoff_next);

                        if (vtxpnum == 0)
                        {
                            if (geo.getPrimitiveClosedFlag(primoff))
                            {
                                pt_next = geo.vertexPoint(geo.getPrimitiveVertexOffset(primoff, numvtx - 1));
                                if (ptoffArray.find(pt_next) == -1)
                                    ptoffArray.emplace_back(pt_next);
                            }
                        }
                        else
                        {
                            pt_next = geo.vertexPoint(geo.getPrimitiveVertexOffset(primoff, vtxpnum - 1));
                            if (ptoffArray.find(pt_next) == -1)
                                ptoffArray.emplace_back(pt_next);
                        }

                        const GA_Size vtxpnum_next = vtxpnum + 1;
                        if (vtxpnum_next == numvtx) {
                            if (geo.getPrimitiveClosedFlag(primoff))
                            {
                                pt_next = geo.vertexPoint(geo.getPrimitiveVertexOffset(primoff, 0));
                                if (ptoffArray.find(pt_next) == -1)
                                    ptoffArray.emplace_back(pt_next);
                            }
                        }
                        else
                        {
                            pt_next = geo.vertexPoint(geo.getPrimitiveVertexOffset(primoff, vtxpnum_next));
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
        const GA_Detail& geo,
        const GA_RWHandleT<UT_ValArray<GA_Offset>>& attrib_h,
        const GA_PointGroup* const geoGroup = nullptr,
        const GA_PointGroup* const seamGroup = nullptr,
        const exint subscribeRatio = 128,
        const exint minGrainSize = 64
    )
    {

        const GA_SplittableRange geo0SplittableRange0(geo.getPointRange(geoGroup));
        UTparallelFor(geo0SplittableRange0, [&geo, &attrib_h, seamGroup](const GA_SplittableRange& r)
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
        GA_Detail& geo,
        const GA_RWHandleT<UT_ValArray<GA_Offset>>& attrib_h,
        const GA_ROHandleT<GA_Offset>& vtxPrev_h,
        const GA_ROHandleT<GA_Offset>& vtxNext_h,
        const GA_PointGroup* const geoGroup = nullptr,
        const GA_PointGroup* const seamGroup = nullptr,
        const exint subscribeRatio = 128,
        const exint minGrainSize = 64
    )
    {
        GA_Topology& topo = geo.getTopology();
        topo.makeFull();
        //topo.makeVertexRef();
        const GA_ATITopology* const vtxPointRef = topo.getPointRef();
        const GA_ATITopology* const pointVtxRef = topo.getVertexRef();
        //const GA_ATITopology* const vtxPrevRef = topo.getVertexPrevRef();
        const GA_ATITopology* const vtxNextRef = topo.getVertexNextRef();
        const GA_SplittableRange geo0SplittableRange0(geo.getPointRange(geoGroup));

        UTparallelFor(geo0SplittableRange0, [&geo, &attrib_h, &vtxPrev_h, &vtxNext_h, pointVtxRef, vtxNextRef, vtxPointRef, seamGroup](const GA_SplittableRange& r)
        {
            UT_ValArray<GA_Offset> ptoffArray(32);
            GA_Offset start, end;
            for (GA_Iterator it(r); it.blockAdvance(start, end); )
            {
                for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                {
                    ptoffArray.clear();
                    for (GA_Offset vtxoff_next = pointVtxRef->getLink(elemoff); vtxoff_next != GFE_INVALID_OFFSET; vtxoff_next = vtxNextRef->getLink(vtxoff_next))
                    {
                        const GA_Offset vtxPrev = vtxPrev_h.get(vtxoff_next);
                        if (vtxPrev != GFE_INVALID_OFFSET)
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
                        if (vtxNext != GFE_INVALID_OFFSET)
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
            GA_Detail& geo,
            const GA_RWHandleT<UT_ValArray<GA_Offset>>& attrib_h,
            const GA_ROHandleT<GA_Offset>& nextEquivAttrib_h,
            const GA_PrimitiveGroup* const geoGroup = nullptr,
            const GA_VertexGroup* const seamGroup = nullptr,
            const exint subscribeRatio = 128,
            const exint minGrainSize = 64
        )
    {
        GA_Topology& topo = geo.getTopology();
        topo.makePrimitiveRef();
        const GA_ATITopology* const vtxPrimRef = topo.getPrimitiveRef();

        const GA_SplittableRange geo0SplittableRange0(geo.getPrimitiveRange(geoGroup));
        UTparallelFor(geo0SplittableRange0, [&geo, &attrib_h, &nextEquivAttrib_h, seamGroup, vtxPrimRef](const GA_SplittableRange& r)
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
                        const GA_OffsetListRef& vertices = geo.getPrimitiveVertexList(elemoff);
                        const GA_Size numvtx = vertices.size();
                        //for (GA_Offset vtxoff : vertices)
                        for (GA_Size i = 0; i < numvtx; ++i)
                        {
                            const GA_Offset vtxoff_start = vertices[i];
                            if (!seamGroup->contains(vtxoff_start))
                                continue;
                            GA_Offset vtxoff_next = nextEquivAttrib_h.get(vtxoff_start);
                            if (vtxoff_next == GFE_INVALID_OFFSET)
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
                        const GA_OffsetListRef& vertices = geo.getPrimitiveVertexList(elemoff);
                        const GA_Size numvtx = vertices.size();
                        //for (GA_Offset vtxoff : vertices)
                        for (GA_Size i = 0; i < numvtx; ++i)
                        {
                            const GA_Offset vtxoff_start = vertices[i];
                            GA_Offset vtxoff_next = nextEquivAttrib_h.get(vtxoff_start);
                            if (vtxoff_next == GFE_INVALID_OFFSET)
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
            const GA_Detail& geo,
            const GA_RWHandleT<UT_ValArray<GA_Offset>>& attrib_h,
            const GA_ROHandleT<GA_Offset>& dstptAttrib_h,
            const GA_PrimitiveGroup* const geoGroup = nullptr,
            const GA_VertexGroup*    const seamGroup = nullptr,
            const exint subscribeRatio = 128,
            const exint minGrainSize = 64
        )
    {
        const GA_SplittableRange geoSplittableRange(geo.getPrimitiveRange(geoGroup));
        UTparallelFor(geoSplittableRange, [&geo, &attrib_h, &dstptAttrib_h, seamGroup](const GA_SplittableRange& r)
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
                        const GA_OffsetListRef& vertices = geo.getPrimitiveVertexList(elemoff);
                        const GA_Size numvtx = vertices.size();
                        for (GA_Size i = 0; i < numvtx; ++i)
                        {
                            const GA_Offset dstpt = dstptAttrib_h.get(vertices[i]);
                            if (dstpt == GFE_INVALID_OFFSET)
                                continue;
                            const GA_Offset ptoff = geo.vertexPoint(vertices[i]);
                            for (vtxoff_next = geo.pointVertex(ptoff); vtxoff_next != GFE_INVALID_OFFSET; vtxoff_next = geo.vertexToNextVertex(vtxoff_next))
                            {
                                if (vtxoff_next == vertices[i])
                                    continue;
                                if (dstptAttrib_h.get(vtxoff_next) != dstpt)
                                    continue;
                                if (adjElems.find(geo.vertexPrimitive(vtxoff_next)) != -1)
                                    continue;
                                adjElems.emplace_back(geo.vertexPrimitive(vtxoff_next));
                            }

                            for (vtxoff_next = geo.pointVertex(dstpt); vtxoff_next != GFE_INVALID_OFFSET; vtxoff_next = geo.vertexToNextVertex(vtxoff_next))
                            {
                                if (dstptAttrib_h.get(vtxoff_next) != ptoff)
                                    continue;
                                if (adjElems.find(geo.vertexPrimitive(vtxoff_next)) != -1)
                                    continue;
                                adjElems.emplace_back(geo.vertexPrimitive(vtxoff_next));
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
                        const GA_OffsetListRef& vertices = geo.getPrimitiveVertexList(elemoff);
                        const GA_Size numvtx = vertices.size();
                        for (GA_Size i = 0; i < numvtx; ++i)
                        {
                            const GA_Offset dstpt = dstptAttrib_h.get(vertices[i]);
                            if (dstpt == GFE_INVALID_OFFSET)
                                continue;
                            const GA_Offset ptoff = geo.vertexPoint(vertices[i]);
                            for (vtxoff_next = geo.pointVertex(ptoff); vtxoff_next != GFE_INVALID_OFFSET; vtxoff_next = geo.vertexToNextVertex(vtxoff_next))
                            {
                                if (vtxoff_next == vertices[i])
                                    continue;
                                if (dstptAttrib_h.get(vtxoff_next) != dstpt)
                                    continue;
                                if (adjElems.find(geo.vertexPrimitive(vtxoff_next)) != GFE_INVALID_OFFSET)
                                    continue;
                                adjElems.emplace_back(geo.vertexPrimitive(vtxoff_next));
                            }

                            for (vtxoff_next = geo.pointVertex(dstpt); vtxoff_next != GFE_INVALID_OFFSET; vtxoff_next = geo.vertexToNextVertex(vtxoff_next))
                            {
                                if (dstptAttrib_h.get(vtxoff_next) != ptoff)
                                    continue;
                                if (adjElems.find(geo.vertexPrimitive(vtxoff_next)) != GFE_INVALID_OFFSET)
                                    continue;
                                adjElems.emplace_back(geo.vertexPrimitive(vtxoff_next));
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
            const GA_Detail& geo,
            const GA_RWHandleT<UT_ValArray<GA_Offset>>& attrib_h,
            const GA_ROHandleT<GA_Offset>& dstptAttrib_h,
            const GA_PrimitiveGroup* const geoGroup = nullptr,
            const GA_VertexGroup*    const seamGroup = nullptr,
            const exint subscribeRatio = 128,
            const exint minGrainSize = 64
        )
    {
        const GA_SplittableRange geo0SplittableRange0(geo.getPrimitiveRange(geoGroup));
        UTparallelFor(geo0SplittableRange0, [&geo, &attrib_h, &dstptAttrib_h, seamGroup](const GA_SplittableRange& r)
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
                        const GA_OffsetListRef& vertices = geo.getPrimitiveVertexList(elemoff);
                        const GA_Size numvtx = vertices.size();
                        for (GA_Size i = 0; i < numvtx; ++i)
                        {
                            const GA_Offset dstpt = dstptAttrib_h.get(vertices[i]);
                            if (dstpt == GFE_INVALID_OFFSET)
                                continue;
                            const GA_Offset ptoff = geo.vertexPoint(vertices[i]);
                            for (vtxoff_next = geo.pointVertex(ptoff); vtxoff_next != GFE_INVALID_OFFSET; vtxoff_next = geo.vertexToNextVertex(vtxoff_next))
                            {
                                if (vtxoff_next == vertices[i])
                                    continue;
                                if (dstptAttrib_h.get(vtxoff_next) != dstpt)
                                    continue;
                                if (adjElems.find(geo.vertexPrimitive(vtxoff_next)) != -1)
                                    continue;
                                adjElems.emplace_back(geo.vertexPrimitive(vtxoff_next));
                            }

                            for (vtxoff_next = geo.pointVertex(dstpt); vtxoff_next != GFE_INVALID_OFFSET; vtxoff_next = geo.vertexToNextVertex(vtxoff_next))
                            {
                                if (dstptAttrib_h.get(vtxoff_next) != ptoff)
                                    continue;
                                if (adjElems.find(geo.vertexPrimitive(vtxoff_next)) != -1)
                                    continue;
                                adjElems.emplace_back(geo.vertexPrimitive(vtxoff_next));
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
                        const GA_OffsetListRef& vertices = geo.getPrimitiveVertexList(elemoff);
                        const GA_Size numvtx = vertices.size();
                        for (GA_Size i = 0; i < numvtx; ++i)
                        {
                            const GA_Offset dstpt = dstptAttrib_h.get(vertices[i]);
                            if (dstpt == GFE_INVALID_OFFSET)
                                continue;
                            const GA_Offset ptoff = geo.vertexPoint(vertices[i]);
                            for (vtxoff_next = geo.pointVertex(ptoff); vtxoff_next != GFE_INVALID_OFFSET; vtxoff_next = geo.vertexToNextVertex(vtxoff_next))
                            {
                                if (vtxoff_next == vertices[i])
                                    continue;
                                if (dstptAttrib_h.get(vtxoff_next) != dstpt)
                                    continue;
                                if (adjElems.find(geo.vertexPrimitive(vtxoff_next)) != GFE_INVALID_OFFSET)
                                    continue;
                                adjElems.emplace_back(geo.vertexPrimitive(vtxoff_next));
                            }

                            for (vtxoff_next = geo.pointVertex(dstpt); vtxoff_next != GFE_INVALID_OFFSET; vtxoff_next = geo.vertexToNextVertex(vtxoff_next))
                            {
                                if (dstptAttrib_h.get(vtxoff_next) != ptoff)
                                    continue;
                                if (adjElems.find(geo.vertexPrimitive(vtxoff_next)) != GFE_INVALID_OFFSET)
                                    continue;
                                adjElems.emplace_back(geo.vertexPrimitive(vtxoff_next));
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
            const GA_Detail& geo,
            const GA_RWHandleT<UT_ValArray<GA_Offset>>& attrib_h,
            const GA_ROHandleT<GA_Offset>& dstptAttrib_h,
            const GA_PrimitiveGroup* const geoGroup = nullptr,
            const GA_VertexGroup*    const seamGroup = nullptr,
            const exint subscribeRatio = 128,
            const exint minGrainSize = 64
        )
    {
        const GA_SplittableRange geo0SplittableRange0(geo.getPrimitiveRange(geoGroup));
        UTparallelFor(geo0SplittableRange0, [&geo, &attrib_h, &dstptAttrib_h, seamGroup](const GA_SplittableRange& r)
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
                        const GA_OffsetListRef& vertices = geo.getPrimitiveVertexList(elemoff);
                        const GA_Size numvtx = vertices.size();
                        for (GA_Size i = 0; i < numvtx; ++i)
                        {
                            const GA_Offset dstpt = dstptAttrib_h.get(vertices[i]);
                            if (dstpt == GFE_INVALID_OFFSET)
                                continue;
                            const GA_Offset ptoff = geo.vertexPoint(vertices[i]);
                            for (vtxoff_next = geo.pointVertex(ptoff); vtxoff_next != GFE_INVALID_OFFSET; vtxoff_next = geo.vertexToNextVertex(vtxoff_next))
                            {
                                if (vtxoff_next == vertices[i])
                                    continue;
                                if (dstptAttrib_h.get(vtxoff_next) != dstpt)
                                    continue;
                                if (adjElems.find(geo.vertexPrimitive(vtxoff_next)) != -1)
                                    continue;
                                adjElems.emplace_back(geo.vertexPrimitive(vtxoff_next));
                            }

                            for (vtxoff_next = geo.pointVertex(dstpt); vtxoff_next != GFE_INVALID_OFFSET; vtxoff_next = geo.vertexToNextVertex(vtxoff_next))
                            {
                                if (dstptAttrib_h.get(vtxoff_next) != ptoff)
                                    continue;
                                if (adjElems.find(geo.vertexPrimitive(vtxoff_next)) != -1)
                                    continue;
                                adjElems.emplace_back(geo.vertexPrimitive(vtxoff_next));
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
                        const GA_OffsetListRef& vertices = geo.getPrimitiveVertexList(elemoff);
                        const GA_Size numvtx = vertices.size();
                        for (GA_Size i = 0; i < numvtx; ++i)
                        {
                            const GA_Offset dstpt = dstptAttrib_h.get(vertices[i]);
                            if (dstpt == GFE_INVALID_OFFSET)
                                continue;
                            const GA_Offset ptoff = geo.vertexPoint(vertices[i]);
                            for (vtxoff_next = geo.pointVertex(ptoff); vtxoff_next != GFE_INVALID_OFFSET; vtxoff_next = geo.vertexToNextVertex(vtxoff_next))
                            {
                                if (vtxoff_next == vertices[i])
                                    continue;
                                if (dstptAttrib_h.get(vtxoff_next) != dstpt)
                                    continue;
                                if (adjElems.find(geo.vertexPrimitive(vtxoff_next)) != GFE_INVALID_OFFSET)
                                    continue;
                                adjElems.emplace_back(geo.vertexPrimitive(vtxoff_next));
                            }

                            for (vtxoff_next = geo.pointVertex(dstpt); vtxoff_next != GFE_INVALID_OFFSET; vtxoff_next = geo.vertexToNextVertex(vtxoff_next))
                            {
                                if (dstptAttrib_h.get(vtxoff_next) != ptoff)
                                    continue;
                                if (adjElems.find(geo.vertexPrimitive(vtxoff_next)) != GFE_INVALID_OFFSET)
                                    continue;
                                adjElems.emplace_back(geo.vertexPrimitive(vtxoff_next));
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
        const GA_Detail& geo,
        const GA_RWHandleT<UT_ValArray<GA_Offset>>& attrib_h,
        const GA_PrimitiveGroup* const geoGroup = nullptr,
        const GA_VertexGroup*    const seamGroup = nullptr,
        const exint subscribeRatio = 128,
        const exint minGrainSize = 64
    )
    {
        const GA_SplittableRange geo0SplittableRange0(geo.getPrimitiveRange(geoGroup));
        UTparallelFor(geo0SplittableRange0, [&geo, &attrib_h, seamGroup](const GA_SplittableRange& r)
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
                        GA_Size numAdj = static_cast<const GEO_Detail&>(geo).getEdgeAdjacentPolygons(adjElemStruct, elemoff);
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
                        GA_Size numAdj = static_cast<const GEO_Detail&>(geo).getEdgeAdjacentPolygons(adjElemStruct, elemoff);
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
        GA_Detail& geo,
        const GA_RWHandleT<UT_ValArray<GA_Offset>>& attrib_h,
        const GA_PrimitiveGroup* const geoGroup  = nullptr,
        const GA_PointGroup*     const seamGroup = nullptr,
        const exint subscribeRatio = 128,
        const exint minGrainSize = 64
    )
    {
        GA_Topology& topo = geo.getTopology();
        //topo.makeFull();
        //topo.makePrimitiveRef();
        //topo.makeVertexRef();
        const GA_ATITopology* const vtxPrimRef = topo.getPrimitiveRef();
        const GA_ATITopology* const vtxPointRef = topo.getPointRef();
        const GA_ATITopology* const pointVtxRef = topo.getVertexRef();
        const GA_ATITopology* const vtxNextRef = topo.getVertexNextRef();


        const GA_SplittableRange geo0SplittableRange0(geo.getPrimitiveRange(geoGroup));
        UTparallelFor(geo0SplittableRange0, [&geo, &attrib_h, seamGroup, vtxNextRef, vtxPointRef, pointVtxRef, vtxPrimRef](const GA_SplittableRange& r)
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
                        const GA_OffsetListRef& vertices = geo.getPrimitiveVertexList(elemoff);
                        const GA_Size numvtx = vertices.size();
                        for (GA_Size i = 0; i < numvtx; ++i)
                        {
                            const GA_Offset ptoff = vtxPointRef->getLink(vertices[i]);
                            if (seamGroup->contains(ptoff))
                                continue;
                            for (vtxoff_next = pointVtxRef->getLink(ptoff); vtxoff_next != GFE_INVALID_OFFSET; vtxoff_next = vtxNextRef->getLink(vtxoff_next))
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
                        const GA_OffsetListRef& vertices = geo.getPrimitiveVertexList(elemoff);
                        const GA_Size numvtx = vertices.size();
                        for (GA_Size i = 0; i < numvtx; ++i)
                        {
                            const GA_Offset ptoff = vtxPointRef->getLink(vertices[i]);
                            for (vtxoff_next = pointVtxRef->getLink(ptoff); vtxoff_next != GFE_INVALID_OFFSET; vtxoff_next = vtxNextRef->getLink(vtxoff_next))
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

















    //GFE_MeshTopology_Namespace::addAttribPointPointEdge(geo, name, geoGroup, seamGroup, storage, nullptr, nullptr, GA_ReuseStrategy(), subscribeRatio, minGrainSize);
    static GA_Attribute*
        addAttribPointPointEdge(
            GA_Detail& geo,
            const GA_PointGroup* const geoGroup = nullptr,
            const GA_PointGroup* const seamGroup = nullptr,
            const GA_Storage storage = GA_STORE_INVALID,
            const UT_StringRef& name = "__topo_nebs",
            const UT_Options* const creation_args = nullptr,
            const GA_AttributeOptions* const attribute_options = nullptr,
            const GA_ReuseStrategy& reuse = GA_ReuseStrategy(),
            const exint subscribeRatio = 128,
            const exint minGrainSize = 64
        )
    {
        GA_Attribute* attribPtr = geo.findPointAttribute(GFE_TOPO_SCOPE, name);
        if (attribPtr)
            return attribPtr;
        GA_Attribute* vtxPrevAttrib = nullptr;
        GA_Attribute* vtxNextAttrib = nullptr;

        const GA_Storage finalStorage = storage == GA_STORE_INVALID ? GFE_Type::getPreferredStorageI(geo) : storage;

        GFE_TopologyReference_Namespace::addAttribVertexVertexPrim(geo, vtxPrevAttrib, vtxNextAttrib, nullptr, finalStorage);

        attribPtr = geo.getAttributes().createArrayAttribute(GA_ATTRIB_POINT, GFE_TOPO_SCOPE, name, finalStorage, 1,
            creation_args, attribute_options, "arraydata", reuse);
        //attribPtr = geo.addIntTuple(GA_ATTRIB_POINT, name, 1, defaults,
        //          creation_args, attribute_options, finalStorage, reuse);
        //attribPtr = geo.addIntArray(GA_ATTRIB_POINT, GFE_TOPO_SCOPE, name, 1,
        //          creation_args, attribute_options, finalStorage, reuse);

        pointPointEdge(geo, attribPtr, vtxPrevAttrib, vtxNextAttrib, geoGroup, seamGroup, subscribeRatio, minGrainSize);
        //UT_ASSERT_P(vtxPrevAttrib);
        //UT_ASSERT_P(vtxNextAttrib);
        //GA_ROHandleT<GA_Offset> vtxPrev_h(vtxPrevAttrib);
        //GA_ROHandleT<GA_Offset> vtxNext_h(vtxNextAttrib);
        //pointPointEdge(geo, attribPtr, vtxPrev_h, vtxNext_h, geoGroup, seamGroup, subscribeRatio, minGrainSize);
        return attribPtr;
    }





    static GA_Attribute*
        addAttribPrimPrimEdge(
            GA_Detail& geo,
            const GA_PrimitiveGroup* const geoGroup = nullptr,
            const GA_VertexGroup* const seamGroup = nullptr,
            const GA_Storage storage = GA_STORE_INVALID,
            const UT_StringRef& name = "__topo_nebs",
            const UT_Options* creation_args = nullptr,
            const GA_AttributeOptions* const attribute_options = nullptr,
            const GA_ReuseStrategy& reuse = GA_ReuseStrategy(),
            const exint subscribeRatio = 128,
            const exint minGrainSize = 64
        )
    {
        GA_Attribute* attribPtr = geo.findPrimitiveAttribute(GFE_TOPO_SCOPE, name);
        if (attribPtr)
            return attribPtr;

        const GA_Storage finalStorage = storage == GA_STORE_INVALID ? GFE_Type::getPreferredStorageI(geo) : storage;
        //GA_Attribute* dstptAttrib = GFE_TopologyReference_Namespace::addAttribVertexPointDst(
        //      geo, "dstpt", nullptr, GA_Defaults(-1), finalStorage, subscribeRatio, minGrainSize);
        //GA_RWHandleT<GA_Offset> dstptAttrib_h(dstptAttrib);
        //GA_Attribute* vtxPrimNextAttrib = addAttribVertexVertexPrimNext(geo, "vtxPrimNext", nullptr, GA_Defaults(-1), finalStorage, subscribeRatio, minGrainSize);

        GA_Attribute* vtxNextEquivAttrib = GFE_VertexNextEquiv_Namespace::addAttribVertexNextEquiv(geo, nullptr, finalStorage);

        attribPtr = geo.getAttributes().createArrayAttribute(GA_ATTRIB_PRIMITIVE, GFE_TOPO_SCOPE, name, finalStorage, 1, creation_args, attribute_options, "arraydata", reuse);
        //attribPtr = geo.addIntArray(GA_ATTRIB_PRIMITIVE, GFE_TOPO_SCOPE, name, 1,
        //          creation_args, attribute_options, finalStorage, reuse);
        //GA_ROHandleT<UT_ValArray<GA_Offset>> attrib_h(attribPtr);
        primPrimEdge(geo, attribPtr, vtxNextEquivAttrib, geoGroup, seamGroup, subscribeRatio, minGrainSize);
        return attribPtr;
    }




    //GFE_MeshTopology_Namespace::addAttribAdjacency(geo, groupType, connectivityGroupType);

    static GA_Attribute*
        addAttribAdjacency(
            GA_Detail& geo,
            const GA_GroupType groupType,
            const GA_GroupType connectivityGroupType,
            const GA_Group* const geoGroup = nullptr,
            const GA_Group* const seamGroup = nullptr,
            const GA_Storage storage = GA_STORE_INVALID,
            const UT_StringRef& name = "__topo_nebs",
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
                //return GFE_MeshTopology_Namespace::addAttribPrimPrimPrim(geo,
                //    UTverify_cast<const GA_PrimitiveGroup*>(geoGroup),
                //    UTverify_cast<const GA_PrimitiveGroup*>(seamGroup),
                //    storage, name, creation_args, attribute_options, reuse, subscribeRatio, minGrainSize);
                break;
            case GA_GROUP_POINT:
                //return GFE_MeshTopology_Namespace::addAttribPrimPrimPoint(geo,
                //    UTverify_cast<const GA_PrimitiveGroup*>(geoGroup),
                //    UTverify_cast<const GA_PointGroup*>(seamGroup),
                //    storage, name, creation_args, attribute_options, reuse, subscribeRatio, minGrainSize);
                break;
            case GA_GROUP_VERTEX:
                //return GFE_MeshTopology_Namespace::addAttribPrimPrimVertex(geo,
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
                //return GFE_MeshTopology_Namespace::addAttribPointPointPrim(geo,
                //    UTverify_cast<const GA_PointGroup*>(geoGroup),
                //    UTverify_cast<const GA_PrimitiveGroup*>(seamGroup),
                //    storage, name, creation_args, attribute_options, reuse, subscribeRatio, minGrainSize);
                break;
            case GA_GROUP_POINT:
                //return GFE_MeshTopology_Namespace::addAttribPointPointPoint(geo,
                //    UTverify_cast<const GA_PointGroup*>(geoGroup),
                //    UTverify_cast<const GA_PointGroup*>(seamGroup),
                //    storage, name, creation_args, attribute_options, reuse, subscribeRatio, minGrainSize);
                break;
            case GA_GROUP_VERTEX:
                //return GFE_MeshTopology_Namespace::addAttribPointPointVertex(geo,
                //    UTverify_cast<const GA_PointGroup*>(geoGroup),
                //    UTverify_cast<const GA_VertexGroup*>(seamGroup),
                //    storage, name, creation_args, attribute_options, reuse, subscribeRatio, minGrainSize);
                break;
            case GA_GROUP_EDGE:
                return GFE_MeshTopology_Namespace::addAttribPointPointEdge(geo,
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
                //return GFE_MeshTopology_Namespace::addAttribVertexVertexPrim(geo,
                //    UTverify_cast<const GA_VertexGroup*>(geoGroup),
                //    UTverify_cast<const GA_PrimitiveGroup*>(seamGroup),
                //    storage, name, creation_args, attribute_options, reuse, subscribeRatio, minGrainSize);
                break;
            case GA_GROUP_POINT:
                //return GFE_MeshTopology_Namespace::addAttribVertexVertexPoint(geo,
                //    UTverify_cast<const GA_VertexGroup*>(geoGroup),
                //    UTverify_cast<const GA_PointGroup*>(seamGroup),
                //    storage, name, creation_args, attribute_options, reuse, subscribeRatio, minGrainSize);
                break;
            case GA_GROUP_VERTEX:
                //return GFE_MeshTopology_Namespace::addAttribVertexVertexVertex(geo,
                //    UTverify_cast<const GA_VertexGroup*>(geoGroup),
                //    UTverify_cast<const GA_VertexGroup*>(seamGroup),
                //    storage, name, creation_args, attribute_options, reuse, subscribeRatio, minGrainSize);
                break;
            case GA_GROUP_EDGE:
                //return GFE_MeshTopology_Namespace::addAttribVertexVertexEdge(geo,
                //    UTverify_cast<const GA_VertexGroup*>(geoGroup),
                //    UTverify_cast<const GA_PointGroup*>(seamGroup),
                //    storage, name, creation_args, attribute_options, reuse, subscribeRatio, minGrainSize);
                break;
            }
            break;
        }
        return nullptr;
    }

    




} // End of namespace GFE_MeshTopology



*/




#endif
