
#pragma once

#ifndef __GFE_Normal2D_h__
#define __GFE_Normal2D_h__

//#include "GFE/GFE_Normal2D.h"



#include "GFE/GFE_Normal3D.h"

#include "GFE/GFE_Adjacency.h"

//#include "GFE/GFE_Attribute.h"
//#include "GFE/GFE_TopologyReference.h"
//#include "GFE/GFE_VertexNextEquiv.h"

//#include "GFE/GFE_GroupParser.h"
//#include "GFE/GFE_GroupBoolean.h"
//#include "GFE/GFE_GroupExpand.h"

class GFE_Normal2D : public GFE_AttribCreateFilter {

public:
    GFE_Normal2D(
        GA_Detail& geo,
        const SOP_NodeVerb::CookParms* const cookparms = nullptr
    )
        : GFE_AttribCreateFilter(geo, cookparms)
        , normal3D(geo, cookparms)
    {
    }

    void
        setComputeParm(
            const UT_Vector3T<fpreal64>& defaultNormal3D = { 0,1,0 },
            const bool scaleByTurns = true,
            const bool normalize = true,
            const fpreal64 uniScale = 1.0,

            const bool useConstantNormal3D = false,
            const UT_StringRef& normal3DAttribName = "N",
            const bool findNormal3D = false,
            const bool addNormal3DIfNoFind = true,
            const GFE_NormalSearchOrder normalSearchOrder = GFE_NormalSearchOrder::INVALID,
            const float cuspAngleDegrees = GEO_DEFAULT_ADJUSTED_CUSP_ANGLE,
            const GEO_NormalMethod normalMethod = GEO_NormalMethod::ANGLE_WEIGHTED,
            const bool copyOrigIfZero = false,

            const exint subscribeRatio = 64,
            const exint minGrainSize = 1024
        )
    {
        setHasComputed();
        setInAttribBumpDataId(method == GFE_RestDir2D_Method::AvgNormal);
        getInAttribArray().findOrCreateTuple();
        this->defaultNormal3D = defaultNormal3D;
        this->scaleByTurns = scaleByTurns;
        this->normalize = normalize;
        this->uniScale = uniScale;

        this->useConstantNormal3D = useConstantNormal3D;
        this->normal3DAttribName = normal3DAttribName;
        this->findNormal3D = findNormal3D;
        this->addNormal3DIfNoFind = addNormal3DIfNoFind;

        this->normalSearchOrder = normalSearchOrder;
        this->cuspAngleDegrees = cuspAngleDegrees;
        this->normalMethod = normalMethod;
        this->copyOrigIfZero = copyOrigIfZero;

        this->subscribeRatio = subscribeRatio;
        this->minGrainSize = minGrainSize;
    }

    

    SYS_FORCE_INLINE void setPosAttrib(const UT_StringRef& attribName)
    { posAttrib = geo->findPointAttribute(attribName); }
    
private:

    virtual bool
        computeCore() override
    {
        if (getOutAttribArray().isEmpty())
            return false;

        if (groupParser.isEmpty())
            return true;

        if (!posAttrib)
            posAttrib = geo->getP();

        normal3D.compute();
        normal3DAttrib = normal3D.getAttrib();


        GA_Storage storage_max = posAttrib->getAIFTuple()->getStorage(posAttrib);
        if (normal3DAttrib)
        {
            const GA_Storage storage_normal3D = normal3DAttrib->getAIFTuple()->getStorage(normal3DAttrib);
            storage_max = storage_max >= storage_normal3D ? storage_max : storage_normal3D;
        }

        switch (storage_max)
        {
        case GA_STORE_REAL16: computeNormal2D<fpreal16>(); break;
        case GA_STORE_REAL32: computeNormal2D<fpreal32>(); break;
        case GA_STORE_REAL64: computeNormal2D<fpreal64>(); break;
        default: UT_ASSERT_MSG(0, "unhandled storage_max"); break;
        }

        return true;
    }




