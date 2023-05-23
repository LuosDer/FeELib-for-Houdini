
//#define UT_ASSERT_LEVEL 3
#include "SOP_FeE_GroupByMeshWinding_1_0.h"


#include "SOP_FeE_GroupByMeshWinding_1_0.proto.h"

#include "GA/GA_Detail.h"
#include "PRM/PRM_TemplateBuilder.h"
#include "UT/UT_Interrupt.h"
#include "UT/UT_DSOVersion.h"



#include "GFE/GFE_GroupByMeshWinding.h"



using namespace SOP_FeE_GroupByMeshWinding_1_0_Namespace;



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
        name    "groupByMeshWindingMethod"
        cppname "GroupByMeshWindingMethod"
        label   "Method"
        type    ordinal
        default { "volume" }
        menu {
            "volume"      "Volume"
            "wn_convex"   "Winding Number Convex"
            "wn_bbox"     "Winding Number BBox"
        }
    }
    parm {
        name    "meshCap"
        cppname "MeshCap"
        label   "Mesh Cap"
        type    toggle
        default { "0" }
    }
    parm {
        name    "outGroup"
        cppname "OutGroup"
        label   "Out Group"
        type    toggle
        default { "0" }
    }
    parm {
        name    "outGroupType"
        cppname "OutGroupType"
        label   "Out Group Type"
        type    ordinal
        default { "prim" }
        disablewhen "{ outGroup == 0 }"
        menu {
            "prim"      "Primitive"
            "point"     "Point"
            "vertex"    "Vertex"
            "edge"      "Edge"
        }
    }
    parm {
        name    "outGroupName"
        cppname "OutGroupName"
        label   "OutGroup Name"
        type    string
        default { "meshWindingCorrect" }
        disablewhen "{ outGroup == 0 }"
    }
    parm {
        name    "outAttrib"
        cppname "OutAttrib"
        label   "Out Attrib"
        type    toggle
        default { "0" }
    }
    parm {
        name    "outAttribName"
        cppname "OutAttribName"
        label   "Out Attrib Name"
        type    string
        default { "meshWindingCorrect" }
        disablewhen "{ outAttrib == 0 }"
    }

    parm {
        name    "reverseGroup"
        cppname "ReverseGroup"
        label   "Reverse Group"
        type    toggle
        default { "0" }
    }
    parm {
        name    "reversePrim"
        cppname "ReversePrim"
        label   "Reverse Prim"
        type    toggle
        default { "0" }
    }

    //parm {
    //    name    "subscribeRatio"
    //    cppname "SubscribeRatio"
    //    label   "Subscribe Ratio"
    //    type    integer
    //    default { 16 }
    //    range   { 0! 256 }
    //    invisible
    //}
    //parm {
    //    name    "minGrainSize"
    //    cppname "MinGrainSize"
    //    label   "Min Grain Size"
    //    type    intlog
    //    default { 1024 }
    //    range   { 0! 2048 }
    //    invisible
    //}
}
)THEDSFILE";



PRM_Template*
SOP_FeE_GroupByMeshWinding_1_0::buildTemplates()
{
    static PRM_TemplateBuilder templ("SOP_FeE_GroupByMeshWinding_1_0.C"_sh, theDsFile);
    if (templ.justBuilt())
    {
        templ.setChoiceListPtr("primGroup"_sh, &SOP_Node::primGroupMenu);
    }
    return templ.templates();
}


const UT_StringHolder SOP_FeE_GroupByMeshWinding_1_0::theSOPTypeName("FeE::groupByMeshWinding::1.0"_sh);

void
newSopOperator(OP_OperatorTable* table)
{
    OP_Operator* newOp = new OP_Operator(
        SOP_FeE_GroupByMeshWinding_1_0::theSOPTypeName,
        "FeE Group by Mesh Winding",
        SOP_FeE_GroupByMeshWinding_1_0::myConstructor,
        SOP_FeE_GroupByMeshWinding_1_0::buildTemplates(),
        1,
        1,
        nullptr,
        OP_FLAG_GENERATOR,
        nullptr,
        1,
        "Five elements Elf/Data/Orient");

    newOp->setIconName("SOP_normal");
    table->addOperator(newOp);

}



//class SOP_FeE_GroupByMeshWinding_1_0Cache : public SOP_NodeCache
//{
//public:
//    SOP_FeE_GroupByMeshWinding_1_0Cache() : SOP_NodeCache(),
//        myPrevOutputDetailID(-1)
//    {}
//    ~SOP_FeE_GroupByMeshWinding_1_0Cache() override {}
//
//    int64 myPrevOutputDetailID;
//};


class SOP_FeE_GroupByMeshWinding_1_0Verb : public SOP_NodeVerb
{
public:
    SOP_FeE_GroupByMeshWinding_1_0Verb() {}
    virtual ~SOP_FeE_GroupByMeshWinding_1_0Verb() {}

