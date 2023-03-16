
#pragma once

#ifndef __GFE_GroupUnion_h__
#define __GFE_GroupUnion_h__

//#include "GFE/GFE_GroupUnion.h"

#include "GA/GA_Detail.h"

#include "GFE/GFE_TopologyReference.h"



namespace GFE_GroupUnion {





    static void
        groupUnion(
            GA_EdgeGroup* const group,
            const GA_PrimitiveGroup* const groupRef
        )
    {
        const GA_Detail* const geo = &groupRef->getDetail();

        const GA_Topology& topo = geo->getTopology();
        const GA_ATITopology* const vtxPointRef = topo.getPointRef();

        const GA_Range range = geo->getPrimitiveRange(static_cast<const GA_PrimitiveGroup*>(groupRef));
        GA_Offset start, end;
        for (GA_Iterator it(range); it.fullBlockAdvance(start, end); )
        {
            for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
            {
                const GA_OffsetListRef& vertices = geo->getPrimitiveVertexList(elemoff);
                const GA_Size numvtx = vertices.size();

                GA_Offset vtxoff_prev = vtxPointRef->getLink(vertices[0]);
                GA_Offset vtxoff_next;
                for (GA_Size vtxpnum = 1; vtxpnum < numvtx; ++vtxpnum)
                {
                    vtxoff_next = vtxPointRef->getLink(vertices[vtxpnum]);
                    group->add(vtxoff_prev, vtxoff_next);
                    vtxoff_prev = vtxoff_next;
                }
                if (geo->getPrimitiveClosedFlag(elemoff))
                {
                    vtxoff_next = vtxPointRef->getLink(vertices[0]);
                    group->add(vtxoff_prev, vtxoff_next);
                }
            }
        }
    }


    static void
        groupUnion(
            GA_EdgeGroup* const group,
            const GA_PointGroup* const groupRef
        )
    {
        const GA_Detail* const geo = &groupRef->getDetail();
        const GA_Range range = geo->getPointRange(static_cast<const GA_PointGroup*>(groupRef));
        GA_Offset start, end;
        for (GA_Iterator it(range); it.fullBlockAdvance(start, end); )
        {
            for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
            {
                GA_Offset pt_next;
                for (GA_Offset vtxoff_next = geo->pointVertex(elemoff); vtxoff_next != GA_INVALID_OFFSET; vtxoff_next = geo->vertexToNextVertex(vtxoff_next))
                {
                    const GA_Offset primoff = geo->vertexPrimitive(vtxoff_next);
                    const GA_Size numvtx = geo->getPrimitiveVertexCount(primoff);
                    const GA_Size vtxpnum = geo->getPrimitiveVertexList(primoff).find(vtxoff_next);

                    if (vtxpnum == 0)
                    {
                        if (geo->getPrimitiveClosedFlag(primoff))
                        {
                            pt_next = geo->vertexPoint(geo->getPrimitiveVertexOffset(primoff, numvtx - 1));
                            group->add(elemoff, pt_next);
                        }
                    }
                    else
                    {
                        pt_next = geo->vertexPoint(geo->getPrimitiveVertexOffset(primoff, vtxpnum - 1));
                        group->add(elemoff, pt_next);
                    }

                    const GA_Size vtxpnum_next = vtxpnum + 1;
                    if (vtxpnum_next == numvtx) {
                        if (geo->getPrimitiveClosedFlag(primoff))
                        {
                            pt_next = geo->vertexPoint(geo->getPrimitiveVertexOffset(primoff, 0));
                            group->add(elemoff, pt_next);
                        }
                    }
                    else
                    {
                        pt_next = geo->vertexPoint(geo->getPrimitiveVertexOffset(primoff, vtxpnum_next));
                        group->add(elemoff, pt_next);
                    }
                }
            }
        }
    }

    static void
        groupUnion(
            GA_EdgeGroup* const group,
            const GA_VertexGroup* const groupRef
        )
    {
        const GA_Detail* const geo = &group->getDetail();
        const GA_Detail* const geoRef = &group->getDetail();

        const GA_Range range = geoRef->getVertexRange(static_cast<const GA_VertexGroup*>(groupRef));
        GA_Offset start, end;
        for (GA_Iterator it(range); it.fullBlockAdvance(start, end); )
        {
            for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
            {
                const GA_Offset dstpt = GFE_TopologyReference::vertexPointDst(geo, elemoff);
                group->add(geo->vertexPoint(elemoff), dstpt);
            }
        }
    }

