
#pragma once

#ifndef __GA_FeE_ConvertLine_h__
#define __GA_FeE_ConvertLine_h__

//#include "GA_FeE/GA_FeE_ConvertLine.h"

#include "GA/GA_Detail.h"


#include "GA_FeE/GA_FeE_Attribute.h"
#include "GA_FeE/GA_FeE_VertexNextEquiv.h"

namespace GA_FeE_ConvertLine
{



static void
convertLine(
    GA_Detail* const geoPoint,
    const GA_Detail* const geoFull,
    const UT_StringHolder& primGroupName = "",
    const UT_StringHolder& pointGroupName = "",
    const UT_StringHolder& vertexGroupName = "",
    const UT_StringHolder& edgeGroupName = "",
    const GA_Storage storage = GA_STORE_INVALID,
    const bool isClosed = false
)
{
    geoPoint->replaceWithPoints(*geoFull);

    GU_DetailHandle geoTmp_h;
    GU_Detail* const geoTmp = new GU_Detail();
    geoTmp_h.allocateAndSet(geoTmp);
    geoTmp->replaceWith(*geoFull);


    const bool hasInputGroup = primGroupName.isstring() || pointGroupName.isstring() || vertexGroupName.isstring() || edgeGroupName.isstring();

    GA_VertexGroupUPtr geo0vtxGroupUPtr;
    GA_VertexGroup* geoVtxGroup = nullptr;
    if (hasInputGroup)
    {
        geo0vtxGroupUPtr = geoTmp->createDetachedVertexGroup();
        geoVtxGroup = geo0vtxGroupUPtr.get();
        if (primGroupName.isstring())
        {

        }

        if (pointGroupName.isstring())
        {

        }

        if (vertexGroupName.isstring())
        {

        }

        if (edgeGroupName.isstring())
        {

        }
    }

    const GA_Storage finalStorage = storage == GA_STORE_INVALID ? GA_FeE_Type::getPreferredStorageI(geoPoint) : storage;
    //const GA_VertexGroup* creatingGroup = GA_FeE_VertexNextEquiv::addGroupVertexNextEquivNoLoop(geoTmp, "__topo_nextEquivValid", geoVtxGroup, subscribeRatio, minGrainSize);
    const GA_VertexGroup* const creatingGroup = GA_FeE_VertexNextEquiv::addGroupVertexNextEquivNoLoop(geoTmp, geoVtxGroup, finalStorage);
    //const GA_Attribute* dstptAttrib = geoPoint->findVertexAttribute("__topo_dstpt");
    const GA_RWHandleT<GA_Offset> dstptAttrib_h = geoTmp->findVertexAttribute(GA_FEE_TOPO_SCOPE, "__topo_dstpt");

    UT_ASSERT_P(dstptAttrib_h.getAttribute());


    GA_Size entries = creatingGroup->getGroupEntries();

    GA_Offset vtxoff_first;
    GA_Offset primoff_first = geoPoint->appendPrimitivesAndVertices(GA_PrimitiveTypeId(1), entries, 2, vtxoff_first, isClosed);


    GA_Topology& topo = geoPoint->getTopology();

    GA_Topology& topo_tmpGeo0 = geoTmp->getTopology();
    const GA_ATITopology* vtxPointRef_tmpGeo0 = topo_tmpGeo0.getPointRef();

    GA_Offset start, end;
    for (GA_Iterator it(geoTmp->getVertexRange(creatingGroup)); it.fullBlockAdvance(start, end); )
    {
        for (GA_Offset vtxoff = start; vtxoff < end; ++vtxoff)
        {
            topo.wireVertexPoint(vtxoff_first, vtxPointRef_tmpGeo0->getLink(vtxoff));
            ++vtxoff_first;
            topo.wireVertexPoint(vtxoff_first, dstptAttrib_h.get(vtxoff));
            ++vtxoff_first;
        }
    }

    geoTmp_h.deleteGdp();
}




SYS_FORCE_INLINE
static GU_DetailHandle
convertLine(
    const GA_Detail* const geoFull,
    const UT_StringHolder& primGroupName = "",
    const UT_StringHolder& pointGroupName = "",
    const UT_StringHolder& vertexGroupName = "",
    const UT_StringHolder& edgeGroupName = "",
    const GA_Storage storage = GA_STORE_INVALID,
    const bool isClosed = false
)
{
    GU_DetailHandle geoOut_h;
    GU_Detail* const geoOut = new GU_Detail();
    geoOut_h.allocateAndSet(geoOut);

    convertLine(geoOut, geoFull, primGroupName, pointGroupName, vertexGroupName, edgeGroupName, GA_STORE_INVALID, isClosed);
    return geoOut_h;
}



} // End of namespace GA_FeE_ConvertLine

#endif
