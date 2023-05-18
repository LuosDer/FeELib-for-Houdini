
//#define UT_ASSERT_LEVEL 3
#include "SOP_FeE_UVRectify_2_0.h"
#include "SOP_FeE_UVRectify_2_0.proto.h"


#include "GA/GA_Detail.h"
#include "PRM/PRM_TemplateBuilder.h"
#include "UT/UT_Interrupt.h"
#include "UT/UT_DSOVersion.h"


#include "GU/GU_Flatten2.h"
#include "GU/GU_UVPack.h"


#include "GFE/GFE_GroupParser.h"


using namespace SOP_FeE_UVRectify_2_0_Namespace;



static const char* theDsFile = R"THEDSFILE(
{
    name        parameters
    parm {
        name    "group"
        cppname "Group"
        label   "Group"
        type    string
        default { "" }
        parmtag { "script_action" "import soputils\nkwargs['geometrytype'] = (hou.geometryType.Primitives,)\nkwargs['inputindex'] = 0\nkwargs['ordered'] = pwd.parm('ordered').eval()\nsoputils.selectGroupParm(kwargs)" }
        parmtag { "script_action_help" "Select geometry from an available viewport.\nShift-click to turn on Select Groups." }
        parmtag { "script_action_icon" "BUTTONS_reselect" }
    }
    parm {
        name    "uvAttrib"
        cppname "UVAttrib"
        label   "UV Attribute"
        type    string
        default { "uv" }
    }
    parm {
        name    "seamGroup"
        cppname "SeamGroup"
        label   "Seam Group"
        type    string
        default { "" }
        parmtag { "script_action" "import soputils\nkwargs['geometrytype'] = (hou.geometryType.Edges,)\nkwargs['inputindex'] = 0\nsoputils.selectGroupParm(kwargs)" }
        parmtag { "script_action_help" "Select geometry from an available viewport.\nShift-click to turn on Select Groups." }
        parmtag { "script_action_icon" "BUTTONS_reselect" }
    }
    parm {
        name    "flattenMethod"
        cppname "FlattenMethod"
        label   "Flatten Method"
        type    ordinal
        default { "abf" }
        menu {
            "scp"   "Spectral (SCP)"
            "abf"   "Angle-Based (ABF)"
        }
    }





    parm {
        name    "straightenArcs"
        cppname "StraightenArcs"
        label   "Straighten Arcs"
        type    toggle
        default { "on" }
        disablewhen "{ flattenMethod != abf }"
    }
    parm {
        name    "straightenGrids"
        cppname "StraightenGrids"
        label   "Straighten Grids"
        type    toggle
        default { "on" }
        disablewhen "{ flattenMethod != abf }"
    }
    parm {
        name    "rectifyPatches"
        cppname "RectifyPatches"
        label   "Rectify Patches"
        type    toggle
        default { "on" }
        disablewhen "{ flattenMethod != abf }"
    }



    parm {
        name    "layout"
        cppname "Layout"
        label   "Layout"
        type    toggle
        default { "on" }
    }
}
)THEDSFILE";

PRM_Template*
SOP_FeE_UVRectify_2_0::buildTemplates()
{
    static PRM_TemplateBuilder templ("SOP_FeE_UVRectify_2_0.C"_sh, theDsFile);
    if (templ.justBuilt())
    {
        templ.setChoiceListPtr("group"_sh, &SOP_Node::vertexNamedGroupMenu);
        templ.setChoiceListPtr("uvAttrib"_sh, &SOP_Node::allAttribReplaceMenu);
    }
    return templ.templates();
}

const UT_StringHolder SOP_FeE_UVRectify_2_0::theSOPTypeName("FeE::uvRectify::0.5"_sh);

void
newSopOperator(OP_OperatorTable* table)
{
    OP_Operator* newOp = new OP_Operator(
        SOP_FeE_UVRectify_2_0::theSOPTypeName,
        "FeE UV Rectify",
        SOP_FeE_UVRectify_2_0::myConstructor,
        SOP_FeE_UVRectify_2_0::buildTemplates(),
        1,
        1,
        nullptr,
        OP_FLAG_GENERATOR,
        nullptr,
        1,
        "Five elements Elf/2D");

    newOp->setIconName("SOP_uvflatten-3.0");
    table->addOperator(newOp);
}

class SOP_FeE_UVRectify_2_0Verb : public SOP_NodeVerb
{
public:
    SOP_FeE_UVRectify_2_0Verb() {}
    virtual ~SOP_FeE_UVRectify_2_0Verb() {}

    virtual SOP_NodeParms* allocParms() const { return new SOP_FeE_UVRectify_2_0Parms(); }
    virtual UT_StringHolder name() const { return SOP_FeE_UVRectify_2_0::theSOPTypeName; }

