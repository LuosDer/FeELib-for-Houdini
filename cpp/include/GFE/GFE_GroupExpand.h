
#pragma once

#ifndef __GFE_GroupExpand_h__
#define __GFE_GroupExpand_h__

//#include "GFE/GFE_GroupExpand.h"


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
            const bool largeConnectivity = false,
            const size_t numiter = 1,
            const bool outTopoAttrib = true,
            const exint subscribeRatio = 64,
            const exint minGrainSize = 64
        )
    {
        setHasComputed();
        this->largeConnectivity = largeConnectivity;
        this->numiter = numiter;
        this->outTopoAttrib = outTopoAttrib;
        this->subscribeRatio = subscribeRatio;
        this->minGrainSize = minGrainSize;
    }

    SYS_FORCE_INLINE GA_Group* setBaseGroup(const GA_GroupType groupClass, const UT_StringRef& groupName = "")
    { return baseGroup = getOutGroupArray().set(groupClass, groupName); }
    
    SYS_FORCE_INLINE GA_Group* setExpandGroup(const bool detached = false, const UT_StringRef& groupName = "")
    { UT_ASSERT_P(baseGroup); return expandGroup = getOutGroupArray().findOrCreate(detached, baseGroup->classType(), groupName); }

    SYS_FORCE_INLINE GA_Group* setBorderGroup(const bool detached = false, const UT_StringRef& groupName = "")
    { UT_ASSERT_P(baseGroup); return borderGroup = getOutGroupArray().findOrCreate(detached, baseGroup->classType(), groupName); }

    SYS_FORCE_INLINE GA_Group* setPrevBorderGroup(const bool detached = false, const UT_StringRef& groupName = "")
    { UT_ASSERT_P(baseGroup); return prevBorderGroup = getOutGroupArray().findOrCreate(detached, baseGroup->classType(), groupName); }

    
    SYS_FORCE_INLINE void setBaseGroup(GA_Group* const group)
    { baseGroup = group;   getOutGroupArray().append(group); }
    
    SYS_FORCE_INLINE void setExpandGroup(GA_Group* const group)
    { expandGroup = group; getOutGroupArray().append(group); }
    
    SYS_FORCE_INLINE void setBorderGroup(GA_Group* const group)
    { borderGroup = group; getOutGroupArray().append(group); }
    
    SYS_FORCE_INLINE void setPrevBorderGroup(GA_Group* const group)
    { prevBorderGroup = group; getOutGroupArray().append(group); }
    
    
