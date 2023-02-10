
#pragma once

#ifndef __GA_FeE_GroupNewEdge_h__
#define __GA_FeE_GroupNewEdge_h__

//#include "GA_FeE/GA_FeE_GroupNewEdge.h"

#include "GA/GA_Detail.h"

#include "GA_FeE/GA_FeE_TopologyReference.h"



namespace GA_FeE_GroupNewEdge {

    



    static void
        groupNewEdge(
            const GA_Detail* const geo,
            GA_EdgeGroup* const group,
            const GA_Group* const groupRef
        )
    {
        UT_ASSERT_P(geo);
        UT_ASSERT_P(group);
    }





    static GA_VertexGroup*
        addGroupNewEdge(
            const GA_Detail* const geo,
        )
    {
        UT_ASSERT_P(geo);
        UT_ASSERT_P(group);
    }





} // End of namespace GA_FeE_GroupNewEdge

#endif
