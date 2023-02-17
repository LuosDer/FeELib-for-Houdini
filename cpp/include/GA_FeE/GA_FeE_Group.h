
#pragma once

#ifndef __GA_FeE_Group_h__
#define __GA_FeE_Group_h__

//#include "GA_FeE/GA_FeE_Group.h"

#include "GA/GA_Detail.h"
#include "GA/GA_SplittableRange.h"

#include "GA_FeE/GA_FeE_Type.h"

namespace GA_FeE_Group {



SYS_FORCE_INLINE
    static void
    groupBumpDataId(
        GA_Group* const group
    )
{
    UT_ASSERT_P(group);
    if (group->classType() == GA_GROUP_EDGE)
        static_cast<GA_EdgeGroup*>(group)->bumpDataId();
    else
        static_cast<GA_ElementGroup*>(group)->bumpDataId();
}


static void
groupBumpDataId(
    GA_GroupTable* const groupTable,
    const UT_StringHolder& groupPattern
)
{
    if (groupPattern == "")
        return;
    for (GA_GroupTable::iterator<GA_Group> it = groupTable->beginTraverse(); !it.atEnd(); ++it)
    {
        GA_Group* const group = it.group();
        //if (group->isDetached())
        //    continue;
        if (!group->getName().multiMatch(groupPattern))
            continue;
        groupBumpDataId(group);
    }
}


SYS_FORCE_INLINE
static void
groupBumpDataId(
    GA_Detail* const geo,
    const GA_GroupType groupType,
    const UT_StringHolder& groupPattern
)
{
    return groupBumpDataId(geo->getGroupTable(groupType), groupPattern);
}



static void
delStdGroup(
    GA_GroupTable* const groupTable,
    const UT_StringHolder& groupPattern
)
{
    if (groupPattern == "")
        return;
    for (GA_GroupTable::iterator<GA_Group> it = groupTable->beginTraverse(); !it.atEnd(); ++it)
    {
        GA_Group* const group = it.group();
        //if (group->isDetached())
        //    continue;
        if (!group->getName().multiMatch(groupPattern))
            continue;
        groupTable->destroy(group);
    }
}

SYS_FORCE_INLINE
static void
delStdGroup(
    GA_Detail* const geo,
    const GA_GroupType groupType,
    const UT_StringHolder& groupPattern
)
{
    return delStdGroup(geo->getGroupTable(groupType), groupPattern);
}


static void
delStdGroup(
    GA_Detail* const geo,
    const UT_StringHolder& primGroupPattern,
    const UT_StringHolder& pointGroupPattern,
    const UT_StringHolder& vertexGroupPattern,
    const UT_StringHolder& edgeGroupPattern
)
{
    delStdGroup(geo, GA_GROUP_PRIMITIVE, primGroupPattern);
    delStdGroup(geo, GA_GROUP_POINT,     pointGroupPattern);
    delStdGroup(geo, GA_GROUP_VERTEX,    vertexGroupPattern);
    delStdGroup(geo, GA_GROUP_EDGE,      edgeGroupPattern);
}





static void
keepStdGroup(
    GA_GroupTable* const groupTable,
    const UT_StringHolder& keepGroupPattern
)
{
    if (keepGroupPattern == "*")
        return;
    for (GA_GroupTable::iterator<GA_Group> it = groupTable->beginTraverse(); !it.atEnd(); ++it)
    {
        GA_Group* const group = it.group();
        //if (group->isDetached())
        //    continue;
        if (group->getName().match(keepGroupPattern))
            continue;
        groupTable->destroy(group);
    }
}



SYS_FORCE_INLINE
static void
keepStdGroup(
    GA_Detail* const geo,
    const GA_GroupType groupTable,
    const UT_StringHolder& keepGroupPattern
)
{
    return keepStdGroup(geo->getGroupTable(groupTable), keepGroupPattern);
}


static void
keepStdGroup(
    GA_Detail* const geo,
    const UT_StringHolder& primGroupPattern,
    const UT_StringHolder& pointGroupPattern,
    const UT_StringHolder& vertexGroupPattern,
    const UT_StringHolder& edgeGroupPattern
)
{
    keepStdGroup(geo, GA_GROUP_PRIMITIVE, primGroupPattern);
    keepStdGroup(geo, GA_GROUP_POINT,     pointGroupPattern);
    keepStdGroup(geo, GA_GROUP_VERTEX,    vertexGroupPattern);
    keepStdGroup(geo, GA_GROUP_EDGE,      edgeGroupPattern);
}





SYS_FORCE_INLINE
    static GA_Group*
    groupDuplicate(
        GA_Detail* const geo,
        const GA_Group* const group,
        const UT_StringHolder& groupName
    )
{
    UT_ASSERT_P(geo);
    UT_ASSERT_P(group);
    GA_Group* newGroup = nullptr;
    if (group->isElementGroup())
    {
        newGroup = geo->getGroupTable(group->classType())->newGroup(groupName);
        UTverify_cast<GA_ElementGroup*>(newGroup)->combine(group);
    }
    else
    {
        newGroup = geo->getGroupTable(group->classType())->newGroup(groupName);
        UTverify_cast<GA_EdgeGroup*>(newGroup)->combine(group);
    }
    return newGroup;
}

SYS_FORCE_INLINE
static GA_Group*
groupDuplicate(
    const GA_Detail* const geo,
    const GA_Group* const group
)
{
    UT_ASSERT_P(geo);
    UT_ASSERT_P(group);
    GA_Group* newGroup = geo->getGroupTable(group->classType())->newDetachedGroup();
    if (group->isElementGroup())
    {
        UTverify_cast<GA_ElementGroup*>(newGroup)->combine(group);
    }
    else
    {
        UTverify_cast<GA_EdgeGroup*>(newGroup)->combine(group);
    }
    return newGroup;
}



SYS_FORCE_INLINE
    static GA_Group*
    groupDuplicateDetached(
        const GA_Detail* const geo,
        const GA_Group* const group
    )
{
    return groupDuplicate(geo, group);
}




SYS_FORCE_INLINE
    static void
    edgeGroupToggle(
        const GA_Detail* const geo,
        GA_EdgeGroup* const group
    )
{
    UT_ASSERT_P(group);
    group->toggle();
    GA_Offset start, end;
    for (GA_Iterator it(geo->getPointRange()); it.fullBlockAdvance(start, end); )
    {
        for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
        {
            group->remove(GA_Edge(elemoff, elemoff));
        }
    }
    //group->makeAllEdgesValid();
}

SYS_FORCE_INLINE
static void
edgeGroupToggle(
    GA_EdgeGroup* const group
)
{
    UT_ASSERT_P(&group->getDetail());
    edgeGroupToggle(&group->getDetail(), group);
}

SYS_FORCE_INLINE
static void
groupToggle(
    const GA_Detail* const geo,
    GA_EdgeGroup* const group
)
{
    UT_ASSERT_P(&group->getDetail());
    edgeGroupToggle(geo, group);
}

SYS_FORCE_INLINE
static void
groupToggle(
    GA_EdgeGroup* const group
)
{
    UT_ASSERT_P(&group->getDetail());
    edgeGroupToggle(&group->getDetail(), group);
}

SYS_FORCE_INLINE
    static void
    groupToggle(
        GA_ElementGroup* const group
    )
{
    UT_ASSERT_P(group);
    UT_ASSERT_P(group->isElementGroup());
    //group->makeUnordered();
    group->toggleAll(group->getIndexMap().indexSize());
}

SYS_FORCE_INLINE
    static void
    elementGroupToggle(
        GA_ElementGroup* const group
    )
{
    return groupToggle(group);
}

SYS_FORCE_INLINE
static void
elementGroupToggle(
    GA_Group* const group
)
{
    return elementGroupToggle(UTverify_cast<GA_ElementGroup*>(group));
}

SYS_FORCE_INLINE
static void
groupToggle(
    GA_Group* const group
)
{
    UT_ASSERT_P(group);
    if (group->isElementGroup())
    {
        return elementGroupToggle(group);
    }
    else
    {
        return edgeGroupToggle(UTverify_cast<GA_EdgeGroup*>(group));
    }
}

SYS_FORCE_INLINE
    static void
    groupToggle(
        const GA_Detail* const geo,
        GA_Group* const group
    )
{
    UT_ASSERT_P(geo);
    UT_ASSERT_P(group);
    if (group->isElementGroup())
    {
        return elementGroupToggle(group);
    }
    else
    {
        return edgeGroupToggle(geo, UTverify_cast<GA_EdgeGroup*>(group));
    }
}


static void
groupToggle(
    const GA_Detail* const geo,
    const GA_GroupType groupType,
    const UT_StringHolder& groupName
)
{
    UT_ASSERT_P(geo);
    GA_Group* groupPtr = nullptr;
    for (GA_GroupTable::iterator<GA_Group> it = geo->getGroupTable(groupType)->beginTraverse(); !it.atEnd(); ++it)
    {
        groupPtr = it.group();
        //if (groupPtr->isDetached())
        //    continue;
        if (!groupPtr->getName().multiMatch(groupName))
            continue;
        groupToggle(geo, groupPtr);
        groupBumpDataId(groupPtr);
    }
}

static void
groupToggle(
    const SOP_NodeVerb::CookParms& cookparms,
    const GA_Detail* const geo,
    const GA_GroupType groupType,
    const UT_StringHolder& groupName
)
{
    UT_ASSERT_P(geo);
    bool doHighlight = true;
    GA_Group* groupPtr = nullptr;
    for (GA_GroupTable::iterator<GA_Group> it = geo->getGroupTable(groupType)->beginTraverse(); !it.atEnd(); ++it)
    {
        groupPtr = it.group();
        //if (groupPtr->isDetached())
        //    continue;
        if (!groupPtr->getName().multiMatch(groupName))
            continue;
        groupToggle(geo, groupPtr);
        groupBumpDataId(groupPtr);

        if (doHighlight)
        {
            cookparms.getNode()->setHighlight(true);
            cookparms.select(*groupPtr);
            doHighlight = false;
        }
    }
}















SYS_FORCE_INLINE
    static GA_Group*
    newGroup(
        GA_Detail* const geo,
        const GA_Group* const group,
        const UT_StringHolder& groupName
    )
{
    UT_ASSERT_P(geo);
    UT_ASSERT_P(group);
    return geo->getGroupTable(group->classType())->newGroup(groupName);
}


SYS_FORCE_INLINE
    static GA_Group*
    newDetachedGroup(
        const GA_Detail* const geo,
        const GA_Group* const group
    )
{
    UT_ASSERT_P(geo);
    UT_ASSERT_P(group);
    return geo->getGroupTable(group->classType())->newDetachedGroup();
}





static GA_Group*
findGroupBase(
    const GA_Detail* const geo,
    const GA_GroupType groupType,
    const UT_StringHolder& groupName
)
{
    const GA_GroupTable* const groupTable = geo->getGroupTable(groupType);
    if (!groupTable)
        return nullptr;
    return groupTable->find(groupName);
}

static GA_Group*
findGroupN(
    const GA_Detail* const geo,
    const UT_StringHolder& groupName
)
{
    UT_ASSERT_P(geo);
    GA_Group* outGroup = findGroupBase(geo, GA_GROUP_PRIMITIVE, groupName);
    if (outGroup)
        return outGroup;
    outGroup = findGroupBase(geo, GA_GROUP_POINT, groupName);
    if (outGroup)
        return outGroup;
    outGroup = findGroupBase(geo, GA_GROUP_VERTEX, groupName);
    if (outGroup)
        return outGroup;
    return findGroupBase(geo, GA_GROUP_EDGE, groupName);
}

static GA_Group*
findGroup(
    const GA_Detail* const geo,
    const GA_GroupType groupType,
    const UT_StringHolder& groupName
)
{
    UT_ASSERT_P(geo);
    if (groupType == GA_GROUP_N)
    {
        return findGroupN(geo, groupName);
    }
    else
    {
        return findGroupBase(geo, groupType, groupName);
    }
}




static GA_Group*
findOrCreateGroup(
    GA_Detail* const geo,
    const GA_GroupType groupType,
    const UT_StringHolder& groupName
)
{
    UT_ASSERT_P(geo);
    GA_GroupTable* const groupTable = geo->getGroupTable(groupType);
    if (!groupTable)
        return nullptr;
    GA_Group* const group = groupTable->find(groupName);
    if (!group)
        return groupTable->newGroup(groupName);
    return nullptr;
}




SYS_FORCE_INLINE
static GA_ElementGroup*
findElementGroup(
    GA_Detail* const geo,
    const GA_GroupType groupType,
    const UT_StringHolder& groupName
)
{
    UT_ASSERT_P(geo);
    UT_ASSERT_P(groupType != GA_GROUP_EDGE);
    return geo->findElementGroup(GA_FeE_Type::attributeOwner_groupType(groupType), groupName);
}

SYS_FORCE_INLINE
static const GA_ElementGroup*
findElementGroup(
    const GA_Detail* const geo,
    const GA_GroupType groupType,
    const UT_StringHolder& groupName
)
{
    UT_ASSERT_P(geo);
    UT_ASSERT_P(groupType != GA_GROUP_EDGE);
    return geo->findElementGroup(GA_FeE_Type::attributeOwner_groupType(groupType), groupName);
}

SYS_FORCE_INLINE
static bool
groupIsEmpty(
    const GA_Group* const group
)
{
    UT_ASSERT_P(group);
    if (group->classType() == GA_GROUP_EDGE)
    {
        return UTverify_cast<const GA_EdgeGroup*>(group)->isEmpty();
    }
    else
    {
        return UTverify_cast<const GA_ElementGroup*>(group)->isEmpty();
    }
}

//GA_FeE_Group::groupRename(outGeo0, N2DAttrib, geo0AttribNames);
SYS_FORCE_INLINE
static bool
groupRename(
    GA_Detail* const geo,
    const GA_Group* const group,
    const UT_StringHolder& newName
)
{
    if (group->getName() == newName)
        return false;
    return geo->getGroupTable(group->classType())->renameGroup(group->getName(), newName);
}

//SYS_FORCE_INLINE
//static void
//groupDestroy(
//    GA_Detail* const geo,
//    GA_Group* const group
//)
//{
//    UT_ASSERT_P(geo);
//    if (!group)
//        return;
//    geo->destroyGroup(group);
//    group = nullptr;
//    //geo->getGroupTable(group->classType())->destroy(group);
//}








static GA_Group*
findOrParseGroupDetached(
    const SOP_NodeVerb::CookParms& cookparms,
    GA_Detail* const geo,
    const GA_GroupType groupType,
    const UT_StringHolder& groupName,
    GOP_Manager& gop
)
{
    UT_ASSERT_P(geo);
    if (!groupName.length())
        return nullptr;

    if (!groupName.isstring())
    {
        cookparms.sopAddWarning(SOP_ERR_BADGROUP, groupName);
        return nullptr;
    }

    GA_GroupTable* const groupTable = geo->getGroupTable(groupType);
    if (groupTable)
    {
        GA_Group* const anyGroup = groupTable->find(groupName);
        if (anyGroup)
            return anyGroup;
    }

    bool success = true;
    GA_Group* const anyGroup = const_cast<GA_Group*>(gop.parseGroupDetached(groupName, groupType, static_cast<GEO_Detail*>(geo), true, false, success));
    //notifyGroupParmListeners(cookparms.getNode(), 0, 1, geo, group);

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

static const GA_Group*
findOrParseGroupDetached(
    const SOP_NodeVerb::CookParms& cookparms,
    const GA_Detail* const geo,
    const GA_GroupType groupType,
    const UT_StringHolder& groupName,
    GOP_Manager& gop
)
{
    UT_ASSERT_P(geo);
    if (!groupName.length())
        return nullptr;

    if (!groupName.isstring())
    {
        cookparms.sopAddWarning(SOP_ERR_BADGROUP, groupName);
        return nullptr;
    }

    const GA_GroupTable* const groupTable = geo->getGroupTable(groupType);
    if (groupTable)
    {
        const GA_Group* const anyGroup = groupTable->find(groupName);
        if (anyGroup)
            return anyGroup;
    }

    bool success = true;
    const GA_Group* const anyGroup = gop.parseGroupDetached(groupName, groupType, static_cast<const GEO_Detail*>(geo), true, false, success);

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




SYS_FORCE_INLINE
static const GA_PrimitiveGroup*
findOrParsePrimitiveGroupDetached(
    const SOP_NodeVerb::CookParms& cookparms,
    const GA_Detail* const geo,
    const UT_StringHolder& groupName,
    GOP_Manager& gop
)
{
    return static_cast<const GA_PrimitiveGroup*>(findOrParseGroupDetached(cookparms, geo, GA_GROUP_PRIMITIVE, groupName, gop));
}

SYS_FORCE_INLINE
static const GA_PointGroup*
findOrParsePointGroupDetached(
    const SOP_NodeVerb::CookParms& cookparms,
    const GA_Detail* geo,
    const UT_StringHolder& groupName,
    GOP_Manager& gop
)
{
    return static_cast<const GA_PointGroup*>(findOrParseGroupDetached(cookparms, geo, GA_GROUP_POINT, groupName, gop));
}

SYS_FORCE_INLINE
static const GA_VertexGroup*
findOrParseVertexGroupDetached(
    const SOP_NodeVerb::CookParms& cookparms,
    const GA_Detail* const geo,
    const UT_StringHolder& groupName,
    GOP_Manager& gop
)
{
    return static_cast<const GA_VertexGroup*>(findOrParseGroupDetached(cookparms, geo, GA_GROUP_VERTEX, groupName, gop));
}

SYS_FORCE_INLINE
static const GA_EdgeGroup*
findOrParseEdgeGroupDetached(
    const SOP_NodeVerb::CookParms& cookparms,
    const GA_Detail* const geo,
    const UT_StringHolder& groupName,
    GOP_Manager& gop
)
{
    return static_cast<const GA_EdgeGroup*>(findOrParseGroupDetached(cookparms, geo, GA_GROUP_EDGE, groupName, gop));
}






SYS_FORCE_INLINE
static GA_PrimitiveGroup*
findOrParsePrimitiveGroupDetached(
    const SOP_NodeVerb::CookParms& cookparms,
    GA_Detail* const geo,
    const UT_StringHolder& groupName,
    GOP_Manager& gop
)
{
    return static_cast<GA_PrimitiveGroup*>(findOrParseGroupDetached(cookparms, geo, GA_GROUP_PRIMITIVE, groupName, gop));
}

SYS_FORCE_INLINE
static GA_PointGroup*
findOrParsePointGroupDetached(
    const SOP_NodeVerb::CookParms& cookparms,
    GA_Detail* const geo,
    const UT_StringHolder& groupName,
    GOP_Manager& gop
)
{
    return static_cast<GA_PointGroup*>(findOrParseGroupDetached(cookparms, geo, GA_GROUP_POINT, groupName, gop));
}

SYS_FORCE_INLINE
static GA_VertexGroup*
findOrParseVertexGroupDetached(
    const SOP_NodeVerb::CookParms& cookparms,
    GA_Detail* const geo,
    const UT_StringHolder& groupName,
    GOP_Manager& gop
)
{
    return static_cast<GA_VertexGroup*>(findOrParseGroupDetached(cookparms, geo, GA_GROUP_VERTEX, groupName, gop));
}

SYS_FORCE_INLINE
static GA_EdgeGroup*
findOrParseEdgeGroupDetached(
    const SOP_NodeVerb::CookParms& cookparms,
    GA_Detail* const geo,
    const UT_StringHolder& groupName,
    GOP_Manager& gop
)
{
    return static_cast<GA_EdgeGroup*>(findOrParseGroupDetached(cookparms, geo, GA_GROUP_EDGE, groupName, gop));
}







static const GA_Group*
parseGroupDetached(
    const SOP_NodeVerb::CookParms& cookparms,
    const GA_Detail* const geo,
    const GA_GroupType groupType,
    const UT_StringHolder& groupName,
    GOP_Manager& gop
)
{
    UT_ASSERT_P(geo);
    if (!groupName.length())
        return nullptr;
    
    if (!groupName.isstring())
    {
        cookparms.sopAddWarning(SOP_ERR_BADGROUP, groupName);
        return nullptr;
    }

    bool success = true;
    const GA_Group* const anyGroup = gop.parseGroupDetached(groupName, groupType, static_cast<const GEO_Detail*>(geo), true, false, success);

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


SYS_FORCE_INLINE
static const GA_PrimitiveGroup*
parsePrimitiveGroupDetached(
    const SOP_NodeVerb::CookParms& cookparms,
    const GA_Detail* const geo,
    const UT_StringHolder& groupName,
    GOP_Manager& gop
)
{
    return static_cast<const GA_PrimitiveGroup*>(parseGroupDetached(cookparms, geo, GA_GROUP_PRIMITIVE, groupName, gop));
}


SYS_FORCE_INLINE
static const GA_PointGroup*
parsePointGroupDetached(
    const SOP_NodeVerb::CookParms& cookparms,
    const GA_Detail* const geo,
    const UT_StringHolder& groupName,
    GOP_Manager& gop
)
{
    return static_cast<const GA_PointGroup*>(parseGroupDetached(cookparms, geo, GA_GROUP_POINT, groupName, gop));
}

SYS_FORCE_INLINE
static const GA_VertexGroup*
parseVertexGroupDetached(
    const SOP_NodeVerb::CookParms& cookparms,
    const GA_Detail* const geo,
    const UT_StringHolder& groupName,
    GOP_Manager& gop
)
{
    return static_cast<const GA_VertexGroup*>(parseGroupDetached(cookparms, geo, GA_GROUP_VERTEX, groupName, gop));
}

SYS_FORCE_INLINE
static const GA_EdgeGroup*
parseEdgeGroupDetached(
    const SOP_NodeVerb::CookParms& cookparms,
    const GA_Detail* const geo,
    const UT_StringHolder& groupName,
    GOP_Manager& gop
)
{
    return static_cast<const GA_EdgeGroup*>(parseGroupDetached(cookparms, geo, GA_GROUP_EDGE, groupName, gop));
}















SYS_FORCE_INLINE
static GA_Range
getRangeByAnyGroup(
    const GA_Detail* const geo,
    const GA_ElementGroup* const group
)
{
    UT_ASSERT_P(geo);
    if (!group)
        return GA_Range();

    const GA_AttributeOwner attribOwner = GA_FeE_Type::attributeOwner_groupType(group->classType());
    return GA_Range(geo->getIndexMap(attribOwner), group);
}

SYS_FORCE_INLINE
static GA_Range
getRangeByAnyGroup(
    const GA_Detail* const geo,
    const GA_Group* const group
)
{
    return getRangeByAnyGroup(geo, static_cast<const GA_ElementGroup*>(group));
}

SYS_FORCE_INLINE
static GA_SplittableRange
getSplittableRangeByAnyGroup(
    const GA_Detail* const geo,
    const GA_ElementGroup* const group
)
{
    return GA_SplittableRange(getRangeByAnyGroup(geo, group));
}

SYS_FORCE_INLINE
static GA_SplittableRange
getSplittableRangeByAnyGroup(
    const GA_Detail* const geo,
    const GA_Group* const group
)
{
    return getSplittableRangeByAnyGroup(geo, static_cast<const GA_ElementGroup*>(group));
}
















static void
copyGroupDst(
    const GA_SplittableRange& geoSplittableRange,
    GA_EdgeGroup* const attribNew,
    const GA_EdgeGroup* const attribRef,
    const GA_Attribute* const attribID = nullptr
)
{
    if (attribID)
    {
        const GA_ROHandleT<exint> attribID_h(attribID);
        for (GA_EdgeGroup::const_iterator it = attribRef->begin(); it.atEnd(); ++it)
        {
            GA_Edge edge = it.getEdge();
            exint id0 = attribID_h.get(edge.p0());
            exint id1 = attribID_h.get(edge.p1());
            attribNew->add(id0, id1);
        }
    }
    else
    {
        for (GA_EdgeGroup::const_iterator it = attribRef->begin(); it.atEnd(); ++it)
        {
            GA_Edge edge = it.getEdge();
            attribNew->add(edge.p0(), edge.p1());
        }
    }
}

static void
copyGroupDst(
    const GA_SplittableRange& geoSplittableRange,
    GA_ElementGroup* const attribNew,
    const GA_ElementGroup* const attribRef,
    const GA_Attribute* const attribID = nullptr,
    const int subscribeRatio = 8,
    const int minGrainSize = 1024
)
{
    if (attribID)
    {
        const GA_ROHandleT<exint> attribID_h(attribID);
        UTparallelFor(geoSplittableRange, [attribNew, attribRef, &attribID_h](const GA_SplittableRange& r)
        {
            GA_Offset start, end;
            for (GA_Iterator it(r); it.blockAdvance(start, end); )
            {
                for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                {
                    exint id = attribID_h.get(elemoff);
                    attribNew->copy(id, *attribRef, elemoff);
                }
            }
        }, subscribeRatio, minGrainSize);
    }
    else
    {
        UTparallelFor(geoSplittableRange, [attribNew, attribRef](const GA_SplittableRange& r)
        {
            GA_Offset start, end;
            for (GA_Iterator it(r); it.blockAdvance(start, end); )
            {
                for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                {
                    attribNew->copy(elemoff, *attribRef, elemoff);
                }
            }
        }, subscribeRatio, minGrainSize);
    }
}





static void
copyGroupRef(
    const GA_SplittableRange& geoSplittableRange,
    GA_EdgeGroup* const attribNew,
    const GA_EdgeGroup* const attribRef,
    const GA_Attribute* const attribID = nullptr
)
{
    if (attribID)
    {
        const GA_ROHandleT<exint> attribID_h(attribID);
        for (GA_EdgeGroup::const_iterator it = attribRef->begin(); it.atEnd(); ++it)
        {
            GA_Edge edge = it.getEdge();
            exint id0 = attribID_h.get(edge.p0());
            exint id1 = attribID_h.get(edge.p1());
            attribNew->add(id0, id1);
        }
    }
    else
    {
        for (GA_EdgeGroup::const_iterator it = attribRef->begin(); it.atEnd(); ++it)
        {
            GA_Edge edge = it.getEdge();
            attribNew->add(edge.p0(), edge.p1());
        }
    }
}

static void
copyGroupRef(
    const GA_SplittableRange& geoSplittableRange,
    GA_ElementGroup* const attribNew,
    const GA_ElementGroup* const attribRef,
    const GA_Attribute* const attribID = nullptr,
    const int subscribeRatio = 8,
    const int minGrainSize = 1024
)
{
    if (attribID)
    {
        const GA_ROHandleT<exint> attribID_h(attribID);
        UTparallelFor(geoSplittableRange, [attribNew, attribRef, &attribID_h](const GA_SplittableRange& r)
        {
            GA_Offset start, end;
            for (GA_Iterator it(r); it.blockAdvance(start, end); )
            {
                for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                {
                    exint id = attribID_h.get(elemoff);
                    attribNew->copy(id, *attribRef, elemoff);
                }
            }
        }, subscribeRatio, minGrainSize);
    }
    else
    {
        UTparallelFor(geoSplittableRange, [attribNew, attribRef](const GA_SplittableRange& r)
        {
            GA_Offset start, end;
            for (GA_Iterator it(r); it.blockAdvance(start, end); )
            {
                for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                {
                    attribNew->copy(elemoff, *attribRef, elemoff);
                }
            }
        }, subscribeRatio, minGrainSize);
    }
}




static void
copyGroupDst(
    const GA_SplittableRange& geoSplittableRange,
    GA_Group* const attribNew,
    const GA_Group* const attribRef,
    const GA_Attribute* const attribID = nullptr,
    const int subscribeRatio = 8,
    const int minGrainSize = 1024
)
{
    if (attribNew->classType() == GA_GROUP_EDGE)
    {
        copyGroupDst(geoSplittableRange, static_cast<GA_EdgeGroup*>(attribNew), static_cast<const GA_EdgeGroup*>(attribRef), attribID);
    }
    else
    {
        copyGroupDst(geoSplittableRange, static_cast<GA_ElementGroup*>(attribNew), static_cast<const GA_ElementGroup*>(attribRef), attribID, subscribeRatio, minGrainSize);
    }
}

static void
copyGroupRef(
    const GA_SplittableRange& geoSplittableRange,
    GA_Group* const attribNew,
    const GA_Group* const attribRef,
    const GA_Attribute* const attribID = nullptr,
    const int subscribeRatio = 8,
    const int minGrainSize = 1024
)
{
    if (attribNew->classType() == GA_GROUP_EDGE)
    {
        copyGroupRef(geoSplittableRange, static_cast<GA_EdgeGroup*>(attribNew), static_cast<const GA_EdgeGroup*>(attribRef), attribID);
    }
    else
    {
        copyGroupRef(geoSplittableRange, static_cast<GA_ElementGroup*>(attribNew), static_cast<const GA_ElementGroup*>(attribRef), attribID, subscribeRatio, minGrainSize);
    }
}

static void
copyGroup(
    GA_Detail* const geo,
    const GA_GroupType owner,
    const GA_Detail* const geoRef,
    const GA_GroupType ownerRef,
    const UT_StringHolder& attribPattern,
    //const UT_StringHolder& attribNameNewPattern,
    const UT_StringHolder& iDAttribName = "",
    const bool iDAttribInput = false,
    const int subscribeRatio = 8,
    const int minGrainSize = 1024
)
{
    if (attribPattern.length() == 0)
        return;

    GA_GroupTable* const attribsDst = geo->getGroupTable(owner);
    GA_Group* attribNew = nullptr;
    const GA_Group* attribRef = nullptr;


    const GA_Attribute* iDAttrib = nullptr;

    GA_Range range;
    if (iDAttribInput)//DESTINATION
    {
        if (iDAttribName.length() > 0)
            iDAttrib = geo->getAttributes().findAttribute(GA_FeE_Type::attributeOwner_groupType(owner), iDAttribName);
        range = GA_Range(geo->getIndexMap(GA_FeE_Type::attributeOwner_groupType(owner)));
    }
    else//Src
    {
        if (iDAttribName.length() > 0)
            iDAttrib = geoRef->getAttributes().findAttribute(GA_FeE_Type::attributeOwner_groupType(ownerRef), iDAttribName);
        range = GA_Range(geoRef->getIndexMap(GA_FeE_Type::attributeOwner_groupType(ownerRef)));
    }
    const GA_SplittableRange geoSplittableRange(range);


    for (GA_GroupTable::iterator<GA_Group> it = geoRef->getGroupTable(ownerRef)->beginTraverse(); !it.atEnd(); ++it)
    {
        attribRef = it.group();
        //if (attribRef->isDetached())
        //    continue;
        const UT_StringHolder& attribName = attribRef->getName();

        if (!attribName.multiMatch(attribPattern, true, " "))
            continue;

        const UT_StringHolder& attribNameNew = attribName;

        attribNew = attribsDst->find(attribNameNew);
        if (attribNew)
        {
            GA_FeE_Group::groupBumpDataId(attribNew);
        }
        else
        {
            if (owner == GA_GROUP_EDGE)
            {
                attribNew = attribsDst->newGroup(attribNameNew);
            }
            else
            {
                attribNew = attribsDst->newGroup(attribNameNew);
                //attribNew = static_cast<GA_ElementGroupTable*>(attribsDst)->cloneAttribute(owner, attribNameNew, *attribRef, true);
            }
        }

        if (iDAttribInput)//DESTINATION
        {
            copyGroupDst(geoSplittableRange, attribNew, attribRef, iDAttrib, subscribeRatio, minGrainSize);
        }
        else//Src
        {
            copyGroupRef(geoSplittableRange, attribNew, attribRef, iDAttrib, subscribeRatio, minGrainSize);
        }
    }
}


















} // End of namespace GA_FeE_Group

#endif
