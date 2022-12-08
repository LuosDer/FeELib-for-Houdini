
//#define UT_ASSERT_LEVEL 3
#include "SOP_FeE_ExtractPoint_1_0.h"


#include "SOP_FeE_ExtractPoint_1_0.proto.h"

#include "GEO/GEO_Detail.h"
#include "PRM/PRM_TemplateBuilder.h"
#include "UT/UT_Interrupt.h"
#include "UT/UT_DSOVersion.h"


#include "GEO_FeE/GEO_FeE_Group.h"
#include "GA_FeE/GA_FeE_VertexNextEquiv.h"
#include "GA_FeE/GA_FeE_Detail.h"

using namespace SOP_FeE_ExtractPoint_1_0_Namespace;



static const char *theDsFile = R"THEDSFILE(
{
    name        parameters
    
    parm {
       name    "extractPoint"
       cppname "ExtractPoint"
       label   "Extract Point"
       type    toggle
       default { 1 }
    }
    parm {
        name    "group"
        cppname "Group"
        label   "Group"
        type    string
        default { "" }
        parmtag { "script_action" "import soputils\nkwargs['geometrytype'] = kwargs['node'].parmTuple('grouptype')\nkwargs['inputindex'] = 0\nsoputils.selectGroupParm(kwargs)" }
        parmtag { "script_action_help" "Select geometry from an available viewport." }
        parmtag { "script_action_icon" "BUTTONS_reselect" }
        disablewhen "{ extractPoint == 0 }"
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
        disablewhen "{ extractPoint == 0 }"
    }
    parm {
        name    "reverseGroup"
        cppname "ReverseGroup"
        label   "Reverse Group"
        type    toggle
        default { 0 }
        disablewhen "{ extractPoint == 0 }"
    }
    


    parm {
        name    "delInputGroup"
        cppname "DelInputGroup"
        label   "Delete Input Group"
        type    toggle
        default { 1 }
        disablewhen "{ extractPoint == 0 }"
    }

    parm {
        name    "delPointMode"
        cppname "DelPointMode"
        label   "Delete Point Mode"
        type    ordinal
        default { "delDegenerateIncompatible" }
        menu {
            "leavePrimitive"             "Leave Primitive"
            "delDegenerate"              "Delete Degenerate"
            "delDegenerateIncompatible"  "Delete Degenerate Incompatible"
        }
        disablewhen "{ extractPoint == 0 }"
    }
    parm {
        name    "guaranteeNoVertexReference"
        cppname "GuaranteeNoVertexReference"
        label   "Guarantee No Vertex Reference"
        type    toggle
        default { 0 }
        disablewhen "{ extractPoint == 0 }"
    }
    parm {
        name    "subscribeRatio"
        cppname "SubscribeRatio"
        label   "Subscribe Ratio"
        type    integer
        default { 16 }
        range   { 0! 256 }
        disablewhen "{ extractPoint == 0 }"
    }
    parm {
        name    "minGrainSize"
        cppname "MinGrainSize"
        label   "Min Grain Size"
        type    intlog
        default { 1024 }
        range   { 0! 2048 }
        disablewhen "{ extractPoint == 0 }"
    }

}
)THEDSFILE";



PRM_Template*
SOP_FeE_ExtractPoint_1_0::buildTemplates()
{
    static PRM_TemplateBuilder templ("SOP_FeE_ExtractPoint_1_0.C"_sh, theDsFile);
    return templ.templates();
}


const UT_StringHolder SOP_FeE_ExtractPoint_1_0::theSOPTypeName("FeE::extractPoint::1.0"_sh);

void
newSopOperator(OP_OperatorTable* table)
{
    OP_Operator* newOp = new OP_Operator(
        SOP_FeE_ExtractPoint_1_0::theSOPTypeName,
        "FeE Extract Point",
        SOP_FeE_ExtractPoint_1_0::myConstructor,
        SOP_FeE_ExtractPoint_1_0::buildTemplates(),
        1,
        1,
        nullptr,
        OP_FLAG_GENERATOR,
        nullptr,
        1,
        "Five elements Elf/Group");

    newOp->setIconName("SOP_add");
    table->addOperator(newOp);

}

static GA_Detail::GA_DestroyPointMode
sopDelPointMode(SOP_FeE_ExtractPoint_1_0Parms::DelPointMode delPointMode)
{
    using namespace SOP_FeE_ExtractPoint_1_0Enums;
    switch (delPointMode)
    {
    case DelPointMode::LEAVEPRIMITIVE:              return GA_Detail::GA_DestroyPointMode::GA_LEAVE_PRIMITIVES;                 break;
    case DelPointMode::DELDEGENERATE:               return GA_Detail::GA_DestroyPointMode::GA_DESTROY_DEGENERATE;               break;
    case DelPointMode::DELDEGENERATEINCOMPATIBLE:   return GA_Detail::GA_DestroyPointMode::GA_DESTROY_DEGENERATE_INCOMPATIBLE;  break;
    }
    UT_ASSERT_MSG(0, "Unhandled Delete Point Mode!");
    return GA_Detail::GA_DestroyPointMode::GA_DESTROY_DEGENERATE_INCOMPATIBLE;
}


