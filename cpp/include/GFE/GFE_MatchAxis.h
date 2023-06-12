
#pragma once

#ifndef __GFE_MatchAxis_h__
#define __GFE_MatchAxis_h__

#include "GFE/GFE_MatchAxis.h"

    
#include "GFE/GFE_XformByAttrib.h"
#include "GFE/GFE_Attribute.h"



#include "GFE/GFE_GeoFilter.h"



class GFE_MatchAxis : public GFE_AttribFilter {


public:
    using GFE_AttribFilter::GFE_AttribFilter;
    
    void
        setComputeParm(
            const UT_Vector3T<fpreal>& fromVec = UT_Vector3T<fpreal>(0,0,1),
            const UT_Vector3T<fpreal>& toVec   = UT_Vector3T<fpreal>(1,0,0),
            const UT_Vector3T<fpreal>& upVec   = UT_Vector3T<fpreal>(1,1,0),
            const UT_Vector3T<fpreal>& center  = UT_Vector3T<fpreal>(0,0,0),
            const exint subscribeRatio = 64,
            const exint minGrainSize   = 64
        )
    {
        setHasComputed();
        this->fromVec = fromVec;
        this->toVec   = toVec;
        this->upVec   = upVec;
        this->center  = center;
        this->subscribeRatio = subscribeRatio;
        this->minGrainSize   = minGrainSize;
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
        
        const UT_Matrix3T<fpreal> xform;
        
        xform.lookat(fromVec, toVec, upVec);
        
        attribPtr = getOutAttribArray()[0];
        // const GA_Storage storage = attribPtr->getAIFTuple()->getStorage(attribPtr);
        switch (attribPtr->getAIFTuple()->getStorage(attribPtr))
        {
        case GA_STORE_INT16:  enumerate<int16>();           break;
        case GA_STORE_INT32:  enumerate<int32>();           break;
        case GA_STORE_INT64:  enumerate<int64>();           break;
        case GA_STORE_REAL16: enumerate<fpreal16>();        break;
        case GA_STORE_REAL32: enumerate<fpreal32>();        break;
        case GA_STORE_REAL64: enumerate<fpreal64>();        break;
        case GA_STORE_STRING: enumerate<UT_StringHolder>(); break;
        default: break;
        }

        return true;
    }
    

public:
    UT_Vector3T<fpreal> fromVec = UT_Vector3T<fpreal>(0,0,1);
    UT_Vector3T<fpreal> toVec   = UT_Vector3T<fpreal>(1,0,0);
    UT_Vector3T<fpreal> upVec   = UT_Vector3T<fpreal>(1,1,0);
    UT_Vector3T<fpreal> center  = UT_Vector3T<fpreal>(0,0,0);
    
private:
    
    GA_Attribute* attribPtr;
    exint subscribeRatio = 64;
    exint minGrainSize = 1024;


}; // End of class GFE_MatchAxis


#endif
