
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

    template<typename _Ty, typename _TVarient>
    struct isVariantMember;
    
    template<typename _Ty, typename... _TVarientMember>
    struct isVariantMember<_Ty, std::variant<_TVarientMember...>> : public std::disjunction<std::is_same<_Ty, _TVarientMember>...> {};

    //template<typename _Ty, typename... ALL_T>
    //_INLINE_VAR constexpr bool isVariantMember_v = isVariantMember<_Ty, std::variant<ALL_T...>>::value;



    
template <int _Val>
using attribTupleSizeConstant = std::integral_constant<int, _Val>;

using attribTupleSizeConstant1  = attribTupleSizeConstant<1>;
using attribTupleSizeConstant2  = attribTupleSizeConstant<2>;
using attribTupleSizeConstant3  = attribTupleSizeConstant<3>;
using attribTupleSizeConstant4  = attribTupleSizeConstant<4>;
using attribTupleSizeConstant9  = attribTupleSizeConstant<9>;
using attribTupleSizeConstant16 = attribTupleSizeConstant<16>;
    
using attribTupleSizeVariant = std::variant<attribTupleSizeConstant1 ,
                                            attribTupleSizeConstant2 ,
                                            attribTupleSizeConstant3 ,
                                            attribTupleSizeConstant4 ,
                                            attribTupleSizeConstant9 ,
                                            attribTupleSizeConstant16>;
        
SYS_FORCE_INLINE attribTupleSizeVariant getAttribTupleSizeVariant(const int v)
{
    switch (v)
    {
    case 1:  return attribTupleSizeConstant<1> {}; break;
    case 2:  return attribTupleSizeConstant<2> {}; break;
    case 3:  return attribTupleSizeConstant<3> {}; break;
    case 4:  return attribTupleSizeConstant<4> {}; break;
    case 9:  return attribTupleSizeConstant<9> {}; break;
    case 16: return attribTupleSizeConstant<16>{}; break;
    default: break;
    }
    UT_ASSERT_MSG(0, "Unhandled Attrib Tuple Size");
    return attribTupleSizeConstant<1>{};
}

        
SYS_FORCE_INLINE auto getAttribTupleSizeVariant(const GA_Attribute& attrib)
{ return getAttribTupleSizeVariant(attrib.getTupleSize()); }

SYS_FORCE_INLINE auto getAttribTupleSizeVariant(const GA_Attribute* const attrib)
{ UT_ASSERT_P(attrib); return getAttribTupleSizeVariant(*attrib); }


    
#define __GFE_Variant_CreateMacroStorage(MacroName) \
        MacroName(I)                                \
        MacroName(F)                                \
        MacroName(S)                                \
        MacroName(IF)                               \
        MacroName(IS)                               \
        MacroName(V2)                               \
        MacroName(V3)                               \
        MacroName(V4)                               \
        MacroName(V2I)                              \
        MacroName(V3I)                              \
        MacroName(V4I)                              \
        MacroName(V2F)                              \
        MacroName(V3F)                              \
        MacroName(V4F)                              \
        MacroName(VI)                               \
        MacroName(VF)                               \
        MacroName(VM)                               \
        MacroName(IFV)                              \
        MacroName(IFVM)                             \


    
    
    template <GFE_AttribStorage _Val>
    using attribStorageConstant = std::integral_constant<GFE_AttribStorage, _Val>;
    
    using attribStorageConstantI8   = attribStorageConstant<GFE_AttribStorage::int8>;
    using attribStorageConstantI16  = attribStorageConstant<GFE_AttribStorage::int16>;
    using attribStorageConstantI32  = attribStorageConstant<GFE_AttribStorage::int32>;
    using attribStorageConstantI64  = attribStorageConstant<GFE_AttribStorage::int64>;
    using attribStorageConstantF16  = attribStorageConstant<GFE_AttribStorage::fpreal16>;
    using attribStorageConstantF32  = attribStorageConstant<GFE_AttribStorage::fpreal32>;
    using attribStorageConstantF64  = attribStorageConstant<GFE_AttribStorage::fpreal64>;
    using attribStorageConstantS    = attribStorageConstant<GFE_AttribStorage::string>;
    
