
#pragma once

#ifndef __GFE_AttributeValueReplace_h__
#define __GFE_AttributeValueReplace_h__

#include "GFE_RestVectorComponent.h"
#include "GFE/GFE_AttributeValueReplace.h"



#include "GFE/GFE_GeoFilter.h"


#include "UFE/UFE_SplittableString.h"



class GFE_AttribValReplace : public GFE_AttribCreateFilter {

    
#define GFE_TEMP_ATTRIB_VALUE_REPLACE_ATTRIBNAME "__GFE_TEMP_ATTRIB_VALUE_REPLACE_ATTRIBNAME"
    
using EntriesValueType = UT_Array<SOP_FeE_AttribValReplace_1_0Parms::NumEntries>;

    
public:
    using GFE_AttribCreateFilter::GFE_AttribCreateFilter;

    void
        setComputeParm(
            const EntriesValueType& entries,
            const bool outNewAttrib = false,
            const UT_StringRef& newAttribNames = "",
            const bool delInAttrib = true,
            const exint subscribeRatio = 64,
            const exint minGrainSize = 64
        )
    {
        setHasComputed();
        this->entries        = &entries;
        this->outNewAttrib   = outNewAttrib;
        this->newAttribNames = newAttribNames;
        this->delInAttrib    = delInAttrib;
        this->subscribeRatio = subscribeRatio;
        this->minGrainSize   = minGrainSize;
    }


    // SYS_FORCE_INLINE GA_Attribute*
    //     findOrCreateTuple(
    //         const EntriesValueType& entries,
    //         const bool outNewAttrib = false,
    //         const UT_StringRef& newAttribName = "",
    //         const bool delInAttrib = true,
    //         const exint subscribeRatio = 64,
    //         const exint minGrainSize = 64
    //     )
    // {
    //     getOutAttribArray().findOrCreateTuple();
    // }

private:

    // can not use in parallel unless for each GA_Detail
    virtual bool
        computeCore() override
    {
        if (outNewAttrib ? getInAttribArray().isEmpty() : getOutAttribArray().isEmpty())
            return false;
        
        if (!entries)
            return false;

        if (groupParser.isEmpty())
            return true;
        
        const UT_StringHolder emptyString = "";
        const UT_StringHolder tmpName = GFE_TEMP_ATTRIB_VALUE_REPLACE_ATTRIBNAME;
        
        if (outNewAttrib)
        {
            const size_t size = getInAttribArray().size();
            for (size_t i = 0; i < size; ++i)
            {
                attrib = getInAttribArray()[i];

                const UT_StringHolder& newName = newAttribNames.getIsValid() ? newAttribNames.getNext<UT_StringHolder>() : emptyString;

                const bool detached = !GFE_Type::isPublicAttribName(newName);
                
                if (detached)
                {
                    attribNew = getOutAttribArray().findOrCreateTuple(detached, attrib->getOwner(),
                        newStorageClass, GFE_Type::getPreferredStorage(newStorageClass, newPrecision), "");
                }
                else
                {
                    isSameName = strcmp(attrib->getName().c_str(), newName.c_str()) == 0;
                    const UT_StringHolder& newNameTemp = isSameName ? tmpName : newName;

                    attribNew = getOutAttribArray().findOrCreateTuple(detached, attrib->getOwner(),
                        newStorageClass, GFE_Type::getPreferredStorage(newStorageClass, newPrecision), newNameTemp);
                }
                
                const GA_AIFTuple* const aifTuple = attrib->getAIFTuple();
                // const GA_AIFStringTuple* const aifStringTuple = attrib->getAIFStringTuple();
                // const GA_AIFTuple* const aifTupleNEW = attribNew->getAIFTuple();
                // const GA_AIFStringTuple* const aifStringTupleNEW = attribNew->getAIFStringTuple();
                if (aifTuple)
                {
                    switch (aifTuple->getStorage(attrib))
                    {
                    case GA_STORE_INT16:  attribValReplaceNew<int16>();                     break;
                    case GA_STORE_INT32:  attribValReplaceNew<int32>();                     break;
                    case GA_STORE_INT64:  attribValReplaceNew<int64>();                     break;
                    //case GA_STORE_REAL16: attribValReplaceNew<fpreal16>();                  break;
                    case GA_STORE_REAL32: attribValReplaceNew<fpreal32>();                  break;
                    case GA_STORE_REAL64: attribValReplaceNew<fpreal64>();                  break;
                    //case GA_STORE_STRING: attribValReplaceNewFromString(); break;
                    default: break;
                    }
                }
                else
                {
                    const GA_AIFStringTuple* const aifStringTuple = attrib->getAIFStringTuple();
                    if (aifStringTuple)
                        attribValReplaceNewFromString();
                }
                
                if (delInAttrib)
                    geo->destroyAttrib(attrib);

                
                if (!detached && isSameName)
                {
                    if (!delInAttrib)
                        geo->destroyAttrib(attrib);
                    geo->renameAttrib(attribNew, newName);
                }
            }
            
        }
        else
        {
            const size_t size = getOutAttribArray().size();
            for (size_t i = 0; i < size; ++i)
            {
                attrib = getOutAttribArray()[i];
                const GA_AIFTuple* const aifTuple = attrib->getAIFTuple();
                if (aifTuple)
                {
                    switch (aifTuple->getStorage(attrib))
                    {
                    case GA_STORE_INT16:  attribValReplace<int16>();           break;
                    case GA_STORE_INT32:  attribValReplace<int32>();           break;
                    case GA_STORE_INT64:  attribValReplace<int64>();           break;
                    //case GA_STORE_REAL16: attribValReplace<fpreal16>();        break;
                    case GA_STORE_REAL32: attribValReplace<fpreal32>();        break;
                    case GA_STORE_REAL64: attribValReplace<fpreal64>();        break;
                    //case GA_STORE_STRING: attribValReplace<UT_StringHolder>(); break;
                    default: break;
                    }
                }
                else
                {
                    const GA_AIFStringTuple* const aifStringTuple = attrib->getAIFStringTuple();
                    if (aifStringTuple)
                        attribValReplace<UT_StringHolder>();
                }
            }
        }

        
        return true;
    }
    

    void attribValReplaceNewFromString()
    {
        const GA_AIFTuple* const aifTuple = attribNew->getAIFTuple();
        if (aifTuple)
        {
            switch (aifTuple->getStorage(attribNew))
            {
            case GA_STORE_INT16:  attribValReplaceNewFromString<int16>();                   break;
            case GA_STORE_INT32:  attribValReplaceNewFromString<int32>();                   break;
            case GA_STORE_INT64:  attribValReplaceNewFromString<int64>();                   break;
            //case GA_STORE_REAL16: attribValReplaceNewFromString<fpreal16>();                break;
            case GA_STORE_REAL32: attribValReplaceNewFromString<fpreal32>();                break;
            case GA_STORE_REAL64: attribValReplaceNewFromString<fpreal64>();                break;
            //case GA_STORE_STRING: attribValReplaceString(); break;
            default: break;
            }
        }
        else
        {
            const GA_AIFStringTuple* const aifStringTuple = attribNew->getAIFStringTuple();
            if (aifStringTuple)
                attribValReplaceString();
        }
    }
    
