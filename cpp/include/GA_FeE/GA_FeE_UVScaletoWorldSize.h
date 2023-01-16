
#pragma once

#ifndef __GA_FeE_UVScaletoWorldSize_h__
#define __GA_FeE_UVScaletoWorldSize_h__

//#include "GA_FeE/GA_FeE_UVScaletoWorldSize.h"

#include "GA/GA_Detail.h"

#define UVScaletoWorldSize_UseDetachedAttrib 1



#include "GA_FeE/GA_FeE_Type.h"
#include "GA_FeE/GA_FeE_Group.h"
#include "GA_FeE/GA_FeE_Attribute.h"
#include "GA_FeE/GA_FeE_Measure.h"
#include "GA_FeE/GA_FeE_Connectivity.h"


namespace GA_FeE_UVScaletoWorldSize {





template<typename T>
static void
uvScaletoWorldSize(
    GA_Detail* const geo,
    const GA_RWHandleT<UT_Vector3T<T>>& uvAttribPtr_h,
    const GA_PrimitiveGroup* const geoGroup = nullptr,
    const bool computeUVAreaInPiece = true,
    const UT_Vector3R uvScale = UT_Vector3R(1.0),
    const bool doUVScalex = true,
    const bool doUVScaley = true,
    const bool doUVScalez = true,
    const exint subscribeRatio = 64,
    const exint minGrainSize = 64
)
{
    if (geoGroup && geoGroup->isEmpty())
        return;

    const GA_Storage inStorage = uvAttribPtr_h.getAttribute()->getStorage();

    const GA_ATINumericUPtr areaUPtr   = GA_FeE_Measure::addDetachedAttribPrimArea(geo,                                                    geoGroup, inStorage);
    const GA_ATINumericUPtr areaUVUPtr = GA_FeE_Measure::addDetachedAttribPrimArea(geo, static_cast<const GA_ROHandleT<T>>(uvAttribPtr_h), geoGroup, inStorage);
    GA_ATINumeric* const areaATIPtr   = areaUPtr.get();
    GA_ATINumeric* const areaUVATIPtr = areaUVUPtr.get();


    const GA_ATINumericUPtr uvScaleATI_deleter = geo->createDetachedTupleAttribute(GA_ATTRIB_PRIMITIVE, inStorage, 3);
    GA_ATINumeric* const uvScaleATIPtr = uvScaleATI_deleter.get();
    
    GA_RWHandleT<T> areaAttrib_h(areaATIPtr);
    GA_RWHandleT<T> areaUVAttrib_h(areaUVATIPtr);
    GA_RWHandleT<UT_Vector3T<T>> uvScaleAttrib_h(uvScaleATIPtr);
    //GA_ROHandleT<exint> connectivityAttrib_h;
    
    if (computeUVAreaInPiece)
    {
        const GA_Attribute* const connectivityATIPtr = GA_FeE_Connectivity::addAttribConnectivity(geo);

        areaAttrib_h = GU_Promote::promote(*static_cast<GU_Detail*>(geo), areaATIPtr, GA_ATTRIB_PRIMITIVE, false, GU_Promote::GU_PROMOTE_SUM, NULL, connectivityATIPtr);
        areaUVAttrib_h = GU_Promote::promote(*static_cast<GU_Detail*>(geo), areaUVATIPtr, GA_ATTRIB_PRIMITIVE, false, GU_Promote::GU_PROMOTE_SUM, NULL, connectivityATIPtr);
    }

    {
        const GA_SplittableRange geo0SplittableRange(geo->getPrimitiveRange(geoGroup));
        UTparallelFor(geo0SplittableRange, [geo, uvScaleAttrib_h, areaUVAttrib_h, areaAttrib_h, uvScale, doUVScalex, doUVScaley, doUVScalez](const GA_SplittableRange& r)
        {
            GA_Offset start, end;
            for (GA_Iterator it(r); it.blockAdvance(start, end); )
            {
                for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                {
                    const T areaUV = areaUVAttrib_h.get(elemoff);
                    const T area = areaAttrib_h.get(elemoff);
                    UT_Vector3T<T> uvS(uvScale * sqrt(area / areaUV));
                    uvS[0] = doUVScalex ? uvS[0] : 1;
                    uvS[1] = doUVScaley ? uvS[1] : 1;
                    uvS[2] = doUVScalez ? uvS[2] : 1;
                    uvScaleAttrib_h.set(elemoff, uvS);
                }
            }
        }, subscribeRatio, minGrainSize);
    }

    {
        const GA_AttributeOwner uvAttribClassFinal = uvAttribPtr_h.getAttribute()->getOwner();
        const GA_AttributeOwner uvAttribClassFinal_bool = uvAttribClassFinal == GA_ATTRIB_POINT;
        const GA_SplittableRange geo0SplittableRange = GA_FeE_Range::getSplittableRangeByAnyGroup(geo, geoGroup, uvAttribClassFinal);
        UTparallelFor(geo0SplittableRange, [geo, uvAttribPtr_h, uvScaleAttrib_h, uvAttribClassFinal_bool](const GA_SplittableRange& r)
        {
            GA_Offset start, end;
            for (GA_Iterator it(r); it.blockAdvance(start, end); )
            {
                for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                {
                    const GA_Offset primoff = uvAttribClassFinal_bool ? geo->vertexPrimitive(geo->pointVertex(elemoff)) : geo->vertexPrimitive(elemoff);
                    UT_Vector3T<T> uv = uvAttribPtr_h.get(elemoff) * uvScaleAttrib_h.get(primoff);
                    uvAttribPtr_h.set(elemoff, uv);
                }
            }
        }, subscribeRatio, minGrainSize);
    }

#if !UVScaletoWorldSize_UseDetachedAttrib
    outGeo0->destroyAttribute(GA_ATTRIB_PRIMITIVE, "__area_SOP_FeE_UVScaletoWorldSize_3_0");
    outGeo0->destroyAttribute(GA_ATTRIB_PRIMITIVE, "__areaUV_SOP_FeE_UVScaletoWorldSize_3_0");
    outGeo0->destroyAttribute(GA_ATTRIB_POINT, "__adjElems_SOP_FeE_UVScaletoWorldSize_3_0");
#endif
}



SYS_FORCE_INLINE
static void
uvScaletoWorldSize(
    GA_Detail* const geo,
    GA_Attribute* const uvAttribPtr,
    const GA_PrimitiveGroup* const geoGroup = nullptr,
    const bool computeUVAreaInPiece = true,
    const UT_Vector3R uvScale = UT_Vector3R(1.0),
    const bool doUVScalex = true,
    const bool doUVScaley = true,
    const bool doUVScalez = true,
    const exint subscribeRatio = 64,
    const exint minGrainSize = 64
)
{

    //static_cast<GA_ATINumeric*>(uvAttribPtr)->getStorage();
    switch (uvAttribPtr->getAIFTuple()->getStorage(uvAttribPtr))
    {
    case GA_STORE_BOOL:
        uvScaletoWorldSize<bool>(geo, uvAttribPtr, geoGroup, computeUVAreaInPiece,
            uvScale, doUVScalex, doUVScaley, doUVScalez, subscribeRatio, minGrainSize);
        break;
    case GA_STORE_INT8:
        uvScaletoWorldSize<int8>(geo, uvAttribPtr, geoGroup, computeUVAreaInPiece,
            uvScale, doUVScalex, doUVScaley, doUVScalez, subscribeRatio, minGrainSize);
        break;
    case GA_STORE_INT16:
        uvScaletoWorldSize<int16>(geo, uvAttribPtr, geoGroup, computeUVAreaInPiece,
            uvScale, doUVScalex, doUVScaley, doUVScalez, subscribeRatio, minGrainSize);
        break;
    case GA_STORE_INT32:
        uvScaletoWorldSize<int>(geo, uvAttribPtr, geoGroup, computeUVAreaInPiece,
            uvScale, doUVScalex, doUVScaley, doUVScalez, subscribeRatio, minGrainSize);
        break;
    case GA_STORE_INT64:
        uvScaletoWorldSize<int64>(geo, uvAttribPtr, geoGroup, computeUVAreaInPiece,
            uvScale, doUVScalex, doUVScaley, doUVScalez, subscribeRatio, minGrainSize);
        break;
    case GA_STORE_REAL16:
        uvScaletoWorldSize<fpreal16>(geo, uvAttribPtr, geoGroup, computeUVAreaInPiece,
            uvScale, doUVScalex, doUVScaley, doUVScalez, subscribeRatio, minGrainSize);
        break;
    case GA_STORE_REAL32:
        uvScaletoWorldSize<fpreal32>(geo, uvAttribPtr, geoGroup, computeUVAreaInPiece,
            uvScale, doUVScalex, doUVScaley, doUVScalez, subscribeRatio, minGrainSize);
        break;
    case GA_STORE_REAL64:
        uvScaletoWorldSize<fpreal64>(geo, uvAttribPtr, geoGroup, computeUVAreaInPiece,
            uvScale, doUVScalex, doUVScaley, doUVScalez, subscribeRatio, minGrainSize);
        break;
    default:
        break;
    }
}

SYS_FORCE_INLINE
static GA_Attribute*
uvScaletoWorldSize(
    GA_Detail* const geo,
    const GA_AttributeOwner uvAttribClass,
    const UT_StringHolder& uvAttribName,
    const GA_PrimitiveGroup* const geoGroup = nullptr,
    const bool computeUVAreaInPiece = true,
    const UT_Vector3R uvScale = UT_Vector3R(1.0),
    const bool doUVScalex = true,
    const bool doUVScaley = true,
    const bool doUVScalez = true,
    const exint subscribeRatio = 64,
    const exint minGrainSize = 64
)
{
    GA_Attribute* const uvAttribPtr = GA_FeE_Attribute::findAttributePointVertex(geo, uvAttribClass, uvAttribName);
    if (!uvAttribPtr)
        return nullptr;

    uvScaletoWorldSize(geo, uvAttribPtr, geoGroup, computeUVAreaInPiece,
        uvScale, doUVScalex, doUVScaley, doUVScalez,
        subscribeRatio, minGrainSize);

    return uvAttribPtr;
}


SYS_FORCE_INLINE
static GA_Attribute*
uvScaletoWorldSize(
    GA_Detail* const geo,
    const GA_AttributeOwner uvAttribClass,
    const UT_StringHolder& uvAttribName,
    const GA_Group* const geoGroup = nullptr,
    const bool computeUVAreaInPiece = true,
    const UT_Vector3R uvScale = UT_Vector3R(1.0),
    const bool doUVScalex = true,
    const bool doUVScaley = true,
    const bool doUVScalez = true,
    const exint subscribeRatio = 64,
    const exint minGrainSize = 64
)
{
    switch (geoGroup->classType())
    {
    case GA_GROUP_PRIMITIVE:
        return uvScaletoWorldSize(geo, uvAttribClass, uvAttribName, static_cast<const GA_PrimitiveGroup*>(geoGroup), computeUVAreaInPiece,
            uvScale, doUVScalex, doUVScaley, doUVScalez,
            subscribeRatio, minGrainSize);
        break;
    case GA_GROUP_POINT:
    {
        const GA_PrimitiveGroup* const geoPrimGroup = GA_FeE_GroupPromote::groupPromotePrimitiveDetached(geo, geoGroup);
        return uvScaletoWorldSize(geo, uvAttribClass, uvAttribName, geoPrimGroup, computeUVAreaInPiece,
            uvScale, doUVScalex, doUVScaley, doUVScalez,
            subscribeRatio, minGrainSize);
    }
        break;
    case GA_GROUP_VERTEX:
    {
        const GA_PrimitiveGroup* const geoPrimGroup = GA_FeE_GroupPromote::groupPromotePrimitiveDetached(geo, geoGroup);
        return uvScaletoWorldSize(geo, uvAttribClass, uvAttribName, geoPrimGroup, computeUVAreaInPiece,
            uvScale, doUVScalex, doUVScaley, doUVScalez,
            subscribeRatio, minGrainSize);
    }
        break;
    default:
        UT_ASSERT_MSG(0, "unhandled group type");
        break;
    }
    return nullptr;
}



static GA_Attribute*
uvScaletoWorldSize(
    const SOP_NodeVerb::CookParms& cookparms,
    GEO_Detail* const geo,
    const GA_AttributeOwner uvAttribClass,
    const UT_StringHolder& uvAttribName,
    const GA_GroupType groupType,
    const UT_StringHolder& groupName,
    const bool computeUVAreaInPiece = true,
    const UT_Vector3R uvScale = UT_Vector3R(1.0),
    const bool doUVScalex = true,
    const bool doUVScaley = true,
    const bool doUVScalez = true,
    const exint subscribeRatio = 64,
    const exint minGrainSize = 64
)
{
    GOP_Manager gop;
    const GA_Group* const geoGroup = GA_FeE_Group::findOrParseGroupDetached(cookparms, geo, groupType, groupName, gop);

    return uvScaletoWorldSize(geo, uvAttribClass, uvAttribName, geoGroup,
        computeUVAreaInPiece,
        uvScale, doUVScalex, doUVScaley, doUVScalez,
        subscribeRatio, minGrainSize);
}


} // End of namespace GA_FeE_UVScaletoWorldSize

#endif
