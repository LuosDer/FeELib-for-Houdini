
//#define UT_ASSERT_LEVEL 3
#include "SOP_FeE_DelElement_1_0.h"


#include "SOP_FeE_DelElement_1_0.proto.h"

#include "GEO/GEO_Detail.h"
#include "PRM/PRM_TemplateBuilder.h"
#include "UT/UT_Interrupt.h"
#include "UT/UT_DSOVersion.h"

#include "GU/GU_IntersectionAnalysis.h"

#include "GA_FeE/GA_FeE_Attribute.h"
#include "GA_FeE/GA_FeE_VertexNextEquiv.h"
#include "GEO_FeE/GEO_FeE_Group.h"



using namespace SOP_FeE_DelElement_1_0_Namespace;


static const char *theDsFile = R"THEDSFILE(
{
    name	parameters
    parm {
        name    "delElements"
        cppname "DelElements"
        label   "Delete Elements"
        type    toggle
        default { "on" }
    }
    groupsimple {
        name    "delElements_folder"
        label   "Delete Elements"
        disablewhen "{ delElements == 0 }"
        grouptag { "group_type" "simple" }

        parm {
            name    "delElementsGroup"
            cppname "DelElementsGroup"
            label   "Group Name"
            type    string
            default { "" }
            parmtag { "script_action" "import soputils\nkwargs['geometrytype'] = kwargs['node'].parmTuple('delElementsGroupType')\nkwargs['inputindex'] = 0\nsoputils.selectGroupParm(kwargs)" }
            parmtag { "script_action_help" "Select geometry from an available viewport.\nShift-click to turn on Select Groups." }
            parmtag { "script_action_icon" "BUTTONS_reselect" }
            parmtag { "sop_input" "0" }
        }
        parm {
            name    "delElementsGroupType"
            cppname "DelElementsGroupType"
            label   "Group Type"
            type    ordinal
            default { "guess" }
            menu {
                "guess"     "Guess from Group"
                "prim"      "Primitive"
                "point"     "Point"
            }
        }
        parm {
            name    "delElementsReverseGroup"
            cppname "DelElementsReverseGroup"
            label   "Delete Non Selected"
            type    toggle
            default { "off" }
        }
        parm {
            name    "delElementsInputGroup"
            cppname "DelElementsInputGroup"
            label   "Delete Elements Input Group"
            type    toggle
            default { "on" }
        }
    }
}
)THEDSFILE";

PRM_Template*
SOP_FeE_DelElement_1_0::buildTemplates()
{
    static PRM_TemplateBuilder templ("SOP_FeE_DelElement_1_0.C"_sh, theDsFile);
    if (templ.justBuilt())
    {
        templ.setChoiceListPtr("delElementsGroup"_sh, &SOP_Node::allGroupMenu);
    }
    return templ.templates();
}





const UT_StringHolder SOP_FeE_DelElement_1_0::theSOPTypeName("FeE::delElement::1.0"_sh);

void
newSopOperator(OP_OperatorTable* table)
{
    OP_Operator* newOp = new OP_Operator(
        SOP_FeE_DelElement_1_0::theSOPTypeName,
        "FeE Delete Element",
        SOP_FeE_DelElement_1_0::myConstructor,
        SOP_FeE_DelElement_1_0::buildTemplates(),
        1,
        1,
        nullptr,
        OP_FLAG_GENERATOR,
        nullptr,
        1,
        "Five elements Elf/Operation/Delete");

    newOp->setIconName("SOP_delete");
    table->addOperator(newOp);
}




class SOP_FeE_DelElement_1_0Verb : public SOP_NodeVerb
{
public:
    SOP_FeE_DelElement_1_0Verb() {}
    virtual ~SOP_FeE_DelElement_1_0Verb() {}

    virtual SOP_NodeParms *allocParms() const { return new SOP_FeE_DelElement_1_0Parms(); }
    virtual UT_StringHolder name() const { return SOP_FeE_DelElement_1_0::theSOPTypeName; }

    virtual CookMode cookMode(const SOP_NodeParms *parms) const { return COOK_GENERIC; }

    virtual void cook(const CookParms &cookparms) const;
    
    static const SOP_NodeVerb::Register<SOP_FeE_DelElement_1_0Verb> theVerb;
};

const SOP_NodeVerb::Register<SOP_FeE_DelElement_1_0Verb> SOP_FeE_DelElement_1_0Verb::theVerb;

const SOP_NodeVerb *
SOP_FeE_DelElement_1_0::cookVerb() const 
{ 
    return SOP_FeE_DelElement_1_0Verb::theVerb.get();
}


void
SOP_FeE_DelElement_1_0Verb::cook(const SOP_NodeVerb::CookParms& cookparms) const
{
    auto&& sopparms = cookparms.parms<SOP_FeE_DelElement_1_0Parms>();
    GEO_Detail* outGeo0 = cookparms.gdh().gdpNC();
    //auto sopcache = (SOP_FeE_DelElement_1_0Cache*)cookparms.cache();

    const GA_Detail* const inGeo0 = cookparms.inputGeo(0);
    outGeo0->replaceWith(*inGeo0);


    const bool reverseGroup = sopparms.getReverseGroup();

    const GA_GroupType groupType = sopGroupType(sopparms.getGroupType());
    const UT_StringHolder& groupName = sopparms.getGroup();

    UT_AutoInterrupt boss("Processing");
    if (boss.wasInterrupted())
        return;

    
    //GA_Group* geo0Group = nullptr;
    //if (reverseGroup)
    //{
    //    geo0Group = GA_FeE_Group::findGroup(inGeo0, groupType, groupName);
    //    if (!geo0Group || geo0Group->isEmpty())
    //    {
    //        const GA_AttributeFilter filter = GA_AttributeFilter::selectNot(GA_AttributeFilter::selectTopology());
    //        outGeo0->replaceWith(*inGeo0, &filter);
    //        return;
    //    }
    //}

    GA_Group* geo0Group = GA_FeE_Group::findGroup(outGeo0, groupType, groupName);


    const bool delByGroup = sopparms.getDelByGroup();
    if (!delByGroup)
    {
        if (geo0Group)
        {
            cookparms.getNode()->setHighlight(true);
            cookparms.select(*geo0Group);
        }
        return;
    }

    const GA_Detail::GA_DestroyPointMode delPointMode = sopDelPointMode(sopparms.getDelPointMode());
    GA_FeE_Detail::delByGroup(outGeo0, geo0Group, reverseGroup, sopparms.getDelGroup(), sopparms.getDelWithPoint(), delPointMode, sopparms.getGuaranteeNoVertexReference());
    //const exint subscribeRatio = sopparms.getSubscribeRatio();
    //const exint minGrainSize = sopparms.getMinGrainSize();
    //const exint minGrainSize = pow(2, 8);
    //const exint minGrainSize = pow(2, 4);


    //const GA_Storage inStorgeF = GA_STORE_REAL32;
    //const GA_Storage inStorgeI = GA_FeE_Type::getPreferredStorageI(outGeo0);
    outGeo0->bumpDataIdsForAddOrRemove(1,1,1);

}



namespace SOP_FeE_DelElement_1_0_Namespace {

} // End SOP_FeE_DelElement_1_0_Namespace namespace
