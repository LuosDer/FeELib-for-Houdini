
#pragma once

#ifndef __GFE_AttributeArray_h__
#define __GFE_AttributeArray_h__

//#include "GFE/GFE_AttributeArray.h"

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

    ~GFE_AttributeArray()
    {
    }

    GA_Attribute*
        operator[](size_t i)
    {
        return attribArray[i];
    }

    GA_Attribute*
        operator[](int i)
    {
        return attribArray[i];
    }
    
    bool
        isEmpty()
    {
        return attribArray.size() == 0;
    }

    void
        clear()
    {
        attribArray.clear();
        attribUPtrArray.clear();
    }

    size_t
        size()
    {
        return attribArray.size();
    }

    void
        reset(
            GA_Detail* const geo,
            const SOP_NodeVerb::CookParms* const cookparms = nullptr
        )
    {
        this->geo = geo;
        this->cookparms = cookparms;
        clear();
    }

    
void
set(
    GA_Attribute* attribPtr
)
{
    clear();
    if (!attribPtr)
        return;

    attribArray.emplace_back(attribPtr);
}

void
set(
    const GA_AttributeOwner attribClass,
    const UT_StringHolder& attribPattern
)
{
    if (!attribPattern.isstring() || attribPattern.length() == 0)
        return;

    GA_Attribute* attribPtr = geo->findAttribute(attribClass, attribPattern);
    set(attribPtr);
}

void   
append(
    GA_Attribute* attribPtr
)
{
    if (!attribPtr)
        return;
    attribArray.emplace_back(attribPtr);
}

void
append(
    const GA_AttributeOwner attribClass,
    const UT_StringHolder& attribPattern
)
{
    if (!attribPattern.isstring() || attribPattern.length() == 0)
        return;

    GA_Attribute* attribPtr = geo->findAttribute(attribClass, attribPattern);
    append(attribPtr);
}

void
appendUV(
    const GA_AttributeOwner attribClass,
    const UT_StringHolder& attribPattern
)
{
    if (!attribPattern.isstring() || attribPattern.length() == 0)
        return;

    GA_Attribute* attribPtr = GFE_Attribute::findUVAttributePointVertex(geo, attribClass, attribPattern);
    //GA_Attribute* attribPtr = GFE_Attribute::findAttributePointVertex(geo, attribClass, attribPattern);
    //if (attribPtr)
    //{
    //    int tupleSize = attribPtr->getTupleSize();
    //    if (tupleSize < 2 || tupleSize > 4)
    //    {
    //        //geo->getAttributes().destroyAttribute(uvAttribPtr);
    //        attribPtr = nullptr;
    //    }
    //}
    append(attribPtr);
}




void
appends(
    const GA_AttributeOwner attribClass,
    const UT_StringHolder& attribPattern
)
{
    if (!attribPattern.isstring() || attribPattern.length() == 0)
        return;

    GA_Attribute* attribPtr = nullptr;
    for (GA_AttributeDict::iterator it = geo->getAttributes().begin(attribClass); !it.atEnd(); ++it)
    {
        attribPtr = it.attrib();
        if (!attribPtr->getName().multiMatch(attribPattern))
            continue;
        attribArray.emplace_back(attribPtr);
    }
}

void
findOrCreateTuple(
    const GA_AttributeOwner owner = GA_ATTRIB_POINT,
    const GA_StorageClass storageClass = GA_STORECLASS_FLOAT,
    const GA_Storage storage = GA_STORE_INVALID,
    const bool detached = false,
    const UT_StringHolder& attribName = "",
    const int tuple_size = 1,
    const GA_Defaults& defaults = GA_Defaults(0.0f),
    const bool emplaceBack = true,
    const UT_Options* create_args = nullptr,
    const GA_AttributeOptions* attribute_options = nullptr
)
{
    const GA_Storage finalStorage = GFE_Type::getPreferredStorage(geo, storage, storageClass);

    GA_Attribute* attribPtr = geo->findAttribute(owner, attribName);
    if (attribPtr)
    {
        const GA_AIFTuple* const aifTuple = attribPtr->getAIFTuple();
        if (attribPtr->getTupleSize() != tuple_size ||
            aifTuple->getStorage(attribPtr) != finalStorage ||
            aifTuple->getDefaults(attribPtr) != defaults)
        {
            geo->getAttributes().destroyAttribute(attribPtr);
            attribPtr = nullptr;
        }
        else
        {
            if (emplaceBack)
                attribArray.emplace_back(attribPtr);
            return;
        }
    }

    if (detached)
    {
        if (emplaceBack)
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
            return;
        }
    }
    if (emplaceBack)
        attribArray.emplace_back(attribPtr);
}

