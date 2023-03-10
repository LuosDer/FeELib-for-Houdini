
#pragma once

#ifndef __GFE_GroupParser_h__
#define __GFE_GroupParser_h__

//#include "GFE/GFE_GroupParser.h"

#include "GA/GA_Detail.h"


#include "GFE/GFE_GroupPromote.h"



class GFE_GroupParser {

public:

    GFE_GroupParser(
        GA_Detail* const geo,
        GOP_Manager& gop,
        const SOP_NodeVerb::CookParms* const cookparms = nullptr
    )
        : geo(static_cast<const GEO_Detail*>(geo))
        , geoNonconst(geo)
        , cookparms(cookparms)
        , gop(gop)
    {
        UT_ASSERT_MSG(geo, "do not find geo");
    }

    GFE_GroupParser(
        const SOP_NodeVerb::CookParms& cookparms,
        GA_Detail* const geo,
        GOP_Manager& gop
    )
        : geo(static_cast<const GEO_Detail*>(geo))
        , geoNonconst(geo)
        , cookparms(&cookparms)
        , gop(gop)
    {
        UT_ASSERT_MSG(geo, "do not find geo");
    }




    GFE_GroupParser(
        const GA_Detail* const geo,
        GOP_Manager& gop,
        const SOP_NodeVerb::CookParms* const cookparms = nullptr
    )
        : geo(static_cast<const GEO_Detail*>(geo))
        , geoNonconst(nullptr)
        , cookparms(cookparms)
        , gop(gop)
    {
        UT_ASSERT_MSG(geo, "do not find geo");
    }

    GFE_GroupParser(
        const SOP_NodeVerb::CookParms& cookparms,
        const GA_Detail* const geo,
        GOP_Manager& gop
    )
        : geo(static_cast<const GEO_Detail*>(geo))
        , geoNonconst(nullptr)
        , cookparms(&cookparms)
        , gop(gop)
    {
        UT_ASSERT_MSG(geo, "do not find geo");
    }




    ~GFE_GroupParser()
    {
        if (delGroup && geoNonconst && geoGroup && !geoGroup->isDetached())
        {
            geoNonconst->destroyGroup(geoNonconst->getGroupTable(geoGroup->classType())->find(geoGroup->getName()));
        }
    }


    //GFE_GroupParser(
    //    const SOP_NodeVerb::CookParms* const cookparms = nullptr
    //)
    //    : cookparms(cookparms)
    //{
    //}

    //GFE_GroupParser(
    //    const SOP_NodeVerb::CookParms& cookparms
    //)
    //    : cookparms(&cookparms)
    //{
    //}

    //GFE_GroupParser(
    //    const GA_Detail* const geo,
    //    const UT_StringHolder& groupName,
    //    const SOP_NodeVerb::CookParms* const cookparms = nullptr
    //)
    //    : geoConst(static_cast<const GEO_Detail*>(geo))
    //    , groupName(groupName)
    //    , cookparms(cookparms)
    //{
    //    UT_ASSERT_MSG(geoConst, "do not find geo");
    //}

    //GFE_GroupParser(
    //    const GEO_Detail* const geo,
    //    const UT_StringHolder& groupName,
    //    const SOP_NodeVerb::CookParms* const cookparms = nullptr
    //)
    //    : geoConst(geo)
    //    , groupName(groupName)
    //    , cookparms(cookparms)
    //{
    //    UT_ASSERT_MSG(geoConst, "do not find geo");
    //}

    //GFE_GroupParser(
    //    const GU_Detail* const geo,
    //    const UT_StringHolder& groupName,
    //    const SOP_NodeVerb::CookParms* const cookparms = nullptr
    //)
    //    : geoConst(static_cast<const GEO_Detail*>(geo))
    //    , groupName(groupName)
    //    , cookparms(cookparms)
    //{
    //    UT_ASSERT_MSG(geoConst, "do not find geo");
    //}

    //GFE_GroupParser(
    //    const SOP_NodeVerb::CookParms& cookparms,
    //    const GA_Detail* const geo,
    //    const UT_StringHolder& groupName
    //)
    //    : geoConst(static_cast<const GEO_Detail*>(geo))
    //    , groupName(groupName)
    //    , cookparms(&cookparms)
    //{
    //    UT_ASSERT_MSG(geoConst, "do not find geo");
    //}

