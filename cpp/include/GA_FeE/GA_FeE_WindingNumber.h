
#pragma once

#ifndef __GA_FeE_WindingNumber_h__
#define __GA_FeE_WindingNumber_h__



//#include "GA_FeE/GA_FeE_WindingNumber.h"

#include "GA/GA_Detail.h"


#include "UT/UT_SolidAngle.h"

#include <GEO/GEO_Curve.h>
#include <GEO/GEO_PrimMesh.h>
#include <GEO/GEO_PrimPolySoup.h>
#include <GEO/GEO_PrimSphere.h>
#include <GEO/GEO_PrimTetrahedron.h>
#include <GEO/GEO_TPSurf.h>
//#include <GA/GA_Handle.h>
#include <GA/GA_SplittableRange.h>
//#include <GA/GA_Types.h>
//#include <OP/OP_Operator.h>
//#include <OP/OP_OperatorTable.h>
//#include "PRM/PRM_TemplateBuilder.h"
//#include <UT/UT_DSOVersion.h>
//#include <UT/UT_Interrupt.h>
#include <UT/UT_ParallelUtil.h>
#include <UT/UT_StringHolder.h>
#include <UT/UT_UniquePtr.h>
#include <SYS/SYS_Math.h>
#include <SOP/SOP_NodeVerb.h>





#include "GA_FeE/GA_FeE_Type.h"





enum GA_WindingNumberType
{
    GA_WNType_XYZ,
    GA_WNType_XY,
    GA_WNType_YZ,
    GA_WNType_ZX,
};

namespace GA_FeE_WindingNumber {


// UT_SolidAngle doesn't currently have a special case to handle quads correctly
// as bilinear patches; it always treats them as two triangles, which can result
// in a difference of about 1 between points that are just inside or just
// outside.  If you want the accurate version to treat them as triangles too,
// (e.g. for testing the approximation error of UT_SolidAngle), set this to 0.
#define QUADS_AS_BILINEAR_PATCHES 1



//
// Help is stored in a "wiki" style text file.  This text file should be copied
// to $HOUDINI_PATH/help/nodes/sop/hdk_windingnumber.txt
//
// See the sample_install.sh file for an example.
//

// Forward declarations of file-scope functions used before they're defined

    static void
        sopAccumulateTriangles(
            const GA_Detail* const geoRefMesh,
            const GA_Offset primoff,
            const UT_Array<int>& ptmap,
            UT_Array<int>& triangle_points);

    static void
        sopAccumulateSegments(
            const GA_Detail* const geoRefMesh,
            const GA_Offset primoff,
            const UT_Array<int>& ptmap,
            UT_Array<int>& segment_points);

    //******************************************************************************
    //*                 Setup                                                      *
    //******************************************************************************

typedef int64                   GA_DataId;
#define GA_INVALID_DATAID       GA_DataId(-1)










    /// This class is for caching data between cooks, e.g. so that we don't have to
    /// rebuild the UT_SolidAngle tree on every cook if the input hasn't changed.
    class GA_WindingNumber_Cache : public SOP_NodeCache
    {
    public:
        GA_WindingNumber_Cache() : SOP_NodeCache()
            , mySolidAngleTree()
            , mySubtendedAngleTree()
            , myTopologyDataID(GA_INVALID_DATAID)
            , myPrimitiveListDataID(GA_INVALID_DATAID)
            , myPDataID(GA_INVALID_DATAID)
            , myApproxOrder(-1)
            , myAxis0(-1)
            , myHadGroup(false)
            , myGroupString()
            , myUniqueId(-1)
            , myMetaCacheCount(-1)
        {}
        virtual ~GA_WindingNumber_Cache() {}

        void update3D(const GA_Detail& geoRefMesh, const GA_PrimitiveGroup* prim_group, const UT_StringHolder& group_string, const int approx_order)
        {
            const GA_DataId topology_data_id = geoRefMesh.getTopology().getDataId();
            const GA_DataId primitive_list_data_id = geoRefMesh.getPrimitiveList().getDataId();
            const GA_DataId P_data_id = geoRefMesh.getP()->getDataId();
            const bool has_group = (prim_group != nullptr);
            if (mySubtendedAngleTree.isClear() &&
                topology_data_id == myTopologyDataID &&
                primitive_list_data_id == myPrimitiveListDataID &&
                P_data_id == myPDataID &&
                approx_order == myApproxOrder &&
                has_group == myHadGroup &&
                (!has_group || (
                    group_string == myGroupString &&
                    geoRefMesh.getUniqueId() == myUniqueId &&
                    geoRefMesh.getMetaCacheCount() == myMetaCacheCount)))
            {
                return;
            }
            mySubtendedAngleTree.clear();
            myPositions2D.clear();

            UT_AutoInterrupt boss("Constructing Solid Angle Tree");

            myTopologyDataID = topology_data_id;
            myPrimitiveListDataID = primitive_list_data_id;
            myPDataID = P_data_id;
            myApproxOrder = approx_order;
            myHadGroup = has_group;
            myGroupString = group_string;
            int a = 1;
            myUniqueId = geoRefMesh.getUniqueId();
            myMetaCacheCount = geoRefMesh.getMetaCacheCount();

            GA_Size nprimitives = prim_group ? prim_group->entries() : geoRefMesh.getNumPrimitives();
            myTrianglePoints.setSize(0);
            myTrianglePoints.setCapacity(3 * nprimitives);
            myPositions3D.setSize(0);
            myPositions3D.setCapacity(0);
            UT_Array<int> ptmap;
            if (!prim_group)
            {
                // Copy all point positions
                geoRefMesh.getAttributeAsArray(geoRefMesh.getP(), geoRefMesh.getPointRange(), myPositions3D);
            }
            else
            {
                // Find the used points
                GA_PointGroup mesh_ptgroup(geoRefMesh);
                mesh_ptgroup.combine(prim_group);
                myPositions3D.setSizeNoInit(mesh_ptgroup.entries());
                ptmap.setSizeNoInit(geoRefMesh.getNumPointOffsets());
                int ptnum = 0;
                geoRefMesh.forEachPoint(&mesh_ptgroup, [&geoRefMesh, this, &ptmap, &ptnum](const GA_Offset ptoff)
                {
                    ptmap[ptoff] = ptnum;
                    myPositions3D[ptnum] = geoRefMesh.getPos3(ptoff);
                    ++ptnum;
                });
            }

            GA_Offset start;
            GA_Offset end;
            for (GA_Iterator it(geoRefMesh.getPrimitiveRange(prim_group)); it.blockAdvance(start, end); )
            {
                for (GA_Offset primoff = start; primoff < end; ++primoff)
                {
                    sopAccumulateTriangles(&geoRefMesh, primoff, ptmap, myTrianglePoints);
                }
            }

            mySolidAngleTree.init(myTrianglePoints.size() / 3, myTrianglePoints.array(), myPositions3D.size(), myPositions3D.array(), approx_order);
        }

