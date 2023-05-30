
#pragma once

#ifndef __GFE_Group_h__
#define __GFE_Group_h__

//#include "GFE/GFE_Group.h"

#include "GA/GA_Detail.h"
#include "GA/GA_SplittableRange.h"


#include "GFE/GFE_Type.h"


class GFE_SetGroup
{
public:
    GFE_SetGroup() {}
    
    GFE_SetGroup(GA_Group* inGroupPtr)
    { operator=(inGroupPtr); }
    
    GFE_SetGroup(GA_ElementGroup* groupPtr)   : elementGroupPtr(groupPtr) {}
    GFE_SetGroup(GA_PrimitiveGroup* groupPtr) : elementGroupPtr(static_cast<GA_ElementGroup*>(groupPtr)) {}
    GFE_SetGroup(GA_PointGroup* groupPtr)     : elementGroupPtr(static_cast<GA_ElementGroup*>(groupPtr)) {}
    GFE_SetGroup(GA_VertexGroup* groupPtr)    : elementGroupPtr(static_cast<GA_ElementGroup*>(groupPtr)) {}
    GFE_SetGroup(GA_EdgeGroup* groupPtr)      : edgeGroupPtr(groupPtr) {}

    ~GFE_SetGroup(){}

	
    void
        setComputeParm(
            const GFE_GroupMergeType groupMergeType = GFE_GroupMergeType::Replace,
            const bool reverseGroup = false
        )
    {
        this->groupMergeType = groupMergeType;
        this->reverseGroup = reverseGroup;
    }

    
    GFE_SetGroup& operator=(GA_Group* const inGroupPtr)
    {
        if (inGroupPtr->isElementGroup())
            elementGroupPtr = static_cast<GA_ElementGroup*>(inGroupPtr);
        else
            edgeGroupPtr    = static_cast<GA_EdgeGroup*>(inGroupPtr);
        return *this;
    }

    SYS_FORCE_INLINE GFE_SetGroup& operator=(GA_ElementGroup* const inGroupPtr)
    {
        elementGroupPtr = inGroupPtr;
        return *this;
    }

    SYS_FORCE_INLINE GFE_SetGroup& operator=(GA_PrimitiveGroup* const inGroupPtr)
    {
        elementGroupPtr = static_cast<GA_ElementGroup*>(inGroupPtr);
        return *this;
    }

    SYS_FORCE_INLINE GFE_SetGroup& operator=(GA_PointGroup* const inGroupPtr)
    {
        elementGroupPtr = static_cast<GA_ElementGroup*>(inGroupPtr);
        return *this;
    }

    SYS_FORCE_INLINE GFE_SetGroup& operator=(GA_VertexGroup* const inGroupPtr)
    {
        elementGroupPtr = static_cast<GA_ElementGroup*>(inGroupPtr);
        return *this;
    }

    SYS_FORCE_INLINE GFE_SetGroup& operator=(GA_EdgeGroup* const inGroupPtr)
    {
        edgeGroupPtr = inGroupPtr;
        return *this;
    }


    
    SYS_FORCE_INLINE void setParm(const bool reverseGroup, const GFE_GroupMergeType groupMergeType)
    {
        this->reverseGroup = reverseGroup;
        this->groupMergeType = groupMergeType;
    }

    
    void set(const GA_Offset elemoff, bool b)
    {
        UT_ASSERT_P(elementGroupPtr);

        b ^= reverseGroup;
        
        switch (groupMergeType)
        {
        case GFE_GroupMergeType::Union:     b |=  elementGroupPtr->contains(elemoff); break;
        case GFE_GroupMergeType::Intersect: b &=  elementGroupPtr->contains(elemoff); break;
        case GFE_GroupMergeType::Subtract:  b &= !elementGroupPtr->contains(elemoff); break;
        }
        elementGroupPtr->setElement(elemoff, b);
    }
    
