
#pragma once

#ifndef __GFE_UVScaletoWorldSize_h__
#define __GFE_UVScaletoWorldSize_h__

//#include "GFE/GFE_UVScaletoWorldSize.h"



#include "GU/GU_Promote.h"

#include "GFE/GFE_GeoFilter.h"

#include "GFE/GFE_Attribute.h"
#include "GFE/GFE_Connectivity.h"
//#include "GFE/GFE_GroupParser.h"
#include "GFE/GFE_GroupPromote.h"
#include "GFE/GFE_Measure.h"
#include "GFE/GFE_Range.h"



class GFE_UVScaletoWorldSize : public GFE_AttribFilter {

#define GFE_UVScaletoWorldSize_AreaAttribName   "__area_GFE_UVScaletoWorldSize"
#define GFE_UVScaletoWorldSize_AreaUVAttribName "__areaUV_GFE_UVScaletoWorldSize"

public:

    using GFE_AttribFilter::GFE_AttribFilter;
    

    void
        setComputeParm(
            const bool computeUVAreaInPiece = true,
            const UT_Vector3D& uvScale = UT_Vector3D(1.0),
            const bool doUVScalex = true,
            const bool doUVScaley = true,
            const bool doUVScalez = true,
            const bool outTopoAttrib = false,
            const exint subscribeRatio = 64,
            const exint minGrainSize = 64
        )
    {
        setHasComputed();
        setOutTopoAttrib(outTopoAttrib);
        this->computeUVAreaInPiece = computeUVAreaInPiece;
        this->uvScale = uvScale;
        this->doUVScalex = doUVScalex;
        this->doUVScaley = doUVScaley;
        this->doUVScalez = doUVScalez;
        this->subscribeRatio = subscribeRatio;
        this->minGrainSize = minGrainSize;
    }





private:


    virtual bool
        computeCore() override
    {
        if (!doUVScalex && !doUVScaley && !doUVScalez)
            return false;

        if (groupParser.isEmpty())
            return true;

        const size_t len = getOutAttribArrayRef().size();
        for (size_t i = 0; i < len; ++i) {
            GA_Attribute* const attribPtr = getOutAttribArrayRef()[i];
            //static_cast<GA_ATINumeric*>(uvAttribPtr)->getStorage();
            switch (attribPtr->getAIFTuple()->getStorage(attribPtr))
            {
            case GA_STORE_INT32:
                uvScaletoWorldSize<int>(attribPtr);
                break;
            case GA_STORE_INT64:
                uvScaletoWorldSize<int64>(attribPtr);
                break;
            case GA_STORE_REAL16:
                uvScaletoWorldSize<fpreal16>(attribPtr);
                break;
            case GA_STORE_REAL32:
                uvScaletoWorldSize<fpreal32>(attribPtr);
                break;
            case GA_STORE_REAL64:
                uvScaletoWorldSize<fpreal64>(attribPtr);
                break;
            default:
                break;
            }
        }
        return true;
    }