#define __GFE_Variant_SpecializationConstantV(NUM)\
        using attribStorageConstantV##NUM##I32  = attribStorageConstant<GFE_AttribStorage::UT_Vector##NUM##I>;\
        using attribStorageConstantV##NUM##I64  = attribStorageConstant<GFE_AttribStorage::UT_Vector##NUM##L>;\
        using attribStorageConstantV##NUM##F16  = attribStorageConstant<GFE_AttribStorage::UT_Vector##NUM##H>;\
        using attribStorageConstantV##NUM##F32  = attribStorageConstant<GFE_AttribStorage::UT_Vector##NUM##F>;\
        using attribStorageConstantV##NUM##F64  = attribStorageConstant<GFE_AttribStorage::UT_Vector##NUM##D>;\
    
        __GFE_Variant_SpecializationConstantV(2)
        __GFE_Variant_SpecializationConstantV(3)
        __GFE_Variant_SpecializationConstantV(4)
        
#undef __GFE_Variant_SpecializationConstantV
        

#define __GFE_Variant_SpecializationConstantM(NUM)\
        using attribStorageConstantM##NUM##F32 = attribStorageConstant<GFE_AttribStorage::UT_Matrix##NUM##F>;\
        using attribStorageConstantM##NUM##F64 = attribStorageConstant<GFE_AttribStorage::UT_Matrix##NUM##D>;\
    
        __GFE_Variant_SpecializationConstantM(2)
        __GFE_Variant_SpecializationConstantM(3)
        __GFE_Variant_SpecializationConstantM(4)
        
#undef __GFE_Variant_SpecializationConstantM

    using attribStorageVariantS = std::variant<attribStorageConstantS>;

    using attribStorageVariantI = std::variant<attribStorageConstantI8 ,
                                               attribStorageConstantI16,
                                               attribStorageConstantI32,
                                               attribStorageConstantI64>;

    using attribStorageVariantF = std::variant<attribStorageConstantF16,
                                               attribStorageConstantF32,
                                               attribStorageConstantF64>;
    
    using attribStorageVariantIF = typename variant_concat_t<attribStorageVariantI, attribStorageVariantF>;
    
    using attribStorageVariantIS = typename variant_concat_t<attribStorageVariantI, attribStorageVariantS>;

#define __GFE_Variant_SpecializationConstantVI(NUM)                                           \
        using attribStorageVariantV##NUM##I = std::variant<attribStorageConstantV##NUM##I32,  \
                                                           attribStorageConstantV##NUM##I64>; \
    
        __GFE_Variant_SpecializationConstantVI(2)
        __GFE_Variant_SpecializationConstantVI(3)
        __GFE_Variant_SpecializationConstantVI(4)
        
#undef __GFE_Variant_SpecializationConstantVI

    
#define __GFE_Variant_SpecializationConstantVF(NUM) \
        using attribStorageVariantV##NUM##F = std::variant<attribStorageConstantV##NUM##F16,  \
                                                           attribStorageConstantV##NUM##F32,  \
                                                           attribStorageConstantV##NUM##F64>; \
    
        __GFE_Variant_SpecializationConstantVF(2)
        __GFE_Variant_SpecializationConstantVF(3)
        __GFE_Variant_SpecializationConstantVF(4)
        
#undef __GFE_Variant_SpecializationConstantVF
        
    
#define __GFE_Variant_SpecializationConstantV(NUM)  \
        using attribStorageVariantV##NUM## = typename variant_concat_t<attribStorageVariantV##NUM##I, attribStorageVariantV##NUM##F>;
    
        __GFE_Variant_SpecializationConstantV(2)
        __GFE_Variant_SpecializationConstantV(3)
        __GFE_Variant_SpecializationConstantV(4)
        