    template<typename KEY_T>
    void attribValReplaceNew()
    {
        const GA_AIFTuple* const aifTuple = attribNew->getAIFTuple();
        if (aifTuple)
        {
            switch (aifTuple->getStorage(attribNew))
            {
            case GA_STORE_INT16:  attribValReplaceNew<KEY_T, int16>();                   break;
            case GA_STORE_INT32:  attribValReplaceNew<KEY_T, int32>();                   break;
            case GA_STORE_INT64:  attribValReplaceNew<KEY_T, int64>();                   break;
            //case GA_STORE_REAL16: attribValReplaceNew<KEY_T, fpreal16>();                break;
            case GA_STORE_REAL32: attribValReplaceNew<KEY_T, fpreal32>();                break;
            case GA_STORE_REAL64: attribValReplaceNew<KEY_T, fpreal64>();                break;
            //case GA_STORE_STRING: break;
            default: break;
            }
        }
        else
        {
            const GA_AIFStringTuple* const aifStringTuple = attribNew->getAIFStringTuple();
            if (aifStringTuple)
            {
                if constexpr(std::is_same_v<KEY_T, UT_StringHolder> ||
                             std::is_same_v<KEY_T, UT_StringRef>    ||
                             std::is_same_v<KEY_T, char*>           )
                {
                    attribValReplaceString();
                }
                else 
                    attribValReplaceNewToString<KEY_T>();
            }
        }
    }
    
    template<typename KEY_T, typename ENTRY_T>
    void attribValReplaceNew()
    {
        UT_ASSERT_P(attrib);
        UT_Map<KEY_T, ENTRY_T> dict = genGntriesDict<KEY_T, ENTRY_T>();
        
        //UT_Map<int32, int32>& dict1 = genGntriesDict<int32, int32>();
        //const int32 in = 0;
        //dict1.find(in);
        //dict1[0];
        UTparallelFor(groupParser.getSplittableRange(attrib), [this, &dict](const GA_SplittableRange& r)
        {
            GA_PageHandleT<KEY_T, KEY_T, true, false, const GA_Attribute, const GA_ATINumeric, const GA_Detail> attrib_ph(attrib);
            GA_PageHandleT<ENTRY_T, ENTRY_T, true, true, GA_Attribute, GA_ATINumeric, GA_Detail> attribNew_ph(attribNew);
            for (GA_PageIterator pit = r.beginPages(); !pit.atEnd(); ++pit)
            {
                GA_Offset start, end;
                for (GA_Iterator it(pit.begin()); it.blockAdvance(start, end); )
                {
                    attrib_ph.setPage(start);
                    attribNew_ph.setPage(start);
                    for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                    {
                        if (!dict.contains(attrib_ph.value(elemoff)))
                            continue;
                        attribNew_ph.value(elemoff) = dict[attrib_ph.value(elemoff)];
                    }
                }
            }
        }, subscribeRatio, minGrainSize);
    }

    template<typename ENTRY_T>
    void attribValReplaceNewFromString()
    {
        UT_ASSERT_P(attrib);
        UT_Map<UT_StringHolder, ENTRY_T> dict = genGntriesDict<UT_StringHolder, ENTRY_T>();
        
        const GA_RWHandleS attrib_h(attrib);
        UTparallelFor(groupParser.getSplittableRange(attrib), [this, &dict, &attrib_h](const GA_SplittableRange& r)
        {
            GA_PageHandleT<ENTRY_T, ENTRY_T, true, true, GA_Attribute, GA_ATINumeric, GA_Detail> attribNew_ph(attribNew);
            for (GA_PageIterator pit = r.beginPages(); !pit.atEnd(); ++pit)
            {
                GA_Offset start, end;
                for (GA_Iterator it(pit.begin()); it.blockAdvance(start, end); )
                {
                    attribNew_ph.setPage(start);
                    for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                    {
                        if (!dict.contains(attrib_h.get(elemoff)))
                            continue;
                        attribNew_ph.value(elemoff) = dict[attrib_h.get(elemoff)];
                    }
                }
            }
        }, subscribeRatio, minGrainSize);
    }
    
    
    void attribValReplaceString()
    {
        UT_ASSERT_P(attrib);
        UT_Map<UT_StringHolder, UT_StringHolder> dict = genGntriesDict<UT_StringHolder, UT_StringHolder>();
        
        const GA_RWHandleS attrib_h(attrib);
        const GA_RWHandleS attribNew_h(attribNew);
        UTparallelFor(groupParser.getSplittableRange(attrib), [this, &dict, &attrib_h, &attribNew_h](const GA_SplittableRange& r)
        {
            for (GA_PageIterator pit = r.beginPages(); !pit.atEnd(); ++pit)
            {
                GA_Offset start, end;
                for (GA_Iterator it(pit.begin()); it.blockAdvance(start, end); )
                {
                    for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                    {
                        if (!dict.contains(attrib_h.get(elemoff)))
                            continue;
                        attribNew_h.set(elemoff, dict[attrib_h.get(elemoff)]);
                    }
                }
            }
        }, subscribeRatio, minGrainSize);
    }
    