        void update2D(
            const GA_Detail& geoRefMesh,
            const GA_PrimitiveGroup* prim_group,
            const UT_StringHolder& group_string,
            const int approx_order,
            const int axis0, const int axis1)
        {
            const GA_DataId topology_data_id = geoRefMesh.getTopology().getDataId();
            const GA_DataId primitive_list_data_id = geoRefMesh.getPrimitiveList().getDataId();
            const GA_DataId P_data_id = geoRefMesh.getP()->getDataId();
            const bool has_group = (prim_group != nullptr);
            if (mySolidAngleTree.isClear() &&
                topology_data_id == myTopologyDataID &&
                primitive_list_data_id == myPrimitiveListDataID &&
                P_data_id == myPDataID &&
                approx_order == myApproxOrder &&
                axis0 == myAxis0 &&
                has_group == myHadGroup &&
                (!has_group || (
                    group_string == myGroupString &&
                    geoRefMesh.getUniqueId() == myUniqueId &&
                    geoRefMesh.getMetaCacheCount() == myMetaCacheCount)))
            {
                return;
            }
            mySolidAngleTree.clear();
            myPositions3D.clear();

            UT_AutoInterrupt boss("Constructing Solid Angle Tree");

            myTopologyDataID = topology_data_id;
            myPrimitiveListDataID = primitive_list_data_id;
            myPDataID = P_data_id;
            myApproxOrder = approx_order;
            myAxis0 = axis0;
            myHadGroup = has_group;
            myGroupString = group_string;
            myUniqueId = geoRefMesh.getUniqueId();
            myMetaCacheCount = geoRefMesh.getMetaCacheCount();

            GA_Size nprimitives = prim_group ? prim_group->entries() : geoRefMesh.getNumPrimitives();
            // NOTE: myTrianglePoints is actually segment points
            myTrianglePoints.setSize(0);
            myTrianglePoints.setCapacity(2 * nprimitives);
            myPositions2D.setSize(0);
            myPositions2D.setCapacity(0);
            UT_Array<int> ptmap;
            if (!prim_group)
            {
                // Copy all point positions
                myPositions2D.setSizeNoInit(geoRefMesh.getNumPoints());
                int ptnum = 0;
                geoRefMesh.forEachPoint([&geoRefMesh, this, &ptnum, axis0, axis1](const GA_Offset ptoff)
                {
                    UT_Vector3 pos = geoRefMesh.getPos3(ptoff);
                    myPositions2D[ptnum] = UT_Vector2(pos[axis0], pos[axis1]);
                    ++ptnum;
                });
            }
            else
            {
                // Find the used points
                GA_PointGroup mesh_ptgroup(geoRefMesh);
                mesh_ptgroup.combine(prim_group);
                myPositions2D.setSizeNoInit(mesh_ptgroup.entries());
                ptmap.setSizeNoInit(geoRefMesh.getNumPointOffsets());
                int ptnum = 0;
                geoRefMesh.forEachPoint(&mesh_ptgroup, [&geoRefMesh, this, &ptmap, &ptnum, axis0, axis1](const GA_Offset ptoff)
                {
                    ptmap[ptoff] = ptnum;
                    UT_Vector3 pos = geoRefMesh.getPos3(ptoff);
                    myPositions2D[ptnum] = UT_Vector2(pos[axis0], pos[axis1]);
                    ++ptnum;
                });
            }

            GA_Offset start;
            GA_Offset end;
            for (GA_Iterator it(geoRefMesh.getPrimitiveRange(prim_group)); it.blockAdvance(start, end); )
            {
                for (GA_Offset primoff = start; primoff < end; ++primoff)
                {
                    sopAccumulateSegments(&geoRefMesh, primoff, ptmap, myTrianglePoints);
                }
            }

            mySubtendedAngleTree.init(myTrianglePoints.size() / 2, myTrianglePoints.array(), myPositions2D.size(), myPositions2D.array(), approx_order);
        }

        void clear()
        {
            mySolidAngleTree.clear();
            mySubtendedAngleTree.clear();
            myTrianglePoints.setCapacity(0);
            myPositions2D.setCapacity(0);
            myPositions3D.setCapacity(0);
            myTopologyDataID = GA_INVALID_DATAID;
            myPrimitiveListDataID = GA_INVALID_DATAID;
            myPDataID = GA_INVALID_DATAID;
            myApproxOrder = -1;
            myHadGroup = false;
            myGroupString.clear();
            myUniqueId = -1;
            myMetaCacheCount = -1;
        }

        UT_SolidAngle<float, float> mySolidAngleTree;
        UT_SubtendedAngle<float, float> mySubtendedAngleTree;
        UT_Array<int> myTrianglePoints;
        UT_Array<UT_Vector2> myPositions2D;
        UT_Array<UT_Vector3> myPositions3D;
        GA_DataId myTopologyDataID;
        GA_DataId myPrimitiveListDataID;
        GA_DataId myPDataID;
        int myApproxOrder;
        int myAxis0;
        bool myHadGroup;
        UT_StringHolder myGroupString;
        exint myUniqueId;
        exint myMetaCacheCount;
    };











