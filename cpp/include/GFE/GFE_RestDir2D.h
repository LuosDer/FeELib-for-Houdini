
#pragma once

#ifndef __GFE_RestDir2D_h__
#define __GFE_RestDir2D_h__

#include "GFE/GFE_RestDir2D.h"


#include "UT/UT_OBBox.h"

#include "GFE/GFE_GeoFilter.h"
#include "GFE/GFE_Normal3D.h"


enum class GFE_RestDir2DMethod
{
	AvgNormal,
	HouOBB,
};






class GFE_RestDir2D : public GFE_AttribFilter {

public:
	//using GFE_AttribFilter::GFE_AttribFilter;
	
	GFE_RestDir2D(
		GA_Detail& geo,
		const SOP_NodeVerb::CookParms* const cookparms = nullptr
	)
		: GFE_AttribFilter(geo, cookparms)
		, normal3D(geo, cookparms)
	{
	}

	~GFE_RestDir2D()
	{
	}

	void
		setComputeParm(
			const GFE_RestDir2DMethod method = GFE_RestDir2DMethod::AvgNormal,
			const exint subscribeRatio = 64,
			const exint minGrainSize = 1024
		)
	{
		setHasComputed();
		//setInAttribBumpDataId(method == GFE_RestDir2DMethod::AvgNormal);
		//getInAttribArray().findOrCreateTuple();
		this->method = method;
		this->subscribeRatio = subscribeRatio;
		this->minGrainSize = minGrainSize;
	}

	SYS_FORCE_INLINE GA_Attribute* findOrCreateDir(
		const bool detached = false,
		const GA_Storage storage = GA_STORE_INVALID,
		const UT_StringRef& attribName = "")
	{ return getOutAttribArray().findOrCreateDir(detached, GA_ATTRIB_DETAIL, storage, attribName); }
	
	SYS_FORCE_INLINE UT_Vector3T<fpreal> getRestDir() const
	{ return restDir; }

	


private:


	virtual bool
		computeCore() override
	{
		if (groupParser.isEmpty())
			return true;
		
		switch (method)
		{
			case GFE_RestDir2DMethod::AvgNormal: restDir2D_avgNormal(); break;
			case GFE_RestDir2DMethod::HouOBB:    restDir2D_houOBB();    break;
		}

		if (!getOutAttribArray().isEmpty())
		{
			GA_Attribute* const outAttrib = getOutAttribArray()[0];
#if 0
			outAttrib->getAIFTuple()->set(outAttrib, 0, restDir[0], 0);
			outAttrib->getAIFTuple()->set(outAttrib, 0, restDir[1], 1);
			outAttrib->getAIFTuple()->set(outAttrib, 0, restDir[2], 2);
#else
			switch (outAttrib->getAIFTuple()->getStorage(outAttrib))
			{
			case GA_STORE_REAL32:
				{
					GA_RWHandleT<UT_Vector3T<fpreal32>> attrib_h(outAttrib);
					attrib_h.set(0, restDir);
				}
				break;
			case GA_STORE_REAL64:
				{
					GA_RWHandleT<UT_Vector3T<fpreal64>> attrib_h(outAttrib);
					attrib_h.set(0, restDir);
				}
				break;
			default:
				UT_ASSERT_MSG(0, "unhandled storage");
				break;
			}
#endif
		}
		
		return true;
	}


	void restDir2D_avgNormal()
	{
		normal3D.groupParser.setGroup(groupParser);

		if (normal3D.getOutAttribArray().isEmpty())
			normal3D.getOutAttribArray().findOrCreateNormal3D(true, GFE_NormalSearchOrder::ALL, GA_STORE_INVALID, "N");
		
		normal3D.compute();

		const GA_Attribute* const normalAttrib = normal3D.getAttrib();

		const GA_Storage storage = normalAttrib ? normalAttrib->getAIFTuple()->getStorage(normalAttrib) : geo->getPreferredStorageF();;
		switch (storage)
		{
		case GA_STORE_REAL16: restDir2D_avgNormal<fpreal16>(normalAttrib); break;
		case GA_STORE_REAL32: restDir2D_avgNormal<fpreal32>(normalAttrib); break;
		case GA_STORE_REAL64: restDir2D_avgNormal<fpreal64>(normalAttrib); break;
		default: break;
		}
	}

