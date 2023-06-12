
#pragma once

#ifndef __GFE_GroupLoopedPrim_h__
#define __GFE_GroupLoopedPrim_h__

#include "GFE/GFE_GroupLoopedPrim.h"

#include "GFE/GFE_GeoFilter.h"



#include "GU/GU_Snap.h"

class GFE_GroupLoopedPrim : public GFE_AttribFilter {

public:
    using GFE_AttribFilter::GFE_AttribFilter;

    //GFE_GroupLoopedPrim(
    //    GA_Detail* const geo,
    //    const SOP_NodeVerb::CookParms* const cookparms = nullptr
    //)
    //    : GFE_AttribCreateFilter(geo, cookparms)
    //    , groupParserSeam(geo, groupParser.getGOP(), cookparms)
    //{
    //}

    //GFE_GroupLoopedPrim(
    //    const SOP_NodeVerb::CookParms& cookparms,
    //    GA_Detail* const geo
    //)
    //    : GFE_AttribCreateFilter(cookparms, geo)
    //    , groupParserSeam(cookparms, geo, groupParser.getGOP())
    //{
    //}

    void
        setComputeParm(
            const bool preFusePoint    = false,
            const fpreal fuseDist      = 1e-05,
            const exint subscribeRatio = 64,
            const exint minGrainSize   = 1024
        )
    {
        setHasComputed();

        this->preFusePoint   = preFusePoint;
        this->fuseDist       = fuseDist;
        this->subscribeRatio = subscribeRatio;
        this->minGrainSize   = minGrainSize;
    }

private:


    virtual bool
        computeCore() override
    {
        if (groupParser.isEmpty())
            return true;

        GU_DetailHandle geoOrigin_h;
        if (preFusePoint)
        {
            geoOriginTmp = new GU_Detail();
            geoOrigin_h.allocateAndSet(geoOriginTmp);
            geoOriginTmp->replaceWith(*geo);

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
            geoOriginTmp = geo->asGU_Detail();
        }
        
        UTparallelFor(groupParser.getPrimitiveSplittableRange(), [this](const GA_SplittableRange& r)
        {
            GA_Offset start, end;
            for (GA_Iterator it(r); it.blockAdvance(start, end); )
            {
                for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                {
                    if (geoOriginTmp->getPrimitiveClosedFlag(elemoff))
                    {
                        loopedPrimGroup->setElement(elemoff, true);
                        continue;
                    }
                    const GA_Size numvtx = geoOriginTmp->getPrimitiveVertexCount(elemoff);
                    if (getPrimitivePointOffset(geoOriginTmp, elemoff, 0) == getPrimitivePointOffset(geoOriginTmp, elemoff, numvtx-1))
                    {
                        loopedPrimGroup->setElement(elemoff, true);
                        continue;
                    }
                }
            }
        }, subscribeRatio, minGrainSize);

        return true;
    }

    SYS_FORCE_INLINE GA_Offset getPrimitivePointOffset(const GU_Detail* const geo, const GA_Offset primoff, const GA_Size vtxpnum) const
    { return geo->vertexPoint(geo->getPrimitiveVertexOffset(primoff, vtxpnum)); }



public:
    bool preFusePoint = false;
    fpreal fuseDist = 1e-05;

private:
    GU_Snap::PointSnapParms fuseParms;
    GA_PrimitiveGroup* loopedPrimGroup;
    
    GU_Detail* geoOriginTmp;
    
    exint subscribeRatio = 64;
    exint minGrainSize = 1024;
    
}; // End of Class GFE_GroupLoopedPrim




#endif