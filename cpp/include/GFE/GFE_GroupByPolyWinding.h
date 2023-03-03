
#pragma once

#ifndef __GFE_GroupByPolyWinding_h__
#define __GFE_GroupByPolyWinding_h__

//#include "GFE/GFE_GroupByPolyWinding.h"

#include "GA/GA_Detail.h"
#include "GA/GA_PageHandle.h"


#include "GU/GU_RayIntersect.h"

#include "GFE/GFE_GroupParse.h"
#include "GFE/GFE_RestDir2D.h"


enum GFE_GroupByPolyWindingMethod
{
	GFE_GroupByPolyWindingMethod_RestDir2D_AvgNormal,
	GFE_GroupByPolyWindingMethod_RestDir2D_HouOBB,
	GFE_GroupByPolyWindingMethod_Ray,
};




namespace GFE_GroupByPolyWinding {











//groupByPolyWinding(geo, group, reverseGroup);
template<typename T>
static void
groupByPolyWinding(
	const GA_Detail* const geo,
	GA_PrimitiveGroup* const reversedGroup,
	const GA_Attribute* const normalAttrib,
	const UT_Vector3T<T>& dir2D,
	const GA_PrimitiveGroup* const geoPrimGroup = nullptr,
	const bool reverseGroup = false,
	const exint subscribeRatio = 64,
	const exint minGrainSize = 1024
)
{
	//const GA_Attribute* const normalAttrib = normal_h.getAttribute();
	UT_ASSERT_P(geo);
	UT_ASSERT_P(reversedGroup);
	UT_ASSERT_P(normalAttrib);

	const GA_SplittableRange geoSplittableRange0(geo->getPrimitiveRange(geoPrimGroup));
	UTparallelFor(geoSplittableRange0, [geo, reversedGroup, normalAttrib, &dir2D, reverseGroup](const GA_SplittableRange& r)
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
					reversedGroup->setElement(elemoff, diffDir ^ reverseGroup);
				}
			}
		}
		//GA_Offset start, end;
		//for (GA_Iterator it(r); it.blockAdvance(start, end); )
		//{
		//	for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
		//	{
		//	}
		//}
	}, subscribeRatio, minGrainSize);
}


//groupByPolyWinding(geo, group, reverseGroup);
template<typename T>
SYS_FORCE_INLINE
static void
groupByPolyWinding(
	GA_Detail* const geo,
	GA_PrimitiveGroup* const reversedGroup,
	const GA_Attribute* const normalAttrib,
	const UT_Vector3T<T>& dir2D,
	const GA_PrimitiveGroup* const geoPrimGroup = nullptr,
	const bool reverseGroup = false,
	const bool reversePrim = false,
	const exint subscribeRatio = 64,
	const exint minGrainSize = 1024
)
{
	groupByPolyWinding<T>(geo, reversedGroup, normalAttrib, dir2D,
		geoPrimGroup, reverseGroup,
		subscribeRatio, minGrainSize);
	
	if (reversePrim)
	{
		//GU_Detail* const geoGU = static_cast<GU_Detail*>(geo);
		//geoGU->reversePolys(reversedGroup);
		static_cast<GU_Detail*>(geo)->reversePolys(reversedGroup);
	}
}



//groupByPolyWinding(geo, group, reverseGroup, delGroup);
template<typename T>
SYS_FORCE_INLINE
static void
groupByPolyWinding_restDir2D(
	const GA_Detail* const geo,
	GA_PrimitiveGroup* const reversedGroup,
	const GA_Attribute* const normalAttrib,
	const GA_PrimitiveGroup* const geoPrimGroup = nullptr,
	const GFE_RestDir2D_Method restDir2DMethod = GFE_RestDir2D_AvgNormal,
	const bool reverseGroup = false,
	const exint subscribeRatio = 64,
	const exint minGrainSize = 1024
)
{
	const UT_Vector3T<T>& dir2D = GFE_RestDir2D::restDir2D<T>(geo, geoPrimGroup, restDir2DMethod);
	groupByPolyWinding<T>(geo, reversedGroup, normalAttrib, dir2D,
		geoPrimGroup, reverseGroup,
		subscribeRatio, minGrainSize);
}

//groupByPolyWinding(geo, group, reverseGroup, delGroup);
template<typename T>
SYS_FORCE_INLINE
static void
groupByPolyWinding_restDir2D(
	GA_Detail* const geo,
	GA_PrimitiveGroup* const reversedGroup,
	const GA_Attribute* const normalAttrib,
	const GA_PrimitiveGroup* const geoPrimGroup = nullptr,
	const GFE_RestDir2D_Method restDir2DMethod = GFE_RestDir2D_AvgNormal,
	const bool reverseGroup = false,
	const bool reversePrim = false,
	const exint subscribeRatio = 64,
	const exint minGrainSize = 1024
)
{
	const UT_Vector3T<T>& dir2D = GFE_RestDir2D::restDir2D<T>(geo, geoPrimGroup, restDir2DMethod);
	groupByPolyWinding<T>(geo, reversedGroup, normalAttrib, dir2D,
		geoPrimGroup, reverseGroup, reversePrim,
		subscribeRatio, minGrainSize);
}