    template<typename T>
    void
        uvScaletoWorldSize(
            const GA_RWHandleT<UT_Vector3T<T>>& uv_h
        )
    {
        const GA_PrimitiveGroup* const geoGroup = groupParser.getPrimitiveGroup();

        const GA_Storage inStorage = uv_h->getStorage();

        GFE_Measure measure(geo, cookparms);
        measure.groupParser.setGroup(geoGroup);
        //measure.setPositionAttrib();
        //measure.setComputeParm(GFE_MeasureType::Area);

        //GA_Attribute* areaATIPtr = measure.getOutAttribArray().set(GA_ATTRIB_PRIMITIVE, GFE_UVScaletoWorldSize_AreaAttribName);
        GA_Attribute* areaATIPtr = measure.getOutAttribArray().findOrCreateTuple(true, GA_ATTRIB_PRIMITIVE);
        measure.compute();

        //GA_Attribute* areaUVATIPtr = measure.getOutAttribArray().set(GA_ATTRIB_PRIMITIVE, GFE_UVScaletoWorldSize_AreaUVAttribName);
        GA_Attribute* areaUVATIPtr = measure.getOutAttribArray().findOrCreateTuple(true, GA_ATTRIB_PRIMITIVE);
        measure.setPositionAttrib(uv_h.getAttribute());
        measure.compute();
        

        const GA_AttributeUPtr uvScaleATIUPtr = geo->createDetachedTupleAttribute(GA_ATTRIB_PRIMITIVE, inStorage, 3);
        GA_Attribute* const uvScaleATIPtr = uvScaleATIUPtr.get();

        //GA_ROHandleT<exint> connectivityAttrib_h;

        if (computeUVAreaInPiece)
        {
            const GA_Attribute* const connectivityATIPtr = GFE_Connectivity_Namespace::addAttribConnectivity(geo);
            //const GA_Attribute* const connectivityATIPtr = GFE_Connectivity::addAttribConnectivity(geo);

            //areaAttrib_h   = GU_Promote::promote(*static_cast<GU_Detail*>(geo), areaATIPtr,   GA_ATTRIB_PRIMITIVE, true, GU_Promote::GU_PROMOTE_SUM, NULL, connectivityATIPtr);
            //areaUVAttrib_h = GU_Promote::promote(*static_cast<GU_Detail*>(geo), areaUVATIPtr, GA_ATTRIB_PRIMITIVE, true, GU_Promote::GU_PROMOTE_SUM, NULL, connectivityATIPtr);
            areaATIPtr = GU_Promote::promote(*static_cast<GU_Detail*>(geo), areaATIPtr, GA_ATTRIB_PRIMITIVE, true, GU_Promote::GU_PROMOTE_SUM, NULL, connectivityATIPtr);
            areaUVATIPtr = GU_Promote::promote(*static_cast<GU_Detail*>(geo), areaUVATIPtr, GA_ATTRIB_PRIMITIVE, true, GU_Promote::GU_PROMOTE_SUM, NULL, connectivityATIPtr);
        }


        GA_ROHandleT<T> areaAttrib_h(areaATIPtr);
        GA_ROHandleT<T> areaUVAttrib_h(areaUVATIPtr);
        GA_RWHandleT<UT_Vector3T<T>> uvScaleAttrib_h(uvScaleATIPtr);

        GA_SplittableRange geoSplittableRange(geo->getPrimitiveRange(geoGroup));
        UTparallelFor(geoSplittableRange, [this, &uvScaleAttrib_h, &areaUVAttrib_h, &areaAttrib_h](const GA_SplittableRange& r)
        {
            GA_Offset start, end;
            UT_Vector3T<T> uvS;
            for (GA_Iterator it(r); it.blockAdvance(start, end); )
            {
                for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                {
                    const T area = areaAttrib_h.get(elemoff);
                    const T areaUV = areaUVAttrib_h.get(elemoff);
                    uvS = uvScale * sqrt(area / areaUV);
                    uvS[0] = doUVScalex ? uvS[0] : T(1);
                    uvS[1] = doUVScaley ? uvS[1] : T(1);
                    uvS[2] = doUVScalez ? uvS[2] : T(1);
                    uvScaleAttrib_h.set(elemoff, uvS);
                }
            }
        }, subscribeRatio, minGrainSize);

        const GA_AttributeOwner uvAttribClassFinal = uv_h.getAttribute()->getOwner();
        const bool uvAttribClassFinal_bool = uvAttribClassFinal == GA_ATTRIB_POINT;
        //geoSplittableRange = GFE_Range::getSplittableRangeByAnyGroup(geo, geoGroup, uvAttribClassFinal);
        geoSplittableRange = groupParser.getRange(uvAttribClassFinal);
        UTparallelFor(geoSplittableRange, [this, &uv_h, &uvScaleAttrib_h, uvAttribClassFinal_bool](const GA_SplittableRange& r)
        {
            GA_Offset start, end;
            for (GA_Iterator it(r); it.blockAdvance(start, end); )
            {
                for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                {
                    const GA_Offset primoff = uvAttribClassFinal_bool ? geo->vertexPrimitive(geo->pointVertex(elemoff)) : geo->vertexPrimitive(elemoff);
                    UT_Vector3T<T> uv = uv_h.get(elemoff) * uvScaleAttrib_h.get(primoff);
                    uv_h.set(elemoff, uv);
                }
            }
        }, subscribeRatio, minGrainSize);

    }



public:
    bool computeUVAreaInPiece = true;
    UT_Vector3D uvScale = UT_Vector3D(1.0);
    bool doUVScalex = true;
    bool doUVScaley = true;
    bool doUVScalez = true;

private:
    exint subscribeRatio = 64;
    exint minGrainSize = 64;
};




