//SYS_FORCE_INLINE
//void
//findOrCreate(
//    const GA_AttributeOwner owner,
//    const GA_Storage storage,
//    const UT_StringHolder& attribName
//)
//{
//    findOrCreate(owner, storage, attribName);
//}

void
findOrCreateUV(
    const GA_AttributeOwner owner = GA_ATTRIB_POINT,
    const GA_Storage storage = GA_STORE_INVALID,
    const bool detached = false,
    const UT_StringHolder& attribName = "",
    const int tuple_size = 3,
    const GA_Defaults& defaults = GA_Defaults(0.0f),
    const bool emplaceBack = true,
    const UT_Options* create_args = nullptr,
    const GA_AttributeOptions* attribute_options = nullptr,
    const GA_StorageClass storageClass = GA_STORECLASS_FLOAT
)
{
    const GA_Storage finalStorage = GFE_Type::getPreferredStorage(geo, storage, storageClass);

    GA_Attribute* attribPtr = GFE_Attribute::findAttributePointVertex(geo, owner, attribName);
    if (attribPtr)
    {
        const GA_AIFTuple* const aifTuple = attribPtr->getAIFTuple();
        if (attribPtr->getTupleSize() != tuple_size ||
            aifTuple->getStorage(attribPtr) != finalStorage ||
            aifTuple->getDefaults(attribPtr) != defaults)
        {
            geo->getAttributes().destroyAttribute(attribPtr);
            attribPtr = nullptr;
        }
        else
        {
            if (emplaceBack)
                attribArray.emplace_back(attribPtr);
            return;
        }
    }

    const GA_AttributeOwner validOwner = owner == GA_ATTRIB_POINT ? GA_ATTRIB_POINT : GA_ATTRIB_VERTEX;
    if (detached)
    {
        if (emplaceBack)
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
            return;
        }
        GFE_Attribute::renameAttribute(attribPtr, attribName);
    }
    if (emplaceBack)
        attribArray.emplace_back(attribPtr);
}

void
findOrCreateDir(
    const GA_AttributeOwner owner = GA_ATTRIB_POINT,
    const GA_Storage storage = GA_STORE_INVALID,
    const bool detached = false,
    const UT_StringHolder& attribName = "",
    const int tuple_size = 3,
    const GA_Defaults& defaults = GA_Defaults(0.0f),
    const bool emplaceBack = true,
    const UT_Options* create_args = nullptr,
    const GA_AttributeOptions* attribute_options = nullptr,
    const GA_StorageClass storageClass = GA_STORECLASS_FLOAT
)
{
    const GA_Storage finalStorage = GFE_Type::getPreferredStorage(geo, storage, storageClass);

    GA_Attribute* attribPtr = geo->findAttribute(owner, attribName);
    if (attribPtr)
    {
        const GA_AIFTuple* const aifTuple = attribPtr->getAIFTuple();
        if (attribPtr->getTupleSize() != tuple_size ||
            aifTuple->getStorage(attribPtr) != finalStorage ||
            aifTuple->getDefaults(attribPtr) != defaults)
        {
            geo->getAttributes().destroyAttribute(attribPtr);
            attribPtr = nullptr;
        }
        else
        {
            if (emplaceBack)
                attribArray.emplace_back(attribPtr);
            return;
        }
    }

    if (detached)
    {
        if (emplaceBack)
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
            return;
        }
        GFE_Attribute::renameAttribute(attribPtr, attribName);
    }
    if (emplaceBack)
        attribArray.emplace_back(attribPtr);
}


void
findOrCreateNormal3D(
    const GFE_NormalSearchOrder owner = GFE_NormalSearchOrder::ALL,
    const GA_Storage storage = GA_STORE_INVALID,
    const bool detached = false,
    const UT_StringHolder& attribName = "",
    const int tuple_size = 3,
    const GA_Defaults& defaults = GA_Defaults(0.0f),
    const bool emplaceBack = true,
    const UT_Options* create_args = nullptr,
    const GA_AttributeOptions* attribute_options = nullptr,
    const GA_StorageClass storageClass = GA_STORECLASS_FLOAT
)
{
    const GA_Storage finalStorage = GFE_Type::getPreferredStorage(geo, storage, storageClass);


    GA_Attribute* attribPtr = GFE_Attribute::findNormal3D(geo, owner, attribName);
    if (attribPtr)
    {
        const GA_AIFTuple* const aifTuple = attribPtr->getAIFTuple();
        if (attribPtr->getTupleSize() != tuple_size ||
            aifTuple->getStorage(attribPtr) != finalStorage ||
            aifTuple->getDefaults(attribPtr) != defaults)
        {
            geo->getAttributes().destroyAttribute(attribPtr);
            attribPtr = nullptr;
        }
        else
        {
            if (emplaceBack)
                attribArray.emplace_back(attribPtr);
            return;
        }
    }

    const GA_AttributeOwner validOwner = GFE_Attribute::toValidOwner(owner);
    if (detached)
    {
        if (emplaceBack)
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
            return;
        }
        GFE_Attribute::renameAttribute(attribPtr, attribName);
    }
    if (emplaceBack)
        attribArray.emplace_back(attribPtr);
}





