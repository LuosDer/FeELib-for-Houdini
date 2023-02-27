
#pragma once

#ifndef __GFE_GroupParse_h__
#define __GFE_GroupParse_h__

//#include "GFE/GFE_GroupParse.h"

#include "GA/GA_Detail.h"
#include "GA/GA_SplittableRange.h"

#include "GFE/GFE_Type.h"



class GFE_GroupParse {

public:

    GFE_GroupParse(
        const SOP_NodeVerb::CookParms* const cookparms = nullptr
    )
        : cookparms(cookparms)
    {
    }

    GFE_GroupParse(
        const SOP_NodeVerb::CookParms& cookparms
    )
        : cookparms(&cookparms)
    {
    }

    GFE_GroupParse(
        const GA_Detail* const geo,
        const UT_StringHolder& groupName,
        const SOP_NodeVerb::CookParms* const cookparms = nullptr,
        bool findGroup = true
    )
        : geoConst(static_cast<const GEO_Detail*>(geo))
        , groupName(groupName)
        , cookparms(cookparms)
        , findGroup(findGroup)
    {
        UT_ASSERT_MSG(geoConst, "do not find geo");
    }

    GFE_GroupParse(
        const GEO_Detail* const geo,
        const UT_StringHolder& groupName,
        const SOP_NodeVerb::CookParms* const cookparms = nullptr,
        bool findGroup = true
    )
        : geoConst(geo)
        , groupName(groupName)
        , cookparms(cookparms)
        , findGroup(findGroup)
    {
        UT_ASSERT_MSG(geoConst, "do not find geo");
    }

    GFE_GroupParse(
        const GU_Detail* const geo,
        const UT_StringHolder& groupName,
        const SOP_NodeVerb::CookParms* const cookparms = nullptr,
        bool findGroup = true
    )
        : geoConst(static_cast<const GEO_Detail*>(geo))
        , groupName(groupName)
        , cookparms(cookparms)
        , findGroup(findGroup)
    {
        UT_ASSERT_MSG(geoConst, "do not find geo");
    }

    GFE_GroupParse(
        const SOP_NodeVerb::CookParms& cookparms,
        const GA_Detail* const geo,
        const UT_StringHolder& groupName,
        bool findGroup = true
    )
        : geoConst(static_cast<const GEO_Detail*>(geo))
        , groupName(groupName)
        , cookparms(&cookparms)
        , findGroup(findGroup)
    {
        UT_ASSERT_MSG(geoConst, "do not find geo");
    }

    GFE_GroupParse(
        const SOP_NodeVerb::CookParms& cookparms,
        const GEO_Detail* const geo,
        const UT_StringHolder& groupName,
        bool findGroup = true
    )
        : geoConst(geo)
        , groupName(groupName)
        , cookparms(&cookparms)
        , findGroup(findGroup)
    {
        UT_ASSERT_MSG(geoConst, "do not find geo");
    }

    GFE_GroupParse(
        const SOP_NodeVerb::CookParms& cookparms,
        const GU_Detail* const geo,
        const UT_StringHolder& groupName,
        bool findGroup = true
        )
        : geoConst(static_cast<const GEO_Detail*>(geo))
        , groupName(groupName)
        , cookparms(&cookparms)
        , findGroup(findGroup)
    {
        UT_ASSERT_MSG(geoConst, "do not find geo");
    }








    GFE_GroupParse(
        GA_Detail* const geo,
        const UT_StringHolder& groupName,
        const SOP_NodeVerb::CookParms* const cookparms = nullptr,
        bool findGroup = true
    )
        : geo(static_cast<GEO_Detail*>(geo))
        , groupName(groupName)
        , cookparms(cookparms)
        , findGroup(findGroup)
    {
        UT_ASSERT_MSG(geo, "do not find geo");
    }

    GFE_GroupParse(
        GEO_Detail* const geo,
        const UT_StringHolder& groupName,
        const SOP_NodeVerb::CookParms* const cookparms = nullptr,
        bool findGroup = true
    )
        : geo(geo)
        , groupName(groupName)
        , cookparms(cookparms)
        , findGroup(findGroup)
    {
        UT_ASSERT_MSG(geo, "do not find geo");
    }

    GFE_GroupParse(
        GU_Detail* const geo,
        const UT_StringHolder& groupName,
        const SOP_NodeVerb::CookParms* const cookparms = nullptr,
        bool findGroup = true
    )
        : geo(static_cast<GEO_Detail*>(geo))
        , groupName(groupName)
        , cookparms(cookparms)
        , findGroup(findGroup)
    {
        UT_ASSERT_MSG(geo, "do not find geo");
    }

    GFE_GroupParse(
        const SOP_NodeVerb::CookParms& cookparms,
        GA_Detail* const geo,
        const UT_StringHolder& groupName,
        bool findGroup = true
    )
        : geo(static_cast<GEO_Detail*>(geo))
        , groupName(groupName)
        , cookparms(&cookparms)
        , findGroup(findGroup)
    {
        UT_ASSERT_MSG(geo, "do not find geo");
    }

    GFE_GroupParse(
        const SOP_NodeVerb::CookParms& cookparms,
        GEO_Detail* const geo,
        const UT_StringHolder& groupName,
        bool findGroup = true
    )
        : geo(geo)
        , groupName(groupName)
        , cookparms(&cookparms)
        , findGroup(findGroup)
    {
        UT_ASSERT_MSG(geo, "do not find geo");
    }