    //GFE_GroupParser(
    //    const SOP_NodeVerb::CookParms& cookparms,
    //    const GEO_Detail* const geo,
    //    const UT_StringHolder& groupName
    //)
    //    : geoConst(geo)
    //    , groupName(groupName)
    //    , cookparms(&cookparms)
    //{
    //    UT_ASSERT_MSG(geoConst, "do not find geo");
    //}

    //GFE_GroupParser(
    //    const SOP_NodeVerb::CookParms& cookparms,
    //    const GU_Detail* const geo,
    //    const UT_StringHolder& groupName
    //    )
    //    : geoConst(static_cast<const GEO_Detail*>(geo))
    //    , groupName(groupName)
    //    , cookparms(&cookparms)
    //{
    //    UT_ASSERT_MSG(geoConst, "do not find geo");
    //}








    //GFE_GroupParser(
    //    GA_Detail* const geo,
    //    const UT_StringHolder& groupName,
    //    const SOP_NodeVerb::CookParms* const cookparms = nullptr
    //)
    //    : geo(static_cast<GEO_Detail*>(geo))
    //    , groupName(groupName)
    //    , cookparms(cookparms)
    //{
    //    UT_ASSERT_MSG(geo, "do not find geo");
    //}

    //GFE_GroupParser(
    //    GEO_Detail* const geo,
    //    const UT_StringHolder& groupName,
    //    const SOP_NodeVerb::CookParms* const cookparms = nullptr
    //)
    //    : geo(geo)
    //    , groupName(groupName)
    //    , cookparms(cookparms)
    //{
    //    UT_ASSERT_MSG(geo, "do not find geo");
    //}

    //GFE_GroupParser(
    //    GU_Detail* const geo,
    //    const UT_StringHolder& groupName,
    //    const SOP_NodeVerb::CookParms* const cookparms = nullptr,
    //    bool findGroup = true
    //)
    //    : geo(static_cast<GEO_Detail*>(geo))
    //    , groupName(groupName)
    //    , cookparms(cookparms)
    //{
    //    UT_ASSERT_MSG(geo, "do not find geo");
    //}

    //GFE_GroupParser(
    //    const SOP_NodeVerb::CookParms& cookparms,
    //    GA_Detail* const geo,
    //    const UT_StringHolder& groupName
    //)
    //    : geo(static_cast<GEO_Detail*>(geo))
    //    , groupName(groupName)
    //    , cookparms(&cookparms)
    //{
    //    UT_ASSERT_MSG(geo, "do not find geo");
    //}

    //GFE_GroupParser(
    //    const SOP_NodeVerb::CookParms& cookparms,
    //    GEO_Detail* const geo,
    //    const UT_StringHolder& groupName
    //)
    //    : geo(geo)
    //    , groupName(groupName)
    //    , cookparms(&cookparms)
    //{
    //    UT_ASSERT_MSG(geo, "do not find geo");
    //}

    //GFE_GroupParser(
    //    const SOP_NodeVerb::CookParms& cookparms,
    //    GU_Detail* const geo,
    //    const UT_StringHolder& groupName
    //)
    //    : geo(static_cast<GEO_Detail*>(geo))
    //    , groupName(groupName)
    //    , cookparms(&cookparms)
    //{
    //    UT_ASSERT_MSG(geo, "do not find geo");
    //}


    //


    //GA_Group*
    //    parse(
    //        const GA_GroupType groupType
    //    )
    //{
    //    if (!geo)
    //    {
    //        UT_ASSERT_MSG(geo, "do not find geo");
    //        return nullptr;
    //    }

    //    if (!groupName.length())
    //        return nullptr;

    //    if (!groupName.isstring())
    //    {
    //        if (cookparms)
    //            cookparms->sopAddWarning(SOP_ERR_BADGROUP, groupName);
    //        return nullptr;
    //    }

    //    if (findGroup && groupType >= 0 && groupType <= 4)
    //    {
    //        GA_GroupTable* const groupTable = geo->getGroupTable(groupType);
    //        if (groupTable)
    //        {
    //            GA_Group* const anyGroup = groupTable->find(groupName);
    //            if (anyGroup)
    //                return anyGroup;
    //        }
    //    }

    //    bool success = true;
    //    GA_Group* const anyGroup = const_cast<GA_Group*>(gop.parseGroupDetached(groupName, groupType, geo, true, false, success));
    //    //if (cookparms) notifyGroupParmListeners(cookparms.getNode(), 0, 1, geo, group);