    SYS_FORCE_INLINE void set(const GA_Offset elemoff0, const GA_Offset elemoff1, bool b)
    {
#if 1
        set(GA_Edge(elemoff0, elemoff1), b);
#else
        UT_ASSERT_P(edgeGroupPtr);
        
        b ^= reverseGroup;
        
        switch (groupMergeType)
        {
        case GFE_GroupMergeType::Union:     b |=  edgeGroupPtr->contains(elemoff0, elemoff1); break;
        case GFE_GroupMergeType::Intersect: b &=  edgeGroupPtr->contains(elemoff0, elemoff1); break;
        case GFE_GroupMergeType::Subtract:  b &= !edgeGroupPtr->contains(elemoff0, elemoff1); break;
        // case GFE_GroupMergeType::Replace: break;
        default: break;
        }
        if (b)
            edgeGroupPtr->add(elemoff0, elemoff1);
        else
            edgeGroupPtr->remove(elemoff0, elemoff1);
#endif
    }
    
    void set(const GA_Edge& edge, bool b)
    {
        UT_ASSERT_P(edgeGroupPtr);
        
        b ^= reverseGroup;
        
        switch (groupMergeType)
        {
        case GFE_GroupMergeType::Union:     b |=  edgeGroupPtr->contains(edge); break;
        case GFE_GroupMergeType::Intersect: b &=  edgeGroupPtr->contains(edge); break;
        case GFE_GroupMergeType::Subtract:  b &= !edgeGroupPtr->contains(edge); break;
        // case GFE_GroupMergeType::Replace:
        //default: break;
        }
        if (b)
            edgeGroupPtr->add(edge);
        else
            edgeGroupPtr->remove(edge);
    }
    
    
    
public:
    
    GFE_GroupMergeType groupMergeType = GFE_GroupMergeType::Replace;
    bool reverseGroup = false;

    
private:
    GA_ElementGroup* elementGroupPtr = nullptr;
    GA_EdgeGroup* edgeGroupPtr = nullptr;
    
}; // End of Class GFE_SetGroup

















