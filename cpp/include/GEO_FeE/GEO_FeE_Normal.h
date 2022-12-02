
#pragma once

#ifndef __GEO_FeE_Normal_h__
#define __GEO_FeE_Normal_h__

//#include <GEO_FeE/GEO_FeE_Normal.h>

#include <GEO/GEO_Detail.h>

#include <GEO/GEO_Normal.h>

#include <GA_FeE/GA_FeE_TopologyReference.h>
#include <GA_FeE/GA_FeE_VertexNextEquiv.h>
#include <GEO_FeE/GEO_FeE_Group.h>


namespace GEO_FeE_Normal {



using attribPrecisonF = fpreal32;
using TAttribTypeV = UT_Vector3T<attribPrecisonF>;




//GEO_FeE_Normal::addAttribNormal3D(outGeo0, nullptr, attribClass, GA_STORE_REAL32, "N", GEO_DEFAULT_ADJUSTED_CUSP_ANGLE, GEO_NormalMethod::ANGLE_WEIGHTED, false, nullptr);
//GEO_FeE_Normal::addAttribNormal3D(geo, geoGroup, attribClass, storage, N3DAttribName, cuspangledegrees, method, copy_orig_if_zero, posAttrib);

SYS_FORCE_INLINE
static GA_Attribute*
addAttribNormal3D(
    GEO_Detail* geo,
    const GA_Group* geoGroup = nullptr,
    const GA_AttributeOwner attribClass = GA_ATTRIB_VERTEX,
    const GA_Storage storage = GA_STORE_REAL32,
    const UT_StringHolder& N3DAttribName = "N",
    const float cuspangledegrees = GEO_DEFAULT_ADJUSTED_CUSP_ANGLE,
    const GEO_NormalMethod method = GEO_NormalMethod::ANGLE_WEIGHTED,
    const bool copy_orig_if_zero = false,
    const GA_Attribute* posAttrib = nullptr
)
{
    if(!posAttrib)
        posAttrib = geo->findPointAttribute("P");

    UT_ASSERT_P(posAttrib);
    UT_ASSERT_P(attribClass != GA_ATTRIB_OWNER_N && attribClass != GA_ATTRIB_INVALID);

    GA_Attribute* normal3DAttrib = geo->addFloatTuple(attribClass, N3DAttribName, 3, GA_Defaults(0.0), nullptr, nullptr, storage);
    GEOcomputeNormals(*geo, posAttrib, normal3DAttrib, geoGroup, cuspangledegrees, method, copy_orig_if_zero);
    return normal3DAttrib;
}




static void
computeNormal2D(
    GEO_Detail* geo,
    const GA_PointGroup* geoPointGroup,
    GA_Attribute* N2DAttrib,
    const GA_Attribute* posAttrib = nullptr,
    const GA_Attribute* N3DAttrib = nullptr,
    UT_Vector3T<fpreal64> defaultN3D = { 0,1,0 },
    const bool scaleByTurns = true,
    const bool normalize = true,
    const fpreal64 uniScale = 1.0,
    const exint subscribeRatio = 64,
    const exint minGrainSize = 1024
)
{
    const GA_AttributeOwner attribCalssN3D = N3DAttrib ? N3DAttrib->getOwner() : GA_ATTRIB_INVALID;

    const GA_VertexGroup* unsharedGroup = GA_FeE_VertexNextEquiv::addGroupVertexNextEquiv(geo, nullptr);
    //GA_PointGroup* unsharedPointGroup = const_cast<GA_PointGroup*>(GEO_FeE_Group::groupPromote(outGeo0, unsharedGroup, GA_GROUP_POINT, geo0AttribNames, true));
    const GA_PointGroup* unsharedPointGroup = GEO_FeE_Group::groupPromotePointDetached(geo, unsharedGroup);
    GEO_FeE_Group::groupBoolean(geo, unsharedPointGroup, geoPointGroup);

    const GA_VertexGroup* geoVertexGroup = const_cast<GA_VertexGroup*>(GEO_FeE_Group::groupPromoteVertexDetached(geo, geoPointGroup));

    if(!posAttrib)
        posAttrib = geo->findPointAttribute("P");

    const GA_Attribute* dstptAttrib = GA_FeE_TopologyReference::addAttribVertexPointDst(geo, geoVertexGroup);
    const GA_ROHandleT<GA_Offset> dstptAttribH(dstptAttrib);

    const GA_ROHandleT<UT_Vector3T<fpreal64>> posH(posAttrib);
    const GA_ROHandleT<UT_Vector3T<fpreal64>> N3DH(N3DAttrib);
    const GA_RWHandleT<UT_Vector3T<fpreal64>> N2DH(N2DAttrib);

    if (attribCalssN3D == GA_ATTRIB_GLOBAL)
        defaultN3D = N3DH.get(0);

    GA_Topology& topo = geo->getTopology();
    topo.makePrimitiveRef();
    const GA_ATITopology* vtxPointRef = topo.getPointRef();
    //const GA_ATITopology* pointVtxRef = topo.getVertexRef();
    const GA_ATITopology* vtxPrimRef = topo.getPrimitiveRef();


    const GA_SplittableRange geoVertexSplittableRange(geo->getVertexRange(geoVertexGroup));
    UTparallelFor(geoVertexSplittableRange, [&geo, &posH, &dstptAttribH, &N2DH, &N3DH,
        &vtxPointRef, &vtxPrimRef,
        &attribCalssN3D, &defaultN3D](const GA_SplittableRange& r)
    {
        UT_Vector3T<fpreal64> dir;
        GA_Offset start, end;
        for (GA_Iterator it(r); it.blockAdvance(start, end); )
        {
            for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
            {
                GA_Offset ptoff = vtxPointRef->getLink(elemoff);
                GA_Offset dstpt = dstptAttribH.get(elemoff);

                dir = posH.get(ptoff) - posH.get(dstpt);
                switch (attribCalssN3D)
                {
                case GA_ATTRIB_PRIMITIVE:
                    dir.cross(N3DH.get(vtxPrimRef->getLink(elemoff)));
                    break;
                case GA_ATTRIB_POINT:
                    dir.cross(N3DH.get(ptoff));
                    break;
                case GA_ATTRIB_VERTEX:
                    dir.cross(N3DH.get(elemoff));
                    break;
                default:
                    dir.cross(defaultN3D);
                    break;
                }
                dir.normalize();
                N2DH.add(ptoff, dir);
                N2DH.add(dstpt, dir);
            }
        }
    }, subscribeRatio, minGrainSize);


    const GA_SplittableRange geoPointSplittableRange(geo->getPointRange(geoPointGroup));
    UTparallelFor(geoPointSplittableRange, [&geo, &N2DAttrib, &uniScale, &defaultN3D, &scaleByTurns, &normalize](const GA_SplittableRange& r)
    {
        GA_PageHandleV<UT_Vector3T<fpreal64>>::RWType N2D_PH(N2DAttrib);
        for (GA_PageIterator pit = r.beginPages(); !pit.atEnd(); ++pit)
        {
            GA_Offset start, end;
            for (GA_Iterator it(pit.begin()); it.blockAdvance(start, end); )
            {
                N2D_PH.setPage(start);
                for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                {
                    if (scaleByTurns)
                    {
                        N2D_PH.value(elemoff) *= 2 * uniScale / N2D_PH.value(elemoff).length2();
                    }
                    else if (normalize)
                    {
                        N2D_PH.value(elemoff).normalize();
                        N2D_PH.value(elemoff) *= uniScale;
                    }
                }
            }
        }
    }, subscribeRatio, minGrainSize);

}

/*
template<typename T>
static T
computeNormal2D(
    const GEO_Detail* geo,
    const GA_ROHandleT<UT_Vector3T<T>>& posH,
    const GA_RWHandleT<UT_Vector3T<T>>& N2DH,
    const GA_Group* geoGroup = nullptr,
    const float cuspangledegrees = GEO_DEFAULT_ADJUSTED_CUSP_ANGLE,
    const GEO_NormalMethod method = GEO_NormalMethod::ANGLE_WEIGHTED,
    const bool copy_orig_if_zero = false
)
{
    posH.getAttribute()->getOwner();
    GEO_Normal::compute
}
//using triangleArea = heronsFormula;
//typedef heronsFormula triangleArea;
*/







} // End of namespace GEO_FeE_Normal

#endif