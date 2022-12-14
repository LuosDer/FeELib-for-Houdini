
#pragma once

#ifndef __GA_FeE_Group_h__
#define __GA_FeE_Group_h__

//#include "GA_FeE/GA_FeE_Group.h"

//#include "GA/GA_Detail.h"
#include "GEO/GEO_Detail.h"
#include "GA/GA_SplittableRange.h"

#include "GA_FeE/GA_FeE_Detail.h"
#include "GA_FeE/GA_FeE_Type.h"

namespace GA_FeE_Group {


static void
delStdGroup(
    GA_GroupTable* const groupTable,
    const UT_StringHolder& delGroupPattern
)
{
    if (delGroupPattern == "")
        return;
    for (GA_GroupTable::iterator it = groupTable->beginTraverse(); !it.atEnd(); ++it)
    {
        GA_Group* group = it.group();
        if (group->isDetached())
            continue;
        if (!group->getName().match(delGroupPattern))
            continue;
        groupTable->destroy(group);
    }
}

SYS_FORCE_INLINE
static void
delStdGroup(
    GA_Detail* const geo,
    const GA_GroupType groupTable,
    const UT_StringHolder& delGroupPattern
)
{
    return delStdGroup(geo->getGroupTable(groupTable), delGroupPattern);
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
    for (GA_GroupTable::iterator it = groupTable->beginTraverse(); !it.atEnd(); ++it)
    {
        GA_Group* group = it.group();
        if (group->isDetached())
            continue;
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
    GA_Group* newGroup = nullptr;
    if (group->isElementGroup())
    {
        newGroup = geo->getGroupTable(group->classType())->newDetachedGroup();
        UTverify_cast<GA_ElementGroup*>(newGroup)->combine(group);
    }
    else
    {
        newGroup = geo->getGroupTable(group->classType())->newDetachedGroup();
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
    groupToggle(
        const GA_Detail* const geo,
        GA_ElementGroup* const group
    )
{
    UT_ASSERT_P(geo);
    UT_ASSERT_P(group);
    UT_ASSERT_P(group->isElementGroup());
    GA_Size numelems = GA_FeE_Detail::numelems(geo, group->classType());
    group->makeUnordered();
    group->toggleAll(numelems);
}

SYS_FORCE_INLINE
    static void
    elementGroupToggle(
        const GA_Detail* const geo,
        GA_ElementGroup* const group
    )
{
    return groupToggle(geo, UTverify_cast<GA_ElementGroup*>(group));
}

SYS_FORCE_INLINE
static void
elementGroupToggle(
    const GA_Detail* const geo,
    GA_Group* const group
)
{
    return elementGroupToggle(geo, UTverify_cast<GA_ElementGroup*>(group));
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
        return elementGroupToggle(geo, group);
    }
    else
    {
        return edgeGroupToggle(geo, UTverify_cast<GA_EdgeGroup*>(group));
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
findGroup(
    const GA_Detail* const geo,
    const GA_GroupType groupType,
    const UT_StringHolder& groupName
)
{
    UT_ASSERT_P(geo);
    const GA_GroupTable* const groupTable = geo->getGroupTable(groupType);
    if (!groupTable)
        return nullptr;
    return groupTable->find(groupName);
}


static GA_Group*
findGroupMultiClass(
    const GA_Detail* const geo,
    const GA_GroupType groupType,
    const UT_StringHolder& groupName
)
{
    UT_ASSERT_P(geo);
    GA_Group* outGroup = findGroup(geo, GA_GROUP_PRIMITIVE, groupName);
    if (outGroup)
        return outGroup;
    outGroup = findGroup(geo, GA_GROUP_POINT, groupName);
    if (outGroup)
        return outGroup;
    outGroup = findGroup(geo, GA_GROUP_VERTEX, groupName);
    if (outGroup)
        return outGroup;
    outGroup = findGroup(geo, GA_GROUP_EDGE, groupName);
    return outGroup;
}


static GA_Group*
findOrCreateGroup(
    GA_Detail* const geo,
    const GA_GroupType groupType,
    const UT_StringHolder& groupName
)
{
    UT_ASSERT_P(geo);
    GA_GroupTable* groupTable = geo->getGroupTable(groupType);
    if (!groupTable)
        return nullptr;
    GA_Group* group = groupTable->find(groupName);
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
//    GA_Group* group
//)
//{
//    UT_ASSERT_P(geo);
//    if (!group)
//        return;
//    geo->destroyGroup(group);
//    group = nullptr;
//    //geo->getGroupTable(group->classType())->destroy(group);
//}

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








static const GA_Group*
findOrParseGroupDetached(
    const SOP_NodeVerb::CookParms& cookparms,
    const GEO_Detail* const geo,
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
    
    const GA_GroupTable* groupTable = geo->getGroupTable(groupType);
    if (groupTable)
    {
        const GA_Group* anyGroup = groupTable->find(groupName);
        if (anyGroup)
            return anyGroup;
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

SYS_FORCE_INLINE
static const GA_PrimitiveGroup*
findOrParsePrimitiveGroupDetached(
    const SOP_NodeVerb::CookParms& cookparms,
    const GEO_Detail* const geo,
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
    const GEO_Detail* geo,
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
    const GEO_Detail* const geo,
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
    const GEO_Detail* const geo,
    const UT_StringHolder& groupName,
    GOP_Manager& gop
)
{
    return static_cast<const GA_EdgeGroup*>(findOrParseGroupDetached(cookparms, geo, GA_GROUP_EDGE, groupName, gop));
}







static const GA_Group*
parseGroupDetached(
    const SOP_NodeVerb::CookParms& cookparms,
    const GEO_Detail* const geo,
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


SYS_FORCE_INLINE
static const GA_PrimitiveGroup*
parsePrimitiveGroupDetached(
    const SOP_NodeVerb::CookParms& cookparms,
    const GEO_Detail* const geo,
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
    const GEO_Detail* const geo,
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
    const GEO_Detail* const geo,
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
    const GEO_Detail* const geo,
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






} // End of namespace GA_FeE_Group

#endif
