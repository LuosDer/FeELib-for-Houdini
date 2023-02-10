
#pragma once

#ifndef __GA_FeE_XformByAttrib_h__
#define __GA_FeE_XformByAttrib_h__

//#include "GA_FeE/GA_FeE_XformByAttrib.h"

#include "GA/GA_Detail.h"


#include "GA_FeE/GA_FeE_Attribute.h"
#include "GA_FeE/GA_FeE_Connectivity.h"
#include "GA_FeE/GA_FeE_Group.h"
#include "GA_FeE/GA_FeE_GroupPromote.h"
#include "GA_FeE/GA_FeE_Measure.h"
#include "GA_FeE/GA_FeE_Range.h"


namespace GA_FeE_XformByAttrib {





template<typename T>
static void
xformByAttrib(
    GA_Detail* const geo,
    const GA_RWHandleT<UT_Vector3T<T>>& uvAttribPtr_h,
    const GA_PrimitiveGroup* const geoGroup = nullptr,
    const bool computeUVAreaInPiece = true,
    const UT_Vector3R& uvScale = UT_Vector3R(1.0),
    const bool doUVScalex = true,
    const bool doUVScaley = true,
    const bool doUVScalez = true,
    const exint subscribeRatio = 64,
    const exint minGrainSize = 64
)
{
}




} // End of namespace GA_FeE_XformByAttrib

#endif
