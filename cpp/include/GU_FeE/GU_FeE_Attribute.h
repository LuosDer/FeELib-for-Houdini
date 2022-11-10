
#pragma once

#ifndef __GU_FeE_Attribute_h__
#define __GU_FeE_Attribute_h__

//#include <GU_FeE/GU_FeE_Attribute.h>

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


#if 1
#define DEFFUNCsetToDefaultF(T)                                                        \
static void setToDefault(                                                              \
    const GA_RWHandleT<T>& attribHandle                                                \
)                                                                                      \
{                                                                                      \
    const GA_ATINumeric* attrib = attribHandle.getAttribute();                         \
    const GA_AttributeOwner attribOwner = attrib->getOwner();                          \
                                                                                       \
    if (attribOwner == GA_ATTRIB_INVALID)                                              \
        return;                                                                        \
                                                                                       \
    const GA_Defaults defaults = attrib->getDefaults();                                \
    T defaultValue;                                                                    \
    defaults.get(0, defaultValue);                                                     \
    attribHandle.makeConstant(defaultValue);                                           \
}                                                                                      \


DEFFUNCsetToDefaultF(int32)
DEFFUNCsetToDefaultF(int64)
DEFFUNCsetToDefaultF(fpreal32)
DEFFUNCsetToDefaultF(fpreal64)

#else

//GU_FeE_Attribute::setToDefault(attribHandle)
template<typename T>                                                      
static void setToDefault(                                                 
    const GA_RWHandleT<T>& attribHandle                                   
)                                                                         
{                                                                         
    const GA_ATINumeric* attrib = attribHandle.getAttribute();            
    const GA_AttributeOwner attribOwner = attrib->getOwner();             
                                                                          
    if (attribOwner == GA_ATTRIB_INVALID)                                 
        return;                                                           
                                                                          
    const GA_Defaults defaults = attrib->getDefaults();                   
    T defaultValue;                                                       
    defaults.get(0, defaultValue);                                        
    attribHandle.makeConstant(defaultValue);                              
}                                                                         
#endif

//static void setToDefault(
//    const GA_RWHandleT<UT_Vector3T<fpreal16>>& attribHandle
//)
//{
//    const GA_ATINumeric* attrib = attribHandle.getAttribute();
//    const GA_AttributeOwner attribOwner = attrib->getOwner();
//
//    if (attribOwner == GA_ATTRIB_INVALID)
//        return;
//
//    const GA_Defaults defaults = attrib->getDefaults();
//    fpreal32 defaultValue[3];
//    defaults.get(0, defaultValue[0]);
//    defaults.get(1, defaultValue[1]);
//    defaults.get(2, defaultValue[2]);
//
//    fpreal16 defaultValue16[3];
//    defaultValue16[0] = defaultValue[0];
//    defaultValue16[1] = defaultValue[1];
//    defaultValue16[2] = defaultValue[2];
//    attribHandle.makeConstantV(defaultValue16, 3);
//}


/*
static void setToDefault(
    const GA_RWHandleT<UT_Vector3T<fpreal32>>& attribHandle
)
{
    const GA_ATINumeric* attrib = attribHandle.getAttribute();
    const GA_AttributeOwner attribOwner = attrib->getOwner();

    if (attribOwner == GA_ATTRIB_INVALID)
        return;

    const GA_Defaults defaults = attrib->getDefaults();
    fpreal32 defaultValue[3];
    defaults.get(0, defaultValue[0]);
    defaults.get(1, defaultValue[1]);
    defaults.get(2, defaultValue[2]);
    attribHandle.makeConstantV(defaultValue, 3);
}

#define DEFFUNCsetToDefaultV3(T)                                                       \
static void setToDefault(                                                              \
    const GA_RWHandleT<UT_Vector3T<T>>& attribHandle                                   \
)                                                                                      \
{                                                                                      \
    const GA_ATINumeric* attrib = attribHandle.getAttribute();                         \
    const GA_AttributeOwner attribOwner = attrib->getOwner();                          \
                                                                                       \
    if (attribOwner == GA_ATTRIB_INVALID)                                              \
        return;                                                                        \
                                                                                       \
    const GA_Defaults defaults = attrib->getDefaults();                                \
                                                                                       \
    T defaultValue[3];                                                                 \
    defaults.get(0, defaultValue[0]);                                                  \
    defaults.get(1, defaultValue[1]);                                                  \
    defaults.get(2, defaultValue[2]);                                                  \
    attribHandle.makeConstantV(defaultValue, 3);                                       \
}                                                                                      

//DEFFUNCsetToDefaultV3(fpreal16)
DEFFUNCsetToDefaultV3(fpreal32)
DEFFUNCsetToDefaultV3(fpreal64)
DEFFUNCsetToDefaultV3(int32)
DEFFUNCsetToDefaultV3(int64)

*/

