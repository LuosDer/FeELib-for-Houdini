
//#define UT_ASSERT_LEVEL 3
#include "SOP_FeE_JoinCurve_1_0.h"

#define MAXLOOPCOUNT 1000000000

// This is an automatically generated header file based on theDsFile, below,
// to provide SOP_FeE_JoinCurve_1_0Parms, an easy way to access parameter values from
// SOP_FeE_JoinCurve_1_0Verb::cook with the correct type.
#include "SOP_FeE_JoinCurve_1_0.proto.h"

#include <GEO/GEO_Detail.h>
#include <PRM/PRM_TemplateBuilder.h>
#include <UT/UT_Interrupt.h>
#include <UT/UT_DSOVersion.h>

//#include <GU/GU_Detail.h>
//#include <GEO/GEO_PrimPoly.h>
//#include <OP/OP_Operator.h>
//#include <OP/OP_OperatorTable.h>
//#include <PRM/PRM_Include.h>
//#include <PRM/PRM_TemplateBuilder.h>
//#include <UT/UT_DSOVersion.h>
//#include <UT/UT_Interrupt.h>
//#include <UT/UT_StringHolder.h>
//#include <SYS/SYS_Math.h>
//#include <limits.h>
//
//
//#include <UT/UT_UniquePtr.h>
//#include <GA/GA_SplittableRange.h>
//#include <GA/GA_PageHandle.h>
//#include <GA/GA_PageIterator.h>


#include <GA_FeE/GA_FeE_Attribute.h>
#include <GEO_FeE/GEO_FeE_Group.h>
#include <GA_FeE/GA_FeE_VertexNextEquiv.h>


using namespace SOP_FeE_JoinCurve_1_0_Namespace;

//
// Help is stored in a "wiki" style text file.  This text file should be copied
// to $HOUDINI_PATH/help/nodes/sop/FeE.txt
//
// See the sample_install.sh file for an example.
//

/// This is the internal name of the SOP type.
/// It isn't allowed to be the same as any other SOP's type name.
const UT_StringHolder SOP_FeE_JoinCurve_1_0::theSOPTypeName("FeE::joinCurve::1.0"_sh);

/// newSopOperator is the hook that Houdini grabs from this dll
/// and invokes to register the SOP.  In this case, we add ourselves
/// to the specified operator table.
void
newSopOperator(OP_OperatorTable *table)
{
    table->addOperator(new OP_Operator(
        SOP_FeE_JoinCurve_1_0::theSOPTypeName,   // Internal name
        "FeE Join Curve",     // UI name
        SOP_FeE_JoinCurve_1_0::myConstructor,    // How to build the SOP
        SOP_FeE_JoinCurve_1_0::buildTemplates(), // My parameters
        1,                         // Min # of sources
        1,                         // Max # of sources
        nullptr,                   // Custom local variables (none)
        OP_FLAG_GENERATOR));       // Flag it as generator
}

