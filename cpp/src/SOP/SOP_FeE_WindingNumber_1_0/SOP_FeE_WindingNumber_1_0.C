/*
 * Copyright (c) 2022
 *      Side Effects Software Inc.  All rights reserved.
 *
 * Redistribution and use of Houdini Development Kit samples in source and
 * binary forms, with or without modification, are permitted provided that the
 * following conditions are met:
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. The name of Side Effects Software may not be used to endorse or
 *    promote products derived from this software without specific prior
 *    written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY SIDE EFFECTS SOFTWARE `AS IS' AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN
 * NO EVENT SHALL SIDE EFFECTS SOFTWARE BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *----------------------------------------------------------------------------
 * This SOP computes the winding number induced by a mesh at the specified points.
 */

#include "SOP_FeE_WindingNumber_1_0.h"

// This is an automatically generated header file based on theDsFile, below,
// to provide SOP_FeE_WindingNumber_1_0Parms, an easy way to access parameter values from
// SOP_FeE_WindingNumber_1_0Verb::cook with the correct type.
#include "SOP_FeE_WindingNumber_1_0.proto.h"

#include "UT/UT_SolidAngle.h"

#include <SOP/SOP_NodeVerb.h>
#include <GU/GU_Detail.h>
#include <GEO/GEO_Curve.h>
#include <GEO/GEO_PrimMesh.h>
#include <GEO/GEO_PrimPolySoup.h>
#include <GEO/GEO_PrimSphere.h>
#include <GEO/GEO_PrimTetrahedron.h>
#include <GEO/GEO_TPSurf.h>
#include <GA/GA_Handle.h>
#include <GA/GA_SplittableRange.h>
#include <GA/GA_Types.h>
#include <OP/OP_Operator.h>
#include <OP/OP_OperatorTable.h>
#include <PRM/PRM_TemplateBuilder.h>
#include <UT/UT_DSOVersion.h>
#include <UT/UT_Interrupt.h>
#include <UT/UT_ParallelUtil.h>
#include <UT/UT_StringHolder.h>
#include <UT/UT_UniquePtr.h>
#include <SYS/SYS_Math.h>

// UT_SolidAngle doesn't currently have a special case to handle quads correctly
// as bilinear patches; it always treats them as two triangles, which can result
// in a difference of about 1 between points that are just inside or just
// outside.  If you want the accurate version to treat them as triangles too,
// (e.g. for testing the approximation error of UT_SolidAngle), set this to 0.
#define QUADS_AS_BILINEAR_PATCHES 1

using namespace SOP_FeE_WindingNumber_1_0;

//
// Help is stored in a "wiki" style text file.  This text file should be copied
// to $HOUDINI_PATH/help/nodes/sop/hdk_windingnumber.txt
//
// See the sample_install.sh file for an example.
//

// Forward declarations of file-scope functions used before they're defined

static void
sopAccumulateTriangles(
    const GA_Detail *const mesh_geo,
    const GA_Offset primoff,
    const UT_Array<int> &ptmap,
    UT_Array<int> &triangle_points);

static void
sopAccumulateSegments(
    const GA_Detail *const mesh_geo,
    const GA_Offset primoff,
    const UT_Array<int> &ptmap,
    UT_Array<int> &segment_points);

// This gives easy access to the enum types for this operator's parameter
using namespace SOP_FeE_WindingNumber_1_0Enums;

//******************************************************************************
//*                 Setup                                                      *
//******************************************************************************

typedef int64                   GA_DataId;
#define GA_INVALID_DATAID       GA_DataId(-1)

/// This class is for caching data between cooks, e.g. so that we don't have to
/// rebuild the UT_SolidAngle tree on every cook if the input hasn't changed.
class SOP_FeE_WindingNumber_1_0Cache : public SOP_NodeCache
{
public:
    SOP_FeE_WindingNumber_1_0Cache() : SOP_NodeCache()
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
    virtual ~SOP_FeE_WindingNumber_1_0Cache() {}

    void update3D(const GA_Detail &mesh_geo, const GA_PrimitiveGroup *prim_group, const UT_StringHolder &group_string,const int approx_order)
    {
        const GA_DataId topology_data_id = mesh_geo.getTopology().getDataId();
        const GA_DataId primitive_list_data_id = mesh_geo.getPrimitiveList().getDataId();
        const GA_DataId P_data_id = mesh_geo.getP()->getDataId();
        const bool has_group = (prim_group != nullptr);
        if (mySubtendedAngleTree.isClear() &&
            topology_data_id == myTopologyDataID &&
            primitive_list_data_id == myPrimitiveListDataID &&
            P_data_id == myPDataID &&
            approx_order == myApproxOrder &&
            has_group == myHadGroup &&
            (!has_group || (
                group_string == myGroupString &&
                mesh_geo.getUniqueId() == myUniqueId &&
                mesh_geo.getMetaCacheCount() == myMetaCacheCount)))
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
        myUniqueId = mesh_geo.getUniqueId();
        myMetaCacheCount = mesh_geo.getMetaCacheCount();

        GA_Size nprimitives = prim_group ? prim_group->entries() : mesh_geo.getNumPrimitives();
        myTrianglePoints.setSize(0);
        myTrianglePoints.setCapacity(3*nprimitives);
        myPositions3D.setSize(0);
        myPositions3D.setCapacity(0);
        UT_Array<int> ptmap;
        if (!prim_group)
        {
            // Copy all point positions
            mesh_geo.getAttributeAsArray(mesh_geo.getP(), mesh_geo.getPointRange(), myPositions3D);
        }
        else
        {
            // Find the used points
            GA_PointGroup mesh_ptgroup(mesh_geo);
            mesh_ptgroup.combine(prim_group);
            myPositions3D.setSizeNoInit(mesh_ptgroup.entries());
            ptmap.setSizeNoInit(mesh_geo.getNumPointOffsets());
            int ptnum = 0;
            mesh_geo.forEachPoint(&mesh_ptgroup, [&mesh_geo,this,&ptmap,&ptnum](const GA_Offset ptoff)
            {
                ptmap[ptoff] = ptnum;
                myPositions3D[ptnum] = mesh_geo.getPos3(ptoff);
                ++ptnum;
            });
        }

        GA_Offset start;
        GA_Offset end;
        for (GA_Iterator it(mesh_geo.getPrimitiveRange(prim_group)); it.blockAdvance(start,end); )
        {
            for (GA_Offset primoff = start; primoff < end; ++primoff)
            {
                sopAccumulateTriangles(&mesh_geo, primoff, ptmap, myTrianglePoints);
            }
        }

        mySolidAngleTree.init(myTrianglePoints.size()/3, myTrianglePoints.array(), myPositions3D.size(), myPositions3D.array(), approx_order);
    }

