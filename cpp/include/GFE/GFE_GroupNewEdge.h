
#pragma once

#ifndef __GFE_GroupNewEdge_h__
#define __GFE_GroupNewEdge_h__

//#include "GFE/GFE_GroupNewEdge.h"


#include "GFE/GFE_GeoFilter.h"


#include "GFE/GFE_MeshTopology.h"


#include "GU/GU_Snap.h"



// Ref GFE_EdgeGroupTransfer
class GFE_GroupNewEdge : public GFE_AttribFilterWithRef {


public:
    using GFE_AttribFilterWithRef::GFE_AttribFilterWithRef;


    void
        setComputeParm(
            const bool useSnapDist = true,
            const fpreal snapDist = 0.001,
            const bool reverseGroup = false,
            const exint subscribeRatio = 64,
            const exint minGrainSize = 64
        )
    {
        setHasComputed();
        this->useSnapDist      = useSnapDist;
        this->snapDist         = snapDist;
        this->reverseOutGroup  = reverseGroup;
        this->subscribeRatio   = subscribeRatio;
        this->minGrainSize     = minGrainSize;
    }

    
    SYS_FORCE_INLINE void findOrCreateGroup(
        const bool detached = false,
        const bool outVertexEdgeGroup = false,
        const UT_StringRef& name = ""
    )
    {
        if(outVertexEdgeGroup)
            findOrCreateVertexGroup(detached, name);
        else
            findOrCreateEdgeGroup(detached, name);
    }

    SYS_FORCE_INLINE GA_VertexGroup* findOrCreateVertexGroup(
        const bool detached = false,
        const UT_StringRef& name = ""
    )
    { return vertexEdgeGroup = getOutGroupArray().findOrCreateVertex(detached, name); }

    SYS_FORCE_INLINE GA_EdgeGroup* findOrCreateEdgeGroup(
        const bool detached = false,
        const UT_StringRef& name = ""
    )
    { return edgeGroup = getOutGroupArray().findOrCreateEdge(detached, name); }

private:

    virtual bool
        computeCore() override
    {
        if (getOutGroupArray().isEmpty())
            return false;

        if (groupParser.isEmpty())
            return true;

        

        geoRef0Tmp = new GU_Detail();
        geoRef0_h.allocateAndSet(geoRef0Tmp);
        geoRef0Tmp->replaceWith(*geoRef0->asGA_Detail());


    
        const GA_ATINumericUPtr snapPtoffAttribUPtr = geoRef0->getAttributes().
            createDetachedTupleAttribute(GA_ATTRIB_POINT, GA_STORE_INT64, 1, GA_Defaults(GFE_INVALID_OFFSET));
    
        snapPtoffAttrib = snapPtoffAttribUPtr.get();
        //const GA_RWHandleID snapPtoff_h(snapPtoffAttribPtr);


        GU_Snap::PointSnapParms pointSnapParms;
        pointSnapParms.myAlgorithm = GU_Snap::PointSnapParms::ALGORITHM_CLOSEST_POINT;
        pointSnapParms.myDistance = useSnapDist ? snapDist : SYS_FP64_MAX;
        pointSnapParms.myOutputAttribH = snapPtoffAttrib;
        snapPtoff_h = snapPtoffAttrib;

        GU_Snap::snapPoints(*geoRef0Tmp, geo->asGU_Detail(), pointSnapParms);
        //GU_Snap::snapPoints(*geo->asGU_Detail(), geoRef0, pointSnapParms);


    
        GFE_MeshTopology meshTopology(geo, cookparms);
        vertexPointDstAttrib = meshTopology.setVertexPointDst(true);
        meshTopology.compute();
        dstpt_h = vertexPointDstAttrib;
    
        GFE_MeshTopology meshTopologyRef0(geoRef0Tmp);
        vertexPointDstRef0Attrib = meshTopologyRef0.setVertexPointDst(true);
        meshTopologyRef0.compute();

    
        const size_t size = getOutGroupArray().size();
        for (size_t i = 0; i < size; ++i)
        {
            
            switch (getOutGroupArray()[i]->classType())
            {
            //case GA_GROUP_PRIMITIVE:  groupNewEdge(getOutGroupArray().getPrimitiveGroup(i));  break;
            //case GA_GROUP_POINT:      groupNewEdge(getOutGroupArray().getPointGroup((i));     break;
            case GA_GROUP_VERTEX:     groupNewEdge(getOutGroupArray().getVertexGroup(i));     break;
            case GA_GROUP_EDGE:       groupNewEdge(getOutGroupArray().getEdgeGroup(i));       break;
            default: break;
            }
        }
        

        return true;
    }



    void groupNewEdge(GA_VertexGroup* const group)
    {
        UTparallelFor(groupParser.getVertexSplittableRange(), [this, group](const GA_SplittableRange& r)
        {
            for (GA_PageIterator pit = r.beginPages(); !pit.atEnd(); ++pit)
            {
                GA_Offset start, end;
                for (GA_Iterator it(pit.begin()); it.blockAdvance(start, end); )
                {
                    for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                    {
                        group->setElement(elemoff, true);
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
    bool useSnapDist = true;
    fpreal snapDist = 0.001;
    
    const char* prefix = "";
    const char* sufix = "";
    

private:
    GU_DetailHandle geoRef0_h;
    GU_Detail* geoRef0Tmp;

    
    GA_EdgeGroup* edgeGroup = nullptr;
    GA_VertexGroup* vertexEdgeGroup = nullptr;

    
    GA_Attribute* snapPtoffAttrib;
    GA_RWHandleT<GA_Offset> snapPtoff_h;

    GA_Attribute* vertexPointDstRef0Attrib;
    
    GA_Attribute* vertexPointDstAttrib;
    GA_RWHandleT<GA_Offset> dstpt_h;
    
    exint subscribeRatio = 64;
    exint minGrainSize = 1024;


}; // End of class GFE_GroupNewEdge





#endif