    static void
        sopSumContributions3D(
            double* sum,
            const UT_Vector3D& query_point,
            const GA_Detail* const geoRefMesh,
            const GA_OffsetList& primoffs,
            const exint start, const exint end)
    {
        if (end - start > 1024)
        {
            exint mid = (start + end) >> 1;
            double sum0;
            double sum1;
            UTparallelInvoke(true, [&] {
                sopSumContributions3D(&sum0, query_point, geoRefMesh, primoffs, start, mid);
            }, [&] {
                sopSumContributions3D(&sum1, query_point, geoRefMesh, primoffs, mid, end);
            });
            *sum = sum0 + sum1;
            return;
        }

        double local_sum = 0;
        for (exint arrayi = start; arrayi < end; ++arrayi)
        {
            GA_Offset primoff = primoffs(arrayi);
            int primtype = geoRefMesh->getPrimitiveTypeId(primoff);
            if (primtype == GA_PRIMPOLY)
            {
                const GA_OffsetListRef vertices = geoRefMesh->getPrimitiveVertexList(primoff);
                const GA_Size n = vertices.size();
                const bool closed = vertices.getExtraFlag();
                if (n < 3 || !closed)
                    continue;

                if (n == 3)
                {
                    const GA_Offset pt0 = geoRefMesh->vertexPoint(vertices(0));
                    const GA_Offset pt1 = geoRefMesh->vertexPoint(vertices(1));
                    const GA_Offset pt2 = geoRefMesh->vertexPoint(vertices(2));
                    const UT_Vector3D p0 = geoRefMesh->getPos3(pt0);
                    const UT_Vector3D p1 = geoRefMesh->getPos3(pt1);
                    const UT_Vector3D p2 = geoRefMesh->getPos3(pt2);
                    const double poly_sum = UTsignedSolidAngleTri(p0, p1, p2, query_point);
                    local_sum += poly_sum;
                    continue;
                }
                if (n == 4)
                {
                    // Special case for quads, to divide the correct diagonal
                    // in the case where the point is inside the convex hull.
                    const GA_Offset pt0 = geoRefMesh->vertexPoint(vertices(0));
                    const GA_Offset pt1 = geoRefMesh->vertexPoint(vertices(1));
                    const GA_Offset pt2 = geoRefMesh->vertexPoint(vertices(2));
                    const GA_Offset pt3 = geoRefMesh->vertexPoint(vertices(3));
                    const UT_Vector3D p0 = geoRefMesh->getPos3(pt0);
                    const UT_Vector3D p1 = geoRefMesh->getPos3(pt1);
                    const UT_Vector3D p2 = geoRefMesh->getPos3(pt2);
                    const UT_Vector3D p3 = geoRefMesh->getPos3(pt3);
#if QUADS_AS_BILINEAR_PATCHES
                    const double poly_sum = UTsignedSolidAngleQuad(p0, p1, p2, p3, query_point);
#else
                    const double poly_sum =
                        UTsignedSolidAngleTri(p0, p1, p2, query_point) +
                        UTsignedSolidAngleTri(p0, p2, p3, query_point);
#endif
                    local_sum += poly_sum;
                    continue;
                }
                // A triangle fan suffices, even if the polygon is non-convex,
                // because the contributions in the opposite direction will
                // partly cancel out the ones in the other direction,
                // in just the right amount.
                double poly_sum = 0;
                const UT_Vector3D p0 = geoRefMesh->getPos3(geoRefMesh->vertexPoint(vertices(0)));
                UT_Vector3D prev = geoRefMesh->getPos3(geoRefMesh->vertexPoint(vertices(1)));
                for (GA_Size i = 2; i < n; ++i)
                {
                    const UT_Vector3D next = geoRefMesh->getPos3(geoRefMesh->vertexPoint(vertices(i)));
                    poly_sum += UTsignedSolidAngleTri(p0, prev, next, query_point);
                    prev = next;
                }
                local_sum += poly_sum;
            }
            else if (primtype == GA_PRIMTETRAHEDRON)
            {
                const GA_OffsetListRef vertices = geoRefMesh->getPrimitiveVertexList(primoff);
                const GEO_PrimTetrahedron tet(SYSconst_cast(geoRefMesh), primoff, vertices);

                double tet_sum = 0;
                for (int i = 0; i < 4; ++i)
                {
                    // Ignore shared tet faces.  They would contribute exactly opposite amounts.
                    if (tet.isFaceShared(i))
                        continue;

                    const int* face_indices = GEO_PrimTetrahedron::fastFaceIndices(i);
                    const GA_Offset pt0 = geoRefMesh->vertexPoint(vertices(face_indices[0]));
                    const GA_Offset pt1 = geoRefMesh->vertexPoint(vertices(face_indices[1]));
                    const GA_Offset pt2 = geoRefMesh->vertexPoint(vertices(face_indices[2]));
                    const UT_Vector3D a = geoRefMesh->getPos3(pt0);
                    const UT_Vector3D b = geoRefMesh->getPos3(pt1);
                    const UT_Vector3D c = geoRefMesh->getPos3(pt2);

                    tet_sum += UTsignedSolidAngleTri(a, b, c, query_point);
                }
                local_sum += tet_sum;
            }
            else if (primtype == GA_PRIMPOLYSOUP)
            {
                double soup_sum = 0;
                const GEO_PrimPolySoup* soup = UTverify_cast<const GEO_PrimPolySoup*>(geoRefMesh->getPrimitive(primoff));
                for (GEO_PrimPolySoup::PolygonIterator poly(*soup); !poly.atEnd(); ++poly)
                {
                    GA_Size n = poly.nvertices();
                    if (n < 3)
                        continue;

                    if (n == 3)
                    {
                        const UT_Vector3D p0 = poly.getPos3(0);
                        const UT_Vector3D p1 = poly.getPos3(1);
                        const UT_Vector3D p2 = poly.getPos3(2);
                        const double poly_sum = UTsignedSolidAngleTri(p0, p1, p2, query_point);
                        local_sum += poly_sum;
                        continue;
                    }
                    if (n == 4)
                    {
                        // Special case for quads, to divide the correct diagonal
                        // in the case where the point is inside the convex hull.
                        const UT_Vector3D p0 = poly.getPos3(0);
                        const UT_Vector3D p1 = poly.getPos3(1);
                        const UT_Vector3D p2 = poly.getPos3(2);
                        const UT_Vector3D p3 = poly.getPos3(3);
#if QUADS_AS_BILINEAR_PATCHES
                        const double poly_sum = UTsignedSolidAngleQuad(p0, p1, p2, p3, query_point);
#else
                        const double poly_sum =
                            UTsignedSolidAngleTri(p0, p1, p2, query_point) +
                            UTsignedSolidAngleTri(p0, p2, p3, query_point);
#endif
                        local_sum += poly_sum;
                        continue;
                    }
                    // A triangle fan suffices, even if the polygon is non-convex,
                    // because the contributions in the opposite direction will
                    // partly cancel out the ones in the other direction,
                    // in just the right amount.
                    double poly_sum = 0;
                    const UT_Vector3D p0 = poly.getPos3(0);
                    UT_Vector3D prev = poly.getPos3(1);
                    for (GA_Size i = 2; i < n; ++i)
                    {
                        const UT_Vector3D next = poly.getPos3(i);
                        poly_sum += UTsignedSolidAngleTri(p0, prev, next, query_point);
                        prev = next;
                    }
                    soup_sum += poly_sum;
                }
                local_sum += soup_sum;
            }
            else if (primtype == GEO_PRIMMESH)
            {
                double mesh_sum = 0;
                const GEO_PrimMesh* mesh = UTverify_cast<const GEO_PrimMesh*>(geoRefMesh->getPrimitive(primoff));
                const int nquadrows = mesh->getNumRows() - !mesh->isWrappedV();
                const int nquadcols = mesh->getNumCols() - !mesh->isWrappedU();
                for (int row = 0; row < nquadrows; ++row)
                {
                    for (int col = 0; col < nquadcols; ++col)
                    {
                        GEO_Hull::Poly poly(*mesh, row, col);
                        const UT_Vector3D p0 = poly.getPos3(0);
                        const UT_Vector3D p1 = poly.getPos3(1);
                        const UT_Vector3D p2 = poly.getPos3(2);
                        const UT_Vector3D p3 = poly.getPos3(3);
#if QUADS_AS_BILINEAR_PATCHES
                        const double poly_sum = UTsignedSolidAngleQuad(p0, p1, p2, p3, query_point);
#else
                        const double poly_sum =
                            UTsignedSolidAngleTri(p0, p1, p2, query_point) +
                            UTsignedSolidAngleTri(p0, p2, p3, query_point);
#endif
                        mesh_sum += poly_sum;
                    }
                }
                local_sum += mesh_sum;
            }
            else if (primtype == GEO_PRIMNURBSURF || primtype == GEO_PRIMBEZSURF)
            {
                const GEO_TPSurf* surf = UTverify_cast<const GEO_TPSurf*>(geoRefMesh->getPrimitive(primoff));
                const int nquadrows = surf->getNumRows() - !surf->isWrappedV();
                const int nquadcols = surf->getNumCols() - !surf->isWrappedU();
                if (nquadcols <= 0 || nquadrows <= 0)
                    continue;

                // For slightly better accuracy, we use the greville points
                // instead of the hull points.
                double surf_sum = 0;
                UT_FloatArray ugrevilles;
                UT_FloatArray vgrevilles;
                surf->getUGrevilles(ugrevilles);
                if (surf->isWrappedU())
                    ugrevilles.append(0);
                surf->getVGrevilles(vgrevilles);
                if (surf->isWrappedV())
                    vgrevilles.append(0);
                UT_ASSERT(ugrevilles.size() >= nquadcols + 1);
                UT_ASSERT(vgrevilles.size() >= nquadrows + 1);
                for (int row = 0; row < nquadrows; ++row)
                {
                    for (int col = 0; col < nquadcols; ++col)
                    {
                        UT_Vector4 a4;
                        UT_Vector4 b4;
                        UT_Vector4 c4;
                        UT_Vector4 d4;
                        surf->evaluateInteriorPoint(a4, ugrevilles(col), vgrevilles(row));
                        surf->evaluateInteriorPoint(b4, ugrevilles(col + 1), vgrevilles(row));
                        surf->evaluateInteriorPoint(c4, ugrevilles(col + 1), vgrevilles(row + 1));
                        surf->evaluateInteriorPoint(d4, ugrevilles(col), vgrevilles(row + 1));
                        const UT_Vector3D p0(a4);
                        const UT_Vector3D p1(b4);
                        const UT_Vector3D p2(c4);
                        const UT_Vector3D p3(d4);
#if QUADS_AS_BILINEAR_PATCHES
                        const double poly_sum = UTsignedSolidAngleQuad(p0, p1, p2, p3, query_point);
#else
                        const double poly_sum =
                            UTsignedSolidAngleTri(p0, p1, p2, query_point) +
                            UTsignedSolidAngleTri(p0, p2, p3, query_point);
#endif
                        surf_sum += poly_sum;
                    }
                }
                local_sum += surf_sum;
            }
            else if (primtype == GEO_PRIMSPHERE)
            {
                const GEO_PrimSphere* sphere = UTverify_cast<const GEO_PrimSphere*>(geoRefMesh->getPrimitive(primoff));
                UT_Vector3D query_minus_centre = query_point - UT_Vector3D(sphere->getPos3());
                UT_Matrix3D transform;
                sphere->getLocalTransform(transform);

                // If we're outside, no contribution.
                // If we're inside and the determinant is negative, +4pi
                // If we're inside and the determinant is positive, -4pi
                // The negation is for consistency with the winding order of polygons
                // that would be created if the sphere were converted to polygons.
                double determinant = transform.determinant();
                if (determinant == 0)
                    continue;

                int failed = transform.invert();
                if (failed)
                    continue;

                query_minus_centre.rowVecMult(transform);
                float length2 = query_minus_centre.length2();

                double sphere_sum = 0;
                if (length2 < 1)
                    sphere_sum = 4 * M_PI;
                else if (length2 == 1)
                    sphere_sum = 2 * M_PI;

                if (determinant > 0)
                    sphere_sum = -sphere_sum;

                local_sum += sphere_sum;
            }
        }
        *sum = local_sum;
    }

