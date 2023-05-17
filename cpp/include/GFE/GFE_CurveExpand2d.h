
#pragma once

#ifndef __GFE_CurveExpand2d_h__
#define __GFE_CurveExpand2d_h__



//#include "GFE/GFE_CurveExpand2d.h"

#include "GA/GA_Detail.h"
//#include "GA/GA_Types.h"

#include "GFE/GFE_GeoFilter.h"

enum GFE_CurveExpand2dType
{
    GFE_CurveExpand2dType_AUTO,
    GFE_CurveExpand2dType_OPEN,
    GFE_CurveExpand2dType_CLOSE,
};










class GFE_CurveExpand2d : public GFE_AttribFilter {



    


public:
    using GFE_AttribFilter::GFE_AttribFilter;

    ~GFE_CurveExpand2d()
    {
    }


    void
        setComputeParm(
            const exint subscribeRatio = 64,
            const exint minGrainSize = 64
        )
    {
        setHasComputed();
        this->subscribeRatio = subscribeRatio;
        this->minGrainSize = minGrainSize;
    }


private:

    // can not use in parallel unless for each GA_Detail
    virtual bool
        computeCore() override
    {
        if (groupParser.isEmpty())
            return true;


        GA_Attribute* const uvAttribPtr = getOutAttribArray()[0];
        switch (uvAttribPtr->getAIFTuple()->getTupleSize(uvAttribPtr))
        {
        case 2:
            switch (uvAttribPtr->getAIFTuple()->getStorage(uvAttribPtr))
            {
            case GA_STORE_REAL16:
                curveUV<UT_Vector2T<fpreal16>>(uvAttribPtr);
                break;
            case GA_STORE_REAL32:
                curveUV<UT_Vector2T<fpreal32>>(uvAttribPtr);
                break;
            case GA_STORE_REAL64:
                curveUV<UT_Vector2T<fpreal64>>(uvAttribPtr);
                break;
            default:
                break;
            }
            break;
        case 3:
            switch (uvAttribPtr->getAIFTuple()->getStorage(uvAttribPtr))
            {
            case GA_STORE_REAL16:
                curveUV<UT_Vector3T<fpreal16>>(uvAttribPtr);
                break;
            case GA_STORE_REAL32:
                curveUV<UT_Vector3T<fpreal32>>(uvAttribPtr);
                break;
            case GA_STORE_REAL64:
                curveUV<UT_Vector3T<fpreal64>>(uvAttribPtr);
                break;
            default:
                break;
            }
            break;
            //case 4:
            //    switch (uvAttribPtr->getAIFTuple()->getStorage(uvAttribPtr))
            //    {
            //    case GA_STORE_REAL16:
            //        curveUV<UT_Vector4T<fpreal16>>(uvAttribPtr);
            //        break;
            //    case GA_STORE_REAL32:
            //        curveUV<UT_Vector4T<fpreal32>>(uvAttribPtr);
            //        break;
            //    case GA_STORE_REAL64:
            //        curveUV<UT_Vector4T<fpreal64>>(uvAttribPtr);
            //        break;
            //    default:
            //        break;
            //    }
            //break;
        default:
            break;
        }


        return true;
    }



    template<typename VECTOR_T>
    SYS_FORCE_INLINE
    void
        curveExpand2d(
        )
    {
    }


    

public:


private:
    exint subscribeRatio = 64;
    exint minGrainSize = 64;


}; // End of class GFE_CurveExpand2d




#endif
