
#pragma once

#ifndef __GFE_GroupParser_h__
#define __GFE_GroupParser_h__

#include "GFE/GFE_GroupParser.h"

#include "GA/GA_SplittableRange.h"
#include "SOP/SOP_NodeVerb.h"


#include "GFE/GFE_Detail.h"

#include "GFE/GFE_GroupUnion.h"


class GFE_GroupParser {

public:

    GFE_GroupParser(
        const GFE_Detail* const inGeo,
        GOP_Manager& gop,
        const SOP_NodeVerb::CookParms* const cookparms = nullptr
        //,const SOP_NodeVerb* const verb = nullptr
    )
        : geo(inGeo->asGEO_Detail())
        , geoNonconst(nullptr)
        , cookparms(cookparms)
        , gop(&gop)
        //, verb(verb)
    {
    }
    
    GFE_GroupParser(
        GFE_Detail* const inGeo,
        GOP_Manager& gop,
        const SOP_NodeVerb::CookParms* const cookparms = nullptr
        //,const SOP_NodeVerb* const verb = nullptr
    )
        : geo(inGeo->asGEO_Detail())
        , geoNonconst(inGeo->asGEO_Detail())
        , cookparms(cookparms)
        , gop(&gop)
        //, verb(verb)
    {
    }
    
    GFE_GroupParser(
        GA_Detail* const inGeo,
        GOP_Manager& gop,
        const SOP_NodeVerb::CookParms* const cookparms = nullptr
        //,const SOP_NodeVerb* const verb = nullptr
    )
        : geo(static_cast<const GEO_Detail*>(inGeo))
        , geoNonconst(inGeo)
        , cookparms(cookparms)
        , gop(&gop)
        //, verb(verb)
    {
    }
    GFE_GroupParser(
        const GA_Detail* const geo,
        GOP_Manager& gop,
        const SOP_NodeVerb::CookParms& cookparms
        //,const SOP_NodeVerb* const verb = nullptr
    )
        : geo(static_cast<const GEO_Detail*>(geo))
        , geoNonconst(nullptr)
        , cookparms(&cookparms)
        , gop(&gop)
        //, verb(verb)
    {
    }
    GFE_GroupParser(
        const GA_Detail* const geo,
        GOP_Manager& gop,
        const SOP_NodeVerb::CookParms* const cookparms = nullptr
        //,const SOP_NodeVerb* const verb = nullptr
    )
        : geo(static_cast<const GEO_Detail*>(geo))
        , geoNonconst(nullptr)
        , cookparms(cookparms)
        , gop(&gop)
        //, verb(verb)
    {
    }

    GFE_GroupParser(
        GA_Detail& geo,
        GOP_Manager& gop,
        const SOP_NodeVerb::CookParms& cookparms
        //,const SOP_NodeVerb* const verb = nullptr
    )
        : geo(&static_cast<const GEO_Detail&>(geo))
        , geoNonconst(&geo)
        , cookparms(&cookparms)
        , gop(&gop)
    {
    }
    GFE_GroupParser(
        GA_Detail& geo,
        GOP_Manager& gop,
        const SOP_NodeVerb::CookParms* const cookparms = nullptr
        //,const SOP_NodeVerb* const verb = nullptr
    )
        : geo(&static_cast<const GEO_Detail&>(geo))
        , geoNonconst(&geo)
        , cookparms(cookparms)
        , gop(&gop)
    {
    }
    GFE_GroupParser(
        const GA_Detail& geo,
        GOP_Manager& gop,
        const SOP_NodeVerb::CookParms& cookparms
        //,const SOP_NodeVerb* const verb = nullptr
    )
        : geo(&static_cast<const GEO_Detail&>(geo))
        , geoNonconst(nullptr)
        , cookparms(&cookparms)
        , gop(&gop)
    {
    }
    GFE_GroupParser(
        const GA_Detail& geo,
        GOP_Manager& gop,
        const SOP_NodeVerb::CookParms* const cookparms = nullptr
        //,const SOP_NodeVerb* const verb = nullptr
    )
        : geo(&static_cast<const GEO_Detail&>(geo))
        , geoNonconst(nullptr)
        , cookparms(cookparms)
        , gop(&gop)
    {
    }