    //    if (!success)
    //    {
    //        if (cookparms)
    //            cookparms->sopAddWarning(SOP_ERR_BADGROUP, groupName);
    //        return nullptr;
    //    }
    //    return anyGroup;
    //}


    //const GA_Group*
    //    parseConst(
    //        const GA_GroupType groupType
    //    )
    //{
    //    if (!geoConst)
    //    {
    //        UT_ASSERT_MSG(geoConst, "do not find geo");
    //        return nullptr;
    //    }

    //    if (!groupName.length())
    //        return nullptr;

    //    if (!groupName.isstring())
    //    {
    //        if (cookparms)
    //            cookparms->sopAddWarning(SOP_ERR_BADGROUP, groupName);
    //        return nullptr;
    //    }

    //    if (findGroup && groupType >= 0 && groupType <= 4)
    //    {
    //        const GA_GroupTable* const groupTable = geoConst->getGroupTable(groupType);
    //        if (groupTable)
    //        {
    //            const GA_Group* const anyGroup = groupTable->find(groupName);
    //            if (anyGroup)
    //                return anyGroup;
    //        }
    //    }

    //    bool success = true;
    //    const GA_Group* const anyGroup = gop.parseGroupDetached(groupName, groupType, geoConst, true, false, success);
    //    //if (cookparms) notifyGroupParmListeners(cookparms.getNode(), 0, 1, geoConst, group);

    //    if (!success)
    //    {
    //        if (cookparms)
    //            cookparms->sopAddWarning(SOP_ERR_BADGROUP, groupName);
    //        return nullptr;
    //    }
    //    return anyGroup;
    //}



    //SYS_FORCE_INLINE
    //    const GA_PrimitiveGroup*
    //    parsePrimitiveConst()
    //{
    //    return static_cast<const GA_PrimitiveGroup*>(parseConst(GA_GROUP_PRIMITIVE));
    //}

    //SYS_FORCE_INLINE
    //    const GA_PointGroup*
    //    parsePointConst()
    //{
    //    return static_cast<const GA_PointGroup*>(parseConst(GA_GROUP_POINT));
    //}

    //SYS_FORCE_INLINE
    //    const GA_VertexGroup*
    //    parseVertexConst()
    //{
    //    return static_cast<const GA_VertexGroup*>(parseConst(GA_GROUP_VERTEX));
    //}

    //SYS_FORCE_INLINE
    //    const GA_EdgeGroup*
    //    parseEdgeConst()
    //{
    //    return static_cast<const GA_EdgeGroup*>(parseConst(GA_GROUP_EDGE));
    //}



    //SYS_FORCE_INLINE
    //    GA_PrimitiveGroup*
    //    parsePrimitive()
    //{
    //    return static_cast<GA_PrimitiveGroup*>(parse(GA_GROUP_PRIMITIVE));
    //}

    //SYS_FORCE_INLINE
    //    GA_PointGroup*
    //    parsePoint()
    //{
    //    return static_cast<GA_PointGroup*>(parse(GA_GROUP_POINT));
    //}

    //SYS_FORCE_INLINE
    //    GA_VertexGroup*
    //    parseVertex()
    //{
    //    return static_cast<GA_VertexGroup*>(parse(GA_GROUP_VERTEX));
    //}

    //SYS_FORCE_INLINE
    //    GA_EdgeGroup*
    //    parseEdge()
    //{
    //    return static_cast<GA_EdgeGroup*>(parse(GA_GROUP_EDGE));
    //}








    //SYS_FORCE_INLINE
    //    void
    //    setParm(
    //        const SOP_NodeVerb::CookParms& cookparms
    //    )
    //{
    //    this->cookparms = &cookparms;
    //}

    //SYS_FORCE_INLINE
    //    void
    //    setParm(
    //        const SOP_NodeVerb::CookParms* const cookparms
    //    )
    //{
    //    this->cookparms = cookparms;
    //}

    //SYS_FORCE_INLINE
    //    void
    //    setParm(
    //        const UT_StringHolder& groupName
    //    )
    //{
    //    this->groupName = groupName;
    //}

    //SYS_FORCE_INLINE
    //    void
    //    setParm(
    //        const SOP_NodeVerb::CookParms& cookparms,
    //        const UT_StringHolder& groupName
    //    )
    //{
    //    this->cookparms = &cookparms;
    //    this->groupName = groupName;
    //}

    //SYS_FORCE_INLINE
    //    void
    //    setParm(
    //        GA_Detail* geo
    //    )
    //{
    //    this->geo = static_cast<GEO_Detail*>(geo);
    //}