    static void
        sopSumContributions2D(
            double* sum,
            const UT_Vector2D& query_point,
            const GA_Detail* const geoRefMesh,
            const GA_OffsetList& primoffs,
            const exint start, const exint end,
            const int axis0, const int axis1)
    {
        if (end - start > 1024)
        {
            exint mid = (start + end) >> 1;
            double sum0;
            double sum1;
            UTparallelInvoke(true, [&] {
                sopSumContributions2D(&sum0, query_point, geoRefMesh, primoffs, start, mid, axis0, axis1);
            }, [&] {
                sopSumContributions2D(&sum1, query_point, geoRefMesh, primoffs, mid, end, axis0, axis1);
            });
            *sum = sum0 + sum1;
            return;
        }

        double local_sum = 0;
        for (exint arrayi = start; arrayi < end; ++arrayi)
        {
            GA_Offset primoff = primoffs(arrayi);
            int primtype = geoRefMesh->getPrimitiveTypeId(primoff);
            if (primtype == GA_PRIMPOLY)
            {
                const GA_OffsetListRef vertices = geoRefMesh->getPrimitiveVertexList(primoff);
                const GA_Size n = vertices.size();
                const bool closed = vertices.getExtraFlag();
                if (n < 2 + int(closed))
                    continue;

                double poly_sum = 0;
                UT_Vector3 pos = geoRefMesh->getPos3(geoRefMesh->vertexPoint(vertices(0)));
                UT_Vector2D prev(pos[axis0], pos[axis1]);
                const UT_Vector2D p0 = prev;
                for (GA_Size i = 1; i < n; ++i)
                {
                    pos = geoRefMesh->getPos3(geoRefMesh->vertexPoint(vertices(i)));
                    const UT_Vector2D next(pos[axis0], pos[axis1]);
                    poly_sum += UTsignedAngleSegment(prev, next, query_point);
                    prev = next;
                }
                if (closed)
                    poly_sum += UTsignedAngleSegment(prev, p0, query_point);

                local_sum += poly_sum;
            }
            else if (primtype == GA_PRIMTETRAHEDRON)
            {
                // NOTE: Tetrahedra never contribute to the 2D winding number, since
                //       every point is contained in 1 forward triangle and 1 backward triangle.
            }
            else if (primtype == GA_PRIMPOLYSOUP)
            {
                double soup_sum = 0;
                const GEO_PrimPolySoup* soup = UTverify_cast<const GEO_PrimPolySoup*>(geoRefMesh->getPrimitive(primoff));
                for (GEO_PrimPolySoup::PolygonIterator poly(*soup); !poly.atEnd(); ++poly)
                {
                    GA_Size n = poly.nvertices();
                    if (n < 3)
                        continue;

                    double poly_sum = 0;
                    UT_Vector3 pos = poly.getPos3(0);
                    UT_Vector2D prev(pos[axis0], pos[axis1]);
                    const UT_Vector2D p0 = prev;
                    for (GA_Size i = 1; i < n; ++i)
                    {
                        pos = poly.getPos3(i);
                        const UT_Vector2D next(pos[axis0], pos[axis1]);
                        poly_sum += UTsignedAngleSegment(prev, next, query_point);
                        prev = next;
                    }
                    poly_sum += UTsignedAngleSegment(prev, p0, query_point);
                    soup_sum += poly_sum;
                }
                local_sum += soup_sum;
            }
            else if (primtype == GEO_PRIMMESH)
            {
                double mesh_sum = 0;
                const GEO_PrimMesh* mesh = UTverify_cast<const GEO_PrimMesh*>(geoRefMesh->getPrimitive(primoff));
                const int nquadrows = mesh->getNumRows() - !mesh->isWrappedV();
                const int nquadcols = mesh->getNumCols() - !mesh->isWrappedU();
                for (int row = 0; row < nquadrows; ++row)
                {
                    for (int col = 0; col < nquadcols; ++col)
                    {
                        GEO_Hull::Poly poly(*mesh, row, col);
                        double poly_sum = 0;
                        UT_Vector3 pos = poly.getPos3(0);
                        UT_Vector2D prev(pos[axis0], pos[axis1]);
                        const UT_Vector2D p0 = prev;
                        for (GA_Size i = 1; i < 4; ++i)
                        {
                            pos = poly.getPos3(i);
                            const UT_Vector2D next(pos[axis0], pos[axis1]);
                            poly_sum += UTsignedAngleSegment(prev, next, query_point);
                            prev = next;
                        }
                        poly_sum += UTsignedAngleSegment(prev, p0, query_point);
                        mesh_sum += poly_sum;
                    }
                }
                local_sum += mesh_sum;
            }
            else if (primtype == GEO_PRIMNURBSURF || primtype == GEO_PRIMBEZSURF)
            {
                const GEO_TPSurf* surf = UTverify_cast<const GEO_TPSurf*>(geoRefMesh->getPrimitive(primoff));
                const int nquadrows = surf->getNumRows() - !surf->isWrappedV();
                const int nquadcols = surf->getNumCols() - !surf->isWrappedU();
                if (nquadcols <= 0 || nquadrows <= 0)
                    continue;

                // For slightly better accuracy, we use the greville points
                // instead of the hull points.
                double surf_sum = 0;
                UT_FloatArray ugrevilles;
                UT_FloatArray vgrevilles;
                surf->getUGrevilles(ugrevilles);
                if (surf->isWrappedU())
                    ugrevilles.append(0);
                surf->getVGrevilles(vgrevilles);
                if (surf->isWrappedV())
                    vgrevilles.append(0);
                UT_ASSERT(ugrevilles.size() >= nquadcols + 1);
                UT_ASSERT(vgrevilles.size() >= nquadrows + 1);
                for (int row = 0; row < nquadrows; ++row)
                {
                    for (int col = 0; col < nquadcols; ++col)
                    {
                        UT_Vector4 a4;
                        UT_Vector4 b4;
                        UT_Vector4 c4;
                        UT_Vector4 d4;
                        surf->evaluateInteriorPoint(a4, ugrevilles(col), vgrevilles(row));
                        surf->evaluateInteriorPoint(b4, ugrevilles(col + 1), vgrevilles(row));
                        surf->evaluateInteriorPoint(c4, ugrevilles(col + 1), vgrevilles(row + 1));
                        surf->evaluateInteriorPoint(d4, ugrevilles(col), vgrevilles(row + 1));
                        const UT_Vector2D p0(a4[axis0], a4[axis1]);
                        const UT_Vector2D p1(b4[axis0], b4[axis1]);
                        const UT_Vector2D p2(c4[axis0], c4[axis1]);
                        const UT_Vector2D p3(d4[axis0], d4[axis1]);
                        double poly_sum;
                        poly_sum = UTsignedAngleSegment(p0, p1, query_point);
                        poly_sum += UTsignedAngleSegment(p1, p2, query_point);
                        poly_sum += UTsignedAngleSegment(p2, p3, query_point);
                        poly_sum += UTsignedAngleSegment(p3, p0, query_point);
                        surf_sum += poly_sum;
                    }
                }
                local_sum += surf_sum;
            }
            else if (primtype == GEO_PRIMNURBCURVE || primtype == GEO_PRIMBEZCURVE)
            {
                const GEO_Curve* curve = UTverify_cast<const GEO_Curve*>(geoRefMesh->getPrimitive(primoff));
                const int nedges = curve->getVertexCount() - !curve->isClosed();
                if (nedges <= 0)
                    continue;

                // For slightly better accuracy, we use the greville points
                // instead of the hull points.
                double curve_sum = 0;
                UT_Array<GEO_Greville> grevilles;
                grevilles.setCapacity(nedges + 1);
                curve->buildGrevilles(grevilles);
                if (curve->isClosed())
                    grevilles.append(grevilles[0]);
                UT_ASSERT(grevilles.size() >= nedges + 1);
                UT_Vector2D prev(grevilles[0].getCVImage()[axis0], grevilles[0].getCVImage()[axis1]);
                for (int i = 0; i < nedges; ++i)
                {
                    const UT_Vector2D next(grevilles[i].getCVImage()[axis0], grevilles[i].getCVImage()[axis1]);
                    curve_sum += UTsignedAngleSegment(prev, next, query_point);
                }
                local_sum += curve_sum;
            }
        }
        *sum = local_sum;
    }

