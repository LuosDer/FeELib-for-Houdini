
#pragma once

#ifndef __GFE_CornerPoint_h__
#define __GFE_CornerPoint_h__

//#include "GFE/GFE_CornerPoint.h"


#include "GFE/GFE_GeoFilter.h"




class GFE_CornerPoint : public GFE_AttribFilter {



public:
    using GFE_AttribFilter::GFE_AttribFilter;

    ~GFE_CornerPoint()
    {
    }

    void
        setComputeParm(
            const bool delCornerPoint = false,
            const fpreal threshold = 1e-05,

            const bool delCornerPoint = false,
            
            const exint subscribeRatio = 64,
            const exint minGrainSize = 1024
        )
    {
        setHasComputed();
        this->threshold = threshold;
        this->delCornerPoint = delCornerPoint;
        this->subscribeRatio = subscribeRatio;
        this->minGrainSize = minGrainSize;
    }

    GA_PointGroup*
    findOrCreateGroup(
        const bool detached = false,
        const UT_StringRef& groupName = ""
    )
    {
        outGroup = getOutGroupArray().findOrCreatePoint(detached, groupName);
    }

private:

    // can not use in parallel unless for each GA_Detail
    virtual bool
        computeCore() override
    {
        if (getOutGroupArray().isEmpty())
            return false;

        if (groupParser.isEmpty())
            return true;
        
        if (!posAttrib)
            posAttrib = geo->getP();
        
        
        GA_Attribute* const attribPtr = getOutAttribArray()[0];
        // const GA_Storage storage = attribPtr->getAIFTuple()->getStorage(attribPtr);
        switch (attribPtr->getAIFTuple()->getStorage(attribPtr))
        {
        case GA_STORE_REAL16: cornerPoint<fpreal16>(); break;
        case GA_STORE_REAL32: cornerPoint<fpreal32>(); break;
        case GA_STORE_REAL64: cornerPoint<fpreal64>(); break;
        default: break;
        }


        return true;
    }


    template<typename T>
    void cornerPoint()
    {
        UT_ASSERT_P(cornerPointGroup);
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
                            cornerPointGroup->setElement
                            attrib_ph.value(elemoff) = elemoff;
                        }
                    }
                }
            }, subscribeRatio, minGrainSize);
        }
        else
        {
            const GA_IndexMap& indexMap = attribPtr->getIndexMap();
            UTparallelFor(geoSplittableRange0, [attribPtr, &indexMap](const GA_SplittableRange& r)
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
                            attrib_ph.value(elemoff) = indexMap.indexFromOffset(elemoff);
                        }
                    }
                }
            }, subscribeRatio, minGrainSize);
        }
    }



public:
    fpreal threshold = 1e-05;
    bool outAsOffset = true;

private:
    GA_Attribute* posAttrib = nullptr;
    GA_Attribute* normalAttrib = nullptr;
    GA_PointGroup* cornerPointGroup = nullptr;
    
    exint subscribeRatio = 64;
    exint minGrainSize = 1024;


}; // End of class GFE_CornerPoint



#endif
