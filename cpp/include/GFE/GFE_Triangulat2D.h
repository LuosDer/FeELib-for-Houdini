
#pragma once

#ifndef __GFE_Triangulate2D_h__
#define __GFE_Triangulate2D_h__



//#include "GFE/GFE_Triangulate2D.h"

#include "GA/GA_Detail.h"
//#include "GA/GA_Types.h"

#include "GFE/GFE_Type.h"
#include "GFE/GFE_TopologyReference.h"


namespace GFE_Triangulate2D {


SYS_FORCE_INLINE
static void
triangulate2D(
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







} // End of namespace GFE_Triangulate2D

#endif
