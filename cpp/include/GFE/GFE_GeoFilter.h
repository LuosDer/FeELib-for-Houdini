
#pragma once

#ifndef __GFE_GeoFilter_h__
#define __GFE_GeoFilter_h__


//#include "GFE/GFE_GeoFilter.h"


//#include "GA/GA_Detail.h"


#include "GFE/GFE_Detail.h"
#include "GFE/GFE_AttributeArray.h"
//#include "GFE/GFE_Attribute.h"
#include "GFE/GFE_GroupParser.h"
//#include "GFE/GFE_GroupPromote.h"




class GFE_GeoFilter {

public:
    //
    // GFE_GeoFilter(
    //     const GFE_Detail* const geo,
    //     const SOP_NodeVerb::CookParms* const cookparms = nullptr
    // )
    //     : geo(nullptr)
    //     , geoConst(geo)
    //     , cookparms(cookparms)
    //     , groupParser(geo, gop, cookparms)
    // {
    // }
    //
    // GFE_GeoFilter(
    //     const GA_Detail& geo,
    //     const SOP_NodeVerb::CookParms* const cookparms = nullptr
    // )
    //     : geo(nullptr)
    //     , geoConst(static_cast<const GFE_Detail*>(&geo))
    //     , cookparms(cookparms)
    //     , groupParser(geo, gop, cookparms)
    // {
    // }
    //
    // GFE_GeoFilter(
    //     const GA_Detail* const geo,
    //     const SOP_NodeVerb::CookParms* const cookparms = nullptr
    // )
    //     : geo(nullptr)
    //     , geoConst(static_cast<const GFE_Detail*>(&geo))
    //     , cookparms(cookparms)
    //     , groupParser(*geo, gop, cookparms)
    // {
    //     //UT_ASSERT_MSG(geo, "do not find geo");
    // }







    
    GFE_GeoFilter(
        GFE_Detail* const geo,
        const SOP_NodeVerb::CookParms* const cookparms = nullptr
    )
        : geo(geo)
        //, geoConst(geo)
        , cookparms(cookparms)
        , groupParser(geo, gop, cookparms)
    {
    }

    GFE_GeoFilter(
        GA_Detail& geo,
        const SOP_NodeVerb::CookParms* const cookparms = nullptr
    )
        : geo(static_cast<GFE_Detail*>(&geo))
        //, geoConst(static_cast<GFE_Detail*>(&geo))
        , cookparms(cookparms)
        , groupParser(geo, gop, cookparms)
    {
    }

    GFE_GeoFilter(
        GA_Detail& geo,
        const SOP_NodeVerb::CookParms& cookparms
    )
        : geo(static_cast<GFE_Detail*>(&geo))
        //, geoConst(static_cast<GFE_Detail*>(&geo))
        , cookparms(&cookparms)
        , groupParser(geo, gop, cookparms)
    {
        //UT_ASSERT_MSG(geo, "do not find geo");
    }

    GFE_GeoFilter(
        GA_Detail* const geo,
        const SOP_NodeVerb::CookParms* const cookparms = nullptr
    )
        : geo(static_cast<GFE_Detail*>(geo))
        //, geoConst(static_cast<GFE_Detail*>(&geo))
        , cookparms(cookparms)
        , groupParser(*geo, gop, cookparms)
    {
        //UT_ASSERT_MSG(geo, "do not find geo");
    }

    GFE_GeoFilter(
        GA_Detail* const geo,
        const SOP_NodeVerb::CookParms& cookparms
    )
        : geo(static_cast<GFE_Detail*>(geo))
        //, geoConst(static_cast<GFE_Detail*>(&geo))
        , cookparms(&cookparms)
        , groupParser(*geo, gop, cookparms)
    {
        //UT_ASSERT_MSG(geo, "do not find geo");
    }

    ~GFE_GeoFilter()
    {
        delTopoAttrib();
    }

    virtual void
        reset(
            GA_Detail* const inGeo,
            const SOP_NodeVerb::CookParms* const cookparms = nullptr
        )
    {
        geo = static_cast<GFE_Detail*>(inGeo);
        this->cookparms = cookparms;
        groupParser.reset(geo, cookparms);
        setHasComputed();
    }

