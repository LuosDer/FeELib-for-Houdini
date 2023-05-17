
#pragma once

#ifndef __GFE_AttributeArray_h__
#define __GFE_AttributeArray_h__

//#include "GFE/GFE_AttributeArray.h"

#include "GA/GA_Detail.h"
#include "GA/GA_Detail.h"


#include "GFE/GFE_Type.h"
#include "GFE/GFE_Attribute.h"


//for (std::vector<GA_Attribute*>::iterator it = attribArray.ref().begin(); it != attribArray.ref().end(); ++it) {
//    GA_Attribute* ptr = *it;
//    if (ptr->isDetached())
//        continue;
//}
//
//for (std::vector<GA_Group*>::iterator it = groupArray.ref().begin(); it != groupArray.ref().end(); ++it)
//{
//    GA_Group* ptr = *it;
//    if (!ptr->isDetached())
//        continue;
//}



class GFE_AttributeArray {

public:
    GFE_AttributeArray(
        GA_Detail* const geo,
        const SOP_NodeVerb::CookParms* const cookparms = nullptr
    )
        : geo(geo)
        , cookparms(cookparms)
    {
        attribUPtrArray.reserve(16);
        attribArray.reserve(16);
    }

    GFE_AttributeArray(
        GA_Detail* const geo,
        const SOP_NodeVerb::CookParms& cookparms
    )
        : geo(geo)
        , cookparms(&cookparms)
    {
        attribUPtrArray.reserve(16);
        attribArray.reserve(16);
    }

    GFE_AttributeArray(
        GA_Detail& geo,
        const SOP_NodeVerb::CookParms* const cookparms = nullptr
    )
        : geo(&geo)
        , cookparms(cookparms)
    {
        attribUPtrArray.reserve(16);
        attribArray.reserve(16);
    }

    GFE_AttributeArray(
        GA_Detail& geo,
        const SOP_NodeVerb::CookParms& cookparms
    )
        : geo(&geo)
        , cookparms(&cookparms)
    {
        attribUPtrArray.reserve(16);
        attribArray.reserve(16);
    }

    ~GFE_AttributeArray()
    {
    }



    
    
    SYS_FORCE_INLINE GA_Attribute* &operator[](const size_t i)
    { return attribArray[i]; }

    SYS_FORCE_INLINE GA_Attribute* operator[](const size_t i) const
    { return attribArray[i]; }

    SYS_FORCE_INLINE bool isEmpty() const
    { return attribArray.size() == 0; }
    
    SYS_FORCE_INLINE void erase(const size_t i)
    { attribArray.erase(attribArray.begin()+i); }

    SYS_FORCE_INLINE void destroy(const size_t i)
    {
        geo->getAttributes().destroyAttribute(attribArray[i]);
        erase(i);
    }

    SYS_FORCE_INLINE void clear()
    {
        attribArray.clear();
        attribUPtrArray.clear();
    }

    SYS_FORCE_INLINE ::std::vector<GA_Attribute*>::size_type size() const
    { return attribArray.size(); }

    void reset(GA_Detail* const inGeo, const SOP_NodeVerb::CookParms* const cookparms = nullptr)
    {
        geo = inGeo;
        this->cookparms = cookparms;
        clear();
    }

    void reset(GA_Detail& inGeo, const SOP_NodeVerb::CookParms* const cookparms = nullptr)
    {
        geo = &inGeo;
        this->cookparms = cookparms;
        clear();
    }

    
    void set(GA_Attribute* const attribPtr)
    {
        if (!attribPtr)
            return;
        clear();
        attribArray.emplace_back(attribPtr);
    }

    SYS_FORCE_INLINE void set(GA_Attribute& attribPtr)
    {
        clear();
        attribArray.emplace_back(&attribPtr);
    }

    GA_Attribute* set(const GA_AttributeOwner attribClass, const UT_StringRef& attribPattern)
    {
        if (!attribPattern.isstring() || attribPattern.length() == 0)
            return nullptr;

        GA_Attribute* attribPtr = geo->findAttribute(attribClass, attribPattern);
        set(attribPtr);
        return attribPtr;
    }


    SYS_FORCE_INLINE void append(GA_Attribute* const attribPtr)
    {
        if (!attribPtr)
            return;
        attribArray.emplace_back(attribPtr);
    }

    SYS_FORCE_INLINE void append(GA_Attribute& attribPtr)
    {
        attribArray.emplace_back(&attribPtr);
    }

    GA_Attribute* append(const GA_AttributeOwner attribClass,const UT_StringRef& attribPattern)
    {
        if (!attribPattern.isstring() || attribPattern.length() == 0)
            return nullptr;

        GA_Attribute* const attribPtr = geo->findAttribute(attribClass, attribPattern);
        append(attribPtr);
        return attribPtr;
    }