SYS_FORCE_INLINE
void
pop_back()
{
    attribArray.pop_back();
}




void
bumpDataId()
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

std::vector<GA_Attribute*>&
ref()
{
    return attribArray;
}


protected:
    GA_Detail* geo;
    const SOP_NodeVerb::CookParms* cookparms;

private:
    std::vector<GA_AttributeUPtr> attribUPtrArray;
    std::vector<GA_Attribute*> attribArray;


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

    ~GFE_GroupArray()
    {
    }

    GA_Group*
        operator[](size_t i)
    {
        return groupArray[i];
    }

    GA_Group*
        operator[](int i)
    {
        return groupArray[i];
    }

    bool
        isEmpty()
    {
        return groupArray.size() == 0;
    }

    void
        clear()
    {
        groupArray.clear();
        groupUPtrArray.clear();
    }

    size_t
        size()
    {
        return groupArray.size();
    }

    void
        reset(
            GA_Detail* const geo,
            const SOP_NodeVerb::CookParms* const cookparms = nullptr
        )
    {
        this->geo = geo;
        this->cookparms = cookparms;
        clear();
    }

    void
        set(
            GA_Group* groupPtr
        )
    {
        clear();
        if (!groupPtr)
            return;

        groupArray.emplace_back(groupPtr);
    }

    void
        set(
            const GA_GroupType groupClass,
            const UT_StringHolder& groupPattern
        )
    {
        if (!groupPattern.isstring() || groupPattern.length() == 0)
            return;

        GA_Group* groupPtr = geo->getGroupTable(groupClass)->find(groupPattern);
        set(groupPtr);
    }

    void
        append(
            GA_Group* groupPtr
        )
    {
        if (!groupPtr)
            return;
        groupArray.emplace_back(groupPtr);
    }

    void
        append(
            const GA_GroupType groupClass,
            const UT_StringHolder& groupPattern
        )
    {
        if (!groupPattern.isstring() || groupPattern.length() == 0)
            return;

        GA_Group* groupPtr = geo->getGroupTable(groupClass)->find(groupPattern);
        append(groupPtr);
    }

    void
        appends(
            const GA_GroupType groupClass,
            const UT_StringHolder& groupPattern
        )
    {
        if (!groupPattern.isstring() || groupPattern.length() == 0)
            return;

        GA_Group* groupPtr = nullptr;
        for (GA_GroupTable::iterator<GA_Group> it = geo->getGroupTable(groupClass)->beginTraverse(); !it.atEnd(); ++it)
        {
            groupPtr = it.group();
            if (!groupPtr->getName().multiMatch(groupPattern))
                continue;
            groupArray.emplace_back(groupPtr);
        }
    }


    void
        findOrCreate(
            const GA_GroupType groupType = GA_GROUP_POINT,
            const bool detached = false,
            const UT_StringHolder& groupName = ""
        )
    {
        GA_Group* groupPtr = nullptr;

        if (detached)
        {
            switch (groupType)
            {
            case GA_GROUP_PRIMITIVE:
                groupUPtrArray.emplace_back(geo->createDetachedPrimitiveGroup());
                break;
            case GA_GROUP_POINT:
                groupUPtrArray.emplace_back(geo->createDetachedPointGroup());
                break;
            case GA_GROUP_VERTEX:
                groupUPtrArray.emplace_back(geo->createDetachedVertexGroup());
                break;
            case GA_GROUP_EDGE:
                groupUPtrArray.emplace_back(geo->createDetachedEdgeGroup());
                break;
            default:
                if (cookparms)
                    cookparms->sopAddError(SOP_ERR_BADGROUP, groupName);
                UT_ASSERT_MSG(groupPtr, "No Group");
                return;
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
                return;
            }
        }
        groupArray.emplace_back(groupPtr);
    }





    void
        bumpDataId()
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

    std::vector<GA_Group*>&
        ref()
    {
        return groupArray;
    }


protected:
    GA_Detail* geo;
    const SOP_NodeVerb::CookParms* cookparms;

private:
    std::vector<GA_GroupUPtr> groupUPtrArray;
    std::vector<GA_Group*> groupArray;


}; // End of class GFE_GroupArray







#endif
