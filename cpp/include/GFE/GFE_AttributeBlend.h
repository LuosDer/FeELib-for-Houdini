
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
        if(this->geoRef0 ? getRef0AttribArray().isEmpty() : getInAttribArray().isEmpty() )
        {
            return true;
        }
        if(groupParser.isEmpty())
        {
            return true;
        }
        
        this->geoTemp = this->geoRef0 ? this->geoRef0 : this->geo;
        
        const size_t sizeInAttrib  = this->geoRef0 ? getRef0AttribArray().size() : getInAttribArray().size();
        const size_t sizeOutAttrib = getOutAttribArray().size();

        if(sizeOutAttrib > 0)
        {
            const size_t attribSize = SYSmin(sizeInAttrib, sizeOutAttrib);
            for(size_t i = 0; i < attribSize; i++)
            {
                this->dstAttribPtr = getOutAttribArray()[i];
                this->srcAttribPtr = this->geoRef0 ? getRef0AttribArray()[i] : getInAttribArray()[i];
                

                const GA_Storage dstStorage = this->dstAttribPtr->getAIFTuple()->getStorage(this->dstAttribPtr);
                const GA_Storage srcStorage = this->srcAttribPtr->getAIFTuple()->getStorage(this->srcAttribPtr);
        
                if(dstStorage == srcStorage)
                {
                        switch (dstStorage)
                        {
                            case GA_Storage::GA_STORE_INT8:   attribBlend<int8>();     break;
                            case GA_Storage::GA_STORE_INT16:  attribBlend<int16>();     break;
                            case GA_Storage::GA_STORE_INT32:  attribBlend<int32>();     break;
                            case GA_Storage::GA_STORE_INT64:  attribBlend<int64>();     break;
                            case GA_Storage::GA_STORE_REAL16: attribBlend<fpreal16>();     break;
                            case GA_Storage::GA_STORE_REAL32: attribBlend<fpreal32>();     break;
                            case GA_Storage::GA_STORE_REAL64: attribBlend<fpreal64>();     break;
                        }
                }
            }
        }
        //dstattrib empty
        else
        {
            for(size_t i = 0; i < sizeInAttrib; i++)
            {
                this->srcAttribPtr = this->geoRef0 ? getRef0AttribArray()[i] : getInAttribArray()[i];
                const UT_StringHolder& newName = this->srcAttribPtr->getName();
                const bool detached = GFE_Type::isInvalid(newName);
                dstAttribPtr = getOutAttribArray().clone(detached, this->*srcAttribPtr, this->destinationOwner, newName);
                attribBlend();
            }
        }

/*
        GFE_AttributeArray destinationAttribArray = getOutAttribArray();
        GFE_AttributeArray sourceAttribArray = getOutAttribArray();
        if(!getRef0AttribArray().isEmpty())
        {
            sourceAttribArray = getRef0AttribArray();
        }
        else if(!getInAttribArray().isEmpty())
        {
            sourceAttribArray = getInAttribArray();
        }
        else
        {
            return false;
        }

        
        const ::std::vector<GA_Attribute*>::size_type size = destinationAttribArray.size();
        for (size_t i = 0; i < size; i++)
        {
            this->attribPtr = destinationAttribArray[i];
            this->refAttribPtr = sourceAttribArray[i];
            
            const GA_AIFTuple* const destinationAifTuple = attribPtr->getAIFTuple();
            const GA_AIFTuple* const sourceAifTuple = refAttribPtr->getAIFTuple();
            
            const GA_Storage destinationStorage = destinationAifTuple->getStorage(attribPtr);
            const GA_Storage sourceStorage = sourceAifTuple->getStorage(refAttribPtr);
            
            
            int destinationAifTupleSize = destinationAifTuple->getTupleSize(attribPtr);
            int sourceAifTupleSize = sourceAifTuple->getTupleSize(refAttribPtr);
            switch (destinationAifTupleSize)
            {
                
            }
        }
*/
        return  true;
    }

    template<typename T, GA_AttributeOwner dstOwner>
    void attribBlend()
    {
        //UT_ASSERT_P(this->&srcAttribPtr->getDetail() == this->geoTemp);
        const GA_Attribute& srcAttribRef = *srcAttribPtr;
        const GA_Attribute& dstAttribRef = *dstAttribPtr;


        if(this->geoRef0)
        {
            const GA_ROHandleT<T> srcAttrib_h(this->srcAttribPtr);
            //const GA_IndexMap& indexMapSrc = geoRef0->getIndexMap(SrcOwner);
            UTparallelFor(groupParser.getSplittableRange(dstOwner), [this, &srcAttrib_h](const GA_SplittableRange& r)
            {
                GA_PageHandleT<T, typename T::value_type, true, true, GA_Attribute, GA_ATINumeric, GA_Detail> dstAttrib_ph(this->dstAttribPtr);
                for(GA_PageIterator pit = r.beginPages(); !pit.atEnd(); pit++)
                {
                    GA_Offset start, end;
                    for(GA_Iterator it(pit.begin()); it.blockAdvance(start, end);)
                    {
                        dstAttrib_ph.setPage(start);
                        for(GA_Offset elemoff = start; elemoff < end; elemoff++)
                        {
                            dstAttrib_ph.value(elemoff) = dstAttrib_ph.value(elemoff) * (1 - this->blend) + srcAttrib_h.value(elemoff) * this->blend;
                        }
                    }
                }
            }, this->subscribeRatio, this->minGrainSize);
            //GA_PageHandleT<VECTOR_T, typename VECTOR_T::value_type, true, true, GA_Attribute, GA_ATINumeric, GA_Detail> attrib_ph(dstAttribPtr);
        }





        
    }
/*
    template<typename VECTOR_T, GA_AttributeOwner SrcOwner, GA_AttributeOwner dstOwner>
    void attribBlend()
    {}
*/
    template<typename T>
    void attribBlend()
    {
        switch (this->srcAttribPtr->getOwner())
        {
        case GA_AttributeOwner::GA_ATTRIB_POINT:        attribBlend<T, GA_AttributeOwner::GA_ATTRIB_POINT>();      return; break;
        case GA_AttributeOwner::GA_ATTRIB_VERTEX:       attribBlend<T, GA_AttributeOwner::GA_ATTRIB_VERTEX>();     return; break;
        case GA_AttributeOwner::GA_ATTRIB_PRIMITIVE:    attribBlend<T, GA_AttributeOwner::GA_ATTRIB_PRIMITIVE>();  return; break;
        case GA_AttributeOwner::GA_ATTRIB_DETAIL:       attribBlend<T, GA_AttributeOwner::GA_ATTRIB_DETAIL>();     return; break;
        }
        UT_ASSERT_MSG(0, "Src AttribOwner Vaild"); 
    }

    
public:
    fpreal64 blend = 1;
    GA_AttributeOwner sourceOwner = GA_ATTRIB_INVALID;
    GA_AttributeOwner destinationOwner = GA_ATTRIB_INVALID;



private:
    const GFE_Detail* geoTemp;
    GA_Attribute* srcAttribPtr;
    GA_Attribute* dstAttribPtr;

    exint subscribeRatio = 64;
    exint minGrainSize = 1024;


}; // End of class GFE_AttribBlend






#endif
