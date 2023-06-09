
#pragma once

#ifndef __GFE_PointGeneratePerElement_h__
#define __GFE_PointGeneratePerElement_h__

#include "GFE/GFE_PointGeneratePerElement.h"



#include "GFE/GFE_GeoFilter.h"



#include "GFE/GFE_AttributeCopy.h"

class GFE_PointGenPerElem : public GFE_AttribFilter {


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
        if (geoSrc)
        {
            
        }
        else
        {
            geo->clear();
        }
        groupParser.getPrimitiveGroup()->entries();
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
    GU_Detail* geoTmp;
    
    GA_Attribute* attribPtr;
    exint subscribeRatio = 64;
    exint minGrainSize = 1024;


}; // End of class GFE_PointGenPerElem


















#endif
