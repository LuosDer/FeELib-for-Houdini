
#pragma once

#ifndef __GFE_AttributePromote_h__
#define __GFE_AttributePromote_h__

#include "GFE/GFE_AttributePromote.h"

#include "GFE/GFE_GeoFilter.h"



#include "UFE/UFE_SplittableString.h"


class GFE_AttribPromote : public GFE_AttribCreateFilterWithRef0 {

public:
    using GFE_AttribCreateFilterWithRef0::GFE_AttribCreateFilterWithRef0;

    void setComputeParm(
            const exint subscribeRatio = 64,
            const exint minGrainSize   = 1024
        )
    {
        setHasComputed();

        this->subscribeRatio = subscribeRatio;
        this->minGrainSize   = minGrainSize;
    }



private:


    virtual bool
        computeCore() override
    {
        if (geoRef0 ? getRef0AttribArray().isEmpty() && getRef0GroupArray().isEmpty() : getInAttribArray().isEmpty() && getInGroupArray().isEmpty())
            return true;

        if (groupParser.isEmpty())
            return true;
        
        geoTmp = geoRef0 ? geoRef0 : geo;

        const size_t sizeInAttrib  = geoRef0 ? getRef0AttribArray().size() : getInAttribArray().size();
        const size_t sizeOutAttrib = getOutAttribArray().size();
        if (sizeOutAttrib > 0)
        {
            const size_t attribSize = SYSmin(sizeInAttrib, sizeOutAttrib);
            for (size_t i = 0; i < attribSize; ++i)
            {
                srcAttrib = geoRef0 ? getRef0AttribArray()[i] : getInAttribArray()[i];
                dstAttrib = getOutAttribArray()[i];
                
                if (GFE_Attribute::isSameType(*srcAttrib, *dstAttrib))
                    attribPromote();
            }
        }
        else
        {
            for (size_t i = 0; i < sizeInAttrib; ++i)
            {
                srcAttrib = geoRef0 ? getRef0AttribArray()[i] : getInAttribArray()[i];
                if (srcAttrib->getOwner() == dstAttribClass)
                    continue;
            
                const UT_StringHolder& newName = newAttribNames.getIsValid() ? newAttribNames.getNext<UT_StringHolder>() : srcAttrib->getName();
                const bool detached = GFE_Type::isInvalid(newName);
                
                dstAttrib = getOutAttribArray().clone(detached, *srcAttrib, dstAttribClass, newName);
    
                attribPromote();
            }
        }
        
        
        
        const size_t sizeInGroup  = geoRef0 ? getRef0GroupArray().size() : getInGroupArray().size();
        const size_t sizeOutGroup = getOutGroupArray().size();
        if (sizeOutGroup > 0)
        {
            const size_t groupSize = SYSmin(sizeInGroup, sizeOutGroup);
            for (size_t i = 0; i < groupSize; ++i)
            {
                srcGroup = geoRef0 ? getRef0GroupArray()[i] : getInGroupArray()[i];
                dstGroup = getOutGroupArray()[i];
                
                groupPromote();
            }
        }
        else
        {
            for (size_t i = 0; i < sizeInGroup; ++i)
            {
                srcGroup = geoRef0 ? getRef0GroupArray()[i] : getInGroupArray()[i];
                if (srcGroup->classType() == dstGroupClass)
                    continue;
            
                const UT_StringHolder& newName = newGroupNames.getIsValid() ? newGroupNames.getNext<UT_StringHolder>() : srcGroup->getName();
                const bool detached = GFE_Type::isInvalid(newName);
                
                dstGroup = getOutGroupArray().findOrCreate(detached, dstGroupClass, newName);
    
                groupPromote();
            }
        }
        
        return true;
    }


    
    SYS_FORCE_INLINE void groupPromote()
    {
        if (dstGroup->classType() == GA_GROUP_VERTEX && srcGroup->classType() == GA_GROUP_EDGE)
        {
            GFE_GroupUnion::groupUnion(reinterpret_cast<GA_VertexGroup&>(*dstGroup),
                                       reinterpret_cast<const GA_EdgeGroup&>(*srcGroup),
                                       vertexEdgeConnectElemType, reverseGroup);
        }
        else
            GFE_GroupUnion::groupUnion(*dstGroup, *srcGroup, reverseGroup, subscribeRatio, minGrainSize);
    }
    
    
    template<GA_AttributeOwner SrcOwner, GA_AttributeOwner dstOwner>
    void attribPromote()
    {
        UT_ASSERT_P(&srcAttrib->getDetail() == geoTmp);
        UT_ASSERT_P(!srcAttrib->getAIFTuple());

        const GA_Attribute& srcAttribRef = *srcAttrib;
    
        if (geoRef0)
        {
            const GA_IndexMap& indexMapSrc = geoRef0->getIndexMap(SrcOwner);
            UTparallelFor(groupParser.getSplittableRange<dstOwner>(), [this, &srcAttribRef, &indexMapSrc](const GA_SplittableRange& r)
            {
                GA_Offset start, end;
                for (GA_Iterator it(r); it.blockAdvance(start, end); )
                {
                    for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                    {
                        const GA_Offset srcOff = geo->offsetPromote<dstOwner, SrcOwner>(elemoff);
                        if (GFE_Type::isValidOffset(indexMapSrc, srcOff))
                            dstAttrib->copy(elemoff, srcAttribRef, srcOff);
                    }
                }
            }, subscribeRatio, minGrainSize);
        }
        else
        {
            UTparallelFor(groupParser.getSplittableRange<dstOwner>(), [this, &srcAttribRef](const GA_SplittableRange& r)
            {
                GA_Offset start, end;
                for (GA_Iterator it(r); it.blockAdvance(start, end); )
                {
                    for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                    {
                        const GA_Offset srcOff = geo->offsetPromote<dstOwner, SrcOwner>(elemoff);
                        if (GFE_Type::isValidOffset(srcOff))
                            dstAttrib->copy(elemoff, srcAttribRef, srcOff);
                    }
                }
            }, subscribeRatio, minGrainSize);
        }
    }
    
