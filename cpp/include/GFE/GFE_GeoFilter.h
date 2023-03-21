
#pragma once

#ifndef __GFE_GeoFilter_h__
#define __GFE_GeoFilter_h__

#include "GA/GA_Detail.h"

//#include "GFE/GFE_GeoFilter.h"

#include "GFE/GFE_AttributeArray.h"
#include "GFE/GFE_Attribute.h"
#include "GFE/GFE_GroupParser.h"
#include "GFE/GFE_TopologyReference.h"
#include "GFE/GFE_GroupPromote.h"




class GFE_GeoFilter {

public:

    GFE_GeoFilter(
        GA_Detail* const geo,
        const SOP_NodeVerb::CookParms* const cookparms = nullptr
    )
        : geo(geo)
        , cookparms(cookparms)
        , groupParser(geo, gop, cookparms)
    {
        UT_ASSERT_MSG(geo, "do not find geo");
    }

    GFE_GeoFilter(
        const SOP_NodeVerb::CookParms& cookparms,
        GA_Detail* const geo
    )
        : geo(geo)
        , cookparms(&cookparms)
        , groupParser(cookparms, geo, gop)
    {
        UT_ASSERT_MSG(geo, "do not find geo");
    }

    ~GFE_GeoFilter()
    {
        GFE_TopologyReference::outTopoAttrib(geo, outTopoAttrib);
    }

    virtual void
        reset(
            GA_Detail* const geo,
            const SOP_NodeVerb::CookParms* const cookparms = nullptr
        )
    {
        this->geo = geo;
        this->cookparms = cookparms;
        groupParser.reset(geo, cookparms);
        setHasComputed();
    }


    SYS_FORCE_INLINE
    bool
        getHasComputed() const
    {
        return hasComputed;
    }


    SYS_FORCE_INLINE
    void
        setOutTopoAttrib(
            bool outTopoAttrib = true
        )
    {
        this->outTopoAttrib = outTopoAttrib;
    }

    SYS_FORCE_INLINE
    bool
        getOutTopoAttrib() const
    {
        return outTopoAttrib;
    }


    GFE_GroupParser&
        getGroupParser()
    {
        return groupParser;
    }
    

    SYS_FORCE_INLINE
    void
        compute()
    {
        hasComputed = computeCore();
    }


    SYS_FORCE_INLINE
    void
        bumpDataIdsForAddOrRemove(
            const bool added_or_removed_points,
            const bool added_or_removed_vertices,
            const bool added_or_removed_primitives
        )
    {
        geo->bumpDataIdsForAddOrRemove(added_or_removed_points, added_or_removed_vertices, added_or_removed_primitives);
    }

    virtual void
        bumpDataIdsForAddOrRemove()
    {
        geo->bumpDataIdsForAddOrRemove(true, true, true);
    }

    SYS_FORCE_INLINE
    void
        computeAndBumpDataIdsForAddOrRemove(
            const bool added_or_removed_points,
            const bool added_or_removed_vertices,
            const bool added_or_removed_primitives
        )
    {
        compute();
        bumpDataIdsForAddOrRemove(added_or_removed_points, added_or_removed_vertices, added_or_removed_primitives);
    }

    SYS_FORCE_INLINE
    void
        computeAndBumpDataIdsForAddOrRemove()
    {
        compute();
        bumpDataIdsForAddOrRemove();
    }

protected:
    SYS_FORCE_INLINE
    void
        setHasComputed(
            const bool hasComputed = false
        )
    {
        this->hasComputed = hasComputed;
    }

private:
    virtual bool
        computeCore()
    {
        if (groupParser.getGroup() && groupParser.getGroup()->isEmpty())
            return false;
        return true;
    }


public:
    GFE_GroupParser groupParser;

protected:
    const SOP_NodeVerb::CookParms* cookparms;
    GA_Detail* geo;

private:
    GOP_Manager gop;
    bool outTopoAttrib = false;
    bool hasComputed = false;
};








class GFE_GeoFilterRef {

public:

    GFE_GeoFilterRef(
        const GA_Detail* const geoRef0,
        GOP_Manager& gop,
        const SOP_NodeVerb::CookParms* const cookparms = nullptr
    )
        : geoRef0(geoRef0)
        , cookparms(cookparms)
        , gop(gop)
        , groupParserRef0(geoRef0, gop, cookparms)
    {
        UT_ASSERT_MSG(geoRef0, "do not find geo");
    }

