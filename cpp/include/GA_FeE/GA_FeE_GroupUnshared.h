
#pragma once

#ifndef __GA_FeE_GroupUnshared_h__
#define __GA_FeE_GroupUnshared_h__

//#include "GA_FeE/GA_FeE_GroupUnshared.h"

#include "GA/GA_Detail.h"


#include "GA_FeE/GA_FeE_Type.h"
#include "GA_FeE/GA_FeE_Group.h"
#include "GA_FeE/GA_FeE_VertexNextEquiv.h"
#include "GA_FeE/GA_FeE_GroupPromote.h"
#include "GA_FeE/GA_FeE_TopologyReference.h"



namespace GA_FeE_GroupUnshared {





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

        const GA_Storage finalStorageI = GA_FeE_Type::getPreferredStorageI(geo);

        //GA_VertexGroup* const unsharedGroup = GA_FeE_VertexNextEquiv::addGroupVertexNextEquiv(geo, geoVtxGroup, inStorageI, "__topo_unshared_SOP_FeE_GroupUnshared_1_0");
        GA_VertexGroup* unsharedGroup = GA_FeE_VertexNextEquiv::addGroupVertexNextEquiv(geo, geoVtxGroup, inStorageI, "__topo_unshared", subscribeRatio, minGrainSize);
        GA_Group* const unshared_promoGroup = GA_FeE_GroupPromote::groupPromote(geo, unsharedGroup, unsharedAttribClass, unsharedAttribName, true);
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
        const GA_VertexGroupUPtr geoVtxGroupUPtr = GA_FeE_GroupPromote::groupPromoteVertexDetached(geo, geoGroup);
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
    const GA_Group* const geoGroup = GA_FeE_Group::findOrParseGroupDetached(cookparms, geo, groupType, sopparms.getGroup(), gop);
    if (geoGroup && geoGroup->isEmpty())
        return;

    GA_FeE_GroupUnshared::groupUnshared(cookparms, geo, groupType, sopparms.getGroup(),
        geo0AttribNames,
        unsharedAttribStorageClass, unsharedAttribClass
        groupUnsharedAfterFuse, sopparms.getOutTopoAttrib(),
        subscribeRatio, minGrainSize
    );


    GA_FeE_TopologyReference::outTopoAttrib(geo, outTopoAttrib);

    if (geoGroup)
    {
        cookparms.getNode()->setHighlight(true);
        cookparms.select(*unshared_promoGroup);
    }
    return unsharedGroup;
}








} // End of namespace GA_FeE_GroupUnshared

#endif