    GFE_GroupParse(
        const SOP_NodeVerb::CookParms& cookparms,
        GU_Detail* const geo,
        const UT_StringHolder& groupName,
        bool findGroup = true
    )
        : geo(static_cast<GEO_Detail*>(geo))
        , groupName(groupName)
        , cookparms(&cookparms)
        , findGroup(findGroup)
    {
        UT_ASSERT_MSG(geo, "do not find geo");
    }











    GA_Group*
        parse(
            const GA_GroupType groupType
        )
    {
        if (!geo)
        {
            UT_ASSERT_MSG(geo, "do not find geo");
            return nullptr;
        }

        if (!groupName.length())
            return nullptr;

        if (!groupName.isstring())
        {
            if (cookparms)
                cookparms->sopAddWarning(SOP_ERR_BADGROUP, groupName);
            return nullptr;
        }

        if (findGroup && groupType >= 0 && groupType <= 4)
        {
            GA_GroupTable* const groupTable = geo->getGroupTable(groupType);
            if (groupTable)
            {
                GA_Group* const anyGroup = groupTable->find(groupName);
                if (anyGroup)
                    return anyGroup;
            }
        }

        bool success = true;
        GA_Group* const anyGroup = const_cast<GA_Group*>(gop.parseGroupDetached(groupName, groupType, geo, true, false, success));
        //if (cookparms) notifyGroupParmListeners(cookparms.getNode(), 0, 1, geo, group);

        if (!success)
        {
            if (cookparms)
                cookparms->sopAddWarning(SOP_ERR_BADGROUP, groupName);
            return nullptr;
        }
        return anyGroup;
    }


    const GA_Group*
        parseConst(
            const GA_GroupType groupType
        )
    {
        if (!geoConst)
        {
            UT_ASSERT_MSG(geoConst, "do not find geo");
            return nullptr;
        }

        if (!groupName.length())
            return nullptr;

        if (!groupName.isstring())
        {
            if (cookparms)
                cookparms->sopAddWarning(SOP_ERR_BADGROUP, groupName);
            return nullptr;
        }

        if (findGroup && groupType >= 0 && groupType <= 4)
        {
            const GA_GroupTable* const groupTable = geoConst->getGroupTable(groupType);
            if (groupTable)
            {
                const GA_Group* const anyGroup = groupTable->find(groupName);
                if (anyGroup)
                    return anyGroup;
            }
        }

        bool success = true;
        const GA_Group* const anyGroup = gop.parseGroupDetached(groupName, groupType, geoConst, true, false, success);
        //if (cookparms) notifyGroupParmListeners(cookparms.getNode(), 0, 1, geoConst, group);

        if (!success)
        {
            if (cookparms)
                cookparms->sopAddWarning(SOP_ERR_BADGROUP, groupName);
            return nullptr;
        }
        return anyGroup;
    }



    SYS_FORCE_INLINE
        GA_PrimitiveGroup*
        parsePrimitive()
    {
        return static_cast<GA_PrimitiveGroup*>(parse(GA_GROUP_PRIMITIVE));
    }

    SYS_FORCE_INLINE
        GA_PointGroup*
        parsePoint()
    {
        return static_cast<GA_PointGroup*>(parse(GA_GROUP_POINT));
    }

    SYS_FORCE_INLINE
        GA_VertexGroup*
        parseVertex()
    {
        return static_cast<GA_VertexGroup*>(parse(GA_GROUP_VERTEX));
    }

    SYS_FORCE_INLINE
        GA_EdgeGroup*
        parseEdge()
    {
        return static_cast<GA_EdgeGroup*>(parse(GA_GROUP_EDGE));
    }






    SYS_FORCE_INLINE
        void
        setParm(
            const SOP_NodeVerb::CookParms& cookparms
        )
    {
        this->cookparms = &cookparms;
    }

    SYS_FORCE_INLINE
        void
        setParm(
            const SOP_NodeVerb::CookParms* const cookparms
        )
    {
        this->cookparms = cookparms;
    }

    SYS_FORCE_INLINE
        void
        setParm(
            const UT_StringHolder& groupName
        )
    {
        this->groupName = groupName;
    }

    SYS_FORCE_INLINE
        void
        setParm(
            const SOP_NodeVerb::CookParms& cookparms,
            const UT_StringHolder& groupName
        )
    {
        this->cookparms = &cookparms;
        this->groupName = groupName;
    }

    SYS_FORCE_INLINE
        void
        setParm(
            GA_Detail* geo
        )
    {
        this->geo = static_cast<GEO_Detail*>(geo);
    }

    SYS_FORCE_INLINE
        void
        setParm(
            const GA_Detail* geo
        )
    {
        this->geoConst = static_cast<const GEO_Detail*>(geo);
    }





private:

    const SOP_NodeVerb::CookParms* cookparms;
    GEO_Detail* geo;
    const GEO_Detail* geoConst;

    bool findGroup;
    GOP_Manager gop;
    UT_StringHolder groupName;
};



























namespace GFE_GroupParse_Namespace {









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



















} // End of namespace GFE_GroupParse

#endif