//groupByPolyWinding(geo, group, reverseGroup, delGroup);
template<typename T>
static void
groupByPolyWinding_ray(
	const GA_Detail* const geo,
	GA_PrimitiveGroup* const reversedGroup,
	const GA_Attribute* const normalAttrib,
	const GA_PrimitiveGroup* const geoPrimGroup = nullptr,
	const bool reverseGroup = false,
	const exint subscribeRatio = 64,
	const exint minGrainSize = 1024
)
{
	UT_ASSERT_P(geo);
	UT_ASSERT_P(reversedGroup);
	UT_ASSERT_P(normalAttrib);
	const GA_SplittableRange geoSplittableRange0(geo->getPrimitiveRange(geoPrimGroup));
	UTparallelFor(geoSplittableRange0, [geo, reversedGroup, geoPrimGroup, normalAttrib, reverseGroup](const GA_SplittableRange& r)
	{
		GU_RayInfo rayInfo;
		const GU_RayIntersect rayIntersect(static_cast<const GU_Detail*>(geo), geoPrimGroup, true, false, false, false, false);
		GA_PageHandleT<UT_Vector3T<T>, typename UT_Vector3T<T>::value_type, true, false, const GA_Attribute, const GA_ATINumeric, const GA_Detail> normal_ph(normalAttrib);
		for (GA_PageIterator pit = r.beginPages(); !pit.atEnd(); ++pit)
		{
			GA_Offset start, end;
			for (GA_Iterator it(pit.begin()); it.blockAdvance(start, end); )
			{
				normal_ph.setPage(start);
				for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
				{
					const UT_Vector3T<T>& origPos = geo->getPos3D(geo->vertexPoint(geo->getPrimitiveVertexOffset(elemoff, 0)));
					UT_Vector3T<T> dir = normal_ph.value(elemoff);
					dir.normalize();
					int numIntersect = rayIntersect.sendRay(origPos + dir * 0.0001, dir * 100000, rayInfo);
					
					reversedGroup->setElement(elemoff, bool(numIntersect % 2) ^ reverseGroup);
				}
			}
		}
		//GA_Offset start, end;
		//for (GA_Iterator it(r); it.blockAdvance(start, end); )
		//{
		//	for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
		//	{
		//	}
		//}
	}, subscribeRatio, minGrainSize);
}

//groupByPolyWinding(geo, group, reverseGroup, delGroup);
template<typename T>
SYS_FORCE_INLINE
static void
groupByPolyWinding_ray(
	GA_Detail* const geo,
	GA_PrimitiveGroup* const reversedGroup,
	const GA_Attribute* const normalAttrib,
	const GA_PrimitiveGroup* const geoPrimGroup = nullptr,
	const bool reverseGroup = false,
	const bool reversePrim = false,
	const exint subscribeRatio = 64,
	const exint minGrainSize = 1024
)
{
	groupByPolyWinding_ray<T>(geo, reversedGroup, normalAttrib,
		geoPrimGroup, reverseGroup,
		subscribeRatio, minGrainSize);

	if (reversePrim)
	{
		//GU_Detail* const geoGU = static_cast<GU_Detail*>(geo);
		//geoGU->reversePolys(reversedGroup);
		static_cast<GU_Detail*>(geo)->reversePolys(reversedGroup);
	}
}






//groupByPolyWinding(geo, group, reverseGroup, delGroup);
template<typename T>
static void
groupByPolyWinding_avgNormal(
	const GA_Detail* const geo,
	GA_PrimitiveGroup* const reversedGroup,
	const GA_PrimitiveGroup* const geoPrimGroup = nullptr,
	const bool reverseGroup = false,
	const bool reversePrim = false,
	const exint subscribeRatio = 64,
	const exint minGrainSize = 1024
)
{
	UT_ASSERT_P(geo);
	UT_ASSERT_P(reversedGroup);
	GA_ATINumericUPtr normalAttribUPtr = geo->getAttributes().createDetachedTupleAttribute(GA_ATTRIB_PRIMITIVE, GA_STORE_REAL64, 1);
	GA_Attribute* const normalAttrib = normalAttribUPtr.get();

	GEOcomputeNormals(*static_cast<const GEO_Detail*>(geo), normalAttrib, geoPrimGroup,
		GEO_DEFAULT_ADJUSTED_CUSP_ANGLE, GEO_NormalMethod::ANGLE_WEIGHTED, false);

	const GA_ROHandleT<UT_Vector3T<T>> normal_h(normalAttrib);

	groupByPolyWinding_restDir2D<T>(geo, normalAttrib, reversedGroup,
		geoPrimGroup, GFE_RestDir2D_AvgNormal,
		reverseGroup, reversePrim,
		subscribeRatio, minGrainSize);
}

















