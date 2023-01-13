
#pragma once

#ifndef __GA_FeE_GroupPromote_h__
#define __GA_FeE_GroupPromote_h__

//#include "GA_FeE/GA_FeE_GroupPromote.h"

#include "GA/GA_Detail.h"

#include "GA_FeE/GA_FeE_Group.h"
#include "GA_FeE/GA_FeE_GroupUnion.h"
//#include "GA/GA_SplittableRange.h"

//#include "GA_FeE/GA_FeE_Type.h"

namespace GA_FeE_GroupPromote {

    



    static const GA_Group*
        groupPromote(
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

    static GA_Group*
        groupPromote(
            GA_Detail* const geo,
            GA_Group* const group,
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

    static const GA_Group*
        groupPromote(
            GA_Detail* const geo,
            const GA_Group* const group,
            const GA_GroupType newType,
            const UT_StringHolder& newName
        )
    {
        UT_ASSERT_P(geo);
        if (!group)
            return nullptr;

        if (group->classType() == newType)
        {
            GA_FeE_Group::groupRename(geo, group, newName);
            return group;
        }

        GA_GroupTable* const groupTable = geo->getGroupTable(newType);
        if (!groupTable)
            return nullptr;

        GA_Group* const newGroup = groupTable->newGroup(newName);
        GA_FeE_GroupUnion::groupUnion(geo, newGroup, group);

        return newGroup;
    }

    static GA_Group*
        groupPromote(
            GA_Detail* const geo,
            GA_Group* const group,
            const GA_GroupType newType,
            const UT_StringHolder& newName
        )
    {
        UT_ASSERT_P(geo);
        if (!group)
            return nullptr;

        if (group->classType() == newType)
        {
            GA_FeE_Group::groupRename(geo, group, newName);
            return group;
        }

        GA_GroupTable* const groupTable = geo->getGroupTable(newType);
        if (!groupTable)
            return nullptr;

        GA_Group* const newGroup = groupTable->newGroup(newName);
        GA_FeE_GroupUnion::groupUnion(geo, newGroup, group);

        return newGroup;
    }




    static GA_Group*
        groupPromoteDetached(
            const GA_Detail* const geo,
            const GA_Group* const group,
            const GA_GroupType newType
        )
    {
        UT_ASSERT_P(geo);
        if (!group)
            return nullptr;

        const GA_GroupTable* const groupTable = geo->getGroupTable(newType);
        if (!groupTable)
            return nullptr;
        
        GA_Group* const newGroup = groupTable->newDetachedGroup();
        //GA_GroupUPtr newGroupUPtr = UTmakeUnique<GA_Group>(*geo);
        //GA_Group* const newGroup = newGroupUPtr.get();
        
        GA_FeE_GroupUnion::groupUnion(geo, newGroup, group);

        return newGroup;
    }

    static const GA_Group*
        groupFindPromoteDetached(
            const GA_Detail* const geo,
            const GA_Group* const group,
            const GA_GroupType newType
        )
    {
        UT_ASSERT_P(geo);
        if (!group)
            return nullptr;

        if (group->classType() == newType)
            return group;

        const GA_GroupTable* const groupTable = geo->getGroupTable(newType);
        if (!groupTable)
            return nullptr;

        GA_Group* const newGroup = groupTable->newDetachedGroup();
        GA_FeE_GroupUnion::groupUnion(geo, newGroup, group);

        return newGroup;
    }




    SYS_FORCE_INLINE
        static const GA_Group*
        groupPromote(
            GA_Detail* const geo,
            const GA_Group* const group,
            const GA_AttributeOwner newType,
            const UT_StringHolder& newName
        )
    {
        return groupPromote(geo, group, GA_FeE_Type::attributeOwner_groupType(newType), newName);
    }

    SYS_FORCE_INLINE
        static GA_Group*
        groupPromote(
            GA_Detail* const geo,
            GA_Group* const group,
            const GA_AttributeOwner newType,
            const UT_StringHolder& newName
        )
    {
        return groupPromote(geo, group, GA_FeE_Type::attributeOwner_groupType(newType), newName);
    }

    SYS_FORCE_INLINE
        static GA_Group*
        groupPromoteDetached(
            const GA_Detail* const geo,
            const GA_Group* const group,
            const GA_AttributeOwner newType
        )
    {
        return groupPromoteDetached(geo, group, GA_FeE_Type::attributeOwner_groupType(newType));
    }

    SYS_FORCE_INLINE
        static const GA_Group*
        groupFindPromoteDetached(
            const GA_Detail* const geo,
            const GA_Group* const group,
            const GA_AttributeOwner newType
        )
    {
        return groupFindPromoteDetached(geo, group, GA_FeE_Type::attributeOwner_groupType(newType));
    }




    SYS_FORCE_INLINE
    static GA_Group*
    groupPromote(
        GA_Detail* const geo,
        GA_Group*& group,
        const GA_GroupType newType,
        const UT_StringHolder& newName,
        const bool delOriginal
    )
    {
        GA_Group* newGroup = groupPromote(geo, group, newType, newName);
        if (delOriginal && group != newGroup)
        {
            geo->destroyGroup(group);
            group = nullptr;
        }
        return newGroup;
    }

    SYS_FORCE_INLINE
        static GA_Group*
        groupPromote(
            GA_Detail* const geo,
            GA_PrimitiveGroup*& group,
            const GA_GroupType newType,
            const UT_StringHolder& newName,
            const bool delOriginal
        )
    {
        GA_Group* newGroup = groupPromote(geo, group, newType, newName);
        if (delOriginal && group != newGroup)
        {
            geo->destroyGroup(group);
            group = nullptr;
        }
        return newGroup;
    }
    SYS_FORCE_INLINE
        static GA_Group*
        groupPromote(
            GA_Detail* const geo,
            GA_PointGroup*& group,
            const GA_GroupType newType,
            const UT_StringHolder& newName,
            const bool delOriginal
        )
    {
        GA_Group* newGroup = groupPromote(geo, group, newType, newName);
        if (delOriginal && group != newGroup)
        {
            geo->destroyGroup(group);
            group = nullptr;
        }
        return newGroup;
    }
    SYS_FORCE_INLINE
        static GA_Group*
        groupPromote(
            GA_Detail* const geo,
            GA_VertexGroup*& group,
            const GA_GroupType newType,
            const UT_StringHolder& newName,
            const bool delOriginal
        )
    {
        GA_Group* newGroup = groupPromote(geo, group, newType, newName);
        if (delOriginal && group != newGroup)
        {
            geo->destroyGroup(group);
            group = nullptr;
        }
        return newGroup;
    }
    SYS_FORCE_INLINE
        static GA_Group*
        groupPromote(
            GA_Detail* const geo,
            GA_EdgeGroup*& group,
            const GA_GroupType newType,
            const UT_StringHolder& newName,
            const bool delOriginal
        )
    {
        GA_Group* newGroup = groupPromote(geo, group, newType, newName);
        if (delOriginal && group != newGroup)
        {
            geo->destroyGroup(group);
            group = nullptr;
        }
        return newGroup;
    }

    SYS_FORCE_INLINE
        static GA_Group*
        groupPromoteDetached(
            GA_Detail* const geo,
            GA_Group*& group,
            const GA_GroupType newType,
            const bool delOriginal
        )
    {
        GA_Group* newGroup = groupPromoteDetached(geo, group, newType);
        if (delOriginal && group != newGroup)
        {
            geo->destroyGroup(group);
            group = nullptr;
        }
        return newGroup;
    }

    SYS_FORCE_INLINE
        static const GA_Group*
        groupFindPromoteDetached(
            GA_Detail* const geo,
            GA_Group*& group,
            const GA_GroupType newType,
            const bool delOriginal
        )
    {
        const GA_Group* newGroup = groupFindPromoteDetached(geo, group, newType);
        if (delOriginal && group != newGroup)
        {
            geo->destroyGroup(group);
            group = nullptr;
        }
        return newGroup;
    }





    SYS_FORCE_INLINE
        static const GA_PrimitiveGroup*
        groupPromotePrimitive(
            GA_Detail* const geo,
            const GA_Group* const group,
            const UT_StringHolder& newName
        )
    {
        return static_cast<const GA_PrimitiveGroup*>(groupPromote(geo, group, GA_GROUP_PRIMITIVE, newName));
    }

    SYS_FORCE_INLINE
        static const GA_PointGroup*
        groupPromotePoint(
            GA_Detail* const geo,
            const GA_Group* const group,
            const UT_StringHolder& newName
        )
    {
        return static_cast<const GA_PointGroup*>(groupPromote(geo, group, GA_GROUP_POINT, newName));
    }

    SYS_FORCE_INLINE
        static const GA_VertexGroup*
        groupPromoteVertex(
            GA_Detail* const geo,
            const GA_Group* const group,
            const UT_StringHolder& newName
        )
    {
        return static_cast<const GA_VertexGroup*>(groupPromote(geo, group, GA_GROUP_VERTEX, newName));
    }

    SYS_FORCE_INLINE
        static const GA_EdgeGroup*
        groupPromoteEdge(
            GA_Detail* const geo,
            const GA_Group* const group,
            const UT_StringHolder& newName
        )
    {
        return static_cast<const GA_EdgeGroup*>(groupPromote(geo, group, GA_GROUP_EDGE, newName));
    }






    SYS_FORCE_INLINE
        static GA_PrimitiveGroup*
        groupPromotePrimitive(
            GA_Detail* const geo,
            GA_Group* const group,
            const UT_StringHolder& newName
        )
    {
        return static_cast<GA_PrimitiveGroup*>(groupPromote(geo, group, GA_GROUP_PRIMITIVE, newName));
    }

    SYS_FORCE_INLINE
        static GA_PointGroup*
        groupPromotePoint(
            GA_Detail* const geo,
            GA_Group* const group,
            const UT_StringHolder& newName
        )
    {
        return static_cast<GA_PointGroup*>(groupPromote(geo, group, GA_GROUP_POINT, newName));
    }

    SYS_FORCE_INLINE
        static GA_VertexGroup*
        groupPromoteVertex(
            GA_Detail* const geo,
            GA_Group* const group,
            const UT_StringHolder& newName
        )
    {
        return static_cast<GA_VertexGroup*>(groupPromote(geo, group, GA_GROUP_VERTEX, newName));
    }

    SYS_FORCE_INLINE
        static GA_EdgeGroup*
        groupPromoteEdge(
            GA_Detail* const geo,
            GA_Group* const group,
            const UT_StringHolder& newName
        )
    {
        return static_cast<GA_EdgeGroup*>(groupPromote(geo, group, GA_GROUP_EDGE, newName));
    }







    SYS_FORCE_INLINE
        static const GA_PrimitiveGroup*
        groupPromotePrimitive(
            GA_Detail* const geo,
            const GA_Group* const group
        )
    {
        return static_cast<const GA_PrimitiveGroup*>(groupPromote(geo, group, GA_GROUP_PRIMITIVE));
    }

    SYS_FORCE_INLINE
        static const GA_PointGroup*
        groupPromotePoint(
            GA_Detail* const geo,
            const GA_Group* const group
        )
    {
        return static_cast<const GA_PointGroup*>(groupPromote(geo, group, GA_GROUP_POINT));
    }

    SYS_FORCE_INLINE
        static const GA_VertexGroup*
        groupPromoteVertex(
            GA_Detail* const geo,
            const GA_Group* const group
        )
    {
        return static_cast<const GA_VertexGroup*>(groupPromote(geo, group, GA_GROUP_VERTEX));
    }

    SYS_FORCE_INLINE
        static const GA_EdgeGroup*
        groupPromoteEdge(
            GA_Detail* const geo,
            const GA_Group* const group
        )
    {
        return static_cast<const GA_EdgeGroup*>(groupPromote(geo, group, GA_GROUP_EDGE));
    }





    SYS_FORCE_INLINE
        static GA_PrimitiveGroup*
        groupPromotePrimitive(
            GA_Detail* const geo,
            GA_Group* const group
        )
    {
        return static_cast<GA_PrimitiveGroup*>(groupPromote(geo, group, GA_GROUP_PRIMITIVE));
    }

    SYS_FORCE_INLINE
        static GA_PointGroup*
        groupPromotePoint(
            GA_Detail* const geo,
            GA_Group* const group
        )
    {
        return static_cast<GA_PointGroup*>(groupPromote(geo, group, GA_GROUP_POINT));
    }

    SYS_FORCE_INLINE
        static GA_VertexGroup*
        groupPromoteVertex(
            GA_Detail* const geo,
            GA_Group* const group
        )
    {
        return static_cast<GA_VertexGroup*>(groupPromote(geo, group, GA_GROUP_VERTEX));
    }

    SYS_FORCE_INLINE
        static GA_EdgeGroup*
        groupPromoteEdge(
            GA_Detail* const geo,
            GA_Group* const group
        )
    {
        return static_cast<GA_EdgeGroup*>(groupPromote(geo, group, GA_GROUP_EDGE));
    }





    SYS_FORCE_INLINE
        static const GA_PrimitiveGroup*
        groupPromotePrimitive(
            GA_Detail* const geo,
            GA_Group*& group,
            const UT_StringHolder& newName,
            const bool delOriginal
        )
    {
        return static_cast<const GA_PrimitiveGroup*>(groupPromote(geo, group, GA_GROUP_PRIMITIVE, newName, delOriginal));
    }

    SYS_FORCE_INLINE
        static const GA_PointGroup*
        groupPromotePoint(
            GA_Detail* const geo,
            GA_Group*& group,
            const UT_StringHolder& newName,
            const bool delOriginal
        )
    {
        return static_cast<const GA_PointGroup*>(groupPromote(geo, group, GA_GROUP_POINT, newName, delOriginal));
    }

    SYS_FORCE_INLINE
        static const GA_VertexGroup*
        groupPromoteVertex(
            GA_Detail* const geo,
            GA_Group*& group,
            const UT_StringHolder& newName,
            const bool delOriginal
        )
    {
        return static_cast<const GA_VertexGroup*>(groupPromote(geo, group, GA_GROUP_VERTEX, newName, delOriginal));
    }

    SYS_FORCE_INLINE
        static const GA_EdgeGroup*
        groupPromoteEdge(
            GA_Detail* const geo,
            GA_Group*& group,
            const UT_StringHolder& newName,
            const bool delOriginal
        )
    {
        return static_cast<const GA_EdgeGroup*>(groupPromote(geo, group, GA_GROUP_EDGE, newName, delOriginal));
    }




    SYS_FORCE_INLINE
        static const GA_PrimitiveGroup*
        groupPromotePrimitiveDetached(
            GA_Detail* const geo,
            GA_Group*& group,
            const bool delOriginal
        )
    {
        return static_cast<const GA_PrimitiveGroup*>(groupPromoteDetached(geo, group, GA_GROUP_PRIMITIVE, delOriginal));
    }

    SYS_FORCE_INLINE
        static const GA_PointGroup*
        groupPromotePointDetached(
            GA_Detail* const geo,
            GA_Group*& group,
            const bool delOriginal
        )
    {
        return static_cast<const GA_PointGroup*>(groupPromoteDetached(geo, group, GA_GROUP_POINT, delOriginal));
    }

    SYS_FORCE_INLINE
        static const GA_VertexGroup*
        groupPromoteVertexDetached(
            GA_Detail* const geo,
            GA_Group*& group,
            const bool delOriginal
        )
    {
        return static_cast<const GA_VertexGroup*>(groupPromoteDetached(geo, group, GA_GROUP_VERTEX, delOriginal));
    }

    SYS_FORCE_INLINE
        static const GA_EdgeGroup*
        groupPromoteEdgeDetached(
            GA_Detail* const geo,
            GA_Group*& group,
            const bool delOriginal
        )
    {
        return static_cast<const GA_EdgeGroup*>(groupPromoteDetached(geo, group, GA_GROUP_EDGE, delOriginal));
    }




    //const UT_UniquePtr<const GA_PrimitiveGroup> geo0GroupUptr(geo0VtxGroup);
    SYS_FORCE_INLINE
        static GA_PrimitiveGroup*
        groupPromotePrimitiveDetached(
            const GA_Detail* const geo,
            const GA_Group* const group
        )
    {
        return static_cast<GA_PrimitiveGroup*>(groupPromoteDetached(geo, group, GA_GROUP_PRIMITIVE));
    }

    //const UT_UniquePtr<const GA_PointGroup> geo0GroupUptr(geo0VtxGroup);
    SYS_FORCE_INLINE
        static GA_PointGroup*
        groupPromotePointDetached(
            const GA_Detail* const geo,
            const GA_Group* const group
        )
    {
        return static_cast<GA_PointGroup*>(groupPromoteDetached(geo, group, GA_GROUP_POINT));
    }

    //const UT_UniquePtr<const GA_VertexGroup> geo0GroupUptr(geo0VtxGroup);
    SYS_FORCE_INLINE
        static GA_VertexGroup*
        groupPromoteVertexDetached(
            const GA_Detail* const geo,
            const GA_Group* const group
        )
    {
        return static_cast<GA_VertexGroup*>(groupPromoteDetached(geo, group, GA_GROUP_VERTEX));
    }

    //const UT_UniquePtr<const GA_EdgeGroup> geo0GroupUptr(geo0VtxGroup);
    SYS_FORCE_INLINE
        static GA_EdgeGroup*
        groupPromoteEdgeDetached(
            const GA_Detail* const geo,
            const GA_Group* const group
        )
    {
        return static_cast<GA_EdgeGroup*>(groupPromoteDetached(geo, group, GA_GROUP_EDGE));
    }




    //const UT_UniquePtr<const GA_PrimitiveGroup> geo0GroupUptr(geo0PrimGroup);
    SYS_FORCE_INLINE
        static const GA_PrimitiveGroup*
        groupFindPromotePrimitiveDetached(
            const GA_Detail* const geo,
            const GA_Group* const group
        )
    {
        return static_cast<const GA_PrimitiveGroup*>(groupFindPromoteDetached(geo, group, GA_GROUP_PRIMITIVE));
    }

    //const UT_UniquePtr<const GA_PointGroup> geo0GroupUptr(geo0PointGroup);
    SYS_FORCE_INLINE
        static const GA_PointGroup*
        groupFindPromotePointDetached(
            const GA_Detail* const geo,
            const GA_Group* const group
        )
    {
        return static_cast<const GA_PointGroup*>(groupFindPromoteDetached(geo, group, GA_GROUP_POINT));
    }

    //const UT_UniquePtr<const GA_VertexGroup> geo0GroupUptr(geo0VtxGroup);
    SYS_FORCE_INLINE
        static const GA_VertexGroup*
        groupFindPromoteVertexDetached(
            const GA_Detail* const geo,
            const GA_Group* const group
        )
    {
        return static_cast<const GA_VertexGroup*>(groupFindPromoteDetached(geo, group, GA_GROUP_VERTEX));
    }

    //const UT_UniquePtr<const GA_EdgeGroup> geo0GroupUptr(geo0EdgeGroup);
    SYS_FORCE_INLINE
        static const GA_EdgeGroup*
        groupFindPromoteEdgeDetached(
            const GA_Detail* const geo,
            const GA_Group* const group
        )
    {
        return static_cast<const GA_EdgeGroup*>(groupFindPromoteDetached(geo, group, GA_GROUP_EDGE));
    }













    SYS_FORCE_INLINE
        static const GA_ElementGroup*
        elementGroupFindPromoteDetached(
            const GA_Detail* const geo,
            const GA_Group* const group,
            const GA_GroupType newType
        )
    {
        return static_cast<const GA_ElementGroup*>(groupFindPromoteDetached(geo, group, newType));
    }

    SYS_FORCE_INLINE
        static const GA_ElementGroup*
        elementGroupFindPromotePrimitiveDetached(
            const GA_Detail* const geo,
            const GA_Group* const group
        )
    {
        return elementGroupFindPromoteDetached(geo, group, GA_GROUP_PRIMITIVE);
    }

    SYS_FORCE_INLINE
        static const GA_ElementGroup*
        elementGroupFindPromotePointDetached(
            const GA_Detail* const geo,
            const GA_Group* const group
        )
    {
        return elementGroupFindPromoteDetached(geo, group, GA_GROUP_POINT);
    }

    SYS_FORCE_INLINE
        static const GA_ElementGroup*
        elementGroupFindPromoteVertexDetached(
            const GA_Detail* const geo,
            const GA_Group* const group
        )
    {
        return elementGroupFindPromoteDetached(geo, group, GA_GROUP_VERTEX);
    }


    SYS_FORCE_INLINE
        static const GA_ElementGroup*
        elementGroupFindPromoteDetached(
            GA_Detail* const geo,
            GA_Group*& group,
            const GA_GroupType newType,
            const bool delOriginal
        )
    {
        return static_cast<const GA_ElementGroup*>(groupFindPromoteDetached(geo, group, newType, delOriginal));
    }

    SYS_FORCE_INLINE
        static const GA_ElementGroup*
        elementGroupFindPromotePrimitiveDetached(
            GA_Detail* const geo,
            GA_Group*& group,
            const bool delOriginal
        )
    {
        return elementGroupFindPromoteDetached(geo, group, GA_GROUP_PRIMITIVE, delOriginal);
    }

    SYS_FORCE_INLINE
        static const GA_ElementGroup*
        elementGroupFindPromotePointDetached(
            GA_Detail* const geo,
            GA_Group*& group,
            const bool delOriginal
        )
    {
        return elementGroupFindPromoteDetached(geo, group, GA_GROUP_POINT, delOriginal);
    }

    SYS_FORCE_INLINE
        static const GA_ElementGroup*
        elementGroupFindPromoteVertexDetached(
            GA_Detail* const geo,
            GA_Group*& group,
            const bool delOriginal
        )
    {
        return elementGroupFindPromoteDetached(geo, group, GA_GROUP_VERTEX, delOriginal);
    }





    SYS_FORCE_INLINE
        static const GA_ElementGroup*
        elementGroupPromoteDetached(
            const GA_Detail* const geo,
            const GA_Group* const group,
            const GA_GroupType newType
        )
    {
        return static_cast<const GA_ElementGroup*>(groupPromoteDetached(geo, group, newType));
    }

    SYS_FORCE_INLINE
        static const GA_ElementGroup*
        elementGroupPromotePrimitiveDetached(
            const GA_Detail* const geo,
            const GA_Group* const group
        )
    {
        return elementGroupPromoteDetached(geo, group, GA_GROUP_PRIMITIVE);
    }

    SYS_FORCE_INLINE
        static const GA_ElementGroup*
        elementGroupPromotePointDetached(
            const GA_Detail* const geo,
            const GA_Group* const group
        )
    {
        return elementGroupPromoteDetached(geo, group, GA_GROUP_POINT);
    }

    SYS_FORCE_INLINE
        static const GA_ElementGroup*
        elementGroupPromoteVertexDetached(
            const GA_Detail* const geo,
            const GA_Group* const group
        )
    {
        return elementGroupPromoteDetached(geo, group, GA_GROUP_VERTEX);
    }











} // End of namespace GA_FeE_GroupPromote

#endif
