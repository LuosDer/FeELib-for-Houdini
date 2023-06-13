
//#define UT_ASSERT_LEVEL 3
#include "SOP_FeE_SmallPiece_1_0.h"


#include "SOP_FeE_SmallPiece_1_0.proto.h"

#include "GA/GA_Detail.h"
#include "PRM/PRM_TemplateBuilder.h"
#include "UT/UT_Interrupt.h"
#include "UT/UT_DSOVersion.h"


#include "GFE/GFE_SmallPiece.h"

using namespace SOP_FeE_SmallPiece_1_0_Namespace;



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
        name    "check_oneof_xyzsize"
        label   "check_oneof_xyzsize"
        type    toggle
        nolabel
        joinnext
        default { "on" }
        range   { 0 10 }
    }
    parm {
        name    "threshold_oneof_xyzsize"
        label   "Threshold One of XYZ Size"
        type    log
        default { "0.1" }
        disablewhen "{ check_oneof_xyzsize == 0 }"
        range   { 0.01 100 }
    }
    parm {
        name    "threshold_oneof_xyzsize_attrib"
        label   "Threshold One of XYZ Size Attrib"
        type    string
        default { "" }
        disablewhen "{ check_oneof_xyzsize == 0 }"
    }
    parm {
        name    "check_all_xyzsize"
        label   "check_all_xyzsize"
        type    toggle
        nolabel
        joinnext
        default { "off" }
        range   { 0 10 }
    }
    parm {
        name    "threshold_all_xyzsize"
        label   "Threshold All XYZ Size"
        type    log
        default { "0.1" }
        disablewhen "{ check_all_xyzsize == 0 }"
        range   { 0.01 100 }
    }
    parm {
        name    "threshold_all_xyzsize_attrib"
        label   "Threshold All XYZ Size Attrib"
        type    string
        default { "" }
        disablewhen "{ check_all_xyzsize == 0 }"
    }
    parm {
        name    "check_xyzsize_sum"
        label   "check_xyzsize_sum"
        type    toggle
        nolabel
        joinnext
        default { "off" }
        range   { 0 10 }
    }
    parm {
        name    "threshold_xyzsize_sum"
        label   "Threshold XYZ Size Sum"
        type    log
        default { "0.1" }
        disablewhen "{ check_xyzsize_sum == 0 }"
        range   { 0.01 100 }
    }
    parm {
        name    "checkperimeter"
        label   "Check Perimeter"
        type    toggle
        nolabel
        joinnext
        default { "off" }
        range   { 0 10 }
    }
    parm {
        name    "threshold_perimeter"
        label   "Threshold Perimeter"
        type    log
        default { "0.1" }
        disablewhen "{ checkperimeter == 0 }"
        range   { 0.01 100 }
    }
    parm {
        name    "checkarea"
        label   "Check Area"
        type    toggle
        nolabel
        joinnext
        default { "off" }
        range   { 0 10 }
    }
    parm {
        name    "threshold_area"
        label   "Threshold Area"
        type    log
        default { "0.1" }
        disablewhen "{ checkarea == 0 }"
        range   { 0.01 100 }
    }
    parm {
        name    "checkvolume"
        label   "Check Volume"
        type    toggle
        nolabel
        joinnext
        default { "off" }
        range   { 0 10 }
    }
    parm {
        name    "threshold_volume"
        label   "Threshold Volume"
        type    log
        default { "0.1" }
        disablewhen "{ checkvolume == 0 }"
        range   { 0.01 100 }
    }
    parm {
        name    "check_bbox_area"
        label   "Check BBox Area"
        type    toggle
        nolabel
        joinnext
        default { "off" }
        range   { 0 10 }
    }
    parm {
        name    "threshold_bbox_area"
        label   "Threshold BBox Area"
        type    log
        default { "0.1" }
        disablewhen "{ check_bbox_area == 0 }"
        range   { 0.01 100 }
    }
    parm {
        name    "check_area_perimeter_ratio"
        label   "Check Area Perimeter Ratio"
        type    toggle
        nolabel
        joinnext
        default { "off" }
        range   { 0 10 }
    }
    parm {
        name    "threshold_area_perimeter_ratio"
        label   "Threshold Area Perimeter Ratio"
        type    log
        default { "0.1" }
        disablewhen "{ check_area_perimeter_ratio == 0 }"
        range   { 0.001 10 }
    }
    parm {
        name    "check_bbox_volume"
        label   "Check BBox Volume"
        type    toggle
        nolabel
        joinnext
        default { "off" }
        range   { 0 10 }
    }
    parm {
        name    "threshold_bbox_volume"
        label   "Threshold BBox Volume"
        type    log
        default { "0.1" }
        disablewhen "{ check_bbox_volume == 0 }"
        range   { 0.01 100 }
    }
    parm {
        name    "numConditionMatch"
        label   "num Condition Match"
        type    integer
        default { "1" }
        range   { 1! 6 }
    }

    parm {
        name    "smallPieceGroupName"
        cppname "SmallPieceGroupName"
        label   "Small Piece Group Name"
        type    string
        default { "small" }
        disablewhen "{ delElem == 1 delGroup == 1 }"
    }
    parm {
        name    "reverseGroup"
        cppname "ReverseGroup"
        label   "Reverse Group"
        type    toggle
        default { "0" }
    }
    parm {
        name    "delElem"
        cppname "DelElem"
        label   "Delete Element"
        type    toggle
        default { "0" }
    }
    parm {
        name    "delGroup"
        cppname "DelGroup"
        label   "Delete Group"
        type    toggle
        default { "1" }
        disablewhen "{ delElem == 0 }"
    }
    parm {
        name    "delUnusedPoint"
        cppname "DelUnusedPoint"
        label   "Delete Unused Point"
        type    toggle
        default { "1" }
        disablewhen "{ delElem == 0 }"
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
        default { 1024 }
        range   { 0! 2048 }
    }
}
)THEDSFILE";



