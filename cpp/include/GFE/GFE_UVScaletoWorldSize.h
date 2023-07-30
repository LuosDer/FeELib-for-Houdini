
#pragma once

#ifndef __GFE_UVScaletoWorldSize_h__
#define __GFE_UVScaletoWorldSize_h__

#include "GFE/GFE_UVScaletoWorldSize.h"

#include "GFE/GFE_GeoFilter.h"




#include "GU/GU_Promote.h"


#include "GFE/GFE_Connectivity.h"
#include "GFE/GFE_Measure.h"
//#include "GFE/GFE_Attribute.h"
//#include "GFE/GFE_GroupParser.h"
//#include "GFE/GFE_GroupPromote.h"
//#include "GFE/GFE_Range.h"



class GFE_UVScaletoWorldSize : public GFE_AttribFilter {

//#define GFE_UVScaletoWorldSize_AreaAttribName   "__area_GFE_UVScaletoWorldSize"
//#define GFE_UVScaletoWorldSize_AreaUVAttribName "__areaUV_GFE_UVScaletoWorldSize"

public:

    using GFE_AttribFilter::GFE_AttribFilter;
    

    void
        setComputeParm(
            const bool computeUVAreaInPiece = true,
            const UT_Vector4D& uvScale = UT_Vector4D(1.0, 1.0, 1.0, 1.0),
            const bool doUVScalex = true,
            const bool doUVScaley = true,
            const bool doUVScalez = true,
            const bool outTopoAttrib = true,
            const exint subscribeRatio = 64,
            const exint minGrainSize   = 1024
        )
    {
        setHasComputed();
        this->outTopoAttrib = outTopoAttrib;
        this->computeUVAreaInPiece = computeUVAreaInPiece;
        this->uvScale = uvScale;
        this->doUVScalex = doUVScalex;
        this->doUVScaley = doUVScaley;
        this->doUVScalez = doUVScalez;
        this->subscribeRatio = subscribeRatio;
        this->minGrainSize   = minGrainSize;
    }





private:


    virtual bool
        computeCore() override
    {
        if (!doUVScalex && !doUVScaley && !doUVScalez)
            return true;

        if (groupParser.isEmpty())
            return true;

        const size_t len = getOutAttribArrayRef().size();
        for (size_t i = 0; i < len; ++i)
        {
            uvAttrib = getOutAttribArrayRef()[i];
            const GA_Storage storage = uvAttrib->getAIFTuple()->getStorage(uvAttrib);
            switch (uvAttrib->getAIFTuple()->getTupleSize(uvAttrib))
            {
            case 2:
                switch (storage)
                {
                case GA_STORE_INT32:  uvScaletoWorldSize<UT_Vector2T<int>>();      break;
                case GA_STORE_INT64:  uvScaletoWorldSize<UT_Vector2T<int64>>();    break;
                case GA_STORE_REAL16: uvScaletoWorldSize<UT_Vector2T<fpreal16>>(); break;
                case GA_STORE_REAL32: uvScaletoWorldSize<UT_Vector2T<fpreal32>>(); break;
                case GA_STORE_REAL64: uvScaletoWorldSize<UT_Vector2T<fpreal64>>(); break;
                }
                break;
            case 3:
                switch (storage)
                {
                case GA_STORE_INT32:  uvScaletoWorldSize<UT_Vector3T<int>>();      break;
                case GA_STORE_INT64:  uvScaletoWorldSize<UT_Vector3T<int64>>();    break;
                case GA_STORE_REAL16: uvScaletoWorldSize<UT_Vector3T<fpreal16>>(); break;
                case GA_STORE_REAL32: uvScaletoWorldSize<UT_Vector3T<fpreal32>>(); break;
                case GA_STORE_REAL64: uvScaletoWorldSize<UT_Vector3T<fpreal64>>(); break;
                }
                break;
            case 4:
                switch (storage)
                {
                //case GA_STORE_INT32:  uvScaletoWorldSize<UT_Vector4T<int>>();      break;
                //case GA_STORE_INT64:  uvScaletoWorldSize<UT_Vector4T<int64>>();    break;
                //case GA_STORE_REAL16: uvScaletoWorldSize<UT_Vector4T<fpreal16>>(); break;
                case GA_STORE_REAL32: uvScaletoWorldSize<UT_Vector4T<fpreal32>>(); break;
                case GA_STORE_REAL64: uvScaletoWorldSize<UT_Vector4T<fpreal64>>(); break;
                }
                break;
            }
        
        }
        return true;
    }



