
#pragma once

#ifndef __GFE_GroupUnion_h__
#define __GFE_GroupUnion_h__

#include "GFE/GFE_GroupUnion.h"



#include "GA/GA_Detail.h"
#include "GA/GA_SplittableRange.h"


#include "GFE/GFE_Type.h"


class GFE_GroupUnion {



public:
    
    /////////////////////////// Utility For Group Pointer //////////////////////////
    
    template<typename GROUP_T, typename GROUP_REF_T>
    SYS_FORCE_INLINE static void groupUnion(
        GROUP_T* const group, const GROUP_REF_T* const groupRef,
        const bool reverse = false,
        const exint subscribeRatio = 64, const exint minGrainSize = 1024)
    { UT_ASSERT_P(group); groupUnion(*group, groupRef, reverse, subscribeRatio, minGrainSize); }
    
    template<typename GROUP_T, typename GROUP_REF_T>
    static void groupUnion(
        GROUP_T& group, const GROUP_REF_T* const groupRef,
        const bool reverse = false,
        const exint subscribeRatio = 64, const exint minGrainSize = 1024)
    {
        if (!groupRef)
        {
            if (!reverse)
                group.addAll();
            return;
        }
        groupUnion(group, *groupRef, reverse, subscribeRatio, minGrainSize);
    }

#define GFE_GroupUnion_FUNC_SPECILIZATION(GroupType)                                                     \
static void groupUnion(GA_EdgeGroup& group, const GroupType* const groupRef, const bool reverse = false) \
{                                                                                                        \
    if (!groupRef)                                                                                       \
    {                                                                                                    \
        if (!reverse)                                                                                    \
            group.addAll();                                                                              \
        return;                                                                                          \
    }                                                                                                    \
    groupUnion(group, *groupRef, reverse);                                                               \
}                                                                                                        \

    GFE_GroupUnion_FUNC_SPECILIZATION(GA_PrimitiveGroup);
    GFE_GroupUnion_FUNC_SPECILIZATION(GA_PointGroup);
    GFE_GroupUnion_FUNC_SPECILIZATION(GA_VertexGroup);
    
#undef GFE_GroupUnion_FUNC_SPECILIZATION

    template<typename GROUP_T>
    static void groupUnion(GROUP_T& group, const GA_EdgeGroup* const groupRef, const bool reverse = false)
    {
        if (!groupRef)
        {
            if (!reverse)
                group->addAll();
            return;
        }
        groupUnion(group, *groupRef, reverse);
    }




    
    /////////////////////////// Utility //////////////////////////

    static void groupUnion(
        GA_Group& group, const GA_Group& groupRef,
        const bool reverse = false,
        const exint subscribeRatio = 64, const exint minGrainSize = 1024)
    {
        switch (group.classType())
        {
        case GA_GROUP_PRIMITIVE: groupUnion(static_cast<GA_PrimitiveGroup&>(group), groupRef, reverse, subscribeRatio, minGrainSize); break;
        case GA_GROUP_POINT:     groupUnion(static_cast<GA_PointGroup&    >(group), groupRef, reverse, subscribeRatio, minGrainSize); break;
        case GA_GROUP_VERTEX:    groupUnion(static_cast<GA_VertexGroup&   >(group), groupRef, reverse, subscribeRatio, minGrainSize); break;
        case GA_GROUP_EDGE:      groupUnion(static_cast<GA_EdgeGroup&     >(group), groupRef, reverse);                               break;
        }
    }

    template<typename GROUP_T>
    static void groupUnion(
        GROUP_T& group, const GA_Group& groupRef,
        const bool reverse = false,
        const exint subscribeRatio = 64, const exint minGrainSize = 1024)
    {
        switch (groupRef.classType())
        {
        case GA_GROUP_PRIMITIVE: groupUnion(group, static_cast<const GA_PrimitiveGroup&>(groupRef), reverse, subscribeRatio, minGrainSize); break;
        case GA_GROUP_POINT:     groupUnion(group, static_cast<const GA_PointGroup&    >(groupRef), reverse, subscribeRatio, minGrainSize); break;
        case GA_GROUP_VERTEX:    groupUnion(group, static_cast<const GA_VertexGroup&   >(groupRef), reverse, subscribeRatio, minGrainSize); break;
        case GA_GROUP_EDGE:      groupUnion(group, static_cast<const GA_EdgeGroup&     >(groupRef), reverse);                               break;
        }
    }
    
