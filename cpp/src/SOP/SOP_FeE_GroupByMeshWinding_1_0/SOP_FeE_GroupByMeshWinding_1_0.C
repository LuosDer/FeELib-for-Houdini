
//#define UT_ASSERT_LEVEL 3
#include "SOP_FeE_GroupByMeshWinding_1_0.h"


#include "SOP_FeE_GroupByMeshWinding_1_0.proto.h"

#include "GEO/GEO_Detail.h"
#include "PRM/PRM_TemplateBuilder.h"
#include "UT/UT_Interrupt.h"
#include "UT/UT_DSOVersion.h"


#include "GA_FeE/GA_FeE_Detail.h"
#include "GA_FeE/GA_FeE_Attribute.h"

using namespace SOP_FeE_GroupByMeshWinding_1_0_Namespace;



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
    groupsimple {
        name    "extractPoint_folder"
        label   "Extract Point"
        disablewhentab "{ extractPoint == 0 }"

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
            default { "point" }
            menu {
                "guess"     "Guess from Group"
                "prim"      "Primitive"
                "point"     "Point"
                "vertex"    "Vertex"
                "edge"      "Edge"
            }
        }
        parm {
            name    "reverseGroup"
            cppname "ReverseGroup"
            label   "Reverse Group"
            type    toggle
            default { 0 }
        }
    


        parm {
            name    "delInputGroup"
            cppname "DelInputGroup"
            label   "Delete Input Group"
            type    toggle
            default { 1 }
        }

        parm {
            name    "delPrimAttrib"
            cppname "DelPrimAttrib"
            label   "Delete Prim Attributes"
            type    string
            default { "*" }
        }
        parm {
            name    "delPointAttrib"
            cppname "DelPointAttrib"
            label   "Delete Point Attributes"
            type    string
            default { "" }
        }
        parm {
            name    "delVertexAttrib"
            cppname "DelVertexAttrib"
            label   "Delete Vertex Attributes"
            type    string
            default { "*" }
        }
        parm {
            name    "delDetailAttrib"
            cppname "DelDetailAttrib"
            label   "Delete Detail Attributes"
            type    string
            default { "*" }
        }
        parm {
            name    "delPrimGroup"
            cppname "DelPrimGroup"
            label   "Delete Prim Group"
            type    string
            default { "*" }
        }
        parm {
            name    "delPointGroup"
            cppname "DelPointGroup"
            label   "Delete Point Group"
            type    string
            default { "" }
        }
        parm {
            name    "delVertexGroup"
            cppname "DelVertexGroup"
            label   "Delete Vertex Group"
            type    string
            default { "*" }
        }
        parm {
            name    "delEdgeGroup"
            cppname "DelEdgeGroup"
            label   "Delete Edge Group"
            type    string
            default { "*" }
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
            invisible
        }
        parm {
            name    "guaranteeNoVertexReference"
            cppname "GuaranteeNoVertexReference"
            label   "Guarantee No Vertex Reference"
            type    toggle
            default { 0 }
            invisible
        }
    }

    parm {
        name    "kernel"
        cppname "Kernel"
        label   "Kernel"
        type    integer
        default { 1 }
        range   { 0! 2! }
    }
    parm {
        name    "subscribeRatio"
        cppname "SubscribeRatio"
        label   "Subscribe Ratio"
        type    integer
        default { 16 }
        range   { 0! 256 }
        invisible
    }
    parm {
        name    "minGrainSize"
        cppname "MinGrainSize"
        label   "Min Grain Size"
        type    intlog
        default { 1024 }
        range   { 0! 2048 }
        invisible
    }
}
)THEDSFILE";



PRM_Template*
SOP_FeE_GroupByMeshWinding_1_0::buildTemplates()
{
    static PRM_TemplateBuilder templ("SOP_FeE_GroupByMeshWinding_1_0.C"_sh, theDsFile);
    if (templ.justBuilt())
    {
        templ.setChoiceListPtr("group"_sh, &SOP_Node::allGroupMenu);
    }
    return templ.templates();
}


const UT_StringHolder SOP_FeE_GroupByMeshWinding_1_0::theSOPTypeName("FeE::groupByMeshWinding::1.0"_sh);

void
newSopOperator(OP_OperatorTable* table)
{
    OP_Operator* newOp = new OP_Operator(
        SOP_FeE_GroupByMeshWinding_1_0::theSOPTypeName,
        "FeE Group by Mesh Winding",
        SOP_FeE_GroupByMeshWinding_1_0::myConstructor,
        SOP_FeE_GroupByMeshWinding_1_0::buildTemplates(),
        1,
        1,
        nullptr,
        OP_FLAG_GENERATOR,
        nullptr,
        1,
        "Five elements Elf/Data/Orient");

    newOp->setIconName("SOP_normal");
    table->addOperator(newOp);

}

static GA_Detail::GA_DestroyPointMode
sopDelPointMode(SOP_FeE_GroupByMeshWinding_1_0Parms::DelPointMode delPointMode)
{
    using namespace SOP_FeE_GroupByMeshWinding_1_0Enums;
    switch (delPointMode)
    {
    case DelPointMode::LEAVEPRIMITIVE:              return GA_Detail::GA_DestroyPointMode::GA_LEAVE_PRIMITIVES;                 break;
    case DelPointMode::DELDEGENERATE:               return GA_Detail::GA_DestroyPointMode::GA_DESTROY_DEGENERATE;               break;
    case DelPointMode::DELDEGENERATEINCOMPATIBLE:   return GA_Detail::GA_DestroyPointMode::GA_DESTROY_DEGENERATE_INCOMPATIBLE;  break;
    }
    UT_ASSERT_MSG(0, "Unhandled Delete Point Mode!");
    return GA_Detail::GA_DestroyPointMode::GA_DESTROY_DEGENERATE_INCOMPATIBLE;
}


