
#pragma once

#ifndef __GFE_Enumerate_h__
#define __GFE_Enumerate_h__

//#include "GFE/GFE_Enumerate.h"

#include "GA/GA_PageHandle.h"
#include "GA/GA_PageIterator.h"


#include "GFE/GFE_GeoFilter.h"





class GFE_Enumerate : public GFE_AttribFilter {


public:
    using GFE_AttribFilter::GFE_AttribFilter;


    void
        setComputeParm(
            const bool outAsOffset = true,
            const exint subscribeRatio = 64,
            const exint minGrainSize = 64
        )
    {
        setHasComputed();
        this->outAsOffset = outAsOffset;
        this->subscribeRatio = subscribeRatio;
        this->minGrainSize = minGrainSize;
    }


private:

    // can not use in parallel unless for each GA_Detail
    virtual bool
        computeCore() override
    {
        if (getOutAttribArray().isEmpty())
            return false;

        if (groupParser.isEmpty())
            return true;
        
        
        GA_Attribute* const attribPtr = getOutAttribArray()[0];
        // const GA_Storage storage = attribPtr->getAIFTuple()->getStorage(attribPtr);
        switch (attribPtr->getAIFTuple()->getStorage(attribPtr))
        {
        case GA_STORE_INT16:  enumerate<int16>(attribPtr);           break;
        case GA_STORE_INT32:  enumerate<int32>(attribPtr);           break;
        case GA_STORE_INT64:  enumerate<int64>(attribPtr);           break;
        case GA_STORE_REAL16: enumerate<fpreal16>(attribPtr);        break;
        case GA_STORE_REAL32: enumerate<fpreal32>(attribPtr);        break;
        case GA_STORE_REAL64: enumerate<fpreal64>(attribPtr);        break;
        case GA_STORE_STRING: enumerate<UT_StringHolder>(attribPtr); break;
        default: break;
        }

        return true;
    }


    template<typename T>
    void enumerate(GA_Attribute* const attribPtr)
    {
        UT_ASSERT_P(attribPtr);
        const GA_SplittableRange geoSplittableRange0(groupParser.getRange(attribPtr->getOwner()));
        if(outAsOffset)
        {
            UTparallelFor(geoSplittableRange0, [attribPtr](const GA_SplittableRange& r)
            {
                GA_PageHandleT<T, T, true, true, GA_Attribute, GA_ATINumeric, GA_Detail> attrib_ph(attribPtr);
                for (GA_PageIterator pit = r.beginPages(); !pit.atEnd(); ++pit)
                {
                    GA_Offset start, end;
                    for (GA_Iterator it(pit.begin()); it.blockAdvance(start, end); )
                    {
                        attrib_ph.setPage(start);
                        for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                        {
                            attrib_ph.value(elemoff) = elemoff;
                        }
                    }
                }
            }, subscribeRatio, minGrainSize);
        }
        else
        {
            const GA_IndexMap& indexMap = attribPtr->getIndexMap();
            UTparallelFor(geoSplittableRange0, [attribPtr, &indexMap](const GA_SplittableRange& r)
            {
                GA_PageHandleT<T, T, true, true, GA_Attribute, GA_ATINumeric, GA_Detail> attrib_ph(attribPtr);
                for (GA_PageIterator pit = r.beginPages(); !pit.atEnd(); ++pit)
                {
                    GA_Offset start, end;
                    for (GA_Iterator it(pit.begin()); it.blockAdvance(start, end); )
                    {
                        attrib_ph.setPage(start);
                        for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                        {
                            attrib_ph.value(elemoff) = indexMap.indexFromOffset(elemoff);
                        }
                    }
                }
            }, subscribeRatio, minGrainSize);
        }
    }


