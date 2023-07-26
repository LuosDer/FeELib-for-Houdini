
#pragma once

#ifndef __GFE_Type_h__
#define __GFE_Type_h__

#include "GFE/GFE_Type.h"

#include "GA/GA_Detail.h"


#include "GU/GU_Snap.h"

#ifdef NDEBUG
#define GFE_DEBUG_MODE false
#else
#define GFE_DEBUG_MODE true
#endif

#define GFE_MAX_LOOP_COUNT 1e10
#define GFE_FIND_INVALID_INDEX -1





#define GFE_INVALID_OFFSET32 2147483647
#define GFE_INVALID_OFFSET64 9223372036854775807


#if 1

#define GFE_INVALID_OFFSET GFE_INVALID_OFFSET32

#else

//#define GFE_INVALID_OFFSET numeric_limits<GA_Offset>::max()
using GFE_INVALID_OFFSET = numeric_limits<GA_Offset>::max();

#if GA_Offset == int64
#define GFE_INVALID_OFFSET GFE_INVALID_OFFSET64
#else
#define GFE_INVALID_OFFSET GFE_INVALID_OFFSET32
#endif

#endif








#if SYS_VERSION_MAJOR_INT > 19 || ( SYS_VERSION_MAJOR_INT == 19 && SYS_VERSION_MINOR_INT == 5 )
#else
#endif


#ifndef PI
#define PI 3.14159265358979323846
#endif

enum class GFE_AttribMergeMethod
{
    First,
    Last,
    Min,
    Max,
    Mode,
    Mean,
    Median,
    Sum,
    SumSquare,
    RootMeanSquare,
};

enum class GFE_GroupMergeMethod
{
    Replace,
    First,
    Last,
    Min,
    Max,
    Mode,
};


enum class GFE_GroupMergeType
{
    Replace,
    Union,
    Intersect,
    Subtract,
};

enum class GFE_AttribMergeType
{
    Set,
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
    Mode,
    Mean,
    Median,
    Sum,
    SumSquare,
    RootMeanSquare,
};


enum class GFE_ElemTraversingMethod
{
    Custom,
    OneElem,
    SkipNElem,
};

enum class GFE_Axis
{
    X,
    Y,
    Z,
    Invalid = -1,
};

enum class GFE_CurveEndsType
{
    Ends,
    Start,
    End,
};

#if 0

switch (curveEndsType)
{
case GFE_CurveEndsType::Ends:  break;
case GFE_CurveEndsType::Start: break;
case GFE_CurveEndsType::End:   break;
default: UT_ASSERT_MSG(0, "Unhandled GFE_CurveEndsType"); break;
}

#endif


enum class GFE_OutArrayType
{
    Attrib,
    Packed,
    Geo,
};



enum class GFE_ScaleAxis
{
    X,
    Y,
    Z,
    XYZMin,
    XYZMiddle,
    XYZMax,
    XYMin,
    YZMin,
    ZXMin,
    XYMax,
    YZMax,
    ZXMax,
    Invalid = -1,
};