    //SYS_FORCE_INLINE
    //    void
    //    setParm(
    //        const GA_Detail* geo
    //    )
    //{
    //    this->geoConst = static_cast<const GEO_Detail*>(geo);
    //}




    //void
    //    setParm(
    //        const SOP_NodeVerb::CookParms& cookparms,
    //        const GA_Detail* geo,
    //        const GA_GroupType groupType,
    //        const UT_StringHolder& groupName
    //    )
    //{
    //    this->cookparms = &cookparms;
    //    this->geo = static_cast<const GEO_Detail*>(geo);
    //    this->groupType = groupType;
    //    this->groupName = groupName;

    //    hasGroup = false;
    //}

    //void
    //    setParm(
    //        const GA_Detail* geo,
    //        const GA_GroupType groupType,
    //        const UT_StringHolder& groupName,
    //        const SOP_NodeVerb::CookParms* const cookparms = nullptr
    //    )
    //{
    //    this->cookparms = cookparms;
    //    this->geo = static_cast<const GEO_Detail*>(geo);
    //    this->groupType = groupType;
    //    this->groupName = groupName;

    //    hasGroup = false;
    //}

    void
    copy(
        const GFE_GroupParser& groupParser
    )
    {
        geo = groupParser.geo;
        cookparms = groupParser.cookparms;
        gop = groupParser.gop;

        findGroup = groupParser.findGroup;

        hasGroup = groupParser.hasGroup;
        geoGroup = groupParser.geoGroup;

        hasPrimitiveGroup = groupParser.hasPrimitiveGroup;
        geoPrimitiveGroup = groupParser.geoPrimitiveGroup;

        hasPointGroup = groupParser.hasPointGroup;
        geoPointGroup = groupParser.geoPointGroup;

        hasVertexGroup = groupParser.hasVertexGroup;
        geoVertexGroup = groupParser.geoVertexGroup;

        hasEdgeGroup = groupParser.hasEdgeGroup;
        geoEdgeGroup = groupParser.geoEdgeGroup;
    }

    virtual void
    reset(
        const GA_Detail* const geo,
        const SOP_NodeVerb::CookParms* const cookparms = nullptr
    )
    {
        this->geo = static_cast<const GEO_Detail*>(geo);
        this->cookparms = cookparms;
    }

    virtual void
    reset(
        const GEO_Detail* const geo,
        const SOP_NodeVerb::CookParms* const cookparms = nullptr
    )
    {
        this->geo = geo;
        this->cookparms = cookparms;
    }

    bool
    getFindGroup()
    {
        return findGroup;
    }

    void
    setFindGroup(
        const bool findGroup
    )
    {
        this->findGroup = findGroup;
    }

    bool
        getDelGroup()
    {
        return delGroup;
    }

    void
        setDelGroup(
            const bool delGroup
        )
    {
        this->delGroup = delGroup;
    }
    
    void
        setGroup(
            const GA_Group* const group
        )
    {
        hasGroup = true;
        geoGroup = group;
        clearElementGroup();
    }

    void
        setGroup(
            const GA_PrimitiveGroup* const group
        )
    {
        setGroup(static_cast<const GA_Group*>(group));
        hasPrimitiveGroup = true;
        geoPrimitiveGroup = group;
    }

    void
        setGroup(
            const GA_PointGroup* const group
        )
    {
        setGroup(static_cast<const GA_Group*>(group));
        hasPointGroup = true;
        geoPointGroup = group;
    }

    void
        setGroup(
            const GA_VertexGroup* const group
        )
    {
        setGroup(static_cast<const GA_Group*>(group));
        hasVertexGroup = true;
        geoVertexGroup = group;
    }

    void
        setGroup(
            const GA_EdgeGroup* const group
        )
    {
        setGroup(static_cast<const GA_Group*>(group));
        hasEdgeGroup = true;
        geoEdgeGroup = group;
    }


