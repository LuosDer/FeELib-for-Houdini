
#pragma once

#ifndef __GA_FeE_Skin_h__
#define __GA_FeE_Skin_h__



//#include "GA_FeE/GA_FeE_Skin.h"

#include "GA/GA_Detail.h"
//#include "GA/GA_Types.h"



namespace GA_FeE_Skin {





SYS_FORCE_INLINE
static void
skin(
    GA_Detail* const geo,
    GA_Offset& primpoint,
    GA_PointGroup* const cutPointGroup
) {
    GA_Offset new_primpoint = geo->appendPoint();
    geo->copyPoint(new_primpoint, primpoint);
    //bool a = cutPointGroup->isOrdered();
    cutPointGroup->setElement(new_primpoint, false);
    primpoint = new_primpoint;
}



} // End of namespace GA_FeE_Skin

#endif
