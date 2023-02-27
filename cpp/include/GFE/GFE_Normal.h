
#pragma once

#ifndef __GFE_Normal_h__
#define __GFE_Normal_h__

//#include "GFE/GFE_Normal.h"

#include "GA/GA_Detail.h"

#include "GEO/GEO_Normal.h"

#include "GFE/GFE_Attribute.h"
#include "GFE/GFE_TopologyReference.h"
#include "GFE/GFE_VertexNextEquiv.h"
#include "GFE/GFE_GroupBoolean.h"
#include "GFE/GFE_GroupExpand.h"

enum GFE_NormalSearchOrder
{
    GFE_NormalSearchOrder_PRIMITIVE,
    GFE_NormalSearchOrder_POINT,
    GFE_NormalSearchOrder_VERTEX,
    GFE_NormalSearchOrder_DETAIL,
    GFE_NormalSearchOrder_POINTVERTEX,
    GFE_NormalSearchOrder_N,
    GFE_NormalSearchOrder_GLOBAL = GFE_NormalSearchOrder_DETAIL,
    GFE_NormalSearchOrder_INVALID = -1,
};


namespace GFE_Normal {

    //#define GFE_Temp_Normal3DAttribName "__Normal3D_SOP_FeE_Normal"
    #define GFE_Temp_Normal2DAttribName "__Normal2D_SOP_FeE_Normal"

    //constexpr UT_StringHolder& GFE_Temp_Normal3DAttribName = "__Normal3D_SOP_FeE_Normal";



    static void
        computeNormal3D(
            GA_Detail* const geo,
            const GA_ROHandleT<UT_Vector3T<fpreal32>>& posAttrib_h,
            const GA_RWHandleT<UT_Vector3T<fpreal32>>& normal3DAttrib_h,
            const GA_Group* const geoGroup = nullptr,
            const float cuspangledegrees = GEO_DEFAULT_ADJUSTED_CUSP_ANGLE,
            const GEO_NormalMethod method = GEO_NormalMethod::ANGLE_WEIGHTED,
            const bool copy_orig_if_zero = false
        )
    {
        GEOcomputeNormals(*static_cast<GEO_Detail*>(geo), posAttrib_h, normal3DAttrib_h, geoGroup, cuspangledegrees, method, copy_orig_if_zero);
    }

#if SYS_VERSION_MAJOR_INT > 19 || ( SYS_VERSION_MAJOR_INT == 19 && SYS_VERSION_MINOR_INT == 5 )
    static void
        computeNormal3D(
            GA_Detail* const geo,
            const GA_ROHandleT<UT_Vector3T<fpreal64>>& posAttrib_h,
            const GA_RWHandleT<UT_Vector3T<fpreal64>>& normal3DAttrib_h,
            const GA_Group* const geoGroup = nullptr,
            const float cuspangledegrees = GEO_DEFAULT_ADJUSTED_CUSP_ANGLE,
            const GEO_NormalMethod method = GEO_NormalMethod::ANGLE_WEIGHTED,
            const bool copy_orig_if_zero = false
        )
    {
        GEOcomputeNormals(*static_cast<GEO_Detail*>(geo), posAttrib_h, normal3DAttrib_h, geoGroup, cuspangledegrees, method, copy_orig_if_zero);
    }
#endif

    SYS_FORCE_INLINE
    static void
        computeNormal3D(
            GA_Detail* const geo,
            const GA_Attribute* const posAttrib,
            GA_Attribute* const normal3DAttrib,
            const GA_Group* const geoGroup = nullptr,
            const GA_Storage storage = GA_STORE_INVALID,
            const float cuspangledegrees = GEO_DEFAULT_ADJUSTED_CUSP_ANGLE,
            const GEO_NormalMethod method = GEO_NormalMethod::ANGLE_WEIGHTED,
            const bool copy_orig_if_zero = false
        )
    {
        UT_ASSERT_P(posAttrib);
        UT_ASSERT_P(normal3DAttrib);

#if SYS_VERSION_MAJOR_INT > 19 || ( SYS_VERSION_MAJOR_INT == 19 && SYS_VERSION_MINOR_INT == 5 )
        const GA_Storage finalStorage = storage == GA_STORE_INVALID ? posAttrib->getAIFTuple()->getStorage(posAttrib) : storage;
        if (finalStorage == GA_STORE_REAL64)
        {
            computeNormal3D(geo, GA_ROHandleV3D(posAttrib), normal3DAttrib, geoGroup, cuspangledegrees, method, copy_orig_if_zero);
        }
        else
        {
            computeNormal3D(geo, GA_ROHandleV3(posAttrib), normal3DAttrib, geoGroup, cuspangledegrees, method, copy_orig_if_zero);
        }
#else
        computeNormal3D(geo, GA_ROHandleV3(posAttrib), normal3DAttrib, geoGroup, cuspangledegrees, method, copy_orig_if_zero);
#endif
    }