    GA_Attribute* appendUV(const UT_StringRef& attribPattern, const GA_AttributeOwner attribClass = GA_ATTRIB_INVALID)
    {
        if (!attribPattern.isstring() || attribPattern.length() == 0)
            return nullptr;

        GA_Attribute* const attribPtr = GFE_Attribute::findUVAttributePointVertex(*geo, attribClass, attribPattern);

        append(attribPtr);
        return attribPtr;
    }




void appends(const GA_AttributeOwner attribClass, const char* attribPattern)
{
    if (!attribPattern)
        return;

    for (GA_AttributeDict::iterator it = geo->getAttributes().begin(attribClass); !it.atEnd(); ++it)
    {
        GA_Attribute* const attribPtr = it.attrib();
        if (attribPtr->getScope() != GA_SCOPE_PUBLIC)
            continue;
        if (!attribPtr->getName().multiMatch(attribPattern))
            continue;
        attribArray.emplace_back(attribPtr);
    }
}
    
SYS_FORCE_INLINE void appends(const GA_AttributeOwner attribClass, const UT_StringRef& attribPattern)
{
    if (!attribPattern.isstring() || attribPattern.length() == 0)
        return;
    appends(attribClass, attribPattern.c_str());
}

SYS_FORCE_INLINE void appendPrimitives(const char* attribPattern)
{ appends(GA_ATTRIB_PRIMITIVE, attribPattern); }

SYS_FORCE_INLINE void appendPoints(const char* attribPattern)
{ appends(GA_ATTRIB_POINT, attribPattern); }

SYS_FORCE_INLINE void appendVertices(const char* attribPattern)
{ appends(GA_ATTRIB_VERTEX, attribPattern); }

SYS_FORCE_INLINE void appendDetails(const char* attribPattern)
{ appends(GA_ATTRIB_DETAIL, attribPattern); }



void oappends(const GA_AttributeOwner attribClass, const char* attribPattern)
{
    if (!attribPattern)
        return;

    for (GA_AttributeDict::ordered_iterator it = geo->getAttributes().getDict(attribClass).obegin(GA_SCOPE_PUBLIC); !it.atEnd(); ++it)
    {
        GA_Attribute* const attribPtr = *it;
        if (!attribPtr->getName().multiMatch(attribPattern))
            continue;
        attribArray.emplace_back(attribPtr);
    }
}

SYS_FORCE_INLINE void oappends(const GA_AttributeOwner attribClass, const UT_StringRef& attribPattern)
{
    if (!attribPattern.isstring() || attribPattern.length() == 0)
        return;
    oappends(attribClass, attribPattern.c_str());
}
    
SYS_FORCE_INLINE void oappendPrims(const char* attribPattern)
{ return oappends(GA_ATTRIB_PRIMITIVE, attribPattern); }

SYS_FORCE_INLINE void oappendPoints(const char* attribPattern)
{ return oappends(GA_ATTRIB_POINT, attribPattern); }
    
SYS_FORCE_INLINE void oappendVertices(const char* attribPattern)
{ return oappends(GA_ATTRIB_VERTEX, attribPattern); }
    
SYS_FORCE_INLINE void oappendDetails(const char* attribPattern)
{ return oappends(GA_ATTRIB_DETAIL, attribPattern); }


    
SYS_FORCE_INLINE void appendPointVertexs(const UT_StringRef& attribPattern)
{
    appends(GA_ATTRIB_POINT, attribPattern);
    appends(GA_ATTRIB_VERTEX, attribPattern);
}


GA_Attribute* createDetachedAttribute(
    const GA_AttributeOwner owner = GA_ATTRIB_POINT,
    // const GA_StorageClass storageClass = GA_STORECLASS_FLOAT,
    // const GA_Storage storage = GA_STORE_INVALID,
    const bool emplaceBack = true,
    const UT_StringRef& attribtype = "numeric",
    const UT_Options* const create_args = nullptr,
    const GA_AttributeOptions* const attribute_options = nullptr
)
{
    // const GA_Storage finalStorage = GFE_Type::getPreferredStorage(geo, storageClass, storage);

    attribUPtrArray.emplace_back(geo->createDetachedAttribute(owner, attribtype, create_args, attribute_options));
    GA_Attribute* attribPtr = attribUPtrArray[attribUPtrArray.size() - 1].get();

    if (emplaceBack)
        attribArray.emplace_back(attribPtr);
    return attribPtr;
}


GA_Attribute* createDetachedAttribute(
    const GA_AttributeOwner owner,
    //const GA_StorageClass storageClass,
    //const GA_Storage storage,
    const bool emplaceBack,
    const GA_AttributeType& attribtype,
    const UT_Options* const create_args = nullptr,
    const GA_AttributeOptions* const attribute_options = nullptr
)
{
    //const GA_Storage finalStorage = GFE_Type::getPreferredStorage(geo, storageClass, storage);

    attribUPtrArray.emplace_back(geo->createDetachedAttribute(owner, attribtype, create_args, attribute_options));
    GA_Attribute* attribPtr = attribUPtrArray[attribUPtrArray.size() - 1].get();

    if (emplaceBack)
        attribArray.emplace_back(attribPtr);
    return attribPtr;
}


GA_Attribute* createDetachedTupleAttribute(
    const GA_AttributeOwner owner = GA_ATTRIB_POINT,
    const GA_StorageClass storageClass = GA_STORECLASS_FLOAT,
    const GA_Storage storage = GA_STORE_INVALID,
    const int tuple_size = 1,
    const GA_Defaults& defaults = GA_Defaults(0.0f),
    const bool emplaceBack = true,
    const GA_AttributeOptions* const attribute_options = nullptr
)
{
    const GA_Storage finalStorage = GFE_Type::getPreferredStorage(geo, storageClass, storage);
    
    attribUPtrArray.emplace_back(geo->createDetachedTupleAttribute(owner, finalStorage, tuple_size, defaults, attribute_options));
    GA_Attribute* attribPtr = attribUPtrArray[attribUPtrArray.size() - 1].get();

    if (emplaceBack)
        attribArray.emplace_back(attribPtr);
    return attribPtr;
}


GA_Attribute* createDetachedStringAttribute(
    const GA_AttributeOwner owner = GA_ATTRIB_POINT,
    const bool emplaceBack = true,
    const GA_AttributeOptions* const attribute_options = nullptr
)
{
    attribUPtrArray.emplace_back(geo->createDetachedTupleAttribute(owner, GA_STORE_STRING, 1, GA_Defaults(0), attribute_options));
    GA_Attribute* attribPtr = attribUPtrArray[attribUPtrArray.size() - 1].get();

    if (emplaceBack)
        attribArray.emplace_back(attribPtr);
    return attribPtr;
}

        
GA_Attribute* createDetachedArrayAttribute(
    const GA_AttributeOwner owner = GA_ATTRIB_POINT,
    // const GA_StorageClass storageClass = GA_STORECLASS_FLOAT,
    // const GA_Storage storage = GA_STORE_INVALID,
    // const int tuple_size = 1,
    const bool emplaceBack = true,
    const UT_Options* const create_args = nullptr,
    const GA_AttributeOptions* const attribute_options = nullptr
)
{
    //const GA_Storage finalStorage = GFE_Type::getPreferredStorage(geo, storageClass, storage);

    attribUPtrArray.emplace_back(geo->createDetachedAttribute(owner, "arraydata", create_args, attribute_options));
    GA_Attribute* attribPtr = attribUPtrArray[attribUPtrArray.size() - 1].get();

    if (emplaceBack)
        attribArray.emplace_back(attribPtr);
    return attribPtr;
}




GA_Attribute*
findOrCreateTuple(
    const bool detached = false,
    const GA_AttributeOwner owner = GA_ATTRIB_POINT,
    const GA_StorageClass storageClass = GA_STORECLASS_FLOAT,
    const GA_Storage storage = GA_STORE_INVALID,
    const UT_StringRef& attribName = "",
    const int tuple_size = 1,
    const GA_Defaults& defaults = GA_Defaults(0.0f),
    const bool emplaceBack = true,
    const UT_Options* const create_args = nullptr,
    const GA_AttributeOptions* const attribute_options = nullptr
)
{
    const GA_Storage finalStorage = GFE_Type::getPreferredStorage(geo, storageClass, storage);
    
    GA_Attribute* attribPtr = geo->findAttribute(owner, attribName);
    checkTupleAttrib(attribPtr, finalStorage, tuple_size, defaults, emplaceBack);

    if (detached)
    {
        if(finalStorage == GA_STORE_STRING)
            attribUPtrArray.emplace_back(geo->createDetachedAttribute(owner, "string", create_args, attribute_options));
        else
            attribUPtrArray.emplace_back(geo->createDetachedTupleAttribute(owner, finalStorage, 1));
        attribPtr = attribUPtrArray[attribUPtrArray.size() - 1].get();
        //attribPtr = attribUPtr.get();
    }
    else
    {
        if (!attribPtr)
            if(finalStorage == GA_STORE_STRING)
                attribPtr = geo->createStringAttribute(owner, attribName, create_args, attribute_options);
            else
                attribPtr = geo->createTupleAttribute(owner, attribName, finalStorage,
                            tuple_size, defaults, create_args, attribute_options);

        if (!attribPtr)
        {
            if (cookparms)
                cookparms->sopAddError(SOP_ATTRIBUTE_INVALID, attribName);
            UT_ASSERT_MSG(attribPtr, "No Attrib");
            return nullptr;
        }
    }
    if (emplaceBack)
        attribArray.emplace_back(attribPtr);
    return attribPtr;
}

// SYS_FORCE_INLINE GA_Attribute* findOrCreateTuple(
//     const bool detached = false,
//     const GA_AttributeOwner owner = GA_ATTRIB_POINT,
//     const GA_Precision precision = GA_PRECISION_INVALID,
//     const GA_StorageClass storageClass = GA_STORECLASS_FLOAT,
//     const UT_StringRef& attribName = "",
//     const int tuple_size = 1,
//     const GA_Defaults& defaults = GA_Defaults(0.0f),
//     const bool emplaceBack = true,
//     const UT_Options* const create_args = nullptr,
//     const GA_AttributeOptions* const attribute_options = nullptr
// )
// {
//     const GA_Storage storage = GFE_Type::getPreferredStorage(precision, storageClass);
//     return findOrCreateTuple(detached, owner, storageClass, storage, attribName, tuple_size, defaults, emplaceBack, create_args, attribute_options);
// }


    GA_Attribute*
    findOrCreateArray(
        const bool detached = false,
        const GA_AttributeOwner owner = GA_ATTRIB_POINT,
        const GA_StorageClass storageClass = GA_STORECLASS_FLOAT,
        const GA_Storage storage = GA_STORE_INVALID,
        const UT_StringRef& attribName = "",
        const int tuple_size = 1,
        const bool emplaceBack = true,
        const UT_Options* const create_args = nullptr,
        const GA_AttributeOptions* const attribute_options = nullptr
    )
    {
        const GA_Storage finalStorage = GFE_Type::getPreferredStorage(geo, storageClass, storage);

        GA_Attribute* attribPtr = geo->findAttribute(owner, attribName);
        checkArrayAttrib(attribPtr, finalStorage, tuple_size, emplaceBack);
        

        if (detached)
        {
#if 0
            switch (finalStorage)
            {
            case GA_STORE_INT16:
                break;
            case GA_STORE_INT32:
                break;
            case GA_STORE_INT64:
                break;
            case GA_STORE_REAL16:
                break;
            case GA_STORE_REAL32:
                break;
            case GA_STORE_REAL64:
                break;
            case GA_STORE_STRING:
                break;
            case GA_STORE_DICT:
                break;
            default:
                break;
            }
            attribUPtrArray.emplace_back(static_cast<GEO_Detail*>(geo)->createDetachedAttribute(owner, "arraydata", tuple_size, create_args, attribute_options));
#else
            attribUPtrArray.emplace_back(geo->getAttributes().createDetachedAttribute(owner, "arraydata", create_args, attribute_options));
#endif
            attribPtr = attribUPtrArray[attribUPtrArray.size() - 1].get();
            //attribPtr = attribUPtr.get();
        }
        else
        {
            if (!attribPtr)
                attribPtr = geo->getAttributes().createArrayAttribute(owner, GA_SCOPE_PUBLIC, attribName, finalStorage,
                    tuple_size, create_args, attribute_options);

            if (!attribPtr)
            {
                if (cookparms)
                    cookparms->sopAddError(SOP_ATTRIBUTE_INVALID, attribName);
                UT_ASSERT_MSG(attribPtr, "No Attrib");
                return nullptr;
            }
        }
        if (emplaceBack)
            attribArray.emplace_back(attribPtr);
        return attribPtr;
    }





    
//SYS_FORCE_INLINE
//void
//findOrCreate(
//    const GA_AttributeOwner owner,
//    const GA_Storage storage,
//    const UT_StringRef& attribName
//)
//{
//    findOrCreate(owner, storage, attribName);
//}





