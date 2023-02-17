
#pragma once

#ifndef __GA_FeE_CurveUV_h__
#define __GA_FeE_CurveUV_h__

//#include "GA_FeE/GA_FeE_CurveUV.h"

#include "GA/GA_Detail.h"

#include "GEO/GEO_Curve.h"


#include "GA_FeE/GA_FeE_Type.h"
#include "GA_FeE/GA_FeE_Attribute.h"
#include "GA_FeE/GA_FeE_Group.h"
#include "GA_FeE/GA_FeE_Measure.h"

enum GFE_CurveUVMethod
{
    GFE_CurveUVMethod_ArcLength,
    GFE_CurveUVMethod_Average,
    GFE_CurveUVMethod_ChordLen,
};

namespace GA_FeE_CurveUV {

    template<typename FLOAT_T>
    SYS_FORCE_INLINE
        static void
        curveUV(
            GA_Detail* const geo,
            const GA_RWHandleT<UT_Vector3T<fpreal32>>& uv_h,
            const GA_PrimitiveGroup* const geoPrimGroup = nullptr,
            const GFE_CurveUVMethod curveUVMethod = GFE_CurveUVMethod_ArcLength,
            const exint subscribeRatio = 64,
            const exint minGrainSize = 64
        )
    {
        if (geoPrimGroup && geoPrimGroup->isEmpty())
            return;

        const bool isPointAttrib = uv_h.getAttribute()->getOwner() == GA_ATTRIB_POINT;
        GA_Attribute* const pAttribPtr = GA_FeE_Measure::addAttribPrimPerimeter(geo, geoPrimGroup);
        GA_ROHandleT<fpreal> p_h(pAttribPtr);

        const GA_SplittableRange geoSplittableRange0(geo->getPrimitiveRange(geoPrimGroup));
        UTparallelFor(geoSplittableRange0, [geo, &uv_h, &p_h, curveUVMethod, isPointAttrib](const GA_SplittableRange& r)
        {
            GA_Offset start, end;
            for (GA_Iterator it(r); it.blockAdvance(start, end); )
            {
                for (GA_Offset primoff = start; primoff < end; ++primoff)
                {
                    const GA_Size numvtx = geo->getPrimitiveVertexCount(primoff);
                    if (numvtx < 2) return;

                    GEO_Curve* GEO_Curve0 = static_cast<GEO_Curve*>(geo->getPrimitive(primoff));

                    bool success = false;
                    switch (curveUVMethod)
                    {
                    case GFE_CurveUVMethod_ArcLength:
                        success = GEO_Curve0->grevilleTexture(uv_h, isPointAttrib);
                        break;
                    case GFE_CurveUVMethod_Average:
                        success = GEO_Curve0->uniformTexture(uv_h, isPointAttrib);
                        break;
                    case GFE_CurveUVMethod_ChordLen:
                        success = GEO_Curve0->chordLenTexture(uv_h, isPointAttrib);
                        break;
                    default:
                        UT_ASSERT_MSG(0, "unhandled curveUVMethod");
                        break;
                    }
                    UT_ASSERT_P(success);
                }
            }
        }, subscribeRatio, minGrainSize);
    }

SYS_FORCE_INLINE
    static void
    curveUV(
        GA_Detail* const geo,
        GA_Attribute* const uvAttribPtr,
        const GA_PrimitiveGroup* const geoPrimGroup = nullptr,
        const GFE_CurveUVMethod curveUVMethod = GFE_CurveUVMethod_ArcLength,
        const exint subscribeRatio = 64,
        const exint minGrainSize = 64
    )
{
    UT_ASSERT_P(uvAttribPtr);
    switch (uvAttribPtr->getAIFTuple()->getStorage(uvAttribPtr))
    {
    case GA_STORE_REAL16:
        curveUV<fpreal16>(geo, uvAttribPtr, geoPrimGroup,
            curveUVMethod,
            subscribeRatio, minGrainSize);
        break;
    case GA_STORE_REAL32:
        curveUV<fpreal32>(geo, uvAttribPtr, geoPrimGroup,
            curveUVMethod,
            subscribeRatio, minGrainSize);
        break;
    case GA_STORE_REAL64:
        curveUV<fpreal64>(geo, uvAttribPtr, geoPrimGroup,
            curveUVMethod,
            subscribeRatio, minGrainSize);
        break;
    default:
        break;
    }
}


SYS_FORCE_INLINE
static GA_Attribute*
curveUV(
    GA_Detail* const geo,
    const GA_PrimitiveGroup* const geoPrimGroup = nullptr,
    const GA_Storage storage = GA_STORE_INVALID,
    const GA_AttributeOwner uvAttribClass = GA_ATTRIB_VERTEX,
    const UT_StringHolder& uvAttribName = "uv",
    const GFE_CurveUVMethod curveUVMethod = GFE_CurveUVMethod_ArcLength,
    const exint subscribeRatio = 64,
    const exint minGrainSize = 64
)
{
    GA_Attribute* const uvAttribPtr = GA_FeE_Attribute::findOrCreateUVAttributePointVertex(geo, uvAttribClass, uvAttribName);

    curveUV(geo, uvAttribPtr, geoPrimGroup,
        curveUVMethod,
        subscribeRatio, minGrainSize);

    return uvAttribPtr;
}


SYS_FORCE_INLINE
static GA_Attribute*
curveUV(
    const SOP_NodeVerb::CookParms& cookparms,
    GA_Detail* const geo,
    const UT_StringHolder& primGroupName,
    const GA_Storage storage = GA_STORE_INVALID,
    const GA_AttributeOwner uvAttribClass = GA_ATTRIB_VERTEX,
    const UT_StringHolder& uvAttribName = "uv",
    const GFE_CurveUVMethod curveUVMethod = GFE_CurveUVMethod_ArcLength,
    const exint subscribeRatio = 64,
    const exint minGrainSize = 64
)
{
    GOP_Manager gop;
    const GA_PrimitiveGroup* const geoPrimGroup = GA_FeE_Group::findOrParsePrimitiveGroupDetached(cookparms, geo, primGroupName, gop);

    return curveUV(geo, geoPrimGroup,
        storage, uvAttribClass, uvAttribName, curveUVMethod,
        subscribeRatio, minGrainSize);
}






} // End of namespace GA_FeE_CurveUV

#endif
