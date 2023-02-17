
#pragma once

#ifndef __GA_FeE_GroupByMeshWinding_h__
#define __GA_FeE_GroupByMeshWinding_h__

//#include "GA_FeE/GA_FeE_GroupByMeshWinding.h"

#include "GA/GA_Detail.h"



#include "GEO/GEO_Hull.h"




#include "GA_FeE/GA_FeE_Group.h"
#include "GA_FeE/GA_FeE_Measure.h"
#include "GA_FeE/GA_FeE_MeshCap.h"

enum GFE_GroupByMeshWindingMethod
{
	GFE_GroupByMeshWindingMethod_VOLUME,
	GFE_GroupByMeshWindingMethod_WINDINGNUMBER,
};


namespace GA_FeE_GroupByMeshWinding {





//isMeshWindingCorrect_volume(geo, group);
static bool
isMeshWindingCorrect_volume(
	const GA_Detail* const geo,
	const GA_PrimitiveGroup* const geoPrimGroup = nullptr
)
{
	fpreal32 volume = GA_FeE_Measure::computeMeshVolume(geo, geoPrimGroup);
	return volume > 0;
}


//isMeshWindingCorrect_wn(geo, group);
static bool
isMeshWindingCorrect_wn(
	const GA_Detail* const geo,
	const GA_PrimitiveGroup* const geoPrimGroup = nullptr
)
{
	fpreal32 volume = GA_FeE_Measure::computeMeshVolume(geo, geoPrimGroup);
	return volume > 0;
}

//isMeshWindingCorrect(geo, group, reverseGroup, delGroup);
SYS_FORCE_INLINE
static bool
isMeshWindingCorrect(
    GA_Detail* const geo,
	const GA_PrimitiveGroup* const geoPrimGroup,
	const GFE_GroupByMeshWindingMethod method
)
{
	switch (method)
	{
	case GFE_GroupByMeshWindingMethod_VOLUME:
		return isMeshWindingCorrect_volume(geo, geoPrimGroup);
		break;
	case GFE_GroupByMeshWindingMethod_WINDINGNUMBER:
		return isMeshWindingCorrect_wn(geo, geoPrimGroup);
		break;
	default:
		break;
	}
	UT_ASSERT_MSG(0, "cant be possible");
	return false;
}


//groupByMeshWinding(geo, group, reverseGroup, delGroup);
static bool
isMeshWindingCorrect(
	GA_Detail* const geo,
	const GA_PrimitiveGroup* const geoPrimGroup,
	const GFE_GroupByMeshWindingMethod method,

	const bool reversePrim,

	const bool polyCap = false
)
{
	bool isMeshWindingCorrectb;
	if (polyCap)
	{
		GU_DetailHandle geoTmp0_h;
		GU_Detail* geoTmp0 = new GU_Detail();
		geoTmp0_h.allocateAndSet(geoTmp0);
		geoTmp0->replaceWith(*geo);

		GA_FeE_MeshCap::meshCapSingle(geoTmp0);

		isMeshWindingCorrectb = isMeshWindingCorrect(geoTmp0, geoPrimGroup, method);
	}
	else
	{
		isMeshWindingCorrectb = isMeshWindingCorrect(geo, geoPrimGroup, method);
	}

	if (reversePrim && !isMeshWindingCorrectb)
	{
		GA_Offset start, end;
		for (GA_Iterator it(geo->getPrimitiveRange()); it.fullBlockAdvance(start, end); )
		{
			for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
			{
				geo->getPrimitive(elemoff)->reverse();
			}
		}
	}

	return isMeshWindingCorrectb;
}

//groupByMeshWinding(geo, group, reverseGroup, delGroup);
static GA_PrimitiveGroup*
groupByMeshWinding(
	GA_Detail* const geo,
	const GA_PrimitiveGroup* const geoPrimGroup,
	const GFE_GroupByMeshWindingMethod method,

	const bool outReversedGroup,
	const UT_StringHolder& reversedGroupName,
	const bool reverseGroup,
	const bool reversePrim,

	const bool polyCap = false
)
{
	const bool isMeshWindingCorrectb = isMeshWindingCorrect(geo, geoPrimGroup, method,
		reversePrim, polyCap);

	if (outReversedGroup)
	{
		GA_PrimitiveGroup* const reversedGroup = geo->newPrimitiveGroup(reversedGroupName);
		if (isMeshWindingCorrectb ^ reverseGroup)
			reversedGroup->addAll();
		else
			reversedGroup->clear();

		return reversedGroup;
	}

	return nullptr;
	//UTparallelFor(geoSplittableRange, [geo, &bboxCenter](const GA_SplittableRange& r)
	//{
	//	fpreal volumeSum = 0;
	//	GA_Offset start, end;
	//	for (GA_Iterator it(r); it.blockAdvance(start, end); )
	//	{
	//		for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
	//		{
	//			GEO_Hull* prim = static_cast<GEO_Hull*>(geo->getPrimitive(elemoff));
	//			volumeSum += prim->calcVolume(bboxCenter);
	//			//GA_Primitive* poly1 = geo->getPrimitive(elemoff);
	//			//poly = static_cast<GEO_Hull*>(poly1);
	//		}
	//	}
	//}
} 


//groupByMeshWinding(geo, group, reverseGroup, delGroup);
static GA_PrimitiveGroup*
groupByMeshWinding(
	GA_Detail* const geo,
	const GA_PrimitiveGroup* const geoPrimGroup,
	const GFE_GroupByMeshWindingMethod method,

	const bool runOverPieces,
	const bool findInputPieceAttrib,
	const UT_StringHolder& pieceAttribName,

	const bool outReversedGroup,
	const UT_StringHolder& reversedGroupName,
	const bool reverseGroup,
	const bool reversePrim,

	const bool polyCap = false
)
{
	if (runOverPieces)
	{
		const bool isMeshWindingCorrectb = isMeshWindingCorrect(geo, geoPrimGroup, method,
			reversePrim, polyCap);

		if (outReversedGroup)
		{
			GA_PrimitiveGroup* const reversedGroup = geo->newPrimitiveGroup(reversedGroupName);
			if (isMeshWindingCorrectb ^ reverseGroup)
				reversedGroup->addAll();
			else
				reversedGroup->clear();

			return reversedGroup;
		}

		return nullptr;
	}
	else
	{
		return groupByMeshWinding(geo, geoPrimGroup, method,
			outReversedGroup, reversedGroupName, reverseGroup, reversePrim, polyCap);
	}

}




//groupByMeshWinding(geo, group, reverseGroup, delGroup);
static void
groupByMeshWinding(
	const SOP_NodeVerb::CookParms& cookparms,
	GA_Detail* const geo,
	const UT_StringHolder& groupName,
	const GFE_GroupByMeshWindingMethod method,

	const bool runOverPieces,
	const bool findInputPieceAttrib,
	const UT_StringHolder& pieceAttribName,

	const bool outReversedGroup,
	const UT_StringHolder& reversedGroupName,
	const bool reverseGroup,
	const bool reversePrim,

	const bool polyCap = false
)
{
	GOP_Manager gop;
	const GA_PrimitiveGroup* const geoPrimGroup = GA_FeE_Group::findOrParsePrimitiveGroupDetached(cookparms, geo, groupName, gop);

	const GA_PrimitiveGroup* const reversedGroup = groupByMeshWinding(geo, geoPrimGroup, method,
		runOverPieces, findInputPieceAttrib, pieceAttribName,
		outReversedGroup, reversedGroupName, reverseGroup, reversePrim, polyCap);

	if (outReversedGroup)
	{
		cookparms.getNode()->setHighlight(true);
		cookparms.select(*reversedGroup);
	}
}





} // End of namespace GA_FeE_GroupByMeshWinding

#endif