/// This is a multi-line raw string specifying the parameter interface
/// for this SOP.
static const char *theDsFile = R"THEDSFILE(
{
    name        parameters
    parm {
        name    "stopGroup"
        label   "Stop Group"
        type    string
        default { "" }
        menutoggle {
            [ "opmenu -l pointwrangle1 group" ]
        }
        range   { 0 1 }
        parmtag { "autoscope" "0000000000000000" }
        parmtag { "script_action" "import soputils\nkwargs['geometrytype'] = kwargs['node'].parmTuple('grouptype')\nkwargs['inputindex'] = 0\nsoputils.selectGroupParm(kwargs)" }
        parmtag { "script_action_help" "Select geometry from an available viewport." }
        parmtag { "script_action_icon" "BUTTONS_reselect" }
    }
    parm {
        name    "sortPtnum"
        label   "Sort ptnum"
        type    toggle
        default { "0" }
    }
    parm {
        name    "keepOrder"
        label   "Keep Order"
        type    toggle
        default { "0" }
    }
    parm {
        name    "closeLoops"
        label   "Make Isolated Loops Closed"
        type    toggle
        default { "0" }
        range   { 0 1 }
    }



    parm {
        name    "copyAttribNameFromFirstElem"
        label   "Copy Attribute Name from First Elem"
        type    string
        default { "" }
    }
    parm {
        name    "uniqueneb_srcprims"
        label   "Unique Neb Source Prims"
        type    toggle
        default { "1" }
        disablewhen "{ outSrcPrims == 0 }"
    }
    parm {
        name    "keepEdgeGroup"
        label   "Keep Edge Group"
        type    toggle
        default { "1" }
    }
    parm {
        name    "removeDuplicateLoop"
        label   "Remove Duplicate Loop"
        type    toggle
        default { "1" }
    }


    parm {
        name    "primGroup"
        cppname "PrimGroup"
        label   "Prim Group"
        type    string
        default { "" }
        parmtag { "script_action" "import soputils\nkwargs['geometrytype'] = (hou.geometryType.Primitives,)\nkwargs['inputindex'] = 0\nsoputils.selectGroupParm(kwargs)" }
        parmtag { "script_action_help" "Select geometry from an available viewport.\nShift-click to turn on Select Groups." }
        parmtag { "script_action_icon" "BUTTONS_reselect" }
        parmtag { "sop_input" "0" }
    }
    parm {
        name    "pointGroup"
        cppname "PointGroup"
        label   "Point Group"
        type    string
        default { "" }
        parmtag { "script_action" "import soputils\nkwargs['geometrytype'] = (hou.geometryType.Points,)\nkwargs['inputindex'] = 0\nsoputils.selectGroupParm(kwargs)" }
        parmtag { "script_action_help" "Select geometry from an available viewport.\nShift-click to turn on Select Groups." }
        parmtag { "script_action_icon" "BUTTONS_reselect" }
        parmtag { "sop_input" "0" }
    }
    parm {
        name    "vertexGroup"
        cppname "VertexGroup"
        label   "Vertex Group"
        type    string
        default { "" }
        parmtag { "script_action" "import soputils\nkwargs['geometrytype'] = (hou.geometryType.Vertices,)\nkwargs['inputindex'] = 0\nsoputils.selectGroupParm(kwargs)" }
        parmtag { "script_action_help" "Select geometry from an available viewport.\nShift-click to turn on Select Groups." }
        parmtag { "script_action_icon" "BUTTONS_reselect" }
        parmtag { "sop_input" "0" }
    }
    parm {
        name    "edgeGroup"
        cppname "EdgeGroup"
        label   "Edge Group"
        type    string
        default { "" }
        parmtag { "script_action" "import soputils\nkwargs['geometrytype'] = (hou.geometryType.Edges,)\nkwargs['inputindex'] = 0\nsoputils.selectGroupParm(kwargs)" }
        parmtag { "script_action_help" "Select geometry from an available viewport.\nShift-click to turn on Select Groups." }
        parmtag { "script_action_icon" "BUTTONS_reselect" }
        parmtag { "sop_input" "0" }
    }



    parm {
        name    "attribFromVertex"
        cppname "AttribFromVertex"
        label   "Attrib from Vertex"
        type    string
        default { "" }
    }
    parm {
        name    "attribFromPrim"
        cppname "AttribFromPrim"
        label   "Attrib from Prim"
        type    string
        default { "" }
    }
    parm {
        name    "groupFromVertex"
        cppname "GroupFromVertex"
        label   "Group from Vertex"
        type    string
        default { "" }
    }
    parm {
        name    "groupFromPrim"
        cppname "GroupFromPrim"
        label   "Group from Prim"
        type    string
        default { "" }
    }
    parm {
        name    "groupFromEdge"
        cppname "GroupFromEdge"
        label   "Group from Edge"
        type    string
        default { "" }
    }

    parm {
        name    "promoteEdgeGroupToPrim"
        cppname "PromoteEdgeGroupToPrim"
        label   "Promote Edge Group to Prim"
        type    string
        default { "" }
    }



    parm {
       name    "outSrcPrims"
       cppname "OutSrcPrims"
       label   "Output Source Prims"
       type    toggle
       default { 0 }
       nolabel
       joinnext
    }
    parm {
        name    "srcPrimsAttribName"
        cppname "SrcPrimsAttribName"
        label   "Source Prims Attrib Name"
        type    string
        default { "srcPrims" }
        disablewhen "{ outSrcPrims == 0 }"
    }



    parm {
        name    "primType"
        cppname "PrimType"
        label   "Prim Type"
        type    ordinal
        default { "polyline" }
        menu {
            "polyline"  "Polyline"
            "poly"      "Poly"
        }
    }



    parm {
       name    "subscribeRatio"
       cppname "SubscribeRatio"
       label   "Subscribe Ratio"
       type    integer
       default { 64 }
       range   { 0! 256 }
    }
    parm {
       name    "minGrainSize"
       cppname "MinGrainSize"
       label   "Min Grain Size"
       type    intlog
       default { 64 }
       range   { 0! 2048 }
    }
}
)THEDSFILE";

