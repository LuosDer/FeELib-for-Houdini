
#pragma once

#ifndef __GFE_NodeVerb_h__
#define __GFE_NodeVerb_h__

#include "GFE/GFE_NodeVerb.h"

#include "SOP/SOP_NodeVerb.h"


namespace GFE_NodeVerb {


    
    
static SOP_NodeVerb::CookParms newCookParms(
    const SOP_NodeVerb::CookParms& cookparms,
    const SOP_NodeParms* const nodeParms,
    GU_DetailHandle* const destgdh = nullptr,
    const UT_Array<GU_ConstDetailHandle>* const inputs = nullptr,
    SOP_NodeCache* const nodeCache = nullptr
)
{
    UT_Array<GU_ConstDetailHandle> nodeInputs;
    if (!inputs)
    {
        const size_t nInputs = cookparms.nInputs();
        for (size_t i = 0; i < nInputs; ++i)
        {
            nodeInputs.emplace_back(cookparms.inputGeoHandle(i));
        }
    }
    
    SOP_Node* const node = cookparms.getNode();
    const SOP_CookEngine cookEngine = node == nullptr ? SOP_COOK_COMPILED : SOP_COOK_TRADITIONAL;
    
    return SOP_NodeVerb::CookParms(
        destgdh ? *destgdh : cookparms.gdh(),
        inputs  ? *inputs  : nodeInputs,
        cookEngine, node, cookparms.getContext(),
        nodeParms, nodeCache, cookparms.error(), cookparms.depnode());
}

    
SYS_FORCE_INLINE static SOP_NodeVerb::CookParms newCookParms(
    const SOP_NodeVerb::CookParms* const cookparms,
    const SOP_NodeParms* const nodeParms,
    GU_DetailHandle* const destgdh = nullptr,
    const UT_Array<GU_ConstDetailHandle>* const inputs = nullptr,
    SOP_NodeCache* const nodeCache = nullptr
)
{ return newCookParms(*cookparms, nodeParms, destgdh, inputs, nodeCache); }

SYS_FORCE_INLINE static SOP_NodeVerb::CookParms newCookParms(
    const SOP_NodeVerb::CookParms* const cookparms,
    const SOP_NodeParms& nodeParms,
    GU_DetailHandle* const destgdh = nullptr,
    const UT_Array<GU_ConstDetailHandle>* const inputs = nullptr,
    SOP_NodeCache* const nodeCache = nullptr
)
{ return newCookParms(*cookparms, &nodeParms, destgdh, inputs, nodeCache); }



} // End of namespace GFE_NodeVerb





#endif
