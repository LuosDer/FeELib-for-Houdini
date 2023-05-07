
#pragma once

#ifndef __GFE_OffsetVertexOrder_h__
#define __GFE_OffsetVertexOrder_h__

//#include "GFE/GFE_OffsetVertexOrder.h"


#include "GFE/GFE_GeoFilter.h"



class GFE_OffsetVertexOrder : public GFE_AttribFilter {



public:
    using GFE_AttribFilter::GFE_AttribFilter;

    ~GFE_OffsetVertexOrder()
    {
    }

    

    SYS_FORCE_INLINE void setOffsetAttrib(const UT_StringRef& attribName)
    {
        offsetAttribNonConst = geo->findAttribute(GA_ATTRIB_PRIMITIVE, attribName);
        offsetAttrib = offsetAttribNonConst;
    }

    SYS_FORCE_INLINE void setOffsetAttrib(const GA_Attribute* const attrib)
    {
        offsetAttribNonConst = nullptr;
        offsetAttrib = attrib;
    }

    SYS_FORCE_INLINE void setOffsetAttrib(GA_Attribute* const attrib)
    {
        offsetAttribNonConst = attrib;
        offsetAttrib = attrib;
    }


    void
        setComputeParm(
            const GA_Size offset = 0,
            const bool delOffsetAttrib = false,
            const exint subscribeRatio = 64,
            const exint minGrainSize = 64
        )
    {
        setHasComputed();
        this->offset = offset;
        this->delOffsetAttrib = delOffsetAttrib;
        this->subscribeRatio = subscribeRatio;
        this->minGrainSize = minGrainSize;
    }


private:

    // can not use in parallel unless for each GA_Detail
    virtual bool
        computeCore() override
    {
        if (offset == 0 && !offsetAttrib)
            return true;

        if (groupParser.isEmpty())
            return true;


        if (offsetAttrib)
            offsetVertexOrderByAttrib();
        else
            offsetVertexOrder();

        if (delOffsetAttrib && offsetAttribNonConst)
        {
            geo->destroyAttrib(offsetAttribNonConst);
        }



        return true;
    }


    void
        offsetVertexOrderByAttrib()
    {
        UT_ASSERT_P(offsetAttrib);

        UTparallelFor(groupParser.getPrimitiveSplittableRange(), [this](const GA_SplittableRange& r)
        {
            GA_PageHandleT<exint, exint, true, false, const GA_Attribute, const GA_ATINumeric, const GA_Detail> attrib_ph(offsetAttrib);
            for (GA_PageIterator pit = r.beginPages(); !pit.atEnd(); ++pit)
            {
                GA_Offset start, end;
                for (GA_Iterator it(pit.begin()); it.blockAdvance(start, end); )
                {
                    attrib_ph.setPage(start);
                    for (GA_Offset primoff = start; primoff < end; ++primoff)
                    {
                        const GA_Size offsetFinal = offset + attrib_ph.value(primoff);

                        const GA_Size numvtx = geo->getPrimitiveVertexCount(primoff);

                        const bool isClosed = geo->getPrimitiveClosedFlag(primoff);
                        const bool isPrimLooped = geo->isPrimitiveLooped(primoff);
                        
                        for (GA_Size vtxpnum = 0; vtxpnum < numvtx; ++vtxpnum)
                        {
                            const GA_Offset vtxoff = geo->getPrimitiveVertexOffset(primoff, vtxpnum);

                            GA_Size vtxpnum_next = vtxpnum + offsetFinal;
                            if (vtxpnum_next >= numvtx || vtxpnum_next < 0)
                            {
                                if (isPrimLooped)
                                {
                                    if (isClosed)
                                    {
                                        vtxpnum_next %= numvtx;
                                    }
                                    else
                                    {
                                        vtxpnum_next %= numvtx - 1;
                                    }
                                }
                                else
                                {
                                    geo->destroyVertexOffset(vtxoff);
                                    continue;
                                }
                            }
                            geo->getTopology().wireVertexPoint(vtxoff, geo->getPrimitivePointOffset(primoff, vtxpnum_next));
                        }
                    }
                }
            }
        }, subscribeRatio, minGrainSize);
    }



