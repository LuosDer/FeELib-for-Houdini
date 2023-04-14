
#pragma once

#ifndef __GFE_GroupAttribBoundary_h__
#define __GFE_GroupAttribBoundary_h__

//#include "GFE/GFE_GroupAttribBoundary.h"

#include "GFE/GFE_GeoFilter.h"


//#include "GEO/GEO_Curve.h"
//#include "GFE/GFE_Type.h"
//#include "GFE/GFE_Attribute.h"
//#include "GFE/GFE_GroupParser.h"
//#include "GFE/GFE_Measure.h"




class GFE_GroupAttribBoundary : public GFE_AttribFilter {



public:
    using GFE_AttribFilter::GFE_AttribFilter;

    ~GFE_GroupAttribBoundary()
    {
    }


    void
        setGroup(
            const GA_PrimitiveGroup* const geoPrimGroup = nullptr
        )
    {
        groupParser.setGroup(geoPrimGroup);
    }

    void
        setGroup(
            const UT_StringHolder& primGroupName = ""
        )
    {
        groupParser.setPrimitiveGroup(primGroupName);
    }


    GA_Attribute*
        findOrCreateUV(
            const bool detached = false,
            const GA_AttributeOwner uvAttribClass = GA_ATTRIB_VERTEX,
            const GA_Storage storage = GA_STORE_INVALID,
            const UT_StringHolder& name = "uv",
            const int tuple_size = 3
        )
    {
        return getOutAttribArray().findOrCreateUV(detached, uvAttribClass, storage, name, tuple_size);
    }

    void
        setComputeParm(
            const GFE_GroupAttribBoundaryMethod curveUVMethod = GFE_GroupAttribBoundaryMethod::WorldArcLength,
            const exint subscribeRatio = 64,
            const exint minGrainSize = 64
        )
    {
        setHasComputed();
        this->curveUVMethod = curveUVMethod;
        this->subscribeRatio = subscribeRatio;
        this->minGrainSize = minGrainSize;
    }


private:

    // can not use in parallel unless for each GA_Detail
    virtual bool
        computeCore() override
    {
        if (groupParser.isEmpty())
            return true;


        GA_Attribute* const uvAttribPtr = getOutAttribArray()[0];
        switch (uvAttribPtr->getAIFTuple()->getTupleSize(uvAttribPtr))
        {
        case 2:
            switch (uvAttribPtr->getAIFTuple()->getStorage(uvAttribPtr))
            {
            case GA_STORE_REAL16:
                curveUV<UT_Vector2T<fpreal16>>(uvAttribPtr);
                break;
            case GA_STORE_REAL32:
                curveUV<UT_Vector2T<fpreal32>>(uvAttribPtr);
                break;
            case GA_STORE_REAL64:
                curveUV<UT_Vector2T<fpreal64>>(uvAttribPtr);
                break;
            default:
                break;
            }
            break;
        case 3:
            switch (uvAttribPtr->getAIFTuple()->getStorage(uvAttribPtr))
            {
            case GA_STORE_REAL16:
                curveUV<UT_Vector3T<fpreal16>>(uvAttribPtr);
                break;
            case GA_STORE_REAL32:
                curveUV<UT_Vector3T<fpreal32>>(uvAttribPtr);
                break;
            case GA_STORE_REAL64:
                curveUV<UT_Vector3T<fpreal64>>(uvAttribPtr);
                break;
            default:
                break;
            }
            break;
        //case 4:
        //    switch (uvAttribPtr->getAIFTuple()->getStorage(uvAttribPtr))
        //    {
        //    case GA_STORE_REAL16:
        //        curveUV<UT_Vector4T<fpreal16>>(uvAttribPtr);
        //        break;
        //    case GA_STORE_REAL32:
        //        curveUV<UT_Vector4T<fpreal32>>(uvAttribPtr);
        //        break;
        //    case GA_STORE_REAL64:
        //        curveUV<UT_Vector4T<fpreal64>>(uvAttribPtr);
        //        break;
        //    default:
        //        break;
        //    }
        //break;
        default:
            break;
        }


        return true;
    }


