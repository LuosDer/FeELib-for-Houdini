
#pragma once

#ifndef __GFE_PrimInlinePoint_h__
#define __GFE_PrimInlinePoint_h__

//#include "GFE/GFE_PrimInlinePoint.h"

#include "GA/GA_Detail.h"
#include "GA/GA_SplittableRange.h"



//#include "GFE/GFE_Type.h
#include "GFE/GFE_Detail.h"
#include "GFE/GFE_GroupParse.h"








class GFE_PrimInlinePoint {


public:
    GFE_PrimInlinePoint(
        const GA_Detail* const geo,
    )
        : geo(geo)
    {
    }

    ~GFE_PrimInlinePoint()
    {
    }

    void
        setInGroup(
            const GA_GroupType inGroupType,
            const UT_StringHolder& inGroupName
        )
    {
        GOP_Manager gop;
        const GA_Group* const inGroup = GFE_GroupParse_Namespace::findOrParseGroupDetached(cookparms, geo, inGroupType, inGroupName, gop);
        if (inGroup && inGroup->isEmpty())
            return;
        GA_PointGroup* const inlinePtGroup = static_cast<GA_PointGroup*>(geo->pointGroups().newGroup(outGroupName));

    }

    //groupPrimInlinePoint_fast(geo, groupName, reverseGroup, delGroup);
    template<typename FLOAT_T>
    GA_PointGroup*
        groupPrimInlinePoint_fast()
    {
        UT_ASSERT_P(inlinePtGroup);

        if (!inGroup || inGroup->classType() == GA_GROUP_PRIMITIVE)
        {
            groupPrimInlinePoint_fast(geo, static_cast<const GA_PrimitiveGroup*>(inGroup), inlinePtGroup, threshold_radians, reverseGroup, subscribeRatio, minGrainSize);
        }
        else
        {
            const GA_PrimitiveGroupUPtr promotedPrimGroupUPtr = geo->createDetachedPrimitiveGroup();
            GA_PrimitiveGroup* promotedPrimGroup = promotedPrimGroupUPtr.get();
            promotedPrimGroup->combine(inGroup);
            groupPrimInlinePoint_fast(geo, promotedPrimGroup, inlinePtGroup, threshold_radians, reverseGroup, subscribeRatio, minGrainSize);

            if (inGroup->classType() == GA_GROUP_POINT)
            {
                *inlinePtGroup &= *static_cast<const GA_PointGroup*>(inGroup);
            }
            else
            {
                const GA_PointGroupUPtr promotedPointGroupUPtr = geo->createDetachedPointGroup();
                GA_PointGroup* promotedPointGroup = promotedPointGroupUPtr.get();
                promotedPointGroup->combine(inGroup);
                *inlinePtGroup &= *promotedPointGroup;
            }
        }

        const fpreal threshold_dot = cos(threshold_radians);
        const GA_SplittableRange geoSplittableRange0(geo->getPrimitiveRange(inGroup));
        UTparallelFor(geoSplittableRange0, [this, threshold_dot](const GA_SplittableRange& r)
            {
                GA_Offset start, end;
                for (GA_Iterator it(r); it.blockAdvance(start, end); )
                {
                    for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                    {
                        const GA_OffsetListRef& vertices = geo->getPrimitiveVertexList(elemoff);
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

                            inlinePtGroup->setElement(ptoff, dot(dir_prev, dir_next) >= threshold_dot ^ reverseGroup);

                            pos = pos_next;

                            ptoff = geo->vertexPoint(geo->getPrimitiveVertexOffset(elemoff, vtxpnum + 2));
                            pos_next = geo->getPos3T<FLOAT_T>(ptoff);

                            dir_prev = dir_next;
                            dir_next = pos_next - pos;
                            dir_next.normalize();
                        }
                        ptoff = geo->vertexPoint(geo->getPrimitiveVertexOffset(elemoff, lastLastIndex));
                        inlinePtGroup->setElement(ptoff, dot(dir_prev, dir_next) >= threshold_dot ^ reverseGroup);
                        if (closed)
                        {
                            pos = pos_next;

                            ptoff = geo->vertexPoint(geo->getPrimitiveVertexOffset(elemoff, 0));
                            pos_next = geo->getPos3T<FLOAT_T>(ptoff);

                            dir_prev = dir_next;
                            dir_next = pos_next - pos;
                            dir_next.normalize();


                            ptoff = geo->vertexPoint(geo->getPrimitiveVertexOffset(elemoff, lastLastIndex + 1));
                            inlinePtGroup->setElement(ptoff, dot(dir_prev, dir_next) >= threshold_dot ^ reverseGroup);
                        }
                    }
                }
            }, subscribeRatio, minGrainSize);
        inlinePtGroup->invalidateGroupEntries();
    }









