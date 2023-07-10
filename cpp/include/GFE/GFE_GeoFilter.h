
#pragma once

#ifndef __GFE_GeoFilter_h__
#define __GFE_GeoFilter_h__


#include "GFE/GFE_GeoFilter.h"



#include "GFE/GFE_Detail.h"
#include "GFE/GFE_AttributeArray.h"
#include "GFE/GFE_GroupParser.h"
#include "GFE/GFE_NodeVerb.h"



class GFE_GeoFilter {

public:
    
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
        GA_Detail* const geo,
        const SOP_NodeVerb::CookParms* const cookparms = nullptr
    )
        : geo(static_cast<GFE_Detail*>(geo))
        //, geoConst(static_cast<GFE_Detail*>(&geo))
        , cookparms(cookparms)
        , groupParser(*geo, gop, cookparms)
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
    }


    
    GFE_GeoFilter(
        GFE_Detail* const geo,
        const GA_Detail* const geoSrc,
        const SOP_NodeVerb::CookParms* const cookparms = nullptr
    )
        : geo(geo)
        , geoSrc(geoSrc)
        , cookparms(cookparms)
        , groupParser(geoSrc ? geoSrc : geo, gop, cookparms)
    {
    }

    GFE_GeoFilter(
        GA_Detail& geo,
        const GA_Detail& geoSrc,
        const SOP_NodeVerb::CookParms& cookparms
    )
        : geo(static_cast<GFE_Detail*>(&geo))
        , geoSrc(&geoSrc)
        , cookparms(&cookparms)
        , groupParser(geoSrc, gop, cookparms)
    {
    }

    GFE_GeoFilter(
        GA_Detail& geo,
        const GA_Detail& geoSrc,
        const SOP_NodeVerb::CookParms* const cookparms = nullptr
    )
        : geo(static_cast<GFE_Detail*>(&geo))
        , geoSrc(&geoSrc)
        , cookparms(cookparms)
        , groupParser(geoSrc, gop, cookparms)
    {
    }

    GFE_GeoFilter(
        GA_Detail& geo,
        const GA_Detail* const geoSrc,
        const SOP_NodeVerb::CookParms* const cookparms = nullptr
    )
        : geo(static_cast<GFE_Detail*>(&geo))
        , geoSrc(geoSrc)
        , cookparms(cookparms)
        , groupParser(geoSrc ? *geoSrc : geo, gop, cookparms)
    {
    }

    ~GFE_GeoFilter()
    {
        //delTopoAttrib();
    }




    SYS_FORCE_INLINE void setGeoSrc(const GA_Detail* const inGeo = nullptr)
    { geoSrc = inGeo; }

    SYS_FORCE_INLINE void setGeoSrc(const GA_Detail& inGeo)
    { geoSrc = &inGeo; }


    virtual void reset(GA_Detail& inGeo, const SOP_NodeVerb::CookParms* const cookparms = nullptr)
    {
        geo = static_cast<GFE_Detail*>(&inGeo);
        this->cookparms = cookparms;
        groupParser.reset(geo, cookparms);
        setHasComputed();
    }

    SYS_FORCE_INLINE virtual void reset(GA_Detail* const inGeo, const SOP_NodeVerb::CookParms* const cookparms = nullptr)
    { reset(*inGeo, cookparms); }


    SYS_FORCE_INLINE bool getHasComputed() const
    { return hasComputed; }
    
    // SYS_FORCE_INLINE void setOutTopoAttrib(const bool outTopoAttrib = true)
    // { this->outTopoAttrib = outTopoAttrib; }
    //
    // SYS_FORCE_INLINE bool getOutTopoAttrib() const
    // { return outTopoAttrib; }
    

    GFE_GroupParser& getGroupParser()
    { return groupParser; }
    
    SYS_FORCE_INLINE void compute()
    { hasComputed = computeCore(); }


    
    
    SYS_FORCE_INLINE void setPieceAttrib(const GA_Attribute* const inAttrib)
    { pieceAttrib = inAttrib; }

    SYS_FORCE_INLINE void setPieceAttrib(const GA_AttributeOwner owner, const UT_StringRef& attribName)
    { pieceAttrib = geo->findAttribute(owner, attribName); }


    
    SYS_FORCE_INLINE void setPositionAttrib(const GA_Attribute* const inPosAttrib)
    { posAttrib = inPosAttrib; posAttribNonConst = nullptr; }

    SYS_FORCE_INLINE void setPositionAttrib(GA_Attribute* const inPosAttrib)
    { posAttrib = inPosAttrib; posAttribNonConst = inPosAttrib; }

    SYS_FORCE_INLINE void setPositionAttrib(const UT_StringRef& attribName)
    { posAttribNonConst = geo->findPointAttribute(attribName); posAttrib = posAttribNonConst; }

    
    
    SYS_FORCE_INLINE void bumpDataIdsForAddOrRemove(const bool point, const bool vertex, const bool prim) const
    { geo->bumpDataIdsForAddOrRemove(point, vertex, prim); }

    virtual void bumpDataIdsForAddOrRemove() const
    { geo->bumpDataIdsForAddOrRemove(true, true, true); }

    SYS_FORCE_INLINE void computeAndBumpDataIdsForAddOrRemove(const bool point, const bool vertex, const bool prim)
    { compute(); bumpDataIdsForAddOrRemove(point, vertex, prim); }

    SYS_FORCE_INLINE void computeAndBumpDataIdsForAddOrRemove()
    { compute(); bumpDataIdsForAddOrRemove(); }



    virtual void bumpDataId() const
    {
    }

    SYS_FORCE_INLINE void computeAndBumpDataId()
    { compute(); bumpDataId(); }

    SYS_FORCE_INLINE GFE_Detail* getDetail() const
    { return geo; }
    
    SYS_FORCE_INLINE void visualizeGroup(const GA_Group& group) const
    { if (cookparms && !group.isDetached()) cookparms->select(group); }
    
    SYS_FORCE_INLINE void visualizeGroup(const GA_Group* const group) const
    { if (group) visualizeGroup(*group); }

    SYS_FORCE_INLINE void visualizeGroup(const GA_GroupType groupType, const UT_StringRef& name) const
    { visualizeGroup(geo->findGroup(groupType, name)); }

    SYS_FORCE_INLINE void setValidPosAttrib()
    {
        if (GFE_Type::isInvalidPosAttrib(posAttrib) || GFE_Type::isInvalidPosAttrib(posAttribNonConst))
        {
            posAttribNonConst = geo->getP();
            posAttrib = posAttribNonConst;
        }
    }
    
    SYS_FORCE_INLINE void setValidConstPosAttrib()
    { if (GFE_Type::isInvalidPosAttrib(posAttrib)) posAttrib = geo->getP(); }


