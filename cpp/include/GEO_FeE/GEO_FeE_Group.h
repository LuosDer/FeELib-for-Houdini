
#pragma once

#ifndef __GEO_FeE_Group_h__
#define __GEO_FeE_Group_h__

//#include <GA_FeE/GEO_FeE_Group.h>

#include <GA/GA_Detail.h>

#include <GA_FeE/GA_FeE_GroupBoolean.h>
#include <GA_FeE/GA_FeE_GroupPromote.h>
#include <GA_FeE/GA_FeE_Group.h>
#include <GA_FeE/GA_FeE_TopologyReference.h>




namespace GEO_FeE_Group {






    SYS_FORCE_INLINE
    static GA_Group*
        groupDuplicate(
            GA_Detail* geo,
            const GA_Group* group,
            const UT_StringHolder& groupName
        )
    {
        UT_ASSERT_P(geo);
        UT_ASSERT_P(group);
        GA_Group* newGroup = geo->getGroupTable(group->classType())->newGroup(groupName);
        GA_FeE_GroupUnion::groupUnion(geo, newGroup, group);
        return newGroup;
    }


    SYS_FORCE_INLINE
    static GA_Group*
        groupDuplicateDetached(
            const GA_Detail* geo,
            const GA_Group* group
        )
    {
        UT_ASSERT_P(geo);
        UT_ASSERT_P(group);
        GA_Group* newGroup = geo->getGroupTable(group->classType())->newDetachedGroup();
        GA_FeE_GroupUnion::groupUnion(geo, newGroup, group);
        return newGroup;
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

        return GA_Range(geo->getIndexMap(GA_FeE_Type::attributeOwner_groupType(groupType)), GA_FeE_GroupPromote::elementGroupPromoteDetached(geo, group, groupType));
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
