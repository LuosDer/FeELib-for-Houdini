
#pragma once

#ifndef __GFE_UVRectify_h__
#define __GFE_UVRectify_h__

#include "GFE/GFE_UVRectify.h"



#include "GFE/GFE_GeoFilter.h"

#include "GFE/GFE_NodeVerb.h"

//#include "GFE/GFE_RestVectorComponent.h"
//#include "GFE/GFE_GroupElementByDirection.h"
//#include "GFE/GFE_GroupUnshared.h"

#include "SOP/SOP_UVFlatten-3.0.proto.h"

class GFE_UVRectify : public GFE_AttribFilter {

#define __TEMP_GFE_UVRectify_VertexGroupName "__TEMP_GFE_UVRectify"
    
public:
    using GFE_AttribFilter::GFE_AttribFilter;

    ~GFE_UVRectify()
    {
    }

    
    void
        setComputeParm(
            const bool flattenMethod = true,
            const bool manualLayout = true
            //,const exint subscribeRatio = 64,
            //const exint minGrainSize = 64
        )
    {
        setHasComputed();
        this->flattenMethod  = flattenMethod;
        this->manualLayout   = manualLayout;
        
        // this->subscribeRatio = subscribeRatio;
        // this->minGrainSize   = minGrainSize;
    }

    
private:

    
    virtual bool
        computeCore() override
    {
        UT_ASSERT_MSG_P(cookparms, "must have cookparms");
        if (!cookparms)
            return false;
        
        if (getOutAttribArray().isEmpty())
            return false;
        
        if (groupParser.isEmpty())
            return true;

        GA_VertexGroup* const rectifyGroup = geo->newVertexGroup(__TEMP_GFE_UVRectify_VertexGroupName);
        if (groupParser.getGroup())
            rectifyGroup->combine(groupParser.getGroup());
        else
            rectifyGroup->addAll();
        
        uvRectify();
        
        return true;
    }


    void uvRectify()
    {
        uvFlattenParms.setKeepExistingLayout(true);
        uvFlattenParms.setKeepExistingSeams(true);
        uvFlattenParms.setRectifyGroup(__TEMP_GFE_UVRectify_VertexGroupName);

        
        uvFlattenParms.setUVAttrib(getOutAttribArray()[0]->getName());
        uvFlattenParms.setManualLayout(manualLayout);
            
        const auto uvFlattenCookparms = GFE_NodeVerb::newCookParms(cookparms, uvFlattenParms);
            
        uvFlattenVerb->cook(uvFlattenCookparms);
    }



public:
    bool flattenMethod = true; // true means ABF  while  false means SCP
    bool manualLayout = true;
    
private:
    
    const SOP_NodeVerb* const uvFlattenVerb = SOP_NodeVerb::lookupVerb("uvflatten::3.0");
    
    SOP_UVFlatten_3_0Parms uvFlattenParms;
    
    // exint subscribeRatio = 64;
    // exint minGrainSize = 256;
    
#undef __TEMP_GFE_UVRectify_VertexGroupName
    
}; // End of class GFE_UVRectify





#endif
