
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





#if 1
    #if 1
        static constexpr int32 GFE_INVALID_OFFSET32 = std::numeric_limits<int32>::max();
        static constexpr int64 GFE_INVALID_OFFSET64 = std::numeric_limits<int64>::max();

        //template<typename _SCALAR_T>
        //static constexpr _SCALAR_T GFE_INVALID_OFFSET = std::numeric_limits<_SCALAR_T>::max();
    #else
        #define GFE_INVALID_OFFSET32 2147483647
        #define GFE_INVALID_OFFSET64 9223372036854775807
    #endif
#else
    #define GFE_INVALID_OFFSET32 -1
    #define GFE_INVALID_OFFSET64 -1
#endif

#define GFE_INVALID_OFFSET GFE_INVALID_OFFSET32







#if SYS_VERSION_MAJOR_INT > 19 || ( SYS_VERSION_MAJOR_INT == 19 && SYS_VERSION_MINOR_INT == 5 )
#else
#endif


#ifndef PI
#define PI 3.14159265358979323846
#endif



#define __GFE_Type_SpecializationVec(NUM)\
        UT_Vector##NUM##I,\
        UT_Vector##NUM##i = UT_Vector##NUM##I,\
        UT_Vector##NUM##L,\
        UT_Vector##NUM##H,\
        UT_Vector##NUM##F,\
        UT_Vector##NUM##D,\
    

#define __GFE_Type_SpecializationMtx(NUM)\
        UT_Matrix##NUM##F,\
        UT_Matrix##NUM##D,\
    
enum class GFE_NumericTupleType
{
    int8,
    int16,
    int32,
    int64,
    fpreal16,
    fpreal32,
    fpreal64,
    
    __GFE_Type_SpecializationVec(2)
    __GFE_Type_SpecializationVec(3)
    __GFE_Type_SpecializationVec(4)
    
    __GFE_Type_SpecializationMtx(2)
    __GFE_Type_SpecializationMtx(3)
    __GFE_Type_SpecializationMtx(4)
    
    //UT_Vector2I,
    //UT_Vector2i = UT_Vector2I,
    //UT_Vector2L,
    //UT_Vector2H,
    //UT_Vector2F,
    //UT_Vector2D,
    //
    //UT_Vector3I,
    //UT_Vector3i = UT_Vector3I,
    //UT_Vector3L,
    //UT_Vector3H,
    //UT_Vector3F,
    //UT_Vector3D,
    //
    //UT_Vector4I,
    //UT_Vector4i = UT_Vector4I,
    //UT_Vector4L,
    //UT_Vector4H,
    //UT_Vector4F,
    //UT_Vector4D,
    //
    //UT_Matrix2F,
    //UT_Matrix2D,
    //
    //UT_Matrix3F,
    //UT_Matrix3D,
    //
    //UT_Matrix4F,
    //UT_Matrix4D,
    
    invalid = -1,
    float16 = fpreal16,
    float32 = fpreal32,
    float64 = fpreal64,
    
}; // End of Class Enum Class GFE_NumericTupleType


#undef __GFE_Type_SpecializationVec
#undef __GFE_Type_SpecializationMtx



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



#define GFE_ForEachStorage1(Func, Storage)                     \
        switch (Storage)                                       \
        {                                                      \
        case GA_STORE_INT8:   Func<int8    >();         break; \
        case GA_STORE_INT16:  Func<int16   >();         break; \
        case GA_STORE_INT32:  Func<int32   >();         break; \
        case GA_STORE_INT64:  Func<int64   >();         break; \
        case GA_STORE_REAL16: Func<fpreal16>();         break; \
        case GA_STORE_REAL32: Func<fpreal32>();         break; \
        case GA_STORE_REAL64: Func<fpreal64>();         break; \
        default: UT_ASSERT_MSG(0, "Unhandled Storage"); break; \
        }                                                      \



#define GFE_ForEachStorageVec(Func, Storage, Num)                         \
        switch (Storage)                                                  \
        {                                                                 \
        case GA_STORE_INT32:  Func<UT_Vector##Num##T<int32   >>(); break; \
        case GA_STORE_INT64:  Func<UT_Vector##Num##T<int64   >>(); break; \
        case GA_STORE_REAL16: Func<UT_Vector##Num##T<fpreal16>>(); break; \
        case GA_STORE_REAL32: Func<UT_Vector##Num##T<fpreal32>>(); break; \
        case GA_STORE_REAL64: Func<UT_Vector##Num##T<fpreal64>>(); break; \
        default: UT_ASSERT_MSG(0, "Unhandled Storage");            break; \
        }                                                                 \


