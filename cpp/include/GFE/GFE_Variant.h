
#pragma once

#ifndef __GFE_Variant_h__
#define __GFE_Variant_h__

#include "GFE/GFE_Variant.h"


#include "GA/GA_Detail.h"



#include "GFE/GFE_Type.h"


namespace GFE_Variant {



    template <typename _Ty0, typename... _Ty1>
    struct variant_concat { using type = void; };

    //template <typename... _Ty0, typename... _Ty1>
    //struct variant_concat { using type = void; };

    //template <typename... _Ty0, typename... _Ty1>
    //struct variant_concat<std::variant<_Ty0...>, _Ty1...> { using type = std::variant<_Ty0..., _Ty1...>; };
    
    template <typename... _Ty0, typename... _Ty1>
    struct variant_concat<std::variant<_Ty0...>, std::variant<_Ty1...>> { using type = std::variant<_Ty0..., _Ty1...>; };
    
    //template <typename... _Ty0, typename... _Ty1, typename... _Ty2>
    //struct variant_concat<std::variant<_Ty0...>, std::variant<_Ty1...>, std::variant<_Ty2...>> { using type = std::variant<_Ty0..., _Ty1..., _Ty2...>; };
    
    template <typename... _Ty0, typename... _Ty1>
    using variant_concat_t = typename variant_concat<_Ty0..., _Ty1...>::type;

    //template <typename... _Ty0, typename... _Ty1, typename... _Ty2>
    //using variant_concat_t = typename variant_concat<_Ty0..., _Ty1..., _Ty2...>::type;


    
template <int _Val>
using attrib_tuplesize_constant = std::integral_constant<int, _Val>;

using attrib_tuplesize_1_type  = attrib_tuplesize_constant<1>;
using attrib_tuplesize_2_type  = attrib_tuplesize_constant<2>;
using attrib_tuplesize_3_type  = attrib_tuplesize_constant<3>;
using attrib_tuplesize_4_type  = attrib_tuplesize_constant<4>;
using attrib_tuplesize_9_type  = attrib_tuplesize_constant<9>;
using attrib_tuplesize_16_type = attrib_tuplesize_constant<16>;


using attrib_tuplesize_variant = std::variant<attrib_tuplesize_1_type ,
                                              attrib_tuplesize_2_type ,
                                              attrib_tuplesize_3_type ,
                                              attrib_tuplesize_4_type ,
                                              attrib_tuplesize_9_type ,
                                              attrib_tuplesize_16_type>;
        
SYS_FORCE_INLINE attrib_tuplesize_variant getAttribTupleSizeVariant(const int v)
{
    switch (v)
    {
    case 1:  return attrib_tuplesize_constant<1> {}; break;
    case 2:  return attrib_tuplesize_constant<2> {}; break;
    case 3:  return attrib_tuplesize_constant<3> {}; break;
    case 4:  return attrib_tuplesize_constant<4> {}; break;
    case 9:  return attrib_tuplesize_constant<9> {}; break;
    case 16: return attrib_tuplesize_constant<16>{}; break;
    default: break;
    }
    UT_ASSERT_MSG(0, "Unhandled Attrib Tuple Size");
    return attrib_tuplesize_constant<1>{};
}

        
SYS_FORCE_INLINE auto getAttribTupleSizeVariant(const GA_Attribute& attrib)
{ return getAttribTupleSizeVariant(attrib.getTupleSize()); }

SYS_FORCE_INLINE auto getAttribTupleSizeVariant(const GA_Attribute* const attrib)
{ UT_ASSERT_P(attrib); return getAttribTupleSizeVariant(*attrib); }





    
        
        template <GFE_NumericTupleType _Val>
        using numeric_tuple_type_constant = std::integral_constant<GFE_NumericTupleType, _Val>;