    static void groupUnion(GA_EdgeGroup& group, const GA_Group& groupRef, const bool reverse = false)
    {
        switch (groupRef.classType())
        {
        case GA_GROUP_PRIMITIVE: groupUnion(group, static_cast<const GA_PrimitiveGroup&>(groupRef), reverse); break;
        case GA_GROUP_POINT:     groupUnion(group, static_cast<const GA_PointGroup&    >(groupRef), reverse); break;
        case GA_GROUP_VERTEX:    groupUnion(group, static_cast<const GA_VertexGroup&   >(groupRef), reverse); break;
        case GA_GROUP_EDGE:      groupUnion(group, static_cast<const GA_EdgeGroup&     >(groupRef), reverse); break;
        }
    }

    



    
    /////////////////////////// Same Type //////////////////////////

    
#define GFE_GroupUnion_FUNC_SPECILIZATION(GroupType)                         \
static void groupUnion(                                                      \
    GroupType& group, const GroupType& groupRef,                             \
    const bool reverse = false,                                              \
    const exint subscribeRatio = 64, const exint minGrainSize = 1024)        \
{                                                                            \
    group.combine(&groupRef);                                                \
    if (reverse)                                                             \
        group.toggleEntries();                                               \
}                                                                            \

    GFE_GroupUnion_FUNC_SPECILIZATION(GA_PrimitiveGroup);
    GFE_GroupUnion_FUNC_SPECILIZATION(GA_PointGroup);
    GFE_GroupUnion_FUNC_SPECILIZATION(GA_VertexGroup);
    
#undef GFE_GroupUnion_FUNC_SPECILIZATION
    
    static void groupUnion(GA_EdgeGroup& group, const GA_EdgeGroup& groupRef, const bool reverse = false)
    {
        group.combine(&groupRef);
        if (reverse)
            group.toggle();
    }



    // SYS_FORCE_INLINE static void groupUnion(GA_Group& group, const GA_VertexGroup* const groupRef)
    // {
    //     if (group.classType() == GA_GROUP_EDGE)
    //         groupUnion(static_cast<GA_EdgeGroup&>(group), groupRef);
    //     else
    //         static_cast<GA_ElementGroup&>(group).combine(groupRef);
    // }
    // 
    // SYS_FORCE_INLINE static void groupUnion(GA_Group& group, const GA_EdgeGroup* const groupRef)
    // {
    //     if (group.classType() == GA_GROUP_EDGE)
    //         groupUnion(static_cast<GA_EdgeGroup&>(group), groupRef);
    //     else
    //         static_cast<GA_ElementGroup&>(group).combine(groupRef);
    // }






    /////////////////////////// GA_PrimitiveGroup //////////////////////////


    
    static void groupUnion(
        GA_PrimitiveGroup& group, const GA_PointGroup& groupRef,
        const bool reverse = false,
        const exint subscribeRatio = 64, const exint minGrainSize = 1024)
    {
        // if (!reverse)
        // {
        //     group.combine(&groupRef);
        //     return;
        // }
        const GA_Detail& geo = group.getDetail();
        const GA_SplittableRange splittableRange(GA_Range(groupRef.getIndexMap(), &groupRef, reverse));
        UTparallelFor(splittableRange, [&geo, &group](const GA_SplittableRange& r)
        {
            GA_Offset start, end;
            for (GA_Iterator it(r); it.blockAdvance(start, end); )
            {
                for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                {
                    for (GA_Offset vtxoff_next = geo.pointVertex(elemoff); GFE_Type::isValidOffset(vtxoff_next); vtxoff_next = geo.vertexToNextVertex(vtxoff_next))
                    {
                        group.setElement(geo.vertexPrimitive(vtxoff_next), true);
                    }
                }
            }
        }, subscribeRatio, minGrainSize);
    }

