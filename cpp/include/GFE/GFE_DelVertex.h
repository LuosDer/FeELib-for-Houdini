
#pragma once

#ifndef __GFE_DelVertex_h__
#define __GFE_DelVertex_h__

//#include "GFE/GFE_DelVertex.h"

#include "GA/GA_Detail.h"

//#include "GA/GA_PageHandle.h"
//#include "GA/GA_PageIterator.h"

#include "GFE/GFE_Group.h"


namespace GFE_DelVertex {

	
	static void
		delAllVertex(
			GA_Detail* const geo,
			const bool delDegeneratePrims = true,
			const bool delUnusedPoints = true
		)
	{
		if (delDegeneratePrims)
			geo->destroyPrimitives(geo->getPrimitiveRange(), delUnusedPoints);
		else
		{
			GA_Topology& topo = geo->getTopology();
			GA_Offset start, end;
			for (GA_Iterator it(geo->getPrimitiveRange()); it.blockAdvance(start, end); )
			{
				for (GA_Offset primoff = start; primoff < end; ++primoff)
				{
					const GA_OffsetListRef& vertices = geo->getPrimitiveVertexList(primoff);
					for (GA_Size i = 0; i < vertices.size(); i++)
					{
						geo->getPrimitive(primoff)->releaseVertex(vertices[i]);
						topo.delVertex(vertices[i]);
					}
				}
			}
			if (delUnusedPoints)
				geo->destroyUnusedPoints();
		}
	}




