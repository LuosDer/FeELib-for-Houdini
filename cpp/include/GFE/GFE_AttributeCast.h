
#pragma once

#ifndef __GFE_AttributeCast_h__
#define __GFE_AttributeCast_h__

//#include "GFE/GFE_AttributeCast.h"


#include "GFE/GFE_GeoFilter.h"

#include "UFE/UFE_SplittableString.h"
    


class GFE_AttribCast : public GFE_AttribCreateFilter
{
public:

    using GFE_AttribCreateFilter::GFE_AttribCreateFilter;


    void setComputeParm(
        const bool delOrigin = true,
        const exint subscribeRatio = 64,
        const exint minGrainSize = 64
    )
    {
        setHasComputed();
        this->delOrigin = delOrigin;
        this->subscribeRatio = subscribeRatio;
        this->minGrainSize = minGrainSize;
    }
    
    
    SYS_FORCE_INLINE void setRenameAttrib(const GA_Attribute& inAttrib)
    {
        setRenameAttrib(inAttrib.isDetached() ? "" : inAttrib.getName().c_str());
    }
    SYS_FORCE_INLINE void setRenameAttrib(const char* inNewNames)
    {
        newAttribNames = inNewNames;
        renameAttrib = newAttribNames.getIsValid();
    }
    SYS_FORCE_INLINE void setRenameAttrib(const UT_StringHolder& inNewNames)
    {
        newAttribNames = inNewNames;
        renameAttrib = newAttribNames.getIsValid();
    }
    SYS_FORCE_INLINE void setRenameAttrib()
    {
        renameAttrib = false;
    }
    
    SYS_FORCE_INLINE void setRenameGroup(const GA_Group& inGroup)
    {
        setRenameGroup(inGroup.isDetached() ? "" : inGroup.getName().c_str());
    }
    SYS_FORCE_INLINE void setRenameGroup(const char* inNewNames)
    {
        newGroupNames = inNewNames;
        renameGroup = newGroupNames.getIsValid();
    }
    SYS_FORCE_INLINE void setRenameGroup(const UT_StringHolder& inNewNames)
    {
        newGroupNames = inNewNames;
        renameGroup = newGroupNames.getIsValid();
    }
    SYS_FORCE_INLINE void setRenameGroup()
    {
        renameGroup = false;
    }
    
    void setDestinationAttribute(const GA_Attribute& inAttrib)
    {
        newStorageClass = inAttrib.getStorageClass();
        if(inAttrib.getAIFTuple())
        {
            precision = GFE_Type::precisionFromStorage(inAttrib.getAIFTuple()->getStorage(&inAttrib));
        }
        else if(inAttrib.getAIFNumericArray())
        {
            precision = GFE_Type::precisionFromStorage(inAttrib.getAIFNumericArray()->getStorage(&inAttrib));
        }
        else
        {
            precision = GA_PRECISION_INVALID;
        }
        setRenameAttrib(inAttrib);
    }

    SYS_FORCE_INLINE void setDestinationGroup(const GA_Group& inGroup)
    {
        newStorageClass = GA_STORECLASS_OTHER;
        precision = GA_PRECISION_1;
        setRenameGroup(inGroup);
    }


private:

    virtual bool
        computeCore() override
    {
        for (size_t i = 0; i < getInAttribArray().size(); i++)
        {
            const GA_Attribute& attrib = *getInAttribArray()[i];
            if (attrib.getAIFTuple() || attrib.getAIFStringTuple() || attrib.getAIFSharedStringTuple())
                continue;
            getInAttribArray().erase(i);
        }
        
        if (getInAttribArray().isEmpty() && !getInGroupArray().size())
            return false;

        if (newStorageClass == GA_STORECLASS_INVALID)
            return false;
        
        if (groupParser.isEmpty())
            return true;
        
        // precision = geo->getPreferredPrecision(precision);
        precision = geo->getValidPrecision(precision);
        
        //::std::string newName;
        
        ::std::vector<GA_Attribute*>::size_type arrayLen;
        arrayLen = getInAttribArray().size();
        for (size_t i = 0; i < arrayLen; i++)
        {
            // UT_StringHolder a = renameAttrib ? newAttribNames.getNext<UT_StringHolder>() : getInAttribArray()[i]->getName();
            // const char* b = a.c_str();
            //attribCast(*getInAttribArray()[i], a);
            attribCast(*getInAttribArray()[i]);
        }
            
        if (newStorageClass == GA_STORECLASS_OTHER)
            return true;

        arrayLen = getInGroupArray().size();
        for (size_t i = 0; i < arrayLen; i++)
        {
            attribCast(*getInGroupArray()[i]);
        }
        
        return true;
    }

#define GFE_TEMP_ATTRIBCAST_ATTRIBNAME "__GFE_TEMP_ATTRIBCAST_ATTRIBNAME"

    void attribCast(GA_Attribute& attrib)
    {
        if (attrib.getStorageClass() == newStorageClass)
        {
            if (newStorageClass == GA_STORECLASS_INVALID || newStorageClass == GA_STORECLASS_OTHER)
                return;
            
            if (GFE_Attribute::getPrecision(attrib) == precision)
                return;
        }
            
        const GA_AttributeOwner attribClass = attrib.getOwner();
        
        const UT_StringHolder& newName = renameAttrib ? newAttribNames.getNext<UT_StringHolder>() : attrib.getName();
        const bool detached = !newName.isstring() || newName.length() == 0;

        if(newStorageClass == GA_STORECLASS_OTHER)
        {
            GA_Group& newAttrib = *getOutGroupArray().findOrCreate(detached, attrib.getOwner(), newName);
            
            attribDuplicate(newAttrib, attrib);

            if(delOrigin)
                geo->destroyNonDetachedAttrib(attrib);
        }
        else
        {
            const GA_Storage storage = GFE_Type::getPreferredStorage(newStorageClass, precision);
            if(!detached && !attrib.isDetached() && strcmp(attrib.getName().c_str(), newName.c_str()) == 0)
            {
                GA_Attribute& newAttrib = *getOutAttribArray().findOrCreateTuple(
                    false, attrib.getOwner(), newStorageClass, storage, GFE_TEMP_ATTRIBCAST_ATTRIBNAME);
                
                attribDuplicate(newAttrib, attrib);

                geo->destroyAttrib(attrib);
                geo->renameAttrib(newAttrib, newName);
            }
            else
            {
                GA_Attribute& newAttrib = *getOutAttribArray().findOrCreateTuple(
                    detached, attrib.getOwner(), newStorageClass, storage, newName);
                
                attribDuplicate(newAttrib, attrib);
                
                if(delOrigin)
                    geo->destroyNonDetachedAttrib(attrib);
            }
        }
    }

