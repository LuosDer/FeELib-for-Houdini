
#pragma once

#ifndef __GFE_Type_h__
#define __GFE_Type_h__

//#include "GFE/GFE_Type.h"

#include "GA/GA_Detail.h"



#ifndef PI
#define PI 3.14159265358979323846
#endif

enum GFE_GroupMergeType
{
    GFE_GroupMerge_Replace,
    GFE_GroupMerge_Union,
    GFE_GroupMerge_Intersect,
    GFE_GroupMerge_Subtract,
};

enum GFE_PieceAttribSearchOrder
{
    GFE_PieceAttribSearchOrder_PRIM,
    GFE_PieceAttribSearchOrder_POINT,
    GFE_PieceAttribSearchOrder_VERTEX,
    GFE_PieceAttribSearchOrder_PRIMPOINT,
    GFE_PieceAttribSearchOrder_POINTPRIM,
    GFE_PieceAttribSearchOrder_ALL,
    GFE_PieceAttribSearchOrder_INVALID = -1,
};






namespace GFE_Type {


#ifndef GFE_TOPO_SCOPE

#if 0
#define GFE_TOPO_SCOPE GFE_SCOPE_PRIVATE
#else
#define GFE_TOPO_SCOPE GA_SCOPE_PUBLIC
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


SYS_FORCE_INLINE
static GA_Storage
getPreferredStorage(
    const GA_Precision precision,
    const GA_StorageClass storageClass
)
{
    switch (storageClass)
    {
    case GA_STORECLASS_REAL:
        return getPreferredStorageF(precision);
        break;
    case GA_STORECLASS_INT:
        return getPreferredStorageI(precision);
        break;
    default:
        return getPreferredStorageF(precision);
        break;
    }
}

SYS_FORCE_INLINE
static GA_Storage
getPreferredStorage(
    const GA_Detail* const geo,
    const GA_StorageClass storageClass
)
{
    return getPreferredStorage(geo->getPreferredPrecision(), storageClass);
}


SYS_FORCE_INLINE
static GA_Storage
getPreferredStorageF(
    const GA_Detail* const geo,
    const GA_Storage storage
)
{
    return storage == GA_STORE_INVALID ? GFE_Type::getPreferredStorageF(geo) : storage;
}

SYS_FORCE_INLINE
static GA_Storage
getPreferredStorageI(
    const GA_Detail* const geo,
    const GA_Storage storage
)
{
    return storage == GA_STORE_INVALID ? GFE_Type::getPreferredStorageI(geo) : storage;
}

SYS_FORCE_INLINE
static GA_Storage
getPreferredStorage(
    const GA_Detail* const geo,
    const GA_Storage storage,
    const GA_StorageClass storageClass
)
{
    return storage == GA_STORE_INVALID ? GFE_Type::getPreferredStorage(geo, storageClass) : storage;
}








} // End of namespace GFE_Type



















#endif