    template<typename KEY_T>
    void attribValReplaceNewToString()
    {
        UT_ASSERT_P(attrib);
        UT_Map<KEY_T, UT_StringHolder> dict = genGntriesDict<KEY_T, UT_StringHolder>();
        
        const GA_RWHandleS attribNew_h(attribNew);
        UTparallelFor(groupParser.getSplittableRange(attrib), [this, &dict, &attribNew_h](const GA_SplittableRange& r)
        {
            GA_PageHandleT<KEY_T, KEY_T, true, false, const GA_Attribute, const GA_ATINumeric, const GA_Detail> attrib_ph(attrib);
            for (GA_PageIterator pit = r.beginPages(); !pit.atEnd(); ++pit)
            {
                GA_Offset start, end;
                for (GA_Iterator it(pit.begin()); it.blockAdvance(start, end); )
                {
                    attrib_ph.setPage(start);
                    for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                    {
                        if (!dict.contains(attrib_ph.value(elemoff)))
                            continue;
                        attribNew_h.set(elemoff, dict[attrib_ph.value(elemoff)]);
                    }
                }
            }
        }, subscribeRatio, minGrainSize);
    }


    
    
    template<typename KEY_T>
    void attribValReplace()
    {
        UT_ASSERT_P(attrib);
        UT_Map<KEY_T, KEY_T> dict;
        const size_t numEntries = entries->size();
        for (size_t i = 0; i < numEntries; ++i)
        {
            const SOP_FeE_AttribValReplace_1_0Parms::NumEntries& entry = entries->operator[](i);
            if (!entry.enable)
                continue;
            
            if constexpr(std::is_same_v<KEY_T, fpreal16> ||
                         std::is_same_v<KEY_T, fpreal32> ||
                         std::is_same_v<KEY_T, fpreal64> )
            {
                dict[atof(entry.key.c_str())] = entry.valf;
            }
            else if constexpr(std::is_same_v<KEY_T, int16> ||
                              std::is_same_v<KEY_T, int32> ||
                              std::is_same_v<KEY_T, int64> )
            {
                dict[atoi(entry.key)] = entry.vali;
            }
            else if constexpr(std::is_same_v<KEY_T, UT_StringHolder> ||
                              std::is_same_v<KEY_T, UT_StringRef>    ||
                              std::is_same_v<KEY_T, char*>           )
            {
                dict[entry.key] = entry.vals;
            }
        }
        
        
        UTparallelFor(groupParser.getSplittableRange(attrib), [this, &dict](const GA_SplittableRange& r)
        {
            GA_PageHandleT<KEY_T, KEY_T, true, true, GA_Attribute, GA_ATINumeric, GA_Detail> attrib_ph(attrib);
            for (GA_PageIterator pit = r.beginPages(); !pit.atEnd(); ++pit)
            {
                GA_Offset start, end;
                for (GA_Iterator it(pit.begin()); it.blockAdvance(start, end); )
                {
                    attrib_ph.setPage(start);
                    for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                    {
                        if (!dict.contains(attrib_ph.value(elemoff)))
                            continue;
                        attrib_ph.value(elemoff) = dict[attrib_ph.value(elemoff)];
                    }
                }
            }
        }, subscribeRatio, minGrainSize);
    }