    template<typename T>
    void computeNormal2D()
    {
        const GA_PointGroup* const geoPointGroup = groupParser.getPointGroup();

        GFE_Adjacency adjacency(geo, cookparms);
        adjacency.setVertexNextEquivGroup(true);
        adjacency.compute();

        GA_VertexGroup* const unsharedVertexGroup = adjacency.getVertexNextEquivGroup();

        //GA_PointGroup* unsharedPointGroup = const_cast<GA_PointGroup*>(GEO_FeE_Group::groupPromote(geo, unsharedGroup, GA_GROUP_POINT, geo0AttribNames, true));
#if 1
        const GA_PointGroupUPtr unsharedPointGroupUPtr = GFE_GroupPromote::groupPromotePointDetached(geo, unsharedVertexGroup);
        GA_PointGroup* const unsharedPointGroup = unsharedPointGroupUPtr.get();
#else
        GA_PointGroup* const unsharedPointGroup = GFE_GroupPromote::groupPromotePoint(geo, unsharedVertexGroup);
#endif
        if (geoPointGroup)
        {
            GA_PointGroupUPtr expandGroupUPtr = geo->createDetachedPointGroup();
            GA_PointGroup* const expandGroup = expandGroupUPtr.get();

            GFE_GroupExpand::groupExpand(geo, expandGroup, geoPointGroup, GA_GROUP_EDGE);

            *unsharedPointGroup &= *expandGroup;

            GFE_GroupBoolean::groupIntersect(geo, unsharedVertexGroup, expandGroup);
        }

        const GA_Attribute* const dstptAttrib = GFE_TopologyReference::addAttribVertexPointDst(geo);
        const GA_ROHandleT<GA_Offset> dstptAttrib_h(dstptAttrib);


        const GA_AttributeOwner normal3DOwner = normal3D_h.getAttribute() ? normal3D_h.getAttribute()->getOwner() : GA_ATTRIB_INVALID;
        if (normal3DOwner == GA_ATTRIB_GLOBAL)
            defaultNormal3D = normal3D_h.get(0);



        const GA_Topology& topo = geo->getTopology();
        //topo.makePrimitiveRef();
        const GA_ATITopology* const vtxPointRef = topo.getPointRef();
        const GA_ATITopology* const vtxPrimRef = topo.getPrimitiveRef();


        const GA_RWHandleT<UT_Vector3T<T>> normal2D_h(getOutAttribArray()[0]);
        const GA_ROHandleT<UT_Vector3T<T>> pos_h(posAttrib);
        const GA_ROHandleT<UT_Vector3T<T>> normal3D_h(normal3D.getAttrib());

        const GA_SplittableRange geoVertexSplittableRange(geo->getVertexRange(unsharedVertexGroup));
        UTparallelFor(geoVertexSplittableRange, [this, &dstptAttrib_h, &normal2D_h, &pos_h, &normal3D_h,
            vtxPointRef, vtxPrimRef, normal3DOwner](const GA_SplittableRange& r)
        {
            UT_Vector3T<T> dir;
            GA_Offset start, end;
            for (GA_Iterator it(r); it.blockAdvance(start, end); )
            {
                for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                {
                    const GA_Offset ptoff = vtxPointRef->getLink(elemoff);
                    const GA_Offset dstpt = dstptAttrib_h.get(elemoff);

                    dir = pos_h.get(ptoff) - pos_h.get(dstpt);
                    switch (normal3DOwner)
                    {
                    case GA_ATTRIB_PRIMITIVE:
                        dir.cross(normal3D_h.get(vtxPrimRef->getLink(elemoff)));
                        break;
                    case GA_ATTRIB_POINT:
                        dir.cross(normal3D_h.get(ptoff));
                        break;
                    case GA_ATTRIB_VERTEX:
                        dir.cross(normal3D_h.get(elemoff));
                        break;
                    default:
                        dir.cross(defaultNormal3D);
                        break;
                    }
                    dir.normalize();
                    normal2D_h.add(ptoff, dir);
                    normal2D_h.add(dstpt, dir);
                }
            }
        }, subscribeRatio, minGrainSize);



        const GA_SplittableRange geoPointSplittableRange(geo->getPointRange(unsharedPointGroup));

        UTparallelFor(geoPointSplittableRange, [this](const GA_SplittableRange& r)
        {
            GA_Offset start, end;
            //GA_PageHandleV<UT_Vector3T<typename T>>::RWType normal2D_ph(normal2DAttrib);
            GA_PageHandleT<UT_Vector3T<T>, typename UT_Vector3T<T>::value_type, true, true, GA_Attribute, GA_ATINumeric, GA_Detail> normal2D_ph(getOutAttribArray()[0]);
            //GA_PageHandleV<UT_Vector3T<fpreal>>::RWType normal2D_ph(normal2DAttrib);
            for (GA_PageIterator pit = r.beginPages(); !pit.atEnd(); ++pit)
            {
                for (GA_Iterator it(pit.begin()); it.blockAdvance(start, end); )
                {
                    normal2D_ph.setPage(start);
                    for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                    {
                        if (scaleByTurns)
                        {
                            normal2D_ph.value(elemoff) *= 2 * uniScale / normal2D_ph.value(elemoff).length2();
                        }
                        else if (normalize)
                        {
                            normal2D_ph.value(elemoff).normalize();
                            normal2D_ph.value(elemoff) *= uniScale;
                        }
                    }
                }
            }
        }, subscribeRatio, minGrainSize);
    }






public:
    GFE_Normal3D normal3D;