    void update2D(
        const GA_Detail &mesh_geo,
        const GA_PrimitiveGroup *prim_group,
        const UT_StringHolder &group_string,
        const int approx_order,
        const int axis0, const int axis1)
    {
        const GA_DataId topology_data_id = mesh_geo.getTopology().getDataId();
        const GA_DataId primitive_list_data_id = mesh_geo.getPrimitiveList().getDataId();
        const GA_DataId P_data_id = mesh_geo.getP()->getDataId();
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
                mesh_geo.getUniqueId() == myUniqueId &&
                mesh_geo.getMetaCacheCount() == myMetaCacheCount)))
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
        myUniqueId = mesh_geo.getUniqueId();
        myMetaCacheCount = mesh_geo.getMetaCacheCount();

        GA_Size nprimitives = prim_group ? prim_group->entries() : mesh_geo.getNumPrimitives();
        // NOTE: myTrianglePoints is actually segment points
        myTrianglePoints.setSize(0);
        myTrianglePoints.setCapacity(2*nprimitives);
        myPositions2D.setSize(0);
        myPositions2D.setCapacity(0);
        UT_Array<int> ptmap;
        if (!prim_group)
        {
            // Copy all point positions
            myPositions2D.setSizeNoInit(mesh_geo.getNumPoints());
            int ptnum = 0;
            mesh_geo.forEachPoint([&mesh_geo,this,&ptnum,axis0,axis1](const GA_Offset ptoff)
            {
                UT_Vector3 pos = mesh_geo.getPos3(ptoff);
                myPositions2D[ptnum] = UT_Vector2(pos[axis0],pos[axis1]);
                ++ptnum;
            });
        }
        else
        {
            // Find the used points
            GA_PointGroup mesh_ptgroup(mesh_geo);
            mesh_ptgroup.combine(prim_group);
            myPositions2D.setSizeNoInit(mesh_ptgroup.entries());
            ptmap.setSizeNoInit(mesh_geo.getNumPointOffsets());
            int ptnum = 0;
            mesh_geo.forEachPoint(&mesh_ptgroup, [&mesh_geo,this,&ptmap,&ptnum,axis0,axis1](const GA_Offset ptoff)
            {
                ptmap[ptoff] = ptnum;
                UT_Vector3 pos = mesh_geo.getPos3(ptoff);
                myPositions2D[ptnum] = UT_Vector2(pos[axis0],pos[axis1]);
                ++ptnum;
            });
        }

        GA_Offset start;
        GA_Offset end;
        for (GA_Iterator it(mesh_geo.getPrimitiveRange(prim_group)); it.blockAdvance(start,end); )
        {
            for (GA_Offset primoff = start; primoff < end; ++primoff)
            {
                sopAccumulateSegments(&mesh_geo, primoff, ptmap, myTrianglePoints);
            }
        }

        mySubtendedAngleTree.init(myTrianglePoints.size()/2, myTrianglePoints.array(), myPositions2D.size(), myPositions2D.array(), approx_order);
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

    UT_SolidAngle<float,float> mySolidAngleTree;
    UT_SubtendedAngle<float,float> mySubtendedAngleTree;
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


class SOP_FeE_WindingNumber_1_0Verb : public SOP_NodeVerb
{
public:
    virtual SOP_NodeParms *allocParms() const { return new SOP_FeE_WindingNumber_1_0Parms(); }
    virtual SOP_NodeCache *allocCache() const { return new SOP_FeE_WindingNumber_1_0Cache(); }
    virtual UT_StringHolder name() const { return theSOPTypeName; }

    /// This SOP wouldn't get any benefit from the results of the previous cook,
    /// (except for what's stored in SOP_FeE_WindingNumber_1_0Cache), and it would
    /// always duplicate its input at the start of the cook anyway, so it might
    /// as well use COOK_INPLACE, (which always either steals the first input's
    /// detail for the output detail or duplicates it into the output detail),
    /// instead of COOK_GENERIC, (which would let us have an output detail
    /// that's separate from the input detail and might be the same output
    /// detail as on a previous cook).
    virtual CookMode cookMode(const SOP_NodeParms *parms) const { return COOK_INPLACE; }

    virtual void cook(const CookParms &cookparms) const;

    /// This is the internal name of the SOP type.
    /// It isn't allowed to be the same as any other SOP's type name.
    static const UT_StringHolder theSOPTypeName;

