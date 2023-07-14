
#pragma once

#ifndef __GFE_MeshTopology_h__
#define __GFE_MeshTopology_h__

#include "GFE/GFE_MeshTopology.h"



#include "GFE/GFE_GeoFilter.h"
#include "GFE/GFE_OffsetAttributeToIndex.h"




class GFE_MeshTopology : public GFE_AttribFilter {

public:
    enum Type {
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
        //unsharedEdgeGroup           = nullptr;
    }
    
    virtual void reset(GA_Detail& inGeo, const SOP_NodeVerb::CookParms* const inCookparms = nullptr) override
    {
        if (geo == &inGeo && cookparms == inCookparms)
            return;
        GFE_AttribFilter::reset(inGeo, inCookparms);
        pointSeamGroup.reset(inGeo, inCookparms);
        vertexEdgeSeamGroup.reset(inGeo, inCookparms);
        edgeSeamGroup.reset(inGeo, inCookparms);
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

    SYS_FORCE_INLINE void setKernel(const uint8 kernel)
    { this->kernel = kernel; }



    // SYS_FORCE_INLINE GA_EdgeGroup* setUnsharedEdgeGroup(const bool detached = false, const UT_StringRef& groupName = "__topo_unshared")
    // {
    //     unsharedEdgeGroup = getOutGroupArray().findOrCreateEdge(detached, groupName);
    //     return unsharedEdgeGroup;
    // }
    // SYS_FORCE_INLINE void setUnsharedEdgeGroup(GA_EdgeGroup* const groupPtr)
    // { unsharedEdgeGroup = groupPtr; }
    // 
    // SYS_FORCE_INLINE GA_EdgeGroup* getUnsharedEdgeGroup() const
    // { return unsharedEdgeGroup; }
    


    
    SYS_FORCE_INLINE void setVertexVertexPrim(
        const bool detached = true,
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
        const bool detached = true,                                                                    \
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
        const bool detached = true,                                                          \
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
        if (largeConnectivity)
        {
            switch (attribOwner)
            {
            case GA_ATTRIB_PRIMITIVE: return setPrimPrimPoint (detached, attribName, storage); break;
            case GA_ATTRIB_POINT:     return setPointPointPrim(detached, attribName, storage); break;
            case GA_ATTRIB_VERTEX:    break;
            }
        }
        else
        {
            switch (attribOwner)
            {
            case GA_ATTRIB_PRIMITIVE: return setPrimPrimEdge  (detached, attribName, storage); break;
            case GA_ATTRIB_POINT:     return setPointPointEdge(detached, attribName, storage); break;
            case GA_ATTRIB_VERTEX:    break;
            }
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


    
#define GFE_MeshTopology_VertexNextEquivDual 1

    
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
            !primPrimPointAttrib &&
            !vertexNextEquivGroup &&
            !vertexNextEquivNoLoopGroup)
                return false;

        if (groupParser.isEmpty())
            return true;
        
        //if (vertexEdgeSeamGroup)
        //    GFE_GroupUnion::groupUnion(vertexEdgeSeamGroup, edgeSeamGroup);

        
#if GFE_MeshTopology_VertexNextEquivDual
        if (primPrimEdgeAttrib && !vertexNextEquivAttrib)
            setVertexNextEquiv(!outIntermediateAttrib);
        
        const bool calVertexNextEquivDual =
                   vertexNextEquivNoLoopAttrib ||
                   vertexNextEquivNoLoopGroup ||
                   vertexNextEquivAttrib ||
                   vertexNextEquivGroup  ||
                   primPrimEdgeAttrib;

#else
        const bool calVertexNextEquivNoLoop =
                   vertexNextEquivNoLoopAttrib ||
                   vertexNextEquivNoLoopGroup;

        const bool calVertexNextEquiv =
                   vertexNextEquivAttrib ||
                   vertexNextEquivGroup  ||
                   primPrimEdgeAttrib;
        
        const bool calVertexNextEquivDual =
                   calVertexNextEquivNoLoop ||
                   calVertexNextEquiv;
#endif
        
        const bool calVertexVertexPrim =
                   vertexVertexPrimPrevAttrib  ||
                   vertexVertexPrimNextAttrib  ||
                   pointPointEdgeAttrib;
        
        const bool calVertexPointDst =
                   vertexPointDstAttrib        ||
                   calVertexNextEquivDual;
        
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
            switch (calVertexNextEquivDual ? 0 : kernel)
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

        
#if GFE_MeshTopology_VertexNextEquivDual
        if (calVertexNextEquivDual)
        {
            vertexNextEquivDual();
        }
#else
        if (calVertexNextEquiv)
        {
            vertexNextEquiv();
        }
        if (calVertexNextEquivNoLoop)
        {
            vertexNextEquivNoLoop();
        }
#endif
        
        
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




    //template<typename T>
    void vertexPrimIndex()
    {
        if (!vertexPrimIndexAttrib)
            setVertexPrimIndex(!outIntermediateAttrib);
        
        int_wh = vertexPrimIndexAttrib;
        //const GA_Detail* const geo = this->geo;
        if (groupParser.isFull())
        {
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
                            int_wh.set(geo->vertexPoint(vertices[vtxpnum]), vtxpnum);
                        }
                    }
                }
            }, subscribeRatio, minGrainSize);
        }
        else
        {
            const GA_VertexGroup* const vtxGroup = groupParser.classType() == GA_GROUP_PRIMITIVE ? nullptr : groupParser.getVertexGroup();
            if (vtxGroup)
            {
                UTparallelFor(groupParser.getPrimitiveSplittableRange(), [this, vtxGroup](const GA_SplittableRange& r)
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
                                if (vtxGroup->contains(vertices[vtxpnum]))
                                    int_wh.set(vertices[vtxpnum], vtxpnum);
                            }
                        }
                    }
                }, subscribeRatio, minGrainSize);
            }
            else
            {
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
        }
    }







    void vertexPointDstByVtxpnum()
    {
        if (!vertexPointDstAttrib)
            setVertexPointDst(!outIntermediateAttrib);
        
        int_wh = vertexPointDstAttrib;
        int_oh = vertexPrimIndexAttrib;
        
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
        if (!vertexVertexPrimPrevAttrib && !vertexVertexPrimNextAttrib)
            setVertexVertexPrim(!outIntermediateAttrib);
        
        int_wh  = vertexVertexPrimPrevAttrib;
        int1_wh = vertexVertexPrimNextAttrib;
        
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
                        const GA_Size lastVtxpnum = vertices.size()-1;
                        const GA_Offset vtxoff0 = vertices[0];
                        const GA_Offset vtxoff1 = vertices[lastVtxpnum];
                        if (geo->getPrimitiveClosedFlag(elemoff))
                        {
                            if (vertexVertexPrimPrevAttrib)
                                int_wh.set(vtxoff0, vtxoff1);
                            if (vertexVertexPrimNextAttrib)
                                int1_wh.set(vtxoff1, vtxoff0);
                        }
                        else
                        {
                            if (vertexVertexPrimPrevAttrib)
                                int_wh.set(vtxoff0, GFE_INVALID_OFFSET);
                            if (vertexVertexPrimNextAttrib)
                                int1_wh.set(vtxoff1, GFE_INVALID_OFFSET);
                        }
                        GA_Offset vtxoff_prev = vtxoff0;
                        GA_Offset vtxoff_next;
                        for (GA_Size vtxpnum = 1; vtxpnum <= lastVtxpnum; ++vtxpnum)
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
            const GA_VertexGroup* const vtxGroup = groupParser.classType() == GA_GROUP_PRIMITIVE ? nullptr : groupParser.getVertexGroup();
            UTparallelFor(groupParser.getPrimitiveSplittableRange(), [this, vtxGroup](const GA_SplittableRange& r)
            {
                GA_Offset start, end;
                for (GA_Iterator it(r); it.blockAdvance(start, end); )
                {
                    for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                    {
                        const GA_OffsetListRef& vertices = geo->getPrimitiveVertexList(elemoff);
                        const GA_Size lastVtxpnum = vertices.size()-1;
                        const GA_Offset vtxoff0 = vertices[0];
                        const GA_Offset vtxoff1 = vertices[lastVtxpnum];
                        if (geo->getPrimitiveClosedFlag(elemoff))
                        {
                            if (vertexVertexPrimPrevAttrib)
                                int_wh.set(vtxoff0, vtxoff1);
                            if (vertexVertexPrimNextAttrib)
                                int1_wh.set(vtxoff1, vtxoff0);
                        }
                        else
                        {
                            if (vertexVertexPrimPrevAttrib)
                                int_wh.set(vtxoff0, GFE_INVALID_OFFSET);
                            if (vertexVertexPrimNextAttrib)
                                int1_wh.set(vtxoff1, GFE_INVALID_OFFSET);
                        }
                        GA_Offset vtxoff_prev = vtxoff0;
                        GA_Offset vtxoff_next;
                        for (GA_Size vtxpnum = 1; vtxpnum <= lastVtxpnum; ++vtxpnum)
                        {
                            vtxoff_next = vertices[vtxpnum];
                            if (vtxGroup && !vtxGroup->contains(vtxoff_next))
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

    void vertexVertexPrim1()
    {
        if(!vertexVertexPrimPrevAttrib && !vertexVertexPrimNextAttrib)
            setVertexVertexPrim(!outIntermediateAttrib);
        
        int_wh  = vertexVertexPrimPrevAttrib;
        int1_wh = vertexVertexPrimNextAttrib;
        
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
        if (!vertexPointDstAttrib)
            setVertexPointDst(!outIntermediateAttrib);
        
        const GA_ATITopology* const vtxPointRef = geo->getTopology().getPointRef();

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
                        if (GFE_Type::isValidOffset(vtxPrimNext_ph.value(elemoff)))
                            dstpt_ph.value(elemoff) = GFE_INVALID_OFFSET;
                        else
                            dstpt_ph.value(elemoff) = vtxPointRef->getLink(vtxPrimNext_ph.value(elemoff));
                    }
                }
            }
        }, subscribeRatio, minGrainSize);
    }




#if GFE_MeshTopology_VertexNextEquivDual

    
void vertexNextEquivDual()
{
    if (!vertexNextEquivAttrib && !vertexNextEquivGroup && !vertexNextEquivNoLoopAttrib && !vertexNextEquivNoLoopGroup)
        setVertexNextEquiv(!outIntermediateAttrib);

    int_wh  = vertexNextEquivAttrib;
    int1_wh = vertexNextEquivNoLoopAttrib;
    int_oh  = vertexPointDstAttrib;
    
    const GA_Topology& topo = geo->getTopology();
    //topo.makeVertexRef();
    const GA_ATITopology* const vtxPointRef = topo.getPointRef();
    const GA_ATITopology* const pointVtxRef = topo.getVertexRef();
    const GA_ATITopology* const vtxNextRef  = topo.getVertexNextRef();

    const GA_EdgeGroup* const edgeGroup = groupParser.classType() == GA_GROUP_EDGE ? groupParser.getEdgeGroup() : nullptr;
    const GA_SplittableRange& srange = edgeGroup ? GA_SplittableRange(GA_Range(geo->getVertexMap())) : groupParser.getVertexSplittableRange();
        
    UTparallelFor(srange, [this, vtxPointRef, pointVtxRef, vtxNextRef, edgeGroup](const GA_SplittableRange& r)
    {
        GA_Offset start, end;
        for (GA_Iterator it(r); it.blockAdvance(start, end); )
        {
            for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
            {
                GA_Offset dstpt = int_oh.get(elemoff);
                
                if (edgeGroup && !edgeGroup->contains(GA_Edge(vtxPointRef->getLink(elemoff), dstpt)))
                    continue;
                
                if (GFE_Type::isInvalidOffset(dstpt))
                {
                    if (vertexNextEquivAttrib)
                        int_wh.set(elemoff, GFE_INVALID_OFFSET);
                    if (vertexNextEquivNoLoopAttrib)
                        int1_wh.set(elemoff, GFE_INVALID_OFFSET);
                    continue;
                }
                for (GA_Offset vtxoff_next = vtxNextRef->getLink(elemoff); GFE_Type::isValidOffset(vtxoff_next); vtxoff_next = vtxNextRef->getLink(vtxoff_next))
                {
                    if (int_oh.get(vtxoff_next) != dstpt)
                        continue;
                    dstpt = GFE_INVALID_OFFSET;
                    if (vertexNextEquivAttrib)
                        int_wh.set(elemoff, vtxoff_next);
                    if (vertexNextEquivNoLoopAttrib)
                        int1_wh.set(elemoff, vtxoff_next);
                    break;
                }
                if (GFE_Type::isInvalidOffset(dstpt))
                    continue;
                const GA_Offset ptoff = vtxPointRef->getLink(elemoff);
                for (GA_Offset vtxoff_next = pointVtxRef->getLink(dstpt); ; vtxoff_next = vtxNextRef->getLink(vtxoff_next))
                {
                    if (GFE_Type::isInvalidOffset(vtxoff_next))
                    {
                        if (vertexNextEquivGroup)
                            vertexNextEquivGroup->setElement(elemoff, true);
                        if (vertexNextEquivAttrib)
                            int_wh.set(elemoff, GFE_INVALID_OFFSET);
                        
                        if (vertexNextEquivNoLoopGroup)
                            vertexNextEquivNoLoopGroup->setElement(elemoff, true);
                        if (vertexNextEquivNoLoopAttrib)
                            int1_wh.set(elemoff, GFE_INVALID_OFFSET);
                        break;
                    }
                    if (int_oh.get(vtxoff_next) == ptoff)
                    {
                        if (vertexNextEquivAttrib)
                            int_wh.set(elemoff, vtxoff_next);
                        if (dstpt > ptoff)
                        {
                            if (vertexNextEquivNoLoopAttrib)
                                int1_wh.set(elemoff, vtxoff_next);
                        }
                        else
                        {
                            if (vertexNextEquivNoLoopGroup)
                                vertexNextEquivNoLoopGroup->setElement(elemoff, true);
                            if (vertexNextEquivNoLoopAttrib)
                                int1_wh.set(elemoff, GFE_INVALID_OFFSET);
                        }
                        break;
                    }
                }
            }
        }
    }, subscribeRatio, minGrainSize);

    if (vertexNextEquivGroup)
        vertexNextEquivGroup->invalidateGroupEntries();
    if (vertexNextEquivNoLoopGroup)
        vertexNextEquivNoLoopGroup->invalidateGroupEntries();
}



#else

void vertexNextEquiv()
{
    if (!vertexNextEquivAttrib && !vertexNextEquivGroup)
        setVertexNextEquiv(!outIntermediateAttrib);
    
    int_wh = vertexNextEquivAttrib;
    int_oh = vertexPointDstAttrib;
    
    const GA_Topology& topo = geo->getTopology();
    //topo.makeVertexRef();
    const GA_ATITopology* const vtxPointRef = topo.getPointRef();
    const GA_ATITopology* const pointVtxRef = topo.getVertexRef();
    const GA_ATITopology* const vtxNextRef  = topo.getVertexNextRef();

    const GA_EdgeGroup* const edgeGroup = groupParser.classType() == GA_GROUP_EDGE ? groupParser.getEdgeGroup() : nullptr;
    const GA_SplittableRange& srange = edgeGroup ? GA_SplittableRange(GA_Range(geo->getVertexMap())) : groupParser.getVertexSplittableRange();
    
    UTparallelFor(groupParser.getVertexSplittableRange(), [this, vtxPointRef, pointVtxRef, vtxNextRef, edgeGroup](const GA_SplittableRange& r)
    {
        GA_Offset vtxoff_next, dstpt, ptoff;
        GA_Offset start, end;
        for (GA_Iterator it(r); it.blockAdvance(start, end); )
        {
            for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
            {
                dstpt = int_oh.get(elemoff);
                if (edgeGroup && !edgeGroup->contains(GA_Edge(vtxPointRef->getLink(elemoff), dstpt)))
                    continue;
                
                if (GFE_Type::isInvalidOffset(dstpt))
                {
                    if (vertexNextEquivAttrib)
                        int_wh.set(elemoff, GFE_INVALID_OFFSET);
                    continue;
                }
                for (vtxoff_next = vtxNextRef->getLink(elemoff); GFE_Type::isValidOffset(vtxoff_next); vtxoff_next = vtxNextRef->getLink(vtxoff_next))
                {
                    if (int_oh.get(vtxoff_next) != dstpt)
                        continue;
                    dstpt = GFE_INVALID_OFFSET;
                    if (vertexNextEquivAttrib)
                        int_wh.set(elemoff, vtxoff_next);
                    break;
                }
                if (GFE_Type::isInvalidOffset(dstpt))
                    continue;
                ptoff = vtxPointRef->getLink(elemoff);
                for (vtxoff_next = pointVtxRef->getLink(dstpt); GFE_Type::isValidOffset(vtxoff_next); vtxoff_next = vtxNextRef->getLink(vtxoff_next))
                {
                    if (int_oh.get(vtxoff_next) != ptoff)
                        continue;
                    dstpt = GFE_INVALID_OFFSET;
                    if (vertexNextEquivAttrib)
                        int_wh.set(elemoff, vtxoff_next);
                    break;
                }
                if (GFE_Type::isInvalidOffset(dstpt))
                    continue;
                if (vertexNextEquivGroup)
                    vertexNextEquivGroup->setElement(elemoff, true);
                if (vertexNextEquivAttrib)
                    int_wh.set(elemoff, GFE_INVALID_OFFSET);
            }
        }
    }, subscribeRatio, minGrainSize);
    
    if (vertexNextEquivGroup)
        vertexNextEquivGroup->invalidateGroupEntries();
}



void vertexNextEquivNoLoop()
{
    if (!vertexNextEquivNoLoopAttrib && !vertexNextEquivNoLoopGroup)
        setVertexNextEquivNoLoop(!outIntermediateAttrib);
    
    int_wh = vertexNextEquivNoLoopAttrib;
    int_oh = vertexPointDstAttrib;
    
    const GA_Topology& topo = geo->getTopology();
    //topo.makeVertexRef();
    const GA_ATITopology* const vtxPointRef = topo.getPointRef();
    const GA_ATITopology* const pointVtxRef = topo.getVertexRef();
    const GA_ATITopology* const vtxNextRef = topo.getVertexNextRef();

    const GA_EdgeGroup* const edgeGroup = groupParser.classType() == GA_GROUP_EDGE ? groupParser.getEdgeGroup() : nullptr;
    const GA_SplittableRange& srange = edgeGroup ? GA_SplittableRange(GA_Range(geo->getVertexMap())) : groupParser.getVertexSplittableRange();

    UTparallelFor(groupParser.getVertexSplittableRange(), [this, vtxPointRef, pointVtxRef, vtxNextRef, edgeGroup](const GA_SplittableRange& r)
    {
        GA_Offset vtxoff_next, dstpt, ptoff;
        GA_Offset start, end;
        for (GA_Iterator it(r); it.blockAdvance(start, end); )
        {
            for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
            {
                dstpt = int_oh.get(elemoff);
                
                if (edgeGroup && !edgeGroup->contains(GA_Edge(vtxPointRef->getLink(elemoff), dstpt)))
                    continue;
                
                if (GFE_Type::isInvalidOffset(dstpt))
                {
                    if (vertexNextEquivNoLoopAttrib)
                        int_wh.set(elemoff, GFE_INVALID_OFFSET);
                    continue;
                }
                for (vtxoff_next = vtxNextRef->getLink(elemoff); GFE_Type::isValidOffset(vtxoff_next); vtxoff_next = vtxNextRef->getLink(vtxoff_next))
                {
                    if (int_oh.get(vtxoff_next) != dstpt)
                        continue;
                    dstpt = GFE_INVALID_OFFSET;
                    if (vertexNextEquivNoLoopAttrib)
                        int_wh.set(elemoff, vtxoff_next);
                    break;
                }
                if (GFE_Type::isInvalidOffset(dstpt))
                    continue;
                ptoff = vtxPointRef->getLink(elemoff);
                for (vtxoff_next = pointVtxRef->getLink(dstpt); GFE_Type::isValidOffset(vtxoff_next); vtxoff_next = vtxNextRef->getLink(vtxoff_next))
                {
                    if (int_oh.get(vtxoff_next) != ptoff)
                        continue;
                    if (dstpt > ptoff)
                    {
                        dstpt = GFE_INVALID_OFFSET;
                        if (vertexNextEquivNoLoopAttrib)
                            int_wh.set(elemoff, vtxoff_next);
                    }
                    break;
                }
                if (GFE_Type::isInvalidOffset(dstpt))
                    continue;
                if (vertexNextEquivNoLoopGroup)
                    vertexNextEquivNoLoopGroup->setElement(elemoff, true);
                if (vertexNextEquivNoLoopAttrib)
                    int_wh.set(elemoff, GFE_INVALID_OFFSET);
            }
        }
    }, subscribeRatio, minGrainSize);
    
    if (vertexNextEquivNoLoopGroup)
        vertexNextEquivNoLoopGroup->invalidateGroupEntries();
}

#endif



    
    // void unsharedEdge()
    // {
    //     if(!unsharedEdgeGroup)
    //         setUnsharedEdgeGroup(!outIntermediateAttrib);
    //     
    // }

    


    

    //Get all prims neighbours prims with adjacent by edge
    void pointPointEdge()
    {
        if(!pointPointEdgeAttrib)
            setPointPointEdge(!outIntermediateAttrib);
        
        intArray_wh = pointPointEdgeAttrib;
        int_oh = vertexPrimIndexAttrib;
        
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

                    const GA_OffsetListRef& vertices = geo->getPrimitiveVertexList(elemoff);
                    GA_Offset pt_next;
                    for (GA_Offset vtxoff_next = geo->pointVertex(elemoff); GFE_Type::isValidOffset(vtxoff_next); vtxoff_next = geo->vertexToNextVertex(vtxoff_next))
                    {
                        const GA_Offset primoff = geo->vertexPrimitive(vtxoff_next);
                        const GA_Size numvtx = geo->getPrimitiveVertexCount(primoff);
                        const GA_Size vtxpnum = int_oh.get(vtxoff_next);

                        if (vtxpnum == 0)
                        {
                            if (geo->getPrimitiveClosedFlag(primoff))
                            {
                                pt_next = geo->vertexPoint(vertices[numvtx-1]);
                                if (ptoffArray.find(pt_next) == GFE_FIND_INVALID_INDEX)
                                    ptoffArray.emplace_back(pt_next);
                            }
                        }
                        else
                        {
                            pt_next = geo->vertexPoint(vertices[vtxpnum-1]);
                            if (ptoffArray.find(pt_next) == GFE_FIND_INVALID_INDEX)
                                ptoffArray.emplace_back(pt_next);
                        }

                        const GA_Size vtxpnum_next = vtxpnum+1;
                        if (vtxpnum_next == numvtx)
                        {
                            if (geo->getPrimitiveClosedFlag(primoff))
                            {
                                pt_next = geo->vertexPoint(vertices[0]);
                                if (ptoffArray.find(pt_next) == GFE_FIND_INVALID_INDEX)
                                    ptoffArray.emplace_back(pt_next);
                            }
                        }
                        else
                        {
                            pt_next = geo->vertexPoint(vertices[vtxpnum_next]);
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
        
        intArray_wh = pointPointEdgeAttrib;
        int_oh = vertexVertexPrimPrevAttrib;
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
                    for (GA_Offset vtxoff_next = pointVtxRef->getLink(elemoff); GFE_Type::isValidOffset(vtxoff_next); vtxoff_next = vtxNextRef->getLink(vtxoff_next))
                    {
                        const GA_Offset vtxPrev = int_oh.get(vtxoff_next);
                        if (GFE_Type::isValidOffset(vtxPrev))
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
                        if (GFE_Type::isValidOffset(vtxNext))
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
    void pointPointEdge2(
            const GA_Offset ptoff,
            UT_ValArray<GA_Offset>& ptoffArray
        )
    {
        //exint cap = 0;
        ptoffArray.clear();

        GA_Offset pt_next;
        for (GA_Offset vtxoff_next = geo->pointVertex(ptoff); GFE_Type::isValidOffset(vtxoff_next); vtxoff_next = geo->vertexToNextVertex(vtxoff_next))
        {
            const GA_Offset primoff = geo->vertexPrimitive(vtxoff_next);
            
            const GA_OffsetListRef& vertices = geo->getPrimitiveVertexList(primoff);
            const GA_Size numvtx = vertices.size();
            const GA_Size vtxpnum = geo->vertexPrimIndex(primoff, vtxoff_next);

            if (vtxpnum == 0)
            {
                if (geo->getPrimitiveClosedFlag(primoff))
                {
                    pt_next = geo->vertexPoint(vertices[numvtx-1]);
                    if (ptoffArray.find(pt_next) == GFE_FIND_INVALID_INDEX)
                        ptoffArray.emplace_back(pt_next);
                }
            }
            else
            {
                pt_next = geo->vertexPoint(vertices[vtxpnum-1]);
                if (ptoffArray.find(pt_next) == GFE_FIND_INVALID_INDEX)
                    ptoffArray.emplace_back(pt_next);
            }

            const GA_Size vtxpnum_next = vtxpnum + 1;
            if (vtxpnum_next == numvtx) {
                if (geo->getPrimitiveClosedFlag(primoff))
                {
                    pt_next = geo->vertexPoint(vertices[0]);
                    if (ptoffArray.find(pt_next) == GFE_FIND_INVALID_INDEX)
                        ptoffArray.emplace_back(pt_next);
                }
            }
            else
            {
                pt_next = geo->vertexPoint(vertices[vtxpnum_next]);
                if (ptoffArray.find(pt_next) == GFE_FIND_INVALID_INDEX)
                    ptoffArray.emplace_back(pt_next);
            }
        }
    }

    //Get all prims neighbours prims with adjacent by edge
    void pointPointEdge2()
    {
        if(!pointPointEdgeAttrib)
            setPointPointEdge(!outIntermediateAttrib);
        
        intArray_wh = pointPointEdgeAttrib;
        
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
    void primPrimEdge()
    {
        if (!primPrimEdgeAttrib)
            setPrimPrimEdge(!outIntermediateAttrib);
        
        intArray_wh = primPrimEdgeAttrib;
        int_oh      = vertexNextEquivAttrib;
        
        const GA_VertexGroup* const seamGroup = vertexEdgeSeamGroup.getVertexGroup();
        const GA_Topology& topo = geo->getTopology();
        //topo.makePrimitiveRef();
        const GA_ATITopology* const vtxPrimRef = topo.getPrimitiveRef();

        //const GA_Detail* const geo = this->geo;
        UTparallelFor(groupParser.getPrimitiveSplittableRange(), [this, seamGroup, vtxPrimRef](const GA_SplittableRange& r)
        {
            UT_ValArray<GA_Offset> adjElems;
            GA_Offset start, end;
            for (GA_Iterator it(r); it.blockAdvance(start, end); )
            {
                for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                {
                    adjElems.clear();
                    const GA_OffsetListRef& vertices = geo->getPrimitiveVertexList(elemoff);
                    const GA_Size numvtx = vertices.size();
                    for (GA_Size vtxpnum = 0; vtxpnum < numvtx; ++vtxpnum)
                    {
                        const GA_Offset vtxoff_start = vertices[vtxpnum];
                        if (seamGroup && !seamGroup->contains(vtxoff_start))
                            continue;
                        GA_Offset vtxoff_next = int_oh.get(vtxoff_start);
                        if (GFE_Type::isInvalidOffset(vtxoff_next))
                            continue;
                        for (; vtxoff_next != vtxoff_start; vtxoff_next = int_oh.get(vtxoff_next))
                        {
                            const GA_Offset primoff = vtxPrimRef->getLink(vtxoff_next);
                            if (adjElems.find(primoff) == GFE_FIND_INVALID_INDEX)
                                adjElems.emplace_back(vtxPrimRef->getLink(vtxoff_next));
                        }
                    }
                    intArray_wh.set(elemoff, adjElems);
                }
            }
        }, subscribeRatio, minGrainSize);
    }

    


    //Get all prims neighbours prims with adjacent by edge
    void primPrimEdge1()
    {
        if (!primPrimEdgeAttrib)
            setPrimPrimEdge(!outIntermediateAttrib);
        
        intArray_wh = primPrimEdgeAttrib;
        int_oh = vertexPointDstAttrib;
        
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
                        for (GA_Size vtxpnum = 0; vtxpnum < numvtx; ++vtxpnum)
                        {
                            const GA_Offset vtxoff = vertices[vtxpnum];
                            const GA_Offset dstpt = int_oh.get(vtxoff);
                            if (GFE_Type::isInvalidOffset(dstpt))
                                continue;
                            const GA_Offset ptoff = geo->vertexPoint(vtxoff);
                            for (vtxoff_next = geo->pointVertex(ptoff); GFE_Type::isValidOffset(vtxoff_next); vtxoff_next = geo->vertexToNextVertex(vtxoff_next))
                            {
                                if (vtxoff_next == vtxoff)
                                    continue;
                                if (int_oh.get(vtxoff_next) != dstpt)
                                    continue;
                                if (adjElems.find(geo->vertexPrimitive(vtxoff_next)) == GFE_FIND_INVALID_INDEX)
                                    adjElems.emplace_back(geo->vertexPrimitive(vtxoff_next));
                            }

                            for (vtxoff_next = geo->pointVertex(dstpt); GFE_Type::isValidOffset(vtxoff_next); vtxoff_next = geo->vertexToNextVertex(vtxoff_next))
                            {
                                if (int_oh.get(vtxoff_next) != ptoff)
                                    continue;
                                if (adjElems.find(geo->vertexPrimitive(vtxoff_next)) == GFE_FIND_INVALID_INDEX)
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
                        for (GA_Size vtxpnum = 0; vtxpnum < numvtx; ++vtxpnum)
                        {
                            const GA_Offset vtxoff = vertices[vtxpnum];
                            const GA_Offset dstpt = int_oh.get(vtxoff);
                            if (GFE_Type::isInvalidOffset(dstpt))
                                continue;
                            const GA_Offset ptoff = geo->vertexPoint(vtxoff);
                            for (vtxoff_next = geo->pointVertex(ptoff); GFE_Type::isValidOffset(vtxoff_next); vtxoff_next = geo->vertexToNextVertex(vtxoff_next))
                            {
                                if (vtxoff_next == vtxoff)
                                    continue;
                                if (int_oh.get(vtxoff_next) != dstpt)
                                    continue;
                                if (adjElems.find(geo->vertexPrimitive(vtxoff_next)) == GFE_FIND_INVALID_INDEX)
                                    adjElems.emplace_back(geo->vertexPrimitive(vtxoff_next));
                            }

                            for (vtxoff_next = geo->pointVertex(dstpt); GFE_Type::isValidOffset(vtxoff_next); vtxoff_next = geo->vertexToNextVertex(vtxoff_next))
                            {
                                if (int_oh.get(vtxoff_next) != ptoff)
                                    continue;
                                if (adjElems.find(geo->vertexPrimitive(vtxoff_next)) == GFE_FIND_INVALID_INDEX)
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
    void primPrimEdge2()
    {
        if (!primPrimEdgeAttrib)
            setPrimPrimEdge(!outIntermediateAttrib);
        
        intArray_wh = primPrimEdgeAttrib;
        int_oh = vertexPointDstAttrib;

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
                        for (GA_Size vtxpnum = 0; vtxpnum < numvtx; ++vtxpnum)
                        {
                            const GA_Offset vtxoff = vertices[vtxpnum];
                            const GA_Offset dstpt = int_oh.get(vtxoff);
                            if (GFE_Type::isInvalidOffset(dstpt))
                                continue;
                            const GA_Offset ptoff = geo->vertexPoint(vtxoff);
                            for (vtxoff_next = geo->pointVertex(ptoff); GFE_Type::isValidOffset(vtxoff_next); vtxoff_next = geo->vertexToNextVertex(vtxoff_next))
                            {
                                if (vtxoff_next == vtxoff)
                                    continue;
                                if (int_oh.get(vtxoff_next) != dstpt)
                                    continue;
                                if (adjElems.find(geo->vertexPrimitive(vtxoff_next)) == GFE_FIND_INVALID_INDEX)
                                    adjElems.emplace_back(geo->vertexPrimitive(vtxoff_next));
                            }

                            for (vtxoff_next = geo->pointVertex(dstpt); GFE_Type::isValidOffset(vtxoff_next); vtxoff_next = geo->vertexToNextVertex(vtxoff_next))
                            {
                                if (int_oh.get(vtxoff_next) != ptoff)
                                    continue;
                                if (adjElems.find(geo->vertexPrimitive(vtxoff_next)) == GFE_FIND_INVALID_INDEX)
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
                        for (GA_Size vtxpnum = 0; vtxpnum < numvtx; ++vtxpnum)
                        {
                            const GA_Offset vtxoff = vertices[vtxpnum];
                            const GA_Offset dstpt = int_oh.get(vtxoff);
                            if (GFE_Type::isInvalidOffset(dstpt))
                                continue;
                            const GA_Offset ptoff = geo->vertexPoint(vtxoff);
                            for (vtxoff_next = geo->pointVertex(ptoff); GFE_Type::isValidOffset(vtxoff_next); vtxoff_next = geo->vertexToNextVertex(vtxoff_next))
                            {
                                if (vtxoff_next == vtxoff)
                                    continue;
                                if (int_oh.get(vtxoff_next) != dstpt)
                                    continue;
                                if (adjElems.find(geo->vertexPrimitive(vtxoff_next)) == GFE_FIND_INVALID_INDEX)
                                    adjElems.emplace_back(geo->vertexPrimitive(vtxoff_next));
                            }

                            for (vtxoff_next = geo->pointVertex(dstpt); GFE_Type::isValidOffset(vtxoff_next); vtxoff_next = geo->vertexToNextVertex(vtxoff_next))
                            {
                                if (int_oh.get(vtxoff_next) != ptoff)
                                    continue;
                                if (adjElems.find(geo->vertexPrimitive(vtxoff_next)) == GFE_FIND_INVALID_INDEX)
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
    void primPrimEdge3()
    {
        if (!primPrimEdgeAttrib)
            setPrimPrimEdge(outTopoAttrib);
        
        intArray_wh = primPrimEdgeAttrib;
        int_oh = vertexPointDstAttrib;
        
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
                        for (GA_Size vtxpnum = 0; vtxpnum < numvtx; ++vtxpnum)
                        {
                            const GA_Offset vtxoff = vertices[vtxpnum];
                            const GA_Offset dstpt = int_oh.get(vtxoff);
                            if (GFE_Type::isInvalidOffset(dstpt))
                                continue;
                            const GA_Offset ptoff = geo->vertexPoint(vtxoff);
                            for (vtxoff_next = geo->pointVertex(ptoff); GFE_Type::isValidOffset(vtxoff_next); vtxoff_next = geo->vertexToNextVertex(vtxoff_next))
                            {
                                if (vtxoff_next == vtxoff)
                                    continue;
                                if (int_oh.get(vtxoff_next) != dstpt)
                                    continue;
                                if (adjElems.find(geo->vertexPrimitive(vtxoff_next)) == GFE_FIND_INVALID_INDEX)
                                    adjElems.emplace_back(geo->vertexPrimitive(vtxoff_next));
                            }

                            for (vtxoff_next = geo->pointVertex(dstpt); GFE_Type::isValidOffset(vtxoff_next); vtxoff_next = geo->vertexToNextVertex(vtxoff_next))
                            {
                                if (int_oh.get(vtxoff_next) != ptoff)
                                    continue;
                                if (adjElems.find(geo->vertexPrimitive(vtxoff_next)) == GFE_FIND_INVALID_INDEX)
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
                        for (GA_Size vtxpnum = 0; vtxpnum < numvtx; ++vtxpnum)
                        {
                            const GA_Offset vtxoff = vertices[vtxpnum];
                            const GA_Offset dstpt = int_oh.get(vtxoff);
                            if (GFE_Type::isInvalidOffset(dstpt))
                                continue;
                            const GA_Offset ptoff = geo->vertexPoint(vtxoff);
                            for (vtxoff_next = geo->pointVertex(ptoff); GFE_Type::isValidOffset(vtxoff_next); vtxoff_next = geo->vertexToNextVertex(vtxoff_next))
                            {
                                if (vtxoff_next == vtxoff)
                                    continue;
                                if (int_oh.get(vtxoff_next) != dstpt)
                                    continue;
                                if (adjElems.find(geo->vertexPrimitive(vtxoff_next)) == GFE_FIND_INVALID_INDEX)
                                    adjElems.emplace_back(geo->vertexPrimitive(vtxoff_next));
                            }

                            for (vtxoff_next = geo->pointVertex(dstpt); GFE_Type::isValidOffset(vtxoff_next); vtxoff_next = geo->vertexToNextVertex(vtxoff_next))
                            {
                                if (int_oh.get(vtxoff_next) != ptoff)
                                    continue;
                                if (adjElems.find(geo->vertexPrimitive(vtxoff_next)) == GFE_FIND_INVALID_INDEX)
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
    void primPrimEdge4()
    {
        if(!primPrimEdgeAttrib)
            setPrimPrimEdge(!outIntermediateAttrib);
        
        intArray_wh = primPrimEdgeAttrib;
        
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

    void primPrimPoint()
    {
        if(!primPrimPointAttrib)
            setPrimPrimPoint(!outIntermediateAttrib);
            
        intArray_wh = primPrimPointAttrib;
        
        const GA_PointGroup* const seamGroup = pointSeamGroup.getPointGroup();
        
        const GA_Topology& topo = geo->getTopology();
        //topo.makeFull();
        //topo.makePrimitiveRef();
        //topo.makeVertexRef();
        const GA_ATITopology* const vtxPrimRef  = topo.getPrimitiveRef();
        const GA_ATITopology* const vtxPointRef = topo.getPointRef();
        const GA_ATITopology* const pointVtxRef = topo.getVertexRef();
        const GA_ATITopology* const vtxNextRef  = topo.getVertexNextRef();
        
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
                        for (GA_Size vtxpnum = 0; vtxpnum < numvtx; ++vtxpnum)
                        {
                            const GA_Offset vtxoff = vertices[vtxpnum];
                            const GA_Offset ptoff = vtxPointRef->getLink(vtxoff);
                            if (seamGroup->contains(ptoff))
                                continue;
                            for (vtxoff_next = pointVtxRef->getLink(ptoff); GFE_Type::isValidOffset(vtxoff_next); vtxoff_next = vtxNextRef->getLink(vtxoff_next))
                            {
                                if (vtxoff_next == vtxoff)
                                    continue;
                                if (adjElems.find(vtxPrimRef->getLink(vtxoff_next)) == GFE_FIND_INVALID_INDEX)
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
                        for (GA_Size vtxpnum = 0; vtxpnum < numvtx; ++vtxpnum)
                        {
                            const GA_Offset vtxoff = vertices[vtxpnum];
                            const GA_Offset ptoff = vtxPointRef->getLink(vtxoff);
                            for (vtxoff_next = pointVtxRef->getLink(ptoff); GFE_Type::isValidOffset(vtxoff_next); vtxoff_next = vtxNextRef->getLink(vtxoff_next))
                            {
                                if (vtxoff_next == vtxoff)
                                    continue;
                                if (adjElems.find(vtxPrimRef->getLink(vtxoff_next)) == GFE_FIND_INVALID_INDEX)
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
    //GA_EdgeGroup*   unsharedEdgeGroup           = nullptr;
    
        
    GA_RWHandleT<GA_Offset> int_wh;
    GA_RWHandleT<GA_Offset> int1_wh;
        
    GA_ROHandleT<GA_Offset> int_oh;
    GA_RWHandleT<UT_ValArray<GA_Offset>> intArray_wh;

        
    GFE_GroupParser pointSeamGroup;
    GFE_GroupParser vertexEdgeSeamGroup;
    GFE_GroupParser edgeSeamGroup;

    uint8 kernel = 0;
    exint subscribeRatio = 64;
    exint minGrainSize = 64;

#undef GFE_MeshTopology_VertexNextEquivDual
    
}; // End of class GFE_MeshTopology





#endif
