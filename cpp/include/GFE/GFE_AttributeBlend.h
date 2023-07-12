
#pragma once

#ifndef __GFE_AttributeBlend_h__
#define __GFE_AttributeBlend_h__

#include "GFE/GFE_AttributeBlend.h"

#include "GFE/GFE_GeoFilter.h"
#include "GFE/GFE_GroupUnion.h"


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
                this->srcAttribPtr = this->geoRef0 ? getRef0AttribArray()[i] : getInAttribArray()[i];
                this->dstAttribPtr = getOutAttribArray()[i];

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
        return  false;
    }

    template<typename VECTOR_T, GA_AttributeOwner SrcOwner, GA_AttributeOwner TO>
    void attribBlend()
    {
        //UT_ASSER
        
    }
    template<typename VECTOR_T, GA_AttributeOwner FROM>
    SYS_FORCE_INLINE void attribBlend()
    {
        switch (this->sourceOwner)
        {
        case GA_AttributeOwner::GA_ATTRIB_PRIMITIVE: attribBlend<VECTOR_T, FROM, GA_AttributeOwner::GA_ATTRIB_PRIMITIVE>();break;
        case GA_AttributeOwner::GA_ATTRIB_POINT: attribBlend<VECTOR_T, FROM, GA_AttributeOwner::GA_ATTRIB_POINT>();break;
        case GA_AttributeOwner::GA_ATTRIB_VERTEX: attribBlend<VECTOR_T, FROM, GA_AttributeOwner::GA_ATTRIB_VERTEX>();break;
        case GA_AttributeOwner::GA_ATTRIB_DETAIL: attribBlend<VECTOR_T, FROM, GA_AttributeOwner::GA_ATTRIB_DETAIL>();break;
        default: UT_ASSERT_MSG(0, "Unhandled Owner"); break;
        }
    }
    template<typename VECTOR_T>
    SYS_FORCE_INLINE void attribBlendBase(VECTOR_T& origin, VECTOR_T& target, fpreal64 blend)
    {
        origin = (1 - blend) * origin + blend * target;
    }

public:
    fpreal64 blend = 1;
    GA_AttributeOwner sourceOwner;
    GA_AttributeOwner destinationOwner;

private:
    const GFE_Detail* geoTemp;
    GA_Attribute* srcAttribPtr;
    GA_Attribute* dstAttribPtr;

    exint subscribeRatio = 64;
    exint minGrainSize = 1024;


}; // End of class GFE_AttribBlend





#endif