//namespace GFE_UVScaletoWorldSize_Namespace {
//
//
//
//
//
//template<typename T>
//static void
//uvScaletoWorldSize(
//    GA_Detail* const geo,
//    const GA_RWHandleT<UT_Vector3T<T>>& uv_h,
//    const GA_PrimitiveGroup* const geoGroup = nullptr,
//    const bool computeUVAreaInPiece = true,
//    const UT_Vector3D& uvScale = UT_Vector3D(1.0),
//    const bool doUVScalex = true,
//    const bool doUVScaley = true,
//    const bool doUVScalez = true,
//    const exint subscribeRatio = 64,
//    const exint minGrainSize = 64
//)
//{
//    if (!doUVScalex && !doUVScaley && !doUVScalez)
//        return;
//
//    if (geoGroup && geoGroup->isEmpty())
//        return;
//
//    const GA_Storage inStorage = uv_h.getAttribute()->getStorage();
//
//    GA_Attribute* areaATIPtr   = GFE_Measure::addAttribPrimArea(geo,                geoGroup, inStorage, GFE_UVScaletoWorldSize_AreaAttribName);
//    GA_Attribute* areaUVATIPtr = GFE_Measure::addAttribPrimArea(geo, uv_h, geoGroup, inStorage, GFE_UVScaletoWorldSize_AreaUVAttribName);

//
//    const GA_AttributeUPtr uvScaleATI_deleter = geo->createDetachedTupleAttribute(GA_ATTRIB_PRIMITIVE, inStorage, 3);
//    GA_Attribute* const uvScaleATIPtr = uvScaleATI_deleter.get();
//
//    //GA_ROHandleT<exint> connectivityAttrib_h;
//    
//    if (computeUVAreaInPiece)
//    {
//        const GA_Attribute* const connectivityATIPtr = GFE_Connectivity::addAttribConnectivity(geo);
//        //areaAttrib_h   = GU_Promote::promote(*static_cast<GU_Detail*>(geo), areaATIPtr,   GA_ATTRIB_PRIMITIVE, true, GU_Promote::GU_PROMOTE_SUM, NULL, connectivityATIPtr);
//        //areaUVAttrib_h = GU_Promote::promote(*static_cast<GU_Detail*>(geo), areaUVATIPtr, GA_ATTRIB_PRIMITIVE, true, GU_Promote::GU_PROMOTE_SUM, NULL, connectivityATIPtr);
//        areaATIPtr   = GU_Promote::promote(*static_cast<GU_Detail*>(geo), areaATIPtr,   GA_ATTRIB_PRIMITIVE, true, GU_Promote::GU_PROMOTE_SUM, NULL, connectivityATIPtr);
//        areaUVATIPtr = GU_Promote::promote(*static_cast<GU_Detail*>(geo), areaUVATIPtr, GA_ATTRIB_PRIMITIVE, true, GU_Promote::GU_PROMOTE_SUM, NULL, connectivityATIPtr);

