
#pragma once

#ifndef __GFE_MatchDirOrient_h__
#define __GFE_MatchDirOrient_h__

//#include "GFE/GFE_MatchDirOrient.h"

#include "GA/GA_Detail.h"


//#include "GFE/GFE_Type.h


namespace GFE_MatchDirOrient {





    template<typename T>
    static UT_Matrix4T<T>
        matchDirOrient(
            GA_Detail* const geo,
            const GA_Detail* const refGeo,
            const GA_Detail* restGeo,
            const UT_StringHolder& attribToXformPattern,
            const GA_ROHandleT<UT_Vector3T<T>>& posRest_h,
            const GA_ROHandleT<UT_Vector3T<T>>& posRef_h
        ) {
    }





} // End of namespace GFE_MatchDirOrient

#endif