    GA_Attribute*
    findOrCreatePiece(
        const bool detached = false,
        const GFE_PieceAttribSearchOrder pieceAttribSearchOrder = GFE_PieceAttribSearchOrder::PRIM,
        const GA_AttributeOwner owner = GA_ATTRIB_PRIMITIVE,
        const GA_StorageClass storageClass = GA_STORECLASS_INT,
        const GA_Storage storage = GA_STORE_INVALID,
        const UT_StringRef& attribName = "",
        const int tuple_size = 1,
        const GA_Defaults& defaults = GA_Defaults(0.0f),
        const bool emplaceBack = true,
        const UT_Options* const create_args = nullptr,
        const GA_AttributeOptions* const attribute_options = nullptr
    )
    {
        const GA_Storage finalStorage = GFE_Type::getPreferredStorage(geo, storageClass, storage);

        GA_Attribute* attribPtr = findPieceAttrib(pieceAttribSearchOrder, attribName);
        checkTupleAttrib(attribPtr, finalStorage, tuple_size, defaults, emplaceBack);

        // if (attribPtr)
        // {
        //     const bool promoteFromOtherClass = sopparms.getPromoteFromOtherClass();
        //     if (promoteFromOtherClass)
        //     {
        //         if (geo0AttribClass != attribPtr->getOwner())
        //         {
        //             attribPtr = GFE_Attribpr::attribPromote(outGeo0, attribPtr, geo0AttribClass);
        //             //attribPtr = GFE_AttribPromote::promote(*static_cast<GU_Detail*>(outGeo0), attribPtr, geo0AttribClass, sopparms.getDelOriginalAttrib(), GU_Promote::GU_PROMOTE_FIRST);
        //         }
        //     }
        //
        //     const bool forceCastAttribType = sopparms.getForceCastAttribType();
        //     if (forceCastAttribType)
        //     {
        //         GFE_AttributeCast::attribCast(outGeo0, attribPtr, connectivityStorageClass, "", outGeo0->getPreferredPrecision());
        //     }
        //     return;
        // }
        

        if (detached)
        {
            attribUPtrArray.emplace_back(geo->createDetachedTupleAttribute(owner, finalStorage, 1));
            attribPtr = attribUPtrArray[attribUPtrArray.size() - 1].get();
            //attribPtr = attribUPtr.get();
        }
        else
        {
            if (!attribPtr)
                attribPtr = geo->createTupleAttribute(owner, attribName, finalStorage,
                    tuple_size, defaults, create_args, attribute_options);

            if (!attribPtr)
            {
                if (cookparms)
                    cookparms->sopAddError(SOP_ATTRIBUTE_INVALID, attribName);
                UT_ASSERT_MSG(attribPtr, "No Attrib");
                return nullptr;
            }
        }
        if (emplaceBack)
            attribArray.emplace_back(attribPtr);
        return attribPtr;
    }


