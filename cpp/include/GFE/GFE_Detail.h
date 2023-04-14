
#pragma once

#ifndef __GFE_Detail_h__
#define __GFE_Detail_h__



//#include "GFE/GFE_Detail.h"

#include "GA/GA_Detail.h"

#include "GA/GA_PageHandle.h"
#include "GA/GA_PageIterator.h"




#include "GFE_Group.h"



class GFE_Detail : public GA_Detail
{

public:

SYS_FORCE_INLINE
    GA_Offset
    getPrimitivePointOffset(
        const GA_Size primoff,
        const GA_Size vtxpnum
    ) const
{
    return vertexPoint(getPrimitiveVertexOffset(primoff, vtxpnum));
}


SYS_FORCE_INLINE
UT_Vector3
getPrimitivePointPos3(
    const GA_Size primoff,
    const GA_Size vtxpnum
) const
{
    return getPos3(getPrimitivePointOffset(primoff, vtxpnum));
}

#if SYS_VERSION_MAJOR_INT > 19 || ( SYS_VERSION_MAJOR_INT == 19 && SYS_VERSION_MINOR_INT == 5 )

SYS_FORCE_INLINE
UT_Vector3D
getPrimitivePointPos3D(
    const GA_Size primoff,
    const GA_Size vtxpnum
) const
{
    return getPos3D(getPrimitivePointOffset(primoff, vtxpnum));

}

template<typename T>
SYS_FORCE_INLINE
UT_Vector3T<T>
getPrimitivePointPos3T(
    const GA_Size primoff,
    const GA_Size vtxpnum
) const
{
    return getPos3T<T>(getPrimitivePointOffset(primoff, vtxpnum));
}

#endif


SYS_FORCE_INLINE
GA_Size
    getNumElements(
        const GA_AttributeOwner attribClass
    ) const
{
    switch (attribClass)
    {
    case GA_ATTRIB_PRIMITIVE:
        return getNumPrimitives();
        break;
    case GA_ATTRIB_POINT:
        return getNumPoints();
        break;
    case GA_ATTRIB_VERTEX:
        return getNumVertices();
        break;
    default:
        return -1;
        break;
    }
    return -1;
}

SYS_FORCE_INLINE
GA_Size
getNumElements(
    const GA_GroupType groupType
) const
{
    switch (groupType)
    {
    case GA_GROUP_PRIMITIVE:
        return getNumPrimitives();
        break;
    case GA_GROUP_POINT:
        return getNumPoints();
        break;
    case GA_GROUP_VERTEX:
        return getNumVertices();
        break;
    default:
        return -1;
        break;
    }
    return -1;
}

void
clearElement()
{
    //clear();
    clearTopologyAttributes();
    getIndexMap(GA_ATTRIB_PRIMITIVE).clear(true);
    getIndexMap(GA_ATTRIB_POINT    ).clear(true);
    getIndexMap(GA_ATTRIB_VERTEX   ).clear(true);
    getPrimitiveList()              .clear(true);
    createTopologyAttributes();
}

    
GA_OffsetList
    getOffsetList(
        const GA_IndexMap& indexMap,
        const GA_ElementGroup* const group = nullptr,
        const bool reverse = false
    ) const
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
    
SYS_FORCE_INLINE
GA_OffsetList
getOffsetList(
    const GA_AttributeOwner owner,
    const GA_ElementGroup* const group = nullptr,
    const bool reverse = false
) const
{
    return getOffsetList(getIndexMap(owner), group, reverse);
}

SYS_FORCE_INLINE
GA_OffsetList
getOffsetList(
    const GA_PrimitiveGroup* const group,
    const bool reverse = false
) const
{
    return getOffsetList(getPrimitiveMap(), group, reverse);
}

SYS_FORCE_INLINE
GA_OffsetList
getOffsetList(
    const GA_PointGroup* const group,
    const bool reverse = false
) const
{
    return getOffsetList(getPointMap(), group, reverse);
}

//GA_OffsetList offList = GFE_Detail::getOffsetList(group);
SYS_FORCE_INLINE
GA_OffsetList
getOffsetList(
    const GA_VertexGroup* const group,
    const bool reverse = false
) const
{
    return getOffsetList(getVertexMap(), group, reverse);
}










SYS_FORCE_INLINE
GA_Precision
    getPreferredPrecision() const
{
    return GA_Detail::getPreferredPrecision();
}
    
SYS_FORCE_INLINE
GA_Precision
    getPreferredPrecision(
        const GA_Precision precision
    ) const
{
    return precision == GA_PRECISION_INVALID ? getPreferredPrecision() : precision;
}
    


SYS_FORCE_INLINE
GA_Storage
    getPreferredStorageI() const
{
    return GFE_Type::getPreferredStorageI(getPreferredPrecision());
}
    
SYS_FORCE_INLINE
GA_Storage
    getPreferredStorageF() const
{
    return GFE_Type::getPreferredStorageF(getPreferredPrecision());
}

SYS_FORCE_INLINE
    GA_Storage
    getPreferredStorage(
        const GA_StorageClass storageClass
    ) const
{
    return GFE_Type::getPreferredStorage(getPreferredPrecision(), storageClass);
}

SYS_FORCE_INLINE
    GA_Storage
    getPreferredStorage(
        const GA_Storage storage = GA_STORE_INVALID
    ) const
{
    return storage == GA_STORE_INVALID ? getPreferredStorage(storage) : storage;
}


SYS_FORCE_INLINE
    GA_Storage
    getPreferredStorageI(
        const GA_Storage storage
    ) const
{
    return storage == GA_STORE_INVALID ? getPreferredStorageI() : storage;
}

SYS_FORCE_INLINE
    GA_Storage
    getPreferredStorage(
        const GA_Storage storage,
        const GA_StorageClass storageClass
    ) const
{
    return storage == GA_STORE_INVALID ? getPreferredStorage(storageClass) : storage;
}
















    
#if 1
    
SYS_FORCE_INLINE
GA_Group*
    findGroup(
        const GA_GroupType groupType,
        const UT_StringHolder& groupName
    )
{
    GA_GroupTable* const groupTable = getGroupTable(groupType);
    UT_ASSERT_P(groupTable);
    return groupTable->find(groupName);
}
    
SYS_FORCE_INLINE
    const GA_Group*
        findGroup(
            const GA_GroupType groupType,
            const UT_StringHolder& groupName
        ) const
{
    const GA_GroupTable* const groupTable = getGroupTable(groupType);
    UT_ASSERT_P(groupTable);
    return groupTable->find(groupName);
}
    
#else
    
GA_Group*
    findGroup(
        const GA_GroupType groupType,
        const UT_StringHolder& groupName
    )
{
    for (GA_GroupTable::iterator<GA_Group> it = getGroupTable(groupType).beginTraverse(); !it.atEnd(); ++it)
    {
        const GA_Group* const group = it.group();
        //if (group->isDetached())
        //    continue;
        if (group->getName() == groupName)
            return group;
    }
    return nullptr;
}
    
#endif

