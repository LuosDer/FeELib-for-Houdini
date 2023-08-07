
#pragma once

#ifndef __GFE_Enumerate_h__
#define __GFE_Enumerate_h__

#include "GFE/GFE_Enumerate.h"

#include "GFE/GFE_GeoFilter.h"



#include "GFE/GFE_AttributeCast.h"
#include "SOP/SOP_Enumerate.proto.h"

/*
    GFE_Enumerate enumerate(geo, cookparms);
    enumerate.findOrCreateTuple(true, GA_ATTRIB_POINT);
    enumerate.compute();
*/
    
class GFE_Enumerate : public GFE_AttribFilter {

#define __TEMP_GFE_Enumerate_GroupName       "__TEMP_GFE_Enumerate_Group"
#define __TEMP_GFE_Enumerate_PieceAttribName "__TEMP_GFE_Enumerate_PieceAttrib"
#define __TEMP_GFE_Enumerate_OutAttribName   "__TEMP_GFE_Enumerate_OutAttrib"
    

public:
    using GFE_AttribFilter::GFE_AttribFilter;


    void
        setComputeParm(
            const GA_Size firstIndex = 0,
            const bool negativeIndex = false,
            const bool outAsOffset   = true,
            const exint subscribeRatio = 64,
            const exint minGrainSize   = 1024
        )
    {
        setHasComputed();
        this->firstIndex    = firstIndex;
        this->negativeIndex = negativeIndex;
        this->outAsOffset   = outAsOffset;
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
    {
#if GFE_DEBUG_MODE
        outAttribName = attribName.c_str();
#endif
        return getOutAttribArray().findOrCreateTuple(detached, owner,
            storageClass, storage, __TEMP_GFE_Enumerate_OutAttribName, 1, GA_Defaults(GFE_INVALID_OFFSET));
        
        // if (pieceAttrib && !detached && !pieceAttrib->isDetached() && GFE_Type::stringEqual(pieceAttrib->getName(), attribName))
        // {
        //     return getOutAttribArray().set(owner, attribName);
        // }
        // else
        // {
        // }
    }

    SYS_FORCE_INLINE GA_Attribute* findOrCreateTuple(
        const bool detached,
        const GA_AttributeOwner owner,
        const UT_StringRef& attribName
    )
    { return findOrCreateTuple(detached, owner, GA_STORECLASS_INT, GA_STORE_INVALID, attribName); }

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
        
        enumAttrib = getOutAttribArray()[0];
        enumerate();
        geo->forceRenameAttribute(enumAttrib, outAttribName);
        // const size_t size = getOutAttribArray().size();
        // for (size_t i = 0; i < size; i++)
        // {
        //     enumAttrib = getOutAttribArray()[i];
        //     enumerate();
        //     geo->forceRenameAttribute(enumAttrib, outAttribName);
        // }
        return true;
    }



    
    template<typename _Ty, bool _outAsOffset, bool _negativeIndex>
    void enumerate()
    {
        if constexpr (GFE_Type::isStringHolder<_Ty>)
        {
            const GA_RWHandleS attrib_h(enumAttrib);
            UTparallelFor(groupParser.getSplittableRange(enumAttrib), [this, &attrib_h](const GA_SplittableRange& r)
            {
                char buffer[100];
                GA_Offset start, end;
                for (GA_Iterator it(r); it.blockAdvance(start, end); )
                {
                    for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                    {
                        if constexpr (_outAsOffset)
                        {
                            std::to_string();
                            sprintf(buffer, "%s%I64d%s", prefix, firstIndex + elemoff, sufix);
                        }
                        else if constexpr (_negativeIndex)
                        {
                            sprintf(buffer, "%s%I64d%s", prefix, firstIndex - enumAttrib->getIndexMap().indexFromOffset(elemoff), sufix);
                        }
                        else
                        {
                            sprintf(buffer, "%s%I64d%s", prefix, firstIndex + enumAttrib->getIndexMap().indexFromOffset(elemoff), sufix);
                        }
                        attrib_h.set(elemoff, buffer);
                    }
                }
            }, subscribeRatio, minGrainSize);
        }
        else if constexpr (GFE_Type::isScalar<_Ty>)
        {
            UTparallelFor(groupParser.getSplittableRange(enumAttrib), [this](const GA_SplittableRange& r)
            {
                GFE_RWPageHandleT<_Ty> attrib_ph(enumAttrib);
                GA_Offset start, end;
                for (GA_PageIterator pit = r.beginPages(); !pit.atEnd(); ++pit)
                {
                    for (GA_Iterator it(pit.begin()); it.blockAdvance(start, end); )
                    {
                        attrib_ph.setPage(start);
                        //const GA_Offset baseOff = start - GAgetPageOff(start);
                        for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                        {
                            if constexpr (_outAsOffset)
                            {
                                attrib_ph.value(elemoff) = firstIndex + elemoff;
                            }
                            else if constexpr (_negativeIndex)
                            {
                                attrib_ph.value(elemoff) = firstIndex - enumAttrib->getIndexMap().indexFromOffset(elemoff);
                            }
                            else
                            {
                                attrib_ph.value(elemoff) = firstIndex + enumAttrib->getIndexMap().indexFromOffset(elemoff);
                            }
                        }
                    }
                }
            }, subscribeRatio, minGrainSize);
        }
    }
    
