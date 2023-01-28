
#pragma once

#ifndef __GA_FeE_Normal_h__
#define __GA_FeE_Normal_h__

//#include "GEO_FeE/GA_FeE_Normal.h"

#include "GA/GA_Detail.h"

#include "GEO/GEO_Normal.h"

#include "GA_FeE/GA_FeE_TopologyReference.h"
#include "GA_FeE/GA_FeE_VertexNextEquiv.h"
#include "GA_FeE/GA_FeE_GroupBoolean.h"
#include "GA_FeE/GA_FeE_GroupExpand.h"


namespace GA_FeE_Normal {



    static void
        computeNormal3D(
            GA_Detail* const geo,
            const GA_ROHandleT<UT_Vector3T<fpreal32>>& posAttrib_h,
            const GA_RWHandleT<UT_Vector3T<fpreal32>>& normalAttrib_h,
            const GA_Group* const geoGroup = nullptr,
            const float cuspangledegrees = GEO_DEFAULT_ADJUSTED_CUSP_ANGLE,
            const GEO_NormalMethod method = GEO_NormalMethod::ANGLE_WEIGHTED,
            const bool copy_orig_if_zero = false
        )
    {
        GEOcomputeNormals(*static_cast<GEO_Detail*>(geo), posAttrib_h, normalAttrib_h, geoGroup, cuspangledegrees, method, copy_orig_if_zero);
    }

#if SYS_VERSION_MAJOR_INT > 19 || ( SYS_VERSION_MAJOR_INT == 19 && SYS_VERSION_MINOR_INT == 5 )
    static void
        computeNormal3D(
            GA_Detail* const geo,
            const GA_ROHandleT<UT_Vector3T<fpreal64>>& posAttrib_h,
            const GA_RWHandleT<UT_Vector3T<fpreal64>>& normalAttrib_h,
            const GA_Group* const geoGroup = nullptr,
            const float cuspangledegrees = GEO_DEFAULT_ADJUSTED_CUSP_ANGLE,
            const GEO_NormalMethod method = GEO_NormalMethod::ANGLE_WEIGHTED,
            const bool copy_orig_if_zero = false
        )
    {
        GEOcomputeNormals(*static_cast<GEO_Detail*>(geo), posAttrib_h, normalAttrib_h, geoGroup, cuspangledegrees, method, copy_orig_if_zero);
    }
#endif



    //GA_FeE_Normal::addAttribNormal3D(outGeo0, nullptr, attribClass, GA_STORE_REAL32, "N", GEO_DEFAULT_ADJUSTED_CUSP_ANGLE, GEO_NormalMethod::ANGLE_WEIGHTED, false, nullptr);
    //GA_FeE_Normal::addAttribNormal3D(geo, geoGroup, attribClass, storage, N3DAttribName, cuspangledegrees, method, copy_orig_if_zero, posAttrib);

