
#pragma once

#ifndef __GFE_JoinCurve_h__
#define __GFE_JoinCurve_h__


//#include "GFE/GFE_JoinCurve.h"

#include "GA/GA_Detail.h"
//#include "GA/GA_Types.h"

#include "GFE_VertexNextEquiv.h"
#include "GFE/GFE_Type.h"
#include "GFE/GFE_TopologyReference.h"
#include "GFE/GFE_GroupParser.h"

#include "GEO/GEO_PrimPoly.h"



class GFE_JoinCurve : public GFE_AttribFilter {

public:
    using GFE_AttribFilter::GFE_AttribFilter;

    //GFE_JoinCurve(
    //    GA_Detail* const geo,
    //    const SOP_NodeVerb::CookParms* const cookparms = nullptr
    //)
    //    : GFE_AttribCreateFilter(geo, cookparms)
    //    , groupParserSeam(geo, groupParser.getGOP(), cookparms)
    //{
    //}

    //GFE_JoinCurve(
    //    const SOP_NodeVerb::CookParms& cookparms,
    //    GA_Detail* const geo
    //)
    //    : GFE_AttribCreateFilter(cookparms, geo)
    //    , groupParserSeam(cookparms, geo, groupParser.getGOP())
    //{
    //}

    void
        setComputeParm(
            const bool groupUnsharedAfterFuse = false,
            const bool outTopoAttrib = false,
            const fpreal fuseDist = 1e-05,
            const exint subscribeRatio = 64,
            const exint minGrainSize = 1024
        )
    {
        setHasComputed();

        this->groupUnsharedAfterFuse = groupUnsharedAfterFuse;
        this->fuseDist = fuseDist;
        this->subscribeRatio = subscribeRatio;
        this->minGrainSize = minGrainSize;

        setOutTopoAttrib(outTopoAttrib);
    }

    void
        findOrCreate(
            const GA_GroupType unsharedAttribClass = GA_GROUP_VERTEX,
            const GA_StorageClass storageClass = GA_STORECLASS_INT,
            const GA_Storage storage = GA_STORE_INVALID,
            const bool detached = false,
            const UT_StringHolder& name = "__topo_connectivity"
        )
    {
        if (storageClass == GA_STORECLASS_OTHER)
        {
            isGroup = true;
            getOutGroupArray().findOrCreate(unsharedAttribClass, detached, name);
            //if (unsharedAttribClass != GA_GROUP_VERTEX)
            //{
            //    getOutGroupArray().findOrCreate(GA_GROUP_VERTEX, true);
            //}
        }
        else
        {
            isGroup = false;
            getOutAttribArray().findOrCreateTuple(GFE_Type::attributeOwner_groupType(unsharedAttribClass),
                storageClass, storage, detached, name);

            //getOutGroupArray().findOrCreate(GA_GROUP_VERTEX, true);
        }
        //unsharedGroup = static_cast<GA_VertexGroup*>(getOutGroupArray().last());
    }








private:


    virtual bool
        computeCore() override
    {
        if (groupParser.isEmpty())
            return true;

        GU_DetailHandle geoOrigin_h;
        GU_Detail* geoOriginTmp = nullptr;
        if (groupUnsharedAfterFuse)
        {
            geoOriginTmp = new GU_Detail();
            geoOrigin_h.allocateAndSet(geoOriginTmp);
            geoOriginTmp->replaceWith(*geo);

            GU_Snap::PointSnapParms fuseParms;
            fuseParms.myDistance = fuseDist;
            fuseParms.myAlgorithm = GU_Snap::PointSnapParms::SnapAlgorithm::ALGORITHM_CLOSEST_POINT;
            fuseParms.myConsolidate = true;
            fuseParms.myDeleteConsolidated = true;
            fuseParms.myQPosH = geoOriginTmp->getP();
            fuseParms.myTPosH = geoOriginTmp->getP();
            GU_Snap::snapPoints(*geoOriginTmp, nullptr, fuseParms);
            //GU_Snap::snapPoints(*geoOriginTmp, static_cast<const GU_Detail*>(geo), fuseParms);
        }
        else
        {
            geoOriginTmp = static_cast<GU_Detail*>(geo);
        }

        const GA_Storage finalStorageI = GFE_Type::getPreferredStorageI(geo);

        GA_VertexGroup* unsharedGroup = GFE_VertexNextEquiv_Namespace::addGroupVertexNextEquiv(geoOriginTmp, groupParser.getVertexGroup(), GA_STORE_INVALID);

        if (isGroup)
        {
            //GFE_GroupPromote::groupPromote(geoOriginTmp, unsharedGroup, unsharedAttribClass, unsharedAttribName, true);
            GFE_GroupUnion::groupUnion(getOutGroupArray()[0], unsharedGroup);
            GFE_GroupUnion::groupUnion_topoAttrib(geo, getOutGroupArray()[0], unsharedGroup);
        }
        else
        {
            GFE_AttribCombine::attribCombine(getOutAttribArray()[0], unsharedGroup);
        }

        return true;
    }


#ifndef GFE_MAXLOOPCOUNT
#define GFE_MAXLOOPCOUNT 1000000000
#endif

static void
    joinCurve(
        GA_Detail* const geo,
        const bool keepOrder = false,
        const bool keepLoop = true,
        const bool closeLoop = false,
        const GA_PointGroup* const stopPointGroup = nullptr,
        GA_Attribute* const srcPrimsAttrib = nullptr
    )
{
    UT_ASSERT_P(geo);

    UT_ValArray<GA_Offset> srcPrims;
    GA_RWHandleT<UT_ValArray<GA_Offset>> srcPrims_h;
    if (srcPrimsAttrib)
    {
        srcPrims.setCapacity(1024);
        srcPrims_h = srcPrimsAttrib;
    }

    //GA_Offset vtxoff_new = geo->appendVertex();
    //geo->getGEOPrimitive(0).wireVertexPrimitive(vtxoff_new, 0);
    //geo->getTopology().wireVertexPrimitive(vtxoff_new, 0);
    //geo->getTopology().wireVertexPoint(vtxoff_new, 2);
    //return;

    const GA_Range& primRange = geo->getPrimitiveRange();

    //const GA_PointGroupUPtr passedPointGroupUPtr = geo->createDetachedPointGroup();
    //GA_PointGroup* passedPointGroup = passedPointGroupUPtr.get();

    const GA_PrimitiveGroupUPtr unPassedPrimitiveGroupUPtr = geo->createDetachedPrimitiveGroup();
    GA_PrimitiveGroup* const unPassedPrimitiveGroup = unPassedPrimitiveGroupUPtr.get();
    unPassedPrimitiveGroup->setElement(primRange, true);

    //GA_PrimitiveGroupUPtr unPassedPrimitiveGroupUPtr;
    //GA_PrimitiveGroup* unPassedPrimitiveGroup;
    //if (keepLoop)
    //{
    //    unPassedPrimitiveGroupUPtr = geo->createDetachedPrimitiveGroup();
    //    unPassedPrimitiveGroup = unPassedPrimitiveGroupUPtr.get();
    //    unPassedPrimitiveGroup->setElement(primRange, true);
    //}

    const GA_PrimitiveGroupUPtr delPrimitiveGroupUPtr = geo->createDetachedPrimitiveGroup();
    GA_PrimitiveGroup* const delPrimitiveGroup = delPrimitiveGroupUPtr.get();
    delPrimitiveGroup->setElement(primRange, false);


    const GA_Topology& topo = geo->getTopology();
    //topo.makeFull();
    const GA_ATITopology* vtxPointRef = topo.getPointRef();
    const GA_ATITopology* pointVtxRef = topo.getVertexRef();
    const GA_ATITopology* vtxNextRef  = topo.getVertexNextRef();
    const GA_ATITopology* vtxPrimRef  = topo.getPrimitiveRef();

    bool isTwoNeb0, isTwoNeb1;
    GA_Offset vtxoff0, ptoff0, vtxoff_next0;
    GA_Offset vtxoff1, ptoff1, vtxoff_next1;
    GA_Offset primoff_neb, vtxoff_next, ptoff_next, vtxoff_prev;
    GA_Offset start, end;
    for (GA_Iterator it(primRange); it.fullBlockAdvance(start, end); )
    {
        for (GA_Offset primoff = start; primoff < end; ++primoff)
        {
            if (!unPassedPrimitiveGroup->contains(primoff))
                continue;
            vtxoff0 = geo->getPrimitiveVertexOffset(primoff, 0);
            ptoff0  = vtxPointRef->getLink(vtxoff0);
            if (stopPointGroup && stopPointGroup->contains(ptoff0))
            {
                isTwoNeb0 = false;
            }
            else
            {
                vtxoff_next0 = pointVtxRef->getLink(ptoff0);
                vtxoff_next0 = vtxNextRef->getLink(vtxoff_next0);
                if (vtxoff_next0 == GA_INVALID_OFFSET)
                {
                    isTwoNeb0 = false;
                }
                else
                {
                    vtxoff_next0 = vtxNextRef->getLink(vtxoff_next0);
                    isTwoNeb0 = vtxoff_next0 == GA_INVALID_OFFSET;
                }
            }
            //if (!topo.isPointShared(ptoff0))

            //if (vtxoff_next == GA_INVALID_OFFSET)
                //outGroup->setElement(ptoff, true);

            vtxoff1 = geo->getPrimitiveVertexOffset(primoff, geo->getPrimitiveVertexCount(primoff)-1);
            ptoff1  = vtxPointRef->getLink(vtxoff1);
            if (stopPointGroup && stopPointGroup->contains(ptoff1))
            {
                isTwoNeb1 = false;
            }
            else
            {
                vtxoff_next1 = pointVtxRef->getLink(ptoff1);
                vtxoff_next1 = vtxNextRef->getLink(vtxoff_next1);
                if (vtxoff_next1 == GA_INVALID_OFFSET)
                {
                    isTwoNeb1 = false;
                }
                else
                {
                    vtxoff_next1 = vtxNextRef->getLink(vtxoff_next1);
                    isTwoNeb1 = vtxoff_next1 == GA_INVALID_OFFSET;
                }
            }

            if (isTwoNeb0 == isTwoNeb1)
            {
                if (keepLoop && !isTwoNeb0)
                {
                    unPassedPrimitiveGroup->setElement(primoff, false);
                }
                continue;
            }

            if (keepOrder && isTwoNeb0)
            {
                continue;
            }

            if (keepLoop)
            {
                unPassedPrimitiveGroup->setElement(primoff, false);
            }

            GEO_PrimPoly* primpoly = UTverify_cast<GEO_PrimPoly*>(geo->getPrimitive(primoff));
            if (isTwoNeb0)
            {
                primpoly->reverse();
            }

            vtxoff_prev = isTwoNeb0 ? vtxoff0 : vtxoff1;
            ptoff_next = isTwoNeb0 ? ptoff0 : ptoff1;

            if (srcPrimsAttrib)
            {
                srcPrims.emplace_back(primoff);
            }
#if 0
            while (1)
#else
            for (int loopNum = 0; loopNum < GFE_MAXLOOPCOUNT; loopNum++)
#endif
            {
                if (stopPointGroup && stopPointGroup->contains(ptoff_next))
                {
                    break;
                }
                else
                {
                    vtxoff_next = pointVtxRef->getLink(ptoff_next);
                    vtxoff_next = vtxNextRef->getLink(vtxoff_next);
                    if (vtxoff_next == GA_INVALID_OFFSET)
                    {
                        break;
                    }
                    else
                    {
                        if (vtxNextRef->getLink(vtxoff_next) != GA_INVALID_OFFSET)
                        {
                            break;
                        }

                    }
                }
                for (vtxoff_next = pointVtxRef->getLink(ptoff_next); vtxoff_next == vtxoff_prev; vtxoff_next = vtxNextRef->getLink(vtxoff_next))
                {
                    UT_ASSERT_MSG_P(vtxoff_next != GA_INVALID_OFFSET, "cant be possible");
                }
                primoff_neb = vtxPrimRef->getLink(vtxoff_next);

                const GA_OffsetListRef& vertices = geo->getPrimitiveVertexList(primoff_neb);
                const GA_Size numvtx = vertices.size() - 1;
                //GEO_Primitive* prim_neb = UTverify_cast<GEO_Primitive*>(geo->getPrimitive(primoff_neb));
                GA_Offset vtx0 = vertices[0];
                GA_Offset vtx1 = vertices[1];

                unPassedPrimitiveGroup->setElement(primoff_neb, false);
                delPrimitiveGroup->setElement(primoff_neb, true);

                //GA_Offset vtxoff_first = geo->appendVertexBlock(numvtx);
                if (vtxoff_next == vertices[0])
                {
                    vtxoff_prev = vertices[numvtx];
                    ptoff_next = vtxPointRef->getLink(vertices[numvtx]);
                    for (GA_Size i = 1; i <= numvtx; ++i)
                    {
                        primpoly->stealVertex(vertices[i]);
                        //topo.wireVertexPrimitive(vertices[i], primoff);
                        //prim_neb->releaseVertex(vertices[i]);
                        //geo->copyVertex(vtxoff_first + i - 1, vertices[i], true);
                        //topo.wireVertexPrimitive(vtxoff_first + i - 1, primoff);
                    }
                    //prim_neb->releaseVertex(vertices[0]);
                    //topo.delVertex(vertices[0]);
                    //topo.wireVertexPoint(vertices[0], );
                    //geo->destroyVertexOffset(vertices[0]);
                }
                else
                {
                    vtxoff_prev = vertices[0];
                    ptoff_next = vtxPointRef->getLink(vertices[0]);
                    for (GA_Size i = numvtx - 1; i >= 0; --i)
                    {
                        primpoly->stealVertex(vertices[i]);
                        //topo.wireVertexPrimitive(vertices[i], primoff);
                        //prim_neb->releaseVertex(vertices[i]);
                        //geo->copyVertex(vtxoff_first + numvtx - i - 1, vertices[i], true);
                        //topo.wireVertexPrimitive(vtxoff_first + numvtx - i - 1, primoff);
                    }
                    //prim_neb->releaseVertex(vertices[numvtx]);
                    //topo.delVertex(vertices[numvtx]);
                    //geo->destroyVertexOffset(vertices[numvtx]);
                }
                if (srcPrimsAttrib)
                {
                    srcPrims.emplace_back(primoff_neb);
                }

            }

            if (srcPrimsAttrib)
            {
                srcPrims_h.set(primoff, srcPrims);
                srcPrims.clear();
            }
        }
    }

    if (keepLoop)
    {
        for (GA_Iterator it(geo->getPrimitiveRange(unPassedPrimitiveGroup)); it.fullBlockAdvance(start, end); )
        {
            for (GA_Offset primoff = start; primoff < end; ++primoff)
            {
                if (!unPassedPrimitiveGroup->contains(primoff))
                    continue;

                //vtxoff0 = geo->getPrimitiveVertexOffset(primoff, 0);
                //ptoff0 = vtxPointRef->getLink(vtxoff0);
                //if (stopPointGroup && stopPointGroup->contains(ptoff0))
                //{
                //    isTwoNeb0 = false;
                //}
                //else
                //{
                //    vtxoff_next0 = pointVtxRef->getLink(ptoff0);
                //    vtxoff_next0 = vtxNextRef->getLink(vtxoff_next0);
                //    if (vtxoff_next0 == GA_INVALID_OFFSET)
                //    {
                //        isTwoNeb0 = false;
                //    }
                //    else
                //    {
                //        vtxoff_next0 = vtxNextRef->getLink(vtxoff_next0);
                //        isTwoNeb0 = vtxoff_next0 == GA_INVALID_OFFSET;
                //    }
                //}

                vtxoff1 = geo->getPrimitiveVertexOffset(primoff, geo->getPrimitiveVertexCount(primoff) - 1);
                ptoff1 = vtxPointRef->getLink(vtxoff1);
                //if (stopPointGroup && stopPointGroup->contains(ptoff1))
                //{
                //    isTwoNeb1 = false;
                //}
                //else
                //{
                //    vtxoff_next1 = pointVtxRef->getLink(ptoff1);
                //    vtxoff_next1 = vtxNextRef->getLink(vtxoff_next1);
                //    if (vtxoff_next1 == GA_INVALID_OFFSET)
                //    {
                //        isTwoNeb1 = false;
                //    }
                //    else
                //    {
                //        vtxoff_next1 = vtxNextRef->getLink(vtxoff_next1);
                //        isTwoNeb1 = vtxoff_next1 == GA_INVALID_OFFSET;
                //    }
                //}

                //if (isTwoNeb0 == isTwoNeb1)
                //{
                //    continue;
                //}

                unPassedPrimitiveGroup->setElement(primoff, false);

                //if (keepOrder && isTwoNeb0)
                //{
                //    continue;
                //}

                GEO_PrimPoly* primpoly = UTverify_cast<GEO_PrimPoly*>(geo->getPrimitive(primoff));
                //if (isTwoNeb0)
                //{
                //    primpoly->reverse();
                //}
                vtxoff_prev = vtxoff1;
                //ptoff_next = isTwoNeb0 ? ptoff0 : ptoff1;
                ptoff_next = ptoff1;
                if (srcPrimsAttrib)
                {
                    srcPrims.emplace_back(primoff_neb);
                }
#if 0
                while (1)
#else
                for (int loopNum = 0; loopNum < GFE_MAXLOOPCOUNT; loopNum++)
#endif
                {
                    //if (stopPointGroup && stopPointGroup->contains(ptoff_next))
                    //{
                    //    break;
                    //}
                    //else
                    //{
                    //    vtxoff_next = pointVtxRef->getLink(ptoff_next);
                    //    vtxoff_next = vtxNextRef->getLink(vtxoff_next);
                    //    if (vtxoff_next == GA_INVALID_OFFSET)
                    //    {
                    //        break;
                    //    }
                    //    else
                    //    {
                    //        if (vtxNextRef->getLink(vtxoff_next) != GA_INVALID_OFFSET)
                    //        {
                    //            break;
                    //        }
                    //    }
                    //}
                    for (vtxoff_next = pointVtxRef->getLink(ptoff_next); vtxoff_next == vtxoff_prev; vtxoff_next = vtxNextRef->getLink(vtxoff_next))
                    {
                        UT_ASSERT_MSG_P(vtxoff_next != GA_INVALID_OFFSET, "cant be possible");
                    }
                    primoff_neb = vtxPrimRef->getLink(vtxoff_next);
                    if (primoff_neb == primoff)
                        break;

                    const GA_OffsetListRef& vertices = geo->getPrimitiveVertexList(primoff_neb);
                    const GA_Size numvtx = vertices.size() - 1;

                    unPassedPrimitiveGroup->setElement(primoff_neb, false);
                    delPrimitiveGroup->setElement(primoff_neb, true);

                    if (vtxoff_next == vertices[0])
                    {
                        vtxoff_prev = vertices[numvtx];
                        ptoff_next = vtxPointRef->getLink(vertices[numvtx]);
                        for (GA_Size i = 1; i <= numvtx; ++i)
                        {
                            primpoly->stealVertex(vertices[i]);
                        }
                        //geo->destroyVertexOffset(vertices[0]);
                    }
                    else
                    {
                        vtxoff_prev = vertices[0];
                        ptoff_next = vtxPointRef->getLink(vertices[0]);
                        for (GA_Size i = numvtx - 1; i >= 0; --i)
                        {
                            primpoly->stealVertex(vertices[i]);
                        }
                        //geo->destroyVertexOffset(vertices[numvtx]);
                    }
                    if (srcPrimsAttrib)
                    {
                        srcPrims.emplace_back(primoff_neb);
                    }

                }

                if (closeLoop)
                {
                    geo->setPrimitiveClosedFlag(primoff, true);
                }

                if (srcPrimsAttrib)
                {
                    srcPrims_h.set(primoff, srcPrims);
                    srcPrims.clear();
                }
            }
        }
    }
    
    geo->destroyPrimitives(geo->getPrimitiveRange(delPrimitiveGroup));
}

SYS_FORCE_INLINE
static void
joinCurve(
    GA_Detail* const geo,
    const bool outSrcPrims,
    const GA_Storage storage = GA_STORE_INVALID,
    const UT_StringHolder& srcPrimsAttribName = "",
    const bool keepOrder = false,
    const bool keepLoop = true,
    const bool closeLoop = false,
    const GA_PointGroup* const stopPointGroup = nullptr
)
{   
    GA_Attribute* srcPrimsAttrib = nullptr;
    if (outSrcPrims && srcPrimsAttribName.isstring() && srcPrimsAttribName.length() != 0)
    {
        const GA_Storage finalStorageI = storage == GA_STORE_INVALID ? GFE_Type::getPreferredStorageI(geo) : storage;
        srcPrimsAttrib = geo->getAttributes().createTupleAttribute(GA_ATTRIB_PRIMITIVE, srcPrimsAttribName, finalStorageI, 1, GA_Defaults(-1));
    }
    joinCurve(geo, keepOrder, keepLoop, closeLoop, stopPointGroup, srcPrimsAttrib);
}


static void
joinCurve(
    const SOP_NodeVerb::CookParms& cookparms,
    GA_Detail* const geo,
    const UT_StringHolder& stopPointGroupName,
    const bool keepOrder = false,
    const bool keepLoop = true,
    const bool closeLoop = false,
    const bool outSrcPrims = false,
    const GA_Storage storage = GA_STORE_INVALID,
    const UT_StringHolder& srcPrimsAttribName = ""
)
{
    GOP_Manager gop;
    const GA_PointGroup* const stopPointGroup = GFE_GroupParser_Namespace::findOrParsePointGroupDetached(cookparms, geo, stopPointGroupName, gop);

    joinCurve(geo, outSrcPrims, storage, srcPrimsAttribName, keepOrder, keepLoop, closeLoop, stopPointGroup);
}


    

static void
joinCurveCheckInputError(
    const SOP_NodeVerb::CookParms& cookparms,
    const GA_Detail* const geo
)
{
    UT_ASSERT_P(geo);

    const GA_Topology& topo = geo->getTopology();
    const GA_ATITopology* vtxPointRef = topo.getPointRef();
    const GA_ATITopology* pointVtxRef = topo.getVertexRef();
    const GA_ATITopology* vtxNextRef = topo.getVertexNextRef();
    const GA_ATITopology* vtxPrimRef = topo.getPrimitiveRef();

    bool breakOut = false;
    GA_Offset vtxoff, ptoff;
    GA_Offset primoff_neb, vtxoff_next;
    GA_Offset start, end;
    for (GA_Iterator it(geo->getPrimitiveRange()); it.fullBlockAdvance(start, end); )
    {
        for (GA_Offset primoff = start; primoff < end; ++primoff)
        {
            vtxoff = geo->getPrimitiveVertexOffset(primoff, 0);
            ptoff = vtxPointRef->getLink(vtxoff);

            for (vtxoff_next = pointVtxRef->getLink(ptoff); vtxoff_next != GA_INVALID_OFFSET; vtxoff_next = vtxNextRef->getLink(vtxoff_next))
            {
                primoff_neb = vtxPrimRef->getLink(vtxoff_next);
                GA_Size vtxpnum = GFE_TopologyReference_Namespace::vertexPrimIndex(geo, primoff_neb, vtxoff_next);
                if (vtxpnum != 0 && vtxpnum != geo->getPrimitiveVertexCount(primoff_neb) - 1)
                {
                    breakOut = true;
                    cookparms.sopAddError(SOP_ERR_INVALID_SRC, "dont support curve ends on other curve's middle\nplz use poly path SOP");
                    break;
                }
            }

            if (breakOut)
                break;

            vtxoff = geo->getPrimitiveVertexOffset(primoff, geo->getPrimitiveVertexCount(primoff) - 1);
            ptoff = vtxPointRef->getLink(vtxoff);

            for (vtxoff_next = pointVtxRef->getLink(ptoff); vtxoff_next != GA_INVALID_OFFSET; vtxoff_next = vtxNextRef->getLink(vtxoff_next))
            {
                primoff_neb = vtxPrimRef->getLink(vtxoff_next);
                GA_Size vtxpnum = GFE_TopologyReference_Namespace::vertexPrimIndex(geo, primoff_neb, vtxoff_next);
                if (vtxpnum != 0 && vtxpnum != geo->getPrimitiveVertexCount(primoff_neb) - 1)
                {
                    breakOut = true;
                    cookparms.sopAddError(SOP_ERR_INVALID_SRC, "dont support curve ends on other curve's middle\nplz use poly path SOP");
                    break;
                }
            }
            if (breakOut)
                break;
        }
        if (breakOut)
            break;
    }
}

#undef GFE_MAXLOOPCOUNT



public:

private:

}; // End of class GFE_JoinCurve












