    void attribCast(GA_ElementGroup& group)
    {
        const GA_AttributeOwner attribClass = GFE_Type::attributeOwner_groupType(group.classType());
        
        const UT_StringHolder& newName = renameGroup ? newGroupNames.getNext<UT_StringHolder>() : group.getName();
        const bool detached = !newName.isstring() || newName.length() == 0;

        GA_Attribute& newAttrib = *getOutAttribArray().findOrCreateTuple(
            detached, attribClass, newStorageClass, GA_STORE_INVALID, newName);
        
        attribDuplicate(newAttrib, group);
        
        if(delOrigin)
            geo->destroyElementGroup(&group);
    }

    SYS_FORCE_INLINE void attribCast(GA_Group& group)
    {
        attribCast(static_cast<GA_ElementGroup&>(group));
    }
        
    template<typename T>
    void
        attribDuplicate(
            GA_ElementGroup& group,
            const GA_Attribute& attribRef
        )
    {
        UTparallelFor(groupParser.getSplittableRange(attribRef.getOwner()), [&group, &attribRef](const GA_SplittableRange& r)
        {
            GA_PageHandleT<T, T, true, false, const GA_Attribute, const GA_ATINumeric, const GA_Detail> attrib_ph(&attribRef);
            for (GA_PageIterator pit = r.beginPages(); !pit.atEnd(); ++pit)
            {
                GA_Offset start, end;
                for (GA_Iterator it(pit.begin()); it.blockAdvance(start, end); )
                {
                    attrib_ph.setPage(start);
                    for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                    {
                        group.setElement(elemoff, bool(attrib_ph.value(elemoff)));
                    }
                }
            }
        }, subscribeRatio, minGrainSize);
        group.invalidateGroupEntries();
    }
    
    template<>
    void
        attribDuplicate<UT_StringHolder>(
            GA_ElementGroup& group,
            const GA_Attribute& attribRef
        )
    {
        const GA_ROHandleS attrib_h(&attribRef);
        UTparallelFor(groupParser.getSplittableRange(attribRef.getOwner()), [&group, &attrib_h](const GA_SplittableRange& r)
        {
            for (GA_PageIterator pit = r.beginPages(); !pit.atEnd(); ++pit)
            {
                GA_Offset start, end;
                for (GA_Iterator it(pit.begin()); it.blockAdvance(start, end); )
                {
                    for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                    {
                        const UT_StringHolder& strval = attrib_h.get(elemoff);
                        group.setElement(elemoff, strval == "1" || strval == "true" || strval == "True");
                    }
                }
            }
        }, subscribeRatio, minGrainSize);
        group.invalidateGroupEntries();
    }
    
    void
    attribDuplicate(
        GA_ElementGroup& group,
        const GA_Attribute& attribRef
    )
    {
        switch (attribRef.getStorageClass())
        {
        case GA_STORECLASS_INT:
            switch (precision)
            {
            case GA_PRECISION_8:
                attribDuplicate<int8>(group, attribRef); break;
            case GA_PRECISION_16:
                attribDuplicate<int8>(group, attribRef); break;
            case GA_PRECISION_32:
                attribDuplicate<int16>(group, attribRef); break;
            case GA_PRECISION_64:
                attribDuplicate<int32>(group, attribRef); break;
            default:         break;
            }
            break;
        case GA_STORECLASS_REAL:
            switch (precision)
            {
            case GA_PRECISION_16:
                attribDuplicate<fpreal16>(group, attribRef); break;
            case GA_PRECISION_32:
                attribDuplicate<fpreal32>(group, attribRef); break;
            case GA_PRECISION_64:
                attribDuplicate<fpreal64>(group, attribRef); break;
            default:         break;
            }
            break;
        case GA_STORECLASS_STRING:
            attribDuplicate<UT_StringHolder>(group, attribRef); break;
        case GA_STORECLASS_DICT:  break;
        case GA_STORECLASS_OTHER: break;
        default:             break;
        }
    }
    
    SYS_FORCE_INLINE void attribDuplicate(
        GA_Group& group,
        const GA_Attribute& attribRef
    )
    {
        return attribDuplicate(static_cast<GA_ElementGroup&>(group), attribRef);
    }
    



    bool
    attribDuplicate(
        GA_Attribute& attrib,
        const GA_ElementGroup& groupRef
    )
    {
        //const GA_SplittableRange geoSplittableRange = GFE_Group::getSplittableRangeByAnyGroup(*this, &group);
        const GA_SplittableRange geoSplittableRange = geo->getSplittableRangeByAnyGroup(groupRef);

        switch (newStorageClass)
        {
        case GA_STORECLASS_INT:
            switch (precision)
            {
            case GA_PRECISION_8:
                setAttribValueTo1<int8> (attrib, geoSplittableRange);
                return true; break;
            case GA_PRECISION_16:
                setAttribValueTo1<int16>(attrib, geoSplittableRange);
                return true; break;
            case GA_PRECISION_32:
                setAttribValueTo1<int32>(attrib, geoSplittableRange);
                return true; break;
            case GA_PRECISION_64:
                setAttribValueTo1<int64>(attrib, geoSplittableRange);
                return true; break;
            default:         break;
            }
            break;
        case GA_STORECLASS_REAL:
            switch (precision)
            {
            case GA_PRECISION_16:
                setAttribValueTo1<fpreal16>(attrib, geoSplittableRange);
                return true; break;
            case GA_PRECISION_32:
                setAttribValueTo1<fpreal32>(attrib, geoSplittableRange);
                return true; break;
            case GA_PRECISION_64:
                setAttribValueTo1<fpreal64>(attrib, geoSplittableRange);
                return true; break;
            default:         break;
            }
            break;
        case GA_STORECLASS_STRING:
            setAttribValueTo1<UT_StringHolder>(attrib, geoSplittableRange);
            return true;     break;
        case GA_STORECLASS_DICT:
            break;

        case GA_STORECLASS_OTHER:
            return false;    break;
        default:             break;
        }
        UT_ASSERT_MSG(0, "Unhandled Precision!");
        return false;
    }
    

    
    SYS_FORCE_INLINE void attribCast(GA_Group& group,const UT_StringHolder& newName)
    {
        attribCast(static_cast<GA_ElementGroup&>(group), newName);
    }
    




    
private:
    
    
    template<typename SCALAR_T, typename SCALAR_T_REF>
    SYS_FORCE_INLINE SCALAR_T scalarConvert(const SCALAR_T_REF inScalar)
    {
        return SCALAR_T(inScalar);
    }

#if 0
    template<>
    SYS_FORCE_INLINE int16 scalarConvert<int16, UT_StringHolder>(const UT_StringHolder inScalar)
    {
        return int16(atoi(inScalar.c_str()));
    }
    template<>
    SYS_FORCE_INLINE int32 scalarConvert<int32>(const char* const inScalar)
    {
        return atoi(inScalar);
    }
    template<>
    SYS_FORCE_INLINE int64 scalarConvert<int64>(const char* const inScalar)
    {
        return atol(inScalar);
    }
    template<>
    SYS_FORCE_INLINE fpreal16 scalarConvert<fpreal16>(const char* const inScalar)
    {
        return fpreal16(atof(inScalar));
    }
    template<>
    SYS_FORCE_INLINE fpreal32 scalarConvert<fpreal32>(const char* const inScalar)
    {
        return fpreal32(atof(inScalar));
    }
    template<>
    SYS_FORCE_INLINE fpreal64 scalarConvert<fpreal64>(const char* const inScalar)
    {
        return atof(inScalar);
    }
#else
    template<typename SCALAR_T>
    SYS_FORCE_INLINE SCALAR_T scalarConvertFromString(const char* const inScalar)
    {
        return atof(inScalar);
    }

