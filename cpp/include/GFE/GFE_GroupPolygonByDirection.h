
#pragma once

#ifndef __GFE_GroupPolygonByDirection_h__
#define __GFE_GroupPolygonByDirection_h__

#include "GFE/GFE_GroupPolygonByDirection.h"


#include "GFE/GFE_GeoFilter.h"
#include "GFE/GFE_Normal3D.h"


#include "GU/GU_RayIntersect.h"

#include "GFE/GFE_RestDir2D.h"


class GFE_GroupPolyByDir : public GFE_AttribFilterWithRef {

public:
	using GFE_AttribFilterWithRef::GFE_AttribFilterWithRef;


	void
		setComputeParm(
			const UT_Vector3R& dir = UT_Vector3R(0,1,0),
			const bool reversePrim = false,
			const exint subscribeRatio = 64,
			const exint minGrainSize = 1024
		)
	{
		setHasComputed();
		this->dir = dir;
		this->reversePrim = reversePrim;
		this->subscribeRatio = subscribeRatio;
		this->minGrainSize = minGrainSize;
	}

	SYS_FORCE_INLINE GA_Attribute* findOrCreateTuple(
		const bool detached = false,
		const UT_StringRef& attribName = ""
	)
	{ return getOutAttribArray().findOrCreateTuple(false, GA_ATTRIB_DETAIL,
			GA_STORECLASS_INT, GA_STORE_INVALID, attribName); }

	
    
	virtual void bumpDataId() const override
	{
		getOutAttribArray().bumpDataId();
		getOutGroupArray().bumpDataId();
		if (reversePrim && !(meshWindingCorrect ^ reverseOutGroup))
			bumpDataIdsForAddOrRemove(false, true, false);
	}

private:

	virtual bool
		computeCore() override
	{
		if (groupParser.isEmpty())
			return true;

		if (geoRef0)
		{
		}
		else
		{
			
		}
	
		GFE_Normal3D normal3D(geo, cookparms);
		normal3D.set
		GA_Attribute* const normalAttrib = getOutAttribArray()[0];
		
		outPrimGroup = getOutGroupArray().getPrimitiveGroup(0);
		switch (method)
		{
			case GFE_GroupPolyByDirMethod::RestDir2D_AvgNormal:    volume(); break;
			case GFE_GroupPolyByDirMethod::RestDir2D_HouOBB:
			case GFE_GroupPolyByDirMethod::Ray:                    wn();     break;
			default: break;
		}

		if (reversePrim)
		{
			geo->asGU_Detail()->reverse(outPrimGroup, false);
			//geo->asGU_Detail()->reversePolys(outPrimGroup);
		}
		
		return true;
	}
	
	void groupByPolyWindingMethod_restDir2D_avgNormal()
	{
	}

	template<typename FLOAT_T>
	void groupByDirMethod()
	{
		GFE_RestDir2D restDir2D();
		
		GEOcomputeNormals(*geo->asGEO_Detail(), normalAttrib, groupParser.getGroup(normalAttrib), cuspAngleDegrees, normalMethod, copyOrigIfZero);

		
		GA_ROHandleT<UT_Vector3T<FLOAT_T>> pos_h(geo->getP());
		const GA_PrimitiveGroup* const geoPrimGroup = groupParser.getPrimitiveGroup();
		UTparallelFor(groupParser.getPrimitiveSplittableRange(), [this, geoPrimGroup, normalAttrib](const GA_SplittableRange& r)
		{
			GU_RayInfo rayInfo;
			const GU_RayIntersect rayIntersect(geo->asGU_Detail(), geoPrimGroup, true, false, false, false, false);
			GA_PageHandleT<UT_Vector3T<FLOAT_T>, typename UT_Vector3T<FLOAT_T>::value_type, true, false, const GA_Attribute, const GA_ATINumeric, const GA_Detail> normal_ph(normalAttrib);
			for (GA_PageIterator pit = r.beginPages(); !pit.atEnd(); ++pit)
			{
				GA_Offset start, end;
				for (GA_Iterator it(pit.begin()); it.blockAdvance(start, end); )
				{
					normal_ph.setPage(start);
					for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
					{
						const UT_Vector3T<FLOAT_T>& origPos = geo->getPos3D(geo->vertexPoint(geo->getPrimitiveVertexOffset(elemoff, 0)));
						UT_Vector3T<FLOAT_T> dir = normal_ph.value(elemoff);
						dir.normalize();
						int numIntersect = rayIntersect.sendRay(origPos + dir * 0.0001, dir * 100000, rayInfo);
					
						outPrimGroup->setElement(elemoff, bool(numIntersect % 2) ^ reverseOutGroup);
					}
				}
			}
		}, subscribeRatio, minGrainSize);
	}
	
	void groupByPolyWindingMethod_restDir2D_houOBB()
	{
	}
	

	void groupByPolyWindingMethod_restDir2D()
	{
		const UT_Vector3T<T>& dir2D = GFE_RestDir2D::restDir2D<T>(geo, geoPrimGroup, restDir2DMethod);
		UTparallelFor(groupParser.getPrimitiveSplittableRange(), [geo, reverseOutGroup, normalAttrib, &dir2D, reverseGroup](const GA_SplittableRange& r)
		{
			GA_PageHandleT<UT_Vector3T<T>, typename UT_Vector3T<T>::value_type, true, false, const GA_Attribute, const GA_ATINumeric, const GA_Detail> normal_ph(normalAttrib);
			for (GA_PageIterator pit = r.beginPages(); !pit.atEnd(); ++pit)
			{
				GA_Offset start, end;
				for (GA_Iterator it(pit.begin()); it.blockAdvance(start, end); )
				{
					normal_ph.setPage(start);
					for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
					{
	#if 0
						const bool diffDir = dot(normal_ph.value(elemoff), dir2D) < 0;
	#else
						const UT_Vector3T<T>& value = normal_ph.value(elemoff);
						const T valueDot = dot(value, dir2D);
						const bool diffDir = valueDot < 0;
	#endif
						reverseOutGroup->setElement(elemoff, diffDir ^ reverseGroup);
					}
				}
			}
		}, subscribeRatio, minGrainSize);
	}
	

public:
	GFE_SetGroup setGroup;
	
	UT_Vector3R dir = UT_Vector3R(0,1,0);
	bool reversePrim = false;
	
	bool outIntermediateAttrib = false;
	
	GA_Attribute* normalAttrib;
	GA_PrimitiveGroup* outPrimGroup;
	
private:
	bool meshWindingCorrect = true;

	exint subscribeRatio = 64;
	exint minGrainSize = 1024;
	
}; // End of class GFE_GroupPolyByDir





#endif
