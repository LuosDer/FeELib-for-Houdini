
#pragma once

#ifndef __GEO_FeE_GroupExpand_h__
#define __GEO_FeE_GroupExpand_h__

//#include <GA_FeE/GEO_FeE_GroupExpand.h>

#include <GEO/GEO_Detail.h>

#include <GA_FeE/GA_FeE_Group.h>
#include <GA_FeE/GA_FeE_Adjacency.h>


namespace GEO_FeE_GroupExpand {


    SYS_FORCE_INLINE
    static void
        pointGroupExpand(
            GEO_Detail* geo,
            GA_PointGroup* group,
            const exint numiter,
            const exint subscribeRatio = 64,
            const exint minGrainSize = 1024
        )
    {
        if (numiter == 0)
            return;

        GA_RWHandleT<UT_ValArray<GA_Offset>> nebsAttribH = GA_FeE_Adjacency::addAttribPointPointEdge(geo, "__topo_nebs", nullptr, nullptr, GA_STORE_INT32, nullptr);

        for (exint iternum = 0; iternum < numiter; iternum++)
        {
            GA_SplittableRange geoSplittableRange(GA_FeE_Group::getRangeByAnyGroup(geo, group));
            UTparallelFor(geoSplittableRange, [&geo, &group, &nebsAttribH](const GA_SplittableRange& r)
            {
                UT_ValArray<GA_Offset> adjElems;
                GA_Offset start, end;
                for (GA_Iterator it(r); it.blockAdvance(start, end); )
                {
                    for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                    {
                        nebsAttribH.get(elemoff, adjElems);
                        for (GA_Size i = 0; i < adjElems.size(); ++i)
                        {
                            group->setElement(adjElems[i], true);
                        }
                    }
                }
            }, subscribeRatio, minGrainSize);
        }
    }









} // End of namespace GEO_FeE_GroupExpand

#endif