    GA_Attribute*
    findPieceAttrib(
        const GFE_PieceAttribSearchOrder pieceAttribSearchOrder,
        const UT_StringRef& pieceAttribName
    )
    {
        GA_Attribute* attribPtr = nullptr;

        switch (pieceAttribSearchOrder)
        {
        case GFE_PieceAttribSearchOrder::PRIM:       attribPtr = geo->findAttribute(GA_ATTRIB_PRIMITIVE, pieceAttribName); break;
        case GFE_PieceAttribSearchOrder::POINT:      attribPtr = geo->findAttribute(GA_ATTRIB_POINT,     pieceAttribName); break;
        case GFE_PieceAttribSearchOrder::VERTEX:     attribPtr = geo->findAttribute(GA_ATTRIB_VERTEX,    pieceAttribName); break;
        case GFE_PieceAttribSearchOrder::PRIMPOINT:
            {
                GA_AttributeOwner searchOrder[2] = { GA_ATTRIB_PRIMITIVE, GA_ATTRIB_POINT };
                attribPtr = geo->findAttribute(pieceAttribName, searchOrder, 2);
            }
            break;
        case GFE_PieceAttribSearchOrder::POINTPRIM:
            {
                GA_AttributeOwner searchOrder[2] = { GA_ATTRIB_POINT, GA_ATTRIB_PRIMITIVE };
                attribPtr = geo->findAttribute(pieceAttribName, searchOrder, 2);
            }
            break;
        case GFE_PieceAttribSearchOrder::ALL:
            {
                GA_AttributeOwner searchOrder[3] = { GA_ATTRIB_PRIMITIVE, GA_ATTRIB_POINT, GA_ATTRIB_VERTEX };
                attribPtr = geo->findAttribute(pieceAttribName, searchOrder, 3);
            }
            break;
        default:
            UT_ASSERT_MSG(0, "Unhandled Geo Piece Attrib Search Order!");
            break;
        }
        return attribPtr;
    }





    
GA_Attribute*
findOrCreateUV(
    const bool detached = false,
    const GA_AttributeOwner owner = GA_ATTRIB_POINT,
    const GA_Storage storage = GA_STORE_INVALID,
    const UT_StringRef& attribName = "",
    const int tuple_size = 3,
    const GA_Defaults& defaults = GA_Defaults(0.0f),
    const bool emplaceBack = true,
    const UT_Options* const create_args = nullptr,
    const GA_AttributeOptions* const attribute_options = nullptr,
    const GA_StorageClass storageClass = GA_STORECLASS_FLOAT
)
{
    const GA_Storage finalStorage = GFE_Type::getPreferredStorage(geo, storageClass, storage);

    GA_Attribute* attribPtr = GFE_Attribute::findAttributePointVertex(*geo, owner, attribName);
    checkTupleAttrib(attribPtr, finalStorage, tuple_size, defaults, emplaceBack);
        
    const GA_AttributeOwner validOwner = owner == GA_ATTRIB_POINT ? GA_ATTRIB_POINT : GA_ATTRIB_VERTEX;
    if (detached)
    {
        attribUPtrArray.emplace_back(geo->createDetachedTupleAttribute(validOwner, finalStorage, 1));
        attribPtr = attribUPtrArray[attribUPtrArray.size() - 1].get();
        //attribPtr = attribUPtr.get();
    }
    else
    {
        if (!attribPtr)
        {
#if 1
            const UT_Options& finalCreateArgs = create_args ? *create_args : UT_Options("uvw");

            //GA_AttributeOptions& finalOptions = attribute_options ? *attribute_options : GA_AttributeOptions();
            if (attribute_options)
            {
                attribPtr = geo->createTupleAttribute(validOwner, attribName, finalStorage,
                    tuple_size, defaults, &finalCreateArgs, attribute_options);
            }
            else
            {
                GA_AttributeOptions finalOptions = GA_AttributeOptions();
                finalOptions.setTypeInfo(GA_TYPE_TEXTURE_COORD);
                attribPtr = geo->createTupleAttribute(validOwner, attribName, finalStorage,
                    tuple_size, defaults, &finalCreateArgs, &finalOptions);
            }
#else
            attribPtr = static_cast<GEO_Detail*>(geo)->addTextureAttribute(owner == GA_ATTRIB_POINT ? GA_ATTRIB_POINT : GA_ATTRIB_VERTEX, finalStorage);
#endif
        }

        if (!attribPtr)
        {
            if (cookparms)
                cookparms->sopAddError(SOP_ATTRIBUTE_INVALID, attribName);
            UT_ASSERT_MSG(attribPtr, "No Attrib");
            return nullptr;
        }
        GFE_Attribute::renameAttribute(*attribPtr, attribName);
    }
    if (emplaceBack)
        attribArray.emplace_back(attribPtr);
    return attribPtr;
}

GA_Attribute*
findOrCreateDir(
    const bool detached = false,
    const GA_AttributeOwner owner = GA_ATTRIB_POINT,
    const GA_Storage storage = GA_STORE_INVALID,
    const UT_StringRef& attribName = "",
    const int tuple_size = 3,
    const GA_Defaults& defaults = GA_Defaults(0.0f),
    const bool emplaceBack = true,
    const UT_Options* const create_args = nullptr,
    const GA_AttributeOptions* const attribute_options = nullptr,
    const GA_StorageClass storageClass = GA_STORECLASS_FLOAT
)
{
    const GA_Storage finalStorage = GFE_Type::getPreferredStorage(geo, storageClass, storage);

    GA_Attribute* attribPtr = geo->findAttribute(owner, attribName);
    checkTupleAttrib(attribPtr, finalStorage, tuple_size, defaults, emplaceBack);
        
    if (detached)
    {
        attribUPtrArray.emplace_back(geo->createDetachedTupleAttribute(owner, finalStorage, 1));
        attribPtr = attribUPtrArray[attribUPtrArray.size() - 1].get();
        //attribPtr = attribUPtr.get();
    }
    else
    {
        if (!attribPtr)
        {
#if 1
            const UT_Options& finalCreateArgs = create_args ? *create_args : UT_Options("vector3");

            //GA_AttributeOptions& finalOptions = attribute_options ? *attribute_options : GA_AttributeOptions();
            if (attribute_options)
            {
                attribPtr = geo->createTupleAttribute(owner, attribName, finalStorage,
                    tuple_size, defaults, &finalCreateArgs, attribute_options);
            }
            else
            {
                GA_AttributeOptions finalOptions = GA_AttributeOptions();
                finalOptions.setTypeInfo(GA_TYPE_NORMAL);
                attribPtr = geo->createTupleAttribute(owner, attribName, finalStorage,
                    tuple_size, defaults, &finalCreateArgs, &finalOptions);
            }
#else
            attribPtr = static_cast<GEO_Detail*>(geo)->addTextureAttribute(owner == GA_ATTRIB_POINT ? GA_ATTRIB_POINT : GA_ATTRIB_VERTEX, finalStorage);
#endif
        }

        if (!attribPtr)
        {
            if (cookparms)
                cookparms->sopAddError(SOP_ATTRIBUTE_INVALID, attribName);
            UT_ASSERT_MSG(attribPtr, "No Attrib");
            return nullptr;
        }
        GFE_Attribute::renameAttribute(*attribPtr, attribName);
    }
    if (emplaceBack)
        attribArray.emplace_back(attribPtr);
    return attribPtr;
}


GA_Attribute*
findOrCreateNormal3D(
    const bool detached = false,
    const GFE_NormalSearchOrder owner = GFE_NormalSearchOrder::ALL,
    const GA_Storage storage = GA_STORE_INVALID,
    const UT_StringRef& attribName = "",
    const int tuple_size = 3,
    const GA_Defaults& defaults = GA_Defaults(0.0f),
    const bool emplaceBack = true,
    const UT_Options* const create_args = nullptr,
    const GA_AttributeOptions* const attribute_options = nullptr,
    const GA_StorageClass storageClass = GA_STORECLASS_FLOAT
)
{
    const GA_Storage finalStorage = GFE_Type::getPreferredStorage(geo, storageClass, storage);


    GA_Attribute* attribPtr = GFE_Attribute::findNormal3D(*geo, owner, attribName);
    checkTupleAttrib(attribPtr, finalStorage, tuple_size, defaults, emplaceBack);
        
    const GA_AttributeOwner validOwner = GFE_Attribute::toValidOwner(owner);
    if (detached)
    {
        attribUPtrArray.emplace_back(geo->createDetachedTupleAttribute(validOwner, finalStorage, 1));
        attribPtr = attribUPtrArray[attribUPtrArray.size() - 1].get();
        //attribPtr = attribUPtr.get();
    }
    else
    {
        if (!attribPtr)
        {
#if 1
            const UT_Options& finalCreateArgs = create_args ? *create_args : UT_Options("vector3");

            //GA_AttributeOptions& finalOptions = attribute_options ? *attribute_options : GA_AttributeOptions();
            if (attribute_options)
            {
                attribPtr = geo->createTupleAttribute(validOwner, attribName, finalStorage,
                    tuple_size, defaults, &finalCreateArgs, attribute_options);
            }
            else
            {
                GA_AttributeOptions finalOptions = GA_AttributeOptions();
                finalOptions.setTypeInfo(GA_TYPE_NORMAL);
                attribPtr = geo->createTupleAttribute(validOwner, attribName, finalStorage,
                    tuple_size, defaults, &finalCreateArgs, &finalOptions);
            }
#else
            attribPtr = static_cast<GEO_Detail*>(geo)->addTextureAttribute(owner == GA_ATTRIB_POINT ? GA_ATTRIB_POINT : GA_ATTRIB_VERTEX, finalStorage);
#endif
        }

        if (!attribPtr)
        {
            if (cookparms)
                cookparms->sopAddError(SOP_ATTRIBUTE_INVALID, attribName);
            UT_ASSERT_MSG(attribPtr, "No Attrib");
            return nullptr;
        }
        GFE_Attribute::renameAttribute(*attribPtr, attribName);
    }
    if (emplaceBack)
        attribArray.emplace_back(attribPtr);
    return attribPtr;
}





SYS_FORCE_INLINE void pop_back()
{ attribArray.pop_back(); }

SYS_FORCE_INLINE GA_Attribute* last() const
{ return attribArray[attribArray.size()-1]; }


void bumpDataId() const
{
    const size_t len = attribArray.size();
    for (size_t i = 0; i < len; i++)
    {
        GA_Attribute* const ptr = attribArray[i];
        if (ptr->isDetached())
            continue;
        ptr->bumpDataId();
    }
}

SYS_FORCE_INLINE std::vector<GA_Attribute*>& ref()
{ return attribArray; }

SYS_FORCE_INLINE const std::vector<GA_Attribute*>& ref() const
{ return attribArray; }








    
protected:
    SYS_FORCE_INLINE void setDetail(GA_Detail* const inGeo)
    { geo = inGeo; }

    SYS_FORCE_INLINE void setDetail(GA_Detail& inGeo)
    { geo = &inGeo; }


void checkTupleAttrib(
    GA_Attribute* &attribPtr,
    const GA_Storage finalStorage = GA_STORE_INVALID,
    const int tuple_size = 3,
    const GA_Defaults& defaults = GA_Defaults(0.0f),
    const bool emplaceBack = true
)
{
    if (!attribPtr)
        return;

    //int a = attribPtr->getTupleSize();
    if (attribPtr->getTupleSize() != tuple_size)
    {
        geo->getAttributes().destroyAttribute(attribPtr);
        attribPtr = nullptr;
        return;
    }
    const GA_AIFTuple* const aifTuple = attribPtr->getAIFTuple();
    if (aifTuple)
    {
        if (aifTuple->getStorage(attribPtr) != finalStorage ||
            aifTuple->getDefaults(attribPtr) != defaults)
        {
            geo->getAttributes().destroyAttribute(attribPtr);
            attribPtr = nullptr;
            return;
        }
    }
    else
    {
        if (finalStorage != GA_STORE_STRING)
        {
            geo->getAttributes().destroyAttribute(attribPtr);
            attribPtr = nullptr;
            return;
        }
        //const GA_AIFStringTuple* const aifStrTuple = attribPtr->getAIFStringTuple();
    }
    if (emplaceBack)
        attribArray.emplace_back(attribPtr);
}

void checkArrayAttrib(
    GA_Attribute*& attribPtr,
    const GA_Storage finalStorage = GA_STORE_INVALID,
    const int tuple_size = 3,
    const bool emplaceBack = true
)
{
    if (!attribPtr)
        return;
    
    if (attribPtr->getTupleSize() != tuple_size)
    {
        geo->getAttributes().destroyAttribute(attribPtr);
        attribPtr = nullptr;
        return;
    }
    const GA_AIFNumericArray* const aifNumericArray = attribPtr->getAIFNumericArray();
    if (aifNumericArray)
    {
        if (aifNumericArray->getStorage(attribPtr) != finalStorage)
        {
            geo->getAttributes().destroyAttribute(attribPtr);
            attribPtr = nullptr;
            return;
        }
    }
    else
    {
        if (finalStorage != GA_STORE_STRING)
        {
            geo->getAttributes().destroyAttribute(attribPtr);
            attribPtr = nullptr;
            return;
        }
        //const GA_AIFSharedStringArray* const aifStrArray = attribPtr->getAIFSharedStringArray();
    }
    if (emplaceBack)
        attribArray.emplace_back(attribPtr);
}



    
    
protected:
    GA_Detail* geo;
    const SOP_NodeVerb::CookParms* cookparms;

private:
    ::std::vector<GA_AttributeUPtr> attribUPtrArray;
    ::std::vector<GA_Attribute*> attribArray;

