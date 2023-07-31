
#pragma once

#ifndef __GFE_ScaleAttributeElement_h__
#define __GFE_ScaleAttributeElement_h__

#include "GFE/GFE_ScaleAttributeElement.h"

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
        const fpreal uniScale = 1.0,
        const exint subscribeRatio = 64,
        const exint minGrainSize   = 1024
    )
{
    setHasComputed();
    this->doNormalize = doNormalize;
    this->uniScale = uniScale;
    this->subscribeRatio = subscribeRatio;
    this->minGrainSize   = minGrainSize;
}


//#define GFE_ForEachStorage2(Storage, Func)                          \
//        switch (Storage)                                            \
//        {                                                           \
//        case GA_STORE_INT8:   Func<UT_Vector2T<int8    >>(); break; \
//        case GA_STORE_INT16:  Func<UT_Vector2T<int16   >>(); break; \
//        case GA_STORE_INT32:  Func<UT_Vector2T<int32   >>(); break; \
//        case GA_STORE_INT64:  Func<UT_Vector2T<int64   >>(); break; \
//        case GA_STORE_REAL16: Func<UT_Vector2T<fpreal16>>(); break; \
//        case GA_STORE_REAL32: Func<UT_Vector2T<fpreal32>>(); break; \
//        case GA_STORE_REAL64: Func<UT_Vector2T<fpreal64>>(); break; \
//        default: UT_ASSERT_MSG(0, "Unhandled Storage");      break; \
//        }                                                           \




    
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

        const size_t size = getOutAttribArray().size();
        for (size_t i = 0; i < size; i++)
        {
            attrib = getOutAttribArray()[i];
            
            const GA_Storage storage = attrib->getAIFTuple()->getStorage(attrib);
            GFE_ForEachStorageTupleSizeVec(scaleAttribElement, storage, attrib->getAIFTuple()->getTupleSize(attrib))
            /*
            switch (attrib->getAIFTuple()->getTupleSize(attrib))
            {
            case 1:
                switch (storage)
                {
                    case GA_STORE_INT8:   scaleAttribElement<int8>();     break;
                    case GA_STORE_INT16:  scaleAttribElement<int16>();    break;
                    case GA_STORE_INT32:  scaleAttribElement<int32>();    break;
                    case GA_STORE_INT64:  scaleAttribElement<int64>();    break;
                    case GA_STORE_REAL16: scaleAttribElement<fpreal16>(); break;
                    case GA_STORE_REAL32: scaleAttribElement<fpreal32>(); break;
                    case GA_STORE_REAL64: scaleAttribElement<fpreal64>(); break;
                    default: break;
                }
            break;
            case 2:
                switch (storage)
                {
                    case GA_STORE_REAL16: scaleAttribElement<UT_Vector2T<fpreal16>>(); break;
                    case GA_STORE_REAL32: scaleAttribElement<UT_Vector2T<fpreal32>>(); break;
                    case GA_STORE_REAL64: scaleAttribElement<UT_Vector2T<fpreal64>>(); break;
                    default: break;
                }
            break;
            case 3:
                switch (storage)
                {
                    case GA_STORE_REAL16: scaleAttribElement<UT_Vector3T<fpreal16>>(); break;
                    case GA_STORE_REAL32: scaleAttribElement<UT_Vector3T<fpreal32>>(); break;
                    case GA_STORE_REAL64: scaleAttribElement<UT_Vector3T<fpreal64>>(); break;
                    default: break;
                }
            break;
            case 4:
                switch (storage)
                {
                    case GA_STORE_REAL16: scaleAttribElement<UT_Vector4T<fpreal16>>(); break;
                    case GA_STORE_REAL32: scaleAttribElement<UT_Vector4T<fpreal32>>(); break;
                    case GA_STORE_REAL64: scaleAttribElement<UT_Vector4T<fpreal64>>(); break;
                    default: break;
                }
            break;
            default: break;
            }
            */
        }
        return true;
    }


    template<typename _Ty>
    void scaleAttribElement()
    {
        UTparallelFor(groupParser.getSplittableRange(attrib), [this](const GA_SplittableRange& r)
        {
            GFE_RWPageHandleT<_Ty> attrib_ph(attrib);
            GA_Offset start, end;
            for (GA_PageIterator pit = r.beginPages(); !pit.atEnd(); ++pit)
            {
                for (GA_Iterator it(pit.begin()); it.blockAdvance(start, end); )
                {
                    attrib_ph.setPage(start);
                    for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                    {
                        if constexpr (GFE_Type::isVector<_Ty>)
                        {
                            if (doNormalize)
                                attrib_ph.value(elemoff).normalize();
                        }
                        attrib_ph.value(elemoff) *= uniScale;
                    }
                }
            }
        }, subscribeRatio, minGrainSize);
    }




public:
    bool doNormalize = true;
    fpreal uniScale = 1.0;

private:
    GA_Attribute* attrib;
    GA_Attribute* refattrib;

    
    exint subscribeRatio = 64;
    exint minGrainSize   = 1024;

    
};








#endif





