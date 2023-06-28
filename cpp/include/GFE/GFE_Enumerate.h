
#pragma once

#ifndef __GFE_Enumerate_h__
#define __GFE_Enumerate_h__

#include "GFE/GFE_Enumerate.h"

#include "GFE/GFE_GeoFilter.h"


#include "SOP/SOP_Enumerate.proto.h"

class GFE_Enumerate : public GFE_AttribFilter {


public:
    using GFE_AttribFilter::GFE_AttribFilter;


    void
        setComputeParm(
            const GA_Size firstIndex   = 0,
            const bool negativeIndex   = false,
            const bool outAsOffset     = true,
            const exint subscribeRatio = 64,
            const exint minGrainSize   = 64
        )
    {
        setHasComputed();
        this->firstIndex     = firstIndex;
        this->negativeIndex  = negativeIndex;
        this->outAsOffset    = outAsOffset;
        this->subscribeRatio = subscribeRatio;
        this->minGrainSize   = minGrainSize;
    }

    
    SYS_FORCE_INLINE GA_Attribute* findOrCreateTuple(
        const bool detached = true,
        const GA_AttributeOwner owner = GA_ATTRIB_POINT,
        const GA_StorageClass storageClass = GA_STORECLASS_INT,
        const GA_Storage storage = GA_STORE_INVALID,
        const UT_StringRef& attribName = ""
    )
    { return getOutAttribArray().findOrCreateTuple(detached, owner,
            storageClass, storage, attribName, 1, GA_Defaults(GFE_INVALID_OFFSET)); }

    SYS_FORCE_INLINE GA_Attribute* findOrCreateTuple(
        const bool detached,
        const GA_AttributeOwner owner,
        const UT_StringRef& attribName
    )
    { return getOutAttribArray().findOrCreateTuple(detached, owner,
            GA_STORECLASS_INT, GA_STORE_INVALID, attribName, 1, GA_Defaults(GFE_INVALID_OFFSET)); }

private:

    virtual bool
        computeCore() override
    {
        if (getOutAttribArray().isEmpty())
            return false;

        if (groupParser.isEmpty())
            return true;
        
        if (pieceAttrib)
        {
            enumerateSideFX();
            return true;
        }
        
        
        const size_t size = getOutAttribArray().size();
        for (size_t i = 0; i < size; i++)
        {
            enumAttrib = getOutAttribArray()[i];
            // const GA_Storage storage = enumAttrib->getAIFTuple()->getStorage(enumAttrib);
            switch (enumAttrib->getAIFTuple()->getStorage(enumAttrib))
            {
            case GA_STORE_INT16:  enumerate<int16>();           break;
            case GA_STORE_INT32:  enumerate<int32>();           break;
            case GA_STORE_INT64:  enumerate<int64>();           break;
            case GA_STORE_REAL16: enumerate<fpreal16>();        break;
            case GA_STORE_REAL32: enumerate<fpreal32>();        break;
            case GA_STORE_REAL64: enumerate<fpreal64>();        break;
            case GA_STORE_STRING: enumerate<UT_StringHolder>(); break;
            default: break;
            }
        }
        return true;
    }


#define __TEMP_GFE_Enumerate_GroupName       "__TEMP_GFE_Enumerate_Group"
#define __TEMP_GFE_Enumerate_PieceAttribName "__TEMP_GFE_Enumerate_PieceAttrib"
#define __TEMP_GFE_Enumerate_OutAttribName   "__TEMP_GFE_Enumerate_OutAttrib"
    