PRM_Template*
SOP_FeE_JoinCurve_1_0::buildTemplates()
{
    static PRM_TemplateBuilder templ("SOP_FeE_JoinCurve_1_0.C"_sh, theDsFile);
    return templ.templates();
}

class SOP_FeE_JoinCurve_1_0Verb : public SOP_NodeVerb
{
public:
    SOP_FeE_JoinCurve_1_0Verb() {}
    virtual ~SOP_FeE_JoinCurve_1_0Verb() {}

    virtual SOP_NodeParms *allocParms() const { return new SOP_FeE_JoinCurve_1_0Parms(); }
    virtual UT_StringHolder name() const { return SOP_FeE_JoinCurve_1_0::theSOPTypeName; }

    virtual CookMode cookMode(const SOP_NodeParms *parms) const { return COOK_GENERIC; }

    virtual void cook(const CookParms &cookparms) const;
    
    /// This static data member automatically registers
    /// this verb class at library load time.
    static const SOP_NodeVerb::Register<SOP_FeE_JoinCurve_1_0Verb> theVerb;
};

// The static member variable definition has to be outside the class definition.
// The declaration is inside the class.
const SOP_NodeVerb::Register<SOP_FeE_JoinCurve_1_0Verb> SOP_FeE_JoinCurve_1_0Verb::theVerb;

const SOP_NodeVerb *
SOP_FeE_JoinCurve_1_0::cookVerb() const 
{ 
    return SOP_FeE_JoinCurve_1_0Verb::theVerb.get();
}




static bool
sopPrimPolyIsClosed(SOP_FeE_JoinCurve_1_0Parms::PrimType parmgrouptype)
{
    using namespace SOP_FeE_JoinCurve_1_0Enums;
    switch (parmgrouptype)
    {
    case PrimType::POLYLINE:   return 0;    break;
    case PrimType::POLY:       return 1;    break;
    }
    UT_ASSERT_MSG(0, "Unhandled Prim type!");
    return 0;
}





//// Calls functor on every active offset in this index map.
//template<typename FUNCTOR>
//SYS_FORCE_INLINE
//void forEachOffset(GA_IndexMap& idxmap, FUNCTOR&& functor)
//{
//    if (idxmap.isTrivialMap())
//    {
//        const GA_Offset end = GA_Offset(GA_Size(idxmap.indexSize()));
//        for (GA_Offset off(0); off != end; ++off)
//        {
//            functor(off, off);
//        }
//    }
//    else
//    {
//        const GA_Offset veryend(idxmap.myMaxOccupiedOffset + 1);
//        GA_Size idx(0);
//        GA_Offset off(0);
//        while (true)
//        {
//            off = idxmap.findActiveOffset(off, veryend);
//            GA_Offset end = idxmap.findInactiveOffset(off, veryend);
//            if (off == end)
//                break;
//            do
//            {
//                functor(off, idx);
//                ++off;
//                ++idx;
//            } while (off != end);
//        }
//    }
//}




