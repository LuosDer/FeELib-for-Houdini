
#pragma once

#ifndef __GA_FeE_DelVertex_h__
#define __GA_FeE_DelVertex_h__

//#include "GA_FeE/GA_FeE_DelVertex.h"

#include "GA/GA_Detail.h"

#include "GA/GA_PageHandle.h"
#include "GA/GA_PageIterator.h"

#include "GA_FeE/GA_FeE_Type.h"


namespace GA_FeE_DelVertex {


	static void
		delVertex(
			GA_Detail* const geo,
			const GA_VertexGroup* const group,
			const bool delDegeneratePrims = true,
			const bool delUnusedPoints = true

		)
	{
		UT_ASSERT_P(geo);

		GA_Topology& topo = geo->getTopology();
		const GA_ATITopology* vtxPrimRef = topo.getPrimitiveRef();
		const GA_ATITopology* vtxPointRef = topo.getPointRef();

		const GA_PrimitiveGroupUPtr primGroupUPtr = geo->createDetachedPrimitiveGroup();
		GA_PrimitiveGroup* primGroup = primGroupUPtr.get();

		const GA_PointGroupUPtr pointGroupUPtr = geo->createDetachedPointGroup();
		GA_PointGroup* pointGroup = pointGroupUPtr.get();

		const GA_SplittableRange geoSplittableRange(geo->getVertexRange(group));
		UTparallelFor(geoSplittableRange, [&geo, &topo, &vtxPrimRef, &vtxPointRef, &primGroup, &pointGroup, delDegeneratePrims, delUnusedPoints](const GA_SplittableRange& r)
			{
				GA_Offset start, end;
				for (GA_Iterator it(r); it.blockAdvance(start, end); )
				{
					for (GA_Offset vtxoff = start; vtxoff < end; ++vtxoff)
					{
						GA_Offset primoff = vtxPrimRef->getLink(vtxoff);

						geo->getPrimitive(primoff)->releaseVertex(vtxoff);
						topo.delVertex(vtxoff);

						if (delDegeneratePrims)
							primGroup->setElement(primoff, true);
						if (delUnusedPoints)
							pointGroup->setElement(vtxPointRef->getLink(vtxoff), true);
					}
				}
			});

		if (delDegeneratePrims)
			geo->destroyDegeneratePrimitives(primGroup);
		if (delUnusedPoints)
			geo->destroyUnusedPoints(pointGroup);
	}

	static void
		delVertex(
			GA_Detail* const geo,
			const GA_PrimitiveGroup* const group,
			const bool delDegeneratePrims = true,
			const bool delUnusedPoints = true
		)
	{
		if (delDegeneratePrims)
			geo->destroyPrimitiveOffsets(geo->getPrimitiveRange(group), delUnusedPoints);
		else
		{
			//uinfinished
			geo->destroyPrimitiveOffsets(geo->getPrimitiveRange(group), delUnusedPoints);
		}
	}

	static void
		delVertex(
			GA_Detail* const geo,
			const GA_PointGroup* const group,
			const bool delDegeneratePrims = true,
			const bool delUnusedPoints = true
		)
	{
		const GA_VertexGroupUPtr vtxGroupUPtr = geo->createDetachedVertexGroup();
		GA_VertexGroup* vtxGroup = vtxGroupUPtr.get();
		vtxGroup->combine(group);
		delVertex(geo, vtxGroup, delDegeneratePrims, false);
		if (delUnusedPoints)
			geo->destroyPointOffsets(geo->getPointRange(group));
	}

	static void
		delVertex(
			GA_Detail* const geo,
			const GA_EdgeGroup* const group,
			const bool delDegeneratePrims = true,
			const bool delUnusedPoints = true
		)
	{
		const GA_VertexGroupUPtr vtxGroupUPtr = geo->createDetachedVertexGroup();
		GA_VertexGroup* vtxGroup = vtxGroupUPtr.get();
		vtxGroup->combine(group);
		delVertex(geo, vtxGroup, delDegeneratePrims, delUnusedPoints);
	}

	//static void
	//	delVertex(
	//		GA_Detail* const geo,
	//		const GA_Range& range,
	//		const bool delUnusedPoints = true
	//	)
	//{
	//	switch (range.getOwner())
	//	{
	//	case GA_ATTRIB_PRIMITIVE:
	//		return delVertex(geo, UTverify_cast<const GA_PrimitiveGroup*>(group),
	//			delUnusedPoints, delUnusedPoints);
	//		break;
	//	case GA_ATTRIB_POINT:
	//		return delVertex(geo, UTverify_cast<const GA_PointGroup*>(group),
	//			delUnusedPoints, delUnusedPoints);
	//		break;
	//	case GA_ATTRIB_VERTEX:
	//		return delVertex(geo, UTverify_cast<const GA_VertexGroup*>(group),
	//			delDegeneratePrims, delUnusedPoints);
	//		break;
	//	case GA_GROUP_EDGE:
	//		return delVertex(geo, UTverify_cast<const GA_EdgeGroup*>(group),
	//			delDegeneratePrims, delUnusedPoints);
	//		break;
	//	}
	//	UT_ASSERT("cant be possible");
	//}

static void
delVertex(
	GA_Detail* const geo,
	const GA_Group* const group,
	const bool delDegeneratePrims = true,
	const bool delUnusedPoints = true
)
{
	switch (group->classType())
	{
	case GA_GROUP_PRIMITIVE:
		return delVertex(geo, UTverify_cast<const GA_PrimitiveGroup*>(group),
			delUnusedPoints, delUnusedPoints);
		break;
	case GA_GROUP_POINT:
		return delVertex(geo, UTverify_cast<const GA_PointGroup*>(group),
			delDegeneratePrims, delUnusedPoints);
		break;
	case GA_GROUP_VERTEX:
		return delVertex(geo, UTverify_cast<const GA_VertexGroup*>(group),
			delDegeneratePrims, delUnusedPoints);
		break;
	case GA_GROUP_EDGE:
		return delVertex(geo, UTverify_cast<const GA_EdgeGroup*>(group),
			delDegeneratePrims, delUnusedPoints);
		break;
	}
	UT_ASSERT("cant be possible");
}


} // End of namespace GA_FeE_DelVertex

#endif
