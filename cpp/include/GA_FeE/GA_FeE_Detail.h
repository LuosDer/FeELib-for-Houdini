
#pragma once

#ifndef __GA_FeE_Detail_h__
#define __GA_FeE_Detail_h__

#ifndef MAXLOOPCOUNT
#define MAXLOOPCOUNT 1000000000
#endif


//#include "GA_FeE/GA_FeE_Detail.h"

#include "GA/GA_Detail.h"
//#include "GA/GA_Types.h"

#include "GA_FeE/GA_FeE_Type.h"




namespace GA_FeE_Detail {


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
    return numelems(geo, GA_FeE_Type::attributeOwner_groupType(groupType));
}


static void
delByGroup(
    GA_Detail* const geo,
    GA_Group*& group,
    const bool reverse = false,
    const bool delGroup = true,
    const bool delWithPoint = true,
    const GA_Detail::GA_DestroyPointMode delPointMode = GA_Detail::GA_DestroyPointMode::GA_DESTROY_DEGENERATE_INCOMPATIBLE,
    const bool guaranteeNoVertexReference = false
)
{
    UT_VERIFY_P(geo);
    if (!group)
    {
        if (reverse)
        {
            geo->destroyPointOffsets(GA_Range(geo->getPointMap(), nullptr), delPointMode, guaranteeNoVertexReference);
            //geo->clearTopologyAttributes();
            //geo->createTopologyAttributes();
        }
        return;
    }
    switch (group->classType())
    {
    case GA_GROUP_PRIMITIVE:
    {
        const GA_Range&& range = GA_Range(geo->getPrimitiveMap(), UTverify_cast<GA_PrimitiveGroup*>(group), reverse);
        geo->destroyPrimitives(range, delWithPoint);
    }
        break;
    case GA_GROUP_POINT:
        geo->destroyPointOffsets( GA_Range(geo->getPointMap(),     UTverify_cast<GA_PointGroup*>(group),     reverse), delPointMode, guaranteeNoVertexReference);
        break;
    case GA_GROUP_VERTEX:
        geo->destroyVertexOffsets(GA_Range(geo->getVertexMap(),    UTverify_cast<GA_VertexGroup*>(group),    reverse));
        break;
    case GA_GROUP_EDGE:
        UT_ASSERT_MSG(0, "not possible");
        break;
    default:
        UT_ASSERT_MSG(0, "not possible");
        break;
    }
    if (delGroup)
    {
        geo->destroyGroup(group);
        group = nullptr;
    }
}

//GA_OffsetList offList = GA_FeE_Detail::getOffsetList(geo->getPrimitiveMap(), group);
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


//GA_OffsetList offList = GA_FeE_Detail::getOffsetList(geo, owner, group);
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

//GA_OffsetList offList = GA_FeE_Detail::getOffsetList(geo, group);
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

//GA_OffsetList offList = GA_FeE_Detail::getOffsetList(geo, group);
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

//GA_OffsetList offList = GA_FeE_Detail::getOffsetList(geo, group);
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




} // End of namespace GA_FeE_Detail

#endif
