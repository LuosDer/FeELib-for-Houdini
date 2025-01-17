
#pragma once

#ifndef __GFE_InlinePoint_h__
#define __GFE_InlinePoint_h__

//#include "GFE/GFE_InlinePoint.h"



#include "GFE/GFE_GeoFilter.h"
#include "GFE/GFE_Math.h"



class GFE_InlinePoint : public GFE_AttribFilter {

public:
    using GFE_AttribFilter::GFE_AttribFilter;

    ~GFE_InlinePoint()
    {
    }

    SYS_FORCE_INLINE void setThreshold_inlineCosRadians(fpreal threshold_inlineAngle)
    { threshold_inlineCosRadians = cos(GFE_Math::radians(threshold_inlineAngle)); }

    void
        setComputeParm(
            const bool doDelGroupElement = false,
            const exint subscribeRatio  = 64,
            const exint minGrainSize    = 16
        )
    {
        setHasComputed();
        this->doDelGroupElement = doDelGroupElement;
        this->subscribeRatio    = subscribeRatio;
        this->minGrainSize      = minGrainSize;
    }




private:

    virtual bool
        computeCore() override
    {
        if (groupParser.isEmpty())
            return true;
        
        const size_t len = getOutGroupArray().size();
        for (size_t i = 0; i < len; i++)
        {
            const GA_GroupType groupType = getOutGroupArray()[i]->classType();
            if (groupType != GA_GROUP_POINT && groupType != GA_GROUP_VERTEX && groupType != GA_GROUP_EDGE)
            {
                UT_ASSERT_MSG(0, "not correct group type");
                continue;
            }
            const GA_Storage posStorage = geo->getPStorage();
            switch (groupType)
            {
            case GA_GROUP_EDGE:
                inlineEdgeGroup = getOutGroupArray().getEdgeGroup(i);
            case GA_GROUP_POINT:
                if (groupType == GA_GROUP_POINT)
                    inlinePointGroup = getOutGroupArray().getPointGroup(i);
                else
                {
                    const GA_PointGroupUPtr inlinePointGroupUPtr = geo->createDetachedPointGroup();
                    inlinePointGroup = inlinePointGroupUPtr.get();
                }
                
                switch (geo->getPreferredPrecision())
                {
                default:
                case GA_PRECISION_64: groupInlinePoint<fpreal64>(); break;
                case GA_PRECISION_32: groupInlinePoint<fpreal32>(); break;
                }
            
                if (groupType == GA_GROUP_POINT)
                {
                    if (!groupParser.groupType() == GA_GROUP_PRIMITIVE)
                    {
                        *inlinePointGroup &= *groupParser.getPointGroup();
                    }

                    if (doDelGroupElement)
                        geo->destroyPointOffsets(geo->getPointRange(inlinePointGroup), GA_Detail::GA_DestroyPointMode::GA_DESTROY_DEGENERATE_INCOMPATIBLE);
                    break;
                }
                
                GFE_GroupUnion::groupUnion(*inlineEdgeGroup, inlinePointGroup);
                if (doDelGroupElement)
                    geo->asGU_Detail()->dissolveEdges(*inlineEdgeGroup, false, 1e-05, true, GU_Detail::BridgeMode::GU_BRIDGEMODE_BRIDGE, false, false);
            
                break;
            case GA_GROUP_VERTEX:
                inlineVertexGroup = getOutGroupArray().getVertexGroup(i);
                switch (posStorage)
                {
                default:
                case GA_PRECISION_64: groupInlineVertex<fpreal64>(); break;
                case GA_PRECISION_32: groupInlineVertex<fpreal32>(); break;
                }
                break;
            }

        }
        return true;
    }

    
    template<typename FLOAT_T>
    void groupInlineVertex()
    {
        const GA_ROHandleT<UT_Vector3T<FLOAT_T>> pos_h(geo->getP());
        groupSetter = inlineVertexGroup;
        UTparallelFor(groupParser.getPrimitiveSplittableRange(), [this, &pos_h](const GA_SplittableRange& r)
        {
            GA_Offset start, end;
            for (GA_Iterator it(r); it.blockAdvance(start, end); )
            {
                for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                {
                    const GA_OffsetListRef vertices = geo->getPrimitiveVertexList(elemoff);
                    if (vertices.size() <= 2)
                        continue;
                    const GA_Size lastLastIndex = vertices.size()-2;
                    
                    UT_Vector3T<FLOAT_T> pos, pos_next, dir_prev, dir_next;
                    GA_Offset vtxoff, ptoff, ptoff_next;

                    const bool closed = geo->getPrimitiveClosedFlag(elemoff);
                    ptoff = geo->vertexPoint(geo->getPrimitiveVertexOffset(elemoff, closed ? 0 : 1));
                    pos = pos_h.get(ptoff);


                    ptoff_next = geo->vertexPoint(geo->getPrimitiveVertexOffset(elemoff, closed ? vertices.size() - 1 : 0));
                    pos_next = pos_h.get(ptoff_next);
                    dir_prev = pos - pos_next;
                    dir_prev.normalize();

                    ptoff_next = geo->vertexPoint(geo->getPrimitiveVertexOffset(elemoff, closed ? 1 : 2));
                    pos_next = pos_h.get(ptoff_next);
                    dir_next = pos_next - pos;
                    dir_next.normalize();

                    for (GA_Size vtxpnum = !closed; vtxpnum < lastLastIndex; ++vtxpnum)
                    {
                        vtxoff = geo->getPrimitiveVertexOffset(elemoff, vtxpnum);
                        ptoff = geo->vertexPoint(vtxoff);

                        fpreal dotVal = dot(dir_prev, dir_next);
                        groupSetter.set(vtxoff, dotVal >= threshold_inlineCosRadians);

                        pos = pos_next;

                        ptoff = geo->vertexPoint(geo->getPrimitiveVertexOffset(elemoff, vtxpnum + 2));
                        pos_next = pos_h.get(ptoff);

                        dir_prev = dir_next;
                        dir_next = pos_next - pos;
                        dir_next.normalize();
                    }
                    vtxoff = geo->getPrimitiveVertexOffset(elemoff, lastLastIndex);
                    //ptoff = geo->vertexPoint(vtxoff);
                    groupSetter.set(vtxoff, dot(dir_prev, dir_next) >= threshold_inlineCosRadians);
                    if (closed)
                    {
                        pos = pos_next;

                        ptoff = geo->vertexPoint(geo->getPrimitiveVertexOffset(elemoff, 0));
                        pos_next = pos_h.get(ptoff);

                        dir_prev = dir_next;
                        dir_next = pos_next - pos;
                        dir_next.normalize();


                        vtxoff = geo->getPrimitiveVertexOffset(elemoff, lastLastIndex+1);
                        //ptoff = geo->vertexPoint(vtxoff);
                        groupSetter.set(vtxoff, dot(dir_prev, dir_next) >= threshold_inlineCosRadians);
                    }
                }
            }
        }, subscribeRatio, minGrainSize);
        inlineVertexGroup->invalidateGroupEntries();
    }