    template<typename VECTOR_T>
    void uvScaletoWorldSize()
    {
        using value_type = typename VECTOR_T::value_type;
        
        const GA_RWHandleT<VECTOR_T>& uv_h(uvAttrib);
        
        GFE_Measure measure(geo, cookparms);
        measure.groupParser.setGroup(groupParser.getPrimitiveGroup());
        //measure.setPositionAttrib();
        //measure.setComputeParm(GFE_MeasureType::Area);

        //GA_Attribute* areaATIPtr = measure.getOutAttribArray().set(GA_ATTRIB_PRIMITIVE, GFE_UVScaletoWorldSize_AreaAttribName);
        GA_Attribute* areaAttrib = measure.findOrCreateTuple(true);
        measure.compute();

        //GA_Attribute* areaUVATIPtr = measure.getOutAttribArray().set(GA_ATTRIB_PRIMITIVE, GFE_UVScaletoWorldSize_AreaUVAttribName);
        GA_Attribute* areaUVAttrib = measure.findOrCreateTuple(true);
        measure.setPositionAttrib(uvAttrib);
        measure.compute();
        

        const GA_AttributeUPtr uvScaleUPtr = geo->createDetachedTupleAttribute(GA_ATTRIB_PRIMITIVE, uv_h->getStorage(), 3);
        GA_Attribute* const uvScaleAttrib = uvScaleUPtr.get();

        GFE_Connectivity connectivity(geo, cookparms);
        if (computeUVAreaInPiece)
        {
            connectivity.groupParser.setGroup(groupParser.getPrimitiveGroup());
            //connectivity.setComputeParm(connectivityConstraint, true);
            connectivity.findOrCreateTuple(true, GA_ATTRIB_PRIMITIVE);
            connectivity.compute();
            const GA_Attribute* const connectivityAttrib = connectivity.getConnectivityAttrib();
#if 0
            GFE_AttribPromote attribPromote(geo, cookparms);

            attribPromote.setSourceAttribute(areaATIPtr);
            attribPromote.setDestinationAttribute(areaATIPtr);
            attribPromote.compute();
            
            attribPromote.setSourceAttribute(GA_ATTRIB_PRIMITIVE, sopparms.getSrcAttrib());
            attribPromote.setDestinationAttribute(dstAttribClass);
            attribPromote.compute();
#else
            areaAttrib   = GU_Promote::promote(*geo->asGU_Detail(), areaAttrib,   GA_ATTRIB_PRIMITIVE, true, GU_Promote::GU_PROMOTE_SUM, NULL, connectivityAttrib);
            areaUVAttrib = GU_Promote::promote(*geo->asGU_Detail(), areaUVAttrib, GA_ATTRIB_PRIMITIVE, true, GU_Promote::GU_PROMOTE_SUM, NULL, connectivityAttrib);
#endif
        }


        const VECTOR_T uvScaleTmp = getUVScale<VECTOR_T>();
        
        UTparallelFor(groupParser.getPrimitiveSplittableRange(), [this, uvScaleAttrib, areaAttrib, areaUVAttrib, &uvScaleTmp](const GA_SplittableRange& r)
        {
            GA_PageHandleT<VECTOR_T,   value_type, true, true, GA_Attribute, GA_ATINumeric, GA_Detail> uvScale_ph(uvScaleAttrib);
            GA_PageHandleT<value_type, value_type, true, false, const GA_Attribute, const GA_ATINumeric, const GA_Detail> area_ph(areaAttrib);
            GA_PageHandleT<value_type, value_type, true, false, const GA_Attribute, const GA_ATINumeric, const GA_Detail> areaUV_ph(areaUVAttrib);
            GA_Offset start, end;
            for (GA_PageIterator pit = r.beginPages(); !pit.atEnd(); ++pit)
            {
                VECTOR_T uvS;
                for (GA_Iterator it(pit.begin()); it.blockAdvance(start, end); )
                {
                    uvScale_ph.setPage(start);
                    area_ph.setPage(start);
                    areaUV_ph.setPage(start);
                    for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                    {
                        uvS = sqrt(area_ph.value(elemoff) / areaUV_ph.value(elemoff)) * uvScaleTmp;
                        uvS[0] = doUVScalex ? uvS[0] : value_type(1);
                        uvS[1] = doUVScaley ? uvS[1] : value_type(1);
                        uvS[2] = doUVScalez ? uvS[2] : value_type(1);
                        uvScale_ph.value(elemoff) = uvS;
                    }
                }
            }
        }, subscribeRatio, minGrainSize);


        
        GA_ROHandleT<VECTOR_T> uvScaleAttrib_h(uvScaleAttrib);
        
        const GA_AttributeOwner uvAttribClassFinal = uvAttrib->getOwner();
        const bool uvAttribClassFinal_bool = uvAttribClassFinal == GA_ATTRIB_POINT;
        UTparallelFor(groupParser.getSplittableRange(uvAttribClassFinal), [this, &uvScaleAttrib_h, uvAttribClassFinal_bool](const GA_SplittableRange& r)
        {
            GA_PageHandleT<VECTOR_T, value_type, true, true, GA_Attribute, GA_ATINumeric, GA_Detail> uv_ph(uvAttrib);
            GA_Offset start, end;
            for (GA_PageIterator pit = r.beginPages(); !pit.atEnd(); ++pit)
            {
                for (GA_Iterator it(pit.begin()); it.blockAdvance(start, end); )
                {
                    uv_ph.setPage(start);
                    for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                    {
                        const GA_Offset primoff = uvAttribClassFinal_bool ? geo->vertexPrimitive(geo->pointVertex(elemoff)) : geo->vertexPrimitive(elemoff);
                        uv_ph.value(elemoff) *= uvScaleAttrib_h.get(primoff);
                    }
                }
            }
        }, subscribeRatio, minGrainSize);
    }


    template<typename VECTOR_T>
    SYS_FORCE_INLINE VECTOR_T getUVScale() const
    {
        if constexpr(VECTOR_T::tuple_size == 2)
            return VECTOR_T(uvScale[0], uvScale[1]);
        else if constexpr(VECTOR_T::tuple_size == 3)
            return VECTOR_T(uvScale[0], uvScale[1], uvScale[2]);
        else
            return uvScale;
    }


public:
    bool computeUVAreaInPiece = true;
    UT_Vector4D uvScale = UT_Vector4D(1.0, 1.0, 1.0, 1.0);
    bool doUVScalex = true;
    bool doUVScaley = true;
    bool doUVScalez = true;

private:
    GA_Attribute* uvAttrib;
    
    exint subscribeRatio = 64;
    exint minGrainSize   = 1024;
}; // End of Class GFE_UVScaletoWorldSize

#endif
