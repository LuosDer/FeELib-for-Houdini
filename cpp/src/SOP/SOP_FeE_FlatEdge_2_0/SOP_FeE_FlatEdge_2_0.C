
//#define UT_ASSERT_LEVEL 3
#include "SOP_FeE_FlatEdge_2_0.h"


#include "SOP_FeE_FlatEdge_2_0.proto.h"

#include "GA/GA_Detail.h"
#include "PRM/PRM_TemplateBuilder.h"
#include "UT/UT_Interrupt.h"
#include "UT/UT_DSOVersion.h"


#include "GFE/GFE_FlatEdge.h"


using namespace SOP_FeE_FlatEdge_2_0_Namespace;



static const char *theDsFile = R"THEDSFILE(
{
    name        parameters
    parm {
        name    "group"
        cppname "Group"
        label   "Group"
        type    string
        default { "" }
        parmtag { "script_action" "import soputils\nkwargs['geometrytype'] = kwargs['node'].parmTuple('groupType')\nkwargs['inputindex'] = 0\nsoputils.selectGroupParm(kwargs)" }
        parmtag { "script_action_help" "Select geometry from an available viewport." }
        parmtag { "script_action_icon" "BUTTONS_reselect" }
    }
    parm {
        name    "groupType"
        cppname "GroupType"
        label   "Group Type"
        type    ordinal
        default { "guess" }
        menu {
            "guess"     "Guess from Group"
            "prim"      "Primitive"
            "point"     "Point"
            "vertex"    "Vertex"
            "edge"      "Edge"
        }
    }
    


    parm {
        name    "flatEdgeGroupName"
        cppname "FlatEdgeGroupName"
        label   "Flat Edge Group Name"
        type    string
        default { "flatEdge" }
    }
    parm {
        name    "flatEdgeAngleThreshold"
        cppname "FlatEdgeAngleThreshold"
        label   "Flat Edge Angle Threshold"
        type    angle
        default { "180" }
        range   { 0! 180! }
    }
    parm {
        name    "absoluteDot"
        cppname "AbsoluteDot"
        label   "Absolute Dot"
        type    toggle
        default { "0" }
    }
    parm {
        name    "includeUnsharedEdge"
        cppname "IncludeUnsharedEdge"
        label   "Include Unshared Edge"
        type    toggle
        default { "0" }
    }
    parm {
        name    "manifoldEdge"
        cppname "ManifoldEdge"
        label   "Manifold Edge"
        type    ordinal
        default { "none" }
        menu {
            "none"  "None"
            "all"   "all"
            "min"   "Min"
            "max"   "Max"
        }
    }

    parm {
        name    "normalAttribClass"
        cppname "NormalAttribClass"
        label   "Normal Attrib Class"
        type    ordinal
        default { "prim" }
        menu {
            "prim"      "Primitive"
            "vertex"    "Vertex"
        }
    }
    parm {
        name    "normalAttrib"
        cppname "NormalAttrib"
        label   "Normal Attrib"
        type    string
        default { "" }
    }
    parm {
        name    "weightingMethod"
        cppname "WeightingMethod"
        label   "Weighting Method"
        type    ordinal
        default { "angle" }
        menu {
            "uniform"   "Each Vertex Equally"
            "angle"     "By Vertex Angle"
            "area"      "By Face Area"
        }
        range   { 0! 2! }
    }

    parm {
        name    "outAsVertexGroup"
        cppname "OutAsVertexGroup"
        label   "Output as Vertex Group"
        type    toggle
        default { "0" }
    }

    parm {
        name    "reverseGroup"
        cppname "ReverseGroup"
        label   "Reverse Group"
        type    toggle
        default { "0" }
    }
    parm {
        name    "delGroupElement"
        cppname "DelGroupElement"
        label   "Delete Group Element"
        type    toggle
        default { "0" }
    }


    parm {
        name    "subscribeRatio"
        cppname "SubscribeRatio"
        label   "Subscribe Ratio"
        type    integer
        default { 16 }
        range   { 0! 256 }
    }
    parm {
        name    "minGrainSize"
        cppname "MinGrainSize"
        label   "Min Grain Size"
        type    intlog
        default { 1024 }
        range   { 0! 2048 }
    }

}
)THEDSFILE";