private:

    const GA_Detail* geo;

    GA_PrimitiveGroupUPtr inPrimGroupUPtr;
    const GA_PrimitiveGroup* inPrimGroup = nullptr;
    const GA_Group* inGroup = nullptr;

    GA_PointGroup* inlinePtGroup = nullptr;

    fpreal threshold_radians;
    bool reverseGroup = false;
    exint subscribeRatio;
    exint minGrainSize;

};















namespace GFE_PrimInlinePoint_Namespace {




//groupPrimInlinePoint_fast(geo, groupName, reverseGroup, delGroup);
static void
groupPrimInlinePoint_fast(
    const GA_Detail* const geo,
    const GA_PrimitiveGroup* const inGroup,
    GA_PointGroup* const inlinePtGroup,
    const fpreal threshold_radians,
    const bool reverseGroup = false,
    const exint subscribeRatio = 64,
    const exint minGrainSize = 16
)
{
    UT_ASSERT_P(inlinePtGroup);
    const fpreal threshold_dot = cos(threshold_radians);
    const GA_SplittableRange geoSplittableRange0(geo->getPrimitiveRange(inGroup));
    UTparallelFor(geoSplittableRange0, [geo, inlinePtGroup, threshold_dot, reverseGroup](const GA_SplittableRange& r)
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

                UT_Vector3 pos, pos_next, dir_prev, dir_next;
                GA_Offset ptoff, ptoff_next;

                const bool closed = geo->getPrimitiveClosedFlag(elemoff);
                ptoff = geo->vertexPoint(geo->getPrimitiveVertexOffset(elemoff, closed ? 0 : 1));
                pos = geo->getPos3(ptoff);


                ptoff_next = geo->vertexPoint(geo->getPrimitiveVertexOffset(elemoff, closed ? vertices.size()-1 : 0));
                pos_next = geo->getPos3(ptoff_next);
                dir_prev = pos - pos_next;
                dir_prev.normalize();

                ptoff_next = geo->vertexPoint(geo->getPrimitiveVertexOffset(elemoff, closed ? 1 : 2));
                pos_next = geo->getPos3(ptoff_next);
                dir_next = pos_next - pos;
                dir_next.normalize();

                for (GA_Size vtxpnum = !closed; vtxpnum < lastLastIndex; ++vtxpnum)
                {
                    ptoff = geo->vertexPoint(geo->getPrimitiveVertexOffset(elemoff, vtxpnum));

                    fpreal dotVal = dot(dir_prev, dir_next);
                    inlinePtGroup->setElement(ptoff, (dotVal >= threshold_dot) ^ reverseGroup);

                    pos = pos_next;

                    ptoff = geo->vertexPoint(geo->getPrimitiveVertexOffset(elemoff, vtxpnum + 2));
                    pos_next = geo->getPos3(ptoff);

                    dir_prev = dir_next;
                    dir_next = pos_next - pos;
                    dir_next.normalize();
                }
                ptoff = geo->vertexPoint(geo->getPrimitiveVertexOffset(elemoff, lastLastIndex));
                inlinePtGroup->setElement(ptoff, (dot(dir_prev, dir_next) >= threshold_dot) ^ reverseGroup);
                if (closed)
                {
                    pos = pos_next;

                    ptoff = geo->vertexPoint(geo->getPrimitiveVertexOffset(elemoff, 0));
                    pos_next = geo->getPos3(ptoff);

                    dir_prev = dir_next;
                    dir_next = pos_next - pos;
                    dir_next.normalize();


                    ptoff = geo->vertexPoint(geo->getPrimitiveVertexOffset(elemoff, lastLastIndex+1));
                    inlinePtGroup->setElement(ptoff, (dot(dir_prev, dir_next) >= threshold_dot) ^ reverseGroup);
                }
            }
        }
    }, subscribeRatio, minGrainSize);
    inlinePtGroup->invalidateGroupEntries();
}


