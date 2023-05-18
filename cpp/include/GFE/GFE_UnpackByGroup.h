
#pragma once

#ifndef __GFE_UnpackByGroup_h__
#define __GFE_UnpackByGroup_h__

#include "GFE/GFE_DeleteAndUnpack.h"


#include "GFE/GFE_GeoFilter.h"


#include "GU/GU_PrimPacked.h"


enum class GFE_UnpackByGroup_Type
{
	Custom,
	OneElem,
	SkipNElem,
};

	
class GFE_UnpackByGroup : public GFE_GeoFilter {

public:
	using GFE_GeoFilter::GFE_GeoFilter;
	//
	// GFE_UnpackByGroup(
	// 	GFE_Detail* const geo,
	// 	const GA_Detail* const inGeo0 = nullptr,
	// 	const SOP_NodeVerb::CookParms* const cookparms = nullptr
	// )
	// 	: GFE_GeoFilter(geo, cookparms)
	// 	, inGeo(inGeo0)
	// {
	// }
	//
	// GFE_UnpackByGroup(
	// 	GA_Detail& geo,
	// 	const GA_Detail* const inGeo = nullptr,
	// 	const SOP_NodeVerb::CookParms* const cookparms = nullptr
	// )
	// 	: GFE_GeoFilter(geo, cookparms)
	// 	, inGeo(inGeo)
	// {
	// }
	//
	// GFE_UnpackByGroup(
	// 	GA_Detail& geo,
	// 	const GA_Detail* const inGeo,
	// 	const SOP_NodeVerb::CookParms& cookparms
	// )
	// 	: GFE_GeoFilter(geo, cookparms)
	// 	, inGeo(inGeo)
	// {
	// }

	
    void
        setComputeParm(
			const GFE_UnpackByGroup_Type type = GFE_UnpackByGroup_Type::OneElem,
            const bool reverseGroup = true,
			const bool delGroup = true
        )
    {
        setHasComputed();
        this->type = type;
        this->reverseGroup = reverseGroup;
        this->delGroup = delGroup;
    }


	SYS_FORCE_INLINE void setPrimoff(const GA_Offset primoff = 0, const bool inputAsOffset = true)
    {
    	this->primoff = primoff;
    	this->inputAsOffset = inputAsOffset;
    }

	SYS_FORCE_INLINE void setSkipNPrim(const GA_Offset primoff = 0, const bool inputAsOffset = true, const GA_Size skipNPrim = 1)
    {
    	this->primoff = primoff;
    	this->inputAsOffset = inputAsOffset;
    	this->skipNPrim = skipNPrim;
    }


	


private:

    virtual bool
        computeCore() override
    {
    	if (geoSrc)
    	{
    		
    	}
    	else
    	{
    		
    		if (geoSrc)
    		{
    			geo.replaceWith(*geoSrc);
    			//this->geoSrc = geoSrc;
    		}
    		else
    		{
    			// geoSrcTmp = new GU_Detail();
    			// geoSrcTmp_h.allocateAndSet(geoSrcTmp);
    			// geoSrcTmp->replaceWith(geo);
    			//this->geoSrc = geoSrc;
    		}
    	}

    	switch (type) {
    	case GFE_UnpackByGroup_Type::Custom:    delAndUnpack_Custom();      break;
    	case GFE_UnpackByGroup_Type::OneElem:   delAndUnpack_OneElem();     break;
    	case GFE_UnpackByGroup_Type::SkipNElem: delAndUnpack_SkipNElem();   break;
    	default: break;
    	}
    
    	if (delGroup)
    		groupParser.delGroup();

    	GA_Offset start, end;
    	for (GA_Iterator it(geo->getPrimitiveRange()); it.fullBlockAdvance(start, end); )
    	{
	        for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
	        {
	        	int typeId = geo->getPrimitiveTypeId(elemoff);
	        	if (typeId != 25 && typeId != 27 )
	        		continue;
	        	GA_Primitive* const prim = geo->getPrimitive(elemoff);
	            GU_PrimPacked* const primPacked = static_cast<GU_PrimPacked*>(prim);
	        	primPacked->unpack(*geo->asGU_Detail());
	        }
    	}
    	
        return true;
    }


	SYS_FORCE_INLINE void delAndUnpack_Custom()
    { geo->deleteElements(groupParser.getGroup(), reverseGroup, true, GA_Detail::GA_LEAVE_PRIMITIVES); }
	
	SYS_FORCE_INLINE void delAndUnpack_OneElem()
    { geo->deleteOnePrimitive(inputAsOffset ? primoff : geo->primitiveOffset(primoff), reverseGroup, true); }
	
	SYS_FORCE_INLINE void delAndUnpack_SkipNElem()
    { geo->deletePrimitiveSkipNElem(inputAsOffset ? primoff : geo->primitiveOffset(primoff), skipNPrim, reverseGroup, true); }


public:
    GFE_UnpackByGroup_Type type = GFE_UnpackByGroup_Type::OneElem;
	
    bool reverseGroup = false;
    bool delGroup = true;
	
	GA_Offset primoff = 0;
	bool inputAsOffset = true;
	GA_Size skipNPrim = 1;
	
private:

    //exint subscribeRatio = 64;
    //exint minGrainSize = 1024;


}; // End of class GFE_UnpackByGroup



#endif