    ~GFE_GroupParser()
    {
        //delGroup();
    }

    
    SYS_FORCE_INLINE void delGroup()
    {
        if (geoNonconst && geoGroup && !geoGroup->isDetached())
            geoNonconst->destroyGroup(geoNonconst->getGroupTable(geoGroup->classType())->find(geoGroup->getName()));
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


    
    void clear()
    {
        hasGroup = false;
        geoGroup = nullptr;
        
        hasPrimitiveGroup = false;
        geoPrimitiveGroup = nullptr;

        hasPointGroup = false;
        geoPointGroup = nullptr;

        hasVertexGroup = false;
        geoVertexGroup = nullptr;

        hasEdgeGroup = false;
        geoEdgeGroup = nullptr;
    }

    
    virtual void reset(GEO_Detail* const inGeo, const SOP_NodeVerb::CookParms* const cookparms = nullptr)
    {
        geo = inGeo;
        geoNonconst = static_cast<GA_Detail*>(inGeo);
        this->cookparms = cookparms;
        clear();
    }
    
    virtual void reset(const GEO_Detail* const inGeo, const SOP_NodeVerb::CookParms* const cookparms = nullptr)
    {
        geo = inGeo;
        geoNonconst = nullptr;
        this->cookparms = cookparms;
        clear();
    }
    
    SYS_FORCE_INLINE virtual void reset(GA_Detail* const inGeo, const SOP_NodeVerb::CookParms* const cookparms = nullptr)
    { reset(static_cast<GEO_Detail*>(inGeo), cookparms); }

    SYS_FORCE_INLINE virtual void reset(const GA_Detail* const inGeo, const SOP_NodeVerb::CookParms* const cookparms = nullptr)
    { reset(static_cast<const GEO_Detail*>(inGeo), cookparms); }

    SYS_FORCE_INLINE virtual void reset(GA_Detail& inGeo, const SOP_NodeVerb::CookParms* const cookparms = nullptr)
    { reset(&inGeo, cookparms); }

    SYS_FORCE_INLINE virtual void reset(const GA_Detail& inGeo, const SOP_NodeVerb::CookParms* const cookparms = nullptr)
    { reset(&inGeo, cookparms); }


    
    // SYS_FORCE_INLINE bool getFindGroup() const
    // { return findGroup; }
    //
    // SYS_FORCE_INLINE void setFindGroup(const bool findGroup)
    // { this->findGroup = findGroup; }
    //
    // SYS_FORCE_INLINE bool getDelGroup() const
    // { return delGroup; }
    //
    // SYS_FORCE_INLINE void setDelGroup(const bool delGroup)
    // { this->delGroup = delGroup; }
    
    void setGroup(const GA_Group* const group)
    {
        hasGroup = true;
        geoGroup = group;
        clearElementGroup();
    }

    SYS_FORCE_INLINE void setGroup(const GFE_GroupParser& groupParser)
    { setGroup(groupParser.getGroup()); }

    
    void setGroup(const GA_PrimitiveGroup* const group)
    {
        setGroup(static_cast<const GA_Group*>(group));
        hasPrimitiveGroup = true;
        geoPrimitiveGroup = group;
    }
    
    void setGroup(const GA_PointGroup* const group)
    {
        setGroup(static_cast<const GA_Group*>(group));
        hasPointGroup = true;
        geoPointGroup = group;
    }
    
    void setGroup(const GA_VertexGroup* const group)
    {
        setGroup(static_cast<const GA_Group*>(group));
        hasVertexGroup = true;
        geoVertexGroup = group;
    }
    
    void setGroup(const GA_EdgeGroup* const group)
    {
        setGroup(static_cast<const GA_Group*>(group));
        hasEdgeGroup = true;
        geoEdgeGroup = group;
    }


    const GA_Group* setGroup(const GA_GroupType groupType, const UT_StringRef& groupName)
    {
        if (!groupName.length())
        {
            setAllGroupFull();
            return nullptr;
        }

        if (!groupName.isstring())
        {
            if (cookparms)
                cookparms->sopAddWarning(SOP_ERR_BADGROUP, groupName);
            hasGroup = false;
            return nullptr;
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
                    return geoGroup;
                }
            }
        }
        
