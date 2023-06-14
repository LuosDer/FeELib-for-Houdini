
#pragma once

#ifndef __GFE_GroupExpand_h__
#define __GFE_GroupExpand_h__

#include "GFE/GFE_GroupExpand.h"


#include "GFE/GFE_GeoFilter.h"

#include "GFE/GFE_MeshTopology.h"
//#include "GFE/GFE_GroupUnion.h"
//#include "GFE/GFE_Range.h"


//#include "GFE/GFE_Measure.h"
//#include "GFE/GFE_Connectivity.h"



class GFE_GroupExpand : public GFE_AttribFilter {

public:
    using GFE_AttribFilter::GFE_AttribFilter;

    void
        setComputeParm(
            const size_t numstep         = 1,
            const bool largeConnectivity = false,
            const exint subscribeRatio   = 64,
            const exint minGrainSize     = 64
        )
    {
        setHasComputed();
        this->numstep           = numstep;
        this->largeConnectivity = largeConnectivity;
        this->subscribeRatio    = subscribeRatio;
        this->minGrainSize      = minGrainSize;
    }

    SYS_FORCE_INLINE const GA_Group* setBaseGroup(const GA_GroupType groupClass, const UT_StringRef& groupName = "")
    { return baseGroup = groupParser.setGroup(groupClass, groupName); }
    
    SYS_FORCE_INLINE GA_Group* setExpandGroup(const bool detached = false, const UT_StringRef& groupName = "")
    { UT_ASSERT_P(baseGroup); return expandGroup = getOutGroupArray().findOrCreate(detached, baseGroup->classType(), groupName); }

    SYS_FORCE_INLINE GA_Group* setBorderGroup(const bool detached = false, const UT_StringRef& groupName = "")
    { UT_ASSERT_P(baseGroup); return borderGroup = getOutGroupArray().findOrCreate(detached, baseGroup->classType(), groupName); }

    //SYS_FORCE_INLINE GA_Group* setPrevBorderGroup(const bool detached = false, const UT_StringRef& groupName = "")
    //{ UT_ASSERT_P(baseGroup); return prevBorderGroup = getOutGroupArray().findOrCreate(detached, baseGroup->classType(), groupName); }

    
    SYS_FORCE_INLINE void setBaseGroup(const GA_Group* const group)
    { baseGroup = group; groupParser.setGroup(group); }
    
    SYS_FORCE_INLINE void setExpandGroup(GA_Group* const group)
    { expandGroup = group; getOutGroupArray().append(group); }
    
    SYS_FORCE_INLINE void setBorderGroup(GA_Group* const group)
    { borderGroup = group; getOutGroupArray().append(group); }
    
