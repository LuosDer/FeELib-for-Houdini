
#pragma once

#ifndef __GFE_GroupUnshared_h__
#define __GFE_GroupUnshared_h__

//#include "GFE/GFE_GroupUnshared.h"

#include "GA/GA_Detail.h"


#include "GFE/GFE_Type.h"
#include "GFE/GFE_Group.h"
#include "GFE/GFE_VertexNextEquiv.h"
#include "GFE/GFE_GroupPromote.h"
#include "GFE/GFE_TopologyReference.h"



namespace GFE_GroupUnshared {





    static GA_Group*
        groupUnshared(
            GA_Detail* const geo,
            const GA_VertexGroup* const geoVtxGroup,
            const UT_StringHolder& unsharedAttribName = "unshared",
            const GA_StorageClass unsharedAttribStorageClass = GA_STORECLASS_REAL,
            const GA_GroupType unsharedAttribClass = GA_GROUP_VERTEX,
            const bool groupUnsharedAfterFuse = false,
            const GA_Storage inStorageI = GA_STORE_INVALID,
            const bool outTopoAttrib = false,
            const exint subscribeRatio = 64,
            const exint minGrainSize = 64
        )
    {
        UT_ASSERT_P(geo);

        const GA_Storage finalStorageI = GFE_Type::getPreferredStorageI(geo);

        //GA_VertexGroup* const unsharedGroup = GFE_VertexNextEquiv::addGroupVertexNextEquiv(geo, geoVtxGroup, inStorageI, "__topo_unshared_SOP_FeE_GroupUnshared_1_0");
        GA_VertexGroup* unsharedGroup = GFE_VertexNextEquiv::addGroupVertexNextEquiv(geo, geoVtxGroup, inStorageI, "__topo_unshared", subscribeRatio, minGrainSize);
        GA_Group* const unshared_promoGroup = GFE_GroupPromote::groupPromote(geo, unsharedGroup, unsharedAttribClass, unsharedAttribName, true);
        return unshared_promoGroup;
    }




    static GA_Group*
        groupUnshared(
            const GA_Detail* const geo,
            const GA_Group* const geoGroup,
            const UT_StringHolder& unsharedAttribName = "unshared",
            const GA_StorageClass unsharedAttribStorageClass = GA_STORECLASS_REAL,
            const GA_GroupType unsharedAttribClass = GA_GROUP_VERTEX,
            const bool groupUnsharedAfterFuse = false,
            const GA_Storage inStorageI = GA_STORE_INVALID,
            const bool outTopoAttrib = false,
            const exint subscribeRatio = 64,
            const exint minGrainSize = 64
        )
    {
        UT_ASSERT_P(geo);

        if (geoGroup && geoGroup->get == )
        {
            return groupUnshared(cookparms, geo, groupType, sopparms.getGroup()
                geo0AttribNames
                unsharedAttribStorageClass, unsharedAttribClass
                groupUnsharedAfterFuse, sopparms.getOutTopoAttrib(),
                subscribeRatio, minGrainSize
            );
        }
        const GA_VertexGroupUPtr geoVtxGroupUPtr = GFE_GroupPromote::groupPromoteVertexDetached(geo, geoGroup);
        //GA_VertexGroup* geoVtxGroup = static_cast<GA_VertexGroup*>(geoVtxGroupUPtr.get());
        const GA_VertexGroup* const geoVtxGroup = geoVtxGroupUPtr.get();


        return groupUnshared(geo, geoVtxGroup,
            geo0AttribNames
            unsharedAttribStorageClass, unsharedAttribClass
            groupUnsharedAfterFuse, sopparms.getOutTopoAttrib(),
            subscribeRatio, minGrainSize
        );


    }





static GA_Group*
    groupUnshared(
    const SOP_NodeVerb::CookParms& cookparms,
    const GA_Detail* const geo,
    const GA_GroupType groupType,
    const UT_StringHolder& groupName,
    const UT_StringHolder& unsharedAttribName = "unshared",
    const GA_StorageClass unsharedAttribStorageClass = GA_STORECLASS_REAL,
    const GA_GroupType unsharedAttribClass = GA_GROUP_VERTEX,
    const bool groupUnsharedAfterFuse = false,
    const GA_Storage inStorageI = GA_STORE_INVALID,
    const bool outTopoAttrib = false,
    const exint subscribeRatio = 64,
    const exint minGrainSize = 64
    )
{
    UT_ASSERT_P(geo);
    UT_ASSERT_P(group);


    GOP_Manager gop;
    const GA_Group* const geoGroup = GFE_Group::findOrParseGroupDetached(cookparms, geo, groupType, sopparms.getGroup(), gop);
    if (geoGroup && geoGroup->isEmpty())
        return;

    GFE_GroupUnshared::groupUnshared(cookparms, geo, groupType, sopparms.getGroup(),
        geo0AttribNames,
        unsharedAttribStorageClass, unsharedAttribClass
        groupUnsharedAfterFuse, sopparms.getOutTopoAttrib(),
        subscribeRatio, minGrainSize
    );


    GFE_TopologyReference::outTopoAttrib(geo, outTopoAttrib);

    if (geoGroup)
    {
        cookparms.getNode()->setHighlight(true);
        cookparms.select(*unshared_promoGroup);
    }
    return unsharedGroup;
}








} // End of namespace GFE_GroupUnshared

#endif