    template<typename SCALAR_T>
    SYS_FORCE_INLINE SCALAR_T scalarConvertFromString(const UT_StringHolder& inScalar)
    {
        return scalarConvertFromString<SCALAR_T>(inScalar.c_str());
    }

    template<>
    SYS_FORCE_INLINE int16 scalarConvertFromString<int16>(const char* const inScalar)
    {
        return int16(atoi(inScalar));
    }
    template<>
    SYS_FORCE_INLINE int32 scalarConvertFromString<int32>(const char* const inScalar)
    {
        return atoi(inScalar);
    }
    template<>
    SYS_FORCE_INLINE int64 scalarConvertFromString<int64>(const char* const inScalar)
    {
        return atol(inScalar);
    }
    template<>
    SYS_FORCE_INLINE fpreal16 scalarConvertFromString<fpreal16>(const char* const inScalar)
    {
        return fpreal16(atof(inScalar));
    }
    template<>
    SYS_FORCE_INLINE fpreal32 scalarConvertFromString<fpreal32>(const char* const inScalar)
    {
        return fpreal32(atof(inScalar));
    }
    template<>
    SYS_FORCE_INLINE fpreal64 scalarConvertFromString<fpreal64>(const char* const inScalar)
    {
        return atof(inScalar);
    }
#endif
    
    template<>
    SYS_FORCE_INLINE UT_StringHolder scalarConvert<UT_StringHolder, int16>(const int16 inScalar)
    {
        char buffer[20];
        sprintf(buffer, "%s%d%s", prefix, inScalar, sufix);
        return UT_StringHolder(buffer);
    }
    template<>
    SYS_FORCE_INLINE UT_StringHolder scalarConvert<UT_StringHolder, int32>(const int32 inScalar)
    {
        char buffer[20];
        sprintf(buffer, "%s%d%s", prefix, inScalar, sufix);
        return UT_StringHolder(buffer);
    }
    template<>
    SYS_FORCE_INLINE UT_StringHolder scalarConvert<UT_StringHolder, int64>(const int64 inScalar)
    {
        char buffer[20];
        sprintf(buffer, "%s%I64d%s", prefix, inScalar, sufix);
        return UT_StringHolder(buffer);
    }
    template<>
    SYS_FORCE_INLINE UT_StringHolder scalarConvert<UT_StringHolder, fpreal16>(const fpreal16 inScalar)
    {
        char buffer[20];
        sprintf(buffer, "%s%f%s", prefix, fpreal32(inScalar), sufix);
        return UT_StringHolder(buffer);
    }
    template<>
    SYS_FORCE_INLINE UT_StringHolder scalarConvert<UT_StringHolder, fpreal32>(const fpreal32 inScalar)
    {
        char buffer[20];
        sprintf(buffer, "%s%f20%s", prefix, inScalar, sufix);
        return UT_StringHolder(buffer);
    }
    template<>
    SYS_FORCE_INLINE UT_StringHolder scalarConvert<UT_StringHolder, fpreal64>(const fpreal64 inScalar)
    {
        char buffer[20];
        sprintf(buffer, "%s%f20%s", prefix, inScalar, sufix);
        return UT_StringHolder(buffer);
    }
    