    const GA_Attribute* posAttrib = nullptr;


    UT_Vector3T<fpreal64> defaultNormal3D = { 0, 1, 0 };
    bool scaleByTurns = true;
    bool normalize = true;
    fpreal64 uniScale = 1.0;

    bool useConstantNormal3D = false;
    bool findNormal3D = false;
    bool addNormal3DIfNoFind = true;

private:
    const GA_Attribute* normal3DAttrib = nullptr;

    exint subscribeRatio = 64;
    exint minGrainSize = 1024;
};





















namespace GFE_Normal2D_Namespace {




    template<typename T>
    static void
        computeNormal2D(
            GA_Detail* const geo,
            const GA_PointGroup* const geoPointGroup,
            const GA_RWHandleT<UT_Vector3T<T>> normal2D_h,
            const GA_ROHandleT<UT_Vector3T<T>> pos_h,
            const GA_ROHandleT<UT_Vector3T<T>> normal3D_h,
            UT_Vector3T<T> defaultNormal3D = { 0,1,0 },
            const bool scaleByTurns = true,
            const bool normalize = true,
            const T uniScale = 1.0,
            const exint subscribeRatio = 64,
            const exint minGrainSize = 1024
        )
    {
        GA_VertexGroup* const unsharedVertexGroup = GFE_VertexNextEquiv::addGroupVertexNextEquiv(geo);
        //GA_PointGroup* unsharedPointGroup = const_cast<GA_PointGroup*>(GEO_FeE_Group::groupPromote(geo, unsharedGroup, GA_GROUP_POINT, geo0AttribNames, true));
#if 1
        const GA_PointGroupUPtr unsharedPointGroupUPtr = GFE_GroupPromote::groupPromotePointDetached(geo, unsharedVertexGroup);
        GA_PointGroup* const unsharedPointGroup = unsharedPointGroupUPtr.get();
#else
        GA_PointGroup* const unsharedPointGroup = GFE_GroupPromote::groupPromotePoint(geo, unsharedVertexGroup);
#endif
        if (geoPointGroup)
        {
            GA_PointGroupUPtr expandGroupUPtr = geo->createDetachedPointGroup();
            GA_PointGroup* const expandGroup = expandGroupUPtr.get();

            GFE_GroupExpand::groupExpand(geo, expandGroup, geoPointGroup, GA_GROUP_EDGE);

            *unsharedPointGroup &= *expandGroup;
            //GFE_GroupBoolean::groupIntersect(geo, unsharedPointGroup, expandGroup);
            GFE_GroupBoolean::groupIntersect(geo, unsharedVertexGroup, expandGroup);

            //GA_PointGroup* expandGroup = GFE_Group::newDetachedGroup(geo, geoGroup);
        }


        //unsharedPointGroup->operator&=(*geoPointGroup);
        //GEO_FeE_Group::groupBoolean(geo, unsharedPointGroup, geoPointGroup, GA_GROUP_BOOLEAN_AND);
        //unsharedPointGroup->makeUnordered();
        //unsharedPointGroup->andEqual(geoPointGroup);


        //GFE_Group::groupBumpDataId(unsharedVertexGroup);
        //GFE_Group::groupBumpDataId(unsharedPointGroup);


        //const GA_Attribute* const dstptAttrib = GFE_TopologyReference::addAttribVertexPointDst(geo, unsharedVertexGroup);
        const GA_Attribute* const dstptAttrib = GFE_TopologyReference::addAttribVertexPointDst(geo);
        const GA_ROHandleT<GA_Offset> dstptAttrib_h(dstptAttrib);


        const GA_AttributeOwner normal3DOwner = normal3D_h.getAttribute() ? normal3D_h.getAttribute()->getOwner() : GA_ATTRIB_INVALID;
        if (normal3DOwner == GA_ATTRIB_GLOBAL)
            defaultNormal3D = normal3D_h.get(0);

        GA_Topology& topo = geo->getTopology();
        topo.makePrimitiveRef();
        const GA_ATITopology* const vtxPointRef = topo.getPointRef();
        //const GA_ATITopology* const pointVtxRef = topo.getVertexRef();
        const GA_ATITopology* const vtxPrimRef = topo.getPrimitiveRef();


        const GA_SplittableRange geoVertexSplittableRange(geo->getVertexRange(unsharedVertexGroup));
        UTparallelFor(geoVertexSplittableRange, [&pos_h, &dstptAttrib_h, &normal2D_h, &normal3D_h,
            vtxPointRef, vtxPrimRef,
            normal3DOwner, &defaultNormal3D](const GA_SplittableRange& r)
        {
            UT_Vector3T<T> dir;
            GA_Offset start, end;
            for (GA_Iterator it(r); it.blockAdvance(start, end); )
            {
                for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                {
                    GA_Offset ptoff = vtxPointRef->getLink(elemoff);
                    GA_Offset dstpt = dstptAttrib_h.get(elemoff);

                    dir = pos_h.get(ptoff) - pos_h.get(dstpt);
                    switch (normal3DOwner)
                    {
                    case GA_ATTRIB_PRIMITIVE:
                        dir.cross(normal3D_h.get(vtxPrimRef->getLink(elemoff)));
                        break;
                    case GA_ATTRIB_POINT:
                        dir.cross(normal3D_h.get(ptoff));
                        break;
                    case GA_ATTRIB_VERTEX:
                        dir.cross(normal3D_h.get(elemoff));
                        break;
                    default:
                        dir.cross(defaultNormal3D);
                        break;
                    }
                    dir.normalize();
                    normal2D_h.add(ptoff, dir);
                    normal2D_h.add(dstpt, dir);
                }
            }
        }, subscribeRatio, minGrainSize);


        GA_Attribute* normal2DAttrib = normal2D_h.getAttribute();

        const GA_SplittableRange geoPointSplittableRange(geo->getPointRange(unsharedPointGroup));

        UTparallelFor(geoPointSplittableRange, [normal2DAttrib, uniScale, scaleByTurns, normalize](const GA_SplittableRange& r)
        {
            GA_Offset start, end;
            //GA_PageHandleV<UT_Vector3T<typename T>>::RWType normal2D_ph(normal2DAttrib);
            GA_PageHandleT<UT_Vector3T<T>, typename UT_Vector3T<T>::value_type, true, true, GA_Attribute, GA_ATINumeric, GA_Detail> normal2D_ph(normal2DAttrib);
            //GA_PageHandleV<UT_Vector3T<fpreal>>::RWType normal2D_ph(normal2DAttrib);
            for (GA_PageIterator pit = r.beginPages(); !pit.atEnd(); ++pit)
            {
                for (GA_Iterator it(pit.begin()); it.blockAdvance(start, end); )
                {
                    normal2D_ph.setPage(start);
                    for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                    {
                        if (scaleByTurns)
                        {
                            normal2D_ph.value(elemoff) *= 2 * uniScale / normal2D_ph.value(elemoff).length2();
                        }
                        else if (normalize)
                        {
                            normal2D_ph.value(elemoff).normalize();
                            normal2D_ph.value(elemoff) *= uniScale;
                        }
                    }
                }
            }
        }, subscribeRatio, minGrainSize);
    }




