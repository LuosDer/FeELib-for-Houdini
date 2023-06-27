
#pragma once

#ifndef __GFE_Carve_h__
#define __GFE_Carve_h__

#include "GFE_DeleteVertex.h"
#include "GFE_Enumerate.h"
#include "GFE/GFE_Carve.h"

#include "GFE/GFE_GeoFilter.h"





#include "GFE/GFE_Attribute.h"
#include "GFE/GFE_CurveUV.h"
#include "GFE/GFE_Math.h"

enum class GFE_CarveSpace {
    CustomAttrib,
    LocalAverage,
    LocalArcLength,
    WorldAverage,
    WorldArcLength,
};


enum class GFE_CarveType {
    Start,
    End,
    Both,
};






class GFE_Carve : public GFE_AttribFilter {

public:
    //using GFE_AttribFilter::GFE_AttribFilter;

    GFE_Carve(
        GA_Detail& inGeo,
        //const GA_Detail* const geoSrc = nullptr,
        const SOP_NodeVerb::CookParms* const cookparms = nullptr
    )
        //: GFE_AttribFilter(inGeo, geoSrc, cookparms)
        : GFE_AttribFilter(inGeo, cookparms)
        , groupParser_carveStart(geo, groupParser.getGOPRef(), cookparms)
        , groupParser_carveEnd  (geo, groupParser.getGOPRef(), cookparms)
        , enumerate(geo, cookparms)
        //, interpAttribArray(geoSrc ? geoSrc : geo, cookparms)
        , interpAttribArray(geo, cookparms)
    {
    }

    SYS_FORCE_INLINE void setGroup(const GA_PrimitiveGroup* const geoPrimGroup = nullptr)
    { groupParser.setGroup(geoPrimGroup); }

    SYS_FORCE_INLINE void setGroup(const UT_StringRef& primGroupName = "")
    { groupParser.setGroup(GA_GROUP_PRIMITIVE, primGroupName);}

    SYS_FORCE_INLINE void setCarveStartGroup(const GA_PrimitiveGroup* const geoPrimGroup = nullptr)
    { groupParser_carveStart.setGroup(geoPrimGroup); }

    SYS_FORCE_INLINE void setCarveEndGroup(const GA_PrimitiveGroup* const geoPrimGroup = nullptr)
    { groupParser_carveEnd.setGroup(geoPrimGroup); }

    SYS_FORCE_INLINE void setCarveStartGroup(const UT_StringRef& primGroupName = "")
    { groupParser_carveStart.setGroup(GA_GROUP_PRIMITIVE, primGroupName); }

    SYS_FORCE_INLINE void setCarveEndGroup(const UT_StringRef& primGroupName = "")
    { groupParser_carveEnd.setGroup(GA_GROUP_PRIMITIVE, primGroupName); }

    void
        setComputeParm(
            const GFE_CarveSpace carveSpace = GFE_CarveSpace::WorldArcLength,
            const bool keepOutsideStart = false,
            const bool keepOutsideEnd = false,
            const bool keepInside = true,
            const bool absCarveUEnd = false,
            const exint subscribeRatio = 64,
            const exint minGrainSize = 1024
        )
    {
        setHasComputed();
        this->carveSpace = carveSpace;
        this->keepOutsideStart = keepOutsideStart;
        this->keepOutsideEnd = keepOutsideEnd;
        this->keepInside = keepInside;
        this->absCarveUEnd = absCarveUEnd;
        
        this->subscribeRatio = subscribeRatio;
        this->minGrainSize   = minGrainSize;
    }

    template<bool IsEnd>
    void setCarveU(const fpreal carveULocal, const fpreal carveUWorld)
    {
        const bool IsLocal = carveSpace == GFE_CarveSpace::LocalAverage || carveSpace == GFE_CarveSpace::LocalArcLength;
        const fpreal carveU = IsLocal ? carveULocal : carveUWorld;
        if constexpr(IsEnd)
            carveUEnd = carveU;
        else
            carveUStart = carveU;
    }
    
    void setUVAttrib(const UT_StringRef& customCarveUVAttribName = "uv")
    {
        setHasComputed();
        if (carveSpace == GFE_CarveSpace::CustomAttrib)
            uvAttrib = GFE_Attribute::findUVAttributePointVertex(*geo, GA_ATTRIB_INVALID, customCarveUVAttribName);
        else
            uvAttrib = nullptr;
    }

    
    void setCarveUStartAttrib(const GA_Attribute* const carveUAttrib, const bool delCarveUAttrib)
    {
        carveUAttribStart    = carveUAttrib;
        delCarveUAttribStart = delCarveUAttrib;
        if (carveUAttribStart)
            startCarveU_h = carveUAttribStart;

        UT_ASSERT_MSG(carveUAttribStart->getOwner() >= 3, "not correct attrib owner");
    }

    SYS_FORCE_INLINE void setCarveUStartAttrib(GA_Attribute* const carveUAttrib, const bool delCarveUAttrib)
    {
        carveUAttribStart_nonConst = carveUAttrib;
        setCarveUStartAttrib(static_cast<const GA_Attribute*>(carveUAttrib), delCarveUAttrib);
    }

