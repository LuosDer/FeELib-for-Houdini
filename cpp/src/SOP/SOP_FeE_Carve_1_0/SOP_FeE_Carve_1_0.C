
//#define UT_ASSERT_LEVEL 3
#include "SOP_FeE_Carve_1_0.h"


#include "SOP_FeE_Carve_1_0.proto.h"

#include "GA/GA_Detail.h"
#include "PRM/PRM_TemplateBuilder.h"
#include "UT/UT_Interrupt.h"
#include "UT/UT_DSOVersion.h"



#include "GA_FeE/GA_FeE_Carve.h"


using namespace SOP_FeE_Carve_1_0_Namespace;


static const char *theDsFile = R"THEDSFILE(
{
    name	parameters
    parm {
        name    "carveSpace"
        cppname "interpAttrib"
        label   "Carve Space"
        type    ordinal
        default { "world" }
        menu {
            "customAttrib"  "CustomAttrib"
            "average"       "Average"
            "arcLength"     "Arc Length"
            "world"         "World"
        }
    }
    parm {
        name    "customCarveUVAttribName"
        label   "Custom Carve UV Attrib Name"
        type    string
        default { "" }
        disablewhen "{ carveSpace != customAttrib }"
    }
    parm {
        name    "sepparm"
        label   "Separator"
        type    separator
        default { "" }
    }
    parm {
        name    "firstptgroup"
        label   "First Point Group"
        type    string
        default { "" }
        range   { 0 1 }
        parmtag { "script_action" "import soputils\nkwargs['geometrytype'] = (hou.geometryType.Points, )\nkwargs['inputindex'] = 0\nsoputils.selectGroupParm(kwargs)" }
        parmtag { "script_action_help" "Select geometry from an available viewport." }
        parmtag { "script_action_icon" "BUTTONS_reselect" }
    }
    parm {
        name    "startCarveULocal"
        label   "Start Carve U Local"
        type    float
        default { "0.25" }
        hidewhen "{ carveSpace != uv }"
        range   { 0! 1! }
    }
    parm {
        name    "startCarveUWorld"
        label   "Start Carve U Local World"
        type    log
        default { "0.25" }
        range   { 0.01 100 }
    }
    parm {
        name    "startCarveUAttrib"
        label   "Start CarveU Attrib"
        type    string
        default { "startCarveU" }
    }
    parm {
        name    "outBreakPtsGrp_outside1"
        label   "Output Break Points Group Outside1"
        type    toggle
        nolabel
        joinnext
        default { "0" }
    }
    parm {
        name    "breakPtsGrp_outside1"
        label   "Break Points Group Outside1"
        type    string
        default { "breakpt" }
        disablewhen "{ outBreakPtsGrp_outside1 == 0 }"
    }
    parm {
        name    "outCurveGrp_outside1"
        label   "Output Curve Group Outside1"
        type    toggle
        nolabel
        joinnext
        default { "0" }
    }
    parm {
        name    "curveGrp_outside1"
        label   "Curve Group Outside1"
        type    string
        default { "outside" }
        disablewhen "{ outCurveGrp_outside1 == 0 }"
    }
    parm {
        name    "outSourcePt_outside1"
        label   "Output Source Points Outside1"
        type    toggle
        nolabel
        joinnext
        default { "0" }
    }
    parm {
        name    "sourcePt_outside1"
        label   "Source Points Outside1"
        type    string
        default { "sourcept" }
        disablewhen "{ outSourcePt_outside1 == 0 }"
    }
    parm {
        name    "keepoutside1"
        label   "Keep Outside"
        type    toggle
        default { "0" }
    }
    parm {
        name    "sepparm2"
        label   "Separator"
        type    separator
        default { "" }
    }
    parm {
        name    "secondptgroup"
        label   "Second Point Group"
        type    string
        default { "" }
        range   { 0 1 }
        parmtag { "script_action" "import soputils\nkwargs['geometrytype'] = kwargs['node'].parmTuple('grouptype')\nkwargs['inputindex'] = 0\nsoputils.selectGroupParm(kwargs)" }
        parmtag { "script_action_help" "Select geometry from an available viewport." }
        parmtag { "script_action_icon" "BUTTONS_reselect" }
    }
    parm {
        name    "absdomainuworld2"
        label   "Absolute Second U"
        type    toggle
        default { "0" }
    }
    parm {
        name    "domainu2"
        label   "Second U"
        type    float
        default { "0.75" }
        hidewhen "{ carveSpace != uv }"
        range   { 0! 1! }
    }
    parm {
        name    "domainuworld2"
        label   "Second U World"
        type    log
        default { "0.75" }
        range   { 0.01 100 }
    }
    parm {
        name    "endCarveU_attribName"
        label   "End CarveU Attrib Name"
        type    string
        default { "endCarveU" }
    }
    parm {
        name    "outBreakPtsGrp_outside2"
        label   "Output Break Points Group Outside2"
        type    toggle
        nolabel
        joinnext
        default { "ch(\"outBreakPtsGrp_outside1\")" }
    }
    parm {
        name    "breakPtsGrp_outside2"
        label   "Break Points Group Outside2"
        type    string
        default { [ "chs(\"breakPtsGrp_outside1\")" hscript-expr ] }
        disablewhen "{ outBreakPtsGrp_outside2 == 0 }"
    }
    parm {
        name    "outCurveGrp_outside2"
        label   "Output Curve Group Outside1"
        type    toggle
        nolabel
        joinnext
        default { "ch(\"outCurveGrp_outside1\")" }
    }
    parm {
        name    "curveGrp_outside2"
        label   "Curve Group Outside1"
        type    string
        default { [ "chs(\"curveGrp_outside1\")" hscript-expr ] }
        disablewhen "{ outCurveGrp_outside2 == 0 }"
    }
    parm {
        name    "outSourcePt_outside2"
        label   "Output Source Points Outside1"
        type    toggle
        nolabel
        joinnext
        default { [ "ch(\"outSourcePt_outside1\")" hscript-expr ] }
    }
    parm {
        name    "sourcePt_outside2"
        label   "Source Points Outside1"
        type    string
        default { [ "chs(\"sourcePt_outside1\")" hscript-expr ] }
        disablewhen "{ outSourcePt_outside2 == 0 }"
    }
    parm {
        name    "reverseoutside2"
        label   "Reverse Outside2"
        type    toggle
        default { "0" }
    }
    parm {
        name    "keepoutside2"
        label   "Keep Outside"
        type    toggle
        default { "0" }
    }
    parm {
        name    "sepparm3"
        label   "Separator"
        type    separator
        default { "" }
    }
    parm {
        name    "outBreakPtsGrp_inside1"
        label   "Output Break Points Group Inside1"
        type    toggle
        nolabel
        joinnext
        default { "ch(\"outBreakPtsGrp_outside1\")" }
    }
    parm {
        name    "breakPtsGrp_inside1"
        label   "Break Points Group Inside1"
        type    string
        default { [ "chs(\"breakPtsGrp_outside1\")" hscript-expr ] }
        disablewhen "{ outBreakPtsGrp_inside1 == 0 }"
    }
    parm {
        name    "outBreakPtsGrp_inside2"
        label   "Output Break Points Group Inside"
        type    toggle
        nolabel
        joinnext
        default { "ch(\"outBreakPtsGrp_inside1\")" }
    }
    parm {
        name    "breakPtsGrp_inside2"
        label   "Break Points Group Inside2"
        type    string
        default { [ "chs(\"breakPtsGrp_inside1\")" hscript-expr ] }
        disablewhen "{ outBreakPtsGrp_inside2 == 0 }"
    }
    parm {
        name    "outCurveGrp_inside"
        label   "Output Curve Group Inside"
        type    toggle
        nolabel
        joinnext
        default { "0" }
    }
    parm {
        name    "curveGrp_inside"
        label   "Curve Group Inside"
        type    string
        default { "inside" }
        disablewhen "{ outCurveGrp_inside == 0 }"
    }
    parm {
        name    "outSourcePt_inside1"
        label   "Output Source Points Inside1"
        type    toggle
        nolabel
        joinnext
        default { [ "ch(\"outSourcePt_outside1\")" hscript-expr ] }
    }
    parm {
        name    "sourcePt_inside1"
        label   "Source Points Inside1"
        type    string
        default { [ "chs(\"sourcePt_outside1\")" hscript-expr ] }
        disablewhen "{ outSourcePt_inside1 == 0 }"
    }
    parm {
        name    "outSourcePt_inside2"
        label   "Output Source Points Inside"
        type    toggle
        nolabel
        joinnext
        default { [ "ch(\"outSourcePt_outside1\")" hscript-expr ] }
    }
    parm {
        name    "sourcePt_inside2"
        label   "Source Points Inside"
        type    string
        default { [ "chs(\"sourcePt_outside1\")" hscript-expr ] }
        disablewhen "{ outSourcePt_inside1 == 0 }"
    }
    parm {
        name    "delReversedInsideCurve"
        label   "Delete Reversed Inside Curve"
        type    toggle
        default { "0" }
        disablewhen "{ keepinside == 0 }"
    }
    parm {
        name    "keepinside"
        label   "Keep Inside"
        type    toggle
        default { "1" }
    }
    parm {
        name    "sepparm4"
        label   "Separator"
        type    separator
        default { "" }
    }
    parm {
        name    "interpAttrib"
        cppname "interpAttrib"
        label   "Interp Attribs Name"
        type    string
        default { "P" }
    }
    parm {
        name    "sort_ptsorder"
        label   "Sort Points Order"
        type    toggle
        default { "0" }
    }
    parm {
        name    "sort_primsorder"
        label   "Sort Primitives Order"
        type    toggle
        default { "0" }
    }
    parm {
        name    "del_startCarveU_attrib"
        label   "Delete Start Carve U Attrib"
        type    toggle
        default { "1" }
    }
    parm {
        name    "del_endCarveU_attrib"
        label   "Delete End CarveU Attrib"
        type    toggle
        default { [ "ch('del_startCarveU_attrib')" hscript-expr ] }
    }


    //parm {
    //    name    "kernel"
    //    cppname "Kernel"
    //    label   "Kernel"
    //    type    integer
    //    default { 0 }
    //    range   { 0! 1! }
    //}



    //parm {
    //    name    "subscribeRatio"
    //    cppname "SubscribeRatio"
    //    label   "Subscribe Ratio"
    //    type    integer
    //    default { 64 }
    //    range   { 0! 256 }
    //}
    //parm {
    //    name    "minGrainSize"
    //    cppname "MinGrainSize"
    //    label   "Min Grain Size"
    //    type    intlog
    //    default { 64 }
    //    range   { 0! 2048 }
    //}
}
)THEDSFILE";