    friend class GFE_AttribFilter;
    friend class GFE_AttribCreateFilter;

}; // End of class GFE_AttributeArray











class GFE_GroupArray {

public:
    GFE_GroupArray(
        GA_Detail* const geo,
        const SOP_NodeVerb::CookParms* const cookparms = nullptr
    )
        : geo(geo)
        , cookparms(cookparms)
    {
        groupUPtrArray.reserve(16);
        groupArray.reserve(16);
    }

    GFE_GroupArray(
        GA_Detail* const geo,
        const SOP_NodeVerb::CookParms& cookparms
    )
        : geo(geo)
        , cookparms(&cookparms)
    {
        groupUPtrArray.reserve(16);
        groupArray.reserve(16);
    }

    GFE_GroupArray(
        GA_Detail& geo,
        const SOP_NodeVerb::CookParms* const cookparms = nullptr
    )
        : geo(&geo)
        , cookparms(cookparms)
    {
        groupUPtrArray.reserve(16);
        groupArray.reserve(16);
    }

    GFE_GroupArray(
        GA_Detail& geo,
        const SOP_NodeVerb::CookParms& cookparms
    )
        : geo(&geo)
        , cookparms(&cookparms)
    {
        groupUPtrArray.reserve(16);
        groupArray.reserve(16);
    }
    
    ~GFE_GroupArray()
    {
    }




    
    
    SYS_FORCE_INLINE GA_ElementGroup* getElementGroup(const size_t i) const
    {
        UT_ASSERT(groupArray[i]->isElementGroup());
        return static_cast<GA_ElementGroup*>(groupArray[i]);
    }

    SYS_FORCE_INLINE GA_PrimitiveGroup* getPrimitiveGroup(const size_t i) const
    {
        UT_ASSERT(groupArray[i]->classType() == GA_GROUP_PRIMITIVE);
        return static_cast<GA_PrimitiveGroup*>(groupArray[i]);
    }

    SYS_FORCE_INLINE GA_PointGroup* getPointGroup(const size_t i) const
    {
        UT_ASSERT(groupArray[i]->classType() == GA_GROUP_POINT);
        return static_cast<GA_PointGroup*>(groupArray[i]);
    }

    SYS_FORCE_INLINE GA_VertexGroup* getVertexGroup(const size_t i) const
    {
        UT_ASSERT(groupArray[i]->classType() == GA_GROUP_VERTEX);
        return static_cast<GA_VertexGroup*>(groupArray[i]);
    }

    SYS_FORCE_INLINE GA_EdgeGroup* getEdgeGroup(const size_t i) const
    {
        UT_ASSERT(groupArray[i]->classType() == GA_GROUP_EDGE);
        return static_cast<GA_EdgeGroup*>(groupArray[i]);
    }


    SYS_FORCE_INLINE GA_Group* &operator[](const size_t i)
    { return groupArray[i]; }

    SYS_FORCE_INLINE bool isEmpty() const
    { return groupArray.size() == 0; }

    SYS_FORCE_INLINE void erase(const size_t i)
    { groupArray.erase(groupArray.begin()+i); }
    
    SYS_FORCE_INLINE void clear()
    {
        groupArray.clear();
        groupUPtrArray.clear();
    }

    SYS_FORCE_INLINE ::std::vector<GA_Group*>::size_type size() const
    { return groupArray.size(); }

    SYS_FORCE_INLINE GA_Group* last() const
    { return groupArray[groupArray.size() - 1]; }

    SYS_FORCE_INLINE void reset(GA_Detail* const inGeo, const SOP_NodeVerb::CookParms* const cookparms = nullptr)
    {
        geo = inGeo;
        this->cookparms = cookparms;
        clear();
    }

    SYS_FORCE_INLINE void reset(GA_Detail& inGeo, const SOP_NodeVerb::CookParms* const cookparms = nullptr)
    {
        geo = &inGeo;
        this->cookparms = cookparms;
        clear();
    }

    SYS_FORCE_INLINE void set(GA_Group* const groupPtr)
    {
        clear();
        if (!groupPtr)
            return;

        groupArray.emplace_back(groupPtr);
    }

    SYS_FORCE_INLINE void set(GA_Group& groupPtr)
    {
        clear();
        groupArray.emplace_back(&groupPtr);
    }

    void set(const GA_GroupType groupClass, const UT_StringRef& groupPattern)
    {
        if (!groupPattern.isstring() || groupPattern.length() == 0)
            return;

        GA_Group* groupPtr = geo->getGroupTable(groupClass)->find(groupPattern);
        set(groupPtr);
    }
    
    SYS_FORCE_INLINE void append(GA_Group* const groupPtr)
    {
        if (!groupPtr)
            return;
        groupArray.emplace_back(groupPtr);
    }

    SYS_FORCE_INLINE void append(GA_Group& groupPtr)
    { groupArray.emplace_back(&groupPtr); }

    void append(const GA_GroupType groupClass, const UT_StringRef& groupPattern)
    {
        if (!groupPattern.isstring() || groupPattern.length() == 0)
            return;

        GA_Group* groupPtr = geo->getGroupTable(groupClass)->find(groupPattern);
        append(groupPtr);
    }
    
    void appends(const GA_GroupType groupClass, const char* groupPattern)
    {
        if (!groupPattern)
            return;

        for (GA_GroupTable::iterator<GA_Group> it = geo->getGroupTable(groupClass)->beginTraverse(); !it.atEnd(); ++it)
        {
            GA_Group* const groupPtr = it.group();
            if (!groupPtr->getName().multiMatch(groupPattern))
                continue;
            groupArray.emplace_back(groupPtr);
        }
    }
    SYS_FORCE_INLINE void appends(const GA_GroupType groupClass, const UT_StringRef& groupPattern)
    {
        if (!groupPattern.isstring() || groupPattern.length() == 0)
            return;
        appends(groupClass, groupPattern.c_str());
    }

    void oappends(const GA_GroupType groupClass, const char* groupPattern)
    {
        if (!groupPattern)
            return;

        for (GA_GroupTable::ordered_iterator it = geo->getGroupTable(groupClass)->obegin(); !it.atEnd(); ++it)
        {
            GA_Group* const groupPtr = *it;
            if (!groupPtr->getName().multiMatch(groupPattern))
                continue;
            groupArray.emplace_back(groupPtr);
        }
    }
    SYS_FORCE_INLINE void oappends(const GA_GroupType groupClass, const UT_StringRef& groupPattern)
    {
        if (!groupPattern.isstring() || groupPattern.length() == 0)
            return;
        appends(groupClass, groupPattern.c_str());
    }

    SYS_FORCE_INLINE void oappendPrims(const char* attribPattern)
    { return oappends(GA_GROUP_PRIMITIVE, attribPattern); }

    SYS_FORCE_INLINE void oappendPoints(const char* attribPattern)
    { return oappends(GA_GROUP_POINT, attribPattern); }
    
    SYS_FORCE_INLINE void oappendVertices(const char* attribPattern)
    { return oappends(GA_GROUP_VERTEX, attribPattern); }
    
    SYS_FORCE_INLINE void oappendEdges(const char* attribPattern)
    { return oappends(GA_GROUP_EDGE, attribPattern); }



    
    SYS_FORCE_INLINE void appends(const GA_AttributeOwner groupClass, const char* groupPattern)
    { appends(GFE_Type::attributeOwner_groupType(groupClass), groupPattern); }

