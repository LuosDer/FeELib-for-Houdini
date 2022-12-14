
#pragma once

#ifndef __GEO_FeE_Attribute_h__
#define __GEO_FeE_Attribute_h__

//#include "GEO_FeE/GEO_FeE_Attribute.h"

#include "GA/GA_Detail.h"

#include "GA_FeE/GA_FeE_Range.h"
#include "GA_FeE/GA_FeE_Attribute.h"
#include "GA_FeE/GA_FeE_Group.h"
#include "GEO_FeE/GEO_FeE_Group.h"


namespace GEO_FeE_Attribute {


    //GEO_FeE_Attribute::normalizeAttribElement(outGeo0, geo0Group, geo0AttribClass, attribPtr,
    //    doNormalize, uniScale,
    //    subscribeRatio, minGrainSize);

    static void
        normalizeAttribElement(
            const GA_Detail* const geo,
            const GA_Group* const geoGroup,
            const GA_AttributeOwner attribOwner,
            GA_Attribute* const attribPtr,
            const bool doNormalize = 1,
            const fpreal64 uniScale = 1,
            const exint subscribeRatio = 64,
            const exint minGrainSize = 64
        )
    {
        UT_ASSERT_P(geo);
        UT_ASSERT_P(geoGroup);
        const GA_SplittableRange geoSplittableRange = GA_FeE_Range::getSplittableRangeByAnyGroup(geo, geoGroup, attribOwner);
        GA_FeE_Attribute::normalizeAttribElement(geoSplittableRange, attribPtr,
            doNormalize, uniScale,
            subscribeRatio, minGrainSize);
    }












} // End of namespace GEO_FeE_Attribute

#endif
