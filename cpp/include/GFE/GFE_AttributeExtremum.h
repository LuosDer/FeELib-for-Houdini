
#pragma once

#ifndef __GFE_AttributeExtremum_h__
#define __GFE_AttributeExtremum_h__

#include "GFE/GFE_AttributeExtremum.h"

#include "GFE/GFE_GeoFilter.h"

#if 0
    GFE_AttribExtremum attribExtremum(geo, inGeo1, cookparms);
    
    attribExtremum.outas(sopparms.getOutAsOffset(),
        sopparms.getSubscribeRatio(), sopparms.getMinGrainSize());
        
    attribExtremum.setAttrib(attribClass, sopparms.getAttrib());
        
    attribExtremum.groupParser.setGroup(groupType, sopparms.getGroup());
        
    attribExtremum.computeAndBumpDataId();

#endif

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
        const GFE_AttribStorage attribStorage = GFE_Type::getAttribStorage(attrib);
        if (!GFE_Variant::isAttribStorageIFV(attribStorage))
            return;
        auto storageVariant = GFE_Variant::getAttribStorageVariantIFV(attribStorage);
        auto outAttribMinVariant = GFE_Variant::getBoolVariant(outAttribMin);
        auto outAttribMaxVariant = GFE_Variant::getBoolVariant(outAttribMax);
        auto outAttribMinElemnumVariant = GFE_Variant::getBoolVariant(outAttribMinElemnum);
        auto outAttribMaxElemnumVariant = GFE_Variant::getBoolVariant(outAttribMaxElemnum);
        std::visit([&] (auto storageVariant,
                        auto outAttribMinVariant,
                        auto outAttribMaxVariant,
                        auto outAttribMinElemnumVariant,
                        auto outAttribMaxElemnumVariant)
        {
            using type = typename GFE_Variant::getAttribStorage_t<storageVariant>;
            using value_type = typename GFE_Type::get_value_type_t<type>;
                
            value_type minVal;
            value_type maxVal;
            GFE_Attribute::computeAttribExtremum<type,
                                                 outAttribMinVariant,
                                                 outAttribMaxVariant,
                                                 outAttribMinElemnumVariant,
                                                 outAttribMaxElemnumVariant>
                (attrib, groupParser.getSplittableRange(attrib), minVal, maxVal, attribMinElemoff, attribMaxElemoff, subscribeRatio, minGrainSize);

            if constexpr (std::is_floating_point_v<type>)
            {
                min.f = minVal;
                max.f = maxVal;
            }
            else
            {
                min.i = minVal;
                max.i = maxVal;
            }
            if constexpr (outAttribMinVariant)
                setAttribValue(attribMin, minVal);
            if constexpr (outAttribMaxVariant)
                setAttribValue(attribMax, maxVal);
            if constexpr (outAttribMinElemnumVariant)
                setAttribValue(attribMinElemnum, attribMinElemoff);
            if constexpr (outAttribMaxElemnumVariant)
                setAttribValue(attribMaxElemnum, attribMaxElemoff);
        }, storageVariant, outAttribMinVariant, outAttribMaxVariant, outAttribMinElemnumVariant, outAttribMaxElemnumVariant);
    }

    template<typename _Ty>
    void setAttribValue(GA_Attribute* const attrib, const _Ty val)
    {
        UT_ASSERT_P(attrib);
        const GA_AIFTuple* const aIFTuple = attrib->getAIFTuple();
        if (attrib && aIFTuple)
        {
            aIFTuple->set(attrib, 0, val);
        }
    }


    //union minU
    //{
    //    fpreal f;
    //    exint  i;
    //};
    //union maxU
    //{
    //    fpreal f;
    //    exint  i;
    //};

    
public:
    bool outAttribMin = false;
    bool outAttribMax = false;
    bool outAttribMinElemnum = false;
    bool outAttribMaxElemnum = false;
    
    bool outAsOffset = true;

    //minU min;
    //maxU max;
    std::variant<fpreal, exint> min;
    std::variant<fpreal, exint> max;
    GA_Attribute* attribMin = nullptr;
    GA_Attribute* attribMax = nullptr;
    GA_Attribute* attribMinElemnum = nullptr;
    GA_Attribute* attribMaxElemnum = nullptr;
    //const char* outAttribMinAttribName = nullptr;
    //const char* outAttribMaxAttribName = nullptr;
    //const char* outAttribMinElemnumAttribName = nullptr;
    //const char* outAttribMaxElemnumAttribName = nullptr;

private:
    const GA_Attribute* attrib = nullptr;
    GA_Offset attribMinElemoff = GFE_INVALID_OFFSET;
    GA_Offset attribMaxElemoff = GFE_INVALID_OFFSET;
    
    exint subscribeRatio = 64;
    exint minGrainSize   = 1024;

    
};








#endif