    template<>
    void enumerate<UT_StringHolder>(GA_Attribute* const attribPtr)
    {
        UT_ASSERT_P(attribPtr);

        const GA_RWHandleS attrib_h(attribPtr);
        const GA_SplittableRange geoSplittableRange0(groupParser.getRange(attribPtr->getOwner()));
        if(outAsOffset)
        {
            UTparallelFor(geoSplittableRange0, [&attrib_h](const GA_SplittableRange& r)
            {
                GA_Offset start, end;
                for (GA_Iterator it(r); it.blockAdvance(start, end); )
                {
                    for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                    {
                        const char cElemoff = (char)(elemoff+'0');
                        attrib_h.set(elemoff, UT_StringHolder(&cElemoff));
                    }
                }
            }, subscribeRatio, minGrainSize);
        }
        else
        {
            const GA_IndexMap& indexMap = attribPtr->getIndexMap();
            UTparallelFor(geoSplittableRange0, [&attrib_h, &indexMap](const GA_SplittableRange& r)
            {
                GA_Offset start, end;
                for (GA_Iterator it(r); it.blockAdvance(start, end); )
                {
                    for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                    {
                        const char cElemoff = (char)(indexMap.indexFromOffset(elemoff)+'0');
                        attrib_h.set(elemoff, cElemoff);
                    }
                }
            }, subscribeRatio, minGrainSize);
        }
    }

    


public:
    bool outAsOffset = true;

private:
    exint subscribeRatio = 64;
    exint minGrainSize = 64;


}; // End of class GFE_Enumerate


















//
//
//
//
//namespace GFE_Enumerate_Namespace {
//
//    template<typename FLOAT_T>
//    static void
//        curveUV(
//            GA_Detail* const geo,
//            const GA_RWHandleT<UT_Vector3T<FLOAT_T>>& uv_h,
//            const GA_PrimitiveGroup* const geoPrimGroup = nullptr,
//            const GFE_EnumerateMethod curveUVMethod = GFE_EnumerateMethod_WorldArcLength,
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
//                    case GFE_EnumerateMethod::WorldArcLength:
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
//                    case GFE_EnumerateMethod::WorldAverage:
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
//                    case GFE_EnumerateMethod::LocalArcLength:
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
//                    case GFE_EnumerateMethod::LocalAverage:
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
//                    case GFE_EnumerateMethod::WorldArcLength:
//                        success = GEO_Curve0->grevilleTexture(uv_h, isPointAttrib);
//                        break;
//                    case GFE_EnumerateMethod::LocalAverage:
//                        success = GEO_Curve0->uniformTexture(uv_h, isPointAttrib);
//                        break;
//                    case GFE_EnumerateMethod::LocalArcLength:
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
//        GA_Attribute* const attribPtr,
//        const GA_PrimitiveGroup* const geoPrimGroup = nullptr,
//        const GFE_EnumerateMethod curveUVMethod = GFE_EnumerateMethod::WorldArcLength,
//        const exint subscribeRatio = 64,
//        const exint minGrainSize = 64
//    )
//{
//    UT_ASSERT_P(attribPtr);
//    switch (attribPtr->getAIFTuple()->getStorage(attribPtr))
//    {
//    case GA_STORE_REAL16:
//        curveUV<fpreal16>(geo, attribPtr, geoPrimGroup,
//            curveUVMethod,
//            subscribeRatio, minGrainSize);
//        break;
//    case GA_STORE_REAL32:
//        curveUV<fpreal32>(geo, attribPtr, geoPrimGroup,
//            curveUVMethod,
//            subscribeRatio, minGrainSize);
//        break;
//    case GA_STORE_REAL64:
//        //curveUV<fpreal64>(geo, attribPtr, geoPrimGroup,
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
//    const GFE_EnumerateMethod curveUVMethod = GFE_EnumerateMethod::WorldArcLength,
//    const exint subscribeRatio = 64,
//    const exint minGrainSize = 64
//)
//{
//    GA_Attribute* const attribPtr = GFE_Attribute::findOrCreateUVAttributePointVertex(geo, uvAttribClass, uvAttribName);
//
//    curveUV(geo, attribPtr, geoPrimGroup,
//        curveUVMethod,
//        subscribeRatio, minGrainSize);
//
//    return attribPtr;
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
//    const GFE_EnumerateMethod curveUVMethod = GFE_EnumerateMethod::WorldArcLength,
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
//} // End of namespace GFE_Enumerate







#endif
