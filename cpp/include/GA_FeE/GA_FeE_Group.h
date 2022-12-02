
#pragma once

#ifndef __GA_FeE_Group_h__
#define __GA_FeE_Group_h__

//#include <GA_FeE/GA_FeE_Group.h>

//#include <GA/GA_Detail.h>
#include <GEO/GEO_Detail.h>
#include <GA/GA_SplittableRange.h>

#include <GA_FeE/GA_FeE_Type.h>

namespace GA_FeE_Group {

    

SYS_FORCE_INLINE
    static GA_Group*
    newGroup(
        GA_Detail* geo,
        const GA_Group* group,
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
        const GA_Detail* geo,
        const GA_Group* group
    )
{
    UT_ASSERT_P(geo);
    UT_ASSERT_P(group);
    return geo->getGroupTable(group->classType())->newDetachedGroup();
}




static GA_Group*
findGroup(
    const GA_Detail* geo,
    const GA_GroupType groupType,
    const UT_StringHolder& groupName
)
{
    UT_ASSERT_P(geo);
    const GA_GroupTable* groupTable = geo->getGroupTable(groupType);
    if (!groupTable)
        return nullptr;
    return groupTable->find(groupName);
}

static GA_Group*
findOrCreateGroup(
    GA_Detail* geo,
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
    GA_Detail* geo,
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
    const GA_Detail* geo,
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
    const GA_Group* group
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
    GA_Detail* geo,
    const GA_Group* group,
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
//    GA_Detail* geo,
//    GA_Group* group
//)
//{
//    UT_ASSERT_P(geo);
//    if (!group)
//        return;
//    geo->destroyGroup(group);
//    //geo->getGroupTable(group->classType())->destroy(group);
//}

SYS_FORCE_INLINE
static void
groupBumpDataId(
    GA_Group* group
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
    const GEO_Detail* geo,
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






static const GA_Group*
parseGroupDetached(
    const SOP_NodeVerb::CookParms& cookparms,
    const GEO_Detail* geo,
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
    const GEO_Detail* geo,
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
    const GEO_Detail* geo,
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
    const GEO_Detail* geo,
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
    const GEO_Detail* geo,
    const UT_StringHolder& groupName,
    GOP_Manager& gop
)
{
    return static_cast<const GA_EdgeGroup*>(parseGroupDetached(cookparms, geo, GA_GROUP_EDGE, groupName, gop));
}















SYS_FORCE_INLINE
static GA_Range
getRangeByAnyGroup(
    const GA_Detail* geo,
    const GA_ElementGroup* group
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
    const GA_Detail* geo,
    const GA_Group* group
)
{
    return getRangeByAnyGroup(geo, static_cast<const GA_ElementGroup*>(group));
}

SYS_FORCE_INLINE
static GA_SplittableRange
getSplittableRangeByAnyGroup(
    const GA_Detail* geo,
    const GA_ElementGroup* group
)
{
    return GA_SplittableRange(getRangeByAnyGroup(geo, group));
}

SYS_FORCE_INLINE
static GA_SplittableRange
getSplittableRangeByAnyGroup(
    const GA_Detail* geo,
    const GA_Group* group
)
{
    return getSplittableRangeByAnyGroup(geo, static_cast<const GA_ElementGroup*>(group));
}






} // End of namespace GA_FeE_Group

#endif
