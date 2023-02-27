
#pragma once

#ifndef __GFE_XformByAttrib_h__
#define __GFE_XformByAttrib_h__

//#include "GFE/GFE_XformByAttrib.h"

#include "GA/GA_Detail.h"



#include "GA/GA_AttributeTransformer.h"



#include "GFE/GFE_Group.h"
#include "GFE/GFE_Range.h"
#include "GFE/GFE_Attribute.h"


namespace GFE_XformByAttrib {

    

template <typename T, typename MATRIX_T>
static void
xformByAttrib(
    const GA_SplittableRange& geoSplittableRange,
    const MATRIX_T& xform,
    GA_Attribute* const attribPtr,
    const bool preserveLength = false,
    const exint subscribeRatio = 64,
    const exint minGrainSize = 4096
)
{
    UT_ASSERT_P(attribPtr);

    UTparallelFor(geoSplittableRange, [attribPtr, &xform](const GA_SplittableRange& r)
    {
        GA_PageHandleT<T, typename T::value_type, true, true, GA_Attribute, GA_ATINumeric, GA_Detail> attrib_ph(attribPtr);
        for (GA_PageIterator pit = r.beginPages(); !pit.atEnd(); ++pit)
        {
            GA_Offset start, end;
            for (GA_Iterator it(pit.begin()); it.blockAdvance(start, end); )
            {
                attrib_ph.setPage(start);
                for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                {
                    attrib_ph.value(elemoff) *= xform;
                }
            }
        }
    }, subscribeRatio, minGrainSize);
}


template <typename FLOAT_T>
SYS_FORCE_INLINE
static bool
xformByAttrib(
    const GA_SplittableRange& geoSplittableRange,
    const UT_Matrix3T<FLOAT_T>& xform,
    GA_Attribute* const attribPtr,
    const bool preserveLength = false,
    const exint subscribeRatio = 64,
    const exint minGrainSize = 4096
)
{
    UT_ASSERT_P(attribPtr);

    if (attribPtr->getStorageClass() != GA_STORECLASS_REAL)
        return false;

    switch (attribPtr->getTupleSize())
    {
    case 1:
        return false;
        break;
    case 2:
        return false;
        //switch (attribPtr->getAIFTuple()->getStorage(attribPtr))
        //{
        //case GA_STORE_REAL16:
        //    return false;
        //    //xformByAttrib<UT_Vector2T<fpreal16>, UT_Matrix3T<FLOAT_T>>(geoSplittableRange, xform, attribPtr, preserveLength, subscribeRatio, minGrainSize);
        //    break;
        //case GA_STORE_REAL32:
        //    xformByAttrib<UT_Vector2T<fpreal32>, UT_Matrix3T<FLOAT_T>>(geoSplittableRange, xform, attribPtr, preserveLength, subscribeRatio, minGrainSize);
        //    break;
        //case GA_STORE_REAL64:
        //    xformByAttrib<UT_Vector2T<fpreal64>, UT_Matrix3T<FLOAT_T>>(geoSplittableRange, xform, attribPtr, preserveLength, subscribeRatio, minGrainSize);
        //    break;
        //default:
        //    return false;
        //    break;
        //}
        break;
    case 3:
        switch (attribPtr->getAIFTuple()->getStorage(attribPtr))
        {
        case GA_STORE_REAL16:
            return false;
            //xformByAttrib<UT_Vector3T<fpreal16>, UT_Matrix3T<FLOAT_T>>(geoSplittableRange, xform, attribPtr, preserveLength, subscribeRatio, minGrainSize);
            break;
        case GA_STORE_REAL32:
            xformByAttrib<UT_Vector3T<fpreal32>, UT_Matrix3T<FLOAT_T>>(geoSplittableRange, xform, attribPtr, preserveLength, subscribeRatio, minGrainSize);
            break;
        case GA_STORE_REAL64:
            xformByAttrib<UT_Vector3T<fpreal64>, UT_Matrix3T<FLOAT_T>>(geoSplittableRange, xform, attribPtr, preserveLength, subscribeRatio, minGrainSize);
            break;
        default:
            return false;
            break;
        }
        break;
    case 4:
        return false;
        //if (attribPtr->getTypeInfo() == GA_TYPE_TRANSFORM)
        //{
        //    switch (attribPtr->getAIFTuple()->getStorage(attribPtr))
        //    {
        //    case GA_STORE_REAL16:
        //        return false;
        //        //xformByAttrib<UT_Matrix2T<fpreal16>, UT_Matrix4T<FLOAT_T>>(geoSplittableRange, xform, attribPtr, preserveLength, subscribeRatio, minGrainSize);
        //        break;
        //    case GA_STORE_REAL32:
        //        xformByAttrib<UT_Matrix2T<fpreal32>, UT_Matrix4T<FLOAT_T>>(geoSplittableRange, xform, attribPtr, preserveLength, subscribeRatio, minGrainSize);
        //        break;
        //    case GA_STORE_REAL64:
        //        xformByAttrib<UT_Matrix2T<fpreal64>, UT_Matrix4T<FLOAT_T>>(geoSplittableRange, xform, attribPtr, preserveLength, subscribeRatio, minGrainSize);
        //        break;
        //    default:
        //        return false;
        //        break;
        //    }
        //}
        //else
        //{
        //    switch (attribPtr->getAIFTuple()->getStorage(attribPtr))
        //    {
        //    case GA_STORE_REAL16:
        //        return false;
        //        //xformByAttrib<UT_Vector4T<fpreal16>, UT_Matrix4T<FLOAT_T>>(geoSplittableRange, xform, attribPtr, preserveLength, subscribeRatio, minGrainSize);
        //        break;
        //    case GA_STORE_REAL32:
        //        xformByAttrib<UT_Vector4T<fpreal32>, UT_Matrix4T<FLOAT_T>>(geoSplittableRange, xform, attribPtr, preserveLength, subscribeRatio, minGrainSize);
        //        break;
        //    case GA_STORE_REAL64:
        //        xformByAttrib<UT_Vector4T<fpreal64>, UT_Matrix4T<FLOAT_T>>(geoSplittableRange, xform, attribPtr, preserveLength, subscribeRatio, minGrainSize);
        //        break;
        //    default:
        //        return false;
        //        break;
        //    }
        //}
        break;
    case 9:
        switch (attribPtr->getAIFTuple()->getStorage(attribPtr))
        {
        case GA_STORE_REAL16:
            return false;
            //xformByAttrib<UT_Matrix3T<fpreal16>, UT_Matrix3T<FLOAT_T>>(geoSplittableRange, xform, attribPtr, preserveLength, subscribeRatio, minGrainSize);
            break;
        case GA_STORE_REAL32:
            xformByAttrib<UT_Matrix3T<fpreal32>, UT_Matrix3T<FLOAT_T>>(geoSplittableRange, xform, attribPtr, preserveLength, subscribeRatio, minGrainSize);
            break;
        case GA_STORE_REAL64:
            xformByAttrib<UT_Matrix3T<fpreal64>, UT_Matrix3T<FLOAT_T>>(geoSplittableRange, xform, attribPtr, preserveLength, subscribeRatio, minGrainSize);
            break;
        default:
            return false;
            break;
        }
        break;
    case 16:
        switch (attribPtr->getAIFTuple()->getStorage(attribPtr))
        {
        case GA_STORE_REAL16:
            return false;
            //xformByAttrib<UT_Matrix4T<fpreal16>, UT_Matrix3T<FLOAT_T>>(geoSplittableRange, xform, attribPtr, preserveLength, subscribeRatio, minGrainSize);
            break;
        case GA_STORE_REAL32:
            xformByAttrib<UT_Matrix4T<fpreal32>, UT_Matrix3T<FLOAT_T>>(geoSplittableRange, xform, attribPtr, preserveLength, subscribeRatio, minGrainSize);
            break;
        case GA_STORE_REAL64:
            xformByAttrib<UT_Matrix4T<fpreal64>, UT_Matrix3T<FLOAT_T>>(geoSplittableRange, xform, attribPtr, preserveLength, subscribeRatio, minGrainSize);
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
    return true;
}


template <typename FLOAT_T>
SYS_FORCE_INLINE
static bool
xformByAttrib(
    const GA_SplittableRange& geoSplittableRange,
    const UT_Matrix4T<FLOAT_T>& xform,
    GA_Attribute* const attribPtr,
    const bool preserveLength = false,
    const exint subscribeRatio = 64,
    const exint minGrainSize = 4096
)
{
    UT_ASSERT_P(attribPtr);

    if (attribPtr->getStorageClass() != GA_STORECLASS_REAL)
        return false;

    switch (attribPtr->getTypeInfo())
    {
    case GA_TYPE_POINT:
        break;
    case GA_TYPE_HPOINT:
        break;
    case GA_TYPE_TRANSFORM:
        break;
    case GA_TYPE_VECTOR:
        xformByAttrib(geoSplittableRange, UT_Matrix3T<FLOAT_T>(xform), attribPtr, preserveLength, subscribeRatio, minGrainSize);
        return true;
        break;
    case GA_TYPE_NORMAL:
        xformByAttrib(geoSplittableRange, UT_Matrix3T<FLOAT_T>(xform), attribPtr, preserveLength, subscribeRatio, minGrainSize);
        return true;
        break;
    case GA_TYPE_QUATERNION:
        xformByAttrib(geoSplittableRange, UT_Matrix3T<FLOAT_T>(xform), attribPtr, preserveLength, subscribeRatio, minGrainSize);
        return true;
        break;
    default:
        return false;
        break;
    }
    switch (attribPtr->getTupleSize())
    {
    case 1:
        return false;
        break;
    case 2:
        return false;
        //switch (attribPtr->getAIFTuple()->getStorage(attribPtr))
        //{
        //case GA_STORE_REAL16:
        //    return false;
        //    //xformByAttrib<UT_Vector2T<fpreal16>, UT_Matrix4T<FLOAT_T>>(geoSplittableRange, xform, attribPtr, preserveLength, subscribeRatio, minGrainSize);
        //    break;
        //case GA_STORE_REAL32:
        //    xformByAttrib<UT_Vector2T<fpreal32>, UT_Matrix4T<FLOAT_T>>(geoSplittableRange, xform, attribPtr, preserveLength, subscribeRatio, minGrainSize);
        //    break;
        //case GA_STORE_REAL64:
        //    xformByAttrib<UT_Vector2T<fpreal64>, UT_Matrix4T<FLOAT_T>>(geoSplittableRange, xform, attribPtr, preserveLength, subscribeRatio, minGrainSize);
        //    break;
        //default:
        //    return false;
        //    break;
        //}
        break;
    case 3:
        switch (attribPtr->getAIFTuple()->getStorage(attribPtr))
        {
        case GA_STORE_REAL16:
            return false;
            //xformByAttrib<UT_Vector3T<fpreal16>, UT_Matrix4T<FLOAT_T>>(geoSplittableRange, xform, attribPtr, preserveLength, subscribeRatio, minGrainSize);
            break;
        case GA_STORE_REAL32:
            xformByAttrib<UT_Vector3T<fpreal32>, UT_Matrix4T<FLOAT_T>>(geoSplittableRange, xform, attribPtr, preserveLength, subscribeRatio, minGrainSize);
            break;
        case GA_STORE_REAL64:
            xformByAttrib<UT_Vector3T<fpreal64>, UT_Matrix4T<FLOAT_T>>(geoSplittableRange, xform, attribPtr, preserveLength, subscribeRatio, minGrainSize);
            break;
        default:
            return false;
            break;
        }
        break;
    case 4:
        if (attribPtr->getTypeInfo() == GA_TYPE_TRANSFORM)
        {
            return false;
            //switch (attribPtr->getAIFTuple()->getStorage(attribPtr))
            //{
            //case GA_STORE_REAL16:
            //    return false;
            //    //xformByAttrib<UT_Matrix2T<fpreal16>, UT_Matrix4T<FLOAT_T>>(geoSplittableRange, xform, attribPtr, preserveLength, subscribeRatio, minGrainSize);
            //    break;
            //case GA_STORE_REAL32:
            //    xformByAttrib<UT_Matrix2T<fpreal32>, UT_Matrix4T<FLOAT_T>>(geoSplittableRange, xform, attribPtr, preserveLength, subscribeRatio, minGrainSize);
            //    break;
            //case GA_STORE_REAL64:
            //    xformByAttrib<UT_Matrix2T<fpreal64>, UT_Matrix4T<FLOAT_T>>(geoSplittableRange, xform, attribPtr, preserveLength, subscribeRatio, minGrainSize);
            //    break;
            //default:
            //    return false;
            //    break;
            //}
        }
        else
        {
            switch (attribPtr->getAIFTuple()->getStorage(attribPtr))
            {
            case GA_STORE_REAL16:
                return false;
                //xformByAttrib<UT_Vector4T<fpreal16>, UT_Matrix4T<FLOAT_T>>(geoSplittableRange, xform, attribPtr, preserveLength, subscribeRatio, minGrainSize);
                break;
            case GA_STORE_REAL32:
                xformByAttrib<UT_Vector4T<fpreal32>, UT_Matrix4T<FLOAT_T>>(geoSplittableRange, xform, attribPtr, preserveLength, subscribeRatio, minGrainSize);
                break;
            case GA_STORE_REAL64:
                xformByAttrib<UT_Vector4T<fpreal64>, UT_Matrix4T<FLOAT_T>>(geoSplittableRange, xform, attribPtr, preserveLength, subscribeRatio, minGrainSize);
                break;
            default:
                return false;
                break;
            }
        }
        break;
    case 9:
        return false;
        //switch (attribPtr->getAIFTuple()->getStorage(attribPtr))
        //{
        //case GA_STORE_REAL16:
        //    return false;
        //    //xformByAttrib<UT_Matrix3T<fpreal16>, UT_Matrix4T<FLOAT_T>>(geoSplittableRange, xform, attribPtr, preserveLength, subscribeRatio, minGrainSize);
        //    break;
        //case GA_STORE_REAL32:
        //    xformByAttrib<UT_Matrix3T<fpreal32>, UT_Matrix4T<FLOAT_T>>(geoSplittableRange, xform, attribPtr, preserveLength, subscribeRatio, minGrainSize);
        //    break;
        //case GA_STORE_REAL64:
        //    xformByAttrib<UT_Matrix3T<fpreal64>, UT_Matrix4T<FLOAT_T>>(geoSplittableRange, xform, attribPtr, preserveLength, subscribeRatio, minGrainSize);
        //    break;
        //default:
        //    return false;
        //    break;
        //}
        break;
    case 16:
        switch (attribPtr->getAIFTuple()->getStorage(attribPtr))
        {
        case GA_STORE_REAL16:
            return false;
            //xformByAttrib<UT_Matrix4T<fpreal16>, UT_Matrix4T<FLOAT_T>>(geoSplittableRange, xform, attribPtr, preserveLength, subscribeRatio, minGrainSize);
            break;
        case GA_STORE_REAL32:
            xformByAttrib<UT_Matrix4T<fpreal32>, UT_Matrix4T<FLOAT_T>>(geoSplittableRange, xform, attribPtr, preserveLength, subscribeRatio, minGrainSize);
            break;
        case GA_STORE_REAL64:
            xformByAttrib<UT_Matrix4T<fpreal64>, UT_Matrix4T<FLOAT_T>>(geoSplittableRange, xform, attribPtr, preserveLength, subscribeRatio, minGrainSize);
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
    return true;
}




template <typename MATRIX_T>
SYS_FORCE_INLINE
static void
xformByAttrib(
    GA_Detail* const geo,
    const GA_Group* const geoGroup,
    const MATRIX_T& xform,
    GA_Attribute* const attribPtr,
    const bool preserveLength = false,
    const exint subscribeRatio = 64,
    const exint minGrainSize = 4096
)
{
    UT_ASSERT_P(attribPtr);
    const GA_SplittableRange geoSplittableRange(GFE_Range::getSplittableRangeByAnyGroup(geo, geoGroup, attribPtr->getOwner()));
    xformByAttrib(geoSplittableRange, xform, attribPtr, preserveLength, subscribeRatio, minGrainSize);
}

template <typename MATRIX_T>
SYS_FORCE_INLINE
static void
xformByAttrib(
    GA_Detail* const geo,
    const GA_Group* const geoGroup,
    const MATRIX_T& xform,
    const GA_AttributeOwner owner,
    const UT_StringHolder& attribToXformName,
    const bool preserveLength = false,
    const exint subscribeRatio = 64,
    const exint minGrainSize = 4096
)
{
    const GA_SplittableRange geoSplittableRange(GFE_Range::getSplittableRangeByAnyGroup(geo, geoGroup, owner));

    //GA_AttributeFilter filter = GA_AttributeFilter::selectByPattern(attribToXformName.c_str());
    GA_AttributeFilter filter = GA_AttributeFilter::selectByPattern(attribToXformName);

#if 1
    UT_Array<GA_Attribute*> attribList;
    geo->getAttributes().matchAttributes(filter, owner, attribList);
    for (exint i = 0; i < attribList.size(); ++i)
    {
        //const UT_StringHolder& name = attribList[i]->getName();
        xformByAttrib(geoSplittableRange, xform, attribList[i], preserveLength, subscribeRatio, minGrainSize);
    }
#else
    //const GA_Range geoRange(geo->getIndexMap(owner), geoGroup);
    const GA_Range geoRange = GFE_Range::getRangeByAnyGroup(geo, geoGroup, owner);

    GA_AttributeTransformer::Transform<FLOAT_T> transform(xform);
    GA_AttributeTransformer attribXformer(*geo, owner);

    attribXformer.addAttributes(filter, preserveLength);

    attribXformer.transform(geoRange, transform);
    //const GA_SplittableRange geoSplittableRange(geo->getIndexMap(owner), geoGroup);
#endif
}




//template <typename FLOAT_T>
//SYS_FORCE_INLINE
//static void
//xformByAttrib(
//    GA_Detail* const geo,
//    const GA_Group* const geoGroup,
//    const UT_Matrix2T<FLOAT_T>& xform,
//    const GA_AttributeOwner owner,
//    const UT_StringHolder& attribToXformName,
//    const bool preserveLength = false,
//    const exint subscribeRatio = 64,
//    const exint minGrainSize = 4096
//)
//{
//    UT_Matrix4T<FLOAT_T> xform4(xform);
//    xformByAttrib<FLOAT_T>(geo, geoGroup, xform4, owner, attribToXformName, preserveLength, subscribeRatio, minGrainSize);
//}





template <typename MATRIX_T>
static void
xformByAttrib(
    GA_Detail* const geo,
    const GA_Group* const geoGroup,
    const GA_Attribute* const xformAttrib,
    const GA_AttributeOwner owner,
    const UT_StringHolder& attribToXformName,
    const bool invertXform = false,
    const bool preserveLength = false,
    const exint subscribeRatio = 64,
    const exint minGrainSize = 4096
)
{
    UT_ASSERT_P(geo);
    UT_ASSERT_P(xformAttrib);

    GA_ROHandleT<MATRIX_T> xform_h(xformAttrib);
    MATRIX_T xform = xform_h.get(0);
    if (invertXform)
    {
        xform.invert();
        //xform4.invert();
    }
    //UT_Matrix4T<fpreal32> xform4 = xform_h.get(0);
    xformByAttrib(geo, geoGroup, xform, owner, attribToXformName, preserveLength, subscribeRatio, minGrainSize);
}


SYS_FORCE_INLINE
static void
xformByAttrib(
    GA_Detail* const geo,
    const GA_Group* const geoGroup,
    const GA_Attribute* const xformAttrib,
    const GA_AttributeOwner owner,
    const UT_StringHolder& attribToXformName,
    const bool invertXform = false,
    const bool preserveLength = false,
    const exint subscribeRatio = 64,
    const exint minGrainSize = 4096
)
{
    UT_ASSERT_P(geo);
    UT_ASSERT_P(xformAttrib);

    switch (xformAttrib->getStorageClass())
    {
    case GA_STORECLASS_REAL:
        switch (xformAttrib->getTupleSize())
        {
        case 1:
            return;
            break;
        case 2:
            return;
            break;
        case 3:
            return;
            break;
        case 4:
            return;
            break;
        case 9:
            switch (xformAttrib->getAIFTuple()->getStorage(xformAttrib))
            {
            case GA_STORE_REAL16:
                return;
                //xformByAttrib<UT_Matrix3T<fpreal16>>(geo, geoGroup, xformAttrib, owner, attribToXformName, invertXform, preserveLength, subscribeRatio, minGrainSize);
                break;
            case GA_STORE_REAL32:
                xformByAttrib<UT_Matrix3T<fpreal32>>(geo, geoGroup, xformAttrib, owner, attribToXformName, invertXform, preserveLength, subscribeRatio, minGrainSize);
                break;
            case GA_STORE_REAL64:
                xformByAttrib<UT_Matrix3T<fpreal64>>(geo, geoGroup, xformAttrib, owner, attribToXformName, invertXform, preserveLength, subscribeRatio, minGrainSize);
                break;
            default:
                return;
                break;
            }
            break;
        case 16:
            switch (xformAttrib->getAIFTuple()->getStorage(xformAttrib))
            {
            case GA_STORE_REAL16:
                return;
                //xformByAttrib<UT_Matrix4T<fpreal16>>(geo, geoGroup, xformAttrib, owner, attribToXformName, invertXform, preserveLength, subscribeRatio, minGrainSize);
                break;
            case GA_STORE_REAL32:
                xformByAttrib<UT_Matrix4T<fpreal32>>(geo, geoGroup, xformAttrib, owner, attribToXformName, invertXform, preserveLength, subscribeRatio, minGrainSize);
                break;
            case GA_STORE_REAL64:
                xformByAttrib<UT_Matrix4T<fpreal64>>(geo, geoGroup, xformAttrib, owner, attribToXformName, invertXform, preserveLength, subscribeRatio, minGrainSize);
                break;
            default:
                return;
                break;
            }
            break;
        default:
            return;
            break;
        }
        break;
    default:
        return;
        break;
    }
}




SYS_FORCE_INLINE
static void
xformByAttrib(
    GA_Detail* const geo,
    const GA_Detail* const refGeo,
    const GA_Group* const geoGroup,
    const UT_StringHolder& xformAttribName,
    const GA_AttributeOwner owner,
    const UT_StringHolder& attribToXformName,
    const bool invertXform = false,
    const bool preserveLength = false,
    const bool delXformAttrib = true,
    const exint subscribeRatio = 64,
    const exint minGrainSize = 4096
)
{
    if (geoGroup && geoGroup->isEmpty())
        return;
    const GA_Attribute* xformAttrib = nullptr;
    if (refGeo)
    {
        xformAttrib = refGeo->findGlobalAttribute(xformAttribName);
    }
    else
    {
        xformAttrib = geo->findGlobalAttribute(xformAttribName);
    }
    if (!xformAttrib)
        return;

    xformByAttrib(geo, geoGroup, xformAttrib, owner, attribToXformName, invertXform, preserveLength, subscribeRatio, minGrainSize);

    if (delXformAttrib && !refGeo)
    {
        geo->getAttributes().destroyAttribute(geo->findGlobalAttribute(xformAttribName));
    }
}




SYS_FORCE_INLINE
static void
xformByAttrib(
    const SOP_NodeVerb::CookParms& cookparms,
    GA_Detail* const geo,
    const GA_Detail* const refGeo,
    const GA_GroupType groupType,
    const UT_StringHolder& geoGroupName,
    const UT_StringHolder& xformAttribName,
    const GA_AttributeOwner owner,
    const UT_StringHolder& attribToXformName,
    const bool invertXform = false,
    const bool preserveLength = false,
    const bool delXformAttrib = true,
    const exint subscribeRatio = 64,
    const exint minGrainSize = 4096
)
{
    GOP_Manager gop;
    const GA_Group* const geoGroup = GFE_Group::findOrParseGroupDetached(cookparms, geo, groupType, geoGroupName, gop);
    xformByAttrib(geo, refGeo, geoGroup, xformAttribName, owner, attribToXformName, invertXform, preserveLength, delXformAttrib, subscribeRatio, minGrainSize);

    GFE_Attribute::bumpDataId(geo, owner, attribToXformName);
}




} // End of namespace GFE_XformByAttrib

#endif