PRM_Template*
SOP_FeE_Carve_1_0::buildTemplates()
{
    static PRM_TemplateBuilder templ("SOP_FeE_Carve_1_0.C"_sh, theDsFile);
    if (templ.justBuilt())
    {
        templ.setChoiceListPtr("cutPointGroup"_sh, &SOP_Node::pointGroupMenu);
        templ.setChoiceListPtr("primGroup"_sh, &SOP_Node::primGroupMenu);

        templ.setChoiceListPtr("keepPrimAttribName"_sh, &SOP_Node::primAttribMenu);
        templ.setChoiceListPtr("keepPointAttribName"_sh, &SOP_Node::pointAttribMenu);
        templ.setChoiceListPtr("keepPrimGroupName"_sh, &SOP_Node::primGroupMenu);
        templ.setChoiceListPtr("keepPointGroupName"_sh, &SOP_Node::pointGroupMenu);
        templ.setChoiceListPtr("keepEdgeGroupName"_sh, &SOP_Node::edgeGroupMenu);
    }
    return templ.templates();
}





const UT_StringHolder SOP_FeE_Carve_1_0::theSOPTypeName("FeE::carve::1.0"_sh);

void
newSopOperator(OP_OperatorTable* table)
{
    OP_Operator* newOp = new OP_Operator(
        SOP_FeE_Carve_1_0::theSOPTypeName,
        "FeE Carve",
        SOP_FeE_Carve_1_0::myConstructor,
        SOP_FeE_Carve_1_0::buildTemplates(),
        1,
        1,
        nullptr,
        OP_FLAG_GENERATOR,
        nullptr,
        1,
        "Five elements Elf/Fracture/Clip");

    newOp->setIconName("SOP_carve");
    table->addOperator(newOp);
}




