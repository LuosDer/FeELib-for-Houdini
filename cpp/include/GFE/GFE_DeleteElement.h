
#pragma once

#ifndef __GFE_DelElement_h__
#define __GFE_DelElement_h__


#include "GFE/GFE_DeleteElement.h"



#include "GFE/GFE_GeoFilter.h"



class GFE_DelElement : public GFE_GeoFilter {

public:
	using GFE_GeoFilter::GFE_GeoFilter;
	//
	// GFE_DelElement(
	// 	GFE_Detail* const geo,
	// 	const GA_Detail* const inGeo0 = nullptr,
	// 	const SOP_NodeVerb::CookParms* const cookparms = nullptr
	// )
	// 	: GFE_GeoFilter(geo, cookparms)
	// 	, geoSrc(inGeo0)
	// {
	// }
	//
	// GFE_DelElement(
	// 	GA_Detail& geo,
	// 	const GA_Detail* const geoSrc = nullptr,
	// 	const SOP_NodeVerb::CookParms* const cookparms = nullptr
	// )
	// 	: GFE_GeoFilter(geo, cookparms)
	// 	, geoSrc(geoSrc)
	// {
	// }
	//
	// GFE_DelElement(
	// 	GA_Detail& geo,
	// 	const GA_Detail* const geoSrc,
	// 	const SOP_NodeVerb::CookParms& cookparms
	// )
	// 	: GFE_GeoFilter(geo, cookparms)
	// 	, geoSrc(geoSrc)
	// {
	// }

	
    void
        setComputeParm(
	        const bool leaveElement = false,
			const bool reverseGroup = false,
			const bool delGroup = true,
			const GA_Detail::GA_DestroyPointMode delPointMode = GA_Detail::GA_LEAVE_PRIMITIVES,
            const bool delUnusedPoint = true,
            const bool guaranteeNoVertexReference = false
        )
    {
    	setHasComputed();
    	this->leaveElement = leaveElement;
    	this->reverseGroup = reverseGroup;
    	this->delGroup = delGroup;
        this->delPointMode = delPointMode;
        this->delUnusedPoint = delUnusedPoint;
        this->guaranteeNoVertexReference = guaranteeNoVertexReference;
    }






private:

    virtual bool
        computeCore() override
    {
        if (!reverseGroup && groupParser.isEmpty())
            return true;

    	if(leaveElement)
    	{
    		switch (groupParser.classType())
    		{
    		case GA_GROUP_PRIMITIVE: leaveElementFunc(groupParser.getPrimitiveGroup()); break;
    		case GA_GROUP_POINT:     leaveElementFunc(groupParser.getPointGroup());     break;
    		case GA_GROUP_VERTEX:    leaveElementFunc(groupParser.getVertexGroup());    break;
    		case GA_GROUP_EDGE:      leaveElementFunc(groupParser.getEdgeGroup());      break;
    		default:                 leaveElementFunc();                                break;
    		}
    	}
        else
        {
        	switch (groupParser.classType())
        	{
        	case GA_GROUP_PRIMITIVE: delElement(groupParser.getPrimitiveGroup()); break;
        	case GA_GROUP_POINT:     delElement(groupParser.getPointGroup());     break;
        	case GA_GROUP_VERTEX:    delElement(groupParser.getVertexGroup());    break;
        	case GA_GROUP_EDGE:      delElement(groupParser.getEdgeGroup());      break;
        	default:                 delElement();                                break;
        	}
        }
    	
    	if (delGroup)
    		groupParser.delGroup();
    	
        return true;
    }

	
	void leaveElementFunc()
    {
    }
	
	void leaveElementFunc(const GA_PrimitiveGroup* const group)
    {
    }
	void leaveElementFunc(const GA_PointGroup* const group)
    {
    }
	void leaveElementFunc(const GA_VertexGroup* const group)
    {
    }
	void leaveElementFunc(const GA_EdgeGroup* const group)
    {
    }
	
