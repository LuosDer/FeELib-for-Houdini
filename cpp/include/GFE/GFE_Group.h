
#pragma once

#ifndef __GFE_Group_h__
#define __GFE_Group_h__

//#include "GFE/GFE_Group.h"

#include "GA/GA_Detail.h"
#include "GA/GA_SplittableRange.h"

#include "GFE/GFE_Type.h"

namespace GFE_Group {



SYS_FORCE_INLINE
    static void
    groupBumpDataId(
        GA_Group& group
    )
{
    if (group.classType() == GA_GROUP_EDGE)
        static_cast<GA_EdgeGroup&>(group).bumpDataId();
    else
        static_cast<GA_ElementGroup&>(group).bumpDataId();
}

SYS_FORCE_INLINE
    static void
    groupBumpDataId(
        GA_Group* group
    )
{
    groupBumpDataId(*group);
}


static void
groupBumpDataId(
    GA_GroupTable& groupTable,
    const UT_StringHolder& groupPattern
)
{
    if (groupPattern == "")
        return;
    for (GA_GroupTable::iterator<GA_Group> it = groupTable.beginTraverse(); !it.atEnd(); ++it)
    {
        GA_Group& group = *it.group();
        //if (group->isDetached())
        //    continue;
        if (!group.getName().multiMatch(groupPattern))
            continue;
        groupBumpDataId(group);
    }
}


SYS_FORCE_INLINE
static void
groupBumpDataId(
    GA_Detail& geo,
    const GA_GroupType groupType,
    const UT_StringHolder& groupPattern
)
{
    return groupBumpDataId(*geo.getGroupTable(groupType), groupPattern);
}



static void
delStdGroup(
    GA_GroupTable& groupTable,
    const UT_StringHolder& groupPattern
)
{
    if (groupPattern == "")
        return;
    for (GA_GroupTable::iterator<GA_Group> it = groupTable.beginTraverse(); !it.atEnd(); ++it)
    {
        GA_Group* const group = it.group();
        //if (group->isDetached())
        //    continue;
        if (!group->getName().multiMatch(groupPattern))
            continue;
        groupTable.destroy(group);
    }
}

SYS_FORCE_INLINE
static void
delStdGroup(
    GA_Detail& geo,
    const GA_GroupType groupType,
    const UT_StringHolder& groupPattern
)
{
    return delStdGroup(*geo.getGroupTable(groupType), groupPattern);
}


static void
delStdGroup(
    GA_Detail& geo,
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
    GA_GroupTable& groupTable,
    const UT_StringHolder& keepGroupPattern
)
{
    if (keepGroupPattern == "*")
        return;
    for (GA_GroupTable::iterator<GA_Group> it = groupTable.beginTraverse(); !it.atEnd(); ++it)
    {
        GA_Group* const group = it.group();
        //if (group->isDetached())
        //    continue;
        if (group->getName().match(keepGroupPattern))
            continue;
        groupTable.destroy(group);
    }
}



SYS_FORCE_INLINE
static void
keepStdGroup(
    GA_Detail& geo,
    const GA_GroupType groupTable,
    const UT_StringHolder& keepGroupPattern
)
{
    return keepStdGroup(*geo.getGroupTable(groupTable), keepGroupPattern);
}


static void
keepStdGroup(
    GA_Detail& geo,
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
    static GA_Group&
    groupDuplicate(
        GA_Detail& geo,
        const GA_Group& group,
        const UT_StringHolder& groupName
    )
{
    GA_Group& newGroup = *geo.getGroupTable(group.classType())->newGroup(groupName);
    if (group.isElementGroup())
        static_cast<GA_ElementGroup&>(newGroup).combine(&group);
    else
        static_cast<GA_EdgeGroup&>(newGroup).combine(&group);
    return newGroup;
}

SYS_FORCE_INLINE
static GA_Group&
groupDuplicate(
    const GA_Detail& geo,
    const GA_Group& group
)
{
    GA_Group& newGroup = *geo.getGroupTable(group.classType())->newDetachedGroup();
    if (group.isElementGroup())
        static_cast<GA_ElementGroup&>(newGroup).combine(&group);
    else
        static_cast<GA_EdgeGroup&>(newGroup).combine(&group);
    return newGroup;
}



SYS_FORCE_INLINE
    static GA_Group&
    groupDuplicateDetached(
        const GA_Detail& geo,
        const GA_Group& group
    )
{
    return groupDuplicate(geo, group);
}




SYS_FORCE_INLINE
    static void
    edgeGroupToggle(
        const GA_Detail& geo,
        GA_EdgeGroup& group
    )
{
    group.toggle();
    GA_Offset start, end;
    for (GA_Iterator it(geo.getPointRange()); it.fullBlockAdvance(start, end); )
    {
        for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
        {
            group.remove(GA_Edge(elemoff, elemoff));
        }
    }
    //group->makeAllEdgesValid();
}

SYS_FORCE_INLINE
static void
edgeGroupToggle(
    GA_EdgeGroup& group
)
{
    edgeGroupToggle(group.getDetail(), group);
}

SYS_FORCE_INLINE
static void
groupToggle(
    const GA_Detail& geo,
    GA_EdgeGroup& group
)
{
    edgeGroupToggle(geo, group);
}

SYS_FORCE_INLINE
static void
groupToggle(
    GA_EdgeGroup& group
)
{
    edgeGroupToggle(group.getDetail(), group);
}

SYS_FORCE_INLINE
static void
    groupToggle(
        GA_ElementGroup& group
    )
{
    //group。makeUnordered();
    group.toggleAll(group.getIndexMap().indexSize());
}

SYS_FORCE_INLINE
static void
    elementGroupToggle(
        GA_ElementGroup& group
    )
{
    groupToggle(group);
}

SYS_FORCE_INLINE
static void
elementGroupToggle(
    GA_Group& group
)
{
    elementGroupToggle(static_cast<GA_ElementGroup&>(group));
}

SYS_FORCE_INLINE
static void
groupToggle(
    GA_Group& group
)
{
    if (group.isElementGroup())
    {
        return elementGroupToggle(group);
    }
    else
    {
        return edgeGroupToggle(static_cast<GA_EdgeGroup&>(group));
    }
}

SYS_FORCE_INLINE
    static void
    groupToggle(
        const GA_Detail& geo,
        GA_Group& group
    )
{
    if (group.isElementGroup())
    {
        return elementGroupToggle(group);
    }
    else
    {
        return edgeGroupToggle(geo, static_cast<GA_EdgeGroup&>(group));
    }
}


static void
groupToggle(
    const GA_Detail& geo,
    const GA_GroupType groupType,
    const UT_StringHolder& groupName
)
{
    for (GA_GroupTable::iterator<GA_Group> it = geo.getGroupTable(groupType)->beginTraverse(); !it.atEnd(); ++it)
    {
        GA_Group& groupPtr = *it.group();
        //if (groupPtr->isDetached())
        //    continue;
        if (!groupPtr.getName().multiMatch(groupName))
            continue;
        groupToggle(geo, groupPtr);
        groupBumpDataId(groupPtr);
    }
}

static void
groupToggle(
    const SOP_NodeVerb::CookParms& cookparms,
    const GA_Detail& geo,
    const GA_GroupType groupType,
    const UT_StringHolder& groupName
)
{
    bool doHighlight = true;
    for (GA_GroupTable::iterator<GA_Group> it = geo.getGroupTable(groupType)->beginTraverse(); !it.atEnd(); ++it)
    {
        GA_Group& group = *it.group();
        //if (groupPtr->isDetached())
        //    continue;
        if (!group.getName().multiMatch(groupName))
            continue;
        groupToggle(geo, group);
        groupBumpDataId(group);

        if (doHighlight)
        {
            cookparms.select(group);
            doHighlight = false;
        }
    }
}















SYS_FORCE_INLINE
    static GA_Group*
    newGroup(
        GA_Detail& geo,
        const GA_Group& group,
        const UT_StringHolder& groupName
    )
{
    return geo.getGroupTable(group.classType())->newGroup(groupName);
}


SYS_FORCE_INLINE
    static GA_Group*
    newDetachedGroup(
        const GA_Detail& geo,
        const GA_Group& group
    )
{
    return geo.getGroupTable(group.classType())->newDetachedGroup();
}





static GA_Group*
findGroupBase(
    const GA_Detail& geo,
    const GA_GroupType groupType,
    const UT_StringHolder& groupName
)
{
    const GA_GroupTable* const groupTable = geo.getGroupTable(groupType);
    if (!groupTable)
        return nullptr;
    return groupTable->find(groupName);
}

static GA_Group*
findGroupN(
    const GA_Detail& geo,
    const UT_StringHolder& groupName
)
{
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
    const GA_Detail& geo,
    const GA_GroupType groupType,
    const UT_StringHolder& groupName
)
{
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
    GA_Detail& geo,
    const GA_GroupType groupType,
    const UT_StringHolder& groupName
)
{
    GA_GroupTable* const groupTable = geo.getGroupTable(groupType);
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
    GA_Detail& geo,
    const GA_GroupType groupType,
    const UT_StringHolder& groupName
)
{
    UT_ASSERT_P(groupType != GA_GROUP_EDGE);
    return geo.findElementGroup(GFE_Type::attributeOwner_groupType(groupType), groupName);
}

SYS_FORCE_INLINE
static const GA_ElementGroup*
findElementGroup(
    const GA_Detail& geo,
    const GA_GroupType groupType,
    const UT_StringHolder& groupName
)
{
    UT_ASSERT_P(groupType != GA_GROUP_EDGE);
    return geo.findElementGroup(GFE_Type::attributeOwner_groupType(groupType), groupName);
}

SYS_FORCE_INLINE
static bool
groupIsEmpty(
    const GA_Group& group
)
{
    if (group.classType() == GA_GROUP_EDGE)
    {
        return static_cast<const GA_EdgeGroup&>(group).isEmpty();
    }
    else
    {
        return static_cast<const GA_ElementGroup&>(group).isEmpty();
    }
}

SYS_FORCE_INLINE
static bool
groupRename(
    GA_Detail& geo,
    const GA_Group& group,
    const UT_StringHolder& newName
)
{
    if (group.getName() == newName)
        return false;
    return geo.getGroupTable(group.classType())->renameGroup(group.getName(), newName);
}

//SYS_FORCE_INLINE
//static void
//groupDestroy(
//    GA_Detail& geo,
//    GA_Group& group
//)
//{
//    if (!group)
//        return;
//    geo.destroyGroup(&group);
//    group = nullptr;
//    //geo.getGroupTable(group.classType())->destroy(&group);
//}










SYS_FORCE_INLINE
static GA_Range
getRangeByAnyGroup(
    const GA_Detail& geo,
    const GA_ElementGroup* const group
)
{
    if (!group)
        return GA_Range();

    const GA_AttributeOwner attribOwner = GFE_Type::attributeOwner_groupType(group->classType());
    return GA_Range(geo.getIndexMap(attribOwner), group);
}

SYS_FORCE_INLINE
static GA_Range
getRangeByAnyGroup(
    const GA_Detail& geo,
    const GA_Group* const group
)
{
    return getRangeByAnyGroup(geo, static_cast<const GA_ElementGroup*>(group));
}

SYS_FORCE_INLINE
static GA_SplittableRange
getSplittableRangeByAnyGroup(
    const GA_Detail& geo,
    const GA_ElementGroup* const group
)
{
    return GA_SplittableRange(getRangeByAnyGroup(geo, group));
}

SYS_FORCE_INLINE
static GA_SplittableRange
getSplittableRangeByAnyGroup(
    const GA_Detail& geo,
    const GA_Group* const group
)
{
    return getSplittableRangeByAnyGroup(geo, static_cast<const GA_ElementGroup*>(group));
}
























} // End of namespace GFE_Group

#endif
