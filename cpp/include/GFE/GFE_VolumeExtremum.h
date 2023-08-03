
#pragma once

#ifndef __GFE_VolumeExtremum_h__
#define __GFE_VolumeExtremum_h__

#include "GFE/GFE_VolumeExtremum.h"

#include "GFE/GFE_GeoFilter.h"


#include "UT/UT_VoxelArray.h"
#include "GU/GU_PrimVolume.h"

class GFE_VolumeExtremum : public GFE_AttribFilterWithRef0 {

public:

    using GFE_AttribFilterWithRef0::GFE_AttribFilterWithRef0;


    ~GFE_VolumeExtremum()
    {
    }


void
    setComputeParm(
        const bool doNormalize = true,
        const fpreal uniScale = 1.0,
        const exint subscribeRatio = 64,
        const exint minGrainSize   = 1024
    )
{
    setHasComputed();
    this->doNormalize = doNormalize;
    this->uniScale = uniScale;
    this->subscribeRatio = subscribeRatio;
    this->minGrainSize   = minGrainSize;
}


    
private:

    virtual bool
        computeCore() override
    {
        if (!getOutAttribArray().size())
            return false;

        if (!doNormalize && uniScale==1.0)
            return true;
        
        if (groupParser.isEmpty())
            return true;

        const size_t size = getOutAttribArray().size();
        for (size_t i = 0; i < size; i++)
        {
            attrib = getOutAttribArray()[i];
            
            scaleAttribElement();
            
        }
        return true;
    }

    VolumeExtremum
    void volumeExtremum()
    {
        UT_VoxelArrayF* vol;
        UTparallelReduce(UT_BlockedRange(0, vol->numTiles()), [&](const UT_BlockedRange<exint> &r)
        {
            const exint curtile = r.begin();
            UT_VoxelTileIteratorF vit;
            vit.setLinearTile(curtile, vol);
            for (vit.rewind(); !vit.atEnd(); vit.advance())
            {
                float delta = 0;
                active->getValue(vit.x(), vit.y(), vit.z());
                if (active &&  < 0.5)
                {
                    if (sopparms.getZeroInactive())
                        vit.setValue(0);
                    continue;
                }
            }
        }
        
        UTparallelForEachNumber(vol->numTiles(), [&](const UT_BlockedRange<exint> &r)
        {
            const exint curtile = r.begin();
            UT_VoxelTileIteratorF vit;
            vit.setLinearTile(curtile, vol);
            for (vit.rewind(); !vit.atEnd(); vit.advance())
            {
                float delta = 0;
                active->getValue(vit.x(), vit.y(), vit.z());
                if (active &&  < 0.5)
                {
                    if (sopparms.getZeroInactive())
                        vit.setValue(0);
                    continue;
                }
            }
        }
        
        UTparallelFor(groupParser.getSplittableRange(attrib), [this](const GA_SplittableRange& r)
        {
            auto tupleTypeVariant = GFE_Variant::getNumericTupleType1vVariant(*attrib);
            auto doNormalizeVariant = GFE_Variant::getBoolVariant(doNormalize);
            auto scaleVariant = GFE_Variant::getBoolVariant(uniScale != 1.0);
            std::visit([&] (auto scaleVariant, auto doNormalizeVariant, auto tupleTypeVariant)
            {
                using type = typename GFE_Variant::get_numeric_tuple_type_t<tupleTypeVariant>;
                GFE_RWPageHandleT<type> attrib_ph(attrib);
                GA_Offset start, end;
                for (GA_PageIterator pit = r.beginPages(); !pit.atEnd(); ++pit)
                {
                    for (GA_Iterator it(pit.begin()); it.blockAdvance(start, end); )
                    {
                        attrib_ph.setPage(start);
                        for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                        {
                            if constexpr (GFE_Type::isVector<type>)
                            {
                                if constexpr (doNormalizeVariant)
                                    attrib_ph.value(elemoff).normalize();
                            }
                            if constexpr (scaleVariant)
                                attrib_ph.value(elemoff) *= uniScale;
                        }
                    }
                }
            }, scaleVariant, doNormalizeVariant, tupleTypeVariant);
        }, subscribeRatio, minGrainSize);
    }
    
/*
    template<typename _Ty>
    void scaleAttribElement()
    {
        UTparallelFor(groupParser.getSplittableRange(attrib), [this](const GA_SplittableRange& r)
        {
            auto scaleVariant = GFE_Variant::getBoolVariant(uniScale != 1.0);
            auto doNormalizeVariant = GFE_Variant::getBoolVariant(doNormalize);
            std::visit([&] (auto scaleVariant, auto doNormalizeVariant)
            {
                GFE_RWPageHandleT<_Ty> attrib_ph(attrib);
                GA_Offset start, end;
                for (GA_PageIterator pit = r.beginPages(); !pit.atEnd(); ++pit)
                {
                    for (GA_Iterator it(pit.begin()); it.blockAdvance(start, end); )
                    {
                        attrib_ph.setPage(start);
                        for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                        {
                            if constexpr (GFE_Type::isVector<_Ty>)
                            {
                                if constexpr (doNormalizeVariant)
                                    attrib_ph.value(elemoff).normalize();
                            }
                            if constexpr (scaleVariant)
                                attrib_ph.value(elemoff) *= uniScale;
                        }
                    }
                }
            }, scaleVariant, doNormalizeVariant);
        }, subscribeRatio, minGrainSize);
    }
*/



public:
    bool doNormalize = true;
    fpreal uniScale = 1.0;

private:
    GA_Attribute* attrib;
    GA_Attribute* refattrib;

    
    exint subscribeRatio = 64;
    exint minGrainSize   = 1024;

    
};








#endif