    static void
        computeNormal2D(
            GA_Detail* const geo,
            const GA_PointGroup* const geoPointGroup,
            GA_Attribute* const normal2DAttrib,
            const GA_Attribute* posAttrib = nullptr,
            const GA_Attribute* const normal3DAttrib = nullptr,
            const UT_Vector3T<fpreal64>& defaultNormal3D = { 0,1,0 },
            const bool scaleByTurns = true,
            const bool normalize = true,
            const fpreal64 uniScale = 1.0,
            const exint subscribeRatio = 64,
            const exint minGrainSize = 1024
        )
    {
        if (!posAttrib)
            posAttrib = geo->getP();

        GA_Storage storage_max = posAttrib->getAIFTuple()->getStorage(posAttrib);
        if (normal3DAttrib)
        {
            const GA_Storage storage_normal3D = normal3DAttrib->getAIFTuple()->getStorage(normal3DAttrib);
            storage_max = storage_max >= storage_normal3D ? storage_max : storage_normal3D;
        }

        switch (storage_max)
        {
        //case GA_STORE_REAL16:
        //    computeNormal2D<fpreal16>(geo, geoPointGroup, normal2DAttrib, posAttrib, normal3DAttrib,
        //        defaultNormal3D, scaleByTurns, normalize, uniScale,
        //        subscribeRatio, minGrainSize);
        //    break;
        case GA_STORE_REAL32:
            computeNormal2D<fpreal32>(geo, geoPointGroup, normal2DAttrib, posAttrib, normal3DAttrib,
                defaultNormal3D, scaleByTurns, normalize, uniScale,
                subscribeRatio, minGrainSize);
            break;
        case GA_STORE_REAL64:
            computeNormal2D<fpreal64>(geo, geoPointGroup, normal2DAttrib, posAttrib, normal3DAttrib,
                defaultNormal3D, scaleByTurns, normalize, uniScale,
                subscribeRatio, minGrainSize);
            break;
        default:
            UT_ASSERT_MSG(0, "unhandled storage_max");
            break;
        }
        //const GA_RWHandleT<UT_Vector3T<fpreal64>> normal2D_h(normal2DAttrib);
        //const GA_ROHandleT<UT_Vector3T<fpreal64>> pos_h(posAttrib);
        //const GA_ROHandleT<UT_Vector3T<fpreal64>> normal3D_h(normal3DAttrib);
        //computeNormal2D(geo, geoPointGroup, normal2D_h, pos_h, normal3D_h,
        //    defaultNormal3D, scaleByTurns, normalize, uniScale,
        //    subscribeRatio, minGrainSize);
    }









