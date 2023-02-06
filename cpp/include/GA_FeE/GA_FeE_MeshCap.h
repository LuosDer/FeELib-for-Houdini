
#pragma once

#ifndef __GA_FeE_MeshCap_h__
#define __GA_FeE_MeshCap_h__



//#include "GA_FeE/GA_FeE_MeshCap.h"

#include "GA/GA_Detail.h"

#include "GU/GU_PolyFill.h"




namespace GA_FeE_MeshCap {








static void
meshCapSingle(
    GA_Detail* const geo
)
{
    UT_Array<GA_OffsetArray> rings;
    UT_Array<GA_OffsetArray> ringOrigs;
    GA_PrimitiveGroup* patchGroup = nullptr;
    GU_PolyFill::singlePolys(static_cast<GU_Detail*>(geo), rings, ringOrigs, patchGroup);
}








} // End of namespace GA_FeE_MeshCap

#endif
