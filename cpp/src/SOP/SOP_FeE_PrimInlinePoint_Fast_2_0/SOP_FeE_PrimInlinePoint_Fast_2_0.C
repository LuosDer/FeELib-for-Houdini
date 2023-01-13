
//#define UT_ASSERT_LEVEL 3
#include "SOP_FeE_PrimInlinePoint_Fast_2_0.h"


#include "SOP_FeE_PrimInlinePoint_Fast_2_0.proto.h"

#include "GEO/GEO_Detail.h"
#include "PRM/PRM_TemplateBuilder.h"
#include "UT/UT_Interrupt.h"
#include "UT/UT_DSOVersion.h"



#include "GA_FeE/GA_FeE_PrimInlinePoint.h"



using namespace SOP_FeE_PrimInlinePoint_Fast_2_0_Namespace;


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
        parmtag { "sop_input" "0" }
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
        name    "threshold_inlineAngle"
        cppname "Threshold_inlineAngle"
        label   "Threshold Inline Angle"
        type    angle
        default { "1e-05" }
        range   { 0! 180! }
    }

    parm {
        name    "primInlinePoint_groupName"
        cppname "PrimInlinePoint_groupName"
        label   "Prim Inline Point Group Name"
        type    string
        default { "primInlinePoint_fast" }
        disablewhen " { delInlinePoint == 1 }"
    }


    parm {
        name    "reverseGroup"
        cppname "ReverseGroup"
        label   "Reverse Group"
        type    toggle
        default { "0" }
    }
    parm {
        name    "delInlinePoint"
        cppname "DelInlinePoint"
        label   "Delete Inline Point"
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
SOP_FeE_PrimInlinePoint_Fast_2_0::buildTemplates()
{
    static PRM_TemplateBuilder templ("SOP_FeE_PrimInlinePoint_Fast_2_0.C"_sh, theDsFile);
    if (templ.justBuilt())
    {
        templ.setChoiceListPtr("group"_sh, &SOP_Node::allGroupMenu);
    }
    return templ.templates();
}





const UT_StringHolder SOP_FeE_PrimInlinePoint_Fast_2_0::theSOPTypeName("FeE::primInlinePoint_fast::4.0"_sh);

void
newSopOperator(OP_OperatorTable* table)
{
    OP_Operator* newOp = new OP_Operator(
        SOP_FeE_PrimInlinePoint_Fast_2_0::theSOPTypeName,
        "FeE Primitive In-Line Point Fast",
        SOP_FeE_PrimInlinePoint_Fast_2_0::myConstructor,
        SOP_FeE_PrimInlinePoint_Fast_2_0::buildTemplates(),
        1,
        1,
        nullptr,
        OP_FLAG_GENERATOR,
        nullptr,
        1,
        "Five elements Elf/Topo/Optimize");

    newOp->setIconName("SOP_edgedivide");
    table->addOperator(newOp);
}




class SOP_FeE_PrimInlinePoint_Fast_2_0Verb : public SOP_NodeVerb
{
public:
    SOP_FeE_PrimInlinePoint_Fast_2_0Verb() {}
    virtual ~SOP_FeE_PrimInlinePoint_Fast_2_0Verb() {}

    virtual SOP_NodeParms *allocParms() const { return new SOP_FeE_PrimInlinePoint_Fast_2_0Parms(); }
    virtual UT_StringHolder name() const { return SOP_FeE_PrimInlinePoint_Fast_2_0::theSOPTypeName; }

    virtual CookMode cookMode(const SOP_NodeParms *parms) const { return COOK_GENERIC; }

    virtual void cook(const CookParms &cookparms) const;
    
    static const SOP_NodeVerb::Register<SOP_FeE_PrimInlinePoint_Fast_2_0Verb> theVerb;
};

const SOP_NodeVerb::Register<SOP_FeE_PrimInlinePoint_Fast_2_0Verb> SOP_FeE_PrimInlinePoint_Fast_2_0Verb::theVerb;

const SOP_NodeVerb *
SOP_FeE_PrimInlinePoint_Fast_2_0::cookVerb() const 
{ 
    return SOP_FeE_PrimInlinePoint_Fast_2_0Verb::theVerb.get();
}











static GA_GroupType
sopGroupType(SOP_FeE_PrimInlinePoint_Fast_2_0Parms::GroupType parmgrouptype)
{
    using namespace SOP_FeE_PrimInlinePoint_Fast_2_0Enums;
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




void
SOP_FeE_PrimInlinePoint_Fast_2_0Verb::cook(const SOP_NodeVerb::CookParms& cookparms) const
{
    auto&& sopparms = cookparms.parms<SOP_FeE_PrimInlinePoint_Fast_2_0Parms>();
    GEO_Detail* const outGeo0 = cookparms.gdh().gdpNC();
    //auto sopcache = (SOP_FeE_PrimInlinePoint_Fast_2_0Cache*)cookparms.cache();

    const GA_Detail* const inGeo0 = cookparms.inputGeo(0);

    outGeo0->replaceWith(*inGeo0);


    const fpreal threshold_inlineAngle = sopparms.getThreshold_inlineAngle();
    const fpreal threshold_inlineAngleRadians = GA_FeE_Type::radians(threshold_inlineAngle);
    const bool reverseGroup = sopparms.getReverseGroup();


    const GA_GroupType groupType = sopGroupType(sopparms.getGroupType());

    const exint subscribeRatio = sopparms.getSubscribeRatio();
    const exint minGrainSize = sopparms.getMinGrainSize();

    //const GA_Storage inStorageI = GA_FeE_Type::getPreferredStorageI(outGeo0);

    UT_AutoInterrupt boss("Processing");
    if (boss.wasInterrupted())
        return;



    if (sopparms.getDelInlinePoint())
    {
        GA_FeE_PrimInlinePoint::delPrimInlinePoint_fast(cookparms, outGeo0, groupType, sopparms.getGroup(),
            threshold_inlineAngleRadians, reverseGroup,
            subscribeRatio, minGrainSize);
        outGeo0->bumpDataIdsForAddOrRemove(1, 1, 1);
    }
    else
    {
        GA_PointGroup* const inlinePtGroup = GA_FeE_PrimInlinePoint::groupPrimInlinePoint_fast(cookparms, outGeo0, groupType, sopparms.getGroup(), sopparms.getPrimInlinePoint_groupName(),
            threshold_inlineAngleRadians, reverseGroup,
            subscribeRatio, minGrainSize);

        cookparms.getNode()->setHighlight(true);
        cookparms.select(*inlinePtGroup);

        inlinePtGroup->bumpDataId();
    }



}



namespace SOP_FeE_PrimInlinePoint_Fast_2_0_Namespace {

} // End SOP_FeE_PrimInlinePoint_Fast_2_0_Namespace namespace