#define GFE_ForEachStorageMtx(Func, Storage, Num)                         \
        switch (Storage)                                                  \
        {                                                                 \
        case GA_STORE_INT32:  Func<UT_Matrix##Num##T<int32   >>(); break; \
        case GA_STORE_INT64:  Func<UT_Matrix##Num##T<int64   >>(); break; \
        case GA_STORE_REAL16: Func<UT_Matrix##Num##T<fpreal16>>(); break; \
        case GA_STORE_REAL32: Func<UT_Matrix##Num##T<fpreal32>>(); break; \
        case GA_STORE_REAL64: Func<UT_Matrix##Num##T<fpreal64>>(); break; \
        default: UT_ASSERT_MSG(0, "Unhandled Storage");            break; \
        }                                                                 \


#define GFE_ForEachStorageV2(Func, Storage)     \
        GFE_ForEachStorageVec(Func, Storage, 2) \

#define GFE_ForEachStorageV3(Func, Storage)     \
        GFE_ForEachStorageVec(Func, Storage, 3) \

#define GFE_ForEachStorageV4(Func, Storage)     \
        GFE_ForEachStorageVec(Func, Storage, 4) \


#define GFE_ForEachStorageM2(Func, Storage)      \
        GFE_ForEachStorageMtx(Func, Storage, 2)  \

#define GFE_ForEachStorageM3(Func, Storage)      \
        GFE_ForEachStorageMtx(Func, Storage, 3)  \

#define GFE_ForEachStorageM4(Func, Storage)      \
        GFE_ForEachStorageMtx(Func, Storage, 4)  \




#define GFE_ForEachStorageTupleSizeVec(Func, Storage, TupleSize)   \
        switch (TupleSize)                                         \
        {                                                          \
        case 1: GFE_ForEachStorage1 (Func, Storage);       break;  \
        case 2: GFE_ForEachStorageV2(Func, Storage);       break;  \
        case 3: GFE_ForEachStorageV3(Func, Storage);       break;  \
        case 4: GFE_ForEachStorageV4(Func, Storage);       break;  \
        default: UT_ASSERT_MSG(0, "Unhandled Tuple Size"); break;  \
        }                                                          \

#define GFE_ForEachStorageTupleSizeVecMtx(Func, Storage, TupleSize)   \
        switch (TupleSize)                                            \
        {                                                             \
        case 1:  GFE_ForEachStorage1 (Func, Storage);     break;      \
        case 4:  GFE_ForEachStorageM2(Func, Storage);     break;      \
        case 9:  GFE_ForEachStorageM3(Func, Storage);     break;      \
        case 16: GFE_ForEachStorageM4(Func, Storage);     break;      \
        default: UT_ASSERT_MSG(0, "Unhandled Tuple Size"); break;     \
        }                                                             \




/*

template<typename _Ty, typename Body>
#define GFEForEachStorageVec(const Body &body, const GA_Storage storage, const int num)
{
    switch (Storage)
    {
    case GA_STORE_INT32:  Func<UT_Vector##Num##T<int32   >>(); break;
    case GA_STORE_INT64:  Func<UT_Vector##Num##T<int64   >>(); break;
    case GA_STORE_REAL16: Func<UT_Vector##Num##T<fpreal16>>(); break;
    case GA_STORE_REAL32: Func<UT_Vector##Num##T<fpreal32>>(); break;
    case GA_STORE_REAL64: Func<UT_Vector##Num##T<fpreal64>>(); break;
    default: UT_ASSERT_MSG(0, "Unhandled Storage");            break;
    }
}


template<typename _Ty, typename Body>
GFEForEachStorageTupleSizeVec(const Body &body, const GA_Storage storage, const int tupleSize)
{
    switch (tupleSize)
    {
    case 1: GFE_ForEachStorage1(body, storage);        break;
    case 2: GFE_ForEachStorage2(body, storage);        break;
    case 3: GFE_ForEachStorage3(body, storage);        break;
    case 4: GFE_ForEachStorage4(body, storage);        break;
    default: UT_ASSERT_MSG(0, "Unhandled Tuple Size"); break;
    }
}

*/