    static void groupUnion(
        GA_PrimitiveGroup& group, const GA_VertexGroup& groupRef,
        const bool reverse = false,
        const exint subscribeRatio = 64, const exint minGrainSize = 1024)
    {
        const GA_Detail& geo = group.getDetail();
        const GA_SplittableRange splittableRange(GA_Range(groupRef.getIndexMap(), &groupRef, reverse));
        UTparallelFor(splittableRange, [&geo, &group](const GA_SplittableRange& r)
        {
            GA_Offset start, end;
            for (GA_Iterator it(r); it.blockAdvance(start, end); )
            {
                for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                {
                    group.setElement(geo.vertexPrimitive(elemoff), true);
                }
            }
        }, subscribeRatio, minGrainSize);
    }
    
    static void groupUnion(GA_PrimitiveGroup& group, const GA_EdgeGroup& groupRef, const bool reverse = false)
    {
        group.combine(&groupRef);
    }


    /////////////////////////// GA_PointGroup //////////////////////////


    static void groupUnion(
        GA_PointGroup& group, const GA_PrimitiveGroup& groupRef,
        const bool reverse = false,
        const exint subscribeRatio = 64, const exint minGrainSize = 1024)
    {
        const GA_Detail& geo = group.getDetail();
        const GA_PrimitiveList& primList = geo.getPrimitiveList();
        
        const GA_SplittableRange splittableRange(GA_Range(groupRef.getIndexMap(), &groupRef, reverse));
        UTparallelFor(splittableRange, [&geo, &group, &primList](const GA_SplittableRange& r)
        {
            GA_Offset start, end;
            for (GA_Iterator it(r); it.blockAdvance(start, end); )
            {
                for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                {
                    const GA_Size numvtx = primList.getVertexCount(elemoff);
                    for (GA_Size vtxpnum = 0; vtxpnum < numvtx; ++vtxpnum)
                    {
                        group.setElement(geo.vertexPoint(primList.getVertexOffset(elemoff, vtxpnum)), true);
                    }
                }
            }
        }, subscribeRatio, minGrainSize);
    }

    static void groupUnion(
        GA_PointGroup& group, const GA_VertexGroup& groupRef,
        const bool reverse = false,
        const exint subscribeRatio = 64, const exint minGrainSize = 1024)
    {
        const GA_Detail& geo = group.getDetail();
        const GA_SplittableRange splittableRange(GA_Range(groupRef.getIndexMap(), &groupRef, reverse));
        UTparallelFor(splittableRange, [&geo, &group](const GA_SplittableRange& r)
        {
            GA_Offset start, end;
            for (GA_Iterator it(r); it.blockAdvance(start, end); )
            {
                for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                {
                    group.setElement(geo.vertexPoint(elemoff), true);
                }
            }
        }, subscribeRatio, minGrainSize);
    }
    
    static void groupUnion(GA_PointGroup& group, const GA_EdgeGroup& groupRef, const bool reverse = false)
    {
        group.combine(&groupRef);
    }



    /////////////////////////// GA_VertexGroup //////////////////////////
    
    static void groupUnion(
        GA_VertexGroup& group, const GA_PrimitiveGroup& groupRef,
        const bool reverse = false,
        const exint subscribeRatio = 64, const exint minGrainSize = 1024)
    {
        if (!reverse)
        {
            group.combine(&groupRef);
            return;
        }
        const GA_Detail& geo = group.getDetail();
        const GA_PrimitiveList& primList = geo.getPrimitiveList();
        
        const GA_SplittableRange splittableRange(GA_Range(groupRef.getIndexMap(), &groupRef, reverse));
        UTparallelFor(splittableRange, [&group, &primList](const GA_SplittableRange& r)
        {
            GA_Offset start, end;
            for (GA_Iterator it(r); it.blockAdvance(start, end); )
            {
                for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                {
                    const GA_Size numvtx = primList.getVertexCount(elemoff);
                    for (GA_Size vtxpnum = 0; vtxpnum < numvtx; ++vtxpnum)
                    {
                        group.setElement(primList.getVertexOffset(elemoff, vtxpnum), true);
                    }
                }
            }
        }, subscribeRatio, minGrainSize);
    }

