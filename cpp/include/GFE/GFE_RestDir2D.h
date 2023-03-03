
#pragma once

#ifndef __GFE_RestDir2D_h__
#define __GFE_RestDir2D_h__



//#include "GFE/GFE_RestDir2D.h"

#include "GA/GA_Detail.h"
#include "GA/GA_PageHandle.h"
#include "GA/GA_PageIterator.h"
#include "UT/UT_OBBox.h"


#include "GEO/GEO_Normal.h"


#include "GFE/GFE_Type.h"
#include "GFE/GFE_GroupParse.h"
#include "GFE/GFE_GroupPromote.h"



enum GFE_RestDir2D_Method
{
	GFE_RestDir2D_AvgNormal,
	GFE_RestDir2D_HouOBB,
};


namespace GFE_RestDir2D {


template<typename T>
class ComputeDir2D_AvgNormal {
public:
	ComputeDir2D_AvgNormal(const GA_Detail* const geo, const GA_Attribute* const normalAttrib = nullptr)
		: myGeo(geo)
		, myNormalAttrib(normalAttrib)
		//, mySum(UT_Vector3T<T>(T(0)))
	{
		mySum = 0;
	}
	ComputeDir2D_AvgNormal(ComputeDir2D_AvgNormal& src, UT_Split)
		: myGeo(src.myGeo)
		, myNormalAttrib(src.myNormalAttrib)
		//, mySum(UT_Vector3T<T>(T(0)))
	{
		mySum = 0;
	}
	void operator()(const GA_SplittableRange& r)
	{
		UT_Vector3T<T> sumTmp(T(0));
		if (myNormalAttrib)
		{
			GA_PageHandleT<UT_Vector3T<T>, typename UT_Vector3T<T>::value_type, true, false, const GA_Attribute, const GA_ATINumeric, const GA_Detail> normal_ph(myNormalAttrib);
			for (GA_PageIterator pit = r.beginPages(); !pit.atEnd(); ++pit)
			{
				GA_Offset start, end;
				for (GA_Iterator it(pit.begin()); it.blockAdvance(start, end); )
				{
					normal_ph.setPage(start);
					for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
					{
						sumTmp += normal_ph.value(elemoff).normalize();
					}
				}
			}
		}
		else
		{
			GA_Offset start, end;
			for (GA_Iterator it(r); it.blockAdvance(start, end); )
			{
				for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
				{
					//sumTmp += myGeo->getPrimitive(elemoff)->computeNormalD();
					sumTmp += static_cast<const GEO_Primitive*>(myGeo->getPrimitive(elemoff))->computeNormalD();
				}
			}
		}
		sumTmp /= T(r.getEntries());
		mySum += sumTmp;
	}