namespace GFE_JoinCurve_Namespace {


#ifndef GFE_MAXLOOPCOUNT
#define GFE_MAXLOOPCOUNT 1000000000
#endif

static void
    joinCurve(
        GA_Detail* const geo,
        const bool keepOrder = false,
        const bool keepLoop = true,
        const bool closeLoop = false,
        const GA_PointGroup* const stopPointGroup = nullptr,
        GA_Attribute* const srcPrimsAttrib = nullptr
    )
{
    UT_ASSERT_P(geo);

    UT_ValArray<GA_Offset> srcPrims;
    GA_RWHandleT<UT_ValArray<GA_Offset>> srcPrims_h;
    if (srcPrimsAttrib)
    {
        srcPrims.setCapacity(1024);
        srcPrims_h = srcPrimsAttrib;
    }

    //GA_Offset vtxoff_new = geo->appendVertex();
    //geo->getGEOPrimitive(0).wireVertexPrimitive(vtxoff_new, 0);
    //geo->getTopology().wireVertexPrimitive(vtxoff_new, 0);
    //geo->getTopology().wireVertexPoint(vtxoff_new, 2);
    //return;

    const GA_Range& primRange = geo->getPrimitiveRange();

    //const GA_PointGroupUPtr passedPointGroupUPtr = geo->createDetachedPointGroup();
    //GA_PointGroup* passedPointGroup = passedPointGroupUPtr.get();

    const GA_PrimitiveGroupUPtr unPassedPrimitiveGroupUPtr = geo->createDetachedPrimitiveGroup();
    GA_PrimitiveGroup* const unPassedPrimitiveGroup = unPassedPrimitiveGroupUPtr.get();
    unPassedPrimitiveGroup->setElement(primRange, true);

    //GA_PrimitiveGroupUPtr unPassedPrimitiveGroupUPtr;
    //GA_PrimitiveGroup* unPassedPrimitiveGroup;
    //if (keepLoop)
    //{
    //    unPassedPrimitiveGroupUPtr = geo->createDetachedPrimitiveGroup();
    //    unPassedPrimitiveGroup = unPassedPrimitiveGroupUPtr.get();
    //    unPassedPrimitiveGroup->setElement(primRange, true);
    //}

    const GA_PrimitiveGroupUPtr delPrimitiveGroupUPtr = geo->createDetachedPrimitiveGroup();
    GA_PrimitiveGroup* const delPrimitiveGroup = delPrimitiveGroupUPtr.get();
    delPrimitiveGroup->setElement(primRange, false);


    const GA_Topology& topo = geo->getTopology();
    //topo.makeFull();
    const GA_ATITopology* vtxPointRef = topo.getPointRef();
    const GA_ATITopology* pointVtxRef = topo.getVertexRef();
    const GA_ATITopology* vtxNextRef  = topo.getVertexNextRef();
    const GA_ATITopology* vtxPrimRef  = topo.getPrimitiveRef();

    bool isTwoNeb0, isTwoNeb1;
    GA_Offset vtxoff0, ptoff0, vtxoff_next0;
    GA_Offset vtxoff1, ptoff1, vtxoff_next1;
    GA_Offset primoff_neb, vtxoff_next, ptoff_next, vtxoff_prev;
    GA_Offset start, end;
    for (GA_Iterator it(primRange); it.fullBlockAdvance(start, end); )
    {
        for (GA_Offset primoff = start; primoff < end; ++primoff)
        {
            if (!unPassedPrimitiveGroup->contains(primoff))
                continue;
            vtxoff0 = geo->getPrimitiveVertexOffset(primoff, 0);
            ptoff0  = vtxPointRef->getLink(vtxoff0);
            if (stopPointGroup && stopPointGroup->contains(ptoff0))
            {
                isTwoNeb0 = false;
            }
            else
            {
                vtxoff_next0 = pointVtxRef->getLink(ptoff0);
                vtxoff_next0 = vtxNextRef->getLink(vtxoff_next0);
                if (vtxoff_next0 == GA_INVALID_OFFSET)
                {
                    isTwoNeb0 = false;
                }
                else
                {
                    vtxoff_next0 = vtxNextRef->getLink(vtxoff_next0);
                    isTwoNeb0 = vtxoff_next0 == GA_INVALID_OFFSET;
                }
            }
            //if (!topo.isPointShared(ptoff0))

            //if (vtxoff_next == GA_INVALID_OFFSET)
                //outGroup->setElement(ptoff, true);

            vtxoff1 = geo->getPrimitiveVertexOffset(primoff, geo->getPrimitiveVertexCount(primoff)-1);
            ptoff1  = vtxPointRef->getLink(vtxoff1);
            if (stopPointGroup && stopPointGroup->contains(ptoff1))
            {
                isTwoNeb1 = false;
            }
            else
            {
                vtxoff_next1 = pointVtxRef->getLink(ptoff1);
                vtxoff_next1 = vtxNextRef->getLink(vtxoff_next1);
                if (vtxoff_next1 == GA_INVALID_OFFSET)
                {
                    isTwoNeb1 = false;
                }
                else
                {
                    vtxoff_next1 = vtxNextRef->getLink(vtxoff_next1);
                    isTwoNeb1 = vtxoff_next1 == GA_INVALID_OFFSET;
                }
            }

            if (isTwoNeb0 == isTwoNeb1)
            {
                if (keepLoop && !isTwoNeb0)
                {
                    unPassedPrimitiveGroup->setElement(primoff, false);
                }
                continue;
            }

            if (keepOrder && isTwoNeb0)
            {
                continue;
            }

            if (keepLoop)
            {
                unPassedPrimitiveGroup->setElement(primoff, false);
            }

            GEO_PrimPoly* primpoly = UTverify_cast<GEO_PrimPoly*>(geo->getPrimitive(primoff));
            if (isTwoNeb0)
            {
                primpoly->reverse();
            }

            vtxoff_prev = isTwoNeb0 ? vtxoff0 : vtxoff1;
            ptoff_next = isTwoNeb0 ? ptoff0 : ptoff1;

            if (srcPrimsAttrib)
            {
                srcPrims.emplace_back(primoff);
            }
#if 0
            while (1)
#else
            for (int loopNum = 0; loopNum < GFE_MAXLOOPCOUNT; loopNum++)
#endif
            {
                if (stopPointGroup && stopPointGroup->contains(ptoff_next))
                {
                    break;
                }
                else
                {
                    vtxoff_next = pointVtxRef->getLink(ptoff_next);
                    vtxoff_next = vtxNextRef->getLink(vtxoff_next);
                    if (vtxoff_next == GA_INVALID_OFFSET)
                    {
                        break;
                    }
                    else
                    {
                        if (vtxNextRef->getLink(vtxoff_next) != GA_INVALID_OFFSET)
                        {
                            break;
                        }

                    }
                }
                for (vtxoff_next = pointVtxRef->getLink(ptoff_next); vtxoff_next == vtxoff_prev; vtxoff_next = vtxNextRef->getLink(vtxoff_next))
                {
                    UT_ASSERT_MSG_P(vtxoff_next != GA_INVALID_OFFSET, "cant be possible");
                }
                primoff_neb = vtxPrimRef->getLink(vtxoff_next);

                const GA_OffsetListRef& vertices = geo->getPrimitiveVertexList(primoff_neb);
                const GA_Size numvtx = vertices.size() - 1;
                //GEO_Primitive* prim_neb = UTverify_cast<GEO_Primitive*>(geo->getPrimitive(primoff_neb));
                GA_Offset vtx0 = vertices[0];
                GA_Offset vtx1 = vertices[1];

                unPassedPrimitiveGroup->setElement(primoff_neb, false);
                delPrimitiveGroup->setElement(primoff_neb, true);

                //GA_Offset vtxoff_first = geo->appendVertexBlock(numvtx);
                if (vtxoff_next == vertices[0])
                {
                    vtxoff_prev = vertices[numvtx];
                    ptoff_next = vtxPointRef->getLink(vertices[numvtx]);
                    for (GA_Size i = 1; i <= numvtx; ++i)
                    {
                        primpoly->stealVertex(vertices[i]);
                        //topo.wireVertexPrimitive(vertices[i], primoff);
                        //prim_neb->releaseVertex(vertices[i]);
                        //geo->copyVertex(vtxoff_first + i - 1, vertices[i], true);
                        //topo.wireVertexPrimitive(vtxoff_first + i - 1, primoff);
                    }
                    //prim_neb->releaseVertex(vertices[0]);
                    //topo.delVertex(vertices[0]);
                    //topo.wireVertexPoint(vertices[0], );
                    //geo->destroyVertexOffset(vertices[0]);
                }
                else
                {
                    vtxoff_prev = vertices[0];
                    ptoff_next = vtxPointRef->getLink(vertices[0]);
                    for (GA_Size i = numvtx - 1; i >= 0; --i)
                    {
                        primpoly->stealVertex(vertices[i]);
                        //topo.wireVertexPrimitive(vertices[i], primoff);
                        //prim_neb->releaseVertex(vertices[i]);
                        //geo->copyVertex(vtxoff_first + numvtx - i - 1, vertices[i], true);
                        //topo.wireVertexPrimitive(vtxoff_first + numvtx - i - 1, primoff);
                    }
                    //prim_neb->releaseVertex(vertices[numvtx]);
                    //topo.delVertex(vertices[numvtx]);
                    //geo->destroyVertexOffset(vertices[numvtx]);
                }
                if (srcPrimsAttrib)
                {
                    srcPrims.emplace_back(primoff_neb);
                }

            }

            if (srcPrimsAttrib)
            {
                srcPrims_h.set(primoff, srcPrims);
                srcPrims.clear();
            }
        }
    }

    if (keepLoop)
    {
        for (GA_Iterator it(geo->getPrimitiveRange(unPassedPrimitiveGroup)); it.fullBlockAdvance(start, end); )
        {
            for (GA_Offset primoff = start; primoff < end; ++primoff)
            {
                if (!unPassedPrimitiveGroup->contains(primoff))
                    continue;

                //vtxoff0 = geo->getPrimitiveVertexOffset(primoff, 0);
                //ptoff0 = vtxPointRef->getLink(vtxoff0);
                //if (stopPointGroup && stopPointGroup->contains(ptoff0))
                //{
                //    isTwoNeb0 = false;
                //}
                //else
                //{
                //    vtxoff_next0 = pointVtxRef->getLink(ptoff0);
                //    vtxoff_next0 = vtxNextRef->getLink(vtxoff_next0);
                //    if (vtxoff_next0 == GA_INVALID_OFFSET)
                //    {
                //        isTwoNeb0 = false;
                //    }
                //    else
                //    {
                //        vtxoff_next0 = vtxNextRef->getLink(vtxoff_next0);
                //        isTwoNeb0 = vtxoff_next0 == GA_INVALID_OFFSET;
                //    }
                //}

                vtxoff1 = geo->getPrimitiveVertexOffset(primoff, geo->getPrimitiveVertexCount(primoff) - 1);
                ptoff1 = vtxPointRef->getLink(vtxoff1);
                //if (stopPointGroup && stopPointGroup->contains(ptoff1))
                //{
                //    isTwoNeb1 = false;
                //}
                //else
                //{
                //    vtxoff_next1 = pointVtxRef->getLink(ptoff1);
                //    vtxoff_next1 = vtxNextRef->getLink(vtxoff_next1);
                //    if (vtxoff_next1 == GA_INVALID_OFFSET)
                //    {
                //        isTwoNeb1 = false;
                //    }
                //    else
                //    {
                //        vtxoff_next1 = vtxNextRef->getLink(vtxoff_next1);
                //        isTwoNeb1 = vtxoff_next1 == GA_INVALID_OFFSET;
                //    }
                //}

                //if (isTwoNeb0 == isTwoNeb1)
                //{
                //    continue;
                //}

                unPassedPrimitiveGroup->setElement(primoff, false);

                //if (keepOrder && isTwoNeb0)
                //{
                //    continue;
                //}

                GEO_PrimPoly* primpoly = UTverify_cast<GEO_PrimPoly*>(geo->getPrimitive(primoff));
                //if (isTwoNeb0)
                //{
                //    primpoly->reverse();
                //}
                vtxoff_prev = vtxoff1;
                //ptoff_next = isTwoNeb0 ? ptoff0 : ptoff1;
                ptoff_next = ptoff1;
                if (srcPrimsAttrib)
                {
                    srcPrims.emplace_back(primoff_neb);
                }
#if 0
                while (1)
#else
                for (int loopNum = 0; loopNum < GFE_MAXLOOPCOUNT; loopNum++)
#endif
                {
                    //if (stopPointGroup && stopPointGroup->contains(ptoff_next))
                    //{
                    //    break;
                    //}
                    //else
                    //{
                    //    vtxoff_next = pointVtxRef->getLink(ptoff_next);
                    //    vtxoff_next = vtxNextRef->getLink(vtxoff_next);
                    //    if (vtxoff_next == GA_INVALID_OFFSET)
                    //    {
                    //        break;
                    //    }
                    //    else
                    //    {
                    //        if (vtxNextRef->getLink(vtxoff_next) != GA_INVALID_OFFSET)
                    //        {
                    //            break;
                    //        }
                    //    }
                    //}
                    for (vtxoff_next = pointVtxRef->getLink(ptoff_next); vtxoff_next == vtxoff_prev; vtxoff_next = vtxNextRef->getLink(vtxoff_next))
                    {
                        UT_ASSERT_MSG_P(vtxoff_next != GA_INVALID_OFFSET, "cant be possible");
                    }
                    primoff_neb = vtxPrimRef->getLink(vtxoff_next);
                    if (primoff_neb == primoff)
                        break;

                    const GA_OffsetListRef& vertices = geo->getPrimitiveVertexList(primoff_neb);
                    const GA_Size numvtx = vertices.size() - 1;

                    unPassedPrimitiveGroup->setElement(primoff_neb, false);
                    delPrimitiveGroup->setElement(primoff_neb, true);

                    if (vtxoff_next == vertices[0])
                    {
                        vtxoff_prev = vertices[numvtx];
                        ptoff_next = vtxPointRef->getLink(vertices[numvtx]);
                        for (GA_Size i = 1; i <= numvtx; ++i)
                        {
                            primpoly->stealVertex(vertices[i]);
                        }
                        //geo->destroyVertexOffset(vertices[0]);
                    }
                    else
                    {
                        vtxoff_prev = vertices[0];
                        ptoff_next = vtxPointRef->getLink(vertices[0]);
                        for (GA_Size i = numvtx - 1; i >= 0; --i)
                        {
                            primpoly->stealVertex(vertices[i]);
                        }
                        //geo->destroyVertexOffset(vertices[numvtx]);
                    }
                    if (srcPrimsAttrib)
                    {
                        srcPrims.emplace_back(primoff_neb);
                    }

                }

                if (closeLoop)
                {
                    geo->setPrimitiveClosedFlag(primoff, true);
                }

                if (srcPrimsAttrib)
                {
                    srcPrims_h.set(primoff, srcPrims);
                    srcPrims.clear();
                }
            }
        }
    }
    
    geo->destroyPrimitives(geo->getPrimitiveRange(delPrimitiveGroup));
}

SYS_FORCE_INLINE
static void
joinCurve(
    GA_Detail* const geo,
    const bool outSrcPrims,
    const GA_Storage storage = GA_STORE_INVALID,
    const UT_StringHolder& srcPrimsAttribName = "",
    const bool keepOrder = false,
    const bool keepLoop = true,
    const bool closeLoop = false,
    const GA_PointGroup* const stopPointGroup = nullptr
)
{   
    GA_Attribute* srcPrimsAttrib = nullptr;
    if (outSrcPrims && srcPrimsAttribName.isstring() && srcPrimsAttribName.length() != 0)
    {
        const GA_Storage finalStorageI = storage == GA_STORE_INVALID ? GFE_Type::getPreferredStorageI(geo) : storage;
        srcPrimsAttrib = geo->getAttributes().createTupleAttribute(GA_ATTRIB_PRIMITIVE, srcPrimsAttribName, finalStorageI, 1, GA_Defaults(-1));
    }
    joinCurve(geo, keepOrder, keepLoop, closeLoop, stopPointGroup, srcPrimsAttrib);
}


static void
joinCurve(
    const SOP_NodeVerb::CookParms& cookparms,
    GA_Detail* const geo,
    const UT_StringHolder& stopPointGroupName,
    const bool keepOrder = false,
    const bool keepLoop = true,
    const bool closeLoop = false,
    const bool outSrcPrims = false,
    const GA_Storage storage = GA_STORE_INVALID,
    const UT_StringHolder& srcPrimsAttribName = ""
)
{
    GOP_Manager gop;
    const GA_PointGroup* const stopPointGroup = GFE_GroupParser_Namespace::findOrParsePointGroupDetached(cookparms, geo, stopPointGroupName, gop);

    joinCurve(geo, outSrcPrims, storage, srcPrimsAttribName, keepOrder, keepLoop, closeLoop, stopPointGroup);
}


    

static void
joinCurveCheckInputError(
    const SOP_NodeVerb::CookParms& cookparms,
    const GA_Detail* const geo
)
{
    UT_ASSERT_P(geo);

    const GA_Topology& topo = geo->getTopology();
    const GA_ATITopology* vtxPointRef = topo.getPointRef();
    const GA_ATITopology* pointVtxRef = topo.getVertexRef();
    const GA_ATITopology* vtxNextRef = topo.getVertexNextRef();
    const GA_ATITopology* vtxPrimRef = topo.getPrimitiveRef();

    bool breakOut = false;
    GA_Offset vtxoff, ptoff;
    GA_Offset primoff_neb, vtxoff_next;
    GA_Offset start, end;
    for (GA_Iterator it(geo->getPrimitiveRange()); it.fullBlockAdvance(start, end); )
    {
        for (GA_Offset primoff = start; primoff < end; ++primoff)
        {
            vtxoff = geo->getPrimitiveVertexOffset(primoff, 0);
            ptoff = vtxPointRef->getLink(vtxoff);

            for (vtxoff_next = pointVtxRef->getLink(ptoff); vtxoff_next != GA_INVALID_OFFSET; vtxoff_next = vtxNextRef->getLink(vtxoff_next))
            {
                primoff_neb = vtxPrimRef->getLink(vtxoff_next);
                GA_Size vtxpnum = GFE_TopologyReference_Namespace::vertexPrimIndex(geo, primoff_neb, vtxoff_next);
                if (vtxpnum != 0 && vtxpnum != geo->getPrimitiveVertexCount(primoff_neb) - 1)
                {
                    breakOut = true;
                    cookparms.sopAddError(SOP_ERR_INVALID_SRC, "dont support curve ends on other curve's middle\nplz use poly path SOP");
                    break;
                }
            }

            if (breakOut)
                break;

            vtxoff = geo->getPrimitiveVertexOffset(primoff, geo->getPrimitiveVertexCount(primoff) - 1);
            ptoff = vtxPointRef->getLink(vtxoff);

            for (vtxoff_next = pointVtxRef->getLink(ptoff); vtxoff_next != GA_INVALID_OFFSET; vtxoff_next = vtxNextRef->getLink(vtxoff_next))
            {
                primoff_neb = vtxPrimRef->getLink(vtxoff_next);
                GA_Size vtxpnum = GFE_TopologyReference_Namespace::vertexPrimIndex(geo, primoff_neb, vtxoff_next);
                if (vtxpnum != 0 && vtxpnum != geo->getPrimitiveVertexCount(primoff_neb) - 1)
                {
                    breakOut = true;
                    cookparms.sopAddError(SOP_ERR_INVALID_SRC, "dont support curve ends on other curve's middle\nplz use poly path SOP");
                    break;
                }
            }
            if (breakOut)
                break;
        }
        if (breakOut)
            break;
    }
}

#undef GFE_MAXLOOPCOUNT

} // End of namespace GFE_JoinCurve_Namespace




#endif