    template<typename _Ty, GA_AttributeOwner SrcOwner, GA_AttributeOwner dstOwner>
    void attribPromote()
    {
        const GA_ROHandleT<_Ty> srcAttrib_h(srcAttrib);

        if (geoRef0)
        {
            const GA_IndexMap& indexMap = srcAttrib->getIndexMap();
            UTparallelFor(groupParser.getSplittableRange<dstOwner>(), [this, &srcAttrib_h, &indexMap](const GA_SplittableRange& r)
            {
                GFE_RWPageHandleT<_Ty> attrib_ph(dstAttrib);
                GA_Offset start, end;
                for (GA_PageIterator pit = r.beginPages(); !pit.atEnd(); ++pit)
                {
                    for (GA_Iterator it(pit.begin()); it.blockAdvance(start, end); )
                    {
                        attrib_ph.setPage(start);
                        for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                        {
                            const GA_Offset srcOff = geo->offsetPromote<dstOwner, SrcOwner>(elemoff);
                            if (GFE_Type::isValidOffset(indexMap, srcOff))
                                attrib_ph.value(elemoff) = srcAttrib_h.get(srcOff);
                        }
                    }
                }
            }, subscribeRatio, minGrainSize);
        }
        else
        {
            UTparallelFor(groupParser.getSplittableRange<dstOwner>(), [this, &srcAttrib_h](const GA_SplittableRange& r)
            {
                GFE_RWPageHandleT<_Ty> attrib_ph(dstAttrib);
                GA_Offset start, end;
                for (GA_PageIterator pit = r.beginPages(); !pit.atEnd(); ++pit)
                {
                    for (GA_Iterator it(pit.begin()); it.blockAdvance(start, end); )
                    {
                        attrib_ph.setPage(start);
                        for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                        {
                            const GA_Offset srcOff = geo->offsetPromote<dstOwner, SrcOwner>(elemoff);
                            if (GFE_Type::isValidOffset(srcOff))
                                attrib_ph.value(elemoff) = srcAttrib_h.get(srcOff);
                        }
                    }
                }
            }, subscribeRatio, minGrainSize);
        }
    }
    
    
    template<typename _Ty, GA_AttributeOwner SrcOwner>
    void attribPromote()
    {
        auto storageVariant = GFE_Type::attribStorageVariant();
        
        switch (dstAttrib->getOwner())
        {
        case GA_ATTRIB_PRIMITIVE: attribPromote<_Ty, SrcOwner, GA_ATTRIB_PRIMITIVE>(); return; break;
        case GA_ATTRIB_POINT:     attribPromote<_Ty, SrcOwner, GA_ATTRIB_POINT    >(); return; break;
        case GA_ATTRIB_VERTEX:    attribPromote<_Ty, SrcOwner, GA_ATTRIB_VERTEX   >(); return; break;
        case GA_ATTRIB_GLOBAL:    attribPromote<_Ty, SrcOwner, GA_ATTRIB_GLOBAL   >(); return; break;
        }
        UT_ASSERT_MSG(0, "Unhandled Dst Attrib Owner");
    }

