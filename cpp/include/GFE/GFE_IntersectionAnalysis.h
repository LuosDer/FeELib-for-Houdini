
#pragma once

#ifndef __GFE_IntersectionAnalysis_h__
#define __GFE_IntersectionAnalysis_h__

#include "GFE/GFE_IntersectionAnalysis.h"

#include "GFE/GFE_GeoFilter.h"



#include "GFE/GFE_NodeVerb.h"
#include "SOP/SOP_IntersectionAnalysis.proto.h"

class GFE_IntersectionAnalysis : public GFE_AttribFilterWithRef {


public:
    using GFE_AttribFilterWithRef::GFE_AttribFilterWithRef;


    void
        setComputeParm(
            const exint subscribeRatio = 64,
            const exint minGrainSize   = 64
        )
    {
        setHasComputed();
        this->firstIndex     = firstIndex;
        this->negativeIndex  = negativeIndex;
        this->outAsOffset    = outAsOffset;
        this->subscribeRatio = subscribeRatio;
        this->minGrainSize   = minGrainSize;
    }

    
private:

    // can not use in parallel unless for each GA_Detail
    virtual bool
        computeCore() override
    {
        UT_ASSERT_MSG_P(interAnalysisVerb, "Does not have interAnalysis Verb");
        
        UT_ASSERT_MSG_P(cookparms, "Does not have cookparms");
        if (!cookparms)
            return false;
        
        if (getOutAttribArray().isEmpty())
            return false;

        if (groupParser.isEmpty())
            return true;
        
        
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

    void intersectionAnalysis()
    {
        interAnalysisParms.setAgroup();
        SOP_NodeCache* const nodeCache = interAnalysisVerb->allocCache();
        const auto interCookparms = GFE_NodeVerb::newCookParms(cookparms, interAnalysisParms, nodeCache, nullptr, &inputgdh);
        interAnalysisVerb->cook(interCookparms);
    }
    
public:
    

private:
    
    UT_Array<GU_ConstDetailHandle> inputgdh;
    SOP_IntersectionAnalysisParms interAnalysisParms;
    const SOP_NodeVerb* const interAnalysisVerb = SOP_NodeVerb::lookupVerb("intersectionanalysis");
    
    exint subscribeRatio = 64;
    exint minGrainSize = 1024;


}; // End of class GFE_IntersectionAnalysis





#endif