    void enumerate()
    {
        UT_ASSERT_P(enumAttrib);
        
        auto storageVarient = GFE_Variant::getAttribStorageVariantIS(*enumAttrib);
        auto outAsOffsetVarient   = GFE_Variant::getBoolVariant(outAsOffset);
        auto negativeIndexVarient = GFE_Variant::getBoolVariant(negativeIndex);
        
        std::visit([&] (auto storageVarient, auto outAsOffsetVarient, auto negativeIndexVarient)
        {
            using type = typename GFE_Variant::getAttribStorage_t<storageVarient>;
            enumerate<type, outAsOffsetVarient, negativeIndexVarient>();
        }, storageVarient, outAsOffsetVarient, negativeIndexVarient);
    }


    void enumerateSideFX()
    {
        enumAttrib = getOutAttribArray()[0];
        
        UT_ASSERT_P(pieceAttrib);
        UT_ASSERT_P(enumAttrib);
        
        const GA_AttributeOwner owner = enumAttrib->getOwner();
        
        UT_ASSERT_MSG(pieceAttrib->getOwner() == owner, "not same owner");
        
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
            enumParms.setGroup("");
        }

        GA_Attribute* namedPieceAttrib = nullptr;
        if (pieceAttrib->isDetached())
        {
            namedPieceAttrib = GFE_Attribute::clone(geo, pieceAttrib, __TEMP_GFE_Enumerate_PieceAttribName);
            enumParms.setPieceAttrib(__TEMP_GFE_Enumerate_PieceAttribName);
            //namedPieceAttrib->bumpDataId();
        }
        else
        {
            enumParms.setPieceAttrib(pieceAttrib->getName());
        }

        SOP_EnumerateEnums::GroupType enumGroupType;
        switch (owner)
        {
        case GA_ATTRIB_PRIMITIVE: enumGroupType = SOP_EnumerateEnums::GroupType::PRIMITIVE; break;
        case GA_ATTRIB_POINT:     enumGroupType = SOP_EnumerateEnums::GroupType::POINT;     break;
        case GA_ATTRIB_VERTEX:    enumGroupType = SOP_EnumerateEnums::GroupType::VERTEX;    break;
        default: UT_ASSERT_P("unhandled enumAttrib class"); break;
        }
        enumParms.setGroupType(enumGroupType);
        enumParms.setUsePieceAttrib(true);
        enumParms.setPieceMode(enumeratePieceElem ? SOP_EnumerateEnums::PieceMode::ELEMENTS : SOP_EnumerateEnums::PieceMode::PIECES);

        //GA_Attribute* outAttrib = nullptr;
        //if (enumAttrib->isDetached() || GFE_Type::stringEqual(enumAttrib->getName(), pieceAttrib->getName()))
        
        enumParms.setAttribname(__TEMP_GFE_Enumerate_OutAttribName);
        // if (enumAttrib->isDetached() || enumAttrib == pieceAttrib)
        // {
        //     //outAttrib = GFE_Attribute::clone(geo, enumAttrib, __TEMP_GFE_Enumerate_OutAttribName);
        //     enumParms.setAttribname(__TEMP_GFE_Enumerate_OutAttribName);
        // }
        // else
        // {
        //     enumParms.setAttribname(enumAttrib->getName());
        // }

        
        //enumParms.setAttribType(SOP_EnumerateEnums::AttribType::INT);
        //enumParms.setPrefix("");
        
        destgdh.allocateAndSet(geo->asGU_Detail(), false);
        
        inputgdh.clear();
        GU_DetailHandle input_h;
        if (geoSrc)
        {
            GU_Detail* const geoSrcGU = new GU_Detail;
            geoSrcGU->replaceWith(*geoSrc);
            input_h.allocateAndSet(geoSrcGU);
        }
        else
        {
            input_h.allocateAndSet(geo->asGU_Detail(), false);
        }
        inputgdh.emplace_back(input_h);
        
        SOP_NodeCache* const nodeCache = enumVerb->allocCache();
        const auto enumCookparms = GFE_NodeVerb::newCookParms(cookparms, enumParms, nodeCache, &destgdh, &inputgdh);
        
        GFE_AttribCast attribCast(geo, cookparms);
        attribCast.newStorageClass = enumAttrib->getStorageClass();
        attribCast.newPrecision    = GFE_Attribute::getPrecision(enumAttrib);
        attribCast.newAttribNames  = enumAttrib->isDetached() ? "" : outAttribName;
        
        enumVerb->cook(enumCookparms);
        
        attribCast.getInAttribArray().set(owner, __TEMP_GFE_Enumerate_OutAttribName);
        if (attribCast.newStorageClass == GA_STORECLASS_STRING)
        {
            attribCast.prefix = prefix;
            attribCast.sufix  = sufix;
        }
        attribCast.compute();

        geo->destroyElementGroup(elemGroup);
        geo->destroyAttrib(namedPieceAttrib);
        
        getOutAttribArray().set(attribCast.getOutAttribArray());
    }



public:
    GA_Size firstIndex = 0;
    bool outAsOffset   = true;
    bool negativeIndex = false;
    
    const char* prefix = "";
    const char* sufix  = "";
    
    bool enumeratePieceElem = false;
    
    
private:
    const char* outAttribName = nullptr;
    GA_Attribute* enumAttrib;
    
    
    exint subscribeRatio = 64;
    exint minGrainSize   = 1024;

private:
    GU_DetailHandle destgdh;
    UT_Array<GU_ConstDetailHandle> inputgdh;
    SOP_EnumerateParms enumParms;
    const SOP_NodeVerb* const enumVerb = SOP_NodeVerb::lookupVerb("enumerate");

#undef __TEMP_GFE_Enumerate_GroupName
#undef __TEMP_GFE_Enumerate_PieceAttribName
#undef __TEMP_GFE_Enumerate_OutAttribName

    
}; // End of class GFE_Enumerate





#endif
