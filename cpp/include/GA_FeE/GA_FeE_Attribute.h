
#pragma once

#ifndef __GA_FeE_Attribute_h__
#define __GA_FeE_Attribute_h__

//#include <GA_FeE/GA_FeE_Attribute.h>

#include <GA/GA_Detail.h>

#include <GA/GA_PageHandle.h>
#include <GA/GA_PageIterator.h>

#include <GA_FeE/GA_FeE_Type.h>


namespace GA_FeE_Attribute {








//GA_FeE_Attribute::findFloatTuplePointVertex(outGeo0, geo0AttribClass, geo0AttribNameSub, attribPtr, geo0AttribClassFinal)
//GA_FeE_Attribute::findFloatTuplePointVertex(geo, attribOwner, GA_SCOPE_PUBLIC, attribName, attribPtr, attribOwnerFianl);

static bool
    findFloatTuplePointVertex(
        GEO_Detail* const geo,
        const GA_AttributeOwner attribOwner,
        const GA_AttributeScope scope,
        const UT_StringRef& attribName,
        GA_Attribute*& attribPtr,
        GA_AttributeOwner& attribOwnerFinal,
        const int min_size = 1,
        const int max_size = -1
    )
{
    if (attribOwner >= GA_ATTRIB_PRIMITIVE)//means Auto
    {
        attribPtr = geo->findFloatTuple(GA_ATTRIB_VERTEX, scope, attribName, min_size, max_size);
        if (attribPtr)
            attribOwnerFinal = GA_ATTRIB_VERTEX;
        else
        {
            attribPtr = geo->findFloatTuple(GA_ATTRIB_POINT, scope, attribName, min_size, max_size);
            if (!attribPtr)
            {
                attribOwnerFinal = GA_ATTRIB_INVALID;
                return false;
            }
            attribOwnerFinal = GA_ATTRIB_POINT;
        }
    }
    else
    {
        attribPtr = geo->findFloatTuple(attribOwner, scope, attribName, min_size, max_size);
        if (!attribPtr)
        {
            attribOwnerFinal = GA_ATTRIB_INVALID;
            return false;
        }
        attribOwnerFinal = attribOwner;
    }
    return true;
}

//GA_FeE_Attribute::findFloatTuplePointVertex(outGeo0, geo0AttribClass, geo0AttribNameSub, attribPtr, geo0AttribClassFinal)
//GA_FeE_Attribute::findFloatTuplePointVertex(geo, attribOwner, attribName, attribPtr, attribOwnerFianl);
static bool
findFloatTuplePointVertex(
    GEO_Detail* const geo,
    const GA_AttributeOwner attribOwner,
    const UT_StringRef& attribName,
    GA_Attribute*& attribPtr,
    GA_AttributeOwner& attribOwnerFianl,
    const int min_size = 1,
    const int max_size = -1
)
{
    return GA_FeE_Attribute::findFloatTuplePointVertex(geo, attribOwner, GA_SCOPE_PUBLIC, attribName, attribPtr, attribOwnerFianl, min_size, max_size);
}


//GA_FeE_Attribute::normalizeElementAttrib(geo0SplittableRange, attribPtr,
//    doNormalize, uniScale,
//    subscribeRatio, minGrainSize);


static void
normalizeElementAttrib(
    const GA_SplittableRange geoSplittableRange,
    GA_Attribute* attribPtr,
    const bool doNormalize = 1,
    const fpreal64 uniScale = 1,
    const exint subscribeRatio = 64,
    const exint minGrainSize = 64
)
{
    UTparallelFor(geoSplittableRange, [&attribPtr, &doNormalize, &uniScale](const GA_SplittableRange& r)
    {
        GA_PageHandleV<UT_Vector3F>::RWType attrib_ph(attribPtr);
        for (GA_PageIterator pit = r.beginPages(); !pit.atEnd(); ++pit)
        {
            GA_Offset start, end;
            for (GA_Iterator it(pit.begin()); it.blockAdvance(start, end); )
            {
                attrib_ph.setPage(start);
                for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                {
                    if (doNormalize)
                        attrib_ph.value(elemoff).normalize();
                    attrib_ph.value(elemoff) *= uniScale;
                }
            }
        }
    }, subscribeRatio, minGrainSize);
}




} // End of namespace GA_FeE_Attribute

#endif