    GFE_GeoFilterRef(
        const SOP_NodeVerb::CookParms& cookparms,
        const GA_Detail* const geoRef0,
        GOP_Manager& gop
    )
        : geoRef0(geoRef0)
        , cookparms(&cookparms)
        , gop(gop)
        , groupParserRef0(cookparms, geoRef0, gop)
    {
        UT_ASSERT_MSG(geoRef0, "do not find geo");
    }

    ~GFE_GeoFilterRef()
    {
    }

    

    void
        resetGeoFilterRef(
            const GA_Detail* const geoRef0,
            const SOP_NodeVerb::CookParms* const cookparms = nullptr
        )
    {
        this->geoRef0 = geoRef0;
        this->cookparms = cookparms;
    }

    GFE_GroupParser&
        getGroupParser()
    {
        return groupParserRef0;
    }


public:
    GFE_GroupParser groupParserRef0;

protected:
    const GA_Detail* geoRef0;

private:
    const SOP_NodeVerb::CookParms* cookparms;
    GOP_Manager& gop;
};



class GFE_AttribFilter : public GFE_GeoFilter {

public:

    //using GFE_GeoFilter::GFE_GeoFilter;

    GFE_AttribFilter(
        GA_Detail* const geo,
        const SOP_NodeVerb::CookParms* const cookparms = nullptr
    )
        : GFE_GeoFilter(geo, cookparms)
        , outAttribArray(geo, cookparms)
        , outGroupArray(geo, cookparms)
    {
    }

    GFE_AttribFilter(
        const SOP_NodeVerb::CookParms& cookparms,
        GA_Detail* const geo
    )
        : GFE_GeoFilter(cookparms, geo)
        , outAttribArray(geo, &cookparms)
        , outGroupArray(geo, &cookparms)
    {
    }

    virtual void
        reset(
            GA_Detail* const geo,
            const SOP_NodeVerb::CookParms* const cookparms = nullptr
        ) override
    {
        GFE_GeoFilter::reset(geo, cookparms);
        outAttribArray.reset(geo, cookparms);
        outGroupArray.reset(geo, cookparms);
    }

    //void
    //    setOutAttrib(
    //        GA_Attribute* attribPtr
    //    )
    //{
    //    attribArray.clear();
    //    if (!attribPtr)
    //        return;

    //    attribArray.emplace_back(attribPtr);
    //}

    //void
    //    setOutGroup(
    //        GA_Group* groupPtr
    //    )
    //{
    //    groupArray.clear();
    //    if (!groupPtr)
    //        return;

    //    groupArray.emplace_back(groupPtr);
    //}

    //virtual void
    //    setOutAttrib(
    //        const GA_AttributeOwner attribClass,
    //        const UT_StringHolder& attribPattern
    //    )
    //{
    //    if (!attribPattern.isstring() || attribPattern.length() == 0)
    //        return;

    //    GA_Attribute* attribPtr = geo->findAttribute(attribClass, attribPattern);
    //    setOutAttrib(attribPtr);
    //}

    //virtual void
    //    setOutGroup(
    //        const GA_GroupType groupClass,
    //        const UT_StringHolder& groupPattern
    //    )
    //{
    //    if (!groupPattern.isstring() || groupPattern.length() == 0)
    //        return;

    //    GA_Group* groupPtr = geo->getGroupTable(groupClass)->find(groupPattern);
    //    setOutGroup(groupPtr);
    //}

    //void
    //    addOutAttrib(
    //        GA_Attribute* attribPtr
    //    )
    //{
    //    if (!attribPtr)
    //        return;
    //    attribArray.emplace_back(attribPtr);
    //}
    //void
    //    addOutGroup(
    //        GA_Group* groupPtr
    //    )
    //{
    //    if (!groupPtr)
    //        return;
    //    groupArray.emplace_back(groupPtr);
    //}


    //virtual void
    //    addOutAttrib(
    //        const GA_AttributeOwner attribClass,
    //        const UT_StringHolder& attribPattern
    //    )
    //{
    //    if (!attribPattern.isstring() || attribPattern.length() == 0)
    //        return;

    //    GA_Attribute* attribPtr = geo->findAttribute(attribClass, attribPattern);
    //    addOutAttrib(attribPtr);
    //}

    //virtual void
    //    addOutGroup(
    //        const GA_GroupType groupClass,
    //        const UT_StringHolder& groupPattern
    //    )
    //{
    //    if (!groupPattern.isstring() || groupPattern.length() == 0)
    //        return;

    //    GA_Group* groupPtr = geo->getGroupTable(groupClass)->find(groupPattern);
    //    addOutGroup(groupPtr);
    //}