    SYS_FORCE_INLINE
    GA_PrimitiveGroup*
        findPrimitiveGroup(
            const UT_StringHolder& groupName
        )
{
    return static_cast<GA_PrimitiveGroup*>(findGroup(GA_GROUP_PRIMITIVE, groupName));
}

    SYS_FORCE_INLINE
        const GA_PrimitiveGroup*
            findPrimitiveGroup(
                const UT_StringHolder& groupName
            ) const
{
    return static_cast<const GA_PrimitiveGroup*>(findGroup(GA_GROUP_PRIMITIVE, groupName));
}

SYS_FORCE_INLINE
GA_EdgeGroup*
    findEdgeGroup(
        const UT_StringHolder& groupName
    )
{
    return static_cast<GA_EdgeGroup*>(findGroup(GA_GROUP_EDGE, groupName));
}

SYS_FORCE_INLINE
    const GA_EdgeGroup*
        findEdgeGroup(
            const UT_StringHolder& groupName
        ) const
{
    return static_cast<const GA_EdgeGroup*>(findGroup(GA_GROUP_EDGE, groupName));
}




    

SYS_FORCE_INLINE
bool
    destroyAttrib(
        GA_Attribute& attrib
    )
{
    return getAttributes().destroyAttribute(&attrib);
}


SYS_FORCE_INLINE
bool
    destroyAttrib(
        GA_Attribute* const attrib
    )
{
    return getAttributes().destroyAttribute(attrib);
}






    
SYS_FORCE_INLINE
bool
    renameAttrib(
        const GA_Attribute& attrib,
        const UT_StringHolder& newName
    )
{
    return renameAttribute(attrib.getOwner(), attrib.getScope(), attrib.getName(), newName);
}
    
SYS_FORCE_INLINE
bool
    renameAttrib(
        const GA_Attribute* const attrib,
        const UT_StringHolder& newName
    )
{
    UT_ASSERT_P(attrib);
    return renameAttrib(*attrib, newName);
}
    

bool
    forceRenameAttribute(
        GA_Attribute& attrib,
        const UT_StringHolder& newName
    )
{
    GA_Attribute* const existAttrib = findAttribute(attrib.getOwner(), newName);
    if (existAttrib)
        getAttributes().destroyAttribute(existAttrib);
    return renameAttribute(attrib.getOwner(), GA_SCOPE_PUBLIC, attrib.getName(), newName);
}

SYS_FORCE_INLINE
    bool
        forceRenameAttribute(
            GA_Attribute* const attrib,
            const UT_StringHolder& newName
        )
{
    UT_ASSERT_P(attrib);
    return forceRenameAttribute(*attrib, newName);
}



    
SYS_FORCE_INLINE
GA_GroupType
attributeOwner_groupType(
    const GA_AttributeOwner attribOwner
)
{
    return GFE_Type::attributeOwner_groupType(attribOwner);
}

SYS_FORCE_INLINE
GA_AttributeOwner
attributeOwner_groupType(
    const GA_GroupType groupType
)
{
    return GFE_Type::attributeOwner_groupType(groupType);
}



public:

SYS_FORCE_INLINE GA_Range getRangeByAnyGroup(const GA_ElementGroup& group) const
{
    return GA_Range(group);
}

SYS_FORCE_INLINE GA_Range getRangeByAnyGroup(const GA_ElementGroup* const group) const
{
    if (!group)
        return GA_Range();
    return GA_Range(*group);
    // const GA_AttributeOwner attribOwner = attributeOwner_groupType(group->classType());
    // return GA_Range(getIndexMap(attribOwner), group);
}

    
SYS_FORCE_INLINE GA_Range getRangeByAnyGroup(const GA_Group* const group) const
{
    return getRangeByAnyGroup(static_cast<const GA_ElementGroup*>(group));
}
SYS_FORCE_INLINE GA_Range getRangeByAnyGroup(const GA_Group& group) const
{
    return getRangeByAnyGroup(static_cast<const GA_ElementGroup&>(group));
}
SYS_FORCE_INLINE GA_SplittableRange getSplittableRangeByAnyGroup(const GA_ElementGroup* const group) const
{
    return GA_SplittableRange(getRangeByAnyGroup(group));
}

SYS_FORCE_INLINE GA_SplittableRange getSplittableRangeByAnyGroup(const GA_ElementGroup& group) const
{
    return getSplittableRangeByAnyGroup(&group);
}
    
SYS_FORCE_INLINE GA_SplittableRange getSplittableRangeByAnyGroup(const GA_Group* const group) const
{
    return getSplittableRangeByAnyGroup(static_cast<const GA_ElementGroup*>(group));
}

SYS_FORCE_INLINE GA_SplittableRange getSplittableRangeByAnyGroup(const GA_Group& group) const
{
    return getSplittableRangeByAnyGroup(&group);
}

    
public:







//     
//     
// SYS_FORCE_INLINE
// GA_Attribute*
// createDetachedAttribute(
//     const GA_AttributeOwner owner = GA_ATTRIB_POINT,
//     const GA_StorageClass storageClass = GA_STORECLASS_FLOAT,
//     const GA_Storage storage = GA_STORE_INVALID,
//     const bool emplaceBack = true,
//     const UT_StringRef& attribtype = "numeric",
//     const UT_Options* create_args = nullptr,
//     const GA_AttributeOptions* attribute_options = nullptr
// )
// {
//     const GA_Storage finalStorage = getPreferredStorage(storage, storageClass);
//
//     attribUPtrArray.emplace_back(createDetachedAttribute(owner, attribtype, create_args, attribute_options));
//     GA_Attribute* attribPtr = attribUPtrArray[attribUPtrArray.size() - 1].get();
//
//     if (emplaceBack)
//         attribArray.emplace_back(attribPtr);
//     return attribPtr;
// }
//
// SYS_FORCE_INLINE
// GA_Attribute*
// createDetachedAttribute(
//     const GA_AttributeOwner owner,
//     const GA_StorageClass storageClass,
//     const GA_Storage storage,
//     const bool emplaceBack,
//     const GA_AttributeType& attribtype,
//     const UT_Options* create_args = nullptr,
//     const GA_AttributeOptions* attribute_options = nullptr
// )
// {
//     const GA_Storage finalStorage = getPreferredStorage(storage, storageClass);
//
//     attribUPtrArray.emplace_back(createDetachedAttribute(owner, attribtype, create_args, attribute_options));
//     GA_Attribute* attribPtr = attribUPtrArray[attribUPtrArray.size() - 1].get();
//
//     if (emplaceBack)
//         attribArray.emplace_back(attribPtr);
//     return attribPtr;
// }
//
// SYS_FORCE_INLINE
// GA_Attribute*
// createDetachedTupleAttribute(
//     const GA_AttributeOwner owner = GA_ATTRIB_POINT,
//     const GA_StorageClass storageClass = GA_STORECLASS_FLOAT,
//     const GA_Storage storage = GA_STORE_INVALID,
//     const int tuple_size = 1,
//     const GA_Defaults& defaults = GA_Defaults(0.0f),
//     const bool emplaceBack = true,
//     const GA_AttributeOptions* attribute_options = nullptr
// )
// {
//     const GA_Storage finalStorage = getPreferredStorage(storage, storageClass);
//     
//     attribUPtrArray.emplace_back(createDetachedTupleAttribute(owner, finalStorage, tuple_size, defaults, attribute_options));
//     GA_Attribute* attribPtr = attribUPtrArray[attribUPtrArray.size() - 1].get();
//
//     if (emplaceBack)
//         attribArray.emplace_back(attribPtr);
//     return attribPtr;
// }
//
// SYS_FORCE_INLINE
// GA_Attribute*
// createDetachedArrayAttribute(
//     const GA_AttributeOwner owner = GA_ATTRIB_POINT,
//     const GA_StorageClass storageClass = GA_STORECLASS_FLOAT,
//     const GA_Storage storage = GA_STORE_INVALID,
//     const int tuple_size = 1,
//     const bool emplaceBack = true,
//     const UT_Options* create_args = nullptr,
//     const GA_AttributeOptions* attribute_options = nullptr
// )
// {
//     const GA_Storage finalStorage = getPreferredStorage(storage, storageClass);
//
//     attribUPtrArray.emplace_back(createDetachedAttribute(owner, "arraydata", create_args, attribute_options));
//     GA_Attribute* attribPtr = attribUPtrArray[attribUPtrArray.size() - 1].get();
//
//     if (emplaceBack)
//         attribArray.emplace_back(attribPtr);
//     return attribPtr;
// }
//
//
//
//
// GA_Attribute*
// findOrCreateTuple(
//     const bool detached = false,
//     const GA_AttributeOwner owner = GA_ATTRIB_POINT,
//     const GA_StorageClass storageClass = GA_STORECLASS_FLOAT,
//     const GA_Storage storage = GA_STORE_INVALID,
//     const UT_StringHolder& attribName = "",
//     const int tuple_size = 1,
//     const GA_Defaults& defaults = GA_Defaults(0.0f),
//     const bool emplaceBack = true,
//     const UT_Options* create_args = nullptr,
//     const GA_AttributeOptions* attribute_options = nullptr
// )
// {
//     const GA_Storage finalStorage = getPreferredStorage(storage, storageClass);
//
//     GA_Attribute* attribPtr = findAttribute(owner, attribName);
//     if (attribPtr)
//     {
//         const GA_AIFTuple* const aifTuple = attribPtr->getAIFTuple();
//         if (!aifTuple ||
//             attribPtr->getTupleSize() != tuple_size ||
//             aifTuple->getStorage(attribPtr) != finalStorage ||
//             aifTuple->getDefaults(attribPtr) != defaults)
//         {
//             getAttributes().destroyAttribute(attribPtr);
//             attribPtr = nullptr;
//         }
//         else
//         {
//             if (emplaceBack)
//                 attribArray.emplace_back(attribPtr);
//             return attribPtr;
//         }
//     }
//
//     if (detached)
//     {
//         attribUPtrArray.emplace_back(createDetachedTupleAttribute(owner, finalStorage, 1));
//         attribPtr = attribUPtrArray[attribUPtrArray.size() - 1].get();
//         //attribPtr = attribUPtr.get();
//     }
//     else
//     {
//         if (!attribPtr)
//             attribPtr = createTupleAttribute(owner, attribName, finalStorage,
//                 tuple_size, defaults, create_args, attribute_options);
//
//         if (!attribPtr)
//         {
//             UT_ASSERT_MSG(attribPtr, "No Attrib");
//             return nullptr;
//         }
//     }
//     if (emplaceBack)
//         attribArray.emplace_back(attribPtr);
//     return attribPtr;
// }
//
//
//
//     GA_Attribute*
//     findOrCreateArray(
//         const bool detached = false,
//         const GA_AttributeOwner owner = GA_ATTRIB_POINT,
//         const GA_StorageClass storageClass = GA_STORECLASS_FLOAT,
//         const GA_Storage storage = GA_STORE_INVALID,
//         const UT_StringHolder& attribName = "",
//         const int tuple_size = 1,
//         const bool emplaceBack = true,
//         const UT_Options* create_args = nullptr,
//         const GA_AttributeOptions* attribute_options = nullptr
//     )
//     {
//         const GA_Storage finalStorage = getPreferredStorage(storage, storageClass);
//
//         GA_Attribute* attribPtr = findAttribute(owner, attribName);
//         if (attribPtr)
//         {
//             const GA_AIFNumericArray* const aifNumericArray = attribPtr->getAIFNumericArray();
//             if (!aifNumericArray ||
//                 attribPtr->getTupleSize() != tuple_size ||
//                 aifNumericArray->getStorage(attribPtr) != finalStorage)
//             {
//                 getAttributes().destroyAttribute(attribPtr);
//                 attribPtr = nullptr;
//             }
//             else
//             {
//                 if (emplaceBack)
//                     attribArray.emplace_back(attribPtr);
//                 return attribPtr;
//             }
//         }
//
//         if (detached)
//         {
// #if 0
//             switch (finalStorage)
//             {
//             case GA_STORE_INT16:
//                 break;
//             case GA_STORE_INT32:
//                 break;
//             case GA_STORE_INT64:
//                 break;
//             case GA_STORE_REAL16:
//                 break;
//             case GA_STORE_REAL32:
//                 break;
//             case GA_STORE_REAL64:
//                 break;
//             case GA_STORE_STRING:
//                 break;
//             case GA_STORE_DICT:
//                 break;
//             default:
//                 break;
//             }
//             attribUPtrArray.emplace_back(static_cast<GEO_Detail*>(geo)->createDetachedAttribute(owner, "arraydata", tuple_size, create_args, attribute_options));
// #else
//             attribUPtrArray.emplace_back(getAttributes().createDetachedAttribute(owner, "arraydata", create_args, attribute_options));
// #endif
//             attribPtr = attribUPtrArray[attribUPtrArray.size() - 1].get();
//             //attribPtr = attribUPtr.get();
//         }
//         else
//         {
//             if (!attribPtr)
//                 attribPtr = getAttributes().createArrayAttribute(owner, GA_SCOPE_PUBLIC, attribName, finalStorage,
//                     tuple_size, create_args, attribute_options);
//
//             if (!attribPtr)
//             {
//                 UT_ASSERT_MSG(attribPtr, "No Attrib");
//                 return nullptr;
//             }
//         }
//         if (emplaceBack)
//             attribArray.emplace_back(attribPtr);
//         return attribPtr;
//     }
//
//
//
//     GA_Attribute*
//     findOrCreatePiece(
//         const bool detached = false,
//         const GFE_PieceAttribSearchOrder pieceAttribSearchOrder = GFE_PieceAttribSearchOrder::PRIM,
//         const GA_AttributeOwner owner = GA_ATTRIB_PRIMITIVE,
//         const GA_StorageClass storageClass = GA_STORECLASS_INT,
//         const GA_Storage storage = GA_STORE_INVALID,
//         const UT_StringHolder& attribName = "",
//         const int tuple_size = 1,
//         const GA_Defaults& defaults = GA_Defaults(0.0f),
//         const bool emplaceBack = true,
//         const UT_Options* create_args = nullptr,
//         const GA_AttributeOptions* attribute_options = nullptr
//     )
//     {
//         const GA_Storage finalStorage = getPreferredStorage(storage, storageClass);
//
//         GA_Attribute* attribPtr = findPieceAttrib(pieceAttribSearchOrder, attribName);
//
//         // if (attribPtr)
//         // {
//         //     const bool promoteFromOtherClass = sopparms.getPromoteFromOtherClass();
//         //     if (promoteFromOtherClass)
//         //     {
//         //         if (geo0AttribClass != attribPtr->getOwner())
//         //         {
//         //             attribPtr = GFE_Attribpr::attribPromote(outGeo0, attribPtr, geo0AttribClass);
//         //             //attribPtr = GFE_AttribPromote::promote(*static_cast<GU_Detail*>(outGeo0), attribPtr, geo0AttribClass, sopparms.getDelOriginalAttrib(), GU_Promote::GU_PROMOTE_FIRST);
//         //         }
//         //     }
//         //
//         //     const bool forceCastAttribType = sopparms.getForceCastAttribType();
//         //     if (forceCastAttribType)
//         //     {
//         //         GFE_AttributeCast::attribCast(outGeo0, attribPtr, connectivityStorageClass, "", outGeo0->getPreferredPrecision());
//         //     }
//         //     return;
//         // }
//         
//         if (attribPtr)
//         {
//             const GA_AIFTuple* const aifTuple = attribPtr->getAIFTuple();
//             if (!aifTuple ||
//                 attribPtr->getTupleSize() != tuple_size ||
//                 aifTuple->getStorage(attribPtr) != finalStorage ||
//                 aifTuple->getDefaults(attribPtr) != defaults)
//             {
//                 getAttributes().destroyAttribute(attribPtr);
//                 attribPtr = nullptr;
//             }
//             else
//             {
//                 if (emplaceBack)
//                     attribArray.emplace_back(attribPtr);
//                 return attribPtr;
//             }
//         }
//
//         if (detached)
//         {
//             attribUPtrArray.emplace_back(createDetachedTupleAttribute(owner, finalStorage, 1));
//             attribPtr = attribUPtrArray[attribUPtrArray.size() - 1].get();
//             //attribPtr = attribUPtr.get();
//         }
//         else
//         {
//             if (!attribPtr)
//                 attribPtr = createTupleAttribute(owner, attribName, finalStorage,
//                     tuple_size, defaults, create_args, attribute_options);
//
//             if (!attribPtr)
//             {
//                 UT_ASSERT_MSG(attribPtr, "No Attrib");
//                 return nullptr;
//             }
//         }
//         if (emplaceBack)
//             attribArray.emplace_back(attribPtr);
//         return attribPtr;
//     }
//
//
//     GA_Attribute*
//     findPieceAttrib(
//         const GFE_PieceAttribSearchOrder pieceAttribSearchOrder,
//         const UT_StringHolder& pieceAttribName
//     )
//     {
//         GA_Attribute* attribPtr = nullptr;
//
//         switch (pieceAttribSearchOrder)
//         {
//         case GFE_PieceAttribSearchOrder::PRIM:       attribPtr = findAttribute(GA_ATTRIB_PRIMITIVE, pieceAttribName); break;
//         case GFE_PieceAttribSearchOrder::POINT:      attribPtr = findAttribute(GA_ATTRIB_POINT,     pieceAttribName); break;
//         case GFE_PieceAttribSearchOrder::VERTEX:     attribPtr = findAttribute(GA_ATTRIB_VERTEX,    pieceAttribName); break;
//         case GFE_PieceAttribSearchOrder::PRIMPOINT:
//             {
//                 GA_AttributeOwner searchOrder[2] = { GA_ATTRIB_PRIMITIVE, GA_ATTRIB_POINT };
//                 attribPtr = findAttribute(pieceAttribName, searchOrder, 2);
//             }
//             break;
//         case GFE_PieceAttribSearchOrder::POINTPRIM:
//             {
//                 GA_AttributeOwner searchOrder[2] = { GA_ATTRIB_POINT, GA_ATTRIB_PRIMITIVE };
//                 attribPtr = findAttribute(pieceAttribName, searchOrder, 2);
//             }
//             break;
//         case GFE_PieceAttribSearchOrder::ALL:
//             {
//                 GA_AttributeOwner searchOrder[3] = { GA_ATTRIB_PRIMITIVE, GA_ATTRIB_POINT, GA_ATTRIB_VERTEX };
//                 attribPtr = findAttribute(pieceAttribName, searchOrder, 3);
//             }
//             break;
//         default:
//             UT_ASSERT_MSG(0, "Unhandled Geo Piece Attrib Search Order!");
//             break;
//         }
//         return attribPtr;
//     }
//
//
//
//
//
//     
// GA_Attribute*
// findOrCreateUV(
//     const bool detached = false,
//     const GA_AttributeOwner owner = GA_ATTRIB_POINT,
//     const GA_Storage storage = GA_STORE_INVALID,
//     const UT_StringHolder& attribName = "",
//     const int tuple_size = 3,
//     const GA_Defaults& defaults = GA_Defaults(0.0f),
//     const bool emplaceBack = true,
//     const UT_Options* create_args = nullptr,
//     const GA_AttributeOptions* attribute_options = nullptr,
//     const GA_StorageClass storageClass = GA_STORECLASS_FLOAT
// )
// {
//     const GA_Storage finalStorage = getPreferredStorage(storage, storageClass);
//
//     GA_Attribute* attribPtr = GFE_Attribute::findAttributePointVertex(owner, attribName);
//     if (attribPtr)
//     {
//         const GA_AIFTuple* const aifTuple = attribPtr->getAIFTuple();
//         if (!aifTuple ||
//             attribPtr->getTupleSize() != tuple_size ||
//             aifTuple->getStorage(attribPtr) != finalStorage ||
//             aifTuple->getDefaults(attribPtr) != defaults)
//         {
//             getAttributes().destroyAttribute(attribPtr);
//             attribPtr = nullptr;
//         }
//         else
//         {
//             if (emplaceBack)
//                 attribArray.emplace_back(attribPtr);
//             return attribPtr;
//         }
//     }
//
//     const GA_AttributeOwner validOwner = owner == GA_ATTRIB_POINT ? GA_ATTRIB_POINT : GA_ATTRIB_VERTEX;
//     if (detached)
//     {
//         attribUPtrArray.emplace_back(createDetachedTupleAttribute(validOwner, finalStorage, 1));
//         attribPtr = attribUPtrArray[attribUPtrArray.size() - 1].get();
//         //attribPtr = attribUPtr.get();
//     }
//     else
//     {
//         if (!attribPtr)
//         {
// #if 1
//             const UT_Options& finalCreateArgs = create_args ? *create_args : UT_Options("uvw");
//
//             //GA_AttributeOptions& finalOptions = attribute_options ? *attribute_options : GA_AttributeOptions();
//             if (attribute_options)
//             {
//                 attribPtr = createTupleAttribute(validOwner, attribName, finalStorage,
//                     tuple_size, defaults, &finalCreateArgs, attribute_options);
//             }
//             else
//             {
//                 GA_AttributeOptions finalOptions = GA_AttributeOptions();
//                 finalOptions.setTypeInfo(GA_TYPE_TEXTURE_COORD);
//                 attribPtr = createTupleAttribute(validOwner, attribName, finalStorage,
//                     tuple_size, defaults, &finalCreateArgs, &finalOptions);
//             }
// #else
//             attribPtr = static_cast<GEO_Detail*>(geo)->addTextureAttribute(owner == GA_ATTRIB_POINT ? GA_ATTRIB_POINT : GA_ATTRIB_VERTEX, finalStorage);
// #endif
//         }
//
//         if (!attribPtr)
//         {
//             UT_ASSERT_MSG(attribPtr, "No Attrib");
//             return nullptr;
//         }
//         GFE_Attribute::renameAttribute(*attribPtr, attribName);
//     }
//     if (emplaceBack)
//         attribArray.emplace_back(attribPtr);
//     return attribPtr;
// }
//
// GA_Attribute*
// findOrCreateDir(
//     const bool detached = false,
//     const GA_AttributeOwner owner = GA_ATTRIB_POINT,
//     const GA_Storage storage = GA_STORE_INVALID,
//     const UT_StringHolder& attribName = "",
//     const int tuple_size = 3,
//     const GA_Defaults& defaults = GA_Defaults(0.0f),
//     const bool emplaceBack = true,
//     const UT_Options* create_args = nullptr,
//     const GA_AttributeOptions* attribute_options = nullptr,
//     const GA_StorageClass storageClass = GA_STORECLASS_FLOAT
// )
// {
//     const GA_Storage finalStorage = getPreferredStorage(storage, storageClass);
//
//     GA_Attribute* attribPtr = findAttribute(owner, attribName);
//     if (attribPtr)
//     {
//         const GA_AIFTuple* const aifTuple = attribPtr->getAIFTuple();
//         if (!aifTuple ||
//             attribPtr->getTupleSize() != tuple_size ||
//             aifTuple->getStorage(attribPtr) != finalStorage ||
//             aifTuple->getDefaults(attribPtr) != defaults)
//         {
//             getAttributes().destroyAttribute(attribPtr);
//             attribPtr = nullptr;
//         }
//         else
//         {
//             if (emplaceBack)
//                 attribArray.emplace_back(attribPtr);
//             return attribPtr;
//         }
//     }
//
//     if (detached)
//     {
//         attribUPtrArray.emplace_back(createDetachedTupleAttribute(owner, finalStorage, 1));
//         attribPtr = attribUPtrArray[attribUPtrArray.size() - 1].get();
//         //attribPtr = attribUPtr.get();
//     }
//     else
//     {
//         if (!attribPtr)
//         {
// #if 1
//             const UT_Options& finalCreateArgs = create_args ? *create_args : UT_Options("vector3");
//
//             //GA_AttributeOptions& finalOptions = attribute_options ? *attribute_options : GA_AttributeOptions();
//             if (attribute_options)
//             {
//                 attribPtr = createTupleAttribute(owner, attribName, finalStorage,
//                     tuple_size, defaults, &finalCreateArgs, attribute_options);
//             }
//             else
//             {
//                 GA_AttributeOptions finalOptions = GA_AttributeOptions();
//                 finalOptions.setTypeInfo(GA_TYPE_NORMAL);
//                 attribPtr = createTupleAttribute(owner, attribName, finalStorage,
//                     tuple_size, defaults, &finalCreateArgs, &finalOptions);
//             }
// #else
//             attribPtr = static_cast<GEO_Detail*>(geo)->addTextureAttribute(owner == GA_ATTRIB_POINT ? GA_ATTRIB_POINT : GA_ATTRIB_VERTEX, finalStorage);
// #endif
//         }
//
//         if (!attribPtr)
//         {
//             if (cookparms)
//                 cookparms->sopAddError(SOP_ATTRIBUTE_INVALID, attribName);
//             UT_ASSERT_MSG(attribPtr, "No Attrib");
//             return nullptr;
//         }
//         GFE_Attribute::renameAttribute(*attribPtr, attribName);
//     }
//     if (emplaceBack)
//         attribArray.emplace_back(attribPtr);
//     return attribPtr;
// }
//
//
// GA_Attribute*
// findOrCreateNormal3D(
//     const bool detached = false,
//     const GFE_NormalSearchOrder owner = GFE_NormalSearchOrder::ALL,
//     const GA_Storage storage = GA_STORE_INVALID,
//     const UT_StringHolder& attribName = "",
//     const int tuple_size = 3,
//     const GA_Defaults& defaults = GA_Defaults(0.0f),
//     const bool emplaceBack = true,
//     const UT_Options* create_args = nullptr,
//     const GA_AttributeOptions* attribute_options = nullptr,
//     const GA_StorageClass storageClass = GA_STORECLASS_FLOAT
// )
// {
//     const GA_Storage finalStorage = getPreferredStorage(storage, storageClass);
//
//
//     GA_Attribute* attribPtr = GFE_Attribute::findNormal3D(owner, attribName);
//     if (attribPtr)
//     {
//         const GA_AIFTuple* const aifTuple = attribPtr->getAIFTuple();
//         if (!aifTuple ||
//             attribPtr->getTupleSize() != tuple_size ||
//             aifTuple->getStorage(attribPtr) != finalStorage ||
//             aifTuple->getDefaults(attribPtr) != defaults)
//         {
//             getAttributes().destroyAttribute(attribPtr);
//             attribPtr = nullptr;
//         }
//         else
//         {
//             if (emplaceBack)
//                 attribArray.emplace_back(attribPtr);
//             return attribPtr;
//         }
//     }
//
//     const GA_AttributeOwner validOwner = GFE_Attribute::toValidOwner(owner);
//     if (detached)
//     {
//         attribUPtrArray.emplace_back(createDetachedTupleAttribute(validOwner, finalStorage, 1));
//         attribPtr = attribUPtrArray[attribUPtrArray.size() - 1].get();
//         //attribPtr = attribUPtr.get();
//     }
//     else
//     {
//         if (!attribPtr)
//         {
// #if 1
//             const UT_Options& finalCreateArgs = create_args ? *create_args : UT_Options("vector3");
//
//             //GA_AttributeOptions& finalOptions = attribute_options ? *attribute_options : GA_AttributeOptions();
//             if (attribute_options)
//             {
//                 attribPtr = createTupleAttribute(validOwner, attribName, finalStorage,
//                     tuple_size, defaults, &finalCreateArgs, attribute_options);
//             }
//             else
//             {
//                 GA_AttributeOptions finalOptions = GA_AttributeOptions();
//                 finalOptions.setTypeInfo(GA_TYPE_NORMAL);
//                 attribPtr = createTupleAttribute(validOwner, attribName, finalStorage,
//                     tuple_size, defaults, &finalCreateArgs, &finalOptions);
//             }
// #else
//             attribPtr = static_cast<GEO_Detail*>(geo)->addTextureAttribute(owner == GA_ATTRIB_POINT ? GA_ATTRIB_POINT : GA_ATTRIB_VERTEX, finalStorage);
// #endif
//         }
//
//         if (!attribPtr)
//         {
//             UT_ASSERT_MSG(attribPtr, "No Attrib");
//             return nullptr;
//         }
//         GFE_Attribute::renameAttribute(*attribPtr, attribName);
//     }
//     if (emplaceBack)
//         attribArray.emplace_back(attribPtr);
//     return attribPtr;
// }


GA_Attribute*
    addAttrib_hasGroup(
        const GA_GroupType groupType,
        const UT_StringHolder& groupName,
        const bool detached = true,
        const UT_StringHolder& attribName = "",
        const GA_Storage storage = GA_STORE_INVALID,
        const GA_StorageClass storageClass = GA_STORECLASS_FLOAT
    )
{
    return NULL;
}

GA_Attribute*
    addAttrib_hasGroup(
        const GA_GroupType groupType,
        const UT_StringHolder& groupName,
        const UT_StringHolder& attribName = "",
        const GA_Storage storage = GA_STORE_INVALID,
        const GA_StorageClass storageClass = GA_STORECLASS_FLOAT
    )
{
    const GA_Storage finalStorage = getPreferredStorage(storage, storageClass);
    const bool hasGroup = bool(findGroup(groupType, groupName));
    
    GA_Attribute* attribPtr = findGlobalAttribute(attribName);
    if(!attribPtr)
        attribPtr = createTupleAttribute(GA_ATTRIB_GLOBAL, attribName, finalStorage, 1, GA_Defaults(0));
    
    attribPtr->getAIFTuple()->set(attribPtr, 0, int(hasGroup));

    return attribPtr;
}

GA_AttributeUPtr
    addDetachedAttrib_hasGroup(
        const GA_GroupType groupType,
        const UT_StringHolder& groupName,
        const GA_Storage storage = GA_STORE_INVALID,
        const GA_StorageClass storageClass = GA_STORECLASS_FLOAT
    )
{
    const GA_Storage finalStorage = getPreferredStorage(storage, storageClass);
    const bool hasGroup = bool(findGroup(groupType, groupName));
    
    GA_AttributeUPtr attribUPtr = getAttributes().createDetachedTupleAttribute(GA_ATTRIB_GLOBAL, finalStorage, 1);
    GA_Attribute* const attribPtr = attribUPtr.get();
    
    attribPtr->getAIFTuple()->set(attribPtr, 0, int(hasGroup));
    
    return attribUPtr;
}


    


}; // End of Class GFE_Detail
