namespace GFE_Type {

// #ifndef fpreal16
// #define fpreal16 float
// #endif
    
template <class _Ty>
_INLINE_VAR constexpr bool isScalar = std::is_same_v<_Ty, char>           ||
                                      std::is_same_v<_Ty, wchar_t>        ||
                                      std::is_same_v<_Ty, signed char>    ||
                                      std::is_same_v<_Ty, unsigned char>  ||
                                      std::is_same_v<_Ty, unsigned short> ||
                                      std::is_same_v<_Ty, short>          ||
                                      std::is_same_v<_Ty, unsigned int>   ||
                                      std::is_same_v<_Ty, int64>          ||
                                      std::is_same_v<_Ty, uint64>         ||
                                      std::is_same_v<_Ty, fpreal16>       ||
                                      std::is_same_v<_Ty, float>          ||
                                      std::is_same_v<_Ty, double>         ;


    
#define __GFE_SPECIALIZATION_IsVector(NUM)                                                                \
template <class _Ty>                                                                                      \
_INLINE_VAR constexpr bool isVector##NUM = std::is_same_v<_Ty, UT_Vector##NUM##T<char>           >||      \
                                           std::is_same_v<_Ty, UT_Vector##NUM##T<wchar_t>        >||      \
                                           std::is_same_v<_Ty, UT_Vector##NUM##T<signed char>    >||      \
                                           std::is_same_v<_Ty, UT_Vector##NUM##T<unsigned char>  >||      \
                                           std::is_same_v<_Ty, UT_Vector##NUM##T<unsigned short> >||      \
                                           std::is_same_v<_Ty, UT_Vector##NUM##T<short>          >||      \
                                           std::is_same_v<_Ty, UT_Vector##NUM##T<unsigned int>   >||      \
                                           std::is_same_v<_Ty, UT_Vector##NUM##T<int64>          >||      \
                                           std::is_same_v<_Ty, UT_Vector##NUM##T<uint64>         >||      \
                                           std::is_same_v<_Ty, UT_Vector##NUM##T<fpreal16>       >||      \
                                           std::is_same_v<_Ty, UT_Vector##NUM##T<float>          >||      \
                                           std::is_same_v<_Ty, UT_Vector##NUM##T<double>         >;       \

__GFE_SPECIALIZATION_IsVector(2)
__GFE_SPECIALIZATION_IsVector(3)
__GFE_SPECIALIZATION_IsVector(4)

#undef  __GFE_SPECIALIZATION_IsVector

#define __GFE_SPECIALIZATION_IsMatrix(NUM)                                                                \
template <class _Ty>                                                                                      \
_INLINE_VAR constexpr bool isMatrix##NUM = std::is_same_v<_Ty, UT_Matrix##NUM##T<char>           >||      \
                                           std::is_same_v<_Ty, UT_Matrix##NUM##T<wchar_t>        >||      \
                                           std::is_same_v<_Ty, UT_Matrix##NUM##T<signed char>    >||      \
                                           std::is_same_v<_Ty, UT_Matrix##NUM##T<unsigned char>  >||      \
                                           std::is_same_v<_Ty, UT_Matrix##NUM##T<unsigned short> >||      \
                                           std::is_same_v<_Ty, UT_Matrix##NUM##T<short>          >||      \
                                           std::is_same_v<_Ty, UT_Matrix##NUM##T<unsigned int>   >||      \
                                           std::is_same_v<_Ty, UT_Matrix##NUM##T<int64>          >||      \
                                           std::is_same_v<_Ty, UT_Matrix##NUM##T<uint64>         >||      \
                                           std::is_same_v<_Ty, UT_Matrix##NUM##T<fpreal16>       >||      \
                                           std::is_same_v<_Ty, UT_Matrix##NUM##T<float>          >||      \
                                           std::is_same_v<_Ty, UT_Matrix##NUM##T<double>         >;       \

__GFE_SPECIALIZATION_IsMatrix(2)
__GFE_SPECIALIZATION_IsMatrix(3)
__GFE_SPECIALIZATION_IsMatrix(4)

#undef  __GFE_SPECIALIZATION_IsMatrix



template <class _Ty>
_INLINE_VAR constexpr bool isVector = isVector<_Ty> ||
                                      isVector<_Ty> ||
                                      isVector<_Ty> ;


template <class _Ty>
_INLINE_VAR constexpr bool isMatrix34 = isMatrix3<_Ty> ||
                                        isMatrix4<_Ty> ;

 

template <class _Ty>
_INLINE_VAR constexpr bool isMatrix = isMatrix2<_Ty> ||
                                      isMatrix3<_Ty> ||
                                      isMatrix4<_Ty> ;


template <class _Ty>
_INLINE_VAR constexpr bool isVecMtx = isVector<_Ty> ||
                                      isMatrix<_Ty> ;


template <bool judge, typename _T1, typename _T2>
struct selectValueType {
    using type = _T1;
};

template <typename _T1, typename _T2>
struct selectValueType<false, _T1, _T2> {
    using type = _T2;
};
    
template <typename _T1>
struct getValueType {
    using type = selectValueType<isScalar<_T1>, _T1, typename _T1::value_type>;
};

    
#ifndef GFE_TOPO_SCOPE

#if 0
#define GFE_TOPO_SCOPE GFE_SCOPE_PRIVATE
#else
#define GFE_TOPO_SCOPE GA_SCOPE_PUBLIC
#endif

#endif

    