PRM_Template*
SOP_FeE_SmallPiece_1_0::buildTemplates()
{
    static PRM_TemplateBuilder templ("SOP_FeE_SmallPiece_1_0.C"_sh, theDsFile);
    if (templ.justBuilt())
    {
        templ.setChoiceListPtr("group"_sh,               &SOP_Node::allGroupMenu);
        templ.setChoiceListPtr("smallPieceGroupName"_sh, &SOP_Node::primGroupMenu);
    }
    return templ.templates();
}


const UT_StringHolder SOP_FeE_SmallPiece_1_0::theSOPTypeName("FeE::smallPiece::1.0"_sh);

void
newSopOperator(OP_OperatorTable* table)
{
    OP_Operator* newOp = new OP_Operator(
        SOP_FeE_SmallPiece_1_0::theSOPTypeName,
        "FeE Small Piece",
        SOP_FeE_SmallPiece_1_0::myConstructor,
        SOP_FeE_SmallPiece_1_0::buildTemplates(),
        1,
        1,
        nullptr,
        OP_FLAG_GENERATOR,
        nullptr,
        1,
        "Five elements Elf/Group");

    newOp->setIconName("SOP_groupcreate");
    table->addOperator(newOp);
}


//class SOP_FeE_SmallPiece_1_0Cache : public SOP_NodeCache
//{
//public:
//    SOP_FeE_SmallPiece_1_0Cache() : SOP_NodeCache(),
//        myPrevOutputDetailID(-1)
//    {}
//    ~SOP_FeE_SmallPiece_1_0Cache() override {}
//
//    int64 myPrevOutputDetailID;
//};


class SOP_FeE_SmallPiece_1_0Verb : public SOP_NodeVerb
{
public:
    SOP_FeE_SmallPiece_1_0Verb() {}
    virtual ~SOP_FeE_SmallPiece_1_0Verb() {}

    virtual SOP_NodeParms *allocParms() const { return new SOP_FeE_SmallPiece_1_0Parms(); }
    //virtual SOP_NodeCache* allocCache() const { return new SOP_FeE_SmallPiece_1_0Cache(); }
    virtual UT_StringHolder name() const { return SOP_FeE_SmallPiece_1_0::theSOPTypeName; }

    virtual CookMode cookMode(const SOP_NodeParms *parms) const { return COOK_GENERIC; }

    virtual void cook(const CookParms &cookparms) const;

    /// This static data member automatically registers
    /// this verb class at library ldir0d time.
    static const SOP_NodeVerb::Register<SOP_FeE_SmallPiece_1_0Verb> theVerb;
};

// The static member variable definition has to be outside the class definition.
// The declaration is inside the class.
const SOP_NodeVerb::Register<SOP_FeE_SmallPiece_1_0Verb> SOP_FeE_SmallPiece_1_0Verb::theVerb;

const SOP_NodeVerb *
SOP_FeE_SmallPiece_1_0::cookVerb() const 
{ 
    return SOP_FeE_SmallPiece_1_0Verb::theVerb.get();
}






static GA_GroupType
sopGroupType(SOP_FeE_SmallPiece_1_0Parms::GroupType parmgrouptype)
{
    using namespace SOP_FeE_SmallPiece_1_0Enums;
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
SOP_FeE_SmallPiece_1_0Verb::cook(const SOP_NodeVerb::CookParms &cookparms) const
{
    auto &&sopparms = cookparms.parms<SOP_FeE_SmallPiece_1_0Parms>();
    GA_Detail& outGeo0 = *cookparms.gdh().gdpNC();
    //auto sopcache = (SOP_FeE_SmallPiece_1_0Cache*)cookparms.cache();

    const GA_Detail& inGeo0 = *cookparms.inputGeo(0);
    outGeo0.replaceWith(inGeo0);


    const GA_GroupType groupType = sopGroupType(sopparms.getGroupType());

    UT_AutoInterrupt boss("Processing");
    if (boss.wasInterrupted())
        return;


    GFE_SmallPiece smallPiece(outGeo0, cookparms);
    smallPiece.findOrCreatePrimitiveGroup(sopparms.getSmallPieceGroupName(), sopparms.getSmallPieceGroupName());

    smallPiece.setComputeParm(
        sopparms.getSubscribeRatio(), sopparms.getMinGrainSize());

    smallPiece.groupParser.setGroup(groupType, sopparms.getGroup());
    smallPiece.computeAndBumpDataId();
    smallPiece.visualizeOutGroup();
    
}

