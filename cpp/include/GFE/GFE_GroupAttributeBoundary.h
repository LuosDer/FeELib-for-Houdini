
#pragma once

#ifndef __GFE_GroupAttribBoundary_h__
#define __GFE_GroupAttribBoundary_h__

#include "GFE/GFE_GroupAttributeBoundary.h"


#include "GFE/GFE_GeoFilter.h"



class GFE_GroupAttribBoundary : public GFE_AttribFilter {



public:
    using GFE_AttribFilter::GFE_AttribFilter;

    ~GFE_GroupAttribBoundary()
    {
    }


    void
        setComputeParm(
            const bool includeUnhsaredEdge = true,
            const exint subscribeRatio = 64,
            const exint minGrainSize = 1024
        )
    {
        setHasComputed();
        this->includeUnhsaredEdge = includeUnhsaredEdge;
        this->subscribeRatio      = subscribeRatio;
        this->minGrainSize        = minGrainSize;
    }


private:

    // can not use in parallel unless for each GA_Detail
    virtual bool
        computeCore() override
    {
        if (getOutAttribArray().isEmpty() || getOutGroupArray())
            return false;

        if (groupParser.isEmpty())
            return true;

        
        attrib = getOutAttribArray()[0];
        switch (attrib->getAIFTuple()->getTupleSize(attrib))
        {
        case 2:
            switch (attrib->getAIFTuple()->getStorage(attrib))
            {
                case GA_STORE_REAL16: groupAttribBoundary<UT_Vector2T<fpreal16>>(); break;
                case GA_STORE_REAL32: groupAttribBoundary<UT_Vector2T<fpreal32>>(); break;
                case GA_STORE_REAL64: groupAttribBoundary<UT_Vector2T<fpreal64>>(); break;
                default: break;
            }
            break;
        case 3:
            switch (attrib->getAIFTuple()->getStorage(attrib))
            {
                case GA_STORE_REAL16: groupAttribBoundary<UT_Vector3T<fpreal16>>(); break;
                case GA_STORE_REAL32: groupAttribBoundary<UT_Vector3T<fpreal32>>(); break;
                case GA_STORE_REAL64: groupAttribBoundary<UT_Vector3T<fpreal64>>(); break;
                default: break;
            }
            break;
        case 4:
            switch (attrib->getAIFTuple()->getStorage(attrib))
            {
                case GA_STORE_REAL16: groupAttribBoundary<UT_Vector4T<fpreal16>>(); break;
                case GA_STORE_REAL32: groupAttribBoundary<UT_Vector4T<fpreal32>>(); break;
                case GA_STORE_REAL64: groupAttribBoundary<UT_Vector4T<fpreal64>>(); break;
                default: break;
            }
        default: break;
        }


        return true;
    }


    template<typename VECTOR_T>
    void groupAttribBoundary()
    {
        using value_type = typename VECTOR_T::value_type;

        const GA_PrimitiveGroup* const geoPrimGroup = groupParser.getPrimitiveGroup();

        //static_cast<GU_Detail*>(geo)->length
        const bool isPointAttrib = uvAttribPtr->getOwner() == GA_ATTRIB_POINT;
        const GA_AttributeOwner owner = uvAttribPtr->getOwner();

        //GA_Attribute* const pAttribPtr = GFE_Measure::addAttribPrimPerimeter(geo, geoPrimGroup);
        //GA_ROHandleT<fpreal> p_h(pAttribPtr);
        //GA_Attribute* const pAttribPtr = GFE_Measure::addAttribPrimPerimeter(geo, geoPrimGroup);
        GA_ROHandleT<UT_Vector3T<value_type>> pos_h(geo->getP());

        const GA_SplittableRange geoSplittableRange0(geo->getPrimitiveRange(geoPrimGroup));
        UTparallelFor(geoSplittableRange0, [this, &pos_h, isPointAttrib](const GA_SplittableRange& r)
        {
            GA_Offset start, end;
            for (GA_Iterator it(r); it.blockAdvance(start, end); )
            {
                for (GA_Offset primoff = start; primoff < end; ++primoff)
                {
                    const GA_Size numvtx = geo->getPrimitiveVertexCount(primoff);
                    if (numvtx < 2) return;
                }
            }
        }, subscribeRatio, minGrainSize);
    }




    
public:
    bool includeUnhsaredEdge = true;
    
private:
    GA_Attribute* const attrib;

    exint subscribeRatio = 64;
    exint minGrainSize = 64;


}; // End of Class GFE_GroupAttribBoundary









#endif