    virtual void
        reset(
            GA_Detail& inGeo,
            const SOP_NodeVerb::CookParms* const cookparms = nullptr
        )
    {
        geo = static_cast<GFE_Detail*>(&inGeo);
        this->cookparms = cookparms;
        groupParser.reset(geo, cookparms);
        setHasComputed();
    }


    SYS_FORCE_INLINE bool getHasComputed() const
    {
        return hasComputed;
    }

    
    // SYS_FORCE_INLINE
    // void
    //     setOutTopoAttrib(
    //         const bool outTopoAttrib = true
    //     )
    // {
    //     this->outTopoAttrib = outTopoAttrib;
    // }
    //
    // SYS_FORCE_INLINE
    // bool
    //     getOutTopoAttrib() const
    // {
    //     return outTopoAttrib;
    // }
    

    GFE_GroupParser& getGroupParser()
    {
        return groupParser;
    }
    

    SYS_FORCE_INLINE void compute()
    {
        hasComputed = computeCore();
    }


    SYS_FORCE_INLINE void bumpDataIdsForAddOrRemove(
        const bool added_or_removed_points,
        const bool added_or_removed_vertices,
        const bool added_or_removed_primitives
    ) const
    {
        geo->bumpDataIdsForAddOrRemove(added_or_removed_points, added_or_removed_vertices, added_or_removed_primitives);
    }

    virtual void bumpDataIdsForAddOrRemove() const
    {
        geo->bumpDataIdsForAddOrRemove(true, true, true);
    }

    SYS_FORCE_INLINE void computeAndBumpDataIdsForAddOrRemove(
        const bool added_or_removed_points,
        const bool added_or_removed_vertices,
        const bool added_or_removed_primitives
    )
    {
        compute();
        bumpDataIdsForAddOrRemove(added_or_removed_points, added_or_removed_vertices, added_or_removed_primitives);
    }

    SYS_FORCE_INLINE void computeAndBumpDataIdsForAddOrRemove()
    {
        compute();
        bumpDataIdsForAddOrRemove();
    }



    virtual void bumpDataId() const
    {
    }

    SYS_FORCE_INLINE void computeAndBumpDataId()
    {
        compute();
        bumpDataId();
    }

    SYS_FORCE_INLINE GFE_Detail* getDetail() const
    {
        return geo;
    }

    


private:

    virtual bool
        computeCore()
    {
        if (groupParser.getGroup() && groupParser.getGroup()->isEmpty())
            return false;
        return true;
    }


private:
    SYS_FORCE_INLINE virtual void setDetailBase(GA_Detail& inGeo)
    {
        geo = static_cast<GFE_Detail*>(&inGeo);
        groupParser.setDetail(geo);
    }

    virtual void
    delTopoAttrib() const
    {
        if (outTopoAttrib)
            return;
        GA_AttributeSet& attribSet = geo->getAttributes();
        GA_AttributeFilter filter = GA_AttributeFilter::selectByPattern("__topo_*");
        filter = GA_AttributeFilter::selectAnd(filter, GA_AttributeFilter::selectPublic());
        filter = GA_AttributeFilter::selectAnd(filter, GA_AttributeFilter::selectNot(GA_AttributeFilter::selectGroup()));
        attribSet.destroyAttributes(GA_ATTRIB_PRIMITIVE, filter);
        attribSet.destroyAttributes(GA_ATTRIB_POINT,     filter);
        attribSet.destroyAttributes(GA_ATTRIB_VERTEX,    filter);
        attribSet.destroyAttributes(GA_ATTRIB_DETAIL,    filter);

        GA_GroupTable* groupTable = nullptr;
        GA_Group* groupPtr = nullptr;
        for (GA_GroupType groupType : {GA_GROUP_PRIMITIVE, GA_GROUP_POINT, GA_GROUP_VERTEX, GA_GROUP_EDGE})
        {
            groupTable = geo->getGroupTable(groupType);
            //for (GA_GroupTable::iterator it = groupTable->beginTraverse(); !it.atEnd(); it.operator++())
            for (GA_GroupTable::iterator<GA_Group> it = groupTable->beginTraverse(); !it.atEnd(); ++it)
            {
                groupPtr = it.group();
                //if (groupPtr->isDetached())
                //    continue;
                if (!groupPtr->getName().startsWith("__topo_"))
                    continue;
                groupTable->destroy(groupPtr);
            }
        }
    }

    
protected:

    SYS_FORCE_INLINE void setDetail(GA_Detail* const inGeo)
    {
        setDetailBase(*inGeo);
    }
    
    SYS_FORCE_INLINE void setDetail(GA_Detail& inGeo)
    {
        setDetail(&inGeo);
    }
    

    SYS_FORCE_INLINE void setDetail(GA_Attribute& attrib)
    {
        setDetail(attrib.getDetail());
    }

    SYS_FORCE_INLINE void setDetail(GA_ElementGroup& group)
    {
        setDetail(static_cast<GA_ElementGroup&>(group).getDetail());
    }

    SYS_FORCE_INLINE void setDetail(GA_Group& group)
    {
        UT_ASSERT_MSG(group.classType() != GA_GROUP_EDGE, "can not be edge group");
#if 1
        setDetail(static_cast<GA_ElementGroup&>(group));
#else
        if (group.classType() == GA_GROUP_EDGE)
            setDetail(static_cast<GA_EdgeGroup&>(group).getDetail());
        else
            setDetail(static_cast<GA_ElementGroup&>(group));
#endif
    }
    
    SYS_FORCE_INLINE
    void
        setHasComputed(
            const bool hasComputed = false
        )
    {
        this->hasComputed = hasComputed;
    }


public:
    GFE_GroupParser groupParser;
    bool outTopoAttrib = true;

protected:
    GFE_Detail* geo;
    //const GFE_Detail* geoConst;
    const SOP_NodeVerb::CookParms* cookparms;

private:
    //bool outTopoAttrib = true;
    GOP_Manager gop;
    bool hasComputed = false;
};








class GFE_GeoFilterRef {

public:

    GFE_GeoFilterRef(
        const GFE_Detail* const geoRef0,
        GOP_Manager& gop,
        const SOP_NodeVerb::CookParms* const cookparms = nullptr
    )
        : geoRef0(geoRef0)
        , cookparmsRef0(cookparms)
        , gopRef0(&gop)
        , groupParserRef0(geoRef0, gop, cookparms)
        , ref0AttribArray(geoRef0, cookparms)
        , ref0GroupArray(geoRef0, cookparms)
    {
        //UT_ASSERT_MSG(geoRef0, "do not find geo");
    }

    GFE_GeoFilterRef(
        const GA_Detail* const geoRef0,
        GOP_Manager& gop,
        const SOP_NodeVerb::CookParms* const cookparms = nullptr
    )
        : geoRef0(static_cast<const GFE_Detail*>(geoRef0))
        , cookparmsRef0(cookparms)
        , gopRef0(&gop)
        , groupParserRef0(geoRef0, gop, cookparms)
        , ref0AttribArray(geoRef0, cookparms)
        , ref0GroupArray(geoRef0, cookparms)
    {
        //UT_ASSERT_MSG(geoRef0, "do not find geo");
    }

    GFE_GeoFilterRef(
        const GA_Detail* const geoRef0,
        GOP_Manager& gop,
        const SOP_NodeVerb::CookParms& cookparms
    )
        : geoRef0(static_cast<const GFE_Detail*>(geoRef0))
        , cookparmsRef0(&cookparms)
        , gopRef0(&gop)
        , groupParserRef0(geoRef0, gop, cookparms)
        , ref0AttribArray(geoRef0, cookparms)
        , ref0GroupArray(geoRef0, cookparms)
    {
        //UT_ASSERT_MSG(geoRef0, "do not find geo");
    }

    GFE_GeoFilterRef(
        const GA_Detail& geoRef0,
        GOP_Manager& gop,
        const SOP_NodeVerb::CookParms* const cookparms = nullptr
    )
        : geoRef0(static_cast<const GFE_Detail*>(&geoRef0))
        , cookparmsRef0(cookparms)
        , gopRef0(&gop)
        , groupParserRef0(geoRef0, gop, cookparms)
        , ref0AttribArray(geoRef0, cookparms)
        , ref0GroupArray(geoRef0, cookparms)
    {
    }