    //GFE_Normal::addAttribNormal3D(geo, nullptr, attribClass, GA_STORE_REAL32, "N", GEO_DEFAULT_ADJUSTED_CUSP_ANGLE, GEO_NormalMethod::ANGLE_WEIGHTED, false, nullptr);
    //GFE_Normal::addAttribNormal3D(geo, geoGroup, attribClass, storage, normal3DAttribName, cuspangledegrees, method, copy_orig_if_zero, posAttrib);

    static GA_Attribute*
        addAttribNormal3D(
            GA_Detail* const geo,
            const GA_Attribute* posAttrib = nullptr,
            const GA_Group* const geoGroup = nullptr,
            const GA_Storage storage = GA_STORE_INVALID,
            const UT_StringHolder& normal3DAttribName = "N",

            const GA_AttributeOwner attribClass = GA_ATTRIB_VERTEX,
            const float cuspangledegrees = GEO_DEFAULT_ADJUSTED_CUSP_ANGLE,
            const GEO_NormalMethod method = GEO_NormalMethod::ANGLE_WEIGHTED,
            const bool copy_orig_if_zero = false
        )
    {
        UT_ASSERT_P(attribClass != GA_ATTRIB_OWNER_N && attribClass != GA_ATTRIB_INVALID);

        if (!posAttrib)
            posAttrib = geo->findPointAttribute("P");

        UT_ASSERT_P(posAttrib);
        UT_ASSERT_P(posAttrib->getAIFTuple());

        GA_Attribute* normal3DAttrib = geo->findAttribute(attribClass, normal3DAttribName);
        if (normal3DAttrib)
            geo->getAttributes().destroyAttribute(normal3DAttrib);

        const GA_Storage finalStorage = storage == GA_STORE_INVALID ? posAttrib->getAIFTuple()->getStorage(posAttrib) : storage;

        UT_ASSERT_P(
            finalStorage == GA_STORE_REAL16 ||
            finalStorage == GA_STORE_REAL32 ||
            finalStorage == GA_STORE_REAL64);

        normal3DAttrib = geo->getAttributes().createTupleAttribute(attribClass, normal3DAttribName, finalStorage, 3, GA_Defaults(0.0));

        computeNormal3D(geo, posAttrib, normal3DAttrib, geoGroup, finalStorage, cuspangledegrees, method, copy_orig_if_zero);

        return normal3DAttrib;
    }


    static GA_AttributeUPtr
        addDetachedAttribNormal3D(
            GA_Detail* const geo,
            const GA_Attribute* posAttrib = nullptr,
            const GA_Group* const geoGroup = nullptr,
            const GA_Storage storage = GA_STORE_INVALID,

            const GA_AttributeOwner attribClass = GA_ATTRIB_VERTEX,
            const float cuspangledegrees = GEO_DEFAULT_ADJUSTED_CUSP_ANGLE,
            const GEO_NormalMethod method = GEO_NormalMethod::ANGLE_WEIGHTED,
            const bool copy_orig_if_zero = false
        )
    {
        UT_ASSERT_P(attribClass != GA_ATTRIB_OWNER_N && attribClass != GA_ATTRIB_INVALID);

        if (!posAttrib)
            posAttrib = geo->findPointAttribute("P");

        UT_ASSERT_P(posAttrib);
        UT_ASSERT_P(posAttrib->getAIFTuple());

        const GA_Storage finalStorage = storage == GA_STORE_INVALID ? posAttrib->getAIFTuple()->getStorage(posAttrib) : storage;

        UT_ASSERT_P(
            finalStorage == GA_STORE_REAL16 ||
            finalStorage == GA_STORE_REAL32 ||
            finalStorage == GA_STORE_REAL64);

        GA_AttributeUPtr normal3DAttribUPtr = geo->getAttributes().createDetachedTupleAttribute(attribClass, finalStorage, 3, GA_Defaults(0.0));
        GA_Attribute* const normal3DAttrib = normal3DAttribUPtr.get();

        computeNormal3D(geo, posAttrib, normal3DAttrib, geoGroup, finalStorage, cuspangledegrees, method, copy_orig_if_zero);

        return normal3DAttribUPtr;
    }