    void
        offsetVertexOrder()
    {
        UT_ASSERT_P(offsetAttrib);

        UTparallelFor(groupParser.getPrimitiveSplittableRange(), [this](const GA_SplittableRange& r)
            {
                for (GA_PageIterator pit = r.beginPages(); !pit.atEnd(); ++pit)
                {
                    GA_Offset start, end;
                    for (GA_Iterator it(pit.begin()); it.blockAdvance(start, end); )
                    {
                        for (GA_Offset primoff = start; primoff < end; ++primoff)
                        {
                            const GA_Size numvtx = geo->getPrimitiveVertexCount(primoff);

                            const bool isClosed = geo->getPrimitiveClosedFlag(primoff);
                            const bool isPrimLooped = geo->isPrimitiveLooped(primoff);

                            for (GA_Size vtxpnum = 0; vtxpnum < numvtx; ++vtxpnum)
                            {
                                const GA_Offset vtxoff = geo->getPrimitiveVertexOffset(primoff, vtxpnum);

                                GA_Size vtxpnum_next = vtxpnum + offset;
                                if (vtxpnum_next >= numvtx || vtxpnum_next < 0)
                                {
                                    if (isPrimLooped)
                                    {
                                        if (isClosed)
                                        {
                                            vtxpnum_next %= numvtx;
                                        }
                                        else
                                        {
                                            vtxpnum_next %= numvtx - 1;
                                        }
                                    }
                                    else
                                    {
                                        geo->destroyVertexOffset(vtxoff);
                                        continue;
                                    }
                                }
                                geo->getTopology().wireVertexPoint(vtxoff, geo->getPrimitivePointOffset(primoff, vtxpnum_next));
                            }
                        }
                    }
                }
            }, subscribeRatio, minGrainSize);
    }



public:
    GA_Size offset = 0;
    const GA_Attribute* offsetAttrib = nullptr;
    GA_Attribute* offsetAttribNonConst = nullptr;
    bool delOffsetAttrib = false;

private:

    exint subscribeRatio = 64;
    exint minGrainSize = 64;


};


