    GFE_GeoFilterRef(
        const GA_Detail& geoRef0,
        GOP_Manager& gop,
        const SOP_NodeVerb::CookParms& cookparms
    )
        : geoRef0(static_cast<const GFE_Detail*>(&geoRef0))
        , cookparmsRef0(&cookparms)
        , gopRef0(&gop)
        , groupParserRef0(geoRef0, gop, cookparms)
        , ref0AttribArray(geoRef0, cookparms)
        , ref0GroupArray(geoRef0, cookparms)
    {
    }

    ~GFE_GeoFilterRef()
    {
    }

    

    void resetGeoFilterRef0(
        const GFE_Detail* const geoRef0,
        GOP_Manager* const gop,
        const SOP_NodeVerb::CookParms* const cookparms = nullptr
    )
    {
        this->geoRef0 = geoRef0;
        cookparmsRef0 = cookparms;
        gopRef0 = gop;
        UT_ASSERT_P(gopRef0);
        ref0AttribArray.reset(geoRef0, cookparms);
        ref0GroupArray.reset(geoRef0, cookparms);
    }
    
    SYS_FORCE_INLINE void resetGeoFilterRef0(
        const GA_Detail* const geoRef0,
        GOP_Manager* const gop,
        const SOP_NodeVerb::CookParms* const cookparms = nullptr
    )
    {
        resetGeoFilterRef0(static_cast<const GFE_Detail*>(geoRef0), gop, cookparmsRef0);
    }

    SYS_FORCE_INLINE GFE_GroupParser& getRef0GroupParser()
    {
        return groupParserRef0;
    }



    SYS_FORCE_INLINE GFE_RefAttribArray& getRef0AttribArray()
    {
        return ref0AttribArray;
    }

    SYS_FORCE_INLINE GFE_RefGroupArray& getRef0GroupArray()
    {
        return ref0GroupArray;
    }

    SYS_FORCE_INLINE ::std::vector<const GA_Attribute*>& getRef0AttribArrayRef()
    {
        return ref0AttribArray.ref();
    }

    SYS_FORCE_INLINE ::std::vector<const GA_Group*>& getRef0GroupArrayRef()
    {
        return ref0GroupArray.ref();
    }


private:
    SYS_FORCE_INLINE virtual void setRef0DetailBase(const GFE_Detail* const inGeo)
    {
        geoRef0 = inGeo;
        ref0AttribArray.setDetail(inGeo);
        ref0GroupArray.setDetail(inGeo);
    }

    SYS_FORCE_INLINE virtual void setRef0DetailBase(const GA_Detail* const inGeo)
    {
        setRef0DetailBase(static_cast<const GFE_Detail*>(inGeo));
    }



public:
    GFE_GroupParser groupParserRef0;

protected:
    const GFE_Detail* geoRef0;

private:
    const SOP_NodeVerb::CookParms* cookparmsRef0;
    GOP_Manager* gopRef0;

    GFE_RefAttribArray ref0AttribArray;
    GFE_RefGroupArray ref0GroupArray;
};








class GFE_AttribFilter : public GFE_GeoFilter {

public:

    //using GFE_GeoFilter::GFE_GeoFilter;
    //
    // GFE_AttribFilter(
    //     const GA_Detail& geo,
    //     const SOP_NodeVerb::CookParms* const cookparms = nullptr
    // )
    //     : GFE_GeoFilter(geo, cookparms)
    //     , outAttribArray(geo, cookparms)
    //     , outGroupArray(geo, cookparms)
    // {
    // }
    //
    // GFE_AttribFilter(
    //     const GA_Detail* const geo,
    //     const SOP_NodeVerb::CookParms* const cookparms = nullptr
    // )
    //     : GFE_GeoFilter(geo, cookparms)
    //     , outAttribArray(geo, cookparms)
    //     , outGroupArray(geo, cookparms)
    // {
    // }
    
