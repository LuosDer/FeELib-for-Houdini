
#pragma once

#ifndef __GFE_PointInBBox_h__
#define __GFE_PointInBBox_h__

#include "GFE/GFE_PointInBBox.h"


#include "GFE/GFE_GeoFilter.h"

//#include "GFE/GFE_Group.h"



class GFE_PointInBBox : public GFE_AttribFilterWithRef {

public:
	using GFE_AttribFilterWithRef::GFE_AttribFilterWithRef;
	
	
    void
        setComputeParm(
            const bool reverseGroup = true,
			const GFE_GroupMergeType groupMergeType = GFE_GroupMergeType::Replace,
            const bool delElement = true,
			exint subscribeRatio = 64,
			exint minGrainSize = 1024
        )
    {
        setHasComputed();
        setGroup.reverseGroup = reverseGroup;
    	setGroup.groupMergeType = groupMergeType;
        this->doDelGroupElement = delElement;
        this->subscribeRatio = subscribeRatio;
        this->minGrainSize = minGrainSize;
    }


#define GFE_PointInBBox_GETFUN_SPECILIZATION(NAME_LOWER, NAME_UPPER)                \
		SYS_FORCE_INLINE void set##NAME_UPPER(const fpreal threshold)				\
		{ NAME_LOWER = true; NAME_LOWER##Threshold = threshold; }					\
																					\
		SYS_FORCE_INLINE void set##NAME_UPPER()										\
		{ NAME_LOWER = false; }														\

        
	GFE_PointInBBox_GETFUN_SPECILIZATION(xn, XN);
	GFE_PointInBBox_GETFUN_SPECILIZATION(xp, XP);
	GFE_PointInBBox_GETFUN_SPECILIZATION(yn, YN);
	GFE_PointInBBox_GETFUN_SPECILIZATION(yp, YP);
	GFE_PointInBBox_GETFUN_SPECILIZATION(zn, ZN);
	GFE_PointInBBox_GETFUN_SPECILIZATION(zp, ZP);
    
#undef GFE_PointInBBox_GETFUN_SPECILIZATION



private:

    virtual bool
        computeCore() override
    {
    	if (getOutGroupArray().isEmpty() || !getOutGroupArray()[0])
    		return true;
    	
    	if (!xn && !xp && !yn && !yp && !zn && !zp)
    		return true;

    	if (geo->getNumPoints() <= 0)
    		return true;
    	
    	if (!posAttrib)
    		posAttrib = geo->getP();

    	
    	UT_BoundingBoxT<float> bbox;
    	if (geoRef0)
    	{
    		if (!posRefAttrib)
    			posRefAttrib = geoRef0->getP();
    		bbox = geoRef0->stdBoundingBox<float>(groupParserRef0.getElementGroup(), posRefAttrib);
    	}
        else
        {
        	bbox = geoRef0->stdBoundingBox<float>(groupParser.getElementGroup(), posAttrib);
        }

    	setGroup = getOutGroupArray()[0];

    	if (xn)
    	{
    		if ( @P.x <= bound[0] ) {
    			numingroup++;
    		}
    	}

    	ut
    	setGroup.set();
    	
    	if (doDelGroupElement)
    		delGroupElement();

        return true;
    }



public:
	
	bool xn = false;
	bool xp = false;
	bool yn = false;
	bool yp = false;
	bool zn = false;
	bool zp = false;
	
	fpreal xnThreshold = 1e-05;
	fpreal xpThreshold = 1e-05;
	fpreal ynThreshold = 1e-05;
	fpreal ypThreshold = 1e-05;
	fpreal znThreshold = 1e-05;
	fpreal zpThreshold = 1e-05;

private:
	
	GFE_SetGroup setGroup;
	
	const GA_Detail* geoSrcTmp;
	GU_Detail* geoSrcTmpGU;
	
	GU_DetailHandle geoSrcTmp_h;
	
	const GA_Attribute* posRefAttrib = nullptr;
	
    exint subscribeRatio = 64;
    exint minGrainSize = 1024;


}; // End of class GFE_PointInBBox


#endif