	static void
		delVertex(
			GA_Detail* const geo,
			const GA_PrimitiveGroup* const group,
			const bool delDegeneratePrims = true,
			const bool delUnusedPoints = true
		)
	{
		if (!group)
			return delAllVertex(geo, delDegeneratePrims, delUnusedPoints);

		if (delDegeneratePrims)
			geo->destroyPrimitives(geo->getPrimitiveRange(group), delUnusedPoints);
		else
		{
			GA_PointGroupUPtr pointGroupUPtr;
			GA_PointGroup* pointGroup;
			if (delUnusedPoints)
			{
				pointGroupUPtr = geo->createDetachedPointGroup();
				pointGroup = pointGroupUPtr.get();
			}

			GA_Topology& topo = geo->getTopology();
			const GA_ATITopology* const vtxPointRef = topo.getPointRef();

			GA_Offset start, end;
			for (GA_Iterator it(geo->getPrimitiveRange(group)); it.blockAdvance(start, end); )
			{
				for (GA_Offset primoff = start; primoff < end; ++primoff)
				{
					const GA_OffsetListRef& vertices = geo->getPrimitiveVertexList(primoff);
					for (GA_Size i = 0; i < vertices.size(); i++)
					{
						if (delUnusedPoints)
							pointGroup->setElement(vtxPointRef->getLink(vertices[i]), true);
						geo->getPrimitive(primoff)->releaseVertex(vertices[i]);
						topo.delVertex(vertices[i]);
					}
				}
			}
			if (delUnusedPoints)
				geo->destroyUnusedPoints(pointGroup);
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
		if (!group)
			return delAllVertex(geo, delDegeneratePrims, delUnusedPoints);

		if (delUnusedPoints)
		{
			geo->destroyPointOffsets(geo->getPointRange(group),
				delDegeneratePrims ? GA_Detail::GA_DestroyPointMode::GA_DESTROY_DEGENERATE : GA_Detail::GA_DestroyPointMode::GA_LEAVE_PRIMITIVES);
		}
		else
		{
			GA_PrimitiveGroupUPtr primGroupUPtr;
			GA_PrimitiveGroup* primGroup;
			if (delDegeneratePrims)
			{
				primGroupUPtr = geo->createDetachedPrimitiveGroup();
				primGroup = primGroupUPtr.get();
			}

			GA_Topology& topo = geo->getTopology();
			const GA_ATITopology* const pointVtxRef = topo.getVertexRef();
			const GA_ATITopology* const vtxPrimRef = topo.getPrimitiveRef();
			const GA_ATITopology* const vtxNextRef = topo.getVertexNextRef();

			GA_Offset start, end;
			for (GA_Iterator it(geo->getPointRange(group)); it.blockAdvance(start, end); )
			{
				for (GA_Offset ptoff = start; ptoff < end; ++ptoff)
				{
					for (GA_Offset vtxoff_next = pointVtxRef->getLink(ptoff); vtxoff_next != GA_INVALID_OFFSET; vtxoff_next = vtxNextRef->getLink(vtxoff_next))
					{
						GA_Offset primoff = vtxPrimRef->getLink(vtxoff_next);

						geo->getPrimitive(primoff)->releaseVertex(vtxoff_next);
						topo.delVertex(vtxoff_next);

						if (delDegeneratePrims)
							primGroup->setElement(primoff, true);
					}
				}
			}
			if (delDegeneratePrims)
				geo->destroyPrimitives(geo->getPrimitiveRange(primGroup));
		}
	}



	

	static void
		delVertex(
			GA_Detail* const geo,
			const GA_VertexGroup* const group,
			const bool delDegeneratePrims = true,
			const bool delUnusedPoints = true
		)
	{
		UT_ASSERT_P(geo);
		if (!group)
			return delAllVertex(geo, delDegeneratePrims, delUnusedPoints);

		GA_Topology& topo = geo->getTopology();
		const GA_ATITopology* const vtxPrimRef = topo.getPrimitiveRef();
		const GA_ATITopology* const vtxPointRef = topo.getPointRef();


		GA_PrimitiveGroupUPtr primGroupUPtr;
		GA_PrimitiveGroup* primGroup;
		if (delDegeneratePrims)
		{
			primGroupUPtr = geo->createDetachedPrimitiveGroup();
			primGroup = primGroupUPtr.get();
		}

		GA_PointGroupUPtr pointGroupUPtr;
		GA_PointGroup* pointGroup;
		if (delUnusedPoints)
		{
			pointGroupUPtr = geo->createDetachedPointGroup();
			pointGroup = pointGroupUPtr.get();
		}
#if 1
		GA_Offset start, end;
		for (GA_Iterator it(geo->getVertexRange(group)); it.blockAdvance(start, end); )
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
#else
		const GA_SplittableRange geoSplittableRange(geo->getVertexRange(group));
		UTparallelFor(geoSplittableRange, [geo, &topo, vtxPrimRef, vtxPointRef, primGroup, pointGroup, delDegeneratePrims, delUnusedPoints](const GA_SplittableRange& r)
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
#endif

		if (delDegeneratePrims)
			geo->destroyDegeneratePrimitives(primGroup);
		if (delUnusedPoints)
			geo->destroyUnusedPoints(pointGroup);
	}






	static void
		delVertex(
			GA_Detail* const geo,
			const GA_EdgeGroup* const group,
			const bool delDegeneratePrims = true,
			const bool delUnusedPoints = true
		)
	{
		if (!group)
			return delAllVertex(geo, delDegeneratePrims, delUnusedPoints);

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
	if (!group)
		return delAllVertex(geo, delDegeneratePrims, delUnusedPoints);

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
	UT_ASSERT_MSG(0, "cant be possible");
}



static void
delVertex(
	const SOP_NodeVerb::CookParms& cookparms,
	GA_Detail* const geo,
	const GA_GroupType groupType,
	const UT_StringHolder& groupName,
	const bool delDegeneratePrims = true,
	const bool delUnusedPoints = true
)
{
	GOP_Manager gop;
	const GA_Group* const geoGroup = GFE_Group::findOrParseGroupDetached(cookparms, geo, groupType, groupName, gop);
	if (geoGroup && geoGroup->isEmpty())
		return;

	delVertex(geo, geoGroup,
		delDegeneratePrims, delUnusedPoints);

	geo->bumpDataIdsForAddOrRemove(1, 1, 1);

}




} // End of namespace GFE_DelVertex

#endif