    virtual CookMode cookMode(const SOP_NodeParms* parms) const { return COOK_GENERIC; }

    virtual void cook(const CookParms& cookparms) const;

    static const SOP_NodeVerb::Register<SOP_FeE_UVRectify_2_0Verb> theVerb;
};

const SOP_NodeVerb::Register<SOP_FeE_UVRectify_2_0Verb> SOP_FeE_UVRectify_2_0Verb::theVerb;

const SOP_NodeVerb*
SOP_FeE_UVRectify_2_0::cookVerb() const
{
    return SOP_FeE_UVRectify_2_0Verb::theVerb.get();
}




static bool
sopFlattenMethod(SOP_FeE_UVRectify_2_0Parms::FlattenMethod flattenMethod)
{
    using namespace SOP_FeE_UVRectify_2_0Enums;
    switch (flattenMethod)
    {
    case FlattenMethod::SCP:      return false;  break;
    case FlattenMethod::ABF:      return true;      break;
    }
    UT_ASSERT_MSG(0, "Unhandled Geo0 Class type!");
    return false;
}



void
SOP_FeE_UVRectify_2_0Verb::cook(const SOP_NodeVerb::CookParms &cookparms) const
{
    auto &&sopparms = cookparms.parms<SOP_FeE_UVRectify_2_0Parms>();
    GA_Detail& outGeo0 = *cookparms.gdh().gdpNC();

    const GA_Detail& inGeo0 = *cookparms.inputGeo(0);

    outGeo0.replaceWith(inGeo0);



    const UT_StringHolder& geo0AttribNames = sopparms.getUVAttrib();
    if (!geo0AttribNames.isstring() || geo0AttribNames.length()==0)
        return;

    //GA_Attribute* uvAttribPtr = GFE_Attribute::findOrCreateUVAttributePointVertex(geo, uvAttribClass, uvAttribName);
    GA_Attribute* geo0Attrib = outGeo0->findAttribute(GA_ATTRIB_VERTEX, geo0AttribNames);
    if (!geo0Attrib)
        geo0Attrib = outGeo0->findAttribute(GA_ATTRIB_VERTEX, geo0AttribNames);
    if (!geo0Attrib)
        geo0Attrib = outGeo0->getAttributes().createTupleAttribute(GA_ATTRIB_VERTEX, geo0AttribNames, inStorageF, 3, GA_Defaults(0));

    //const fpreal uniScale = sopparms.getUniScale();
    //const bool doNormalize = sopparms.getNormalize();


    GOP_Manager gop;
    const GA_Group* const geo0Group = GFE_GroupParser_Namespace::findOrParsePrimitiveGroupDetached(cookparms, outGeo0, sopparms.getGroup(), gop);
    if (geo0Group && geo0Group->isEmpty())
        return;

    GA_VertexGroupUPtr allVtxGroupUPtr;
    GA_VertexGroup* allVtxGroup;
    if (geo0Group)
    {
        allVtxGroupUPtr = outGeo0->createDetachedVertexGroup();
        allVtxGroup = allVtxGroupUPtr.get();
        allVtxGroup->combine(geo0Group);
    }
    else
    {
        allVtxGroup = nullptr;
    }




    const GA_EdgeGroup* const seamGroup = GFE_GroupParser_Namespace::findOrParseEdgeGroupDetached(cookparms, outGeo0, sopparms.getSeamGroup(), gop);
    


    const GA_AttributeOwner geo0AttribClass = geo0Attrib->getOwner();
    const bool layoutit = sopparms.getLayout();
    const bool flattenMethod = sopFlattenMethod(sopparms.getFlattenMethod());

    GA_RWHandleT<UT_Vector3R> uv_h(geo0Attrib);

    const bool straightenArcs = sopparms.getStraightenArcs();
    const bool straightenGrids = sopparms.getStraightenGrids();
    const bool rectifyPatches = sopparms.getRectifyPatches();

#if 1
    GU_Flatten2::IslandBundle islandBundle(outGeo0, nullptr, seamGroup, nullptr);
    islandBundle.forEachIsland([uv_h, outGeo0, layoutit, flattenMethod, straightenArcs, straightenGrids, rectifyPatches](GU_Flatten2::Island& island) {

        GU_Flatten2::ConstraintSet constraints;
        //island.triangualte(seamGroup);

        GU_Flatten2::generateQuadLayoutConstraints(island, straightenArcs, straightenGrids, rectifyPatches, constraints);


#if 0
        GU_Flatten2::ConstraintSet::AlignedGroupSet& alignGroups = constraints.alignedSets();
        GU_Flatten2::ConstraintSet::StraightGroupSet& straightGroups = constraints.straightSets();
#endif



#if 0
        GU_Flatten2::balanceIsland(island, uv_h);

        GU_Flatten2::AngleConstraintArray angle_constraints;
        GU_Flatten2::generateStraighLoopConstraints(island, constraints, angle_constraints);
#endif

#if 0
        GA_Attribute* geo0AttribNew = outGeo0->getAttributes().createTupleAttribute(GA_ATTRIB_VERTEX, "newUV", inStorageF, 3, GA_Defaults(0));
        GA_RWHandleT<UT_Vector3R> uvNew_h(geo0AttribNew);

        const bool use_custom_pins = true;
        GU_Flatten2::repositionIsland(island, constraints, uv_h, uvNew_h, use_custom_pins);
#endif


#if 1
        if (flattenMethod)
        {
            //GU_Flatten2::Status status = GU_Flatten2::flattenAngleBased(island, constraints, uv_h);
            GU_Flatten2::flattenAngleBased(island, constraints, uv_h);
        }
        else
        {
#if 0
            UT_Fpreal64Array opposite_angle_cotan;
            UT_Fpreal64Array tri_area;
            GU_Flatten2::calcAnglesAndAreas(island, opposite_angle_cotan, tri_area);
            GU_Flatten2::flattenSpectral(island, constraints, opposite_angle_cotan, tri_area, uv_h);
#else
            GU_Flatten2::flattenIsland(GU_Flatten2::Method::SCP, island, constraints, uv_h);
            //GU_Flatten2::flattenSpectral(island, constraints, uv_h);
#endif
        }
#endif

    }, true);
#else

    GU_Flatten2::Island island(outGeo0);
    GU_Flatten2::ConstraintSet constraints;

    GA_Offset start, end;
    for (GA_Iterator it(outGeo0->getPrimitiveRange()); it.fullBlockAdvance(start, end); )
    {
        for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
        {
            island.appendPoly(elemoff);
        }
    }
    island.triangualte(seamGroup);

    const bool straighten_arcs = true;
    const bool straighten_grids = true;
    const bool rectify_patches = true;
    GU_Flatten2::generateQuadLayoutConstraints(island, straighten_arcs, straighten_grids, rectify_patches, constraints);


#if 0
    GU_Flatten2::ConstraintSet::AlignedGroupSet& alignGroups = constraints.alignedSets();
    GU_Flatten2::ConstraintSet::StraightGroupSet& straightGroups = constraints.straightSets();
#endif

    

#if 0
    GU_Flatten2::balanceIsland(island, uv_h);

    GU_Flatten2::AngleConstraintArray angle_constraints;
    GU_Flatten2::generateStraighLoopConstraints(island, constraints, angle_constraints);
#endif

#if 0
    GA_Attribute* geo0AttribNew = outGeo0->getAttributes().createTupleAttribute(GA_ATTRIB_VERTEX, "newUV", inStorageF, 3, GA_Defaults(0));
    GA_RWHandleT<UT_Vector3R> uvNew_h(geo0AttribNew);

    const bool use_custom_pins = true;
    GU_Flatten2::repositionIsland(island, constraints, uv_h, uvNew_h, use_custom_pins);
#endif


#if 1
    if (flattenMethod)
    {
        //GU_Flatten2::Status status = GU_Flatten2::flattenAngleBased(island, constraints, uv_h);
        GU_Flatten2::flattenAngleBased(island, constraints, uv_h);
    }
    else
    {
#if 0
        UT_Fpreal64Array opposite_angle_cotan;
        UT_Fpreal64Array tri_area;
        GU_Flatten2::calcAnglesAndAreas(island, opposite_angle_cotan, tri_area);
        GU_Flatten2::flattenSpectral(island, constraints, opposite_angle_cotan, tri_area, uv_h);
#else
        GU_Flatten2::flattenIsland(GU_Flatten2::Method::SCP, island, constraints, uv_h);
        //GU_Flatten2::flattenSpectral(island, constraints, uv_h);
#endif
    }
#endif

#endif


    if (layoutit)
    {
        const GA_PrimitiveGroup* islandGroup = nullptr;
        int resolution = 1024;
        int padding = 0;
        bool pad_boundary = false;
        bool correct_area_proprtions = false;
        bool axis_align_islands = true;
        bool repack_wasted = true;

        GU_UVPack uvPack(outGeo0, islandGroup,
            resolution, padding,
            correct_area_proprtions, axis_align_islands, repack_wasted);

        uvPack.tilePack();
    }

    geo0Attrib->bumpDataId();






}

