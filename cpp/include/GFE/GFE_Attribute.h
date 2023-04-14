
#pragma once

#ifndef __GFE_Attribute_h__
#define __GFE_Attribute_h__

//#include "GFE/GFE_Attribute.h"

#include "GA/GA_Detail.h"


#include "GA/GA_AttributeFilter.h"



#include "GFE/GFE_Type.h"


enum class GFE_PieceAttribSearchOrder
{
    PRIM,
    POINT,
    VERTEX,
    PRIMPOINT,
    POINTPRIM,
    ALL,
    INVALID = -1,
};


enum class GFE_NormalSearchOrder
{
    PRIMITIVE,
    POINT,
    VERTEX,
    DETAIL,
    POINTVERTEX,
    ALL,
    GLOBAL = DETAIL,
    INVALID = -1,
};






namespace GFE_Attribute {


static GA_AttributeOwner
    toOwner(
        const GFE_NormalSearchOrder normalSearchOrder
    )
{
    switch (normalSearchOrder)
    {
    case GFE_NormalSearchOrder::PRIMITIVE:
        return GA_ATTRIB_PRIMITIVE;
        break;
    case GFE_NormalSearchOrder::POINT:
        return GA_ATTRIB_POINT;
        break;
    case GFE_NormalSearchOrder::VERTEX:
        return GA_ATTRIB_VERTEX;
        break;
    case GFE_NormalSearchOrder::DETAIL:
        return GA_ATTRIB_DETAIL;
        break;
    case GFE_NormalSearchOrder::POINTVERTEX:
        return GA_ATTRIB_VERTEX;
        break;
    case GFE_NormalSearchOrder::ALL:
        return GA_ATTRIB_OWNER_N;
        break;
    case GFE_NormalSearchOrder::INVALID:
        return GA_ATTRIB_INVALID;
        break;
    default:
        break;
    }
}

static GA_AttributeOwner
toValidOwner(
    const GFE_NormalSearchOrder normalSearchOrder
)
{
    switch (normalSearchOrder)
    {
    case GFE_NormalSearchOrder::PRIMITIVE:
        return GA_ATTRIB_PRIMITIVE;
        break;
    case GFE_NormalSearchOrder::POINT:
        return GA_ATTRIB_POINT;
        break;
    case GFE_NormalSearchOrder::VERTEX:
        return GA_ATTRIB_VERTEX;
        break;
    case GFE_NormalSearchOrder::DETAIL:
        return GA_ATTRIB_DETAIL;
        break;
    case GFE_NormalSearchOrder::POINTVERTEX:
        return GA_ATTRIB_VERTEX;
        break;
    case GFE_NormalSearchOrder::ALL:
        return GA_ATTRIB_VERTEX;
        break;
    case GFE_NormalSearchOrder::INVALID:
        return GA_ATTRIB_VERTEX;
        break;
    default:
        return GA_ATTRIB_VERTEX;
        break;
    }
}


static void
bumpDataId(
    GA_Detail& geo,
    const GA_AttributeOwner owner,
    const UT_StringHolder& attribPattern
)
{
    GA_AttributeFilter filter = GA_AttributeFilter::selectByPattern(attribPattern);
    UT_Array<GA_Attribute*> attribList;
    geo.getAttributes().matchAttributes(filter, owner, attribList);
    for (GA_Size i = 0; i < attribList.size(); ++i)
    {
        attribList[i]->bumpDataId();
    }
}


static GA_Attribute*
findPieceAttrib(
    GA_Detail& geo,
    const GFE_PieceAttribSearchOrder pieceAttribSearchOrder,
    const UT_StringHolder& pieceAttribName
)
{
    GA_Attribute* attribPtr = nullptr;

    switch (pieceAttribSearchOrder)
    {
    case GFE_PieceAttribSearchOrder::PRIM:       attribPtr = geo.findAttribute(GA_ATTRIB_PRIMITIVE, pieceAttribName); break;
    case GFE_PieceAttribSearchOrder::POINT:      attribPtr = geo.findAttribute(GA_ATTRIB_POINT,     pieceAttribName); break;
    case GFE_PieceAttribSearchOrder::VERTEX:     attribPtr = geo.findAttribute(GA_ATTRIB_VERTEX,    pieceAttribName); break;
    case GFE_PieceAttribSearchOrder::PRIMPOINT:
    {
        GA_AttributeOwner searchOrder[2] = { GA_ATTRIB_PRIMITIVE, GA_ATTRIB_POINT };
        attribPtr = geo.findAttribute(pieceAttribName, searchOrder, 2);
    }
        break;
    case GFE_PieceAttribSearchOrder::POINTPRIM:
    {
        GA_AttributeOwner searchOrder[2] = { GA_ATTRIB_POINT, GA_ATTRIB_PRIMITIVE };
        attribPtr = geo.findAttribute(pieceAttribName, searchOrder, 2);
    }
        break;
    case GFE_PieceAttribSearchOrder::ALL:
    {
        GA_AttributeOwner searchOrder[3] = { GA_ATTRIB_PRIMITIVE, GA_ATTRIB_POINT, GA_ATTRIB_VERTEX };
        attribPtr = geo.findAttribute(pieceAttribName, searchOrder, 3);
    }
        break;
    default:
        UT_ASSERT_MSG(0, "Unhandled Geo Piece Attrib Search Order!");
        break;
    }
    return attribPtr;
}

// SYS_FORCE_INLINE
// static bool
// renameAttribute(
//     GA_Detail& geo,
//     const GA_Attribute& attrib,
//     const UT_StringHolder& newName
// )
// {
//     return geo.renameAttribute(attrib.getOwner(), attrib.getScope(), attrib.getName(), newName);
// }
//
//
//
SYS_FORCE_INLINE
static bool
renameAttribute(
    GA_Attribute& attrib,
    const UT_StringHolder& newName
)
{
    return attrib.getDetail().renameAttribute(attrib.getOwner(), attrib.getScope(), attrib.getName(), newName);
}

    
static bool
forceRenameAttribute(
    GA_Detail& geo,
    GA_Attribute& attrib,
    const UT_StringHolder& newName
)
{
    GA_Attribute* const existAttribute = geo.findAttribute(attrib.getOwner(), newName);
    if (existAttribute)
        geo.getAttributes().destroyAttribute(existAttribute);
    return geo.renameAttribute(attrib.getOwner(), GA_SCOPE_PUBLIC, attrib.getName(), newName);
}


SYS_FORCE_INLINE
static bool
forceRenameAttribute(
    GA_Attribute& attrib,
    const UT_StringHolder& newName
)
{
    return forceRenameAttribute(attrib.getDetail(), attrib, newName);
}






//GFE_Attribute::findAttributePointVertex(geo, geo0AttribClass, geo0AttribNameSub, geo0AttribClassFinal)
//GFE_Attribute::findAttributePointVertex(geo, attribOwner, GA_SCOPE_PUBLIC, attribName, attribOwnerFianl);

static const GA_Attribute*
findAttributePointVertex(
    const GA_Detail& geo,
    const GA_AttributeOwner attribOwner,
    const GA_AttributeScope scope,
    const UT_StringRef& attribName,
    GA_AttributeOwner& attribOwnerFinal
)
{
    const GA_Attribute* attribPtr = nullptr;
    const GA_AttributeSet& geoAttribs = geo.getAttributes();
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
    const GA_Detail& geo,
    const GA_AttributeOwner attribOwner,
    const GA_AttributeScope scope,
    const UT_StringRef& attribName
)
{
    const GA_Attribute* attribPtr = nullptr;
    const GA_AttributeSet& geoAttribs = geo.getAttributes();
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
    GA_Detail& geo,
    const GA_AttributeOwner attribOwner,
    const GA_AttributeScope scope,
    const UT_StringRef& attribName,
    GA_AttributeOwner& attribOwnerFinal
)
{
    GA_Attribute* attribPtr = nullptr;
    GA_AttributeSet& geoAttribs = geo.getAttributes();
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
    GA_Detail& geo,
    const GA_AttributeOwner attribOwner,
    const GA_AttributeScope scope,
    const UT_StringRef& attribName
)
{
    GA_Attribute* attribPtr = nullptr;
    GA_AttributeSet& geoAttribs = geo.getAttributes();
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




//GFE_Attribute::findAttributePointVertex(geo, geo0AttribClass, geo0AttribNameSub, geo0AttribClassFinal)
//GFE_Attribute::findAttributePointVertex(geo, attribOwner, attribName, attribOwnerFianl);



SYS_FORCE_INLINE
static const GA_Attribute*
findAttributePointVertex(
    const GA_Detail& geo,
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
    const GA_Detail& geo,
    const GA_AttributeOwner attribOwner,
    const UT_StringRef& attribName
)
{
    return findAttributePointVertex(geo, attribOwner, GA_SCOPE_PUBLIC, attribName);
}



SYS_FORCE_INLINE
static GA_Attribute*
findAttributePointVertex(
    GA_Detail& geo,
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
    GA_Detail& geo,
    const GA_AttributeOwner attribOwner,
    const UT_StringRef& attribName
)
{
    return findAttributePointVertex(geo, attribOwner, GA_SCOPE_PUBLIC, attribName);
}




static GA_Attribute*
findUVAttributePointVertex(
    GA_Detail& geo,
    const GA_AttributeOwner uvAttribClass = GA_ATTRIB_INVALID,
    const UT_StringRef& uvAttribName = "uv"
)
{
    GA_Attribute* uvAttribPtr = findAttributePointVertex(geo, uvAttribClass, uvAttribName);
    if (uvAttribPtr)
    {
        const int tupleSize = uvAttribPtr->getTupleSize();
        if (tupleSize < 2 || tupleSize > 4)
        {
            //geo.getAttributes().destroyAttribute(uvAttribPtr);
            uvAttribPtr = nullptr;
        }
    }
    return uvAttribPtr;
}

static const GA_Attribute*
findUVAttributePointVertex(
    const GA_Detail& geo,
    const GA_AttributeOwner uvAttribClass = GA_ATTRIB_INVALID,
    const UT_StringRef& uvAttribName = "uv"
)
{
    const GA_Attribute* uvAttribPtr = findAttributePointVertex(geo, uvAttribClass, uvAttribName);
    if (uvAttribPtr)
    {
        const int tupleSize = uvAttribPtr->getTupleSize();
        if (tupleSize < 2 || tupleSize > 4)
        {
            //geo.getAttributes().destroyAttribute(uvAttribPtr);
            uvAttribPtr = nullptr;
        }
    }
    return uvAttribPtr;
}



static GA_Attribute*
findOrCreateUVAttributePointVertex(
    GA_Detail& geo,
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
            geo.getAttributes().destroyAttribute(uvAttribPtr);
            uvAttribPtr = nullptr;
        }
    }
    if (!uvAttribPtr)
    {
        const GA_Storage finalStorageF = storage == GA_STORE_INVALID ? GFE_Type::getPreferredStorageF(geo) : storage;
#if 1
        uvAttribPtr = static_cast<GEO_Detail&>(geo).addTextureAttribute(uvAttribClass == GA_ATTRIB_POINT ? GA_ATTRIB_POINT : GA_ATTRIB_VERTEX, finalStorageF);
#else
        uvAttribPtr = geo.getAttributes().createTupleAttribute(
            uvAttribClass == GA_ATTRIB_POINT ? GA_ATTRIB_POINT : GA_ATTRIB_VERTEX, uvAttribName, finalStorageF, 3, GA_Defaults(0));
#endif
    }
    return uvAttribPtr;
}