    void
    setGroup(
        const GA_GroupType groupType,
        const UT_StringHolder& groupName
    )
    {
        if (!geo)
        {
            UT_ASSERT_MSG(geo, "do not find geo");
            return;
        }

        if (!groupName.length())
        {
            setAllGroupFull();
            return;
        }

        if (!groupName.isstring())
        {
            if (cookparms)
                cookparms->sopAddWarning(SOP_ERR_BADGROUP, groupName);
            hasGroup = false;
            return;
        }

        clearElementGroup();

        if (findGroup && groupType >= 0 && groupType <= 4)
        {
            const GA_GroupTable* const groupTable = geo->getGroupTable(groupType);
            if (groupTable)
            {
                geoGroup = groupTable->find(groupName);
                if (geoGroup)
                {
                    hasGroup = true;
                    return;
                }
            }
        }

        geoGroup = gop.parseGroupDetached(groupName, groupType, geo, true, false, hasGroup);
        //if (cookparms) notifyGroupParmListeners(cookparms.getNode(), 0, 1, geoConst, group);

        if (!hasGroup)
        {
            if (cookparms)
                cookparms->sopAddWarning(SOP_ERR_BADGROUP, groupName);
        }
    }

    SYS_FORCE_INLINE
    void
        setPrimitiveGroup(
            const UT_StringHolder& groupName
        )
    {
        setGroup(GA_GROUP_PRIMITIVE, groupName);
    }

    SYS_FORCE_INLINE
    void
        setPointGroup(
            const UT_StringHolder& groupName
        )
    {
        setGroup(GA_GROUP_POINT, groupName);
    }

    SYS_FORCE_INLINE
        void
        setVertexGroup(
            const UT_StringHolder& groupName
        )
    {
        setGroup(GA_GROUP_VERTEX, groupName);
    }

    SYS_FORCE_INLINE
        void
        setEdgeGroup(
            const UT_StringHolder& groupName
        )
    {
        setGroup(GA_GROUP_EDGE, groupName);
    }


    bool
        isEmpty()
    {
        if(!hasGroup)
            return true;
        return bool(geoGroup) && geoGroup->isEmpty();
    }



    bool
        getHasGroup()
    {
        return hasGroup;
    }

    const GA_Group*
        getGroup()
    {
        if (!hasGroup)
            return nullptr;
        return geoGroup;
    }

    const GA_Group*
        getGroup(
            const GA_GroupType groupType
        )
    {
        switch (groupType)
        {
        case GA_GROUP_PRIMITIVE:
            return getPrimitiveGroup();
            break;
        case GA_GROUP_POINT:
            return getPointGroup();
            break;
        case GA_GROUP_EDGE:
            return getEdgeGroup();
            break;
        case GA_GROUP_VERTEX:
            return getVertexGroup();
            break;
        }
        UT_ASSERT_MSG(0, "Unhandled Group Type");
        return nullptr;
    }

    SYS_FORCE_INLINE
        const GA_Group*
        getGroup(
            const GA_AttributeOwner attribOwner
        )
    {
        return getGroup(GFE_Type::attributeOwner_groupType(attribOwner));
    }


    SYS_FORCE_INLINE
    const GA_Group*
        getGroup(
            const GA_Attribute* attribPtr
        )
    {
        return getGroup(attribPtr->getOwner());
    }

    GA_GroupType
        groupType()
    {
        if (!hasGroup)
            return GA_GROUP_INVALID;
        if (!geoGroup)
            return GA_GROUP_N;
        return getGroup()->classType();
    }


    const GA_PrimitiveGroup*
        getPrimitiveGroup()
    {
        if (!hasPrimitiveGroup)
            setPrimitiveGroup();
        if (!hasPrimitiveGroup)
            return nullptr;
        return geoPrimitiveGroup;
    }

    const GA_PointGroup*
        getPointGroup()
    {
        if (!hasPointGroup)
            setPointGroup();
        if (!hasPointGroup)
            return nullptr;
        return geoPointGroup;
    }

    const GA_VertexGroup*
        getVertexGroup()
    {
        if (!hasVertexGroup)
            setVertexGroup();
        if (!hasVertexGroup)
            return nullptr;
        return geoVertexGroup;
    }

    const GA_EdgeGroup*
        getEdgeGroup()
    {
        if (!hasEdgeGroup)
            setEdgeGroup();
        if (!hasEdgeGroup)
            return nullptr;
        return geoEdgeGroup;
    }

    GA_Range
        getPrimitiveRange()
    {
        return GA_Range(geo->getPrimitiveMap(), getPrimitiveGroup());
    }

    GA_Range
        getPointRange()
    {
        return GA_Range(geo->getPointMap(), getPointGroup());
    }

    GA_Range
        getVertexRange()
    {
        return GA_Range(geo->getVertexMap(), getVertexGroup());
    }

