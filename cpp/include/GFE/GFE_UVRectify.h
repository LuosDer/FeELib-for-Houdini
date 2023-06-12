
#pragma once

#ifndef __GFE_UVRectify_h__
#define __GFE_UVRectify_h__

#include "GFE/GFE_UVRectify.h"



#include "GFE/GFE_GeoFilter.h"

#include "GFE/GFE_NodeVerb.h"

// #include "SOP/SOP_UVFlatten-3.0.h"
#include "SOP/SOP_UVFlatten-3.0.proto.h"

class GFE_UVRectify : public GFE_AttribFilter {

#define __TEMP_GFE_UVRectify_VertexGroupName "__TEMP_GFE_UVRectify_GROUP"
#define __TEMP_GFE_UVRectify_VertexRectifyGroupName "__TEMP_GFE_UVRectify_Rectify_GROUP"
    
public:
    //using GFE_AttribFilter::GFE_AttribFilter;

    GFE_UVRectify(
        GA_Detail& geo,
        const SOP_NodeVerb::CookParms* const cookparms = nullptr
    )
        : GFE_AttribFilter(geo, cookparms)
        , groupParserRectify(geo, groupParser.getGOPRef(), cookparms)
    {
    }

    GFE_UVRectify(
        GA_Detail& geo,
        const GA_Detail& geoSrc,
        const SOP_NodeVerb::CookParms* const cookparms = nullptr
    )
        : GFE_AttribFilter(geo, geoSrc, cookparms)
        , groupParserRectify(geoSrc, groupParser.getGOPRef(), cookparms)
    {
    }

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
        UT_ASSERT_MSG_P(uvFlattenVerb, "Does not have UV Flatten 3.0 Verb");
        
        UT_ASSERT_MSG_P(cookparms, "Does not have cookparms");
        if (!cookparms)
            return false;
        
        //if (getOutAttribArray().isEmpty())
        //    return false;
        
        if (groupParser.isEmpty())
            return true;
        
        GU_DetailHandle geoTmp_h;
        geoTmp = new GU_Detail();
        geoTmp_h.allocateAndSet(geoTmp);
        geoTmp->replaceWith(geoSrc ? *geoSrc : *geo);
        
        // if (!getOutAttribArray().isEmpty())
        // {
        //     const GA_Attribute& attrib = *getOutAttribArray()[0];
        //     if (!geoTmp->findAttribute(attrib.getOwner(), attrib.getName()))
        //         geoTmp->getAttributes().cloneAttribute(attrib.getOwner(), attrib.getName(), attrib, true);
        // }
        
        inputgdh.clear();
        inputgdh.emplace_back(geoTmp_h);
        
        
        
        GA_PrimitiveGroup* const inGroup = geoTmp->newPrimitiveGroup(__TEMP_GFE_UVRectify_VertexGroupName);
        GA_VertexGroup* const rectifyGroup = geoTmp->newVertexGroup(__TEMP_GFE_UVRectify_VertexRectifyGroupName);
        GFE_GroupUnion::groupUnion(inGroup, groupParser.getGroup());
        GFE_GroupUnion::groupUnion(rectifyGroup, groupParserRectify.getGroup());
        //rectifyGroup->combine(groupParser.getGroup());
        
        uvRectify();

        geo->getGroupTable(GA_GROUP_PRIMITIVE)->destroy(__TEMP_GFE_UVRectify_VertexGroupName);
        geo->getGroupTable(GA_GROUP_VERTEX)   ->destroy(__TEMP_GFE_UVRectify_VertexRectifyGroupName);
        return true;
    }


    void uvRectify()
    {
        uvFlattenParms.setGroup(__TEMP_GFE_UVRectify_VertexGroupName);
        uvFlattenParms.setRectifyGroup(__TEMP_GFE_UVRectify_VertexRectifyGroupName);
        
        uvFlattenParms.setKeepExistingLayout(true);
        uvFlattenParms.setKeepExistingSeams(true);
        uvFlattenParms.setMethod(flattenMethod ? SOP_UVFlatten_3_0Enums::Method::ABF : SOP_UVFlatten_3_0Enums::Method::SCP);
        
        //uvFlattenParms.setUVAttrib(getOutAttribArray()[0]->getName());
        uvFlattenParms.setUVAttrib(uvAttribName);
        
        uvFlattenParms.setManualLayout(manualLayout);
        SOP_NodeCache* const nodeCache = uvFlattenVerb->allocCache();
        const auto uvFlattenCookparms = GFE_NodeVerb::newCookParms(cookparms, uvFlattenParms, nodeCache, nullptr, &inputgdh);
        
        uvFlattenVerb->cook(uvFlattenCookparms);
    }



public:
    GFE_GroupParser groupParserRectify;
    
    bool flattenMethod = true; // true means ABF  while  false means SCP
    bool manualLayout = true;
    const char* uvAttribName = "uv";

    
private:
    GU_Detail* geoTmp = nullptr;

    
private:
    UT_Array<GU_ConstDetailHandle> inputgdh;
    SOP_UVFlatten_3_0Parms uvFlattenParms;
    const SOP_NodeVerb* const uvFlattenVerb = SOP_NodeVerb::lookupVerb("uvflatten::3.0");
    
    // exint subscribeRatio = 64;
    // exint minGrainSize = 256;
    
#undef __TEMP_GFE_UVRectify_VertexGroupName
#undef __TEMP_GFE_UVRectify_VertexRectifyGroupName
    
}; // End of class GFE_UVRectify





#endif