/*
template<typename FUNCTOR>
static void forEachOffset(FUNCTOR&& functor, const GA_IndexMap& index_map, const GA_ElementGroup* group = nullptr, bool complement = false)
{
    // Fall back to regular case if no group.
    //if (!group)
    //{
    //    if (!complement)
    //        index_map.forEachOffset(functor);
    //    return;
    //}

    // Group order is only relevant if not complemented.
    if (!complement)
    {
        const GA_ElementGroupOrder* order = group->getOrdered();
        if (order)
        {
            GA_Size idx(0);
            for (GA_ElementGroupOrderIndex i(0), n(order->entries()); i != n; ++i)
            {
                GA_Offset off = order->getElement(i);
                functor(off, idx);
                ++idx;
            }
            return;
        }
    }

    // We have a group, treated as unordered.
    const GA_Offset veryend = index_map.offsetSize();
    GA_Size idx(0);
    GA_Offset off(0);
    while (true)
    {
        bool value;
        GA_Size span_size;
        group->getConstantSpan(off, veryend, span_size, value);
        if (span_size == 0)
            break;
        if (value == complement)
        {
            off += span_size;
            continue;
        }
        const GA_Offset span_end = off + span_size;
        while (true)
        {
            off = index_map.findActiveOffset(off, span_end);
            GA_Offset end = index_map.findInactiveOffset(off, span_end);
            if (off == end)
                break;
            do
            {
                functor(off, idx);
                ++off;
                ++idx;
            } while (off != end);
        }
    }
}

template<typename FUNCTOR>
SYS_FORCE_INLINE
void forEachPrimitive(GA_Detail* geo, const GA_PrimitiveGroup* group, bool complement, FUNCTOR&& functor)
{
    forEachOffset(functor, geo->getPrimitiveMap(), group, complement);
}

*/


//template<typename FUNCTOR>
//SYS_FORCE_INLINE
//void forEachVertex(GA_Detail* geo, const GA_VertexGroup* group, bool complement, FUNCTOR&& functor)
//{
//    forEachOffset(functor, geo->getVertexMap(), group, complement);
//}