/*


namespace GFE_Detail_Namespace {



SYS_FORCE_INLINE
    static GA_Offset
    getPrimitivePointOffset(
        const GA_Detail& geo,
        const GA_Size primoff,
        const GA_Size vtxpnum
    )
{
    return geo.vertexPoint(geo.getPrimitiveVertexOffset(primoff, vtxpnum));
}


SYS_FORCE_INLINE
static UT_Vector3
getPrimitivePointPos3(
    const GA_Detail& geo,
    const GA_Size primoff,
    const GA_Size vtxpnum
)
{
    return geo.getPos3(getPrimitivePointOffset(geo, primoff, vtxpnum));
}

#if SYS_VERSION_MAJOR_INT > 19 || ( SYS_VERSION_MAJOR_INT == 19 && SYS_VERSION_MINOR_INT == 5 )

SYS_FORCE_INLINE
static UT_Vector3D
getPrimitivePointPos3D(
    const GA_Detail& geo,
    const GA_Size primoff,
    const GA_Size vtxpnum
)
{
    return geo.getPos3D(getPrimitivePointOffset(geo, primoff, vtxpnum));

}

template<typename T>
SYS_FORCE_INLINE
static UT_Vector3T<T>
getPrimitivePointPos3T(
    const GA_Detail& geo,
    const GA_Size primoff,
    const GA_Size vtxpnum
)
{
    return geo.getPos3T<T>(getPrimitivePointOffset(geo, primoff, vtxpnum));
}

#endif


SYS_FORCE_INLINE
static GA_Size
    numelems(
        const GA_Detail& geo,
        GA_AttributeOwner attribClass
    )
{
    UT_ASSERT_P(geo);
    switch (attribClass)
    {
    case GA_ATTRIB_PRIMITIVE:
        return geo.getNumPrimitives();
        break;
    case GA_ATTRIB_POINT:
        return geo.getNumPoints();
        break;
    case GA_ATTRIB_VERTEX:
        return geo.getNumVertices();
        break;
    default:
        return -1;
        break;
    }
    return -1;
}

SYS_FORCE_INLINE
static GA_Size
getNumElements(
    const GA_Detail& geo,
    GA_GroupType groupType
)
{
    //return numelems(geo, GFE_Type::attributeOwner_groupType(groupType));
    UT_ASSERT_P(geo);
    switch (groupType)
    {
    case GA_GROUP_PRIMITIVE:
        return geo.getNumPrimitives();
        break;
    case GA_GROUP_POINT:
        return geo.getNumPoints();
        break;
    case GA_GROUP_VERTEX:
        return geo.getNumVertices();
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
    GA_Detail& geo
)
{
    //geo.clear();
    geo.clearTopologyAttributes();
    geo.getIndexMap(GA_ATTRIB_PRIMITIVE).clear(true);
    geo.getIndexMap(GA_ATTRIB_POINT).clear(true);
    geo.getIndexMap(GA_ATTRIB_VERTEX).clear(true);
    geo.getPrimitiveList().clear(true);
    geo.createTopologyAttributes();
}














//GA_OffsetList offList = GFE_Detail::getOffsetList(geo.getPrimitiveMap(), group);
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
    const GA_Detail& geo,
    const GA_AttributeOwner owner,
    const GA_ElementGroup* const group = nullptr,
    const bool reverse = false
)
{
    return getOffsetList(geo.getIndexMap(owner), group, reverse);
}

//GA_OffsetList offList = GFE_Detail::getOffsetList(geo, group);
SYS_FORCE_INLINE
static GA_OffsetList
getOffsetList(
    const GA_Detail& geo,
    const GA_PrimitiveGroup* const group,
    const bool reverse = false
)
{
    return getOffsetList(geo.getPrimitiveMap(), group, reverse);
}

//GA_OffsetList offList = GFE_Detail::getOffsetList(geo, group);
SYS_FORCE_INLINE
static GA_OffsetList
getOffsetList(
    const GA_Detail& geo,
    const GA_PointGroup* const group,
    const bool reverse = false
)
{
    return getOffsetList(geo.getPointMap(), group, reverse);
}

//GA_OffsetList offList = GFE_Detail::getOffsetList(geo, group);
SYS_FORCE_INLINE
static GA_OffsetList
getOffsetList(
    const GA_Detail& geo,
    const GA_VertexGroup* const group,
    const bool reverse = false
)
{
    return getOffsetList(geo.getVertexMap(), group, reverse);
}




} // End of namespace GFE_Detail_Namespace

*/



#endif
