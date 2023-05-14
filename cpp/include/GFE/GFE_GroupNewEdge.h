
#pragma once

#ifndef __GFE_GroupNewEdge_h__
#define __GFE_GroupNewEdge_h__

//#include "GFE/GFE_GroupNewEdge.h"


#include "GFE/GFE_GeoFilter.h"



class GFE_GroupNewEdge : public GFE_AttribFilterWithRef {


public:
    using GFE_AttribFilterWithRef::GFE_AttribFilterWithRef;


    void
        setComputeParm(
            const GA_Size firstIndex = 0,
            const bool outAsOffset = true,
            const bool negativeIndex = false,
            const exint subscribeRatio = 64,
            const exint minGrainSize = 64
        )
    {
        setHasComputed();
        this->firstIndex = firstIndex;
        this->outAsOffset = outAsOffset;
        this->negativeIndex = negativeIndex;
        this->subscribeRatio = subscribeRatio;
        this->minGrainSize = minGrainSize;
    }

    
    SYS_FORCE_INLINE void findOrCreateGroup(
        const bool detached = false,
        const bool outVertexEdgeGroup = false,
        const UT_StringRef& name = ""
    ) const
    {
        if(outVertexEdgeGroup)
            findOrCreateVertexGroup(detached, name);
        else
            findOrCreateEdgeGroup(detached, name);
    }

    SYS_FORCE_INLINE GA_VertexGroup* findOrCreateVertexGroup(
        const bool detached = false,
        const UT_StringRef& name = ""
    ) const
    {
        edgeGroup = getOutGroupArray().findOrCreateVertex(detached, name);
        return edgeGroup;
    }

    SYS_FORCE_INLINE GA_EdgeGroup* findOrCreateEdgeGroup(
        const bool detached = false,
        const UT_StringRef& name = ""
    ) const
    {
        edgeGroup = getOutGroupArray().findOrCreateEdge(detached, name);
        return edgeGroup;
    }

private:

    virtual bool
        computeCore() override
    {
        if (getOutGroupArray().isEmpty())
            return false;

        if (groupParser.isEmpty())
            return true;

        const size_t size = getOutGroupArray().size();
        for (size_t i = 0; i < size; ++i)
        {
            
            switch (getOutGroupArray()[i]->classType())
            {
            //case GA_GROUP_PRIMITIVE:  groupNewEdge(getOutGroupArray().getPrimitiveGroup(0));  break;
            //case GA_GROUP_POINT:      groupNewEdge(getOutGroupArray().getPointGroup((0));     break;
            case GA_GROUP_VERTEX:     groupNewEdge(getOutGroupArray().getVertexGroup(0));     break;
            case GA_GROUP_EDGE:       groupNewEdge(getOutGroupArray().getEdgeGroup(0));       break;
            default: break;
            }
        }
        

        return true;
    }



    void groupNewEdge(GA_VertexGroup* const group)
    {
        const GA_SplittableRange geoSplittableRange0(groupParser.getRange(attribPtr->getOwner()));
        UTparallelFor(geoSplittableRange0, [this, group](const GA_SplittableRange& r)
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


    void groupNewEdge(GA_EdgeGroup* const group)
    {
        const GA_SplittableRange geoSplittableRange0(groupParser.getRange(attribPtr->getOwner()));
        UTparallelFor(geoSplittableRange0, [this, group](const GA_SplittableRange& r)
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

    


public:
    GA_Size firstIndex = 0;
    bool outAsOffset = true;
    bool negativeIndex = false;
    
    const char* prefix = "";
    const char* sufix = "";
    

private:
    
    GA_EdgeGroup* const edgeGroup = nullptr;

    
    exint subscribeRatio = 64;
    exint minGrainSize = 1024;


}; // End of class GFE_GroupNewEdge





#endif
