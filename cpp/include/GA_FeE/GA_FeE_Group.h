
#pragma once

#ifndef __GA_FeE_Group_h__
#define __GA_FeE_Group_h__

//#include <GA_FeE/GA_FeE_Group.h>

//#include <GA/GA_Detail.h>
#include <GEO/GEO_Detail.h>

#include <GU/GU_Detail.h>
#include <GEO/GEO_PrimPoly.h>
#include <OP/OP_Operator.h>
#include <OP/OP_OperatorTable.h>
#include <PRM/PRM_Include.h>
#include <PRM/PRM_TemplateBuilder.h>
#include <UT/UT_DSOVersion.h>
#include <UT/UT_Interrupt.h>
#include <UT/UT_StringHolder.h>
#include <SYS/SYS_Math.h>
#include <limits.h>


namespace GA_FeE_Group {




static GA_GroupType
    attributeOwner_groupType(
        const GA_AttributeOwner& attribOwner
    )
{
    switch (attribOwner)
    {
    case GA_ATTRIB_PRIMITIVE:       return GA_GROUP_PRIMITIVE; break;
    case GA_ATTRIB_POINT:           return GA_GROUP_POINT;     break;
    case GA_ATTRIB_VERTEX:          return GA_GROUP_VERTEX;    break;
    case GA_ATTRIB_OWNER_N:         return GA_GROUP_N;         break;
    }
    UT_ASSERT_MSG(0, "Unhandled Group Type!");
    return GA_GROUP_INVALID;
}

static GA_AttributeOwner
    attributeOwner_groupType(
        const GA_GroupType& groupType
    )
{
    switch (groupType)
    {
    case GA_GROUP_PRIMITIVE:       return GA_ATTRIB_PRIMITIVE; break;
    case GA_GROUP_POINT:           return GA_ATTRIB_POINT;     break;
    case GA_GROUP_VERTEX:          return GA_ATTRIB_VERTEX;    break;
    case GA_GROUP_EDGE:            return GA_ATTRIB_VERTEX;    break;
    case GA_GROUP_N:               return GA_ATTRIB_OWNER_N;   break;
    }
    UT_ASSERT_MSG(0, "Unhandled Group Type!");
    return GA_ATTRIB_INVALID;
}


static bool
isValid(
    const GA_AttributeOwner& attribOwner
)
{
    return attribOwner == GA_ATTRIB_PRIMITIVE ||
           attribOwner == GA_ATTRIB_POINT     ||
           attribOwner == GA_ATTRIB_VERTEX    ||
           attribOwner == GA_ATTRIB_GLOBAL    ;
}

static bool
isValid(
    const GA_GroupType& groupType
)
{
    return groupType == GA_GROUP_PRIMITIVE ||
           groupType == GA_GROUP_POINT     ||
           groupType == GA_GROUP_VERTEX    ||
           groupType == GA_GROUP_EDGE;
}

static bool
isInvalid(
    const GA_AttributeOwner& attribOwner
)
{
    return attribOwner != GA_ATTRIB_PRIMITIVE &&
           attribOwner != GA_ATTRIB_POINT     &&
           attribOwner != GA_ATTRIB_VERTEX    &&
           attribOwner != GA_ATTRIB_GLOBAL;
}

static bool
isInvalid(
    const GA_GroupType& groupType
)
{
    return groupType != GA_GROUP_PRIMITIVE &&
           groupType != GA_GROUP_POINT     &&
           groupType != GA_GROUP_VERTEX    &&
           groupType != GA_GROUP_EDGE;
}

//const GA_ElementGroup* geo0Group = GA_FeE_Group::parseGroupDetached(cookparms, geo, groupType, sopparms.getGroup(), gop);




static const GA_ElementGroup*
parseGroupDetached(
    const SOP_NodeVerb::CookParms& cookparms,
    const GEO_Detail* geo,
    const GA_GroupType& groupType,
    const UT_StringHolder& groupName,
    GOP_Manager& gop
)
{
    if (!groupName.length())
        return nullptr;
    
    if (!groupName.isstring())
    {
        cookparms.sopAddWarning(SOP_ERR_BADGROUP, groupName);
        return nullptr;
    }

    bool ok = true;
    const GA_Group* anyGroup = gop.parseGroupDetached(groupName, groupType, geo, true, false, ok);

    if (!ok || (anyGroup && !anyGroup->isElementGroup()))
    {
        cookparms.sopAddWarning(SOP_ERR_BADGROUP, groupName);
        return nullptr;
    }
    if (anyGroup && anyGroup->isElementGroup())
    {
        return UTverify_cast<const GA_ElementGroup*>(anyGroup);
    }
    return nullptr;
}


static const GA_ElementGroup*
parseGroupDetached(
    const SOP_NodeVerb::CookParms& cookparms,
    const GEO_Detail* geo,
    const GA_GroupType& groupType,
    const UT_StringHolder& groupName
)
{
    GOP_Manager gop;
    return GA_FeE_Group::parseGroupDetached(cookparms, geo, groupType, groupName, gop);
}






static const GA_EdgeGroup*
parseEdgeGroupDetached(
    const SOP_NodeVerb::CookParms& cookparms,
    const GEO_Detail* geo,
    const UT_StringHolder& groupName,
    GOP_Manager& gop
)
{
    if (!groupName.length())
        return nullptr;

    if (!groupName.isstring())
    {
        cookparms.sopAddWarning(SOP_ERR_BADGROUP, groupName);
        return nullptr;
    }

    bool ok = true;
    const GA_Group* anyGroup = gop.parseGroupDetached(groupName, GA_GROUP_EDGE, geo, true, false, ok);

    if (!ok || (anyGroup && !anyGroup->isElementGroup()))
    {
        cookparms.sopAddWarning(SOP_ERR_BADGROUP, groupName);
        return nullptr;
    }
    if (anyGroup && anyGroup->isElementGroup())
    {
        return UTverify_cast<const GA_EdgeGroup*>(anyGroup);
    }
    return nullptr;
}


static const GA_EdgeGroup*
parseEdgeGroupDetached(
    const SOP_NodeVerb::CookParms& cookparms,
    const GEO_Detail* geo,
    const UT_StringHolder& groupName
)
{
    GOP_Manager gop;
    return GA_FeE_Group::parseEdgeGroupDetached(cookparms, geo, groupName, gop);
}






//GA_Range geo0Range = GA_FeE_Group::groupPromoteRange(geo, geo0Group, groupType);
//using GA_ElementGroupUPtr = ::std::unique_ptr<GA_ElementGroup, ::std::default_delete<GA_ElementGroup>>;

static GA_Range
getRangeByAnyGroup(
    const GA_Detail* geo,
    const GA_ElementGroup* group,
    const GA_GroupType& groupType
)
{
    if (!group)
    {
        switch (groupType)
        {
        case GA_GROUP_PRIMITIVE: return geo->getPrimitiveRange(); break;
        case GA_GROUP_POINT:     return geo->getPointRange();     break;
        case GA_GROUP_VERTEX:    return geo->getVertexRange();    break;
        }
        UT_ASSERT_MSG(0, "Unhandled Group Type!");
        return GA_Range();
    }

    //if (groupType != GA_GROUP_PRIMITIVE &&
    //    groupType != GA_GROUP_POINT     &&
    //    groupType != GA_GROUP_VERTEX)
    //    return nullptr;

    const GA_GroupType& inGroupType = group->classType();

    //if (isInvalid(inGroupType))
    //    return nullptr;


    switch (groupType)
    {
    case GA_GROUP_PRIMITIVE:
    {
        const GA_PrimitiveGroup* promotedGroup = nullptr;
        if (groupType == GA_GROUP_PRIMITIVE)
            promotedGroup = static_cast<const GA_PrimitiveGroup*>(group);
        else
        {
            //GA_PrimitiveGroupUPtr groupUPtr = group->getDetail()->createDetachedPointGroup();
            GA_PrimitiveGroupUPtr groupUPtr;
            GA_PrimitiveGroup* newDetachedGroup = new GA_PrimitiveGroup(*geo);
            groupUPtr.reset(newDetachedGroup);
            newDetachedGroup->combine(group);
            promotedGroup = newDetachedGroup;
        }
        return geo->getPrimitiveRange(promotedGroup);
    }
        break;




    case GA_GROUP_POINT:
    {
        const GA_PointGroup* promotedGroup = nullptr;
        if (groupType == GA_GROUP_POINT)
            promotedGroup = static_cast<const GA_PointGroup*>(group);
        else
        {
            //GA_PrimitiveGroupUPtr groupUPtr = group->getDetail()->createDetachedPrimitiveGroup();
            GA_PointGroupUPtr groupUPtr;
            GA_PointGroup* newDetachedGroup = new GA_PointGroup(*geo);
            groupUPtr.reset(newDetachedGroup);
            newDetachedGroup->combine(group);
            promotedGroup = newDetachedGroup;
        }
        //return static_cast<GA_ElementGroup*>(promotedGroup);
        return geo->getPointRange(promotedGroup);
    }
    break;


    case GA_GROUP_VERTEX:
    {
        const GA_VertexGroup* promotedGroup = nullptr;
        if (groupType == GA_GROUP_VERTEX)
            promotedGroup = static_cast<const GA_VertexGroup*>(group);
        else
        {
            //GA_VertexGroupGroupUPtr groupUPtr = group->getDetail()->createDetachedVertexGroup();
            GA_VertexGroupUPtr groupUPtr;
            GA_VertexGroup* newDetachedGroup = new GA_VertexGroup(*geo);
            groupUPtr.reset(newDetachedGroup);
            newDetachedGroup->combine(group);
            promotedGroup = newDetachedGroup;
        }
        return geo->getVertexRange(promotedGroup);
    }
        break;
    }

    UT_ASSERT_MSG(0, "Unhandled Group Type!");
    return GA_Range();
}



static GA_Range
getRangeByAnyGroup(
    const GA_Detail* geo,
    const GA_ElementGroup* group,
    const GA_AttributeOwner& attribOwner
    )
{
    return getRangeByAnyGroup(geo, group, attributeOwner_groupType(attribOwner));
}


static GA_SplittableRange
getSplittableRangeByAnyGroup(
    const GA_Detail* geo,
    const GA_ElementGroup* group,
    const GA_GroupType& groupType
)
{
    return GA_SplittableRange(getRangeByAnyGroup(geo, group, groupType));
}

static GA_SplittableRange
getSplittableRangeByAnyGroup(
    const GA_Detail* geo,
    const GA_ElementGroup* group,
    const GA_AttributeOwner& attribOwner
)
{
    return GA_SplittableRange(getRangeByAnyGroup(geo, group, attribOwner));
}


//static void
//combineVertexFromEdgeGroup(
//    const GA_Detail* geo,
//    const GA_VertexGroup* group,
//    const GA_EdgeGroup* groupToCombine
//)
//{
//    if (!group)
//        return;
//    UT_UniquePtr<GA_VertexGroup> groupDeleter;
//    GA_VertexGroup* newDetachedGroup = new GA_VertexGroup(*geo);
//    groupDeleter.reset(newDetachedGroup);
//    newDetachedGroup->combine(groupToCombine);
//    group = newDetachedGroup;
//}



static GA_ElementGroup*
groupPromote(
    GA_Detail* geo,
    GA_ElementGroup* group,
    const GA_GroupType newType,
    const UT_StringHolder& newName,
    const bool internal = false,
    const bool delOriginal = true
)
{
    if (!geo || !group)
        return nullptr;

    if (group->classType() == newType)
        return group;

    GA_GroupTable* groupTable = geo->getGroupTable(newType);
    if (!groupTable)
        return nullptr;

    GA_ElementGroup* newGroup = static_cast<GA_ElementGroup*>(groupTable->newGroup(newName, internal));
    newGroup->combine(group);
    if (delOriginal)
    {
        delete group;
        group = nullptr;
    }
    return newGroup;
}

static GA_ElementGroup*
groupPromotePrimitive(
    GA_Detail* geo,
    GA_ElementGroup* group,
    const UT_StringHolder& newName,
    const bool internal = false,
    const bool delOriginal = true
)
{
    return groupPromote(geo, group, GA_GROUP_PRIMITIVE, newName, internal, delOriginal);
}

static GA_ElementGroup*
groupPromotePoint(
    GA_Detail* geo,
    GA_ElementGroup* group,
    const UT_StringHolder& newName,
    const bool internal = false,
    const bool delOriginal = true
)
{
    return groupPromote(geo, group, GA_GROUP_POINT, newName, internal, delOriginal);
}

static GA_ElementGroup*
groupPromoteVertex(
    GA_Detail* geo,
    GA_ElementGroup* group,
    const UT_StringHolder& newName,
    const bool internal = false,
    const bool delOriginal = true
)
{
    return groupPromote(geo, group, GA_GROUP_VERTEX, newName, internal, delOriginal);
}

static GA_ElementGroup*
groupPromoteEdge(
    GA_Detail* geo,
    GA_ElementGroup* group,
    const UT_StringHolder& newName,
    const bool internal = false,
    const bool delOriginal = true
)
{
    return groupPromote(geo, group, GA_GROUP_EDGE, newName, internal, delOriginal);
}
















static GA_Group*
groupPromote(
    GA_Detail* geo,
    GA_Group* group,
    const GA_GroupType newType,
    const UT_StringHolder& newName,
    const bool internal = false,
    const bool delOriginal = true
)
{
    if (!geo || !group)
        return nullptr;

    if (group->classType() == newType)
        return group;

    GA_GroupTable* groupTable = geo->getGroupTable(newType);
    if (!groupTable)
        return nullptr;


    GA_Group* newGroup = groupTable->newGroup(newName, internal);
    newGroup->combine(group);
    if (delOriginal)
    {
        delete group;
        group = nullptr;
    }
    return newGroup;
}

static GA_Group*
groupPromotePrimitive(
    GA_Detail* geo,
    GA_Group* group,
    const UT_StringHolder& newName,
    const bool internal = false,
    const bool delOriginal = true
)
{
    return groupPromote(geo, group, GA_GROUP_PRIMITIVE, newName, internal, delOriginal);
}

static GA_Group*
groupPromotePoint(
    GA_Detail* geo,
    GA_Group* group,
    const UT_StringHolder& newName,
    const bool internal = false,
    const bool delOriginal = true
)
{
    return groupPromote(geo, group, GA_GROUP_POINT, newName, internal, delOriginal);
}

static GA_Group*
groupPromoteVertex(
    GA_Detail* geo,
    GA_Group* group,
    const UT_StringHolder& newName,
    const bool internal = false,
    const bool delOriginal = true
)
{
    return groupPromote(geo, group, GA_GROUP_VERTEX, newName, internal, delOriginal);
}

static GA_Group*
groupPromoteEdge(
    GA_Detail* geo,
    GA_Group* group,
    const UT_StringHolder& newName,
    const bool internal = false,
    const bool delOriginal = true
)
{
    return groupPromote(geo, group, GA_GROUP_EDGE, newName, internal, delOriginal);
}




static UT_UniquePtr<GA_ElementGroup>
groupPromoteDetached(
    const GA_Detail* geo,
    const GA_ElementGroup* group,
    const GA_AttributeOwner newType
)
{
    if (!geo || !group)
        return nullptr;
    UT_UniquePtr<GA_ElementGroup> groupDeleter;
    GA_ElementGroup* newDetachedGroup = new GA_ElementGroup(*geo, newType);
    groupDeleter.reset(newDetachedGroup);
    newDetachedGroup->combine(group);
    return groupDeleter;
}

static UT_UniquePtr<GA_ElementGroup>
groupPromotePrimitiveDetached(
    const GA_Detail* geo,
    const GA_ElementGroup* group
)
{
    return groupPromoteDetached(geo, group, GA_ATTRIB_PRIMITIVE);
}

static UT_UniquePtr<GA_ElementGroup>
groupPromotePointDetached(
    const GA_Detail* geo,
    const GA_ElementGroup* group
)
{
    return groupPromoteDetached(geo, group, GA_ATTRIB_POINT);
}

static UT_UniquePtr<GA_ElementGroup>
groupPromoteVertexDetached(
    const GA_Detail* geo,
    const GA_ElementGroup* group
)
{
    return groupPromoteDetached(geo, group, GA_ATTRIB_VERTEX);
}




//static UT_UniquePtr<GA_PrimitiveGroup>
//groupPromotePrimitiveDetached(
//    const GA_Detail* geo,
//    const GA_ElementGroup* group
//)
//{
//    if (!group)
//        return nullptr;
//    UT_UniquePtr<GA_PrimitiveGroup> groupDeleter;
//    GA_PrimitiveGroup* newDetachedGroup = new GA_PrimitiveGroup(*geo);
//    groupDeleter.reset(newDetachedGroup);
//    newDetachedGroup->combine(group);
//    return groupDeleter;
//}
//
//static UT_UniquePtr<GA_PointGroup>
//groupPromotePointDetached(
//    const GA_Detail* geo,
//    const GA_ElementGroup* group
//)
//{
//    if (!group)
//        return nullptr;
//    UT_UniquePtr<GA_PointGroup> groupDeleter;
//    GA_PointGroup* newDetachedGroup = new GA_PointGroup(*geo);
//    groupDeleter.reset(newDetachedGroup);
//    newDetachedGroup->combine(group);
//    return groupDeleter;
//}
//
//static UT_UniquePtr<GA_VertexGroup>
//groupPromoteVertexDetached(
//    const GA_Detail* geo,
//    const GA_ElementGroup* group
//)
//{
//    if (!group)
//        return nullptr;
//    UT_UniquePtr<GA_VertexGroup> groupDeleter;
//    GA_VertexGroup* newDetachedGroup = new GA_VertexGroup(*geo);
//    groupDeleter.reset(newDetachedGroup);
//    newDetachedGroup->combine(group);
//    return groupDeleter;
//}





} // End of namespace GA_FeE_Group

#endif
