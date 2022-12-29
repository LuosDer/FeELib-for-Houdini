
#pragma once

#ifndef __GA_FeE_PolyCut_h__
#define __GA_FeE_PolyCut_h__



//#include "GA_FeE/GA_FeE_PolyCut.h"

#include "GA/GA_Detail.h"
//#include "GA/GA_Types.h"

#include "GA_FeE/GA_FeE_Type.h"
#include "GA_FeE/GA_FeE_TopologyReference.h"

#include "GEO/GEO_PrimPoly.h"



namespace GA_FeE_PolyCut {





SYS_FORCE_INLINE
static void
appendPoint_polyCut(
    GA_Detail* const geo,
    GA_Offset& primpoint,
    GA_PointGroup* const cutPointGroup
) {
    GA_Offset new_primpoint = geo->appendPoint();
    geo->copyPoint(new_primpoint, primpoint);
    primpoint = new_primpoint;
    cutPointGroup->setElement(primpoint, false);
}


SYS_FORCE_INLINE
static GEO_PrimPoly*
appendPrimitive_polyCut(
    GA_Detail* const geoPoint,
    const GA_Detail* geo,
    const bool closeFlag,
    const GA_Offset primpoint,
    const GA_Offset elemoff
) {
    GEO_PrimPoly* newPrim = static_cast<GEO_PrimPoly*>(geoPoint->appendPrimitive(GA_PrimitiveTypeId(1)));
    geoPoint->copyAttributes(GA_ATTRIB_PRIMITIVE, newPrim->getMapOffset(), *geo, elemoff);
    geoPoint->setPrimitiveClosedFlag(newPrim->getMapOffset(), closeFlag);

    newPrim->appendVertex(primpoint);
    return newPrim;
}


//SYS_FORCE_INLINE
//static void
//appendVertex_polyCut(
//    GA_Offset& primpoint,
//    GEO_PrimPoly* newPrim,
//    const GA_ATITopology* const vtxPointRef,
//    GA_OffsetListRef& vertices,
//    GA_Size vtxpnum
//) {
//    primpoint = vtxPointRef->getLink(vertices[vtxpnum]);
//    newPrim->appendVertex(primpoint);
//}


static void
polyCut(
    GA_Detail* const geoPoint,
    const GA_Detail* const geo,
    const GA_PointGroup* const cutPointGroup,
    const bool cutPoint = false,
    const GA_PrimitiveGroup* const primGroup = nullptr,
    const bool mergePrimEndsIfClosed = true,
    const int primType = 0
    //GA_Attribute* const srcPrimsAttrib = nullptr,
    //GA_Attribute* const srcPointsAttrib = nullptr
)
{
    UT_ASSERT_P(geoPoint);
    UT_ASSERT_P(geo);
    UT_ASSERT_P(geoPoint->getNumPoints() == geo->getNumPoints());

    //GA_RWHandleT<GA_Offset> srcPrimsAttrib_h;
    //if (srcPrimsAttrib)
    //{
    //    srcPrimsAttrib_h = srcPrimsAttrib;
    //}
    //GA_RWHandleT<GA_Offset> srcPointsAttrib_h;
    //if (cutPoint && srcPointsAttrib)
    //{
    //    srcPointsAttrib_h = srcPointsAttrib;
    //}

    const GA_PointGroupUPtr tmp_cutPointGroupUPtr = geo->createDetachedPointGroup();
    GA_PointGroup* const tmp_cutPointGroup = tmp_cutPointGroupUPtr.get();
    if (cutPoint)
    {
        //tmp_cutPointGroup->combine(cutPointGroup);
        *tmp_cutPointGroup |= *cutPointGroup;
        
    }

    GA_Topology& topo = geoPoint->getTopology();
    const GA_Topology& topoRef = geo->getTopology();
    const GA_ATITopology* const vtxPointRef = topoRef.getPointRef();

    bool closeFlag;
    switch (primType)
    {
    case 0:
        break;
    case 1:
        closeFlag = false;
        break;
    case 2:
        closeFlag = true;
        break;
    default:
        closeFlag = false;
        break;
    }
    GEO_PrimPoly* newPrim = nullptr;
    GA_Offset start, end;
    for (GA_Iterator it(geo->getPrimitiveRange(primGroup)); it.fullBlockAdvance(start, end); )
    {
        for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
        {
            if (primType == 0)
            {
                closeFlag = geo->getPrimitiveClosedFlag(elemoff);
            }
            GA_OffsetListRef vertices = geo->getPrimitiveVertexList(elemoff);
            //const GA_Size numvtx = geo->getPrimitiveVertexCount(elemoff) - 1;
            const GA_Size numvtx = vertices.size() - 1;
            if (numvtx == -1) continue;

            //const GEO_Primitive* const srcPrim = static_cast<const GEO_Primitive*>(geo->getPrimitive(elemoff));

            GA_Offset primpoint = vtxPointRef->getLink(vertices[0]);


            if (mergePrimEndsIfClosed && !cutPointGroup->contains(primpoint) && (geo->getPrimitiveClosedFlag(elemoff) || primpoint == vtxPointRef->getLink(vertices[numvtx])))
            {
                GA_Size firstPrimIndex = 0;
                for (GA_Size vtxpnum = 1; vtxpnum <= numvtx; ++vtxpnum) {
                    primpoint = vtxPointRef->getLink(vertices[vtxpnum]);
                    if (!cutPointGroup->contains(primpoint)) continue;
                    firstPrimIndex = vtxpnum;
                    break;
                }

                if (firstPrimIndex == 0) {
                    primpoint = vtxPointRef->getLink(vertices[0]);
                }

                if (cutPoint)
                {
                    appendPoint_polyCut(geoPoint, primpoint, tmp_cutPointGroup);
                }

                newPrim = appendPrimitive_polyCut(geoPoint, geo, closeFlag, primpoint, elemoff);

                if (cutPoint)
                {
                    for (GA_Size vtxpnum = firstPrimIndex + 1; vtxpnum <= numvtx; ++vtxpnum) {
                        primpoint = vtxPointRef->getLink(vertices[vtxpnum]);
                        if (cutPointGroup->contains(primpoint))
                        {
                            appendPoint_polyCut(geoPoint, primpoint, tmp_cutPointGroup);
                            newPrim->appendVertex(primpoint);
                            appendPoint_polyCut(geoPoint, primpoint, tmp_cutPointGroup);
                            newPrim = appendPrimitive_polyCut(geoPoint, geo, closeFlag, primpoint, elemoff);
                        }
                        else
                        {
                            newPrim->appendVertex(primpoint);
                        }
                    }
                }
                else
                {
                    for (GA_Size vtxpnum = firstPrimIndex + 1; vtxpnum <= numvtx; ++vtxpnum) {
                        primpoint = vtxPointRef->getLink(vertices[vtxpnum]);
                        newPrim->appendVertex(primpoint);
                        if (!cutPointGroup->contains(primpoint)) continue;

                        newPrim = appendPrimitive_polyCut(geoPoint, geo, closeFlag, primpoint, elemoff);
                    }
                }

                for (GA_Size vtxpnum = !geo->getPrimitiveClosedFlag(elemoff); vtxpnum <= firstPrimIndex; ++vtxpnum) {
                    primpoint = vtxPointRef->getLink(vertices[vtxpnum]);
                    newPrim->appendVertex(primpoint);
                }

                if (cutPoint)
                {
                    primpoint = vtxPointRef->getLink(vertices[firstPrimIndex]);
                    appendPoint_polyCut(geoPoint, primpoint, tmp_cutPointGroup);
                    newPrim->appendVertex(primpoint);
                }
            }
            else
            {
                newPrim = appendPrimitive_polyCut(geoPoint, geo, closeFlag, primpoint, elemoff);

                if (cutPoint)
                {
                    if (cutPointGroup->contains(primpoint))
                    {
                        appendPoint_polyCut(geoPoint, primpoint, tmp_cutPointGroup);
                    }
                    for (GA_Size vtxpnum = 1; vtxpnum < numvtx; ++vtxpnum) {
                        primpoint = vtxPointRef->getLink(vertices[vtxpnum]);

                        if (cutPointGroup->contains(primpoint))
                        {
                            appendPoint_polyCut(geoPoint, primpoint, tmp_cutPointGroup);
                            newPrim->appendVertex(primpoint);
                            appendPoint_polyCut(geoPoint, primpoint, tmp_cutPointGroup);
                            newPrim = appendPrimitive_polyCut(geoPoint, geo, closeFlag, primpoint, elemoff);
                        }
                        else
                        {
                            newPrim->appendVertex(primpoint);
                        }
                    }
                }
                else
                {
                    for (GA_Size vtxpnum = 1; vtxpnum < numvtx; ++vtxpnum) {
                        primpoint = vtxPointRef->getLink(vertices[vtxpnum]);
                        newPrim->appendVertex(primpoint);

                        if (!cutPointGroup->contains(primpoint)) continue;

                        newPrim = appendPrimitive_polyCut(geoPoint, geo, closeFlag, primpoint, elemoff);
                    }
                }


                primpoint = vtxPointRef->getLink(vertices[numvtx]);
                if (cutPoint && cutPointGroup->contains(primpoint))
                {
                    appendPoint_polyCut(geoPoint, primpoint, tmp_cutPointGroup);
                }
                newPrim->appendVertex(primpoint);

                if (geo->getPrimitiveClosedFlag(elemoff) && primType == 1) {

                    if (cutPoint)
                    {
                        appendPoint_polyCut(geoPoint, primpoint, tmp_cutPointGroup);
                    }

                    newPrim = appendPrimitive_polyCut(geoPoint, geo, closeFlag, primpoint, elemoff);
                    primpoint = vtxPointRef->getLink(vertices[0]);

                    if (cutPoint)
                    {
                        appendPoint_polyCut(geoPoint, primpoint, tmp_cutPointGroup);
                    }

                    newPrim->appendVertex(primpoint);
                }
            }
        }
    }
    if (cutPoint)
    {
        //geoPoint->destroyUnusedPoints(geoPoint->getPointRange(tmp_cutPointGroup));
        //geoPoint->destroyPointOffsets(geoPoint->getPointRange(tmp_cutPointGroup), GA_Detail::GA_DestroyPointMode::GA_LEAVE_PRIMITIVES, true);
    }
}


} // End of namespace GA_FeE_PolyCut

#endif