    static GA_Attribute*
        addAttribNormal2D(
            GA_Detail* const geo,
            const GA_Attribute* const posAttrib = nullptr,
            const GA_Attribute* const normal3DAttrib = nullptr,
            const GA_PointGroup* const geoPointGroup = nullptr,
            const GA_Storage storage = GA_STORE_INVALID,
            const UT_StringRef& normal2DAttribName = "N",

            const UT_Vector3T<fpreal64>& defaultNormal3D = { 0,1,0 },
            const bool scaleByTurns = true,
            const bool normalize = true,
            const fpreal64 uniScale = 1.0,
            const exint subscribeRatio = 64,
            const exint minGrainSize = 1024
        )
    {
        GA_Attribute* normal2DAttrib = geo->findPointAttribute(GFE_Temp_Normal2DAttribName);
        if (normal2DAttrib)
            geo->getAttributes().destroyAttribute(normal2DAttrib);

        const GA_Storage finalStorage = storage == GA_STORE_INVALID ? GFE_Type::getPreferredStorageF(geo) : storage;
        normal2DAttrib = geo->getAttributes().createTupleAttribute(GA_ATTRIB_POINT, GFE_Temp_Normal2DAttribName, finalStorage, 3, GA_Defaults(0.0));

        computeNormal2D(geo, geoPointGroup,
            normal2DAttrib, posAttrib, normal3DAttrib, defaultNormal3D,
            scaleByTurns, normalize, uniScale,
            subscribeRatio, minGrainSize);

        GFE_Attribute::forceRenameAttribute(geo, normal2DAttrib, normal2DAttribName);

        return normal2DAttrib;
    }


