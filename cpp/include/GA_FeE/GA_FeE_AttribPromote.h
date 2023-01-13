
#pragma once

#ifndef __GA_FeE_AttribPromote_h__
#define __GA_FeE_AttribPromote_h__

//#include "GA_FeE/GA_FeE_AttribPromote.h"

#include "GA/GA_Detail.h"

//#include "GA/GA_SplittableRange.h"

//#include "GA_FeE/GA_FeE_Type.h"

namespace GA_FeE_AttribPromote {

    



    static GA_Attribute*
        attribPromoteDetached(
            GA_Detail* const geo,
            const GA_Group* const group,
            const GA_GroupType newType
        )
    {
        UT_ASSERT_P(geo);
        if (!group)
            return nullptr;

        if (group->classType() == newType)
            return group;

        GA_GroupTable* const groupTable = geo->getGroupTable(newType);
        if (!groupTable)
            return nullptr;

        GA_Group* const newGroup = groupTable->newGroup(group->getName());
        GA_FeE_GroupUnion::groupUnion(geo, newGroup, group);

        return newGroup;
    }


    static GA_Attribute*
        attribPromote(
            GA_Detail* const geo,
            const GA_Group* const group,
            const GA_AttributeOwner owner
        )
    {
        UT_ASSERT_P(geo);
        if (!group)
            return nullptr;

        GA_GroupTable* const groupTable = geo->getAttributes().cloneAttribute();
        if (!groupTable)
            return nullptr;

        GA_Group* const newGroup = groupTable->newGroup(group->getName());
        GA_FeE_GroupUnion::groupUnion(geo, newGroup, group);

        return newGroup;
    }



    static const GA_Attribute*
        attribFindPromote(
            GA_Detail* const geo,
            const GA_Group* const group,
            const GA_GroupType newType
        )
    {
        UT_ASSERT_P(geo);
        if (!group)
            return nullptr;

        if (group->classType() == newType)
            return group;

        GA_GroupTable* const groupTable = geo->getGroupTable(newType);
        if (!groupTable)
            return nullptr;

        GA_Group* const newGroup = groupTable->newGroup(group->getName());
        GA_FeE_GroupUnion::groupUnion(geo, newGroup, group);

        return newGroup;
    }




} // End of namespace GA_FeE_AttribPromote

#endif