    SYS_FORCE_INLINE void appends(const GA_AttributeOwner groupClass, const UT_StringRef& groupPattern)
    { (GFE_Type::attributeOwner_groupType(groupClass), groupPattern); }

    
    SYS_FORCE_INLINE void set(const GA_AttributeOwner groupClass, const UT_StringRef& groupPattern)
    { set(GFE_Type::attributeOwner_groupType(groupClass), groupPattern); }

    SYS_FORCE_INLINE void append(const GA_AttributeOwner groupClass, const UT_StringRef& groupPattern)
    { append(GFE_Type::attributeOwner_groupType(groupClass), groupPattern); }


    GA_Group* findOrCreate(
        const bool detached = false,
        const GA_GroupType groupType = GA_GROUP_POINT,
        const UT_StringRef& groupName = "",
        const bool emplaceBack = true
    )
    {
        GA_Group* groupPtr;
        if (detached)
        {
            switch (groupType)
            {
            case GA_GROUP_PRIMITIVE: groupUPtrArray.emplace_back(geo->createDetachedPrimitiveGroup()); break;
            case GA_GROUP_POINT:     groupUPtrArray.emplace_back(geo->createDetachedPointGroup());     break;
            case GA_GROUP_VERTEX:    groupUPtrArray.emplace_back(geo->createDetachedVertexGroup());    break;
            case GA_GROUP_EDGE:      groupUPtrArray.emplace_back(geo->createDetachedEdgeGroup());      break;
            default:
                if (cookparms)
                    cookparms->sopAddError(SOP_ERR_BADGROUP, groupName);
                UT_ASSERT_MSG(0, "No Group");
                return nullptr;
                break;
            }
            groupPtr = groupUPtrArray[groupUPtrArray.size()-1].get();
            //groupPtr = attribUPtr.get();
        }
        else
        {
            groupPtr = geo->getGroupTable(groupType)->find(groupName);

            if (!groupPtr)
                groupPtr = geo->getGroupTable(groupType)->newGroup(groupName);

            if (!groupPtr)
            {
                if (cookparms)
                    cookparms->sopAddError(SOP_ERR_BADGROUP, groupName);
                UT_ASSERT_MSG(groupPtr, "No Group");
                return nullptr;
            }
        }
        if (emplaceBack)
            groupArray.emplace_back(groupPtr);
        return groupPtr;
    }


    SYS_FORCE_INLINE GA_Group*
        findOrCreate(
            const bool detached = false,
            const GA_AttributeOwner owner = GA_ATTRIB_POINT,
            const UT_StringRef& groupName = "",
            const bool emplaceBack = true
        )
    { return findOrCreate(detached, GFE_Type::attributeOwner_groupType(owner), groupName, emplaceBack); }


    SYS_FORCE_INLINE GA_PrimitiveGroup*
        findOrCreatePrimitive(
            const bool detached = false,
            const UT_StringRef& groupName = "",
            const bool emplaceBack = true
        )
    { return static_cast<GA_PrimitiveGroup*>(findOrCreate(detached, GA_GROUP_PRIMITIVE, groupName, emplaceBack)); }

    SYS_FORCE_INLINE GA_PointGroup*
        findOrCreatePoint(
            const bool detached = false,
            const UT_StringRef& groupName = "",
            const bool emplaceBack = true
        )
    { return static_cast<GA_PointGroup*>(findOrCreate(detached, GA_GROUP_POINT, groupName, emplaceBack)); }

    SYS_FORCE_INLINE GA_VertexGroup*
        findOrCreateVertex(
            const bool detached = false,
            const UT_StringRef& groupName = "",
            const bool emplaceBack = true
        )
    { return static_cast<GA_VertexGroup*>(findOrCreate(detached, GA_GROUP_VERTEX, groupName, emplaceBack)); }

    SYS_FORCE_INLINE GA_EdgeGroup*
        findOrCreateEdge(
            const bool detached = false,
            const UT_StringRef& groupName = "",
            const bool emplaceBack = true
        )
    { return static_cast<GA_EdgeGroup*>(findOrCreate(detached, GA_GROUP_EDGE, groupName, emplaceBack)); }

    
    
    void bumpDataId() const
    {
        const size_t len = groupArray.size();
        for (size_t i = 0; i < len; i++)
        {
            GA_Group* ptr = groupArray[i];
            if (ptr->isDetached())
                continue;
            if (ptr->classType() == GA_GROUP_EDGE)
                static_cast<GA_EdgeGroup*>(ptr)->bumpDataId();
            else
                static_cast<GA_ElementGroup*>(ptr)->bumpDataId();
        }
    }

    SYS_FORCE_INLINE std::vector<GA_Group*>& ref()
    { return groupArray; }

    SYS_FORCE_INLINE const std::vector<GA_Group*>& ref() const
    { return groupArray; }


protected:
    SYS_FORCE_INLINE void setDetail(GA_Detail* const inGeo)
    { geo = inGeo; }

    SYS_FORCE_INLINE void setDetail(GA_Detail& inGeo)
    { geo = &inGeo; }




protected:
    GA_Detail* geo;
    const SOP_NodeVerb::CookParms* cookparms;

private:
    std::vector<GA_GroupUPtr> groupUPtrArray;
    std::vector<GA_Group*> groupArray;

    friend class GFE_AttribFilter;
    friend class GFE_AttribCreateFilter;

}; // End of class GFE_GroupArray







class GFE_RefAttribArray {

public:
    GFE_RefAttribArray(
        const GA_Detail* const geo,
        const SOP_NodeVerb::CookParms* const cookparms = nullptr
    )
        : geo(geo)
        , cookparms(cookparms)
    {
        attribUPtrArray.reserve(16);
        attribArray.reserve(16);
    }

    GFE_RefAttribArray(
        const GA_Detail* const geo,
        const SOP_NodeVerb::CookParms& cookparms
    )
        : geo(geo)
        , cookparms(&cookparms)
    {
        attribUPtrArray.reserve(16);
        attribArray.reserve(16);
    }

    GFE_RefAttribArray(
        const GA_Detail& geo,
        const SOP_NodeVerb::CookParms* const cookparms = nullptr
    )
        : geo(&geo)
        , cookparms(cookparms)
    {
        attribUPtrArray.reserve(16);
        attribArray.reserve(16);
    }

    GFE_RefAttribArray(
        const GA_Detail& geo,
        const SOP_NodeVerb::CookParms& cookparms
    )
        : geo(&geo)
        , cookparms(&cookparms)
    {
        attribUPtrArray.reserve(16);
        attribArray.reserve(16);
    }
    
    ~GFE_RefAttribArray()
    {
    }

    SYS_FORCE_INLINE const GA_Attribute* &operator[](const size_t i)
    { return attribArray[i]; }

    SYS_FORCE_INLINE bool isEmpty() const
    { return attribArray.size() == 0; }

    SYS_FORCE_INLINE void erase(const size_t i)
    { attribArray.erase(attribArray.begin()+i); }

    SYS_FORCE_INLINE void clear()
    {
        attribArray.clear();
        attribUPtrArray.clear();
    }

    SYS_FORCE_INLINE ::std::vector<GA_Attribute*>::size_type size() const
    {
        return attribArray.size();
    }

    SYS_FORCE_INLINE void reset(
        const GA_Detail* const geo,
        const SOP_NodeVerb::CookParms* const cookparms = nullptr
    )
    {
        this->geo = geo;
        this->cookparms = cookparms;
        clear();
    }

    SYS_FORCE_INLINE void set(const GA_Attribute* const attribPtr)
    {
        clear();
        if (!attribPtr)
            return;
        attribArray.emplace_back(attribPtr);
    }

    SYS_FORCE_INLINE void set(const GA_Attribute& attribPtr)
    {
        clear();
        attribArray.emplace_back(&attribPtr);
    }

    const GA_Attribute*
        set(
            const GA_AttributeOwner attribClass,
            const UT_StringRef& attribPattern
        )
    {
        if (!attribPattern.isstring() || attribPattern.length() == 0)
            return nullptr;

        const GA_Attribute* attribPtr = geo->findAttribute(attribClass, attribPattern);
        set(attribPtr);
        return attribPtr;
    }

    SYS_FORCE_INLINE void append(const GA_Attribute* const attribPtr)
    {
        if (!attribPtr)
            return;
        attribArray.emplace_back(attribPtr);
    }

