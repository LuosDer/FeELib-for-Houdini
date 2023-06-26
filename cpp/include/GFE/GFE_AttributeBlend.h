
#pragma once

#ifndef __GFE_AttributeBlend_h__
#define __GFE_AttributeBlend_h__

#include "GFE/GFE_AttributeBlend.h"

#include "GFE/GFE_GeoFilter.h"



class GFE_AttribBlend : public GFE_AttribCreateFilterWithRef0 {
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
        for (size_t i = 0; i < size; i++){}
        
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

public:
     
    

private:
    GA_Attribute* attribPtr;
    
    fpreal64 blend = 1;
    
    exint subscribeRatio = 64;
    exint minGrainSize = 1024;


}; // End of class GFE_AttribBlend





#endif