static const GA_Attribute*
findNormal3D(
    const GA_Detail& geo,
    const GFE_NormalSearchOrder normalSearchOrder = GFE_NormalSearchOrder::INVALID,
    const UT_StringHolder& normal3DAttribName = "N"
)
{
    const GA_Attribute* normal3DAttrib = nullptr;
    switch (normalSearchOrder)
    {
    case GFE_NormalSearchOrder::PRIMITIVE:
        normal3DAttrib = geo.findPrimitiveAttribute(normal3DAttribName);
        break;
    case GFE_NormalSearchOrder::POINT:
        normal3DAttrib = geo.findPointAttribute(normal3DAttribName);
        break;
    case GFE_NormalSearchOrder::VERTEX:
        normal3DAttrib = geo.findVertexAttribute(normal3DAttribName);
        break;
    case GFE_NormalSearchOrder::DETAIL:
        normal3DAttrib = geo.findGlobalAttribute(normal3DAttribName);
        break;
    case GFE_NormalSearchOrder::POINTVERTEX:
        normal3DAttrib = geo.findPointAttribute(normal3DAttribName);
        if (!normal3DAttrib)
        {
            normal3DAttrib = geo.findVertexAttribute(normal3DAttribName);
        }
        break;
    case GFE_NormalSearchOrder::ALL:
        normal3DAttrib = geo.findPrimitiveAttribute(normal3DAttribName);
        if (!normal3DAttrib)
        {
            normal3DAttrib = geo.findPointAttribute(normal3DAttribName);
            if (!normal3DAttrib)
            {
                normal3DAttrib = geo.findVertexAttribute(normal3DAttribName);
                if (!normal3DAttrib)
                {
                    normal3DAttrib = geo.findGlobalAttribute(normal3DAttribName);
                }
            }
        }
        break;
    default:
        UT_ASSERT_MSG(0, "unhandled GFE_NormalSearchOrder");
        break;
    }
    return normal3DAttrib;
}