    /// This static data member automatically registers
    /// this verb class at library load time.
    static const SOP_NodeVerb::Register<SOP_FeE_WindingNumber_1_0Verb> theVerb;

    /// This is the parameter interface string, below.
    static const char *const theDsFile;
};

// The static member variable definitions have to be outside the class definition.
// The declarations are inside the class.
const UT_StringHolder SOP_FeE_WindingNumber_1_0Verb::theSOPTypeName("hdk_windingnumber"_sh);
const SOP_NodeVerb::Register<SOP_FeE_WindingNumber_1_0Verb> SOP_FeE_WindingNumber_1_0Verb::theVerb;

PRM_Template *SOP_FeE_WindingNumber_1_0::buildTemplates()
{
    static PRM_TemplateBuilder templ("SOP_FeE_WindingNumber_1_0.C"_sh, SOP_FeE_WindingNumber_1_0Verb::theDsFile);
    if (templ.justBuilt())
    {
        templ.setChoiceListPtr("querypoints"_sh, &SOP_Node::pointGroupMenu);
        templ.setChoiceListPtr("meshprims"_sh, &SOP_Node::primGroupMenu);
    }
    return templ.templates();
}

const SOP_NodeVerb *SOP_FeE_WindingNumber_1_0::cookVerb() const
{
    return SOP_FeE_WindingNumber_1_0Verb::theVerb.get();
}

/// newSopOperator is the hook that Houdini grabs from this dll
/// and invokes to register the SOP.  In this case, we add ourselves
/// to the specified operator table.
void newSopOperator(OP_OperatorTable *table)
{
    table->addOperator(new OP_Operator(
        SOP_FeE_WindingNumber_1_0Verb::theSOPTypeName,  // Internal name
        "Winding Number",                       // UI name
        SOP_FeE_WindingNumber_1_0::myConstructor,       // How to build the SOP
        SOP_FeE_WindingNumber_1_0::buildTemplates(),    // My parameters
        2,      // Min # of sources
        2,      // Max # of sources
        nullptr,// Custom local variables (none)
        0));    // No flags: not a generator, no merge input, not an output
}

//******************************************************************************
//*                 Parameters                                                 *
//******************************************************************************

/// This is a multi-line raw string specifying the parameter interface for this SOP.
const char *const SOP_FeE_WindingNumber_1_0Verb::theDsFile = R"THEDSFILE(
{
    name        parameters
    parm {
        name    "querypoints"
        cppname "QueryPoints"
        label   "Query Points"
        type    string
        default { "" }
        parmtag { "script_action" "import soputils\nkwargs['geometrytype'] = (hou.geometryType.Points,)\nkwargs['inputindex'] = 0\nsoputils.selectGroupParm(kwargs)" }
        parmtag { "script_action_help" "Select geometry from an available viewport.\nShift-click to turn on Select Groups." }
        parmtag { "script_action_icon" "BUTTONS_reselect" }
    }
    parm {
        name    "meshprims"
        cppname "MeshPrims"
        label   "Mesh Primitives"
        type    string
        default { "" }
        parmtag { "script_action" "import soputils\nkwargs['geometrytype'] = (hou.geometryType.Primitives,)\nkwargs['inputindex'] = 1\nsoputils.selectGroupParm(kwargs)" }
        parmtag { "script_action_help" "Select geometry from an available viewport.\nShift-click to turn on Select Groups." }
        parmtag { "script_action_icon" "BUTTONS_reselect" }
        parmtag { "sop_input" "1" }
    }
    parm {
        name    "type"
        label   "Winding Number Type"
        type    ordinal
        default { "0" }
        menu {
            "xyz"   "3D"
            "xy"    "2D in XY Plane"
            "yz"    "2D in YZ Plane"
            "zx"    "2D in ZX Plane"
        }
    }
    parm {
        name    "attrib"
        label   "Attribute Name"
        type    string
        default { "winding_number" }
    }
    parm {
        name    "assolidangle"
        cppname "AsSolidAngle"
        label   "Scale to Solid Angle"
        type    toggle
        default { "0" }
        joinnext
    }
    parm {
        name    "negate"
        cppname "Negate"
        label   "Negate Value (Reverse)"
        type    toggle
        default { "0" }
    }
    parm {
        name    "fullaccuracy"
        cppname "FullAccuracy"
        label   "Full Accuracy (Slow)"
        type    toggle
        default { "0" }
    }
    parm {
        name    "accuracyscale"
        cppname "AccuracyScale"
        label   "Accuracy Scale"
        type    float
        default { "2" }
        range   { 1! 20 }
        disablewhen "{ fullaccuracy == 1 }"
    }
}
)THEDSFILE";

//******************************************************************************
//*                 Cooking                                                    *
//******************************************************************************

