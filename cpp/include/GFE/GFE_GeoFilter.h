
#pragma once

#ifndef __GFE_GeoFilter_h__
#define __GFE_GeoFilter_h__

#include "GA/GA_Detail.h"

//#include "GFE/GFE_GeoFilter.h"

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


    GFE_GroupParser&
        getGroupParser()
    {
        return groupParser;
    }

    virtual void
        setComputeParm()
    {
        hasParm_computeParm = true;
    }


    virtual void
        initialParm()
    {
        if (!hasParm_computeParm)
            setComputeParm();
    }

    virtual bool
        computeCore()
    {
        if (groupParser.getGroup() && groupParser.getGroup()->isEmpty())
            return false;
        return true;
    }

    void
        compute()
    {
        initialParm();
        hasComputed = computeCore();
    }

    bool
        getHasComputed() const
    {
        return hasComputed;
    }



public:
    GFE_GroupParser groupParser;

protected:
    const SOP_NodeVerb::CookParms* cookparms;
    GA_Detail* geo;
    GOP_Manager gop;


    //bool hasParm_inGroup = false;
    //const GA_Group* geoGroup = nullptr;

    bool hasParm_computeParm = false;
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
    {
        attribUPtrArray.reserve(16);
        attribArray.reserve(16);
    }

    GFE_AttribFilter(
        const SOP_NodeVerb::CookParms& cookparms,
        GA_Detail* const geo
    )
        : GFE_GeoFilter(cookparms, geo)
    {
        attribUPtrArray.reserve(16);
        attribArray.reserve(16);
    }

    ~GFE_AttribFilter()
    {
    }

    void
        setOutAttrib(
            GA_Attribute* attribPtr
        )
    {
        attribArray.clear();
        if (!attribPtr)
            return;

        attribArray.emplace_back(attribPtr);
    }

    virtual void
        setOutAttrib(
            const GA_AttributeOwner attribClass,
            const UT_StringHolder& attribPattern
        )
    {
        if (!attribPattern.isstring() || attribPattern.length() == 0)
            return;

        GA_Attribute* attribPtr = geo->findAttribute(attribClass, attribPattern);
        setOutAttrib(attribPtr);
    }

    void
        addOutAttrib(
            GA_Attribute* attribPtr
        )
    {
        if (!attribPtr)
            return;
        attribArray.emplace_back(attribPtr);
    }


    virtual void
        addOutAttrib(
            const GA_AttributeOwner attribClass,
            const UT_StringHolder& attribPattern
        )
    {
        if (!attribPattern.isstring() || attribPattern.length() == 0)
            return;

        GA_Attribute* attribPtr = geo->findAttribute(attribClass, attribPattern);
        addOutAttrib(attribPtr);
    }


    virtual void
        addOutAttribs(
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


    virtual void
        findOrCreateOutAttrib(
            const GA_AttributeOwner owner = GA_ATTRIB_POINT,
            const GA_StorageClass storageClass = GA_STORECLASS_FLOAT,
            const GA_Storage storage = GA_STORE_INVALID,
            const bool detached = false,
            const UT_StringHolder& attribName = "__topo_"
        )
    {
        GA_Attribute* attribPtr = nullptr;

        const GA_Storage finalStorage = GFE_Type::getPreferredStorage(geo, storage, storageClass);

        if (detached)
        {
            //const GA_AttributeUPtr& attribUPtr = geo->createDetachedTupleAttribute(owner, finalStorage, 1);
            attribUPtrArray.emplace_back(geo->createDetachedTupleAttribute(owner, finalStorage, 1));
            attribPtr = attribUPtrArray[attribUPtrArray.size()-1].get();
            //attribPtr = attribUPtr.get();
        }
        else
        {
            attribPtr = geo->findAttribute(owner, attribName);

            if (attribPtr && attribPtr->getAIFTuple()->getStorage(attribPtr) != finalStorage)
                geo->getAttributes().destroyAttribute(attribPtr);

            if (!attribPtr)
                attribPtr = geo->createTupleAttribute(owner, attribName, finalStorage, 1);

            if (!attribPtr)
            {
                if (cookparms)
                    cookparms->sopAddError(SOP_ATTRIBUTE_INVALID, attribName);
                UT_ASSERT_MSG(attribPtr, "No Attrib");
                return;
            }
        }
        attribArray.emplace_back(attribPtr);
    }



    const std::vector<GA_Attribute*>&
        getAttrib() const
    {
        return attribArray;
    }

    std::vector<GA_Attribute*>&
        getAttrib()
    {
        return attribArray;
    }

    void
        bumpDataId() const
    {
        for (int i = 0; i < attribArray.size(); i++)
        {
            if (attribArray[i]->isDetached())
                continue;
            attribArray[i]->bumpDataId();
        }
    }

protected:
    std::vector<GA_AttributeUPtr> attribUPtrArray;
    std::vector<GA_Attribute*> attribArray;
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



class GFE_UVFilter : public GFE_AttribFilter {

public:

    using GFE_AttribFilter::GFE_AttribFilter;

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


    void
        setOutAttrib(
            GA_Attribute* attribPtr
        )
    {
        GFE_AttribFilter::setOutAttrib(attribPtr);
    }

    virtual void
        setOutAttrib(
            const GA_AttributeOwner attribClass,
            const UT_StringHolder& attribPattern
        ) override
    {
        if (!attribPattern.isstring() || attribPattern.length() == 0)
            return;

        GA_Attribute* attribPtr = GFE_Attribute::findUVAttributePointVertex(geo, attribClass, attribPattern);
        setOutAttrib(attribPtr);
    }

    virtual void
        setOrCreateOutAttrib(
            const GA_AttributeOwner attribClass,
            const UT_StringHolder& attribPattern,
            const GA_Storage storage = GA_STORE_INVALID
        )
    {
        if (!attribPattern.isstring() || attribPattern.length() == 0)
            return;

        GA_Attribute* attribPtr = GFE_Attribute::findOrCreateUVAttributePointVertex(geo, attribClass, attribPattern, storage);
        setOutAttrib(attribPtr);
    }

//protected:
//    GA_PrimitiveGroupUPtr geoPrimGroupUPtr;
//    const GA_PrimitiveGroup* geoPrimGroup = nullptr;
};










#endif