    //virtual void
    //    addOutAttribs(
    //        const GA_AttributeOwner attribClass,
    //        const UT_StringHolder& attribPattern
    //    )
    //{
    //    if (!attribPattern.isstring() || attribPattern.length() == 0)
    //        return;

    //    GA_Attribute* attribPtr = nullptr;
    //    for (GA_AttributeDict::iterator it = geo->getAttributes().begin(attribClass); !it.atEnd(); ++it)
    //    {
    //        attribPtr = it.attrib();
    //        if (!attribPtr->getName().multiMatch(attribPattern))
    //            continue;
    //        attribArray.emplace_back(attribPtr);
    //    }
    //}

    //virtual void
    //    addOutGroups(
    //        const GA_GroupType groupClass,
    //        const UT_StringHolder& groupPattern
    //    )
    //{
    //    if (!groupPattern.isstring() || groupPattern.length() == 0)
    //        return;

    //    GA_Group* groupPtr = nullptr;
    //    for (GA_GroupTable::iterator<GA_Group> it = geo->getGroupTable(groupClass)->beginTraverse(); !it.atEnd(); ++it)
    //    {
    //        groupPtr = it.group();
    //        if (!groupPtr->getName().multiMatch(groupPattern))
    //            continue;
    //        groupArray.emplace_back(groupPtr);
    //    }
    //}

    //virtual void
    //    findOrCreateOutAttrib(
    //        const GA_AttributeOwner owner = GA_ATTRIB_POINT,
    //        const GA_StorageClass storageClass = GA_STORECLASS_FLOAT,
    //        const GA_Storage storage = GA_STORE_INVALID,
    //        const bool detached = false,
    //        const UT_StringHolder& attribName = "__topo_"
    //    )
    //{
    //    GA_Attribute* attribPtr = nullptr;

    //    const GA_Storage finalStorage = GFE_Type::getPreferredStorage(geo, storage, storageClass);

    //    if (detached)
    //    {
    //        //const GA_AttributeUPtr& attribUPtr = geo->createDetachedTupleAttribute(owner, finalStorage, 1);
    //        attribUPtrArray.emplace_back(geo->createDetachedTupleAttribute(owner, finalStorage, 1));
    //        attribPtr = attribUPtrArray[attribUPtrArray.size()-1].get();
    //        //attribPtr = attribUPtr.get();
    //    }
    //    else
    //    {
    //        attribPtr = geo->findAttribute(owner, attribName);

    //        if (attribPtr && attribPtr->getAIFTuple()->getStorage(attribPtr) != finalStorage)
    //            geo->getAttributes().destroyAttribute(attribPtr);

    //        if (!attribPtr)
    //            attribPtr = geo->createTupleAttribute(owner, attribName, finalStorage, 1);

    //        if (!attribPtr)
    //        {
    //            if (cookparms)
    //                cookparms->sopAddError(SOP_ATTRIBUTE_INVALID, attribName);
    //            UT_ASSERT_MSG(attribPtr, "No Attrib");
    //            return;
    //        }
    //    }
    //    attribArray.emplace_back(attribPtr);
    //}


    //virtual void
    //    findOrCreateOutGroup(
    //        const GA_GroupType groupType = GA_GROUP_POINT,
    //        const bool detached = false,
    //        const UT_StringHolder& groupName = "__topo_"
    //    )
    //{
    //    GA_Group* groupPtr = nullptr;

    //    if (detached)
    //    {
    //        switch (groupType)
    //        {
    //        case GA_GROUP_PRIMITIVE:
    //            groupUPtrArray.emplace_back(geo->createDetachedPrimitiveGroup());
    //            break;
    //        case GA_GROUP_POINT:
    //            groupUPtrArray.emplace_back(geo->createDetachedPointGroup());
    //            break;
    //        case GA_GROUP_VERTEX:
    //            groupUPtrArray.emplace_back(geo->createDetachedVertexGroup());
    //            break;
    //        case GA_GROUP_EDGE:
    //            groupUPtrArray.emplace_back(geo->createDetachedEdgeGroup());
    //            break;
    //        default:
    //            if (cookparms)
    //                cookparms->sopAddError(SOP_ERR_BADGROUP, groupName);
    //            UT_ASSERT_MSG(groupPtr, "No Group");
    //            return;
    //            break;
    //        }
    //        groupPtr = groupUPtrArray[groupUPtrArray.size() - 1].get();
    //        //groupPtr = attribUPtr.get();
    //    }
    //    else
    //    {
    //        groupPtr = geo->getGroupTable(groupType)->find(groupName);