        using numeric_tuple_type_int8_type     = numeric_tuple_type_constant<GFE_NumericTupleType::int8>;
        using numeric_tuple_type_int16_type    = numeric_tuple_type_constant<GFE_NumericTupleType::int16>;
        using numeric_tuple_type_int32_type    = numeric_tuple_type_constant<GFE_NumericTupleType::int32>;
        using numeric_tuple_type_int64_type    = numeric_tuple_type_constant<GFE_NumericTupleType::int64>;
        using numeric_tuple_type_float16_type  = numeric_tuple_type_constant<GFE_NumericTupleType::fpreal16>;
        using numeric_tuple_type_float32_type  = numeric_tuple_type_constant<GFE_NumericTupleType::fpreal32>;
        using numeric_tuple_type_float64_type  = numeric_tuple_type_constant<GFE_NumericTupleType::fpreal64>;
        
#define __GFE_Variant_SpecializationVec(NUM)\
        using numeric_tuple_type_v##NUM##int32_type    = numeric_tuple_type_constant<GFE_NumericTupleType::UT_Vector##NUM##I>;\
        using numeric_tuple_type_v##NUM##int64_type    = numeric_tuple_type_constant<GFE_NumericTupleType::UT_Vector##NUM##L>;\
        using numeric_tuple_type_v##NUM##float16_type  = numeric_tuple_type_constant<GFE_NumericTupleType::UT_Vector##NUM##H>;\
        using numeric_tuple_type_v##NUM##float32_type  = numeric_tuple_type_constant<GFE_NumericTupleType::UT_Vector##NUM##F>;\
        using numeric_tuple_type_v##NUM##float64_type  = numeric_tuple_type_constant<GFE_NumericTupleType::UT_Vector##NUM##D>;\
    
        __GFE_Variant_SpecializationVec(2)
        __GFE_Variant_SpecializationVec(3)
        __GFE_Variant_SpecializationVec(4)
        
#undef __GFE_Variant_SpecializationVec
        

#define __GFE_Variant_SpecializationMtx(NUM)\
        using numeric_tuple_type_m##NUM##float32_type  = numeric_tuple_type_constant<GFE_NumericTupleType::UT_Matrix##NUM##F>;\
        using numeric_tuple_type_m##NUM##float64_type  = numeric_tuple_type_constant<GFE_NumericTupleType::UT_Matrix##NUM##D>;\
    
        __GFE_Variant_SpecializationMtx(2)
        __GFE_Variant_SpecializationMtx(3)
        __GFE_Variant_SpecializationMtx(4)
        
#undef __GFE_Variant_SpecializationMtx



    using numeric_tuple_type_1_variant = std::variant<numeric_tuple_type_int8_type   ,
                                                      numeric_tuple_type_int16_type  ,
                                                      numeric_tuple_type_int32_type  ,
                                                      numeric_tuple_type_int64_type  ,
                                                      numeric_tuple_type_float16_type,
                                                      numeric_tuple_type_float32_type,
                                                      numeric_tuple_type_float64_type>;

    
#define __GFE_Variant_SpecializationVec(NUM)\
        using numeric_tuple_type_v##NUM##_variant = std::variant<numeric_tuple_type_v##NUM##int32_type  ,  \
                                                                 numeric_tuple_type_v##NUM##int64_type  ,  \
                                                                 numeric_tuple_type_v##NUM##float16_type,  \
                                                                 numeric_tuple_type_v##NUM##float32_type,  \
                                                                 numeric_tuple_type_v##NUM##float64_type>; \
    
        __GFE_Variant_SpecializationVec(2)
        __GFE_Variant_SpecializationVec(3)
        __GFE_Variant_SpecializationVec(4)
        
#undef __GFE_Variant_SpecializationVec

    
#define __GFE_Variant_SpecializationVecF(NUM)\
        using numeric_tuple_type_v##NUM##f_variant = std::variant<numeric_tuple_type_v##NUM##float16_type,  \
                                                                 numeric_tuple_type_v##NUM##float32_type,   \
                                                                 numeric_tuple_type_v##NUM##float64_type>;  \
    
        __GFE_Variant_SpecializationVecF(2)
        __GFE_Variant_SpecializationVecF(3)
        __GFE_Variant_SpecializationVecF(4)
        
#undef __GFE_Variant_SpecializationVecF
        
    

    
#define __GFE_Variant_SpecializationMtx(NUM)\
        using numeric_tuple_type_m##NUM##_variant = std::variant<numeric_tuple_type_m##NUM##float32_type,  \
                                                                 numeric_tuple_type_m##NUM##float64_type>; \
    