namespace GFE_Group {

    

SYS_FORCE_INLINE static GA_AttributeOwner getOwner(const GA_Group& group)
{ return GFE_Type::attributeOwner_groupType(group.classType()); }

SYS_FORCE_INLINE static GA_AttributeOwner getOwner(const GA_Group* const group)
{
    if (!group)
        return GA_ATTRIB_INVALID;
    return getOwner(*group);
}

SYS_FORCE_INLINE static void groupBumpDataId(GA_Group& group)
{
    if (group.classType() == GA_GROUP_EDGE)
        static_cast<GA_EdgeGroup&>(group).bumpDataId();
    else
        static_cast<GA_ElementGroup&>(group).bumpDataId();
}

SYS_FORCE_INLINE static void groupBumpDataId(GA_Group* group)
{ groupBumpDataId(*group); }


static void groupBumpDataId(GA_GroupTable& groupTable, const char* groupPattern)
{
    if (!groupPattern || groupPattern == "")
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








static void groupToggle(GA_EdgeGroup& group)
{
    group.toggle();
    GA_Offset start, end;
    for (GA_Iterator it(group.getDetail().getPointRange()); it.fullBlockAdvance(start, end); )
    {
        for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
        {
            group.remove(GA_Edge(elemoff, elemoff));
        }
    }
}
SYS_FORCE_INLINE static void groupToggle(GA_ElementGroup& group)
{ group.toggleAll(group.getIndexMap().indexSize()); }

SYS_FORCE_INLINE static void groupToggle(GA_Group& group)
{
    return group.isElementGroup() ?
        groupToggle(static_cast<GA_ElementGroup&>(group)) :
        groupToggle(static_cast<GA_EdgeGroup&>(group));
}


void groupToggle(const GA_GroupTable& groupTable, const char* groupName)
{
    for (GA_GroupTable::iterator<GA_Group> it = groupTable.beginTraverse(); !it.atEnd(); ++it)
    {
        GA_Group& groupPtr = *it.group();
        if (!groupPtr.getName().multiMatch(groupName))
            continue;
        groupToggle(groupPtr);
    }
}






void delStdGroup(GA_GroupTable& groupTable, const char* groupName)
{
    for (GA_GroupTable::iterator<GA_Group> it = groupTable.beginTraverse(); !it.atEnd(); ++it)
    {
        GA_Group& groupPtr = *it.group();
        if (!groupPtr.getName().multiMatch(groupName))
            continue;
        groupTable.destroy(&groupPtr);
    }
}

static void keepStdGroup(GA_GroupTable& groupTable, const char* pattern)
{
    if (!pattern || pattern == "*")
        return;
    for (GA_GroupTable::iterator<GA_Group> it = groupTable.beginTraverse(); !it.atEnd(); ++it)
    {
        GA_Group* const group = it.group();
        //if (group->isDetached())
        //    continue;
        if (group->getName().multiMatch(pattern))
            continue;
        groupTable.destroy(group);
    }
}




SYS_FORCE_INLINE static bool groupIsEmpty(const GA_Group& group)
{
    return group.classType()==GA_GROUP_EDGE ?
           static_cast<const GA_EdgeGroup&>(group).isEmpty() :
           static_cast<const GA_ElementGroup&>(group).isEmpty();
}








//
// SYS_FORCE_INLINE static void groupBumpDataId(
//     GA_Detail& geo, const GA_GroupType groupType, const UT_StringRef& groupPattern
//     )
// { return groupBumpDataId(*geo.getGroupTable(groupType), groupPattern); }


//
// SYS_FORCE_INLINE
// static void
// delStdGroup(
//     GA_Detail& geo,
//     const GA_GroupType groupType,
//     const UT_StringRef& groupPattern
// )
// { return delStdGroup(*geo.getGroupTable(groupType), groupPattern); }
//
//
// static void
// delStdGroup(
//     GA_Detail& geo,
//     const UT_StringRef& primGroupPattern,
//     const UT_StringRef& pointGroupPattern,
//     const UT_StringRef& vertexGroupPattern,
//     const UT_StringRef& edgeGroupPattern
// )
// {
//     delStdGroup(geo, GA_GROUP_PRIMITIVE, primGroupPattern);
//     delStdGroup(geo, GA_GROUP_POINT,     pointGroupPattern);
//     delStdGroup(geo, GA_GROUP_VERTEX,    vertexGroupPattern);
//     delStdGroup(geo, GA_GROUP_EDGE,      edgeGroupPattern);
// }



//
//
// static void keepStdGroup(GA_GroupTable& groupTable, const UT_StringRef& keepGroupPattern)
// {
//     if (keepGroupPattern == "*")
//         return;
//     for (GA_GroupTable::iterator<GA_Group> it = groupTable.beginTraverse(); !it.atEnd(); ++it)
//     {
//         GA_Group* const group = it.group();
//         //if (group->isDetached())
//         //    continue;
//         if (group->getName().multiMatch(keepGroupPattern))
//             continue;
//         groupTable.destroy(group);
//     }
// }


//
// SYS_FORCE_INLINE
// static void
// keepStdGroup(
//     GA_Detail& geo,
//     const GA_GroupType groupTable,
//     const UT_StringRef& keepGroupPattern
// )
// {
//     return keepStdGroup(*geo.getGroupTable(groupTable), keepGroupPattern);
// }
//
//
// static void
// keepStdGroup(
//     GA_Detail& geo,
//     const UT_StringRef& primGroupPattern,
//     const UT_StringRef& pointGroupPattern,
//     const UT_StringRef& vertexGroupPattern,
//     const UT_StringRef& edgeGroupPattern
// )
// {
//     keepStdGroup(geo, GA_GROUP_PRIMITIVE, primGroupPattern);
//     keepStdGroup(geo, GA_GROUP_POINT,     pointGroupPattern);
//     keepStdGroup(geo, GA_GROUP_VERTEX,    vertexGroupPattern);
//     keepStdGroup(geo, GA_GROUP_EDGE,      edgeGroupPattern);
// }
//




//
// static GA_Group&
// groupDuplicate(
//     GA_Detail& geo,
//     const GA_Group& group,
//     const UT_StringRef& groupName
// )
// {
//     GA_Group& newGroup = *geo.getGroupTable(group.classType())->newGroup(groupName);
//     if (group.isElementGroup())
//         static_cast<GA_ElementGroup&>(newGroup).combine(&group);
//     else
//         static_cast<GA_EdgeGroup&>(newGroup).combine(&group);
//     return newGroup;
// }
//
//
// static GA_Group&
// groupDuplicate(
//     const GA_Detail& geo,
//     const GA_Group& group
// )
// {
//     GA_Group& newGroup = *geo.getGroupTable(group.classType())->newDetachedGroup();
//     if (group.isElementGroup())
//         static_cast<GA_ElementGroup&>(newGroup).combine(&group);
//     else
//         static_cast<GA_EdgeGroup&>(newGroup).combine(&group);
//     return newGroup;
// }
//
//
//
// SYS_FORCE_INLINE static GA_Group& groupDuplicateDetached(const GA_Detail& geo,const GA_Group& group)
// { return groupDuplicate(geo, group); }
//
//
//
//
//

//
// SYS_FORCE_INLINE static void elementGroupToggle(GA_ElementGroup& group)
// { groupToggle(group); }
//
// SYS_FORCE_INLINE static void elementGroupToggle(GA_Group& group)
// { elementGroupToggle(static_cast<GA_ElementGroup&>(group)); }
//
// static void groupToggle(GA_Group& group)
// {
//     if (group.isElementGroup())
//     {
//         return elementGroupToggle(group);
//     }
//     else
//     {
//         return edgeGroupToggle(static_cast<GA_EdgeGroup&>(group));
//     }
// }
//
// SYS_FORCE_INLINE static void groupToggle(const GA_Detail& geo, GA_Group& group)
// {
//     if (group.isElementGroup())
//     {
//         return elementGroupToggle(group);
//     }
//     else
//     {
//         return edgeGroupToggle(geo, static_cast<GA_EdgeGroup&>(group));
//     }
// }
//
//
// static void
// groupToggle(
//     const GA_Detail& geo,
//     const GA_GroupType groupType,
//     const UT_StringRef& groupName
// )
// {
//     for (GA_GroupTable::iterator<GA_Group> it = geo.getGroupTable(groupType)->beginTraverse(); !it.atEnd(); ++it)
//     {
//         GA_Group& groupPtr = *it.group();
//         //if (groupPtr->isDetached())
//         //    continue;
//         if (!groupPtr.getName().multiMatch(groupName))
//             continue;
//         groupToggle(geo, groupPtr);
//         groupBumpDataId(groupPtr);
//     }
// }
//














//
//
// SYS_FORCE_INLINE static GA_Group* newGroup(
//         GA_Detail& geo,
//         const GA_Group& group,
//         const UT_StringRef& groupName
//     )
// {
//     return geo.getGroupTable(group.classType())->newGroup(groupName);
// }
//
//
// SYS_FORCE_INLINE
//     static GA_Group*
//     newDetachedGroup(
//         const GA_Detail& geo,
//         const GA_Group& group
//     )
// {
//     return geo.getGroupTable(group.classType())->newDetachedGroup();
// }
//
//
//
//
//
// static GA_Group*
// findGroupBase(
//     const GA_Detail& geo,
//     const GA_GroupType groupType,
//     const UT_StringRef& groupName
// )
// {
//     const GA_GroupTable* const groupTable = geo.getGroupTable(groupType);
//     if (!groupTable)
//         return nullptr;
//     return groupTable->find(groupName);
// }
//
// static GA_Group*
// findGroupN(
//     const GA_Detail& geo,
//     const UT_StringRef& groupName
// )
// {
//     GA_Group* outGroup = findGroupBase(geo, GA_GROUP_PRIMITIVE, groupName);
//     if (outGroup)
//         return outGroup;
//     outGroup = findGroupBase(geo, GA_GROUP_POINT, groupName);
//     if (outGroup)
//         return outGroup;
//     outGroup = findGroupBase(geo, GA_GROUP_VERTEX, groupName);
//     if (outGroup)
//         return outGroup;
//     return findGroupBase(geo, GA_GROUP_EDGE, groupName);
// }
//
// static GA_Group*
// findGroup(
//     const GA_Detail& geo,
//     const GA_GroupType groupType,
//     const UT_StringRef& groupName
// )
// {
//     if (groupType == GA_GROUP_N)
//     {
//         return findGroupN(geo, groupName);
//     }
//     else
//     {
//         return findGroupBase(geo, groupType, groupName);
//     }
// }


//
//
// static GA_Group*
// findOrCreateGroup(
//     GA_Detail& geo,
//     const GA_GroupType groupType,
//     const UT_StringRef& groupName
// )
// {
//     GA_GroupTable* const groupTable = geo.getGroupTable(groupType);
//     if (!groupTable)
//         return nullptr;
//     GA_Group* const group = groupTable->find(groupName);
//     if (!group)
//         return groupTable->newGroup(groupName);
//     return nullptr;
// }




// SYS_FORCE_INLINE
// static GA_ElementGroup*
// findElementGroup(
//     GA_Detail& geo,
//     const GA_GroupType groupType,
//     const UT_StringRef& groupName
// )
// {
//     UT_ASSERT_P(groupType != GA_GROUP_EDGE);
//     return geo.findElementGroup(GFE_Type::attributeOwner_groupType(groupType), groupName);
// }

// SYS_FORCE_INLINE static const GA_ElementGroup*
// findElementGroup(
//     const GA_Detail& geo,
//     const GA_GroupType groupType,
//     const UT_StringRef& groupName
// )
// {
//     UT_ASSERT_P(groupType != GA_GROUP_EDGE);
//     return geo.findElementGroup(GFE_Type::attributeOwner_groupType(groupType), groupName);
// }
// 
// 
// 
// SYS_FORCE_INLINE bool groupRename(GA_Detail& geo, const GA_Group& group, const UT_StringRef& newName)
// {
//     if (group.getName() == newName)
//         return false;
//     return geo.getGroupTable(group.classType())->renameGroup(group.getName(), newName);
// }


    
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









//
// SYS_FORCE_INLINE static GA_Range getRangeByAnyGroup(const GA_Detail& geo, const GA_ElementGroup* const group)
// {
//     if (!group)
//         return GA_Range();
//
//     const GA_AttributeOwner attribOwner = GFE_Type::attributeOwner_groupType(group->classType());
//     return GA_Range(geo.getIndexMap(attribOwner), group);
// }
//
// SYS_FORCE_INLINE static GA_Range getRangeByAnyGroup(const GA_Detail& geo, const GA_Group* const group)
// { return getRangeByAnyGroup(geo, static_cast<const GA_ElementGroup*>(group)); }
//
// SYS_FORCE_INLINE static GA_SplittableRange getSplittableRangeByAnyGroup(const GA_Detail& geo, const GA_ElementGroup* const group)
// { return GA_SplittableRange(getRangeByAnyGroup(geo, group)); }
//
// SYS_FORCE_INLINE static GA_SplittableRange getSplittableRangeByAnyGroup(const GA_Detail& geo, const GA_Group* const group)
// { return getSplittableRangeByAnyGroup(geo, static_cast<const GA_ElementGroup*>(group)); }
























} // End of namespace GFE_Group

#endif
