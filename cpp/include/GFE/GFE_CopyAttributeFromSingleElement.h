
#pragma once

#ifndef __GFE_CopyAttribFromSingleElement_h__
#define __GFE_CopyAttribFromSingleElement_h__

//#include "GFE/GFE_CopyAttribFromSingleElement.h"

#include "GA/GA_Detail.h"



#include "GFE/GFE_CopyAttribFromSingleElement.h"




#if 0







#else














namespace GFE_CopyAttribFromSingleElement {


    static void
        copyAttribFromSingleElement(
            const GA_Detail* const geo,
            const GA_ElementGroup* const geoGroup,
            const GA_AttributeOwner attribClass,
            const UT_StringHolder& attribPattern,
            const bool doNormalize = true,
            const fpreal64 uniScale = 1,
            const exint subscribeRatio = 64,
            const exint minGrainSize = 64
        )
    {
        if (!attribPattern.isstring() || attribPattern.length() == 0)
            return;

        const GA_SplittableRange& geoSplittableRange(GA_Range(geo->getIndexMap(attribClass), geoGroup));
        //const GA_SplittableRange geo0SplittableRange = GFE_Range::getSplittableRangeByAnyGroup(geo, geoGroup, attribClass);

        GA_Attribute* attribPtr = nullptr;
        for (GA_AttributeDict::iterator it = geo->getAttributes().begin(attribClass); !it.atEnd(); ++it)
        {
            attribPtr = it.attrib();
            if (attribPtr->isDetached())
                continue;
            if (!attribPtr->getName().multiMatch(attribPattern))
                continue;
            normalizeAttribElement(attribPtr, geoSplittableRange, doNormalize, uniScale, subscribeRatio, minGrainSize);
            attribPtr->bumpDataId();
        }
    }


#endif







#endif





