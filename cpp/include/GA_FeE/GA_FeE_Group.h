
#pragma once

#ifndef __GA_FeE_Group_h__
#define __GA_FeE_Group_h__

//#include <GA_FeE/GA_FeE_Group.h>

//#include <GA/GA_Detail.h>
#include <GEO/GEO_Detail.h>


#include <GU/GU_EdgeGroup.h>

#include <GA_FeE/GA_FeE_TopologyReference.h>

namespace GA_FeE_Group {




static GA_GroupType
    attributeOwner_groupType(
        const GA_AttributeOwner& attribOwner
    )
{
    switch (attribOwner)
    {
    case GA_ATTRIB_PRIMITIVE:       return GA_GROUP_PRIMITIVE; break;
    case GA_ATTRIB_POINT:           return GA_GROUP_POINT;     break;
    case GA_ATTRIB_VERTEX:          return GA_GROUP_VERTEX;    break;
    case GA_ATTRIB_OWNER_N:         return GA_GROUP_N;         break;
    }
    UT_ASSERT_MSG(0, "Unhandled Group Type!");
    return GA_GROUP_INVALID;
}

static GA_AttributeOwner
    attributeOwner_groupType(
        const GA_GroupType& groupType
    )
{
    switch (groupType)
    {
    case GA_GROUP_PRIMITIVE:       return GA_ATTRIB_PRIMITIVE; break;
    case GA_GROUP_POINT:           return GA_ATTRIB_POINT;     break;
    case GA_GROUP_VERTEX:          return GA_ATTRIB_VERTEX;    break;
    case GA_GROUP_EDGE:            return GA_ATTRIB_VERTEX;    break;
    case GA_GROUP_N:               return GA_ATTRIB_OWNER_N;   break;
    }
    UT_ASSERT_MSG(0, "Unhandled Group Type!");
    return GA_ATTRIB_INVALID;
}


static bool
isValid(
    const GA_AttributeOwner& attribOwner
)
{
    return attribOwner == GA_ATTRIB_PRIMITIVE ||
           attribOwner == GA_ATTRIB_POINT     ||
           attribOwner == GA_ATTRIB_VERTEX    ||
           attribOwner == GA_ATTRIB_GLOBAL    ;
}

static bool
isValid(
    const GA_GroupType& groupType
)
{
    return groupType == GA_GROUP_PRIMITIVE ||
           groupType == GA_GROUP_POINT     ||
           groupType == GA_GROUP_VERTEX    ||
           groupType == GA_GROUP_EDGE;
}

static bool
isInvalid(
    const GA_AttributeOwner& attribOwner
)
{
    return attribOwner != GA_ATTRIB_PRIMITIVE &&
           attribOwner != GA_ATTRIB_POINT     &&
           attribOwner != GA_ATTRIB_VERTEX    &&
           attribOwner != GA_ATTRIB_GLOBAL;
}

static bool
isInvalid(
    const GA_GroupType& groupType
)
{
    return groupType != GA_GROUP_PRIMITIVE &&
           groupType != GA_GROUP_POINT     &&
           groupType != GA_GROUP_VERTEX    &&
           groupType != GA_GROUP_EDGE;
}

//const GA_ElementGroup* geo0Group = GA_FeE_Group::parseGroupDetached(cookparms, geo, groupType, sopparms.getGroup(), gop);


static GA_Group*
findGroup(
    GA_Detail* geo,
    const GA_GroupType groupType,
    const UT_StringHolder& groupName
)
{
    switch (groupType)
    {
    case GA_GROUP_PRIMITIVE:
        return geo->primitiveGroups().find(groupName);
    break;
    case GA_GROUP_POINT:
        return geo->pointGroups().find(groupName);
    break;
    case GA_GROUP_VERTEX:
        return geo->vertexGroups().find(groupName);
    break;
    case GA_GROUP_EDGE:
        return geo->edgeGroups().find(groupName);
        break;
    default:
        break;
    }
    return nullptr;

    //if (groupType == GA_GROUP_EDGE)
    //{
    //    return geo->edgeGroups().find(groupName);
    //}
    //else
    //{
    //    return geo->findElementGroup(attributeOwner_groupType(groupType), groupName);
    //}
}

static GA_ElementGroup*
findElementGroup(
    GA_Detail* geo,
    const GA_GroupType groupType,
    const UT_StringHolder& groupName
)
{
    if(groupType == GA_GROUP_EDGE)
        return nullptr;
    return geo->findElementGroup(attributeOwner_groupType(groupType), groupName);
}

static bool
isEmpty(
    const GA_Group* group,
    const GA_GroupType groupType
)
{
    if (!group)
        return true;
    if (groupType == GA_GROUP_EDGE)
    {
        return static_cast<const GA_EdgeGroup*>(group)->isEmpty();
    }
    else
    {
        return static_cast<const GA_ElementGroup*>(group)->isEmpty();
    }
}

static bool
isEmpty(
    GA_Group* group,
    const GA_GroupType groupType
)
{
    if (groupType == GA_GROUP_EDGE)
    {
        return static_cast<GA_EdgeGroup*>(group)->isEmpty();
    }
    else
    {
        return static_cast<GA_ElementGroup*>(group)->isEmpty();
    }
}

static void
bumpDataId(
    GA_Group* group
)
{
    if (group->classType() == GA_GROUP_EDGE)
    {
        static_cast<GA_EdgeGroup*>(group)->bumpDataId();
    }
    else
    {
        static_cast<GA_ElementGroup*>(group)->bumpDataId();
    }
}







static void
edgeGroupCombineParallel(
    GA_Detail* geo,
    GA_EdgeGroup* group,
    const GA_Group* groupRef,
    const exint subscribeRatio = 64,
    const exint minGrainSize = 128
)
{
    GA_GroupType groupTypeRef = groupRef->classType();
    switch (groupTypeRef)
    {
    case GA_GROUP_PRIMITIVE:
    {
        const GA_Topology& topo = geo->getTopology();
        const GA_ATITopology* vtxPointRef = topo.getPointRef();

        const GA_SplittableRange geoSplittableRange(geo->getPrimitiveRange(static_cast<const GA_PrimitiveGroup*>(groupRef)));
        UTparallelFor(geoSplittableRange, [&geo, &group, &vtxPointRef](const GA_SplittableRange& r)
        {
            GA_Offset start, end;
            for (GA_Iterator it(r); it.blockAdvance(start, end); )
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
        }, subscribeRatio, minGrainSize);
    }
    break;
    case GA_GROUP_POINT:
    {
        const GA_SplittableRange geoSplittableRange(geo->getPointRange(static_cast<const GA_PointGroup*>(groupRef)));
        UTparallelFor(geoSplittableRange, [&geo, &group](const GA_SplittableRange& r)
        {
            GA_Offset start, end;
            for (GA_Iterator it(r); it.blockAdvance(start, end); )
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
        }, subscribeRatio, minGrainSize);
    }
    break;
    case GA_GROUP_VERTEX:
    {
        const GA_SplittableRange geoSplittableRange(geo->getVertexRange(static_cast<const GA_VertexGroup*>(groupRef)));
        UTparallelFor(geoSplittableRange, [&geo, &group](const GA_SplittableRange& r)
        {
            GA_Offset start, end;
            for (GA_Iterator it(r); it.blockAdvance(start, end); )
            {
                for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                {
                    const GA_Offset dstpt = GA_FeE_TopologyReference::vertexPointDst(geo, elemoff);
                    group->add(geo->vertexPoint(elemoff), dstpt);
                }
            }
        }, subscribeRatio, minGrainSize);
    }
    break;
    case GA_GROUP_EDGE:
        group->combine(groupRef);
        break;
    default:
        break;
    }
}




static void
edgeGroupCombine(
    GA_Detail* geo,
    GA_EdgeGroup* group,
    const GA_Group* groupRef
)
{
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

static void
edgeGroupCombine(
    GA_Detail* geo,
    GA_Group* group,
    const GA_Group* groupRef,
    const exint subscribeRatio = 64,
    const exint minGrainSize = 128
)
{
    edgeGroupCombine(geo, static_cast<GA_EdgeGroup*>(group), groupRef, subscribeRatio, minGrainSize);
}


static void
groupCombine(
    GA_Detail* geo,
    GA_Group* group,
    const GA_Group* groupRef
)
{
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
parseGroupDetached(
    const SOP_NodeVerb::CookParms& cookparms,
    const GEO_Detail* geo,
    const GA_GroupType& groupType,
    const UT_StringHolder& groupName,
    GOP_Manager& gop
)
{
    if (!groupName.length())
        return nullptr;
    
    if (!groupName.isstring())
    {
        cookparms.sopAddWarning(SOP_ERR_BADGROUP, groupName);
        return nullptr;
    }

    bool success = true;
    const GA_Group* anyGroup = gop.parseGroupDetached(groupName, groupType, geo, true, false, success);

    //if (!success || (anyGroup && !anyGroup->isElementGroup()))
    if (!success)
    {
        cookparms.sopAddWarning(SOP_ERR_BADGROUP, groupName);
        return nullptr;
    }
    if (anyGroup)
    {
        return anyGroup;
    }
    return nullptr;
}


//static const GA_Group*
//parseGroupDetached(
//    const SOP_NodeVerb::CookParms& cookparms,
//    const GEO_Detail* geo,
//    const GA_GroupType& groupType,
//    const UT_StringHolder& groupName
//)
//{
//    GOP_Manager gop;
//    return parseGroupDetached(cookparms, geo, groupType, groupName, gop);
//}





//
//static const GA_EdgeGroup*
//parseEdgeGroupDetached(
//    const SOP_NodeVerb::CookParms& cookparms,
//    const GEO_Detail* geo,
//    const UT_StringHolder& groupName,
//    GOP_Manager& gop
//)
//{
//    if (!groupName.length())
//        return nullptr;
//
//    if (!groupName.isstring())
//    {
//        cookparms.sopAddWarning(SOP_ERR_BADGROUP, groupName);
//        return nullptr;
//    }
//
//    bool ok = true;
//    const GA_Group* anyGroup = gop.parseGroupDetached(groupName, GA_GROUP_EDGE, geo, true, false, ok);
//
//    if (!ok || (anyGroup && !anyGroup->isElementGroup()))
//    {
//        cookparms.sopAddWarning(SOP_ERR_BADGROUP, groupName);
//        return nullptr;
//    }
//    if (anyGroup && anyGroup->isElementGroup())
//    {
//        return UTverify_cast<const GA_EdgeGroup*>(anyGroup);
//    }
//    return nullptr;
//}
//
//
//static const GA_EdgeGroup*
//parseEdgeGroupDetached(
//    const SOP_NodeVerb::CookParms& cookparms,
//    const GEO_Detail* geo,
//    const UT_StringHolder& groupName
//)
//{
//    GOP_Manager gop;
//    return GA_FeE_Group::parseEdgeGroupDetached(cookparms, geo, groupName, gop);
//}






//GA_Range geo0Range = GA_FeE_Group::groupPromoteRange(geo, geo0Group, groupType);
//using GA_ElementGroupUPtr = ::std::unique_ptr<GA_ElementGroup, ::std::default_delete<GA_ElementGroup>>;

static GA_Range
getRangeByAnyGroup(
    const GA_Detail* geo,
    const GA_Group* group,
    const GA_GroupType& groupType
)
{
    if (!group)
    {
        switch (groupType)
        {
        case GA_GROUP_PRIMITIVE: return geo->getPrimitiveRange(); break;
        case GA_GROUP_POINT:     return geo->getPointRange();     break;
        case GA_GROUP_VERTEX:    return geo->getVertexRange();    break;
        }
        UT_ASSERT_MSG(0, "Unhandled Group Type!");
        return GA_Range();
    }

    //if (groupType != GA_GROUP_PRIMITIVE &&
    //    groupType != GA_GROUP_POINT     &&
    //    groupType != GA_GROUP_VERTEX)
    //    return nullptr;

    const GA_GroupType inGroupType = group->classType();

    //if (isInvalid(inGroupType))
    //    return nullptr;


    switch (groupType)
    {
    case GA_GROUP_PRIMITIVE:
    {
        const GA_PrimitiveGroup* promotedGroup = nullptr;
        if (groupType == GA_GROUP_PRIMITIVE)
            promotedGroup = static_cast<const GA_PrimitiveGroup*>(group);
        else
        {
            //GA_PrimitiveGroupUPtr groupUPtr = group->getDetail()->createDetachedPointGroup();
            GA_PrimitiveGroupUPtr groupUPtr;
            GA_PrimitiveGroup* newDetachedGroup = new GA_PrimitiveGroup(*geo);
            groupUPtr.reset(newDetachedGroup);
            newDetachedGroup->combine(group);
            promotedGroup = newDetachedGroup;
        }
        return geo->getPrimitiveRange(promotedGroup);
    }
        break;




    case GA_GROUP_POINT:
    {
        const GA_PointGroup* promotedGroup = nullptr;
        if (groupType == GA_GROUP_POINT)
            promotedGroup = static_cast<const GA_PointGroup*>(group);
        else
        {
            //GA_PrimitiveGroupUPtr groupUPtr = group->getDetail()->createDetachedPrimitiveGroup();
            GA_PointGroupUPtr groupUPtr;
            GA_PointGroup* newDetachedGroup = new GA_PointGroup(*geo);
            groupUPtr.reset(newDetachedGroup);
            newDetachedGroup->combine(group);
            promotedGroup = newDetachedGroup;
        }
        //return static_cast<GA_ElementGroup*>(promotedGroup);
        return geo->getPointRange(promotedGroup);
    }
    break;


    case GA_GROUP_VERTEX:
    {
        const GA_VertexGroup* promotedGroup = nullptr;
        if (groupType == GA_GROUP_VERTEX)
            promotedGroup = static_cast<const GA_VertexGroup*>(group);
        else
        {
            //GA_VertexGroupGroupUPtr groupUPtr = group->getDetail()->createDetachedVertexGroup();
            GA_VertexGroupUPtr groupUPtr;
            GA_VertexGroup* newDetachedGroup = new GA_VertexGroup(*geo);
            groupUPtr.reset(newDetachedGroup);
            newDetachedGroup->combine(group);
            promotedGroup = newDetachedGroup;
        }
        return geo->getVertexRange(promotedGroup);
    }
        break;
    }

    UT_ASSERT_MSG(0, "Unhandled Group Type!");
    return GA_Range();
}



static GA_Range
getRangeByAnyGroup(
    const GA_Detail* geo,
    const GA_Group* group,
    const GA_AttributeOwner& attribOwner
    )
{
    return getRangeByAnyGroup(geo, group, attributeOwner_groupType(attribOwner));
}


static GA_SplittableRange
getSplittableRangeByAnyGroup(
    const GA_Detail* geo,
    const GA_Group* group,
    const GA_GroupType& groupType
)
{
    return GA_SplittableRange(getRangeByAnyGroup(geo, group, groupType));
}

static GA_SplittableRange
getSplittableRangeByAnyGroup(
    const GA_Detail* geo,
    const GA_Group* group,
    const GA_AttributeOwner& attribOwner
)
{
    return GA_SplittableRange(getRangeByAnyGroup(geo, group, attribOwner));
}


//static void
//combineVertexFromEdgeGroup(
//    const GA_Detail* geo,
//    const GA_VertexGroup* group,
//    const GA_EdgeGroup* groupToCombine
//)
//{
//    if (!group)
//        return;
//    UT_UniquePtr<GA_VertexGroup> groupDeleter;
//    GA_VertexGroup* newDetachedGroup = new GA_VertexGroup(*geo);
//    groupDeleter.reset(newDetachedGroup);
//    newDetachedGroup->combine(groupToCombine);
//    group = newDetachedGroup;
//}




static const GA_Group*
groupPromoteDetached(
    GA_Detail* geo,
    const GA_Group* group,
    const GA_GroupType newType
)
{
    if (!geo || !group)
        return nullptr;

    if (group->classType() == newType)
        return group;

    GA_GroupTable* groupTable = geo->getGroupTable(newType);
    if (!groupTable)
        return nullptr;

    GA_Group* newGroup = groupTable->newDetachedGroup();
    newGroup->combine(group);
    return newGroup;
}

static const GA_Group*
groupPromoteDetached(
    GA_Detail* geo,
    const GA_Group* group,
    const GA_AttributeOwner newType
)
{
    return groupPromoteDetached(geo, group, attributeOwner_groupType(newType));
}


static const GA_Group*
groupPromotePrimitiveDetached(
    GA_Detail* geo,
    const GA_Group* group
)
{
    return groupPromoteDetached(geo, group, GA_GROUP_PRIMITIVE);
}

static const GA_Group*
groupPromotePointDetached(
    GA_Detail* geo,
    const GA_Group* group
)
{
    return groupPromoteDetached(geo, group, GA_GROUP_POINT);
}

static const GA_Group*
groupPromoteVertexDetached(
    GA_Detail* geo,
    const GA_Group* group
)
{
    return groupPromoteDetached(geo, group, GA_GROUP_VERTEX);
}





#if 1



static GA_Group*
groupPromoteDetached(
    GA_Detail* geo,
    GA_Group* group,
    const GA_GroupType newType,
    const bool delOriginal = false
)
{
    if (!geo || !group)
        return nullptr;

    if (group->classType() == newType)
        return group;

    GA_GroupTable* groupTable = geo->getGroupTable(newType);
    if (!groupTable)
        return nullptr;

    GA_Group* newGroup = static_cast<GA_Group*>(groupTable->newDetachedGroup());
    newGroup->combine(group);
    if (delOriginal)
    {
        geo->getGroupTable(group->classType())->destroy(group);
    }
    return newGroup;
}

static GA_Group*
groupPromoteDetached(
    GA_Detail* geo,
    GA_Group* group,
    const GA_AttributeOwner newType,
    const bool delOriginal = false
)
{
    return groupPromoteDetached(geo, group, attributeOwner_groupType(newType), delOriginal);
}


static GA_Group*
groupPromotePrimitiveDetached(
    GA_Detail* geo,
    GA_Group* group,
    const bool delOriginal = false
)
{
    return groupPromoteDetached(geo, group, GA_GROUP_PRIMITIVE, delOriginal);
}

static GA_Group*
groupPromotePointDetached(
    GA_Detail* geo,
    GA_Group* group,
    const bool delOriginal = false
)
{
    return groupPromoteDetached(geo, group, GA_GROUP_POINT, delOriginal);
}

static GA_Group*
groupPromoteVertexDetached(
    GA_Detail* geo,
    GA_Group* group,
    const bool delOriginal = false
)
{
    return groupPromoteDetached(geo, group, GA_GROUP_VERTEX, delOriginal);
}



#else

static UT_UniquePtr<GA_ElementGroup>
groupPromoteDetached(
    const GA_Detail* geo,
    const GA_ElementGroup* group,
    const GA_AttributeOwner newType
)
{
    if (!geo || !group)
        return nullptr;
    UT_UniquePtr<GA_ElementGroup> groupDeleter;
    GA_ElementGroup* newDetachedGroup = new GA_ElementGroup(*geo, newType);
    groupDeleter.reset(newDetachedGroup);
    newDetachedGroup->combine(group);
    return groupDeleter;
}

static UT_UniquePtr<GA_ElementGroup>
groupPromotePrimitiveDetached(
    const GA_Detail* geo,
    const GA_ElementGroup* group
)
{
    return groupPromoteDetached(geo, group, GA_ATTRIB_PRIMITIVE);
}

static UT_UniquePtr<GA_ElementGroup>
groupPromotePointDetached(
    const GA_Detail* geo,
    const GA_ElementGroup* group
)
{
    return groupPromoteDetached(geo, group, GA_ATTRIB_POINT);
}

static UT_UniquePtr<GA_ElementGroup>
groupPromoteVertexDetached(
    const GA_Detail* geo,
    const GA_ElementGroup* group
)
{
    return groupPromoteDetached(geo, group, GA_ATTRIB_VERTEX);
}



#endif



static bool
groupRename(
    GA_Detail* geo,
    GA_Group* group,
    const UT_StringHolder& newName
)
{
    return geo->getGroupTable(group->classType())->renameGroup(group->getName(), newName);

    //GA_GroupType groupType = group->classType();
    //switch (groupType)
    //{
    //case GA_GROUP_PRIMITIVE:
    //    geo->getGroupTable(groupType)->renameGroup(group->getName(), newName);
    //break;
    //case GA_GROUP_POINT:
    //break;
    //case GA_GROUP_VERTEX:
    //break;
    //case GA_GROUP_EDGE:
    //break;
    //default:
    //    break;
    //}
}

static void
groupDestroy(
    GA_Detail* geo,
    GA_Group* group
)
{
    geo->getGroupTable(group->classType())->destroy(group);
}

static GA_Group*
groupPromote(
    GA_Detail* geo,
    GA_Group* group,
    const GA_GroupType newType,
    const UT_StringHolder& newName,
    const bool detached = false,
    const bool delOriginal = false
)
{
    if (!geo || !group)
        return nullptr;

    if (group->classType() == newType)
    {
        groupRename(geo, group, newName);
        return group;
    }

    GA_GroupTable* groupTable = geo->getGroupTable(newType);
    if (!groupTable)
        return nullptr;

#if 1
    GA_Group* newGroup = detached ? groupTable->newDetachedGroup() : groupTable->newGroup(newName);
#else
    GA_Group* newGroup = nullptr;
    if (detached)
    {
        newGroup = groupTable->newDetachedGroup();
    }
    else
    {
        newGroup = groupTable->newGroup(newName);
    }
#endif
    groupCombine(geo, newGroup, group);
    if (delOriginal)
    {
        groupDestroy(geo, group);
    }
    return newGroup;
}

static GA_Group*
groupPromote(
    GA_Detail* geo,
    GA_Group* group,
    const GA_AttributeOwner newType,
    const UT_StringHolder& newName,
    const bool detached = false,
    const bool delOriginal = false
)
{
    return groupPromote(geo, group, attributeOwner_groupType(newType), newName, detached, delOriginal);
}


//static GA_ElementGroup*
//groupPromote(
//    GA_Detail* geo,
//    GA_ElementGroup* group,
//    const GA_GroupType newType,
//    const UT_StringHolder& newName,
//    const bool detached = false,
//    const bool delOriginal = false
//)
//{
//    if (!geo || !group)
//        return nullptr;
//
//    if (group->classType() == newType)
//        return group;
//
//    GA_GroupTable* groupTable = geo->getGroupTable(newType);
//    if (!groupTable)
//        return nullptr;
//
//    GA_ElementGroup* newGroup = static_cast<GA_ElementGroup*>(detached ? groupTable->newDetachedGroup() : groupTable->newGroup(newName));
//    newGroup->groupCombine(group);
//    if (delOriginal)
//    {
//        geo->getGroupTable(group->classType())->destroy(group);
//    }
//    return newGroup;
//}
//
//
//
//
//static GA_ElementGroup*
//groupPromotePrimitive(
//    GA_Detail* geo,
//    GA_ElementGroup* group,
//    const UT_StringHolder& newName,
//    const bool detached = false,
//    const bool delOriginal = false
//)
//{
//    return groupPromote(geo, group, GA_GROUP_PRIMITIVE, newName, detached, delOriginal);
//}
//
//static GA_ElementGroup*
//groupPromotePoint(
//    GA_Detail* geo,
//    GA_ElementGroup* group,
//    const UT_StringHolder& newName,
//    const bool detached = false,
//    const bool delOriginal = false
//)
//{
//    return groupPromote(geo, group, GA_GROUP_POINT, newName, detached, delOriginal);
//}
//
//static GA_ElementGroup*
//groupPromoteVertex(
//    GA_Detail* geo,
//    GA_ElementGroup* group,
//    const UT_StringHolder& newName,
//    const bool detached = false,
//    const bool delOriginal = false
//)
//{
//    return groupPromote(geo, group, GA_GROUP_VERTEX, newName, detached, delOriginal);
//}
//
//static GA_ElementGroup*
//groupPromoteEdge(
//    GA_Detail* geo,
//    GA_ElementGroup* group,
//    const UT_StringHolder& newName,
//    const bool detached = false,
//    const bool delOriginal = false
//)
//{
//    return groupPromote(geo, group, GA_GROUP_EDGE, newName, detached, delOriginal);
//}


















static GA_Group*
groupPromotePrimitive(
    GA_Detail* geo,
    GA_Group* group,
    const UT_StringHolder& newName,
    const bool detached = false,
    const bool delOriginal = false
)
{
    return groupPromote(geo, group, GA_GROUP_PRIMITIVE, newName, detached, delOriginal);
}

static GA_Group*
groupPromotePoint(
    GA_Detail* geo,
    GA_Group* group,
    const UT_StringHolder& newName,
    const bool detached = false,
    const bool delOriginal = false
)
{
    return groupPromote(geo, group, GA_GROUP_POINT, newName, detached, delOriginal);
}

static GA_Group*
groupPromoteVertex(
    GA_Detail* geo,
    GA_Group* group,
    const UT_StringHolder& newName,
    const bool detached = false,
    const bool delOriginal = false
)
{
    return groupPromote(geo, group, GA_GROUP_VERTEX, newName, detached, delOriginal);
}

static GA_Group*
groupPromoteEdge(
    GA_Detail* geo,
    GA_Group* group,
    const UT_StringHolder& newName,
    const bool detached = false,
    const bool delOriginal = false
)
{
    return groupPromote(geo, group, GA_GROUP_EDGE, newName, detached, delOriginal);
}




//static UT_UniquePtr<GA_PrimitiveGroup>
//groupPromotePrimitiveDetached(
//    const GA_Detail* geo,
//    const GA_ElementGroup* group
//)
//{
//    if (!group)
//        return nullptr;
//    UT_UniquePtr<GA_PrimitiveGroup> groupDeleter;
//    GA_PrimitiveGroup* newDetachedGroup = new GA_PrimitiveGroup(*geo);
//    groupDeleter.reset(newDetachedGroup);
//    newDetachedGroup->combine(group);
//    return groupDeleter;
//}
//
//static UT_UniquePtr<GA_PointGroup>
//groupPromotePointDetached(
//    const GA_Detail* geo,
//    const GA_ElementGroup* group
//)
//{
//    if (!group)
//        return nullptr;
//    UT_UniquePtr<GA_PointGroup> groupDeleter;
//    GA_PointGroup* newDetachedGroup = new GA_PointGroup(*geo);
//    groupDeleter.reset(newDetachedGroup);
//    newDetachedGroup->combine(group);
//    return groupDeleter;
//}
//
//static UT_UniquePtr<GA_VertexGroup>
//groupPromoteVertexDetached(
//    const GA_Detail* geo,
//    const GA_ElementGroup* group
//)
//{
//    if (!group)
//        return nullptr;
//    UT_UniquePtr<GA_VertexGroup> groupDeleter;
//    GA_VertexGroup* newDetachedGroup = new GA_VertexGroup(*geo);
//    groupDeleter.reset(newDetachedGroup);
//    newDetachedGroup->combine(group);
//    return groupDeleter;
//}





} // End of namespace GA_FeE_Group

#endif