    template<>
    void attribValReplace<UT_StringHolder>()
    {
        UT_ASSERT_P(attrib);

        UT_Map<UT_StringHolder, UT_StringHolder> dict;
        const size_t numEntries = entries->size();
        for (size_t i = 0; i < numEntries; ++i)
        {
            const SOP_FeE_AttribValReplace_1_0Parms::NumEntries& entry = entries->operator[](i);
            if (!entry.enable)
                continue;
            dict[entry.key] = entry.vals;
        }
        
        const GA_RWHandleS attrib_h(attrib);
        UTparallelFor(groupParser.getSplittableRange(attrib), [this, &attrib_h, &dict](const GA_SplittableRange& r)
        {
            GA_Offset start, end;
            for (GA_Iterator it(r); it.blockAdvance(start, end); )
            {
                for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                {
                    if (!dict.contains(attrib_h.get(elemoff)))
                        continue;
                    attrib_h.set(elemoff, dict[attrib_h.get(elemoff)]);
                }
            }
        }, subscribeRatio, minGrainSize);
    }

    

    template<typename KEY_T, typename ENTRY_T>
    UT_Map<KEY_T, ENTRY_T> genGntriesDict()
    {
        UT_ASSERT_P(entries);
        
        UT_Map<KEY_T, ENTRY_T> dict;
        const size_t numEntries = entries->size();
        for (size_t i = 0; i < numEntries; ++i)
        {
            const SOP_FeE_AttribValReplace_1_0Parms::NumEntries& entry = entries->operator[](i);
            if (!entry.enable)
                continue;
            
            if constexpr(std::is_same_v<KEY_T, fpreal16> ||
                         std::is_same_v<KEY_T, fpreal32> ||
                         std::is_same_v<KEY_T, fpreal64> )
            {
                dict[atof(entry.key.c_str())] = getEntryValue<ENTRY_T>(entry);
            }
            else if constexpr(std::is_same_v<KEY_T, int16> ||
                              std::is_same_v<KEY_T, int32> ||
                              std::is_same_v<KEY_T, int64> )
            {
                dict[atoi(entry.key)] = getEntryValue<ENTRY_T>(entry);
            }
            else if constexpr(std::is_same_v<KEY_T, UT_StringHolder> ||
                              std::is_same_v<KEY_T, UT_StringRef>    ||
                              std::is_same_v<KEY_T, char*>           )
            {
                dict[entry.key] = getEntryValue<ENTRY_T>(entry);
            }
        }
        return dict;
    }

    template<typename ENTRY_T>
    const ENTRY_T getEntryValue(const SOP_FeE_AttribValReplace_1_0Parms::NumEntries& entry)
    {
        if constexpr(std::is_same_v<ENTRY_T, fpreal16> ||
                     std::is_same_v<ENTRY_T, fpreal32> ||
                     std::is_same_v<ENTRY_T, fpreal64> )
        {
            return entry.valf;
        }
        else if constexpr(std::is_same_v<ENTRY_T, int16> ||
                          std::is_same_v<ENTRY_T, int32> ||
                          std::is_same_v<ENTRY_T, int64> )
        {
            return entry.vali;
        }
        else if constexpr(std::is_same_v<ENTRY_T, UT_StringHolder> ||
                          std::is_same_v<ENTRY_T, UT_StringRef>    ||
                          std::is_same_v<ENTRY_T, char*>           )
        {
            return entry.vals;
        }
    }
    
public:
    const EntriesValueType* entries = nullptr;

    
    bool outNewAttrib = false;
    GA_StorageClass newStorageClass = GA_STORECLASS_INVALID;
    GA_Precision newPrecision = GA_PRECISION_INVALID;

    UFE_SplittableString newAttribNames;
    
    bool delInAttrib = true;
    
    
private:
    
    GA_Attribute* attrib;
    GA_Attribute* attribNew;
    bool isSameName;
    
    exint subscribeRatio = 64;
    exint minGrainSize = 1024;

    
#undef GFE_TEMP_ATTRIB_VALUE_REPLACE_ATTRIBNAME
    
}; // End of class GFE_AttribValReplace








#endif
