
#pragma once

#ifndef __GFE_AttributeBlend_h__
#define __GFE_AttributeBlend_h__

#include "GFE/GFE_AttributeBlend.h"

#include "GFE/GFE_GeoFilter.h"



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
        if(getInAttribArray().isEmpty())
        {
            return  false;
        }

        const ::std::vector<GA_Attribute*>::size_type size = getInAttribArray().size();
        for (size_t i = 0; i < size; i++)
        {
            
            
        }
        
        //attribPtr = getInAttribArray()[i];
        /*
        UTparallelFor(groupParser.getSplittableRange(attribPtr->getOwner()), [this](const GA_SplittableRange& r)
    {
        GA_PageHandleT<UT_Vector3T, UT_Vector3T::value_type, true, true, GA_Attribute, GA_ATINumeric, GA_Detail> attrib_ph(attribPtr);
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
        */
        return  false;
    }

    template<typename VECTOR_T, GA_AttributeOwner FROM, GA_AttributeOwner TO>
    void attribBlend()
    {
        UT_ASSERT_P(attrib);
        const GA_ROHandleT<VECTOR_T> attribOrigin_h(this->att)

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
     
    

private:
    GA_Attribute* attribPtr


    
    GA_AttributeOwner sourceOwner;
    GA_AttributeOwner destinationOwner;

    
    fpreal64 blend = 1;
    

    
    exint subscribeRatio = 64;
    exint minGrainSize = 1024;


}; // End of class GFE_AttribBlend





#endif