    void setCarveUStartAttrib(const UT_StringRef& carveUAttribName = "carveUStart", const bool delCarveUAttrib = false)
    {
        carveUAttribStart_nonConst = geo->findPrimitiveAttribute(carveUAttribName);
        if (!carveUAttribStart_nonConst)
        {
            carveUAttribStart_nonConst = geo->findPointAttribute(carveUAttribName);
            if (!carveUAttribStart_nonConst)
            {
                carveUAttribStart_nonConst = geo->findVertexAttribute(carveUAttribName);
            }
        }
        if (carveUAttribStart_nonConst)
        {
            setCarveUStartAttrib(carveUAttribStart_nonConst, delCarveUAttrib);
        }
        else
        {
            delCarveUAttribStart = delCarveUAttrib;
        }
    }



    
    void setCarveUEndAttrib(const GA_Attribute* const carveUAttrib, const bool delCarveUAttrib)
    {
        endCarveUAttrib = carveUAttrib;
        delEndCarveUAttrib = delCarveUAttrib;
        if (endCarveUAttrib)
        {
            endCarveU_h.bind(endCarveUAttrib);
        }

        UT_ASSERT_MSG(endCarveUAttrib->getOwner() >= 3, "not correct attrib owner");
    }

    SYS_FORCE_INLINE void setCarveUEndAttrib(GA_Attribute* const carveUAttrib, const bool delCarveUAttrib)
    {
        endCarveUAttrib_nonConst = carveUAttrib;
        setCarveUEndAttrib(static_cast<const GA_Attribute*>(carveUAttrib), delCarveUAttrib);
    }

    
    void setCarveUEndAttrib(const UT_StringRef& carveUAttribName = "carveUEnd", const bool delCarveUAttrib = false)
    {
        endCarveUAttrib_nonConst = geo->findPrimitiveAttribute(carveUAttribName);
        if (!endCarveUAttrib_nonConst)
        {
            endCarveUAttrib_nonConst = geo->findPointAttribute(carveUAttribName);
            if (!endCarveUAttrib_nonConst)
            {
                endCarveUAttrib_nonConst = geo->findVertexAttribute(carveUAttribName);
            }
        }
        if (endCarveUAttrib_nonConst)
        {
            setCarveUEndAttrib(endCarveUAttrib_nonConst, delCarveUAttrib);
        }
        else
        {
            delEndCarveUAttrib = delCarveUAttrib;
        }
    }


private:

virtual bool
    computeCore() override
{
    if (!(keepOutsideStart || keepOutsideEnd || keepInside))
        return true;
    
    if (groupParser.isEmpty())
        return true;
    
    if (!posAttrib)
        posAttrib = geo->getP();

    //interpAttribArray.uappend();
    
    enumerate.compute();
    
    GFE_CurveUV curveUV(geo, cookparms);
    curveUV.setPositionAttrib(posAttrib);
    if (carveSpace != GFE_CarveSpace::CustomAttrib)
    {
        switch (carveSpace)
        {
        case GFE_CarveSpace::LocalAverage:   curveUV.curveUVMethod = GFE_CurveUVMethod::LocalAverage;   break;
        case GFE_CarveSpace::LocalArcLength: curveUV.curveUVMethod = GFE_CurveUVMethod::LocalArcLength; break;
        case GFE_CarveSpace::WorldAverage:   curveUV.curveUVMethod = GFE_CurveUVMethod::WorldAverage;   break;
        case GFE_CarveSpace::WorldArcLength:
        default:                             curveUV.curveUVMethod = GFE_CurveUVMethod::WorldArcLength; break;
        }
        curveUV.groupParser.setGroup(groupParser.getPrimitiveGroup());
        uvAttrib = curveUV.getOutAttribArray().findOrCreateUV(true, GA_ATTRIB_VERTEX, GA_STORE_INVALID);
        curveUV.compute();
    }
    if (carveSpace != GFE_CarveSpace::LocalAverage && !uvAttrib)
        return false;
    UT_ASSERT_MSG(!uvAttrib || uvAttrib->getOwner() == GA_ATTRIB_POINT || uvAttrib->getOwner() == GA_ATTRIB_VERTEX, "Unhandled UV Attrib Owner");

    carveSpace_isWorld = carveSpace == GFE_CarveSpace::CustomAttrib   ||
                         carveSpace == GFE_CarveSpace::WorldAverage   ||
                         carveSpace == GFE_CarveSpace::WorldArcLength;

    if (carveSpace != GFE_CarveSpace::LocalAverage)
        uvAIF = uvAttrib->getAIFTuple();
    isPointUV = uvAttrib && uvAttrib->getOwner() == GA_ATTRIB_POINT;
    //uvTupleSize = uvAttrib->getTupleSize();
    //uv_h = uvAttrib;


    
    carve();

    if (delCarveUAttribStart && carveUAttribStart_nonConst)
        geo->getAttributes().destroyAttribute(carveUAttribStart_nonConst);
    
    if (delEndCarveUAttrib && endCarveUAttrib_nonConst)
        geo->getAttributes().destroyAttribute(endCarveUAttrib_nonConst);
    
    return true;
}

    
SYS_FORCE_INLINE GA_Offset primPoint(const GA_Detail* const geo, const GA_Offset primoff, const GA_Offset vtxpnum)
{ return geo->vertexPoint(geo->getPrimitiveVertexOffset(primoff, vtxpnum)); }

template<GFE_CarveType CARVE_TYPE, bool isEnd>
void carveSingle(
    GA_Detail* const geo,
    const GA_Offset primoff,
    fpreal uv,
    const GA_Size numvtx,
    const fpreal domainUStart,
    const fpreal domainUEnd,
    const fpreal uvMax,
    const fpreal step
)
{
    fpreal uv_rest = 0;
    
    if constexpr(!isEnd && CARVE_TYPE != GFE_CarveType::End)
    {
        if (carveStartPrimGroup && carveStartPrimGroup->contains(primoff))
            return;
    }
    else if constexpr(isEnd && CARVE_TYPE != GFE_CarveType::Start)
    {
        if (carveEndPrimGroup && carveEndPrimGroup->contains(primoff))
            return;
    }
        
    if (carveSpace == GFE_CarveSpace::LocalAverage)
        uv = isEnd;
        
#if 1
    if constexpr(isEnd)
    {
        for (GA_Size vtxpnum = numvtx-1; vtxpnum >= 0; --vtxpnum)
        {
            //flag = constexpr(isEnd) ? vtxpnum >= 0 : vtxpnum < numvtx;
            GA_Offset vtxoff = geo->getPrimitiveVertexOffset(primoff, vtxpnum);
            GA_Offset ptoff  = geo->vertexPoint(vtxoff);

            if (carveSpace != GFE_CarveSpace::LocalAverage)
                uvAIF->get(uvAttrib, isPointUV ? ptoff : vtxoff, uv);

            if (uv <= domainUEnd)
            {
                const GA_Size breakVtxpnum = vtxpnum + (uv != domainUEnd);
                if constexpr(CARVE_TYPE == GFE_CarveType::Both)
                {
                    //for (GA_Size vtxpnum_prev = numvtx-1; vtxpnum_prev > breakVtxpnum; --vtxpnum_prev)
                    for (GA_Size vtxpnum_prev = breakVtxpnum+1; vtxpnum_prev < numvtx; ++vtxpnum_prev)
                        delVertexGroup->setElement(geo->getPrimitiveVertexOffset(primoff, vtxpnum_prev), true);
                }
                else
                {
                    //for (GA_Size vtxpnum_prev = breakVtxpnum-1; vtxpnum_prev >= 0; --vtxpnum_prev)
                    for (GA_Size vtxpnum_prev = 0; vtxpnum_prev < breakVtxpnum; ++vtxpnum_prev)
                        delVertexGroup->setElement(geo->getPrimitiveVertexOffset(primoff, vtxpnum_prev), true);
                }

                if (vtxpnum == numvtx-1 && uv == domainUEnd)
                    break;
                
                if constexpr(CARVE_TYPE == GFE_CarveType::Both)
                {
                    if (hasCarveEndPrimGroup)
                        hasCarveEndPrimGroup->setElement(primoff, true);
                }
                else
                {
                    if (hasCarveStartPrimGroup)
                        hasCarveStartPrimGroup->setElement(primoff, true);
                }
                
                if (uv == domainUEnd)
                    break;

                const GA_Offset ptoff0 = primPoint(geo, primoff, vtxpnum);
                const GA_Offset ptoff1 = primPoint(geo, primoff, breakVtxpnum);
                if constexpr(CARVE_TYPE == GFE_CarveType::Both)
                    interpAttribs(geo, ptoff1, ptoff0,       (domainUEnd - uv_rest) / (uv - uv_rest));
                else
                    interpAttribs(geo, ptoff0, ptoff1, 1.0 - (domainUEnd - uv_rest) / (uv - uv_rest));
                break;
            }

            uv_rest = uv;
            if (carveSpace == GFE_CarveSpace::LocalAverage)
                uv -= step;
        }
    }
    else
    {
        for (GA_Size vtxpnum = 0; vtxpnum < numvtx; ++vtxpnum)
        {
            //flag = constexpr(isEnd) ? vtxpnum >= 0 : vtxpnum < numvtx;
            GA_Offset vtxoff = geo->getPrimitiveVertexOffset(primoff, vtxpnum);
            GA_Offset ptoff  = geo->vertexPoint(vtxoff);

            if (carveSpace != GFE_CarveSpace::LocalAverage)
                uvAIF->get(uvAttrib, isPointUV ? ptoff : vtxoff, uv);

            if (uv >= domainUStart)
            {
                const GA_Size breakVtxpnum = vtxpnum - (uv != domainUStart);
                if constexpr(CARVE_TYPE == GFE_CarveType::Both)
                {
                    for (GA_Size vtxpnum_prev = 0; vtxpnum_prev < breakVtxpnum; ++vtxpnum_prev)
                        delVertexGroup->setElement(geo->getPrimitiveVertexOffset(primoff, vtxpnum_prev), true);
                }
                else
                {
                    for (GA_Size vtxpnum_prev = breakVtxpnum+2; vtxpnum_prev < numvtx; ++vtxpnum_prev)
                        delVertexGroup->setElement(geo->getPrimitiveVertexOffset(primoff, vtxpnum_prev), true);
                }

                if (vtxpnum == 0 && uv == domainUStart)
                    break;
                
                if constexpr(CARVE_TYPE == GFE_CarveType::Both)
                {
                    if (hasCarveStartPrimGroup)
                        hasCarveStartPrimGroup->setElement(primoff, true);
                }
                else
                {
                    if (hasCarveEndPrimGroup)
                        hasCarveEndPrimGroup->setElement(primoff, true);
                }
                
                if (uv == domainUStart)
                    break;
                
                const GA_Offset ptoff0 = primPoint(geo, primoff, vtxpnum);
                const GA_Offset ptoff1 = primPoint(geo, primoff, breakVtxpnum);
                if constexpr(CARVE_TYPE == GFE_CarveType::Both)
                    interpAttribs(geo, ptoff1, ptoff0,     (domainUStart - uv_rest) / (uv - uv_rest));
                else
                    interpAttribs(geo, ptoff0, ptoff1, 1 - (domainUStart - uv_rest) / (uv - uv_rest));
                break;
            }
            uv_rest = uv;
            if (carveSpace == GFE_CarveSpace::LocalAverage)
                uv += step;
        }
    }
#else
    
    GA_Size vtxpnum;
    bool flag;
    if constexpr(isEnd)
    {
        vtxpnum = numvtx-1;
        flag = vtxpnum >= 0;
    }
    else
    {
        ++vtxpnum;
        flag = vtxpnum < numvtx;
    }
    while(flag)
    {
        //flag = constexpr(isEnd) ? vtxpnum >= 0 : vtxpnum < numvtx;
        GA_Offset vtxoff = geo->getPrimitiveVertexOffset(primoff, vtxpnum);
        GA_Offset ptoff  = geo->vertexPoint(vtxoff);

        if (carveSpace != GFE_CarveSpace::LocalAverage)
            uvAIF->get(uvAttrib, isPointUV ? ptoff : vtxoff, uv);

        if constexpr(isEnd)
        {
            if (uv <= domainUEnd)
            {
                const GA_Size breakVtxpnum = vtxpnum + (uv != domainUEnd);
                if constexpr(CARVE_TYPE == GFE_CarveType::Both)
                {
                    //for (GA_Size vtxpnum_prev = numvtx-1; vtxpnum_prev > breakVtxpnum; --vtxpnum_prev)
                    for (GA_Size vtxpnum_prev = breakVtxpnum+1; vtxpnum_prev < numvtx; ++vtxpnum_prev)
                        delVertexGroup->setElement(geo->getPrimitiveVertexOffset(primoff, vtxpnum_prev), true);
                }
                else
                {
                    //for (GA_Size vtxpnum_prev = breakVtxpnum-1; vtxpnum_prev >= 0; --vtxpnum_prev)
                    for (GA_Size vtxpnum_prev = 0; vtxpnum_prev < breakVtxpnum; ++vtxpnum_prev)
                        delVertexGroup->setElement(geo->getPrimitiveVertexOffset(primoff, vtxpnum_prev), true);
                }

                if (vtxpnum == numvtx-1 && uv == domainUEnd)
                    break;
                
                if constexpr(CARVE_TYPE == GFE_CarveType::Both)
                {
                    if (hasCarveEndPrimGroup)
                        hasCarveEndPrimGroup->setElement(primoff, true);
                }
                else
                {
                    if (hasCarveStartPrimGroup)
                        hasCarveStartPrimGroup->setElement(primoff, true);
                }
                
                if (uv == domainUEnd)
                    break;

                const GA_Offset ptoff0 = primPoint(geo, primoff, vtxpnum);
                const GA_Offset ptoff1 = primPoint(geo, primoff, breakVtxpnum);
                if constexpr(CARVE_TYPE == GFE_CarveType::Both)
                    interpAttribs(geo, ptoff1, ptoff0,       (domainUEnd - uv_rest) / (uv - uv_rest));
                else
                    interpAttribs(geo, ptoff0, ptoff1, 1.0 - (domainUEnd - uv_rest) / (uv - uv_rest));
                break;
            }
        }
        else
        {
            if (uv >= domainUStart)
            {
                const GA_Size breakVtxpnum = vtxpnum - (uv != domainUStart);
                if constexpr(CARVE_TYPE == GFE_CarveType::Both)
                {
                    for (GA_Size vtxpnum_prev = 0; vtxpnum_prev < breakVtxpnum; ++vtxpnum_prev)
                        delVertexGroup->setElement(geo->getPrimitiveVertexOffset(primoff, vtxpnum_prev), true);
                }
                else
                {
                    for (GA_Size vtxpnum_prev = breakVtxpnum+2; vtxpnum_prev < numvtx; ++vtxpnum_prev)
                        delVertexGroup->setElement(geo->getPrimitiveVertexOffset(primoff, vtxpnum_prev), true);
                }

                if (vtxpnum == 0 && uv == domainUStart)
                    break;
                
                if constexpr(CARVE_TYPE == GFE_CarveType::Both)
                {
                    if (hasCarveStartPrimGroup)
                        hasCarveStartPrimGroup->setElement(primoff, true);
                }
                else
                {
                    if (hasCarveEndPrimGroup)
                        hasCarveEndPrimGroup->setElement(primoff, true);
                }
                
                if (uv == domainUStart)
                    break;
                
                const GA_Offset ptoff0 = primPoint(geo, primoff, vtxpnum);
                const GA_Offset ptoff1 = primPoint(geo, primoff, breakVtxpnum);
                if constexpr(CARVE_TYPE == GFE_CarveType::Both)
                    interpAttribs(geo, ptoff1, ptoff0,     (domainUStart - uv_rest) / (uv - uv_rest));
                else
                    interpAttribs(geo, ptoff0, ptoff1, 1 - (domainUStart - uv_rest) / (uv - uv_rest));
                break;
            }
        }

        uv_rest = uv;
        if (carveSpace == GFE_CarveSpace::LocalAverage)
        {
            if constexpr(isEnd)
                uv -= step;
            else
                uv += step;
        }
        
        if constexpr(isEnd)
        {
            --vtxpnum;
            flag = vtxpnum >= 0;
        }
        else
        {
            ++vtxpnum;
            flag = vtxpnum < numvtx;
        }
    }
#endif
}


template<GFE_CarveType CARVE_TYPE>
void carve(GA_Detail* const geo)
{
    GA_PrimitiveGroupUPtr delPrimGroupUPtr   = geo->createDetachedPrimitiveGroup();
    //GA_PointGroupUPtr     delPointGroupUPtr  = geo->createDetachedPointGroup();
    GA_VertexGroupUPtr    delVertexGroupUPtr = geo->createDetachedVertexGroup();
    delPrimGroup   = delPrimGroupUPtr.get();
    //delPointGroup  = delPointGroupUPtr.get();
    delVertexGroup = delVertexGroupUPtr.get();
    
    carveStartPrimGroup = groupParser_carveStart.getPrimitiveGroup();
    carveEndPrimGroup   = groupParser_carveEnd.getPrimitiveGroup();

    UTparallelFor(groupParser.getPrimitiveSplittableRange(), [this, geo](const GA_SplittableRange& r)
    {
        fpreal domainUStart = 0, domainUEnd = 0, uvMax = 0;
        GA_Offset primvtx0 = 0, primpt0 = 0, primvtx1 = 0, primpt1 = 0;
        GA_Offset start, end;
        for (GA_Iterator it(r); it.blockAdvance(start, end); )
        {
            for (GA_Offset primoff = start; primoff < end; ++primoff)
            {
                if constexpr(CARVE_TYPE == GFE_CarveType::Start)
                {
                    if (carveStartPrimGroup && !carveStartPrimGroup->contains(primoff))
                    {
                        delPrimGroup->setElement(primoff, true);
                        continue;
                    }
                }
                if constexpr(CARVE_TYPE == GFE_CarveType::End)
                {
                    if (carveEndPrimGroup && !carveEndPrimGroup->contains(primoff))
                    {
                        delPrimGroup->setElement(primoff, true);
                        continue;
                    }
                }

                const GA_Size numvtx = geo->getPrimitiveVertexCount(primoff);
                const GA_Size lastVtxpnum = numvtx-1;
                
                if constexpr(CARVE_TYPE != GFE_CarveType::End)
                {
                    primvtx0 = geo->getPrimitiveVertexOffset(primoff, 0);
                    primpt0  = geo->vertexPoint(primvtx0);
                }
                if constexpr(CARVE_TYPE != GFE_CarveType::Start)
                {
                    primvtx1 = geo->getPrimitiveVertexOffset(primoff, numvtx-1);
                    primpt1  = geo->vertexPoint(primvtx1);
                }
                
                if constexpr(CARVE_TYPE != GFE_CarveType::End)
                {
                    if (carveUAttribStart)
                    {
                        switch (carveUAttribStart->getOwner())
                        {
                        case GA_ATTRIB_PRIMITIVE: domainUStart = startCarveU_h.get(primoff);  break;
                        case GA_ATTRIB_POINT:     domainUStart = startCarveU_h.get(primpt0);  break;
                        case GA_ATTRIB_VERTEX:    domainUStart = startCarveU_h.get(primvtx0); break;
                        default: UT_ASSERT_MSG(0, "unhandled carveU Attrib class");           break;
                        }
                    }
                    else
                    {
                        domainUStart = carveUStart;
                    }

                    if constexpr(CARVE_TYPE == GFE_CarveType::Both)
                    {
                        if (carveSpace_isWorld)
                        {
                            switch (uvAttrib->getOwner())
                            {
                            case GA_ATTRIB_POINT:  uvMax = uv_h.get(primpt1);  break;
                            case GA_ATTRIB_VERTEX: uvMax = uv_h.get(primvtx1); break;
                            default: UT_ASSERT_MSG(0, "unhandled uv class");   break;
                            }

                            if (carveStartPrimGroup && carveStartPrimGroup->contains(primoff) && domainUStart >= uvMax)
                            {
                                delPrimGroup->setElement(primoff, true);
                                continue;
                            }
                        }
                        else
                        {
                            uvMax = 1;
                            if (carveStartPrimGroup && carveStartPrimGroup->contains(primoff) && domainUStart >= 1)
                            {
                                delPrimGroup->setElement(primoff, true);
                                continue;
                            }
                        }
                    }
                    else if (domainUStart <= 0)
                    {
                        delPrimGroup->setElement(primoff, true);
                        continue;
                    }
                }

                if constexpr(CARVE_TYPE != GFE_CarveType::Start)
                {
                    if (endCarveUAttrib)
                    {
                        switch (endCarveUAttrib->getOwner())
                        {
                        case GA_ATTRIB_PRIMITIVE: domainUEnd = endCarveU_h.get(primoff);  break;
                        case GA_ATTRIB_POINT:     domainUEnd = endCarveU_h.get(primpt0);  break;
                        case GA_ATTRIB_VERTEX:    domainUEnd = endCarveU_h.get(primvtx0); break;
                        default: break;
                        }
                    }
                    else
                    {
                        domainUEnd = carveUEnd;
                    }
                    
                    // cout(domainUEnd);
                    if (!absCarveUEnd)
                    {
                        if constexpr(CARVE_TYPE != GFE_CarveType::Both)
                        {
                            // if ( domainUEnd <= domainUStart ) {
                            if (domainUEnd <= 0)
                            {
                                delPrimGroup->setElement(primoff, true);
                                continue;
                            }
                        }
                        if (carveSpace_isWorld)
                        {
                            fpreal uvmax2;
                            switch (uvAttrib->getOwner())
                            {
                            case GA_ATTRIB_POINT:  uvmax2 = uv_h.get(primpt1);  break;
                            case GA_ATTRIB_VERTEX: uvmax2 = uv_h.get(primvtx1); break;
                            default: UT_ASSERT_MSG(0, "unhandled uv class");    break;
                            }
                            domainUEnd = uvmax2 - domainUEnd;
                        }
                        else
                        {
                            domainUEnd = 1.0 - domainUEnd;
                        }
                    }
                    // cout(domainUEnd);
                    if constexpr(CARVE_TYPE == GFE_CarveType::Both)
                    {
                        // if ( inpointgroup(0, chs("../secondptgroup"), primpt1) && domainUEnd <= domainUStart ) {
                        if (carveEndPrimGroup && carveEndPrimGroup->contains(primoff) && domainUEnd <= 0)
                        {
                            delPrimGroup->setElement(primoff, true);
                            continue;
                        }
                    }
                }
                
                if constexpr(CARVE_TYPE == GFE_CarveType::Both)
                {
                    if (delReversedInsidePrim && domainUEnd < domainUStart)
                    {
                        delPrimGroup->setElement(primoff, true);
                        continue;
                    }
                }

                fpreal uv_rest = 0;

                fpreal uv;
                fpreal step;
                if (carveSpace == GFE_CarveSpace::LocalAverage)
                {
                    uv = static_cast<fpreal>(CARVE_TYPE);
                    step = 1.0 / lastVtxpnum;
                }
                
                if constexpr(CARVE_TYPE != GFE_CarveType::End)
                    carveSingle<CARVE_TYPE, 0>(geo, primoff, uv, numvtx, domainUStart, domainUEnd, uvMax, step);
                
                if constexpr(CARVE_TYPE != GFE_CarveType::Start)
                    carveSingle<CARVE_TYPE, 1>(geo, primoff, uv, numvtx, domainUStart, domainUEnd, uvMax, step);
            }
        }
    }, subscribeRatio, minGrainSize);
                    
    delPrimGroup->invalidateGroupEntries();
    //delPointGroup->invalidateGroupEntries();
    delVertexGroup->invalidateGroupEntries();
                    
    geo->destroyPrimitiveOffsets(geo->getPrimitiveRange(delPrimGroup), true);
    //geo->destroyPointOffsets(geo->getPointRange(delPointGroup));
                    
    GFE_DelVertex delVertex(geo, cookparms);
    delVertex.groupParser.setGroup(delVertexGroup);
    delVertex.compute();
}


void carve()
{
    if ((keepOutsideStart && keepOutsideEnd) || ((keepOutsideStart || keepOutsideEnd) && keepInside))
    {
        GU_DetailHandle geoOrigin_h;
        geoOrigin = new GU_Detail();
        geoOrigin_h.allocateAndSet(geoOrigin);
        geoOrigin->replaceWith(geoSrc ? *geoSrc : *geo);
        
        GU_DetailHandle geoTmp_h;
        GU_Detail* const geoTmp = new GU_Detail();
        geoTmp_h.allocateAndSet(geoTmp);
        if (keepOutsideStart)
        {
            geoTmp->replaceWith(*geoOrigin);
            carve<GFE_CarveType::Start>(geoTmp);
            geo->baseMerge(*geoTmp);
        }
        if (keepOutsideEnd)
        {
            geoTmp->replaceWith(*geoOrigin);
            carve<GFE_CarveType::End>(geoTmp);
            geo->baseMerge(*geoTmp);
        }
        if (keepInside)
        {
            geoTmp->replaceWith(*geoOrigin);
            carve<GFE_CarveType::Both>(geoTmp);
            geo->baseMerge(*geoTmp);
        }
    }
    else
    {
        if (geoSrc)
            geo->replaceWith(*geoSrc);
        if (keepOutsideStart)
            carve<GFE_CarveType::Start>(geo);
        else if (keepOutsideEnd)
            carve<GFE_CarveType::End>(geo);
        else if (keepInside)
            carve<GFE_CarveType::Both>(geo);
    }
}



private:
            
