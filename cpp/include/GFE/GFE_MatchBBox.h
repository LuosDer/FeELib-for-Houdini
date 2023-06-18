
#pragma once

#ifndef __GFE_MatchBBox_h__
#define __GFE_MatchBBox_h__

#include "GFE/GFE_MatchBBox.h"

#include "GFE/GFE_GeoFilter.h"


#include "UT/UT_BoundingBox.h"

#include "GFE/GFE_Bound.h"
#include "GFE/GFE_XformByAttrib.h"



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
        xform.
        UT_Vector3T<fpreal> center0 = restBBox.center();
        UT_Vector3T<fpreal> center1 = refBBox.center();
        UT_Vector3T<fpreal> size0   = restBBox.size();
        UT_Vector3T<fpreal> size1   = refBBox.size();
        xform.translate(center1-center0);
    }
    
    
public:
    UT_BoundingBoxT<fpreal> restBBox;
    UT_BoundingBoxT<fpreal> refBBox;
    UT_Matrix4T<fpreal> xform;
    
}; // End of class GFE_MatchBBoxXform



class GFE_MatchBBox : public GFE_AttribFilterWithRefRest {

public:
    using GFE_AttribFilterWithRefRest::GFE_AttribFilterWithRefRest;
    

    void
        setComputeParm(
        )
    {
        setHasComputed();
        this->subscribeRatio = subscribeRatio;
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

    // can not use in parallel unless for each GA_Detail
    virtual bool
        computeCore() override
    {
        if (getOutAttribArray().isEmpty())
            return false;

        if (groupParser.isEmpty())
            return true;

        UT_ASSERT_MSG_P(!xformAttrib ||
                       (xformAttrib->getTupleSize() == 16 &&
                        xformAttrib->getStorageClass() == GA_STORECLASS_REAL &&
                        xformAttrib->getAIFTuple()), "not correct xform attrib type");
        

        
        GFE_MatchBBoxXform bboxXform;
        
        GFE_Bound::setStd(bboxXform.restBBox);
        (geoRest ? geoRest : geo)->enlargeBoundingBox(bboxXform.restBBox, groupParser.getPointRange(), posAttrib);
        
        if (geoRef0)
        {
            GFE_Bound::setStd(bboxXform.refBBox);
            geoRef0.enlargeBoundingBox(bboxXform.refBBox, groupParserRef0.getPointRange(), posRef0Attrib);
        }
        else
            bboxXform.refBBox.setBounds(-0.5, -0.5, -0.5, 0.5, 0.5, 0.5);

        
        if (xformAttrib)
        {
            const GA_RWHandleT<UT_Matrix4T<fpreal>> xform_h(xformAttrib);
            xform_h.set(0, xform);
        }
        
        GFE_XformByAttrib xformByAttrib(geo, nullptr, cookparms);
        xformByAttrib.getOutAttribArray().append(getOutAttribArray());
        xformByAttrib.setXformAttrib();
        xformByAttrib.setXform4(xform);
        //xformByAttrib.setComputeParm(false, false, false);
        xformByAttrib.groupParser.setGroup(groupParser);
        xformByAttrib.compute();

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
    
    


}; // End of class GFE_MatchBBox


#endif