    template<typename VECTOR_T>
    void
        curveUV(
            GA_Attribute* const uvAttribPtr
        )
    {
        using value_type = typename VECTOR_T::value_type;

        const GA_RWHandleT<VECTOR_T>& uv_h(uvAttribPtr);

        const GA_PrimitiveGroup* const geoPrimGroup = groupParser.getPrimitiveGroup();

        //static_cast<GU_Detail*>(geo)->length
        const bool isPointAttrib = uvAttribPtr->getOwner() == GA_ATTRIB_POINT;
        const GA_AttributeOwner owner = uvAttribPtr->getOwner();

        //GA_Attribute* const pAttribPtr = GFE_Measure::addAttribPrimPerimeter(geo, geoPrimGroup);
        //GA_ROHandleT<fpreal> p_h(pAttribPtr);
        //GA_Attribute* const pAttribPtr = GFE_Measure::addAttribPrimPerimeter(geo, geoPrimGroup);
        GA_ROHandleT<UT_Vector3T<value_type>> pos_h(geo->getP());

        const GA_SplittableRange geoSplittableRange0(geo->getPrimitiveRange(geoPrimGroup));
        UTparallelFor(geoSplittableRange0, [this, &uv_h, &pos_h, isPointAttrib](const GA_SplittableRange& r)
        {
            UT_Array<value_type> uvs;
            value_type dist = 0;
            VECTOR_T uv(0.0);
            UT_Vector3T<value_type> pos, pos_prev;
            GA_Offset ptoff, vtxoff;
            GA_Offset start, end;
            for (GA_Iterator it(r); it.blockAdvance(start, end); )
            {
                for (GA_Offset primoff = start; primoff < end; ++primoff)
                {
                    const GA_Size numvtx = geo->getPrimitiveVertexCount(primoff);
                    if (numvtx < 2) return;
#if 1
                    switch (curveUVMethod)
                    {
                    case GFE_GroupAttribBoundaryMethod::LocalAverage:
                        dist = 1.0 / (numvtx - 1);
                        for (GA_Size vtxpnum = 1; vtxpnum < numvtx; vtxpnum++)
                        {
                            vtxoff = geo->getPrimitiveVertexOffset(primoff, vtxpnum);
                            uv[0] += dist;
                            uv_h.set(isPointAttrib ? geo->vertexPoint(vtxoff) : vtxoff, 0, uv);
                        }
                        break;
                    case GFE_GroupAttribBoundaryMethod::LocalArcLength:
                    {
                        uvs.setSize(numvtx);
                        ptoff = geo->vertexPoint(geo->getPrimitiveVertexOffset(primoff, 0));
                        pos_prev = pos_h.get(ptoff);
                        for (GA_Size vtxpnum = 1; vtxpnum < numvtx; vtxpnum++)
                        {
                            vtxoff = geo->getPrimitiveVertexOffset(primoff, vtxpnum);
                            ptoff = geo->vertexPoint(vtxoff);
                            pos = pos_h.get(ptoff);
                            dist += pos.distance(pos_prev);
                            uvs[vtxpnum] = dist;
                            pos_prev = pos;
                        }
                        GA_Size lastIndex = numvtx - 1;
                        value_type p = uvs[lastIndex];
                        for (GA_Size vtxpnum = 1; vtxpnum < lastIndex; vtxpnum++)
                        {
                            vtxoff = geo->getPrimitiveVertexOffset(primoff, vtxpnum);
                            uv[0] = uvs[vtxpnum] / p;
                            uv_h.set(isPointAttrib ? geo->vertexPoint(vtxoff) : vtxoff, 0, uv);
                        }
                        vtxoff = geo->getPrimitiveVertexOffset(primoff, lastIndex);
                        uv[0] = 1.0;
                        uv_h.set(isPointAttrib ? geo->vertexPoint(vtxoff) : vtxoff, 0, uv);
                    }
                    break;
                    case GFE_GroupAttribBoundaryMethod::WorldAverage:
                        ptoff = geo->vertexPoint(geo->getPrimitiveVertexOffset(primoff, 0));
                        pos_prev = pos_h.get(ptoff);
                        for (GA_Size vtxpnum = 1; vtxpnum < numvtx; vtxpnum++)
                        {
                            vtxoff = geo->getPrimitiveVertexOffset(primoff, vtxpnum);
                            ptoff = geo->vertexPoint(vtxoff);
                            pos = pos_h.get(ptoff);
                            dist += pos.distance(pos_prev);
                            pos_prev = pos;
                        }
                        dist /= (numvtx - 1);
                        for (GA_Size vtxpnum = 1; vtxpnum < numvtx; vtxpnum++)
                        {
                            vtxoff = geo->getPrimitiveVertexOffset(primoff, vtxpnum);
                            uv[0] += dist;
                            uv_h.set(isPointAttrib ? geo->vertexPoint(vtxoff) : vtxoff, 0, uv);
                        }
                        break;
                    case GFE_GroupAttribBoundaryMethod::WorldArcLength:
                        ptoff = geo->vertexPoint(geo->getPrimitiveVertexOffset(primoff, 0));
                        pos_prev = pos_h.get(ptoff);
                        for (GA_Size vtxpnum = 1; vtxpnum < numvtx; vtxpnum++)
                        {
                            vtxoff = geo->getPrimitiveVertexOffset(primoff, vtxpnum);
                            ptoff = geo->vertexPoint(vtxoff);
                            pos = pos_h.get(ptoff);
                            uv[0] += pos.distance(pos_prev);
                            uv_h.set(isPointAttrib ? ptoff : vtxoff, 0, uv);
                            pos_prev = pos;
                        }
                        break;
                    default:
                        UT_ASSERT_MSG(0, "unhandled curveUVMethod");
                        break;
                    }
#else
                    GEO_Curve* GEO_Curve0 = static_cast<GEO_Curve*>(geo->getPrimitive(primoff));

                    bool success = false;
                    switch (curveUVMethod)
                    {
                    case GFE_GroupAttribBoundaryMethod::LocalAverage:
                        success = GEO_Curve0->uniformTexture(uv_h, isPointAttrib);
                        break;
                    case GFE_GroupAttribBoundaryMethod::LocalArcLength:
                        success = GEO_Curve0->chordLenTexture(uv_h, isPointAttrib);
                        break;
                    case GFE_GroupAttribBoundaryMethod::WorldArcLength:
                        success = GEO_Curve0->grevilleTexture(uv_h, isPointAttrib);
                        break;
                    default:
                        UT_ASSERT_MSG(0, "unhandled curveUVMethod");
                        break;
                    }
                    UT_ASSERT_P(success);
#endif
                }
            }
        }, subscribeRatio, minGrainSize);
    }




public:
    GFE_GroupAttribBoundaryMethod curveUVMethod = GFE_GroupAttribBoundaryMethod::WorldArcLength;

private:
    exint subscribeRatio = 64;
    exint minGrainSize = 64;


};


















