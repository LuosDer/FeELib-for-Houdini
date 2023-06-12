
#pragma once

#ifndef __GFE_GroupOneNebPoint_h__
#define __GFE_GroupOneNebPoint_h__

#include "GFE/GFE_GroupOneNebPoint.h"

#include "GFE/GFE_GeoFilter.h"



#include "GU/GU_Snap.h"

class GFE_GroupOneNebPoint : public GFE_AttribFilter {

public:
    using GFE_AttribFilter::GFE_AttribFilter;

    ~GFE_GroupOneNebPoint()
    {
    }

    
    void
        setComputeParm(
            const bool preFusePoint    = false,
            const fpreal fuseDist      = 1e-05,
            const exint subscribeRatio = 64,
            const exint minGrainSize   = 64
        )
    {
        setHasComputed();
        this->preFusePoint   = preFusePoint;
        this->fuseDist       = fuseDist;
        this->subscribeRatio = subscribeRatio;
        this->minGrainSize   = minGrainSize;
    }

    
private:

    virtual bool
        computeCore() override
    {
        if (getOutGroupArray().isEmpty() || getOutGroupArray()[0]->classType() != GA_GROUP_POINT)
            return false;
        
        if (groupParser.isEmpty())
            return true;

        GU_DetailHandle geoOrigin_h;
        if (preFusePoint)
        {
            geoOriginTmp = new GU_Detail();
            geoOrigin_h.allocateAndSet(geoOriginTmp);
            geoOriginTmp->replaceWith(*geo);

            fuseParms.myDistance = fuseDist;
            fuseParms.myAlgorithm = GU_Snap::PointSnapParms::SnapAlgorithm::ALGORITHM_CLOSEST_POINT;
            fuseParms.myConsolidate = true;
            fuseParms.myDeleteConsolidated = true;
            fuseParms.myQPosH = geoOriginTmp->getP();
            fuseParms.myTPosH = geoOriginTmp->getP();
            GU_Snap::snapPoints(*geoOriginTmp, nullptr, fuseParms);
            //GU_Snap::snapPoints(*geoOriginTmp, static_cast<const GU_Detail*>(geo), fuseParms);
        }
        else
        {
            geoOriginTmp = geo->asGU_Detail();
        }
        
        groupSetter = getOutGroupArray().getPointGroup(0);

        switch (groupParser.classType())
        {
        default:
        case GA_GROUP_PRIMITIVE: return groupOneNeb<GA_GROUP_PRIMITIVE>(); break;
        case GA_GROUP_POINT:     return groupOneNeb<GA_GROUP_POINT>    (); break;
        case GA_GROUP_VERTEX:    return groupOneNeb<GA_GROUP_VERTEX>   (); break;
        }

        return true;
    }

    template<GA_GroupType GroupType_T>
    void groupOneNeb()
    {
        //UT_ASSERT_P(geoGroup);
        
        const GA_Topology& topo = geo->getTopology();
        //topo.makeVertexRef();
        const GA_ATITopology& vtxPointRef = *topo.getPointRef();
        const GA_ATITopology& pointVtxRef = *topo.getVertexRef();
        const GA_ATITopology& vtxNextRef = *topo.getVertexNextRef();

        UTparallelFor(groupParser.getPrimitiveSplittableRange(), [this, &vtxPointRef, &pointVtxRef, &vtxNextRef](const GA_SplittableRange& r)
        {
            GA_Offset vtxoff, vtxoff_next, ptoff;
            GA_Offset start, end;
            for (GA_Iterator it(r); it.blockAdvance(start, end); )
            {
                for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                {
                    const GA_Size numvtx = geo->getPrimitiveVertexCount(elemoff);
                    if (numvtx <= 0)
                        continue;
                    
                    vtxoff = geo->getPrimitiveVertexOffset(elemoff, 0);
                    if constexpr (GroupType_T == GA_GROUP_POINT)
                    {
                        
                    }
                    if constexpr (GroupType_T == GA_GROUP_VERTEX)
                    {
                        
                        if (groupParser.cont)
                        {
                        
                        }
                    }
                    
                    ptoff       = vtxPointRef.getLink(vtxoff);
                    vtxoff_next = pointVtxRef.getLink(ptoff);
                    if (vtxoff_next == vtxoff)
                    {
                        vtxoff_next = vtxNextRef.getLink(vtxoff_next);
                        if (GFE_Type::isInvalidOffset(vtxoff_next))
                            groupSetter.set(ptoff, true);
                    }
                    
                    if (numvtx <= 1)
                        continue;
                    
                    vtxoff = geo->getPrimitiveVertexOffset(elemoff, numvtx-1);
                    ptoff       = vtxPointRef.getLink(vtxoff);
                    vtxoff_next = pointVtxRef.getLink(ptoff);
                    if (vtxoff_next == vtxoff)
                    {
                        vtxoff_next = vtxNextRef.getLink(vtxoff_next);
                        if (GFE_Type::isInvalidOffset(vtxoff_next))
                            groupSetter.set(ptoff, true);
                    }
                }
            }
        }, subscribeRatio, minGrainSize);
        groupSetter.invalidateGroupEntries();
    }


    void groupOneNeb(const GA_PointGroup* const geoGroup)
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
                            oneNebPointGroup->setElement(ptoff, true);

                        vtxoff = geo->getPrimitiveVertexOffset(elemoff, geo->getPrimitiveVertexCount(elemoff) - 1);
                        ptoff = vtxPointRef->getLink(vtxoff);
                        vtxoff = pointVtxRef->getLink(ptoff);
                        vtxoff = vtxNextRef->getLink(vtxoff);
                        //if (!topo.isPointShared(ptoff) && geoGroup->contains(ptoff))
                        if (vtxoff == GA_INVALID_OFFSET && geoGroup->contains(ptoff))
                            oneNebPointGroup->setElement(ptoff, true);
                    }
                }
            }, subscribeRatio, minGrainSize);
        oneNebPointGroup->invalidateGroupEntries();
    }


    void groupOneNeb(const GA_VertexGroup* const geoGroup)
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
                            oneNebPointGroup->setElement(ptoff, true);

                        vtxoff = geo->getPrimitiveVertexOffset(elemoff, geo->getPrimitiveVertexCount(elemoff) - 1);
                        ptoff = vtxPointRef->getLink(vtxoff);
                        vtxoff_next = pointVtxRef->getLink(ptoff);
                        vtxoff_next = vtxNextRef->getLink(vtxoff_next);
                        //if (!topo.isPointShared(ptoff) && geoGroup->contains(ptoff))
                        if (vtxoff_next == GA_INVALID_OFFSET && geoGroup->contains(vtxoff))
                            oneNebPointGroup->setElement(ptoff, true);
                    }
                }
            }, subscribeRatio, minGrainSize);
        oneNebPointGroup->invalidateGroupEntries();
    }

    SYS_FORCE_INLINE GA_Offset getPrimitivePointOffset(const GU_Detail* const geo, const GA_Offset primoff, const GA_Size vtxpnum) const
    { return geo->vertexPoint(geo->getPrimitiveVertexOffset(primoff, vtxpnum)); }



public:
    bool preFusePoint = false;
    fpreal fuseDist = 1e-05;
    
private:
    GU_Snap::PointSnapParms fuseParms;
    
    GU_DetailHandle geoRef0_h;
    GU_Detail* geoRef0Tmp;
    GU_Detail* geoOriginTmp;
    
    exint subscribeRatio = 64;
    exint minGrainSize = 256;

}; // End of class GFE_GroupOneNebPoint











#endif
