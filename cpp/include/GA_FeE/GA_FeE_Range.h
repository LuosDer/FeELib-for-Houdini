
#pragma once

#ifndef __GA_FeE_Range_h__
#define __GA_FeE_Range_h__

//#include "GA_FeE/GA_FeE_Range.h"

#include "GA/GA_Detail.h"

#include "GA_FeE/GA_FeE_Type.h"
#include "GA_FeE/GA_FeE_GroupPromote.h"




namespace GA_FeE_Range {











    //GA_Range geo0Range = GA_FeE_Group::groupPromoteRange(geo, geo0Group, groupType);
    //using GA_ElementGroupUPtr = ::std::unique_ptr<GA_ElementGroup, ::std::default_delete<GA_ElementGroup>>;

    static GA_Range
        getRangeByAnyGroup(
            const GA_Detail* const geo,
            const GA_Group* const group,
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

        return GA_Range(geo->getIndexMap(GA_FeE_Type::attributeOwner_groupType(groupType)), GA_FeE_GroupPromote::elementGroupPromoteDetached(geo, group, groupType).get());
    }



    SYS_FORCE_INLINE
    static GA_Range
        getRangeByAnyGroup(
            const GA_Detail* const geo,
            const GA_Group* const group,
            const GA_AttributeOwner attribOwner
        )
    {
        return getRangeByAnyGroup(geo, group, GA_FeE_Type::attributeOwner_groupType(attribOwner));
    }


    SYS_FORCE_INLINE
    static GA_SplittableRange
        getSplittableRangeByAnyGroup(
            const GA_Detail* const geo,
            const GA_Group* const group,
            const GA_GroupType groupType
        )
    {
        return GA_SplittableRange(getRangeByAnyGroup(geo, group, groupType));
    }

    SYS_FORCE_INLINE
    static GA_SplittableRange
        getSplittableRangeByAnyGroup(
            const GA_Detail* const geo,
            const GA_Group* const group,
            const GA_AttributeOwner attribOwner
        )
    {
        return GA_SplittableRange(getRangeByAnyGroup(geo, group, attribOwner));
    }









} // End of namespace GA_FeE_Range

#endif
