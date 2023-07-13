
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
                this->srcAttribPtr = this->geoRef0 ? getRef0AttribArray()[i] : getInAttribArray()[i];
                this->dstAttribPtr = getOutAttribArray()[i];
                //compute
                attribBlend();
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

    template<GA_AttributeOwner SrcOwner, GA_AttributeOwner dstOwner>
    void attribBlend()
    {
        UT_ASSERT_P(this->&srcAttribPtr->getDetail() == this->geoTemp);
        const GA_Attribute& srcAttribRef = *srcAttribPtr;
        if(this->geoRef0)
        {
            const GA_IndexMap& indexMapSrc = geoRef0->getIndexMap(SrcOwner)
            UTparallelFor(groupParser.getSplittableRange(dstOwner), [this, &srcAttribRef, &indexMapSrc](const GA_SplittableRange& r)
            {
               GA_Offset start, end;
               for(GA_Iterator it(r); it.blockAdvance(start, end); )
               {
                   for(GA_Offset elemoff = start; elemoff < end; elemoff++)
                   {
                       const GA_Offset dstOff = this->geo->offsetPromote<dstOwner, SrcOwner>(elemoff);
                       if (GFE_Type::isValidOffset(dstOff))
                       {
                          
                       }
                        //const GA_Offset dstOff = this->geo                      
                   }
               }
            }, this->subscribeRatio, this->minGrainSize);
        }





        
    }
/*
    template<typename VECTOR_T, GA_AttributeOwner SrcOwner, GA_AttributeOwner dstOwner>
    void attribBlend()
    {}
*/
    template<GA_AttributeOwner SrcOwner>
    void attribBlend()
    {
        switch (this->dstAttribPtr->getOwner())
        {
        case GA_AttributeOwner::GA_ATTRIB_POINT:        attribBlend<SrcOwner, GA_AttributeOwner::GA_ATTRIB_POINT>();      return; break;
        case GA_AttributeOwner::GA_ATTRIB_VERTEX:       attribBlend<SrcOwner, GA_AttributeOwner::GA_ATTRIB_VERTEX>();     return; break;
        case GA_AttributeOwner::GA_ATTRIB_PRIMITIVE:    attribBlend<SrcOwner, GA_AttributeOwner::GA_ATTRIB_PRIMITIVE>();  return; break;
        case GA_AttributeOwner::GA_ATTRIB_DETAIL:       attribBlend<SrcOwner, GA_AttributeOwner::GA_ATTRIB_DETAIL>();     return; break;
        }
        UT_ASSERT_MSG(0, "dst AttribOwner Vaild"); 
    }
    
    void attribBlend()
    {
        switch (this->srcAttribPtr->getOwner())
        {
        case GA_AttributeOwner::GA_ATTRIB_POINT:        attribBlend<GA_AttributeOwner::GA_ATTRIB_POINT>();      return; break;
        case GA_AttributeOwner::GA_ATTRIB_VERTEX:       attribBlend<GA_AttributeOwner::GA_ATTRIB_VERTEX>();     return; break;
        case GA_AttributeOwner::GA_ATTRIB_PRIMITIVE:    attribBlend<GA_AttributeOwner::GA_ATTRIB_PRIMITIVE>();  return; break;
        case GA_AttributeOwner::GA_ATTRIB_DETAIL:       attribBlend<GA_AttributeOwner::GA_ATTRIB_DETAIL>();     return; break;
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
