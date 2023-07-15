
#pragma once

#ifndef __GFE_AttributeBlend_h__
#define __GFE_AttributeBlend_h__

#include "GFE/GFE_AttributeBlend.h"

#include "GFE/GFE_GeoFilter.h"
#include "GFE/GFE_GroupUnion.h"
#include "UFE/UFE_SplittableString.h"


class GFE_AttribBlend : public GFE_AttribCreateFilterWithRef0
{
public:
    using GFE_AttribCreateFilterWithRef0::GFE_AttribCreateFilterWithRef0;
    void
    setComputeParm(
            const fpreal64 blend = 1,
            const exint subscribeRatio = 64,
            const exint minGrainSize   = 64
        )
    {
        setHasComputed();
        this->blend = blend;
        this->subscribeRatio = subscribeRatio;
        this->minGrainSize   = minGrainSize;
    }
private:

    // can not use in parallel unless for each GA_Detail
    virtual bool
        computeCore() override
    {
        if(geoRef0 ? getRef0AttribArray().isEmpty() : getInAttribArray().isEmpty() )
        {
            return true;
        }
        if(groupParser.isEmpty())
        {
            return true;
        }
        
        geoTemp = geoRef0 ? geoRef0 : geo;
        
        const size_t sizeInAttrib  = geoRef0 ? getRef0AttribArray().size() : getInAttribArray().size();
        const size_t sizeOutAttrib = getOutAttribArray().size();

        if(sizeOutAttrib > 0)
        {
            const size_t attribSize = SYSmin(sizeInAttrib, sizeOutAttrib);
            for(size_t i = 0; i < attribSize; i++)
            {
                dstAttribPtr = getOutAttribArray()[i];
                srcAttribPtr = geoRef0 ? getRef0AttribArray()[i] : getInAttribArray()[i];
                
                if(!dstAttribPtr->getAIFTuple() || !srcAttribPtr->getAIFTuple()) continue;
                
                const GA_Storage dstStorage = dstAttribPtr->getAIFTuple()->getStorage(dstAttribPtr);
                const GA_Storage srcStorage = srcAttribPtr->getAIFTuple()->getStorage(srcAttribPtr);
        
                if(dstStorage == srcStorage)
                {
                    attribBlend();
                }
            }
        }
        
        return  true;
    }

