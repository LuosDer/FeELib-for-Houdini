
#pragma once

#ifndef __GFE_GroupNewEdge_h__
#define __GFE_GroupNewEdge_h__

//#include "GFE/GFE_GroupNewEdge.h"

#include "GA/GA_Detail.h"

#include "GFE/GFE_TopologyReference.h"



namespace GFE_GroupNewEdge {

    



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





} // End of namespace GFE_GroupNewEdge

#endif