    //SYS_FORCE_INLINE static GA_PageNum getNumPage(const GA_Offset v)
    //{ return GA_PageNum(v >> GA_PAGE_BITS); }

    //SYS_FORCE_INLINE static GA_PrimitiveTypeId typeId(const GEO_PrimitiveType type)
    //{ return GA_PrimitiveTypeId(static_cast<int>(type)); }

    static GFE_GroupMergeMethod attribGroupMergeMethod(const GFE_AttribMergeMethod mergeMethod)
    {
        switch (mergeMethod)
        {
        case GFE_AttribMergeMethod::First:          return GFE_GroupMergeMethod::First;     break;
        case GFE_AttribMergeMethod::Last:           return GFE_GroupMergeMethod::Last;      break;
        case GFE_AttribMergeMethod::Min:            return GFE_GroupMergeMethod::Min;       break;
        case GFE_AttribMergeMethod::Max:            return GFE_GroupMergeMethod::Max;       break;
        case GFE_AttribMergeMethod::Mode:           return GFE_GroupMergeMethod::Mode;      break;
        default:                                    return GFE_GroupMergeMethod::Max;       break;
        }
        UT_ASSERT_MSG(0, "Unhandled Attrib Merge Method!");
        return GFE_GroupMergeMethod::First;
    }

    static GFE_AttribMergeMethod attribGroupMergeMethod(const GFE_GroupMergeMethod mergeMethod)
    {
        switch (mergeMethod)
        {
        case GFE_GroupMergeMethod::First:          return GFE_AttribMergeMethod::First;     break;
        case GFE_GroupMergeMethod::Last:           return GFE_AttribMergeMethod::Last;      break;
        case GFE_GroupMergeMethod::Min:            return GFE_AttribMergeMethod::Min;       break;
        case GFE_GroupMergeMethod::Max:            return GFE_AttribMergeMethod::Max;       break;
        case GFE_GroupMergeMethod::Mode:           return GFE_AttribMergeMethod::Mode;      break;
        }
        UT_ASSERT_MSG(0, "Unhandled Group Merge Method!");
        return GFE_AttribMergeMethod::First;
    }

    static GU_Snap::AttributeMergeMethod snapMergeMethod(const GFE_AttribMergeMethod mergeMethod)
    {
        switch (mergeMethod)
        {
        case GFE_AttribMergeMethod::First:          return GU_Snap::AttributeMergeMethod::MERGE_ATTRIBUTE_FIRST;     break;
        case GFE_AttribMergeMethod::Last:           return GU_Snap::AttributeMergeMethod::MERGE_ATTRIBUTE_LAST;      break;
        case GFE_AttribMergeMethod::Min:            return GU_Snap::AttributeMergeMethod::MERGE_ATTRIBUTE_MIN;       break;
        case GFE_AttribMergeMethod::Max:            return GU_Snap::AttributeMergeMethod::MERGE_ATTRIBUTE_MAX;       break;
        case GFE_AttribMergeMethod::Mode:           return GU_Snap::AttributeMergeMethod::MERGE_ATTRIBUTE_MODE;      break;
        case GFE_AttribMergeMethod::Mean:           return GU_Snap::AttributeMergeMethod::MERGE_ATTRIBUTE_MEAN;      break;
        case GFE_AttribMergeMethod::Median:         return GU_Snap::AttributeMergeMethod::MERGE_ATTRIBUTE_MEDIAN;    break;
        case GFE_AttribMergeMethod::Sum:            return GU_Snap::AttributeMergeMethod::MERGE_ATTRIBUTE_SUM;       break;
        case GFE_AttribMergeMethod::SumSquare:      return GU_Snap::AttributeMergeMethod::MERGE_ATTRIBUTE_SUMSQUARE; break;
        case GFE_AttribMergeMethod::RootMeanSquare: return GU_Snap::AttributeMergeMethod::MERGE_ATTRIBUTE_RMS;       break;
        }
        UT_ASSERT_MSG(0, "Unhandled Attrib Merge Method!");
        return GU_Snap::AttributeMergeMethod::MERGE_ATTRIBUTE_FIRST;
    }