        __GFE_Variant_SpecializationMtx(2)
        __GFE_Variant_SpecializationMtx(3)
        __GFE_Variant_SpecializationMtx(4)
        
#undef __GFE_Variant_SpecializationMtx

    
    //using numeric_tuple_type_v_variant   = typename variant_concat_t<numeric_tuple_type_v2_variant, numeric_tuple_type_v3_variant, numeric_tuple_type_v4_variant>;
    //using numeric_tuple_type_m_variant   = typename variant_concat_t<numeric_tuple_type_m2_variant, numeric_tuple_type_m3_variant, numeric_tuple_type_m4_variant>;
    using numeric_tuple_type_v_variant   = typename variant_concat_t<variant_concat_t<numeric_tuple_type_v2_variant, numeric_tuple_type_v3_variant>, numeric_tuple_type_v4_variant>;
    using numeric_tuple_type_m_variant   = typename variant_concat_t<variant_concat_t<numeric_tuple_type_m2_variant, numeric_tuple_type_m3_variant>, numeric_tuple_type_m4_variant>;
    using numeric_tuple_type_1v_variant  = typename variant_concat_t<numeric_tuple_type_1_variant,  numeric_tuple_type_v_variant>;
    using numeric_tuple_type_1vm_variant = typename variant_concat_t<numeric_tuple_type_1v_variant, numeric_tuple_type_m_variant>;
    using numeric_tuple_type_variant     = typename numeric_tuple_type_1vm_variant;


        

#define __GFE_Variant_Specialization1\
        case GFE_NumericTupleType::int8:     return numeric_tuple_type_constant<GFE_NumericTupleType::int8>    {}; break;\
        case GFE_NumericTupleType::int16:    return numeric_tuple_type_constant<GFE_NumericTupleType::int16>   {}; break;\
        case GFE_NumericTupleType::int32:    return numeric_tuple_type_constant<GFE_NumericTupleType::int32>   {}; break;\
        case GFE_NumericTupleType::int64:    return numeric_tuple_type_constant<GFE_NumericTupleType::int64>   {}; break;\
        case GFE_NumericTupleType::fpreal16: return numeric_tuple_type_constant<GFE_NumericTupleType::fpreal16>{}; break;\
        case GFE_NumericTupleType::fpreal32: return numeric_tuple_type_constant<GFE_NumericTupleType::fpreal32>{}; break;\
        case GFE_NumericTupleType::fpreal64: return numeric_tuple_type_constant<GFE_NumericTupleType::fpreal64>{}; break;\
    
        
        
#define __GFE_Variant_SpecializationVec(NUM)\
        case GFE_NumericTupleType::UT_Vector##NUM##I: return numeric_tuple_type_constant<GFE_NumericTupleType::UT_Vector##NUM##I>{}; break;\
        case GFE_NumericTupleType::UT_Vector##NUM##L: return numeric_tuple_type_constant<GFE_NumericTupleType::UT_Vector##NUM##L>{}; break;\
        case GFE_NumericTupleType::UT_Vector##NUM##H: return numeric_tuple_type_constant<GFE_NumericTupleType::UT_Vector##NUM##H>{}; break;\
        case GFE_NumericTupleType::UT_Vector##NUM##F: return numeric_tuple_type_constant<GFE_NumericTupleType::UT_Vector##NUM##F>{}; break;\
        case GFE_NumericTupleType::UT_Vector##NUM##D: return numeric_tuple_type_constant<GFE_NumericTupleType::UT_Vector##NUM##D>{}; break;\
    
        
#define __GFE_Variant_SpecializationVecF(NUM)\
        case GFE_NumericTupleType::UT_Vector##NUM##H: return numeric_tuple_type_constant<GFE_NumericTupleType::UT_Vector##NUM##H>{}; break;\
        case GFE_NumericTupleType::UT_Vector##NUM##F: return numeric_tuple_type_constant<GFE_NumericTupleType::UT_Vector##NUM##F>{}; break;\
        case GFE_NumericTupleType::UT_Vector##NUM##D: return numeric_tuple_type_constant<GFE_NumericTupleType::UT_Vector##NUM##D>{}; break;\

    
#define __GFE_Variant_SpecializationV\
        __GFE_Variant_SpecializationVec(2)\
        __GFE_Variant_SpecializationVec(3)\
        __GFE_Variant_SpecializationVec(4)\

    
#define __GFE_Variant_SpecializationMtx(NUM)\
        case GFE_NumericTupleType::UT_Matrix##NUM##F: return numeric_tuple_type_constant<GFE_NumericTupleType::UT_Matrix##NUM##F>{}; break;\
        case GFE_NumericTupleType::UT_Matrix##NUM##D: return numeric_tuple_type_constant<GFE_NumericTupleType::UT_Matrix##NUM##D>{}; break;\

    
#define __GFE_Variant_SpecializationM\
        __GFE_Variant_SpecializationMtx(2)\
        __GFE_Variant_SpecializationMtx(3)\
        __GFE_Variant_SpecializationMtx(4)\


    
    
