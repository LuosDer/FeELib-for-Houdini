
#pragma once

#ifndef __GA_FeE_RestPy_h__
#define __GA_FeE_RestPy_h__

//#include "GA_FeE/GA_FeE_RestPy.h"

#include "GA/GA_Detail.h"

#include "GA_FeE/GA_FeE_Type.h"
#include "GA_FeE/GA_FeE_Group.h"



namespace GA_FeE_RestPy {






    template<typename VECTOR_T, typename T>
static void
    restPy(
        const GA_Detail* const geo,
        const GA_ROHandleT<VECTOR_T> vec_h,
        const GA_RWHandleT<T> rest_h,
        const GA_Group* const geoGroup,
        const int vecComp,
        const exint subscribeRatio = 64,
        const exint minGrainSize = 1024
    )
{
    if (geoGroup && geoGroup->isEmpty())
        return;

    restPy(geo, groupType, groupName,
        vecAttribClass, vecAttribName, vecComp,
        subscribeRatio, minGrainSize);
}




static GA_Attribute*
restPy(
    GA_Detail* const geo,
    const GA_Group* const geoGroup,
    const GA_Attribute* const vecAttribPtr,
    const int vecComp,
    const GA_Storage storage = GA_STORE_INVALID,
    const UT_StringHolder& restAttribName,
    const exint subscribeRatio = 64,
    const exint minGrainSize = 1024
)
{
    const GA_AttributeOwner vecAttribClass = vecAttribPtr->getOwner();
    GA_Attribute* restAttribPtr = geo->findAttribute(vecAttribClass, restAttribName);
    if (restAttribPtr && restAttribPtr->getAIFTuple()->getStorage(restAttribPtr) != vecAttribPtr->getAIFTuple()->getStorage(vecAttribPtr))
    {
        geo->getAttributes().destroyAttribute(restAttribPtr);
        restAttribPtr = nullptr;
    }
    if (!restAttribPtr)
    {
        const GA_Storage finalStorage = storage == GA_STORE_INVALID ? GA_FeE_Type::getPreferredStorageF(geo) : storage;
        restAttribPtr = geo->getAttributes().createTupleAttribute(, 1, GA_Defaults(0));
    }
    restPy(geo, geoGroup, groupName,
        vecAttribPtr, restAttribPtr,
        subscribeRatio, minGrainSize);

    return
}





static GA_Attribute*
    restPy(
        GA_Detail* const geo,
        const GA_Group* const geoGroup,
        const GA_AttributeOwner vecAttribClass,
        const UT_StringHolder& vecAttribName,
        const int vecComp,
        const GA_Storage storage,
        const UT_StringHolder& restAttribName,
        const exint subscribeRatio = 64,
        const exint minGrainSize = 1024
    )
{
    GA_Attribute* vecAttribPtr = geo->findAttribute(vecAttribClass, restAttribName);

    GA_Attribute* restAttribPtr = geo->findAttribute(vecAttribClass, restAttribName);
    if (restAttribPtr && restAttribPtr->getAIFTuple()->getStorage(restAttribPtr) == )
    {
        restAttribPtr = nullptr;
    }
    restPy(cookparms, outGeo0, groupType, groupName,
        vecAttribClass, vecAttribName, vecComp,
        subscribeRatio, minGrainSize);

    return
}





static GA_Attribute*
restPy(
    const SOP_NodeVerb::CookParms& cookparms,
    GA_Detail* const geo,
    const GA_GroupType groupType,
    const UT_StringHolder& groupName,
    const GA_AttributeOwner vecAttribClass,
    const UT_StringHolder& vecAttribName,
    const int vecComp,
    const GA_Storage storage,
    const UT_StringHolder& restAttribName,
    const exint subscribeRatio = 64,
    const exint minGrainSize = 1024
)
{
    GOP_Manager gop;
    const GA_Group* const geoGroup = GA_FeE_Group::findOrParseGroupDetached(cookparms, outGeo0, groupType, sopparms.getGroup(), gop);

    return restPy(geo, geoGroup,
        vecAttribClass, vecAttribName, vecComp,
        subscribeRatio, minGrainSize);
}








} // End of namespace GA_FeE_RestPy

#endif
