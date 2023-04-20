
#pragma once

#ifndef __GFE_GroupParser_h__
#define __GFE_GroupParser_h__

//#include "GFE/GFE_GroupParser.h"

#include "GA/GA_Detail.h"
#include "GA/GA_SplittableRange.h"

#include "GFE/GFE_GroupUnion.h"


class GFE_GroupParser {

public:

    GFE_GroupParser(
        GA_Detail* const geo,
        GOP_Manager& gop,
        const SOP_NodeVerb::CookParms& cookparms
    )
        : geo(static_cast<const GEO_Detail*>(geo))
        , geoNonconst(geo)
        , cookparms(&cookparms)
        , gop(gop)
    {
    }
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
    }
    GFE_GroupParser(
        const GA_Detail* const geo,
        GOP_Manager& gop,
        const SOP_NodeVerb::CookParms& cookparms
    )
        : geo(static_cast<const GEO_Detail*>(geo))
        , geoNonconst(nullptr)
        , cookparms(&cookparms)
        , gop(gop)
    {
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
    }


    GFE_GroupParser(
        GA_Detail& geo,
        GOP_Manager& gop,
        const SOP_NodeVerb::CookParms& cookparms
    )
        : geo(&static_cast<const GEO_Detail&>(geo))
        , geoNonconst(&geo)
        , cookparms(&cookparms)
        , gop(gop)
    {
    }
    GFE_GroupParser(
        GA_Detail& geo,
        GOP_Manager& gop,
        const SOP_NodeVerb::CookParms* const cookparms = nullptr
    )
        : geo(&static_cast<const GEO_Detail&>(geo))
        , geoNonconst(&geo)
        , cookparms(cookparms)
        , gop(gop)
    {
    }
    GFE_GroupParser(
        const GA_Detail& geo,
        GOP_Manager& gop,
        const SOP_NodeVerb::CookParms& cookparms
    )
        : geo(&static_cast<const GEO_Detail&>(geo))
        , geoNonconst(nullptr)
        , cookparms(&cookparms)
        , gop(gop)
    {
    }
    GFE_GroupParser(
        const GA_Detail& geo,
        GOP_Manager& gop,
        const SOP_NodeVerb::CookParms* const cookparms = nullptr
    )
        : geo(&static_cast<const GEO_Detail&>(geo))
        , geoNonconst(nullptr)
        , cookparms(cookparms)
        , gop(gop)
    {
    }



    ~GFE_GroupParser()
    {
        if (delGroup && geoNonconst && geoGroup && !geoGroup->isDetached())
        {
            geoNonconst->destroyGroup(geoNonconst->getGroupTable(geoGroup->classType())->find(geoGroup->getName()));
        }
    }

    void copy(const GFE_GroupParser& groupParser)
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

    inline virtual void reset(
        const GA_Detail* const inGeo,
        const SOP_NodeVerb::CookParms* const cookparms = nullptr
    )
    {
        geo = static_cast<const GEO_Detail*>(inGeo);
        this->cookparms = cookparms;
    }

    inline virtual void reset(
        const GEO_Detail* const inGeo,
        const SOP_NodeVerb::CookParms* const cookparms = nullptr
    )
    {
        geo = inGeo;
        this->cookparms = cookparms;
    }

    inline bool getFindGroup() const
    {
        return findGroup;
    }
    inline void setFindGroup(const bool findGroup)
    {
        this->findGroup = findGroup;
    }
    inline bool getDelGroup() const
    {
        return delGroup;
    }
    inline void setDelGroup(const bool delGroup)
    {
        this->delGroup = delGroup;
    }
    inline void setGroup(const GA_Group* const group)
    {
        hasGroup = true;
        geoGroup = group;
        clearElementGroup();
    }

    inline void setGroup(const GA_PrimitiveGroup* const group)
    {
        setGroup(static_cast<const GA_Group*>(group));
        hasPrimitiveGroup = true;
        geoPrimitiveGroup = group;
    }
    inline void setGroup(const GA_PointGroup* const group)
    {
        setGroup(static_cast<const GA_Group*>(group));
        hasPointGroup = true;
        geoPointGroup = group;
    }
    inline void setGroup(const GA_VertexGroup* const group)
    {
        setGroup(static_cast<const GA_Group*>(group));
        hasVertexGroup = true;
        geoVertexGroup = group;
    }
    inline void setGroup(const GA_EdgeGroup* const group)
    {
        setGroup(static_cast<const GA_Group*>(group));
        hasEdgeGroup = true;
        geoEdgeGroup = group;
    }


    void setGroup(const GA_GroupType groupType, const UT_StringHolder& groupName)
    {
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

    SYS_FORCE_INLINE void setPrimitiveGroup(const UT_StringHolder& groupName)
    {
        setGroup(GA_GROUP_PRIMITIVE, groupName);
    }
    SYS_FORCE_INLINE void setPointGroup(const UT_StringHolder& groupName)
    {
        setGroup(GA_GROUP_POINT, groupName);
    }
    SYS_FORCE_INLINE void setVertexGroup(const UT_StringHolder& groupName)
    {
        setGroup(GA_GROUP_VERTEX, groupName);
    }
    SYS_FORCE_INLINE void setEdgeGroup(const UT_StringHolder& groupName)
    {
        setGroup(GA_GROUP_EDGE, groupName);
    }

    
    SYS_FORCE_INLINE bool isEmpty() const
    {
        return hasGroup ? ( bool(geoGroup) && geoGroup->isEmpty() ) : false;
    }
    SYS_FORCE_INLINE bool isFull() const
    {
        return hasGroup ? ( !bool(geoGroup) || !geoGroup->isEmpty() ) : true;
    }




    inline bool getHasGroup() const
    {
        return hasGroup;
    }

    inline const GA_Group* getGroup() const
    {
        if (!hasGroup)
            return nullptr;
        return geoGroup;
    }

    inline const GA_Group* getGroup(const GA_GroupType groupType)
    {
        if (!hasGroup)
            return nullptr;
        switch (groupType)
        {
        case GA_GROUP_PRIMITIVE: return getPrimitiveGroup(); break;
        case GA_GROUP_POINT:     return getPointGroup();     break;
        case GA_GROUP_EDGE:      return getEdgeGroup();      break;
        case GA_GROUP_VERTEX:    return getVertexGroup();    break;
        }
        UT_ASSERT_MSG(0, "Unhandled Group Type");
        return nullptr;
    }

    SYS_FORCE_INLINE const GA_Group* getGroup(const GA_AttributeOwner attribOwner)
    {
        return getGroup(GFE_Type::attributeOwner_groupType(attribOwner));
    }
    
    SYS_FORCE_INLINE const GA_Group* getGroup(const GA_Attribute* attribPtr)
    {
        return getGroup(attribPtr->getOwner());
    }

    GA_GroupType classType() const
    {
        if (!hasGroup)
            return GA_GROUP_INVALID;
        if (!geoGroup)
            return GA_GROUP_N;
        return getGroup()->classType();
    }

    inline GA_GroupType groupType() const
    {
        return groupType();
    }

    inline const GA_PrimitiveGroup* getPrimitiveGroup()
    {
        if (!hasGroup)
            return nullptr;
        if (!hasPrimitiveGroup)
            setPrimitiveGroup();
        return geoPrimitiveGroup;
    }

    inline const GA_PointGroup* getPointGroup()
    {
        if (!hasGroup)
            return nullptr;
        if (!hasPointGroup)
            setPointGroup();
        return geoPointGroup;
    }

    inline const GA_VertexGroup* getVertexGroup()
    {
        if (!hasGroup)
            return nullptr;
        if (!hasVertexGroup)
            setVertexGroup();
        return geoVertexGroup;
    }

    inline const GA_EdgeGroup* getEdgeGroup()
    {
        if (!hasGroup)
            return nullptr;
        if (!hasEdgeGroup)
            setEdgeGroup();
        return geoEdgeGroup;
    }

    inline GA_Range getPrimitiveRange()
    {
        return GA_Range(geo->getPrimitiveMap(), getPrimitiveGroup());
    }

    inline GA_Range getPointRange()
    {
        return GA_Range(geo->getPointMap(), getPointGroup());
    }

    inline GA_Range getVertexRange()
    {
        return GA_Range(geo->getVertexMap(), getVertexGroup());
    }

    inline GA_Range getRange(const GA_AttributeOwner attribOwner)
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

    
    SYS_FORCE_INLINE GA_SplittableRange getPrimitiveSplittableRange()
    {
        return GA_SplittableRange(getPrimitiveRange());
    }
    SYS_FORCE_INLINE GA_SplittableRange getPointSplittableRange()
    {
        return GA_SplittableRange(getPointRange());
    }
    SYS_FORCE_INLINE GA_SplittableRange getVertexSplittableRange()
    {
        return GA_SplittableRange(getVertexRange());
    }

    SYS_FORCE_INLINE GA_SplittableRange getSplittableRange(const GA_AttributeOwner attribOwner)
    {
        switch (attribOwner)
        {
        case GA_ATTRIB_PRIMITIVE:
            return getPrimitiveSplittableRange();
            break;
        case GA_ATTRIB_POINT:
            return getPointSplittableRange();
            break;
        case GA_ATTRIB_VERTEX:
            return getVertexSplittableRange();
            break;
        default:
            break;
        }
        return GA_SplittableRange();
    }

    inline GA_SplittableRange getSplittableRange(const GA_GroupType groupType)
    {
        switch (groupType)
        {
        case GA_GROUP_PRIMITIVE:
            return getPrimitiveSplittableRange();
            break;
        case GA_GROUP_POINT:
            return getPointSplittableRange();
            break;
        case GA_GROUP_VERTEX:
            return getVertexSplittableRange();
            break;
        default:
            break;
        }
        return GA_SplittableRange();
    }
    
    SYS_FORCE_INLINE GA_SplittableRange getSplittableRange(const GA_Attribute* const attrib)
    {
        UT_ASSERT_P(attrib);
        return getSplittableRange(attrib->getOwner());
    }
    
    SYS_FORCE_INLINE GA_SplittableRange getSplittableRange(const GA_Group* const group)
    {
        UT_ASSERT_P(group);
        return getSplittableRange(group->classType());
    }
    
    SYS_FORCE_INLINE GA_SplittableRange getSplittableRange(const GA_Attribute& attrib)
    {
        return getSplittableRange(attrib.getOwner());
    }
    
    SYS_FORCE_INLINE GA_SplittableRange getSplittableRange(const GA_Group& group)
    {
        return getSplittableRange(group.classType());
    }
    


    SYS_FORCE_INLINE GOP_Manager& getGOP() const
    {
        return gop;
    }



private:

    void clearElementGroup()
    {
        hasPrimitiveGroup = false;
        hasPointGroup = false;
        hasVertexGroup = false;
        hasEdgeGroup = false;
    }

    void setAllGroupFull()
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


    void setPrimitiveGroup()
    {
        if (!hasGroup)
            return;

        hasPrimitiveGroup = true;
        geoPrimitiveGroup = groupFindPromotePrimitiveDetached(geoGroup);
        if (geoPrimitiveGroup && geoPrimitiveGroup->isDetached())
            //geoPrimitiveGroupUPtr.reset(geoPrimitiveGroup);
            gop.appendAdhocGroup(const_cast<GA_PrimitiveGroup*>(geoPrimitiveGroup), false);
    }


    void setPointGroup()
    {
        if (!hasGroup)
            return;

        hasPointGroup = true;
        geoPointGroup = groupFindPromotePointDetached(geoGroup);
        if (geoPointGroup && geoPointGroup->isDetached())
            gop.appendAdhocGroup(const_cast<GA_PointGroup*>(geoPointGroup), false);
    }


    void setVertexGroup()
    {
        if (!hasGroup)
            return;

        hasVertexGroup = true;
        geoVertexGroup = groupFindPromoteVertexDetached(geoGroup);
        if (geoVertexGroup && geoVertexGroup->isDetached())
            gop.appendAdhocGroup(const_cast<GA_VertexGroup*>(geoVertexGroup), false);
    }


    void setEdgeGroup()
    {
        if (!hasGroup)
            return;

        hasEdgeGroup = true;
        geoEdgeGroup = groupFindPromoteEdgeDetached(geoGroup);
        if (geoEdgeGroup && geoEdgeGroup->isDetached())
            gop.appendAdhocGroup(const_cast<GA_EdgeGroup*>(geoEdgeGroup), false);
    }




    const GA_Group*
        groupFindPromoteDetached(
            const GA_Group* const group,
            const GA_GroupType groupType
        )
    {
        if (!group)
            return nullptr;

        if (group->classType() == groupType)
            return group;

        const GA_GroupTable* const groupTable = group->getDetail().getGroupTable(groupType);
        if (!groupTable)
            return group;

        GA_Group* const newGroup = groupTable->newDetachedGroup();

        //GFE_GroupUnion::groupUnion(newGroup, group);
        if (groupType == GA_GROUP_EDGE)
        {
            GFE_GroupUnion::groupUnion(*static_cast<GA_EdgeGroup*>(newGroup), group);
        }
        else
        {
            static_cast<GA_ElementGroup*>(newGroup)->combine(group);
        }

        gop.appendAdhocGroup(newGroup, false);

        return newGroup;
    }

    SYS_FORCE_INLINE
        const GA_PrimitiveGroup*
        groupFindPromotePrimitiveDetached(
            const GA_Group* const group
        )
    {
        return static_cast<const GA_PrimitiveGroup*>(groupFindPromoteDetached(group, GA_GROUP_PRIMITIVE));
    }

    SYS_FORCE_INLINE
        const GA_PointGroup*
        groupFindPromotePointDetached(
            const GA_Group* const group
        )
    {
        return static_cast<const GA_PointGroup*>(groupFindPromoteDetached(group, GA_GROUP_POINT));
    }

    SYS_FORCE_INLINE
        const GA_VertexGroup*
        groupFindPromoteVertexDetached(
            const GA_Group* const group
        )
    {
        return static_cast<const GA_VertexGroup*>(groupFindPromoteDetached(group, GA_GROUP_VERTEX));
    }

    SYS_FORCE_INLINE
        const GA_EdgeGroup*
        groupFindPromoteEdgeDetached(
            const GA_Group* const group
        )
    {
        return static_cast<const GA_EdgeGroup*>(groupFindPromoteDetached(group, GA_GROUP_EDGE));
    }


protected:

    SYS_FORCE_INLINE
    void
        setDetail(
            const GA_Detail* const geo
        )
    {
        this->geo = static_cast<const GEO_Detail*>(geo);
    }

    SYS_FORCE_INLINE
    void
        setDetail(
            const GEO_Detail* const geo
        )
    {
        this->geo = geo;
    }

    SYS_FORCE_INLINE
    void
        setDetail(
            const GU_Detail* const geo
        )
    {
        //this->geo = static_cast<const GEO_Detail*>(geo);
        this->geo = geo;
    }

    SYS_FORCE_INLINE
    void
        setDetail(
            GA_Detail* const geo
        )
    {
        this->geo = static_cast<const GEO_Detail*>(geo);
        geoNonconst = geo;
    }

    SYS_FORCE_INLINE
    void
        setDetail(
            GEO_Detail* const geo
        )
    {
        this->geo = geo;
        geoNonconst = geo;
    }

    SYS_FORCE_INLINE
    void
        setDetail(
            GU_Detail* const geo
        )
    {
        //this->geo = static_cast<const GEO_Detail*>(geo);
        this->geo = geo;
        geoNonconst = geo;
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


    friend class GFE_GeoFilter;
}; // End of class GFE_GroupParser









































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



















} // End of namespace GFE_GroupParser_Namespace

#endif