    numeric_tuple_type_1_variant getNumericTupleType1Variant(const GFE_NumericTupleType v)
    {
        switch (v)
        {
            __GFE_Variant_Specialization1
            default:  break;
        }
        UT_ASSERT_MSG(0, "Unhandled Attrib Tuple Type");
        return numeric_tuple_type_constant<GFE_NumericTupleType::float32>{};
    }
    
    numeric_tuple_type_v3_variant getNumericTupleTypev3Variant(const GFE_NumericTupleType v)
    {
        switch (v)
        {
            __GFE_Variant_SpecializationVec(3)
            default:  break;
        }
        UT_ASSERT_MSG(0, "Unhandled Attrib Tuple Type");
        return numeric_tuple_type_constant<GFE_NumericTupleType::UT_Vector3F>{};
    }
    
    numeric_tuple_type_v3f_variant getNumericTupleTypev3fVariant(const GFE_NumericTupleType v)
    {
        switch (v)
        {
            __GFE_Variant_SpecializationVecF(3)
            default:  break;
        }
        UT_ASSERT_MSG(0, "Unhandled Attrib Tuple Type");
        return numeric_tuple_type_constant<GFE_NumericTupleType::UT_Vector3F>{};
    }
    
    numeric_tuple_type_1v_variant getNumericTupleType1vVariant(const GFE_NumericTupleType v)
    {
        switch (v)
        {
            __GFE_Variant_Specialization1
            __GFE_Variant_SpecializationV
            default:  break;
        }
        UT_ASSERT_MSG(0, "Unhandled Attrib Tuple Type");
        return numeric_tuple_type_constant<GFE_NumericTupleType::float32>{};
    }

    numeric_tuple_type_1vm_variant getNumericTupleType1vmVariant(const GFE_NumericTupleType v)
    {
        switch (v)
        {
            __GFE_Variant_Specialization1
            __GFE_Variant_SpecializationV
            __GFE_Variant_SpecializationM
            default:  break;
        }
        UT_ASSERT_MSG(0, "Unhandled Attrib Tuple Type");
        return numeric_tuple_type_constant<GFE_NumericTupleType::float32>{};
    }

#undef __GFE_Variant_Specialization1
#undef __GFE_Variant_SpecializationVec
#undef __GFE_Variant_SpecializationV
#undef __GFE_Variant_SpecializationMtx
#undef __GFE_Variant_SpecializationM

    
    SYS_FORCE_INLINE auto getNumericTupleType1Variant(const GA_Attribute& attrib)
    { return getNumericTupleType1Variant(GFE_Type::getNumericTupleType(attrib)); }
    
    SYS_FORCE_INLINE auto getNumericTupleTypev3Variant(const GA_Attribute& attrib)
    { return getNumericTupleTypev3Variant(GFE_Type::getNumericTupleType(attrib)); }
    
    SYS_FORCE_INLINE auto getNumericTupleTypev3fVariant(const GA_Attribute& attrib)
    { return getNumericTupleTypev3fVariant(GFE_Type::getNumericTupleType(attrib)); }
    
    SYS_FORCE_INLINE auto getNumericTupleType1vVariant(const GA_Attribute& attrib)
    { return getNumericTupleType1vVariant(GFE_Type::getNumericTupleType(attrib)); }

