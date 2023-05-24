
#pragma once

#ifndef __GFE_UnpackByGroup_h__
#define __GFE_UnpackByGroup_h__

#include "GFE/GFE_UnpackByGroup.h"



#include "GFE/GFE_GeoFilter.h"


#include "GU/GU_PrimPacked.h"

	
class GFE_UnpackByGroup : public GFE_GeoFilter {

public:
	using GFE_GeoFilter::GFE_GeoFilter;
	
	
    void
        setComputeParm(
			const GFE_ElemTraversingMethod elemTraversingMethod = GFE_ElemTraversingMethod::OneElem,
            const bool reverseGroup = true,
			const bool delGroup = true
        )
    {
        setHasComputed();
        this->elemTraversingMethod = elemTraversingMethod;
        this->reverseGroup = reverseGroup;
        this->delGroup = delGroup;
    }


	SYS_FORCE_INLINE void setPrimoff(const GA_Offset primoff = 0)
    { this->primoff = primoff; }

	SYS_FORCE_INLINE void setSkipNPrim(const GA_Offset primoff = 0, const GA_Size skipNPrim = 1)
    {
    	this->primoff = primoff;
    	this->skipNPrim = skipNPrim;
    }


	


private:

    virtual bool
        computeCore() override
    {
    	if(geoSrc)
    	{
        	geoSrcTmp = geoSrc;
    	}
        else
        {
        	geoSrcTmpGU = new GU_Detail();
        	geoSrcTmp_h.allocateAndSet(geoSrcTmpGU);
        	geoSrcTmpGU->replaceWith(*geo->asGA_Detail());
        	geoSrcTmp = geoSrcTmpGU;
        }

    	GFE_GroupParser groupParserSrc(geoSrcTmp, groupParser.getGOPRef(), cookparms);
    	groupParserSrc.setGroup(groupParser);
    	
    	geo->clear();
		
    	switch (elemTraversingMethod) {
    	case GFE_ElemTraversingMethod::Custom:    unpackByGroup_Custom(groupParserSrc);      break;
    	case GFE_ElemTraversingMethod::OneElem:   unpackByGroup_OneElem();     break;
    	case GFE_ElemTraversingMethod::SkipNElem: unpackByGroup_SkipNElem();   break;
    	default: break;
    	}
    
    	if (delGroup)
    		groupParser.delGroup();

        return true;
    }


	void unpackPrim(GU_Detail& geoGU, const GA_Offset primoff)
	{
    	if (GFE_Type::isPacked(geoSrcTmp->getPrimitiveTypeId(primoff)))
			static_cast<const GU_PrimPacked*>(geoSrcTmp->getPrimitive(primoff))->unpack(geoGU);
		//GA_Primitive* const prim = geoSrcTmp->getPrimitive(primoff);
		//GU_PrimPacked* const primPacked = static_cast<GU_PrimPacked*>(prim);
	}
	
	void unpackByGroup_Custom(GFE_GroupParser &groupParser)
    {
    	GU_Detail& geoGU = *geo->asGU_Detail();
    	GA_Offset start, end;
    	for (GA_Iterator it(groupParser.getPrimitiveRange()); it.fullBlockAdvance(start, end); )
    	{
    		for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
    		{
    			unpackPrim(geoGU, elemoff);
    		}
    	}
	}
	
	void unpackByGroup_OneElem()
    {
    	GU_Detail& geoGU = *geo->asGU_Detail();
    	if (reverseGroup)
    	{
    		GA_Offset start, end;
    		for (GA_Iterator it(geoSrcTmp->getPrimitiveRange()); it.fullBlockAdvance(start, end); )
    		{
    			for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
    			{
    				if (elemoff == primoff)
    					continue;
    				unpackPrim(geoGU, elemoff);
				}
    		}
    	}
    	else
    	{
    		unpackPrim(geoGU, primoff);
    	}
    }
	
	void unpackByGroup_SkipNElem()
    {
    	GU_Detail& geoGU = *geo->asGU_Detail();
    	GA_Offset start, end;
    	if (skipNPrim <= 0)
    	{
    		if (!reverseGroup)
    		{
    			for (GA_Iterator it(geoSrcTmp->getPrimitiveRange()); it.fullBlockAdvance(start, end); )
    			{
    				for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
    				{
    					unpackPrim(geoGU, elemoff);
    				}
    			}
    		}
    		return;
    	}
        
    	GA_Size delSize = geo->getPrimitiveMap().indexFromOffset(primoff);
    	delSize %= skipNPrim;
        
    	for (GA_Iterator it(geoSrcTmp->getPrimitiveRange()); it.fullBlockAdvance(start, end); )
    	{
    		for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
    		{
    			if (delSize!=0 ^ !reverseGroup)
    				unpackPrim(geoGU, elemoff);
                
    			if (delSize == skipNPrim)
    				delSize = 0;
    			else
    				++delSize;
    		}
    	}
    }


public:
    GFE_ElemTraversingMethod elemTraversingMethod = GFE_ElemTraversingMethod::OneElem;
	
    bool reverseGroup = false;
    bool delGroup = true;
	
	GA_Offset primoff = 0;
	GA_Size skipNPrim = 1;
	
private:
	
	const GA_Detail* geoSrcTmp;
	GU_Detail* geoSrcTmpGU;
	
	GU_DetailHandle geoSrcTmp_h;
	
    //exint subscribeRatio = 64;
    //exint minGrainSize = 1024;


}; // End of class GFE_UnpackByGroup



#endif