//GU_FeE_Attribute::setToDefault(geo, attribHandle, geoGroup)
//GU_FeE_Attribute::setToDefault(geo, attribHandle, geoGroup, subscribeRatio, minGrainSize)

template<typename T>
static void
setToDefault(
    const GU_Detail* geo,
    const GA_RWHandleT<T>& attribHandle,
    const GA_ElementGroup* geoGroup = nullptr,
    const int& subscribeRatio = 16,
    const int& minGrainSize = 1024
)
{
    const GA_ATINumeric* attrib = attribHandle.getAttribute();

    /*const GA_StorageClass storageClass = attrib->getStorageClass();
    const GA_Storage storage = attrib->getStorage();*/
    const GA_AttributeOwner attribOwner = attrib->getOwner();

    if (attribOwner == GA_ATTRIB_INVALID)
        return;

    GA_Defaults defaults = attrib->getDefaults();

    T defaultValue;
    int tupleSize = attrib->getTupleSize();
    for (size_t i = 0; i < tupleSize; i++)
    {
        defaults.get(i, defaultValue(i));
    }

    if (attribOwner == GA_ATTRIB_GLOBAL)
    {
        //attribHandle.set(0, defaults);
        attribHandle.makeConstant(defaultValue);
        return;
    }


    GA_Range geo0Range;
    switch (attribOwner)
    {
    case GA_ATTRIB_VERTEX:
    {
        geo0Range = geo->getVertexRange(static_cast<const GA_VertexGroup*>(geoGroup));
    }
    break;
    case GA_ATTRIB_POINT:
    {
        geo0Range = geo->getPointRange(static_cast<const GA_PointGroup*>(geoGroup));
    }
    break;
    case GA_ATTRIB_PRIMITIVE:
    {
        geo0Range = geo->getPrimitiveRange(static_cast<const GA_PrimitiveGroup*>(geoGroup));
    }
    break;
    default:
        return;
    }
    const GA_SplittableRange geo0SplittableRange(geo0Range);

    //attribHandle.makeConstant(defaultValue);
    UTparallelFor(geo0SplittableRange, [attribHandle, subscribeRatio, minGrainSize, defaultValue](const GA_SplittableRange& r)
    {
        GA_Offset start;
        GA_Offset end;
        for (GA_Iterator it(r); it.blockAdvance(start, end); )
        {
            for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
            {
                attribHandle.set(elemoff, defaultValue);
            }
        }
    }, subscribeRatio, minGrainSize);
    attribHandle->bumpDataId();
}

#if 1

//GU_FeE_Attribute::setToDefault(geo, attribHandle, geoGroup)
//GU_FeE_Attribute::setToDefault(geo, attribHandle, geoGroup, subscribeRatio, minGrainSize)

#define DEFFUNCsetToDefaultVD(T)                                                                \
template<>                                                                                      \
static void setToDefault<T>(                                                                    \
    const GU_Detail* geo,                                                                       \
    const GA_RWHandleT<T>& attribHandle,                                                        \
    const GA_ElementGroup* geoGroup,                                                            \
    const int& subscribeRatio,                                                                  \
    const int& minGrainSize                                                                     \
)                                                                                               \
{                                                                                               \
    if (!geoGroup)                                                                              \
    {                                                                                           \
        setToDefault(attribHandle);                                                             \
        return;                                                                                 \
    }                                                                                           \
    return setToDefault(geo, attribHandle, geoGroup, subscribeRatio, minGrainSize);             \
}                                                                                               \

DEFFUNCsetToDefaultVD(int32)
DEFFUNCsetToDefaultVD(int64)
DEFFUNCsetToDefaultVD(fpreal32)
DEFFUNCsetToDefaultVD(fpreal64)

#else

template<>
static void setToDefault<int32>(
    const GU_Detail* geo,
    const GA_RWHandleT<int32>& attribHandle,
    const GA_ElementGroup* geoGroup,
    const int& subscribeRatio,
    const int& minGrainSize
    )
{
    if (!geoGroup)
    {
        setToDefault(attribHandle);
        return;
    }
    return setToDefault(geo, attribHandle, geoGroup, subscribeRatio, minGrainSize);
}


#endif


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


//GU_FeE_Attribute::setToDefault(geo, attrib, geoGroup)
//GU_FeE_Attribute::setToDefault(geo, attrib, geoGroup, subscribeRatio, minGrainSize)


static void
setToDefault(
    const GU_Detail* geo,
    GA_Attribute* attrib,
    const GA_ElementGroup* geoGroup = nullptr,
    const int& subscribeRatio = 16,
    const int& minGrainSize = 1024
)
{
    int typeId = attrib->getType().getTypeId();
    int tupleSize = attrib->getTupleSize();

    GA_RWHandleT<UT_Vector3T<fpreal32>> attribHandle(attrib);
    return GU_FeE_Attribute::setToDefault(geo, attribHandle, geoGroup, subscribeRatio, minGrainSize);
}





} // End of namespace GU_FeE_Attribute

#endif