    template<typename SCALAR_T, typename SCALAR_T_REF>
    void
    attribDuplicate(
        GA_Attribute& attrib,
        const GA_Attribute& attribRef
    )
    {
        UTparallelFor(groupParser.getSplittableRange(attrib.getOwner()),
            [this, &attrib, &attribRef](const GA_SplittableRange& r)
        {
            GA_PageHandleT<SCALAR_T, SCALAR_T, true, true, GA_Attribute, GA_ATINumeric, GA_Detail> attrib_ph(&attrib);
            GA_PageHandleT<SCALAR_T_REF, SCALAR_T_REF, true, false, const GA_Attribute, const GA_ATINumeric, const GA_Detail> attribRef_ph(&attribRef);
            for (GA_PageIterator pit = r.beginPages(); !pit.atEnd(); ++pit)
            {
                GA_Offset start, end;
                for (GA_Iterator it(pit.begin()); it.blockAdvance(start, end); )
                {
                    attrib_ph.setPage(start);
                    attribRef_ph.setPage(start);
                    for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                    {
                        attrib_ph.value(elemoff) = scalarConvert<SCALAR_T, SCALAR_T_REF>(attribRef_ph.value(elemoff));
                    }
                }
            }
        }, subscribeRatio, minGrainSize);
    }



#if 0
    template<typename SCALAR_T_REF>
    void
        attribDuplicate<UT_StringHolder, SCALAR_T_REF>(
            GA_Attribute& attrib,
            const GA_Attribute& attribRef
            )
    {
        const GA_RWHandleS attrib_h(&attrib);
        UTparallelFor(groupParser.getSplittableRange(attrib.getOwner()),
            [this, &attrib_h, &attribRef](const GA_SplittableRange& r)
            {
                GA_PageHandleT<SCALAR_T_REF, SCALAR_T_REF, true, false, const GA_Attribute, const GA_ATINumeric, const GA_Detail> attribRef_ph(&attribRef);
                for (GA_PageIterator pit = r.beginPages(); !pit.atEnd(); ++pit)
                {
                    GA_Offset start, end;
                    for (GA_Iterator it(pit.begin()); it.blockAdvance(start, end); )
                    {
                        attribRef_ph.setPage(start);
                        for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                        {
                            attrib_h.set(elemoff, scalarConvert<UT_StringHolder, SCALAR_T_REF>(attribRef_ph.value(elemoff)));
                        }
                    }
                }
            }, subscribeRatio, minGrainSize);
    }
#else
    template<typename SCALAR_T_REF>
    void
        attribDuplicateString(
            GA_Attribute& attrib,
            const GA_Attribute& attribRef
        )
    {
        const GA_RWHandleS attrib_h(&attrib);
        UTparallelFor(groupParser.getSplittableRange(attrib.getOwner()),
            [this, &attrib_h, &attribRef](const GA_SplittableRange& r)
        {
            GA_PageHandleT<SCALAR_T_REF, SCALAR_T_REF, true, false, const GA_Attribute, const GA_ATINumeric, const GA_Detail> attribRef_ph(&attribRef);
            for (GA_PageIterator pit = r.beginPages(); !pit.atEnd(); ++pit)
            {
                GA_Offset start, end;
                for (GA_Iterator it(pit.begin()); it.blockAdvance(start, end); )
                {
                    attribRef_ph.setPage(start);
                    for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                    {
                        attrib_h.set(elemoff, scalarConvert<UT_StringHolder, SCALAR_T_REF>(attribRef_ph.get(elemoff)));
                    }
                }
            }
        }, subscribeRatio, minGrainSize);
    }
#endif



#if 0
    template<typename SCALAR_T>
    void
        attribDuplicate<SCALAR_T, UT_StringHolder>(
            GA_Attribute& attrib,
            const GA_Attribute& attribRef
        )
    {
        const GA_ROHandleS attribRef_h(&attribRef);
        UTparallelFor(groupParser.getSplittableRange(attrib.getOwner()),
            [this, &attrib, &attribRef_h](const GA_SplittableRange& r)
            {
                GA_PageHandleT<SCALAR_T, SCALAR_T, true, true, GA_Attribute, GA_ATINumeric, GA_Detail> attrib_ph(&attrib);
                for (GA_PageIterator pit = r.beginPages(); !pit.atEnd(); ++pit)
                {
                    GA_Offset start, end;
                    for (GA_Iterator it(pit.begin()); it.blockAdvance(start, end); )
                    {
                        attrib_ph.setPage(start);
                        for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                        {
                            attrib_ph.value(elemoff) = scalarConvert<SCALAR_T, UT_StringHolder>(attribRef_h.get(elemoff));
                        }
                    }
                }
            }, subscribeRatio, minGrainSize);
    }
#else
    template<typename SCALAR_T>
    void
        attribDuplicateFromString(
            GA_Attribute& attrib,
            const GA_Attribute& attribRef
            )
    {
        const GA_ROHandleS attribRef_h(&attribRef);
        UTparallelFor(groupParser.getSplittableRange(attrib.getOwner()),
            [this, &attrib, &attribRef_h](const GA_SplittableRange& r)
            {
                GA_PageHandleT<SCALAR_T, SCALAR_T, true, true, GA_Attribute, GA_ATINumeric, GA_Detail> attrib_ph(&attrib);
                for (GA_PageIterator pit = r.beginPages(); !pit.atEnd(); ++pit)
                {
                    GA_Offset start, end;
                    for (GA_Iterator it(pit.begin()); it.blockAdvance(start, end); )
                    {
                        attrib_ph.setPage(start);
                        for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                        {
                            attrib_ph.value(elemoff) = scalarConvertFromString<SCALAR_T>(attribRef_h.get(elemoff));
                        }
                    }
                }
            }, subscribeRatio, minGrainSize);
    }


#define GFE_ATTRIBDUPLICATE_SPECIALIZATION(SCALAR_T)              \
    template<>                                                    \
    SYS_FORCE_INLINE                                              \
    void                                                          \
        attribDuplicate<SCALAR_T, UT_StringHolder>(               \
            GA_Attribute& attrib,                                 \
            const GA_Attribute& attribRef                         \
            )                                                     \
    {                                                             \
        attribDuplicateFromString<SCALAR_T>(attrib, attribRef);   \
    }                                                             \


    GFE_ATTRIBDUPLICATE_SPECIALIZATION(int16)
    GFE_ATTRIBDUPLICATE_SPECIALIZATION(int32)
    GFE_ATTRIBDUPLICATE_SPECIALIZATION(int64)
    GFE_ATTRIBDUPLICATE_SPECIALIZATION(fpreal16)
    GFE_ATTRIBDUPLICATE_SPECIALIZATION(fpreal32)
    GFE_ATTRIBDUPLICATE_SPECIALIZATION(fpreal64)

#undef GFE_ATTRIBDUPLICATE_SPECIALIZATION
    


#define GFE_ATTRIBDUPLICATE_SPECIALIZATION(SCALAR_T_REF)          \
    template<>                                                    \
    SYS_FORCE_INLINE                                              \
    void                                                          \
        attribDuplicate<UT_StringHolder, SCALAR_T_REF>(           \
            GA_Attribute& attrib,                                 \
            const GA_Attribute& attribRef                         \
            )                                                     \
    {                                                             \
        attribDuplicateString<SCALAR_T_REF>(attrib, attribRef);   \
    }                                                             \


        GFE_ATTRIBDUPLICATE_SPECIALIZATION(int16)
        GFE_ATTRIBDUPLICATE_SPECIALIZATION(int32)
        GFE_ATTRIBDUPLICATE_SPECIALIZATION(int64)
        GFE_ATTRIBDUPLICATE_SPECIALIZATION(fpreal16)
        GFE_ATTRIBDUPLICATE_SPECIALIZATION(fpreal32)
        GFE_ATTRIBDUPLICATE_SPECIALIZATION(fpreal64)

#undef GFE_ATTRIBDUPLICATE_SPECIALIZATION

#endif