    static GA_Attribute*
        findOrAddAttribNormal2D(
            GA_Detail* const geo,
            const GA_Attribute* const posAttrib = nullptr,
            const GA_Attribute* const normal3DAttrib = nullptr,
            const GA_PointGroup* const geoPointGroup = nullptr,
            const GA_Storage storage = GA_STORE_INVALID,
            const UT_StringRef& normal2DAttribName = "N",

            const UT_Vector3T<fpreal64>& defaultNormal3D = { 0,1,0 },
            const bool scaleByTurns = true,
            const bool normalize = true,
            const fpreal64 uniScale = 1.0,
            const exint subscribeRatio = 64,
            const exint minGrainSize = 1024
        )
    {
        GA_Attribute* normal2DAttrib = geo->findPointAttribute(normal2DAttribName);
        if (normal2DAttrib)
            return normal2DAttrib;

        const GA_Storage finalStorage = storage == GA_STORE_INVALID ? GFE_Type::getPreferredStorageF(geo) : storage;
        normal2DAttrib = geo->getAttributes().createTupleAttribute(GA_ATTRIB_POINT, normal2DAttribName, finalStorage, 3, GA_Defaults(0.0));

        computeNormal2D(geo, geoPointGroup,
            normal2DAttrib, posAttrib, normal3DAttrib, defaultNormal3D,
            scaleByTurns, normalize, uniScale,
            subscribeRatio, minGrainSize);

        return normal2DAttrib;
    }




    SYS_FORCE_INLINE
    static GA_Attribute*
        addAttribNormal2D(
            GA_Detail* const geo,
            const GA_Attribute* posAttrib = nullptr,
            const GA_PointGroup* const geoPointGroup = nullptr,
            const GA_Storage storage = GA_STORE_INVALID,
            const UT_StringRef& normal2DAttribName = "N",

            const UT_Vector3T<fpreal64>& defaultNormal3D = { 0,1,0 },
            const bool scaleByTurns = true,
            const bool normalize = true,
            const fpreal64 uniScale = 1.0,

            const bool useConstantNormal3D = false,
            const UT_StringRef& normal3DAttribName = "N",
            const bool findNormal3D = false,
            const bool addNormal3DIfNoFind = true,
            const GFE_NormalSearchOrder normalSearchOrder = GFE_NormalSearchOrder::INVALID,
            const float cuspangledegrees = GEO_DEFAULT_ADJUSTED_CUSP_ANGLE,
            const GEO_NormalMethod method = GEO_NormalMethod::ANGLE_WEIGHTED,
            const bool copy_orig_if_zero = false,


            const exint subscribeRatio = 64,
            const exint minGrainSize = 1024
        )
    {
        if (!posAttrib)
            posAttrib = geo->getP();

        GA_AttributeUPtr normal3DAttribUPtr;
        const GA_Attribute* normal3DAttrib = useConstantNormal3D ? nullptr :
            findOrAddAttribNormal3D(geo, normal3DAttribUPtr, posAttrib, geoPointGroup,
                storage, normal3DAttribName,
                findNormal3D, addNormal3DIfNoFind, normalSearchOrder,
                cuspangledegrees, method, copy_orig_if_zero);

        GA_Attribute* const normal2DAttrib = addAttribNormal2D(geo, posAttrib, normal3DAttrib,
            geoPointGroup, storage, normal2DAttribName,
            defaultNormal3D, scaleByTurns, normalize, uniScale,
            subscribeRatio, minGrainSize);
        return normal2DAttrib;
    }