    SYS_FORCE_INLINE void append(const GA_Attribute& attribPtr)
    {
        attribArray.emplace_back(&attribPtr);
    }

    const GA_Attribute*
        append(
            const GA_AttributeOwner attribClass,
            const UT_StringRef& attribPattern
        )
    {
        if (!attribPattern.isstring() || attribPattern.length() == 0)
            return nullptr;

        const GA_Attribute* attribPtr = geo->findAttribute(attribClass, attribPattern);
        append(attribPtr);
        return attribPtr;
    }

    const GA_Attribute*
        appendUV(
            const UT_StringRef& attribPattern,
            const GA_AttributeOwner attribClass = GA_ATTRIB_INVALID
        )
    {
        if (!attribPattern.isstring() || attribPattern.length() == 0)
            return nullptr;

        const GA_Attribute* const attribPtr = GFE_Attribute::findUVAttributePointVertex(*geo, attribClass, attribPattern);
        append(attribPtr);
        return attribPtr;
    }




    void
        appends(
            const GA_AttributeOwner attribClass,
            const UT_StringRef& attribPattern
        )
    {
        if (!attribPattern.isstring() || attribPattern.length() == 0)
            return;

        for (GA_AttributeDict::iterator it = geo->getAttributes().begin(attribClass); !it.atEnd(); ++it)
        {
            GA_Attribute* const attribPtr = it.attrib();
            if (attribPtr->getScope() != GA_SCOPE_PUBLIC)
                continue;
            if (!attribPtr->getName().multiMatch(attribPattern))
                continue;
            attribArray.emplace_back(attribPtr);
        }
    }

    SYS_FORCE_INLINE void appendPointVertexs(const UT_StringRef& attribPattern)
    {
        appends(GA_ATTRIB_POINT, attribPattern);
        appends(GA_ATTRIB_VERTEX, attribPattern);
    }

    const GA_Attribute*
        findTuple(
            const GA_AttributeOwner owner = GA_ATTRIB_POINT,
            const GA_StorageClass storageClass = GA_STORECLASS_FLOAT,
            const GA_Storage storage = GA_STORE_INVALID,
            const UT_StringRef& attribName = "",
            const int tuple_size = 1,
            const GA_Defaults& defaults = GA_Defaults(0.0f),
            const bool emplaceBack = true,
            const GA_AttributeOptions* const attribute_options = nullptr
        )
    {
        const GA_Storage finalStorage = GFE_Type::getPreferredStorage(geo, storageClass, storage);

        const GA_Attribute* attribPtr = geo->findAttribute(owner, attribName);
        if (attribPtr)
        {
            const GA_AIFTuple* const aifTuple = attribPtr->getAIFTuple();
            if (attribPtr->getTupleSize() != tuple_size ||
                aifTuple->getStorage(attribPtr) != finalStorage ||
                aifTuple->getDefaults(attribPtr) != defaults)
            {
                attribPtr = nullptr;
            }
            else
            {
                if (emplaceBack)
                    attribArray.emplace_back(attribPtr);
                return attribPtr;
            }
        }

        if (!attribPtr)
        {
            if (cookparms)
                cookparms->sopAddError(SOP_ATTRIBUTE_INVALID, attribName);
            UT_ASSERT_MSG(attribPtr, "No Attrib");
            return nullptr;
        }
        if (emplaceBack)
            attribArray.emplace_back(attribPtr);
        return attribPtr;
    }

    //SYS_FORCE_INLINE
    //void
    //findOrCreate(
    //    const GA_AttributeOwner owner,
    //    const GA_Storage storage,
    //    const UT_StringRef& attribName
    //)
    //{
    //    findOrCreate(owner, storage, attribName);
    //}

    const GA_Attribute*
        findUV(
            const GA_AttributeOwner owner = GA_ATTRIB_POINT,
            const GA_Storage storage = GA_STORE_INVALID,
            const UT_StringRef& attribName = "",
            const int tuple_size = 3,
            const GA_Defaults& defaults = GA_Defaults(0.0f),
            const bool emplaceBack = true,
            const GA_StorageClass storageClass = GA_STORECLASS_FLOAT
        )
    {
        const GA_Storage finalStorage = GFE_Type::getPreferredStorage(geo, storageClass, storage);

        const GA_Attribute* attribPtr = GFE_Attribute::findAttributePointVertex(*geo, owner, attribName);
        if (attribPtr)
        {
            const GA_AIFTuple* const aifTuple = attribPtr->getAIFTuple();
            if (!aifTuple ||
                attribPtr->getTupleSize() != tuple_size ||
                aifTuple->getStorage(attribPtr) != finalStorage ||
                aifTuple->getDefaults(attribPtr) != defaults)
            {
                attribPtr = nullptr;
            }
            else
            {
                if (emplaceBack)
                    attribArray.emplace_back(attribPtr);
                return attribPtr;
            }
        }

        const GA_AttributeOwner validOwner = owner == GA_ATTRIB_POINT ? GA_ATTRIB_POINT : GA_ATTRIB_VERTEX;
        if (!attribPtr)
        {
            if (cookparms)
                cookparms->sopAddError(SOP_ATTRIBUTE_INVALID, attribName);
            UT_ASSERT_MSG(attribPtr, "No Attrib");
            return nullptr;
        }
        if (emplaceBack)
            attribArray.emplace_back(attribPtr);
        return attribPtr;
    }

    const GA_Attribute*
        findDir(
            const GA_AttributeOwner owner = GA_ATTRIB_POINT,
            const GA_Storage storage = GA_STORE_INVALID,
            const UT_StringRef& attribName = "",
            const int tuple_size = 3,
            const GA_Defaults& defaults = GA_Defaults(0.0f),
            const bool emplaceBack = true,
            const GA_StorageClass storageClass = GA_STORECLASS_FLOAT
        )
    {
        const GA_Storage finalStorage = GFE_Type::getPreferredStorage(geo, storageClass, storage);

        const GA_Attribute* attribPtr = geo->findAttribute(owner, attribName);
        if (attribPtr)
        {
            const GA_AIFTuple* const aifTuple = attribPtr->getAIFTuple();
            if (attribPtr->getTupleSize() != tuple_size ||
                aifTuple->getStorage(attribPtr) != finalStorage ||
                aifTuple->getDefaults(attribPtr) != defaults)
            {
                attribPtr = nullptr;
            }
            else
            {
                if (emplaceBack)
                    attribArray.emplace_back(attribPtr);
                return attribPtr;
            }
        }

        if (!attribPtr)
        {
            if (cookparms)
                cookparms->sopAddError(SOP_ATTRIBUTE_INVALID, attribName);
            UT_ASSERT_MSG(attribPtr, "No Attrib");
            return nullptr;
        }
        if (emplaceBack)
            attribArray.emplace_back(attribPtr);
        return attribPtr;
    }


    const GA_Attribute*
        findNormal3D(
            const GFE_NormalSearchOrder owner = GFE_NormalSearchOrder::ALL,
            const GA_Storage storage = GA_STORE_INVALID,
            const UT_StringRef& attribName = "",
            const int tuple_size = 3,
            const GA_Defaults& defaults = GA_Defaults(0.0f),
            const bool emplaceBack = true,
            const GA_StorageClass storageClass = GA_STORECLASS_FLOAT
        )
    {
        const GA_Storage finalStorage = GFE_Type::getPreferredStorage(geo, storageClass, storage);

        const GA_Attribute* attribPtr = GFE_Attribute::findNormal3D(*geo, owner, attribName);
        if (attribPtr)
        {
            const GA_AIFTuple* const aifTuple = attribPtr->getAIFTuple();
            if (attribPtr->getTupleSize() != tuple_size ||
                aifTuple->getStorage(attribPtr) != finalStorage ||
                aifTuple->getDefaults(attribPtr) != defaults)
            {
                attribPtr = nullptr;
            }
            else
            {
                if (emplaceBack)
                    attribArray.emplace_back(attribPtr);
                return attribPtr;
            }
        }

        const GA_AttributeOwner validOwner = GFE_Attribute::toValidOwner(owner);

        if (!attribPtr)
        {
            if (cookparms)
                cookparms->sopAddError(SOP_ATTRIBUTE_INVALID, attribName);
            UT_ASSERT_MSG(attribPtr, "No Attrib");
            return nullptr;
        }
        if (emplaceBack)
            attribArray.emplace_back(attribPtr);
        return attribPtr;
    }





