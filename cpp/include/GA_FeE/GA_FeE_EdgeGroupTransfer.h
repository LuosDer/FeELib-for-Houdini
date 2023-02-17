
#pragma once

#ifndef __GA_FeE_EdgeGroupTransfer_h__
#define __GA_FeE_EdgeGroupTransfer_h__

//#include "GA_FeE/GA_FeE_EdgeGroupTransfer.h"

#include "GA/GA_Detail.h"
#include "GU/GU_Snap.h"


#include "GA_FeE/GA_FeE_Group.h"
#include "GA_FeE/GA_FeE_GroupPromote.h"
#include "GA_FeE/GA_FeE_Type.h"

namespace GA_FeE_EdgeGroupTransfer {


static void
edgeGroupTransfer(
    GA_Detail* const geo,
    const GA_Detail* const geoRef,
    const GA_VertexGroup* const geoVtxGroup,
    GA_EdgeGroup* const edgeGroup,

    const bool useSnapDist = true,
    const fpreal snapDist = 0.001,
    const bool useNewGroupName = false,
    const UT_StringHolder& newGroupName = "",
    const bool reverseGroup = false,
    const bool outAsVertexGroup = false,
    const exint subscribeRatio = 64,
    const exint minGrainSize = 64
    )
{
    UT_ASSERT_P(geo);
    UT_ASSERT_P(geoRef);

    GA_Attribute* snapDstPointAttribPtr = geo->getAttributes().createTupleAttribute(GA_ATTRIB_POINT, "snapDstPoint", GA_STORE_INT64, 1, GA_Defaults(-1));
    const GA_RWHandleID snapDstPoint_h(snapDstPointAttribPtr);


    GU_Snap::PointSnapParms pointSnapParms;
    pointSnapParms.myAlgorithm = GU_Snap::PointSnapParms::ALGORITHM_CLOSEST_POINT;
    pointSnapParms.myDistance = useSnapDist ? snapDist : SYS_FP64_MAX;
    pointSnapParms.myOutputAttribH = snapDstPoint_h;


    GU_Snap::snapPoints(*static_cast<GU_Detail*>(geo), static_cast<const GU_Detail*>(geoRef), pointSnapParms);


}


SYS_FORCE_INLINE
    static void
    edgeGroupTransfer(
        GA_Detail* const geo,
        const GA_Detail* const geoRef,
        const GA_VertexGroup* const geoVtxGroup,

        const UT_StringHolder& edgeGroupName,
        const bool useSnapDist = true,
        const fpreal snapDist = 0.001,
        const bool useNewGroupName = false,
        const UT_StringHolder& newGroupName = "",
        const bool reverseGroup = false,
        const bool outAsVertexGroup = false,
        const exint subscribeRatio = 64,
        const exint minGrainSize = 64
    )
{
    UT_ASSERT_P(geo);
    UT_ASSERT_P(geoRef);

    if (!edgeGroupName.isstring() || edgeGroupName.length() == 0)
        return;

    for (GA_GroupTable::iterator<GA_Group> it = geo->getGroupTable(GA_GROUP_EDGE)->beginTraverse(); !it.atEnd(); ++it)
    {
        GA_EdgeGroup* const group = UTverify_cast<GA_EdgeGroup*>(it.group());
        //if (group->isDetached())
        //    continue;
        if (!group->getName().multiMatch(edgeGroupName))
            continue;

        edgeGroupTransfer(geo, geoRef,
            geoVtxGroup, group,
            useSnapDist, snapDist,
            useNewGroupName, newGroupName,
            reverseGroup, outAsVertexGroup,
            subscribeRatio, minGrainSize);
    }

}

SYS_FORCE_INLINE
static void
edgeGroupTransfer(
    GA_Detail* const geo,
    const GA_Detail* const geoRef,
    const GA_Group* const geoGroup,

    const UT_StringHolder& edgeGroupName,
    const bool useSnapDist = true,
    const fpreal snapDist = 0.001,
    const bool useNewGroupName = false,
    const UT_StringHolder& newGroupName = "",
    const bool reverseGroup = false,
    const bool outAsVertexGroup = false,
    const exint subscribeRatio = 64,
    const exint minGrainSize = 64
)
{
    UT_ASSERT_P(geo);
    UT_ASSERT_P(geoRef);
    
    const GA_VertexGroup* const geoVtxGroup = GA_FeE_GroupPromote::groupFindPromoteVertexDetached(geo, geoGroup);

    edgeGroupTransfer(geo, geoRef,
        geoVtxGroup, edgeGroupName,
        useSnapDist, snapDist,
        useNewGroupName, newGroupName,
        reverseGroup, outAsVertexGroup,
        subscribeRatio, minGrainSize);

    if (geoVtxGroup->isDetached())
        delete geoVtxGroup;
}


SYS_FORCE_INLINE
static void
edgeGroupTransfer(
    const SOP_NodeVerb::CookParms& cookparms,
    GA_Detail* const geo,
    const GA_Detail* const geoRef,
    const GA_GroupType groupType,
    const UT_StringHolder& groupName,

    const UT_StringHolder& edgeGroupName,
    const bool useSnapDist = true,
    const fpreal snapDist = 0.001,
    const bool useNewGroupName = false,
    const UT_StringHolder& newGroupName = "",
    const bool reverseGroup = false,
    const bool outAsVertexGroup = false,
    const exint subscribeRatio = 64,
    const exint minGrainSize = 64
    )
{
    UT_ASSERT_P(geo);
    UT_ASSERT_P(geoRef);

    if (!edgeGroupName.isstring() || edgeGroupName.length() == 0)
        return;

    GOP_Manager gop;
    const GA_Group* const geoGroup = GA_FeE_Group::findOrParseGroupDetached(cookparms, geo, groupType, groupName, gop);
    if (geoGroup && geoGroup->isEmpty())
        return;

    edgeGroupTransfer(geo, geoRef,
        geoGroup, edgeGroupName,
        useSnapDist, snapDist,
        useNewGroupName, newGroupName,
        reverseGroup, outAsVertexGroup,
        subscribeRatio, minGrainSize);

    GA_FeE_Group::groupBumpDataId(geo, GA_GROUP_EDGE, edgeGroupName);
}




} // End of namespace GA_FeE_EdgeGroupTransfer

#endif