    template<typename T>
    void
        setAttribVal(
            const GA_RWHandleT<T>& attrib_h,
            const GA_Offset ptoff0,
            const GA_Offset ptoff1,
            const T bias
        )
    {
        T attrib0 = attrib_h.get(ptoff0);
        const T attrib1 = attrib_h.get(ptoff1);
        //attrib0 = GFE_Math::vlerp(attrib0, attrib1, bias);
        attrib0 = attrib0 + (attrib1 - attrib0) * bias;
        attrib_h.set(ptoff0, attrib0);
    }
    
    template<typename VECTOR_T>
    void
        setAttribVector(
            const GA_RWHandleT<VECTOR_T>& attrib_h,
            const GA_Offset ptoff0,
            const GA_Offset ptoff1,
            const typename VECTOR_T::value_type bias
        )
    {
        VECTOR_T attrib0 = attrib_h.get(ptoff0);
        const VECTOR_T& attrib1 = attrib_h.get(ptoff1);
        //attrib0 = GFE_Math::vlerp(attrib0, attrib1, bias);
        attrib0 = attrib0 + (attrib1 - attrib0) * bias;
        attrib_h.set(ptoff0, attrib0);
    }
    
    //template<typename T>
    //void
    //    setAttribValInt(
    //        const GA_RWHandleT<T>& attrib_h,
    //        const GA_Offset ptoff0,
    //        const GA_Offset ptoff1,
    //        const T bias
    //    )
    //{
    //    T attrib0 = attrib_h.get(ptoff0);
    //    const T attrib1 = attrib_h.get(ptoff1);
    //    attrib0 = attrib0 + (attrib1 - attrib0) * bias;
    //    attrib_h.set(ptoff0, attrib0);
    //}
    
