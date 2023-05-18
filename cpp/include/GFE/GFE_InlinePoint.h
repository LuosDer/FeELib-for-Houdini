
#pragma once

#ifndef __GFE_InlinePoint_h__
#define __GFE_InlinePoint_h__

//#include "GFE/GFE_InlinePoint.h"



#include "GFE/GFE_GeoFilter.h"
#include "GFE/GFE_Math.h"


enum class GFE_InlinePoint_Method
{
    Prim,
    ,
};

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
            const fpreal threshold_inlineCosRadians = 1e-05,
            //const bool reverseGroup   = false,
            //const bool delInlinePoint = false,
            const exint subscribeRatio  = 64,
            const exint minGrainSize    = 16
        )
    {
        setHasComputed();
        this->threshold_inlineCosRadians = threshold_inlineCosRadians;
        //this->reverseOutGroup          = reverseGroup;
        //this->doDelOutGroup            = delInlinePoint;
        this->subscribeRatio             = subscribeRatio;
        this->minGrainSize               = minGrainSize;
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
            if (getOutGroupArray()[i]->classType() != GA_GROUP_POINT)
            {
                UT_ASSERT_MSG(0, "not correct group type");
                continue;
            }
            
            inlinePointtGroup = getOutGroupArray().getPointGroup(i);
            switch (geo->getPreferredPrecision())
            {
            default:
            case GA_PRECISION_64: groupPrimInlinePoint<fpreal64>(); break;
            case GA_PRECISION_32: groupPrimInlinePoint<fpreal32>(); break;
            }
            
            if (!groupParser.groupType() == GA_GROUP_PRIMITIVE)
            {
                *inlinePointtGroup &= *groupParser.getPointGroup();
            }

            if (doDelGroupElement)
            {
                //delOutGroup();
                geo->destroyPointOffsets(geo->getPointRange(inlinePointtGroup), GA_Detail::GA_DestroyPointMode::GA_DESTROY_DEGENERATE_INCOMPATIBLE);
            }
        }
        return true;
    }


    template<typename FLOAT_T>
    void groupPrimInlinePoint()
    {
        UTparallelFor(groupParser.getPrimitiveGroup(), [this](const GA_SplittableRange& r)
        {
            GA_Offset start, end;
            for (GA_Iterator it(r); it.blockAdvance(start, end); )
            {
                for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                {
                    const GA_OffsetListRef vertices = geo->getPrimitiveVertexList(elemoff);
                    //const GA_Size numvtx = vertices.size();
                    const GA_Size lastLastIndex = vertices.size() - 2;
                    //if (numvtx <= 2)
                    if (lastLastIndex <= 0)
                        continue;

                    UT_Vector3T<FLOAT_T> pos, pos_next, dir_prev, dir_next;
                    GA_Offset ptoff, ptoff_next;

                    const bool closed = geo->getPrimitiveClosedFlag(elemoff);
                    ptoff = geo->vertexPoint(geo->getPrimitiveVertexOffset(elemoff, closed ? 0 : 1));
                    pos = geo->getPos3T<FLOAT_T>(ptoff);


                    ptoff_next = geo->vertexPoint(geo->getPrimitiveVertexOffset(elemoff, closed ? vertices.size() - 1 : 0));
                    pos_next = geo->getPos3T<FLOAT_T>(ptoff_next);
                    dir_prev = pos - pos_next;
                    dir_prev.normalize();

                    ptoff_next = geo->vertexPoint(geo->getPrimitiveVertexOffset(elemoff, closed ? 1 : 2));
                    pos_next = geo->getPos3T<FLOAT_T>(ptoff_next);
                    dir_next = pos_next - pos;
                    dir_next.normalize();

                    for (GA_Size vtxpnum = !closed; vtxpnum < lastLastIndex; ++vtxpnum)
                    {
                        ptoff = geo->vertexPoint(geo->getPrimitiveVertexOffset(elemoff, vtxpnum));

                        fpreal dotVal = dot(dir_prev, dir_next);
                        inlinePointtGroup->setElement(ptoff, (dotVal >= threshold_inlineCosRadians) ^ reverseOutGroup);

                        pos = pos_next;

                        ptoff = geo->vertexPoint(geo->getPrimitiveVertexOffset(elemoff, vtxpnum + 2));
                        pos_next = geo->getPos3T<FLOAT_T>(ptoff);

                        dir_prev = dir_next;
                        dir_next = pos_next - pos;
                        dir_next.normalize();
                    }
                    ptoff = geo->vertexPoint(geo->getPrimitiveVertexOffset(elemoff, lastLastIndex));
                    inlinePointtGroup->setElement(ptoff, (dot(dir_prev, dir_next) >= threshold_inlineCosRadians) ^ reverseOutGroup);
                    if (closed)
                    {
                        pos = pos_next;

                        ptoff = geo->vertexPoint(geo->getPrimitiveVertexOffset(elemoff, 0));
                        pos_next = geo->getPos3T<FLOAT_T>(ptoff);

                        dir_prev = dir_next;
                        dir_next = pos_next - pos;
                        dir_next.normalize();


                        ptoff = geo->vertexPoint(geo->getPrimitiveVertexOffset(elemoff, lastLastIndex + 1));
                        inlinePointtGroup->setElement(ptoff, (dot(dir_prev, dir_next) >= threshold_inlineCosRadians) ^ reverseOutGroup);
                    }
                }
            }
        }, subscribeRatio, minGrainSize);
        inlinePointtGroup->invalidateGroupEntries();
    }







public:
    fpreal threshold_inlineCosRadians = 1e-05;
    //GA_PointGroup* inlinePointtGroup = nullptr;

private:
    GA_PointGroup* inlinePointtGroup = nullptr;
    GA_EdgeGroup* inlineEdgeGroup = nullptr;
    
    exint subscribeRatio = 64;
    exint minGrainSize = 16;

    
}; // End of Class GFE_InlinePoint














#endif
