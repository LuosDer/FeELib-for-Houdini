
#pragma once

#ifndef __GA_FeE_GeoProperty_h__
#define __GA_FeE_GeoProperty_h__

//#include "GA_FeE/GA_FeE_GeoProperty.h"

//#include "GA/GA_Detail.h"
#include "GEO/GEO_Detail.h"
#include "GA/GA_SplittableRange.h"

//#include "GA_FeE/GA_FeE_Type.h
#include "GA_FeE/GA_FeE_Detail.h"
#include "GA_FeE/GA_FeE_Group.h"

namespace GA_FeE_GeoProperty {







//groupPrimInlinePoint_fast(geo, groupName, reverseGroup, delGroup);
static void
groupPrimInlinePoint_fast(
    const GA_Detail* const geo,
    const GA_PrimitiveGroup* const inGroup,
    GA_PointGroup* const inlinePtGroup,
    const bool reverseGroup = false,
    const exint subscribeRatio = 64,
    const exint minGrainSize = 16
)
{
    const GA_SplittableRange geoSplittableRange0(geo->getPrimitiveRange(inGroup));
    UTparallelFor(geoSplittableRange0, [geo, inlinePtGroup](const GA_SplittableRange& r)
    {
        GA_Offset start, end;
        for (GA_Iterator it(r); it.blockAdvance(start, end); )
        {
            for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
            {
                const GA_OffsetListRef vertices = geo->getPrimitiveVertexList(elemoff);
                const GA_Size numvtx = vertices.size();
                if (numvtx <= 2)
                    continue;

                UT_Vector3 pos, pos_prev, pos_next, dir_prev, dir_next;
                GA_Offset vtxoff, ptoff, ptoff_next;

                vtxoff = geo->getPrimitiveVertexOffset(elemoff, 0);
                ptoff = geo->vertexPoint(vtxoff);
                pos_prev = geo->getPos3(ptoff_next);

                vtxoff = geo->getPrimitiveVertexOffset(elemoff, 1);
                ptoff = geo->vertexPoint(vtxoff);
                pos = geo->getPos3(ptoff_next);

                vtxoff = geo->getPrimitiveVertexOffset(elemoff, 2);
                ptoff = geo->vertexPoint(vtxoff);
                pos_next = geo->getPos3(ptoff_next);

                dir_prev = pos - pos_prev;
                dir_next = pos_next - pos;

                const GA_Size lastIndex = numvtx - 1;
                for (GA_Size vtxpnum = 1; vtxpnum < lastIndex; ++vtxpnum)
                {
                    vtxoff = geo->getPrimitiveVertexOffset(elemoff, 1);
                    ptoff = geo->vertexPoint(vtxoff);

                    pos_next = geo->getPos3(ptoff);
                    dir_prev = dir_next;
                    dir_next = pos_next - pos;
                    inlinePtGroup->addOffset(ptoff);

                    pos_prev = pos;
                    pos =
                        vtxoff = geo->getPrimitiveVertexOffset(elemoff, vtxpnum);
                    GA_Offset ptoff = geo->vertexPoint(vtxoff);
                    pos_next = geo->getPos3(ptoff);

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
    const bool reverseGroup = false,
    const exint subscribeRatio = 64,
    const exint minGrainSize = 16
)
{
    if (inGroup->classType() == GA_GROUP_PRIMITIVE)
    {
        groupPrimInlinePoint_fast(geo, static_cast<const GA_PrimitiveGroup*>(inGroup), inlinePtGroup, reverseGroup, subscribeRatio, minGrainSize);
    }
    else
    {
        const GA_PrimitiveGroupUPtr promotedGroupUPtr = geo->createDetachedPrimitiveGroup();
        GA_PrimitiveGroup* promotedGroup = promotedGroupUPtr.get();
        promotedGroup->combine(inGroup);
        groupPrimInlinePoint_fast(geo, promotedGroup, inlinePtGroup, reverseGroup, subscribeRatio, minGrainSize);
    }
}


//groupPrimInlinePoint_fast(geo, inGroupType, inGroupName, ptGroup, reverseGroup);
SYS_FORCE_INLINE
static void
    groupPrimInlinePoint_fast(
        const SOP_NodeVerb::CookParms& cookparms,
        const GEO_Detail* const geo,
        const GA_GroupType inGroupType,
        const UT_StringHolder& inGroupName,
        GA_PointGroup* const inlinePtGroup,
        const bool reverseGroup = false,
        const exint subscribeRatio = 64,
        const exint minGrainSize = 16
    )
{
    GOP_Manager gop;
    const GA_Group* const inGroup = GA_FeE_Group::findOrParseGroupDetached(cookparms, geo, inGroupType, inGroupName, gop);
    if (inGroup && inGroup->isEmpty())
        return;
    groupPrimInlinePoint_fast(geo, inGroup, inlinePtGroup, reverseGroup, subscribeRatio, minGrainSize);
}


//groupPrimInlinePoint_fast(geo, inGroupType, inGroupName, outGroupName, reverseGroup);
SYS_FORCE_INLINE
static GA_PointGroup*
    groupPrimInlinePoint_fast(
        const SOP_NodeVerb::CookParms& cookparms,
        GEO_Detail* const geo,
        const GA_GroupType inGroupType,
        const UT_StringHolder& inGroupName,
        const UT_StringHolder& outGroupName,
        const bool reverseGroup = false,
        const exint subscribeRatio = 64,
        const exint minGrainSize = 16
    )
{
    UT_ASSERT_P(geo);
    GA_PointGroup* const inlinePtGroup = static_cast<GA_PointGroup*>(geo->pointGroups().newGroup(outGroupName));
    groupPrimInlinePoint_fast(cookparms, geo, inGroupType, inGroupName, inlinePtGroup, reverseGroup, subscribeRatio, minGrainSize);
    return inlinePtGroup;
}


//groupPrimInlinePoint_fast(geo, inGroupType, inGroupName, outGroupName, reverseGroup);
SYS_FORCE_INLINE
static GA_PointGroupUPtr
    groupDetachedPrimInlinePoint_fast(
        const SOP_NodeVerb::CookParms& cookparms,
        const GEO_Detail* const geo,
        const GA_GroupType inGroupType,
        const UT_StringHolder& inGroupName,
        const bool reverseGroup = false,
        const exint subscribeRatio = 64,
        const exint minGrainSize = 16
    )
{
    UT_ASSERT_P(geo);
    GA_PointGroupUPtr inlinePtGroupUPtr = geo->createDetachedPointGroup();
    GA_PointGroup* const inlinePtGroup = inlinePtGroupUPtr.get();
    groupPrimInlinePoint_fast(cookparms, geo, inGroupType, inGroupName, inlinePtGroup, reverseGroup, subscribeRatio, minGrainSize);
    return inlinePtGroupUPtr;
}


//delPrimInlinePoint_fast(geo, inGroupType, inGroupName, outGroupName, reverseGroup);
static void
    delPrimInlinePoint_fast(
        const SOP_NodeVerb::CookParms& cookparms,
        GEO_Detail* const geo,
        const GA_GroupType inGroupType,
        const UT_StringHolder& inGroupName,
        const bool reverseGroup = false,
        const exint subscribeRatio = 64,
        const exint minGrainSize = 16
    )
{
    GA_PointGroupUPtr inlinePtGroupUPtr = groupDetachedPrimInlinePoint_fast(cookparms, geo, inGroupType, inGroupName, reverseGroup, subscribeRatio, minGrainSize);
    GA_PointGroup* const inlinePtGroup = inlinePtGroupUPtr.get();
    geo->destroyPointOffsets(geo->getPointRange(inlinePtGroup), GA_Detail::GA_DestroyPointMode::GA_DESTROY_DEGENERATE_INCOMPATIBLE);
}














} // End of namespace GA_FeE_GeoProperty

#endif