    SYS_FORCE_INLINE auto getNumericTupleType1vmVariant(const GA_Attribute& attrib)
    { return getNumericTupleType1vmVariant(GFE_Type::getNumericTupleType(attrib)); }


        

/*
         
template <GA_Storage _Val>
using attrib_store_constant = std::integral_constant<GA_Storage, _Val>;

using attrib_store_invalid_type = attrib_store_constant<GA_STORE_INVALID>;
using attrib_store_b_type       = attrib_store_constant<GA_STORE_BOOL>;
using attrib_store_i8_type      = attrib_store_constant<GA_STORE_INT8>;
using attrib_store_i16_type     = attrib_store_constant<GA_STORE_INT16>;
using attrib_store_i32_type     = attrib_store_constant<GA_STORE_INT32>;
using attrib_store_i64_type     = attrib_store_constant<GA_STORE_INT64>;
using attrib_store_f16_type     = attrib_store_constant<GA_STORE_REAL16>;
using attrib_store_f32_type     = attrib_store_constant<GA_STORE_REAL32>;
using attrib_store_f64_type     = attrib_store_constant<GA_STORE_REAL64>;
using attrib_store_fs_type      = attrib_store_constant<GA_STORE_STRING>;
using attrib_store_fd_type      = attrib_store_constant<GA_STORE_DICT>;



using attrib_store_variant = std::variant<attrib_store_invalid_type,
                                          attrib_store_b_type      ,
                                          attrib_store_i8_type     ,
                                          attrib_store_i16_type    ,
                                          attrib_store_i32_type    ,
                                          attrib_store_i64_type    ,
                                          attrib_store_f16_type    ,
                                          attrib_store_f32_type    ,
                                          attrib_store_f64_type    ,
                                          attrib_store_fs_type     ,
                                          attrib_store_fd_type     >;
        
SYS_FORCE_INLINE attrib_store_variant getAttribStorageVariant(const GA_Storage v)
{
    switch (v)
    {
    case GA_STORE_INVALID: return attrib_store_constant<GA_STORE_INVALID>{}; break;
    case GA_STORE_INT8:    return attrib_store_constant<GA_STORE_INT8>   {}; break;
    case GA_STORE_INT16:   return attrib_store_constant<GA_STORE_INT16>  {}; break;
    case GA_STORE_INT32:   return attrib_store_constant<GA_STORE_INT32>  {}; break;
    case GA_STORE_INT64:   return attrib_store_constant<GA_STORE_INT64>  {}; break;
    case GA_STORE_REAL16:  return attrib_store_constant<GA_STORE_REAL16> {}; break;
    case GA_STORE_REAL32:  return attrib_store_constant<GA_STORE_REAL32> {}; break;
    case GA_STORE_REAL64:  return attrib_store_constant<GA_STORE_REAL64> {}; break;
    case GA_STORE_STRING:  return attrib_store_constant<GA_STORE_STRING> {}; break;
    case GA_STORE_DICT:    return attrib_store_constant<GA_STORE_DICT>   {}; break;
    default: break;
    }
    UT_ASSERT_MSG(0, "Unhandled Attrib Storage");
    return attrib_store_constant<GA_STORE_INVALID>{};
}

        
SYS_FORCE_INLINE auto getAttribStorageVariant(const GA_Attribute& attrib)
{ return getAttribStorageVariant(attrib.getAIFTuple()->getStorage(&attrib)); }

SYS_FORCE_INLINE auto getAttribStorageVariant(const GA_Attribute* const attrib)
{ UT_ASSERT_P(attrib); return getAttribStorageVariant(attrib->getAIFTuple()->getStorage(attrib)); }

*/


        

        
        
template <GA_AttributeOwner _Val>
using attrib_owner_constant = std::integral_constant<GA_AttributeOwner, _Val>;

using attrib_owner_invalid_type = attrib_owner_constant<GA_ATTRIB_INVALID>;
using attrib_owner_prim_type    = attrib_owner_constant<GA_ATTRIB_PRIMITIVE>;
using attrib_owner_point_type   = attrib_owner_constant<GA_ATTRIB_POINT>;
using attrib_owner_vertex_type  = attrib_owner_constant<GA_ATTRIB_VERTEX>;
using attrib_owner_detail_type  = attrib_owner_constant<GA_ATTRIB_DETAIL>;
using attrib_owner_n_type       = attrib_owner_constant<GA_ATTRIB_OWNER_N>;

        
using attrib_owner_variant = std::variant<attrib_owner_prim_type   ,
                                          attrib_owner_point_type  ,
                                          attrib_owner_vertex_type ,
                                          attrib_owner_detail_type >;

        
SYS_FORCE_INLINE attrib_owner_variant getAttribOwnerVariant(const GA_AttributeOwner v)
{
    switch (v)
    {
    case GA_ATTRIB_PRIMITIVE: return attrib_owner_constant<GA_ATTRIB_PRIMITIVE>{}; break;
    case GA_ATTRIB_POINT:     return attrib_owner_constant<GA_ATTRIB_POINT    >{}; break;
    case GA_ATTRIB_VERTEX:    return attrib_owner_constant<GA_ATTRIB_VERTEX   >{}; break;
    case GA_ATTRIB_DETAIL:    return attrib_owner_constant<GA_ATTRIB_DETAIL   >{}; break;
    default: break;
    }
    UT_ASSERT_MSG(0, "Unhandled Attrib Owner");
    return attrib_owner_constant<GA_ATTRIB_PRIMITIVE>{};
}

SYS_FORCE_INLINE auto getAttribOwnerVariant(const GA_Attribute& attrib)
{ return getAttribOwnerVariant(attrib.getOwner()); }

SYS_FORCE_INLINE auto getAttribOwnerVariant(const GA_Attribute* const attrib)
{ UT_ASSERT_P(attrib); return getAttribOwnerVariant(*attrib); }


        
using bool_variant = std::variant<std::true_type, std::false_type>;

bool_variant getBoolVariant(const bool v)
{
    if (v)
        return std::true_type{};
    else
        return std::false_type{};
}







    
    template <GFE_NumericTupleType _Ty>
    struct get_numeric_tuple_type { using type = void; };