    void enumerateSideFX()
    {
        enumAttrib = getOutAttribArray()[0];
        UT_ASSERT_P(pieceAttrib);
        UT_ASSERT_P(enumAttrib);
        UT_ASSERT_MSG(pieceAttrib->getOwner() == enumAttrib->getOwner(), "not same owner");

        GA_ElementGroup* elemGroup = nullptr;
        const GA_Group* const group = groupParser.getGroup(pieceAttrib);
        if (group)
        {
            if (group->isDetached())
            {
                elemGroup = geo->createElementGroup(pieceAttrib->getOwner(), __TEMP_GFE_Enumerate_GroupName);
                elemGroup->combine(group);
                enumParms.setGroup(__TEMP_GFE_Enumerate_GroupName);
            }
            else
            {
                enumParms.setGroup(group->getName());
            }
        }
        else
        {
            enumParms.setGroup();
        }

        GA_Attribute* namedPieceAttrib = nullptr;
        if (pieceAttrib->isDetached())
        {
            namedPieceAttrib = GFE_Attribute::clone(geo, pieceAttrib, __TEMP_GFE_Enumerate_PieceAttribName);
            enumParms.setPieceAttrib(__TEMP_GFE_Enumerate_PieceAttribName);
        }
        else
        {
            enumParms.setPieceAttrib(pieceAttrib->getName());
        }
        
        enumParms.setGroupType(GFE_Type::attributeOwner_groupType(enumAttrib->getOwner()));
        enumParms.setUsePieceAttrib(true);
        enumParms.setPieceMode(enumeratePieceElem ? SOP_EnumerateEnums::PieceMode::ELEMENTS : SOP_EnumerateEnums::PieceMode::PIECES);
        
        GA_Attribute* outAttrib = nullptr;
        if (enumAttrib->isDetached() || enumAttrib->getName() == pieceAttrib->getName())
        {
            outAttrib = GFE_Attribute::clone(geo, enumAttrib, __TEMP_GFE_Enumerate_OutAttribName);
            enumParms.setAttribname(__TEMP_GFE_Enumerate_OutAttribName);
        }
        else
        {
            enumParms.setAttribname(outAttrib->getName());
        }
        //enumParms.setAttribType(SOP_EnumerateEnums::AttribType::INT);
        //enumParms.setPrefix("");
        
        destgdh.allocateAndSet(geo, false);
        
        inputgdh.clear();
        GU_DetailHandle input_h;
        input_h.allocateAndSet(geoSrc ? geoSrc : geo, false);
        inputgdh.emplace_back(input_h);
        
        SOP_NodeCache* const nodeCache = enumVerb->allocCache();
        const auto enumCookparms = GFE_NodeVerb::newCookParms(cookparms, enumParms, nodeCache, &destgdh, &inputgdh);
        enumVerb->cook(enumCookparms);
        
        
        geo->destroyElementGroup(elemGroup);
        geo->destroyAttrib(namedPieceAttrib);
    }

#undef __TEMP_GFE_Enumerate_GroupName
#undef __TEMP_GFE_Enumerate_PieceAttribName
#undef __TEMP_GFE_Enumerate_OutAttribName

    
    template<typename T>
    void enumerate()
    {
        UT_ASSERT_P(enumAttrib);
        const GA_SplittableRange geoSplittableRange0(groupParser.getRange(enumAttrib));
        if(outAsOffset)
        {
            UTparallelFor(geoSplittableRange0, [this](const GA_SplittableRange& r)
            {
                GA_PageHandleT<T, T, true, true, GA_Attribute, GA_ATINumeric, GA_Detail> attrib_ph(enumAttrib);
                for (GA_PageIterator pit = r.beginPages(); !pit.atEnd(); ++pit)
                {
                    GA_Offset start, end;
                    for (GA_Iterator it(pit.begin()); it.blockAdvance(start, end); )
                    {
                        attrib_ph.setPage(start);
                        for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                        {
                            attrib_ph.value(elemoff) = firstIndex + elemoff;
                        }
                    }
                }
            }, subscribeRatio, minGrainSize);
        }
        else
        {
            const GA_IndexMap& indexMap = enumAttrib->getIndexMap();
            if (negativeIndex)
            {
                UTparallelFor(geoSplittableRange0, [this, &indexMap](const GA_SplittableRange& r)
                {
                    GA_PageHandleT<T, T, true, true, GA_Attribute, GA_ATINumeric, GA_Detail> attrib_ph(enumAttrib);
                    for (GA_PageIterator pit = r.beginPages(); !pit.atEnd(); ++pit)
                    {
                        GA_Offset start, end;
                        for (GA_Iterator it(pit.begin()); it.blockAdvance(start, end); )
                        {
                            attrib_ph.setPage(start);
                            for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                            {
                                attrib_ph.value(elemoff) = firstIndex - indexMap.indexFromOffset(elemoff);
                            }
                        }
                    }
                }, subscribeRatio, minGrainSize);
            }
            else
            {
                UTparallelFor(geoSplittableRange0, [this, &indexMap](const GA_SplittableRange& r)
                {
                    GA_PageHandleT<T, T, true, true, GA_Attribute, GA_ATINumeric, GA_Detail> attrib_ph(enumAttrib);
                    for (GA_PageIterator pit = r.beginPages(); !pit.atEnd(); ++pit)
                    {
                        GA_Offset start, end;
                        for (GA_Iterator it(pit.begin()); it.blockAdvance(start, end); )
                        {
                            attrib_ph.setPage(start);
                            for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                            {
                                attrib_ph.value(elemoff) = firstIndex + indexMap.indexFromOffset(elemoff);
                            }
                        }
                    }
                }, subscribeRatio, minGrainSize);
            }
        }
    }


