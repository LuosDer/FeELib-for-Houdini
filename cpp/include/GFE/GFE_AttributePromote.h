
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
            const exint minGrainSize = 1024
        )
    {
        setHasComputed();

        this->subscribeRatio = subscribeRatio;
        this->minGrainSize = minGrainSize;
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


    
#if 1
    
    
    SYS_FORCE_INLINE void groupPromote()
    { GFE_GroupUnion::groupUnion(*dstGroup, *srcGroup); }
    
    //template<GA_GroupType SrcOwner, GA_GroupType dstOwner>
    //void groupPromote()
    //{
    //    UTparallelFor(groupParser.getSplittableRange(dstOwner), [this](const GA_SplittableRange& r)
    //    {
    //        GA_Offset start, end;
    //        for (GA_Iterator it(r); it.blockAdvance(start, end); )
    //        {
    //            for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
    //            {
    //                dstGroup->setElement(elemoff, srcGroup->contains(geoTmp->offsetPromote<SrcOwner, dstOwner>(elemoff)));
    //            }
    //        }
    //    }, subscribeRatio, minGrainSize);
    //}
    
    template<GA_AttributeOwner SrcOwner, GA_AttributeOwner dstOwner>
    void attribPromote()
    {
        UT_ASSERT_P(&srcAttrib->getDetail() == geoTmp);
        const GA_Attribute& srcAttribRef = *srcAttrib;

        if (geoRef0)
        {
            const GA_IndexMap& indexMapSrc = geoTmp->getIndexMap(SrcOwner);
            UTparallelFor(groupParser.getSplittableRange(dstOwner), [this, &srcAttribRef, &indexMapSrc](const GA_SplittableRange& r)
            {
                GA_Offset start, end;
                for (GA_Iterator it(r); it.blockAdvance(start, end); )
                {
                    for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                    {
                        const GA_Offset dstOff = geoTmp->offsetPromote<dstOwner, SrcOwner>(elemoff);
                        if (GFE_Type::isValidOffset(indexMapSrc, dstOff))
                            dstAttrib->copy(elemoff, srcAttribRef, dstOff);
                    }
                }
            }, subscribeRatio, minGrainSize);
        }
        else
        {
            UTparallelFor(groupParser.getSplittableRange(dstOwner), [this, &srcAttribRef](const GA_SplittableRange& r)
            {
                GA_Offset start, end;
                for (GA_Iterator it(r); it.blockAdvance(start, end); )
                {
                    for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                    {
                        const GA_Offset dstOff = geoTmp->offsetPromote<dstOwner, SrcOwner>(elemoff);
                        if (GFE_Type::isValidOffset(dstOff))
                            dstAttrib->copy(elemoff, srcAttribRef, dstOff);
                    }
                }
            }, subscribeRatio, minGrainSize);
        }
    }
    
    template<typename FLOAT_T, GA_AttributeOwner SrcOwner, GA_AttributeOwner dstOwner>
    void attribPromote()
    {
        const GA_ROHandleT<FLOAT_T> srcAttrib_h(srcAttrib);

        UTparallelFor(groupParser.getSplittableRange(dstOwner), [this, &srcAttrib_h](const GA_SplittableRange& r)
        {
            GA_PageHandleT<FLOAT_T, FLOAT_T, true, true, GA_Attribute, GA_ATINumeric, GA_Detail> attrib_ph(dstAttrib);
            for (GA_PageIterator pit = r.beginPages(); !pit.atEnd(); ++pit)
            {
                GA_Offset start, end;
                for (GA_Iterator it(pit.begin()); it.blockAdvance(start, end); )
                {
                    attrib_ph.setPage(start);
                    for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                    {
                        const GA_Offset dstOff = geoTmp->offsetPromote<dstOwner, SrcOwner>(elemoff);
                        if (GFE_Type::isValidOffset(dstOff))
                            attrib_ph.value(elemoff) = srcAttrib_h.get(dstOff);
                    }
                }
            }
        }, subscribeRatio, minGrainSize);
    }
    
    template<GA_AttributeOwner SrcOwner>
    void attribPromote()
    {
        switch (dstAttrib->getOwner())
        {
        case GA_ATTRIB_PRIMITIVE: attribPromote<SrcOwner, GA_ATTRIB_PRIMITIVE>(); return; break;
        case GA_ATTRIB_POINT:     attribPromote<SrcOwner, GA_ATTRIB_POINT>();     return; break;
        case GA_ATTRIB_VERTEX:    attribPromote<SrcOwner, GA_ATTRIB_VERTEX>();    return; break;
        case GA_ATTRIB_GLOBAL:    attribPromote<SrcOwner, GA_ATTRIB_GLOBAL>();    return; break;
        }
        UT_ASSERT_MSG(0, "Unhandled Dst Attrib Owner");
    }

    void attribPromote()
    {
        switch (srcAttrib->getOwner())
        {
        case GA_ATTRIB_PRIMITIVE: attribPromote<GA_ATTRIB_PRIMITIVE>(); return; break;
        case GA_ATTRIB_POINT:     attribPromote<GA_ATTRIB_POINT>();     return; break;
        case GA_ATTRIB_VERTEX:    attribPromote<GA_ATTRIB_VERTEX>();    return; break;
        case GA_ATTRIB_GLOBAL:    attribPromote<GA_ATTRIB_GLOBAL>();    return; break;
        }
        UT_ASSERT_MSG(0, "Unhandled Src Attrib Owner");
    }



    
#else

    
    void attribPromote()
    {
        const GA_Attribute& srcAttrib = *srcAttrib;
        srcOwner = srcAttrib->getOwner();

        UTparallelFor(GA_SplittableRange(GA_Range(dstAttrib->getIndexMap())), [this, &srcAttrib](const GA_SplittableRange& r)
        {
            GA_Offset start, end;
            for (GA_Iterator it(r); it.blockAdvance(start, end); )
            {
                for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                {
                    dstAttrib->copy(elemoff, srcAttrib, offsetPromote(elemoff));
                }
            }
        }, subscribeRatio, minGrainSize);
    }


    GA_Offset offsetPromote(const GA_Offset elemoff)
    {
        //const GA_Topology& topo = geoTmp->getTopology();
        switch (dstOwner)
        {
        case GA_ATTRIB_PRIMITIVE:
        switch (srcOwner)
        {
            case GA_ATTRIB_PRIMITIVE: return elemoff;                   break;
            case GA_ATTRIB_POINT:     return geoTmp->primPoint(elemoff);   break;
            case GA_ATTRIB_VERTEX:    return geoTmp->primVertex(elemoff);  break;
            default: UT_ASSERT_MSG(0, "Unhandled newClass!");           break;
        }
        break;
        case GA_ATTRIB_POINT:
        switch (srcOwner)
        {
            case GA_ATTRIB_PRIMITIVE: return geoTmp->pointPrim(elemoff);   break;
            case GA_ATTRIB_POINT:     return elemoff;                   break;
            case GA_ATTRIB_VERTEX:    return geoTmp->pointVertex(elemoff); break;
            default: UT_ASSERT_MSG(0, "Unhandled newClass!");           break;
        }
        break;
        case GA_ATTRIB_VERTEX:
        switch (srcOwner)
        {
            case GA_ATTRIB_PRIMITIVE: return geoTmp->vertexPrimitive(elemoff); break;
            case GA_ATTRIB_POINT:     return geoTmp->vertexPoint(elemoff);     break;
            case GA_ATTRIB_VERTEX:    return elemoff;                       break;
            default: UT_ASSERT_MSG(0, "Unhandled newClass!");               break;
        }
        break;
        default: UT_ASSERT_MSG(0, "Unhandled Owner!"); break;
        }
        return GFE_INVALID_OFFSET;
    }


        
#endif




public:
    GA_AttributeOwner dstAttribClass = GA_ATTRIB_INVALID;
    GA_GroupType dstGroupClass = GA_GROUP_INVALID;
        
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
    exint minGrainSize = 1024;

}; // End of class GFE_AttribPromote








#endif
