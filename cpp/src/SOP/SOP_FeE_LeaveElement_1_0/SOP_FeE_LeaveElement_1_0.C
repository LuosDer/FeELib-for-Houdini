
//#define UT_ASSERT_LEVEL 3
#include "SOP_FeE_LeaveElement_1_0.h"


#include "SOP_FeE_LeaveElement_1_0.proto.h"

#include "GA/GA_Detail.h"
#include "PRM/PRM_TemplateBuilder.h"
#include "UT/UT_Interrupt.h"
#include "UT/UT_DSOVersion.h"


#include "GA_FeE/GA_FeE_LeaveElement.h"



using namespace SOP_FeE_LeaveElement_1_0_Namespace;


static const char *theDsFile = R"THEDSFILE(
{
    name	parameters
    parm {
        name    "leaveElement"
        cppname "LeaveElement"
        label   "Leave Element"
        type    toggle
        default { "on" }
    }
    groupsimple {
        name    "leaveElement_folder"
        label   "Leave Element"
        disablewhen "{ leaveElement == 0 }"
        grouptag { "group_type" "simple" }

        parm {
            name    "leaveElementGroup"
            cppname "LeaveElementGroup"
            label   "Group Name"
            type    string
            default { "" }
            parmtag { "script_action" "import soputils\nkwargs['geometrytype'] = kwargs['node'].parmTuple('leaveElementGroupType')\nkwargs['inputindex'] = 0\nsoputils.selectGroupParm(kwargs)" }
            parmtag { "script_action_help" "Select geometry from an available viewport.\nShift-click to turn on Select Groups." }
            parmtag { "script_action_icon" "BUTTONS_reselect" }
            parmtag { "sop_input" "0" }
        }
        parm {
            name    "leaveElementGroupType"
            cppname "LeaveElementGroupType"
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
            name    "leaveElementReverseGroup"
            cppname "LeaveElementReverseGroup"
            label   "Leave Non Selected"
            type    toggle
            default { "off" }
        }
        parm {
            name    "delElementInputGroup"
            cppname "DelElementInputGroup"
            label   "Delete Element Input Group"
            type    toggle
            default { "on" }
        }
    }
}
)THEDSFILE";

PRM_Template*
SOP_FeE_LeaveElement_1_0::buildTemplates()
{
    static PRM_TemplateBuilder templ("SOP_FeE_LeaveElement_1_0.C"_sh, theDsFile);
    if (templ.justBuilt())
    {
        templ.setChoiceListPtr("leaveElementGroup"_sh, &SOP_Node::allGroupMenu);
    }
    return templ.templates();
}





const UT_StringHolder SOP_FeE_LeaveElement_1_0::theSOPTypeName("FeE::leaveElement::1.0"_sh);

void
newSopOperator(OP_OperatorTable* table)
{
    OP_Operator* newOp = new OP_Operator(
        SOP_FeE_LeaveElement_1_0::theSOPTypeName,
        "FeE Leave Element",
        SOP_FeE_LeaveElement_1_0::myConstructor,
        SOP_FeE_LeaveElement_1_0::buildTemplates(),
        1,
        1,
        nullptr,
        OP_FLAG_GENERATOR,
        nullptr,
        1,
        "Five elements Elf/Operation/Delete");

    newOp->setIconName("SOP_blast");
    table->addOperator(newOp);
}




class SOP_FeE_LeaveElement_1_0Verb : public SOP_NodeVerb
{
public:
    SOP_FeE_LeaveElement_1_0Verb() {}
    virtual ~SOP_FeE_LeaveElement_1_0Verb() {}

    virtual SOP_NodeParms *allocParms() const { return new SOP_FeE_LeaveElement_1_0Parms(); }
    virtual UT_StringHolder name() const { return SOP_FeE_LeaveElement_1_0::theSOPTypeName; }

    virtual CookMode cookMode(const SOP_NodeParms *parms) const { return COOK_GENERIC; }

    virtual void cook(const CookParms &cookparms) const;
    
    static const SOP_NodeVerb::Register<SOP_FeE_LeaveElement_1_0Verb> theVerb;
};

const SOP_NodeVerb::Register<SOP_FeE_LeaveElement_1_0Verb> SOP_FeE_LeaveElement_1_0Verb::theVerb;

const SOP_NodeVerb *
SOP_FeE_LeaveElement_1_0::cookVerb() const 
{ 
    return SOP_FeE_LeaveElement_1_0Verb::theVerb.get();
}








static GA_GroupType
sopGroupType(SOP_FeE_LeaveElement_1_0Parms::LeaveElementGroupType parmgrouptype)
{
    using namespace SOP_FeE_LeaveElement_1_0Enums;
    switch (parmgrouptype)
    {
    case LeaveElementGroupType::GUESS:     return GA_GROUP_INVALID;    break;
    case LeaveElementGroupType::PRIM:      return GA_GROUP_PRIMITIVE;  break;
    case LeaveElementGroupType::POINT:     return GA_GROUP_POINT;      break;
    case LeaveElementGroupType::VERTEX:    return GA_GROUP_VERTEX;     break;
    case LeaveElementGroupType::EDGE:      return GA_GROUP_EDGE;       break;
    }
    UT_ASSERT_MSG(0, "Unhandled geo0Group type!");
    return GA_GROUP_INVALID;
}


void
SOP_FeE_LeaveElement_1_0Verb::cook(const SOP_NodeVerb::CookParms& cookparms) const
{
    auto&& sopparms = cookparms.parms<SOP_FeE_LeaveElement_1_0Parms>();
    GA_Detail* const outGeo0 = cookparms.gdh().gdpNC();
    //auto sopcache = (SOP_FeE_LeaveElement_1_0Cache*)cookparms.cache();

    const GA_Detail* const inGeo0 = cookparms.inputGeo(0);


    UT_AutoInterrupt boss("Processing");
    if (boss.wasInterrupted())
        return;


    const bool reverseGroup = sopparms.getLeaveElementReverseGroup();

    const GA_GroupType groupType = sopGroupType(sopparms.getLeaveElementGroupType());
    const UT_StringHolder& groupName = sopparms.getLeaveElementGroup();


    GA_FeE_LeaveElement::leaveElement(sopparms.getLeaveElement(), cookparms, outGeo0, inGeo0, groupType, groupName,
        reverseGroup, sopparms.getDelElementInputGroup());

    if (sopparms.getLeaveElement())
    {
        outGeo0->bumpDataIdsForAddOrRemove(1, 1, 1);
    }
}



namespace SOP_FeE_LeaveElement_1_0_Namespace {

} // End SOP_FeE_LeaveElement_1_0_Namespace namespace
