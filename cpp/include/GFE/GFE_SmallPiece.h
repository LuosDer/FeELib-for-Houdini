
#pragma once

#ifndef __GFE_SmallPiece_h__
#define __GFE_SmallPiece_h__

#include "GFE/GFE_SmallPiece.h"

#include "GFE/GFE_GeoFilter.h"


class GFE_SmallPiece : public GFE_AttribFilter {

public:
    using GFE_AttribFilter::GFE_AttribFilter;
    
    void
        setComputeParm(
            const exint subscribeRatio = 64,
            const exint minGrainSize   = 64
        )
    {
        setHasComputed();
        this->subscribeRatio = subscribeRatio;
        this->minGrainSize   = minGrainSize;
    }

private:

    // can not use in parallel unless for each GA_Detail
    virtual bool
        computeCore() override
    {
        if (getOutAttribArray().isEmpty())
            return false;

        if (groupParser.isEmpty())
            return true;
        
        
        attribPtr = getOutAttribArray()[0];
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

        return true;
    }


    template<typename T>
    void enumerate()
    {
        UT_ASSERT_P(attribPtr);
        const GA_SplittableRange geoSplittableRange0(groupParser.getRange(attribPtr->getOwner()));
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

    


public:
    

private:
    
    exint subscribeRatio = 64;
    exint minGrainSize = 1024;


}; // End of class GFE_SmallPiece


#endif