static GA_Attribute*
findNormal3D(
    GA_Detail& geo,
    const GFE_NormalSearchOrder normalSearchOrder = GFE_NormalSearchOrder::INVALID,
    const UT_StringHolder& normal3DAttribName = "N"
)
{
    GA_Attribute* normal3DAttrib = nullptr;
    switch (normalSearchOrder)
    {
    case GFE_NormalSearchOrder::PRIMITIVE:
        normal3DAttrib = geo.findPrimitiveAttribute(normal3DAttribName);
        break;
    case GFE_NormalSearchOrder::POINT:
        normal3DAttrib = geo.findPointAttribute(normal3DAttribName);
        break;
    case GFE_NormalSearchOrder::VERTEX:
        normal3DAttrib = geo.findVertexAttribute(normal3DAttribName);
        break;
    case GFE_NormalSearchOrder::DETAIL:
        normal3DAttrib = geo.findGlobalAttribute(normal3DAttribName);
        break;
    case GFE_NormalSearchOrder::POINTVERTEX:
        normal3DAttrib = geo.findPointAttribute(normal3DAttribName);
        if (!normal3DAttrib)
        {
            normal3DAttrib = geo.findVertexAttribute(normal3DAttribName);
        }
        break;
    case GFE_NormalSearchOrder::ALL:
        normal3DAttrib = geo.findPrimitiveAttribute(normal3DAttribName);
        if (!normal3DAttrib)
        {
            normal3DAttrib = geo.findPointAttribute(normal3DAttribName);
            if (!normal3DAttrib)
            {
                normal3DAttrib = geo.findVertexAttribute(normal3DAttribName);
                if (!normal3DAttrib)
                {
                    normal3DAttrib = geo.findGlobalAttribute(normal3DAttribName);
                }
            }
        }
        break;
    default:
        UT_ASSERT_MSG(0, "unhandled GFE_NormalSearchOrder");
        break;
    }
    return normal3DAttrib;
}






} // End of namespace GFE_Attribute

#endif
