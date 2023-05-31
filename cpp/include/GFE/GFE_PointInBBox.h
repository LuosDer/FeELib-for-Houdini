
#pragma once

#ifndef __GFE_PointInBBox_h__
#define __GFE_PointInBBox_h__

#include "GFE/GFE_PointInBBox.h"


#include "GFE/GFE_GeoFilter.h"

#include "GFE/GFE_Bound.h"



class GFE_PointInBBox : public GFE_AttribFilterWithRef {

public:
	//using GFE_AttribFilterWithRef::GFE_AttribFilterWithRef;
	
	GFE_PointInBBox(
		GA_Detail& geo,
		const GA_Detail* const geoRef,
		const SOP_NodeVerb::CookParms* const cookparms = nullptr
	)
		: GFE_AttribFilterWithRef(geo, geoRef, cookparms)
		, groupParserBound(geoRef ? geoRef : &geo, groupParser.getGOPRef(), cookparms)
	{
	}

	GFE_PointInBBox(
		GA_Detail& geo,
		const GA_Detail* const geoRef,
		const SOP_NodeVerb::CookParms& cookparms
	)
		: GFE_AttribFilterWithRef(geo, geoRef, cookparms)
		, groupParserBound(geoRef ? geoRef : &geo, groupParser.getGOPRef(), cookparms)
	{
	}


	
    void
        setComputeParm(
			const char numInBoundMin = 1,
            const bool delElement = true,
			exint subscribeRatio = 64,
			exint minGrainSize = 1024
        )
    {
        setHasComputed();
        this->numInBoundMin = numInBoundMin;
        this->doDelGroupElement = delElement;
        this->subscribeRatio = subscribeRatio;
        this->minGrainSize = minGrainSize;
    }


#define GFE_PointInBBox_GETFUN_SPECILIZATION(NAME_LOWER, NAME_UPPER)                \
		public:                                                                     \
			bool NAME_LOWER = false;                                                \
			fpreal NAME_LOWER##Threshold = false;                                   \
		                                                                            \
		public:                                                                     \
			SYS_FORCE_INLINE void set##NAME_UPPER(const fpreal threshold)			\
			{ NAME_LOWER = true; NAME_LOWER##Threshold = threshold; }				\
																					\
			SYS_FORCE_INLINE void set##NAME_UPPER()									\
			{ NAME_LOWER = false; }													\

        
	GFE_PointInBBox_GETFUN_SPECILIZATION(xMin, XMin);
	GFE_PointInBBox_GETFUN_SPECILIZATION(xMax, XMax);
	GFE_PointInBBox_GETFUN_SPECILIZATION(yMin, YMin);
	GFE_PointInBBox_GETFUN_SPECILIZATION(yMax, YMax);
	GFE_PointInBBox_GETFUN_SPECILIZATION(zMin, ZMin);
	GFE_PointInBBox_GETFUN_SPECILIZATION(zMax, ZMax);
    
#undef GFE_PointInBBox_GETFUN_SPECILIZATION



private:

    virtual bool
        computeCore() override
    {
    	if (getOutGroupArray().isEmpty() || !getOutGroupArray()[0])
    		return true;
    	
    	if (!xMin && !xMax && !yMin && !yMax && !zMin && !zMax)
    		return true;

    	if (geo->getNumPoints() <= 0)
    		return true;
    	
    	if (!posAttrib)
    		posAttrib = geo->getP();
    	
    	if (geoRef0)
    	{
    		if (!posRefAttrib)
    			posRefAttrib = geoRef0->getP();
    		bbox = geoRef0->stdBoundingBox<float>(groupParserBound.getElementGroup(), posRefAttrib);
    	}
        else
        {
        	bbox = geo->stdBoundingBox<float>(groupParserBound.getElementGroup(), posAttrib);
        }
    	//GFE_Bound::expandBounds(bbox, enlargeBBox);
    	bbox(0,0) += xMinThreshold;
    	bbox(0,1) += xMaxThreshold;
    	bbox(1,0) += yMinThreshold;
    	bbox(1,1) += yMaxThreshold;
    	bbox(2,0) += zMinThreshold;
    	bbox(2,1) += zMaxThreshold;
    	
    	//bbox.enlargeBounds(enlargeBBox);
    	//bbox.enlargeFloats(enlargeBBox);
    	
    	setGroup = getOutGroupArray()[0];
    	
        const GA_Storage storage = posAttrib->getAIFTuple()->getStorage(posAttrib);
    	switch (storage)
    	{
    	case GA_STORE_REAL16: pointInBBox<UT_Vector3T<fpreal16>>(); break;
    	case GA_STORE_REAL32: pointInBBox<UT_Vector3T<fpreal32>>(); break;
    	case GA_STORE_REAL64: pointInBBox<UT_Vector3T<fpreal64>>(); break;
    	default: break;
    	}
    	
    	if (doDelGroupElement)
    		delGroupElement();

        return true;
    }


	template<typename VECTOR_T>
	void pointInBBox()
    {
    	UTparallelFor(groupParser.getPointSplittableRange(), [this](const GA_SplittableRange& r)
		{
			GA_PageHandleT<VECTOR_T, typename VECTOR_T::value_type, true, false, const GA_Attribute, const GA_ATINumeric, const GA_Detail> pos_ph(posAttrib);
			for (GA_PageIterator pit = r.beginPages(); !pit.atEnd(); ++pit)
			{
				GA_Offset start, end;
				for (GA_Iterator it(pit.begin()); it.blockAdvance(start, end); )
				{
					pos_ph.setPage(start);
					for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
					{
						uint8 numInLimit = 0;
						if (xMin && pos_ph.value(elemoff)[0] >= bbox.xmin())
							++numInLimit;
						if (xMax && pos_ph.value(elemoff)[0] <= bbox.xmax())
							++numInLimit;
						if (yMin && pos_ph.value(elemoff)[1] >= bbox.ymin())
							++numInLimit;
						if (yMax && pos_ph.value(elemoff)[1] <= bbox.ymax())
							++numInLimit;
						if (zMin && pos_ph.value(elemoff)[2] >= bbox.zmin())
							++numInLimit;
						if (zMax && pos_ph.value(elemoff)[2] <= bbox.zmax())
							++numInLimit;
						
    					setGroup.set(elemoff, numInLimit >= numInBoundMin);
					}
				}
			}
		}, subscribeRatio, minGrainSize);

    }


public:
	GFE_GroupParser groupParserBound;
	GFE_SetGroup setGroup;
	
	//UT_BoundingBoxT<float> enlargeBBox = UT_BoundingBoxT<float>(0, 0, 0, 0, 0, 0);
	//
	// bool xMin = false;
	// bool xMax = false;
	// bool yMin = false;
	// bool yMax = false;
	// bool zMin = false;
	// bool zMax = false;

private:
	
	
	//uint8 numInLimit = 0;
	
	uint8 numInBoundMin = 1;
	
	//const GA_Detail* geoSrcTmp;
	//GU_Detail* geoSrcTmpGU;
	
	//GU_DetailHandle geoSrcTmp_h;
	
	UT_BoundingBoxT<float> bbox;
	
	const GA_Attribute* posRefAttrib = nullptr;
	
    exint subscribeRatio = 64;
    exint minGrainSize = 1024;


}; // End of class GFE_PointInBBox


#endif