	void join(const ComputeDir2D_AvgNormal& other)
	{
		mySum += other.mySum;
	}
	UT_Vector3T<T> getSum()
	{
		return mySum;
	}
private:
	UT_Vector3T<T> mySum;
	const GA_Detail* const myGeo;
	const GA_Attribute* const myNormalAttrib;
};


//UT_Vector3T<T> dir2D = restDir2D(geo);
template<typename T>
static UT_Vector3T<T>
restDir2D_avgNormal(
	const GA_Detail* const geo,
	const GA_ROHandleT<UT_Vector3T<T>>& normal_h,
	const GA_PrimitiveGroup* const geoPrimGroup = nullptr,
	const exint subscribeRatio = 64,
	const exint minGrainSize = 1024
)
{
	ComputeDir2D_AvgNormal<T> body(geo, normal_h.getAttribute());
	const GA_SplittableRange geoSplittableRange(geo->getPrimitiveRange(geoPrimGroup));
	UTparallelReduce(geoSplittableRange, body, subscribeRatio, minGrainSize);
	return body.getSum();
}



//UT_Vector3T<T> dir2D = restDir2D(geo);
template<typename T>
static UT_Vector3T<T>
restDir2D_avgNormal(
    const GA_Detail* const geo,
    const GA_PrimitiveGroup* const geoPrimGroup = nullptr,
	const exint subscribeRatio = 64,
	const exint minGrainSize = 1024
)
{
	const GA_ATINumericUPtr normalAttribUPtr = geo->getAttributes().createDetachedTupleAttribute(GA_ATTRIB_PRIMITIVE, GA_STORE_REAL64, 3);
	GA_Attribute* const normalAttrib = normalAttribUPtr.get();

	GEOcomputeNormals(*static_cast<const GEO_Detail*>(geo), normalAttrib, geoPrimGroup,
		GEO_DEFAULT_ADJUSTED_CUSP_ANGLE, GEO_NormalMethod::ANGLE_WEIGHTED, false);

    ComputeDir2D_AvgNormal<T> body(geo);
    const GA_SplittableRange geoSplittableRange(geo->getPrimitiveRange(geoPrimGroup));
    UTparallelReduce(geoSplittableRange, body, subscribeRatio, minGrainSize);
    return body.getSum();
}



//UT_Vector3T<T> dir2D = restDir2D(geo);
template<typename T>
static UT_Vector3T<T>
restDir2D_houOBB(
	const GA_Detail* const geo,
	const GA_PrimitiveGroup* const geoPrimGroup = nullptr
)
{
	//UT_Matrix4 transform;
	//UT_Vector3 radii;
	//static_cast<const GU_Detail*>(geo)->getOBBox(geoPrimGroup, transform, radii);
	//int minAbsAxis = radii.findMinAbsAxis();

	//switch (minAbsAxis)
	//{
	//case 0:
	//	break;
	//case 1:
	//	break;
	//case 2:
	//	break;
	//}

	UT_OBBoxD obb;
	//static_cast<const GEO_Detail*>(geo)->setDetailAttributeI("minAbsAxis", minAbsAxis);
	static_cast<const GU_Detail*>(geo)->getOBBoxForPrims(geoPrimGroup, obb);

	return obb.getMinAxis();
}






















static void
computeRestDir2D_avgNormal(
	const GA_Detail* const geo,
	GA_Attribute* const attribPtr,
	const GA_PrimitiveGroup* const geoPrimGroup = nullptr,
	const GA_Storage storage = GA_STORE_INVALID,
	const exint subscribeRatio = 64,
	const exint minGrainSize = 1024
)
{
	UT_ASSERT_P(geo);
	UT_ASSERT_P(attribPtr);
	switch (storage)
	{
	case GA_STORE_REAL32:
	{
		const UT_Vector3T<fpreal32>& dir2D = restDir2D_avgNormal<fpreal32>(geo, geoPrimGroup);
		GA_RWHandleV3 attrib_h(attribPtr);
		attrib_h.set(0, dir2D);
	}
	break;
	case GA_STORE_REAL64:
	{
		const UT_Vector3T<fpreal64>& dir2D = restDir2D_avgNormal<fpreal64>(geo, geoPrimGroup);
		GA_RWHandleV3D attrib_h(attribPtr);
		attrib_h.set(0, dir2D);
	}
	break;
	default:
		UT_ASSERT_MSG(0, "unhandled storage");
		break;
	}
}

static void
computeRestDir2D_avgNormal(
	const GA_Detail* const geo,
	const GA_Attribute* const normalAttrib,
	GA_Attribute* const attribPtr,
	const GA_PrimitiveGroup* const geoPrimGroup = nullptr,
	const GA_Storage storage = GA_STORE_INVALID,
	const exint subscribeRatio = 64,
	const exint minGrainSize = 1024
)
{
	UT_ASSERT_P(geo);
	UT_ASSERT_P(attribPtr);
	UT_ASSERT_P(normalAttrib);
	switch (storage)
	{
	case GA_STORE_REAL32:
	{
		const UT_Vector3T<fpreal32>& dir2D = restDir2D_avgNormal<fpreal32>(geo, normalAttrib, geoPrimGroup);
		GA_RWHandleV3 attrib_h(attribPtr);
		attrib_h.set(0, dir2D);
	}
	break;
	case GA_STORE_REAL64:
	{
		const UT_Vector3T<fpreal64>& dir2D = restDir2D_avgNormal<fpreal64>(geo, normalAttrib, geoPrimGroup);
		GA_RWHandleV3D attrib_h(attribPtr);
		attrib_h.set(0, dir2D);
	}
	break;
	default:
		UT_ASSERT_MSG(0, "unhandled storage");
		break;
	}
}



static GA_Attribute*
addAttribRestDir2D_avgNormal(
	GA_Detail* const geo,
	const GA_PrimitiveGroup* const geoPrimGroup = nullptr,
	const GA_Storage storage = GA_STORE_INVALID,
	const UT_StringHolder& name = "__topo_restDir2D",
	const UT_Options* const creation_args = nullptr,
	const GA_AttributeOptions* const attribute_options = nullptr,
	const GA_ReuseStrategy& reuse = GA_ReuseStrategy(),
	const exint subscribeRatio = 64,
	const exint minGrainSize = 1024
)
{
	UT_ASSERT_P(geo);
	const GA_Storage finalStorage = storage == GA_STORE_INVALID ? GFE_Type::getPreferredStorageF(geo) : storage;
	GA_Attribute* const outAttrib = geo->getAttributes().createTupleAttribute(GA_ATTRIB_DETAIL, name, finalStorage, 3, GA_Defaults(0), creation_args, attribute_options, reuse);
	
	computeRestDir2D_avgNormal(geo, outAttrib, geoPrimGroup, finalStorage, subscribeRatio, minGrainSize);

	return outAttrib;
}


static GA_Attribute*
addAttribRestDir2D_avgNormal(
	GA_Detail* const geo,
	const GA_Attribute* const normalAttrib,
	const GA_PrimitiveGroup* const geoPrimGroup = nullptr,
	const GA_Storage storage = GA_STORE_INVALID,
	const UT_StringHolder& name = "__topo_restDir2D",
	const UT_Options* const creation_args = nullptr,
	const GA_AttributeOptions* const attribute_options = nullptr,
	const GA_ReuseStrategy& reuse = GA_ReuseStrategy(),
	const exint subscribeRatio = 64,
	const exint minGrainSize = 1024
)
{
	UT_ASSERT_P(geo);
	const GA_Storage finalStorage = storage == GA_STORE_INVALID ? GFE_Type::getPreferredStorageF(geo) : storage;
	GA_Attribute* const outAttrib = geo->getAttributes().createTupleAttribute(GA_ATTRIB_DETAIL, name, finalStorage, 3, GA_Defaults(0), creation_args, attribute_options, reuse);

	computeRestDir2D_avgNormal(geo, normalAttrib, outAttrib, geoPrimGroup, finalStorage, subscribeRatio, minGrainSize);

	return outAttrib;
}




static GA_ATINumericUPtr
addDetachedAttribRestDir2D_avgNormal(
	const GA_Detail* const geo,
	const GA_PrimitiveGroup* const geoPrimGroup = nullptr,
	const GA_Storage storage = GA_STORE_INVALID,
	const GA_AttributeOptions* const attribute_options = nullptr,
	const exint subscribeRatio = 64,
	const exint minGrainSize = 1024
)
{
	UT_ASSERT_P(geo);
	const GA_Storage finalStorage = storage == GA_STORE_INVALID ? GFE_Type::getPreferredStorageF(geo) : storage;
	GA_ATINumericUPtr outAttribUPtr = geo->getAttributes().createDetachedTupleAttribute(GA_ATTRIB_DETAIL, finalStorage, 3, GA_Defaults(0), attribute_options);
	GA_Attribute* const outAttrib = outAttribUPtr.get();

	computeRestDir2D_avgNormal(geo, outAttrib, geoPrimGroup, finalStorage, subscribeRatio, minGrainSize);

	return outAttribUPtr;
}









//UT_Vector3T<T> dir2D = restDir2D(geo);
template<typename T>
static UT_Vector3T<T>
restDir2D(
	const GA_Detail* const geo,
	const GA_PrimitiveGroup* const geoPrimGroup = nullptr,
	const GFE_RestDir2D_Method method = GFE_RestDir2D_AvgNormal,
	const exint subscribeRatio = 64,
	const exint minGrainSize = 1024
)
{
	switch (method)
	{
	case GFE_RestDir2D_AvgNormal:
		return restDir2D_avgNormal<T>(geo, geoPrimGroup, subscribeRatio, minGrainSize);
		break;
	case GFE_RestDir2D_HouOBB:
		return restDir2D_houOBB<T>(geo, geoPrimGroup);
		break;
	}
	UT_ASSERT_MSG(0, "unhandled method");
	return UT_Vector3T<T>(T(0));
}



static void
computeRestDir2D(
	const GA_Detail* const geo,
	GA_Attribute* const attribPtr,
	const GA_PrimitiveGroup* const geoPrimGroup = nullptr,
	const GFE_RestDir2D_Method method = GFE_RestDir2D_AvgNormal,
	const GA_Storage storage = GA_STORE_INVALID,
	const exint subscribeRatio = 64,
	const exint minGrainSize = 1024
)
{
	UT_ASSERT_P(geo);
	UT_ASSERT_P(attribPtr);
	switch (storage)
	{
	case GA_STORE_REAL32:
	{
		const UT_Vector3T<fpreal32>& dir2D = restDir2D<fpreal32>(geo, geoPrimGroup, method);
		GA_RWHandleV3 attrib_h(attribPtr);
		attrib_h.set(0, dir2D);
	}
	break;
	case GA_STORE_REAL64:
	{
		const UT_Vector3T<fpreal64>& dir2D = restDir2D<fpreal64>(geo, geoPrimGroup, method);
		GA_RWHandleV3D attrib_h(attribPtr);
		attrib_h.set(0, dir2D);
	}
	break;
	default:
		UT_ASSERT_MSG(0, "unhandled storage");
		break;
	}
}



static GA_Attribute*
addAttribRestDir2D(
	GA_Detail* const geo,
	const GA_PrimitiveGroup* const geoPrimGroup = nullptr,
	const GFE_RestDir2D_Method method = GFE_RestDir2D_AvgNormal,
	const GA_Storage storage = GA_STORE_INVALID,
	const UT_StringHolder& name = "__topo_restDir2D",
	const UT_Options* const creation_args = nullptr,
	const GA_AttributeOptions* const attribute_options = nullptr,
	const GA_ReuseStrategy& reuse = GA_ReuseStrategy(),
	const exint subscribeRatio = 64,
	const exint minGrainSize = 1024
)
{
	UT_ASSERT_P(geo);
	const GA_Storage finalStorage = storage == GA_STORE_INVALID ? GFE_Type::getPreferredStorageF(geo) : storage;
	GA_Attribute* const outAttrib = geo->getAttributes().createTupleAttribute(GA_ATTRIB_DETAIL, name, finalStorage, 3, GA_Defaults(0), creation_args, attribute_options, reuse);

	computeRestDir2D(geo, outAttrib, geoPrimGroup, method, finalStorage, subscribeRatio, minGrainSize);

	return outAttrib;
}


static GA_Attribute*
addAttribRestDir2D(
	const SOP_NodeVerb::CookParms& cookparms,
	GA_Detail* const geo,
	const GA_GroupType groupType,
	const UT_StringHolder& groupName,
	const GFE_RestDir2D_Method method,
	const GA_Storage storage = GA_STORE_INVALID,
	const UT_StringHolder& name = "__topo_restDir2D",
	const UT_Options* const creation_args = nullptr,
	const GA_AttributeOptions* const attribute_options = nullptr,
	const GA_ReuseStrategy& reuse = GA_ReuseStrategy(),
	const exint subscribeRatio = 64,
	const exint minGrainSize = 1024
)
{
	UT_ASSERT_P(geo);

	GOP_Manager gop;
	const GA_Group* const geoGroup = GFE_GroupParse_Namespace::findOrParseGroupDetached(cookparms, geo, groupType, groupName, gop);
	const GA_PrimitiveGroup* const geoPrimGroup = GFE_GroupPromote::groupFindPromotePrimitiveDetached(geo, geoGroup);

	return addAttribRestDir2D(geo, geoPrimGroup,
		method, storage, name,
		creation_args, attribute_options, reuse,
		subscribeRatio, minGrainSize);

	if (geoPrimGroup->isDetached())
		delete geoPrimGroup;
}







} // End of namespace GFE_RestDir2D

#endif
