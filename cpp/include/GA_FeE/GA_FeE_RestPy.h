
#pragma once

#ifndef __GA_FeE_RestPy_h__
#define __GA_FeE_RestPy_h__

//#include "GA_FeE/GA_FeE_RestPy.h"

#include "GA/GA_Detail.h"

#include "GA_FeE/GA_FeE_Type.h"
#include "GA_FeE/GA_FeE_Group.h"
#include "GA_FeE/GA_FeE_Range.h"



namespace GA_FeE_RestPy {






template<typename VECTOR_T>
static void
    restPy(
        const GA_Detail* const geo,
        const GA_RWHandleT<VECTOR_T>& vec_h,
        const GA_RWHandleT<typename VECTOR_T::value_type>& rest_h,
        const GA_Group* const geoGroup,
        const int vecComp,
        const bool setConstant,
        const typename VECTOR_T::value_type constantValue,
        const exint subscribeRatio = 64,
        const exint minGrainSize = 1024
    )
{
    UT_ASSERT_P(vec_h.getAttribute());
    UT_ASSERT_P(rest_h.getAttribute());

    if (geoGroup && geoGroup->isEmpty())
        return;
    
    const GA_SplittableRange geoSplittableRange(GA_FeE_Range::getSplittableRangeByAnyGroup(geo, geoGroup, vec_h.getAttribute()->getOwner()));
    UTparallelFor(geoSplittableRange, [&vec_h, &rest_h, vecComp, setConstant, constantValue](const GA_SplittableRange& r)
    {
        //VECTOR_T vec;
        GA_PageHandleT<VECTOR_T, typename VECTOR_T::value_type, true, true, GA_Attribute, GA_ATINumeric, GA_Detail> vec_ph(vec_h.getAttribute());
        GA_PageHandleT<typename VECTOR_T::value_type, typename VECTOR_T::value_type, true, true, GA_Attribute, GA_ATINumeric, GA_Detail> rest_ph(rest_h.getAttribute());
        for (GA_PageIterator pit = r.beginPages(); !pit.atEnd(); ++pit)
        {
            GA_Offset start, end;
            for (GA_Iterator it(pit.begin()); it.blockAdvance(start, end); )
            {
                vec_ph.setPage(start);
                rest_ph.setPage(start);
                for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                {
                    rest_ph.value(elemoff) = vec_ph.value(elemoff)[vecComp];
                    if (setConstant)
                    {
                        vec_ph.value(elemoff)[vecComp] = constantValue;
                    }
                }
            }
        }
    }, subscribeRatio, minGrainSize);

}



template<typename T>
static bool
restPy(
    const GA_Detail* const geo,
    GA_Attribute* const vecAttribPtr,
    GA_Attribute* const restAttribPtr,
    const GA_Group* const geoGroup,
    const int vecComp,
    const bool setConstant,
    const T constantValue,
    const exint subscribeRatio = 64,
    const exint minGrainSize = 1024
)
{
    UT_ASSERT_P(vecAttribPtr);
    UT_ASSERT_P(restAttribPtr);

    switch (vecAttribPtr->getTupleSize())
    {
    case 1:
        return false;
        break;
    case 2:
        switch (vecAttribPtr->getAIFTuple()->getStorage(vecAttribPtr))
        {
        case GA_STORE_INT16:
            restPy(geo, GA_RWHandleT<UT_Vector2T<int16>>(vecAttribPtr), restAttribPtr, geoGroup,
                vecComp, setConstant, constantValue,
                subscribeRatio, minGrainSize);
            break;
        case GA_STORE_INT32:
            restPy(geo, GA_RWHandleT<UT_Vector2T<int32>>(vecAttribPtr), restAttribPtr, geoGroup,
                vecComp, setConstant, constantValue,
                subscribeRatio, minGrainSize);
            break;
        case GA_STORE_INT64:
            restPy(geo, GA_RWHandleT<UT_Vector2T<int64>>(vecAttribPtr), restAttribPtr, geoGroup,
                vecComp, setConstant, constantValue,
                subscribeRatio, minGrainSize);
            break;
        case GA_STORE_REAL16:
            restPy(geo, GA_RWHandleT<UT_Vector2T<fpreal16>>(vecAttribPtr), restAttribPtr, geoGroup,
                vecComp, setConstant, constantValue,
                subscribeRatio, minGrainSize);
            break;
        case GA_STORE_REAL32:
            restPy(geo, GA_RWHandleT<UT_Vector2T<fpreal32>>(vecAttribPtr), restAttribPtr, geoGroup,
                vecComp, setConstant, constantValue,
                subscribeRatio, minGrainSize);
            break;
        case GA_STORE_REAL64:
            restPy(geo, GA_RWHandleT<UT_Vector2T<fpreal64>>(vecAttribPtr), restAttribPtr, geoGroup,
                vecComp, setConstant, constantValue,
                subscribeRatio, minGrainSize);
            break;
        default:
            return false;
            break;
        }
        break;
    case 3:
        switch (vecAttribPtr->getAIFTuple()->getStorage(vecAttribPtr))
        {
        case GA_STORE_INT16:
            restPy(geo, GA_RWHandleT<UT_Vector3T<int16>>(vecAttribPtr), restAttribPtr, geoGroup,
                vecComp, setConstant, constantValue,
                subscribeRatio, minGrainSize);
            break;
        case GA_STORE_INT32:
            restPy(geo, GA_RWHandleT<UT_Vector3T<int32>>(vecAttribPtr), restAttribPtr, geoGroup,
                vecComp, setConstant, constantValue,
                subscribeRatio, minGrainSize);
            break;
        case GA_STORE_INT64:
            restPy(geo, GA_RWHandleT<UT_Vector3T<int64>>(vecAttribPtr), restAttribPtr, geoGroup,
                vecComp, setConstant, constantValue,
                subscribeRatio, minGrainSize);
            break;
        case GA_STORE_REAL16:
            restPy(geo, GA_RWHandleT<UT_Vector3T<fpreal16>>(vecAttribPtr), restAttribPtr, geoGroup,
                vecComp, setConstant, constantValue,
                subscribeRatio, minGrainSize);
            break;
        case GA_STORE_REAL32:
            restPy(geo, GA_RWHandleT<UT_Vector3T<fpreal32>>(vecAttribPtr), restAttribPtr, geoGroup,
                vecComp, setConstant, constantValue,
                subscribeRatio, minGrainSize);
            break;
        case GA_STORE_REAL64:
            restPy(geo, GA_RWHandleT<UT_Vector3T<fpreal64>>(vecAttribPtr), restAttribPtr, geoGroup,
                vecComp, setConstant, constantValue,
                subscribeRatio, minGrainSize);
            break;
        default:
            return false;
            break;
        }
        break;
    case 4:
        switch (vecAttribPtr->getAIFTuple()->getStorage(vecAttribPtr))
        {
        case GA_STORE_INT16:
            restPy(geo, GA_RWHandleT<UT_Vector4T<int16>>(vecAttribPtr), restAttribPtr, geoGroup,
                vecComp, setConstant, constantValue,
                subscribeRatio, minGrainSize);
            break;
        case GA_STORE_INT32:
            restPy(geo, GA_RWHandleT<UT_Vector4T<int32>>(vecAttribPtr), restAttribPtr, geoGroup,
                vecComp, setConstant, constantValue,
                subscribeRatio, minGrainSize);
            break;
        case GA_STORE_INT64:
            restPy(geo, GA_RWHandleT<UT_Vector4T<int64>>(vecAttribPtr), restAttribPtr, geoGroup,
                vecComp, setConstant, constantValue,
                subscribeRatio, minGrainSize);
            break;
        case GA_STORE_REAL16:
            restPy(geo, GA_RWHandleT<UT_Vector4T<fpreal16>>(vecAttribPtr), restAttribPtr, geoGroup,
                vecComp, setConstant, constantValue,
                subscribeRatio, minGrainSize);
            break;
        case GA_STORE_REAL32:
            restPy(geo, GA_RWHandleT<UT_Vector4T<fpreal32>>(vecAttribPtr), restAttribPtr, geoGroup,
                vecComp, setConstant, constantValue,
                subscribeRatio, minGrainSize);
            break;
        case GA_STORE_REAL64:
            restPy(geo, GA_RWHandleT<UT_Vector4T<fpreal64>>(vecAttribPtr), restAttribPtr, geoGroup,
                vecComp, setConstant, constantValue,
                subscribeRatio, minGrainSize);
            break;
        default:
            return false;
            break;
        }
        break;
    default:
        return false;
        break;
    }

}

template<typename T>
static GA_Attribute*
restPy(
    GA_Detail* const geo,
    GA_Attribute* const vecAttribPtr,
    const GA_Group* const geoGroup,
    const GA_Storage storage,
    const UT_StringHolder& restAttribName,
    const int vecComp,
    const bool setConstant,
    const T constantValue,
    const exint subscribeRatio = 64,
    const exint minGrainSize = 1024
)
{
    UT_ASSERT_P(vecAttribPtr);

    const GA_AttributeOwner vecAttribClass = vecAttribPtr->getOwner();

    GA_Attribute* restAttribPtr = geo->findAttribute(vecAttribClass, restAttribName);
    if (restAttribPtr && restAttribPtr->getAIFTuple()->getStorage(restAttribPtr) != vecAttribPtr->getAIFTuple()->getStorage(vecAttribPtr))
    {
        geo->getAttributes().destroyAttribute(restAttribPtr);
        restAttribPtr = nullptr;
    }
    if (!restAttribPtr)
    {
        GA_Storage finalStorage;
        if (storage == GA_STORE_INVALID)
        {
            if (vecAttribPtr->getStorageClass() == GA_STORECLASS_INT)
            {
                finalStorage = GA_FeE_Type::getPreferredStorageI(geo);
            }
            else
            {
                finalStorage = GA_FeE_Type::getPreferredStorageF(geo);
            }
        }
        else
        {
            finalStorage = storage;
        }
        restAttribPtr = geo->getAttributes().createTupleAttribute(vecAttribClass, restAttribName, finalStorage, 1, GA_Defaults(0));
    }

    UT_ASSERT_P(restAttribPtr);

    restPy(geo, vecAttribPtr, restAttribPtr, geoGroup,
        vecComp, setConstant, constantValue,
        subscribeRatio, minGrainSize);

    return restAttribPtr;
}




template<typename T>
static GA_Attribute*
    restPy(
        GA_Detail* const geo,
        const GA_Group* const geoGroup,
        const GA_AttributeOwner vecAttribClass,
        const UT_StringHolder& vecAttribName,
        const GA_Storage storage,
        const UT_StringHolder& restAttribName,
        const int vecComp,
        const bool setConstant,
        const T constantValue,
        const exint subscribeRatio = 64,
        const exint minGrainSize = 1024
    )
{
    GA_Attribute* vecAttribPtr = geo->findAttribute(vecAttribClass, restAttribName);

    if (!vecAttribPtr)
    {
        return nullptr;
    }
    return restPy(geo, geoGroup, vecAttribPtr, storage, restAttribName,
        vecComp, setConstant, constantValue,
        subscribeRatio, minGrainSize);
}





template<typename T>
static GA_Attribute*
restPy(
    const SOP_NodeVerb::CookParms& cookparms,
    GA_Detail* const geo,
    const GA_GroupType groupType,
    const UT_StringHolder& groupName,
    const GA_AttributeOwner vecAttribClass,
    const UT_StringHolder& vecAttribName,
    const GA_Storage storage,
    const UT_StringHolder& restAttribName,
    const int vecComp,
    const bool setConstant,
    const T constantValue,
    const exint subscribeRatio = 64,
    const exint minGrainSize = 1024
)
{
    GOP_Manager gop;
    const GA_Group* const geoGroup = GA_FeE_Group::findOrParseGroupDetached(cookparms, geo, groupType, groupName, gop);

    return restPy<T>(geo, geoGroup,
        vecAttribClass, vecAttribName, storage, restAttribName,
        vecComp, setConstant, constantValue,
        subscribeRatio, minGrainSize);
}








} // End of namespace GA_FeE_RestPy

#endif
