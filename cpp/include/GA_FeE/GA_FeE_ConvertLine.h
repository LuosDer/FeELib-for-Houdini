
#pragma once

#ifndef __GA_FeE_ConvertLine_h__
#define __GA_FeE_ConvertLine_h__

//#include "GA_FeE/GA_FeE_ConvertLine.h"

#include "GA/GA_Detail.h"


#include "GA_FeE/GA_FeE_Attribute.h"
#include "GA_FeE/GA_FeE_VertexNextEquiv.h"

namespace GA_FeE_ConvertLine {


#define TMP_GA_FeE_ConvertLine_UseDetached_SrcPrimAttrib 1

#if !TMP_GA_FeE_ConvertLine_UseDetached_SrcPrimAttrib
#define TMP_GA_FeE_ConvertLine_SrcPrimAttribName "GA_FeE_ConvertLine_srcPrim"
#endif


static GA_Offset
convertLine(
    GA_Detail* const geoPoint,
    const GA_Detail* const geoFull,
    const bool isClosed = false,
    const bool copyPrimAttrib = false,
    bool outSrcPrimAttrib = false,
    const UT_StringHolder& srcPrimAttribName = "",
    const bool keepSourcePrim = false,
    const UT_StringHolder& primGroupName = "",
    const UT_StringHolder& pointGroupName = "",
    const UT_StringHolder& vertexGroupName = "",
    const UT_StringHolder& edgeGroupName = "",
    const GA_Storage storage = GA_STORE_INVALID
)
{
    UT_ASSERT_P(geoPoint);
    UT_ASSERT_P(geoFull);

    if (keepSourcePrim)
    {
        geoPoint->replaceWith(*geoFull);
    }
    else
    {
        geoPoint->replaceWithPoints(*geoFull);
    }

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
    //geoPoint->appendPrimitivesAndVertices(GA_PrimitiveTypeId(1), entries, 2, vtxoff_first, isClosed);



#if TMP_GA_FeE_ConvertLine_UseDetached_SrcPrimAttrib
    GA_AttributeUPtr srcPrimAttribUPtr;
#endif
    GA_Attribute* srcPrimAttrib = nullptr;
    if (copyPrimAttrib || outSrcPrimAttrib)
    {
        if (outSrcPrimAttrib)
        {
            if (srcPrimAttribName.isstring() && !srcPrimAttribName.isEmpty())
            {
                srcPrimAttrib = geoPoint->getAttributes().createTupleAttribute(GA_ATTRIB_PRIMITIVE, srcPrimAttribName, finalStorage, 1, GA_Defaults(-1));
            }
            else
            {
                if (copyPrimAttrib)
                {
#if TMP_GA_FeE_ConvertLine_UseDetached_SrcPrimAttrib
                    srcPrimAttribUPtr = geoPoint->getAttributes().createDetachedTupleAttribute(GA_ATTRIB_PRIMITIVE, finalStorage, 1, GA_Defaults(0));
                    srcPrimAttrib = srcPrimAttribUPtr.get();
#else
                    srcPrimAttrib = geoPoint->getAttributes().createTupleAttribute(GA_ATTRIB_PRIMITIVE, TMP_GA_FeE_ConvertLine_SrcPrimAttribName, finalStorage, 1, GA_Defaults(-1));
                    //srcPrimAttribUPtr.reset(srcPrimAttrib);
#endif
                }
                outSrcPrimAttrib = false;
                UT_ASSERT_MSG(0, "srcPrimAttribName invliad");
            }
        }
        else
        {
#if TMP_GA_FeE_ConvertLine_UseDetached_SrcPrimAttrib
            srcPrimAttribUPtr = geoPoint->getAttributes().createDetachedTupleAttribute(GA_ATTRIB_PRIMITIVE, finalStorage, 1, GA_Defaults(0));
            srcPrimAttrib = srcPrimAttribUPtr.get();
#else
            srcPrimAttrib = geoPoint->getAttributes().createTupleAttribute(GA_ATTRIB_PRIMITIVE, TMP_GA_FeE_ConvertLine_SrcPrimAttribName, finalStorage, 1, GA_Defaults(-1));
            //srcPrimAttribUPtr.reset(srcPrimAttrib);
#endif
        }
    }
    //if (srcPrimAttrib)
    //{
    //    exint indexSize = srcPrimAttrib->getIndexMap().indexSize();
    //    exint offsetSize = srcPrimAttrib->getIndexMap().offsetSize();
    //}
    GA_RWHandleT<GA_Offset> srcPrim_h(srcPrimAttrib);



    GA_Topology& topo = geoPoint->getTopology();
    const GA_ATITopology* const vtxPrimRef = topo.getPrimitiveRef();


    GA_Topology& topo_tmpGeo0 = geoTmp->getTopology();
    topo_tmpGeo0.makePrimitiveRef();
    const GA_ATITopology* const vtxPointRef_geoTmp = topo_tmpGeo0.getPointRef();
    const GA_ATITopology* const vtxPrimRef_geoTmp = topo_tmpGeo0.getPrimitiveRef();

    GA_Offset start, end;
    for (GA_Iterator it(geoTmp->getVertexRange(creatingGroup)); it.fullBlockAdvance(start, end); )
    {
        for (GA_Offset vtxoff = start; vtxoff < end; ++vtxoff)
        {
            if (copyPrimAttrib || outSrcPrimAttrib)
            {
                srcPrim_h.set(vtxPrimRef->getLink(vtxoff_first), vtxPrimRef_geoTmp->getLink(vtxoff));
            }
            topo.wireVertexPoint(vtxoff_first, vtxPointRef_geoTmp->getLink(vtxoff));
            ++vtxoff_first;
            topo.wireVertexPoint(vtxoff_first, dstptAttrib_h.get(vtxoff));
            ++vtxoff_first;
        }
    }

    //////////////////// copy attributes //////////////////////

    if (copyPrimAttrib)
    {
#if 1
        GA_Offset primoff_last = primoff_first + entries;

        for (GA_AttributeDict::iterator it = geoPoint->getAttributes().begin(GA_ATTRIB_PRIMITIVE); !it.atEnd(); ++it)
        {
            GA_Attribute* const attribPtr = it.attrib();
            if (attribPtr == srcPrimAttrib)
                continue;
            //if (attribPtr->isDetached())
            //    continue;
            if (keepSourcePrim)
            {
                for (GA_Offset primoff = primoff_first; primoff < primoff_last; ++primoff)
                {
                    attribPtr->copy(primoff, srcPrim_h.get(primoff));
                }
            }
            else
            {
                GA_Attribute* const srcAttribPtr = geoTmp->findPrimitiveAttribute(it.name());
                for (GA_Offset primoff = primoff_first; primoff < primoff_last; ++primoff)
                {
                    attribPtr->copy(primoff, *srcAttribPtr, srcPrim_h.get(primoff));
                }
            }
        }
#else
        GA_Offset start, end;
        for (GA_Iterator it(GA_Range(geoPoint->getPrimitiveMap(), primoff_first, geoPoint->getNumPrimitiveOffsets())); it.fullBlockAdvance(start, end); )
        {
            for (GA_Offset primoff = start; primoff < end; ++primoff)
            {
            }
        }
#endif

#if !TMP_GA_FeE_ConvertLine_UseDetached_SrcPrimAttrib
        if (!outSrcPrimAttrib)
        {
            geoPoint->getAttributes().destroyAttribute(srcPrimAttrib);
        }
#endif
    }



    //geoTmp_h.deleteGdp();

    return primoff_first;
}



SYS_FORCE_INLINE
static GA_Offset
convertLine(
    GA_Detail* const geoFull,
    const bool isClosed = false,
    const bool copyPrimAttrib = false,
    const bool outSrcPrimAttrib = false,
    const UT_StringHolder& srcPrimAttribName = "",
    const bool keepSourcePrim = false,
    const UT_StringHolder& primGroupName = "",
    const UT_StringHolder& pointGroupName = "",
    const UT_StringHolder& vertexGroupName = "",
    const UT_StringHolder& edgeGroupName = "",
    const GA_Storage storage = GA_STORE_INVALID
)
{
    UT_ASSERT_P(geoFull);

    GU_DetailHandle geoFullTmp_h;
    GU_Detail* const geoFullTmp = new GU_Detail();
    geoFullTmp_h.allocateAndSet(geoFullTmp);
    geoFullTmp->replaceWith(*geoFull);

    geoFull->replaceWithPoints(*geoFullTmp);

    return convertLine(geoFull, geoFullTmp, isClosed, copyPrimAttrib, outSrcPrimAttrib, srcPrimAttribName, keepSourcePrim, primGroupName, pointGroupName, vertexGroupName, edgeGroupName, GA_STORE_INVALID);
}




SYS_FORCE_INLINE
static GU_DetailHandle
convertLine(
    const GA_Detail* const geoFull,
    const bool isClosed = false,
    const bool copyPrimAttrib = false,
    const bool outSrcPrimAttrib = false,
    const UT_StringHolder& srcPrimAttribName = "",
    const bool keepSourcePrim = false,
    const UT_StringHolder& primGroupName = "",
    const UT_StringHolder& pointGroupName = "",
    const UT_StringHolder& vertexGroupName = "",
    const UT_StringHolder& edgeGroupName = "",
    const GA_Storage storage = GA_STORE_INVALID
)
{
    UT_ASSERT_P(geoFull);

    GU_DetailHandle geoOut_h;
    GU_Detail* const geoOut = new GU_Detail();
    geoOut_h.allocateAndSet(geoOut);

    convertLine(geoOut, geoFull, isClosed, copyPrimAttrib, outSrcPrimAttrib, srcPrimAttribName, keepSourcePrim, primGroupName, pointGroupName, vertexGroupName, edgeGroupName, GA_STORE_INVALID);
    return geoOut_h;
}



} // End of namespace GA_FeE_ConvertLine

#endif