    //SYS_FORCE_INLINE void setPrevBorderGroup(GA_Group* const group)
    //{ prevBorderGroup = group; getOutGroupArray().append(group); }



    
    virtual SYS_FORCE_INLINE void visualizeOutGroup() const override
    { if (!doDelGroupElement) visualizeGroup(expandGroup ? expandGroup : (borderGroup ? borderGroup : baseGroup)); }

    
private:


virtual bool
    computeCore() override
{
    if (numstep == 0)
        return true;

    //if (groupParser.isEmpty())
    //    return false;
    
    baseGroup = groupParser.getGroup();
    if (!baseGroup)
        return false;
        
    if (numstep == 1)
    {
        if (!expandGroup)
            setExpandGroup(true);
    }
    else
    {
        if (!expandGroup)
            setExpandGroup(true);
        if (!borderGroup)
            setExpandGroup(true);
    }
    
    groupType = baseGroup->classType();
    
    // if (groupParser.isEmpty())
    //     return true;
    
    if (groupType == GA_GROUP_EDGE)
        edgeGroupExpandEdge();
    else
        elementGroupExpand();
    
    return true;
}



SYS_FORCE_INLINE void edgeGroupExpandEdge()
{
    const GA_EdgeGroupUPtr prevBorderGroupUPtr = geo->createDetachedEdgeGroup();
    GA_EdgeGroup* const prevBorderGroup = prevBorderGroupUPtr.get();
    GA_EdgeGroup* const expandEdgeGroup = static_cast<GA_EdgeGroup*>(expandGroup);
    GA_EdgeGroup* const borderEdgeGroup = static_cast<GA_EdgeGroup*>(borderGroup);
    expandEdgeGroup->combine(baseGroup);
    borderEdgeGroup->combine(baseGroup);
    prevBorderGroup->combine(borderEdgeGroup);
}

SYS_FORCE_INLINE void elementGroupExpand()
{
    GA_ElementGroup* const expandElemGroup = static_cast<GA_ElementGroup*>(expandGroup);
    GA_ElementGroup* const borderElemGroup = static_cast<GA_ElementGroup*>(borderGroup);
    if (expandElemGroup)
        expandElemGroup->combine(baseGroup);
    
    GA_ElementGroup* const prevBorderGroup = getOutGroupArray().findOrCreateElement(true, groupType);
    GA_SplittableRange geoSplittableRange;

    
    GFE_MeshTopology meshTopology(geo, cookparms);
    //meshTopology.groupParser.setGroup(groupParser);
    const GA_Attribute* const nebsAttrib = meshTopology.setAdjacency(true, groupType, largeConnectivity);
    meshTopology.compute();
    const GA_ROHandleT<UT_ValArray<GA_Offset>> nebs_h(nebsAttrib);
    UT_ASSERT_P(nebsAttrib);

    
    if (numstep == 1)
    {
        prevBorderGroup->combine(baseGroup);
        geoSplittableRange = GA_SplittableRange(GA_Range(*prevBorderGroup));
        UTparallelFor(geoSplittableRange, [prevBorderGroup, expandElemGroup, borderElemGroup, &nebs_h](const GA_SplittableRange& r)
        {
            UT_ValArray<GA_Offset> adjElems(32);
            GA_Offset start, end;
            for (GA_Iterator it(r); it.blockAdvance(start, end); )
            {
                for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                {
                    nebs_h.get(elemoff, adjElems);
                    const size_t size = adjElems.size();
                    for (size_t i = 0; i < size; ++i)
                    {
                        if (prevBorderGroup->contains(adjElems[i]))
                            continue;
                        if (expandElemGroup)
                            expandElemGroup->setElement(adjElems[i], true);
                        if (borderElemGroup)
                            borderElemGroup->setElement(adjElems[i], true);
                    }
                }
            }
        }, subscribeRatio, minGrainSize);
    }
    else
    {
        borderElemGroup->combine(baseGroup);

        for (size_t iternum = 0; iternum < numstep; iternum++)
        {
            prevBorderGroup->combine(borderGroup);
            geoSplittableRange = GA_SplittableRange(GA_Range(*prevBorderGroup));
            UTparallelFor(geoSplittableRange, [expandElemGroup, borderElemGroup, prevBorderGroup, &nebs_h](const GA_SplittableRange& r)
            {
                UT_ValArray<GA_Offset> adjElems(32);
                GA_Offset start, end;
                for (GA_Iterator it(r); it.blockAdvance(start, end); )
                {
                    for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                    {
                        borderElemGroup->setElement(elemoff, false);
                        prevBorderGroup->setElement(elemoff, false);

                        nebs_h.get(elemoff, adjElems);
                        const GA_Size size = adjElems.size();
                        for (GA_Size i = 0; i < size; ++i)
                        {
                            if (expandElemGroup->contains(adjElems[i]))
                                continue;
                            expandElemGroup->setElement(adjElems[i], true);
                            borderElemGroup->setElement(adjElems[i], true);
                        }
                    }
                }
            }, subscribeRatio, minGrainSize);
        }
        borderElemGroup->invalidateGroupEntries();
    }
    expandElemGroup->invalidateGroupEntries();
}


public:


    //
    // static void
    //     groupExpand(
    //         GA_Detail* const geo,
    //         GA_Group* const expandGroup,
    //         const GA_Group* const baseGroup,
    //         const GA_GroupType connectivityGroupType,
    //         const exint subscribeRatio = 64,
    //         const exint minGrainSize = 1024
    //     )
    // {
    //     if (!baseGroup || baseGroup->isEmpty())
    //     {
    //         expandGroup->addAll();
    //         return;
    //     }
    //
    //     const GA_GroupType groupType = baseGroup->classType();
    //
    //     GFE_GroupUnion::groupUnion(expandGroup, baseGroup);
    //
    //     if (groupType == GA_GROUP_EDGE)
    //     {
    //         //const GA_EdgeGroup* baseEdgeGroup = UTverify_cast<const GA_EdgeGroup*>(baseGroup);
    //         switch (connectivityGroupType)
    //         {
    //         case GA_GROUP_PRIMITIVE:
    //             break;
    //         case GA_GROUP_POINT:
    //             break;
    //         case GA_GROUP_VERTEX:
    //             break;
    //         case GA_GROUP_EDGE:
    //             edgeGroupExpandEdge(geo,
    //                 UTverify_cast<GA_EdgeGroup*>(expandGroup),
    //                 UTverify_cast<const GA_EdgeGroup*>(baseGroup),
    //                 subscribeRatio, minGrainSize);
    //             break;
    //         default:
    //             break;
    //         }
    //     }
    //     else
    //     {
    //         GA_ElementGroup* const expandElemGroup = UTverify_cast<GA_ElementGroup*>(expandGroup);
    //
    //         const GA_Attribute* const nebsAttrib = GFE_Adjacency::addAttribAdjacency(geo, groupType, connectivityGroupType);
    //         if (!nebsAttrib)
    //             return;
    //         GA_ROHandleT<UT_ValArray<GA_Offset>> nebs_h(nebsAttrib);
    //
    //         GA_SplittableRange geoSplittableRange(GFE_Group::getRangeByAnyGroup(geo, baseGroup));
    //         UTparallelFor(geoSplittableRange, [expandElemGroup, &nebs_h](const GA_SplittableRange& r)
    //         {
    //             UT_ValArray<GA_Offset> adjElems(32);
    //             GA_Offset start, end;
    //             for (GA_Iterator it(r); it.blockAdvance(start, end); )
    //             {
    //                 for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
    //                 {
    //                     nebs_h.get(elemoff, adjElems);
    //                     for (int i = 0; i < adjElems.size(); ++i)
    //                     {
    //                         expandElemGroup->setElement(adjElems[i], true);
    //                     }
    //                 }
    //             }
    //         }, subscribeRatio, minGrainSize);
    //     }
    //
    // }
    //
    //
    //
    // static void
    //     edgeGroupExpandEdge(
    //         GA_Detail* const geo,
    //         GA_EdgeGroup* const expandGroup,
    //         const GA_EdgeGroup* const baseGroup,
    //         const exint subscribeRatio = 64,
    //         const exint minGrainSize = 1024
    //     )
    // {
    //     if (!baseGroup || baseGroup->isEmpty())
    //         return;
    //
    //     GA_SplittableRange geoSplittableRange(GFE_Group::getRangeByAnyGroup(geo, group));
    //     UTparallelFor(geoSplittableRange, [geo, group, &nebs_h](const GA_SplittableRange& r)
    //     {
    //         UT_ValArray<GA_Offset> adjElems;
    //         GA_Offset start, end;
    //         for (GA_Iterator it(r); it.blockAdvance(start, end); )
    //         {
    //             for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
    //             {
    //                 nebs_h.get(elemoff, adjElems);
    //                 for (GA_Size i = 0; i < adjElems.size(); ++i)
    //                 {
    //                     group->setElement(adjElems[i], true);
    //                 }
    //             }
    //         }
    //     }, subscribeRatio, minGrainSize);
    // }


public:
    size_t numstep = 1;
    bool largeConnectivity = false;
    
    GA_Group* expandGroup = nullptr;
    GA_Group* borderGroup = nullptr;
    //GA_Group* prevBorderGroup = nullptr;

private:
    //GA_GroupType baseGroupType;
    GA_GroupType groupType;
    const GA_Group* baseGroup = nullptr;
    
    exint subscribeRatio = 64;
    exint minGrainSize = 1024;

}; // End of class GFE_GroupExpand





#endif
