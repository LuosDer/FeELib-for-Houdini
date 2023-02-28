
//#define UT_ASSERT_LEVEL 3
#include "SOP_FeE_MatchDirOrient_1_0.h"


#include "SOP_FeE_MatchDirOrient_1_0.proto.h"

#include "GA/GA_Detail.h"
#include "PRM/PRM_TemplateBuilder.h"
#include "UT/UT_Interrupt.h"
#include "UT/UT_DSOVersion.h"




#include "GFE/GFE_MatchDirOrient.h"




using namespace SOP_FeE_MatchDirOrient_1_0_Namespace;


static const char *theDsFile = R"THEDSFILE(
{
    name        parameters
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
        name    "promoteEdgeGroupToPrim"
        cppname "PromoteEdgeGroupToPrim"
        label   "Promote Edge Group to Prim"
        type    string
        default { "" }
    }



    parm {
        name    "outSrcPrims"
        cppname "OutSrcPrims"
        label   "Source Prims"
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
        name    "excludeSharedEdge"
        cppname "ExcludeSharedEdge"
        label   "Exclude Shared Edge"
        type    toggle
        default { "0" }
    }
    parm {
        name    "close"
        cppname "Close"
        label   "Close"
        type    toggle
        default { "0" }
    }
    parm {
        name    "useEndGroup"
        cppname "UseEndGroup"
        label   "Use End Group"
        type    toggle
        default { "0" }
    }
    parm {
        name    "endGroup"
        cppname "EndGroup"
        label   "End Group"
        type    string
        default { "end" }
        disablewhen "{ useEndGroup == 0 }"
    }

)THEDSFILE"
// ==== This is necessary because MSVC++ has a limit of 16380 character per
// ==== string literal
R"THEDSFILE(

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
        name    "mergeInput"
        cppname "MergeInput"
        label   "Merge Input"
        type    toggle
        default { "0" }
        disablewhen "{ close == 1 }"
    }


    parm {
        name    "correctGeoWinding"
        cppname "CorrectGeoWinding"
        label   "Correct Geo Winding"
        type    toggle
        default { "0" }
    }
    parm {
        name    "reverse"
        cppname "Reverse"
        label   "Reverse"
        type    toggle
        default { "0" }
    }
    parm {
        name    "meshCap"
        cppname "MeshCap"
        label   "Mesh Cap"
        type    toggle
        default { "0" }
    }

    parm {
        name    "addUV"
        cppname "AddUV"
        label   "Add UV"
        type    toggle
        default { "0" }
    }
    groupsimple {
        name    "uv_folder"
        label   "UV"
        disablewhen "{ addUV == 0 }"

        parm {
            name    "uvAttribName"
            cppname "UVAttribName"
            label   "UV Attribute Name"
            type    string
            default { "uv" }
        }
        parm {
            name    "uvSize"
            cppname "UVSize"
            label   "UV Size"
            type    integer
            default { "3" }
            range   { 1! 3! }

        }
        parm {
            name    "uvMethod"
            cppname "UVMethod"
            label   "UV Method"
            type    ordinal
            default { "uniform" }
            menu {
                "uniform"   "Uniform"
                "length"    "Length"
            }
        }
        parm {
            name    "seamGroup"
            cppname "SeamGroup"
            label   "Seam Group"
            type    string
            default { "seams" }
            disablewhen "{ uvMethod != uniform }"
        }
        parm {
            name    "uvLayout"
            cppname "UVLayout"
            label   "UV Layout"
            type    toggle
            default { "0" }
        }
    }



    parm {
        name    "outTopoAttrib"
        cppname "OutTopoAttrib"
        label   "Output Topo Attribute"
        type    toggle
        default { "0" }
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
SOP_FeE_MatchDirOrient_1_0::buildTemplates()
{
    static PRM_TemplateBuilder templ("SOP_FeE_MatchDirOrient_1_0.C"_sh, theDsFile);
    if (templ.justBuilt())
    {
        //templ.setChoiceListPtr("primGroup"_sh, &SOP_Node::primGroupMenu);
        //templ.setChoiceListPtr("pointGroup"_sh, &SOP_Node::pointGroupMenu);
        //templ.setChoiceListPtr("vertexGroup"_sh, &SOP_Node::vertexNamedGroupMenu);
        //templ.setChoiceListPtr("edgeGroup"_sh, &SOP_Node::edgeGroupMenu);

        //templ.setChoiceListPtr("attribFromPrim"_sh, &SOP_Node::primAttribMenu);
        //templ.setChoiceListPtr("attribFromVertex"_sh, &SOP_Node::vertexAttribMenu);

        //templ.setChoiceListPtr("groupFromPrim"_sh, &SOP_Node::primGroupMenu);
        //templ.setChoiceListPtr("groupFromVertex"_sh, &SOP_Node::vertexNamedGroupMenu);
        //templ.setChoiceListPtr("groupFromEdge"_sh, &SOP_Node::edgeGroupMenu);
        //templ.setChoiceListPtr("promoteEdgeGroupToPrim"_sh, &SOP_Node::edgeGroupMenu);
        templ.setChoiceListPtr("seamGroup"_sh, &SOP_Node::edgeGroupMenu);
        
        templ.setChoiceListPtr("uvAttribName"_sh, &SOP_Node::allTextureCoordMenu);
        
    }
    return templ.templates();
}

const UT_StringHolder SOP_FeE_MatchDirOrient_1_0::theSOPTypeName("FeE::matchDirOrient::1.0"_sh);

void
newSopOperator(OP_OperatorTable* table)
{
    OP_Operator* newOp = new OP_Operator(
        SOP_FeE_MatchDirOrient_1_0::theSOPTypeName,
        "FeE Match Dir Orient",
        SOP_FeE_MatchDirOrient_1_0::myConstructor,
        SOP_FeE_MatchDirOrient_1_0::buildTemplates(),
        1,
        2,
        nullptr,
        OP_FLAG_GENERATOR,
        nullptr,
        1,
        "Five elements Elf/Data/Orient");

    newOp->setIconName("SOP_matchaxis");
    table->addOperator(newOp);

}





class SOP_FeE_MatchDirOrient_1_0Verb : public SOP_NodeVerb
{
public:
    SOP_FeE_MatchDirOrient_1_0Verb() {}
    virtual ~SOP_FeE_MatchDirOrient_1_0Verb() {}

    virtual SOP_NodeParms *allocParms() const { return new SOP_FeE_MatchDirOrient_1_0Parms(); }
    virtual UT_StringHolder name() const { return SOP_FeE_MatchDirOrient_1_0::theSOPTypeName; }

    virtual CookMode cookMode(const SOP_NodeParms *parms) const { return COOK_GENERIC; }

    virtual void cook(const CookParms &cookparms) const;
    
    /// This static data member automatically registers
    /// this verb class at library load time.
    static const SOP_NodeVerb::Register<SOP_FeE_MatchDirOrient_1_0Verb> theVerb;
};

// The static member variable definition has to be outside the class definition.
// The declaration is inside the class.
const SOP_NodeVerb::Register<SOP_FeE_MatchDirOrient_1_0Verb> SOP_FeE_MatchDirOrient_1_0Verb::theVerb;

const SOP_NodeVerb *
SOP_FeE_MatchDirOrient_1_0::cookVerb() const 
{ 
    return SOP_FeE_MatchDirOrient_1_0Verb::theVerb.get();
}




static bool
sopPrimPolyIsClosed(SOP_FeE_MatchDirOrient_1_0Parms::PrimType parmgrouptype)
{
    using namespace SOP_FeE_MatchDirOrient_1_0Enums;
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
SOP_FeE_MatchDirOrient_1_0Verb::cook(const SOP_NodeVerb::CookParms &cookparms) const
{
    auto&& sopparms = cookparms.parms<SOP_FeE_MatchDirOrient_1_0Parms>();
    GA_Detail* const outGeo0 = cookparms.gdh().gdpNC();
    //auto sopcache = (SOP_FeE_MatchDirOrient_1_0Cache*)cookparms.cache();

    const GA_Detail* const inGeo0 = cookparms.inputGeo(0);

    outGeo0->replaceWith(*inGeo0);


    GU_DetailHandle geoTmp_h;
    GU_Detail* geoTmp = new GU_Detail();
    geoTmp_h.allocateAndSet(geoTmp);
    geoTmp->replaceWith(*inGeo0);


    //outGeo0 = sopNodeProcess(*inGeo0);

    const UT_StringHolder& primGroupName = sopparms.getPrimGroup();
    const UT_StringHolder& pointGroupName = sopparms.getPointGroup();
    const UT_StringHolder& vertexGroupName = sopparms.getVertexGroup();
    const UT_StringHolder& edgeGroupName = sopparms.getEdgeGroup();


    const bool hasInputGroup = primGroupName.isstring() || pointGroupName.isstring() || vertexGroupName.isstring() || edgeGroupName.isstring();
    GA_VertexGroup* geo0VtxGroup = nullptr;
    GA_VertexGroupUPtr geo0vtxGroupUPtr;
    if (hasInputGroup)
    {
        geo0vtxGroupUPtr = geoTmp->createDetachedVertexGroup();
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


    const exint subscribeRatio = sopparms.getSubscribeRatio();
    const exint minGrainSize = sopparms.getMinGrainSize();

    const UT_StringHolder& seamGroupName = sopparms.getSeamGroup();
    const UT_StringHolder& uvAttribName = sopparms.getUVAttribName();

    const GA_Storage inStorageI = GFE_Type::getPreferredStorageI(outGeo0);

    UT_AutoInterrupt boss("Processing");
    if (boss.wasInterrupted())
        return;

    GFE_Skin::skin(outGeo0, true);

    outGeo0->bumpDataIdsForAddOrRemove(false, true, true);


    GFE_TopologyReference::outTopoAttrib(outGeo0, sopparms.getOutTopoAttrib());

}



namespace SOP_FeE_MatchDirOrient_1_0_Namespace {

} // End SOP_FeE_MatchDirOrient_1_0_Namespace namespace