    template<typename FLOAT_T>
    void groupInlinePoint()
    {
        groupSetter = inlinePointGroup;
        const GA_ROHandleT<UT_Vector3T<FLOAT_T>> pos_h(geo->getP());
        UTparallelFor(groupParser.getPrimitiveSplittableRange(), [this, &pos_h](const GA_SplittableRange& r)
        {
            GA_Offset start, end;
            for (GA_Iterator it(r); it.blockAdvance(start, end); )
            {
                for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                {
                    const GA_OffsetListRef vertices = geo->getPrimitiveVertexList(elemoff);
                    if (vertices.size() <= 2)
                        continue;
                    const GA_Size lastLastIndex = vertices.size() - 2;

                    UT_Vector3T<FLOAT_T> pos, pos_next, dir_prev, dir_next;
                    GA_Offset ptoff, ptoff_next;

                    const bool closed = geo->getPrimitiveClosedFlag(elemoff);
                    ptoff = geo->vertexPoint(geo->getPrimitiveVertexOffset(elemoff, closed ? 0 : 1));
                    pos = pos_h.get(ptoff);


                    ptoff_next = geo->vertexPoint(geo->getPrimitiveVertexOffset(elemoff, closed ? vertices.size() - 1 : 0));
                    pos_next = pos_h.get(ptoff_next);
                    dir_prev = pos - pos_next;
                    dir_prev.normalize();

                    ptoff_next = geo->vertexPoint(geo->getPrimitiveVertexOffset(elemoff, closed ? 1 : 2));
                    pos_next = pos_h.get(ptoff_next);
                    dir_next = pos_next - pos;
                    dir_next.normalize();

                    for (GA_Size vtxpnum = !closed; vtxpnum < lastLastIndex; ++vtxpnum)
                    {
                        ptoff = geo->vertexPoint(geo->getPrimitiveVertexOffset(elemoff, vtxpnum));

                        fpreal dotVal = dot(dir_prev, dir_next);
                        groupSetter.set(ptoff, dotVal >= threshold_inlineCosRadians);

                        pos = pos_next;

                        ptoff = geo->vertexPoint(geo->getPrimitiveVertexOffset(elemoff, vtxpnum + 2));
                        pos_next = pos_h.get(ptoff);

                        dir_prev = dir_next;
                        dir_next = pos_next - pos;
                        dir_next.normalize();
                    }
                    ptoff = geo->vertexPoint(geo->getPrimitiveVertexOffset(elemoff, lastLastIndex));
                    groupSetter.set(ptoff, dot(dir_prev, dir_next) >= threshold_inlineCosRadians);
                    if (closed)
                    {
                        pos = pos_next;

                        ptoff = geo->vertexPoint(geo->getPrimitiveVertexOffset(elemoff, 0));
                        pos_next = pos_h.get(ptoff);

                        dir_prev = dir_next;
                        dir_next = pos_next - pos;
                        dir_next.normalize();


                        ptoff = geo->vertexPoint(geo->getPrimitiveVertexOffset(elemoff, lastLastIndex + 1));
                        groupSetter.set(ptoff, dot(dir_prev, dir_next) >= threshold_inlineCosRadians);
                    }
                }
            }
        }, subscribeRatio, minGrainSize);
        inlinePointGroup->invalidateGroupEntries();
    }




public:
        
    fpreal threshold_inlineCosRadians = 1e-05;
    //GA_PointGroup* inlinePointGroup = nullptr;

private:
    
    
    GA_PointGroup* inlinePointGroup = nullptr;
    GA_VertexGroup* inlineVertexGroup = nullptr;
    GA_EdgeGroup* inlineEdgeGroup = nullptr;
    
    exint subscribeRatio = 64;
    exint minGrainSize = 16;

    
}; // End of Class GFE_InlinePoint














#endif
