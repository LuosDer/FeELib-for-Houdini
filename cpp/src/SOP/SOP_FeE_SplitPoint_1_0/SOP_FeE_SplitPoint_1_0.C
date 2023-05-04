
//#define UT_ASSERT_LEVEL 3
#include "SOP_FeE_SplitPoint_1_0.h"


#include "SOP_FeE_SplitPoint_1_0.proto.h"

#include "GA/GA_Detail.h"
#include "PRM/PRM_TemplateBuilder.h"
#include "UT/UT_Interrupt.h"
#include "UT/UT_DSOVersion.h"




#include "GFE/GFE_SplitPoint.h"


using namespace SOP_FeE_SplitPoint_1_0_Namespace;


static const char *theDsFile = R"THEDSFILE(
{
    name	parameters
    parm {
        name    "group"
        cppname "Group"
        label   "Group"
        type    string
        default { "" }
        parmtag { "script_action" "import soputils\nkwargs['geometrytype'] = kwargs['node'].parmTuple('groupType')\nkwargs['inputindex'] = 0\nsoputils.selectGroupParm(kwargs)" }
        parmtag { "script_action_help" "Select geometry from an available viewport.\nShift-click to turn on Select Groups." }
        parmtag { "script_action_icon" "BUTTONS_reselect" }
    }
    parm {
        name    "groupType"
        cppname "GroupType"
        label   "Group Type"
        type    ordinal
        default { "0" }
        menu {
            "guess"     "Guess from Group"
            "prim"      "Primitive"
            "point"     "Point"
            "vertex"    "Vertex"
        }
    }
    parm {
        name    "splitByAttrib"
        cppname "SplitByAttrib"
        label   "Split by Attribute"
        type    toggle
        default { "0" }
        nolabel
        joinnext
    }
    parm {
        name    "splitByAttribName"
        cppname "SplitByAttribName"
        label   "Split by Attrib Name"
        type    string
        default { "N" }
        disablewhen "{ splitByAttrib == 0 }"
    }
    parm {
        name    "splitByAttribTol"
        cppname "SplitByAttribTol"
        label   "Split by Attrib Tolerance"
        type    log
        default { "0.001" }
        range   { 0! 1 }
        disablewhen "{ splitByAttrib == 0 }"
    }
    parm {
        name    "promoteVertexAttribToPoint"
        cppname "PromoteVertexAttribToPoint"
        label   "Promote Vertex Attribute to Point"
        type    toggle
        default { "0" }
        disablewhen "{ splitByAttrib == 0 }"
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





void
SOP_FeE_SplitPoint_1_0::buildAttribMenu(
    void* data, PRM_Name* entries, int size,
    const PRM_SpareData*, const PRM_Parm*)
{
    SOP_FeE_SplitPoint_1_0* sop = (SOP_FeE_SplitPoint_1_0*)data;
    if (!sop)
        return;

    const int input = 0; // input from which to obtain attribs

    sop->fillAttribNameMenu(entries, size, GA_ATTRIB_VERTEX, input);
}
static PRM_ChoiceList  sop_attribmenu(
    (PRM_ChoiceListType)(PRM_CHOICELIST_TOGGLE),
    SOP_FeE_SplitPoint_1_0::buildAttribMenu);




PRM_Template*
SOP_FeE_SplitPoint_1_0::buildTemplates()
{
    static PRM_TemplateBuilder templ("SOP_FeE_SplitPoint_1_0.C"_sh, theDsFile);
    if (templ.justBuilt())
    {
        templ.setChoiceListPtr("group", &SOP_Node::pointGroupMenu);
        templ.setChoiceListPtr("attribname", &sop_attribmenu);
    }
    return templ.templates();
}





const UT_StringHolder SOP_FeE_SplitPoint_1_0::theSOPTypeName("FeE::splitPoint::1.0"_sh);

void
newSopOperator(OP_OperatorTable* table)
{
    OP_Operator* newOp = new OP_Operator(
        SOP_FeE_SplitPoint_1_0::theSOPTypeName,
        "FeE Split Point",
        SOP_FeE_SplitPoint_1_0::myConstructor,
        SOP_FeE_SplitPoint_1_0::buildTemplates(),
        1,
        1,
        nullptr,
        OP_FLAG_GENERATOR,
        nullptr,
        1,
        "Five elements Elf/Topo/Primitive");

    newOp->setIconName("SOP_splitpoints");
    table->addOperator(newOp);
}




class SOP_FeE_SplitPoint_1_0Verb : public SOP_NodeVerb
{
public:
    SOP_FeE_SplitPoint_1_0Verb() {}
    virtual ~SOP_FeE_SplitPoint_1_0Verb() {}

    virtual SOP_NodeParms *allocParms() const { return new SOP_FeE_SplitPoint_1_0Parms(); }
    virtual UT_StringHolder name() const { return SOP_FeE_SplitPoint_1_0::theSOPTypeName; }

    virtual CookMode cookMode(const SOP_NodeParms *parms) const { return COOK_GENERIC; }

    virtual void cook(const CookParms &cookparms) const;
    
    static const SOP_NodeVerb::Register<SOP_FeE_SplitPoint_1_0Verb> theVerb;
};

const SOP_NodeVerb::Register<SOP_FeE_SplitPoint_1_0Verb> SOP_FeE_SplitPoint_1_0Verb::theVerb;

const SOP_NodeVerb *
SOP_FeE_SplitPoint_1_0::cookVerb() const 
{ 
    return SOP_FeE_SplitPoint_1_0Verb::theVerb.get();
}






static GA_GroupType
sopSplitPointsGroupType(SOP_FeE_SplitPoint_1_0Parms::GroupType parmgrouptype)
{
    using namespace SOP_FeE_SplitPoint_1_0Enums;
    switch (parmgrouptype)
    {
    case GroupType::GUESS:      return GA_GROUP_INVALID;    break;
    case GroupType::PRIM:       return GA_GROUP_PRIMITIVE;  break;
    case GroupType::POINT:      return GA_GROUP_POINT;      break;
    case GroupType::VERTEX:     return GA_GROUP_VERTEX;     break;
    }
    UT_ASSERT_MSG(0, "Unhandled group type!");
    return GA_GROUP_INVALID;
}


void
SOP_FeE_SplitPoint_1_0Verb::cook(const SOP_NodeVerb::CookParms& cookparms) const
{
    auto&& sopparms = cookparms.parms<SOP_FeE_SplitPoint_1_0Parms>();
    GA_Detail& outGeo0 = *cookparms.gdh().gdpNC();
    //auto sopcache = (SOP_FeE_SplitPoint_1_0Cache*)cookparms.cache();

    const GA_Detail& inGeo0 = *cookparms.inputGeo(0);


    UT_AutoInterrupt boss("Processing");
    if (boss.wasInterrupted())
        return;
    
    GFE_SplitPoint splitPoint(outGeo0, &cookparms);

    if (sopparms.getSplitByAttrib())
    {
        splitPoint.setSplitByAttrib(sopparms.getSplitByAttribName());
        splitPoint.splitAttribTol = sopparms.getSplitByAttribTol();
    }
    
    splitPoint.setComputeParm(sopparms.getSubscribeRatio(), sopparms.getMinGrainSize());


    splitPoint.setGroup(sopparms.getPrimGroup());
    splitPoint.setSplitByAttrib(false, uvAttribClass, GA_STORE_INVALID, uvAttribName, 3);

    splitPoint.computeAndBumpDataId();

}



namespace SOP_FeE_SplitPoint_1_0_Namespace {

} // End SOP_FeE_SplitPoint_1_0_Namespace namespace