#undef __GFE_Variant_SpecializationConstantV

    
#define __GFE_Variant_SpecializationConstantM(NUM)\
        using attribStorageVariantM##NUM## = std::variant<attribStorageConstantM##NUM##F32,  \
                                                          attribStorageConstantM##NUM##F64>; \
    
        __GFE_Variant_SpecializationConstantM(2)
        __GFE_Variant_SpecializationConstantM(3)
        __GFE_Variant_SpecializationConstantM(4)
        
#undef __GFE_Variant_SpecializationConstantM

    
    //using attribStorageVariantV   = typename variant_concat_t<attribStorageVariantV2, attribStorageVariantV3, attribStorageVariantV4>;
    //using attribStorageVariantM   = typename variant_concat_t<attribStorageVariantM2, attribStorageVariantM3, attribStorageVariantM4>;
    using attribStorageVariantV    = typename variant_concat_t<variant_concat_t<attribStorageVariantV2, attribStorageVariantV3>, attribStorageVariantV4>;
    using attribStorageVariantM    = typename variant_concat_t<variant_concat_t<attribStorageVariantM2, attribStorageVariantM3>, attribStorageVariantM4>;
    using attribStorageVariantVF   = typename variant_concat_t<variant_concat_t<attribStorageVariantV2F, attribStorageVariantV3F>, attribStorageVariantV4F>;
    using attribStorageVariantVI   = typename variant_concat_t<variant_concat_t<attribStorageVariantV2I, attribStorageVariantV3I>, attribStorageVariantV4I>;
    using attribStorageVariantIFV  = typename variant_concat_t<attribStorageVariantIF,  attribStorageVariantV>;
    using attribStorageVariantVM   = typename variant_concat_t<attribStorageVariantV,   attribStorageVariantM>;
    using attribStorageVariantIFVM = typename variant_concat_t<attribStorageVariantIFV, attribStorageVariantM>;




    

#define __GFE_Variant_SpecializationSwitchS\
        case GFE_AttribStorage::string:   return attribStorageConstant<GFE_AttribStorage::string>  {}; break;\

#define __GFE_Variant_SpecializationSwitchI\
        case GFE_AttribStorage::int8:     return attribStorageConstant<GFE_AttribStorage::int8>    {}; break;\
        case GFE_AttribStorage::int16:    return attribStorageConstant<GFE_AttribStorage::int16>   {}; break;\
        case GFE_AttribStorage::int32:    return attribStorageConstant<GFE_AttribStorage::int32>   {}; break;\
        case GFE_AttribStorage::int64:    return attribStorageConstant<GFE_AttribStorage::int64>   {}; break;\
    
#define __GFE_Variant_SpecializationSwitchF\
        case GFE_AttribStorage::fpreal16: return attribStorageConstant<GFE_AttribStorage::fpreal16>{}; break;\
        case GFE_AttribStorage::fpreal32: return attribStorageConstant<GFE_AttribStorage::fpreal32>{}; break;\
        case GFE_AttribStorage::fpreal64: return attribStorageConstant<GFE_AttribStorage::fpreal64>{}; break;\

#define __GFE_Variant_SpecializationSwitchVIN(NUM)\
        case GFE_AttribStorage::UT_Vector##NUM##I: return attribStorageConstant<GFE_AttribStorage::UT_Vector##NUM##I>{}; break;\
        case GFE_AttribStorage::UT_Vector##NUM##L: return attribStorageConstant<GFE_AttribStorage::UT_Vector##NUM##L>{}; break;\

#define __GFE_Variant_SpecializationSwitchVFN(NUM)\
        case GFE_AttribStorage::UT_Vector##NUM##H: return attribStorageConstant<GFE_AttribStorage::UT_Vector##NUM##H>{}; break;\
        case GFE_AttribStorage::UT_Vector##NUM##F: return attribStorageConstant<GFE_AttribStorage::UT_Vector##NUM##F>{}; break;\
        case GFE_AttribStorage::UT_Vector##NUM##D: return attribStorageConstant<GFE_AttribStorage::UT_Vector##NUM##D>{}; break;\

