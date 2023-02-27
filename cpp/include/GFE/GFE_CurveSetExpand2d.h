
#pragma once

#ifndef __GA_FeE_CurveSetExpand2d_h__
#define __GA_FeE_CurveSetExpand2d_h__



//#include "GA_FeE/GA_FeE_CurveSetExpand2d.h"

#include "GA/GA_Detail.h"
//#include "GA/GA_Types.h"



namespace GA_FeE_CurveSetExpand2d {




#define TMP_CutPointGroup_GA_FeE_CurveSetExpand2d "__tmp_cutPointGroup_GA_FeE_CurveSetExpand2d"


SYS_FORCE_INLINE
static void
curveSetExpand2d(
    GA_Detail* const geoPoint,
    GA_Offset& primpoint,
    GA_PointGroup* const cutPointGroup
) {
    UT_ASSERT_MSG(!cutPointGroup->isDetached(), "cutPointGroup must be Not Detached");
    GA_Offset new_primpoint = geoPoint->appendPoint();
    geoPoint->copyPoint(new_primpoint, primpoint);
    //bool a = cutPointGroup->isOrdered();
    cutPointGroup->setElement(new_primpoint, false);
    primpoint = new_primpoint;
}



} // End of namespace GA_FeE_CurveSetExpand2d

#endif
