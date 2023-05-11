
#pragma once

#ifndef __GFE_Clip_h__
#define __GFE_Clip_h__

//#include "GFE/GFE_Clip.h"

#include "GFE/GFE_GeoFilter.h"

enum class GFE_ClipMethod
{
    Abode,
    Below,
    Both,
};

class GFE_Clip : public GFE_AttribFilter {

public:
    using GFE_AttribFilter::GFE_AttribFilter;


    void
        setComputeParm(
            GFE_ClipMethod clipMethod = GFE_ClipMethod::Both,
            const UT_Vector3T<fpreal64>& dir = UT_Vector3T<fpreal64>(0,1,0),
            const fpreal64 dist = 0,
            const UT_Vector3T<fpreal64>& origin = UT_Vector3T<fpreal64>(0,0,0),
            const exint normlize = 0,
            const bool clipPoint = false,
            const exint subscribeRatio = 64,
            const exint minGrainSize = 1024
        )
    {
        setHasComputed();
        this->clipMethod = clipMethod;
        this->dir = dir;
        this->dist = dist;
        this->origin = origin;
        this->normlize = normlize;
        this->clipPoint = clipPoint;
        this->subscribeRatio = subscribeRatio;
        this->minGrainSize = minGrainSize;
    }


private:

    virtual bool
        computeCore() override
    {
        if (getOutAttribArray().isEmpty())
            return false;

        if (groupParser.isEmpty())
            return true;

        if (!posAttrib)
            posAttrib = geo->getP();


        switch (GFE_ClipMethod)
        {
        case GFE_ClipMethod::Abode:
            geo->asGU_Detail()->clip(dir, dist, normlize, groupParser.getPrimitiveGroup(), clipPoint);
            break;
        case GFE_ClipMethod::Below:
            geo->asGU_Detail()->clip(-dir, dist, normlize, groupParser.getPrimitiveGroup(), clipPoint);
            break;
        case GFE_ClipMethod::Both:
            {
                GU_Detail* geoOriginTmp = new GU_Detail();
                geoOriginTmp->replaceWith(geo);
                
                GU_DetailHandle geoOrigin_h;
                geoOrigin_h.allocateAndSet(geoOriginTmp);
                
                geoOriginTmp->clip(-dir, dist, normlize, groupParser.getPrimitiveGroup(), clipPoint);
                
                geo->asGU_Detail()->clip(dir, dist, normlize, groupParser.getPrimitiveGroup(), clipPoint);
                geo->baseMerge(*geoOriginTmp);
            }
            break;
        }
        


        return true;
    }



public:
    
    GFE_ClipMethod clipMethod = GFE_ClipMethod::Both;
    UT_Vector3T<fpreal64> dir = UT_Vector3T<fpreal64>(0,1,0);
    fpreal64 dist = 0;
    UT_Vector3T<fpreal64> origin = UT_Vector3T<fpreal64>(0,0,0);
    exint normlize = 0;
    bool clipPoint = false;
    
private:
    
    GA_Attribute* posAttrib = nullptr;

    exint subscribeRatio = 64;
    exint minGrainSize = 1024;

}; // End of class GFE_Clip



#endif
