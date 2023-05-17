
#pragma once

#ifndef __GFE_ScaleAttributeElement_h__
#define __GFE_ScaleAttributeElement_h__

//#include "GFE/GFE_ScaleAttribElement.h"



#include "GFE/GFE_GeoFilter.h"




class GFE_ScaleAttribElement : public GFE_AttribFilter {


public:

    using GFE_AttribFilter::GFE_AttribFilter;


~GFE_ScaleAttribElement()
{
}


void
setComputeParm(
    const bool doNormalize = true,
    const fpreal64 uniScale = 1.0,
    const exint subscribeRatio = 64,
    const exint minGrainSize = 64
)
{
    setHasComputed();
    this->doNormalize = doNormalize;
    this->uniScale = uniScale;
    this->subscribeRatio = subscribeRatio;
    this->minGrainSize = minGrainSize;
}




private:

    virtual bool
        computeCore() override
    {
        if (!getOutAttribArray().size())
            return false;

        if (!doNormalize && uniScale==1.0)
            return true;
        
        if (groupParser.isEmpty())
            return true;

        const ::std::vector<GA_Attribute*>::size_type size = getOutAttribArray().size();
        for (size_t i = 0; i < size; i++)
        {
            attribPtr = getOutAttribArray()[i];
            
            const GA_Storage storage = attribPtr->getAIFTuple()->getStorage(attribPtr);
            switch (attribPtr->getAIFTuple()->getTupleSize(attribPtr))
            {
            case 1:
                switch (storage)
                {
                    case GA_STORE_INT8:   scaleNumericAttribElement<int8>();     break;
                    case GA_STORE_INT16:  scaleNumericAttribElement<int16>();    break;
                    case GA_STORE_INT32:  scaleNumericAttribElement<int32>();    break;
                    case GA_STORE_INT64:  scaleNumericAttribElement<int64>();    break;
                    case GA_STORE_REAL16: scaleNumericAttribElement<fpreal16>(); break;
                    case GA_STORE_REAL32: scaleNumericAttribElement<fpreal32>(); break;
                    case GA_STORE_REAL64: scaleNumericAttribElement<fpreal64>(); break;
                    default: break;
                }
            case 2:
                switch (storage)
                {
                    case GA_STORE_REAL16: scaleVectorAttribElement<UT_Vector2T<fpreal16>>(); break;
                    case GA_STORE_REAL32: scaleVectorAttribElement<UT_Vector2T<fpreal32>>(); break;
                    case GA_STORE_REAL64: scaleVectorAttribElement<UT_Vector2T<fpreal64>>(); break;
                    default: break;
                }
                break;
            case 3:
                switch (storage)
                {
                    case GA_STORE_REAL16: scaleVectorAttribElement<UT_Vector3T<fpreal16>>(); break;
                    case GA_STORE_REAL32: scaleVectorAttribElement<UT_Vector3T<fpreal32>>(); break;
                    case GA_STORE_REAL64: scaleVectorAttribElement<UT_Vector3T<fpreal64>>(); break;
                    default: break;
                }
                break;
            case 4:
                switch (storage)
                {
                    case GA_STORE_REAL16: scaleVectorAttribElement<UT_Vector4T<fpreal16>>(); break;
                    case GA_STORE_REAL32: scaleVectorAttribElement<UT_Vector4T<fpreal32>>(); break;
                    case GA_STORE_REAL64: scaleVectorAttribElement<UT_Vector4T<fpreal64>>(); break;
                    default: break;
                }
                break;
            default: break;
            }
        }
        return true;
    }


    template<typename VECTOR_T>
    void scaleVectorAttribElement()
    {
        UTparallelFor(groupParser.getSplittableRange(attribPtr->getOwner()), [this](const GA_SplittableRange& r)
        {
            GA_PageHandleT<VECTOR_T, typename VECTOR_T::value_type, true, true, GA_Attribute, GA_ATINumeric, GA_Detail> attrib_ph(attribPtr);
            for (GA_PageIterator pit = r.beginPages(); !pit.atEnd(); ++pit)
            {
                GA_Offset start, end;
                for (GA_Iterator it(pit.begin()); it.blockAdvance(start, end); )
                {
                    attrib_ph.setPage(start);
                    for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                    {
                        if (doNormalize)
                            attrib_ph.value(elemoff).normalize();
                        attrib_ph.value(elemoff) *= uniScale;
                    }
                }
            }
        }, subscribeRatio, minGrainSize);
    }


    template<typename SCALE_T>
    void scaleNumericAttribElement()
    {
        UTparallelFor(groupParser.getSplittableRange(attribPtr->getOwner()), [this](const GA_SplittableRange& r)
        {
            GA_PageHandleT<SCALE_T, SCALE_T, true, true, GA_Attribute, GA_ATINumeric, GA_Detail> attrib_ph(attribPtr);
            for (GA_PageIterator pit = r.beginPages(); !pit.atEnd(); ++pit)
            {
                GA_Offset start, end;
                for (GA_Iterator it(pit.begin()); it.blockAdvance(start, end); )
                {
                    attrib_ph.setPage(start);
                    for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                    {
                        attrib_ph.value(elemoff) *= uniScale;
                    }
                }
            }
        }, subscribeRatio, minGrainSize);
    }



    #if SYS_VERSION_MAJOR_INT > 19 || ( SYS_VERSION_MAJOR_INT == 19 && SYS_VERSION_MINOR_INT == 5 )
    
    template<typename VECTOR_T>
    void scaleVectorAttribElement1()
    {
        GAparallelForEachPage(groupParser.getRange(attribPtr->getOwner()), true, [this](GA_PageIterator pit)
        {
            GA_PageHandleT<VECTOR_T, typename VECTOR_T::value_type, true, true, GA_Attribute, GA_ATINumeric, GA_Detail> attrib_ph(attribPtr);
            GAforEachPageBlock(pit, [&attrib_ph, this](GA_Offset start, GA_Offset end)
            {
                attrib_ph.setPage(start);
                for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                {
                    if (doNormalize)
                        attrib_ph.value(elemoff).normalize();
                    attrib_ph.value(elemoff) *= uniScale;
                }
            });
        });
    }
    
    #endif



public:
    bool doNormalize = true;
    fpreal64 uniScale = 1;

private:
    GA_Attribute* attribPtr;
    GA_Attribute* refAttribPtr;

    
    exint subscribeRatio = 64;
    exint minGrainSize = 64;

    
};








#endif