//class SOP_FeE_ExtractPoint_1_0Cache : public SOP_NodeCache
//{
//public:
//    SOP_FeE_ExtractPoint_1_0Cache() : SOP_NodeCache(),
//        myPrevOutputDetailID(-1)
//    {}
//    ~SOP_FeE_ExtractPoint_1_0Cache() override {}
//
//    int64 myPrevOutputDetailID;
//};


class SOP_FeE_ExtractPoint_1_0Verb : public SOP_NodeVerb
{
public:
    SOP_FeE_ExtractPoint_1_0Verb() {}
    virtual ~SOP_FeE_ExtractPoint_1_0Verb() {}

    virtual SOP_NodeParms *allocParms() const { return new SOP_FeE_ExtractPoint_1_0Parms(); }
    //virtual SOP_NodeCache* allocCache() const { return new SOP_FeE_ExtractPoint_1_0Cache(); }
    virtual UT_StringHolder name() const { return SOP_FeE_ExtractPoint_1_0::theSOPTypeName; }

    virtual CookMode cookMode(const SOP_NodeParms *parms) const { return COOK_GENERIC; }

    virtual void cook(const CookParms &cookparms) const;

    /// This static data member automatically registers
    /// this verb class at library ldir0d time.
    static const SOP_NodeVerb::Register<SOP_FeE_ExtractPoint_1_0Verb> theVerb;
};

// The static member variable definition has to be outside the class definition.
// The declaration is inside the class.
const SOP_NodeVerb::Register<SOP_FeE_ExtractPoint_1_0Verb> SOP_FeE_ExtractPoint_1_0Verb::theVerb;

const SOP_NodeVerb *
SOP_FeE_ExtractPoint_1_0::cookVerb() const 
{ 
    return SOP_FeE_ExtractPoint_1_0Verb::theVerb.get();
}






static GA_GroupType
sopGroupType(SOP_FeE_ExtractPoint_1_0Parms::GroupType parmgrouptype)
{
    using namespace SOP_FeE_ExtractPoint_1_0Enums;
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
SOP_FeE_ExtractPoint_1_0Verb::cook(const SOP_NodeVerb::CookParms &cookparms) const
{
    auto &&sopparms = cookparms.parms<SOP_FeE_ExtractPoint_1_0Parms>();
    GEO_Detail* outGeo0 = cookparms.gdh().gdpNC();
    //auto sopcache = (SOP_FeE_ExtractPoint_1_0Cache*)cookparms.cache();

    const GA_Detail* const inGeo0 = cookparms.inputGeo(0);

    
    if (!sopparms.getExtractPoint())
    {
        outGeo0->replaceWith(*inGeo0);
        return;
    }
    GA_AttributeFilter::selectGroup()
    const GA_AttributeFilter attribFilter = GA_AttributeFilter::selectByName();
    outGeo0->replaceWithPoints(*inGeo0);

    //const GA_GroupType groupType = sopGroupType(sopparms.getGroupType());

    const bool reverseGroup = sopparms.getReverseGroup();

    const UT_StringHolder& groupName = sopparms.getGroup();

    GOP_Manager gop;
    //onst GA_Group* geo0Group = GA_FeE_Group::findOrParseGroupDetached(cookparms, outGeo0, groupType, sopparms.getGroup(), gop);
    const GA_PointGroup* geo0Group = GA_FeE_Group::findOrParsePointGroupDetached(cookparms, outGeo0, groupName, gop);
    //notifyGroupParmListeners(cookparms.getNode(), 0, 1, outGeo0, geo0Group);
    if (geo0Group && !reverseGroup && geo0Group->isEmpty())
        return;


    UT_AutoInterrupt boss("Processing");
    if (boss.wasInterrupted())
        return;

    
    //const GA_Detail::GA_DestroyPointMode delPointMode = sopDelPointMode(sopparms.getDelPointMode());
    //outGeo0->destroyPointOffsets(GA_Range(outGeo0->getPointMap(), geo0Group, reverseGroup), delPointMode, sopparms.getGuaranteeNoVertexReference());
    outGeo0->destroyPointOffsets(GA_Range(outGeo0->getPointMap(), geo0Group, reverseGroup), GA_Detail::GA_DestroyPointMode::GA_LEAVE_PRIMITIVES, true);

    const bool delInputGroup = sopparms.getDelInputGroup();
    if (delInputGroup)
    {
        GA_PointGroup* geo0PointGroup = outGeo0->findPointGroup(groupName);
        if (geo0PointGroup)
        {
            outGeo0->destroyGroup(geo0PointGroup);
        }
    }
    outGeo0->bumpDataIdsForAddOrRemove(1,0,0);

    //const exint subscribeRatio = sopparms.getSubscribeRatio();
    //const exint minGrainSize = sopparms.getMinGrainSize();
    //const exint minGrainSize = pow(2, 8);
    //const exint minGrainSize = pow(2, 4);


    //const GA_Storage& inStorgeF = SYSisSame<T, fpreal32>() ? GA_STORE_REAL32 : GA_STORE_REAL64;
    //const GA_Storage inStorgeF = GA_STORE_REAL32;
    //const GA_Storage inStorgeI = GA_FeE_Type::getPreferredStorageI(outGeo0);
}



namespace SOP_FeE_ExtractPoint_1_0_Namespace {

} // End SOP_FeE_ExtractPoint_1_0_Namespace namespace