        geoGroup = gop->parseGroupDetached(groupName, groupType, geo, true, false, hasGroup);
        //hasGroup &= bool(geoGroup);

        // const SOP_NodeVerb* const verb = cookparms->getNode()->cookVerb();
        // if (cookparms && verb)
        //     verb->notifyGroupParmListeners(cookparms->getNode(), 0, 1, static_cast<const GU_Detail*>(geo), geoGroup);
        
        if (!hasGroup && cookparms)
            cookparms->sopAddWarning(SOP_ERR_BADGROUP, groupName);
        
        return geoGroup;
    }

    SYS_FORCE_INLINE void setPrimitiveGroup(const UT_StringRef& groupName)
    { setGroup(GA_GROUP_PRIMITIVE, groupName); }
    
    SYS_FORCE_INLINE void setPointGroup(const UT_StringRef& groupName)
    { setGroup(GA_GROUP_POINT, groupName); }
    
    SYS_FORCE_INLINE void setVertexGroup(const UT_StringRef& groupName)
    { setGroup(GA_GROUP_VERTEX, groupName); }
    
    SYS_FORCE_INLINE void setEdgeGroup(const UT_StringRef& groupName)
    { setGroup(GA_GROUP_EDGE, groupName); }

    
    SYS_FORCE_INLINE bool isEmpty() const
    { return hasGroup ? ( bool(geoGroup) && geoGroup->isEmpty() ) : false; }
    
    SYS_FORCE_INLINE bool isFull() const
    { return hasGroup ? ( !bool(geoGroup) || (geoGroup->classType() != GA_GROUP_EDGE && !geoGroup->entries() == static_cast<const GA_ElementGroup*>(geoGroup)->getIndexMap().indexSize()) ) : true; }
    
    SYS_FORCE_INLINE bool getHasGroup() const
    { return hasGroup; }

    const GA_Group* getGroup() const
    {
        if (!hasGroup)
            return nullptr;
        return geoGroup;
    }

    const GA_ElementGroup* getElementGroup() const
    {
        if (!hasGroup || !geoGroup || !geoGroup->isElementGroup())
            return nullptr;
        return static_cast<const GA_ElementGroup*>(geoGroup);
    }

    const GA_Group* getGroup(const GA_GroupType groupType)
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

    const GA_Group* getGroup(const GA_AttributeOwner attribOwner)
    {
        if (!hasGroup)
            return nullptr;
        switch (attribOwner)
        {
        case GA_ATTRIB_PRIMITIVE: return getPrimitiveGroup(); break;
        case GA_ATTRIB_POINT:     return getPointGroup();     break;
        case GA_ATTRIB_VERTEX:    return getVertexGroup();    break;
        }
        UT_ASSERT_MSG(0, "Unhandled Attrib Owner");
        return nullptr;
    }

    // SYS_FORCE_INLINE const GA_Group* getGroup(const GA_AttributeOwner attribOwner)
    // { return getGroup(GFE_Type::attributeOwner_groupType(attribOwner)); }
    
    SYS_FORCE_INLINE const GA_Group* getGroup(const GA_Attribute& attrib)
    { return getGroup(attrib.getOwner()); }

    SYS_FORCE_INLINE const GA_Group* getGroup(const GA_Attribute* const attrib)
    { UT_ASSERT_P(attrib); return getGroup(*attrib); }

    SYS_FORCE_INLINE const GA_Group* getGroup(const GA_Group& group)
    { return getGroup(group.classType()); }

    SYS_FORCE_INLINE const GA_Group* getGroup(const GA_Group* const group)
    { UT_ASSERT_P(group); return getGroup(*group); }


    SYS_FORCE_INLINE const GA_ElementGroup* getElementGroup(const GA_GroupType groupType)
    { return static_cast<const GA_ElementGroup*>(getGroup(groupType)); }
    
    SYS_FORCE_INLINE const GA_ElementGroup* getElementGroup(const GA_AttributeOwner attribOwner)
    { return static_cast<const GA_ElementGroup*>(getGroup(attribOwner)); }

    
    SYS_FORCE_INLINE const GA_ElementGroup* getElementGroup(const GA_Attribute& attrib)
    { return getElementGroup(attrib.getOwner()); }
    
    SYS_FORCE_INLINE const GA_ElementGroup* getElementGroup(const GA_Attribute* const attrib)
    { UT_ASSERT_P(attrib); return getElementGroup(*attrib); }
    
    SYS_FORCE_INLINE const GA_ElementGroup* getElementGroup(const GA_Group& group)
    { return getElementGroup(group.classType()); }
    
    SYS_FORCE_INLINE const GA_ElementGroup* getElementGroup(const GA_Group* const group)
    { UT_ASSERT_P(group); return getElementGroup(*group); }

    
    
    GA_GroupType classType() const
    {
        if (!hasGroup)
            return GA_GROUP_INVALID;
        if (!geoGroup)
            return GA_GROUP_N;
        return geoGroup->classType();
    }

    SYS_FORCE_INLINE GA_GroupType groupType() const
    { return classType(); }

    SYS_FORCE_INLINE bool isPrimitiveGroup() const
    { return classType() == GA_GROUP_PRIMITIVE; }

    SYS_FORCE_INLINE bool isPointGroup() const
    { return classType() == GA_GROUP_POINT; }

    SYS_FORCE_INLINE bool isVertexGroup() const
    { return classType() == GA_GROUP_VERTEX; }

    SYS_FORCE_INLINE bool isEdgeGroup() const
    { return classType() == GA_GROUP_EDGE; }

    SYS_FORCE_INLINE const GA_PrimitiveGroup* getPrimitiveGroup()
    {
        if (!hasGroup)
            return nullptr;
        if (!hasPrimitiveGroup)
            setPrimitiveGroup();
        return geoPrimitiveGroup;
    }

    SYS_FORCE_INLINE const GA_PointGroup* getPointGroup()
    {
        if (!hasGroup)
            return nullptr;
        if (!hasPointGroup)
            setPointGroup();
        return geoPointGroup;
    }

    SYS_FORCE_INLINE const GA_VertexGroup* getVertexGroup()
    {
        if (!hasGroup)
            return nullptr;
        if (!hasVertexGroup)
            setVertexGroup();
        return geoVertexGroup;
    }

    SYS_FORCE_INLINE const GA_EdgeGroup* getEdgeGroup()
    {
        if (!hasGroup)
            return nullptr;
        if (!hasEdgeGroup)
            setEdgeGroup();
        return geoEdgeGroup;
    }
    
    SYS_FORCE_INLINE GA_Range getPrimitiveRange()
    { return GA_Range(geo->getPrimitiveMap(), getPrimitiveGroup()); }

    SYS_FORCE_INLINE GA_Range getPointRange()
    { return GA_Range(geo->getPointMap(), getPointGroup()); }

    SYS_FORCE_INLINE GA_Range getVertexRange()
    { return GA_Range(geo->getVertexMap(), getVertexGroup()); }

    SYS_FORCE_INLINE GA_Range getPrimitiveRange(const bool reverseGroup)
    { return GA_Range(geo->getPrimitiveMap(), getPrimitiveGroup(), reverseGroup); }

    SYS_FORCE_INLINE GA_Range getPointRange(const bool reverseGroup)
    { return GA_Range(geo->getPointMap(), getPointGroup(), reverseGroup); }

    SYS_FORCE_INLINE GA_Range getVertexRange(const bool reverseGroup)
    { return GA_Range(geo->getVertexMap(), getVertexGroup(), reverseGroup); }

    SYS_FORCE_INLINE GA_Range getRange(const GA_AttributeOwner attribOwner)
    {
        switch (attribOwner)
        {
        case GA_ATTRIB_PRIMITIVE: return getPrimitiveRange(); break;
        case GA_ATTRIB_POINT:     return getPointRange();     break;
        case GA_ATTRIB_VERTEX:    return getVertexRange();    break;
        default:                  return GA_Range();          break;
        }
    }
    
    
    SYS_FORCE_INLINE GA_Range getRange(const GA_Attribute& attrib)
    { return GA_Range(getIndexMap(attrib), getElementGroup(attrib)); }

    SYS_FORCE_INLINE GA_Range getRange(const GA_Attribute* const attrib)
    { UT_ASSERT_P(attrib); return getRange(*attrib); }

    SYS_FORCE_INLINE GA_Range getRange(const GA_Attribute& attrib, const bool reverseGroup)
    { return GA_Range(getIndexMap(attrib), getElementGroup(attrib), reverseGroup); }

    SYS_FORCE_INLINE GA_Range getRange(const GA_Attribute* const attrib, const bool reverseGroup)
    { UT_ASSERT_P(attrib); return getRange(*attrib, reverseGroup); }

