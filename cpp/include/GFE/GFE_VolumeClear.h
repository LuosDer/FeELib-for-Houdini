
#pragma once

#ifndef __GFE_VolumeClear_h__
#define __GFE_VolumeClear_h__

#include "GFE/GFE_VolumeClear.h"

#include "GFE/GFE_GeoFilter.h"



class GFE_VolumeClear : public GFE_GeoFilter {

public:

    using GFE_GeoFilter::GFE_GeoFilter;


    ~GFE_VolumeClear()
    {
    }
    
    SYS_FORCE_INLINE std::vector<fpreal>& getVoxelValues()
    { return voxValues; }

void
    setComputeParm(
        const exint subscribeRatio = 64,
        const exint minGrainSize   = 1024
    )
{
    setHasComputed();
    this->subscribeRatio = subscribeRatio;
    this->minGrainSize   = minGrainSize;
}


    
private:

    virtual bool
        computeCore() override
    {
        const size_t size = getVolumeArray().size();
        UT_ASSERT_P(voxValues.size() == size);
        
        for (size_t i = 0; i < size; i++)
        {
            vol = getVolumeArray()[i];
            volumeClear();
        }
        return true;
    }


    void volumeClear()
    {
        vol->setVoxels();
    }
    


public:
    
private:
    GU_PrimVolume* vol;
    std::vector<fpreal> voxValues;
    
    exint subscribeRatio = 64;
    exint minGrainSize   = 1024;

    
};








#endif