    GA_Range
        getRange(
            const GA_AttributeOwner attribOwner
        )
    {
        switch (attribOwner)
        {
        case GA_ATTRIB_PRIMITIVE:
            return getPrimitiveRange();
            break;
        case GA_ATTRIB_POINT:
            return getPointRange();
            break;
        case GA_ATTRIB_VERTEX:
            return getVertexRange();
            break;
        default:
            break;
        }
        return GA_Range();
    }

    


    GOP_Manager&
        getGOP()
    {
        return gop;
    }



private:

    void
        clearElementGroup()
    {
        hasPrimitiveGroup = false;
        hasPointGroup = false;
        hasVertexGroup = false;
        hasEdgeGroup = false;
    }

    void
        setAllGroupFull()
    {
        hasGroup = true;
        hasPrimitiveGroup = true;
        hasPointGroup = true;
        hasVertexGroup = true;
        hasEdgeGroup = true;

        gop.destroyAdhocGroup(geoGroup);
        gop.destroyAdhocGroup(geoPrimitiveGroup);
        gop.destroyAdhocGroup(geoPointGroup);
        gop.destroyAdhocGroup(geoVertexGroup);
        gop.destroyAdhocGroup(geoEdgeGroup);

        geoGroup = nullptr;
        geoPrimitiveGroup = nullptr;
        geoPointGroup = nullptr;
        geoVertexGroup = nullptr;
        geoEdgeGroup = nullptr;
    }


    void
        setPrimitiveGroup()
    {
        if (!hasGroup)
            return;

        hasPrimitiveGroup = true;
        geoPrimitiveGroup = GFE_GroupPromote::groupFindPromotePrimitiveDetached(geo, geoGroup, gop);
        if (geoPrimitiveGroup->isDetached())
            //geoPrimitiveGroupUPtr.reset(geoPrimitiveGroup);
            gop.appendAdhocGroup(const_cast<GA_PrimitiveGroup*>(geoPrimitiveGroup), false);
    }


    void
        setPointGroup()
    {
        if (!hasGroup)
            return;

        hasPointGroup = true;
        geoPointGroup = GFE_GroupPromote::groupFindPromotePointDetached(geo, geoGroup, gop);
        if (geoPointGroup->isDetached())
            gop.appendAdhocGroup(const_cast<GA_PointGroup*>(geoPointGroup), false);
    }


    void
        setVertexGroup()
    {
        if (!hasGroup)
            return;

        hasVertexGroup = true;
        geoVertexGroup = GFE_GroupPromote::groupFindPromoteVertexDetached(geo, geoGroup, gop);
        if (geoVertexGroup->isDetached())
            gop.appendAdhocGroup(const_cast<GA_VertexGroup*>(geoVertexGroup), false);
    }


    void
        setEdgeGroup()
    {
        if (!hasGroup)
            return;

        hasEdgeGroup = true;
        geoEdgeGroup = GFE_GroupPromote::groupFindPromoteEdgeDetached(geo, geoGroup, gop);
        if (geoEdgeGroup->isDetached())
            gop.appendAdhocGroup(const_cast<GA_EdgeGroup*>(geoEdgeGroup), false);
    }

private:

    //GEO_Detail* geo = nullptr;
    const SOP_NodeVerb::CookParms* cookparms;
    const GEO_Detail* geo;
    GA_Detail* geoNonconst;
    
    GOP_Manager& gop;
    //GA_GroupType groupType;
    //UT_StringHolder groupName;

    bool findGroup = true;
    bool delGroup = false;


    bool hasGroup = false;
    const GA_Group* geoGroup = nullptr;

    bool hasPrimitiveGroup = false;
    const GA_PrimitiveGroup* geoPrimitiveGroup = nullptr;

    bool hasPointGroup = false;
    const GA_PointGroup* geoPointGroup = nullptr;

    bool hasVertexGroup = false;
    const GA_VertexGroup* geoVertexGroup = nullptr;

    bool hasEdgeGroup = false;
    const GA_EdgeGroup* geoEdgeGroup = nullptr;

    //GA_PrimitiveGroupUPtr geoPrimitiveGroupUPtr;
    //GA_PointGroupUPtr     geoPointGroupUPtr;
    //GA_VertexGroupUPtr    geoVertexGroupUPtr;
    //GA_EdgeGroupUPtr      geoEdgeGroupUPtr;
};








