//    }
//
//
//    GA_ROHandleT<T> areaAttrib_h(areaATIPtr);
//    GA_ROHandleT<T> areaUVAttrib_h(areaUVATIPtr);
//    GA_RWHandleT<UT_Vector3T<T>> uvScaleAttrib_h(uvScaleATIPtr);
//
//    {
//        const GA_SplittableRange geoSplittableRange(geo->getPrimitiveRange(geoGroup));
//        UTparallelFor(geoSplittableRange, [geo, &uvScaleAttrib_h, &areaUVAttrib_h, &areaAttrib_h, uvScale, doUVScalex, doUVScaley, doUVScalez](const GA_SplittableRange& r)
//        {
//            GA_Offset start, end;
//            UT_Vector3T<T> uvS;
//            for (GA_Iterator it(r); it.blockAdvance(start, end); )
//            {
//                for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
//                {
//                    const T area = areaAttrib_h.get(elemoff);
//                    const T areaUV = areaUVAttrib_h.get(elemoff);
//                    uvS = uvScale * sqrt(area / areaUV);
//                    uvS[0] = doUVScalex ? uvS[0] : T(1);
//                    uvS[1] = doUVScaley ? uvS[1] : T(1);
//                    uvS[2] = doUVScalez ? uvS[2] : T(1);
//                    uvScaleAttrib_h.set(elemoff, uvS);
//                }
//            }
//        }, subscribeRatio, minGrainSize);
//    }
//
//    {
//        const GA_AttributeOwner uvAttribClassFinal = uv_h.getAttribute()->getOwner();
//        const bool uvAttribClassFinal_bool = uvAttribClassFinal == GA_ATTRIB_POINT;
//        const GA_SplittableRange geoSplittableRange = GFE_Range::getSplittableRangeByAnyGroup(geo, geoGroup, uvAttribClassFinal);
//        UTparallelFor(geoSplittableRange, [geo, &uv_h, &uvScaleAttrib_h, uvAttribClassFinal_bool](const GA_SplittableRange& r)
//        {
//            GA_Offset start, end;
//            for (GA_Iterator it(r); it.blockAdvance(start, end); )
//            {
//                for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
//                {
//                    const GA_Offset primoff = uvAttribClassFinal_bool ? geo->vertexPrimitive(geo->pointVertex(elemoff)) : geo->vertexPrimitive(elemoff);
//                    UT_Vector3T<T> uv = uv_h.get(elemoff) * uvScaleAttrib_h.get(primoff);
//                    uv_h.set(elemoff, uv);
//                }
//            }
//        }, subscribeRatio, minGrainSize);
//    }
//
//}
//
//
//
//SYS_FORCE_INLINE
//static void
//uvScaletoWorldSize(
//    GA_Detail* const geo,
//    GA_Attribute* const uvAttribPtr,
//    const GA_PrimitiveGroup* const geoGroup = nullptr,
//    const bool computeUVAreaInPiece = true,
//    const UT_Vector3D& uvScale = UT_Vector3D(1.0),
//    const bool doUVScalex = true,
//    const bool doUVScaley = true,
//    const bool doUVScalez = true,
//    const exint subscribeRatio = 64,
//    const exint minGrainSize = 64
//)
//{
//
//    //static_cast<GA_ATINumeric*>(uvAttribPtr)->getStorage();
//    switch (uvAttribPtr->getAIFTuple()->getStorage(uvAttribPtr))
//    {
//    //case GA_STORE_INT8:
//    //    uvScaletoWorldSize<int8>(geo, uvAttribPtr, geoGroup, computeUVAreaInPiece,
//    //        uvScale, doUVScalex, doUVScaley, doUVScalez, subscribeRatio, minGrainSize);
//    //    break;
//    //case GA_STORE_INT16:
//    //    uvScaletoWorldSize<int16>(geo, uvAttribPtr, geoGroup, computeUVAreaInPiece,
//    //        uvScale, doUVScalex, doUVScaley, doUVScalez, subscribeRatio, minGrainSize);
//    //    break;
//    case GA_STORE_INT32:
//        uvScaletoWorldSize<int>(geo, uvAttribPtr, geoGroup, computeUVAreaInPiece,
//            uvScale, doUVScalex, doUVScaley, doUVScalez, subscribeRatio, minGrainSize);
//        break;
//    case GA_STORE_INT64:
//        uvScaletoWorldSize<int64>(geo, uvAttribPtr, geoGroup, computeUVAreaInPiece,
//            uvScale, doUVScalex, doUVScaley, doUVScalez, subscribeRatio, minGrainSize);
//        break;
//    case GA_STORE_REAL16:
//        uvScaletoWorldSize<fpreal16>(geo, uvAttribPtr, geoGroup, computeUVAreaInPiece,
//            uvScale, doUVScalex, doUVScaley, doUVScalez, subscribeRatio, minGrainSize);
//        break;
//    case GA_STORE_REAL32:
//        uvScaletoWorldSize<fpreal32>(geo, uvAttribPtr, geoGroup, computeUVAreaInPiece,
//            uvScale, doUVScalex, doUVScaley, doUVScalez, subscribeRatio, minGrainSize);
//        break;
//    case GA_STORE_REAL64:
//        uvScaletoWorldSize<fpreal64>(geo, uvAttribPtr, geoGroup, computeUVAreaInPiece,
//            uvScale, doUVScalex, doUVScaley, doUVScalez, subscribeRatio, minGrainSize);
//        break;
//    default:
//        break;
//    }
//}
//
//SYS_FORCE_INLINE
//static GA_Attribute*
//uvScaletoWorldSize(
//    GA_Detail* const geo,
//    const GA_AttributeOwner uvAttribClass,
//    const UT_StringHolder& uvAttribName,
//    const GA_PrimitiveGroup* const geoGroup = nullptr,
//    const bool computeUVAreaInPiece = true,
//    const UT_Vector3D& uvScale = UT_Vector3D(1.0),
//    const bool doUVScalex = true,
//    const bool doUVScaley = true,
//    const bool doUVScalez = true,
//    const exint subscribeRatio = 64,
//    const exint minGrainSize = 64
//)
//{
//    GA_Attribute* uvAttribPtr = GFE_Attribute::findUVAttributePointVertex(geo, uvAttribClass, uvAttribName);
//    if (!uvAttribPtr)
//        return nullptr;
//
//    uvScaletoWorldSize(geo, uvAttribPtr, geoGroup, computeUVAreaInPiece,
//        uvScale, doUVScalex, doUVScaley, doUVScalez,
//        subscribeRatio, minGrainSize);
//
//    return uvAttribPtr;
//}
//
//
//SYS_FORCE_INLINE
//static GA_Attribute*
//uvScaletoWorldSize(
//    GA_Detail* const geo,
//    const GA_AttributeOwner uvAttribClass,
//    const UT_StringHolder& uvAttribName,
//    const GA_Group* const geoGroup = nullptr,
//    const bool computeUVAreaInPiece = true,
//    const UT_Vector3D& uvScale = UT_Vector3D(1.0),
//    const bool doUVScalex = true,
//    const bool doUVScaley = true,
//    const bool doUVScalez = true,
//    const exint subscribeRatio = 64,
//    const exint minGrainSize = 64
//)
//{
//    if (!geoGroup)
//    {
//        return uvScaletoWorldSize(geo, uvAttribClass, uvAttribName, static_cast<const GA_PrimitiveGroup*>(geoGroup), computeUVAreaInPiece,
//            uvScale, doUVScalex, doUVScaley, doUVScalez,
//            subscribeRatio, minGrainSize);
//    }
//    switch (geoGroup->classType())
//    {
//    case GA_GROUP_PRIMITIVE:
//        return uvScaletoWorldSize(geo, uvAttribClass, uvAttribName, static_cast<const GA_PrimitiveGroup*>(geoGroup), computeUVAreaInPiece,
//            uvScale, doUVScalex, doUVScaley, doUVScalez,
//            subscribeRatio, minGrainSize);
//        break;
//    case GA_GROUP_POINT:
//    {
//        const GA_PrimitiveGroupUPtr geoPrimGroupUPtr = GFE_GroupPromote::groupPromotePrimitiveDetached(geo, geoGroup);
//        const GA_PrimitiveGroup* const geoPrimGroup = geoPrimGroupUPtr.get();
//        return uvScaletoWorldSize(geo, uvAttribClass, uvAttribName, geoPrimGroup, computeUVAreaInPiece,
//            uvScale, doUVScalex, doUVScaley, doUVScalez,
//            subscribeRatio, minGrainSize);
//    }
//        break;
//    case GA_GROUP_VERTEX:
//    {
//        const GA_PrimitiveGroupUPtr geoPrimGroupUPtr = GFE_GroupPromote::groupPromotePrimitiveDetached(geo, geoGroup);
//        const GA_PrimitiveGroup* const geoPrimGroup = geoPrimGroupUPtr.get();
//        return uvScaletoWorldSize(geo, uvAttribClass, uvAttribName, geoPrimGroup, computeUVAreaInPiece,
//            uvScale, doUVScalex, doUVScaley, doUVScalez,
//            subscribeRatio, minGrainSize);
//    }
//        break;
//    default:
//        UT_ASSERT_MSG(0, "unhandled group type");
//        break;
//    }
//    return nullptr;
//}
//
//
//
//static GA_Attribute*
//uvScaletoWorldSize(
//    const SOP_NodeVerb::CookParms& cookparms,
//    GA_Detail* const geo,
//    const GA_AttributeOwner uvAttribClass,
//    const UT_StringHolder& uvAttribName,
//    const GA_GroupType groupType,
//    const UT_StringHolder& groupName,
//    const bool computeUVAreaInPiece = true,
//    const UT_Vector3D& uvScale = UT_Vector3D(1.0),
//    const bool doUVScalex = true,
//    const bool doUVScaley = true,
//    const bool doUVScalez = true,
//    const bool outTopoAttrib = false,
//    const exint subscribeRatio = 64,
//    const exint minGrainSize = 64
//)
//{
//    GOP_Manager gop;
//    const GA_Group* const geoGroup = GFE_GroupParser_Namespace::findOrParseGroupDetached(cookparms, geo, groupType, groupName, gop);
//
//    GA_Attribute* const uvAttribPtr = uvScaletoWorldSize(geo, uvAttribClass, uvAttribName, geoGroup,
//        computeUVAreaInPiece,
//        uvScale, doUVScalex, doUVScaley, doUVScalez,
//        subscribeRatio, minGrainSize);
//
//    if(uvAttribPtr)
//        uvAttribPtr->bumpDataId();
//
//    GFE_TopologyReference::outTopoAttrib(geo, outTopoAttrib);
//
//    return uvAttribPtr;
//}
//
//
//} // End of namespace GFE_UVScaletoWorldSize

#endif
