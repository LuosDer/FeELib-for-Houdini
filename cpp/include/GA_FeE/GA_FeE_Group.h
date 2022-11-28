
#pragma once

#ifndef __GA_FeE_Group_h__
#define __GA_FeE_Group_h__

//#include <GA_FeE/GA_FeE_Group.h>

//#include <GA/GA_Detail.h>
#include <GEO/GEO_Detail.h>


#include <GA_FeE/GA_FeE_Type.h>

namespace GA_FeE_Group {




static GA_Group*
findGroup(
    GA_Detail* geo,
    const GA_GroupType groupType,
    const UT_StringHolder& groupName
)
{
    UT_ASSERT_P(geo);
    return geo->getGroupTable(groupType)->find(groupName);

    //switch (groupType)
    //{
    //case GA_GROUP_PRIMITIVE:
    //    return geo->primitiveGroups().find(groupName);
    //break;
    //case GA_GROUP_POINT:
    //    return geo->pointGroups().find(groupName);
    //break;
    //case GA_GROUP_VERTEX:
    //    return geo->vertexGroups().find(groupName);
    //break;
    //case GA_GROUP_EDGE:
    //    return geo->edgeGroups().find(groupName);
    //    break;
    //default:
    //    break;
    //}
    //return nullptr;

    //if (groupType == GA_GROUP_EDGE)
    //{
    //    return geo->edgeGroups().find(groupName);
    //}
    //else
    //{
    //    return geo->findElementGroup(GA_FeE_Type::attributeOwner_groupType(groupType), groupName);
    //}
}

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

static bool
isEmpty(
    const GA_Group* group,
    const GA_GroupType groupType
)
{
    UT_ASSERT_P(group);
    if (groupType == GA_GROUP_EDGE)
    {
        return UTverify_cast<const GA_EdgeGroup*>(group)->isEmpty();
    }
    else
    {
        return UTverify_cast<const GA_ElementGroup*>(group)->isEmpty();
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
        return UTverify_cast<GA_EdgeGroup*>(group)->isEmpty();
    }
    else
    {
        return UTverify_cast<GA_ElementGroup*>(group)->isEmpty();
    }
}


static bool
groupRename(
    GA_Detail* geo,
    GA_Group* group,
    const UT_StringHolder& newName
)
{
    if (group->getName() == newName)
        return false;
    return geo->getGroupTable(group->classType())->renameGroup(group->getName(), newName);
}

static void
groupDestroy(
    GA_Detail* geo,
    GA_Group* group
)
{
    UT_ASSERT_P(geo);
    UT_ASSERT_P(group);
    geo->getGroupTable(group->classType())->destroy(group);
}


static void
bumpDataId(
    GA_Group* group
)
{
    UT_ASSERT_P(group);
    if (group->classType() == GA_GROUP_EDGE)
    {
        static_cast<GA_EdgeGroup*>(group)->bumpDataId();
    }
    else
    {
        static_cast<GA_ElementGroup*>(group)->bumpDataId();
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





















static GA_Range
getRangeByAnyGroup(
    const GA_Detail* geo,
    const GA_Group* group
)
{
    UT_ASSERT_P(geo);
    if (!group)
        return GA_Range();

    const GA_GroupType inGroupType = group->classType();
    const GA_AttributeOwner attribOwner = GA_FeE_Type::attributeOwner_groupType(inGroupType);
    return GA_Range(geo->getIndexMap(attribOwner), static_cast<const GA_ElementGroup*>(group));
}

static GA_SplittableRange
getSplittableRangeByAnyGroup(
    const GA_Detail* geo,
    const GA_Group* group
)
{
    UT_ASSERT_P(geo);
    return GA_SplittableRange(getRangeByAnyGroup(geo, group));
}






} // End of namespace GA_FeE_Group

#endif