    template<typename _Ty>
    void attribPromote()
    {
        switch (srcAttrib->getOwner())
        {
        case GA_ATTRIB_PRIMITIVE: attribPromote<_Ty, GA_ATTRIB_PRIMITIVE>(); return; break;
        case GA_ATTRIB_POINT:     attribPromote<_Ty, GA_ATTRIB_POINT    >(); return; break;
        case GA_ATTRIB_VERTEX:    attribPromote<_Ty, GA_ATTRIB_VERTEX   >(); return; break;
        case GA_ATTRIB_GLOBAL:    attribPromote<_Ty, GA_ATTRIB_GLOBAL   >(); return; break;
        }
        UT_ASSERT_MSG(0, "Unhandled Src Attrib Owner");
    }

    template<GA_AttributeOwner SrcOwner>
    void attribPromote()
    {
        switch (dstAttrib->getOwner())
        {
        case GA_ATTRIB_PRIMITIVE: attribPromote<SrcOwner, GA_ATTRIB_PRIMITIVE>(); return; break;
        case GA_ATTRIB_POINT:     attribPromote<SrcOwner, GA_ATTRIB_POINT    >(); return; break;
        case GA_ATTRIB_VERTEX:    attribPromote<SrcOwner, GA_ATTRIB_VERTEX   >(); return; break;
        case GA_ATTRIB_GLOBAL:    attribPromote<SrcOwner, GA_ATTRIB_GLOBAL   >(); return; break;
        }
        UT_ASSERT_MSG(0, "Unhandled Dst Attrib Owner");
    }

    void attribPromote()
    {
        UT_ASSERT_P(&srcAttrib->getDetail() == geoTmp);
        
        auto storageVariant = GFE_Type::getAttribStorageVariant(attrib);
        auto dstOwnerVariant = GFE_Type::getAttribOwnerVariant(dstAttrib);
        auto srcOwnerVariant = GFE_Type::getAttribOwnerVariant(srcAttrib);
        std::visit([&] (auto storageVariant, auto dstOwnerVariant, auto srcOwnerVariant, auto doNormalizeVariant)
        {
            scaleAttribElement<storageVariant, dstOwnerVariant, srcOwnerVariant>
        },  storageVariant, dstOwnerVariant, srcOwnerVariant);

        
        const GA_AIFTuple* const aifTuple = srcAttrib->getAIFTuple();
        if (aifTuple)
        {
            const GA_Storage storage = aifTuple->getStorage(srcAttrib);
            GFE_ForEachStorageTupleSizeVecMtx(attribPromote, aifTuple->getStorage(srcAttrib), srcAttrib->getTupleSize())
        }
        else
        {
            switch (srcAttrib->getOwner())
            {
            case GA_ATTRIB_PRIMITIVE: attribPromote<GA_ATTRIB_PRIMITIVE>(); return; break;
            case GA_ATTRIB_POINT:     attribPromote<GA_ATTRIB_POINT    >(); return; break;
            case GA_ATTRIB_VERTEX:    attribPromote<GA_ATTRIB_VERTEX   >(); return; break;
            case GA_ATTRIB_GLOBAL:    attribPromote<GA_ATTRIB_GLOBAL   >(); return; break;
            }
            UT_ASSERT_MSG(0, "Unhandled Src Attrib Owner");
        }
    }

        



public:
    GA_AttributeOwner dstAttribClass = GA_ATTRIB_INVALID;
    GA_GroupType dstGroupClass = GA_GROUP_INVALID;
        
    GA_AttributeOwner vertexEdgeConnectElemType = GA_ATTRIB_OWNER_N;
    bool reverseGroup = false;
    
    UFE_SplittableString newAttribNames;
    UFE_SplittableString newGroupNames;

private:
    //GA_AttributeOwner dstAttribClassMid = GA_ATTRIB_INVALID;
    //GA_GroupType dstGroupClassMid = GA_GROUP_INVALID;
        
    const GFE_Detail* geoTmp;
    const GA_Attribute* srcAttrib;
    const GA_Group* srcGroup;
        
    GA_Attribute* dstAttrib;
    GA_Group* dstGroup;
    //GA_AttributeOwner srcOwner;

    exint subscribeRatio = 64;
    exint minGrainSize   = 1024;

}; // End of class GFE_AttribPromote








#endif