    static void
        sopAccumulateTriangles(
            const GA_Detail* const geoRefMesh,
            const GA_Offset primoff,
            const UT_Array<int>& ptmap,
            UT_Array<int>& triangle_points)
    {
        const bool has_ptmap = !ptmap.isEmpty();
        int primtype = geoRefMesh->getPrimitiveTypeId(primoff);
        if (primtype == GA_PRIMPOLY)
        {
            const GA_OffsetListRef vertices = geoRefMesh->getPrimitiveVertexList(primoff);
            const GA_Size n = vertices.size();
            const bool closed = vertices.getExtraFlag();
            if (n < 3 || !closed)
                return;

            // A triangle fan suffices, even if the polygon is non-convex,
            // because the contributions in the opposite direction will
            // partly cancel out the ones in the other direction,
            // in just the right amount.
            const GA_Offset p0 = geoRefMesh->vertexPoint(vertices(0));
            const int p0i = has_ptmap ? ptmap[p0] : int(geoRefMesh->pointIndex(p0));
            GA_Offset prev = geoRefMesh->vertexPoint(vertices(1));
            int previ = has_ptmap ? ptmap[prev] : int(geoRefMesh->pointIndex(prev));
            for (GA_Size i = 2; i < n; ++i)
            {
                const GA_Offset next = geoRefMesh->vertexPoint(vertices(i));
                const int nexti = has_ptmap ? ptmap[next] : int(geoRefMesh->pointIndex(next));
                triangle_points.append(p0i);
                triangle_points.append(previ);
                triangle_points.append(nexti);
                previ = nexti;
            }
        }
        else if (primtype == GA_PRIMTETRAHEDRON)
        {
            const GA_OffsetListRef vertices = geoRefMesh->getPrimitiveVertexList(primoff);
            const GEO_PrimTetrahedron tet(SYSconst_cast(geoRefMesh), primoff, vertices);

            for (int i = 0; i < 4; ++i)
            {
                // Ignore shared tet faces.  They would contribute exactly opposite amounts.
                if (tet.isFaceShared(i))
                    continue;

                const int* face_indices = GEO_PrimTetrahedron::fastFaceIndices(i);
                const GA_Offset a = geoRefMesh->vertexPoint(vertices(face_indices[0]));
                const GA_Offset b = geoRefMesh->vertexPoint(vertices(face_indices[1]));
                const GA_Offset c = geoRefMesh->vertexPoint(vertices(face_indices[2]));
                const int ai = has_ptmap ? ptmap[a] : int(geoRefMesh->pointIndex(a));
                const int bi = has_ptmap ? ptmap[b] : int(geoRefMesh->pointIndex(b));
                const int ci = has_ptmap ? ptmap[c] : int(geoRefMesh->pointIndex(c));
                triangle_points.append(ai);
                triangle_points.append(bi);
                triangle_points.append(ci);
            }
        }
        else if (primtype == GA_PRIMPOLYSOUP)
        {
            const GEO_PrimPolySoup* soup = UTverify_cast<const GEO_PrimPolySoup*>(geoRefMesh->getPrimitive(primoff));
            for (GEO_PrimPolySoup::PolygonIterator poly(*soup); !poly.atEnd(); ++poly)
            {
                GA_Size n = poly.nvertices();
                if (n < 3)
                    continue;

                // A triangle fan suffices, even if the polygon is non-convex,
                // because the contributions in the opposite direction will
                // partly cancel out the ones in the other direction,
                // in just the right amount.
                const GA_Offset p0 = poly.getPointOffset(0);
                const int p0i = has_ptmap ? ptmap[p0] : int(geoRefMesh->pointIndex(p0));
                GA_Offset prev = poly.getPointOffset(1);
                int previ = has_ptmap ? ptmap[prev] : int(geoRefMesh->pointIndex(prev));
                for (GA_Size i = 2; i < n; ++i)
                {
                    const GA_Offset next = poly.getPointOffset(i);
                    const int nexti = has_ptmap ? ptmap[next] : int(geoRefMesh->pointIndex(next));
                    triangle_points.append(p0i);
                    triangle_points.append(previ);
                    triangle_points.append(nexti);
                    previ = nexti;
                }
            }
        }
        else if (primtype == GEO_PRIMMESH || primtype == GEO_PRIMNURBSURF || primtype == GEO_PRIMBEZSURF)
        {
            // In this mode, we're only using points in the detail, so no grevilles.
            const GEO_Hull* mesh = UTverify_cast<const GEO_Hull*>(geoRefMesh->getPrimitive(primoff));
            const int nquadrows = mesh->getNumRows() - !mesh->isWrappedV();
            const int nquadcols = mesh->getNumCols() - !mesh->isWrappedU();
            for (int row = 0; row < nquadrows; ++row)
            {
                for (int col = 0; col < nquadcols; ++col)
                {
                    GEO_Hull::Poly poly(*mesh, row, col);
                    const GA_Offset a = poly.getPointOffset(0);
                    const GA_Offset b = poly.getPointOffset(1);
                    const GA_Offset c = poly.getPointOffset(2);
                    const GA_Offset d = poly.getPointOffset(3);
                    const int ai = has_ptmap ? ptmap[a] : int(geoRefMesh->pointIndex(a));
                    const int bi = has_ptmap ? ptmap[b] : int(geoRefMesh->pointIndex(b));
                    const int ci = has_ptmap ? ptmap[c] : int(geoRefMesh->pointIndex(c));
                    const int di = has_ptmap ? ptmap[d] : int(geoRefMesh->pointIndex(d));
                    triangle_points.append(ai);
                    triangle_points.append(bi);
                    triangle_points.append(ci);
                    triangle_points.append(ai);
                    triangle_points.append(ci);
                    triangle_points.append(di);
                }
            }
        }
    }