class SOP_FeE_Carve_1_0Verb : public SOP_NodeVerb
{
public:
    SOP_FeE_Carve_1_0Verb() {}
    virtual ~SOP_FeE_Carve_1_0Verb() {}

    virtual SOP_NodeParms *allocParms() const { return new SOP_FeE_Carve_1_0Parms(); }
    virtual UT_StringHolder name() const { return SOP_FeE_Carve_1_0::theSOPTypeName; }

    virtual CookMode cookMode(const SOP_NodeParms *parms) const { return COOK_GENERIC; }

    virtual void cook(const CookParms &cookparms) const;
    
    static const SOP_NodeVerb::Register<SOP_FeE_Carve_1_0Verb> theVerb;
};

const SOP_NodeVerb::Register<SOP_FeE_Carve_1_0Verb> SOP_FeE_Carve_1_0Verb::theVerb;

const SOP_NodeVerb *
SOP_FeE_Carve_1_0::cookVerb() const 
{ 
    return SOP_FeE_Carve_1_0Verb::theVerb.get();
}








static GA_FeE_PolyCutType
sopPolyType(SOP_FeE_Carve_1_0Parms::PolyType parmgrouptype)
{
    using namespace SOP_FeE_Carve_1_0Enums;
    switch (parmgrouptype)
    {
    case PolyType::AUTO:       return GA_FeE_PolyCutType_AUTO;    break;
    case PolyType::POLYLINE:   return GA_FeE_PolyCutType_OPEN;    break;
    case PolyType::POLY:       return GA_FeE_PolyCutType_CLOSE;    break;
    }
    UT_ASSERT_MSG(0, "Unhandled Poly type!");
    return GA_FeE_PolyCutType_AUTO;
}