//
//
//
//
//namespace GFE_GroupAttribBoundary_Namespace {
//
//    template<typename FLOAT_T>
//    static void
//        curveUV(
//            GA_Detail* const geo,
//            const GA_RWHandleT<UT_Vector3T<FLOAT_T>>& uv_h,
//            const GA_PrimitiveGroup* const geoPrimGroup = nullptr,
//            const GFE_GroupAttribBoundaryMethod curveUVMethod = GFE_GroupAttribBoundaryMethod_WorldArcLength,
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
//                    case GFE_GroupAttribBoundaryMethod::WorldArcLength:
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
//                    case GFE_GroupAttribBoundaryMethod::WorldAverage:
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
//                    case GFE_GroupAttribBoundaryMethod::LocalArcLength:
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
//                    case GFE_GroupAttribBoundaryMethod::LocalAverage:
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
//                    case GFE_GroupAttribBoundaryMethod::WorldArcLength:
//                        success = GEO_Curve0->grevilleTexture(uv_h, isPointAttrib);
//                        break;
//                    case GFE_GroupAttribBoundaryMethod::LocalAverage:
//                        success = GEO_Curve0->uniformTexture(uv_h, isPointAttrib);
//                        break;
//                    case GFE_GroupAttribBoundaryMethod::LocalArcLength:
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
//        const GFE_GroupAttribBoundaryMethod curveUVMethod = GFE_GroupAttribBoundaryMethod::WorldArcLength,
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
//    const GFE_GroupAttribBoundaryMethod curveUVMethod = GFE_GroupAttribBoundaryMethod::WorldArcLength,
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
//    const GFE_GroupAttribBoundaryMethod curveUVMethod = GFE_GroupAttribBoundaryMethod::WorldArcLength,
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
//} // End of namespace GFE_GroupAttribBoundary







#endif
