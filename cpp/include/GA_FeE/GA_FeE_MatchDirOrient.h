
#pragma once

#ifndef __GA_FeE_MatchDirOrient_h__
#define __GA_FeE_MatchDirOrient_h__

//#include "GA_FeE/GA_FeE_MatchDirOrient.h"

#include "GA/GA_Detail.h"


//#include "GA_FeE/GA_FeE_Type.h


namespace GA_FeE_MatchDirOrient {





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





} // End of namespace GA_FeE_MatchDirOrient

#endif
