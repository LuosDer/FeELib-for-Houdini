
#pragma once

#ifndef __GA_FeE_Type_h__
#define __GA_FeE_Type_h__

//#include "GA_FeE/GA_FeE_Type.h"

#include "GA/GA_Detail.h"



#ifndef PI
#define PI 3.14159265358979323846
#endif

enum GA_GroupMergeType
{
    GA_GroupMerge_Replace,
    GA_GroupMerge_Union,
    GA_GroupMerge_Intersect,
    GA_GroupMerge_Subtract,
};


namespace GA_FeE_Type {


#ifndef GA_FEE_TOPO_SCOPE

#if 0
#define GA_FEE_TOPO_SCOPE GA_FEE_SCOPE_PRIVATE
#else
#define GA_FEE_TOPO_SCOPE GA_SCOPE_PUBLIC
#endif


#endif

static GA_GroupType
attributeOwner_groupType(
    const GA_AttributeOwner attribOwner
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
    const GA_GroupType groupType
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


SYS_FORCE_INLINE
static bool
isValid(
    const GA_AttributeOwner attribOwner
)
{
    return attribOwner == GA_ATTRIB_PRIMITIVE ||
           attribOwner == GA_ATTRIB_POINT     ||
           attribOwner == GA_ATTRIB_VERTEX    ||
           attribOwner == GA_ATTRIB_GLOBAL    ;
}

SYS_FORCE_INLINE
static bool
isValid(
    const GA_GroupType groupType
)
{
    return groupType == GA_GROUP_PRIMITIVE ||
           groupType == GA_GROUP_POINT     ||
           groupType == GA_GROUP_VERTEX    ||
           groupType == GA_GROUP_EDGE;
}

SYS_FORCE_INLINE
static bool
isInvalid(
    const GA_AttributeOwner attribOwner
)
{
    return attribOwner != GA_ATTRIB_PRIMITIVE &&
           attribOwner != GA_ATTRIB_POINT     &&
           attribOwner != GA_ATTRIB_VERTEX    &&
           attribOwner != GA_ATTRIB_GLOBAL;
}

SYS_FORCE_INLINE
static bool
isInvalid(
    const GA_GroupType groupType
)
{
    return groupType != GA_GROUP_PRIMITIVE &&
           groupType != GA_GROUP_POINT     &&
           groupType != GA_GROUP_VERTEX    &&
           groupType != GA_GROUP_EDGE;
}


SYS_FORCE_INLINE
static bool
isElementGroup(
    const GA_AttributeOwner attribOwner
)
{
    return attribOwner == GA_ATTRIB_PRIMITIVE || attribOwner == GA_ATTRIB_POINT || attribOwner == GA_ATTRIB_VERTEX;
}


SYS_FORCE_INLINE
static bool
isElementGroup(
    const GA_GroupType groupType
)
{
    return groupType == GA_GROUP_PRIMITIVE || groupType == GA_GROUP_POINT || groupType == GA_GROUP_VERTEX;
}







static GA_Storage
getPreferredStorageI(
    const GA_Precision precision
)
{
    switch (precision)
    {
    case GA_PRECISION_8:    return  GA_STORE_INT8;    break;
    case GA_PRECISION_16:   return  GA_STORE_INT16;   break;
    case GA_PRECISION_32:   return  GA_STORE_INT32;   break;
    case GA_PRECISION_64:   return  GA_STORE_INT64;   break;
    default:
        break;
    }
    UT_ASSERT_MSG(0, "Unhandled Precision!");
    return GA_STORE_INVALID;
}

SYS_FORCE_INLINE
static GA_Storage
getPreferredStorageI(
    const GA_Detail* const geo
)
{
    return getPreferredStorageI(geo->getPreferredPrecision());
}



static GA_Storage
getPreferredStorageF(
    const GA_Precision precision
)
{
    switch (precision)
    {
    case GA_PRECISION_16:        return  GA_STORE_REAL16;   break;
    case GA_PRECISION_32:        return  GA_STORE_REAL32;   break;
    case GA_PRECISION_64:        return  GA_STORE_REAL64;   break;
    default:                     break;
    }
    UT_ASSERT_MSG(0, "Unhandled Precision!");
    return GA_STORE_INVALID;
}

SYS_FORCE_INLINE
static GA_Storage
getPreferredStorageF(
    const GA_Detail* const geo
)
{
    return getPreferredStorageF(geo->getPreferredPrecision());
}



template<typename T>
SYS_FORCE_INLINE
static T
radians(
    const T degrees
)
{
    return degrees * PI / 180;
}

template<typename T>
SYS_FORCE_INLINE
static T
degrees(
    const T radians
)
{
    return radians * 180 / PI;
}









} // End of namespace GA_FeE_Type

#endif
