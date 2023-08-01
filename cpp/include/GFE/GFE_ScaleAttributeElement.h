
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
        }
        return true;
    }



    void scaleAttribElement()
    {
        const GA_Storage storage = attrib->getAIFTuple()->getStorage(attrib);

        
        auto tupleSizeVariant = GFE_Type::getAttribTupleSizeVariant(attrib);
        auto storeVariant = GFE_Type::getAttribStorageVariant(storage);
        auto doNormalizeVariant = GFE_Type::getBoolVariant(doNormalize);
        auto scaleVariant = GFE_Type::getBoolVariant(uniScale != 1.0);
        
        std::visit([this] (auto scaleVariant, auto doNormalizeVariant, auto tupleSizeVariant, auto storeVariant)
        {
            UTparallelFor(groupParser.getSplittableRange(attrib), [=](const GA_SplittableRange& r)
            {
                GFE_RWPageHandleT<GFE_Type::storeTupleSizeValueType_t<tupleSizeVariant, storeVariant>> attrib_ph(attrib);
                GA_Offset start, end;
                for (GA_PageIterator pit = groupParser.getSplittableRange(attrib).beginPages(); !pit.atEnd(); ++pit)
                {
                    for (GA_Iterator it(pit.begin()); it.blockAdvance(start, end); )
                    {
                        attrib_ph.setPage(start);
                        for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                        {
                            if constexpr (GFE_Type::isVector<GFE_Type::storeTupleSizeValueType_t<tupleSizeVariant, storeVariant>>)
                            {
                                if constexpr (doNormalizeVariant)
                                    attrib_ph.value(elemoff).normalize();
                            }
                            if constexpr (scaleVariant)
                                attrib_ph.value(elemoff) *= uniScale;
                        }
                    }
                }
            }, subscribeRatio, minGrainSize);
        }, scaleVariant, doNormalizeVariant);
    }

    
    template<typename _Ty>
    void scaleAttribElement()
    {
        auto doNormalizeVariant = GFE_Type::getBoolVariant(doNormalize);
        auto scaleVariant = GFE_Type::getBoolVariant(uniScale != 1.0);
        
        std::visit([this] (auto scaleVariant, auto doNormalizeVariant)
        {
            UTparallelFor(groupParser.getSplittableRange(attrib), [=](const GA_SplittableRange& r)
            {
                GFE_RWPageHandleT<_Ty> attrib_ph(attrib);
                GA_Offset start, end;
                for (GA_PageIterator pit = groupParser.getSplittableRange(attrib).beginPages(); !pit.atEnd(); ++pit)
                {
                    for (GA_Iterator it(pit.begin()); it.blockAdvance(start, end); )
                    {
                        attrib_ph.setPage(start);
                        for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                        {
                            if constexpr (GFE_Type::isVector<_Ty>)
                            {
                                if constexpr (doNormalizeVariant)
                                    attrib_ph.value(elemoff).normalize();
                            }
                            if constexpr (scaleVariant)
                                attrib_ph.value(elemoff) *= uniScale;
                        }
                    }
                }
            }, subscribeRatio, minGrainSize);
        }, scaleVariant, doNormalizeVariant);
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