//groupByPolyWinding(geo, group, reverseGroup, delGroup);
SYS_FORCE_INLINE
static void
groupByPolyWinding(
	const GA_Detail* const geo,
	GA_PrimitiveGroup* const reversedGroup,
	const GA_Attribute* normalAttrib,
	const GA_PrimitiveGroup* const geoPrimGroup = nullptr,
	const GFE_GroupByPolyWindingMethod method = GFE_GroupByPolyWindingMethod_RestDir2D_AvgNormal,
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
	case GFE_GroupByPolyWindingMethod_RestDir2D_AvgNormal:
		groupByPolyWinding_restDir2D<fpreal>(geo, reversedGroup, normalAttrib,
			geoPrimGroup, GFE_RestDir2D_AvgNormal,
			reverseGroup,
			subscribeRatio, minGrainSize);
		break;
	case GFE_GroupByPolyWinding_RestDir2D_HouOBB:
		groupByPolyWinding_restDir2D<fpreal>(geo, reversedGroup, normalAttrib,
			geoPrimGroup, GFE_RestDir2D_HouOBB,
			reverseGroup,
			subscribeRatio, minGrainSize);
		break;
	case GFE_GroupByPolyWinding_Ray:
		groupByPolyWinding_ray<fpreal>(geo, reversedGroup, normalAttrib,
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
	GA_PrimitiveGroup* const reversedGroup,
	const GA_Attribute* const normalAttrib,
	const GA_PrimitiveGroup* const geoPrimGroup = nullptr,
	const GFE_GroupByPolyWindingMethod method = GFE_GroupByPolyWindingMethod_RestDir2D_AvgNormal,
	const bool reverseGroup = false,
	const bool reversePrim = false,
	const exint subscribeRatio = 64,
	const exint minGrainSize = 1024
)
{
	groupByPolyWinding(static_cast<const GA_Detail*>(geo), reversedGroup, normalAttrib,
		geoPrimGroup, method, reverseGroup,
		subscribeRatio, minGrainSize);

	if (reversePrim)
	{
		static_cast<GU_Detail*>(geo)->reversePolys(reversedGroup);
	}
}



//groupByPolyWinding(geo, group, reverseGroup, delGroup);
static GA_PrimitiveGroup*
groupByPolyWinding(
	GA_Detail* const geo,
	const GA_Attribute* const normalAttrib,
	const GA_PrimitiveGroup* const geoPrimGroup = nullptr,
	const GFE_GroupByPolyWindingMethod method = GFE_GroupByPolyWindingMethod_RestDir2D_AvgNormal,

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
	GA_PrimitiveGroup* reversedGroup = nullptr;
	if (outReversedGroup)
	{
		reversedGroup = geo->newPrimitiveGroup(reversedGroupName);
	}
	else
	{
		reversedGroupUPtr = geo->createDetachedPrimitiveGroup();
		reversedGroup = reversedGroupUPtr.get();
	}

	groupByPolyWinding(geo, reversedGroup, normalAttrib, geoPrimGroup,
		method, reverseGroup, reversePrim,
		subscribeRatio, minGrainSize);

	if (!outReversedGroup)
	{
		reversedGroup = nullptr;
	}
	return reversedGroup;
} 




//groupByPolyWinding(geo, group, reverseGroup, delGroup);
SYS_FORCE_INLINE
static GA_PrimitiveGroup*
groupByPolyWinding(
	GA_Detail* const geo,
	const UT_StringHolder& normalAttribName = "N",
	const GA_PrimitiveGroup* const geoPrimGroup = nullptr,
	const GFE_GroupByPolyWindingMethod method = GFE_GroupByPolyWindingMethod_RestDir2D_AvgNormal,

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
	const GFE_GroupByPolyWindingMethod method = GFE_GroupByPolyWindingMethod_RestDir2D_AvgNormal,
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

	GA_PrimitiveGroup* const reversedGroup = groupByPolyWinding(geo, normalAttribName, geoPrimGroup,
		//runOverPieces, findInputPieceAttrib, pieceAttribName,
		method, outReversedGroup, reversedGroupName, reverseGroup, reversePrim,
		subscribeRatio, minGrainSize);

	if (outReversedGroup)
	{
		cookparms.getNode()->setHighlight(true);
		cookparms.select(*reversedGroup);
	}

	return reversedGroup;
}





} // End of namespace GFE_GroupByPolyWinding

#endif
