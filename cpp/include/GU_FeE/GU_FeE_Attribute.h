
#pragma once

#ifndef __GU_FeE_Attribute_h__
#define __GU_FeE_Attribute_h__


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


#include <GA/GA_Defaults.h>



namespace GU_FeE_Attribute {




//GU_FeE_Attribute::setToDefault(attribHandle)

template<typename T>
static void
setToDefault(
    const GA_RWHandleT<T>& attribHandle
)
{
    const GA_ATINumeric* attrib = attribHandle.getAttribute();
    const GA_AttributeOwner attribOwner = attrib->getOwner();

    if (attribOwner == GA_ATTRIB_INVALID)
        return;

    const GA_Defaults defaults = attrib->getDefaults();
    attribHandle.makeConstant(UTverify_cast<T>(defaults));
}

//GU_FeE_Attribute::setToDefault(geo, attribHandle, geoGroup)
//GU_FeE_Attribute::setToDefault(geo, attribHandle, geoGroup, subscribeRatio, minGrainSize)

//setToDefault<int>

//using T = exint;

#define DEFINEFUNCsetToDefault(T)                                                                                                       \
static void                                                                                                                             \
setToDefault(                                                                                                                           \
    const GU_Detail* geo,                                                                                                               \
    const GA_RWHandleT<T>& attribHandle,                                                                                                \
    const GA_ElementGroup* geoGroup = nullptr,                                                                                          \
    const int& subscribeRatio = 16,                                                                                                     \
    const int& minGrainSize = 1024                                                                                                      \
)                                                                                                                                       \
{                                                                                                                                       \
    if (!geoGroup)                                                                                                                      \
    {                                                                                                                                   \
        setToDefault(attribHandle);                                                                                                     \
        return;                                                                                                                         \
    }                                                                                                                                   \
    const GA_ATINumeric* attrib = attribHandle.getAttribute();                                                                          \
    /*const GA_StorageClass storageClass = attrib->getStorageClass();                                                                   \
    const GA_Storage storage = attrib->getStorage();*/                                                                                  \
    const GA_AttributeOwner attribOwner = attrib->getOwner();                                                                           \
                                                                                                                                        \
    if (attribOwner == GA_ATTRIB_INVALID)                                                                                               \
        return;                                                                                                                         \
                                                                                                                                        \
    GA_Defaults defaults = attrib->getDefaults();                                                                                       \
                                                                                                                                        \
                                                                                                                                        \
    T defaultValue();                                                                                                                   \
    defaults.get(0, defaultValue);                                                                                                      \
                                                                                                                                        \
    if (attribOwner == GA_ATTRIB_GLOBAL)                                                                                                \
    {                                                                                                                                   \
        /*attribHandle.set(0, defaults);*/                                                                                              \
        attribHandle.makeConstant(defaultValue);                                                                                        \
        return;                                                                                                                         \
    }                                                                                                                                   \
                                                                                                                                        \
                                                                                                                                        \
    GA_Range geo0Range;                                                                                                                 \
    switch (attribOwner)                                                                                                                \
    {                                                                                                                                   \
    case GA_ATTRIB_VERTEX:                                                                                                              \
    {                                                                                                                                   \
        geo0Range = geo->getVertexRange(static_cast<const GA_VertexGroup*>(geoGroup));                                                  \
    }                                                                                                                                   \
    break;                                                                                                                              \
    case GA_ATTRIB_POINT:                                                                                                               \
    {                                                                                                                                   \
        geo0Range = geo->getPointRange(static_cast<const GA_PointGroup*>(geoGroup));                                                    \
    }                                                                                                                                   \
    break;                                                                                                                              \
    case GA_ATTRIB_PRIMITIVE:                                                                                                           \
    {                                                                                                                                   \
        geo0Range = geo->getPrimitiveRange(static_cast<const GA_PrimitiveGroup*>(geoGroup));                                            \
    }                                                                                                                                   \
    break;                                                                                                                              \
    default:                                                                                                                            \
        return;                                                                                                                         \
    }                                                                                                                                   \
    const GA_SplittableRange geo0SplittableRange(geo0Range);                                                                            \
                                                                                                                                        \
    /*attribHandle.makeConstant(defaultValue);*/                                                                                          \
    UTparallelFor(geo0SplittableRange, [attribHandle, subscribeRatio, minGrainSize, defaultValue](const GA_SplittableRange& r)          \
    {                                                                                                                                   \
        GA_Offset start;                                                                                                                \
        GA_Offset end;                                                                                                                  \
        for (GA_Iterator it(r); it.blockAdvance(start, end); )                                                                          \
        {                                                                                                                               \
            for (GA_Offset elemoff = start; elemoff < end; ++elemoff)                                                                   \
            {                                                                                                                           \
                attribHandle.set(elemoff, defaultValue);                                                                                \
            }                                                                                                                           \
        }                                                                                                                               \
    }, subscribeRatio, minGrainSize);                                                                                                   \
    attribHandle->bumpDataId();                                                                                                         \
}                                                                                                                                       \


DEFINEFUNCsetToDefault(int32)

//GU_FeE_Attribute::setToDefault(attribHandle, geoGroup)
//GU_FeE_Attribute::setToDefault(attribHandle, geoGroup, subscribeRatio, minGrainSize)

template<typename T>
static void
setToDefault(
    const GA_RWHandleT<T>& attribHandle,
    const GA_ElementGroup* geoGroup = nullptr,
    const int& subscribeRatio = 16,
    const int& minGrainSize = 1024
)
{
    if (!geoGroup)
    {
        setToDefault(attribHandle);
        return;
    }
    const GU_Detail* geo = attribHandle.getAttribute()->getDetail();
    setToDefault(geo, attribHandle, geoGroup, subscribeRatio, minGrainSize);
}




} // End of namespace GU_FeE_Attribute

#endif
