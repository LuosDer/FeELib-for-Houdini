
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

//const GA_ElementGroup* geo0Group = GA_FeE_Group::parseGroupDetached(cookparms, outGeo0, groupType, sopparms.getGroup(), gop);




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






//GA_Range geo0Range = GA_FeE_Group::groupPromoteRange(outGeo0, geo0Group, groupType);
//using GA_ElementGroupUPtr = ::std::unique_ptr<GA_ElementGroup, ::std::default_delete<GA_ElementGroup>>;

static GA_Range
groupPromoteRange(
    const GA_Detail* geo,
    const GA_ElementGroup* group,
    const GA_GroupType& groupType
)
{
    if (!group)
    {
        switch (groupType)
        {
        case GA_GROUP_PRIMITIVE: return geo->getPrimitiveRange(nullptr); break;
        case GA_GROUP_POINT:     return geo->getPointRange(nullptr);     break;
        case GA_GROUP_VERTEX:    return geo->getVertexRange(nullptr);    break;
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
groupPromoteRange(
    const GA_Detail* geo,
    const GA_ElementGroup* group,
    const GA_AttributeOwner& attribOwner
    )
{
    return groupPromoteRange(geo, group, attributeOwner_groupType(attribOwner));
}


static GA_SplittableRange
groupPromoteSplittableRange(
    const GA_Detail* geo,
    const GA_ElementGroup* group,
    const GA_GroupType& groupType
)
{
    return GA_SplittableRange(groupPromoteRange(geo, group, groupType));
}

static GA_SplittableRange
groupPromoteSplittableRange(
    const GA_Detail* geo,
    const GA_ElementGroup* group,
    const GA_AttributeOwner& attribOwner
)
{
    return GA_SplittableRange(groupPromoteRange(geo, group, attribOwner));
}


static void
combineVertexFromEdgeGroup(
    const GA_Detail* outGeo0,
    const GA_VertexGroup* group,
    const GA_EdgeGroup* groupToCombine
)
{
    if (!group)
        return;
    UT_UniquePtr<GA_VertexGroup> groupDeleter;
    GA_VertexGroup* newDetachedGroup = new GA_VertexGroup(*outGeo0);
    groupDeleter.reset(newDetachedGroup);
    newDetachedGroup->combine(groupToCombine);
    group = newDetachedGroup;
}

/*
template<typename T, typename T1>
static void
combineGroup(
    const GA_Detail* outGeo0,
    const T* group,
    const T1* groupToCombine
)
{
    if (!group)
        return;
    UT_UniquePtr<T> groupDeleter;
    T* newDetachedGroup = new T(*outGeo0);
    groupDeleter.reset(newDetachedGroup);
    newDetachedGroup->combine(groupToCombine);
    group = newDetachedGroup;
}
*/



} // End of namespace GA_FeE_Group

#endif
