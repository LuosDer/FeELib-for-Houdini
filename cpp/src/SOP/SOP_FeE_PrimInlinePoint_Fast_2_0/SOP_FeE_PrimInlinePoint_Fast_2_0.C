
//#define UT_ASSERT_LEVEL 3
#include "SOP_FeE_PrimInlinePoint_Fast_2_0.h"


#include "SOP_FeE_PrimInlinePoint_Fast_2_0.proto.h"

#include "GEO/GEO_Detail.h"
#include "PRM/PRM_TemplateBuilder.h"
#include "UT/UT_Interrupt.h"
#include "UT/UT_DSOVersion.h"



#include "GA_FeE/GA_FeE_GeoProperty.h"



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
        name    "reverseGroup"
        cppname "ReverseGroup"
        label   "Reverse Group"
        type    toggle
        default { "0" }
    }


}
)THEDSFILE";

PRM_Template*
SOP_FeE_PrimInlinePoint_Fast_2_0::buildTemplates()
{
    static PRM_TemplateBuilder templ("SOP_FeE_PrimInlinePoint_Fast_2_0.C"_sh, theDsFile);
    if (templ.justBuilt())
    {
        templ.setChoiceListPtr("primGroup"_sh, &SOP_Node::primGroupMenu);
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
    GEO_Detail* outGeo0 = cookparms.gdh().gdpNC();
    //auto sopcache = (SOP_FeE_PrimInlinePoint_Fast_2_0Cache*)cookparms.cache();

    const GEO_Detail* const inGeo0 = cookparms.inputGeo(0);


    outGeo0->replaceWithPoints(*inGeo0);

    const fpreal threshold_inlineAngle = sopparms.getThreshold_inlineAngle();
    const bool reverseGroup = sopparms.getReverseGroup();
    //const fpreal threshold_inlineDot = radians(threshold_inlineAngle);


    const GA_GroupType groupType = sopGroupType(sopparms.getGroupType());

    GA_FeE_GeoProperty::delPrimInlinePoint_fast(cookparms, outGeo0, groupType, sopparms.getGroup(), threshold_inlineAngle, reverseGroup);

    //const exint subscribeRatio = sopparms.getSubscribeRatio();
    //const exint minGrainSize = sopparms.getMinGrainSize();


    //const GA_Storage inStorageI = GA_FeE_Type::getPreferredStorageI(outGeo0);

    UT_AutoInterrupt boss("Processing");
    if (boss.wasInterrupted())
        return;
    


    outGeo0->bumpDataIdsForAddOrRemove(1, 1, 1);

}



namespace SOP_FeE_PrimInlinePoint_Fast_2_0_Namespace {

} // End SOP_FeE_PrimInlinePoint_Fast_2_0_Namespace namespace