    // template<typename SCALAR_T>
    // void
    // attribDuplicate(
    //     GA_Attribute& attrib,
    //     const GA_Attribute& attribRef
    // )
    // {
    //     switch (attribRef.getAIFTuple()->getStorage(&attribRef))
    //     {
    //     case GA_STORE_INT16:  attribDuplicate<SCALAR_T, int16>          (attrib, attribRef);  break;
    //     case GA_STORE_INT32:  attribDuplicate<SCALAR_T, int32>          (attrib, attribRef);  break;
    //     case GA_STORE_INT64:  attribDuplicate<SCALAR_T, int64>          (attrib, attribRef);  break;
    //     case GA_STORE_REAL16: attribDuplicate<SCALAR_T, fpreal16>       (attrib, attribRef);  break;
    //     case GA_STORE_REAL32: attribDuplicate<SCALAR_T, fpreal32>       (attrib, attribRef);  break;
    //     case GA_STORE_REAL64: attribDuplicate<SCALAR_T, fpreal64>       (attrib, attribRef);  break;
    //     case GA_STORE_STRING: attribDuplicate<SCALAR_T, UT_StringHolder>(attrib, attribRef);  break;
    //     default: UT_ASSERT_MSG(0, "unhandled attrib storage"); break;
    //     }
    // }
    
    void
    attribDuplicate(
        GA_Attribute& attrib,
        const GA_Attribute& attribRef
    )
    {
        // GA_AttributeOwner owner = attrib.getOwner();
        // const GA_AIFStringTuple* a = attrib.getAIFStringTuple();
        // const GA_AIFSharedStringTuple* b = attrib.getAIFSharedStringTuple();
        UT_ASSERT_MSG(attrib.getOwner() == attribRef.getOwner(), "two attrib owner must be same");
        UT_ASSERT_P(attrib.getAIFTuple() || attrib.getAIFStringTuple() || attrib.getAIFSharedStringTuple());
        UT_ASSERT_P(attribRef.getAIFTuple() || attribRef.getAIFStringTuple() || attribRef.getAIFSharedStringTuple());

        const GA_AIFTuple* aifTuple = attribRef.getAIFTuple();

        GA_Storage storageRef;
        if (aifTuple)
            storageRef = aifTuple->getStorage(&attribRef);
        else if(attribRef.getAIFStringTuple())
            storageRef = GA_STORE_STRING;
        else
            UT_ASSERT_MSG(0, "impossible");
            

        aifTuple = attrib.getAIFTuple();
        if(aifTuple)
        {
            switch (aifTuple->getStorage(&attrib))
            {
            case GA_STORE_INT16:
                switch (storageRef)
                {
                    //case GA_STORE_INT16:  attribDuplicate<int16, int16>                                break;
                    case GA_STORE_INT32:  attribDuplicate<int16, int32>           (attrib, attribRef); break;
                    case GA_STORE_INT64:  attribDuplicate<int16, int64>           (attrib, attribRef); break;
                    case GA_STORE_REAL16: attribDuplicate<int16, fpreal16>        (attrib, attribRef); break;
                    case GA_STORE_REAL32: attribDuplicate<int16, fpreal32>        (attrib, attribRef); break;
                    case GA_STORE_REAL64: attribDuplicate<int16, fpreal64>        (attrib, attribRef); break;
                    case GA_STORE_STRING: attribDuplicate<int16, UT_StringHolder> (attrib, attribRef); break;
                    default: UT_ASSERT_MSG(0, "unhandled attrib storage"); break;
                }
                break;
            case GA_STORE_INT32:
                switch (storageRef)
                {
                    case GA_STORE_INT16:  attribDuplicate<int32, int16>           (attrib, attribRef); break;
                    //case GA_STORE_INT32:  attribDuplicate<int32, int32>           (attrib, attribRef); break;
                    case GA_STORE_INT64:  attribDuplicate<int16, int64>           (attrib, attribRef); break;
                    case GA_STORE_REAL16: attribDuplicate<int16, fpreal16>        (attrib, attribRef); break;
                    case GA_STORE_REAL32: attribDuplicate<int16, fpreal32>        (attrib, attribRef); break;
                    case GA_STORE_REAL64: attribDuplicate<int16, fpreal64>        (attrib, attribRef); break;
                    case GA_STORE_STRING: attribDuplicate<int16, UT_StringHolder> (attrib, attribRef); break;
                    default: UT_ASSERT_MSG(0, "unhandled attrib storage"); break;
                }
                break;
            case GA_STORE_INT64:
                switch (storageRef)
                {
                    case GA_STORE_INT16:  attribDuplicate<int64, int16>           (attrib, attribRef); break;
                    case GA_STORE_INT32:  attribDuplicate<int64, int32>           (attrib, attribRef); break;
                    //case GA_STORE_INT64:  attribDuplicate<int64, int64>           (attrib, attribRef); break;
                    case GA_STORE_REAL16: attribDuplicate<int64, fpreal16>        (attrib, attribRef); break;
                    case GA_STORE_REAL32: attribDuplicate<int64, fpreal32>        (attrib, attribRef); break;
                    case GA_STORE_REAL64: attribDuplicate<int64, fpreal64>        (attrib, attribRef); break;
                    case GA_STORE_STRING: attribDuplicate<int64, UT_StringHolder> (attrib, attribRef); break;
                    default: UT_ASSERT_MSG(0, "unhandled attrib storage"); break;
                }
                break;
            case GA_STORE_REAL16:
                switch (storageRef)
                {
                    case GA_STORE_INT16:  attribDuplicate<fpreal16, int16>           (attrib, attribRef); break;
                    case GA_STORE_INT32:  attribDuplicate<fpreal16, int32>           (attrib, attribRef); break;
                    case GA_STORE_INT64:  attribDuplicate<fpreal16, int64>           (attrib, attribRef); break;
                    //case GA_STORE_REAL16: attribDuplicate<fpreal16, fpreal16>        (attrib, attribRef); break;
                    case GA_STORE_REAL32: attribDuplicate<fpreal16, fpreal32>        (attrib, attribRef); break;
                    case GA_STORE_REAL64: attribDuplicate<fpreal16, fpreal64>        (attrib, attribRef); break;
                    case GA_STORE_STRING: attribDuplicate<fpreal16, UT_StringHolder> (attrib, attribRef); break;
                    default: UT_ASSERT_MSG(0, "unhandled attrib storage"); break;
                }
                break;
            case GA_STORE_REAL32:
                switch (storageRef)
                {
                    case GA_STORE_INT16:  attribDuplicate<fpreal32, int16>           (attrib, attribRef); break;
                    case GA_STORE_INT32:  attribDuplicate<fpreal32, int32>           (attrib, attribRef); break;
                    case GA_STORE_INT64:  attribDuplicate<fpreal32, int64>           (attrib, attribRef); break;
                    case GA_STORE_REAL16: attribDuplicate<fpreal32, fpreal16>        (attrib, attribRef); break;
                    //case GA_STORE_REAL32: attribDuplicate<fpreal32, fpreal32>        (attrib, attribRef); break;
                    case GA_STORE_REAL64: attribDuplicate<fpreal32, fpreal64>        (attrib, attribRef); break;
                    case GA_STORE_STRING: attribDuplicate<fpreal32, UT_StringHolder> (attrib, attribRef); break;
                    default: UT_ASSERT_MSG(0, "unhandled attrib storage"); break;
                }
                break;
            case GA_STORE_REAL64:
                switch (storageRef)
                {
                    case GA_STORE_INT16:  attribDuplicate<fpreal64, int16>           (attrib, attribRef); break;
                    case GA_STORE_INT32:  attribDuplicate<fpreal64, int32>           (attrib, attribRef); break;
                    case GA_STORE_INT64:  attribDuplicate<fpreal64, int64>           (attrib, attribRef); break;
                    case GA_STORE_REAL16: attribDuplicate<fpreal64, fpreal16>        (attrib, attribRef); break;
                    case GA_STORE_REAL32: attribDuplicate<fpreal64, fpreal32>        (attrib, attribRef); break;
                    //case GA_STORE_REAL64: attribDuplicate<fpreal64, fpreal64>        (attrib, attribRef); break;
                    case GA_STORE_STRING: attribDuplicate<fpreal64, UT_StringHolder> (attrib, attribRef); break;
                    default: UT_ASSERT_MSG(0, "unhandled attrib storage"); break;
                }
                break;
            case GA_STORE_STRING:
                switch (storageRef)
                {
    #if 0
                    case GA_STORE_INT16:  attribDuplicateString<int16>               (attrib, attribRef); break;
                    case GA_STORE_INT32:  attribDuplicateString<int32>               (attrib, attribRef); break;
                    case GA_STORE_INT64:  attribDuplicateString<int64>               (attrib, attribRef); break;
                    case GA_STORE_REAL16: attribDuplicateString<fpreal16>            (attrib, attribRef); break;
                    case GA_STORE_REAL32: attribDuplicateString<fpreal32>            (attrib, attribRef); break;
                    case GA_STORE_REAL64: attribDuplicateString<fpreal64>            (attrib, attribRef); break;
    #else
                    case GA_STORE_INT16:  attribDuplicate<UT_StringHolder, int16>    (attrib, attribRef); break;
                    case GA_STORE_INT32:  attribDuplicate<UT_StringHolder, int32>    (attrib, attribRef); break;
                    case GA_STORE_INT64:  attribDuplicate<UT_StringHolder, int64>    (attrib, attribRef); break;
                    case GA_STORE_REAL16: attribDuplicate<UT_StringHolder, fpreal16> (attrib, attribRef); break;
                    case GA_STORE_REAL32: attribDuplicate<UT_StringHolder, fpreal32> (attrib, attribRef); break;
                    case GA_STORE_REAL64: attribDuplicate<UT_StringHolder, fpreal64> (attrib, attribRef); break;
    #endif
                    //case GA_STORE_STRING: attribDuplicate<UT_StringHolder, UT_StringHolder> (attrib, attribRef); break;
                    default: UT_ASSERT_MSG(0, "unhandled attrib storage"); break;
                }
                break;
            default: UT_ASSERT_MSG(0, "unhandled attrib storage"); break;
            }
        }
        else
        {
            //attrib.getAIFStringTuple()
            //attrib.getAIFSharedStringTuple();
            switch (storageRef)
            {
#if 0
            case GA_STORE_INT16:  attribDuplicateString<int16>               (attrib, attribRef); break;
            case GA_STORE_INT32:  attribDuplicateString<int32>               (attrib, attribRef); break;
            case GA_STORE_INT64:  attribDuplicateString<int64>               (attrib, attribRef); break;
            case GA_STORE_REAL16: attribDuplicateString<fpreal16>            (attrib, attribRef); break;
            case GA_STORE_REAL32: attribDuplicateString<fpreal32>            (attrib, attribRef); break;
            case GA_STORE_REAL64: attribDuplicateString<fpreal64>            (attrib, attribRef); break;
#else
            case GA_STORE_INT16:  attribDuplicate<UT_StringHolder, int16>    (attrib, attribRef); break;
            case GA_STORE_INT32:  attribDuplicate<UT_StringHolder, int32>    (attrib, attribRef); break;
            case GA_STORE_INT64:  attribDuplicate<UT_StringHolder, int64>    (attrib, attribRef); break;
            case GA_STORE_REAL16: attribDuplicate<UT_StringHolder, fpreal16> (attrib, attribRef); break;
            case GA_STORE_REAL32: attribDuplicate<UT_StringHolder, fpreal32> (attrib, attribRef); break;
            case GA_STORE_REAL64: attribDuplicate<UT_StringHolder, fpreal64> (attrib, attribRef); break;
#endif
            //case GA_STORE_STRING: attribDuplicate<UT_StringHolder, UT_StringHolder> (attrib, attribRef); break;
            default: UT_ASSERT_MSG(0, "unhandled attrib storage"); break;
            }
        }
    }
    