    static GU_Snap::AttributeMergeMethod snapMergeMethod(const GFE_GroupMergeMethod mergeMethod)
    {
        switch (mergeMethod)
        {
        case GFE_GroupMergeMethod::First: return GU_Snap::AttributeMergeMethod::MERGE_ATTRIBUTE_FIRST;     break;
        case GFE_GroupMergeMethod::Last:  return GU_Snap::AttributeMergeMethod::MERGE_ATTRIBUTE_LAST;      break;
        case GFE_GroupMergeMethod::Min:   return GU_Snap::AttributeMergeMethod::MERGE_ATTRIBUTE_MIN;       break;
        case GFE_GroupMergeMethod::Max:   return GU_Snap::AttributeMergeMethod::MERGE_ATTRIBUTE_MAX;       break;
        case GFE_GroupMergeMethod::Mode:  return GU_Snap::AttributeMergeMethod::MERGE_ATTRIBUTE_MODE;      break;
        }
        UT_ASSERT_MSG(0, "Unhandled Group Merge Method!");
        return GU_Snap::AttributeMergeMethod::MERGE_ATTRIBUTE_FIRST;
    }

    
    
    
    
template<typename VECTOR_T>
SYS_FORCE_INLINE static VECTOR_T getZeroVector()
{
    if constexpr(VECTOR_T::tuple_size == 4)
        return VECTOR_T(0, 0, 0, 0);
    else
        return VECTOR_T(0.0);
}
    

SYS_FORCE_INLINE static bool isPacked(const int i)
{ return i >= 25 && i <= 27; }



    
#if GFE_INVALID_OFFSET >= 0
    
SYS_FORCE_INLINE static bool isValidOffset(const GA_Offset elemoff)
{ return elemoff < GFE_INVALID_OFFSET && elemoff >= 0; }

SYS_FORCE_INLINE static bool isInvalidOffset(const GA_Offset elemoff)
{ return elemoff >= GFE_INVALID_OFFSET || elemoff < 0; }

#else
    
SYS_FORCE_INLINE static bool isValidOffset(const GA_Offset elemoff)
{ return elemoff >= 0; }

SYS_FORCE_INLINE static bool isInvalidOffset(const GA_Offset elemoff)
{ return elemoff < 0; }
    
#endif



SYS_FORCE_INLINE static bool isValidOffset(const GA_IndexMap& indexMap, const GA_Offset elemoff)
#if GFE_DEBUG_MODE
{
    const GA_Index index = indexMap.indexFromOffset(elemoff);
    const bool isOffsetActiveFast = indexMap.isOffsetActive(elemoff);
    return indexMap.isOffsetActive(elemoff);
}
#else
{ return indexMap.isOffsetActive(elemoff); }
#endif

SYS_FORCE_INLINE static bool isInvalidOffset(const GA_IndexMap& indexMap, const GA_Offset elemoff)
{ return isInvalidOffset(elemoff) || !indexMap.isOffsetActiveFast(elemoff); }


    
SYS_FORCE_INLINE static bool isValid(const UT_StringRef& str)
{ return str.isstring() && str.length() > 0; }

SYS_FORCE_INLINE static bool isInvalid(const UT_StringRef& str)
{ return !str.isstring() || str.length() <= 0; }

SYS_FORCE_INLINE static bool isPublicAttribName(const UT_StringRef& attribName)
{ return isValid(attribName) && strcmp(attribName.c_str(), "P") != 0; }

SYS_FORCE_INLINE static bool isPublicAttribName(const char* const attribName)
{ return attribName && strcmp(attribName, "P") != 0; }


    
static GA_GroupType attributeOwner_groupType(const GA_AttributeOwner attribOwner)
{
    switch (attribOwner)
    {
    case GA_ATTRIB_PRIMITIVE:       return GA_GROUP_PRIMITIVE;    break;
    case GA_ATTRIB_POINT:           return GA_GROUP_POINT;        break;
    case GA_ATTRIB_VERTEX:          return GA_GROUP_VERTEX;       break;
    case GA_ATTRIB_DETAIL:          return GA_GROUP_EDGE;         break;
    case GA_ATTRIB_OWNER_N:         return GA_GROUP_N;            break;
    case GA_ATTRIB_INVALID:         return GA_GROUP_INVALID;      break;
    }
    UT_ASSERT_MSG(0, "Unhandled Attrib Owner!");
    return GA_GROUP_INVALID;
}

static GA_AttributeOwner attributeOwner_groupType(const GA_GroupType groupType)
{
    switch (groupType)
    {
    case GA_GROUP_PRIMITIVE:       return GA_ATTRIB_PRIMITIVE;   break;
    case GA_GROUP_POINT:           return GA_ATTRIB_POINT;       break;
    case GA_GROUP_VERTEX:          return GA_ATTRIB_VERTEX;      break;
    case GA_GROUP_EDGE:            return GA_ATTRIB_DETAIL;      break;
    case GA_GROUP_BREAKPOINT:      return GA_ATTRIB_INVALID;     break;
    case GA_GROUP_N:               return GA_ATTRIB_OWNER_N;     break;
    case GA_GROUP_INVALID:         return GA_ATTRIB_INVALID;     break;
    }
    UT_ASSERT_MSG(0, "Unhandled Group Type!");
    return GA_ATTRIB_INVALID;
}
    
static GA_Precision precisionFromStorage(const GA_Storage storage)
{
    switch (storage)
    {
    case GA_STORE_INVALID:       return GA_PRECISION_INVALID;   break;
    case GA_STORE_BOOL:          return GA_PRECISION_1;         break;
    case GA_STORE_UINT8:         return GA_PRECISION_8;         break;
    case GA_STORE_INT8:          return GA_PRECISION_8;         break;
    case GA_STORE_INT16:         return GA_PRECISION_16;        break;
    case GA_STORE_INT32:         return GA_PRECISION_32;        break;
    case GA_STORE_INT64:         return GA_PRECISION_64;        break;
    case GA_STORE_REAL16:        return GA_PRECISION_16;        break;
    case GA_STORE_REAL32:        return GA_PRECISION_32;        break;
    case GA_STORE_REAL64:        return GA_PRECISION_64;        break;
    case GA_STORE_STRING:        return GA_PRECISION_INVALID;   break;
    case GA_STORE_DICT:          return GA_PRECISION_INVALID;   break;
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
{ return attribOwner == GA_ATTRIB_PRIMITIVE || attribOwner == GA_ATTRIB_POINT || attribOwner == GA_ATTRIB_VERTEX; }
    
SYS_FORCE_INLINE static bool isElementGroup(const GA_GroupType groupType)
{ return groupType == GA_GROUP_PRIMITIVE || groupType == GA_GROUP_POINT || groupType == GA_GROUP_VERTEX; }






static GA_Storage getPreferredStorageF(const GA_Precision precision)
{
    switch (precision)
    {
    case GA_PRECISION_16: return GA_STORE_REAL16; break;
    case GA_PRECISION_32: return GA_STORE_REAL32; break;
    case GA_PRECISION_64: return GA_STORE_REAL64; break;
    default:              break;
    }
    //UT_ASSERT_MSG(0, "Unhandled Precision!");
    return GA_STORE_INVALID;
}
    

static GA_Storage getPreferredStorageI(const GA_Precision precision)
{
    switch (precision)
    {
    case GA_PRECISION_8:    return GA_STORE_INT8;    break;
    case GA_PRECISION_16:   return GA_STORE_INT16;   break;
    case GA_PRECISION_32:   return GA_STORE_INT32;   break;
    case GA_PRECISION_64:   return GA_STORE_INT64;   break;
    default: break;
    }
    //UT_ASSERT_MSG(0, "Unhandled Precision!");
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
{ return getPreferredStorageI(geo.getPreferredPrecision()); }


SYS_FORCE_INLINE static GA_Storage getPreferredStorageF(const GA_Detail& geo)
{ return getPreferredStorageF(geo.getPreferredPrecision()); }



SYS_FORCE_INLINE static GA_Storage getPreferredStorage(const GA_Detail& geo, const GA_StorageClass storageClass)
{ return getPreferredStorage(storageClass, geo.getPreferredPrecision()); }


SYS_FORCE_INLINE static GA_Storage getPreferredStorageF(const GA_Detail& geo, const GA_Storage storage)
{ return storage == GA_STORE_INVALID ? getPreferredStorageF(geo) : storage; }

SYS_FORCE_INLINE static GA_Storage getPreferredStorageI(const GA_Detail& geo, const GA_Storage storage)
{ return storage == GA_STORE_INVALID ? getPreferredStorageI(geo) : storage; }

SYS_FORCE_INLINE static GA_Storage getPreferredStorage(const GA_Detail& geo, const GA_StorageClass storageClass, const GA_Storage storage)
{ return storage == GA_STORE_INVALID ? getPreferredStorage(geo, storageClass) : storage; }

SYS_FORCE_INLINE static GA_Storage getPreferredStorage(const GA_Detail* const geo, const GA_StorageClass storageClass, const GA_Storage storage)
{ return getPreferredStorage(*geo, storageClass, storage); }


    
    
    
    
    
    
    

template<typename VECTOR_T>
static VECTOR_T axisDir(const GFE_Axis axis)
{
    switch (axis)
    {
    case GFE_Axis::X: return VECTOR_T(1,0,0); break;
    case GFE_Axis::Y: return VECTOR_T(0,1,0); break;
    case GFE_Axis::Z: return VECTOR_T(0,0,1); break;
    }
    UT_ASSERT_MSG(0, "Unhandled Axis");
    return VECTOR_T(0,1,0);
}

SYS_FORCE_INLINE static UT_Vector3T<fpreal16> axisDirH(const GFE_Axis axis)
{ return axisDir<UT_Vector3T<fpreal16>>(axis); }

SYS_FORCE_INLINE static UT_Vector3T<fpreal32> axisDirF(const GFE_Axis axis)
{ return axisDir<UT_Vector3T<fpreal32>>(axis); }
    
SYS_FORCE_INLINE static UT_Vector3T<fpreal64> axisDirD(const GFE_Axis axis)
{ return axisDir<UT_Vector3T<fpreal64>>(axis); }
    
SYS_FORCE_INLINE static bool stringEqual(const char* const str0, const char* const str1)
{ return strcmp(str0, str1) == 0; }

SYS_FORCE_INLINE static bool stringEqual(const UT_StringRef& str0, const UT_StringRef& str1)
{ return stringEqual(str0.c_str(), str1.c_str()); }

    
static bool isAttribTypeEqual(const GA_Attribute* const attrib0, const GA_Attribute* const attrib1)
{
    UT_ASSERT_P(attrib0);
    UT_ASSERT_P(attrib1);
    return &attrib0->getDetail()       == &attrib1->getDetail()       &&
            attrib0->getTupleSize()    ==  attrib1->getTupleSize()    &&
            attrib0->getStorageClass() ==  attrib1->getStorageClass() &&
            (
                !attrib0->getAIFTuple() ||
                 attrib0->getAIFTuple() &&
                 attrib1->getAIFTuple() &&
                 attrib0->getAIFTuple()->getStorage(attrib0) == attrib1->getAIFTuple()->getStorage(attrib1)
            );
}

    static bool checkTupleAttrib(
        const GA_Attribute* const attrib,
        const GA_StorageClass storageClass = GA_STORECLASS_INVALID,
        const GA_Precision precision = GA_PRECISION_INVALID,
        const int tupleSize = 1,
        const GA_Defaults* const defaults = nullptr
    )
    {
        if (!attrib)
            return false;
        if (attrib->getTupleSize() != tupleSize)
            return false;
        if (storageClass != GA_STORECLASS_INVALID && attrib->getStorageClass() != storageClass)
            return false;
        
        const GA_AIFTuple* const aifTuple = attrib->getAIFTuple();
        if (aifTuple)
        {
            if ((precision != GA_PRECISION_INVALID && precisionFromStorage(aifTuple->getStorage(attrib)) != precision) ||
                (defaults && aifTuple->getDefaults(attrib) != *defaults))
                    return false;
        }
        else
        {
            if (storageClass != GA_STORECLASS_INVALID && storageClass != GA_STORECLASS_STRING)
                return false;
            //const GA_AIFStringTuple* const aifStrTuple = attribPtr->getAIFStringTuple();
        }
        return true;
    }
    
static bool checkTupleAttrib(
    const GA_Attribute* const attrib,
    const GA_StorageClass storageClass = GA_STORECLASS_INVALID,
    const GA_Storage storage = GA_STORE_INVALID,
    const int tupleSize = 1,
    const GA_Defaults* const defaults = nullptr
)
{
    if (!attrib)
        return false;
    if (attrib->getTupleSize() != tupleSize)
        return false;
    if (storageClass != GA_STORECLASS_INVALID && attrib->getStorageClass() != storageClass)
        return false;
        
    const GA_AIFTuple* const aifTuple = attrib->getAIFTuple();
    if (aifTuple)
    {
        if ((storage != GA_STORE_INVALID && aifTuple->getStorage(attrib) != storage) ||
            (defaults && aifTuple->getDefaults(attrib) != *defaults))
            return false;
    }
    else
    {
        if (storageClass != GA_STORECLASS_INVALID && storageClass != GA_STORECLASS_STRING)
            return false;
        if (storage != GA_STORE_INVALID && storage != GA_STORE_STRING)
            return false;
        //const GA_AIFStringTuple* const aifStrTuple = attribPtr->getAIFStringTuple();
    }
    return true;
}

SYS_FORCE_INLINE static bool checkDirAttrib(const GA_Attribute* const attrib, const GA_StorageClass storageClass = GA_STORECLASS_INVALID, const GA_Storage storage = GA_STORE_INVALID)
{ return checkTupleAttrib(attrib, storageClass, storage, 3); }

static bool checkArrayAttrib(
    const GA_Attribute* const attrib,
    const GA_StorageClass storageClass = GA_STORECLASS_INVALID,
    const GA_Storage storage = GA_STORE_INVALID,
    const int tupleSize = 3
)
{
    if (!attrib)
        return false;
    if (attrib->getTupleSize() != tupleSize)
        return false;
    if (storageClass != GA_STORECLASS_INVALID && attrib->getStorageClass() != storageClass)
        return false;
    const GA_AIFNumericArray* const aifNumericArray = attrib->getAIFNumericArray();
    if (aifNumericArray)
    {
        if (storage != GA_STORE_INVALID && aifNumericArray->getStorage(attrib) != storage)
            return false;
    }
    else
    {
        if (storageClass != GA_STORECLASS_INVALID && storageClass != GA_STORECLASS_STRING)
            return false;
        if (storage != GA_STORE_INVALID && storage != GA_STORE_STRING)
            return false;
        //const GA_AIFSharedStringArray* const aifStrArray = attribPtr->getAIFSharedStringArray();
    }
    return true;
}


SYS_FORCE_INLINE static bool isValidPosAttrib(const GA_Attribute& posAttrib)
{ return posAttrib.getOwner() == GA_ATTRIB_POINT && posAttrib.getAIFTuple() && posAttrib.getTupleSize()==3; }

SYS_FORCE_INLINE static bool isValidPosAttrib(const GA_Attribute* const posAttrib)
{ return posAttrib && isValidPosAttrib(*posAttrib); }

SYS_FORCE_INLINE static bool isInvalidPosAttrib(const GA_Attribute& posAttrib)
{ return !isValidPosAttrib(posAttrib); }
    
SYS_FORCE_INLINE static bool isInvalidPosAttrib(const GA_Attribute* const posAttrib)
{ return !isValidPosAttrib(posAttrib); }








} // End of namespace GFE_Type



















#endif