    template<typename FLOAT_T>
    void
        setAttribDir(
            const GA_RWHandleT<UT_Vector3T<FLOAT_T>>& attrib_h,
            const GA_Offset ptoff0,
            const GA_Offset ptoff1,
            const fpreal bias
        )
    {
        UT_Vector3T<FLOAT_T> attrib0 = attrib_h.get(ptoff0);
        UT_Vector3T<FLOAT_T> attrib1 = attrib_h.get(ptoff1);
        //aifTuple.set();
        FLOAT_T length0 = length2(attrib0);
        const FLOAT_T length1 = length2(attrib1);
        length0 = sqrt(SYSlerp(length0, length1, bias));

        attrib0.normalize();
        attrib1.normalize();

        UT_QuaternionT<fpreal> q;
        q.updateFromVectors(attrib0, attrib1);
        q = UT_QuaternionT<fpreal>(0, 0, 0, 1).interpolate(q, bias);

        attrib0 = q.rotate(attrib0);
        attrib0 *= length0;

        attrib_h.set(ptoff0, attrib0);
    }


    void interpAttribs(GA_Detail* const geo, const GA_Offset ptoff0, const GA_Offset ptoff1, const fpreal bias)
    {
        const size_t size = interpAttribArray.size();
        for (size_t i = 0; i < size; i++)
        {
            const GA_Attribute* const attribSrc = interpAttribArray[i];
            
            UT_ASSERT_MSG(!attribSrc->isDetached(), "unsupport detached attrib");
            if (attribSrc->isDetached())
                continue;
            
            GA_Attribute* const attrib = geo->findAttribute(attribSrc->getOwner(), attribSrc->getName());

            const GA_AIFTuple* const aifTuple = attrib->getAIFTuple();
            
            if (aifTuple)
            {
                const int tupleSize = aifTuple->getTupleSize(attrib);
                const GA_Storage storage = aifTuple->getStorage(attrib);
                switch (tupleSize)
                {
                case 1:
                    switch (storage)
                    {
                        //case GA_STORE_BOOL: setAttribVal<bool    >(attrib, ptoff0, ptoff1, bias); break;
                        case GA_STORE_UINT8:  setAttribVal<uint8   >(attrib, ptoff0, ptoff1, bias); break;
                        case GA_STORE_INT8:   setAttribVal<int8    >(attrib, ptoff0, ptoff1, bias); break;
                        case GA_STORE_INT16:  setAttribVal<int16   >(attrib, ptoff0, ptoff1, bias); break;
                        case GA_STORE_INT32:  setAttribVal<int32   >(attrib, ptoff0, ptoff1, bias); break;
                        case GA_STORE_INT64:  setAttribVal<int64   >(attrib, ptoff0, ptoff1, bias); break;
                        case GA_STORE_REAL16: setAttribVal<fpreal16>(attrib, ptoff0, ptoff1, bias); break;
                        case GA_STORE_REAL32: setAttribVal<fpreal32>(attrib, ptoff0, ptoff1, bias); break;
                        case GA_STORE_REAL64: setAttribVal<fpreal64>(attrib, ptoff0, ptoff1, bias); break;
                        case GA_STORE_STRING:  break;
                        case GA_STORE_DICT:    break;
                        case GA_STORE_INVALID: break;
                        default: break;
                    }
                    break;
                case 2:
                    switch (storage)
                    {
                        //case GA_STORE_BOOL:  setAttribVector<UT_Vector2T<bool    >>(attrib, ptoff0, ptoff1, bias); break;
                        //case GA_STORE_UINT8: setAttribVector<UT_Vector2T<uint8   >>(attrib, ptoff0, ptoff1, bias); break;
                        //case GA_STORE_INT8:  setAttribVector<UT_Vector2T<int8    >>(attrib, ptoff0, ptoff1, bias); break;
                        //case GA_STORE_INT16: setAttribVector<UT_Vector2T<int16   >>(attrib, ptoff0, ptoff1, bias); break;
                        case GA_STORE_INT32:   setAttribVector<UT_Vector2T<int32   >>(attrib, ptoff0, ptoff1, bias); break;
                        case GA_STORE_INT64:   setAttribVector<UT_Vector2T<int64   >>(attrib, ptoff0, ptoff1, bias); break;
                        case GA_STORE_REAL16:  setAttribVector<UT_Vector2T<fpreal16>>(attrib, ptoff0, ptoff1, bias); break;
                        case GA_STORE_REAL32:  setAttribVector<UT_Vector2T<fpreal32>>(attrib, ptoff0, ptoff1, bias); break;
                        case GA_STORE_REAL64:  setAttribVector<UT_Vector2T<fpreal64>>(attrib, ptoff0, ptoff1, bias); break;
                        case GA_STORE_STRING:  break;
                        case GA_STORE_DICT:    break;
                        case GA_STORE_INVALID: break;
                        default: break;
                    }
                    break;
                case 3:
                    if (attrib->getTypeInfo() == GA_TYPE_VECTOR || attrib->getTypeInfo() == GA_TYPE_NORMAL)
                    {
                        switch (storage)
                        {
                        case GA_STORE_BOOL:     break;
                        case GA_STORE_UINT8:    break;
                        case GA_STORE_INT8:     break;
                        case GA_STORE_INT16:    break;
                        case GA_STORE_INT32:    break;
                        case GA_STORE_INT64:    break;
                        case GA_STORE_REAL16:  setAttribDir<fpreal16>(attrib, ptoff0, ptoff1, bias); break;
                        case GA_STORE_REAL32:  setAttribDir<fpreal32>(attrib, ptoff0, ptoff1, bias); break;
                        case GA_STORE_REAL64:  setAttribDir<fpreal64>(attrib, ptoff0, ptoff1, bias); break;
                        case GA_STORE_STRING:   break;
                        case GA_STORE_DICT:     break;
                        case GA_STORE_INVALID:  break;
                        default:                break;
                        }
                        continue;
                    }
                    else
                    {
                        switch (storage)
                        {
                            //case GA_STORE_BOOL:  setAttribVector<UT_Vector3T<bool    >>(attrib, ptoff0, ptoff1, bias); break;
                            //case GA_STORE_UINT8: setAttribVector<UT_Vector3T<uint8   >>(attrib, ptoff0, ptoff1, bias); break;
                            //case GA_STORE_INT8:  setAttribVector<UT_Vector3T<int8    >>(attrib, ptoff0, ptoff1, bias); break;
                            //case GA_STORE_INT16: setAttribVector<UT_Vector3T<int16   >>(attrib, ptoff0, ptoff1, bias); break;
                            case GA_STORE_INT32:   setAttribVector<UT_Vector3T<int32   >>(attrib, ptoff0, ptoff1, bias); break;
                            case GA_STORE_INT64:   setAttribVector<UT_Vector3T<int64   >>(attrib, ptoff0, ptoff1, bias); break;
                            case GA_STORE_REAL16:  setAttribVector<UT_Vector3T<fpreal16>>(attrib, ptoff0, ptoff1, bias); break;
                            case GA_STORE_REAL32:  setAttribVector<UT_Vector3T<fpreal32>>(attrib, ptoff0, ptoff1, bias); break;
                            case GA_STORE_REAL64:  setAttribVector<UT_Vector3T<fpreal64>>(attrib, ptoff0, ptoff1, bias); break;
                            case GA_STORE_STRING:  break;
                            case GA_STORE_DICT:    break;
                            case GA_STORE_INVALID: break;
                            default: break;
                        }
                        
                    }
                    break;
                    
                case 4:
                    switch (storage)
                    {
                        //case GA_STORE_BOOL:  setAttribVector<UT_Vector4T<bool    >(attrib, ptoff0, ptoff1, bias); break;
                        //case GA_STORE_UINT8: setAttribVector<UT_Vector4T<uint8   >(attrib, ptoff0, ptoff1, bias); break;
                        //case GA_STORE_INT8:  setAttribVector<UT_Vector4T<int8    >>(attrib, ptoff0, ptoff1, bias); break;
                        //case GA_STORE_INT16: setAttribVector<UT_Vector4T<int16   >>(attrib, ptoff0, ptoff1, bias); break;
                        case GA_STORE_INT32:   setAttribVector<UT_Vector4T<int32   >>(attrib, ptoff0, ptoff1, bias); break;
                        case GA_STORE_INT64:   setAttribVector<UT_Vector4T<int64   >>(attrib, ptoff0, ptoff1, bias); break;
                        case GA_STORE_REAL16:  setAttribVector<UT_Vector4T<fpreal16>>(attrib, ptoff0, ptoff1, bias); break;
                        case GA_STORE_REAL32:  setAttribVector<UT_Vector4T<fpreal32>>(attrib, ptoff0, ptoff1, bias); break;
                        case GA_STORE_REAL64:  setAttribVector<UT_Vector4T<fpreal64>>(attrib, ptoff0, ptoff1, bias); break;
                        case GA_STORE_STRING:  break;
                        case GA_STORE_DICT:    break;
                        case GA_STORE_INVALID: break;
                        default: break;
                    }
                    break;
                }
            }
            else if (attrib->getAIFNumericArray())
            {
            }
            else
            {
            }
        }
    }



public:
    GFE_GroupParser groupParser_carveStart;
    GFE_GroupParser groupParser_carveEnd;
    