    GFE_AttribFilter(
        GFE_Detail* const geo,
        const SOP_NodeVerb::CookParms* const cookparms = nullptr
    )
        : GFE_GeoFilter(geo, cookparms)
        , outAttribArray(geo, cookparms)
        , outGroupArray(geo, cookparms)
    {
    }

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
        GA_Detail* const geo,
        const SOP_NodeVerb::CookParms& cookparms
    )
        : GFE_GeoFilter(geo, cookparms)
        , outAttribArray(geo, cookparms)
        , outGroupArray(geo, cookparms)
    {
    }

    GFE_AttribFilter(
        GA_Detail& geo,
        const SOP_NodeVerb::CookParms* const cookparms = nullptr
    )
        : GFE_GeoFilter(geo, cookparms)
        , outAttribArray(geo, cookparms)
        , outGroupArray(geo, cookparms)
    {
    }

    GFE_AttribFilter(
        GA_Detail& geo,
        const SOP_NodeVerb::CookParms& cookparms
    )
        : GFE_GeoFilter(geo, cookparms)
        , outAttribArray(geo, cookparms)
        , outGroupArray(geo, cookparms)
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

    virtual void
        reset(
            GA_Detail& geo,
            const SOP_NodeVerb::CookParms* const cookparms = nullptr
        ) override
    {
        GFE_GeoFilter::reset(geo, cookparms);
        outAttribArray.reset(geo, cookparms);
        outGroupArray.reset(geo, cookparms);
    }
    
    virtual void bumpDataId() const override
    {
        if (doDelOutGroup)
            bumpDataIdsForAddOrRemove();
        else
        {
            outAttribArray.bumpDataId();
            outGroupArray.bumpDataId();
        }
    }

    SYS_FORCE_INLINE void findOrCreateOutGroup(
        const bool detached,
        const GA_GroupType groupType = GA_GROUP_POINT,
        const UT_StringHolder& groupName = ""
    )
    {
        getOutGroupArray().findOrCreate(detached, groupType, groupName);
    }

    SYS_FORCE_INLINE void findOrCreateOutGroup(const GA_GroupType groupType = GA_GROUP_POINT,const UT_StringHolder& groupName = "")
    {
        getOutGroupArray().findOrCreate(doDelOutGroup, groupType, groupName);
    }
    
    virtual void delOutGroup()
    {
        if (outGroupArray.isEmpty() || !outGroupArray[0])
            return;

        const GA_Group* const outGroup = outGroupArray[0];

        if (outGroup->classType() == GA_GROUP_EDGE)
        {

        }
        else
        {
            const GA_ElementGroup* const outElementGroup = static_cast<const GA_ElementGroup*>(outGroup);
            //GA_Range range(outElementGroup->getIndexMap());
            GA_Range range(*outElementGroup);
            switch (outGroup->classType())
            {
            case GA_GROUP_PRIMITIVE:
                geo->destroyPrimitiveOffsets(range, true);
                break;
            case GA_GROUP_POINT:
                geo->destroyPointOffsets(range, GA_Detail::GA_DestroyPointMode::GA_DESTROY_DEGENERATE_INCOMPATIBLE);
                break;
            case GA_GROUP_VERTEX:
                geo->destroyVertexOffsets(range);
                break;
            }
        }
    }

    virtual SYS_FORCE_INLINE void visualizeOutGroup()
    {
        if (doDelOutGroup || !cookparms || outGroupArray.isEmpty() || !outGroupArray[0] || outGroupArray[0]->isDetached())
            return;
        //cookparms->getNode()->setHighlight(true);
        cookparms->select(*outGroupArray[0]);
    }

    void SYS_FORCE_INLINE delOrVisualizeOutGroup()
    {
        doDelOutGroup ? delOutGroup() : visualizeOutGroup();
        // if (doDelOutGroup)
        //     delOutGroup();
        // else
        //     visualizeOutGroup();
    }

    






    SYS_FORCE_INLINE GFE_AttributeArray& getOutAttribArray()
    {
        return outAttribArray;
    }

    SYS_FORCE_INLINE GFE_GroupArray& getOutGroupArray()
    {
        return outGroupArray;
    }

    SYS_FORCE_INLINE ::std::vector<GA_Attribute*>& getOutAttribArrayRef()
    {
        return outAttribArray.ref();
    }

    SYS_FORCE_INLINE ::std::vector<GA_Group*>& getOutGroupArrayRef()
    {
        return outGroupArray.ref();
    }


    
    SYS_FORCE_INLINE const GFE_AttributeArray& getOutAttribArray() const
    {
        return outAttribArray;
    }

    SYS_FORCE_INLINE const GFE_GroupArray& getOutGroupArray() const
    {
        return outGroupArray;
    }

    SYS_FORCE_INLINE const ::std::vector<GA_Attribute*>& getOutAttribArrayRef() const
    {
        return outAttribArray.ref();
    }

    SYS_FORCE_INLINE const ::std::vector<GA_Group*>& getOutGroupArrayRef() const
    {
        return outGroupArray.ref();
    }