    // void
    // attribDuplicate(
    //     GA_Attribute& attrib,
    //     const GA_Attribute& attribRef
    // )
    // {
    //     UT_ASSERT_MSG(attrib.getOwner() == attribRef.getOwner(), "two attrib owner must be same");
    //     
    //     UT_ASSERT_P(attrib.getAIFTuple());
    //     switch (attrib.getAIFTuple()->getStorage(&attrib))
    //     {
    //     case GA_STORE_INT16:  attribDuplicate<int16>          (attrib, attribRef); break;
    //     case GA_STORE_INT32:  attribDuplicate<int32>          (attrib, attribRef); break;
    //     case GA_STORE_INT64:  attribDuplicate<int64>          (attrib, attribRef); break;
    //     case GA_STORE_REAL16: attribDuplicate<fpreal16>       (attrib, attribRef); break;
    //     case GA_STORE_REAL32: attribDuplicate<fpreal32>       (attrib, attribRef); break;
    //     case GA_STORE_REAL64: attribDuplicate<fpreal64>       (attrib, attribRef); break;
    //     case GA_STORE_STRING: attribDuplicate<UT_StringHolder>(attrib, attribRef);  break;
    //     default: UT_ASSERT_MSG(0, "unhandled attrib storage"); break;
    //     }
    // }

