
#pragma once

#ifndef __GFE_GroupByDirection_h__
#define __GFE_GroupByDirection_h__

#include "GFE/GFE_GroupByDirection.h"


#include "GFE/GFE_GeoFilter.h"
#include "GFE/GFE_Normal3D.h"


#include "GU/GU_RayIntersect.h"

#include "GFE/GFE_RestDir2D.h"


enum class GFE_GroupByDirMethod
{
	RestDir2D_AvgNormal,
	RestDir2D_HouOBB,
	Ray,
};



class GFE_GroupByDir : public GFE_AttribFilterWithRef {

public:
	using GFE_AttribFilterWithRef::GFE_AttribFilterWithRef;


	void
		setComputeParm(
			const GFE_GroupByDirMethod method = GFE_GroupByDirMethod::VOLUME,
			
			const bool useConstant = false,
			const bool reverseGroup = false,
			const bool reversePrim = false,
			const bool meshCap = false
		)
	{
		setHasComputed();
		this->method = method;
			
		this->reverseOutGroup = reverseGroup;
		this->reversePrim = reversePrim;
		this->meshCap = meshCap;
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
			case GFE_GroupByDirMethod::RestDir2D_AvgNormal:    volume(); break;
			case GFE_GroupByDirMethod::RestDir2D_HouOBB:
			case GFE_GroupByDirMethod::Ray:                    wn();     break;
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
	GFE_GroupByDirMethod method = GFE_GroupByDirMethod::RestDir2D_AvgNormal;
	
	bool reversePrim = false;
	bool meshCap = false;
	
	bool outIntermediateAttrib = false;
	
	GA_Attribute* normalAttrib;
	GA_PrimitiveGroup* outPrimGroup;
	
private:
	bool meshWindingCorrect = true;

	exint subscribeRatio = 64;
	exint minGrainSize = 1024;
	
}; // End of class GFE_GroupByDir






//groupByPolyWinding(geo, group, reverseGroup, delGroup);
template<typename T>
SYS_FORCE_INLINE
static void
groupByPolyWinding_ray(
	GA_Detail* const geo,
	GA_PrimitiveGroup* const reverseOutGroup,
	const GA_Attribute* const normalAttrib,
	const GA_PrimitiveGroup* const geoPrimGroup = nullptr,
	const bool reverseGroup = false,
	const bool reversePrim = false,
	const exint subscribeRatio = 64,
	const exint minGrainSize = 1024
)
{
	groupByPolyWinding_ray<T>(geo, reverseOutGroup, normalAttrib,
		geoPrimGroup, reverseGroup,
		subscribeRatio, minGrainSize);

	if (reversePrim)
	{
		//GU_Detail* const geoGU = static_cast<GU_Detail*>(geo);
		//geoGU->reversePolys(reverseOutGroup);
		static_cast<GU_Detail*>(geo)->reversePolys(reverseOutGroup);
	}
}






//groupByPolyWinding(geo, group, reverseGroup, delGroup);
template<typename T>
static void
groupByPolyWinding_avgNormal(
	const GA_Detail* const geo,
	GA_PrimitiveGroup* const reverseOutGroup,
	const GA_PrimitiveGroup* const geoPrimGroup = nullptr,
	const bool reverseGroup = false,
	const bool reversePrim = false,
	const exint subscribeRatio = 64,
	const exint minGrainSize = 1024
)
{
	UT_ASSERT_P(geo);
	UT_ASSERT_P(reverseOutGroup);
	GA_ATINumericUPtr normalAttribUPtr = geo->getAttributes().createDetachedTupleAttribute(GA_ATTRIB_PRIMITIVE, GA_STORE_REAL64, 1);
	GA_Attribute* const normalAttrib = normalAttribUPtr.get();

	GEOcomputeNormals(*static_cast<const GEO_Detail*>(geo), normalAttrib, geoPrimGroup,
		GEO_DEFAULT_ADJUSTED_CUSP_ANGLE, GEO_NormalMethod::ANGLE_WEIGHTED, false);

	const GA_ROHandleT<UT_Vector3T<T>> normal_h(normalAttrib);

	groupByPolyWinding_restDir2D<T>(geo, normalAttrib, reverseOutGroup,
		geoPrimGroup, GFE_RestDir2D_AvgNormal,
		reverseGroup, reversePrim,
		subscribeRatio, minGrainSize);
}

















//groupByPolyWinding(geo, group, reverseGroup, delGroup);
SYS_FORCE_INLINE
static void
groupByPolyWinding(
	const GA_Detail* const geo,
	GA_PrimitiveGroup* const reverseOutGroup,
	const GA_Attribute* normalAttrib,
	const GA_PrimitiveGroup* const geoPrimGroup = nullptr,
	const GFE_GroupByDirMethod method = GFE_GroupByDirMethod_RestDir2D_AvgNormal,
	const bool reverseGroup = false,
	const exint subscribeRatio = 64,
	const exint minGrainSize = 1024
)
{
	GA_ATINumericUPtr normalAttribUPtr;
	if (!normalAttrib)
	{
		normalAttribUPtr = geo->getAttributes().createDetachedTupleAttribute(GA_ATTRIB_PRIMITIVE, GA_STORE_REAL64, 1);
		GA_Attribute* const finalNormalAttrib = normalAttribUPtr.get();
		GEOcomputeNormals(*static_cast<const GEO_Detail*>(geo), finalNormalAttrib, geoPrimGroup,
			GEO_DEFAULT_ADJUSTED_CUSP_ANGLE, GEO_NormalMethod::ANGLE_WEIGHTED, false);
		normalAttrib = finalNormalAttrib;
	}
	switch (method)
	{
	case GFE_GroupByDirMethod_RestDir2D_AvgNormal:
		groupByPolyWinding_restDir2D<fpreal>(geo, reverseOutGroup, normalAttrib,
			geoPrimGroup, GFE_RestDir2D_AvgNormal,
			reverseGroup,
			subscribeRatio, minGrainSize);
		break;
	case GFE_GroupByDir_RestDir2D_HouOBB:
		groupByPolyWinding_restDir2D<fpreal>(geo, reverseOutGroup, normalAttrib,
			geoPrimGroup, GFE_RestDir2D_HouOBB,
			reverseGroup,
			subscribeRatio, minGrainSize);
		break;
	case GFE_GroupByDir_Ray:
		groupByPolyWinding_ray<fpreal>(geo, reverseOutGroup, normalAttrib,
			geoPrimGroup,
			reverseGroup,
			subscribeRatio, minGrainSize);
		break;
	}
}


//groupByPolyWinding(geo, group, reverseGroup, delGroup);
SYS_FORCE_INLINE
static void
groupByPolyWinding(
	GA_Detail* const geo,
	GA_PrimitiveGroup* const reverseOutGroup,
	const GA_Attribute* const normalAttrib,
	const GA_PrimitiveGroup* const geoPrimGroup = nullptr,
	const GFE_GroupByDirMethod method = GFE_GroupByDirMethod_RestDir2D_AvgNormal,
	const bool reverseGroup = false,
	const bool reversePrim = false,
	const exint subscribeRatio = 64,
	const exint minGrainSize = 1024
)
{
	groupByPolyWinding(static_cast<const GA_Detail*>(geo), reverseOutGroup, normalAttrib,
		geoPrimGroup, method, reverseGroup,
		subscribeRatio, minGrainSize);

	if (reversePrim)
	{
		static_cast<GU_Detail*>(geo)->reversePolys(reverseOutGroup);
	}
}



//groupByPolyWinding(geo, group, reverseGroup, delGroup);
static GA_PrimitiveGroup*
groupByPolyWinding(
	GA_Detail* const geo,
	const GA_Attribute* const normalAttrib,
	const GA_PrimitiveGroup* const geoPrimGroup = nullptr,
	const GFE_GroupByDirMethod method = GFE_GroupByDirMethod_RestDir2D_AvgNormal,

	const bool outReversedGroup = true,
	const UT_StringHolder& reversedGroupName = "reversed",
	const bool reverseGroup = false,
	const bool reversePrim = false,
	const exint subscribeRatio = 64,
	const exint minGrainSize = 1024
)
{
	UT_ASSERT_P(geo);
	if (!outReversedGroup && !reversePrim)
		return nullptr;

	GA_PrimitiveGroupUPtr reversedGroupUPtr;
	GA_PrimitiveGroup* reverseOutGroup = nullptr;
	if (outReversedGroup)
	{
		reverseOutGroup = geo->newPrimitiveGroup(reversedGroupName);
	}
	else
	{
		reversedGroupUPtr = geo->createDetachedPrimitiveGroup();
		reverseOutGroup = reversedGroupUPtr.get();
	}

	groupByPolyWinding(geo, reverseOutGroup, normalAttrib, geoPrimGroup,
		method, reverseGroup, reversePrim,
		subscribeRatio, minGrainSize);

	if (!outReversedGroup)
	{
		reverseOutGroup = nullptr;
	}
	return reverseOutGroup;
} 




//groupByPolyWinding(geo, group, reverseGroup, delGroup);
SYS_FORCE_INLINE
static GA_PrimitiveGroup*
groupByPolyWinding(
	GA_Detail* const geo,
	const UT_StringHolder& normalAttribName = "N",
	const GA_PrimitiveGroup* const geoPrimGroup = nullptr,
	const GFE_GroupByDirMethod method = GFE_GroupByDirMethod_RestDir2D_AvgNormal,

	const bool outReversedGroup = true,
	const UT_StringHolder& reversedGroupName = "reversed",
	const bool reverseGroup = false,
	const bool reversePrim = false,
	const exint subscribeRatio = 64,
	const exint minGrainSize = 1024
)
{
	const GA_Attribute* const normalAttrib = geo->findPrimitiveAttribute(normalAttribName);

	return groupByPolyWinding(geo, normalAttrib, geoPrimGroup,
		method, outReversedGroup, reversedGroupName, reverseGroup, reversePrim,
		subscribeRatio, minGrainSize);
}




//groupByPolyWinding(geo, group, reverseGroup, delGroup);
static GA_PrimitiveGroup*
groupByPolyWinding(
	const SOP_NodeVerb::CookParms& cookparms,
	GA_Detail* const geo,
	const UT_StringHolder& groupName,
	const UT_StringHolder& normalAttribName = "N",
	const GFE_GroupByDirMethod method = GFE_GroupByDirMethod_RestDir2D_AvgNormal,
	const bool outReversedGroup = true,
	const UT_StringHolder& reversedGroupName = "reversed",
	const bool reverseGroup = false,
	const bool reversePrim = false,
	const exint subscribeRatio = 64,
	const exint minGrainSize = 1024
)
{
	if (!outReversedGroup && !reversePrim)
		return nullptr;

	GOP_Manager gop;
	const GA_PrimitiveGroup* const geoPrimGroup = GFE_GroupParse_Namespace::findOrParsePrimitiveGroupDetached(cookparms, geo, groupName, gop);

	GA_PrimitiveGroup* const reverseOutGroup = groupByPolyWinding(geo, normalAttribName, geoPrimGroup,
		//runOverPieces, findInputPieceAttrib, pieceAttribName,
		method, outReversedGroup, reversedGroupName, reverseGroup, reversePrim,
		subscribeRatio, minGrainSize);

	if (outReversedGroup)
	{
		cookparms.getNode()->setHighlight(true);
		cookparms.select(*reverseOutGroup);
	}

	return reverseOutGroup;
}





} // End of namespace GFE_GroupByDir

#endif