    SYS_FORCE_INLINE void pop_back()
    { attribArray.pop_back(); }

    SYS_FORCE_INLINE const GA_Attribute* last() const
    { return attribArray[attribArray.size() - 1]; }


    SYS_FORCE_INLINE std::vector<const GA_Attribute*>& ref()
    { return attribArray; }


protected:
    SYS_FORCE_INLINE void setDetail(const GA_Detail* const inGeo)
    {
        geo = inGeo;
    }



protected:
    const GA_Detail* geo;
    const SOP_NodeVerb::CookParms* cookparms;

private:
    std::vector<GA_AttributeUPtr> attribUPtrArray;
    std::vector<const GA_Attribute*> attribArray;

    friend class GFE_GeoFilterRef;

}; // End of class GFE_RefAttribArray














class GFE_RefGroupArray {

public:
    GFE_RefGroupArray(
        const GA_Detail* const geo,
        const SOP_NodeVerb::CookParms* const cookparms = nullptr
    )
        : geo(geo)
        , cookparms(cookparms)
    {
        groupUPtrArray.reserve(16);
        groupArray.reserve(16);
    }

    GFE_RefGroupArray(
        const GA_Detail* const geo,
        const SOP_NodeVerb::CookParms& cookparms
    )
        : geo(geo)
        , cookparms(&cookparms)
    {
        groupUPtrArray.reserve(16);
        groupArray.reserve(16);
    }

    GFE_RefGroupArray(
        const GA_Detail& geo,
        const SOP_NodeVerb::CookParms* const cookparms = nullptr
    )
        : geo(&geo)
        , cookparms(cookparms)
    {
        groupUPtrArray.reserve(16);
        groupArray.reserve(16);
    }

    GFE_RefGroupArray(
        const GA_Detail& geo,
        const SOP_NodeVerb::CookParms& cookparms
    )
        : geo(&geo)
        , cookparms(&cookparms)
    {
        groupUPtrArray.reserve(16);
        groupArray.reserve(16);
    }

    ~GFE_RefGroupArray()
    {
    }
    

    
    
    SYS_FORCE_INLINE const GA_ElementGroup* getElementGroup(const size_t i) const
    {
        UT_ASSERT(groupArray[i]->isElementGroup());
        return static_cast<const GA_ElementGroup*>(groupArray[i]);
    }

    SYS_FORCE_INLINE const GA_PrimitiveGroup* getPrimitiveGroup(const size_t i) const
    {
        UT_ASSERT(groupArray[i]->classType() == GA_GROUP_PRIMITIVE);
        return static_cast<const GA_PrimitiveGroup*>(groupArray[i]);
    }

    SYS_FORCE_INLINE const GA_PointGroup* getPointGroup(const size_t i) const
    {
        UT_ASSERT(groupArray[i]->classType() == GA_GROUP_POINT);
        return static_cast<const GA_PointGroup*>(groupArray[i]);
    }

    SYS_FORCE_INLINE const GA_VertexGroup* getVertexGroup(const size_t i) const
    {
        UT_ASSERT(groupArray[i]->classType() == GA_GROUP_VERTEX);
        return static_cast<const GA_VertexGroup*>(groupArray[i]);
    }

    SYS_FORCE_INLINE const GA_EdgeGroup* getEdgeGroup(const size_t i) const
    {
        UT_ASSERT(groupArray[i]->classType() == GA_GROUP_EDGE);
        return static_cast<const GA_EdgeGroup*>(groupArray[i]);
    }

    SYS_FORCE_INLINE const GA_Group* &operator[](const size_t i)
    { return groupArray[i]; }

    SYS_FORCE_INLINE bool isEmpty() const
    { return groupArray.size() == 0; }

    SYS_FORCE_INLINE void erase(const size_t i)
    { groupArray.erase(groupArray.begin()+i); }
    
    SYS_FORCE_INLINE void clear()
    {
        groupArray.clear();
        groupUPtrArray.clear();
    }

    SYS_FORCE_INLINE ::std::vector<GA_Group*>::size_type size() const
    { return groupArray.size(); }

    SYS_FORCE_INLINE const GA_Group* last()
    { return groupArray[groupArray.size() - 1]; }

    SYS_FORCE_INLINE void reset(
        const GA_Detail* const geo,
        const SOP_NodeVerb::CookParms* const cookparms = nullptr
    )
    {
        this->geo = geo;
        this->cookparms = cookparms;
        clear();
    }

    SYS_FORCE_INLINE void set(GA_Group* const groupPtr)
    {
        clear();
        if (!groupPtr)
            return;
        groupArray.emplace_back(groupPtr);
    }

    SYS_FORCE_INLINE void set(GA_Group& groupPtr)
    {
        clear();
        groupArray.emplace_back(&groupPtr);
    }

    void set(const GA_GroupType groupClass, const UT_StringRef& groupPattern)
    {
        if (!groupPattern.isstring() || groupPattern.length() == 0)
            return;

        GA_Group* groupPtr = geo->getGroupTable(groupClass)->find(groupPattern);
        set(groupPtr);
    }

    SYS_FORCE_INLINE void append(GA_Group* const groupPtr)
    {
        if (!groupPtr)
            return;
        groupArray.emplace_back(groupPtr);
    }

    SYS_FORCE_INLINE void append(GA_Group& groupPtr)
    {
        groupArray.emplace_back(&groupPtr);
    }

    void append(const GA_GroupType groupClass, const UT_StringRef& groupPattern)
    {
        if (!groupPattern.isstring() || groupPattern.length() == 0)
            return;

        GA_Group* groupPtr = geo->getGroupTable(groupClass)->find(groupPattern);
        append(groupPtr);
    }

    void appends(const GA_GroupType groupClass, const char* groupPattern)
    {
        if (!groupPattern)
            return;

        for (GA_GroupTable::iterator<GA_Group> it = geo->getGroupTable(groupClass)->beginTraverse(); !it.atEnd(); ++it)
        {
            GA_Group* const groupPtr = it.group();
            if (!groupPtr->getName().multiMatch(groupPattern))
                continue;
            groupArray.emplace_back(groupPtr);
        }
    }

    SYS_FORCE_INLINE void appends(const GA_GroupType groupClass, const UT_StringRef& groupPattern)
    {
        if (!groupPattern.isstring() || groupPattern.length() == 0)
            return;
        appends(groupClass, groupPattern.c_str());
    }

    SYS_FORCE_INLINE void appends(const GA_AttributeOwner groupClass, const char* groupPattern)
    { appends(GFE_Type::attributeOwner_groupType(groupClass), groupPattern); }

    SYS_FORCE_INLINE void appends(const GA_AttributeOwner groupClass, const UT_StringRef& groupPattern)
    { appends(GFE_Type::attributeOwner_groupType(groupClass), groupPattern); }
    

    SYS_FORCE_INLINE void set(const GA_AttributeOwner groupClass, const UT_StringRef& groupPattern)
    { set(GFE_Type::attributeOwner_groupType(groupClass), groupPattern); }

    SYS_FORCE_INLINE void append(const GA_AttributeOwner groupClass, const UT_StringRef& groupPattern)
    { append(GFE_Type::attributeOwner_groupType(groupClass), groupPattern); }



    GA_Group* find(const GA_GroupType groupType = GA_GROUP_POINT, const UT_StringRef& groupName = "")
    {
        GA_Group* groupPtr = geo->getGroupTable(groupType)->find(groupName);

        if (!groupPtr)
        {
            if (cookparms)
                cookparms->sopAddError(SOP_ERR_BADGROUP, groupName);
            UT_ASSERT_MSG(groupPtr, "No Group");
            return nullptr;
        }
        groupArray.emplace_back(groupPtr);
        return groupPtr;
    }



    SYS_FORCE_INLINE ::std::vector<const GA_Group*>& ref()
    { return groupArray; }


protected:
    SYS_FORCE_INLINE void setDetail(const GA_Detail* const inGeo)
    { geo = inGeo; }


protected:
    const GA_Detail* geo;
    const SOP_NodeVerb::CookParms* cookparms;

private:
    ::std::vector<GA_GroupUPtr> groupUPtrArray;
    ::std::vector<const GA_Group*> groupArray;

    friend class GFE_GeoFilterRef;

}; // End of class GFE_RefGroupArray















#endif