private:

    SYS_FORCE_INLINE virtual void setDetailBase(GA_Detail& inGeo) override
    {
        setDetail(inGeo);
        outAttribArray.setDetail(inGeo);
        outGroupArray.setDetail(inGeo);
    }



public:
    bool reverseOutGroup = false;
    bool doDelOutGroup = false;
private:
    //::std::vector<GA_AttributeUPtr> attribUPtrArray;
    //::std::vector<GA_Attribute*> attribArray;
    //::std::vector<GA_GroupUPtr> groupUPtrArray;
    //::std::vector<GA_Group*> groupArray;
    GFE_AttributeArray outAttribArray;
    GFE_GroupArray outGroupArray;
};




class GFE_AttribCreateFilter : public GFE_AttribFilter {

public:

    GFE_AttribCreateFilter(
        GFE_Detail* const geo,
        const SOP_NodeVerb::CookParms* const cookparms = nullptr
    )
        : GFE_AttribFilter(geo, cookparms)
        , inAttribArray(geo, cookparms)
        , inGroupArray(geo, cookparms)
    {
    }

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
        GA_Detail* const geo,
        const SOP_NodeVerb::CookParms& cookparms
    )
        : GFE_AttribFilter(geo, cookparms)
        , inAttribArray(geo, cookparms)
        , inGroupArray(geo, cookparms)
    {
    }

    GFE_AttribCreateFilter(
        GA_Detail& geo,
        const SOP_NodeVerb::CookParms* const cookparms = nullptr
    )
        : GFE_AttribFilter(geo, cookparms)
        , inAttribArray(geo, cookparms)
        , inGroupArray(geo, cookparms)
    {
    }

    GFE_AttribCreateFilter(
        GA_Detail& geo,
        const SOP_NodeVerb::CookParms& cookparms
    )
        : GFE_AttribFilter(geo, cookparms)
        , inAttribArray(geo, cookparms)
        , inGroupArray(geo, cookparms)
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

    SYS_FORCE_INLINE GFE_AttributeArray& getInAttribArray()
    {
        return inAttribArray;
    }
    SYS_FORCE_INLINE GFE_GroupArray& getInGroupArray()
    {
        return inGroupArray;
    }
    SYS_FORCE_INLINE ::std::vector<GA_Attribute*>& getInAttribArrayRef()
    {
        return inAttribArray.ref();
    }
    SYS_FORCE_INLINE ::std::vector<GA_Group*>& getInGroupArrayRef()
    {
        return inGroupArray.ref();
    }


    SYS_FORCE_INLINE void setInAttribBumpDataId(bool val = true)
    {
        inAttribBumpDataId = val;
    }
    SYS_FORCE_INLINE void setInGroupBumpDataId(bool val = true)
    {
        inGroupBumpDataId = val;
    }

    virtual void bumpDataId() const override
    {
        GFE_AttribFilter::bumpDataId();
        if (inAttribBumpDataId)
            inAttribArray.bumpDataId();
        if (inGroupBumpDataId)
            inGroupArray.bumpDataId();
    }

    void visualizeInGroup()
    {
        if (!cookparms || inGroupArray.isEmpty() || !inGroupArray[0] || inGroupArray[0]->isDetached())
            return;
        //cookparms->getNode()->setHighlight(true);
        cookparms->select(*inGroupArray[0]);
    }


private:

    SYS_FORCE_INLINE virtual void setDetailBase(GA_Detail& inGeo) override
    {
        setDetail(inGeo);
        inAttribArray.setDetail(inGeo);
        inGroupArray.setDetail(inGeo);
    }


private:
    bool inAttribBumpDataId = false;
    bool inGroupBumpDataId = false;
    GFE_AttributeArray inAttribArray;
    GFE_GroupArray inGroupArray;
};










#endif
