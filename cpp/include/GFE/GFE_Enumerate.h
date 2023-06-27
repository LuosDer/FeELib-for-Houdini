
#pragma once

#ifndef __GFE_Enumerate_h__
#define __GFE_Enumerate_h__

#include "GFE/GFE_Enumerate.h"



#include "GFE/GFE_GeoFilter.h"



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

    // can not use in parallel unless for each GA_Detail
    virtual bool
        computeCore() override
    {
        if (getOutAttribArray().isEmpty())
            return false;

        if (groupParser.isEmpty())
            return true;
        
        
        const size_t size = getOutAttribArray().size();
        for (size_t i = 0; i < size; i++)
        {
            attribPtr = getOutAttribArray()[i];
            // const GA_Storage storage = attribPtr->getAIFTuple()->getStorage(attribPtr);
            switch (attribPtr->getAIFTuple()->getStorage(attribPtr))
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


    template<typename T>
    void enumerate()
    {
        UT_ASSERT_P(attribPtr);
        const GA_SplittableRange geoSplittableRange0(groupParser.getRange(attribPtr));
        if(outAsOffset)
        {
            UTparallelFor(geoSplittableRange0, [this](const GA_SplittableRange& r)
            {
                GA_PageHandleT<T, T, true, true, GA_Attribute, GA_ATINumeric, GA_Detail> attrib_ph(attribPtr);
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
            const GA_IndexMap& indexMap = attribPtr->getIndexMap();
            if (negativeIndex)
            {
                UTparallelFor(geoSplittableRange0, [this, &indexMap](const GA_SplittableRange& r)
                {
                    GA_PageHandleT<T, T, true, true, GA_Attribute, GA_ATINumeric, GA_Detail> attrib_ph(attribPtr);
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
                    GA_PageHandleT<T, T, true, true, GA_Attribute, GA_ATINumeric, GA_Detail> attrib_ph(attribPtr);
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
        UT_ASSERT_P(attribPtr);

        const GA_RWHandleS attrib_h(attribPtr);
        const GA_SplittableRange geoSplittableRange0(groupParser.getRange(attribPtr->getOwner()));
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
            const GA_IndexMap& indexMap = attribPtr->getIndexMap();
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
    

private:
    
    GA_Attribute* attribPtr;
    exint subscribeRatio = 64;
    exint minGrainSize = 1024;


}; // End of class GFE_Enumerate





#endif