namespace GFE_Type {

//#ifndef fpreal16
//#define fpreal16 float
//#endif
    
// template <class _Ty>
// _INLINE_VAR constexpr bool isScalar = std::is_same_v<_Ty, char>           ||
//                                       std::is_same_v<_Ty, wchar_t>        ||
//                                       std::is_same_v<_Ty, signed char>    ||
//                                       std::is_same_v<_Ty, unsigned char>  ||
//                                       std::is_same_v<_Ty, unsigned short> ||
//                                       std::is_same_v<_Ty, short>          ||
//                                       std::is_same_v<_Ty, unsigned int>   ||
//                                       std::is_same_v<_Ty, int64>          ||
//                                       std::is_same_v<_Ty, uint64>         ||
//                                       std::is_same_v<_Ty, fpreal16>       ||
//                                       std::is_same_v<_Ty, float>          ||
//                                       std::is_same_v<_Ty, double>         ;

template <class _Ty>
_INLINE_VAR constexpr bool isStringHolder = std::is_same_v<_Ty, UT_StringHolder>;


template <class _Ty>
_INLINE_VAR constexpr bool isScalar = std::is_arithmetic_v<_Ty>      ||
                                      std::is_same_v<_Ty, int64>     ||
                                      std::is_same_v<_Ty, uint64>    ||
                                      std::is_same_v<_Ty, fpreal16>  ;


    
#define __GFE_SPECIALIZATION_IsVector(NUM)                                                                  \
template <class _Ty>                                                                                        \
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

#define __GFE_SPECIALIZATION_IsMatrix(NUM)                                                                  \
template <class _Ty>                                                                                        \
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
_INLINE_VAR constexpr bool isVector = isVector2<_Ty> ||
                                      isVector3<_Ty> ||
                                      isVector4<_Ty> ;


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


template <class _Ty>
_INLINE_VAR constexpr bool isTuple = isScalar<_Ty> ||
                                     isVecMtx<_Ty> ;


    
    
template <bool judge, typename _Ty>
struct select_value_type { using type = typename _Ty; };

template <typename _Ty>
struct select_value_type<false, _Ty> { using type = typename _Ty::value_type; };
    
template <typename _Ty>
struct get_value_type { using type = typename select_value_type<isScalar<_Ty>, _Ty>::type; };

template <class _Ty>
using get_value_type_t = typename get_value_type<_Ty>::type;
    
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


SYS_FORCE_INLINE static bool isHoudiniVolume(const int id)
{ return id == GA_PRIMVOLUME; } // 20

SYS_FORCE_INLINE static bool isVDB(const int id)
{ return id == GA_PRIMVDB; } // 23

SYS_FORCE_INLINE static bool isVolume(const int id)
{ return isHoudiniVolume(id) || isVDB(id); }


SYS_FORCE_INLINE static bool isPackedGeometry(const int id)
{ return id == 25; } // 25

SYS_FORCE_INLINE static bool isPackedFragment(const int id)
{ return id == 27; } // 27

SYS_FORCE_INLINE static bool isPacked(const int id)
{ return isPackedGeometry(id) || isPackedFragment(id); }


    SYS_FORCE_INLINE static bool stringEqual(const char* const str0, const char* const str1)
    { return strcmp(str0, str1) == 0; }

    SYS_FORCE_INLINE static bool stringEqual(const UT_StringRef& str0, const UT_StringRef& str1)
    { return stringEqual(str0.c_str(), str1.c_str()); }

    SYS_FORCE_INLINE static bool stringEqual(const UT_StringRef& str0, const char* const str1)
    { return stringEqual(str0.c_str(), str1); }

    SYS_FORCE_INLINE static bool stringEqual(const char* const str0, const UT_StringRef& str1)
    { return stringEqual(str0, str1.c_str()); }

    SYS_FORCE_INLINE static bool stringEqualP(const UT_StringRef& str0)
    { return stringEqual(str0, "P"); }
    
    SYS_FORCE_INLINE static bool stringEqualP(const char* const str0)
    { return stringEqual(str0, "P"); }
    
    
    
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
{ return isValid(attribName) && stringEqual(attribName, "P"); }

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