    static void groupUnion(
        GA_VertexGroup& group, const GA_PointGroup& groupRef,
        const bool reverse = false,
        const exint subscribeRatio = 64, const exint minGrainSize = 1024)
    {
        const GA_Detail& geo = group.getDetail();
        const GA_SplittableRange splittableRange(GA_Range(groupRef.getIndexMap(), &groupRef, reverse));
        UTparallelFor(splittableRange, [&geo, &group](const GA_SplittableRange& r)
        {
            GA_Offset start, end;
            for (GA_Iterator it(r); it.blockAdvance(start, end); )
            {
                for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                {
                    for (GA_Offset vtxoff_next = geo.pointVertex(elemoff); GFE_Type::isValidOffset(vtxoff_next); vtxoff_next = geo.vertexToNextVertex(vtxoff_next))
                    {
                        group.setElement(vtxoff_next, true);
                    }
                }
            }
        }, subscribeRatio, minGrainSize);
    }
    
    static void groupUnion(GA_VertexGroup& group, const GA_EdgeGroup& groupRef, const bool reverse = false)
    {
        group.combine(&groupRef);
    }



    
    /////////////////////////// GA_EdgeGroup //////////////////////////

    
    static void groupUnion(GA_EdgeGroup& group, const GA_PrimitiveGroup& groupRef, const bool reverse = false)
    {
        const GA_Detail& geo = groupRef.getDetail();
        
        const GA_PrimitiveList& primList = geo.getPrimitiveList();
        const GA_Topology& topo = geo.getTopology();
        const GA_ATITopology& vtxPointRef = *topo.getPointRef();

        GA_Offset start, end;
        for (GA_Iterator it(GA_Range(groupRef.getIndexMap(), &groupRef, reverse)); it.fullBlockAdvance(start, end); )
        {
            for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
            {
                const GA_Size numvtx = primList.getVertexCount(elemoff);

                const GA_Offset vtxoff0 = primList.getVertexOffset(elemoff, 0);
                GA_Offset vtxoff_prev = vtxPointRef.getLink(vtxoff0);
                GA_Offset vtxoff_next;
                for (GA_Size vtxpnum = 1; vtxpnum < numvtx; ++vtxpnum)
                {
                    vtxoff_next = vtxPointRef.getLink(primList.getVertexOffset(elemoff, vtxpnum));
                    group.add(vtxoff_prev, vtxoff_next);
                    vtxoff_prev = vtxoff_next;
                }
                if (primList.getClosedFlag(elemoff))
                {
                    vtxoff_next = vtxPointRef.getLink(vtxoff0);
                    group.add(vtxoff_prev, vtxoff_next);
                }
            }
        }
    }

