
#pragma once

#ifndef __GFE_ScaleAttributeElement_h__
#define __GFE_ScaleAttributeElement_h__

#include "GFE/GFE_ScaleAttributeElement.h"

#include "GFE/GFE_GeoFilter.h"


#include "USRefl/USRefl.h"

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
            
            auto storageVariant = GFE_Type::attribStorageVariant(storage);
            
            GFE_ForEachStorageTupleSizeVec(scaleAttribElement, storage, attrib->getAIFTuple()->getTupleSize(attrib))
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





