
#pragma once

#ifndef __GA_FeE_GroupUnion_h__
#define __GA_FeE_GroupUnion_h__

//#include <GA_FeE/GA_FeE_GroupUnion.h>

#include <GA/GA_Detail.h>

#include <GA_FeE/GA_FeE_TopologyReference.h>



namespace GA_FeE_GroupUnion {

    



    static void
        edgeGroupUnion(
            const GA_Detail* geo,
            GA_EdgeGroup* group,
            const GA_Group* groupRef
        )
    {
        UT_ASSERT_P(geo);
        UT_ASSERT_P(group);

        GA_GroupType groupTypeRef = groupRef->classType();
        switch (groupTypeRef)
        {
        case GA_GROUP_PRIMITIVE:
        {
            const GA_Topology& topo = geo->getTopology();
            const GA_ATITopology* vtxPointRef = topo.getPointRef();

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
        break;
        case GA_GROUP_POINT:
        {
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
                        const GA_Size vtxpnum = GA_FeE_TopologyReference::vertexPrimIndex(geo, primoff, vtxoff_next);

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
        break;
        case GA_GROUP_VERTEX:
        {
            const GA_Range range = geo->getVertexRange(static_cast<const GA_VertexGroup*>(groupRef));
            GA_Offset start, end;
            for (GA_Iterator it(range); it.fullBlockAdvance(start, end); )
            {
                for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                {
                    const GA_Offset dstpt = GA_FeE_TopologyReference::vertexPointDst(geo, elemoff);
                    group->add(geo->vertexPoint(elemoff), dstpt);
                }
            }
        }
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







    //SYS_FORCE_INLINE
    //    static void
    //    edgeGroupUnionParallel(
    //        const GA_Detail* geo,
    //        GA_Group* group,
    //        const GA_Group* groupRef,
    //        const exint subscribeRatio = 64,
    //        const exint minGrainSize = 128
    //    )
    //{
    //    edgeGroupUnionParallel(geo, UTverify_cast<GA_EdgeGroup*>(group), groupRef, subscribeRatio, minGrainSize);
    //}

    SYS_FORCE_INLINE
        static void
        edgeGroupUnion(
            const GA_Detail* geo,
            GA_Group* group,
            const GA_Group* groupRef
        )
    {
        edgeGroupUnion(geo, UTverify_cast<GA_EdgeGroup*>(group), groupRef);
    }

    SYS_FORCE_INLINE
        static void
        groupUnion(
            const GA_Detail* geo,
            GA_Group* group,
            const GA_Group* groupRef
        )
    {
        UT_ASSERT_P(geo);
        UT_ASSERT_P(group);
        if (group->classType() == GA_GROUP_EDGE)
        {
            edgeGroupUnion(geo, group, groupRef);
        }
        else
        {
            //group->combine(groupRef);
            UTverify_cast<GA_ElementGroup*>(group)->combine(groupRef);
        }
    }







    //static void
    //    edgeGroupUnionParallel(
    //        GA_Detail* geo,
    //        GA_EdgeGroup* group,
    //        const GA_Group* groupRef,
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
    //        const GA_ATITopology* vtxPointRef = topo.getPointRef();

    //        const GA_SplittableRange geoSplittableRange(geo->getPrimitiveRange(static_cast<const GA_PrimitiveGroup*>(groupRef)));
    //        UTparallelFor(geoSplittableRange, [&geo, &group, &vtxPointRef](const GA_SplittableRange& r)
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
    //        UTparallelFor(geoSplittableRange, [&geo, &group](const GA_SplittableRange& r)
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
    //                            const GA_Size vtxpnum = GA_FeE_TopologyReference::vertexPrimIndex(geo, primoff, vtxoff_next);

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
    //        UTparallelFor(geoSplittableRange, [&geo, &group](const GA_SplittableRange& r)
    //            {
    //                GA_Offset start, end;
    //                for (GA_Iterator it(r); it.blockAdvance(start, end); )
    //                {
    //                    for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
    //                    {
    //                        const GA_Offset dstpt = GA_FeE_TopologyReference::vertexPointDst(geo, elemoff);
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




} // End of namespace GA_FeE_GroupUnion

#endif