void
SOP_FeE_JoinCurve_1_0Verb::cook(const SOP_NodeVerb::CookParms &cookparms) const
{
    auto&& sopparms = cookparms.parms<SOP_FeE_JoinCurve_1_0Parms>();
    GU_Detail* outGeo0 = cookparms.gdh().gdpNC();
    //auto sopcache = (SOP_FeE_JoinCurve_1_0Cache*)cookparms.cache();

    const GEO_Detail* const inGeo0 = cookparms.inputGeo(0);

    outGeo0->replaceWithPoints(*inGeo0);


    //GU_DetailHandle tmpGeoH0;
    //GU_Detail* tmpGeo0 = new GU_Detail();
    //tmpGeoH0.allocateAndSet(tmpGeo0);
    //tmpGeo0->replaceWith(*inGeo0);

    GA_PointGroup* groupOneNeb = GA_FeE_TopologyReference::addGroupOneNeb(outGeo0, nullptr);

    //outGeo0 = sopNodeProcess(*inGeo0);

    const UT_StringHolder& primGroupName = sopparms.getPrimGroup();
    const UT_StringHolder& pointGroupName = sopparms.getPointGroup();
    const UT_StringHolder& vertexGroupName = sopparms.getVertexGroup();
    const UT_StringHolder& edgeGroupName = sopparms.getEdgeGroup();



    GA_Offset start, end;
    for (GA_Iterator it(outGeo0->getPointRange(groupOneNeb)); it.fullBlockAdvance(start, end); )
    {
        for (GA_Offset primoff = start; primoff < end; ++primoff)
        {
        }
    }


    int passedPoints[];
    resize(passedPoints, @numpt);
    
        for (int ptnum = 0; ptnum < @numpt; ++ptnum) {
#if 1
            int vtxnum_neb = pointvertex(1, ptnum);
            if (vertexnext(1, vtxnum_neb) != -1) continue;
            int vtxpnum_neb = vertexprimindex(1, vtxnum_neb);
            if (chi("../keepOrder")) {
                if (vtxpnum_neb != 0) continue;
            }
            else {
                if (!inpointgroup(1, chs("../tmpGroup_oneNeb"), ptnum)) continue;
            }
#else
            if (!inpointgroup(1, chi("../keepOrder") ? chs("../tmpGroup_start") : chs("../tmpGroup_oneNeb"), ptnum)) continue;
            int vtxnum_neb = pointvertex(1, ptnum);
            int vtxpnum_neb = vertexprimindex(1, vtxnum_neb);
#endif

            if (passedPoints[ptnum]) continue;
            if (chs("../stopgroup") != '' && inpointgroup(1, chs("../stopgroup"), ptnum)) continue;


            int newprim = addprim(0, 'polyline', ptnum);
            int srcPrim[];

            // int vtxnum_neb = pointvertex(1, ptnum);
            int primnum_neb = vertexprim(1, vtxnum_neb);
#if 0
            while (1) {
#else
            for (int LOOPCOUNT = 0; LOOPCOUNT <= MAXLOOPCOUNT; ++LOOPCOUNT) {
                if (LOOPCOUNT == MAXLOOPCOUNT) {
                    error('Unsupport Input Geo so arrive death loop');
                }
#endif
                int flag = 0;
                vtxpnum_neb = vertexprimindex(1, vtxnum_neb);

                int numvtx = primvertexcount(1, primnum_neb);
                int ptnum_neb;
                // printf('\n %d', ptnum);
                if (vtxpnum_neb == 0) {
                    for (int vtxpnum = 1; vtxpnum < numvtx; ++vtxpnum) {
                        ptnum_neb = primpoint(1, primnum_neb, vtxpnum);
                        addvertex(0, newprim, ptnum_neb);
                        if (inpointgroup(1, chs("../tmpGroup_oneNeb"), ptnum_neb)) {
                            flag = 1;
                            break;
                        }
                    }
                }
                else {
                    for (int vtxpnum = numvtx - 2; vtxpnum >= 0; --vtxpnum) {
                        ptnum_neb = primpoint(1, primnum_neb, vtxpnum);
                        addvertex(0, newprim, ptnum_neb);
                        if (inpointgroup(1, chs("../tmpGroup_oneNeb"), ptnum_neb)) {
                            flag = 1;
                            break;
                        }
                    }
                }
                if (chi("../outsrcprims")) {
                    push(srcPrim, primnum_neb);
                }
                if (flag) {
                    passedPoints[ptnum_neb] = 1;
                    if (chi("../outsrcprims")) {
                        setprimattrib(0, chs("../srcprimsname"), newprim, srcPrim);
                    }
                    break;
                }

                vtxnum_neb = pointvertex(1, ptnum_neb);
                // for (vtxnum_neb = pointvertex(1, ptnum); vtxnum_neb != -1; vtxnum_neb = vertexnext(1, vtxnum_neb)) {
                while (1) {
                    int primnum_neb_next = vertexprim(1, vtxnum_neb);
                    if (primnum_neb_next != primnum_neb) {
                        primnum_neb = primnum_neb_next;
                        break;
                    }
                    vtxnum_neb = vertexnext(1, vtxnum_neb);
                    if (vtxnum_neb == -1) {
                        error('cant be possible');
                        return;
                    }
                }
            }
            }



    const bool hasInputGroup = primGroupName.isstring() || pointGroupName.isstring() || vertexGroupName.isstring() || edgeGroupName.isstring();
    GA_VertexGroup* geo0VtxGroup = nullptr;
    GA_VertexGroupUPtr geo0vtxGroupUPtr;
    if (hasInputGroup)
    {
        geo0vtxGroupUPtr = tmpGeo0->createDetachedVertexGroup();
        geo0VtxGroup = geo0vtxGroupUPtr.get();
        if (primGroupName.isstring())
        {

        }

        if (pointGroupName.isstring())
        {

        }

        if (vertexGroupName.isstring())
        {

        }

        if (edgeGroupName.isstring())
        {

        }
    }

    const bool isClosed = sopPrimPolyIsClosed(sopparms.getPrimType());

    const exint subscribeRatio = sopparms.getSubscribeRatio();
    const exint minGrainSize = sopparms.getMinGrainSize();


    const GA_Storage inStorageI = GA_FeE_Type::getPreferredStorageI(outGeo0);


    UT_AutoInterrupt boss("Processing");
    if (boss.wasInterrupted())
        return;



    //
    //#if 1
    //    const GA_VertexGroupUPtr creatingGroupUPtr = tmpGeo0->createDetachedVertexGroup();
    //    GA_VertexGroup* creatingGroup = creatingGroupUPtr.get();
    //#else
    //    GA_VertexGroup* creatingGroup = outGeo0->newVertexGroup("creatingGroup");
    //#endif
    //
    //
    //
    //#if 1
    //    const GA_ATINumericUPtr vtxpnumAttribUPtr = tmpGeo0->createDetachedTupleAttribute(GA_ATTRIB_VERTEX, inStorageI, 1, GA_Defaults(0));
    //    GA_ATINumeric* vtxpnumATI = vtxpnumAttribUPtr.get();
    //
    //    const GA_ATINumericUPtr dstptAttribUPtr = tmpGeo0->createDetachedTupleAttribute(GA_ATTRIB_VERTEX, inStorageI, 1, GA_Defaults(0));
    //    GA_ATINumeric* dstptATI = dstptAttribUPtr.get();
    //#else
    //    GA_Attribute* vtxpnumATI = outGeo0->createTupleAttribute(GA_ATTRIB_VERTEX, "vtxpnum", inStorageI, 1, GA_Defaults(0));
    //#endif
    //
    //    GA_RWHandleT<GA_Size> vtxpnumAttribHandle;
    //    GA_RWHandleT<GA_Size> dstptAttribHandle;
    //    vtxpnumAttribHandle.bind(vtxpnumATI);
    //    dstptAttribHandle.bind(dstptATI);
    //
    //    GA_FeE_TopologyReference::vertexPrimIndex(tmpGeo0, vtxpnumAttribHandle, geo0VtxGroup);
    //
    //    GA_FeE_TopologyReference::vertexPointDst(tmpGeo0, dstptAttribHandle, vtxpnumAttribHandle, geo0VtxGroup);
    //
    //    //GA_FeE_VertexNextEquiv::vertexNextEquivNoLoop(tmpGeo0, vtxpnumAttribHandle, creatingGroup, dstptAttribHandle, geo0VtxGroup);
    //    ///////////// after this, vtxpnumAttribHandle is not vtxpnum anymore
    //
    //
    //    GA_FeE_VertexNextEquiv::vertexNextEquivNoLoop(tmpGeo0, creatingGroup, dstptAttribHandle, geo0VtxGroup);

    //const GA_VertexGroup* creatingGroup = GA_FeE_VertexNextEquiv::addGroupVertexNextEquivNoLoop(tmpGeo0, "__topo_nextEquivValid", geo0VtxGroup, subscribeRatio, minGrainSize);
    const GA_VertexGroup* creatingGroup = GA_FeE_VertexNextEquiv::addGroupVertexNextEquivNoLoop(tmpGeo0, geo0VtxGroup, inStorageI, "__topo_nextEquivValid", subscribeRatio, minGrainSize);
    //const GA_Attribute* dstptAttrib = outGeo0->findVertexAttribute("__topo_dstpt");
    const GA_RWHandleT<GA_Offset> dstptAttribH = tmpGeo0->findVertexAttribute(GA_FEE_TOPO_SCOPE, "__topo_dstpt");

    UT_ASSERT_P(dstptAttribH.getAttribute());


    GA_Size entries = creatingGroup->getGroupEntries();

    GA_Offset vtxoff_first;
    GA_Offset primoff_first = outGeo0->appendPrimitivesAndVertices(GA_PrimitiveTypeId(1), entries, 2, vtxoff_first, isClosed);


    GA_Topology& topo = outGeo0->getTopology();

    GA_Topology& topo_tmpGeo0 = tmpGeo0->getTopology();
    const GA_ATITopology* vtxPointRef_tmpGeo0 = topo_tmpGeo0.getPointRef();

    GA_Offset start, end;
    for (GA_Iterator it(tmpGeo0->getVertexRange(creatingGroup)); it.fullBlockAdvance(start, end); )
    {
        for (GA_Offset vtxoff = start; vtxoff < end; ++vtxoff)
        {
            topo.wireVertexPoint(vtxoff_first, vtxPointRef_tmpGeo0->getLink(vtxoff));
            ++vtxoff_first;
            topo.wireVertexPoint(vtxoff_first, dstptAttribH.get(vtxoff));
            ++vtxoff_first;
        }
    }

    outGeo0->bumpDataIdsForAddOrRemove(false, true, true);

    tmpGeoH0.deleteGdp();


}



namespace SOP_FeE_JoinCurve_1_0_Namespace {

} // End SOP_FeE_JoinCurve_1_0_Namespace namespace