    //GFE_Normal::findAndAddAttribNormal3D();

    //!!!!! WARNING !!!!! this func may return a detached attrib
    static GA_Attribute*
        findOrAddAttribNormal3D(
            GA_Detail* const geo,
            GA_AttributeUPtr& normal3DAttribUPtr,
            const GA_Attribute* const posAttrib = nullptr,
            const GA_Group* const geoGroup = nullptr,
            const GA_Storage storage = GA_STORE_INVALID,
            const UT_StringHolder& normal3DAttribName = "N",

            const bool findNormal3D = false,
            const bool addNormal3DIfNoFind = true,
            const GFE_NormalSearchOrder normalSearchOrder = GFE_NormalSearchOrder_INVALID,
            const float cuspangledegrees = GEO_DEFAULT_ADJUSTED_CUSP_ANGLE,
            const GEO_NormalMethod method = GEO_NormalMethod::ANGLE_WEIGHTED,
            const bool copy_orig_if_zero = false
        )
    {
        GA_Attribute* normal3DAttrib = nullptr;
        if (findNormal3D)
        {
            switch (normalSearchOrder)
            {
            case GFE_NormalSearchOrder_PRIMITIVE:
                normal3DAttrib = geo->findPrimitiveAttribute(normal3DAttribName);
                //if (!normal3DAttrib)
                //    return nullptr;
                break;
            case GFE_NormalSearchOrder_POINT:
                normal3DAttrib = geo->findPointAttribute(normal3DAttribName);
                //if (!normal3DAttrib)
                //    return nullptr;
                break;
            case GFE_NormalSearchOrder_VERTEX:
                normal3DAttrib = geo->findVertexAttribute(normal3DAttribName);
                //if (!normal3DAttrib)
                //    return nullptr;
                break;
            case GFE_NormalSearchOrder_DETAIL:
                normal3DAttrib = geo->findGlobalAttribute(normal3DAttribName);
                //if (!normal3DAttrib)
                //    return nullptr;
                break;
            case GFE_NormalSearchOrder_N:
                normal3DAttrib = geo->findPrimitiveAttribute(normal3DAttribName);
                if (!normal3DAttrib)
                {
                    normal3DAttrib = geo->findPointAttribute(normal3DAttribName);
                    if (!normal3DAttrib)
                    {
                        normal3DAttrib = geo->findVertexAttribute(normal3DAttribName);
                        if (!normal3DAttrib)
                        {
                            normal3DAttrib = geo->findGlobalAttribute(normal3DAttribName);
                            //if (!normal3DAttrib)
                            //    return nullptr;
                        }
                    }
                }
                break;
            case GFE_NormalSearchOrder_POINTVERTEX:
                normal3DAttrib = geo->findPointAttribute(normal3DAttribName);
                if (!normal3DAttrib)
                {
                    normal3DAttrib = geo->findVertexAttribute(normal3DAttribName);
                    //if (!normal3DAttrib)
                    //    return nullptr;
                }
                break;
            default:
                UT_ASSERT_MSG(0, "unhandled GFE_NormalSearchOrder");
                //return nullptr;
                break;
            }
        }

        if (!findNormal3D || (addNormal3DIfNoFind && !normal3DAttrib))
        {
            GA_AttributeOwner geoNormal3DAttribClass;
            switch (normalSearchOrder)
            {
            case GFE_NormalSearchOrder_PRIMITIVE:
                geoNormal3DAttribClass = GA_ATTRIB_PRIMITIVE;
                break;
            case GFE_NormalSearchOrder_POINT:
                geoNormal3DAttribClass = GA_ATTRIB_POINT;
                break;
            case GFE_NormalSearchOrder_VERTEX:
                geoNormal3DAttribClass = GA_ATTRIB_VERTEX;
                break;
            default:
                return nullptr;
                break;
            }
            //if (!findNormal3D)
            //{
            //    GA_Attribute* normal3DAttrib = geo->findAttribute(geoNormal3DAttribClass, normal3DAttribName);
            //    if (normal3DAttrib)
            //        return normal3DAttrib;
            //}
            normal3DAttribUPtr = addDetachedAttribNormal3D(geo, posAttrib, geoGroup, storage,
                geoNormal3DAttribClass,
                cuspangledegrees, method, copy_orig_if_zero);
            normal3DAttrib = normal3DAttribUPtr.get();
        }

        //if (normal3DAttrib && normal3DAttrib->getOwner() == GA_ATTRIB_DETAIL)
        //{
        //    const GA_ROHandleT<UT_Vector3T<fpreal64>> normal3D_h(normal3DAttrib);
        //    defaultNormal3D = normal3D_h.get(0);
        //}

        return normal3DAttrib;
    }