#if 1
    SYS_FORCE_INLINE GA_Range getRange(const GA_Group& group)
    { return GA_Range(getIndexMap(group), getElementGroup(group)); }

    SYS_FORCE_INLINE GA_Range getRange(const GA_Group* const group)
    { UT_ASSERT_P(group); return getRange(*group); }

    SYS_FORCE_INLINE GA_Range getRange(const GA_Group& group, const bool reverseGroup)
    { return GA_Range(getIndexMap(group), getElementGroup(group), reverseGroup); }

    SYS_FORCE_INLINE GA_Range getRange(const GA_Group* const group, const bool reverseGroup)
    { UT_ASSERT_P(group); return getRange(*group, reverseGroup); }
#else
    SYS_FORCE_INLINE GA_Range getRange(const GA_ElementGroup& group)
    { return GA_Range(getIndexMap(group), getElementGroup(group)); }

    SYS_FORCE_INLINE GA_Range getRange(const GA_ElementGroup* const group)
    { UT_ASSERT_P(group); return getRange(*group); }

    SYS_FORCE_INLINE GA_Range getRange(const GA_ElementGroup& group, const bool reverseGroup)
    { return GA_Range(getIndexMap(group), getElementGroup(group), reverseGroup); }

    SYS_FORCE_INLINE GA_Range getRange(const GA_ElementGroup* const group, const bool reverseGroup)
    { UT_ASSERT_P(group); return getRange(*group, reverseGroup); }