    template<>
    void enumerate<UT_StringHolder>()
    {
        UT_ASSERT_P(enumAttrib);

        const GA_RWHandleS attrib_h(enumAttrib);
        const GA_SplittableRange geoSplittableRange0(groupParser.getRange(enumAttrib->getOwner()));
        if(outAsOffset)
        {
            UTparallelFor(geoSplittableRange0, [this, &attrib_h](const GA_SplittableRange& r)
            {
                GA_Offset start, end;
                for (GA_Iterator it(r); it.blockAdvance(start, end); )
                {
                    for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                    {
                        char buffer[100];
                        sprintf(buffer, "%s%I64d%s", prefix, firstIndex+elemoff, sufix);
                        attrib_h.set(elemoff, buffer);
                    }
                }
            }, subscribeRatio, minGrainSize);
        }
        else
        {
            const GA_IndexMap& indexMap = enumAttrib->getIndexMap();
            if(negativeIndex)
            {
                UTparallelFor(geoSplittableRange0, [this, &attrib_h, &indexMap](const GA_SplittableRange& r)
                {
                    GA_Offset start, end;
                    for (GA_Iterator it(r); it.blockAdvance(start, end); )
                    {
                        for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                        {
                            char buffer[100];
                            sprintf(buffer, "%s%I64d%s", prefix, firstIndex-indexMap.indexFromOffset(elemoff), sufix);
                            attrib_h.set(elemoff, buffer);
                        }
                    }
                }, subscribeRatio, minGrainSize);
            }
            else
            {
                UTparallelFor(geoSplittableRange0, [this, &attrib_h, &indexMap](const GA_SplittableRange& r)
                {
                    GA_Offset start, end;
                    for (GA_Iterator it(r); it.blockAdvance(start, end); )
                    {
                        for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                        {
                            char buffer[100];
                            sprintf(buffer, "%s%I64d%s", prefix, firstIndex+indexMap.indexFromOffset(elemoff), sufix);
                            attrib_h.set(elemoff, buffer);
                        }
                    }
                }, subscribeRatio, minGrainSize);
            }
        }
    }

    


public:
    GA_Size firstIndex = 0;
    bool outAsOffset = true;
    bool negativeIndex = false;
    
    const char* prefix = "";
    const char* sufix = "";
    
    bool enumeratePieceElem = false;

private:
    
    GA_Attribute* enumAttrib;
    exint subscribeRatio = 64;
    exint minGrainSize = 1024;

private:
    GU_DetailHandle destgdh;
    UT_Array<GU_ConstDetailHandle> inputgdh;
    SOP_EnumerateParms enumParms;
    const SOP_NodeVerb* const enumVerb = SOP_NodeVerb::lookupVerb("enumerate");

}; // End of class GFE_Enumerate





#endif