PRM_Template*
SOP_FeE_FlatEdge_2_0::buildTemplates()
{
    static PRM_TemplateBuilder templ("SOP_FeE_FlatEdge_2_0.C"_sh, theDsFile);
    if (templ.justBuilt())
    {
        templ.setChoiceListPtr("group"_sh, &SOP_Node::allGroupMenu);
        templ.setChoiceListPtr("normalAttrib"_sh, &SOP_Node::allAttribMenu);
    }
    return templ.templates();
}


const UT_StringHolder SOP_FeE_FlatEdge_2_0::theSOPTypeName("FeE::flatEdge::2.0"_sh);

void
newSopOperator(OP_OperatorTable* table)
{
    OP_Operator* newOp = new OP_Operator(
        SOP_FeE_FlatEdge_2_0::theSOPTypeName,
        "FeE Flat Edge",
        SOP_FeE_FlatEdge_2_0::myConstructor,
        SOP_FeE_FlatEdge_2_0::buildTemplates(),
        1,
        1,
        nullptr,
        OP_FLAG_GENERATOR,
        nullptr,
        1,
        "Five elements Elf/Data/Topology");

    newOp->setIconName("SOP_dissolve-2.0");
    table->addOperator(newOp);

}


//class SOP_FeE_FlatEdge_2_0Cache : public SOP_NodeCache
//{
//public:
//    SOP_FeE_FlatEdge_2_0Cache() : SOP_NodeCache(),
//        myPrevOutputDetailID(-1)
//    {}
//    ~SOP_FeE_FlatEdge_2_0Cache() override {}
//
//    int64 myPrevOutputDetailID;
//};


class SOP_FeE_FlatEdge_2_0Verb : public SOP_NodeVerb
{
public:
    SOP_FeE_FlatEdge_2_0Verb() {}
    virtual ~SOP_FeE_FlatEdge_2_0Verb() {}

    virtual SOP_NodeParms *allocParms() const { return new SOP_FeE_FlatEdge_2_0Parms(); }
    //virtual SOP_NodeCache* allocCache() const { return new SOP_FeE_FlatEdge_2_0Cache(); }
    virtual UT_StringHolder name() const { return SOP_FeE_FlatEdge_2_0::theSOPTypeName; }

    virtual CookMode cookMode(const SOP_NodeParms *parms) const { return COOK_GENERIC; }

    virtual void cook(const CookParms &cookparms) const;

    /// This static data member automatically registers
    /// this verb class at library ldir0d time.
    static const SOP_NodeVerb::Register<SOP_FeE_FlatEdge_2_0Verb> theVerb;
};

// The static member variable definition has to be outside the class definition.
// The declaration is inside the class.
const SOP_NodeVerb::Register<SOP_FeE_FlatEdge_2_0Verb> SOP_FeE_FlatEdge_2_0Verb::theVerb;

const SOP_NodeVerb *
SOP_FeE_FlatEdge_2_0::cookVerb() const 
{ 
    return SOP_FeE_FlatEdge_2_0Verb::theVerb.get();
}







static GA_GroupType
sopGroupType(SOP_FeE_FlatEdge_2_0Parms::GroupType parmGroupType)
{
    using namespace SOP_FeE_FlatEdge_2_0Enums;
    switch (parmGroupType)
    {
    case GroupType::GUESS:     return GA_GROUP_INVALID;    break;
    case GroupType::PRIM:      return GA_GROUP_PRIMITIVE;  break;
    case GroupType::POINT:     return GA_GROUP_POINT;      break;
    case GroupType::VERTEX:    return GA_GROUP_VERTEX;     break;
    case GroupType::EDGE:      return GA_GROUP_EDGE;       break;
    }
    UT_ASSERT_MSG(0, "Unhandled geo0Group type!");
    return GA_GROUP_INVALID;
}