    virtual SOP_NodeParms *allocParms() const { return new SOP_FeE_GroupByMeshWinding_1_0Parms(); }
    //virtual SOP_NodeCache* allocCache() const { return new SOP_FeE_GroupByMeshWinding_1_0Cache(); }
    virtual UT_StringHolder name() const { return SOP_FeE_GroupByMeshWinding_1_0::theSOPTypeName; }

    virtual CookMode cookMode(const SOP_NodeParms *parms) const { return COOK_GENERIC; }

    virtual void cook(const CookParms &cookparms) const;

    /// This static data member automatically registers
    /// this verb class at library ldir0d time.
    static const SOP_NodeVerb::Register<SOP_FeE_GroupByMeshWinding_1_0Verb> theVerb;
};

// The static member variable definition has to be outside the class definition.
// The declaration is inside the class.
const SOP_NodeVerb::Register<SOP_FeE_GroupByMeshWinding_1_0Verb> SOP_FeE_GroupByMeshWinding_1_0Verb::theVerb;

const SOP_NodeVerb *
SOP_FeE_GroupByMeshWinding_1_0::cookVerb() const 
{ 
    return SOP_FeE_GroupByMeshWinding_1_0Verb::theVerb.get();
}







static GA_GroupType
sopGroupType(SOP_FeE_GroupByMeshWinding_1_0Parms::OutGroupType parmGroupType)
{
    using namespace SOP_FeE_GroupByMeshWinding_1_0Enums;
    switch (parmGroupType)
    {
    case OutGroupType::PRIM:      return GA_GROUP_PRIMITIVE;  break;
    case OutGroupType::POINT:     return GA_GROUP_POINT;      break;
    case OutGroupType::VERTEX:    return GA_GROUP_VERTEX;     break;
    case OutGroupType::EDGE:      return GA_GROUP_EDGE;       break;
    }
    UT_ASSERT_MSG(0, "Unhandled geo0Group type!");
    return GA_GROUP_INVALID;
}



static GFE_GroupByMeshWindingMethod
sopMethod(SOP_FeE_GroupByMeshWinding_1_0Parms::GroupByMeshWindingMethod parmgrouptype)
{
    using namespace SOP_FeE_GroupByMeshWinding_1_0Enums;
    switch (parmgrouptype)
    {
    case GroupByMeshWindingMethod::VOLUME:     return GFE_GroupByMeshWindingMethod::VOLUME;                break;
    case GroupByMeshWindingMethod::WN_CONVEX:  return GFE_GroupByMeshWindingMethod::WINDINGNUMBER_CONVEX;  break;
    case GroupByMeshWindingMethod::WN_BBOX:    return GFE_GroupByMeshWindingMethod::WINDINGNUMBER_BBOX;    break;
    }
    UT_ASSERT_MSG(0, "Unhandled GFE_GroupByMeshWinding METHOD!");
    return GFE_GroupByMeshWindingMethod::VOLUME;
}



void
SOP_FeE_GroupByMeshWinding_1_0Verb::cook(const SOP_NodeVerb::CookParms& cookparms) const
{
    auto&& sopparms = cookparms.parms<SOP_FeE_GroupByMeshWinding_1_0Parms>();
    GA_Detail& outGeo0 = *cookparms.gdh().gdpNC();
    //auto sopcache = (SOP_FeE_GroupByMeshWinding_1_0Cache*)cookparms.cache();

    const GA_Detail& inGeo0 = *cookparms.inputGeo(0);
    const GA_Detail* const inGeo1 = cookparms.inputGeo(1);
    
    outGeo0.replaceWith(inGeo0);

    UT_AutoInterrupt boss("Processing");
    if (boss.wasInterrupted())
        return;

    const GFE_GroupByMeshWindingMethod method = sopMethod(sopparms.getGroupByMeshWindingMethod());

    
    GFE_GroupByMeshWinding groupByMeshWinding(outGeo0, inGeo1, cookparms);
    groupByMeshWinding.setComputeParm(method, sopparms.getReverseGroup(), sopparms.getReversePrim(), sopparms.getMeshCap());

    if (sopparms.getOutGroup())
    {
        const GA_GroupType outGroupType = sopGroupType(sopparms.getOutGroupType());
        groupByMeshWinding.getOutGroupArray().findOrCreate(false, outGroupType, sopparms.getOutGroupName());
    }
    
    if (sopparms.getOutAttrib())
    {
        groupByMeshWinding.findOrCreateTuple(false, sopparms.getOutAttribName());
    }
    
    groupByMeshWinding.groupParser.setPrimitiveGroup(sopparms.getPrimGroup());
    groupByMeshWinding.computeAndBumpDataId();
    groupByMeshWinding.visualizeOutGroup();

}
