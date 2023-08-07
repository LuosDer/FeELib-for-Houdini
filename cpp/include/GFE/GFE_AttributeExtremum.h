
#pragma once

#ifndef __GFE_AttributeExtremum_h__
#define __GFE_AttributeExtremum_h__

#include "GFE/GFE_AttributeExtremum.h"

#include "GFE/GFE_GeoFilter.h"



class GFE_AttribExtremum : public GFE_AttribFilterWithRef0 {

public:

    using GFE_AttribFilterWithRef0::GFE_AttribFilterWithRef0;

    
    ~GFE_AttribExtremum()
    {
    }


void
    setComputeParm(
        const bool outAsOffset = true,
        const exint subscribeRatio = 64,
        const exint minGrainSize   = 1024
    )
{
    setHasComputed();
    this->outAsOffset = outAsOffset;
    this->subscribeRatio = subscribeRatio;
    this->minGrainSize   = minGrainSize;
}

    
    const GA_Attribute* setAttrib(const GA_AttributeOwner owner, const UT_StringRef& name)
    { return attrib = geo->findAttribute(owner, name); }
    
    void setAttrib(const GA_Attribute* const inAttrib)
    { attrib = inAttrib; }
    

    
private:

    virtual bool
        computeCore() override
    {
        if (!attrib)
            return false;
        
        if (!outAttribMin && !outAttribMax && !outAttribMinElemnum && !outAttribMaxElemnum)
            return false;
            
        if (groupParser.isEmpty())
            return true;

        attribExtremum();
        
        if (outAsOffset)
        {
            if (GFE_Type::isValidOffset(attribMinElemoff))
            {
                UT_ASSERT_P(GFE_Type::isValidOffset(attrib->getIndexMap(), attribMinElemoff));
                attribMinElemoff = attrib->getIndexMap().indexFromOffset(attribMinElemoff);
            }
            if (GFE_Type::isValidOffset(attribMaxElemoff))
            {
                UT_ASSERT_P(GFE_Type::isValidOffset(attrib->getIndexMap(), attribMaxElemoff));
                attribMaxElemoff = attrib->getIndexMap().indexFromOffset(attribMaxElemoff);
            }
        }
        
        
        return true;
    }


    void attribExtremum()
    {
        if (attrib->getAIFTuple())
        {
            auto tupleTypeVariant = GFE_Variant::getAttribStorageVariantIFVM(*attrib);
            auto outAttribMinVariant = GFE_Variant::getBoolVariant(outAttribMin);
            auto outAttribMaxVariant = GFE_Variant::getBoolVariant(outAttribMax);
            auto outAttribMinElemnumVariant = GFE_Variant::getBoolVariant(outAttribMinElemnum);
            auto outAttribMaxElemnumVariant = GFE_Variant::getBoolVariant(outAttribMaxElemnum);
            std::visit([&] (auto tupleTypeVariant,
                            auto outAttribMinVariant,
                            auto outAttribMaxVariant,
                            auto outAttribMinElemnumVariant,
                            auto outAttribMaxElemnumVariant)
            {
                using type = typename GFE_Variant::getAttribStorage_t<tupleTypeVariant>;
                
                type min;
                type max;
                GFE_Attribute::computeAttribExtremum<type,
                                                     outAttribMinVariant,
                                                     outAttribMaxVariant,
                                                     outAttribMinElemnumVariant,
                                                     outAttribMaxElemnumVariant>
                    (attrib, groupParser.getSplittableRange(attrib), min, max, attribMinElemoff, attribMaxElemoff, subscribeRatio, minGrainSize);
                
            }, tupleTypeVariant, outAttribMinVariant, outAttribMaxVariant, outAttribMinElemnumVariant, outAttribMaxElemnumVariant);
        }
    }
    


public:
    bool outAttribMin = false;
    bool outAttribMax = false;
    bool outAttribMinElemnum = false;
    bool outAttribMaxElemnum = false;
    bool outAsOffset = true;
    

private:
    const GA_Attribute* attrib = nullptr;
    GA_Attribute* attribMin = nullptr;
    GA_Attribute* attribMax = nullptr;
    GA_Offset attribMinElemoff = GFE_INVALID_OFFSET;
    GA_Offset attribMaxElemoff = GFE_INVALID_OFFSET;
    
    exint subscribeRatio = 64;
    exint minGrainSize   = 1024;

    
};








#endif





