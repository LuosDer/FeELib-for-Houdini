
#pragma once

#ifndef __GFE_GroupOneNebPoint_h__
#define __GFE_GroupOneNebPoint_h__

//#include "GFE/GFE_GroupOneNebPoint.h"

#include "GFE/GFE_GeoFilter.h"


//#include "GEO/GEO_Curve.h"
//#include "GFE/GFE_Type.h"
//#include "GFE/GFE_Attribute.h"
//#include "GFE/GFE_GroupParser.h"
//#include "GFE/GFE_Measure.h"




class GFE_GroupOneNebPoint : public GFE_AttribFilter {



public:
    using GFE_AttribFilter::GFE_AttribFilter;

    ~GFE_GroupOneNebPoint()
    {
    }

     
    GA_Group*
        findOrCreate(
            const bool detached = false,
            const UT_StringHolder& groupName = ""
        )
    {
        return getOutGroupArray().findOrCreate(detached, GA_GROUP_POINT, groupName);
    }

    
    void
        setComputeParm(
            const exint subscribeRatio = 64,
            const exint minGrainSize = 64
        )
    {
        setHasComputed();
        this->subscribeRatio = subscribeRatio;
        this->minGrainSize = minGrainSize;
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

        outGroup = getOutGroupArray()[0];
        
        if (groupParser.isFull())
        {
            groupOneNeb(const GA_PrimitiveGroup*());
            return true;
        }
        switch (groupParser.classType())
        {
        case GA_GROUP_PRIMITIVE:
            return groupOneNeb(groupParser.getPrimitiveGroup());
            break;
        case GA_GROUP_POINT:
            return groupOneNeb(groupParser.getPointGroup());
            break;
        case GA_GROUP_VERTEX:
            return groupOneNeb(groupParser.getVertexGroup());
            break;
        case GA_GROUP_EDGE:
            return groupOneNeb(groupParser.getEdgeGroup());
            break;
        default:
            break;
        }

        return true;
    }


    void
        groupOneNeb(
            const GA_PrimitiveGroup* const geoGroup
        )
    {
        // UT_ASSERT_P(geoGroup);
        
        const GA_Topology& topo = geo->getTopology();
        //topo.makeVertexRef();
        const GA_ATITopology* const vtxPointRef = topo.getPointRef();
        const GA_ATITopology* const pointVtxRef = topo.getVertexRef();
        const GA_ATITopology* const vtxNextRef = topo.getVertexNextRef();

        const GA_SplittableRange geoSplittableRange0(geo->getPrimitiveRange(geoGroup));
        UTparallelFor(geoSplittableRange0, [this,
            vtxPointRef, pointVtxRef, vtxNextRef](const GA_SplittableRange& r)
            {
                GA_Offset vtxoff, ptoff;
                GA_Offset start, end;
                for (GA_Iterator it(r); it.blockAdvance(start, end); )
                {
                    for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                    {
                        vtxoff = geo->getPrimitiveVertexOffset(elemoff, 0);
                        ptoff = vtxPointRef->getLink(vtxoff);
                        vtxoff = pointVtxRef->getLink(ptoff);
                        vtxoff = vtxNextRef->getLink(vtxoff);
                        //if (!topo.isPointShared(ptoff))
                        if (vtxoff == GA_INVALID_OFFSET)
                            outGroup->setElement(ptoff, true);

                        vtxoff = geo->getPrimitiveVertexOffset(elemoff, geo->getPrimitiveVertexCount(elemoff) - 1);
                        ptoff = vtxPointRef->getLink(vtxoff);
                        vtxoff = pointVtxRef->getLink(ptoff);
                        vtxoff = vtxNextRef->getLink(vtxoff);
                        //if (!topo.isPointShared(ptoff))
                        if (vtxoff == GA_INVALID_OFFSET)
                            outGroup->setElement(ptoff, true);
                    }
                }
            }, subscribeRatio, minGrainSize);
        outGroup->invalidateGroupEntries();
    }


