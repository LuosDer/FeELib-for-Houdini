
#pragma once

#ifndef __GA_FeE_UVScaletoWorldSize_h__
#define __GA_FeE_UVScaletoWorldSize_h__

//#include "GA_FeE/GA_FeE_UVScaletoWorldSize.h"

#include "GA/GA_Detail.h"

#define UVScaletoWorldSize_UseDetachedAttrib 1



#include "GA_FeE/GA_FeE_Type.h"
#include "GA_FeE/GA_FeE_Attribute.h"
#include "GA_FeE/GA_FeE_Measure.h"
#include "GA_FeE/GA_FeE_Connectivity.h"


namespace GA_FeE_UVScaletoWorldSize {


static void
uvScaletoWorldSize(
    GA_Detail* const geo,
    const GA_PrimitiveGroup* const geoGroup,
    const GA_AttributeOwner uvAttribClass,
    const UT_StringHolder& uvAttribName,
    const bool computeUVAreaInPiece = true,
    const GA_Precision preferredPrecision = GA_PRECISION_INVALID,
    const exint subscribeRatio = 64,
    const exint minGrainSize = 64
)
{
    GA_AttributeOwner uvAttribClassFinal;
    GA_Attribute* const uvAttribPtr = GA_FeE_Attribute::findFloatTuplePointVertex(geo, uvAttribClass, uvAttribName, uvAttribClassFinal);
    if (!uvAttribPtr)
        return;

    GA_RWHandleT<TAttribTypeV> uvAttribPtr_h(uvAttribPtr);

    const GA_Precision finalPreferredPrecision = preferredPrecision == GA_PRECISION_INVALID ? geo->getPreferredPrecision() : preferredPrecision;
    const GA_Storage inStorageI = GA_FeE_Type::getPreferredStorageI(finalPreferredPrecision);
    const GA_Storage inStorageF = GA_FeE_Type::getPreferredStorageF(finalPreferredPrecision);


#if UVScaletoWorldSize_UseDetachedAttrib
#if 1
    const GA_ATINumericUPtr areaATI_deleter    = geo->createDetachedTupleAttribute(GA_ATTRIB_PRIMITIVE, inStorageF, 1);
    const GA_ATINumericUPtr areaUVATI_deleter  = geo->createDetachedTupleAttribute(GA_ATTRIB_PRIMITIVE, inStorageF, 1);
    const GA_ATINumericUPtr uvScaleATI_deleter = geo->createDetachedTupleAttribute(GA_ATTRIB_PRIMITIVE, inStorageF, 3);
    GA_ATINumeric* const areaATIPtr    = areaATI_deleter.get();
    GA_ATINumeric* const areaUVATIPtr  = areaUVATI_deleter.get();
    GA_ATINumeric* const uvScaleATIPtr = uvScaleATI_deleter.get();
#else

    GA_Attribute* const areaATIPtr   = GA_FeE_Measure::addAttribPrimArea(geo,              geoGroup, inStorageF, "__area_SOP_FeE_UVScaletoWorldSize_3_0");
    GA_Attribute* const areaUVATIPtr = GA_FeE_Measure::addAttribPrimArea(geo, uvAttribPtr, geoGroup, inStorageF, "__areaUV_SOP_FeE_UVScaletoWorldSize_3_0");

    if (!areaUVATIPtr)
        return;
    GA_ATINumericUPtr uvScaleATI_UPtr = geo->createDetachedTupleAttribute(GA_ATTRIB_PRIMITIVE, inStorageF, 3);
    GA_ATINumeric* uvScaleATIPtr = uvScaleATI_UPtr.get();
#endif
#else
    GA_Attribute* areaATIPtr = geo->addFloatTuple(GA_ATTRIB_PRIMITIVE, "__area_SOP_FeE_UVScaletoWorldSize_3_0", 1, GA_Defaults(0.0), 0, 0, inStorageF);
    GA_Attribute* areaUVATIPtr = geo->addFloatTuple(GA_ATTRIB_PRIMITIVE, "__areaUV_SOP_FeE_UVScaletoWorldSize_3_0", 1, GA_Defaults(0.0), 0, 0, inStorageF);
    GA_Attribute* uvScaleATIPtr = geo->addFloatTuple(GA_ATTRIB_PRIMITIVE, "__uvScale_SOP_FeE_UVScaletoWorldSize_3_0", 3, GA_Defaults(1.0), 0, 0, inStorageF);
#endif

    GA_RWHandleT<attribPrecisonF> areaAttribHandle(areaATIPtr);
    GA_RWHandleT<attribPrecisonF> areaUVAttribHandle(areaUVATIPtr);
    GA_RWHandleT<TAttribTypeV> uvScaleAttribHandle(uvScaleATIPtr);

    if (computeUVAreaInPiece)
    {
#if UVScaletoWorldSize_UseDetachedAttrib
        GA_ATINumericUPtr connectivityATIUPtr = geo->createDetachedTupleAttribute(GA_ATTRIB_POINT, inStorageI, 1, GA_Defaults(-1));
        GA_Attribute* connectivityATIPtr = connectivityATIUPtr.get();
#else
        GA_Attribute* connectivityATIPtr = geo->addIntTuple(GA_ATTRIB_POINT, "__connectivity_SOP_FeE_UVScaletoWorldSize_3_0", 1, GA_Defaults(-1), 0, 0, inStorageI);
#endif


#if UVScaletoWorldSize_UseDetachedAttrib
        GA_AttributeUPtr connectivityAPtr_deleter = GU_Promote::create(GA_ATTRIB_PRIMITIVE, *connectivityATIPtr, GU_Promote::GU_PROMOTE_FIRST);
        connectivityATIPtr = connectivityAPtr_deleter.get();
#else
        connectivityATIPtr = GU_Promote::promote(*static_cast<GU_Detail*>(geo), connectivityATIPtr, GA_ATTRIB_PRIMITIVE, true, GU_Promote::GU_PROMOTE_FIRST, NULL);
#endif
        connectivityAttribHandle = connectivityATIPtr;


        GA_Attribute* const connectivityATIPtr = GA_FeE_Connectivity::addAttribConnectivityPoint(geo);
        GA_AttributeUPtr connectivityATIUPtr =


#if UVScaletoWorldSize_UseDetachedAttrib
        areaAttribHandle   = GU_Promote::promote(*static_cast<GU_Detail*>(geo), areaATIPtr,   GA_ATTRIB_PRIMITIVE, false, GU_Promote::GU_PROMOTE_SUM, NULL, connectivityATIPtr);
        areaUVAttribHandle = GU_Promote::promote(*static_cast<GU_Detail*>(geo), areaUVATIPtr, GA_ATTRIB_PRIMITIVE, false, GU_Promote::GU_PROMOTE_SUM, NULL, connectivityATIPtr);
#else
        areaAttribHandle = GU_Promote::promote(*static_cast<GU_Detail*>(geo), areaATIPtr, GA_ATTRIB_PRIMITIVE, true, GU_Promote::GU_PROMOTE_SUM, NULL, connectivityATIPtr);
        areaUVAttribHandle = GU_Promote::promote(*static_cast<GU_Detail*>(geo), areaUVATIPtr, GA_ATTRIB_PRIMITIVE, true, GU_Promote::GU_PROMOTE_SUM, NULL, connectivityATIPtr);
#endif

    }

    const int& attribSize = uvAttribPtr->getTupleSize();
    //template <typename T>
    switch (attribSize)
    {
    case 3:
        break;
    default:
        UT_ASSERT_MSG(0, "Unhandled geo Attrib Size");
    }



    {
        const GA_SplittableRange geo0SplittableRange(geo->getPrimitiveRange());
        UTparallelFor(geo0SplittableRange, [&geo, &uvScaleAttribHandle, &areaUVAttribHandle, &areaAttribHandle, uvScale, doUVScalex, doUVScaley, doUVScalez](const GA_SplittableRange& r)
        {
            GA_Offset start, end;
            for (GA_Iterator it(r); it.blockAdvance(start, end); )
            {
                for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                {
                    const fpreal areaUV = areaUVAttribHandle.get(elemoff);
                    const fpreal area = areaAttribHandle.get(elemoff);
                    TAttribTypeV uvS = uvScale * sqrt(area / areaUV);
                    uvS[0] = doUVScalex ? uvS[0] : 1;
                    uvS[1] = doUVScaley ? uvS[1] : 1;
                    uvS[2] = doUVScalez ? uvS[2] : 1;
                    uvScaleAttribHandle.set(elemoff, uvS);
                }
            }
        }, subscribeRatio, minGrainSize);
    }

    {
        const GA_SplittableRange geo0SplittableRange = GA_FeE_Range::getSplittableRangeByAnyGroup(geo, geoGroup, uvAttribClassFinal);
        UTparallelFor(geo0SplittableRange, [&geo, &uvAttribPtr_h, &uvScaleAttribHandle, &uvAttribClassFinal](const GA_SplittableRange& r)
        {
            GA_Offset start, end;
            for (GA_Iterator it(r); it.blockAdvance(start, end); )
            {
                for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                {
                    const GA_Offset& primoff = uvAttribClassFinal == GA_ATTRIB_POINT ? geo->vertexPrimitive(geo->pointVertex(elemoff)) : geo->vertexPrimitive(elemoff);
                    TAttribTypeV uv = uvAttribPtr_h.get(elemoff) * uvScaleAttribHandle.get(primoff);
                    uvAttribPtr_h.set(elemoff, uv);
                }
            }
        }, subscribeRatio, minGrainSize);
    }

}



} // End of namespace GA_FeE_UVScaletoWorldSize

#endif