    static GA_Attribute*
        findOrAddAttribNormal3D(
            GA_Detail* const geo,
            const GA_Attribute* posAttrib = nullptr,
            const GA_Group* const geoGroup = nullptr,
            const GA_Storage storage = GA_STORE_INVALID,
            const UT_StringHolder& normal3DAttribName = "N",

            const bool findNormal3D = false,
            const bool addNormal3DIfNoFind = true,
            const GFE_NormalSearchOrder normalSearchOrder = GFE_NormalSearchOrder_INVALID,
            const float cuspangledegrees = GEO_DEFAULT_ADJUSTED_CUSP_ANGLE,
            const GEO_NormalMethod method = GEO_NormalMethod::ANGLE_WEIGHTED,
            const bool copy_orig_if_zero = false
        )
    {
        GA_Attribute* normal3DAttrib = nullptr;
        if (findNormal3D)
        {
            switch (normalSearchOrder)
            {
            case GFE_NormalSearchOrder_PRIMITIVE:
                normal3DAttrib = geo->findPrimitiveAttribute(normal3DAttribName);
                //if (!normal3DAttrib)
                //    return nullptr;
                break;
            case GFE_NormalSearchOrder_POINT:
                normal3DAttrib = geo->findPointAttribute(normal3DAttribName);
                //if (!normal3DAttrib)
                //    return nullptr;
                break;
            case GFE_NormalSearchOrder_VERTEX:
                normal3DAttrib = geo->findVertexAttribute(normal3DAttribName);
                //if (!normal3DAttrib)
                //    return nullptr;
                break;
            case GFE_NormalSearchOrder_DETAIL:
                normal3DAttrib = geo->findGlobalAttribute(normal3DAttribName);
                //if (!normal3DAttrib)
                //    return nullptr;
                break;
            case GFE_NormalSearchOrder_N:
                normal3DAttrib = geo->findPrimitiveAttribute(normal3DAttribName);
                if (!normal3DAttrib)
                {
                    normal3DAttrib = geo->findPointAttribute(normal3DAttribName);
                    if (!normal3DAttrib)
                    {
                        normal3DAttrib = geo->findVertexAttribute(normal3DAttribName);
                        if (!normal3DAttrib)
                        {
                            normal3DAttrib = geo->findGlobalAttribute(normal3DAttribName);
                            //if (!normal3DAttrib)
                            //    return nullptr;
                        }
                    }
                }
                break;
            case GFE_NormalSearchOrder_POINTVERTEX:
                normal3DAttrib = geo->findPointAttribute(normal3DAttribName);
                if (!normal3DAttrib)
                {
                    normal3DAttrib = geo->findVertexAttribute(normal3DAttribName);
                    //if (!normal3DAttrib)
                    //    return nullptr;
                }
                break;
            default:
                UT_ASSERT_MSG(0, "unhandled GFE_NormalSearchOrder");
                //return nullptr;
                break;
            }
        }

        if (!findNormal3D || (addNormal3DIfNoFind && !normal3DAttrib))
        {
            GA_AttributeOwner geoNormal3DAttribClass;
            switch (normalSearchOrder)
            {
            case GFE_NormalSearchOrder_PRIMITIVE:
                geoNormal3DAttribClass = GA_ATTRIB_PRIMITIVE;
                break;
            case GFE_NormalSearchOrder_POINT:
                geoNormal3DAttribClass = GA_ATTRIB_POINT;
                break;
            case GFE_NormalSearchOrder_VERTEX:
                geoNormal3DAttribClass = GA_ATTRIB_VERTEX;
                break;
            default:
                return nullptr;
                break;
            }
            //if (!findNormal3D)
            //{
            //    GA_Attribute* normal3DAttrib = geo->findAttribute(geoNormal3DAttribClass, normal3DAttribName);
            //    if (normal3DAttrib)
            //        return normal3DAttrib;
            //}
            normal3DAttrib = addAttribNormal3D(geo, posAttrib, geoGroup, storage, normal3DAttribName, geoNormal3DAttribClass,
                cuspangledegrees, method, copy_orig_if_zero);
        }
        //if (normal3DAttrib && normal3DAttrib->getOwner() == GA_ATTRIB_DETAIL)
        //{
        //    const GA_ROHandleT<UT_Vector3T<fpreal64>> normal3D_h(normal3DAttrib);
        //    defaultNormal3D = normal3D_h.get(0);
        //}

        return normal3DAttrib;
    }


















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