#endif
    
    SYS_FORCE_INLINE GA_SplittableRange getPrimitiveSplittableRange()
    { return GA_SplittableRange(getPrimitiveRange()); }
    
    SYS_FORCE_INLINE GA_SplittableRange getPointSplittableRange()
    { return GA_SplittableRange(getPointRange()); }
    
    SYS_FORCE_INLINE GA_SplittableRange getVertexSplittableRange()
    { return GA_SplittableRange(getVertexRange()); }

    SYS_FORCE_INLINE GA_SplittableRange getSplittableRange(const GA_AttributeOwner attribOwner)
    {
        switch (attribOwner)
        {
        case GA_ATTRIB_PRIMITIVE: return getPrimitiveSplittableRange(); break;
        case GA_ATTRIB_POINT:     return getPointSplittableRange();     break;
        case GA_ATTRIB_VERTEX:    return getVertexSplittableRange();    break;
        default:                  return GA_SplittableRange();          break;
        }
    }

    SYS_FORCE_INLINE GA_SplittableRange getSplittableRange(const GA_GroupType groupType)
    {
        switch (groupType)
        {
        case GA_GROUP_PRIMITIVE: return getPrimitiveSplittableRange(); break;
        case GA_GROUP_POINT:     return getPointSplittableRange();     break;
        case GA_GROUP_VERTEX:    return getVertexSplittableRange();    break;
        default:                 return GA_SplittableRange();          break;
        }
    }
    
    SYS_FORCE_INLINE GA_SplittableRange getSplittableRange(const GA_Attribute& attrib)
    { return getSplittableRange(attrib.getOwner()); }
    
    SYS_FORCE_INLINE GA_SplittableRange getSplittableRange(const GA_Attribute* const attrib)
    { UT_ASSERT_P(attrib); return getSplittableRange(*attrib); }
    
    SYS_FORCE_INLINE GA_SplittableRange getSplittableRange(const GA_Group& group)
    { return getSplittableRange(group.classType()); }
    
    SYS_FORCE_INLINE GA_SplittableRange getSplittableRange(const GA_Group* const group)
    { UT_ASSERT_P(group); return getSplittableRange(*group); }
    


    SYS_FORCE_INLINE GOP_Manager* getGOP() const
    { return gop; }

    SYS_FORCE_INLINE GOP_Manager& getGOPRef() const
    { return *gop; }