    static void groupUnion(GA_EdgeGroup& group, const GA_PointGroup& groupRef, const bool reverse = false)
    {
        const GA_Detail& geo = groupRef.getDetail();
        
        const GA_PrimitiveList& primList = geo.getPrimitiveList();
        const GA_Topology& topo = geo.getTopology();
        const GA_ATITopology& vtxPointRef = *topo.getPointRef();
        
        GA_Offset start, end;
        for (GA_Iterator it(GA_Range(groupRef.getIndexMap(), &groupRef, reverse)); it.fullBlockAdvance(start, end); )
        {
            for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
            {
                GA_Offset pt_next;
                for (GA_Offset vtxoff_next = geo.pointVertex(elemoff); GFE_Type::isValidOffset(vtxoff_next); vtxoff_next = geo.vertexToNextVertex(vtxoff_next))
                {
                    const GA_Offset primoff = geo.vertexPrimitive(vtxoff_next);
                    const GA_Size numvtx = primList.getVertexCount(primoff);
                    const GA_Size vtxpnum = vertexPrimIndex(primList, primoff, vtxoff_next);

                    if (vtxpnum == 0)
                    {
                        if (primList.getClosedFlag(primoff))
                        {
                            pt_next = vtxPointRef.getLink(primList.getVertexOffset(primoff, numvtx-1));
                            group.add(elemoff, pt_next);
                        }
                    }
                    else
                    {
                        pt_next = vtxPointRef.getLink(primList.getVertexOffset(primoff, vtxpnum-1));
                        group.add(elemoff, pt_next);
                    }

                    const GA_Size vtxpnum_next = vtxpnum+1;
                    if (vtxpnum_next == numvtx)
                    {
                        if (primList.getClosedFlag(primoff))
                        {
                            pt_next = vtxPointRef.getLink(primList.getVertexOffset(primoff, 0));
                            group.add(elemoff, pt_next);
                        }
                    }
                    else
                    {
                        pt_next = vtxPointRef.getLink(primList.getVertexOffset(primoff, vtxpnum_next));
                        group.add(elemoff, pt_next);
                    }
                }
            }
        }
    }

    static void groupUnion(GA_EdgeGroup& group, const GA_VertexGroup& groupRef, const bool reverse = false)
    {
        const GA_Detail& geo = group.getDetail();
        const GA_ATITopology& vtxPointRef = *geo.getTopology().getPointRef();

        GA_Offset start, end;
        for (GA_Iterator it(GA_Range(groupRef.getIndexMap(), &groupRef, reverse)); it.fullBlockAdvance(start, end); )
        {
            for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
            {
                const GA_Offset dstpt = vertexPointDst(geo, elemoff);
                group.add(vtxPointRef.getLink(elemoff), dstpt);
            }
        }
    }

    SYS_FORCE_INLINE static void
        groupUnion(
            GA_EdgeGroup& group,
            const GA_VertexGroup* const groupRef,
            const GA_Attribute& dstptAttrib,
            const bool reverse = false
        )
    {
        if (!groupRef)
        {
            if (!reverse)
                group.addAll();
            return;
        }
        groupUnion(group, groupRef, dstptAttrib, reverse);
    }
    
    static void
        groupUnion(
            GA_EdgeGroup& group,
            const GA_VertexGroup& groupRef,
            const GA_Attribute& dstptAttrib,
            const bool reverse = false
        )
    {
        // dstptAttrib is in geo but not in geoRef
        
        const GA_ATITopology& vtxPointRef = *group.getDetail().getTopology().getPointRef();

        GA_PageHandleT<GA_Offset, GA_Offset, true, false, const GA_Attribute, const GA_ATINumeric, const GA_Detail> refAttrib_ph(&dstptAttrib);
        
        const GA_SplittableRange geoSplittableRange(GA_Range(groupRef.getIndexMap(), &groupRef, reverse));
        for (GA_PageIterator pit = geoSplittableRange.beginPages(); !pit.atEnd(); ++pit)
        {
            GA_Offset start, end;
            for (GA_Iterator it(pit.begin()); it.blockAdvance(start, end); )
            {
                refAttrib_ph.setPage(start);
                for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                {
                    group.add(vtxPointRef.getLink(elemoff), refAttrib_ph.value(elemoff));
                }
            }
        }
        // const GA_ROHandleT<GA_Offset> refAttrib_h(&dstptAttrib);
        // GA_Offset start, end;
        // for (GA_Iterator it(); it.fullBlockAdvance(start, end); )
        // {
        //     for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
        //     {
        //         group.add(vtxPointRef.getLink(elemoff), refAttrib_h.get(elemoff));
        //     }
        // }
    }
    