private:


    virtual bool
        computeCore() override
    {
        if (!baseGroup || !expandGroup || !borderGroup || !prevBorderGroup)
            return false;
            
        if (numiter == 0)
            return true;
        
        if (groupParser.isEmpty())
            return true;
        
        elementGroupExpand();
        
        return true;
    }


    
    void edgeGroupExpandEdge()
    {
        GA_EdgeGroup* const expandElemGroup     = static_cast<GA_EdgeGroup*>(expandGroup);
        GA_EdgeGroup* const borderElemGroup     = static_cast<GA_EdgeGroup*>(borderGroup);
        GA_EdgeGroup* const prevBorderElemGroup = static_cast<GA_EdgeGroup*>(prevBorderGroup);
        expandElemGroup->combine(baseGroup);
        borderElemGroup->combine(baseGroup);
    }
    
    void elementGroupExpand()
    {
        const GA_GroupType groupType = baseGroup->classType();


        if (groupType == GA_GROUP_EDGE)
        {
            edgeGroupExpandEdge();
        }
        else
        {
            GA_ElementGroup* const expandElemGroup = static_cast<GA_ElementGroup*>(expandGroup);
            expandElemGroup->combine(baseGroup);

            GFE_MeshTopology meshTopology(geo, cookparms);
            meshTopology.groupParser.setGroup(groupParser);
            const GA_Attribute* const nebsAttrib = meshTopology.setAdjacency(true, groupType, largeConnectivity);
            meshTopology.compute();

            UT_ASSERT_P(nebsAttrib);
            
            const GA_ROHandleT<UT_ValArray<GA_Offset>> nebs_h(nebsAttrib);

            if (numiter == 1)
            {
                //const GA_SplittableRange geoSplittableRange(GFE_Range::getRangeByAnyGroup(baseGroup));
                const GA_SplittableRange geoSplittableRange(geo->getRangeByAnyGroup(baseGroup));
                UTparallelFor(geoSplittableRange, [expandElemGroup, &nebs_h](const GA_SplittableRange& r)
                {
                    // GA_PageHandleT<GA_Offset>::RWType nebs_ph(&attrib);
                    // for (GA_PageIterator pit = r.beginPages(); !pit.atEnd(); ++pit)
                    // {
                    //     GA_Offset start, end;
                    //     for (GA_Iterator it(pit.begin()); it.blockAdvance(start, end); )
                    //     {
                    //         nebs_ph.setPage(start);
                    //         for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                    //         {
                    //             nebs_ph.value(elemoff);
                    //         }
                    //     }
                    // }
                
                    UT_ValArray<GA_Offset> adjElems(32);
                    size_t size;
                    GA_Offset start, end;
                    for (GA_Iterator it(r); it.blockAdvance(start, end); )
                    {
                        for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                        {
                            nebs_h.get(elemoff, adjElems);
                            size = adjElems.size();
                            for (size_t i = 0; i < size; ++i)
                            {
                                expandElemGroup->setElement(adjElems[i], true);
                            }
                        }
                    }
                }, subscribeRatio, minGrainSize);
            }
            else
            {
                GA_ElementGroup* borderElemGroup     = static_cast<GA_ElementGroup*>(borderGroup);
                GA_ElementGroup* prevBorderElemGroup = static_cast<GA_ElementGroup*>(prevBorderGroup);
                borderElemGroup->combine(baseGroup);

                for (size_t iternum = 0; iternum < numiter; iternum++)
                {
                    prevBorderGroup->combine(borderGroup);
                    //const GA_SplittableRange geoSplittableRange(GFE_Range::getRangeByAnyGroup(geo, prevBorderElemGroup));
                    const GA_SplittableRange geoSplittableRange(geo->getRangeByAnyGroup(prevBorderElemGroup));
                    UTparallelFor(geoSplittableRange, [expandElemGroup, borderElemGroup, prevBorderElemGroup, &nebs_h](const GA_SplittableRange& r)
                    {
                        UT_ValArray<GA_Offset> adjElems(32);
                        GA_Offset start, end;
                        for (GA_Iterator it(r); it.blockAdvance(start, end); )
                        {
                            for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                            {
                                borderElemGroup    ->setElement(elemoff, false);
                                prevBorderElemGroup->setElement(elemoff, false);

                                nebs_h.get(elemoff, adjElems);
                                for (int i = 0; i < adjElems.size(); ++i)
                                {
                                    if (expandElemGroup->contains(adjElems[i]))
                                        continue;
                                    borderElemGroup->setElement(adjElems[i], true);
                                }
                            }
                        }
                    }, subscribeRatio, minGrainSize);
                    expandElemGroup->combine(borderElemGroup);
                }

            }

            
        }
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
    bool largeConnectivity = false;
    size_t numiter = 1;
    
    GA_Group* expandGroup     = nullptr;
    GA_Group* borderGroup     = nullptr;
    GA_Group* prevBorderGroup = nullptr;

private:
    //GA_GroupType baseGroupType;
    GA_Group* baseGroup       = nullptr;
    
    exint subscribeRatio = 64;
    exint minGrainSize = 1024;

}; // End of class GFE_GroupExpand



















