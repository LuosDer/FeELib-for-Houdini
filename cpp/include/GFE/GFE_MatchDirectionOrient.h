
#pragma once

#ifndef __GFE_MatchDirectionOrient_h__
#define __GFE_MatchDirectionOrient_h__

#include "GFE/GFE_MatchDirectionOrient.h"

#include "GFE/GFE_GeoFilter.h"



class GFE_MatchDirOrient : public GFE_AttribFilterWithRef0 {

public:
    using GFE_AttribFilterWithRef0::GFE_AttribFilterWithRef0;

    ~GFE_EdgeGroupTransfer()
    {
    }

    

    void
    setComputeParm(
        const bool useSnapDist = true,
        const fpreal snapDist = 0.001,
        const bool reverseGroup = false,
        const bool outAsVertexGroup = false,
        const exint subscribeRatio = 64,
        const exint minGrainSize = 1024
    )
    {
        setHasComputed();
        this->useSnapDist      = useSnapDist;
        this->snapDist         = snapDist;
        this->reverseGroup     = reverseGroup;
        this->outAsVertexGroup = outAsVertexGroup;
        this->subscribeRatio   = subscribeRatio;
        this->minGrainSize     = minGrainSize;
    }

    
    

private:

// can not use in parallel unless for each GA_Detail
virtual bool
    computeCore() override
{
    if (getOutGroupArray().isEmpty())
        return false;
    
    if (groupParser.isEmpty())
        return true;


    
    return true;
}




    template<typename T>
    UT_Matrix4T<T>
    matchDirOrient(
        const UT_StringHolder& attribToXformPattern,
        const GA_ROHandleT<UT_Vector3T<T>>& posRest_h,
        const GA_ROHandleT<UT_Vector3T<T>>& posRef_h
    )
    {
    
    }





}; // End of class GFE_MatchDirOrient

#endif