    static void
        groupUnion_topoAttrib(
            GA_Detail& geo,
            GA_Group& group,
            const GA_VertexGroup* const groupRef,
            const bool reverse = false
        )
    {
        if (!groupRef)
        {
            if (!reverse)
                group.addAll();
            return;
        }
        if (group.classType() == GA_GROUP_EDGE)
        {
            if (groupRef->classType() == GA_GROUP_VERTEX)
                groupUnion_topoAttrib(geo, static_cast<GA_EdgeGroup&>(group), static_cast<const GA_VertexGroup*>(groupRef));
            else
                groupUnion(static_cast<GA_EdgeGroup&>(group), groupRef);
        }
        else
        {
            //group->combine(groupRef);
            static_cast<GA_ElementGroup&>(group).combine(groupRef);
        }
    }

    



    //static void
    //    edgeGroupUnionParallel(
    //        GA_Detail* const geo,
    //        GA_EdgeGroup* const group,
    //        const GA_Group* const groupRef,
    //        const exint subscribeRatio = 64,
    //        const exint minGrainSize = 128
    //    )
    //{
    //    GA_GroupType groupTypeRef = groupRef->classType();
    //    switch (groupTypeRef)
    //    {
    //    case GA_GROUP_PRIMITIVE:
    //    {
    //        const GA_Topology& topo = geo->getTopology();
    //        const GA_ATITopology* const vtxPointRef = topo.getPointRef();

    //        const GA_SplittableRange geoSplittableRange(geo->getPrimitiveRange(static_cast<const GA_PrimitiveGroup*>(groupRef)));
    //        UTparallelFor(geoSplittableRange, [geo, group, vtxPointRef](const GA_SplittableRange& r)
    //            {
    //                GA_Offset start, end;
    //                for (GA_Iterator it(r); it.blockAdvance(start, end); )
    //                {
    //                    for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
    //                    {
    //                        const GA_OffsetListRef& vertices = geo->getPrimitiveVertexList(elemoff);
    //                        const GA_Size numvtx = vertices.size();

    //                        GA_Offset vtxoff_prev = vtxPointRef->getLink(vertices[0]);
    //                        GA_Offset vtxoff_next;
    //                        for (GA_Size vtxpnum = 1; vtxpnum < numvtx; ++vtxpnum)
    //                        {
    //                            vtxoff_next = vtxPointRef->getLink(vertices[vtxpnum]);
    //                            group->add(vtxoff_prev, vtxoff_next);
    //                            vtxoff_prev = vtxoff_next;
    //                        }
    //                        if (geo->getPrimitiveClosedFlag(elemoff))
    //                        {
    //                            vtxoff_next = vtxPointRef->getLink(vertices[0]);
    //                            group->add(vtxoff_prev, vtxoff_next);
    //                        }
    //                    }
    //                }
    //            }, subscribeRatio, minGrainSize);
    //    }
    //    break;
    //    case GA_GROUP_POINT:
    //    {
    //        const GA_SplittableRange geoSplittableRange(geo->getPointRange(static_cast<const GA_PointGroup*>(groupRef)));
    //        UTparallelFor(geoSplittableRange, [geo, group](const GA_SplittableRange& r)
    //            {
    //                GA_Offset start, end;
    //                for (GA_Iterator it(r); it.blockAdvance(start, end); )
    //                {
    //                    for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
    //                    {
    //                        GA_Offset pt_next;
    //                        for (GA_Offset vtxoff_next = geo->pointVertex(elemoff); vtxoff_next != GA_INVALID_OFFSET; vtxoff_next = geo->vertexToNextVertex(vtxoff_next))
    //                        {
    //                            const GA_Offset primoff = geo->vertexPrimitive(vtxoff_next);
    //                            const GA_Size numvtx = geo->getPrimitiveVertexCount(primoff);
    //                            const GA_Size vtxpnum = GFE_TopologyReference_Namespace::vertexPrimIndex(geo, primoff, vtxoff_next);

