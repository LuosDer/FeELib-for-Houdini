
#pragma once

#ifndef __GEO_FeE_Group_h__
#define __GEO_FeE_Group_h__

//#include <GA_FeE/GEO_FeE_Group.h>

#include <GA/GA_Detail.h>

#include <GA_FeE/GA_FeE_Group.h>
#include <GA_FeE/GA_FeE_TopologyReference.h>


namespace GEO_FeE_Group {




    //static void
    //    edgeGroupCombineParallel(
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




    static void
        edgeGroupCombine(
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
    //    edgeGroupCombineParallel(
    //        const GA_Detail* geo,
    //        GA_Group* group,
    //        const GA_Group* groupRef,
    //        const exint subscribeRatio = 64,
    //        const exint minGrainSize = 128
    //    )
    //{
    //    edgeGroupCombineParallel(geo, UTverify_cast<GA_EdgeGroup*>(group), groupRef, subscribeRatio, minGrainSize);
    //}

    SYS_FORCE_INLINE
    static void
        edgeGroupCombine(
            const GA_Detail* geo,
            GA_Group* group,
            const GA_Group* groupRef
        )
    {
        edgeGroupCombine(geo, UTverify_cast<GA_EdgeGroup*>(group), groupRef);
    }

    static void
        groupCombine(
            const GA_Detail* geo,
            GA_Group* group,
            const GA_Group* groupRef
        )
    {
        UT_ASSERT_P(geo);
        UT_ASSERT_P(group);
        if (group->classType() == GA_GROUP_EDGE)
        {
            edgeGroupCombine(geo, group, groupRef);
        }
        else
        {
            group->combine(groupRef);
            //static_cast<GA_ElementGroup*>(group)->combine(groupRef);
        }
    }













    static const GA_Group*
    groupPromote(
        GA_Detail* geo,
        const GA_Group* group,
        const GA_GroupType newType,
        const UT_StringHolder& newName
    )
    {
        UT_ASSERT_P(geo);
        UT_ASSERT_P(group);

        if (group->classType() == newType)
            return group;

        GA_GroupTable* groupTable = geo->getGroupTable(newType);
        if (!groupTable)
            return nullptr;

        GA_Group* newGroup = groupTable->newGroup(newName);
        groupCombine(geo, newGroup, group);

        return newGroup;
    }

    static const GA_Group*
        groupPromoteDetached(
            const GA_Detail* geo,
            const GA_Group* group,
            const GA_GroupType newType
        )
    {
        UT_ASSERT_P(geo);
        UT_ASSERT_P(group);

        const GA_GroupTable* groupTable = geo->getGroupTable(newType);
        if (!groupTable)
            return nullptr;

        GA_Group* newGroup = groupTable->newDetachedGroup();

        groupCombine(geo, newGroup, group);

        return newGroup;
    }

    static const GA_Group*
    groupFindPromoteDetached(
        const GA_Detail* geo,
        const GA_Group* group,
        const GA_GroupType newType
    )
    {
        UT_ASSERT_P(geo);
        UT_ASSERT_P(group);

        if (group->classType() == newType)
            return group;

        const GA_GroupTable* groupTable = geo->getGroupTable(newType);
        if (!groupTable)
            return nullptr;

        GA_Group* newGroup = groupTable->newDetachedGroup();
        groupCombine(geo, newGroup, group);

        return newGroup;
    }




    SYS_FORCE_INLINE
    static const GA_Group*
    groupPromote(
        GA_Detail* geo,
        const GA_Group* group,
        const GA_AttributeOwner newType,
        const UT_StringHolder& newName
    )
    {
        return groupPromote(geo, group, GA_FeE_Type::attributeOwner_groupType(newType), newName);
    }

    SYS_FORCE_INLINE
    static const GA_Group*
        groupPromoteDetached(
            const GA_Detail* geo,
            const GA_Group* group,
            const GA_AttributeOwner newType
        )
    {
        return groupPromoteDetached(geo, group, GA_FeE_Type::attributeOwner_groupType(newType));
    }

    SYS_FORCE_INLINE
    static const GA_Group*
        groupFindPromoteDetached(
            const GA_Detail* geo,
            const GA_Group* group,
            const GA_AttributeOwner newType
        )
    {
        return groupFindPromoteDetached(geo, group, GA_FeE_Type::attributeOwner_groupType(newType));
    }




    SYS_FORCE_INLINE
    static const GA_Group*
    groupPromote(
        GA_Detail* geo,
        GA_Group* group,
        const GA_GroupType newType,
        const UT_StringHolder& newName,
        const bool delOriginal = false
    )
    {
        const GA_Group* newGroup = groupPromote(geo, static_cast<const GA_Group*>(group), newType, newName);
        if (delOriginal)
            geo->destroyGroup(group);
        return newGroup;
    }

    SYS_FORCE_INLINE
    static const GA_Group*
    groupPromoteDetached(
        GA_Detail* geo,
        GA_Group* group,
        const GA_GroupType newType,
        const bool delOriginal = false
    )
    {
        const GA_Group* newGroup = groupPromoteDetached(geo, group, newType);
        if (delOriginal)
            geo->destroyGroup(group);
        return newGroup;
    }

    SYS_FORCE_INLINE
    static const GA_Group*
    groupFindPromoteDetached(
        GA_Detail* geo,
        GA_Group* group,
        const GA_GroupType newType,
        const bool delOriginal = false
    )
    {
        const GA_Group* newGroup = groupFindPromoteDetached(geo, group, newType);
        if (delOriginal)
            geo->destroyGroup(group);
        return newGroup;
    }





    SYS_FORCE_INLINE
        static const GA_Group*
        groupPromotePrimitive(
            GA_Detail* geo,
            const GA_Group* group,
            const UT_StringHolder& newName
        )
    {
        return groupPromote(geo, group, GA_GROUP_PRIMITIVE, newName);
    }

    SYS_FORCE_INLINE
        static const GA_Group*
        groupPromotePoint(
            GA_Detail* geo,
            const GA_Group* group,
            const UT_StringHolder& newName
        )
    {
        return groupPromote(geo, group, GA_GROUP_POINT, newName);
    }

    SYS_FORCE_INLINE
        static const GA_Group*
        groupPromoteVertex(
            GA_Detail* geo,
            const GA_Group* group,
            const UT_StringHolder& newName
        )
    {
        return groupPromote(geo, group, GA_GROUP_VERTEX, newName);
    }

    SYS_FORCE_INLINE
        static const GA_Group*
        groupPromoteEdge(
            GA_Detail* geo,
            const GA_Group* group,
            const UT_StringHolder& newName
        )
    {
        return groupPromote(geo, group, GA_GROUP_EDGE, newName);
    }






    SYS_FORCE_INLINE
    static const GA_Group*
        groupPromotePrimitiveDetached(
            GA_Detail* geo,
            GA_Group* group,
            const bool delOriginal = false
        )
    {
        return groupPromoteDetached(geo, group, GA_GROUP_PRIMITIVE, delOriginal);
    }

    SYS_FORCE_INLINE
    static const GA_Group*
        groupPromotePointDetached(
            GA_Detail* geo,
            GA_Group* group,
            const bool delOriginal = false
        )
    {
        return groupPromoteDetached(geo, group, GA_GROUP_POINT, delOriginal);
    }

    SYS_FORCE_INLINE
        static const GA_Group*
        groupPromoteVertexDetached(
            GA_Detail* geo,
            GA_Group* group,
            const bool delOriginal = false
        )
    {
        return groupPromoteDetached(geo, group, GA_GROUP_VERTEX, delOriginal);
    }

    SYS_FORCE_INLINE
        static const GA_Group*
        groupPromoteEdgeDetached(
            GA_Detail* geo,
            GA_Group* group,
            const bool delOriginal = false
        )
    {
        return groupPromoteDetached(geo, group, GA_GROUP_EDGE, delOriginal);
    }




    SYS_FORCE_INLINE
        static const GA_Group*
        groupPromotePrimitiveDetached(
            const GA_Detail* geo,
            const GA_Group* group
        )
    {
        return groupPromoteDetached(geo, group, GA_GROUP_PRIMITIVE);
    }

    SYS_FORCE_INLINE
        static const GA_Group*
        groupPromotePointDetached(
            const GA_Detail* geo,
            const GA_Group* group
        )
    {
        return groupPromoteDetached(geo, group, GA_GROUP_POINT);
    }

    SYS_FORCE_INLINE
        static const GA_Group*
        groupPromoteVertexDetached(
            const GA_Detail* geo,
            const GA_Group* group
        )
    {
        return groupPromoteDetached(geo, group, GA_GROUP_VERTEX);
    }

    SYS_FORCE_INLINE
        static const GA_Group*
        groupPromoteEdgeDetached(
            const GA_Detail* geo,
            const GA_Group* group
        )
    {
        return groupPromoteDetached(geo, group, GA_GROUP_EDGE);
    }




    SYS_FORCE_INLINE
        static const GA_Group*
        groupFindPromotePrimitiveDetached(
            const GA_Detail* geo,
            const GA_Group* group
        )
    {
        return groupFindPromoteDetached(geo, group, GA_GROUP_PRIMITIVE);
    }

    SYS_FORCE_INLINE
        static const GA_Group*
        groupFindPromotePointDetached(
            const GA_Detail* geo,
            const GA_Group* group
        )
    {
        return groupFindPromoteDetached(geo, group, GA_GROUP_POINT);
    }

    SYS_FORCE_INLINE
        static const GA_Group*
        groupFindPromoteVertexDetached(
            const GA_Detail* geo,
            const GA_Group* group
        )
    {
        return groupFindPromoteDetached(geo, group, GA_GROUP_VERTEX);
    }

    SYS_FORCE_INLINE
        static const GA_Group*
        groupFindPromoteEdgeDetached(
            const GA_Detail* geo,
            const GA_Group* group
        )
    {
        return groupFindPromoteDetached(geo, group, GA_GROUP_EDGE);
    }













    SYS_FORCE_INLINE
        static const GA_ElementGroup*
        elementGroupFindPromoteDetached(
            const GA_Detail* geo,
            const GA_Group* group,
            const GA_GroupType newType
        )
    {
        return static_cast<const GA_ElementGroup*>(groupFindPromoteDetached(geo, group, newType));
    }

    SYS_FORCE_INLINE
        static const GA_ElementGroup*
        elementGroupFindPromotePrimitiveDetached(
            const GA_Detail* geo,
            const GA_Group* group
        )
    {
        return elementGroupFindPromoteDetached(geo, group, GA_GROUP_PRIMITIVE);
    }

    SYS_FORCE_INLINE
        static const GA_ElementGroup*
        elementGroupFindPromotePointDetached(
            const GA_Detail* geo,
            const GA_Group* group
        )
    {
        return elementGroupFindPromoteDetached(geo, group, GA_GROUP_POINT);
    }

    SYS_FORCE_INLINE
        static const GA_ElementGroup*
        elementGroupFindPromoteVertexDetached(
            const GA_Detail* geo,
            const GA_Group* group
        )
    {
        return elementGroupFindPromoteDetached(geo, group, GA_GROUP_VERTEX);
    }


    SYS_FORCE_INLINE
    static const GA_ElementGroup*
        elementGroupFindPromoteDetached(
            GA_Detail* geo,
            GA_Group* group,
            const GA_GroupType newType,
            const bool delOriginal = false
        )
    {
        return static_cast<const GA_ElementGroup*>(groupFindPromoteDetached(geo, group, newType, delOriginal));
    }

    SYS_FORCE_INLINE
        static const GA_ElementGroup*
        elementGroupFindPromotePrimitiveDetached(
            GA_Detail* geo,
            GA_Group* group,
            const bool delOriginal = false
        )
    {
        return elementGroupFindPromoteDetached(geo, group, GA_GROUP_PRIMITIVE, delOriginal);
    }

    SYS_FORCE_INLINE
        static const GA_ElementGroup*
        elementGroupFindPromotePointDetached(
            GA_Detail* geo,
            GA_Group* group,
            const bool delOriginal = false
        )
    {
        return elementGroupFindPromoteDetached(geo, group, GA_GROUP_POINT, delOriginal);
    }

    SYS_FORCE_INLINE
        static const GA_ElementGroup*
        elementGroupFindPromoteVertexDetached(
            GA_Detail* geo,
            GA_Group* group,
            const bool delOriginal = false
        )
    {
        return elementGroupFindPromoteDetached(geo, group, GA_GROUP_VERTEX, delOriginal);
    }





    SYS_FORCE_INLINE
        static const GA_ElementGroup*
        elementGroupPromoteDetached(
            const GA_Detail* geo,
            const GA_Group* group,
            const GA_GroupType newType
        )
    {
        return static_cast<const GA_ElementGroup*>(groupPromoteDetached(geo, group, newType));
    }

    SYS_FORCE_INLINE
        static const GA_ElementGroup*
        elementGroupPromotePrimitiveDetached(
            const GA_Detail* geo,
            const GA_Group* group
        )
    {
        return elementGroupPromoteDetached(geo, group, GA_GROUP_PRIMITIVE);
    }

    SYS_FORCE_INLINE
        static const GA_ElementGroup*
        elementGroupPromotePointDetached(
            const GA_Detail* geo,
            const GA_Group* group
        )
    {
        return elementGroupPromoteDetached(geo, group, GA_GROUP_POINT);
    }

    SYS_FORCE_INLINE
        static const GA_ElementGroup*
        elementGroupPromoteVertexDetached(
            const GA_Detail* geo,
            const GA_Group* group
        )
    {
        return elementGroupPromoteDetached(geo, group, GA_GROUP_VERTEX);
    }










    //GA_Range geo0Range = GA_FeE_Group::groupPromoteRange(geo, geo0Group, groupType);
    //using GA_ElementGroupUPtr = ::std::unique_ptr<GA_ElementGroup, ::std::default_delete<GA_ElementGroup>>;

    static GA_Range
        getRangeByAnyGroup(
            const GA_Detail* geo,
            const GA_Group* group,
            const GA_GroupType groupType
        )
    {
        UT_ASSERT_P(geo);

        if (groupType != GA_GROUP_PRIMITIVE && groupType != GA_GROUP_POINT && groupType != GA_GROUP_VERTEX)
            return GA_Range();

        if (!group)
            return GA_Range(geo->getIndexMap(GA_FeE_Type::attributeOwner_groupType(groupType)), nullptr);

        if (!group->isElementGroup())
            return GA_Range();

        return GA_Range(geo->getIndexMap(GA_FeE_Type::attributeOwner_groupType(groupType)), elementGroupPromoteDetached(geo, group, groupType));
    }



    SYS_FORCE_INLINE
    static GA_Range
        getRangeByAnyGroup(
            const GA_Detail* geo,
            const GA_Group* group,
            const GA_AttributeOwner attribOwner
        )
    {
        return getRangeByAnyGroup(geo, group, GA_FeE_Type::attributeOwner_groupType(attribOwner));
    }


    SYS_FORCE_INLINE
    static GA_SplittableRange
        getSplittableRangeByAnyGroup(
            const GA_Detail* geo,
            const GA_Group* group,
            const GA_GroupType groupType
        )
    {
        return GA_SplittableRange(getRangeByAnyGroup(geo, group, groupType));
    }

    SYS_FORCE_INLINE
    static GA_SplittableRange
        getSplittableRangeByAnyGroup(
            const GA_Detail* geo,
            const GA_Group* group,
            const GA_AttributeOwner attribOwner
        )
    {
        return GA_SplittableRange(getRangeByAnyGroup(geo, group, attribOwner));
    }









} // End of namespace GEO_FeE_Group

#endif