//class SOP_FeE_GroupByMeshWinding_1_0Cache : public SOP_NodeCache
//{
//public:
//    SOP_FeE_GroupByMeshWinding_1_0Cache() : SOP_NodeCache(),
//        myPrevOutputDetailID(-1)
//    {}
//    ~SOP_FeE_GroupByMeshWinding_1_0Cache() override {}
//
//    int64 myPrevOutputDetailID;
//};


class SOP_FeE_GroupByMeshWinding_1_0Verb : public SOP_NodeVerb
{
public:
    SOP_FeE_GroupByMeshWinding_1_0Verb() {}
    virtual ~SOP_FeE_GroupByMeshWinding_1_0Verb() {}

    virtual SOP_NodeParms *allocParms() const { return new SOP_FeE_GroupByMeshWinding_1_0Parms(); }
    //virtual SOP_NodeCache* allocCache() const { return new SOP_FeE_GroupByMeshWinding_1_0Cache(); }
    virtual UT_StringHolder name() const { return SOP_FeE_GroupByMeshWinding_1_0::theSOPTypeName; }

    virtual CookMode cookMode(const SOP_NodeParms *parms) const { return COOK_GENERIC; }

    virtual void cook(const CookParms &cookparms) const;

    /// This static data member automatically registers
    /// this verb class at library ldir0d time.
    static const SOP_NodeVerb::Register<SOP_FeE_GroupByMeshWinding_1_0Verb> theVerb;
};

// The static member variable definition has to be outside the class definition.
// The declaration is inside the class.
const SOP_NodeVerb::Register<SOP_FeE_GroupByMeshWinding_1_0Verb> SOP_FeE_GroupByMeshWinding_1_0Verb::theVerb;

const SOP_NodeVerb *
SOP_FeE_GroupByMeshWinding_1_0::cookVerb() const 
{ 
    return SOP_FeE_GroupByMeshWinding_1_0Verb::theVerb.get();
}






static GA_GroupType
sopGroupType(SOP_FeE_GroupByMeshWinding_1_0Parms::GroupType parmgrouptype)
{
    using namespace SOP_FeE_GroupByMeshWinding_1_0Enums;
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
SOP_FeE_GroupByMeshWinding_1_0Verb::cook(const SOP_NodeVerb::CookParms& cookparms) const
{
    auto&& sopparms = cookparms.parms<SOP_FeE_GroupByMeshWinding_1_0Parms>();
    GEO_Detail* outGeo0 = cookparms.gdh().gdpNC();
    //auto sopcache = (SOP_FeE_GroupByMeshWinding_1_0Cache*)cookparms.cache();

    const GEO_Detail* const inGeo0 = cookparms.inputGeo(0);

    if (!sopparms.getExtractPoint())
    {
        outGeo0->replaceWith(*inGeo0);
        return;
    }


    const UT_StringHolder& groupName = sopparms.getGroup();

    const UT_StringHolder& delPrimAttrib = sopparms.getDelPrimAttrib();
    const UT_StringHolder& delPointAttrib = sopparms.getDelPointAttrib();
    const UT_StringHolder& delVertexAttrib = sopparms.getDelVertexAttrib();
    const UT_StringHolder& delDetailAttrib = sopparms.getDelDetailAttrib();

    const UT_StringHolder& delPrimGroup = sopparms.getDelPrimGroup();
    const UT_StringHolder& delPointGroup = sopparms.getDelPointGroup();
    const UT_StringHolder& delVertexGroup = sopparms.getDelVertexGroup();
    const UT_StringHolder& delEdgeGroup = sopparms.getDelEdgeGroup();
    //const GA_GroupType groupType = sopGroupType(sopparms.getGroupType());

    UT_AutoInterrupt boss("Processing");
    if (boss.wasInterrupted())
        return;

#if 1
    GEO_FeE_Detail::extractPoint(cookparms, outGeo0, inGeo0, groupName,
        delPrimAttrib, delPointAttrib, delVertexAttrib, delDetailAttrib,
        delPrimGroup, delPointGroup, delVertexGroup, delEdgeGroup,
        sopparms.getReverseGroup(), sopparms.getDelInputGroup());
#else
    GA_FeE_Attribute::delStdAttribute(outGeo0, delPrimAttrib, delPointAttrib, delVertexAttrib, delDetailAttrib);
    GEO_FeE_Detail::extractPoint(cookparms, outGeo0, groupName,
        delPrimGroup, delPointGroup, delVertexGroup, delEdgeGroup,
        sopparms.getReverseGroup(), sopparms.getDelInputGroup());
#endif

    outGeo0->bumpDataIdsForAddOrRemove(1, 0, 0);

    //const exint subscribeRatio = sopparms.getSubscribeRatio();
    //const exint minGrainSize = sopparms.getMinGrainSize();


    //const GA_Storage inStorgeI = GA_FeE_Type::getPreferredStorageI(outGeo0);
}



namespace SOP_FeE_GroupByMeshWinding_1_0_Namespace {

} // End SOP_FeE_GroupByMeshWinding_1_0_Namespace namespace