    void
        groupOneNeb(
            const GA_PointGroup* const geoGroup
        )
    {
        UT_ASSERT_P(geoGroup);
        
        const GA_Topology& topo = geo->getTopology();
        //topo.makeVertexRef();
        const GA_ATITopology* const vtxPointRef = topo.getPointRef();
        const GA_ATITopology* const pointVtxRef = topo.getVertexRef();
        const GA_ATITopology* const vtxNextRef = topo.getVertexNextRef();

        const GA_PrimitiveGroupUPtr geoPromoGroupUPtr = geo->createDetachedPrimitiveGroup();
        GA_PrimitiveGroup* geoPromoGroup = geoPromoGroupUPtr.get();
        geoPromoGroup->combine(geoGroup);

        const GA_SplittableRange geoSplittableRange0(geo->getPrimitiveRange(geoPromoGroup));
        UTparallelFor(geoSplittableRange0, [this, geoGroup,
            vtxPointRef, pointVtxRef, vtxNextRef](const GA_SplittableRange& r)
            {
                GA_Offset vtxoff, ptoff;
                GA_Offset start, end;
                for (GA_Iterator it(r); it.blockAdvance(start, end); )
                {
                    for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                    {
                        vtxoff = geo->getPrimitiveVertexOffset(elemoff, 0);
                        ptoff = vtxPointRef->getLink(vtxoff);
                        vtxoff = pointVtxRef->getLink(ptoff);
                        vtxoff = vtxNextRef->getLink(vtxoff);
                        //if (!topo.isPointShared(ptoff) && geoGroup->contains(ptoff))
                        if (vtxoff == GA_INVALID_OFFSET && geoGroup->contains(ptoff))
                            outGroup->setElement(ptoff, true);

                        vtxoff = geo->getPrimitiveVertexOffset(elemoff, geo->getPrimitiveVertexCount(elemoff) - 1);
                        ptoff = vtxPointRef->getLink(vtxoff);
                        vtxoff = pointVtxRef->getLink(ptoff);
                        vtxoff = vtxNextRef->getLink(vtxoff);
                        //if (!topo.isPointShared(ptoff) && geoGroup->contains(ptoff))
                        if (vtxoff == GA_INVALID_OFFSET && geoGroup->contains(ptoff))
                            outGroup->setElement(ptoff, true);
                    }
                }
            }, subscribeRatio, minGrainSize);
        outGroup->invalidateGroupEntries();
    }


    void
        groupOneNeb(
            const GA_VertexGroup* const geoGroup
        )
    {
        UT_ASSERT_P(geoGroup);
        
        const GA_Topology& topo = geo->getTopology();
        //topo.makeVertexRef();
        const GA_ATITopology* const vtxPointRef = topo.getPointRef();
        const GA_ATITopology* const pointVtxRef = topo.getVertexRef();
        const GA_ATITopology* const vtxNextRef = topo.getVertexNextRef();

        const GA_PrimitiveGroupUPtr geoPromoGroupUPtr = geo->createDetachedPrimitiveGroup();
        GA_PrimitiveGroup* const geoPromoGroup = geoPromoGroupUPtr.get();
        geoPromoGroup->combine(geoGroup);

        const GA_SplittableRange geoSplittableRange0(geo->getPrimitiveRange(geoPromoGroup));
        UTparallelFor(geoSplittableRange0, [this, geoGroup,
            vtxPointRef, pointVtxRef, vtxNextRef](const GA_SplittableRange& r)
            {
                GA_Offset vtxoff, ptoff, vtxoff_next;
                GA_Offset start, end;
                for (GA_Iterator it(r); it.blockAdvance(start, end); )
                {
                    for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                    {
                        vtxoff = geo->getPrimitiveVertexOffset(elemoff, 0);
                        ptoff = vtxPointRef->getLink(vtxoff);
                        vtxoff_next = pointVtxRef->getLink(ptoff);
                        vtxoff_next = vtxNextRef->getLink(vtxoff_next);
                        //if (!topo.isPointShared(ptoff) && geoGroup->contains(ptoff))
                        if (vtxoff_next == GA_INVALID_OFFSET && geoGroup->contains(vtxoff))
                            outGroup->setElement(ptoff, true);

                        vtxoff = geo->getPrimitiveVertexOffset(elemoff, geo->getPrimitiveVertexCount(elemoff) - 1);
                        ptoff = vtxPointRef->getLink(vtxoff);
                        vtxoff_next = pointVtxRef->getLink(ptoff);
                        vtxoff_next = vtxNextRef->getLink(vtxoff_next);
                        //if (!topo.isPointShared(ptoff) && geoGroup->contains(ptoff))
                        if (vtxoff_next == GA_INVALID_OFFSET && geoGroup->contains(vtxoff))
                            outGroup->setElement(ptoff, true);
                    }
                }
            }, subscribeRatio, minGrainSize);
        outGroup->invalidateGroupEntries();
    }




    
private:
    GA_PointGroup* outGroup = nullptr;
    
    exint subscribeRatio = 64;
    exint minGrainSize = 256;

};











#endif
