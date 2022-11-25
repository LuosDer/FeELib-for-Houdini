
#pragma once

#ifndef __GEO_FeE_Attribute_h__
#define __GEO_FeE_Attribute_h__

//#include <GA_FeE/GEO_FeE_Attribute.h>

#include <GA/GA_Detail.h>

#include <GA_FeE/GA_FeE_Attribute.h>
#include <GA_FeE/GA_FeE_Group.h>


namespace GEO_FeE_Attribute {

//GEO_FeE_Attribute::normalizeElementAttrib(outGeo0, geo0Group, geo0AttribClass, attribPtr,
//    doNormalize, uniScale,
//    subscribeRatio, minGrainSize);

static void
normalizeElementAttrib(
    const GA_Detail* const geo,
    const GA_ElementGroup* geoGroup,
    const GA_AttributeOwner attribOwner,
    GA_Attribute* attribPtr,
    const bool doNormalize = 1,
    const fpreal64 uniScale = 1,
    const exint subscribeRatio = 64,
    const exint minGrainSize = 64
)
{
    const GA_SplittableRange geoSplittableRange = GA_FeE_Group::getSplittableRangeByAnyGroup(geo, geoGroup, attribOwner);
    GA_FeE_Attribute::normalizeElementAttrib(geoSplittableRange, attribPtr,
        doNormalize, uniScale,
        subscribeRatio, minGrainSize);
}





} // End of namespace GEO_FeE_Attribute

#endif
