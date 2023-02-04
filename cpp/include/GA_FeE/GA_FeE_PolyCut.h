
#pragma once

#ifndef __GA_FeE_PolyCut_h__
#define __GA_FeE_PolyCut_h__



//#include "GA_FeE/GA_FeE_PolyCut.h"

#include "GA/GA_Detail.h"
//#include "GA/GA_Types.h"

#include "GA_FeE/GA_FeE_Type.h"
#include "GA_FeE/GA_FeE_TopologyReference.h"

#include "GEO/GEO_PrimPoly.h"


enum GA_FeE_PolyCutType
{
    GA_FeE_PolyCutType_AUTO,
    GA_FeE_PolyCutType_OPEN,
    GA_FeE_PolyCutType_CLOSE,
};

namespace GA_FeE_PolyCut {




#define TMP_CutPointGroup_GA_FeE_PolyCut "__tmp_cutPointGroup_GA_FeE_PolyCut"


SYS_FORCE_INLINE
static void
appendPoint_polyCut(
    GA_Detail* const geo,
    GA_Offset& primpoint,
    GA_PointGroup* const cutPointGroup
) {
    GA_Offset new_primpoint = geo->appendPoint();
    geo->copyPoint(new_primpoint, primpoint);
    //bool a = cutPointGroup->isOrdered();
    cutPointGroup->setElement(new_primpoint, false);
    primpoint = new_primpoint;
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






// can not use in parallel unless for each GA_Detail
static void
polyCut(
    GA_Detail* const geo,
    const GA_Detail* const srcGeo,
    GA_PointGroup* const cutPointGroup,
    const GA_PrimitiveGroup* const primGroup = nullptr,
    const bool cutPoint = false,
    const bool mergePrimEndsIfClosed = true,
    const GA_FeE_PolyCutType polyType = GA_FeE_PolyCutType_AUTO
)
{
    UT_ASSERT_P(geo);
    UT_ASSERT_P(srcGeo);
    UT_ASSERT_P(geo->getNumPoints() == srcGeo->getNumPoints());

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

    //const GA_PointGroupUPtr tmp_cutPointGroupUPtr = geo->createDetachedPointGroup();
    //GA_PointGroup* const tmp_cutPointGroup = tmp_cutPointGroupUPtr.get();
    GA_PointGroup* tmp_cutPointGroup = nullptr;
    if (cutPoint)
    {
        tmp_cutPointGroup = static_cast<GA_PointGroup*>(geo->pointGroups().newGroup(TMP_CutPointGroup_GA_FeE_PolyCut));
        if (cutPointGroup)
        {
            //tmp_cutPointGroup->combine(cutPointGroup);
            //*tmp_cutPointGroup = *cutPointGroup;
            tmp_cutPointGroup->copyMembership(*cutPointGroup, false);
        }
        else
        {
            tmp_cutPointGroup->setElement(geo->getPointRange(), true);
        }
    }

    GA_Topology& topo = geo->getTopology();
    const GA_Topology& topoRef = srcGeo->getTopology();
    const GA_ATITopology* const vtxPointRef = topoRef.getPointRef();

    bool closeFlag;
    switch (polyType)
    {
    case GA_FeE_PolyCutType_AUTO:
        break;
    case GA_FeE_PolyCutType_OPEN:
        closeFlag = false;
        break;
    case GA_FeE_PolyCutType_CLOSE:
        closeFlag = true;
        break;
    default:
        closeFlag = false;
        break;
    }
    GEO_PrimPoly* newPrim = nullptr;
    GA_Offset start, end;
    for (GA_Iterator it(srcGeo->getPrimitiveRange(primGroup)); it.fullBlockAdvance(start, end); )
    {
        for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
        {
            if (polyType == GA_FeE_PolyCutType_AUTO)
            {
                closeFlag = srcGeo->getPrimitiveClosedFlag(elemoff);
            }
            GA_OffsetListRef vertices = srcGeo->getPrimitiveVertexList(elemoff);
            //const GA_Size numvtx = srcGeo->getPrimitiveVertexCount(elemoff) - 1;
            const GA_Size numvtx = vertices.size() - 1;
            if (numvtx == -1) continue;

            //const GEO_Primitive* const srcPrim = static_cast<const GEO_Primitive*>(srcGeo->getPrimitive(elemoff));

            GA_Offset primpoint = vtxPointRef->getLink(vertices[0]);


            if (mergePrimEndsIfClosed && (cutPointGroup && !cutPointGroup->contains(primpoint)) && (srcGeo->getPrimitiveClosedFlag(elemoff) || primpoint == vtxPointRef->getLink(vertices[numvtx])))
            {
                GA_Size firstPrimIndex = 0;
                for (GA_Size vtxpnum = 1; vtxpnum <= numvtx; ++vtxpnum) {
                    primpoint = vtxPointRef->getLink(vertices[vtxpnum]);
                    if (cutPointGroup && !cutPointGroup->contains(primpoint)) continue;
                    firstPrimIndex = vtxpnum;
                    break;
                }

                if (firstPrimIndex == 0) {
                    primpoint = vtxPointRef->getLink(vertices[0]);
                }

                if (cutPoint)
                {
                    appendPoint_polyCut(geo, primpoint, tmp_cutPointGroup);
                }
                firstPrimIndex;
                primpoint;

                newPrim = appendPrimitive_polyCut(geo, srcGeo, closeFlag, primpoint, elemoff);

                if (cutPoint)
                {
                    for (GA_Size vtxpnum = firstPrimIndex + 1; vtxpnum <= numvtx; ++vtxpnum) {
                        primpoint = vtxPointRef->getLink(vertices[vtxpnum]);
                        if (!cutPointGroup || cutPointGroup->contains(primpoint))
                        {
                            appendPoint_polyCut(geo, primpoint, tmp_cutPointGroup);
                            newPrim->appendVertex(primpoint);
                            appendPoint_polyCut(geo, primpoint, tmp_cutPointGroup);
                            newPrim = appendPrimitive_polyCut(geo, srcGeo, closeFlag, primpoint, elemoff);
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
                        if (cutPointGroup && !cutPointGroup->contains(primpoint)) continue;

                        newPrim = appendPrimitive_polyCut(geo, srcGeo, closeFlag, primpoint, elemoff);
                    }
                }

                for (GA_Size vtxpnum = !srcGeo->getPrimitiveClosedFlag(elemoff); vtxpnum <= firstPrimIndex; ++vtxpnum) {
                    primpoint = vtxPointRef->getLink(vertices[vtxpnum]);
                    newPrim->appendVertex(primpoint);
                }

                if (cutPoint)
                {
                    primpoint = vtxPointRef->getLink(vertices[firstPrimIndex]);
                    appendPoint_polyCut(geo, primpoint, tmp_cutPointGroup);
                    newPrim->appendVertex(primpoint);
                }
            }
            else
            {
                if (cutPoint)
                {
                    newPrim = static_cast<GEO_PrimPoly*>(geo->appendPrimitive(GA_PrimitiveTypeId(1)));
                    geo->copyAttributes(GA_ATTRIB_PRIMITIVE, newPrim->getMapOffset(), *srcGeo, elemoff);
                    geo->setPrimitiveClosedFlag(newPrim->getMapOffset(), closeFlag);
                }
                else
                {
                    newPrim = appendPrimitive_polyCut(geo, srcGeo, closeFlag, primpoint, elemoff);
                }

                if (cutPoint)
                {
                    if (!cutPointGroup || cutPointGroup->contains(primpoint))
                    {
                        appendPoint_polyCut(geo, primpoint, tmp_cutPointGroup);
                    }
                    newPrim->appendVertex(primpoint);
                    for (GA_Size vtxpnum = 1; vtxpnum < numvtx; ++vtxpnum) {
                        primpoint = vtxPointRef->getLink(vertices[vtxpnum]);

                        if (!cutPointGroup || cutPointGroup->contains(primpoint))
                        {
                            appendPoint_polyCut(geo, primpoint, tmp_cutPointGroup);
                            newPrim->appendVertex(primpoint);
                            appendPoint_polyCut(geo, primpoint, tmp_cutPointGroup);
                            newPrim = appendPrimitive_polyCut(geo, srcGeo, closeFlag, primpoint, elemoff);
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

                        if (cutPointGroup && !cutPointGroup->contains(primpoint)) continue;

                        newPrim = appendPrimitive_polyCut(geo, srcGeo, closeFlag, primpoint, elemoff);
                    }
                }


                primpoint = vtxPointRef->getLink(vertices[numvtx]);
                if (cutPoint && (!cutPointGroup || cutPointGroup->contains(primpoint)))
                {
                    appendPoint_polyCut(geo, primpoint, tmp_cutPointGroup);
                }
                newPrim->appendVertex(primpoint);

                if (srcGeo->getPrimitiveClosedFlag(elemoff) && polyType == 1) {

                    if (cutPoint)
                    {
                        appendPoint_polyCut(geo, primpoint, tmp_cutPointGroup);
                    }

                    newPrim = appendPrimitive_polyCut(geo, srcGeo, closeFlag, primpoint, elemoff);
                    primpoint = vtxPointRef->getLink(vertices[0]);

                    if (cutPoint)
                    {
                        appendPoint_polyCut(geo, primpoint, tmp_cutPointGroup);
                    }

                    newPrim->appendVertex(primpoint);
                }
            }
        }
    }
    if (cutPoint)
    {
        //geo->destroyUnusedPoints(geo->getPointRange(tmp_cutPointGroup));
        geo->destroyPointOffsets(geo->getPointRange(tmp_cutPointGroup), GA_Detail::GA_DestroyPointMode::GA_LEAVE_PRIMITIVES, true);
        geo->destroyGroup(tmp_cutPointGroup);

        //geo->edgeGroups()
    }
}

//
//static void
//polyCut(
//    GA_Detail* const geo,
//    const GA_Detail* const srcGeo,
//    GA_PointGroup* const cutPointGroup,
//    const GA_PrimitiveGroup* const primGroup = nullptr,
//    const bool cutPoint = false,
//    const bool mergePrimEndsIfClosed = true,
//    const GA_FeE_PolyCutType polyType = GA_FeE_PolyCutType_AUTO,
//    const bool delInputPointGroup = false
//)
//{
//    polyCut(geo, srcGeo, cutPointGroup, primGroup, cutPoint, mergePrimEndsIfClosed, polyType);
//
//    if (delInputPointGroup && cutPointGroup && !cutPointGroup->isDetached())
//    {
//        geo->destroyGroup(cutPointGroup);
//    }
//}


// can not use in parallel unless for each GA_Detail
static void
polyCut(
    GA_Detail* const geo,
    const GA_Detail* const srcGeo,
    const GA_PointGroup* const cutPointGroup,
    const GA_PrimitiveGroup* const primGroup = nullptr,
    const bool cutPoint = false,
    const bool mergePrimEndsIfClosed = true,
    const GA_FeE_PolyCutType polyType = GA_FeE_PolyCutType_AUTO,

    const GA_Storage storage = GA_STORE_INVALID,
    //const bool createSrcPrimAttrib = false,
    const UT_StringHolder& srcPrimAttribName = "",
    //const bool createSrcPointAttrib = false,
    const UT_StringHolder& srcPointAttribName = ""

)
{
    //GU_DetailHandle geoTmp0_h;
    //GU_Detail* geoTmp0 = new GU_Detail();
    //geoTmp0_h.allocateAndSet(geoTmp0);
    //geoTmp0->replaceWith(*srcGeo);

    const GA_Storage finalStorageI = storage == GA_STORE_INVALID ? GA_FeE_Type::getPreferredStorageI(geo) : storage;
    if (srcPrimAttribName.length() != 0 && srcPrimAttribName.isstring())
    {
        geo->getAttributes().createTupleAttribute(GA_ATTRIB_PRIMITIVE, srcPrimAttribName, finalStorageI, 1, GA_Defaults(-1));
    }

    if (srcPointAttribName.length() != 0 && srcPointAttribName.isstring())
    {
        geo->getAttributes().createTupleAttribute(GA_ATTRIB_POINT, srcPointAttribName, finalStorageI, 1, GA_Defaults(-1));
    }

    polyCut(geo, srcGeo, cutPointGroup, primGroup,
        cutPoint, mergePrimEndsIfClosed, polyType);
}



// can not use in parallel unless for each GA_Detail
static void
polyCut(
    const SOP_NodeVerb::CookParms& cookparms,
    GA_Detail* const geo,
    const GA_Detail* const srcGeo,
    const UT_StringHolder& cutPointGroupName = "",
    const UT_StringHolder& primGroupName = "",
    const bool cutPoint = false,
    const bool mergePrimEndsIfClosed = true,
    const GA_FeE_PolyCutType polyType = GA_FeE_PolyCutType_AUTO,

    const GA_Storage storage = GA_STORE_INVALID,
    //const bool createSrcPrimAttrib = false,
    const UT_StringHolder& srcPrimAttribName = "",
    //const bool createSrcPointAttrib = false,
    const UT_StringHolder& srcPointAttribName = "",
    const bool delInputPointGroup = false
)
{
    GOP_Manager gop;
    GA_PointGroup* const cutPointGroup = GA_FeE_Group::findOrParsePointGroupDetached(cookparms, geo, cutPointGroupName, gop);
    if (cutPointGroup && cutPointGroup->isEmpty())
    {
        geo->replaceWith(*srcGeo);
        return;
    }

    const GA_PrimitiveGroup* const primGroup = GA_FeE_Group::findOrParsePrimitiveGroupDetached(cookparms, srcGeo, primGroupName, gop);
    if (primGroup && primGroup->isEmpty())
    {
        geo->replaceWith(*srcGeo);
        return;
    }

    geo->replaceWithPoints(*srcGeo);

    polyCut(geo, srcGeo, cutPointGroup, primGroup,
        cutPoint, mergePrimEndsIfClosed, polyType,
        storage, srcPrimAttribName, srcPointAttribName);

    if (delInputPointGroup && cutPointGroup && !cutPointGroup->isDetached())
    {
        geo->destroyGroup(cutPointGroup);
    }
}






} // End of namespace GA_FeE_PolyCut

#endif
