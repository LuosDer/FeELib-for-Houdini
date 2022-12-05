
#pragma once

#ifndef __GA_FeE_JoinCurve_h__
#define __GA_FeE_JoinCurve_h__

//#include "GA_FeE/GA_FeE_JoinCurve.h"

#include "GA/GA_Detail.h"
//#include "GA/GA_Types.h"

#include "GA_FeE/GA_FeE_Type.h"
#include "GA_FeE/GA_FeE_TopologyReference.h"

namespace GA_FeE_JoinCurve {



static void
    joinCurve(
        GU_Detail* const geo,
        const GA_PointGroup* const stopPointGroup,
        const bool keepOrder = false,
        const bool keepLoop = true
    )
{
    UT_ASSERT_P(geo);

    const GA_Range& primRange = geo->getPrimitiveRange();

    const GA_PointGroupUPtr passedPointGroupUPtr = geo->createDetachedPointGroup();
    GA_PointGroup* passedPointGroup = passedPointGroupUPtr.get();


    const GA_PrimitiveGroupUPtr passedPrimitiveGroupUPtr = geo->createDetachedPrimitiveGroup();
    GA_PrimitiveGroup* passedPrimitiveGroup = passedPrimitiveGroupUPtr.get();
    passedPrimitiveGroup->setElement(primRange, true);

    const GA_PrimitiveGroupUPtr delPrimitiveGroupUPtr = geo->createDetachedPrimitiveGroup();
    GA_PrimitiveGroup* delPrimitiveGroup = delPrimitiveGroupUPtr.get();

    //GA_PrimitiveGroupUPtr passedPrimitiveGroupUPtr;
    //GA_PrimitiveGroup* passedPrimitiveGroup = nullptr;

    //if (keepLoop)
    //{
    //    passedPrimitiveGroupUPtr = geo->createDetachedPrimitiveGroup();
    //    passedPrimitiveGroup = passedPrimitiveGroupUPtr.get();
    //}



    GA_Topology& topo = geo->getTopology();
    topo.makeFull();
    const GA_ATITopology* vtxPointRef = topo.getPointRef();
    const GA_ATITopology* pointVtxRef = topo.getVertexRef();
    const GA_ATITopology* vtxNextRef  = topo.getVertexNextRef();
    const GA_ATITopology* vtxPrimRef  = topo.getPrimitiveRef();

    bool isTwoNeb0, isTwoNeb1;
    GA_Offset vtxoff0, ptoff0, vtxoff_next0;
    GA_Offset vtxoff1, ptoff1, vtxoff_next1;
    GA_Offset primoff_prev, primoff_neb, vtxoff_next, ptoff_next;
    GA_Offset start, end;
    for (GA_Iterator it(primRange); it.fullBlockAdvance(start, end); )
    {
        for (GA_Offset primoff = start; primoff < end; ++primoff)
        {
            if (!passedPrimitiveGroup->contains(primoff))
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
                continue;
            }

            if (keepOrder && isTwoNeb0)
            {
                continue;
            }

            if (keepLoop)
            {
                passedPrimitiveGroup->setElement(primoff, false);
            }
            if (isTwoNeb0)
            {
                GEO_Primitive* prim = geo->getGEOPrimitive(primoff);
                prim->reverse();
            }
            primoff_prev = primoff;
            ptoff_next = isTwoNeb0 ? ptoff0 : ptoff1;
            while (ptoff_next != GA_INVALID_OFFSET)
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
                for (vtxoff_next = pointVtxRef->getLink(ptoff_next); ; vtxoff_next = vtxNextRef->getLink(vtxoff_next))
                {
                    primoff_neb = vtxPrimRef->getLink(vtxoff_next);
                    if (primoff_neb != primoff_prev)
                        break;
                    if (vtxoff_next == GA_INVALID_OFFSET)
                        UT_ASSERT("cant be possible");
                }

                const GA_OffsetListRef& vertices = geo->getPrimitiveVertexList(primoff_neb);
                const GA_Size numvtx = vertices.size() - 1;
                GEO_Primitive* prim_neb = geo->getGEOPrimitive(primoff_neb);

                if (keepLoop)
                {
                    passedPrimitiveGroup->setElement(primoff_neb, false);
                }
                delPrimitiveGroup->setElement(primoff_neb, true);

                //GA_Offset vtxoff_first = geo->appendVertexBlock(numvtx);
                if (vtxoff_next == vertices[0])
                {
                    for (GA_Size i = 1; i <= numvtx; ++i)
                    {
                        topo.wireVertexPrimitive(vertices[i], primoff);
                        //prim_neb->releaseVertex(vertices[i]);
                        //geo->copyVertex(vtxoff_first + i - 1, vertices[i], true);
                        //topo.wireVertexPrimitive(vtxoff_first + i - 1, primoff);
                    }
                    prim_neb->releaseVertex(vertices[0]);
                    //topo.delVertex(vertices[0]);
                    //topo.wireVertexPoint(vertices[0], );
                    geo->destroyVertexOffset(vertices[0]);
                    ptoff_next = vtxPointRef->getLink(vertices[numvtx]);
                }
                else
                {
                    for (GA_Size i = numvtx - 1; i >= 0; --i)
                    {
                        topo.wireVertexPrimitive(vertices[i], primoff);
                        //prim_neb->releaseVertex(vertices[i]);
                        //geo->copyVertex(vtxoff_first + numvtx - i - 1, vertices[i], true);
                        //topo.wireVertexPrimitive(vtxoff_first + numvtx - i - 1, primoff);
                    }
                    prim_neb->releaseVertex(vertices[numvtx]);
                    //topo.delVertex(vertices[numvtx]);
                    geo->destroyVertexOffset(vertices[numvtx]);
                    ptoff_next = vtxPointRef->getLink(vertices[0]);
                }

                primoff_prev = primoff_neb;
            }

            
            //if (outSrcPrims)
            //{
            //    srcPrims.emplace_back();
            //    srcPrimsAttribH.set(newPrimoff, srcPrims);
            //}
            //if (outSrcPrims)
            //{
            //    srcPrims.clear();
            //}
            
        }
    }
//    return;
//    for (GA_Iterator it(geo->getPrimitiveRange(passedPrimitiveGroup)); it.fullBlockAdvance(start, end); )
//    {
//        for (GA_Offset primoff = start; primoff < end; ++primoff)
//        {
//#if 1
//            if (!passedPrimitiveGroup->contains(primoff))
//            {
//                UT_ASSERT("cant be possible");
//            }
//            if (delPrimitiveGroup->contains(primoff))
//            {
//                UT_ASSERT("cant be possible");
//            }
//#endif
//            passedPrimitiveGroup->setElement(primoff, false);
//            passedPrimitiveGroup->setElement(primoff_neb, false);
//            //delPrimitiveGroup->setElement(primoff_neb, false);
//
//        }
//    }
    //geo->destroyPrimitiveOffsets(geo->getPrimitiveRange(passedPrimitiveGroup));
    //geo->destroyPrimitives(geo->getPrimitiveRange(delPrimitiveGroup));
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
                GA_Size vtxpnum = GA_FeE_TopologyReference::vertexPrimIndex(geo, primoff_neb, vtxoff_next);
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
                GA_Size vtxpnum = GA_FeE_TopologyReference::vertexPrimIndex(geo, primoff_neb, vtxoff_next);
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


} // End of namespace GA_FeE_JoinCurve

#endif
