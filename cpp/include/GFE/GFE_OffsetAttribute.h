
#pragma once

#ifndef __GFE_OffsetAttribute_h__
#define __GFE_OffsetAttribute_h__

//#include "GFE/GFE_OffsetAttribute.h"

#include "GA/GA_Detail.h"
#include "GA/GA_PageHandle.h"

#include "GFE/GFE_Type.h"





class GFE_OffsetAttrib {

public:

	GFE_OffsetAttrib(){}

	~GFE_OffsetAttrib(){}

public:

	//const GA_Attribute*
	//	getAttribute()
	//{
	//	return attrib;
	//}

	//GA_Attribute*
	//	getAttributeNonconst()
	//{
	//	return attribNonconst;
	//}

	inline
	bool
		isValidh() const
	{
		return attrib_h.isValid();
	}

	inline
	bool
		isValidph() const
	{
		return attrib_ph.isValid();
	}
	
	inline
	void
		clear()
	{
		attrib_h.clear();
		attrib_ph.clear();
	}

	
	void
		bind(
			const GA_Attribute& attrib,
			const bool isOffset
		)
	{
		attrib_h.bind(&attrib);
		this->isOffset = isOffset;
		indexMap = &attrib.getIndexMap();
	}

	inline
		void
		bind(
			const GA_Detail& geo,
			const GA_AttributeOwner owner,
			const UT_StringHolder& attribName,
			const bool isOffset
		)
	{
		const GA_Attribute* const attrib = geo.findAttribute(owner, attribName);
		if (attrib)
			bind(*attrib, isOffset);
		else
			attrib_h.clear();
	}

	inline
	void
		bind(
			const GA_Detail& geo,
			const GA_GroupType owner,
			const UT_StringHolder& attribName,
			const bool isOffset
		)
	{
		bind(geo, GFE_Type::attributeOwner_groupType(owner), attribName, isOffset);
	}



	void
		bindPage(
			const GA_Attribute& attrib,
			const bool isOffset
		)
	{
		attrib_ph.bind(&attrib);
		this->isOffset = isOffset;
		indexMap = &attrib.getIndexMap();
	}

	inline
		void
		bindPage(
			const GA_Detail& geo,
			const GA_AttributeOwner owner,
			const UT_StringHolder& attribName,
			const bool isOffset
		)
	{
		const GA_Attribute* const attrib = geo.findAttribute(owner, attribName);
		if (attrib)
			bindPage(*attrib, isOffset);
		else
			attrib_ph.clear();
	}

	inline
		void
		bindPage(
			const GA_Detail& geo,
			const GA_GroupType owner,
			const UT_StringHolder& attribName,
			const bool isOffset
		)
	{
		bindPage(geo, GFE_Type::attributeOwner_groupType(owner), attribName, isOffset);
	}





	inline
	void
		setPage(
			const GA_Offset pageBase
		)
	{
		UT_ASSERT(attrib_ph.getAttribute());
		attrib_ph.setPage(pageBase);
	}

	void
		bindIndexMap(
			const GA_IndexMap& indexMap
		)
	{
		this->indexMap = &indexMap;
	}




	inline
	GA_Offset
		getOffset(
			const GA_Offset elemoff
		) const
	{
		UT_ASSERT(attrib_h.getAttribute());
		if (isOffset)
			return attrib_h.get(elemoff);
		else
			return indexMap->offsetFromIndex(attrib_h.get(elemoff));
	}

	inline
	GA_Index
		getIndex(
			const GA_Offset elemoff
		) const
	{
		UT_ASSERT(attrib_h.getAttribute());
		if (isOffset)
			return indexMap->indexFromOffset(attrib_h.get(elemoff));
		else
			return attrib_h.get(elemoff);
	}

	inline
	GA_Offset
		getPageOffset(
			const GA_Offset elemoff
		) const
	{
		UT_ASSERT(attrib_ph.getAttribute());
		if (isOffset)
			return attrib_ph.get(elemoff);
		else
			return indexMap->offsetFromIndex(attrib_ph.get(elemoff));
	}

	inline
	GA_Index
		getPageIndex(
			const GA_Offset elemoff
		) const
	{
		UT_ASSERT(attrib_ph.getAttribute());
		if (isOffset)
			return indexMap->indexFromOffset(attrib_ph.get(elemoff));
		else
			return attrib_ph.get(elemoff);
	}

private:
	//const GA_Attribute* attrib;
	GA_ROHandleT<GA_Offset> attrib_h;
	GA_PageHandleScalar<GA_Offset>::ROType attrib_ph;
	//GA_AttributeOwner attribOwner;
	const GA_IndexMap* indexMap;

	//GA_Attribute* attribNonconst;
	bool isOffset;

}; // End of class GFE_OffsetAttribute

#endif