    GFE_Enumerate enumerate;
    GFE_RefAttributeArray interpAttribArray;

    GFE_CarveSpace carveSpace = GFE_CarveSpace::WorldArcLength;
    bool keepOutsideStart = false;
    bool keepOutsideEnd   = false;
    bool keepInside       = true;

    fpreal carveUStart = 0.25;
    fpreal carveUEnd   = 0.25;

    bool absCarveUEnd = false;
    bool delReversedInsidePrim = false;

    bool delCarveUAttribStart = false;
    bool delEndCarveUAttrib   = false;



private:
    GA_PrimitiveGroup* hasCarveStartPrimGroup = nullptr;
    GA_PrimitiveGroup* hasCarveEndPrimGroup   = nullptr;
        
private:

    const GA_Attribute* uvAttrib = nullptr;
    //GA_Attribute* uvAttrib_nonConst = nullptr;
    GA_ROHandleT<fpreal> uv_h;
    const GA_AIFTuple* uvAIF;
    bool isPointUV;
    //int uvTupleSize;
        
    const GA_Attribute* carveUAttribStart = nullptr;
    const GA_Attribute* endCarveUAttrib = nullptr;
    GA_ROHandleT<fpreal> startCarveU_h;
    GA_ROHandleT<fpreal> endCarveU_h;
        
        
    GA_Attribute* carveUAttribStart_nonConst = nullptr;
    GA_Attribute* endCarveUAttrib_nonConst = nullptr;
        
    bool carveSpace_isWorld;
    GA_PrimitiveGroup* delPrimGroup;
    //GA_PointGroup*     delPointGroup;
    GA_VertexGroup*    delVertexGroup;
        
    const GA_PrimitiveGroup* carveStartPrimGroup;
    const GA_PrimitiveGroup* carveEndPrimGroup;

    GU_Detail* geoOrigin;
    //GU_Detail* geoOriginGU;

    exint subscribeRatio = 64;
    exint minGrainSize = 1024;


}; // End of class GFE_Carve

#endif