    template <>
    struct get_numeric_tuple_type<GFE_NumericTupleType::int8>     { using type = int8; };
    template <>
    struct get_numeric_tuple_type<GFE_NumericTupleType::int16>    { using type = int16; };
    template <>
    struct get_numeric_tuple_type<GFE_NumericTupleType::int32>    { using type = int32; };
    template <>
    struct get_numeric_tuple_type<GFE_NumericTupleType::int64>    { using type = int64; };
    template <>
    struct get_numeric_tuple_type<GFE_NumericTupleType::fpreal16> { using type = fpreal16; };
    template <>
    struct get_numeric_tuple_type<GFE_NumericTupleType::fpreal32> { using type = fpreal32; };
    template <>
    struct get_numeric_tuple_type<GFE_NumericTupleType::fpreal64> { using type = fpreal64; };

#define __GFE_Variant_SpecializationVec(NUM)\
    template <>\
    struct get_numeric_tuple_type<GFE_NumericTupleType::UT_Vector##NUM##I> { using type = UT_Vector##NUM##i; };\
    template <>\
    struct get_numeric_tuple_type<GFE_NumericTupleType::UT_Vector##NUM##L> { using type = UT_Vector##NUM##I; };\
    template <>\
    struct get_numeric_tuple_type<GFE_NumericTupleType::UT_Vector##NUM##H> { using type = UT_Vector##NUM##H; };\
    template <>\
    struct get_numeric_tuple_type<GFE_NumericTupleType::UT_Vector##NUM##F> { using type = UT_Vector##NUM##F; };\
    template <>\
    struct get_numeric_tuple_type<GFE_NumericTupleType::UT_Vector##NUM##D> { using type = UT_Vector##NUM##D; };\
    
        __GFE_Variant_SpecializationVec(2)
        __GFE_Variant_SpecializationVec(3)
        __GFE_Variant_SpecializationVec(4)
        
#undef __GFE_Variant_SpecializationVec
        
#define __GFE_Variant_SpecializationMtx(NUM)\
    template <>\
    struct get_numeric_tuple_type<GFE_NumericTupleType::UT_Matrix##NUM##F> { using type = UT_Matrix##NUM##F; };\
    template <>\
    struct get_numeric_tuple_type<GFE_NumericTupleType::UT_Matrix##NUM##D> { using type = UT_Matrix##NUM##D; };\
    
        __GFE_Variant_SpecializationMtx(2)
        __GFE_Variant_SpecializationMtx(3)
        __GFE_Variant_SpecializationMtx(4)
        
#undef __GFE_Variant_SpecializationMtx

template <GFE_NumericTupleType _Ty>
using get_numeric_tuple_type_t = typename get_numeric_tuple_type<_Ty>::type;



/*
    
    template <GA_AttributeOwner _Ty>
    struct get_owner { using type = void; };

    template <>
    struct get_owner<GA_ATTRIB_PRIMITIVE>     { using type = GA_ATTRIB_PRIMITIVE; };
    template <>
    struct get_owner<GA_ATTRIB_POINT>    { using type = GA_ATTRIB_POINT; };
    template <>
    struct get_owner<GA_ATTRIB_VERTEX>    { using type = GA_ATTRIB_VERTEX; };
    template <>
    struct get_owner<GA_ATTRIB_DETAIL>    { using type = GA_ATTRIB_DETAIL; };

    
template <GA_AttributeOwner _Ty>
using get_owner_t = typename get_owner<_Ty>::type;
*/



} // End of namespace GFE_Variant












#endif
