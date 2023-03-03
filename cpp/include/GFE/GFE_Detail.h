
#pragma once

#ifndef __GFE_Detail_h__
#define __GFE_Detail_h__



//#include "GFE/GFE_Detail.h"

#include "GA/GA_Detail.h"


#include "GFE/GFE_Type.h"




namespace GFE_Detail {








SYS_FORCE_INLINE
    static GA_Offset
    getPrimitivePointOffset(
        const GA_Detail* const geo,
        const GA_Size primoff,
        const GA_Size vtxpnum
    )
{
    return geo->vertexPoint(geo->getPrimitiveVertexOffset(primoff, vtxpnum));
}

SYS_FORCE_INLINE
static UT_Vector3
getPrimitivePointPos3(
    const GA_Detail* const geo,
    const GA_Size primoff,
    const GA_Size vtxpnum
)
{
    return geo->getPos3(getPrimitivePointOffset(geo, primoff, vtxpnum));
}

SYS_FORCE_INLINE
static UT_Vector3D
getPrimitivePointPos3D(
    const GA_Detail* const geo,
    const GA_Size primoff,
    const GA_Size vtxpnum
)
{
    return geo->getPos3D(getPrimitivePointOffset(geo, primoff, vtxpnum));
}



SYS_FORCE_INLINE
static GA_Size
    numelems(
        const GA_Detail* const geo,
        GA_AttributeOwner attribClass
    )
{
    UT_ASSERT_P(geo);
    switch (attribClass)
    {
    case GA_ATTRIB_PRIMITIVE:
        return geo->getNumPrimitives();
        break;
    case GA_ATTRIB_POINT:
        return geo->getNumPoints();
        break;
    case GA_ATTRIB_VERTEX:
        return geo->getNumVertices();
        break;
    default:
        return -1;
        break;
    }
    return -1;
}

SYS_FORCE_INLINE
static GA_Size
numelems(
    const GA_Detail* const geo,
    GA_GroupType groupType
)
{
    //return numelems(geo, GFE_Type::attributeOwner_groupType(groupType));
    UT_ASSERT_P(geo);
    switch (groupType)
    {
    case GA_GROUP_PRIMITIVE:
        return geo->getNumPrimitives();
        break;
    case GA_GROUP_POINT:
        return geo->getNumPoints();
        break;
    case GA_GROUP_VERTEX:
        return geo->getNumVertices();
        break;
    default:
        return -1;
        break;
    }
    return -1;
}


//extractPoint(geo, srcGeo, groupName, reverseGroup, delGroup);
static void
clearElement(
    GA_Detail* const geo
)
{
    //geo->clear();
    geo->clearTopologyAttributes();
    geo->getIndexMap(GA_ATTRIB_PRIMITIVE).clear(true);
    geo->getIndexMap(GA_ATTRIB_POINT).clear(true);
    geo->getIndexMap(GA_ATTRIB_VERTEX).clear(true);
    geo->getPrimitiveList().clear(true);
    geo->createTopologyAttributes();
}














//GA_OffsetList offList = GFE_Detail::getOffsetList(geo->getPrimitiveMap(), group);
static GA_OffsetList
getOffsetList(
    const GA_IndexMap& indexMap,
    const GA_ElementGroup* const group = nullptr,
    const bool reverse = false
)
{
    GA_OffsetList offs;
    if (!group && indexMap.isTrivialMap())
    {
        if (!reverse)
        {
            offs.setTrivial(GA_Offset(0), indexMap.indexSize());
        }
    }
    else
    {
        GA_Offset start, end;
        for (GA_Iterator it(GA_Range(indexMap, group, reverse)); it.fullBlockAdvance(start, end); )
        {
            offs.setTrivialRange(offs.size(), start, end - start);
        }
    }
    return offs;
}


//GA_OffsetList offList = GFE_Detail::getOffsetList(geo, owner, group);
SYS_FORCE_INLINE
static GA_OffsetList
getOffsetList(
    const GA_Detail* const geo,
    const GA_AttributeOwner owner,
    const GA_ElementGroup* const group = nullptr,
    const bool reverse = false
)
{
    return getOffsetList(geo->getIndexMap(owner), group, reverse);
}

//GA_OffsetList offList = GFE_Detail::getOffsetList(geo, group);
SYS_FORCE_INLINE
static GA_OffsetList
getOffsetList(
    const GA_Detail* const geo,
    const GA_PrimitiveGroup* const group,
    const bool reverse = false
)
{
    return getOffsetList(geo->getPrimitiveMap(), group, reverse);
}

//GA_OffsetList offList = GFE_Detail::getOffsetList(geo, group);
SYS_FORCE_INLINE
static GA_OffsetList
getOffsetList(
    const GA_Detail* const geo,
    const GA_PointGroup* const group,
    const bool reverse = false
)
{
    return getOffsetList(geo->getPointMap(), group, reverse);
}

//GA_OffsetList offList = GFE_Detail::getOffsetList(geo, group);
SYS_FORCE_INLINE
static GA_OffsetList
getOffsetList(
    const GA_Detail* const geo,
    const GA_VertexGroup* const group,
    const bool reverse = false
)
{
    return getOffsetList(geo->getVertexMap(), group, reverse);
}













} // End of namespace GFE_Detail

#endif