//groupPrimInlinePoint_fast(geo, groupName, reverseGroup, delGroup);
SYS_FORCE_INLINE
static void
groupPrimInlinePoint_fast(
    const GA_Detail* const geo,
    const GA_Group* const inGroup,
    GA_PointGroup* const inlinePtGroup,
    const fpreal threshold_radians,
    const bool reverseGroup = false,
    const exint subscribeRatio = 64,
    const exint minGrainSize = 16
)
{
    if (!inGroup || inGroup->classType() == GA_GROUP_PRIMITIVE)
    {
        groupPrimInlinePoint_fast(geo, static_cast<const GA_PrimitiveGroup*>(inGroup), inlinePtGroup, threshold_radians, reverseGroup, subscribeRatio, minGrainSize);
    }
    else
    {
        const GA_PrimitiveGroupUPtr promotedPrimGroupUPtr = geo->createDetachedPrimitiveGroup();
        GA_PrimitiveGroup* promotedPrimGroup = promotedPrimGroupUPtr.get();
        promotedPrimGroup->combine(inGroup);
        groupPrimInlinePoint_fast(geo, promotedPrimGroup, inlinePtGroup, threshold_radians, reverseGroup, subscribeRatio, minGrainSize);

        if (inGroup->classType() == GA_GROUP_POINT)
        {
            *inlinePtGroup &= *static_cast<const GA_PointGroup*>(inGroup);
        }
        else
        {
            const GA_PointGroupUPtr promotedPointGroupUPtr = geo->createDetachedPointGroup();
            GA_PointGroup* promotedPointGroup = promotedPointGroupUPtr.get();
            promotedPointGroup->combine(inGroup);
            *inlinePtGroup &= *promotedPointGroup;
        }
    }
}


//groupPrimInlinePoint_fast(geo, inGroupType, inGroupName, outGroupName, reverseGroup);
SYS_FORCE_INLINE
static GA_PointGroup*
groupPrimInlinePoint_fast(
    GA_Detail* const geo,
    const GA_Group* const inGroup,
    const UT_StringHolder& outGroupName,
    const fpreal threshold_radians,
    const bool reverseGroup = false,
    const exint subscribeRatio = 64,
    const exint minGrainSize = 16
)
{
    GA_PointGroup* const inlinePtGroup = static_cast<GA_PointGroup*>(geo->pointGroups().newGroup(outGroupName));
    groupPrimInlinePoint_fast(static_cast<const GA_Detail*>(geo), inGroup, inlinePtGroup, threshold_radians, reverseGroup, subscribeRatio, minGrainSize);
    return inlinePtGroup;
}









//groupPrimInlinePoint_fast(geo, inGroupType, inGroupName, ptGroup, reverseGroup);
SYS_FORCE_INLINE
static void
    groupPrimInlinePoint_fast(
        const SOP_NodeVerb::CookParms& cookparms,
        const GA_Detail* const geo,
        const GA_GroupType inGroupType,
        const UT_StringHolder& inGroupName,
        GA_PointGroup* const inlinePtGroup,
        const fpreal threshold_radians,
        const bool reverseGroup = false,
        const exint subscribeRatio = 64,
        const exint minGrainSize = 16
    )
{
    GOP_Manager gop;
    const GA_Group* const inGroup = GFE_GroupParse_Namespace::findOrParseGroupDetached(cookparms, geo, inGroupType, inGroupName, gop);
    if (inGroup && inGroup->isEmpty())
        return;
    groupPrimInlinePoint_fast(geo, inGroup, inlinePtGroup, threshold_radians, reverseGroup, subscribeRatio, minGrainSize);
}


//groupPrimInlinePoint_fast(geo, inGroupType, inGroupName, outGroupName, reverseGroup);
SYS_FORCE_INLINE
static GA_PointGroup*
    groupPrimInlinePoint_fast(
        const SOP_NodeVerb::CookParms& cookparms,
        GA_Detail* const geo,
        const GA_GroupType inGroupType,
        const UT_StringHolder& inGroupName,
        const UT_StringHolder& outGroupName,
        const fpreal threshold_radians,
        const bool reverseGroup = false,
        const exint subscribeRatio = 64,
        const exint minGrainSize = 16
    )
{
    UT_ASSERT_P(geo);
#if 1
    GA_PointGroup* const inlinePtGroup = static_cast<GA_PointGroup*>(geo->pointGroups().newGroup(outGroupName));
#else
    GA_PointGroupTable& pointGroups = geo->pointGroups();
    GA_PointGroup* inlinePtGroup = pointGroups.find(outGroupName);
    if (!inlinePtGroup)
        GA_PointGroup* inlinePtGroup = static_cast<GA_PointGroup*>(pointGroups.newGroup(outGroupName));
#endif
    groupPrimInlinePoint_fast(cookparms, geo, inGroupType, inGroupName, inlinePtGroup, threshold_radians, reverseGroup, subscribeRatio, minGrainSize);
    return inlinePtGroup;
}







