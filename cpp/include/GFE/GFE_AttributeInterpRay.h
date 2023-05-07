
#pragma once

#ifndef __GFE_AttributeInterpRay_h__
#define __GFE_AttributeInterpRay_h__

//#include "GFE/GFE_AttributeInterpRay.h"


#include "GFE/GFE_GeoFilter.h"

#include "GU/GU_RayIntersect.h"




class GFE_AttributeInterpRay : public GFE_AttribFilterWithRef {

public:
    using GFE_AttribFilterWithRef::GFE_AttribFilterWithRef;


    void
        setComputeParm(
            const float cuspAngleDegrees = GEO_DEFAULT_ADJUSTED_CUSP_ANGLE,
            const GEO_NormalMethod normalMethod = GEO_NormalMethod::ANGLE_WEIGHTED,
            const bool copyOrigIfZero = false
        )
    {
        setHasComputed();
        this->cuspAngleDegrees = cuspAngleDegrees;
        this->normalMethod = normalMethod;
        this->copyOrigIfZero = copyOrigIfZero;
    }

    SYS_FORCE_INLINE GA_Attribute* getAttrib() const
    {
        //UT_ASSERT_MSG(!getOutAttribArray().isEmpty(), "no attrib found");
        return getOutAttribArray().isEmpty() ? nullptr : getOutAttribArray()[0];
    }


private:

    virtual bool
        computeCore() override
    {
        if (groupParser.isEmpty())
            return true;

        if (1)
        {
            UTparallelFor(groupParser.getPointSplittableRange(), [this](const GA_SplittableRange& r)
            {
                GU_RayIntersect rayIntersect(geoRef0->asGU_Detail());
                GU_RayInfo rayInfo(maxDist);

                GA_PageHandleT<UT_Vector3T<fpreal64>, fpreal64, true, true, GA_Attribute, GA_ATINumeric, GA_Detail> pos_ph(geo->getP());
                GA_PageHandleT<UT_Vector3T<fpreal64>, fpreal64, true, true, GA_Attribute, GA_ATINumeric, GA_Detail> dir_ph(geo->getP());
                for (GA_PageIterator pit = r.beginPages(); !pit.atEnd(); ++pit)
                {
                    GA_Offset start, end;
                    for (GA_Iterator it(pit.begin()); it.blockAdvance(start, end); )
                    {
                        pos_ph.setPage(start);
                        dir_ph.setPage(start);
                        for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                        {
                            rayIntersect.sendRay(pos_ph.value(elemoff), dir_ph.value(elemoff), rayInfo);
                            rayInfo.
                        }
                    }
                }
            }, subscribeRatio, minGrainSize);
        }
        else
        {
            UTparallelFor(groupParser.getPointSplittableRange(), [this](const GA_SplittableRange& r)
            {
                GU_RayIntersect rayIntersect(geoRef0->asGU_Detail());
                GU_MinInfo minInfo(maxDist);

                GA_PageHandleT<UT_Vector3T<fpreal64>, fpreal64, true, true, GA_Attribute, GA_ATINumeric, GA_Detail> pos_ph(geo->getP());
                for (GA_PageIterator pit = r.beginPages(); !pit.atEnd(); ++pit)
                {
                    GA_Offset start, end;
                    for (GA_Iterator it(pit.begin()); it.blockAdvance(start, end); )
                    {
                        pos_ph.setPage(start);
                        for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                        {
                            rayIntersect.minimumPoint(pos_ph.value(elemoff), minInfo);
                        }
                    }
                }
            }, subscribeRatio, minGrainSize);
        }
        return true;
    }



public:
    float maxDist = 1e10;

private:
    exint subscribeRatio = 64;
    exint minGrainSize = 64;

}; // End of class GFE_AttributeInterpRay












#endif





