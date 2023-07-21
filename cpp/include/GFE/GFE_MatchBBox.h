
#pragma once

#ifndef __GFE_MatchBBox_h__
#define __GFE_MatchBBox_h__

#include "GFE/GFE_MatchBBox.h"

#include "GFE/GFE_GeoFilter.h"


#include "UT/UT_BoundingBox.h"

#include "GFE/GFE_Bound.h"
#include "GFE/GFE_XformByAttrib.h"

template<typename _TScalar = fpreal>
class GFE_MatchBBoxXform {

public:
    
    GFE_MatchBBoxXform()
    {
    }
    
    ~GFE_MatchBBoxXform()
    {
    }
    

    void setMatchCenter()
    {
        UT_Vector3T<fpreal> center0 = restBBox.center();
        UT_Vector3T<fpreal> center1 = refBBox.center();
        UT_Vector3T<fpreal> size0   = restBBox.size();
        UT_Vector3T<fpreal> size1   = refBBox.size();
        xform.translate(center1-center0);
    }

    
    
    SYS_FORCE_INLINE void setRestBBox(const GA_Detail& geo, const GA_Range& pointRange, const GA_Attribute* posAttrib = nullptr)
    { restBBox = GFE_Bound::getBBox<fpreal>(geo, restBBox); }
    
    SYS_FORCE_INLINE void setRefBBox(const GA_Detail& geo, const GA_Range& pointRange, const GA_Attribute* posAttrib = nullptr)
    { refBBox = GFE_Bound::getBBox<fpreal>(geo, restBBox); }
    
    //SYS_FORCE_INLINE UT_BoundingBoxT<fpreal> getRestBBox() const
    //{ return restBBox; }
    //SYS_FORCE_INLINE UT_BoundingBoxT<fpreal> getRefBBox() const
    //{ return refBBox; }
    
    SYS_FORCE_INLINE UT_Vector3T<_TScalar> computeTranslate(const _TScalar biasRest, const _TScalar biasRef) const
    {
        return restBBox.maxvec() * (biasRest - 1.0) - restBBox.minvec() * biasRest
             + refBBox .maxvec() * (biasRest + 1.0) - refBBox .minvec() * biasRef;
    }
    
    SYS_FORCE_INLINE UT_Vector3T<_TScalar> computeTranslate(const _TScalar bias) const
    { return computeTranslate(bias, bias); }

    
    SYS_FORCE_INLINE UT_Vector3T<_TScalar> computeScale(const _TScalar biasRest, const _TScalar biasRef) const
    {
        return restBBox.maxvec() * (biasRest - 1.0) - restBBox.minvec() * biasRest
             + refBBox .maxvec() * (biasRest + 1.0) - refBBox .minvec() * biasRef;
    }
    
    SYS_FORCE_INLINE UT_Vector3T<_TScalar> computeScale(const _TScalar bias) const
    { return computeScale(bias, bias); }

    
    UT_Matrix4T<_TScalar> computeXform(const _TScalar biasRestT, const _TScalar biasRefT, const _TScalar biasRestS, const _TScalar biasRefS) const
    {
        UT_Vector3T<_TScalar> translate = computeTranslate();
        UT_Vector3T<_TScalar> scale = computeTranslate();
    }
    
public:
    UT_BoundingBoxT<_TScalar> restBBox;
    UT_BoundingBoxT<_TScalar> refBBox;
    //UT_Vector3T<_TScalar> translate;
    //UT_Vector3T<_TScalar> scale;
    //UT_Matrix4T<_TScalar> xform;
    
}; // End of class GFE_MatchBBoxXform



class GFE_MatchBBox : public GFE_AttribFilterWithRef1 {

public:
    using GFE_AttribFilterWithRef1::GFE_AttribFilterWithRef1;
    

