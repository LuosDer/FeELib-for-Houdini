
#pragma once

#ifndef __GFE_Attribute_h__
#define __GFE_Attribute_h__

//#include "GFE/GFE_Attribute.h"

#include "GA/GA_Detail.h"


#include "GA/GA_AttributeFilter.h"
#include "GA/GA_PageHandle.h"
#include "GA/GA_PageIterator.h"



#include "GFE/GFE_Type.h"


namespace GFE_Attribute {

    


static void
bumpDataId(
    GA_Detail* const geo,
    const GA_AttributeOwner owner,
    const UT_StringHolder& attribPattern
)
{
    GA_AttributeFilter filter = GA_AttributeFilter::selectByPattern(attribPattern);
    UT_Array<GA_Attribute*> attribList;
    geo->getAttributes().matchAttributes(filter, owner, attribList);
    for (GA_Size i = 0; i < attribList.size(); ++i)
    {
        attribList[i]->bumpDataId();
    }
}







SYS_FORCE_INLINE
static bool
renameAttribute(
    GA_Detail* const geo,
    const GA_Attribute* const attrib,
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
    GA_Attribute* const attrib,
    const UT_StringHolder& newName
)
{
    UT_ASSERT_P(attrib);
    return attrib->getDetail().renameAttribute(attrib->getOwner(), attrib->getScope(), attrib->getName(), newName);
}





static bool
forceRenameAttribute(
    GA_Detail* geo,
    GA_Attribute* const attrib,
    const UT_StringHolder& newName
)
{
    GA_Attribute* const existAttribute = geo->findAttribute(attrib->getOwner(), newName);
    if (existAttribute)
        geo->getAttributes().destroyAttribute(existAttribute);
    return geo->renameAttribute(attrib->getOwner(), GA_SCOPE_PUBLIC, attrib->getName(), newName);
}


SYS_FORCE_INLINE
static bool
forceRenameAttribute(
    GA_Attribute* const attrib,
    const UT_StringHolder& newName
)
{
    return forceRenameAttribute(&(attrib->getDetail()), attrib, newName);
    //GA_Detail& geo = attrib->getDetail();
    //return forceRenameAttribute(&geo, attrib, newName);
}






//GFE_Attribute::findAttributePointVertex(outGeo0, geo0AttribClass, geo0AttribNameSub, geo0AttribClassFinal)
//GFE_Attribute::findAttributePointVertex(geo, attribOwner, GA_SCOPE_PUBLIC, attribName, attribOwnerFianl);

static const GA_Attribute*
findAttributePointVertex(
    const GA_Detail* const geo,
    const GA_AttributeOwner attribOwner,
    const GA_AttributeScope scope,
    const UT_StringRef& attribName,
    GA_AttributeOwner& attribOwnerFinal
)
{
    const GA_Attribute* attribPtr = nullptr;
    const GA_AttributeSet& geoAttribs = geo->getAttributes();
    if (attribOwner < 0 || attribOwner >= GA_ATTRIB_PRIMITIVE)//not point or vertex means Auto
    {
        attribPtr = geoAttribs.findAttribute(GA_ATTRIB_VERTEX, scope, attribName);
        if (attribPtr)
            attribOwnerFinal = GA_ATTRIB_VERTEX;
        else
        {
            attribPtr = geoAttribs.findAttribute(GA_ATTRIB_POINT, scope, attribName);
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
        attribPtr = geoAttribs.findAttribute(attribOwner, scope, attribName);
        if (!attribPtr)
        {
            attribOwnerFinal = GA_ATTRIB_INVALID;
            return nullptr;
        }
        attribOwnerFinal = attribOwner;
    }
    return attribPtr;
}

static const GA_Attribute*
findAttributePointVertex(
    const GA_Detail* const geo,
    const GA_AttributeOwner attribOwner,
    const GA_AttributeScope scope,
    const UT_StringRef& attribName
)
{
    const GA_Attribute* attribPtr = nullptr;
    const GA_AttributeSet& geoAttribs = geo->getAttributes();
    if (attribOwner < 0 || attribOwner >= GA_ATTRIB_PRIMITIVE)//not point or vertex means Auto
    {
        attribPtr = geoAttribs.findAttribute(GA_ATTRIB_VERTEX, scope, attribName);
        if (!attribPtr)
        {
            attribPtr = geoAttribs.findAttribute(GA_ATTRIB_POINT, scope, attribName);
            if (!attribPtr)
            {
                return nullptr;
            }
        }
    }
    else
    {
        attribPtr = geoAttribs.findAttribute(attribOwner, scope, attribName);
        if (!attribPtr)
        {
            return nullptr;
        }
    }
    return attribPtr;
}





static GA_Attribute*
findAttributePointVertex(
    GA_Detail* const geo,
    const GA_AttributeOwner attribOwner,
    const GA_AttributeScope scope,
    const UT_StringRef& attribName,
    GA_AttributeOwner& attribOwnerFinal
)
{
    GA_Attribute* attribPtr = nullptr;
    GA_AttributeSet& geoAttribs = geo->getAttributes();
    if (attribOwner < 0 || attribOwner >= GA_ATTRIB_PRIMITIVE)//not point or vertex means Auto
    {
        attribPtr = geoAttribs.findAttribute(GA_ATTRIB_VERTEX, scope, attribName);
        if (attribPtr)
            attribOwnerFinal = GA_ATTRIB_VERTEX;
        else
        {
            attribPtr = geoAttribs.findAttribute(GA_ATTRIB_POINT, scope, attribName);
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
        attribPtr = geoAttribs.findAttribute(attribOwner, scope, attribName);
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
findAttributePointVertex(
    GA_Detail* const geo,
    const GA_AttributeOwner attribOwner,
    const GA_AttributeScope scope,
    const UT_StringRef& attribName
)
{
    GA_Attribute* attribPtr = nullptr;
    GA_AttributeSet& geoAttribs = geo->getAttributes();
    if (attribOwner < 0 || attribOwner >= GA_ATTRIB_PRIMITIVE)//not point or vertex means Auto
    {
        attribPtr = geoAttribs.findAttribute(GA_ATTRIB_VERTEX, scope, attribName);
        if (!attribPtr)
        {
            attribPtr = geoAttribs.findAttribute(GA_ATTRIB_POINT, scope, attribName);
            if (!attribPtr)
            {
                return nullptr;
            }
        }
    }
    else
    {
        attribPtr = geoAttribs.findAttribute(attribOwner, scope, attribName);
        if (!attribPtr)
        {
            return nullptr;
        }
    }
    return attribPtr;
}




//GFE_Attribute::findAttributePointVertex(outGeo0, geo0AttribClass, geo0AttribNameSub, geo0AttribClassFinal)
//GFE_Attribute::findAttributePointVertex(geo, attribOwner, attribName, attribOwnerFianl);



SYS_FORCE_INLINE
static const GA_Attribute*
findAttributePointVertex(
    const GA_Detail* const geo,
    const GA_AttributeOwner attribOwner,
    const UT_StringRef& attribName,
    GA_AttributeOwner& attribOwnerFianl
)
{
    return findAttributePointVertex(geo, attribOwner, GA_SCOPE_PUBLIC, attribName, attribOwnerFianl);
}

SYS_FORCE_INLINE
static const GA_Attribute*
findAttributePointVertex(
    const GA_Detail* const geo,
    const GA_AttributeOwner attribOwner,
    const UT_StringRef& attribName
)
{
    return findAttributePointVertex(geo, attribOwner, GA_SCOPE_PUBLIC, attribName);
}



SYS_FORCE_INLINE
static GA_Attribute*
findAttributePointVertex(
    GA_Detail* const geo,
    const GA_AttributeOwner attribOwner,
    const UT_StringRef& attribName,
    GA_AttributeOwner& attribOwnerFianl
)
{
    return findAttributePointVertex(geo, attribOwner, GA_SCOPE_PUBLIC, attribName, attribOwnerFianl);
}

SYS_FORCE_INLINE
static GA_Attribute*
findAttributePointVertex(
    GA_Detail* const geo,
    const GA_AttributeOwner attribOwner,
    const UT_StringRef& attribName
)
{
    return findAttributePointVertex(geo, attribOwner, GA_SCOPE_PUBLIC, attribName);
}




static GA_Attribute*
findUVAttributePointVertex(
    GA_Detail* const geo,
    const GA_AttributeOwner uvAttribClass = GA_ATTRIB_INVALID,
    const UT_StringRef& uvAttribName = "uv",
    const GA_Storage storage = GA_STORE_INVALID
)
{
    GA_Attribute* uvAttribPtr = findAttributePointVertex(geo, uvAttribClass, uvAttribName);
    if (uvAttribPtr)
    {
        int tupleSize = uvAttribPtr->getTupleSize();
        if (tupleSize < 2 || tupleSize > 4)
        {
            //geo->getAttributes().destroyAttribute(uvAttribPtr);
            uvAttribPtr = nullptr;
        }
    }
    return uvAttribPtr;
}



static GA_Attribute*
findOrCreateUVAttributePointVertex(
    GA_Detail* const geo,
    const GA_AttributeOwner uvAttribClass = GA_ATTRIB_INVALID,
    const UT_StringRef& uvAttribName = "uv",
    const GA_Storage storage = GA_STORE_INVALID
)
{
    GA_Attribute* uvAttribPtr = findAttributePointVertex(geo, uvAttribClass, uvAttribName);
    if (uvAttribPtr)
    {
        int tupleSize = uvAttribPtr->getTupleSize();
        if (tupleSize < 2 || tupleSize > 4)
        {
            geo->getAttributes().destroyAttribute(uvAttribPtr);
            uvAttribPtr = nullptr;
        }
    }
    if (!uvAttribPtr)
    {
        const GA_Storage finalStorageF = storage == GA_STORE_INVALID ? GFE_Type::getPreferredStorageF(geo) : storage;
#if 1
        uvAttribPtr = static_cast<GEO_Detail*>(geo)->addTextureAttribute(uvAttribClass == GA_ATTRIB_POINT ? GA_ATTRIB_POINT : GA_ATTRIB_VERTEX, finalStorageF);
#else
        uvAttribPtr = geo->getAttributes().createTupleAttribute(
            uvAttribClass == GA_ATTRIB_POINT ? GA_ATTRIB_POINT : GA_ATTRIB_VERTEX, uvAttribName, finalStorageF, 3, GA_Defaults(0));
#endif
    }
    return uvAttribPtr;
}



} // End of namespace GFE_Attribute

#endif