static GA_GroupType
sopManifoldEdge(SOP_FeE_FlatEdge_2_0Parms::ManifoldEdge parmgrouptype)
{
    using namespace SOP_FeE_FlatEdge_2_0Enums;
    switch (parmgrouptype)
    {
    case ManifoldEdge::NONE:    return GA_GROUP_VERTEX;     break;
    case ManifoldEdge::ALL:     return GA_GROUP_INVALID;    break;
    case ManifoldEdge::MIN:     return GA_GROUP_POINT;      break;
    case ManifoldEdge::MAX:     return GA_GROUP_PRIMITIVE;  break;
    }
    UT_ASSERT_MSG(0, "Unhandled Manifold Edge!");
    return GA_GROUP_INVALID;
}


static GA_AttributeOwner
sopNormalAttribClass(SOP_FeE_FlatEdge_2_0Parms::NormalAttribClass parmgrouptype)
{
    using namespace SOP_FeE_FlatEdge_2_0Enums;
    switch (parmgrouptype)
    {
    case NormalAttribClass::VERTEX:    return GA_ATTRIB_VERTEX;    break;
    case NormalAttribClass::PRIM:      return GA_ATTRIB_PRIMITIVE; break;
    }
    UT_ASSERT_MSG(0, "Unhandled Manifold Edge!");
    return GA_ATTRIB_INVALID;
}

static GEO_NormalMethod
sopWeightingMethod(SOP_FeE_FlatEdge_2_0Parms::WeightingMethod parmNormalMethod)
{
    using namespace SOP_FeE_FlatEdge_2_0Enums;
    switch (parmNormalMethod)
    {
    case WeightingMethod::VERTEX:    return GEO_NormalMethod::UNIFORM_WEIGHTED;    break;
    case WeightingMethod::PRIM:      return GEO_NormalMethod::ANGLE_WEIGHTED;      break;
    case WeightingMethod::PRIM:      return GEO_NormalMethod::AREA_WEIGHTED;       break;
    }
    UT_ASSERT_MSG(0, "Unhandled Manifold Edge!");
    return GEO_NormalMethod::ANGLE_WEIGHTED;
}

void
SOP_FeE_FlatEdge_2_0Verb::cook(const SOP_NodeVerb::CookParms &cookparms) const
{
    auto &&sopparms = cookparms.parms<SOP_FeE_FlatEdge_2_0Parms>();
    GA_Detail& outGeo0 = *cookparms.gdh().gdpNC();
    //auto sopcache = (SOP_FeE_FlatEdge_2_0Cache*)cookparms.cache();

    const GA_Detail& inGeo0 = *cookparms.inputGeo(0);

    outGeo0.replaceWith(inGeo0);


    const UT_StringHolder& flatEdgeGroupName = sopparms.getFlatEdgeGroupName();
    
    if (!flatEdgeGroupName.isstring())
        return;


    const GA_GroupType groupType = sopGroupType(sopparms.getGroupType());
    const GA_GroupType manifoldEdge = sopManifoldEdge(sopparms.getManifoldEdge());
    const GA_AttributeOwner normalAttribClass = sopNormalAttribClass(sopparms.getNormalAttribClass());
        
    
    UT_AutoInterrupt boss("Processing");
    if (boss.wasInterrupted())
        return;

    GFE_FlatEdge flatEdge(outGeo0, &cookparms);

    
    flatEdge.setComputeParm(sopparms.getFlatEdgeAngleThreshold(), sopparms.getAbsoluteDot(),
        sopparms.getIncludeUnsharedEdge(), manifoldEdge,
        sopparms.getOutAsVertexGroup(), sopparms.getSubscribeRatio(), sopparms.getMinGrainSize());

    

    flatEdge.normal3D.normalMethod = sopWeightingMethod(sopparms.getWeightingMethod());
    flatEdge.normal3D.getOutAttribArray().findOrCreateNormal3D(normalAttribClass, sopparms.getNormalAttrib());

    flatEdge.reverseOutGroup = sopparms.getReverseGroup();
    flatEdge.doDelGroupElement = sopparms.getDelGroupElement();

    
    flatEdge.groupParser.setGroup(groupType, sopparms.getGroup());
    flatEdge.findOrCreateGroup(flatEdgeGroupName);
    
    
    flatEdge.computeAndBumpDataId();
    flatEdge.delOrVisualizeGroup();


}