static void
sopSumContributions3D(
    double *sum,
    const UT_Vector3D &query_point,
    const GEO_Detail *const mesh_geo,
    const GA_OffsetList &primoffs,
    const exint start, const exint end)
{
    if (end-start > 1024)
    {
        exint mid = (start+end)>>1;
        double sum0;
        double sum1;
        UTparallelInvoke(true, [&] {
            sopSumContributions3D(&sum0, query_point, mesh_geo, primoffs, start, mid);
        }, [&] {
            sopSumContributions3D(&sum1, query_point, mesh_geo, primoffs, mid, end);
        });
        *sum = sum0 + sum1;
        return;
    }

    double local_sum = 0;
    for (exint arrayi = start; arrayi < end; ++arrayi)
    {
        GA_Offset primoff = primoffs(arrayi);
        int primtype = mesh_geo->getPrimitiveTypeId(primoff);
        if (primtype == GA_PRIMPOLY)
        {
            const GA_OffsetListRef vertices = mesh_geo->getPrimitiveVertexList(primoff);
            const GA_Size n = vertices.size();
            const bool closed = vertices.getExtraFlag();
            if (n < 3 || !closed)
                continue;

            if (n == 3)
            {
                const GA_Offset pt0 = mesh_geo->vertexPoint(vertices(0));
                const GA_Offset pt1 = mesh_geo->vertexPoint(vertices(1));
                const GA_Offset pt2 = mesh_geo->vertexPoint(vertices(2));
                const UT_Vector3D p0 = mesh_geo->getPos3(pt0);
                const UT_Vector3D p1 = mesh_geo->getPos3(pt1);
                const UT_Vector3D p2 = mesh_geo->getPos3(pt2);
                const double poly_sum = UTsignedSolidAngleTri(p0, p1, p2, query_point);
                local_sum += poly_sum;
                continue;
            }
            if (n == 4)
            {
                // Special case for quads, to divide the correct diagonal
                // in the case where the point is inside the convex hull.
                const GA_Offset pt0 = mesh_geo->vertexPoint(vertices(0));
                const GA_Offset pt1 = mesh_geo->vertexPoint(vertices(1));
                const GA_Offset pt2 = mesh_geo->vertexPoint(vertices(2));
                const GA_Offset pt3 = mesh_geo->vertexPoint(vertices(3));
                const UT_Vector3D p0 = mesh_geo->getPos3(pt0);
                const UT_Vector3D p1 = mesh_geo->getPos3(pt1);
                const UT_Vector3D p2 = mesh_geo->getPos3(pt2);
                const UT_Vector3D p3 = mesh_geo->getPos3(pt3);
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
            const UT_Vector3D p0 = mesh_geo->getPos3(mesh_geo->vertexPoint(vertices(0)));
            UT_Vector3D prev = mesh_geo->getPos3(mesh_geo->vertexPoint(vertices(1)));
            for (GA_Size i = 2; i < n; ++i)
            {
                const UT_Vector3D next = mesh_geo->getPos3(mesh_geo->vertexPoint(vertices(i)));
                poly_sum += UTsignedSolidAngleTri(p0, prev, next, query_point);
                prev = next;
            }
            local_sum += poly_sum;
        }
        else if (primtype == GA_PRIMTETRAHEDRON)
        {
            const GA_OffsetListRef vertices = mesh_geo->getPrimitiveVertexList(primoff);
            const GEO_PrimTetrahedron tet(SYSconst_cast(mesh_geo), primoff, vertices);

            double tet_sum = 0;
            for (int i = 0; i < 4; ++i)
            {
                // Ignore shared tet faces.  They would contribute exactly opposite amounts.
                if (tet.isFaceShared(i))
                    continue;

                const int *face_indices = GEO_PrimTetrahedron::fastFaceIndices(i);
                const GA_Offset pt0 = mesh_geo->vertexPoint(vertices(face_indices[0]));
                const GA_Offset pt1 = mesh_geo->vertexPoint(vertices(face_indices[1]));
                const GA_Offset pt2 = mesh_geo->vertexPoint(vertices(face_indices[2]));
                const UT_Vector3D a = mesh_geo->getPos3(pt0);
                const UT_Vector3D b = mesh_geo->getPos3(pt1);
                const UT_Vector3D c = mesh_geo->getPos3(pt2);

                tet_sum += UTsignedSolidAngleTri(a, b, c, query_point);
            }
            local_sum += tet_sum;
        }
        else if (primtype == GA_PRIMPOLYSOUP)
        {
            double soup_sum = 0;
            const GEO_PrimPolySoup *soup = UTverify_cast<const GEO_PrimPolySoup *>(mesh_geo->getPrimitive(primoff));
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
            const GEO_PrimMesh *mesh = UTverify_cast<const GEO_PrimMesh *>(mesh_geo->getPrimitive(primoff));
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
            const GEO_TPSurf *surf = UTverify_cast<const GEO_TPSurf *>(mesh_geo->getPrimitive(primoff));
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
            UT_ASSERT(ugrevilles.size() >= nquadcols+1);
            UT_ASSERT(vgrevilles.size() >= nquadrows+1);
            for (int row = 0; row < nquadrows; ++row)
            {
                for (int col = 0; col < nquadcols; ++col)
                {
                    UT_Vector4 a4;
                    UT_Vector4 b4;
                    UT_Vector4 c4;
                    UT_Vector4 d4;
                    surf->evaluateInteriorPoint(a4, ugrevilles(col), vgrevilles(row));
                    surf->evaluateInteriorPoint(b4, ugrevilles(col+1), vgrevilles(row));
                    surf->evaluateInteriorPoint(c4, ugrevilles(col+1), vgrevilles(row+1));
                    surf->evaluateInteriorPoint(d4, ugrevilles(col), vgrevilles(row+1));
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
            const GEO_PrimSphere *sphere = UTverify_cast<const GEO_PrimSphere *>(mesh_geo->getPrimitive(primoff));
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
                sphere_sum = 4*M_PI;
            else if (length2 == 1)
                sphere_sum = 2*M_PI;

            if (determinant > 0)
                sphere_sum = -sphere_sum;

            local_sum += sphere_sum;
        }
    }
    *sum = local_sum;
}

static void
sopSumContributions2D(
    double *sum,
    const UT_Vector2D &query_point,
    const GEO_Detail *const mesh_geo,
    const GA_OffsetList &primoffs,
    const exint start, const exint end,
    const int axis0, const int axis1)
{
    if (end-start > 1024)
    {
        exint mid = (start+end)>>1;
        double sum0;
        double sum1;
        UTparallelInvoke(true, [&] {
            sopSumContributions2D(&sum0, query_point, mesh_geo, primoffs, start, mid, axis0, axis1);
        }, [&] {
            sopSumContributions2D(&sum1, query_point, mesh_geo, primoffs, mid, end, axis0, axis1);
        });
        *sum = sum0 + sum1;
        return;
    }

    double local_sum = 0;
    for (exint arrayi = start; arrayi < end; ++arrayi)
    {
        GA_Offset primoff = primoffs(arrayi);
        int primtype = mesh_geo->getPrimitiveTypeId(primoff);
        if (primtype == GA_PRIMPOLY)
        {
            const GA_OffsetListRef vertices = mesh_geo->getPrimitiveVertexList(primoff);
            const GA_Size n = vertices.size();
            const bool closed = vertices.getExtraFlag();
            if (n < 2+int(closed))
                continue;

            double poly_sum = 0;
            UT_Vector3 pos = mesh_geo->getPos3(mesh_geo->vertexPoint(vertices(0)));
            UT_Vector2D prev(pos[axis0], pos[axis1]);
            const UT_Vector2D p0 = prev;
            for (GA_Size i = 1; i < n; ++i)
            {
                pos = mesh_geo->getPos3(mesh_geo->vertexPoint(vertices(i)));
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
            const GEO_PrimPolySoup *soup = UTverify_cast<const GEO_PrimPolySoup *>(mesh_geo->getPrimitive(primoff));
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
            const GEO_PrimMesh *mesh = UTverify_cast<const GEO_PrimMesh *>(mesh_geo->getPrimitive(primoff));
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
            const GEO_TPSurf *surf = UTverify_cast<const GEO_TPSurf *>(mesh_geo->getPrimitive(primoff));
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
            UT_ASSERT(ugrevilles.size() >= nquadcols+1);
            UT_ASSERT(vgrevilles.size() >= nquadrows+1);
            for (int row = 0; row < nquadrows; ++row)
            {
                for (int col = 0; col < nquadcols; ++col)
                {
                    UT_Vector4 a4;
                    UT_Vector4 b4;
                    UT_Vector4 c4;
                    UT_Vector4 d4;
                    surf->evaluateInteriorPoint(a4, ugrevilles(col), vgrevilles(row));
                    surf->evaluateInteriorPoint(b4, ugrevilles(col+1), vgrevilles(row));
                    surf->evaluateInteriorPoint(c4, ugrevilles(col+1), vgrevilles(row+1));
                    surf->evaluateInteriorPoint(d4, ugrevilles(col), vgrevilles(row+1));
                    const UT_Vector2D p0(a4[axis0],a4[axis1]);
                    const UT_Vector2D p1(b4[axis0],b4[axis1]);
                    const UT_Vector2D p2(c4[axis0],c4[axis1]);
                    const UT_Vector2D p3(d4[axis0],d4[axis1]);
                    double poly_sum;
                    poly_sum  = UTsignedAngleSegment(p0, p1, query_point);
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
            const GEO_Curve *curve = UTverify_cast<const GEO_Curve *>(mesh_geo->getPrimitive(primoff));
            const int nedges = curve->getVertexCount() - !curve->isClosed();
            if (nedges <= 0)
                continue;

            // For slightly better accuracy, we use the greville points
            // instead of the hull points.
            double curve_sum = 0;
            UT_Array<GEO_Greville> grevilles;
            grevilles.setCapacity(nedges+1);
            curve->buildGrevilles(grevilles);
            if (curve->isClosed())
                grevilles.append(grevilles[0]);
            UT_ASSERT(grevilles.size() >= nedges+1);
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
    const GA_Detail *const mesh_geo,
    const GA_Offset primoff,
    const UT_Array<int> &ptmap,
    UT_Array<int> &triangle_points)
{
    const bool has_ptmap = !ptmap.isEmpty();
    int primtype = mesh_geo->getPrimitiveTypeId(primoff);
    if (primtype == GA_PRIMPOLY)
    {
        const GA_OffsetListRef vertices = mesh_geo->getPrimitiveVertexList(primoff);
        const GA_Size n = vertices.size();
        const bool closed = vertices.getExtraFlag();
        if (n < 3 || !closed)
            return;

        // A triangle fan suffices, even if the polygon is non-convex,
        // because the contributions in the opposite direction will
        // partly cancel out the ones in the other direction,
        // in just the right amount.
        const GA_Offset p0 = mesh_geo->vertexPoint(vertices(0));
        const int p0i = has_ptmap ? ptmap[p0] : int(mesh_geo->pointIndex(p0));
        GA_Offset prev = mesh_geo->vertexPoint(vertices(1));
        int previ = has_ptmap ? ptmap[prev] : int(mesh_geo->pointIndex(prev));
        for (GA_Size i = 2; i < n; ++i)
        {
            const GA_Offset next = mesh_geo->vertexPoint(vertices(i));
            const int nexti = has_ptmap ? ptmap[next] : int(mesh_geo->pointIndex(next));
            triangle_points.append(p0i);
            triangle_points.append(previ);
            triangle_points.append(nexti);
            previ = nexti;
        }
    }
    else if (primtype == GA_PRIMTETRAHEDRON)
    {
        const GA_OffsetListRef vertices = mesh_geo->getPrimitiveVertexList(primoff);
        const GEO_PrimTetrahedron tet(SYSconst_cast(mesh_geo), primoff, vertices);

        for (int i = 0; i < 4; ++i)
        {
            // Ignore shared tet faces.  They would contribute exactly opposite amounts.
            if (tet.isFaceShared(i))
                continue;

            const int *face_indices = GEO_PrimTetrahedron::fastFaceIndices(i);
            const GA_Offset a = mesh_geo->vertexPoint(vertices(face_indices[0]));
            const GA_Offset b = mesh_geo->vertexPoint(vertices(face_indices[1]));
            const GA_Offset c = mesh_geo->vertexPoint(vertices(face_indices[2]));
            const int ai = has_ptmap ? ptmap[a] : int(mesh_geo->pointIndex(a));
            const int bi = has_ptmap ? ptmap[b] : int(mesh_geo->pointIndex(b));
            const int ci = has_ptmap ? ptmap[c] : int(mesh_geo->pointIndex(c));
            triangle_points.append(ai);
            triangle_points.append(bi);
            triangle_points.append(ci);
        }
    }
    else if (primtype == GA_PRIMPOLYSOUP)
    {
        const GEO_PrimPolySoup *soup = UTverify_cast<const GEO_PrimPolySoup *>(mesh_geo->getPrimitive(primoff));
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
            const int p0i = has_ptmap ? ptmap[p0] : int(mesh_geo->pointIndex(p0));
            GA_Offset prev = poly.getPointOffset(1);
            int previ = has_ptmap ? ptmap[prev] : int(mesh_geo->pointIndex(prev));
            for (GA_Size i = 2; i < n; ++i)
            {
                const GA_Offset next = poly.getPointOffset(i);
                const int nexti = has_ptmap ? ptmap[next] : int(mesh_geo->pointIndex(next));
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
        const GEO_Hull *mesh = UTverify_cast<const GEO_Hull *>(mesh_geo->getPrimitive(primoff));
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
                const int ai = has_ptmap ? ptmap[a] : int(mesh_geo->pointIndex(a));
                const int bi = has_ptmap ? ptmap[b] : int(mesh_geo->pointIndex(b));
                const int ci = has_ptmap ? ptmap[c] : int(mesh_geo->pointIndex(c));
                const int di = has_ptmap ? ptmap[d] : int(mesh_geo->pointIndex(d));
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
    const GA_Detail *const mesh_geo,
    const GA_Offset primoff,
    const UT_Array<int> &ptmap,
    UT_Array<int> &segment_points)
{
    const bool has_ptmap = !ptmap.isEmpty();
    int primtype = mesh_geo->getPrimitiveTypeId(primoff);
    if (primtype == GA_PRIMPOLY || primtype == GA_PRIMBEZCURVE || primtype == GA_PRIMNURBCURVE)
    {
        const GA_OffsetListRef vertices = mesh_geo->getPrimitiveVertexList(primoff);
        const GA_Size n = vertices.size();
        const bool closed = vertices.getExtraFlag();
        if (n < 2+int(closed))
            return;

        GA_Offset prev = mesh_geo->vertexPoint(vertices(0));
        int previ = has_ptmap ? ptmap[prev] : int(mesh_geo->pointIndex(prev));
        const int pt0i = previ;
        for (GA_Size i = 1; i < n; ++i)
        {
            const GA_Offset next = mesh_geo->vertexPoint(vertices(i));
            const int nexti = has_ptmap ? ptmap[next] : int(mesh_geo->pointIndex(next));
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
        const GEO_PrimPolySoup *soup = UTverify_cast<const GEO_PrimPolySoup *>(mesh_geo->getPrimitive(primoff));
        for (GEO_PrimPolySoup::PolygonIterator poly(*soup); !poly.atEnd(); ++poly)
        {
            GA_Size n = poly.nvertices();
            if (n < 3)
                continue;

            GA_Offset prev = poly.getPointOffset(0);
            int previ = has_ptmap ? ptmap[prev] : int(mesh_geo->pointIndex(prev));
            const int pt0i = previ;
            for (GA_Size i = 1; i < n; ++i)
            {
                const GA_Offset next = poly.getPointOffset(i);
                const int nexti = has_ptmap ? ptmap[next] : int(mesh_geo->pointIndex(next));
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
        const GEO_Hull *mesh = UTverify_cast<const GEO_Hull *>(mesh_geo->getPrimitive(primoff));
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
                const int ai = has_ptmap ? ptmap[a] : int(mesh_geo->pointIndex(a));
                const int bi = has_ptmap ? ptmap[b] : int(mesh_geo->pointIndex(b));
                const int ci = has_ptmap ? ptmap[c] : int(mesh_geo->pointIndex(c));
                const int di = has_ptmap ? ptmap[d] : int(mesh_geo->pointIndex(d));
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
    const GEO_Detail *const query_points,
    const GA_SplittableRange &point_range,
    const GEO_Detail *const mesh_geo,
    const GA_PrimitiveGroup *const mesh_prim_group,
    const GA_RWHandleF &winding_number_attrib,
    const bool as_solid_angle,
    const bool negate)
{
    UT_AutoInterrupt boss("Computing Winding Numbers");

    // See comment below for why we create this GA_Offsetlist of mesh primitive offsets.
    GA_OffsetList primoffs;
    if (!mesh_prim_group && mesh_geo->getPrimitiveMap().isTrivialMap())
    {
        primoffs.setTrivial(GA_Offset(0), mesh_geo->getNumPrimitives());
    }
    else
    {
        GA_Offset start;
        GA_Offset end;
        for (GA_Iterator it(mesh_geo->getPrimitiveRange(mesh_prim_group)); it.fullBlockAdvance(start, end); )
        {
            primoffs.setTrivialRange(primoffs.size(), start, end-start);
        }
    }

    UTparallelFor(point_range, [query_points,mesh_geo,winding_number_attrib,&primoffs,as_solid_angle,negate,&boss](const GA_SplittableRange &r)
    {
        GA_Offset start;
        GA_Offset end;
        for (GA_Iterator it(r); it.blockAdvance(start, end); )
        {
            for (GA_Offset ptoff = start; ptoff != end; ++ptoff)
            {
                if (boss.wasInterrupted())
                    return;

                const UT_Vector3 query_point = query_points->getPos3(ptoff);

                // NOTE: We can't use UTparallelReduce, because that would have
                //       nondeterministic roundoff error due to floating-point
                //       addition being non-associative.  We can't just use
                //       UTparallelInvoke with GA_SplittableRange either, because
                //       the roundoff error would change with defragmentation,
                //       e.g. from locking the mesh input and reloading the HIP file.
                //       Instead, we always split in the middle of the list of offsets.
                double sum;
                sopSumContributions3D(&sum, query_point, mesh_geo, primoffs, 0, primoffs.size());

                if (!as_solid_angle)
                    sum *= (0.25*M_1_PI); // Divide by 4pi (solid angle of full sphere)
                if (negate)
                    sum = -sum;

                winding_number_attrib.set(ptoff, sum);
            }
        }
    });
}

static void
sop2DFullAccuracy(
    const GEO_Detail *const query_points,
    const GA_SplittableRange &point_range,
    const GEO_Detail *const mesh_geo,
    const GA_PrimitiveGroup *const mesh_prim_group,
    const GA_RWHandleF &winding_number_attrib,
    const bool as_solid_angle,
    const bool negate,
    const int axis0,
    const int axis1)
{
    UT_AutoInterrupt boss("Computing Winding Numbers");

    // See comment below for why we create this GA_Offsetlist of mesh primitive offsets.
    GA_OffsetList primoffs;
    if (!mesh_prim_group && mesh_geo->getPrimitiveMap().isTrivialMap())
    {
        primoffs.setTrivial(GA_Offset(0), mesh_geo->getNumPrimitives());
    }
    else
    {
        GA_Offset start;
        GA_Offset end;
        for (GA_Iterator it(mesh_geo->getPrimitiveRange(mesh_prim_group)); it.fullBlockAdvance(start, end); )
        {
            primoffs.setTrivialRange(primoffs.size(), start, end-start);
        }
    }

    UTparallelFor(point_range, [query_points,mesh_geo,winding_number_attrib,&primoffs,as_solid_angle,negate,&boss,axis0,axis1](const GA_SplittableRange &r)
    {
        GA_Offset start;
        GA_Offset end;
        for (GA_Iterator it(r); it.blockAdvance(start, end); )
        {
            for (GA_Offset ptoff = start; ptoff != end; ++ptoff)
            {
                if (boss.wasInterrupted())
                    return;

                const UT_Vector3D query_point_3d = query_points->getPos3(ptoff);
                const UT_Vector2D query_point(query_point_3d[axis0], query_point_3d[axis1]);

                // NOTE: We can't use UTparallelReduce, because that would have
                //       nondeterministic roundoff error due to floating-point
                //       addition being non-associative.  We can't just use
                //       UTparallelInvoke with GA_SplittableRange either, because
                //       the roundoff error would change with defragmentation,
                //       e.g. from locking the mesh input and reloading the HIP file.
                //       Instead, we always split in the middle of the list of offsets.
                double sum;
                sopSumContributions2D(&sum, query_point, mesh_geo, primoffs, 0, primoffs.size(), axis0, axis1);

                if (!as_solid_angle)
                    sum *= (0.5*M_1_PI); // Divide by 2pi (angle of full circle)
                if (negate)
                    sum = -sum;

                winding_number_attrib.set(ptoff, sum);
            }
        }
    });
}

static void
sop3DApproximate(
    const GEO_Detail *const query_points,
    const GA_SplittableRange &point_range,
    const UT_SolidAngle<float,float> &solid_angle_tree,
    const double accuracy_scale,
    const GA_RWHandleF &winding_number_attrib,
    const bool as_solid_angle,
    const bool negate)
{
    UT_AutoInterrupt boss("Computing Winding Numbers");

    UTparallelFor(point_range, [query_points,&winding_number_attrib,&solid_angle_tree,as_solid_angle,negate,accuracy_scale,&boss](const GA_SplittableRange &r)
    {
        GA_Offset start;
        GA_Offset end;
        for (GA_Iterator it(r); it.blockAdvance(start, end); )
        {
            if (boss.wasInterrupted())
                return;

            for (GA_Offset ptoff = start; ptoff != end; ++ptoff)
            {
                const UT_Vector3 query_point = query_points->getPos3(ptoff);

                double sum = solid_angle_tree.computeSolidAngle(query_point, accuracy_scale);

                if (!as_solid_angle)
                    sum *= (0.25*M_1_PI); // Divide by 4pi (solid angle of full sphere)
                if (negate)
                    sum = -sum;

                winding_number_attrib.set(ptoff, sum);
            }
        }
    }, 10); // Large subscribe ratio, because expensive points are often clustered
}

static void
sop2DApproximate(
    const GEO_Detail *const query_points,
    const GA_SplittableRange &point_range,
    const UT_SubtendedAngle<float,float> &subtended_angle_tree,
    const double accuracy_scale,
    const GA_RWHandleF &winding_number_attrib,
    const bool as_angle,
    const bool negate,
    const int axis0,
    const int axis1)
{
    UT_AutoInterrupt boss("Computing Winding Numbers");

    UTparallelFor(point_range, [query_points,&winding_number_attrib,&subtended_angle_tree,as_angle,negate,accuracy_scale,&boss,axis0,axis1](const GA_SplittableRange &r)
    {
        GA_Offset start;
        GA_Offset end;
        for (GA_Iterator it(r); it.blockAdvance(start, end); )
        {
            if (boss.wasInterrupted())
                return;

            for (GA_Offset ptoff = start; ptoff != end; ++ptoff)
            {
                const UT_Vector3 query_point_3d = query_points->getPos3(ptoff);
                const UT_Vector2D query_point(query_point_3d[axis0], query_point_3d[axis1]);

                double sum = subtended_angle_tree.computeAngle(query_point, accuracy_scale);

                if (!as_angle)
                    sum *= (0.5*M_1_PI); // Divide by 2pi (angle of full circle)
                if (negate)
                    sum = -sum;

                winding_number_attrib.set(ptoff, sum);
            }
        }
    }, 10); // Large subscribe ratio, because expensive points are often clustered
}

/// This is the function that does the actual work.
void SOP_FeE_WindingNumber_1_0Verb::cook(const CookParms &cookparms) const
{
    // This gives easy access to all of the current parameter values
    auto &&sopparms = cookparms.parms<SOP_FeE_WindingNumber_1_0Parms>();
    auto sopcache = (SOP_FeE_WindingNumber_1_0Cache *)cookparms.cache();

    // The output detail
    GEO_Detail *const query_points = cookparms.gdh().gdpNC();

    const GEO_Detail *const mesh_geo = cookparms.inputGeo(1);

    GA_RWHandleF winding_number_attrib(query_points->findFloatTuple(GA_ATTRIB_POINT, sopparms.getAttrib()));
    if (!winding_number_attrib.isValid())
        winding_number_attrib.bind(query_points->addFloatTuple(GA_ATTRIB_POINT, sopparms.getAttrib(), 1));

    if (!winding_number_attrib.isValid())
    {
        cookparms.sopAddError(SOP_ATTRIBUTE_INVALID, sopparms.getAttrib());
        return;
    }

    GA_Size npoints = query_points->getNumPoints();
    if (npoints == 0)
        return;

    GOP_Manager group_manager;

    // Parse group of mesh primitives
    const UT_StringHolder &mesh_prim_group_string = sopparms.getMeshPrims();
    const GA_PrimitiveGroup *mesh_prim_group = nullptr;
    if (mesh_prim_group_string.isstring())
    {
        bool success;
        mesh_prim_group = group_manager.parsePrimitiveDetached(
            mesh_prim_group_string.c_str(),
            mesh_geo, true, success);
    }

    // Parse group of query points
    const UT_StringHolder &query_point_group_string = sopparms.getQueryPoints();
    const GA_PointGroup *query_point_group = nullptr;
    if (query_point_group_string.isstring())
    {
        bool success;
        query_point_group = group_manager.parsePointDetached(
            query_point_group_string.c_str(),
            query_points, true, success);
    }

    const GA_SplittableRange point_range(query_points->getPointRange(query_point_group));

    const bool full_accuracy = sopparms.getFullAccuracy();
    const bool as_solid_angle = sopparms.getAsSolidAngle();

    Type winding_number_type = sopparms.getType();

    if (winding_number_type == Type::XYZ)
    {
        // NOTE: The negation is because Houdini's normals are
        //       left-handed with respect to the winding order,
        //       whereas most use the right-handed convension.
        const bool negate = !sopparms.getNegate();
        if (full_accuracy)
        {
            sopcache->clear();

            sop3DFullAccuracy(
                query_points, point_range,
                mesh_geo, mesh_prim_group,
                winding_number_attrib,
                as_solid_angle, negate);
        }
        else
        {
            sopcache->update3D(*mesh_geo, mesh_prim_group, mesh_prim_group_string, 2);

            const UT_SolidAngle<float,float> &solid_angle_tree = sopcache->mySolidAngleTree;
            const double accuracy_scale = sopparms.getAccuracyScale();

            sop3DApproximate(
                query_points, point_range,
                solid_angle_tree, accuracy_scale,
                winding_number_attrib,
                as_solid_angle, negate);
        }
    }
    else
    {
        int axis0 = int(winding_number_type)-1;
        int axis1 = (axis0 == 2) ? 0 : (axis0+1);

        const bool negate = sopparms.getNegate();
        if (full_accuracy)
        {
            sopcache->clear();

            sop2DFullAccuracy(
                query_points, point_range,
                mesh_geo, mesh_prim_group,
                winding_number_attrib,
                as_solid_angle, negate,
                axis0, axis1);
        }
        else
        {
            sopcache->update2D(*mesh_geo, mesh_prim_group, mesh_prim_group_string, 2, axis0, axis1);

            const UT_SubtendedAngle<float,float> &subtended_angle_tree = sopcache->mySubtendedAngleTree;
            const double accuracy_scale = sopparms.getAccuracyScale();

            sop2DApproximate(
                query_points, point_range,
                subtended_angle_tree, accuracy_scale,
                winding_number_attrib,
                as_solid_angle, negate,
                axis0, axis1);
        }
    }

    winding_number_attrib->bumpDataId();
}