    void
        setComputeParm(
            const exint subscribeRatio = 64,
            const exint minGrainSize = 1024
        )
    {
        setHasComputed();
        this->subscribeRatio = subscribeRatio;
        this->minGrainSize   = minGrainSize;
    }
    
    SYS_FORCE_INLINE void setRepairPrecision()
    { repairPrecision = false; }
    
    SYS_FORCE_INLINE void setRepairPrecision(const fpreal threshold)
    { repairPrecision = true; repairPrecisionThreshold = threshold; }

    
    SYS_FORCE_INLINE GA_Attribute* setXformAttrib(GA_Attribute* const inAttrib = nullptr)
    { return xformAttrib = !inAttrib || inAttrib->getTupleSize() != 16 || inAttrib->getStorageClass() != GA_STORECLASS_REAL || !inAttrib->getAIFTuple() ? nullptr : inAttrib; }
    
    SYS_FORCE_INLINE GA_Attribute* setXformAttrib(const bool detached = false, const GA_Storage storage = GA_STORE_INVALID, const UT_StringRef& attribName = "xform")
    { return getOutAttribArray().findOrCreateTuple(detached, GA_ATTRIB_DETAIL, GA_STORECLASS_FLOAT, storage, attribName, 16, GA_Defaults(0.0f), false); }
    
    
    
private:

    virtual bool
        computeCore() override
    {
        if (getOutAttribArray().isEmpty())
            return false;

        UT_ASSERT_MSG_P(!xformAttrib ||
                       (xformAttrib->getTupleSize() == 16 &&
                        xformAttrib->getStorageClass() == GA_STORECLASS_REAL &&
                        xformAttrib->getAIFTuple()), "not correct xform attrib type");
        
        if (groupParser.isEmpty())
            return true;

        setValidPosAttrib();
        
        geoRest = geoRef1 ? geoRef1 : geo;
        
        GFE_MatchBBoxXform bboxXform;
        GFE_XformByAttrib xformByAttrib(geo, nullptr, cookparms);
        xformByAttrib.groupParser.setGroup(groupParser);
        
        bboxXform.setRestBBox(*geoRest, groupParser.getPointRange(), posAttrib);
        
        const size_t sizeAttrib = getOutAttribArray().size();
        for (size_t i = 0; i < sizeAttrib; ++i)
        {
            attrib = getOutAttribArray()[i];

            
            geo->stdBoundingBox<>();
        
            GFE_Bound::setStd(bboxXform.restBBox);
            geoRest->enlargeBoundingBox(bboxXform.restBBox, groupParser.getPointRange(), posAttrib);

        
        
            if (geoRef0)
            {
                GFE_Bound::setStd(bboxXform.refBBox);
                geoRef0.enlargeBoundingBox(bboxXform.refBBox, groupParserRef0.getPointRange(), posRef0Attrib);
            }
            else
                bboxXform.refBBox.setBounds(-0.5, -0.5, -0.5, 0.5, 0.5, 0.5);

        
            if (xformAttrib && i == 0)
            {
                const GA_RWHandleT<UT_Matrix4T<fpreal>> xform_h(xformAttrib);
                xform_h.set(0, xform);
            }
        
            xformByAttrib.getOutAttribArray().set(attrib);
            xformByAttrib.setXformAttrib();
            xformByAttrib.setXform4(xform);
            //xformByAttrib.setComputeParm(false, false, false);
            xformByAttrib.compute();
        }

        return true;
    }
    

public:
    bool doTranslate = false;
    bool doScale = false;
    bool ignoreHFHeight = false;
    
private:
    bool repairPrecision = true;
    fpreal repairPrecisionThreshold = 1e-05;

    
private:
    UT_Matrix4T<fpreal> xform;
    GA_Attribute* xformAttrib = nullptr;
    GA_Attribute* attrib;
    const GFE_Detail* geoRest;
    
    exint subscribeRatio = 64;
    exint minGrainSize = 1024;


}; // End of class GFE_MatchBBox


#endif