    static void
        groupUnion(
            GA_EdgeGroup* const group,
            const GA_VertexGroup* const groupRef,
            const GA_Attribute* const refAttrib
        )
    {
        // refAttrib is in geo but not in geoRef
        const GA_Detail* const geo = &group->getDetail();
        const GA_Detail* const geoRef = &groupRef->getDetail();

        const GA_ROHandleT<GA_Offset> refAttrib_h(refAttrib);

        const GA_Range range = geoRef->getVertexRange(static_cast<const GA_VertexGroup*>(groupRef));
        GA_Offset start, end;
        for (GA_Iterator it(range); it.fullBlockAdvance(start, end); )
        {
            for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
            {
                group->add(geo->vertexPoint(elemoff), refAttrib_h.get(elemoff));
            }
        }
    }

    static void
        groupUnion_topoAttrib(
            GA_Detail* const geo,
            GA_EdgeGroup* const group,
            const GA_VertexGroup* const groupRef
        )
    {
        const GA_Attribute* const refAttrib = GFE_TopologyReference::addAttribVertexPointDst(geo);
        groupUnion(group, groupRef, refAttrib);
    }


    static void
        groupUnion(
            GA_EdgeGroup* const group,
            const GA_EdgeGroup* const groupRef
        )
    {
        group->combine(groupRef);
    }

    static void
        groupUnion(
            GA_EdgeGroup* const group,
            const GA_Group* const groupRef
        )
    {
        UT_ASSERT_P(group);

        switch (groupRef->classType())
        {
        case GA_GROUP_PRIMITIVE:
            groupUnion(group, static_cast<const GA_PrimitiveGroup*>(groupRef));
        break;
        case GA_GROUP_POINT:
            groupUnion(group, static_cast<const GA_PointGroup*>(groupRef));
        break;
        case GA_GROUP_VERTEX:
            groupUnion(group, static_cast<const GA_VertexGroup*>(groupRef));
        break;
        case GA_GROUP_EDGE:
            //*group = *static_cast<const GA_EdgeGroup*>(groupRef);
            //*group += *static_cast<const GA_EdgeGroup*>(groupRef);
            group->combine(groupRef);
            break;
        default:
            break;
        }
    }

    SYS_FORCE_INLINE
        static void
        groupUnion(
            GA_Group* const group,
            const GA_VertexGroup* const groupRef
        )
    {
        UT_ASSERT_P(group);
        if (group->classType() == GA_GROUP_EDGE)
        {
            groupUnion(static_cast<GA_EdgeGroup*>(group), groupRef);
        }
        else
        {
            //group->combine(groupRef);
            static_cast<GA_ElementGroup*>(group)->combine(groupRef);
        }
    }

    SYS_FORCE_INLINE
        static void
        groupUnion_topoAttrib(
            GA_Detail* const geo,
            GA_Group* const group,
            const GA_VertexGroup* const groupRef
        )
    {
        UT_ASSERT_P(group);
        if (group->classType() == GA_GROUP_EDGE)
        {
            if (groupRef->classType() == GA_GROUP_VERTEX)
            {
                groupUnion_topoAttrib(geo, static_cast<GA_EdgeGroup*>(group), static_cast<const GA_VertexGroup*>(groupRef));
            }
            else
            {
                groupUnion(static_cast<GA_EdgeGroup*>(group), groupRef);
            }
        }
        else
        {
            //group->combine(groupRef);
            static_cast<GA_ElementGroup*>(group)->combine(groupRef);
        }
    }





    //SYS_FORCE_INLINE
    //    static void
    //    edgeGroupUnionParallel(
    //        const GA_Detail* const geo,
    //        GA_Group* const group,
    //        const GA_Group* const groupRef,
    //        const exint subscribeRatio = 64,
    //        const exint minGrainSize = 128
    //    )
    //{
    //    edgeGroupUnionParallel(geo, UTverify_cast<GA_EdgeGroup*>(group), groupRef, subscribeRatio, minGrainSize);
    //}

    SYS_FORCE_INLINE
        static void
        edgeGroupUnion(
            GA_Group* const group,
            const GA_Group* const groupRef
        )
    {
        groupUnion(static_cast<GA_EdgeGroup*>(group), groupRef);
    }

    SYS_FORCE_INLINE
        static void
        groupUnion(
            GA_Group* const group,
            const GA_Group* const groupRef
        )
    {
        UT_ASSERT_P(group);
        if (group->classType() == GA_GROUP_EDGE)
        {
            groupUnion(static_cast<GA_EdgeGroup*>(group), groupRef);
        }
        else
        {
            //group->combine(groupRef);
            static_cast<GA_ElementGroup*>(group)->combine(groupRef);
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
    //                            const GA_Size vtxpnum = GFE_TopologyReference::vertexPrimIndex(geo, primoff, vtxoff_next);

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
    //                        const GA_Offset dstpt = GFE_TopologyReference::vertexPointDst(geo, elemoff);
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




} // End of namespace GFE_GroupUnion

#endif