	void delElement()
	{
    	if (geoSrc)
    	{
    		if (reverseGroup)
    		{
    			geo->replaceWith(*geoSrc);
    			return;
    		}
    	}
    	else
    	{
    		if (reverseGroup)
    			return;
    		
    		if (delPointMode)
    			//geo->destroyPointOffsets(GA_Range(geo->getPointMap(), nullptr), delPointMode, guaranteeNoVertexReference);
    			geo->destroyPrimitives(geo->getPrimitiveRange(), delUnusedPoint);
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
    			if (delUnusedPoint)
    				geo->destroyUnusedPoints();
    		}
    	}
	}


	void delElement(const GA_PrimitiveGroup* const group)
    {
    	if (geoSrc)
    	{
    	}
    	else
    	{
    	}
    	if (!group)
			return delElement();

    	const GA_Range range(geo->getPrimitiveMap(), group, reverseGroup);
    	geo->destroyPrimitives(range, delUnusedPoint);
    	
		if (delPointMode)
			geo->destroyPrimitives(geo->getPrimitiveRange(group, reverseGroup), delUnusedPoint);
		else
		{
			GA_PointGroupUPtr pointGroupUPtr;
			GA_PointGroup* pointGroup;
			if (delUnusedPoint)
			{
				pointGroupUPtr = geo->createDetachedPointGroup();
				pointGroup = pointGroupUPtr.get();
			}

			GA_Topology& topo = geo->getTopology();
			const GA_ATITopology* const vtxPointRef = topo.getPointRef();

			GA_Offset start, end;
			for (GA_Iterator it(geo->getPrimitiveRange(group, reverseGroup)); it.blockAdvance(start, end); )
			{
				for (GA_Offset primoff = start; primoff < end; ++primoff)
				{
					const GA_OffsetListRef& vertices = geo->getPrimitiveVertexList(primoff);
					for (GA_Size i = 0; i < vertices.size(); i++)
					{
						if (delUnusedPoint)
							pointGroup->setElement(vtxPointRef->getLink(vertices[i]), true);
						geo->getPrimitive(primoff)->releaseVertex(vertices[i]);
						topo.delVertex(vertices[i]);
					}
				}
			}
			if (delUnusedPoint)
				geo->destroyUnusedPoints(pointGroup);
		}
	}


	void delElement(const GA_PointGroup* const group)
    {
    	if (geoSrc)
    	{
    	}
    	else
    	{
    	}
		if (!group)
			return delElement();

    	geo->destroyPointOffsets(GA_Range(geo->getPointMap(), group, reverseGroup), delPointMode, guaranteeNoVertexReference);

    	
		if (delUnusedPoint)
		{
			geo->destroyPointOffsets(geo->getPointRange(group, reverseGroup), delPointMode);
		}
		else
		{
			GA_PrimitiveGroupUPtr primGroupUPtr;
			GA_PrimitiveGroup* primGroup;
			if (delPointMode)
			{
				primGroupUPtr = geo->createDetachedPrimitiveGroup();
				primGroup = primGroupUPtr.get();
			}

			GA_Topology& topo = geo->getTopology();
			const GA_ATITopology* const pointVtxRef = topo.getVertexRef();
			const GA_ATITopology* const vtxPrimRef = topo.getPrimitiveRef();
			const GA_ATITopology* const vtxNextRef = topo.getVertexNextRef();

			GA_Offset start, end;
			for (GA_Iterator it(geo->getPointRange(group, reverseGroup)); it.blockAdvance(start, end); )
			{
				for (GA_Offset ptoff = start; ptoff < end; ++ptoff)
				{
					for (GA_Offset vtxoff_next = pointVtxRef->getLink(ptoff); vtxoff_next != GA_INVALID_OFFSET; vtxoff_next = vtxNextRef->getLink(vtxoff_next))
					{
						GA_Offset primoff = vtxPrimRef->getLink(vtxoff_next);

						geo->getPrimitive(primoff)->releaseVertex(vtxoff_next);
						topo.delVertex(vtxoff_next);

						if (delPointMode)
							primGroup->setElement(primoff, true);
					}
				}
			}
			if (delPointMode)
				geo->destroyPrimitives(geo->getPrimitiveRange(primGroup));
		}
	}





	void delElement(const GA_VertexGroup* const group)
    {
    	if (geoSrc)
    	{
    	}
    	else
    	{
    	}
		if (!group)
			return delElement();

    	geo->destroyVertexOffsets(GA_Range(geo->getVertexMap(), group, reverseGroup));
    	
		GA_Topology& topo = geo->getTopology();
		const GA_ATITopology* const vtxPrimRef = topo.getPrimitiveRef();
		const GA_ATITopology* const vtxPointRef = topo.getPointRef();


		GA_PrimitiveGroupUPtr primGroupUPtr;
		GA_PrimitiveGroup* primGroup;
		if (delPointMode)
		{
			primGroupUPtr = geo->createDetachedPrimitiveGroup();
			primGroup = primGroupUPtr.get();
		}

		GA_PointGroupUPtr pointGroupUPtr;
		GA_PointGroup* pointGroup;
		if (delUnusedPoint)
		{
			pointGroupUPtr = geo->createDetachedPointGroup();
			pointGroup = pointGroupUPtr.get();
		}
    	
		GA_Offset start, end;
		for (GA_Iterator it(geo->getVertexRange(group, reverseGroup)); it.blockAdvance(start, end); )
		{
			for (GA_Offset vtxoff = start; vtxoff < end; ++vtxoff)
			{
				GA_Offset primoff = vtxPrimRef->getLink(vtxoff);

				geo->getPrimitive(primoff)->releaseVertex(vtxoff);
				topo.delVertex(vtxoff);

				if (delPointMode)
					primGroup->setElement(primoff, true);
				if (delUnusedPoint)
					pointGroup->setElement(vtxPointRef->getLink(vtxoff), true);
			}
		}

		if (delPointMode)
			geo->destroyDegeneratePrimitives(primGroup);
		if (delUnusedPoint)
			geo->destroyUnusedPoints(pointGroup);
	}


	void delElement(const GA_EdgeGroup* const group)
    {
    	if (geoSrc)
    	{
    	}
    	else
    	{
    	}
		if (!group)
			return delElement();

		const GA_VertexGroupUPtr vtxGroupUPtr = geo->createDetachedVertexGroup();
		GA_VertexGroup* vtxGroup = vtxGroupUPtr.get();
		vtxGroup->combine(group);
		delElement(vtxGroup);
	}


public:
	bool leaveElement = false;
	bool reverseGroup = false;
	bool delGroup = true;
	
    GA_Detail::GA_DestroyPointMode delPointMode = GA_Detail::GA_LEAVE_PRIMITIVES;
    bool delUnusedPoint = true;
    bool guaranteeNoVertexReference = false;
	
private:

    //exint subscribeRatio = 64;
    //exint minGrainSize = 1024;


}; // End of class GFE_DelElement


#endif