    template<typename T>
    void
    setAttribValueTo1(
        GA_Attribute& attrib,
        const GA_SplittableRange& geoSplittableRange
    )
    {
        UTparallelFor(geoSplittableRange, [&attrib](const GA_SplittableRange& r)
        {
            GA_PageHandleScalar<GA_Offset>::RWType attrib_ph(&attrib);
            for (GA_PageIterator pit = r.beginPages(); !pit.atEnd(); ++pit)
            {
                GA_Offset start, end;
                for (GA_Iterator it(pit.begin()); it.blockAdvance(start, end); )
                {
                    attrib_ph.setPage(start);
                    for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                    {
                        attrib_ph.value(elemoff) = T(1);
                    }
                }
            }
        }, subscribeRatio, minGrainSize);
    }

    template<>
    void
    setAttribValueTo1<UT_StringHolder>(
        GA_Attribute& attrib,
        const GA_SplittableRange& geoSplittableRange
    )
    {
        const GA_RWHandleS attrib_h(&attrib);
        UTparallelFor(geoSplittableRange, [&attrib_h](const GA_SplittableRange& r)
        {
            GA_Offset start, end;
            for (GA_Iterator it(r); it.blockAdvance(start, end); )
            {
                for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                {
                    attrib_h.set(elemoff, "1");
                }
            }
        }, subscribeRatio, minGrainSize);
    }






//#undef GFE_TEMP_ATTRIBCAST_ATTRIBNAME
    

public:
        GA_StorageClass newStorageClass = GA_STORECLASS_INVALID;
        GA_Precision precision = GA_PRECISION_INVALID;
        bool delOrigin = true;

    
        const char* prefix = "";
        const char* sufix  = "";

private:

        bool renameAttrib = false;
        bool renameGroup = false;
        // ::std::string newAttribNames;
        // ::std::string newGroupNames;
        UFE_SplittableString newAttribNames;
        UFE_SplittableString newGroupNames;
        //UT_String newAttribNames;
        //UT_String newGroupNames;

        exint subscribeRatio = 64;
        exint minGrainSize = 1024;

}; // End of class GFE_AttribCast













































