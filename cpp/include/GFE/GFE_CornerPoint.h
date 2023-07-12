
#pragma once

#ifndef __GFE_CornerPoint_h__
#define __GFE_CornerPoint_h__

#include "GFE/GFE_CornerPoint.h"

#include "GFE/GFE_GeoFilter.h"


#include "GFE/GFE_Normal2D.h"

class GFE_CornerPoint : public GFE_AttribFilter {

public:
    using GFE_AttribFilter::GFE_AttribFilter;

    ~GFE_CornerPoint()
    {
    }

    void
        setComputeParm(
            const fpreal threshold = 1e-05,
            const exint subscribeRatio = 64,
            const exint minGrainSize   = 1024
        )
    {
        setHasComputed();
        
        this->threshold = threshold;
        this->subscribeRatio = subscribeRatio;
        this->minGrainSize   = minGrainSize;
    }


    
private:

    virtual bool
        computeCore() override
    {
        if (getOutGroupArray().isEmpty())
            return false;

        if (groupParser.isEmpty())
            return true;
        
        setValidConstPosAttrib();

        
        GFE_Normal2D normal2D(geo, cookparms);
        normal2D.groupParser.setGroup(groupParser);
        normal2D.setPositionAttrib(posAttrib);
        normal2D.setNormal2DAttrib(true);
        //const float cuspAngleDegrees = GEO_DEFAULT_ADJUSTED_CUSP_ANGLE;
        //const GEO_NormalMethod method = GEO_NormalMethod::ANGLE_WEIGHTED;
        //const bool copyOrigIfZero = false;
        //normal2D.normal3D.setComputeParm(cuspAngleDegrees, method, copyOrigIfZero);
        normal2D.defaultNormal3D = {0,1,0};
        if (!sopparms.getUseConstantNormal3D())
            normal2D.findOrCreateNormal3D(sopparms.getFindNormal3D(), sopparms.getAddNormal3DIfNoFind(),
                geo0Normal3DSearchOrder, sopparms.getNormal3DAttrib());
        normal2D.setComputeParm(sopparms.getExtrapolateEnds(), sopparms.getScaleByTurns(),
            sopparms.getNormalize(), sopparms.getUniScale(), sopparms.getBlend());
        normal2D.compute();
        
        const GA_Attribute* normal2DAttrib = normal2D.getAttrib();
        
        cornerPointGroup = getOutGroupArray()[0];
        switch (posAttrib->getAIFTuple()->getStorage(posAttrib))
        {
        case GA_STORE_REAL16: cornerPoint<fpreal16>(); break;
        case GA_STORE_REAL32: cornerPoint<fpreal32>(); break;
        case GA_STORE_REAL64: cornerPoint<fpreal64>(); break;
        default: break;
        }


        return true;
    }


    template<typename T>
    void cornerPoint()
    {
        UT_ASSERT_P(cornerPointGroup);
        
        UTparallelFor(groupParser.getPointSplittableRange(), [this](const GA_SplittableRange& r)
        {
            GA_PageHandleT<T, T, true, true, GA_Attribute, GA_ATINumeric, GA_Detail> pos_ph(posAttrib);
            for (GA_PageIterator pit = r.beginPages(); !pit.atEnd(); ++pit)
            {
                GA_Offset start, end;
                for (GA_Iterator it(pit.begin()); it.blockAdvance(start, end); )
                {
                    pos_ph.setPage(start);
                    for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                    {
                        cornerPointGroup->setElement(elemoff, );
                        pos_ph.value(elemoff);
                        
                    }
                }
            }
        }, subscribeRatio, minGrainSize);
    }



public:
    fpreal threshold = 1e-05;

private:
    GA_Attribute* normalAttrib;
    GA_PointGroup* cornerPointGroup;
    
    exint subscribeRatio = 64;
    exint minGrainSize = 1024;


}; // End of class GFE_CornerPoint



#endif