    //                            if (vtxpnum == 0)
    //                            {
    //                                if (geo->getPrimitiveClosedFlag(primoff))
    //                                {
    //                                    pt_next = geo->vertexPoint(geo->getPrimitiveVertexOffset(primoff, numvtx - 1));
    //                                    group->add(elemoff, pt_next);
    //                                }
    //                            }
    //                            else
    //                            {
    //                                pt_next = geo->vertexPoint(geo->getPrimitiveVertexOffset(primoff, vtxpnum - 1));
    //                                group->add(elemoff, pt_next);
    //                            }

    //                            const GA_Size vtxpnum_next = vtxpnum + 1;
    //                            if (vtxpnum_next == numvtx) {
    //                                if (geo->getPrimitiveClosedFlag(primoff))
    //                                {
    //                                    pt_next = geo->vertexPoint(geo->getPrimitiveVertexOffset(primoff, 0));
    //                                    group->add(elemoff, pt_next);
    //                                }
    //                            }
    //                            else
    //                            {
    //                                pt_next = geo->vertexPoint(geo->getPrimitiveVertexOffset(primoff, vtxpnum_next));
    //                                group->add(elemoff, pt_next);
    //                            }
    //                        }
    //                    }
    //                }
    //            }, subscribeRatio, minGrainSize);
    //    }
    //    break;
    //    case GA_GROUP_VERTEX:
    //    {
    //        const GA_SplittableRange geoSplittableRange(geo->getVertexRange(static_cast<const GA_VertexGroup*>(groupRef)));
    //        UTparallelFor(geoSplittableRange, [geo, group](const GA_SplittableRange& r)
    //            {
    //                GA_Offset start, end;
    //                for (GA_Iterator it(r); it.blockAdvance(start, end); )
    //                {
    //                    for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
    //                    {
    //                        const GA_Offset dstpt = GFE_TopologyReference_Namespace::vertexPointDst(geo, elemoff);
    //                        group->add(geo->vertexPoint(elemoff), dstpt);
    //                    }
    //                }
    //            }, subscribeRatio, minGrainSize);
    //    }
    //    break;
    //    case GA_GROUP_EDGE:
    //        group->combine(groupRef);
    //        break;
    //    default:
    //        break;
    //    }
    //}

private:
    
    SYS_FORCE_INLINE static GA_Size vertexPrimIndex(const GA_Detail& geo, const GA_Offset vtxoff)
    { return vertexPrimIndex(geo, geo.vertexPrimitive(vtxoff), vtxoff); }

    SYS_FORCE_INLINE static GA_Size vertexPrimIndex(const GA_Detail& geo, const GA_Offset primoff, const GA_Offset vtxoff)
    { return vertexPrimIndex(geo.getPrimitiveList(), primoff, vtxoff); }

    static GA_Size vertexPrimIndex(const GA_PrimitiveList& primList, const GA_Offset primoff, const GA_Offset vtxoff)
    {
        const GA_Size numvtx = primList.getVertexCount(primoff);
        for (GA_Size vtxpnum = 0; vtxpnum < numvtx; ++vtxpnum)
        {
            if (primList.getVertexOffset(primoff, vtxpnum) == vtxoff)
                return vtxpnum;
        }
        return GFE_INVALID_OFFSET;
    }

    static GA_Offset vertexPointDst(const GA_Detail& geo, const GA_Offset primoff, const GA_Size vtxpnum)
    {
        const GA_Size vtxpnum_next = vtxpnum+1;
        if (vtxpnum_next != geo.getPrimitiveVertexCount(primoff))
            return geo.vertexPoint(geo.getPrimitiveVertexOffset(primoff, vtxpnum_next));
        if (geo.getPrimitiveClosedFlag(primoff))
            return geo.vertexPoint(geo.getPrimitiveVertexOffset(primoff, 0));
        return GFE_INVALID_OFFSET;
    }

    SYS_FORCE_INLINE static GA_Offset vertexPointDst(const GA_Detail& geo, const GA_Offset vtxoff)
    {
        const GA_Offset primoff = geo.vertexPrimitive(vtxoff);
        return vertexPointDst(geo, primoff, vertexPrimIndex(geo, primoff, vtxoff));
    }
    


}; // End of class GFE_GroupUnion

#endif