
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
#include "GA_FeE/GA_FeE_TopologyReference.h"

#include "GEO/GEO_PrimPoly.h"



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



    


} // End of namespace GA_FeE_Detail

#endif