    //        if (!groupPtr)
    //            groupPtr = geo->getGroupTable(groupType)->newGroup(groupName);

    //        if (!groupPtr)
    //        {
    //            if (cookparms)
    //                cookparms->sopAddError(SOP_ERR_BADGROUP, groupName);
    //            UT_ASSERT_MSG(groupPtr, "No Group");
    //            return;
    //        }
    //    }
    //    groupArray.emplace_back(groupPtr);
    //}

    //const std::vector<GA_Attribute*>&
    //    getAttrib() const
    //{
    //    return attribArray;
    //}

    //std::vector<GA_Attribute*>&
    //    getAttrib()
    //{
    //    return attribArray;
    //}

    //const std::vector<GA_Group*>&
    //    getGroup() const
    //{
    //    return groupArray;
    //}

    //std::vector<GA_Group*>&
    //    getGroup()
    //{
    //    return groupArray;
    //}

    //void
    //    bumpDataId() const
    //{
    //    for (int i = 0; i < attribArray.size(); i++)
    //    {
    //        if (attribArray[i]->isDetached())
    //            continue;
    //        attribArray[i]->bumpDataId();
    //    }
    //    for (int i = 0; i < groupArray.size(); i++)
    //    {
    //        if (groupArray[i]->isDetached())
    //            continue;
    //        if(groupArray[i]->classType() == GA_GROUP_EDGE)
    //            static_cast<GA_EdgeGroup*>(groupArray[i])->bumpDataId();
    //        else
    //            static_cast<GA_ElementGroup*>(groupArray[i])->bumpDataId();
    //    }
    //}

    virtual void
        bumpDataId()
    {
        outAttribArray.bumpDataId();
        outGroupArray.bumpDataId();
    }

    SYS_FORCE_INLINE
    void
        computeAndBumpDataId()
    {
        compute();
        bumpDataId();
    }

    virtual void
        visualizeOutGroup()
    {
        if (!cookparms || !outGroupArray[0])
            return;

        cookparms->getNode()->setHighlight(true);
        cookparms->select(*outGroupArray[0]);
    }

    






    SYS_FORCE_INLINE
    GFE_AttributeArray&
        getOutAttribArray()
    {
        return outAttribArray;
    }

    SYS_FORCE_INLINE
    GFE_GroupArray&
        getOutGroupArray()
    {
        return outGroupArray;
    }

    SYS_FORCE_INLINE
    std::vector<GA_Attribute*>&
        getOutAttribArrayRef()
    {
        return outAttribArray.ref();
    }

    SYS_FORCE_INLINE
    std::vector<GA_Group*>&
        getOutGroupArrayRef()
    {
        return outGroupArray.ref();
    }

private:
    //std::vector<GA_AttributeUPtr> attribUPtrArray;
    //std::vector<GA_Attribute*> attribArray;
    //std::vector<GA_GroupUPtr> groupUPtrArray;
    //std::vector<GA_Group*> groupArray;
    GFE_AttributeArray outAttribArray;
    GFE_GroupArray outGroupArray;
};




//class GFE_GeoFilter_OutTopoAttrib {
//
//public:
//    GFE_GeoFilter_OutTopoAttrib(GA_Detail* geo)
//        : geo(geo)
//    {
//    }
//
//    ~GFE_GeoFilter_OutTopoAttrib()
//    {
//        GFE_TopologyReference::outTopoAttrib(geo, outTopoAttrib);
//    }
//
//protected:
//    bool outTopoAttrib = false;
//
//private:
//    GA_Detail* geo;
//};

class GFE_AttribCreateFilter : public GFE_AttribFilter {

public:

    GFE_AttribCreateFilter(
        GA_Detail* const geo,
        const SOP_NodeVerb::CookParms* const cookparms = nullptr
    )
        : GFE_AttribFilter(geo, cookparms)
        , inAttribArray(geo, cookparms)
        , inGroupArray(geo, cookparms)
    {
    }

    GFE_AttribCreateFilter(
        const SOP_NodeVerb::CookParms& cookparms,
        GA_Detail* const geo
    )
        : GFE_AttribFilter(cookparms, geo)
        , inAttribArray(cookparms, geo)
        , inGroupArray(cookparms, geo)
    {
    }

    ~GFE_AttribCreateFilter()
    {
    }




    virtual void
        reset(
            GA_Detail* const geo,
            const SOP_NodeVerb::CookParms* const cookparms = nullptr
        ) override
    {
        GFE_AttribFilter::reset(geo, cookparms);
        inAttribArray.reset(geo, cookparms);
        inGroupArray.reset(geo, cookparms);
    }