private:

    virtual bool computeCore()
    { return false; }


private:
    SYS_FORCE_INLINE virtual void setDetailBase(GA_Detail& inGeo)
    {
        geo = static_cast<GFE_Detail*>(&inGeo);
        groupParser.setDetail(geo);
    }

    SYS_FORCE_INLINE virtual void delTopoAttrib() const
    { if (outTopoAttrib) geo->delTopoAttrib(); }

    
protected:

    SYS_FORCE_INLINE void setDetail(GA_Detail* const inGeo)
    { setDetailBase(*inGeo); }
    
    SYS_FORCE_INLINE void setDetail(GA_Detail& inGeo)
    { setDetail(&inGeo); }
    
    SYS_FORCE_INLINE void setDetail(GA_Attribute& attrib)
    { setDetail(attrib.getDetail()); }
    
    SYS_FORCE_INLINE void setDetail(GA_ElementGroup& group)
    { setDetail(static_cast<GA_ElementGroup&>(group).getDetail()); }

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
    
    SYS_FORCE_INLINE void setHasComputed(const bool inHasComputed = false)
    { hasComputed = inHasComputed; }


public:
    GFE_GroupParser groupParser;
    bool outTopoAttrib = true;

protected:
    GFE_Detail* geo;
    const GA_Detail* geoSrc = nullptr;
    //const GFE_Detail* geoConst;
    const SOP_NodeVerb::CookParms* cookparms;
    
    const GA_Attribute* pieceAttrib = nullptr;
    const GA_Attribute* posAttrib = nullptr;
    GA_Attribute* posAttribNonConst = nullptr;
    
    GOP_Manager gop;

private:
    //bool outTopoAttrib = true;
    bool hasComputed = false;
    

}; // End of class GFE_GeoFilter











#if 0

class GFE_GeoFilterRef0 {

public:

    GFE_GeoFilterRef0(
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

    GFE_GeoFilterRef0(
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

    GFE_GeoFilterRef0(
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

    GFE_GeoFilterRef0(
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


    ~GFE_GeoFilterRef0()
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
        const GFE_Detail& geoRef0,
        GOP_Manager* const gop,
        const SOP_NodeVerb::CookParms* const cookparms = nullptr
    )
    { resetGeoFilterRef0(&geoRef0, gop, cookparmsRef0); }

    SYS_FORCE_INLINE void resetGeoFilterRef0(
        const GA_Detail* const geoRef0,
        GOP_Manager* const gop,
        const SOP_NodeVerb::CookParms* const cookparms = nullptr
    )
    { resetGeoFilterRef0(static_cast<const GFE_Detail*>(geoRef0), gop, cookparmsRef0); }

    SYS_FORCE_INLINE void resetGeoFilterRef0(
        const GA_Detail& geoRef0,
        GOP_Manager* const gop,
        const SOP_NodeVerb::CookParms* const cookparms = nullptr
    )
    { resetGeoFilterRef0(static_cast<const GFE_Detail&>(geoRef0), gop, cookparmsRef0); }


    
    // SYS_FORCE_INLINE GFE_GroupParser& getRef0GroupParser()
    // { return groupParserRef0; }
    SYS_FORCE_INLINE GFE_RefAttributeArray& getRef0AttribArray()
    { return ref0AttribArray; }
    SYS_FORCE_INLINE GFE_RefGroupArray& getRef0GroupArray()
    { return ref0GroupArray; }
    SYS_FORCE_INLINE ::std::vector<const GA_Attribute*>& getRef0AttribArrayRef()
    { return ref0AttribArray.ref(); }
    SYS_FORCE_INLINE ::std::vector<const GA_Group*>& getRef0GroupArrayRef()
    { return ref0GroupArray.ref(); }


    SYS_FORCE_INLINE void setPositionRef0Attrib(const GA_Attribute* const inPosAttrib)
    { posRef0Attrib = inPosAttrib; }

    SYS_FORCE_INLINE void setPositionRef0Attrib(const UT_StringRef& attribName)
    { posRef0Attrib = geoRef0->findPointAttribute(attribName); }

    
private:
    SYS_FORCE_INLINE virtual void setRef0DetailBase(const GFE_Detail* const inGeo)
    {
        geoRef0 = inGeo;
        ref0AttribArray.setDetail(inGeo);
        ref0GroupArray.setDetail(inGeo);
    }

    SYS_FORCE_INLINE virtual void setRef0DetailBase(const GA_Detail* const inGeo)
    { setRef0DetailBase(static_cast<const GFE_Detail*>(inGeo)); }



public:
    GFE_GroupParser groupParserRef0;

protected:
    const GFE_Detail* geoRef0;

private:
    const SOP_NodeVerb::CookParms* cookparmsRef0;
    GOP_Manager* gopRef0;
    
    const GA_Attribute* posRef0Attrib = nullptr;

    GFE_RefAttributeArray ref0AttribArray;
    GFE_RefGroupArray ref0GroupArray;

}; // End of class GFE_GeoFilterRef0


#else



#define __GFE_GeoFilterRef_SPECILIZATION(NUM)                                                        \
class GFE_GeoFilterRef##NUM {                                                                        \
                                                                                                     \
public:                                                                                              \
                                                                                                     \
    GFE_GeoFilterRef##NUM(                                                                           \
        const GFE_Detail* const geoRef##NUM,                                                         \
        GOP_Manager& gop,                                                                            \
        const SOP_NodeVerb::CookParms* const cookparms = nullptr                                     \
    )                                                                                                \
        : geoRef##NUM(geoRef##NUM)                                                                   \
        , cookparmsRef##NUM(cookparms)                                                               \
        , gopRef##NUM(&gop)                                                                          \
        , groupParserRef##NUM(geoRef##NUM, gop, cookparms)                                           \
        , ref##NUM##AttribArray(geoRef##NUM, cookparms)                                              \
        , ref##NUM##GroupArray(geoRef##NUM, cookparms)                                               \
    {                                                                                                \
    }                                                                                                \
                                                                                                     \
    GFE_GeoFilterRef##NUM(                                                                           \
        const GA_Detail* const geoRef##NUM,                                                          \
        GOP_Manager& gop,                                                                            \
        const SOP_NodeVerb::CookParms* const cookparms = nullptr                                     \
    )                                                                                                \
        : geoRef##NUM(static_cast<const GFE_Detail*>(geoRef##NUM))                                   \
        , cookparmsRef##NUM(cookparms)                                                               \
        , gopRef##NUM(&gop)                                                                          \
        , groupParserRef##NUM(geoRef##NUM, gop, cookparms)                                           \
        , ref##NUM##AttribArray(geoRef##NUM, cookparms)                                              \
        , ref##NUM##GroupArray(geoRef##NUM, cookparms)                                               \
    {                                                                                                \
    }                                                                                                \
                                                                                                     \
    GFE_GeoFilterRef##NUM(                                                                           \
        const GA_Detail& geoRef##NUM,                                                                \
        GOP_Manager& gop,                                                                            \
        const SOP_NodeVerb::CookParms* const cookparms = nullptr                                     \
    )                                                                                                \
        : geoRef##NUM(static_cast<const GFE_Detail*>(&geoRef##NUM))                                  \
        , cookparmsRef##NUM(cookparms)                                                               \
        , gopRef##NUM(&gop)                                                                          \
        , groupParserRef##NUM(geoRef##NUM, gop, cookparms)                                           \
        , ref##NUM##AttribArray(geoRef##NUM, cookparms)                                              \
        , ref##NUM##GroupArray(geoRef##NUM, cookparms)                                               \
    {                                                                                                \
    }                                                                                                \
                                                                                                     \
    GFE_GeoFilterRef##NUM(                                                                           \
        const GA_Detail* const geoRef##NUM,                                                          \
        GOP_Manager& gop,                                                                            \
        const SOP_NodeVerb::CookParms& cookparms                                                     \
    )                                                                                                \
        : geoRef##NUM(static_cast<const GFE_Detail*>(geoRef##NUM))                                   \
        , cookparmsRef##NUM(&cookparms)                                                              \
        , gopRef##NUM(&gop)                                                                          \
        , groupParserRef##NUM(geoRef##NUM, gop, cookparms)                                           \
        , ref##NUM##AttribArray(geoRef##NUM, cookparms)                                              \
        , ref##NUM##GroupArray(geoRef##NUM, cookparms)                                               \
    {                                                                                                \
    }                                                                                                \
                                                                                                     \
                                                                                                     \
    ~GFE_GeoFilterRef##NUM()                                                                         \
    {                                                                                                \
    }                                                                                                \
                                                                                                     \
                                                                                                     \
                                                                                                     \
    void resetGeoFilterRef##NUM(                                                                     \
        const GFE_Detail* const geoRef##NUM,                                                         \
        GOP_Manager* const gop,                                                                      \
        const SOP_NodeVerb::CookParms* const cookparms = nullptr                                     \
    )                                                                                                \
    {                                                                                                \
        this->geoRef##NUM = geoRef##NUM;                                                             \
        cookparmsRef##NUM = cookparms;                                                               \
        gopRef##NUM = gop;                                                                           \
        UT_ASSERT_P(gopRef##NUM);                                                                    \
        ref##NUM##AttribArray.reset(geoRef##NUM, cookparms);                                         \
        ref##NUM##GroupArray.reset(geoRef##NUM, cookparms);                                          \
    }                                                                                                \
                                                                                                     \
    SYS_FORCE_INLINE void resetGeoFilterRef##NUM(                                                    \
        const GFE_Detail& geoRef##NUM,                                                               \
        GOP_Manager* const gop,                                                                      \
        const SOP_NodeVerb::CookParms* const cookparms = nullptr                                     \
    )                                                                                                \
    { resetGeoFilterRef##NUM(&geoRef##NUM, gop, cookparmsRef##NUM); }                                \
                                                                                                     \
    SYS_FORCE_INLINE void resetGeoFilterRef##NUM(                                                    \
        const GA_Detail* const geoRef##NUM,                                                          \
        GOP_Manager* const gop,                                                                      \
        const SOP_NodeVerb::CookParms* const cookparms = nullptr                                     \
    )                                                                                                \
    { resetGeoFilterRef##NUM(static_cast<const GFE_Detail*>(geoRef##NUM), gop, cookparmsRef##NUM); } \
                                                                                                     \
    SYS_FORCE_INLINE void resetGeoFilterRef##NUM(                                                    \
        const GA_Detail& geoRef##NUM,                                                                \
        GOP_Manager* const gop,                                                                      \
        const SOP_NodeVerb::CookParms* const cookparms = nullptr                                     \
    )                                                                                                \
    { resetGeoFilterRef##NUM(static_cast<const GFE_Detail&>(geoRef##NUM), gop, cookparmsRef##NUM); } \
                                                                                                     \
                                                                                                     \
                                                                                                     \
    SYS_FORCE_INLINE GFE_RefAttributeArray& getRef##NUM##AttribArray()                               \
    { return ref##NUM##AttribArray; }                                                                \
    SYS_FORCE_INLINE GFE_RefGroupArray& getRef##NUM##GroupArray()                                    \
    { return ref##NUM##GroupArray; }                                                                 \
    SYS_FORCE_INLINE ::std::vector<const GA_Attribute*>& getRef##NUM##AttribArrayRef()               \
    { return ref##NUM##AttribArray.ref(); }                                                          \
    SYS_FORCE_INLINE ::std::vector<const GA_Group*>& getRef##NUM##GroupArrayRef()                    \
    { return ref##NUM##GroupArray.ref(); }                                                           \
                                                                                                     \
                                                                                                     \
    SYS_FORCE_INLINE void setPositionRef##NUM##Attrib(const GA_Attribute* const inPosAttrib)         \
    { posRef##NUM##Attrib = inPosAttrib; }                                                           \
                                                                                                     \
    SYS_FORCE_INLINE void setPositionRef##NUM##Attrib(const UT_StringRef& attribName)                \
    { posRef##NUM##Attrib = geoRef##NUM->findPointAttribute(attribName); }                           \
                                                                                                     \
                                                                                                     \
    SYS_FORCE_INLINE void setValidPosRef##NUM##Attrib()                                              \
    {                                                                                                \
        if (geoRef##NUM && GFE_Type::isInvalidPosAttrib(posRef##NUM##Attrib))                        \
        {                                                                                            \
            posRef##NUM##Attrib = geoRef##NUM->getP();                                               \
        }                                                                                            \
    }                                                                                                \
                                                                                                     \
                                                                                                     \
                                                                                                     \
private:                                                                                             \
    SYS_FORCE_INLINE virtual void setRef##NUM##DetailBase(const GFE_Detail* const inGeo)             \
    {                                                                                                \
        geoRef##NUM = inGeo;                                                                         \
        ref##NUM##AttribArray.setDetail(inGeo);                                                      \
        ref##NUM##GroupArray.setDetail(inGeo);                                                       \
    }                                                                                                \
                                                                                                     \
    SYS_FORCE_INLINE virtual void setRef##NUM##DetailBase(const GA_Detail* const inGeo)              \
    { setRef##NUM##DetailBase(static_cast<const GFE_Detail*>(inGeo)); }                              \
                                                                                                     \
                                                                                                     \
                                                                                                     \
public:                                                                                              \
    GFE_GroupParser groupParserRef##NUM;                                                             \
    const GA_Attribute* posRef##NUM##Attrib = nullptr;                                               \
                                                                                                     \
protected:                                                                                           \
    const GFE_Detail* geoRef##NUM;                                                                   \
                                                                                                     \
private:                                                                                             \
    const SOP_NodeVerb::CookParms* cookparmsRef##NUM;                                                \
    GOP_Manager* gopRef##NUM;                                                                        \
                                                                                                     \
                                                                                                     \
    GFE_RefAttributeArray ref##NUM##AttribArray;                                                     \
    GFE_RefGroupArray     ref##NUM##GroupArray;                                                      \
                                                                                                     \
}; // End of class GFE_GeoFilterRef


__GFE_GeoFilterRef_SPECILIZATION(0)
__GFE_GeoFilterRef_SPECILIZATION(1)
__GFE_GeoFilterRef_SPECILIZATION(2)
__GFE_GeoFilterRef_SPECILIZATION(3)
__GFE_GeoFilterRef_SPECILIZATION(4)
        
#undef __GFE_GeoFilterRef_SPECILIZATION

#endif






class GFE_AttribFilter : public GFE_GeoFilter {

public:
    
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


    
    GFE_AttribFilter(
        GFE_Detail* const geo,
        const GA_Detail* const geoSrc,
        const SOP_NodeVerb::CookParms* const cookparms = nullptr
    )
        : GFE_GeoFilter(geo, geoSrc, cookparms)
        , outAttribArray(geo, cookparms)
        , outGroupArray(geo, cookparms)
    {
        //UT_ASSERT_MSG(geo, "do not find geo");
    }

    GFE_AttribFilter(
        GA_Detail& geo,
        const GA_Detail& geoSrc,
        const SOP_NodeVerb::CookParms& cookparms
    )
        : GFE_GeoFilter(geo, geoSrc, cookparms)
        , outAttribArray(geo, cookparms)
        , outGroupArray(geo, cookparms)
    {
        //UT_ASSERT_MSG(geo, "do not find geo");
    }

    GFE_AttribFilter(
        GA_Detail& geo,
        const GA_Detail& geoSrc,
        const SOP_NodeVerb::CookParms* const cookparms = nullptr
    )
        : GFE_GeoFilter(geo, geoSrc, cookparms)
        , outAttribArray(geo, cookparms)
        , outGroupArray(geo, cookparms)
    {
        //UT_ASSERT_MSG(geo, "do not find geo");
    }

    GFE_AttribFilter(
        GA_Detail& geo,
        const GA_Detail* const geoSrc,
        const SOP_NodeVerb::CookParms* const cookparms
    )
        : GFE_GeoFilter(geo, geoSrc, cookparms)
        , outAttribArray(geo, cookparms)
        , outGroupArray(geo, cookparms)
    {
        //UT_ASSERT_MSG(geo, "do not find geo");
    }

    
    ~GFE_AttribFilter()
    {
    }


    virtual void reset(GA_Detail& geo, const SOP_NodeVerb::CookParms* const cookparms = nullptr) override
    {
        GFE_GeoFilter::reset(geo, cookparms);
        outAttribArray.reset(geo, cookparms);
        outGroupArray. reset(geo, cookparms);
    }

    SYS_FORCE_INLINE virtual void reset(GA_Detail* const geo, const SOP_NodeVerb::CookParms* const cookparms = nullptr) override
    { reset(*geo, cookparms); }


    
    virtual void bumpDataId() const override
    {
        if (doDelGroupElement)
            bumpDataIdsForAddOrRemove();
        else
        {
            outAttribArray.bumpDataId();
            outGroupArray.bumpDataId();
        }
    }

    SYS_FORCE_INLINE virtual GA_Group* findOrCreateGroup(const bool detached, const GA_GroupType groupType, const UT_StringRef& name = "")
    { return getOutGroupArray().findOrCreate(detached, groupType, name); }

    SYS_FORCE_INLINE virtual GA_Group* findOrCreateGroup(const GA_GroupType groupType, const UT_StringRef& groupName = "")
    { return findOrCreateGroup(doDelGroupElement, groupType, groupName); }

    SYS_FORCE_INLINE virtual GA_Group* findOrCreateGroup(const bool detached, const GA_AttributeOwner attribOwner, const UT_StringRef& name = "")
    { return findOrCreateGroup(detached, GFE_Type::attributeOwner_groupType(attribOwner), name); }

    SYS_FORCE_INLINE virtual GA_Group* findOrCreateGroup(const GA_AttributeOwner attribOwner, const UT_StringRef& name = "")
    { return findOrCreateGroup(doDelGroupElement, attribOwner, name); }

    
    SYS_FORCE_INLINE virtual GA_PrimitiveGroup* findOrCreatePrimitiveGroup(const bool detached = true, const UT_StringRef& name = "")
    { return getOutGroupArray().findOrCreatePrimitive(detached, name); }

    SYS_FORCE_INLINE virtual GA_PointGroup*  findOrCreatePointGroup (const bool detached = true, const UT_StringRef& name = "")
    { return getOutGroupArray().findOrCreatePoint(detached, name); }

    SYS_FORCE_INLINE virtual GA_VertexGroup* findOrCreateVertexGroup(const bool detached = true, const UT_StringRef& name = "")
    { return getOutGroupArray().findOrCreateVertex(detached, name); }

    SYS_FORCE_INLINE virtual GA_EdgeGroup*   findOrCreateEdgeGroup  (const bool detached = true, const UT_StringRef& name = "")
    { return getOutGroupArray().findOrCreateEdge(detached, name); }



virtual void delGroupElement(const GA_Group* group = nullptr)
{
    if (!group)
    {
        if (outGroupArray.isEmpty() || !outGroupArray[0])
            return;
        group = outGroupArray[0];
    }

    if (group->classType() == GA_GROUP_EDGE)
    {
        geo->asGU_Detail()->dissolveEdges(static_cast<const GA_EdgeGroup&>(*group),
            false, 0, delUnusedPoint, GU_Detail::GU_BRIDGEMODE_BRIDGE, false, false);
    }
    else
    {
        const GA_Range range(static_cast<const GA_ElementGroup&>(*group));
        switch (group->classType())
        {
        case GA_GROUP_PRIMITIVE:
            geo->destroyPrimitiveOffsets(range, delUnusedPoint);
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

virtual SYS_FORCE_INLINE void visualizeOutGroup() const
{ if (!doDelGroupElement && !outGroupArray.isEmpty()) visualizeGroup(outGroupArray[0]); }

    
SYS_FORCE_INLINE void delOrVisualizeGroup()
{ doDelGroupElement ? delGroupElement() : visualizeOutGroup(); }








SYS_FORCE_INLINE GFE_AttributeArray& getOutAttribArray()
{ return outAttribArray; }

SYS_FORCE_INLINE GFE_GroupArray& getOutGroupArray()
{ return outGroupArray; }

SYS_FORCE_INLINE ::std::vector<GA_Attribute*>& getOutAttribArrayRef()
{ return outAttribArray.ref(); }

SYS_FORCE_INLINE ::std::vector<GA_Group*>& getOutGroupArrayRef()
{ return outGroupArray.ref(); }



SYS_FORCE_INLINE const GFE_AttributeArray& getOutAttribArray() const
{ return outAttribArray; }

SYS_FORCE_INLINE const GFE_GroupArray& getOutGroupArray() const
{ return outGroupArray; }

SYS_FORCE_INLINE const ::std::vector<GA_Attribute*>& getOutAttribArrayRef() const
{ return outAttribArray.ref(); }

SYS_FORCE_INLINE const ::std::vector<GA_Group*>& getOutGroupArrayRef() const
{ return outGroupArray.ref(); }


    
private:

SYS_FORCE_INLINE virtual void setDetailBase(GA_Detail& inGeo) override
{
    setDetail(inGeo);
    outAttribArray.setDetail(inGeo);
    outGroupArray.setDetail(inGeo);
}



public:
    GFE_GroupSetter groupSetter;
    //bool reverseOutGroup = false;
    bool doDelGroupElement = false;
    bool delUnusedPoint = true;
    
private:
    //::std::vector<GA_AttributeUPtr> attribUPtrArray;
    //::std::vector<GA_Attribute*> attribArray;
    //::std::vector<GA_GroupUPtr> groupUPtrArray;
    //::std::vector<GA_Group*> groupArray;
    GFE_AttributeArray outAttribArray;
    GFE_GroupArray outGroupArray;


}; // End of class GFE_AttribFilter



















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





    virtual void reset(GA_Detail& geo, const SOP_NodeVerb::CookParms* const cookparms = nullptr) override
    {
        GFE_AttribFilter::reset(geo, cookparms);
        inAttribArray.reset(geo, cookparms);
        inGroupArray.reset(geo, cookparms);
    }

    SYS_FORCE_INLINE virtual void reset(GA_Detail* const geo, const SOP_NodeVerb::CookParms* const cookparms = nullptr) override
    { reset(*geo, cookparms); }


    
    SYS_FORCE_INLINE GFE_AttributeArray& getInAttribArray()
    { return inAttribArray; }
    
    SYS_FORCE_INLINE GFE_GroupArray& getInGroupArray()
    { return inGroupArray; }
    
    SYS_FORCE_INLINE ::std::vector<GA_Attribute*>& getInAttribArrayRef()
    { return inAttribArray.ref(); }
    
    SYS_FORCE_INLINE ::std::vector<GA_Group*>& getInGroupArrayRef()
    { return inGroupArray.ref(); }

    SYS_FORCE_INLINE const GFE_AttributeArray& getInAttribArray() const
    { return inAttribArray; }
    
    SYS_FORCE_INLINE const GFE_GroupArray& getInGroupArray() const
    { return inGroupArray; }
    
    SYS_FORCE_INLINE const ::std::vector<GA_Attribute*>& getInAttribArrayRef() const
    { return inAttribArray.ref(); }
    
    SYS_FORCE_INLINE const ::std::vector<GA_Group*>& getInGroupArrayRef() const
    { return inGroupArray.ref(); }


    SYS_FORCE_INLINE void setInAttribBumpDataId(bool val = true)
    { inAttribBumpDataId = val; }
    
    SYS_FORCE_INLINE void setInGroupBumpDataId(bool val = true)
    { inGroupBumpDataId = val; }

    virtual void bumpDataId() const override
    {
        if (doDelGroupElement)
            bumpDataIdsForAddOrRemove();
        else
        {
            getOutAttribArray().bumpDataId();
            getOutGroupArray().bumpDataId();
            if (inAttribBumpDataId)
                inAttribArray.bumpDataId();
            if (inGroupBumpDataId)
                inGroupArray.bumpDataId();
        }
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

}; // End of class GFE_AttribCreateFilter
















class GFE_GeoFilterWithRef0 : public GFE_GeoFilter, public GFE_GeoFilterRef0 {

public:

    GFE_GeoFilterWithRef0(
        GFE_Detail* const geo,
        const GA_Detail* const geoRef0,
        const SOP_NodeVerb::CookParms* const cookparms = nullptr
    )
        : GFE_GeoFilter(geo, cookparms)
        , GFE_GeoFilterRef0(geoRef0, groupParser.getGOPRef(), cookparms)
    {
    }

    GFE_GeoFilterWithRef0(
        GFE_Detail& geo,
        const GA_Detail* const geoRef0,
        const SOP_NodeVerb::CookParms* const cookparms = nullptr
    )
        : GFE_GeoFilter(geo, cookparms)
        , GFE_GeoFilterRef0(geoRef0, groupParser.getGOPRef(), cookparms)
    {
    }

    GFE_GeoFilterWithRef0(
        GA_Detail& geo,
        const GA_Detail* const geoRef0,
        const SOP_NodeVerb::CookParms* const cookparms = nullptr
    )
        : GFE_GeoFilter(geo, cookparms)
        , GFE_GeoFilterRef0(geoRef0, groupParser.getGOPRef(), cookparms)
    {
    }


    GFE_GeoFilterWithRef0(
        GA_Detail* const geo,
        const GA_Detail* const geoRef0,
        const SOP_NodeVerb::CookParms* const cookparms = nullptr
    )
        : GFE_GeoFilter(geo, cookparms)
        , GFE_GeoFilterRef0(geoRef0, groupParser.getGOPRef(), cookparms)
    {
    }

    GFE_GeoFilterWithRef0(
        GA_Detail* const geo,
        const GA_Detail& geoRef0,
        const SOP_NodeVerb::CookParms* const cookparms = nullptr
    )
        : GFE_GeoFilter(geo, cookparms)
        , GFE_GeoFilterRef0(geoRef0, groupParser.getGOPRef(), cookparms)
    {
    }

    GFE_GeoFilterWithRef0(
        GA_Detail& geo,
        const GA_Detail* const geoRef0,
        const SOP_NodeVerb::CookParms& cookparms
    )
        : GFE_GeoFilter(geo, cookparms)
        , GFE_GeoFilterRef0(geoRef0, groupParser.getGOPRef(), cookparms)
    {
    }

    GFE_GeoFilterWithRef0(
        GA_Detail& geo,
        const GA_Detail& geoRef0,
        const SOP_NodeVerb::CookParms& cookparms
    )
        : GFE_GeoFilter(geo, cookparms)
        , GFE_GeoFilterRef0(geoRef0, groupParser.getGOPRef(), &cookparms)
    {
    }


    
    GFE_GeoFilterWithRef0(
        GA_Detail& geo,
        const GA_Detail* const geoSrc,
        const GA_Detail* const geoRef0,
        const SOP_NodeVerb::CookParms* const cookparms
    )
        : GFE_GeoFilter(geo, geoSrc, cookparms)
        , GFE_GeoFilterRef0(geoRef0, groupParser.getGOPRef(), cookparms)
    {
    }


    ~GFE_GeoFilterWithRef0()
    {
    }



}; // End of class GFE_GeoFilterWithRef0




class GFE_GeoFilterWithRef1 : public GFE_GeoFilterWithRef0, public GFE_GeoFilterRef1 {

public:

    GFE_GeoFilterWithRef1(
        GFE_Detail* const geo,
        const GA_Detail* const geoRef0,
        const GA_Detail* const geoRef1,
        const SOP_NodeVerb::CookParms* const cookparms = nullptr
    )
        : GFE_GeoFilterWithRef0(geo, geoRef0, cookparms)
        , GFE_GeoFilterRef1(geoRef1, groupParser.getGOPRef(), cookparms)
    {
    }

    GFE_GeoFilterWithRef1(
        GFE_Detail& geo,
        const GA_Detail* const geoRef0,
        const GA_Detail* const geoRef1,
        const SOP_NodeVerb::CookParms* const cookparms = nullptr
    )
        : GFE_GeoFilterWithRef0(geo, geoRef0, cookparms)
        , GFE_GeoFilterRef1(geoRef1, groupParser.getGOPRef(), cookparms)
    {
    }

    GFE_GeoFilterWithRef1(
        GA_Detail& geo,
        const GA_Detail* const geoRef0,
        const GA_Detail* const geoRef1,
        const SOP_NodeVerb::CookParms* const cookparms = nullptr
    )
        : GFE_GeoFilterWithRef0(geo, geoRef0, cookparms)
        , GFE_GeoFilterRef1(geoRef1, groupParser.getGOPRef(), cookparms)
    {
    }


    GFE_GeoFilterWithRef1(
        GA_Detail* const geo,
        const GA_Detail* const geoRef0,
        const GA_Detail* const geoRef1,
        const SOP_NodeVerb::CookParms* const cookparms = nullptr
    )
        : GFE_GeoFilterWithRef0(geo, geoRef0, cookparms)
        , GFE_GeoFilterRef1(geoRef1, groupParser.getGOPRef(), cookparms)
    {
    }

    GFE_GeoFilterWithRef1(
        GA_Detail* const geo,
        const GA_Detail& geoRef0,
        const GA_Detail* const geoRef1,
        const SOP_NodeVerb::CookParms* const cookparms = nullptr
    )
        : GFE_GeoFilterWithRef0(geo, geoRef0, cookparms)
        , GFE_GeoFilterRef1(geoRef1, groupParser.getGOPRef(), cookparms)
    {
    }

    GFE_GeoFilterWithRef1(
        GA_Detail& geo,
        const GA_Detail* const geoRef0,
        const GA_Detail* const geoRef1,
        const SOP_NodeVerb::CookParms& cookparms
    )
        : GFE_GeoFilterWithRef0(geo, geoRef0, cookparms)
        , GFE_GeoFilterRef1(geoRef1, groupParser.getGOPRef(), cookparms)
    {
    }

    GFE_GeoFilterWithRef1(
        GA_Detail& geo,
        const GA_Detail& geoRef,
        const GA_Detail* const geoRef1,
        const SOP_NodeVerb::CookParms& cookparms
    )
        : GFE_GeoFilterWithRef0(geo, geoRef0, cookparms)
        , GFE_GeoFilterRef1(geoRef1, groupParser.getGOPRef(), cookparms)
    {
    }



    
    GFE_GeoFilterWithRef1(
        GA_Detail& geo,
        const GA_Detail* const geoSrc,
        const GA_Detail* const geoRef0,
        const GA_Detail* const geoRef1,
        const SOP_NodeVerb::CookParms* const cookparms = nullptr
    )
        : GFE_GeoFilterWithRef0(geo, geoSrc, geoRef0, cookparms)
        , GFE_GeoFilterRef1(geoRef1, groupParser.getGOPRef(), cookparms)
    {
    }


    
    ~GFE_GeoFilterWithRef1()
    {
    }

}; // End of class GFE_GeoFilterWithRef1




class GFE_AttribFilterWithRef0 : public GFE_AttribFilter, public GFE_GeoFilterRef0 {

public:

    GFE_AttribFilterWithRef0(
        GFE_Detail* const geo,
        const GA_Detail* const geoRef,
        const SOP_NodeVerb::CookParms* const cookparms = nullptr
    )
        : GFE_AttribFilter(geo, cookparms)
        , GFE_GeoFilterRef0(geoRef, groupParser.getGOPRef(), cookparms)
    {
    }

    GFE_AttribFilterWithRef0(
        GFE_Detail& geo,
        const GA_Detail* const geoRef,
        const SOP_NodeVerb::CookParms* const cookparms = nullptr
    )
        : GFE_AttribFilter(geo, cookparms)
        , GFE_GeoFilterRef0(geoRef, groupParser.getGOPRef(), cookparms)
    {
    }

    GFE_AttribFilterWithRef0(
        GA_Detail& geo,
        const GA_Detail* const geoRef,
        const SOP_NodeVerb::CookParms* const cookparms = nullptr
    )
        : GFE_AttribFilter(geo, cookparms)
        , GFE_GeoFilterRef0(geoRef, groupParser.getGOPRef(), cookparms)
    {
    }


    GFE_AttribFilterWithRef0(
        GA_Detail* const geo,
        const GA_Detail* const geoRef,
        const SOP_NodeVerb::CookParms* const cookparms = nullptr
    )
        : GFE_AttribFilter(geo, cookparms)
        , GFE_GeoFilterRef0(geoRef, groupParser.getGOPRef(), cookparms)
    {
    }

    GFE_AttribFilterWithRef0(
        GA_Detail* const geo,
        const GA_Detail& geoRef,
        const SOP_NodeVerb::CookParms* const cookparms = nullptr
    )
        : GFE_AttribFilter(geo, cookparms)
        , GFE_GeoFilterRef0(geoRef, groupParser.getGOPRef(), cookparms)
    {
    }

    GFE_AttribFilterWithRef0(
        GA_Detail& geo,
        const GA_Detail* const geoRef,
        const SOP_NodeVerb::CookParms& cookparms
    )
        : GFE_AttribFilter(geo, cookparms)
        , GFE_GeoFilterRef0(geoRef, groupParser.getGOPRef(), cookparms)
    {
    }

    GFE_AttribFilterWithRef0(
        GA_Detail& geo,
        const GA_Detail& geoRef,
        const SOP_NodeVerb::CookParms& cookparms
    )
        : GFE_AttribFilter(geo, cookparms)
        , GFE_GeoFilterRef0(geoRef, groupParser.getGOPRef(), &cookparms)
    {
    }


    ~GFE_AttribFilterWithRef0()
    {
    }



}; // End of class GFE_AttribFilterWithRef0





class GFE_AttribCreateFilterWithRef0 : public GFE_AttribCreateFilter, public GFE_GeoFilterRef0 {

public:

    GFE_AttribCreateFilterWithRef0(
        GFE_Detail* const geo,
        const GA_Detail* const geoRef,
        const SOP_NodeVerb::CookParms* const cookparms = nullptr
    )
        : GFE_AttribCreateFilter(geo, cookparms)
        , GFE_GeoFilterRef0(geoRef, groupParser.getGOPRef(), cookparms)
    {
    }

    GFE_AttribCreateFilterWithRef0(
        GFE_Detail& geo,
        const GA_Detail* const geoRef,
        const SOP_NodeVerb::CookParms* const cookparms = nullptr
    )
        : GFE_AttribCreateFilter(geo, cookparms)
        , GFE_GeoFilterRef0(geoRef, groupParser.getGOPRef(), cookparms)
    {
    }

    GFE_AttribCreateFilterWithRef0(
        GA_Detail& geo,
        const GA_Detail* const geoRef,
        const SOP_NodeVerb::CookParms* const cookparms = nullptr
    )
        : GFE_AttribCreateFilter(geo, cookparms)
        , GFE_GeoFilterRef0(geoRef, groupParser.getGOPRef(), cookparms)
    {
    }


    GFE_AttribCreateFilterWithRef0(
        GA_Detail* const geo,
        const GA_Detail* const geoRef,
        const SOP_NodeVerb::CookParms* const cookparms = nullptr
    )
        : GFE_AttribCreateFilter(geo, cookparms)
        , GFE_GeoFilterRef0(geoRef, groupParser.getGOPRef(), cookparms)
    {
    }

    GFE_AttribCreateFilterWithRef0(
        GA_Detail* const geo,
        const GA_Detail& geoRef,
        const SOP_NodeVerb::CookParms* const cookparms = nullptr
    )
        : GFE_AttribCreateFilter(geo, cookparms)
        , GFE_GeoFilterRef0(geoRef, groupParser.getGOPRef(), cookparms)
    {
    }

    GFE_AttribCreateFilterWithRef0(
        GA_Detail& geo,
        const GA_Detail* const geoRef,
        const SOP_NodeVerb::CookParms& cookparms
    )
        : GFE_AttribCreateFilter(geo, cookparms)
        , GFE_GeoFilterRef0(geoRef, groupParser.getGOPRef(), cookparms)
    {
    }

    GFE_AttribCreateFilterWithRef0(
        GA_Detail& geo,
        const GA_Detail& geoRef,
        const SOP_NodeVerb::CookParms& cookparms
    )
        : GFE_AttribCreateFilter(geo, cookparms)
        , GFE_GeoFilterRef0(geoRef, groupParser.getGOPRef(), &cookparms)
    {
    }


    ~GFE_AttribCreateFilterWithRef0()
    {
    }


    SYS_FORCE_INLINE void appendInRef0Attribs(const GA_AttributeOwner attribClass, const UT_StringRef& attribPattern)
    {
        if (geoRef0)
            getRef0AttribArray().appends(attribClass, attribPattern);
        else
            getInAttribArray().appends(attribClass, attribPattern);
    }

}; // End of class GFE_AttribCreateFilterWithRef0





// class GFE_AttribFilterWithRef0Rest : public GFE_AttribFilterWithRef0 {
//
// public:
//
//     GFE_AttribFilterWithRef0Rest(
//         GFE_Detail* const geo,
//         const GA_Detail* const geoRef  = nullptr,
//         const GA_Detail* const geoRest = nullptr,
//         const SOP_NodeVerb::CookParms* const cookparms = nullptr
//     )
//         : geo(geo)
//         //, geoConst(geo)
//         , cookparms(cookparms)
//         , groupParser(geoRest ? geoRest : geo->asGA_Detail(), gop, cookparms)
//         , outAttribArray(geo, cookparms)
//         , outGroupArray(geo, cookparms)
//         , GFE_GeoFilterRef0(geoRef, groupParser.getGOPRef(), cookparms)
//         , geoRest(geoRest)
//     {
//     }
//
//     GFE_AttribFilterWithRef0Rest(
//         GA_Detail& geo,
//         const GA_Detail* const geoRef  = nullptr,
//         const GA_Detail* const geoRest = nullptr,
//         const SOP_NodeVerb::CookParms* const cookparms = nullptr
//     )
//         : geo(geo)
//         //, geoConst(geo)
//         , cookparms(cookparms)
//         , groupParser(geoRest ? geoRest : &geo, gop, cookparms)
//         , outAttribArray(geo, cookparms)
//         , outGroupArray(geo, cookparms)
//         , GFE_GeoFilterRef0(geoRef, groupParser.getGOPRef(), cookparms)
//         , geoRest(geoRest)
//     {
//     }
//
//     ~GFE_AttribFilterWithRef0Rest()
//     {
//     }
//
//
// private:
//     GA_Detail* geoRest;
//
// }; // End of class GFE_AttribFilterWithRef0Rest






#endif