    GFE_NumericTupleType getNumericTupleType(const int tupleSize, const GA_Storage store)
    {
        switch (tupleSize)
        {
        case 1:
            switch (store)
            {
            case GA_STORE_INT8:   return GFE_NumericTupleType::int8;     break;
            case GA_STORE_INT16:  return GFE_NumericTupleType::int16;    break;
            case GA_STORE_INT32:  return GFE_NumericTupleType::int32;    break;
            case GA_STORE_INT64:  return GFE_NumericTupleType::int64;    break;
            case GA_STORE_REAL16: return GFE_NumericTupleType::fpreal16; break;
            case GA_STORE_REAL32: return GFE_NumericTupleType::fpreal32; break;
            case GA_STORE_REAL64: return GFE_NumericTupleType::fpreal64; break;
            default: UT_ASSERT_MSG(0, "Unhandled Attrib Storage"); break;
            }
        break;
        case 2:
            switch (store)
            {
            case GA_STORE_INT32:  return GFE_NumericTupleType::UT_Vector2I; break;
            case GA_STORE_INT64:  return GFE_NumericTupleType::UT_Vector2L; break;
            case GA_STORE_REAL16: return GFE_NumericTupleType::UT_Vector2H; break;
            case GA_STORE_REAL32: return GFE_NumericTupleType::UT_Vector2F; break;
            case GA_STORE_REAL64: return GFE_NumericTupleType::UT_Vector2D; break;
            default: UT_ASSERT_MSG(0, "Unhandled Attrib Storage"); break;
            }
        break;
        case 3:
            switch (store)
            {
            case GA_STORE_INT32:  return GFE_NumericTupleType::UT_Vector3I; break;
            case GA_STORE_INT64:  return GFE_NumericTupleType::UT_Vector3L; break;
            case GA_STORE_REAL16: return GFE_NumericTupleType::UT_Vector3H; break;
            case GA_STORE_REAL32: return GFE_NumericTupleType::UT_Vector3F; break;
            case GA_STORE_REAL64: return GFE_NumericTupleType::UT_Vector3D; break;
            default: UT_ASSERT_MSG(0, "Unhandled Attrib Storage"); break;
            }
        break;
        case 4:
            switch (store)
            {
            case GA_STORE_INT32:  return GFE_NumericTupleType::UT_Vector4I; break;
            case GA_STORE_INT64:  return GFE_NumericTupleType::UT_Vector4L; break;
            case GA_STORE_REAL16: return GFE_NumericTupleType::UT_Vector4H; break;
            case GA_STORE_REAL32: return GFE_NumericTupleType::UT_Vector4F; break;
            case GA_STORE_REAL64: return GFE_NumericTupleType::UT_Vector4D; break;
            default: UT_ASSERT_MSG(0, "Unhandled Attrib Storage"); break;
            }
        break;
        default: UT_ASSERT_MSG(0, "Unhandled Attrib Tuple Size"); break;
        }
        return GFE_NumericTupleType::invalid;
    }


    SYS_FORCE_INLINE GFE_NumericTupleType getNumericTupleType(const GA_Attribute* const attrib)
    {
        UT_ASSERT_P(attrib);
        UT_ASSERT_P(attrib->getAIFTuple());
        return getNumericTupleType(attrib->getTupleSize(), attrib->getAIFTuple()->getStorage(attrib));
    }
    
    SYS_FORCE_INLINE GFE_NumericTupleType getNumericTupleType(const GA_Attribute& attrib)
    {
        UT_ASSERT_P(attrib.getAIFTuple());
        return getNumericTupleType(attrib.getTupleSize(), attrib.getAIFTuple()->getStorage(&attrib));
    }
    





} // End of namespace GFE_Type





template<typename _Ty>
using GFE_RWPageHandleT = GA_PageHandleT<_Ty, typename GFE_Type::get_value_type_t<_Ty>, true, true, GA_Attribute, GA_ATINumeric, GA_Detail>;

template<typename _Ty>
using GFE_ROPageHandleT = GA_PageHandleT<_Ty, typename GFE_Type::get_value_type_t<_Ty>, true, false, const GA_Attribute, const GA_ATINumeric, const GA_Detail>;











#endif