    static GA_Attribute*
        addAttribNormal3D(
            GA_Detail* const geo,
            const GA_Group* const geoGroup = nullptr,
            const GA_AttributeOwner attribClass = GA_ATTRIB_VERTEX,
            const GA_Storage storage = GA_STORE_INVALID,
            const UT_StringHolder& N3DAttribName = "N",
            const float cuspangledegrees = GEO_DEFAULT_ADJUSTED_CUSP_ANGLE,
            const GEO_NormalMethod method = GEO_NormalMethod::ANGLE_WEIGHTED,
            const bool copy_orig_if_zero = false,
            const GA_Attribute* posAttrib = nullptr
        )
    {
        if (!posAttrib)
            posAttrib = geo->findPointAttribute("P");

        UT_ASSERT_P(posAttrib);
        UT_ASSERT_P(posAttrib->getAIFTuple());
        UT_ASSERT_P(attribClass != GA_ATTRIB_OWNER_N && attribClass != GA_ATTRIB_INVALID);

        const GA_Storage fianlStorage = storage == GA_STORE_INVALID ? posAttrib->getAIFTuple()->getStorage(posAttrib) : storage;

        UT_ASSERT_P(
            fianlStorage == GA_STORE_REAL16 ||
            fianlStorage == GA_STORE_REAL32 ||
            fianlStorage == GA_STORE_REAL64);

        GA_Attribute* const normal3DAttrib = geo->getAttributes().createTupleAttribute(attribClass, N3DAttribName, fianlStorage, 3, GA_Defaults(0.0));

#if SYS_VERSION_MAJOR_INT > 19 || ( SYS_VERSION_MAJOR_INT == 19 && SYS_VERSION_MINOR_INT == 5 )
        if (fianlStorage == GA_STORE_REAL64)
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
        GA_VertexGroup* const unsharedVertexGroup = GA_FeE_VertexNextEquiv::addGroupVertexNextEquiv(geo);
        //GA_PointGroup* unsharedPointGroup = const_cast<GA_PointGroup*>(GEO_FeE_Group::groupPromote(outGeo0, unsharedGroup, GA_GROUP_POINT, geo0AttribNames, true));
#if 0
        GA_PointGroup* const unsharedPointGroup = GA_FeE_GroupPromote::groupPromotePointDetached(geo, unsharedGroup);
#else
        GA_PointGroup* const unsharedPointGroup = GA_FeE_GroupPromote::groupPromotePoint(geo, unsharedVertexGroup);
#endif
        if (geoPointGroup)
        {
            GA_PointGroup* const expandGroup = geo->newDetachedPointGroup();
            UT_UniquePtr<GA_PointGroup> expandGroupUPtr(expandGroup);

            GA_FeE_GroupExpand::groupExpand(geo, expandGroup, geoPointGroup, GA_GROUP_EDGE);

            *unsharedPointGroup &= *expandGroup;
            //GA_FeE_GroupBoolean::groupIntersect(geo, unsharedPointGroup, expandGroup);
            GA_FeE_GroupBoolean::groupIntersect(geo, unsharedVertexGroup, expandGroup);

            //GA_PointGroup* expandGroup = GA_FeE_Group::newDetachedGroup(outGeo0, geo0Group);
        }


        //unsharedPointGroup->operator&=(*geoPointGroup);
        //GEO_FeE_Group::groupBoolean(geo, unsharedPointGroup, geoPointGroup, GA_GROUP_BOOLEAN_AND);
        //unsharedPointGroup->makeUnordered();
        //unsharedPointGroup->andEqual(geoPointGroup);


        //GA_FeE_Group::groupBumpDataId(unsharedVertexGroup);
        //GA_FeE_Group::groupBumpDataId(unsharedPointGroup);


        //const GA_Attribute* const dstptAttrib = GA_FeE_TopologyReference::addAttribVertexPointDst(geo, unsharedVertexGroup);
        const GA_Attribute* const dstptAttrib = GA_FeE_TopologyReference::addAttribVertexPointDst(geo, nullptr);
        const GA_ROHandleT<GA_Offset> dstptAttrib_h(dstptAttrib);


        const GA_AttributeOwner attribCalssNormal3D = normal3D_h.getAttribute() ? normal3D_h.getAttribute()->getOwner() : GA_ATTRIB_INVALID;
        if (attribCalssNormal3D == GA_ATTRIB_GLOBAL)
            defaultNormal3D = normal3D_h.get(0);

        GA_Topology& topo = geo->getTopology();
        topo.makePrimitiveRef();
        const GA_ATITopology* const vtxPointRef = topo.getPointRef();
        //const GA_ATITopology* pointVtxRef = topo.getVertexRef();
        const GA_ATITopology* const vtxPrimRef = topo.getPrimitiveRef();


        const GA_SplittableRange geoVertexSplittableRange(geo->getVertexRange(unsharedVertexGroup));
        UTparallelFor(geoVertexSplittableRange, [&pos_h, &dstptAttrib_h, &normal2D_h, &normal3D_h,
            vtxPointRef, vtxPrimRef,
            attribCalssNormal3D, &defaultNormal3D](const GA_SplittableRange& r)
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
                    switch (attribCalssNormal3D)
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


        GA_Attribute* const normal2DAttrib = normal2D_h.getAttribute();

        const GA_SplittableRange geoPointSplittableRange(geo->getPointRange(unsharedPointGroup));
        UTparallelFor(geoPointSplittableRange, [normal2DAttrib, uniScale, scaleByTurns, normalize](const GA_SplittableRange& r)
        {
            GA_PageHandleV<UT_Vector3T<T>>::RWType normal2D_ph(normal2DAttrib);
            for (GA_PageIterator pit = r.beginPages(); !pit.atEnd(); ++pit)
            {
                GA_Offset start, end;
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
        case GA_STORE_REAL16:
            computeNormal2D<fpreal16>(geo, geoPointGroup, normal2DAttrib, posAttrib, normal3DAttrib,
                defaultNormal3D, scaleByTurns, normalize, uniScale,
                subscribeRatio, minGrainSize);
            break;
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
            const GA_PointGroup* const geoPointGroup,
            const UT_StringHolder& N2DAttribName,
            const GA_Attribute* const posAttrib = nullptr,
            const GA_Attribute* const normal3DAttrib = nullptr,
            UT_Vector3T<fpreal64> defaultNormal3D = { 0,1,0 },
            const bool scaleByTurns = true,
            const bool normalize = true,
            const fpreal64 uniScale = 1.0,
            const GA_Storage storage = GA_STORE_INVALID,
            const exint subscribeRatio = 64,
            const exint minGrainSize = 1024
        )
    {
        GA_Attribute* const normal2DAttrib = geo->getAttributes().createTupleAttribute(GA_ATTRIB_POINT, N2DAttribName,
            storage == GA_STORE_INVALID ? GA_FeE_Type::getPreferredStorageF(geo) : storage,
            3, GA_Defaults(0.0));
        //GA_Attribute* const normal2DAttrib = geo->addFloatTuple(GA_ATTRIB_POINT, N2DAttribName, 3, GA_Defaults(0.0), nullptr, nullptr, inStorageF);

        GA_FeE_Normal::computeNormal2D(geo, geoPointGroup,
            normal2DAttrib, posAttrib, normal3DAttrib, defaultNormal3D,
            scaleByTurns, normalize, uniScale,
            subscribeRatio, minGrainSize);
        return normal2DAttrib;
    }
    /*
    template<typename T>
    static T
    computeNormal2D(
        const GA_Detail* const geo,
        const GA_ROHandleT<UT_Vector3T<T>>& pos_h,
        const GA_RWHandleT<UT_Vector3T<T>>& normal2D_h,
        const GA_Group* const geoGroup = nullptr,
        const float cuspangledegrees = GEO_DEFAULT_ADJUSTED_CUSP_ANGLE,
        const GEO_NormalMethod method = GEO_NormalMethod::ANGLE_WEIGHTED,
        const bool copy_orig_if_zero = false
    )
    {
        pos_h.getAttribute()->getOwner();
        GEO_Normal::compute
    }
    //using triangleArea = heronsFormula;
    //typedef heronsFormula triangleArea;
    */







} // End of namespace GA_FeE_Normal

#endif
