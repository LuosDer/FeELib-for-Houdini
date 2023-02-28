
#pragma once

#ifndef __GFE_SetVectorComponent_h__
#define __GFE_SetVectorComponent_h__

//#include "GFE/GFE_SetVectorComponent.h"

#include "GA/GA_Detail.h"


#include "GA/GA_AttributeFilter.h"
#include "GA/GA_PageHandle.h"
#include "GA/GA_PageIterator.h"



#include "GFE/GFE_Type.h"


class GFE_SetVectorComponent {



public:

    GFE_SetVectorComponent(
        GA_Detail* const geo,
        const SOP_NodeVerb::CookParms* const cookparms = nullptr
    )
        : geo(geo)
        , cookparms(cookparms)
    {
        UT_ASSERT_MSG(geo, "do not find geo");
    }

    GFE_SetVectorComponent(
        const SOP_NodeVerb::CookParms& cookparms,
        GA_Detail* const geo
    )
        : geo(geo)
        , cookparms(&cookparms)
    {
        UT_ASSERT_MSG(geo, "do not find geo");
    }

    GFE_SetVectorComponent(
        const GA_Detail* const geo,
        const GA_ElementGroup* const geoGroup,
        const GA_AttributeOwner attribClass,
        const bool doNormalize = true,
        const fpreal64 uniScale = 1
    )
        : geo(geo)
        , geoGroup(geoGroup)
        , attribClass(attribClass)
        , doNormalize(doNormalize)
        , uniScale(uniScale)
    {
    }


    ~GFE_SetVectorComponent()
    {
    }



};

namespace GFE_SetVectorComponent_Namespace {

template<typename T>
static GA_Size
setVectorComponent(
    const GA_Attribute* const attribPtr,
    const int comp,
    const T& attribVal,
    const exint subscribeRatio = 64,
    const exint minGrainSize = 1024
)
{
    UT_ASSERT_P(attribPtr);
    const GA_SplittableRange geoSplittableRange(GA_Range(attribPtr->getIndexMap()));
    UTparallelFor(geoSplittableRange, [attribPtr, &attribVal](const GA_SplittableRange& r)
    {
        GA_PageHandleT<T, typename T::value_type, true, true, GA_Attribute, GA_ATINumeric, GA_Detail> attrib_ph(attribPtr);
        for (GA_PageIterator pit = r.beginPages(); !pit.atEnd(); ++pit)
        {
            GA_Offset start, end;
            for (GA_Iterator it(pit.begin()); it.blockAdvance(start, end); )
            {
                attrib_ph.setPage(start);
                for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                {
                    count += attrib_ph.value(elemoff) == attribVal;
                }
            }
        }
    }, subscribeRatio, minGrainSize);
}

template<typename T>
static GA_Size
setVectorComponent(
    const GA_Attribute* const attribPtr,
    const GA_Attribute* const attribRefPtr,
    const int comp,
    const T& attribVal,
    const exint subscribeRatio = 64,
    const exint minGrainSize = 1024
    )
{
    UT_ASSERT_P(attribPtr);
    if (!attribRefPtr)
        return setVectorComponent(attribPtr, attribVal, subscribeRatio, minGrainSize);

    const GA_SplittableRange geoSplittableRange(GA_Range(attribPtr->getIndexMap()));
    UTparallelFor(geoSplittableRange, [attribPtr, &attribVal](const GA_SplittableRange& r)
    {
        GA_PageHandleT<T, typename T::value_type, true, true, GA_Attribute, GA_ATINumeric, GA_Detail> attrib_ph(attribPtr);
        for (GA_PageIterator pit = r.beginPages(); !pit.atEnd(); ++pit)
        {
            GA_Offset start, end;
            for (GA_Iterator it(pit.begin()); it.blockAdvance(start, end); )
            {
                attrib_ph.setPage(start);
                for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                {
                    count += attrib_ph.value(elemoff) == attribVal;
                }
            }
        }
    }, subscribeRatio, minGrainSize);

    return count;
}













} // End of namespace GFE_SetVectorComponent

#endif