//
//
// namespace GFE_AttribCast_Namespace {
//
//     
//
//     template<typename T>
//     static void
//     setAttribValue1(
//         GA_Detail& geo,
//         GA_Attribute& attrib,
//         const GA_SplittableRange& geoSplittableRange,
//         const exint subscribeRatio = 64,
//         const exint minGrainSize = 128
//     )
//     {
//         const GA_RWHandleT<T> attrib_h(&attrib);
//         UTparallelFor(geoSplittableRange, [&geo, &attrib_h](const GA_SplittableRange& r)
//         {
//             GA_Offset start, end;
//             for (GA_Iterator it(r); it.blockAdvance(start, end); )
//             {
//                 for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
//                 {
//                     attrib_h.set(elemoff, T(1));
//                 }
//             }
//         }, subscribeRatio, minGrainSize);
//     }
//
//     static void
//         setAttribStringValue1(
//             GA_Detail& geo,
//             GA_Attribute& attrib,
//             const GA_SplittableRange& geoSplittableRange,
//             const exint subscribeRatio = 64,
//             const exint minGrainSize = 128
//         )
//     {
//         const GA_RWHandleS attrib_h(&attrib);
//         UTparallelFor(geoSplittableRange, [&geo, &attrib_h](const GA_SplittableRange& r)
//         {
//             GA_Offset start, end;
//             for (GA_Iterator it(r); it.blockAdvance(start, end); )
//             {
//                 for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
//                 {
//                     attrib_h.set(elemoff, "1");
//                 }
//             }
//         }, subscribeRatio, minGrainSize);
//     }
//
//
//
//     static bool
//         attribCast(
//             GA_Detail& geo,
//             const GA_Group& group,
//             const GA_StorageClass newStorageClass,
//             const UT_StringHolder& newName,
//             const GA_Precision precision = GA_PRECISION_INVALID,
//             const exint subscribeRatio = 64,
//             const exint minGrainSize = 128
//         )
//     {
//         if (newStorageClass == GA_STORECLASS_OTHER)//group
//             return false;
//
//         if (!group.isElementGroup())
//             return false;
//
//         const GA_Precision precisionFinal = precision == GA_PRECISION_INVALID ? geo.getPreferredPrecision() : precision;
//         const UT_StringHolder& newNameFinal = (newName.isstring() && newName.length() != 0) ? group.getName() : newName;
//         //const GA_GroupType classType = group->classType();
//         const GA_AttributeOwner attribClass = GFE_Type::attributeOwner_groupType(group.classType());
//
//         GA_SplittableRange geoSplittableRange = GFE_Group::getSplittableRangeByAnyGroup(geo, &group);
//
//         switch (newStorageClass)
//         {
//         case GA_STORECLASS_INT:
//         {
//             GA_Attribute& attrib = *geo.getAttributes().createTupleAttribute(attribClass, newNameFinal, GFE_Type::getPreferredStorageI(precisionFinal), 1, GA_Defaults(0));
//             
//             switch (precisionFinal)
//             {
//             case GA_PRECISION_8:
//                 setAttribValue1<int8> (geo, attrib, geoSplittableRange, subscribeRatio, minGrainSize);
//                 return true; break;
//             case GA_PRECISION_16:
//                 setAttribValue1<int16>(geo, attrib, geoSplittableRange, subscribeRatio, minGrainSize);
//                 return true; break;
//             case GA_PRECISION_32:
//                 setAttribValue1<int32>(geo, attrib, geoSplittableRange, subscribeRatio, minGrainSize);
//                 return true; break;
//             case GA_PRECISION_64:
//                 setAttribValue1<int64>(geo, attrib, geoSplittableRange, subscribeRatio, minGrainSize);
//                 return true; break;
//             default:         break;
//             }
//         }
//             break;
//         case GA_STORECLASS_REAL:
//         {
//             GA_Attribute& attrib = *geo.getAttributes().createTupleAttribute(attribClass, newNameFinal, GFE_Type::getPreferredStorageF(precisionFinal), 1, GA_Defaults(0));
//             switch (precisionFinal)
//             {
//             case GA_PRECISION_16:
//                 setAttribValue1<fpreal16>(geo, attrib, geoSplittableRange, subscribeRatio, minGrainSize);
//                 return true; break;
//             case GA_PRECISION_32:
//                 setAttribValue1<fpreal32>(geo, attrib, geoSplittableRange, subscribeRatio, minGrainSize);
//                 return true; break;
//             case GA_PRECISION_64:
//                 setAttribValue1<fpreal64>(geo, attrib, geoSplittableRange, subscribeRatio, minGrainSize);
//                 return true; break;
//             default:         break;
//             }
//         }
//             break;
//         case GA_STORECLASS_STRING:
//         {
//             GA_Attribute& attrib = *geo.getAttributes().createStringAttribute(attribClass, newNameFinal, 1);
//             setAttribStringValue1(geo, attrib, geoSplittableRange, subscribeRatio, minGrainSize);
//             return true;
//         }
//             break;
//         case GA_STORECLASS_DICT:
//             break;
//
//         case GA_STORECLASS_OTHER:
//             return false;
//             break;
//         default:
//             break;
//         }
//         //return geo->changeAttributeStorage(attribClass, group->getName(), newStorage);
//         UT_ASSERT_MSG(0, "Unhandled Precision!");
//         return false;
//     }
//
//     SYS_FORCE_INLINE
//     static bool
//     attribCast(
//         GA_Detail& geo,
//         GA_Group& group,
//         const GA_StorageClass newStorageClass,
//         const UT_StringHolder& newName,
//         const GA_Precision precision = GA_PRECISION_INVALID,
//         const bool delOriginal = false,
//         const exint subscribeRatio = 64,
//         const exint minGrainSize = 128
//     )
//     {
//         const bool success = attribCast(geo, static_cast<const GA_Group&>(group), newStorageClass, newName, precision, subscribeRatio, minGrainSize);
//         if (delOriginal)
//             geo.destroyGroup(&group);
//         
//         return success;
//     }
//
//
//     
//
//     static bool
//         changeAttribStorageClass(
//             GA_Detail& geo,
//             const GA_AttributeOwner attribClass,
//             const UT_StringHolder& attribName,
//             const GA_StorageClass newStorageClass,
//             const GA_Precision precision = GA_PRECISION_INVALID
//         )
//     {
//         GEO_Detail& geoGEO = static_cast<GEO_Detail&>(geo);
//         const GA_Precision precisionFinal = precision == GA_PRECISION_INVALID ? geo.getPreferredPrecision() : precision;
//
//         //GA_Attribute* attrib = nullptr;
//         switch (newStorageClass)
//         {
//         case GA_STORECLASS_INT:
//         {
//             //attrib = geo->getAttributes().createTupleAttribute(attribClass, newNameFinal, GFE_Type::getPreferredStorageI(precisionFinal), 1, GA_Defaults(0));
//
//             switch (precision)
//             {
//             case GA_PRECISION_8:
//                 geoGEO.changeAttributeStorage(attribClass, attribName, GA_STORE_INT8);
//                 break;
//             case GA_PRECISION_16:
//                 geoGEO.changeAttributeStorage(attribClass, attribName, GA_STORE_INT16);
//                 break;
//             case GA_PRECISION_32:
//                 geoGEO.changeAttributeStorage(attribClass, attribName, GA_STORE_INT32);
//                 break;
//             case GA_PRECISION_64:
//                 geoGEO.changeAttributeStorage(attribClass, attribName, GA_STORE_INT64);
//                 break;
//             default:         break;
//             }
//         }
//         break;
//         case GA_STORECLASS_REAL:
//         {
//             //attrib = geo->getAttributes().createTupleAttribute(attribClass, newNameFinal, GFE_Type::getPreferredStorageF(precisionFinal), 1, GA_Defaults(0));
//
//             switch (precision)
//             {
//             case GA_PRECISION_16:
//                 geoGEO.changeAttributeStorage(attribClass, attribName, GA_STORE_REAL16);
//                 break;
//             case GA_PRECISION_32:
//                 geoGEO.changeAttributeStorage(attribClass, attribName, GA_STORE_REAL32);
//                 break;
//             case GA_PRECISION_64:
//                 geoGEO.changeAttributeStorage(attribClass, attribName, GA_STORE_REAL64);
//                 break;
//             default:         break;
//             }
//         }
//         break;
//         case GA_STORECLASS_STRING:
//         {
//             geoGEO.changeAttributeStorage(attribClass, attribName, GA_STORE_STRING);
//             return true;
//         }
//         break;
//         case GA_STORECLASS_DICT:
//             geoGEO.changeAttributeStorage(attribClass, attribName, GA_STORE_DICT);
//             return false;
//             break;
//         case GA_STORECLASS_OTHER:
//             return false;
//             break;
//         default:
//             UT_ASSERT_MSG(0, "Unhandled Precision!");
//             break;
//         }
//         //return geo->changeAttributeStorage(attribClass, group->getName(), newStorage);
//         return true;
//     }
//
//     static bool
//         attribCast(
//             GA_Detail& geo,
//             const GA_Attribute& attribPtr,
//             const GA_StorageClass newStorageClass,
//             const UT_StringHolder& newName,
//             const GA_Precision precision = GA_PRECISION_INVALID,
//             const exint subscribeRatio = 64,
//             const exint minGrainSize = 128
//         )
//     {
//         const GA_StorageClass inStorageClass = attribPtr.getStorageClass();
//         if (newStorageClass == inStorageClass)
//             return false;
//
//         const GA_AttributeOwner attribClass = attribPtr.getOwner();
//         const UT_StringHolder& attribName = attribPtr.getName();
//
//         const UT_StringHolder& newNameFinal = (newName.isstring() && newName.length() != 0) ? attribName : newName;
//
//         return changeAttribStorageClass(geo, attribClass, attribName, newStorageClass, precision);
//     }
//
//     //SYS_FORCE_INLINE
//     //    static bool
//     //    attribCast(
//     //        GA_Detail& geo,
//     //        GA_Attribute& attribPtr,
//     //        const GA_StorageClass newStorageClass,
//     //        const UT_StringHolder& newName,
//     //        const GA_Precision precision = GA_PRECISION_INVALID,
//     //        const bool delOriginal = false,
//     //        const exint subscribeRatio = 64,
//     //        const exint minGrainSize = 128
//     //    )
//     //{
//     //    const bool success = attribCast(geo, static_cast<const GA_Attribute*>(attribPtr), newStorageClass, newName, precision, subscribeRatio, minGrainSize);
//     //    if (delOriginal) {
//     //        GFE_Attribute::destroyAttribute(attribPtr);
//     //    }
//     //    return success;
//     //}
//
//
//
//
// } // End of namespace GFE_AttributeCast

#endif