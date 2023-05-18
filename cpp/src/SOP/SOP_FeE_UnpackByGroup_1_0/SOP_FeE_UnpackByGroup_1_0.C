
//#define UT_ASSERT_LEVEL 3
#include "SOP_FeE_UnpackByGroup_1_0.h"


#include "SOP_FeE_UnpackByGroup_1_0.proto.h"

#include "GA/GA_Detail.h"
#include "PRM/PRM_TemplateBuilder.h"
#include "UT/UT_Interrupt.h"
#include "UT/UT_DSOVersion.h"


#include "GFE/GFE_UnpackByGroup.h"



using namespace SOP_FeE_UnpackByGroup_1_0_Namespace;


static const char *theDsFile = R"THEDSFILE(
{
    name	parameters
    parm {
        name    "iterationMethod"
        cppname "IterationMethod"
        label   "Iteration Method"
        type    ordinal
        default { "oneElem" }
        menu {
            "custom"    "Custom"
            "oneElem"   "One Elem"
            "skipNElem" "Skip N Elem"
        }
    }
    parm {
        name    "group"
        cppname "Group"
        label   "Group"
        type    string
        default { "" }
        hidewhen "{ iterationMethod != custom }"
        parmtag { "script_action" "import soputils\nkwargs['geometrytype'] = kwargs['node'].parmTuple('groupType')\nkwargs['inputindex'] = 0\nsoputils.selectGroupParm(kwargs)" }
        parmtag { "script_action_help" "Select geometry from an available viewport.\nShift-click to turn on Select Groups." }
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
        name    "primoff"
        cppname "Primoff"
        label   "Primoff"
        type    integer
        default { "0" }
        hidewhen "{ iterationMethod == custom }"
        range   { 0! 10 }
    }
    parm {
        name    "inputAsOffset"
        cppname "InputAsOffset"
        label   "Input as Offset"
        type    toggle
        default { "0" }
        hidewhen "{ iterationMethod == custom }"
    }
    parm {
        name    "skipNPrim"
        cppname "SkipNPrim"
        label   "Skip N Prim"
        type    integer
        default { "1" }
        hidewhen "{ iterationMethod != skipNElem }"
        range   { 0! 10 }
    }
    parm {
        name    "reverseGroup"
        cppname "ReverseGroup"
        label   "Delete Non Selected"
        type    toggle
        default { "1" }
    }
    parm {
        name    "delGroup"
        cppname "DelGroup"
        label   "Delete Group"
        type    toggle
        default { "1" }
    }

}
)THEDSFILE";

PRM_Template*
SOP_FeE_UnpackByGroup_1_0::buildTemplates()
{
    static PRM_TemplateBuilder templ("SOP_FeE_UnpackByGroup_1_0.C"_sh, theDsFile);
    if (templ.justBuilt())
    {
        templ.setChoiceListPtr("delElementGroup"_sh, &SOP_Node::allGroupMenu);
    }
    return templ.templates();
}





const UT_StringHolder SOP_FeE_UnpackByGroup_1_0::theSOPTypeName("FeE::delAndUnpack::1.0"_sh);

void
newSopOperator(OP_OperatorTable* table)
{
    OP_Operator* newOp = new OP_Operator(
        SOP_FeE_UnpackByGroup_1_0::theSOPTypeName,
        "FeE Delete and Unpack",
        SOP_FeE_UnpackByGroup_1_0::myConstructor,
        SOP_FeE_UnpackByGroup_1_0::buildTemplates(),
        1,
        1,
        nullptr,
        OP_FLAG_GENERATOR,
        nullptr,
        1,
        "Five elements Elf/Operation/Blast");

    newOp->setIconName("SOP_delete");
    table->addOperator(newOp);
}




class SOP_FeE_UnpackByGroup_1_0Verb : public SOP_NodeVerb
{
public:
    SOP_FeE_UnpackByGroup_1_0Verb() {}
    virtual ~SOP_FeE_UnpackByGroup_1_0Verb() {}

    virtual SOP_NodeParms *allocParms() const { return new SOP_FeE_UnpackByGroup_1_0Parms(); }
    virtual UT_StringHolder name() const { return SOP_FeE_UnpackByGroup_1_0::theSOPTypeName; }

    virtual CookMode cookMode(const SOP_NodeParms *parms) const { return COOK_GENERIC; }

    virtual void cook(const CookParms &cookparms) const;
    
    static const SOP_NodeVerb::Register<SOP_FeE_UnpackByGroup_1_0Verb> theVerb;
};

const SOP_NodeVerb::Register<SOP_FeE_UnpackByGroup_1_0Verb> SOP_FeE_UnpackByGroup_1_0Verb::theVerb;

const SOP_NodeVerb *
SOP_FeE_UnpackByGroup_1_0::cookVerb() const 
{ 
    return SOP_FeE_UnpackByGroup_1_0Verb::theVerb.get();
}









static GA_GroupType
sopGroupType(SOP_FeE_UnpackByGroup_1_0Parms::GroupType parmgrouptype)
{
    using namespace SOP_FeE_UnpackByGroup_1_0Enums;
    switch (parmgrouptype)
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

static GFE_DelAndUnpack_Type
sopiterationMethod(SOP_FeE_UnpackByGroup_1_0Parms::iterationMethod parmiterationMethod)
{
    using namespace SOP_FeE_UnpackByGroup_1_0Enums;
    switch (parmiterationMethod)
    {
    case iterationMethod::CUSTOM:           return GFE_DelAndUnpack_Type::Custom;     break;
    case iterationMethod::ONEELEM:          return GFE_DelAndUnpack_Type::OneElem;    break;
    case iterationMethod::SKIPNELEM:        return GFE_DelAndUnpack_Type::SkipNElem;  break;
    }
    UT_ASSERT_MSG(0, "Unhandled Delete Type!");
    return GFE_DelAndUnpack_Type::OneElem;
}

void
SOP_FeE_UnpackByGroup_1_0Verb::cook(const SOP_NodeVerb::CookParms& cookparms) const
{
    auto&& sopparms = cookparms.parms<SOP_FeE_UnpackByGroup_1_0Parms>();
    GA_Detail& outGeo0 = *cookparms.gdh().gdpNC();
    //auto sopcache = (SOP_FeE_UnpackByGroup_1_0Cache*)cookparms.cache();

    const GA_Detail& inGeo0 = *cookparms.inputGeo(0);
    outGeo0.replaceWith(inGeo0);


    const GA_GroupType groupType = sopGroupType(sopparms.getGroupType());
    const GFE_DelAndUnpack_Type iterationMethod = sopiterationMethod(sopparms.getiterationMethod());
    
    UT_AutoInterrupt boss("Processing");
    if (boss.wasInterrupted())
        return;



    GFE_DelAndUnpack delAndUnpack(outGeo0, inGeo0, cookparms);
    
    delAndUnpack.setComputeParm(iterationMethod, sopparms.getReverseGroup(), sopparms.getDelGroup());
    
    switch (iterationMethod) {
    case GFE_DelAndUnpack_Type::Custom:    delAndUnpack.groupParser.setGroup(groupType, sopparms.getGroup());                               break;
    case GFE_DelAndUnpack_Type::OneElem:   delAndUnpack.setPrimoff(sopparms.getPrimoff(), sopparms.getInputAsOffset());                              break;
    case GFE_DelAndUnpack_Type::SkipNElem: delAndUnpack.setSkipNPrim(sopparms.getPrimoff(), sopparms.getInputAsOffset(), sopparms.getSkipNPrim());   break;
    default: break;
    }
    

    delAndUnpack.computeAndBumpDataIdsForAddOrRemove();
    
}