    SYS_FORCE_INLINE
    GFE_AttributeArray&
        getInAttribArray()
    {
        return inAttribArray;
    }

    SYS_FORCE_INLINE
    GFE_GroupArray&
        getInGroupArray()
    {
        return inGroupArray;
    }

    SYS_FORCE_INLINE
    std::vector<GA_Attribute*>&
        getInAttribArrayRef()
    {
        return inAttribArray.ref();
    }

    SYS_FORCE_INLINE
    std::vector<GA_Group*>&
        getInGroupArrayRef()
    {
        return inGroupArray.ref();
    }


    SYS_FORCE_INLINE
    void
        setInAttribBumpDataId(
            bool val = true
        )
    {
        inAttribBumpDataId = val;
    }

    SYS_FORCE_INLINE
    void
        setInGroupBumpDataId(
            bool val = true
        )
    {
        inGroupBumpDataId = val;
    }

    virtual void
        bumpDataId() override
    {
        GFE_AttribFilter::bumpDataId();
        if (inAttribBumpDataId)
            inAttribArray.bumpDataId();
        if (inGroupBumpDataId)
            inGroupArray.bumpDataId();
    }

    void
        visualizeInGroup()
    {
        if (!cookparms || !inGroupArray[0])
            return;

        cookparms->getNode()->setHighlight(true);
        cookparms->select(*inGroupArray[0]);
    }

    

private:
    bool inAttribBumpDataId = false;
    bool inGroupBumpDataId = false;
    GFE_AttributeArray inAttribArray;
    GFE_GroupArray inGroupArray;
};


//class GFE_UVFilter : public GFE_AttribFilter {
//
//public:
//
//    using GFE_AttribFilter::GFE_AttribFilter;

    //GFE_UVFilter(
    //    GA_Detail* const geo,
    //    const SOP_NodeVerb::CookParms* const cookparms = nullptr
    //)
    //    : GFE_AttribFilter(geo, cookparms)
    //{
    //    UT_ASSERT_MSG(geo, "do not find geo");
    //}

    //GFE_UVFilter(
    //    const SOP_NodeVerb::CookParms& cookparms,
    //    GA_Detail* const geo
    //)
    //    : GFE_AttribFilter(cookparms, geo)
    //{
    //    UT_ASSERT_MSG(geo, "do not find geo");
    //}








    //void
    //    setInGroup(
    //        const GA_PrimitiveGroup* const geoPrimGroup = nullptr
    //    )
    //{
    //    this->geoPrimGroup = geoPrimGroup;
    //}

    //virtual void
    //    setInGroup(
    //        const GA_Group* const geoGroup
    //    ) override
    //{
    //    hasParm_inGroup = true;
    //    geoPrimGroup = GFE_GroupPromote::groupFindPromotePrimitiveDetached(geo, geoGroup, groupParser.getGOP());
    //}

    ////using GFE_GeoFilter::setInGroup;
    //void
    //    setInGroup(
    //        const GA_GroupType groupType,
    //        const UT_StringHolder& groupName
    //    )
    //{
    //    GFE_GeoFilter::setInGroup(groupType, groupName);
    //}


    //void
    //    setOutAttrib(
    //        GA_Attribute* attribPtr
    //    )
    //{
    //    GFE_AttribFilter::setOutAttrib(attribPtr);
    //}

    //virtual void
    //    setOutAttrib(
    //        const GA_AttributeOwner attribClass,
    //        const UT_StringHolder& attribPattern
    //    ) override
    //{
    //    if (!attribPattern.isstring() || attribPattern.length() == 0)
    //        return;

    //    GA_Attribute* attribPtr = GFE_Attribute::findUVAttributePointVertex(geo, attribClass, attribPattern);
    //    setOutAttrib(attribPtr);
    //}

    //virtual void
    //    setOrCreateOutAttrib(
    //        const GA_AttributeOwner attribClass,
    //        const UT_StringHolder& attribPattern,
    //        const GA_Storage storage = GA_STORE_INVALID
    //    )
    //{
    //    if (!attribPattern.isstring() || attribPattern.length() == 0)
    //        return;

    //    GA_Attribute* attribPtr = GFE_Attribute::findOrCreateUVAttributePointVertex(geo, attribClass, attribPattern, storage);
    //    setOutAttrib(attribPtr);
    //}

//protected:
//    GA_PrimitiveGroupUPtr geoPrimGroupUPtr;
//    const GA_PrimitiveGroup* geoPrimGroup = nullptr;
// 
// 
//};










#endif