#define __GFE_Variant_SpecializationSwitchVN(NUM)   \
        __GFE_Variant_SpecializationSwitchVIN(NUM)  \
        __GFE_Variant_SpecializationSwitchVFN(NUM)  \
    

#define __GFE_Variant_SpecializationSwitchMN(NUM) \
        case GFE_AttribStorage::UT_Matrix##NUM##F: return attribStorageConstant<GFE_AttribStorage::UT_Matrix##NUM##F>{}; break;\
        case GFE_AttribStorage::UT_Matrix##NUM##D: return attribStorageConstant<GFE_AttribStorage::UT_Matrix##NUM##D>{}; break;\

    
#define __GFE_Variant_SpecializationSwitchVI       \
        __GFE_Variant_SpecializationSwitchVIN(2)   \
        __GFE_Variant_SpecializationSwitchVIN(3)   \
        __GFE_Variant_SpecializationSwitchVIN(4)   \

    
#define __GFE_Variant_SpecializationSwitchVF       \
        __GFE_Variant_SpecializationSwitchVFN(2)   \
        __GFE_Variant_SpecializationSwitchVFN(3)   \
        __GFE_Variant_SpecializationSwitchVFN(4)   \

    
#define __GFE_Variant_SpecializationSwitchV2I __GFE_Variant_SpecializationSwitchVIN(2)
#define __GFE_Variant_SpecializationSwitchV3I __GFE_Variant_SpecializationSwitchVIN(3)
#define __GFE_Variant_SpecializationSwitchV4I __GFE_Variant_SpecializationSwitchVIN(4)

#define __GFE_Variant_SpecializationSwitchV2F __GFE_Variant_SpecializationSwitchVFN(2)
#define __GFE_Variant_SpecializationSwitchV3F __GFE_Variant_SpecializationSwitchVFN(3)
#define __GFE_Variant_SpecializationSwitchV4F __GFE_Variant_SpecializationSwitchVFN(4)

    
#define __GFE_Variant_SpecializationSwitchIF \
        __GFE_Variant_SpecializationSwitchI  \
        __GFE_Variant_SpecializationSwitchF  \
        
#define __GFE_Variant_SpecializationSwitchIS \
        __GFE_Variant_SpecializationSwitchI  \
        __GFE_Variant_SpecializationSwitchS  \
        
#define __GFE_Variant_SpecializationSwitchV      \
        __GFE_Variant_SpecializationSwitchVN(2)  \
        __GFE_Variant_SpecializationSwitchVN(3)  \
        __GFE_Variant_SpecializationSwitchVN(4)  \

#define __GFE_Variant_SpecializationSwitchV2 __GFE_Variant_SpecializationSwitchVN(2)
#define __GFE_Variant_SpecializationSwitchV3 __GFE_Variant_SpecializationSwitchVN(3)
#define __GFE_Variant_SpecializationSwitchV4 __GFE_Variant_SpecializationSwitchVN(4)

#define __GFE_Variant_SpecializationSwitchM      \
        __GFE_Variant_SpecializationSwitchMN(2)  \
        __GFE_Variant_SpecializationSwitchMN(3)  \
        __GFE_Variant_SpecializationSwitchMN(4)  \

#define __GFE_Variant_SpecializationSwitchM2 __GFE_Variant_SpecializationSwitchMN(2)
#define __GFE_Variant_SpecializationSwitchM3 __GFE_Variant_SpecializationSwitchMN(3)
#define __GFE_Variant_SpecializationSwitchM4 __GFE_Variant_SpecializationSwitchMN(4)

#define __GFE_Variant_SpecializationSwitchVM    \
        __GFE_Variant_SpecializationSwitchV     \
        __GFE_Variant_SpecializationSwitchM     \