    static void
        sopAccumulateSegments(
            const GA_Detail* const geoRefMesh,
            const GA_Offset primoff,
            const UT_Array<int>& ptmap,
            UT_Array<int>& segment_points)
    {
        const bool has_ptmap = !ptmap.isEmpty();
        int primtype = geoRefMesh->getPrimitiveTypeId(primoff);
        if (primtype == GA_PRIMPOLY || primtype == GA_PRIMBEZCURVE || primtype == GA_PRIMNURBCURVE)
        {
            const GA_OffsetListRef vertices = geoRefMesh->getPrimitiveVertexList(primoff);
            const GA_Size n = vertices.size();
            const bool closed = vertices.getExtraFlag();
            if (n < 2 + int(closed))
                return;

            GA_Offset prev = geoRefMesh->vertexPoint(vertices(0));
            int previ = has_ptmap ? ptmap[prev] : int(geoRefMesh->pointIndex(prev));
            const int pt0i = previ;
            for (GA_Size i = 1; i < n; ++i)
            {
                const GA_Offset next = geoRefMesh->vertexPoint(vertices(i));
                const int nexti = has_ptmap ? ptmap[next] : int(geoRefMesh->pointIndex(next));
                segment_points.append(previ);
                segment_points.append(nexti);
                previ = nexti;
            }
            if (closed)
            {
                segment_points.append(previ);
                segment_points.append(pt0i);
            }
        }
        else if (primtype == GA_PRIMTETRAHEDRON)
        {
            // NOTE: Tetrahedra never contribute to the 2D winding number, since
            //       every point is contained in 1 forward triangle and 1 backward triangle.
        }
        else if (primtype == GA_PRIMPOLYSOUP)
        {
            const GEO_PrimPolySoup* soup = UTverify_cast<const GEO_PrimPolySoup*>(geoRefMesh->getPrimitive(primoff));
            for (GEO_PrimPolySoup::PolygonIterator poly(*soup); !poly.atEnd(); ++poly)
            {
                GA_Size n = poly.nvertices();
                if (n < 3)
                    continue;

                GA_Offset prev = poly.getPointOffset(0);
                int previ = has_ptmap ? ptmap[prev] : int(geoRefMesh->pointIndex(prev));
                const int pt0i = previ;
                for (GA_Size i = 1; i < n; ++i)
                {
                    const GA_Offset next = poly.getPointOffset(i);
                    const int nexti = has_ptmap ? ptmap[next] : int(geoRefMesh->pointIndex(next));
                    segment_points.append(previ);
                    segment_points.append(nexti);
                    previ = nexti;
                }
                segment_points.append(previ);
                segment_points.append(pt0i);
            }
        }
        else if (primtype == GEO_PRIMMESH || primtype == GEO_PRIMNURBSURF || primtype == GEO_PRIMBEZSURF)
        {
            // In this mode, we're only using points in the detail, so no grevilles.
            const GEO_Hull* mesh = UTverify_cast<const GEO_Hull*>(geoRefMesh->getPrimitive(primoff));
            const int nquadrows = mesh->getNumRows() - !mesh->isWrappedV();
            const int nquadcols = mesh->getNumCols() - !mesh->isWrappedU();
            for (int row = 0; row < nquadrows; ++row)
            {
                for (int col = 0; col < nquadcols; ++col)
                {
                    GEO_Hull::Poly poly(*mesh, row, col);
                    const GA_Offset a = poly.getPointOffset(0);
                    const GA_Offset b = poly.getPointOffset(1);
                    const GA_Offset c = poly.getPointOffset(2);
                    const GA_Offset d = poly.getPointOffset(3);
                    const int ai = has_ptmap ? ptmap[a] : int(geoRefMesh->pointIndex(a));
                    const int bi = has_ptmap ? ptmap[b] : int(geoRefMesh->pointIndex(b));
                    const int ci = has_ptmap ? ptmap[c] : int(geoRefMesh->pointIndex(c));
                    const int di = has_ptmap ? ptmap[d] : int(geoRefMesh->pointIndex(d));
                    segment_points.append(ai);
                    segment_points.append(bi);
                    segment_points.append(bi);
                    segment_points.append(ci);
                    segment_points.append(ci);
                    segment_points.append(di);
                    segment_points.append(di);
                    segment_points.append(ai);
                }
            }
        }
    }