    template<typename VECTOR_T, GA_AttributeOwner dstOwner, GA_AttributeOwner srcOwner>
    void attribBlendVector()
    {
        const GA_ROHandleT<VECTOR_T> srcAttrib_h(srcAttribPtr);
        //const GA_IndexMap& indexMapSrc = geoRef0->getIndexMap(SrcOwner);

        UTparallelFor(groupParser.getSplittableRange(dstOwner), [this, &srcAttrib_h](const GA_SplittableRange& r)
        {
            GA_PageHandleT<VECTOR_T, typename VECTOR_T::value_type, true, true, GA_Attribute, GA_ATINumeric, GA_Detail> dstAttrib_ph(dstAttribPtr);
            for(GA_PageIterator pit = r.beginPages(); !pit.atEnd(); ++pit)
            {
                GA_Offset start, end;
                for(GA_Iterator it(pit.begin()); it.blockAdvance(start, end);)
                {
                    dstAttrib_ph.setPage(start);
                    for(GA_Offset elemoff = start; elemoff < end; elemoff++)
                    {
                        dstAttrib_ph.value(elemoff) = dstAttrib_ph.value(elemoff) * (1.0 - blend) + srcAttrib_h.get(elemoff) * blend;
                    }
                }
            }
        }, subscribeRatio, minGrainSize);
    }

    
    template<typename FLOAT_T, GA_AttributeOwner dstOwner, GA_AttributeOwner srcOwner>
    void attribBlendScalar()
    {
        const GA_ROHandleT<FLOAT_T> srcAttrib_h(srcAttribPtr);
        //const GA_IndexMap& indexMapSrc = geoRef0->getIndexMap(SrcOwner);

        UTparallelFor(groupParser.getSplittableRange(dstOwner), [this, &srcAttrib_h](const GA_SplittableRange& r)
        {
            GA_PageHandleT<FLOAT_T, FLOAT_T, true, true, GA_Attribute, GA_ATINumeric, GA_Detail> dstAttrib_ph(dstAttribPtr);
            for(GA_PageIterator pit = r.beginPages(); !pit.atEnd(); ++pit)
            {
                GA_Offset start, end;
                for(GA_Iterator it(pit.begin()); it.blockAdvance(start, end);)
                {
                    dstAttrib_ph.setPage(start);
                    for(GA_Offset elemoff = start; elemoff < end; elemoff++)
                    {
                        dstAttrib_ph.value(elemoff) = dstAttrib_ph.value(elemoff) * (1.0 - blend) + srcAttrib_h.get(elemoff) * blend;
                    }
                }
            }
        }, subscribeRatio, minGrainSize);
    }

    
    template<GA_AttributeOwner dstOwner, GA_AttributeOwner srcOwner>
    void attribBlend()
    {
        const GA_Storage storage = dstAttribPtr->getAIFTuple()->getStorage(dstAttribPtr);
        switch (srcAttribPtr->getTupleSize())
        {
        case 1:
            switch (storage)
            {
                case GA_STORE_INT32:   attribBlendScalar<int32,    dstOwner, srcOwner>(); return; break;
                case GA_STORE_INT64:   attribBlendScalar<int64,    dstOwner, srcOwner>(); return; break;
                case GA_STORE_REAL32:  attribBlendScalar<fpreal32, dstOwner, srcOwner>(); return; break;
                case GA_STORE_REAL64:  attribBlendScalar<fpreal64, dstOwner, srcOwner>(); return; break;
                default: UT_ASSERT_MSG(0, "Unhandled Storage"); break;
            }
        case 2:
            switch (storage)
            {
                case GA_STORE_INT32:   attribBlendVector<UT_Vector2T<int32>,    dstOwner, srcOwner>(); return; break;
                case GA_STORE_INT64:   attribBlendVector<UT_Vector2T<int64>,    dstOwner, srcOwner>(); return; break;
                case GA_STORE_REAL32:  attribBlendVector<UT_Vector2T<fpreal32>, dstOwner, srcOwner>(); return; break;
                case GA_STORE_REAL64:  attribBlendVector<UT_Vector2T<fpreal64>, dstOwner, srcOwner>(); return; break;
                default: UT_ASSERT_MSG(0, "Unhandled Storage"); break;
            }
        case 3:
            switch (storage)
            {
                case GA_STORE_INT32:   attribBlendVector<UT_Vector3T<int32>,    dstOwner, srcOwner>(); return; break;
                case GA_STORE_INT64:   attribBlendVector<UT_Vector3T<int64>,    dstOwner, srcOwner>(); return; break;
                case GA_STORE_REAL32:  attribBlendVector<UT_Vector3T<fpreal32>, dstOwner, srcOwner>(); return; break;
                case GA_STORE_REAL64:  attribBlendVector<UT_Vector3T<fpreal64>, dstOwner, srcOwner>(); return; break;
                default: UT_ASSERT_MSG(0, "Unhandled Storage"); break;
            }
        case 4:
            switch (storage)
            {
                case GA_STORE_INT32:   attribBlendVector<UT_Vector4T<int32>,    dstOwner, srcOwner>(); return; break;
                case GA_STORE_INT64:   attribBlendVector<UT_Vector4T<int64>,    dstOwner, srcOwner>(); return; break;
                case GA_STORE_REAL32:  attribBlendVector<UT_Vector4T<fpreal32>, dstOwner, srcOwner>(); return; break;
                case GA_STORE_REAL64:  attribBlendVector<UT_Vector4T<fpreal64>, dstOwner, srcOwner>(); return; break;
                default: UT_ASSERT_MSG(0, "Unhandled Storage"); break;
            }
            
        }
    } 
    
    template<GA_AttributeOwner dstOwner>
    void attribBlend()
    {
        switch (srcAttribPtr->getOwner())
        {
        case GA_ATTRIB_POINT:        attribBlend<dstOwner, GA_ATTRIB_POINT>();      return; break;
        case GA_ATTRIB_VERTEX:       attribBlend<dstOwner, GA_ATTRIB_VERTEX>();     return; break;
        case GA_ATTRIB_PRIMITIVE:    attribBlend<dstOwner, GA_ATTRIB_PRIMITIVE>();  return; break;
        case GA_ATTRIB_DETAIL:       attribBlend<dstOwner, GA_ATTRIB_DETAIL>();     return; break;
        default: break;
        }
    }

    
    void attribBlend()
    {
        switch (dstAttribPtr->getOwner())
        {
        case GA_ATTRIB_POINT:        attribBlend<GA_ATTRIB_POINT>();      return; break;
        case GA_ATTRIB_VERTEX:       attribBlend<GA_ATTRIB_VERTEX>();     return; break;
        case GA_ATTRIB_PRIMITIVE:    attribBlend<GA_ATTRIB_PRIMITIVE>();  return; break;
        case GA_ATTRIB_DETAIL:       attribBlend<GA_ATTRIB_DETAIL>();     return; break;
        default: break;
        }
    }

    
public:
    fpreal blend = 1.0;
    GA_AttributeOwner sourceOwner = GA_ATTRIB_INVALID;
    GA_AttributeOwner destinationOwner = GA_ATTRIB_INVALID;



private:
    const GFE_Detail* geoTemp;
    const GA_Attribute* srcAttribPtr;
    GA_Attribute* dstAttribPtr;

    exint subscribeRatio = 64;
    exint minGrainSize = 1024;


}; // End of class GFE_AttribBlend






#endif