	template<typename FLOAT_T>
	void restDir2D_avgNormal(const GA_Attribute* const normalAttrib)
	{
		ComputeDir2D_AvgNormal<FLOAT_T> body(geo, normalAttrib);
		const GA_SplittableRange geoSplittableRange(groupParser.getPrimitiveRange());
		UTparallelReduce(geoSplittableRange, body, subscribeRatio, minGrainSize);
		restDir = body.getSumRef();
	}


	
	void restDir2D_houOBB()
	{
		UT_OBBoxD obb;
		geo->asGU_Detail()->getOBBoxForPrims(groupParser.getPrimitiveGroup(), obb);

		//geo->asGU_Detail()->setDetailAttributeI("minAbsAxis", minAbsAxis);
		restDir = obb.getMinAxis();
	}



	
	template<typename FLOAT_T>
	class ComputeDir2D_AvgNormal {
	public:
		ComputeDir2D_AvgNormal(const GA_Detail* const geo, const GA_Attribute* const normal3DAttribPtr = nullptr)
			: myGeo(geo)
			, myNormal3DAttribPtr(normal3DAttribPtr)
			//, mySum(UT_Vector3T<FLOAT_T>(FLOAT_T(0)))
		{
			mySum = UT_Vector3T<FLOAT_T>(FLOAT_T(0.0));
		}
		ComputeDir2D_AvgNormal(ComputeDir2D_AvgNormal& src, UT_Split)
			: myGeo(src.myGeo)
			, myNormal3DAttribPtr(src.myNormal3DAttribPtr)
			//, mySum(UT_Vector3T<FLOAT_T>(FLOAT_T(0)))
		{
			mySum = UT_Vector3T<FLOAT_T>(FLOAT_T(0.0));
		}
		void operator()(const GA_SplittableRange& r)
		{
			UT_Vector3T<FLOAT_T> sumTmp(FLOAT_T(0));
			if (myNormal3DAttribPtr)
			{
				GA_PageHandleT<UT_Vector3T<FLOAT_T>, typename UT_Vector3T<FLOAT_T>::value_type, true, false, const GA_Attribute, const GA_ATINumeric, const GA_Detail> normal_ph(myNormal3DAttribPtr);
				for (GA_PageIterator pit = r.beginPages(); !pit.atEnd(); ++pit)
				{
					GA_Offset start, end;
					for (GA_Iterator it(pit.begin()); it.blockAdvance(start, end); )
					{
						normal_ph.setPage(start);
						for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
						{
							sumTmp += normal_ph.value(elemoff);
							//sumTmp += normal_ph.value(elemoff).normalize();
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
						if constexpr (std::is_same_v<FLOAT_T, fpreal64>)
							sumTmp += static_cast<const GEO_Primitive*>(myGeo->getPrimitive(elemoff))->computeNormalD();
						else
							sumTmp += static_cast<const GEO_Primitive*>(myGeo->getPrimitive(elemoff))->computeNormal();
					}
				}
			}
			sumTmp /= FLOAT_T(r.getEntries());
			mySum += sumTmp;
		}
		SYS_FORCE_INLINE void join(const ComputeDir2D_AvgNormal& other)
		{ mySum += other.mySum; }
		SYS_FORCE_INLINE const UT_Vector3T<FLOAT_T>& getSumRef() const
		{ return mySum; }
		// SYS_FORCE_INLINE UT_Vector3T<FLOAT_T>& getSumRef()
		// { return mySum; }
	private:
		UT_Vector3T<FLOAT_T> mySum;
		const GA_Detail* const myGeo;
		const GA_Attribute* const myNormal3DAttribPtr;
	}; // End of Class ComputeDir2D_AvgNormal


public:
	GFE_Normal3D normal3D;
	GFE_RestDir2DMethod method = GFE_RestDir2DMethod::AvgNormal;

private:
	
	UT_Vector3T<fpreal> restDir;
	exint subscribeRatio = 64;
	exint minGrainSize = 1024;
	
}; // End of Class GFE_RestDir2D





#endif
