
//#define UT_ASSERT_LEVEL 3
#include "SOP_FeE_ExtendCurveStraight_1_0.h"


#include "SOP_FeE_ExtendCurveStraight_1_0.proto.h"

#include "GA/GA_Detail.h"
#include "PRM/PRM_TemplateBuilder.h"
#include "UT/UT_Interrupt.h"
#include "UT/UT_DSOVersion.h"



#include "GFE/GFE_ExtendCurveStraight.h"


using namespace SOP_FeE_ExtendCurveStraight_1_0_Namespace;


static const char *theDsFile = R"THEDSFILE(
{
    name	parameters
    parm {
        name    "primGroup"
        cppname "PrimGroup"
        label   "Prim Group"
        type    string
        default { "" }
        range   { 0 1 }
        parmtag { "script_action" "import soputils\nkwargs['geometrytype'] = (hou.geometryType.Primitives,)\nkwargs['inputindex'] = 0\nsoputils.selectGroupParm(kwargs)" }
        parmtag { "script_action_help" "Select geometry from an available viewport." }
        parmtag { "script_action_icon" "BUTTONS_reselect" }
    }
    parm {
        name    "pointGroup"
        cppname "PointGroup"
        label   "Point Group"
        type    string
        default { "" }
        range   { 0 1 }
        parmtag { "script_action" "import soputils\nkwargs['geometrytype'] = (hou.geometryType.Points,)\nkwargs['inputindex'] = 0\nsoputils.selectGroupParm(kwargs)" }
        parmtag { "script_action_help" "Select geometry from an available viewport." }
        parmtag { "script_action_icon" "BUTTONS_reselect" }
    }

    parm {
        name    "extendStartPoint"
        cppname "ExtendStartPoint"
        label   "Extend Start Point"
        type    toggle
        default { "1" }
    }
    parm {
        name    "extendEndPoint"
        cppname "ExtendEndPoint"
        label   "Extend End Point"
        type    toggle
        default { "1" }
    }
    parm {
        name    "tryFindNextOverlapPoint"
        cppname "TryFindNextOverlapPoint"
        label   "Try Find Next Overlaping Point"
        type    toggle
        default { "0" }
    }
    parm {
        name    "extendCurveStraightDist"
        cppname "ExtendCurveStraightDist"
        label   "Distance"
        type    float
        default { "0" }
        range   { -10 10 }
    }

    parm {
        name    "posAttrib"
        cppname "PosAttrib"
        label   "Pos Attrib"
        type    string
        default { "P" }
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
SOP_FeE_ExtendCurveStraight_1_0::buildTemplates()
{
    static PRM_TemplateBuilder templ("SOP_FeE_ExtendCurveStraight_1_0.C"_sh, theDsFile);
    if (templ.justBuilt())
    {
        templ.setChoiceListPtr("primGroup"_sh, &SOP_Node::primGroupMenu);
        templ.setChoiceListPtr("pointGroup"_sh, &SOP_Node::pointGroupMenu);
    }
    return templ.templates();
}





const UT_StringHolder SOP_FeE_ExtendCurveStraight_1_0::theSOPTypeName("FeE::extendCurveStraight::1.0"_sh);

void
newSopOperator(OP_OperatorTable* table)
{
    OP_Operator* newOp = new OP_Operator(
        SOP_FeE_ExtendCurveStraight_1_0::theSOPTypeName,
        "FeE Extend Curve Straight",
        SOP_FeE_ExtendCurveStraight_1_0::myConstructor,
        SOP_FeE_ExtendCurveStraight_1_0::buildTemplates(),
        1,
        1,
        nullptr,
        OP_FLAG_GENERATOR,
        nullptr,
        1,
        "Five elements Elf/Filter/Curve");

    newOp->setIconName("SOP_curve");
    table->addOperator(newOp);
}




class SOP_FeE_ExtendCurveStraight_1_0Verb : public SOP_NodeVerb
{
public:
    SOP_FeE_ExtendCurveStraight_1_0Verb() {}
    virtual ~SOP_FeE_ExtendCurveStraight_1_0Verb() {}

    virtual SOP_NodeParms *allocParms() const { return new SOP_FeE_ExtendCurveStraight_1_0Parms(); }
    virtual UT_StringHolder name() const { return SOP_FeE_ExtendCurveStraight_1_0::theSOPTypeName; }

    virtual CookMode cookMode(const SOP_NodeParms *parms) const { return COOK_GENERIC; }

    virtual void cook(const CookParms &cookparms) const;
    
    static const SOP_NodeVerb::Register<SOP_FeE_ExtendCurveStraight_1_0Verb> theVerb;
};

const SOP_NodeVerb::Register<SOP_FeE_ExtendCurveStraight_1_0Verb> SOP_FeE_ExtendCurveStraight_1_0Verb::theVerb;

const SOP_NodeVerb *
SOP_FeE_ExtendCurveStraight_1_0::cookVerb() const 
{ 
    return SOP_FeE_ExtendCurveStraight_1_0Verb::theVerb.get();
}







void
SOP_FeE_ExtendCurveStraight_1_0Verb::cook(const SOP_NodeVerb::CookParms& cookparms) const
{
    auto&& sopparms = cookparms.parms<SOP_FeE_ExtendCurveStraight_1_0Parms>();
    GA_Detail* const outGeo0 = cookparms.gdh().gdpNC();
    //auto sopcache = (SOP_FeE_ExtendCurveStraight_1_0Cache*)cookparms.cache();

    const GA_Detail* const inGeo0 = cookparms.inputGeo(0);

    outGeo0->replaceWith(*inGeo0);

    
    const bool extendStartPoint = sopparms.getExtendStartPoint();
    const bool extendEndPoint = sopparms.getExtendEndPoint();

    const bool tryFindNextOverlapPoint = sopparms.getTryFindNextOverlapPoint();
    const fpreal extendCurveStraightDist = sopparms.getExtendCurveStraightDist();

    UT_AutoInterrupt boss("Processing");
    if (boss.wasInterrupted())
        return;
    

    const exint subscribeRatio = sopparms.getSubscribeRatio();
    const exint minGrainSize = sopparms.getMinGrainSize();

    
    //if (cutPoint)
    //{
    //    if (createSrcPointAttrib || keepPointAttribName.length() > 0 || keepPointGroupName.length() > 0)
    //    {
    //        const UT_StringHolder& srcPointAttribName = sopparms.getSrcPointAttribName();
    //        srcPointsAttrib = outGeo0->addIntTuple(GA_ATTRIB_POINT, srcPointAttribName, 1, GA_Defaults(-1), nullptr, nullptr, inStorageI);
    //    }
    //}


    
        
    GFE_ExtendCurveStraight::extendCurveStraight(cookparms, outGeo0,
        extendCurveStraightDist, sopparms.getPrimGroup(), sopparms.getPointGroup(),
        extendStartPoint, extendEndPoint, tryFindNextOverlapPoint,
        sopparms.getPosAttrib(),
        subscribeRatio, minGrainSize);


    outGeo0->bumpDataIdsForAddOrRemove(1, 1, 1);

}



namespace SOP_FeE_ExtendCurveStraight_1_0_Namespace {

} // End SOP_FeE_ExtendCurveStraight_1_0_Namespace namespace
