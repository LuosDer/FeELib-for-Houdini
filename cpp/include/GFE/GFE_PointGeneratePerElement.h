
#pragma once

#ifndef __GFE_PointGeneratePerElement_h__
#define __GFE_PointGeneratePerElement_h__

#include "GFE_Math.h"
#include "GFE/GFE_PointGeneratePerElement.h"



#include "GFE/GFE_GeoFilter.h"

#include "GFE/GFE_AttributeCopy.h"

#include "SOP/SOP_Scatter-2.0.proto.h"


class GFE_PointGenPerElem : public GFE_AttribFilter {

#define __TEMP_GFE_PointGenPerElem_GroupName "__TEMP_GFE_PointGenPerElem_GROUP"

public:
    using GFE_AttribFilter::GFE_AttribFilter;


    void
        setComputeParm(
            const GA_GroupType elementType = GA_GROUP_PRIMITIVE,
            const exint subscribeRatio = 64,
            const exint minGrainSize = 64
        )
    {
        setHasComputed();
        this->elementType    = elementType;
        this->subscribeRatio = subscribeRatio;
        this->minGrainSize   = minGrainSize;
    }

private:

    // can not use in parallel unless for each GA_Detail
    virtual bool
        computeCore() override
    {
        switch (elementType)
        {
        case GA_GROUP_PRIMITIVE:
        case GA_GROUP_POINT:
        case GA_GROUP_EDGE:       break;
        default:  return false;   break;
        }

        if (groupParser.isEmpty())
            return true;
        
        switch (elementType)
        {
        case GA_GROUP_PRIMITIVE: pointGenPerPrim();   break;
        case GA_GROUP_POINT:     pointGenPerPoint();  break;
        case GA_GROUP_EDGE:      pointGenPerEdge();   break;
        default:                 return false;        break;
        }

        return true;
    }

    void pointGenPerPrim()
    {
        GU_DetailHandle geoTmp_h;
        geoTmp = new GU_Detail();
        geoTmp_h.allocateAndSet(geoTmp);
        geoTmp->replaceWith(geoSrc ? *geoSrc : *geo);
        
        const GA_PrimitiveGroup* const geoPrimGroup = groupParser.isPrimitiveDetached() ? groupParser.getPrimitiveGroup() : geoTmp->findPrimitiveGroup(groupParser.getName());
        const GA_SplittableRange geoPrimSplittableRange(geoTmp->getPrimitiveRange(geoPrimGroup));
        
        geo->clear();
        const GA_Size numpt = geoPrimGroup->entries();
        geo->appendPointBlock(numpt);
        
        const GA_RWHandleT<UT_Vector3T<fpreal>> pos_h(geo->getP());
        const GA_RWHandleT<UT_Vector3T<fpreal>> posRef_h(geoTmp->getP());
        UTparallelFor(geoPrimSplittableRange, [this, &pos_h, &posRef_h](const GA_SplittableRange& r)
        {
            UT_Vector3T<fpreal> pos0, pos1, pos2;
            GA_Offset start, end;
            for (GA_Iterator it(r.begin()); it.blockAdvance(start, end); )
            {
                bool flag;
                for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                {
                    flag = true;
                    const GA_Size numvtx = geoTmp->getPrimitiveVertexCount(elemoff);
                    const GA_Size numvtx2 = numvtx-2;
                    for (GA_Size vtxpnum = 0; vtxpnum < numvtx; ++vtxpnum)
                    {
                        const bool isEndVtx = vtxpnum >= numvtx2;
                        // const GA_Offset vtxoff = geoTmp->getPrimitiveVertexOffset(elemoff, vtxpnum);
                        // const GA_Offset ptoff = geoTmp->vertexPoint(vtxoff);
                        pos0 = posRef_h.get(getPrimitivePointOffset(geoTmp, elemoff, vtxpnum));
                        pos1 = posRef_h.get(getPrimitivePointOffset(geoTmp, elemoff, isEndVtx ? (vtxpnum == numvtx2 ? numvtx-1 : 0) : vtxpnum + 1));
                        pos2 = posRef_h.get(getPrimitivePointOffset(geoTmp, elemoff, isEndVtx ? (vtxpnum == numvtx2 ? 0 : 1)        : vtxpnum + 2));
                        
                        for (GA_Size vtxpnum1 = isEndVtx ? (vtxpnum > numvtx2 ? 2 : 1) : 0; vtxpnum1 < vtxpnum; ++vtxpnum1)
                        {
                            if (!GFE_Math::pointInTriangleT<fpreal>(posRef_h.get(getPrimitivePointOffset(geoTmp, elemoff, vtxpnum1)), pos0, pos1, pos2))
                            {
                                flag = false;
                                break;
                            }
                        }
                        if (!flag)
                            continue;
                        for (GA_Size vtxpnum1 = vtxpnum+2; vtxpnum1 < numvtx; ++vtxpnum1)
                        {
                            if (!GFE_Math::pointInTriangleT<fpreal>(posRef_h.get(getPrimitivePointOffset(geoTmp, elemoff, vtxpnum1)), pos0, pos1, pos2))
                            {
                                flag = false;
                                break;
                            }
                        }
                        if (flag)
                            break;
                    }
                    if (flag)
                    {
                        pos_h.set(elemoff, (pos0 + pos1 + pos2) / 3.0);
                        break;
                    }
                }
                UT_ASSERT_MSG(!flag, "not possible");
            }
        }, subscribeRatio, minGrainSize);
    }

    SYS_FORCE_INLINE GA_Offset getPrimitivePointOffset(const GA_Detail* const geo, const GA_Offset primoff, const GA_Size vtxpnum)
    { return geoTmp->vertexPoint(geoTmp->getPrimitiveVertexOffset(primoff, vtxpnum)); }


    void scatter()
    {
        scatterParms.setGroup(__TEMP_GFE_PointGenPerElem_GroupName);
        //scatterParms.setPrimNumAttrib();
        //scatterParms.setPrimNumAttrib();
        
        SOP_NodeCache* const nodeCache = scatterVerb->allocCache();
        const auto scatterCookparms = GFE_NodeVerb::newCookParms(cookparms, scatterParms, nodeCache, nullptr, &inputgdh);
        
        scatterVerb->cook(scatterCookparms);
    }



    void pointGenPerPoint()
    {
        
    }

    void pointGenPerEdge()
    {
        
    }

public:
    GA_GroupType elementType = GA_GROUP_PRIMITIVE;
    
private:
    GA_Attribute* attribPtr;
    exint subscribeRatio = 64;
    exint minGrainSize = 1024;


private:
    
    GU_Detail* geoTmp;


private:
    
    UT_Array<GU_ConstDetailHandle> inputgdh;
    SOP_Scatter_2_0Parms scatterParms;
    const SOP_NodeVerb* const scatterVerb = SOP_NodeVerb::lookupVerb("scatter::2.0");
    
#undef __TEMP_GFE_PointGenPerElem_GroupName
    
}; // End of class GFE_PointGenPerElem


















#endif