    static void
        sop3DFullAccuracy(
            const GA_Detail* const geoPoint,
            const GA_SplittableRange& geoPointRange,
            const GA_Detail* const geoRefMesh,
            const GA_PrimitiveGroup* const geoRefMeshGroup,
            const GA_RWHandleF& wn_h,
            const bool asSolidAngle,
            const bool negate)
    {
        UT_AutoInterrupt boss("Computing Winding Numbers");

        // See comment below for why we create this GA_Offsetlist of mesh primitive offsets.
        GA_OffsetList primoffs;
        if (!geoRefMeshGroup && geoRefMesh->getPrimitiveMap().isTrivialMap())
        {
            primoffs.setTrivial(GA_Offset(0), geoRefMesh->getNumPrimitives());
        }
        else
        {
            GA_Offset start;
            GA_Offset end;
            for (GA_Iterator it(geoRefMesh->getPrimitiveRange(geoRefMeshGroup)); it.fullBlockAdvance(start, end); )
            {
                primoffs.setTrivialRange(primoffs.size(), start, end - start);
            }
        }

        UTparallelFor(geoPointRange, [geoPoint, geoRefMesh, wn_h, &primoffs, asSolidAngle, negate, &boss](const GA_SplittableRange& r)
        {
            GA_Offset start;
            GA_Offset end;
            for (GA_Iterator it(r); it.blockAdvance(start, end); )
            {
                for (GA_Offset ptoff = start; ptoff != end; ++ptoff)
                {
                    if (boss.wasInterrupted())
                        return;

                    const UT_Vector3 query_point = geoPoint->getPos3(ptoff);

                    // NOTE: We can't use UTparallelReduce, because that would have
                    //       nondeterministic roundoff error due to floating-point
                    //       addition being non-associative.  We can't just use
                    //       UTparallelInvoke with GA_SplittableRange either, because
                    //       the roundoff error would change with defragmentation,
                    //       e.g. from locking the mesh input and reloading the HIP file.
                    //       Instead, we always split in the middle of the list of offsets.
                    double sum;
                    sopSumContributions3D(&sum, query_point, geoRefMesh, primoffs, 0, primoffs.size());

                    if (!asSolidAngle)
                        sum *= (0.25 * M_1_PI); // Divide by 4pi (solid angle of full sphere)
                    if (negate)
                        sum = -sum;

                    wn_h.set(ptoff, sum);
                }
            }
        });
    }

    static void
        sop2DFullAccuracy(
            const GA_Detail* const geoPoint,
            const GA_SplittableRange& geoPointRange,
            const GA_Detail* const geoRefMesh,
            const GA_PrimitiveGroup* const geoRefMeshGroup,
            const GA_RWHandleF& wn_h,
            const bool asSolidAngle,
            const bool negate,
            const int axis0,
            const int axis1)
    {
        UT_AutoInterrupt boss("Computing Winding Numbers");

        // See comment below for why we create this GA_Offsetlist of mesh primitive offsets.
        GA_OffsetList primoffs;
        if (!geoRefMeshGroup && geoRefMesh->getPrimitiveMap().isTrivialMap())
        {
            primoffs.setTrivial(GA_Offset(0), geoRefMesh->getNumPrimitives());
        }
        else
        {
            GA_Offset start;
            GA_Offset end;
            for (GA_Iterator it(geoRefMesh->getPrimitiveRange(geoRefMeshGroup)); it.fullBlockAdvance(start, end); )
            {
                primoffs.setTrivialRange(primoffs.size(), start, end - start);
            }
        }

        UTparallelFor(geoPointRange, [geoPoint, geoRefMesh, wn_h, &primoffs, asSolidAngle, negate, &boss, axis0, axis1](const GA_SplittableRange& r)
        {
            GA_Offset start;
            GA_Offset end;
            for (GA_Iterator it(r); it.blockAdvance(start, end); )
            {
                for (GA_Offset ptoff = start; ptoff != end; ++ptoff)
                {
                    if (boss.wasInterrupted())
                        return;

                    const UT_Vector3D query_point_3d = geoPoint->getPos3(ptoff);
                    const UT_Vector2D query_point(query_point_3d[axis0], query_point_3d[axis1]);

                    // NOTE: We can't use UTparallelReduce, because that would have
                    //       nondeterministic roundoff error due to floating-point
                    //       addition being non-associative.  We can't just use
                    //       UTparallelInvoke with GA_SplittableRange either, because
                    //       the roundoff error would change with defragmentation,
                    //       e.g. from locking the mesh input and reloading the HIP file.
                    //       Instead, we always split in the middle of the list of offsets.
                    double sum;
                    sopSumContributions2D(&sum, query_point, geoRefMesh, primoffs, 0, primoffs.size(), axis0, axis1);

                    if (!asSolidAngle)
                        sum *= (0.5 * M_1_PI); // Divide by 2pi (angle of full circle)
                    if (negate)
                        sum = -sum;

                    wn_h.set(ptoff, sum);
                }
            }
        });
    }

    static void
        sop3DApproximate(
            const GA_Detail* const geoPoint,
            const GA_SplittableRange& geoPointRange,
            const UT_SolidAngle<float, float>& solid_angle_tree,
            const double accuracyScale,
            const GA_RWHandleF& wn_h,
            const bool asSolidAngle,
            const bool negate)
    {
        UT_AutoInterrupt boss("Computing Winding Numbers");

        UTparallelFor(geoPointRange, [geoPoint, &wn_h, &solid_angle_tree, asSolidAngle, negate, accuracyScale, &boss](const GA_SplittableRange& r)
        {
            GA_Offset start;
            GA_Offset end;
            for (GA_Iterator it(r); it.blockAdvance(start, end); )
            {
                if (boss.wasInterrupted())
                    return;

                for (GA_Offset ptoff = start; ptoff != end; ++ptoff)
                {
                    const UT_Vector3 query_point = geoPoint->getPos3(ptoff);

                    double sum = solid_angle_tree.computeSolidAngle(query_point, accuracyScale);

                    if (!asSolidAngle)
                        sum *= (0.25 * M_1_PI); // Divide by 4pi (solid angle of full sphere)
                    if (negate)
                        sum = -sum;

                    wn_h.set(ptoff, sum);
                }
            }
        }, 10); // Large subscribe ratio, because expensive points are often clustered
    }