private:

    SYS_FORCE_INLINE const GA_IndexMap& getIndexMap(const GA_AttributeOwner attribOwner) const
    { return geo->getIndexMap(attribOwner); }

    SYS_FORCE_INLINE const GA_IndexMap& getIndexMap(const GA_GroupType groupType) const
    { return getIndexMap(GFE_Type::attributeOwner_groupType(groupType)); }

        
    SYS_FORCE_INLINE const GA_IndexMap& getIndexMap(const GA_Attribute& attrib) const
    { return getIndexMap(attrib.getOwner()); }
        
    SYS_FORCE_INLINE const GA_IndexMap& getIndexMap(const GA_Attribute* const attrib) const
    { return getIndexMap(*attrib); }

    SYS_FORCE_INLINE const GA_IndexMap& getIndexMap(const GA_Group& group) const
    { return getIndexMap(group.classType()); }

    SYS_FORCE_INLINE const GA_IndexMap& getIndexMap(const GA_Group* const group) const
    { return getIndexMap(*group); }
    
    // SYS_FORCE_INLINE const GA_IndexMap& getIndexMap(const GA_ElementGroup& group) const
    // { return getIndexMap(GFE_Type::attributeOwner_groupType(group.classType())); }
    //
    // SYS_FORCE_INLINE const GA_IndexMap& getIndexMap(const GA_ElementGroup* const group) const
    // { return getIndexMap(*group); }



    
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

        gop->destroyAdhocGroup(geoGroup);
        gop->destroyAdhocGroup(geoPrimitiveGroup);
        gop->destroyAdhocGroup(geoPointGroup);
        gop->destroyAdhocGroup(geoVertexGroup);
        gop->destroyAdhocGroup(geoEdgeGroup);

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
            gop->appendAdhocGroup(const_cast<GA_PrimitiveGroup*>(geoPrimitiveGroup), false);
    }


    void setPointGroup()
    {
        if (!hasGroup)
            return;

        hasPointGroup = true;
        geoPointGroup = groupFindPromotePointDetached(geoGroup);
        if (geoPointGroup && geoPointGroup->isDetached())
            gop->appendAdhocGroup(const_cast<GA_PointGroup*>(geoPointGroup), false);
    }


    void setVertexGroup()
    {
        if (!hasGroup)
            return;

        hasVertexGroup = true;
        geoVertexGroup = groupFindPromoteVertexDetached(geoGroup);
        if (geoVertexGroup && geoVertexGroup->isDetached())
            gop->appendAdhocGroup(const_cast<GA_VertexGroup*>(geoVertexGroup), false);
    }


    void setEdgeGroup()
    {
        if (!hasGroup)
            return;

        hasEdgeGroup = true;
        geoEdgeGroup = groupFindPromoteEdgeDetached(geoGroup);
        if (geoEdgeGroup && geoEdgeGroup->isDetached())
            gop->appendAdhocGroup(const_cast<GA_EdgeGroup*>(geoEdgeGroup), false);
    }




    const GA_Group* groupFindPromoteDetached(const GA_Group* const group, const GA_GroupType groupType)
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

        gop->appendAdhocGroup(newGroup, false);

        return newGroup;
    }

    SYS_FORCE_INLINE const GA_PrimitiveGroup* groupFindPromotePrimitiveDetached(const GA_Group* const group)
    { return static_cast<const GA_PrimitiveGroup*>(groupFindPromoteDetached(group, GA_GROUP_PRIMITIVE)); }

    SYS_FORCE_INLINE const GA_PointGroup* groupFindPromotePointDetached(const GA_Group* const group)
    { return static_cast<const GA_PointGroup*>(groupFindPromoteDetached(group, GA_GROUP_POINT)); }

    SYS_FORCE_INLINE const GA_VertexGroup* groupFindPromoteVertexDetached(const GA_Group* const group)
    { return static_cast<const GA_VertexGroup*>(groupFindPromoteDetached(group, GA_GROUP_VERTEX)); }

    SYS_FORCE_INLINE const GA_EdgeGroup* groupFindPromoteEdgeDetached(const GA_Group* const group)
    { return static_cast<const GA_EdgeGroup*>(groupFindPromoteDetached(group, GA_GROUP_EDGE)); }