    SYS_FORCE_INLINE
    static GA_Attribute*
        addAttribNormal2D(
            const SOP_NodeVerb::CookParms& cookparms,
            GA_Detail* const geo,
            const UT_StringRef& posAttribName = "P",
            const GA_GroupType groupType = GA_GROUP_INVALID,
            const UT_StringRef& groupName = "",
            const GA_Storage storage = GA_STORE_INVALID,
            const UT_StringRef& normal2DAttribName = "N",

            const UT_Vector3T<fpreal64>& defaultNormal3D = { 0,1,0 },
            const bool scaleByTurns = true,
            const bool normalize = true,
            const fpreal64 uniScale = 1.0,

            const bool useConstantNormal3D = false,
            const UT_StringRef& normal3DAttribName = "N",
            const bool findNormal3D = false,
            const bool addNormal3DIfNoFind = true,
            const GFE_NormalSearchOrder normalSearchOrder = GFE_NormalSearchOrder::INVALID,
            const float cuspangledegrees = GEO_DEFAULT_ADJUSTED_CUSP_ANGLE,
            const GEO_NormalMethod method = GEO_NormalMethod::ANGLE_WEIGHTED,
            const bool copy_orig_if_zero = false,


            const exint subscribeRatio = 64,
            const exint minGrainSize = 1024
        )
    {
        GOP_Manager gop;
        const GA_Group* const geoGroup = GFE_GroupParser_Namespace::findOrParseGroupDetached(cookparms, geo, groupType, groupName, gop);
        //const bool hasGroup = !!geoGroup;

        if (geoGroup && geoGroup->isEmpty())
        {
            GA_Attribute* normal2DAttrib = geo->findPointAttribute(normal2DAttribName);
            if (normal2DAttrib)
                geo->getAttributes().destroyAttribute(normal2DAttrib);

            const GA_Storage finalStorage = storage == GA_STORE_INVALID ? GFE_Type::getPreferredStorageF(geo) : storage;
            normal2DAttrib = geo->getAttributes().createTupleAttribute(GA_ATTRIB_POINT, normal2DAttribName, finalStorage, 3, GA_Defaults(0.0));
            return normal2DAttrib;
        }

        GA_PointGroupUPtr geo0PointGroupUPtr;
        const GA_PointGroup* geo0PointGroup = nullptr;
        if (geoGroup)
        {
            if (geoGroup->classType() == GA_GROUP_POINT)
            {
                geo0PointGroup = static_cast<const GA_PointGroup*>(geoGroup);
            }
            else
            {
                geo0PointGroupUPtr = GFE_GroupPromote::groupPromotePointDetached(geo, geoGroup);
                geo0PointGroup = geo0PointGroupUPtr.get();
            }
        }

        const GA_Attribute* const posAttrib = ( posAttribName.length() == 0 || posAttribName == "P" || !posAttribName.isstring() ) ? geo->getP() : geo->findPointAttribute(posAttribName);

        GA_Attribute* const normal2DAttrib = addAttribNormal2D(geo, posAttrib,
            geo0PointGroup, storage, normal2DAttribName,
            defaultNormal3D, scaleByTurns, normalize, uniScale,

            useConstantNormal3D, normal3DAttribName,
            findNormal3D, addNormal3DIfNoFind, normalSearchOrder,
            cuspangledegrees, method, copy_orig_if_zero,
            subscribeRatio, minGrainSize);


        return normal2DAttrib;
    }





} // End of namespace GFE_Normal2D

#endif