    static void
        sop2DApproximate(
            const GA_Detail* const geoPoint,
            const GA_SplittableRange& geoPointRange,
            const UT_SubtendedAngle<float, float>& subtended_angle_tree,
            const double accuracyScale,
            const GA_RWHandleF& wn_h,
            const bool as_angle,
            const bool negate,
            const int axis0,
            const int axis1)
    {
        UT_AutoInterrupt boss("Computing Winding Numbers");

        UTparallelFor(geoPointRange, [geoPoint, &wn_h, &subtended_angle_tree, as_angle, negate, accuracyScale, &boss, axis0, axis1](const GA_SplittableRange& r)
        {
            GA_Offset start;
            GA_Offset end;
            for (GA_Iterator it(r); it.blockAdvance(start, end); )
            {
                if (boss.wasInterrupted())
                    return;

                for (GA_Offset ptoff = start; ptoff != end; ++ptoff)
                {
                    const UT_Vector3 query_point_3d = geoPoint->getPos3(ptoff);
                    const UT_Vector2D query_point(query_point_3d[axis0], query_point_3d[axis1]);

                    double sum = subtended_angle_tree.computeAngle(query_point, accuracyScale);

                    if (!as_angle)
                        sum *= (0.5 * M_1_PI); // Divide by 2pi (angle of full circle)
                    if (negate)
                        sum = -sum;

                    wn_h.set(ptoff, sum);
                }
            }
        }, 10); // Large subscribe ratio, because expensive points are often clustered
    }



    
static void
    computeWindingNumber(
        GA_Detail* const geoPoint,
        const GA_Detail* const geoRefMesh,
        const GA_RWHandleT<fpreal32>& wn_h,
        GA_WindingNumber_Cache* sopcache = nullptr,
        const GA_PointGroup* const geoPointGroup = nullptr,
        const GA_PrimitiveGroup* const geoRefMeshGroup = nullptr,
        const GA_WindingNumberType wnType = GA_WNType_XYZ,
        const bool fullAccuracy = false,
        const double accuracyScale = 2.0,
        const bool asSolidAngle = false,
        const bool negate = false
    )
{
    UT_ASSERT_P(geoPoint);
    UT_ASSERT_P(geoRefMesh);
    UT_ASSERT_P(wn_h.isValid());

    const GA_SplittableRange geoPointRange(geoPoint->getPointRange(geoPointGroup));

    if (wnType == GA_WNType_XYZ)
    {
        // NOTE: The negation is because Houdini's normals are
        //       left-handed with respect to the winding order,
        //       whereas most use the right-handed convension.
        const bool finalNegate = !negate;
        if (fullAccuracy)
        {
            if(sopcache)
                sopcache->clear();

            sop3DFullAccuracy(
                geoPoint, geoPointRange,
                geoRefMesh, geoRefMeshGroup,
                wn_h,
                asSolidAngle, finalNegate);
        }
        else
        {
            const bool noCache = !sopcache;
            if(noCache)
                sopcache = new GA_WindingNumber_Cache();

            const UT_StringHolder& geoRefMeshGroupNameEmpty = "";
            const UT_StringHolder& geoRefMeshGroupName = !geoRefMeshGroup || geoRefMeshGroup->isDetached() ? UT_StringHolder() : geoRefMeshGroup->getName();
            sopcache->update3D(*geoRefMesh, geoRefMeshGroup, geoRefMeshGroupName, 2);
            const UT_SolidAngle<float, float>& solid_angle_tree = sopcache->mySolidAngleTree;

            sop3DApproximate(
                geoPoint, geoPointRange,
                solid_angle_tree, accuracyScale,
                wn_h,
                asSolidAngle, finalNegate);

            if (noCache)
                delete sopcache;
        }
    }
    else
    {
        const int axis0 = int(wnType) - 1;
        const int axis1 = (axis0 == 2) ? 0 : (axis0 + 1);

        if (fullAccuracy)
        {
            if (sopcache)
                sopcache->clear();

            sop2DFullAccuracy(
                geoPoint, geoPointRange,
                geoRefMesh, geoRefMeshGroup,
                wn_h,
                asSolidAngle, negate,
                axis0, axis1);
        }
        else
        {
            const bool noCache = !sopcache;
            if (noCache)
                sopcache = new GA_WindingNumber_Cache();

            const UT_StringHolder& geoRefMeshGroupName = !geoRefMeshGroup || geoRefMeshGroup->isDetached() ? UT_StringHolder() : geoRefMeshGroup->getName();
            sopcache->update2D(*geoRefMesh, geoRefMeshGroup, geoRefMeshGroupName, 2, axis0, axis1);
            const UT_SubtendedAngle<float, float>& subtended_angle_tree = sopcache->mySubtendedAngleTree;

            sop2DApproximate(
                geoPoint, geoPointRange,
                subtended_angle_tree, accuracyScale,
                wn_h,
                asSolidAngle, negate,
                axis0, axis1);

            if (noCache)
                delete sopcache;
        }
    }

}







static GA_Attribute*
addAttribWindingNumber(
    const SOP_NodeVerb::CookParms& cookparms,
    GA_Detail* const geoPoint,
    const GA_Detail* const geoRefMesh,
    const UT_StringHolder& geoPoint_groupName,
    const UT_StringHolder& geoRefMesh_groupName,
    GA_WindingNumber_Cache* const sopcache = nullptr,
    const GA_Storage storage = GA_STORE_INVALID,
    const UT_StringHolder& wnAttribName = "__topo_windingNumber",
    const GA_WindingNumberType wnType = GA_WNType_XYZ,
    const bool fullAccuracy = false,
    const double accuracyScale = 2.0,
    const bool asSolidAngle = false,
    const bool negate = false
)
{
    const GA_Storage finalStorageF = storage == GA_STORE_INVALID ? GA_FeE_Type::getPreferredStorageF(geoPoint) : storage;

    GA_Attribute* wnAttribPtr = geoPoint->findAttribute(GA_ATTRIB_POINT, wnAttribName);
    if (!wnAttribPtr || wnAttribPtr->getStorageClass() != GA_STORECLASS_REAL)
        wnAttribPtr = geoPoint->createTupleAttribute(GA_ATTRIB_POINT, wnAttribName, finalStorageF, 1);

    if (!wnAttribPtr)
    {
        cookparms.sopAddError(SOP_ATTRIBUTE_INVALID, wnAttribName);
        return nullptr;
    }
    //const GA_RWHandleF wn_h(wnAttribPtr);

    const GA_Size npoints = geoPoint->getNumPoints();
    if (npoints == 0)
        return wnAttribPtr;


    GOP_Manager gop;
    bool success;
    const GA_PrimitiveGroup* geoRefMeshGroup = geoRefMesh_groupName.isstring() ? gop.parsePrimitiveDetached(geoRefMesh_groupName.c_str(), static_cast<const GEO_Detail*>(geoRefMesh), true, success) : nullptr;

    const GA_PointGroup* geoPointGroup = geoPoint_groupName.isstring() ? gop.parsePointDetached(geoPoint_groupName.c_str(), static_cast<GEO_Detail*>(geoPoint), true, success) : nullptr;

    computeWindingNumber(
        geoPoint, geoRefMesh, wnAttribPtr,
        sopcache, geoPointGroup, geoRefMeshGroup,
        wnType, fullAccuracy, accuracyScale, asSolidAngle, negate);

    return wnAttribPtr;
}








} // End of namespace GA_FeE_WindingNumber

#endif
