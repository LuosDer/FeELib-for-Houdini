
#pragma once

#ifndef __GFE_ExtremePrim_h__
#define __GFE_ExtremePrim_h__



//#include "GFE/GFE_ExtremePrim.h"

#include "GFE/GFE_GeoFilter.h"


#include "GFE/GFE_Measure.h"








class GFE_ExtremePrim : public GFE_AttribFilter {

public:

    using GFE_AttribFilter::GFE_AttribFilter;

    void
        setComputeParm(
            const GFE_StatisticalFunction statisticalFunction = GFE_StatisticalFunction::Min,
            const GFE_MeasureType measureType = GFE_MeasureType::Area,
            const exint subscribeRatio = 64,
            const exint minGrainSize = 64
        )
    {
        setHasComputed();
        this->statisticalFunction = statisticalFunction;
        this->measureType = measureType;
        this->subscribeRatio = subscribeRatio;
        this->minGrainSize = minGrainSize;
    }


private:


    virtual bool
        computeCore() override
    {
        if (getOutGroupArray().isEmpty())
            return false;

        if (groupParser.isEmpty())
            return true;

        GA_PrimitiveGroup* const outGroup = static_cast<GA_PrimitiveGroup*>(getOutGroupArray()[0]);


        GA_Size numelems;
        if ((int)measureType <= 1) {
            numelems = geo->getNumPrimitives();
            //numelems = nprimitives(0);
        }

        // if ( chi("../runOverPieces") ) {
        //     chs("../pieceAttribName")
        // }
        
        GA_Attribute* measureAttrib = nullptr;
        switch (measureType)
        {
        case GFE_MeasureType::Perimeter:
            measureAttrib = GFE_Measure::addAttribPrimPerimeter(geo, nullptr, GA_STORE_INVALID);
            break;
        case GFE_MeasureType::Area:
            measureAttrib = GFE_Measure::addAttribPrimArea(geo, nullptr, GA_STORE_INVALID);
            break;
        default:
            break;
        }
        GA_ROHandleT<fpreal> measure_h(measureAttrib);



        GA_Size extremeElemnum = 0;
        fpreal extremeValue = statisticalFunction == GFE_StatisticalFunction::Max ? SYS_FP64_MIN : SYS_FP64_MAX;
        fpreal measureValue;

        measureValue = measure_h.get();
        switch (statisticalFunction)
        {
        case GFE_StatisticalFunction::Min:
            if (measureValue >= extremeValue) continue;
            break;
        case GFE_StatisticalFunction::Max:
            if (measureValue <= extremeValue) continue;
            break;
        default:
            break;
        }
        extremeValue = measureValue;
        extremeElemnum = elemnum;


        outGroup->setElement(extremeElemnum, true);

        
        return true;
    }



public:
    GFE_StatisticalFunction statisticalFunction = GFE_StatisticalFunction::Min;
    GFE_MeasureType measureType = GFE_MeasureType::Area;


private:
    exint subscribeRatio = 64;
    exint minGrainSize = 64;
};






#endif