namespace GFE_GroupParser_Namespace {
//namespace GFE_GroupParser {









static GA_Group*
findOrParseGroupDetached(
    const SOP_NodeVerb::CookParms& cookparms,
    GA_Detail* const geo,
    const GA_GroupType groupType,
    const UT_StringHolder& groupName,
    GOP_Manager& gop
)
{
    UT_ASSERT_P(geo);
    if (!groupName.length())
        return nullptr;

    if (!groupName.isstring())
    {
        cookparms.sopAddWarning(SOP_ERR_BADGROUP, groupName);
        return nullptr;
    }

    GA_GroupTable* const groupTable = geo->getGroupTable(groupType);
    if (groupTable)
    {
        GA_Group* const anyGroup = groupTable->find(groupName);
        if (anyGroup)
            return anyGroup;
    }

    bool success = true;
    GA_Group* const anyGroup = const_cast<GA_Group*>(gop.parseGroupDetached(groupName, groupType, static_cast<GEO_Detail*>(geo), true, false, success));
    //notifyGroupParmListeners(cookparms.getNode(), 0, 1, geo, group);

    //if (!success || (anyGroup && !anyGroup->isElementGroup()))
    if (!success)
    {
        cookparms.sopAddWarning(SOP_ERR_BADGROUP, groupName);
        return nullptr;
    }
    if (anyGroup)
    {
        return anyGroup;
    }
    return nullptr;
}

static const GA_Group*
findOrParseGroupDetached(
    const SOP_NodeVerb::CookParms& cookparms,
    const GA_Detail* const geo,
    const GA_GroupType groupType,
    const UT_StringHolder& groupName,
    GOP_Manager& gop
)
{
    UT_ASSERT_P(geo);
    if (!groupName.length())
        return nullptr;

    if (!groupName.isstring())
    {
        cookparms.sopAddWarning(SOP_ERR_BADGROUP, groupName);
        return nullptr;
    }

    const GA_GroupTable* const groupTable = geo->getGroupTable(groupType);
    if (groupTable)
    {
        const GA_Group* const anyGroup = groupTable->find(groupName);
        if (anyGroup)
            return anyGroup;
    }

    bool success = true;
    const GA_Group* const anyGroup = gop.parseGroupDetached(groupName, groupType, static_cast<const GEO_Detail*>(geo), true, false, success);

    //if (!success || (anyGroup && !anyGroup->isElementGroup()))
    if (!success)
    {
        cookparms.sopAddWarning(SOP_ERR_BADGROUP, groupName);
        return nullptr;
    }
    if (anyGroup)
    {
        return anyGroup;
    }
    return nullptr;
}




SYS_FORCE_INLINE
static const GA_PrimitiveGroup*
findOrParsePrimitiveGroupDetached(
    const SOP_NodeVerb::CookParms& cookparms,
    const GA_Detail* const geo,
    const UT_StringHolder& groupName,
    GOP_Manager& gop
)
{
    return static_cast<const GA_PrimitiveGroup*>(findOrParseGroupDetached(cookparms, geo, GA_GROUP_PRIMITIVE, groupName, gop));
}

SYS_FORCE_INLINE
static const GA_PointGroup*
findOrParsePointGroupDetached(
    const SOP_NodeVerb::CookParms& cookparms,
    const GA_Detail* geo,
    const UT_StringHolder& groupName,
    GOP_Manager& gop
)
{
    return static_cast<const GA_PointGroup*>(findOrParseGroupDetached(cookparms, geo, GA_GROUP_POINT, groupName, gop));
}

SYS_FORCE_INLINE
static const GA_VertexGroup*
findOrParseVertexGroupDetached(
    const SOP_NodeVerb::CookParms& cookparms,
    const GA_Detail* const geo,
    const UT_StringHolder& groupName,
    GOP_Manager& gop
)
{
    return static_cast<const GA_VertexGroup*>(findOrParseGroupDetached(cookparms, geo, GA_GROUP_VERTEX, groupName, gop));
}

SYS_FORCE_INLINE
static const GA_EdgeGroup*
findOrParseEdgeGroupDetached(
    const SOP_NodeVerb::CookParms& cookparms,
    const GA_Detail* const geo,
    const UT_StringHolder& groupName,
    GOP_Manager& gop
)
{
    return static_cast<const GA_EdgeGroup*>(findOrParseGroupDetached(cookparms, geo, GA_GROUP_EDGE, groupName, gop));
}






SYS_FORCE_INLINE
static GA_PrimitiveGroup*
findOrParsePrimitiveGroupDetached(
    const SOP_NodeVerb::CookParms& cookparms,
    GA_Detail* const geo,
    const UT_StringHolder& groupName,
    GOP_Manager& gop
)
{
    return static_cast<GA_PrimitiveGroup*>(findOrParseGroupDetached(cookparms, geo, GA_GROUP_PRIMITIVE, groupName, gop));
}

SYS_FORCE_INLINE
static GA_PointGroup*
findOrParsePointGroupDetached(
    const SOP_NodeVerb::CookParms& cookparms,
    GA_Detail* const geo,
    const UT_StringHolder& groupName,
    GOP_Manager& gop
)
{
    return static_cast<GA_PointGroup*>(findOrParseGroupDetached(cookparms, geo, GA_GROUP_POINT, groupName, gop));
}

SYS_FORCE_INLINE
static GA_VertexGroup*
findOrParseVertexGroupDetached(
    const SOP_NodeVerb::CookParms& cookparms,
    GA_Detail* const geo,
    const UT_StringHolder& groupName,
    GOP_Manager& gop
)
{
    return static_cast<GA_VertexGroup*>(findOrParseGroupDetached(cookparms, geo, GA_GROUP_VERTEX, groupName, gop));
}

SYS_FORCE_INLINE
static GA_EdgeGroup*
findOrParseEdgeGroupDetached(
    const SOP_NodeVerb::CookParms& cookparms,
    GA_Detail* const geo,
    const UT_StringHolder& groupName,
    GOP_Manager& gop
)
{
    return static_cast<GA_EdgeGroup*>(findOrParseGroupDetached(cookparms, geo, GA_GROUP_EDGE, groupName, gop));
}







static const GA_Group*
parseGroupDetached(
    const SOP_NodeVerb::CookParms& cookparms,
    const GA_Detail* const geo,
    const GA_GroupType groupType,
    const UT_StringHolder& groupName,
    GOP_Manager& gop
)
{
    UT_ASSERT_P(geo);
    if (!groupName.length())
        return nullptr;
    
    if (!groupName.isstring())
    {
        cookparms.sopAddWarning(SOP_ERR_BADGROUP, groupName);
        return nullptr;
    }

    bool success = true;
    const GA_Group* const anyGroup = gop.parseGroupDetached(groupName, groupType, static_cast<const GEO_Detail*>(geo), true, false, success);

    //if (!success || (anyGroup && !anyGroup->isElementGroup()))
    if (!success)
    {
        cookparms.sopAddWarning(SOP_ERR_BADGROUP, groupName);
        return nullptr;
    }
    if (anyGroup)
    {
        return anyGroup;
    }
    return nullptr;
}


SYS_FORCE_INLINE
static const GA_PrimitiveGroup*
parsePrimitiveGroupDetached(
    const SOP_NodeVerb::CookParms& cookparms,
    const GA_Detail* const geo,
    const UT_StringHolder& groupName,
    GOP_Manager& gop
)
{
    return static_cast<const GA_PrimitiveGroup*>(parseGroupDetached(cookparms, geo, GA_GROUP_PRIMITIVE, groupName, gop));
}


SYS_FORCE_INLINE
static const GA_PointGroup*
parsePointGroupDetached(
    const SOP_NodeVerb::CookParms& cookparms,
    const GA_Detail* const geo,
    const UT_StringHolder& groupName,
    GOP_Manager& gop
)
{
    return static_cast<const GA_PointGroup*>(parseGroupDetached(cookparms, geo, GA_GROUP_POINT, groupName, gop));
}

SYS_FORCE_INLINE
static const GA_VertexGroup*
parseVertexGroupDetached(
    const SOP_NodeVerb::CookParms& cookparms,
    const GA_Detail* const geo,
    const UT_StringHolder& groupName,
    GOP_Manager& gop
)
{
    return static_cast<const GA_VertexGroup*>(parseGroupDetached(cookparms, geo, GA_GROUP_VERTEX, groupName, gop));
}

SYS_FORCE_INLINE
static const GA_EdgeGroup*
parseEdgeGroupDetached(
    const SOP_NodeVerb::CookParms& cookparms,
    const GA_Detail* const geo,
    const UT_StringHolder& groupName,
    GOP_Manager& gop
)
{
    return static_cast<const GA_EdgeGroup*>(parseGroupDetached(cookparms, geo, GA_GROUP_EDGE, groupName, gop));
}



















} // End of namespace GFE_GroupParser

#endif