protected:

    SYS_FORCE_INLINE void setDetail(const GA_Detail* const geo)
    { this->geo = static_cast<const GEO_Detail*>(geo); }

    SYS_FORCE_INLINE void setDetail(const GEO_Detail* const geo)
    { this->geo = geo; }

    SYS_FORCE_INLINE void setDetail(const GU_Detail* const geo)
    { this->geo = geo; }

    SYS_FORCE_INLINE void setDetail(GA_Detail* const geo)
    {
        this->geo = static_cast<const GEO_Detail*>(geo);
        geoNonconst = geo;
    }

    SYS_FORCE_INLINE void setDetail(GEO_Detail* const geo)
    {
        this->geo = geo;
        geoNonconst = geo;
    }

    SYS_FORCE_INLINE void setDetail(GU_Detail* const geo)
    {
        this->geo = geo;
        geoNonconst = geo;
    }


public:
    bool findGroup = true;
    //bool delGroup = false;

    
private:

    //GEO_Detail* geo = nullptr;
    const SOP_NodeVerb::CookParms* cookparms;
    //const SOP_NodeVerb* verb = nullptr;
    const GEO_Detail* geo;
    GA_Detail* geoNonconst;
        
    GOP_Manager* gop;


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
    const UT_StringRef& groupName,
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

    // if (cookparms && verb)
    //     verb->notifyGroupParmListeners(cookparms->getNode(), 0, 1, geo, geoGroup);
    
    //if (!success || (anyGroup && !anyGroup->isElementGroup()))
    if (!success)
    {
        cookparms.sopAddWarning(SOP_ERR_BADGROUP, groupName);
        return nullptr;
    }
    
    return anyGroup;
}

static const GA_Group*
findOrParseGroupDetached(
    const SOP_NodeVerb::CookParms& cookparms,
    const GA_Detail* const geo,
    const GA_GroupType groupType,
    const UT_StringRef& groupName,
    GOP_Manager& gop
)
{
    UT_ASSERT_P(geo);
    if (!groupName.isstring())
    {
        cookparms.sopAddWarning(SOP_ERR_BADGROUP, groupName);
        return nullptr;
    }

    if (!groupName.length())
        return nullptr;

    const GA_GroupTable* const groupTable = geo->getGroupTable(groupType);
    if (groupTable)
    {
        const GA_Group* const anyGroup = groupTable->find(groupName);
        if (anyGroup)
            return anyGroup;
    }

    bool success = true;
    const GA_Group* const anyGroup = gop.parseGroupDetached(groupName, groupType, static_cast<const GEO_Detail*>(geo), true, false, success);

    // if (cookparms && verb)
    //     verb->notifyGroupParmListeners(cookparms->getNode(), 0, 1, geo, geoGroup);
    
    //if (!success || (anyGroup && !anyGroup->isElementGroup()))
    if (!success)
    {
        cookparms.sopAddWarning(SOP_ERR_BADGROUP, groupName);
        return nullptr;
    }
    
    return anyGroup;
}




SYS_FORCE_INLINE
static const GA_PrimitiveGroup*
findOrParsePrimitiveGroupDetached(
    const SOP_NodeVerb::CookParms& cookparms,
    const GA_Detail* const geo,
    const UT_StringRef& groupName,
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
    const UT_StringRef& groupName,
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
    const UT_StringRef& groupName,
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
    const UT_StringRef& groupName,
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
    const UT_StringRef& groupName,
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
    const UT_StringRef& groupName,
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
    const UT_StringRef& groupName,
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
    const UT_StringRef& groupName,
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
    const UT_StringRef& groupName,
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
    const UT_StringRef& groupName,
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
    const UT_StringRef& groupName,
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
    const UT_StringRef& groupName,
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
    const UT_StringRef& groupName,
    GOP_Manager& gop
)
{
    return static_cast<const GA_EdgeGroup*>(parseGroupDetached(cookparms, geo, GA_GROUP_EDGE, groupName, gop));
}



















} // End of namespace GFE_GroupParser_Namespace

#endif
