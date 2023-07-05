
#pragma once

#ifndef __GFE_OffsetAttributeToIndex_h__
#define __GFE_OffsetAttributeToIndex_h__

#include "GFE/GFE_OffsetAttributeToIndex.h"

#include "GFE/GFE_GeoFilter.h"




class GFE_OffsetAttribToIndex : public GFE_AttribFilter {

public:
    using GFE_AttribFilter::GFE_AttribFilter;

    
    void
    setComputeParm(
        const bool offsetToIndex = false,
        const exint subscribeRatio = 64,
        const exint minGrainSize = 1024
    )
    {
        setHasComputed();
        this->offsetToIndex = offsetToIndex;
        this->subscribeRatio = subscribeRatio;
        this->minGrainSize = minGrainSize;
    }

private:

    virtual bool
        computeCore() override
    {
        const size_t sizeAttrib = getOutAttribArray().size();
        for (size_t i = 0; i < sizeAttrib; i++)
        {
            GA_Attribute& attrib = *getOutAttribArray()[i];
            //const char* name = attrib.getName().c_str();
            switch (attrib.getType().getTypeId())
            {
            case 0:                                     break; // unknown
            case 1:                                     break; // topo
            case 2:  offsetTupleAttribToIndex(attrib);  break; // int float vector(tuple)
            case 3:                                     break; // string
            case 4:                                     break; // unknown
            case 5:                                     break; // unknown
            case 6:  offsetArrayAttribToIndex(attrib);  break; // int array
            case 7:                                     break; // unknown
            case 8:  offsetArrayAttribToIndex(attrib);  break; // float array
            case 9:                                     break; // unknown
            case 10:                                    break; // string array
            case 11:                                    break; // dict
            case 12:                                    break; // dict array
            default:                                    break;
            }
        }

        return true;
    }


    void offsetTupleAttribToIndex(GA_Attribute& attrib)
    {
        
        const GA_IndexMap& indexMap = attrib.getIndexMap();
#if 1
        const GA_SplittableRange geoSplittableRange0 = groupParser.getSplittableRange(attrib);
#else
        const GA_Range geoRange0(indexMap);
        const GA_SplittableRange geoSplittableRange0(geoRange0);
#endif
        if (offsetToIndex)
        {
            const GA_Offset offsetSize = indexMap.lastOffset();
            UTparallelFor(geoSplittableRange0, [&indexMap, &attrib, offsetSize](const GA_SplittableRange& r)
            {
                GA_Offset attribVal;
                GA_PageHandleScalar<GA_Offset>::RWType attrib_ph(&attrib);
                for (GA_PageIterator pit = r.beginPages(); !pit.atEnd(); ++pit)
                {
                    GA_Offset start, end;
                    for (GA_Iterator it(pit.begin()); it.blockAdvance(start, end); )
                    {
                        attrib_ph.setPage(start);
                        for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                        {
                            attribVal = attrib_ph.value(elemoff);
                            if (attribVal < 0 || attribVal >= offsetSize)
                                continue;
                            attrib_ph.value(elemoff) = indexMap.indexFromOffset(attribVal);
                        }
                    }
                }
            }, subscribeRatio, minGrainSize);
        }
        else
        {
            const GA_Index indexSize = indexMap.indexSize();
            UTparallelFor(geoSplittableRange0, [&indexMap, &attrib, indexSize](const GA_SplittableRange& r)
            {
                GA_Index attribVal;
                GA_PageHandleScalar<GA_Offset>::RWType attrib_ph(&attrib);
                for (GA_PageIterator pit = r.beginPages(); !pit.atEnd(); ++pit)
                {
                    GA_Offset start, end;
                    for (GA_Iterator it(pit.begin()); it.blockAdvance(start, end); )
                    {
                        attrib_ph.setPage(start);
                        for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                        {
                            attribVal = attrib_ph.value(elemoff);
                            if (attribVal < 0 || attribVal >= indexSize)
                                continue;
                            attrib_ph.value(elemoff) = indexMap.offsetFromIndex(attribVal);
                        }
                    }
                }
            }, subscribeRatio, minGrainSize);
        }
    }



    void offsetArrayAttribToIndex(GA_Attribute& attrib)
    {
        GA_RWHandleT<UT_ValArray<GA_Offset>> attrib_h(&attrib);
        
        const GA_IndexMap& indexMap = attrib.getIndexMap();
#if 1
        const GA_SplittableRange geoSplittableRange0 = groupParser.getSplittableRange(attrib);
#else
        const GA_Range geoRange0(indexMap);
        const GA_SplittableRange geoSplittableRange0(geoRange0);
#endif
        if (offsetToIndex)
        {
            const GA_Offset offsetSize = indexMap.lastOffset();
            UTparallelFor(geoSplittableRange0, [&indexMap, &attrib_h, offsetSize](const GA_SplittableRange& r)
            {
                UT_ValArray<GA_Offset> attribVal;
                GA_Offset start, end;
                for (GA_Iterator it(r); it.blockAdvance(start, end); )
                {
                    for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                    {
                        attrib_h.get(elemoff, attribVal);
                        const size_t arrayLen = attribVal.size();
                        for (size_t i = 0; i < arrayLen; ++i)
                        {
                            if (attribVal[i] < 0 || attribVal[i] >= offsetSize)
                                continue;
                            attribVal[i] = indexMap.indexFromOffset(attribVal[i]);
                        }
                        attrib_h.set(elemoff, attribVal);
                    }
                }
            }, subscribeRatio, minGrainSize);
        }
        else
        {
            const GA_Index indexSize = indexMap.indexSize();
            UTparallelFor(geoSplittableRange0, [&indexMap, &attrib_h, indexSize](const GA_SplittableRange& r)
            {
                UT_ValArray<GA_Offset> attribVal;
                GA_Offset start, end;
                for (GA_Iterator it(r); it.blockAdvance(start, end); )
                {
                    for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                    {
                        attrib_h.get(elemoff, attribVal);
                        const size_t arrayLen = attribVal.size();
                        for (size_t i = 0; i < arrayLen; ++i)
                        {
                            if (attribVal[i] < 0 || attribVal[i] >= indexSize)
                                continue;
                            attribVal[i] = indexMap.offsetFromIndex(attribVal[i]);
                        }
                        attrib_h.set(elemoff, attribVal);
                    }
                }
            }, subscribeRatio, minGrainSize);
        }
    }



    
public:
    bool offsetToIndex = true;
    

private:
    exint subscribeRatio = 64;
    exint minGrainSize = 1024;

}; // End of class GFE_OffsetAttribToIndex




#endif
