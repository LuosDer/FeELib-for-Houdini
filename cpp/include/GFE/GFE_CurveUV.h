
#pragma once

#ifndef __GFE_CurveUV_h__
#define __GFE_CurveUV_h__

#include "GFE/GFE_CurveUV.h"

#include "GFE/GFE_GeoFilter.h"


//#include "GEO/GEO_Curve.h"
//#include "GFE/GFE_Type.h"
//#include "GFE/GFE_Attribute.h"
//#include "GFE/GFE_GroupParser.h"
//#include "GFE/GFE_Measure.h"



enum class GFE_CurveUVMethod
{
    LocalAverage,
    LocalArcLength,
    WorldAverage,
    WorldArcLength,
};


class GFE_CurveUV : public GFE_AttribFilter {

public:
    using GFE_AttribFilter::GFE_AttribFilter;

    ~GFE_CurveUV()
    {
    }


    //SYS_FORCE_INLINE void setGroup(const GA_PrimitiveGroup* const geoPrimGroup = nullptr)
    //{ groupParser.setGroup(geoPrimGroup); }

    //SYS_FORCE_INLINE void setGroup(const UT_StringHolder& primGroupName = "")
    //{ groupParser.setPrimitiveGroup(primGroupName); }


    //SYS_FORCE_INLINE GA_Attribute*
    //    findOrCreateUV(
    //        const bool detached = false,
    //        const GA_AttributeOwner uvAttribClass = GA_ATTRIB_VERTEX,
    //        const GA_Storage storage = GA_STORE_INVALID,
    //        const UT_StringHolder& name = "uv",
    //        const int tuple_size = 3
    //    )
    //{ return getOutAttribArray().findOrCreateUV(detached, uvAttribClass, storage, name, tuple_size); }

    void
        setComputeParm(
            const GFE_CurveUVMethod curveUVMethod = GFE_CurveUVMethod::WorldArcLength,
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


        uvAttrib = getOutAttribArray()[0];
        const GA_Storage storage = uvAttrib->getAIFTuple()->getStorage(uvAttrib);
        switch (uvAttrib->getAIFTuple()->getTupleSize(uvAttrib))
        {
        case 2:
            switch (storage)
            {
            case GA_STORE_REAL16: curveUV<UT_Vector2T<fpreal16>>(); break;
            case GA_STORE_REAL32: curveUV<UT_Vector2T<fpreal32>>(); break;
            case GA_STORE_REAL64:
            default:              curveUV<UT_Vector2T<fpreal64>>(); break;
            }
            break;
        case 3:
            switch (storage)
            {
            case GA_STORE_REAL16: curveUV<UT_Vector3T<fpreal16>>(); break;
            case GA_STORE_REAL32: curveUV<UT_Vector3T<fpreal32>>(); break;
            case GA_STORE_REAL64:
            default:              curveUV<UT_Vector3T<fpreal64>>(); break;
            }
            break;
        case 4:
            switch (storage)
            {
            case GA_STORE_REAL16: curveUV<UT_Vector4T<fpreal16>>(); break;
            case GA_STORE_REAL32: curveUV<UT_Vector4T<fpreal32>>(); break;
            case GA_STORE_REAL64:
            default:              curveUV<UT_Vector4T<fpreal64>>(); break;
            }
        break;
        default:
            break;
        }


        return true;
    }


    template<typename VECTOR_T>
    void curveUV()
    {
        using value_type = typename VECTOR_T::value_type;

        const GA_RWHandleT<VECTOR_T> uv_h(uvAttrib);

        //static_cast<GU_Detail*>(geo)->length
        const bool isPointAttrib = uvAttrib->getOwner() == GA_ATTRIB_POINT;
        const GA_AttributeOwner owner = uvAttrib->getOwner();

        //GA_Attribute* const pAttribPtr = GFE_Measure::addAttribPrimPerimeter(geo, geoPrimGroup);
        //GA_ROHandleT<fpreal> p_h(pAttribPtr);
        //GA_Attribute* const pAttribPtr = GFE_Measure::addAttribPrimPerimeter(geo, geoPrimGroup);
        GA_ROHandleT<UT_Vector3T<value_type>> pos_h(geo->getP());

        UTparallelFor(groupParser.getPrimitiveSplittableRange(), [this, &uv_h, &pos_h, isPointAttrib](const GA_SplittableRange& r)
        {
            UT_Array<value_type> uvs;
            value_type dist = 0;
            //VECTOR_T uv(0.0);
            VECTOR_T uv(GFE_Type::getZeroVector<VECTOR_T>());
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
                    case GFE_CurveUVMethod::LocalAverage:
                        dist = 1.0 / (numvtx - 1);
                        for (GA_Size vtxpnum = 1; vtxpnum < numvtx; vtxpnum++)
                        {
                            vtxoff = geo->getPrimitiveVertexOffset(primoff, vtxpnum);
                            uv[0] += dist;
                            uv_h.set(isPointAttrib ? geo->vertexPoint(vtxoff) : vtxoff, 0, uv);
                        }
                        break;
                    case GFE_CurveUVMethod::LocalArcLength:
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
                    case GFE_CurveUVMethod::WorldAverage:
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
                    case GFE_CurveUVMethod::WorldArcLength:
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
                    case GFE_CurveUVMethod::LocalAverage:
                        success = GEO_Curve0->uniformTexture(uv_h, isPointAttrib);
                        break;
                    case GFE_CurveUVMethod::LocalArcLength:
                        success = GEO_Curve0->chordLenTexture(uv_h, isPointAttrib);
                        break;
                    case GFE_CurveUVMethod::WorldArcLength:
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
    GFE_CurveUVMethod curveUVMethod = GFE_CurveUVMethod::WorldArcLength;

private:

    GA_Attribute* uvAttrib;

    
    exint subscribeRatio = 64;
    exint minGrainSize = 64;


}; // End of Class GFE_CurveUV







#endif
