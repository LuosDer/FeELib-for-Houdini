
#pragma once

#ifndef __GFE_Type_h__
#define __GFE_Type_h__

//#include "GFE/GFE_Type.h"

#include "GA/GA_Detail.h"


#if SYS_VERSION_MAJOR_INT > 19 || ( SYS_VERSION_MAJOR_INT == 19 && SYS_VERSION_MINOR_INT == 5 )
#else
#endif


#ifndef PI
#define PI 3.14159265358979323846
#endif

enum class GFE_GroupMergeType
{
    Replace,
    Union,
    Intersect,
    Subtract,
};

enum class GFE_AttribMergeType
{
    Set = 0,
    Add,
    Sub,
    Mult,
    Min,
    Max,
    Xor,
    Toggle,
    Append,
    Intersect
};


enum class GFE_StatisticalFunction
{
    Min,
    Max,
};




namespace GFE_Type {


#ifndef GFE_TOPO_SCOPE

#if 0
#define GFE_TOPO_SCOPE GFE_SCOPE_PRIVATE
#else
#define GFE_TOPO_SCOPE GA_SCOPE_PUBLIC
#endif


#endif

static GA_GroupType attributeOwner_groupType(const GA_AttributeOwner attribOwner)
{
    switch (attribOwner)
    {
    case GA_ATTRIB_PRIMITIVE:       return GA_GROUP_PRIMITIVE; break;
    case GA_ATTRIB_POINT:           return GA_GROUP_POINT;     break;
    case GA_ATTRIB_VERTEX:          return GA_GROUP_VERTEX;    break;
    case GA_ATTRIB_DETAIL:          return GA_GROUP_N;         break;
    case GA_ATTRIB_OWNER_N:         return GA_GROUP_N;         break;
    }
    UT_ASSERT_MSG(0, "Unhandled Group Type!");
    return GA_GROUP_INVALID;
}

static GA_AttributeOwner attributeOwner_groupType(const GA_GroupType groupType)
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

    
static GA_Precision precisionFromStorage(const GA_Storage storage)
{
    switch (storage)
    {
    case GA_STORE_INVALID:       return GA_PRECISION_INVALID; break;
    case GA_STORE_BOOL:          return GA_PRECISION_1;       break;
    case GA_STORE_UINT8:         return GA_PRECISION_8;       break;
    case GA_STORE_INT8:          return GA_PRECISION_8;       break;
    case GA_STORE_INT16:         return GA_PRECISION_16;      break;
    case GA_STORE_INT32:         return GA_PRECISION_32;      break;
    case GA_STORE_INT64:         return GA_PRECISION_64;      break;
    case GA_STORE_REAL16:        return GA_PRECISION_16;      break;
    case GA_STORE_REAL32:        return GA_PRECISION_32;      break;
    case GA_STORE_REAL64:        return GA_PRECISION_64;      break;
    case GA_STORE_STRING:        return GA_PRECISION_INVALID; break;
    case GA_STORE_DICT:          return GA_PRECISION_INVALID; break;
    // case GA_STORE_UTF8:          return GA_PRECISION_8;       break;
    // case GA_STORE_UTF16:         return GA_PRECISION_16;      break;
    // case GA_STORE_UTF32:         return GA_PRECISION_32;      break;
    }
    UT_ASSERT_MSG(0, "Unhandled Storage!");
    return GA_PRECISION_INVALID;
}

SYS_FORCE_INLINE static bool isValid(const GA_AttributeOwner attribOwner)
{
    return attribOwner == GA_ATTRIB_PRIMITIVE ||
           attribOwner == GA_ATTRIB_POINT     ||
           attribOwner == GA_ATTRIB_VERTEX    ||
           attribOwner == GA_ATTRIB_GLOBAL    ;
}

SYS_FORCE_INLINE static bool isValid(const GA_GroupType groupType)
{
    return groupType == GA_GROUP_PRIMITIVE ||
           groupType == GA_GROUP_POINT     ||
           groupType == GA_GROUP_VERTEX    ||
           groupType == GA_GROUP_EDGE;
}

SYS_FORCE_INLINE static bool isInvalid(const GA_AttributeOwner attribOwner)
{
    return attribOwner != GA_ATTRIB_PRIMITIVE &&
           attribOwner != GA_ATTRIB_POINT     &&
           attribOwner != GA_ATTRIB_VERTEX    &&
           attribOwner != GA_ATTRIB_GLOBAL;
}

SYS_FORCE_INLINE static bool isInvalid(const GA_GroupType groupType)
{
    return groupType != GA_GROUP_PRIMITIVE &&
           groupType != GA_GROUP_POINT     &&
           groupType != GA_GROUP_VERTEX    &&
           groupType != GA_GROUP_EDGE;
}


SYS_FORCE_INLINE static bool isElementGroup(const GA_AttributeOwner attribOwner)
{
    return attribOwner == GA_ATTRIB_PRIMITIVE || attribOwner == GA_ATTRIB_POINT || attribOwner == GA_ATTRIB_VERTEX;
}


SYS_FORCE_INLINE static bool isElementGroup(const GA_GroupType groupType)
{
    return groupType == GA_GROUP_PRIMITIVE || groupType == GA_GROUP_POINT || groupType == GA_GROUP_VERTEX;
}






static GA_Storage getPreferredStorageF(const GA_Precision precision)
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
    

static GA_Storage getPreferredStorageI(const GA_Precision precision)
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
    
SYS_FORCE_INLINE static GA_Storage getPreferredStorage(const GA_StorageClass storageClass, const GA_Precision precision)
{
    switch (storageClass)
    {
    case GA_STORECLASS_REAL:    return getPreferredStorageF(precision);  break;
    case GA_STORECLASS_INT:     return getPreferredStorageI(precision);  break;
    case GA_STORECLASS_STRING:  return GA_STORE_STRING;                  break;
    case GA_STORECLASS_DICT:    return GA_STORE_DICT;                    break;
    default:                    return getPreferredStorageF(precision);  break;
    }
}
    
SYS_FORCE_INLINE static GA_Storage getPreferredStorageI(const GA_Detail& geo)
{
    return getPreferredStorageI(geo.getPreferredPrecision());
}


SYS_FORCE_INLINE static GA_Storage getPreferredStorageF(const GA_Detail& geo)
{
    return getPreferredStorageF(geo.getPreferredPrecision());
}



SYS_FORCE_INLINE static GA_Storage getPreferredStorage(
    const GA_Detail& geo,
    const GA_StorageClass storageClass
)
{
    return getPreferredStorage(storageClass, geo.getPreferredPrecision());
}


SYS_FORCE_INLINE static GA_Storage getPreferredStorageF(
    const GA_Detail& geo,
    const GA_Storage storage
)
{
    return storage == GA_STORE_INVALID ? getPreferredStorageF(geo) : storage;
}

SYS_FORCE_INLINE static GA_Storage getPreferredStorageI(
    const GA_Detail& geo,
    const GA_Storage storage
)
{
    return storage == GA_STORE_INVALID ? getPreferredStorageI(geo) : storage;
}

SYS_FORCE_INLINE static GA_Storage getPreferredStorage(
    const GA_Detail& geo,
    const GA_StorageClass storageClass,
    const GA_Storage storage
)
{
    return storage == GA_STORE_INVALID ? getPreferredStorage(geo, storageClass) : storage;
}

SYS_FORCE_INLINE static GA_Storage getPreferredStorage(
    const GA_Detail* const geo,
    const GA_StorageClass storageClass,
    const GA_Storage storage
)
{
    return getPreferredStorage(*geo, storageClass, storage);
}








} // End of namespace GFE_Type



















#endif