#define __GFE_Variant_SpecializationSwitchIFV   \
        __GFE_Variant_SpecializationSwitchIF    \
        __GFE_Variant_SpecializationSwitchV     \
        
#define __GFE_Variant_SpecializationSwitchIFVM  \
        __GFE_Variant_SpecializationSwitchIFV   \
        __GFE_Variant_SpecializationSwitchM     \






    
#define __GFE_Variant_SpecializationReturnStorageConstantReflection(STORAGE, STORAGE1)                                                    \
        else if constexpr (isVariantMember<attribStorageConstant<GFE_AttribStorage::STORAGE1>, attribStorageVariant##STORAGE>::value)     \
            return attribStorageConstant<GFE_AttribStorage::STORAGE1>{};                                                                  \


    
    
#define __GFE_Variant_SpecializationGetAttribStorageVariant(STORAGE)                                                              \
        attribStorageVariant##STORAGE getAttribStorageVariant##STORAGE(const GFE_AttribStorage v)                                 \
        {                                                                                                                         \
            switch (v)                                                                                                            \
            {                                                                                                                     \
                __GFE_Variant_SpecializationSwitch##STORAGE                                                                       \
                default:  break;                                                                                                  \
            }                                                                                                                     \
            UT_ASSERT_MSG(0, "Unhandled Attrib Tuple Type");                                                                      \
        if constexpr (0)                                                                                                          \
            return attribStorageConstant<GFE_AttribStorage::invalid>{};                                                           \
        __GFE_Variant_SpecializationReturnStorageConstantReflection(STORAGE, int32)                                               \
        __GFE_Variant_SpecializationReturnStorageConstantReflection(STORAGE, int64)                                               \
        __GFE_Variant_SpecializationReturnStorageConstantReflection(STORAGE, float32)                                             \
        __GFE_Variant_SpecializationReturnStorageConstantReflection(STORAGE, float64)                                             \
        __GFE_Variant_SpecializationReturnStorageConstantReflection(STORAGE, UT_Vector2F)                                         \
        __GFE_Variant_SpecializationReturnStorageConstantReflection(STORAGE, UT_Vector3F)                                         \
        __GFE_Variant_SpecializationReturnStorageConstantReflection(STORAGE, UT_Vector4F)                                         \
        __GFE_Variant_SpecializationReturnStorageConstantReflection(STORAGE, UT_Matrix2F)                                         \
        __GFE_Variant_SpecializationReturnStorageConstantReflection(STORAGE, UT_Matrix3F)                                         \
        __GFE_Variant_SpecializationReturnStorageConstantReflection(STORAGE, UT_Matrix4F)                                         \
        __GFE_Variant_SpecializationReturnStorageConstantReflection(STORAGE, UT_Vector2I)                                         \
        __GFE_Variant_SpecializationReturnStorageConstantReflection(STORAGE, UT_Vector3I)                                         \
        __GFE_Variant_SpecializationReturnStorageConstantReflection(STORAGE, UT_Vector4I)                                         \
        __GFE_Variant_SpecializationReturnStorageConstantReflection(STORAGE, string)                                              \
        __GFE_Variant_SpecializationReturnStorageConstantReflection(STORAGE, dict)                                                \
        __GFE_Variant_SpecializationReturnStorageConstantReflection(STORAGE, int32Array)                                          \
        __GFE_Variant_SpecializationReturnStorageConstantReflection(STORAGE, int64Array)                                          \
        __GFE_Variant_SpecializationReturnStorageConstantReflection(STORAGE, fpreal32Array)                                       \
        __GFE_Variant_SpecializationReturnStorageConstantReflection(STORAGE, fpreal64Array)                                       \
        __GFE_Variant_SpecializationReturnStorageConstantReflection(STORAGE, UT_Vector2FArray)                                    \
        __GFE_Variant_SpecializationReturnStorageConstantReflection(STORAGE, UT_Vector3FArray)                                    \
        __GFE_Variant_SpecializationReturnStorageConstantReflection(STORAGE, UT_Vector4FArray)                                    \
        __GFE_Variant_SpecializationReturnStorageConstantReflection(STORAGE, UT_Matrix2FArray)                                    \
        __GFE_Variant_SpecializationReturnStorageConstantReflection(STORAGE, UT_Matrix3FArray)                                    \
        __GFE_Variant_SpecializationReturnStorageConstantReflection(STORAGE, UT_Matrix4FArray)                                    \
        __GFE_Variant_SpecializationReturnStorageConstantReflection(STORAGE, stringArray)                                         \
        __GFE_Variant_SpecializationReturnStorageConstantReflection(STORAGE, dictArray)                                           \
        }                                                                                                                         \

    __GFE_Variant_CreateMacroStorage(__GFE_Variant_SpecializationGetAttribStorageVariant)
    
#undef __GFE_Variant_SpecializationGetAttribStorageVariant

#undef __GFE_Variant_SpecializationReturnStorageConstantReflection
    
#undef __GFE_Variant_SpecializationGetAttribStorageVariant

    
#define __GFE_Variant_SpecializationGetAttribStorageVariant(STORAGE)                       \
        SYS_FORCE_INLINE auto getAttribStorageVariant##STORAGE(const GA_Attribute& attrib) \
        { return getAttribStorageVariant##STORAGE(GFE_Type::getAttribStorage(attrib)); }   \
    
    __GFE_Variant_CreateMacroStorage(__GFE_Variant_SpecializationGetAttribStorageVariant)
    
#undef __GFE_Variant_SpecializationGetAttribStorageVariant


    
#undef __GFE_Variant_SpecializationSwitchI
#undef __GFE_Variant_SpecializationSwitchF
#undef __GFE_Variant_SpecializationSwitchS
#undef __GFE_Variant_SpecializationSwitchV
#undef __GFE_Variant_SpecializationSwitchM
    
#undef __GFE_Variant_SpecializationSwitchIF
#undef __GFE_Variant_SpecializationSwitchIS
#undef __GFE_Variant_SpecializationSwitchV2F
#undef __GFE_Variant_SpecializationSwitchV3F
#undef __GFE_Variant_SpecializationSwitchV4F
#undef __GFE_Variant_SpecializationSwitchVF
    
#undef __GFE_Variant_SpecializationSwitchV2I
#undef __GFE_Variant_SpecializationSwitchV3I
#undef __GFE_Variant_SpecializationSwitchV4I
#undef __GFE_Variant_SpecializationSwitchVI
    
#undef __GFE_Variant_SpecializationSwitchIFV
#undef __GFE_Variant_SpecializationSwitchIFVM

        

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

    #undef __GFE_Variant_CreateMacroStorage




    

        
        
template <GA_AttributeOwner _Val>
using attribOwnerConstant = std::integral_constant<GA_AttributeOwner, _Val>;

using attribOwnerConstantInvalid = attribOwnerConstant<GA_ATTRIB_INVALID  >;
using attribOwnerConstantPrim    = attribOwnerConstant<GA_ATTRIB_PRIMITIVE>;
using attribOwnerConstantPoint   = attribOwnerConstant<GA_ATTRIB_POINT    >;
using attribOwnerConstantVertex  = attribOwnerConstant<GA_ATTRIB_VERTEX   >;
using attribOwnerConstantDetail  = attribOwnerConstant<GA_ATTRIB_DETAIL   >;
using attribOwnerConstantN       = attribOwnerConstant<GA_ATTRIB_OWNER_N  >;
    
using attribOwnerVariantPPVD = std::variant<attribOwnerConstantPrim   ,
                                            attribOwnerConstantPoint  ,
                                            attribOwnerConstantVertex ,
                                            attribOwnerConstantDetail >;

        
SYS_FORCE_INLINE attribOwnerVariantPPVD getAttribOwnerVariant(const GA_AttributeOwner v)
{
    switch (v)
    {
    case GA_ATTRIB_PRIMITIVE: return attribOwnerConstant<GA_ATTRIB_PRIMITIVE>{}; break;
    case GA_ATTRIB_POINT:     return attribOwnerConstant<GA_ATTRIB_POINT    >{}; break;
    case GA_ATTRIB_VERTEX:    return attribOwnerConstant<GA_ATTRIB_VERTEX   >{}; break;
    case GA_ATTRIB_DETAIL:    return attribOwnerConstant<GA_ATTRIB_DETAIL   >{}; break;
    default: break;
    }
    UT_ASSERT_MSG(0, "Unhandled Attrib Owner");
    return attribOwnerConstant<GA_ATTRIB_PRIMITIVE>{};
}

SYS_FORCE_INLINE auto getAttribOwnerVariant(const GA_Attribute& attrib)
{ return getAttribOwnerVariant(attrib.getOwner()); }

SYS_FORCE_INLINE auto getAttribOwnerVariant(const GA_Attribute* const attrib)
{ UT_ASSERT_P(attrib); return getAttribOwnerVariant(*attrib); }




    
        
using boolVariant = std::variant<std::true_type, std::false_type>;

boolVariant getBoolVariant(const bool v)
{
    if (v)
        return std::true_type{};
    else
        return std::false_type{};
}







    
    template <GFE_AttribStorage _Ty>
    struct getAttribStorage { using type = void; };

    template <> struct getAttribStorage<GFE_AttribStorage::int8>     { using type = int8; };
    template <> struct getAttribStorage<GFE_AttribStorage::int16>    { using type = int16; };
    template <> struct getAttribStorage<GFE_AttribStorage::int32>    { using type = int32; };
    template <> struct getAttribStorage<GFE_AttribStorage::int64>    { using type = int64; };
    template <> struct getAttribStorage<GFE_AttribStorage::fpreal16> { using type = fpreal16; };
    template <> struct getAttribStorage<GFE_AttribStorage::fpreal32> { using type = fpreal32; };
    template <> struct getAttribStorage<GFE_AttribStorage::fpreal64> { using type = fpreal64; };

#define __GFE_Variant_SpecializationGetAttribStorageV(NUM)\
    template <> struct getAttribStorage<GFE_AttribStorage::UT_Vector##NUM##I> { using type = UT_Vector##NUM##i; };\
    template <> struct getAttribStorage<GFE_AttribStorage::UT_Vector##NUM##L> { using type = UT_Vector##NUM##I; };\
    template <> struct getAttribStorage<GFE_AttribStorage::UT_Vector##NUM##H> { using type = UT_Vector##NUM##H; };\
    template <> struct getAttribStorage<GFE_AttribStorage::UT_Vector##NUM##F> { using type = UT_Vector##NUM##F; };\
    template <> struct getAttribStorage<GFE_AttribStorage::UT_Vector##NUM##D> { using type = UT_Vector##NUM##D; };\
    
        __GFE_Variant_SpecializationGetAttribStorageV(2)
        __GFE_Variant_SpecializationGetAttribStorageV(3)
        __GFE_Variant_SpecializationGetAttribStorageV(4)
        
#undef __GFE_Variant_SpecializationGetAttribStorageV
        
#define __GFE_Variant_SpecializationGetAttribStorageM(NUM)\
    template <> struct getAttribStorage<GFE_AttribStorage::UT_Matrix##NUM##F> { using type = UT_Matrix##NUM##F; };\
    template <> struct getAttribStorage<GFE_AttribStorage::UT_Matrix##NUM##D> { using type = UT_Matrix##NUM##D; };\
    
        __GFE_Variant_SpecializationGetAttribStorageM(2)
        __GFE_Variant_SpecializationGetAttribStorageM(3)
        __GFE_Variant_SpecializationGetAttribStorageM(4)
        
#undef __GFE_Variant_SpecializationGetAttribStorageM

template <GFE_AttribStorage _Ty>
using getAttribStorage_t = typename getAttribStorage<_Ty>::type;



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