        const GA_AttributeOwner attribClassNormal3D = normal3D_h.getAttribute() ? normal3D_h.getAttribute()->getOwner() : GA_ATTRIB_INVALID;
        if (attribClassNormal3D == GA_ATTRIB_GLOBAL)
            defaultNormal3D = normal3D_h.get(0);

        GA_Topology& topo = geo->getTopology();
        topo.makePrimitiveRef();
        const GA_ATITopology* const vtxPointRef = topo.getPointRef();
        //const GA_ATITopology* const pointVtxRef = topo.getVertexRef();
        const GA_ATITopology* const vtxPrimRef = topo.getPrimitiveRef();


        const GA_SplittableRange geoVertexSplittableRange(geo->getVertexRange(unsharedVertexGroup));
        UTparallelFor(geoVertexSplittableRange, [&pos_h, &dstptAttrib_h, &normal2D_h, &normal3D_h,
            vtxPointRef, vtxPrimRef,
            attribClassNormal3D, &defaultNormal3D](const GA_SplittableRange& r)
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
                    switch (attribClassNormal3D)
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
            const UT_StringHolder& normal2DAttribName = "N",

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
            const UT_StringHolder& normal2DAttribName = "N",

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
            const UT_StringHolder& normal2DAttribName = "N",

            const UT_Vector3T<fpreal64>& defaultNormal3D = { 0,1,0 },
            const bool scaleByTurns = true,
            const bool normalize = true,
            const fpreal64 uniScale = 1.0,

            const bool useConstantNormal3D = false,
            const UT_StringHolder& normal3DAttribName = "N",
            const bool findNormal3D = false,
            const bool addNormal3DIfNoFind = true,
            const GFE_NormalSearchOrder normalSearchOrder = GFE_NormalSearchOrder_INVALID,
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
            const UT_StringHolder& posAttribName = "P",
            const GA_GroupType groupType = GA_GROUP_INVALID,
            const UT_StringHolder& groupName = "",
            const GA_Storage storage = GA_STORE_INVALID,
            const UT_StringHolder& normal2DAttribName = "N",

            const UT_Vector3T<fpreal64>& defaultNormal3D = { 0,1,0 },
            const bool scaleByTurns = true,
            const bool normalize = true,
            const fpreal64 uniScale = 1.0,

            const bool useConstantNormal3D = false,
            const UT_StringHolder& normal3DAttribName = "N",
            const bool findNormal3D = false,
            const bool addNormal3DIfNoFind = true,
            const GFE_NormalSearchOrder normalSearchOrder = GFE_NormalSearchOrder_INVALID,
            const float cuspangledegrees = GEO_DEFAULT_ADJUSTED_CUSP_ANGLE,
            const GEO_NormalMethod method = GEO_NormalMethod::ANGLE_WEIGHTED,
            const bool copy_orig_if_zero = false,


            const exint subscribeRatio = 64,
            const exint minGrainSize = 1024
        )
    {
        GOP_Manager gop;
        const GA_Group* const geoGroup = GFE_Group::findOrParseGroupDetached(cookparms, geo, groupType, groupName, gop);
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

        GA_Attribute* const normal2DAttrib = GFE_Normal::addAttribNormal2D(geo, posAttrib,
            geo0PointGroup, storage, normal2DAttribName,
            defaultNormal3D, scaleByTurns, normalize, uniScale,

            useConstantNormal3D, normal3DAttribName,
            findNormal3D, addNormal3DIfNoFind, normalSearchOrder,
            cuspangledegrees, method, copy_orig_if_zero,
            subscribeRatio, minGrainSize);


        return normal2DAttrib;
    }





} // End of namespace GFE_Normal

#endif
