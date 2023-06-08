
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
        
        //if (getOutAttribArray().isEmpty())
        //    return false;
        
        //if (groupParser.isEmpty())
        //    return true;
        
        
        // GU_DetailHandle geoTmp_h;
        // geoTmp = new GU_Detail();
        // geoTmp_h.allocateAndSet(geoTmp);
        // geoTmp->replaceWith(*geoSrc);
        // 
        // inputgdh.clear();
        // inputgdh.emplace_back(geoTmp_h);
        // 
        // GA_VertexGroup* const rectifyGroup = geoTmp->newVertexGroup(__TEMP_GFE_UVRectify_VertexGroupName);
        // GFE_GroupUnion::groupUnion(rectifyGroup, groupParser.getGroup());

        
        //if (groupParser.getGroup())
        //    GFE_GroupUnion::groupUnion(rectifyGroup, groupParser.getVertexGroup());
        //else
        //    rectifyGroup->addAll();
        
        uvRectify();
        
        return true;
    }


    void uvRectify()
    {
        SOP_UVFlatten_3_0Parms uvFlattenParms;
        // uvFlattenParms.setKeepExistingLayout(true);
        // uvFlattenParms.setKeepExistingSeams(true);
        // uvFlattenParms.setRectifyGroup(__TEMP_GFE_UVRectify_VertexGroupName);
        // uvFlattenParms.setMethod(flattenMethod ? SOP_UVFlatten_3_0Enums::Method::ABF : SOP_UVFlatten_3_0Enums::Method::SCP);
        // 
        // uvFlattenParms.setUVAttrib(getOutAttribArray()[0]->getName());
        // uvFlattenParms.setUVAttrib(getOutAttribArray()[0]->getName());
        // uvFlattenParms.setManualLayout(manualLayout);
        
        //const auto uvFlattenCookparms = GFE_NodeVerb::newCookParms(cookparms, uvFlattenParms, nullptr, &inputgdh);
        //const auto uvFlattenCookparms = GFE_NodeVerb::newCookParms(cookparms, uvFlattenParms);
        
        //geo->clear();
        
        //geo->replaceWith(*geoTmp);
        //return;

        UT_Array<GU_ConstDetailHandle> nodeInputs;
        const size_t nInputs = cookparms->nInputs();
        for (size_t i = 0; i < nInputs; ++i)
        {
            nodeInputs.emplace_back(cookparms->inputGeoHandle(i));
        }
    
        SOP_Node* const node = cookparms->getNode();
        const SOP_CookEngine cookEngine = node == nullptr ? SOP_COOK_COMPILED : SOP_COOK_TRADITIONAL;
    
        SOP_NodeVerb::CookParms uvFlattenCookparms(cookparms->gdh(), nodeInputs,
            cookEngine, node, cookparms->getContext(),
            &uvFlattenParms, nullptr, cookparms->error(), cookparms->depnode());
        
        const SOP_NodeVerb* const uvFlattenVerb = SOP_NodeVerb::lookupVerb("uvflatten::3.0");
        if (uvFlattenVerb)
            uvFlattenVerb->cook(uvFlattenCookparms);
    }



public:
    bool flattenMethod = true; // true means ABF  while  false means SCP
    bool manualLayout = true;
    
private:
    GU_Detail* geoTmp = nullptr;
    
    UT_Array<GU_ConstDetailHandle> inputgdh;
    //SOP_UVFlatten_3_0Parms uvFlattenParms;
    //const SOP_NodeVerb* const uvFlattenVerb = SOP_NodeVerb::lookupVerb("uvflatten::3.0");
    
    // exint subscribeRatio = 64;
    // exint minGrainSize = 256;
    
#undef __TEMP_GFE_UVRectify_VertexGroupName
    
}; // End of class GFE_UVRectify





#endif