/*

namespace GFE_GroupExpand_Namespace {




    static void
        edgeGroupExpandEdge(
            GA_Detail* const geo,
            GA_EdgeGroup* const expandGroup,
            const GA_EdgeGroup* const baseGroup,
            const exint subscribeRatio = 64,
            const exint minGrainSize = 1024
        )
    {
        if (!baseGroup || baseGroup->isEmpty())
            return;

        //GA_SplittableRange geoSplittableRange(GFE_Group::getRangeByAnyGroup(geo, group));
        //UTparallelFor(geoSplittableRange, [geo, group, &nebs_h](const GA_SplittableRange& r)
        //{
        //    UT_ValArray<GA_Offset> adjElems;
        //    GA_Offset start, end;
        //    for (GA_Iterator it(r); it.blockAdvance(start, end); )
        //    {
        //        for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
        //        {
        //            nebs_h.get(elemoff, adjElems);
        //            for (GA_Size i = 0; i < adjElems.size(); ++i)
        //            {
        //                group->setElement(adjElems[i], true);
        //            }
        //        }
        //    }
        //}, subscribeRatio, minGrainSize);
    }



    static void
        groupExpand(
            GA_Detail* const geo,
            GA_Group* const expandGroup,
            const GA_Group* const baseGroup,
            const GA_GroupType connectivityGroupType,
            const exint subscribeRatio = 64,
            const exint minGrainSize = 1024
        )
    {
        if (!baseGroup || baseGroup->isEmpty())
        {
            expandGroup->addAll();
            return;
        }

        const GA_GroupType groupType = baseGroup->classType();

        GFE_GroupUnion::groupUnion(expandGroup, baseGroup);

        if (groupType == GA_GROUP_EDGE)
        {
            //const GA_EdgeGroup* baseEdgeGroup = UTverify_cast<const GA_EdgeGroup*>(baseGroup);
            switch (connectivityGroupType)
            {
            case GA_GROUP_PRIMITIVE:
                break;
            case GA_GROUP_POINT:
                break;
            case GA_GROUP_VERTEX:
                break;
            case GA_GROUP_EDGE:
                edgeGroupExpandEdge(geo,
                    UTverify_cast<GA_EdgeGroup*>(expandGroup),
                    UTverify_cast<const GA_EdgeGroup*>(baseGroup),
                    subscribeRatio, minGrainSize);
                break;
            default:
                break;
            }
        }
        else
        {
            GA_ElementGroup* const expandElemGroup = UTverify_cast<GA_ElementGroup*>(expandGroup);

            const GA_Attribute* const nebsAttrib = GFE_Adjacency::addAttribAdjacency(geo, groupType, connectivityGroupType);
            if (!nebsAttrib)
                return;
            GA_ROHandleT<UT_ValArray<GA_Offset>> nebs_h(nebsAttrib);

            GA_SplittableRange geoSplittableRange(GFE_Group::getRangeByAnyGroup(geo, baseGroup));
            UTparallelFor(geoSplittableRange, [expandElemGroup, &nebs_h](const GA_SplittableRange& r)
            {
                UT_ValArray<GA_Offset> adjElems(32);
                GA_Offset start, end;
                for (GA_Iterator it(r); it.blockAdvance(start, end); )
                {
                    for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                    {
                        nebs_h.get(elemoff, adjElems);
                        for (int i = 0; i < adjElems.size(); ++i)
                        {
                            expandElemGroup->setElement(adjElems[i], true);
                        }
                    }
                }
            }, subscribeRatio, minGrainSize);
        }

    }











    static void
        edgeGroupExpandEdge(
            GA_Detail* const geo,
            GA_EdgeGroup* const expandGroup,
            GA_EdgeGroup* const borderGroup,
            GA_EdgeGroup* const prevBorderGroup,
            const GA_EdgeGroup* baseGroup,
            const exint numiter,
            const exint subscribeRatio = 64,
            const exint minGrainSize = 1024
        )
    {
        if (numiter == 0)
            return;

        for (exint iternum = 0; iternum < numiter; iternum++)
        {
            //GA_SplittableRange geoSplittableRange(GFE_Group::getRangeByAnyGroup(geo, group));
            //UTparallelFor(geoSplittableRange, [geo, group, &nebs_h](const GA_SplittableRange& r)
            //{
            //    UT_ValArray<GA_Offset> adjElems;
            //    GA_Offset start, end;
            //    for (GA_Iterator it(r); it.blockAdvance(start, end); )
            //    {
            //        for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
            //        {
            //            nebs_h.get(elemoff, adjElems);
            //            for (GA_Size i = 0; i < adjElems.size(); ++i)
            //            {
            //                group->setElement(adjElems[i], true);
            //            }
            //        }
            //    }
            //}, subscribeRatio, minGrainSize);
        }
    }



    static void
    groupExpand(
        GA_Detail* const geo,
        GA_Group* const expandGroup,
        GA_Group* const borderGroup,
        GA_Group* const prevBorderGroup,
        const GA_Group* baseGroup,
        const GA_GroupType connectivityGroupType,
        const exint numiter,
        const exint subscribeRatio = 64,
        const exint minGrainSize = 1024
    )
    {
        if (numiter == 0 || !baseGroup || baseGroup->isEmpty())
            return;

        if (numiter == 1)
        {
            groupExpand(geo, expandGroup, baseGroup, connectivityGroupType, subscribeRatio, minGrainSize);
            return;
        }

        const GA_GroupType groupType = baseGroup->classType();

        UT_ASSERT_P(groupType == expandGroup->classType() && groupType == borderGroup->classType() && groupType == prevBorderGroup->classType());
        
        //GFE_GroupBoolean::groupUnion(geo, expandGroup, baseGroup);
        //GFE_GroupBoolean::groupUnion(geo, borderGroup, baseGroup);
        if (groupType == GA_GROUP_EDGE)
        {
            GFE_GroupUnion::edgeGroupUnion(expandGroup, baseGroup);
            GFE_GroupUnion::edgeGroupUnion(borderGroup, baseGroup);
            //const GA_EdgeGroup* baseEdgeGroup = UTverify_cast<const GA_EdgeGroup*>(baseGroup);
            switch (connectivityGroupType)
            {
            case GA_GROUP_PRIMITIVE:
                break;
            case GA_GROUP_POINT:
                break;
            case GA_GROUP_VERTEX:
                break;
            case GA_GROUP_EDGE:
                edgeGroupExpandEdge(geo,
                    UTverify_cast<GA_EdgeGroup*>(expandGroup),
                    UTverify_cast<GA_EdgeGroup*>(borderGroup),
                    UTverify_cast<GA_EdgeGroup*>(prevBorderGroup),
                    UTverify_cast<const GA_EdgeGroup*>(baseGroup),
                    numiter, subscribeRatio, minGrainSize);
                break;
            default:
                break;
            }
        }
        else
        {
            GA_ElementGroup* expandElemGroup     = UTverify_cast<GA_ElementGroup*>(expandGroup);
            GA_ElementGroup* borderElemGroup     = UTverify_cast<GA_ElementGroup*>(borderGroup);
            GA_ElementGroup* prevBorderElemGroup = UTverify_cast<GA_ElementGroup*>(prevBorderGroup);
            expandElemGroup->combine(baseGroup);
            borderElemGroup->combine(baseGroup);

            GA_Attribute* nebsAttrib = GFE_Adjacency::addAttribAdjacency(geo, groupType, connectivityGroupType);
            if (!nebsAttrib)
                return;
            GA_RWHandleT<UT_ValArray<GA_Offset>> nebs_h(nebsAttrib);
            for (int iternum = 0; iternum < numiter; iternum++)
            {
                //GEO_FeE_Group::groupUnion(geo, prevBorderGroup, borderGroup);
                prevBorderGroup->combine(borderGroup);
                GA_SplittableRange geoSplittableRange(GFE_Group::getRangeByAnyGroup(geo, prevBorderElemGroup));
                UTparallelFor(geoSplittableRange, [geo, expandElemGroup, borderElemGroup, prevBorderElemGroup, &nebs_h](const GA_SplittableRange& r)
                {
                    UT_ValArray<GA_Offset> adjElems(32);
                    GA_Offset start, end;
                    for (GA_Iterator it(r); it.blockAdvance(start, end); )
                    {
                        for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                        {
                            borderElemGroup    ->setElement(elemoff, false);
                            prevBorderElemGroup->setElement(elemoff, false);

                            nebs_h.get(elemoff, adjElems);
                            for (int i = 0; i < adjElems.size(); ++i)
                            {
                                if (expandElemGroup->contains(adjElems[i]))
                                    continue;
                                borderElemGroup->setElement(adjElems[i], true);
                                
                            }
                        }
                    }
                }, subscribeRatio, minGrainSize);
                expandElemGroup->combine(borderElemGroup);
            }
        }

    }


    static void
        groupExpand(
            const SOP_NodeVerb::CookParms& cookparms,
            GA_Detail* const geo,
            GA_Group* const expandGroup,
            GA_Group* const borderGroup,
            GA_Group* const prevBorderGroup,
            const GA_GroupType baseGroupType,
            const UT_StringHolder& baseGroupName,
            const GA_GroupType connectivityGroupType,
            const exint numiter,
            const bool outTopoAttrib,
            const exint subscribeRatio = 64,
            const exint minGrainSize = 1024
        )
    {
        GOP_Manager gop;
        const GA_Group* geo0Group = GFE_GroupParser_Namespace::findOrParseGroupDetached(cookparms, geo, baseGroupType, baseGroupName, gop);

        if (!geo0Group)
            return;

        GFE_TopologyReference::outTopoAttrib(geo, outTopoAttrib);


        groupExpand(geo, expandGroup, borderGroup, prevBorderGroup, geo0Group, GA_GROUP_EDGE, numiter, subscribeRatio, minGrainSize);
        //cookparms.selectInputGroup(expandGroup, expandGroup->classType());
        //cookparms.select(outGeo0->getPrimitiveRange(static_cast<GA_PrimitiveGroup*>(expandGroup)), expandGroup->classType());
        cookparms.getNode()->setHighlight(true);
        cookparms.select(*expandGroup);
    }



    //static void
    //    pointGroupExpandEdge(
    //        GA_Detail* const geo,
    //        GA_PointGroup* const group,
    //        const exint numiter,
    //        const exint subscribeRatio = 64,
    //        const exint minGrainSize = 1024
    //    )
    //{
    //    if (numiter == 0)
    //        return;

    //    GA_RWHandleT<UT_ValArray<GA_Offset>> nebs_h = GFE_Adjacency::addAttribPointPointEdge(geo);

    //    for (exint iternum = 0; iternum < numiter; iternum++)
    //    {
    //        GA_SplittableRange geoSplittableRange(GFE_Group::getRangeByAnyGroup(geo, group));
    //        UTparallelFor(geoSplittableRange, [geo, group, &nebs_h](const GA_SplittableRange& r)
    //        {
    //            UT_ValArray<GA_Offset> adjElems;
    //            GA_Offset start, end;
    //            for (GA_Iterator it(r); it.blockAdvance(start, end); )
    //            {
    //                for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
    //                {
    //                    nebs_h.get(elemoff, adjElems);
    //                    for (GA_Size i = 0; i < adjElems.size(); ++i)
    //                    {
    //                        group->setElement(adjElems[i], true);
    //                    }
    //                }
    //            }
    //        }, subscribeRatio, minGrainSize);
    //    }
    //}


    //static void
    //groupExpand1(
    //    GA_Detail* const geo,
    //    GA_Group* const group,
    //    const GA_GroupType connectivityGroupType,
    //    const exint numiter,
    //    const exint subscribeRatio = 64,
    //    const exint minGrainSize = 1024
    //)
    //{
    //    if (numiter == 0)
    //        return;

    //    GA_GroupType groupType = group->classType();
    //    switch (groupType)
    //    {
    //    case GA_GROUP_PRIMITIVE:
    //        break;
    //    case GA_GROUP_POINT:
    //        switch (connectivityGroupType)
    //        {
    //        case GA_GROUP_PRIMITIVE:
    //            break;
    //        case GA_GROUP_POINT:
    //            break;
    //        case GA_GROUP_VERTEX:
    //            break;
    //        case GA_GROUP_EDGE:
    //            pointGroupExpandEdge(geo, UTverify_cast<GA_PointGroup*>(group), numiter, subscribeRatio, minGrainSize);
    //            break;
    //        default:
    //            break;
    //        }


    //        break;
    //    case GA_GROUP_VERTEX:
    //        break;
    //    case GA_GROUP_EDGE:
    //        break;
    //    default:
    //        break;
    //    }

    //}








} // End of namespace GFE_GroupExpand


*/

#endif