//
//
//
//
//namespace GFE_OffsetVertexOrder_Namespace {
//
//    template<typename FLOAT_T>
//    static void
//        curveUV(
//            GA_Detail* const geo,
//            const GA_RWHandleT<UT_Vector3T<FLOAT_T>>& uv_h,
//            const GA_PrimitiveGroup* const geoPrimGroup = nullptr,
//            const GFE_OffsetVertexOrderMethod curveUVMethod = GFE_OffsetVertexOrderMethod_WorldArcLength,
//            const exint subscribeRatio = 64,
//            const exint minGrainSize = 64
//        )
//    {
//        if (geoPrimGroup && geoPrimGroup->isEmpty())
//            return;
//        //static_cast<GU_Detail*>(geo)->length
//        const bool isPointAttrib = uv_h.getAttribute()->getOwner() == GA_ATTRIB_POINT;
//        const GA_AttributeOwner owner = uv_h.getAttribute()->getOwner();
//
//        //GA_Attribute* const pAttribPtr = GFE_Measure::addAttribPrimPerimeter(geo, geoPrimGroup);
//        //GA_ROHandleT<fpreal> p_h(pAttribPtr);
//        //GA_Attribute* const pAttribPtr = GFE_Measure::addAttribPrimPerimeter(geo, geoPrimGroup);
//        GA_ROHandleT<UT_Vector3T<fpreal>> pos_h(geo->getP());
//
//        const GA_SplittableRange geoSplittableRange0(geo->getPrimitiveRange(geoPrimGroup));
//        UTparallelFor(geoSplittableRange0, [geo, &uv_h, &pos_h, curveUVMethod, isPointAttrib](const GA_SplittableRange& r)
//        {
//            UT_Array<fpreal> uvs;
//            fpreal dist = 0;
//            UT_Vector3T<fpreal> uv(0.0);
//            UT_Vector3T<fpreal> pos, pos_prev;
//            GA_Offset ptoff, vtxoff;
//            GA_Offset start, end;
//            for (GA_Iterator it(r); it.blockAdvance(start, end); )
//            {
//                for (GA_Offset primoff = start; primoff < end; ++primoff)
//                {
//                    const GA_Size numvtx = geo->getPrimitiveVertexCount(primoff);
//                    if (numvtx < 2) return;
//#if 1
//                    switch (curveUVMethod)
//                    {
//                    case GFE_OffsetVertexOrderMethod::WorldArcLength:
//                        ptoff = geo->vertexPoint(geo->getPrimitiveVertexOffset(primoff, 0));
//                        pos_prev = pos_h.get(ptoff);
//                        for (GA_Size vtxpnum = 1; vtxpnum < numvtx; vtxpnum++)
//                        {
//                            vtxoff = geo->getPrimitiveVertexOffset(primoff, vtxpnum);
//                            ptoff = geo->vertexPoint(vtxoff);
//                            pos = pos_h.get(ptoff);
//                            uv[0] += pos.distance(pos_prev);
//                            uv_h.set(isPointAttrib ? ptoff : vtxoff, 0, uv);
//                            pos_prev = pos;
//                        }
//                        break;
//                    case GFE_OffsetVertexOrderMethod::WorldAverage:
//                        ptoff = geo->vertexPoint(geo->getPrimitiveVertexOffset(primoff, 0));
//                        pos_prev = pos_h.get(ptoff);
//                        for (GA_Size vtxpnum = 1; vtxpnum < numvtx; vtxpnum++)
//                        {
//                            vtxoff = geo->getPrimitiveVertexOffset(primoff, vtxpnum);
//                            ptoff = geo->vertexPoint(vtxoff);
//                            pos = pos_h.get(ptoff);
//                            dist += pos.distance(pos_prev);
//                            pos_prev = pos;
//                        }
//                        dist /= (numvtx-1);
//                        for (GA_Size vtxpnum = 1; vtxpnum < numvtx; vtxpnum++)
//                        {
//                            vtxoff = geo->getPrimitiveVertexOffset(primoff, vtxpnum);
//                            uv[0] += dist;
//                            uv_h.set(isPointAttrib ? geo->vertexPoint(vtxoff) : vtxoff, 0, uv);
//                        }
//                        break;
//                    case GFE_OffsetVertexOrderMethod::LocalArcLength:
//                    {
//                        uvs.setSize(numvtx);
//                        ptoff = geo->vertexPoint(geo->getPrimitiveVertexOffset(primoff, 0));
//                        pos_prev = pos_h.get(ptoff);
//                        for (GA_Size vtxpnum = 1; vtxpnum < numvtx; vtxpnum++)
//                        {
//                            vtxoff = geo->getPrimitiveVertexOffset(primoff, vtxpnum);
//                            ptoff = geo->vertexPoint(vtxoff);
//                            pos = pos_h.get(ptoff);
//                            dist += pos.distance(pos_prev);
//                            uvs[vtxpnum] = dist;
//                            pos_prev = pos;
//                        }
//                        GA_Size lastIndex = numvtx - 1;
//                        fpreal p = uvs[lastIndex];
//                        for (GA_Size vtxpnum = 1; vtxpnum < lastIndex; vtxpnum++)
//                        {
//                            vtxoff = geo->getPrimitiveVertexOffset(primoff, vtxpnum);
//                            uv[0] = uvs[vtxpnum] / p;
//                            uv_h.set(isPointAttrib ? geo->vertexPoint(vtxoff) : vtxoff, 0, uv);
//                        }
//                        vtxoff = geo->getPrimitiveVertexOffset(primoff, lastIndex);
//                        uv[0] = 1.0;
//                        uv_h.set(isPointAttrib ? geo->vertexPoint(vtxoff) : vtxoff, 0, uv);
//                    }
//                        break;
//                    case GFE_OffsetVertexOrderMethod::LocalAverage:
//                        dist = 1.0 / (numvtx-1);
//                        for (GA_Size vtxpnum = 1; vtxpnum < numvtx; vtxpnum++)
//                        {
//                            vtxoff = geo->getPrimitiveVertexOffset(primoff, vtxpnum);
//                            uv[0] += dist;
//                            uv_h.set(isPointAttrib ? geo->vertexPoint(vtxoff) : vtxoff, 0, uv);
//                        }
//                        break;
//                    default:
//                        UT_ASSERT_MSG(0, "unhandled curveUVMethod");
//                        break;
//                    }
//#else
//                    GEO_Curve* GEO_Curve0 = static_cast<GEO_Curve*>(geo->getPrimitive(primoff));
//
//                    bool success = false;
//                    switch (curveUVMethod)
//                    {
//                    case GFE_OffsetVertexOrderMethod::WorldArcLength:
//                        success = GEO_Curve0->grevilleTexture(uv_h, isPointAttrib);
//                        break;
//                    case GFE_OffsetVertexOrderMethod::LocalAverage:
//                        success = GEO_Curve0->uniformTexture(uv_h, isPointAttrib);
//                        break;
//                    case GFE_OffsetVertexOrderMethod::LocalArcLength:
//                        success = GEO_Curve0->chordLenTexture(uv_h, isPointAttrib);
//                        break;
//                    default:
//                        UT_ASSERT_MSG(0, "unhandled curveUVMethod");
//                        break;
//                    }
//                    UT_ASSERT_P(success);
//#endif
//                }
//            }
//        }, subscribeRatio, minGrainSize);
//    }
//
//SYS_FORCE_INLINE
//    static void
//    curveUV(
//        GA_Detail* const geo,
//        GA_Attribute* const uvAttribPtr,
//        const GA_PrimitiveGroup* const geoPrimGroup = nullptr,
//        const GFE_OffsetVertexOrderMethod curveUVMethod = GFE_OffsetVertexOrderMethod::WorldArcLength,
//        const exint subscribeRatio = 64,
//        const exint minGrainSize = 64
//    )
//{
//    UT_ASSERT_P(uvAttribPtr);
//    switch (uvAttribPtr->getAIFTuple()->getStorage(uvAttribPtr))
//    {
//    case GA_STORE_REAL16:
//        curveUV<fpreal16>(geo, uvAttribPtr, geoPrimGroup,
//            curveUVMethod,
//            subscribeRatio, minGrainSize);
//        break;
//    case GA_STORE_REAL32:
//        curveUV<fpreal32>(geo, uvAttribPtr, geoPrimGroup,
//            curveUVMethod,
//            subscribeRatio, minGrainSize);
//        break;
//    case GA_STORE_REAL64:
//        //curveUV<fpreal64>(geo, uvAttribPtr, geoPrimGroup,
//        //    curveUVMethod,
//        //    subscribeRatio, minGrainSize);
//        break;
//    default:
//        break;
//    }
//}
//
//
//static GA_Attribute*
//curveUV(
//    GA_Detail* const geo,
//    const GA_PrimitiveGroup* const geoPrimGroup = nullptr,
//    const GA_Storage storage = GA_STORE_INVALID,
//    const GA_AttributeOwner uvAttribClass = GA_ATTRIB_VERTEX,
//    const UT_StringHolder& uvAttribName = "uv",
//    const GFE_OffsetVertexOrderMethod curveUVMethod = GFE_OffsetVertexOrderMethod::WorldArcLength,
//    const exint subscribeRatio = 64,
//    const exint minGrainSize = 64
//)
//{
//    GA_Attribute* const uvAttribPtr = GFE_Attribute::findOrCreateUVAttributePointVertex(geo, uvAttribClass, uvAttribName);
//
//    curveUV(geo, uvAttribPtr, geoPrimGroup,
//        curveUVMethod,
//        subscribeRatio, minGrainSize);
//
//    return uvAttribPtr;
//}
//
//
//SYS_FORCE_INLINE
//static GA_Attribute*
//curveUV(
//    const SOP_NodeVerb::CookParms& cookparms,
//    GA_Detail* const geo,
//    const UT_StringHolder& primGroupName,
//    const GA_Storage storage = GA_STORE_INVALID,
//    const GA_AttributeOwner uvAttribClass = GA_ATTRIB_VERTEX,
//    const UT_StringHolder& uvAttribName = "uv",
//    const GFE_OffsetVertexOrderMethod curveUVMethod = GFE_OffsetVertexOrderMethod::WorldArcLength,
//    const exint subscribeRatio = 64,
//    const exint minGrainSize = 64
//)
//{
//    GOP_Manager gop;
//    const GA_PrimitiveGroup* const geoPrimGroup = GFE_GroupParser_Namespace::findOrParsePrimitiveGroupDetached(cookparms, geo, primGroupName, gop);
//
//    return curveUV(geo, geoPrimGroup,
//        storage, uvAttribClass, uvAttribName, curveUVMethod,
//        subscribeRatio, minGrainSize);
//}
//
//
//
//
//
//
//} // End of namespace GFE_OffsetVertexOrder







#endif