//groupPrimInlinePoint_fast(geo, inGroupType, inGroupName, outGroupName, reverseGroup);
SYS_FORCE_INLINE
static GA_PointGroupUPtr
groupDetachedPrimInlinePoint_fast(
    const GA_Detail* const geo,
    const GA_Group* const inGroup,
    const fpreal threshold_radians,
    const bool reverseGroup = false,
    const exint subscribeRatio = 64,
    const exint minGrainSize = 16
)
{
    UT_ASSERT_P(geo);
    GA_PointGroupUPtr inlinePtGroupUPtr = geo->createDetachedPointGroup();
    groupPrimInlinePoint_fast(geo, inGroup, inlinePtGroupUPtr.get(), threshold_radians, reverseGroup, subscribeRatio, minGrainSize);
    return inlinePtGroupUPtr;
}


//groupPrimInlinePoint_fast(geo, inGroupType, inGroupName, outGroupName, reverseGroup);
SYS_FORCE_INLINE
static GA_PointGroupUPtr
    groupDetachedPrimInlinePoint_fast(
        const SOP_NodeVerb::CookParms& cookparms,
        const GA_Detail* const geo,
        const GA_GroupType inGroupType,
        const UT_StringHolder& inGroupName,
        const fpreal threshold_radians,
        const bool reverseGroup = false,
        const exint subscribeRatio = 64,
        const exint minGrainSize = 16
    )
{
    UT_ASSERT_P(geo);
    GA_PointGroupUPtr inlinePtGroupUPtr = geo->createDetachedPointGroup();
    groupPrimInlinePoint_fast(cookparms, geo, inGroupType, inGroupName, inlinePtGroupUPtr.get(), threshold_radians, reverseGroup, subscribeRatio, minGrainSize);
    return inlinePtGroupUPtr;
}







//delPrimInlinePoint_fast(geo, inlinePtGroupUPtr.get(), threshold_radians, reverseGroup, subscribeRatio, minGrainSize);
SYS_FORCE_INLINE
static void
delPrimInlinePoint_fast(
    GA_Detail* const geo,
    const GA_Group* const inGroup,
    const fpreal threshold_radians,
    const bool reverseGroup = false,
    const exint subscribeRatio = 64,
    const exint minGrainSize = 16
)
{
    GA_PointGroupUPtr inlinePtGroupUPtr = groupDetachedPrimInlinePoint_fast(geo, inGroup, threshold_radians, reverseGroup, subscribeRatio, minGrainSize);
    geo->destroyPointOffsets(geo->getPointRange(inlinePtGroupUPtr.get()), GA_Detail::GA_DestroyPointMode::GA_DESTROY_DEGENERATE_INCOMPATIBLE);
}



//delPrimInlinePoint_fast(cookparms,geo, inGroupType, inGroupName, threshold_radians, reverseGroup, subscribeRatio, minGrainSize);
SYS_FORCE_INLINE
static void
    delPrimInlinePoint_fast(
        const SOP_NodeVerb::CookParms& cookparms,
        GA_Detail* const geo,
        const GA_GroupType inGroupType,
        const UT_StringHolder& inGroupName,
        const fpreal threshold_radians,
        const bool reverseGroup = false,
        const exint subscribeRatio = 64,
        const exint minGrainSize = 16
    )
{
    GA_PointGroupUPtr inlinePtGroupUPtr = groupDetachedPrimInlinePoint_fast(cookparms, geo, inGroupType, inGroupName, threshold_radians, reverseGroup, subscribeRatio, minGrainSize);
    geo->destroyPointOffsets(geo->getPointRange(inlinePtGroupUPtr.get()), GA_Detail::GA_DestroyPointMode::GA_DESTROY_DEGENERATE_INCOMPATIBLE);
}










} // End of namespace GFE_PrimInlinePoint


#endif
