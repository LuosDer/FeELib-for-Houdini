
#pragma once

#ifndef __GA_FeE_Attribute_h__
#define __GA_FeE_Attribute_h__

//#include "GA_FeE/GA_FeE_Attribute.h"

#include "GA/GA_Detail.h"

#include "GA/GA_PageHandle.h"
#include "GA/GA_PageIterator.h"

#include "GA_FeE/GA_FeE_Type.h"


namespace GA_FeE_Attribute {





SYS_FORCE_INLINE
    static bool
    deleteAttribute(
        GA_Attribute* attrib
    )
{
    UT_ASSERT_P(attrib);
    return attrib->getDetail().getAttributes().destroyAttribute(attrib);
}



SYS_FORCE_INLINE
static bool
attributeDelete(
    GA_Attribute* attrib
)
{
    UT_ASSERT_P(attrib);
    return attrib->getDetail().getAttributes().destroyAttribute(attrib);
}

SYS_FORCE_INLINE
static bool
attribDelete(
    GA_Attribute* attrib
)
{
    UT_ASSERT_P(attrib);
    return attrib->getDetail().getAttributes().destroyAttribute(attrib);
}


SYS_FORCE_INLINE
static bool
    destroyAttribute(
        GA_Attribute* attrib
    )
{
    UT_ASSERT_P(attrib);
    return attrib->getDetail().getAttributes().destroyAttribute(attrib);
}




SYS_FORCE_INLINE
static bool
renameAttribute(
    GA_Detail* geo,
    const GA_Attribute* attrib,
    const UT_StringHolder& newName
)
{
    UT_ASSERT_P(geo);
    UT_ASSERT_P(attrib);
    return geo->renameAttribute(attrib->getOwner(), attrib->getScope(), attrib->getName(), newName);
}



SYS_FORCE_INLINE
static bool
renameAttribute(
    GA_Attribute* attrib,
    const UT_StringHolder& newName
)
{
    UT_ASSERT_P(attrib);
    return attrib->getDetail().renameAttribute(attrib->getOwner(), attrib->getScope(), attrib->getName(), newName);
}





static bool
forceRenameAttribute(
    GA_Detail* geo,
    GA_Attribute* attrib,
    const UT_StringHolder& newName
)
{
    GA_Attribute* existAttribute = geo->findAttribute(attrib->getOwner(), newName);
    if (existAttribute)
        geo->getAttributes().destroyAttribute(existAttribute);
    return geo->renameAttribute(GA_ATTRIB_POINT, GA_SCOPE_PUBLIC, attrib->getName(), newName);
}


SYS_FORCE_INLINE
static bool
forceRenameAttribute(
    GA_Attribute* attrib,
    const UT_StringHolder& newName
)
{
    GA_Detail& geo = attrib->getDetail();
    return forceRenameAttribute(&geo, attrib, newName);
}






//GA_FeE_Attribute::findFloatTuplePointVertex(outGeo0, geo0AttribClass, geo0AttribNameSub, geo0AttribClassFinal)
//GA_FeE_Attribute::findFloatTuplePointVertex(geo, attribOwner, GA_SCOPE_PUBLIC, attribName, attribOwnerFianl);

static const GA_Attribute*
findFloatTuplePointVertex(
    const GEO_Detail* geo,
    const GA_AttributeOwner attribOwner,
    const GA_AttributeScope scope,
    const UT_StringRef& attribName,
    GA_AttributeOwner& attribOwnerFinal,
    const int min_size = 1,
    const int max_size = -1
)
{
    const GA_Attribute* attribPtr = nullptr;
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
                return nullptr;
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
            return nullptr;
        }
        attribOwnerFinal = attribOwner;
    }
    return attribPtr;
}


static GA_Attribute*
findFloatTuplePointVertex(
    GEO_Detail* geo,
    const GA_AttributeOwner attribOwner,
    const GA_AttributeScope scope,
    const UT_StringRef& attribName,
    GA_AttributeOwner& attribOwnerFinal,
    const int min_size = 1,
    const int max_size = -1
)
{
    GA_Attribute* attribPtr = nullptr;
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
                return nullptr;
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
            return nullptr;
        }
        attribOwnerFinal = attribOwner;
    }
    return attribPtr;
}




//GA_FeE_Attribute::findFloatTuplePointVertex(outGeo0, geo0AttribClass, geo0AttribNameSub, geo0AttribClassFinal)
//GA_FeE_Attribute::findFloatTuplePointVertex(geo, attribOwner, attribName, attribOwnerFianl);



SYS_FORCE_INLINE
static const GA_Attribute*
findFloatTuplePointVertex(
    const GEO_Detail* geo,
    const GA_AttributeOwner attribOwner,
    const UT_StringRef& attribName,
    GA_AttributeOwner& attribOwnerFianl,
    const int min_size = 1,
    const int max_size = -1
)
{
    return findFloatTuplePointVertex(geo, attribOwner, GA_SCOPE_PUBLIC, attribName, attribOwnerFianl, min_size, max_size);
}


SYS_FORCE_INLINE
static GA_Attribute*
findFloatTuplePointVertex(
    GEO_Detail* geo,
    const GA_AttributeOwner attribOwner,
    const UT_StringRef& attribName,
    GA_AttributeOwner& attribOwnerFianl,
    const int min_size = 1,
    const int max_size = -1
)
{
    return findFloatTuplePointVertex(geo, attribOwner, GA_SCOPE_PUBLIC, attribName, attribOwnerFianl, min_size, max_size);
}





//GA_FeE_Attribute::normalizeElementAttrib(geo0SplittableRange, attribPtr,
//    doNormalize, uniScale,
//    subscribeRatio, minGrainSize);


static void
normalizeAttribElement(
    const GA_SplittableRange& geoSplittableRange,
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


static void
normalizeAttribElement(
    const GA_SplittableRange& geoSplittableRange,
    const GA_RWHandleT<UT_Vector3F>& attribHandle,
    const bool doNormalize = 1,
    const fpreal64 uniScale = 1,
    const exint subscribeRatio = 64,
    const exint minGrainSize = 64
)
{
    UTparallelFor(geoSplittableRange, [&attribHandle, &doNormalize, &uniScale](const GA_SplittableRange& r)
    {
        GA_Offset start, end;
        for (GA_Iterator it(r); it.blockAdvance(start, end); )
        {
            for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
            {
                UT_Vector3F attribValue = attribHandle.get(elemoff);
                if (doNormalize)
                    attribValue.normalize();
                attribValue *= uniScale;
                attribHandle.set(elemoff, attribValue);
            }
        }
    }, subscribeRatio, minGrainSize);
}




} // End of namespace GA_FeE_Attribute

#endif