void
SOP_FeE_Carve_1_0Verb::cook(const SOP_NodeVerb::CookParms& cookparms) const
{
    auto&& sopparms = cookparms.parms<SOP_FeE_Carve_1_0Parms>();
    GA_Detail* const outGeo0 = cookparms.gdh().gdpNC();
    //auto sopcache = (SOP_FeE_Carve_1_0Cache*)cookparms.cache();

    const GA_Detail* const inGeo0 = cookparms.inputGeo(0);



    const bool mergePrimEndsIfClosed = sopparms.getMergePrimEndsIfClosed();
    const bool cutPoint = sopparms.getCutPoint();
    const GA_FeE_PolyCutType polyType = sopPolyType(sopparms.getPolyType());


    const GA_Storage inStorageI = GA_FeE_Type::getPreferredStorageI(outGeo0);

    UT_AutoInterrupt boss("Processing");
    if (boss.wasInterrupted())
        return;
    

    const UT_StringHolder& keepPrimAttribName  = sopparms.getKeepPrimAttribName();
    const UT_StringHolder& keepPointAttribName = sopparms.getKeepPointAttribName();
    const UT_StringHolder& keepPrimGroupName   = sopparms.getKeepPrimGroupName();
    const UT_StringHolder& keepPointGroupName  = sopparms.getKeepPointGroupName();
    const UT_StringHolder& keepEdgeGroupName   = sopparms.getKeepEdgeGroupName();

    const bool delInputPointGroup = sopparms.getDelInputPointGroup();


    const UT_StringHolder& emptyStr = "";

    const bool createSrcPrimAttrib = sopparms.getCreateSrcPrimAttrib();
    const UT_StringHolder& srcPrimAttribName = createSrcPrimAttrib ? sopparms.getSrcPrimAttribName() : emptyStr;

    const bool createSrcPointAttrib = sopparms.getCreateSrcPointAttrib();
    const UT_StringHolder& srcPointAttribName = createSrcPointAttrib ? sopparms.getSrcPointAttribName() : emptyStr;

    
    //if (cutPoint)
    //{
    //    if (createSrcPointAttrib || keepPointAttribName.length() > 0 || keepPointGroupName.length() > 0)
    //    {
    //        const UT_StringHolder& srcPointAttribName = sopparms.getSrcPointAttribName();
    //        srcPointsAttrib = outGeo0->addIntTuple(GA_ATTRIB_POINT, srcPointAttribName, 1, GA_Defaults(-1), nullptr, nullptr, inStorageI);
    //    }
    //}


    GA_FeE_PolyCut::polyCut(cookparms, outGeo0, inGeo0,
        sopparms.getCutPointGroup(), sopparms.getPrimGroup(),
        cutPoint, mergePrimEndsIfClosed, polyType,
        inStorageI, srcPrimAttribName, srcPointAttribName);


    outGeo0->bumpDataIdsForAddOrRemove(1, 1, 1);

}



namespace SOP_FeE_Carve_1_0_Namespace {

} // End SOP_FeE_Carve_1_0_Namespace namespace
