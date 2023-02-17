
//#define UT_ASSERT_LEVEL 3
#include "SOP_FeE_GroupByMeshWinding_1_0.h"


#include "SOP_FeE_GroupByMeshWinding_1_0.proto.h"

#include "GA/GA_Detail.h"
#include "PRM/PRM_TemplateBuilder.h"
#include "UT/UT_Interrupt.h"
#include "UT/UT_DSOVersion.h"



#include "GA_FeE/GA_FeE_GroupByMeshWinding.h"

using namespace SOP_FeE_GroupByMeshWinding_1_0_Namespace;



static const char *theDsFile = R"THEDSFILE(
{
    name        parameters
    
    parm {
        name    "primGroup"
        cppname "PrimGroup"
        label   "Prim Group"
        type    string
        default { "" }
        parmtag { "script_action" "import soputils\nkwargs['geometrytype'] = (hou.geometryType.Primitives,)\nkwargs['inputindex'] = 0\nsoputils.selectGroupParm(kwargs)" }
        parmtag { "script_action_help" "Select geometry from an available viewport.\nShift-click to turn on Select Groups." }
        parmtag { "script_action_icon" "BUTTONS_reselect" }
        parmtag { "sop_input" "0" }
    }
    parm {
        name    "runOverPieces"
        cppname "RunOverPieces"
        label   "Run Over Pieces"
        type    toggle
        default { "0" }
    }
    parm {
        name    "findInputPieceAttrib"
        cppname "FindInputPieceAttrib"
        label   "Find Input Piece Attribute"
        type    toggle
        default { "0" }
        disablewhen "{ runOverPieces == 0 }"
    }
    parm {
        name    "pieceAttribName"
        cppname "PieceAttribName"
        label   "Piece Attribute Name"
        type    string
        default { "class" }
        disablewhen "{ runOverPieces == 0 }"
    }
    parm {
        name    "polyCap"
        cppname "PolyCap"
        label   "Poly Cap"
        type    toggle
        default { "0" }
    }
    parm {
        name    "outReversedGroup"
        cppname "OutReversedGroup"
        label   "Output Reversed Group"
        type    toggle
        default { "1" }
    }
    parm {
        name    "reversedGroupName"
        cppname "ReversedGroupName"
        label   "Reversed Group Name"
        type    string
        default { "reversed" }
        disablewhen "{ outReversedGroup == 0 }"
    }
    parm {
        name    "reverseGroup"
        cppname "ReverseGroup"
        label   "Reverse Group"
        type    toggle
        default { "0" }
    }
    parm {
        name    "reversePrim"
        cppname "ReversePrim"
        label   "Reverse Prim"
        type    toggle
        default { "1" }
    }
    //parm {
    //     name    "deleteGroup"
    //     cppname "DeleteGroup"
    //     label   "Delete Group"
    //     type    toggle
    //     default { "0" }
    // }


    parm {
        name    "groupByMeshWindingMethod"
        cppname "GroupByMeshWindingMethod"
        label   "Method"
        type    ordinal
        default { "volume" }
        menu {
            "volume"  "Volume"
            "wn"      "Winding Number"
        }
    }
    //parm {
    //    name    "subscribeRatio"
    //    cppname "SubscribeRatio"
    //    label   "Subscribe Ratio"
    //    type    integer
    //    default { 16 }
    //    range   { 0! 256 }
    //    invisible
    //}
    //parm {
    //    name    "minGrainSize"
    //    cppname "MinGrainSize"
    //    label   "Min Grain Size"
    //    type    intlog
    //    default { 1024 }
    //    range   { 0! 2048 }
    //    invisible
    //}
}
)THEDSFILE";



PRM_Template*
SOP_FeE_GroupByMeshWinding_1_0::buildTemplates()
{
    static PRM_TemplateBuilder templ("SOP_FeE_GroupByMeshWinding_1_0.C"_sh, theDsFile);
    if (templ.justBuilt())
    {
        templ.setChoiceListPtr("primGroup"_sh, &SOP_Node::primGroupMenu);
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






static GFE_GroupByMeshWindingMethod
sopMethod(SOP_FeE_GroupByMeshWinding_1_0Parms::GroupByMeshWindingMethod parmgrouptype)
{
    using namespace SOP_FeE_GroupByMeshWinding_1_0Enums;
    switch (parmgrouptype)
    {
    case GroupByMeshWindingMethod::VOLUME:     return GFE_GroupByMeshWindingMethod_VOLUME;         break;
    case GroupByMeshWindingMethod::WN:         return GFE_GroupByMeshWindingMethod_WINDINGNUMBER;  break;
    }
    UT_ASSERT_MSG(0, "Unhandled GA_FeE_GroupByMeshWinding METHOD!");
    return GFE_GroupByMeshWindingMethod_VOLUME;
}



void
SOP_FeE_GroupByMeshWinding_1_0Verb::cook(const SOP_NodeVerb::CookParms& cookparms) const
{
    auto&& sopparms = cookparms.parms<SOP_FeE_GroupByMeshWinding_1_0Parms>();
    GA_Detail* const outGeo0 = cookparms.gdh().gdpNC();
    //auto sopcache = (SOP_FeE_GroupByMeshWinding_1_0Cache*)cookparms.cache();

    const GA_Detail* const inGeo0 = cookparms.inputGeo(0);
    
    outGeo0->replaceWith(*inGeo0);

    UT_AutoInterrupt boss("Processing");
    if (boss.wasInterrupted())
        return;

    const UT_StringHolder& primGroupName = sopparms.getPrimGroup();
    const GFE_GroupByMeshWindingMethod method = sopMethod(sopparms.getGroupByMeshWindingMethod());

    
    const bool runOverPieces = sopparms.getRunOverPieces();
    const bool findInputPieceAttrib = sopparms.getFindInputPieceAttrib();
    const UT_StringHolder& pieceAttribName = sopparms.getPieceAttribName();


    const bool outReversedGroup = sopparms.getOutReversedGroup();
    const UT_StringHolder& reversedGroupName = sopparms.getReversedGroupName();

    const bool reverseGroup = sopparms.getReverseGroup();
    const bool reversePrim = sopparms.getReversePrim();
    const bool polyCap = sopparms.getPolyCap();
    //const bool delAttrib = sopparms.getDelAttrib();
    
    GA_FeE_GroupByMeshWinding::groupByMeshWinding(cookparms, outGeo0, primGroupName, method,
        runOverPieces, findInputPieceAttrib, pieceAttribName,
        outReversedGroup, reversedGroupName, reverseGroup, reversePrim, polyCap);

    //const exint subscribeRatio = sopparms.getSubscribeRatio();
    //const exint minGrainSize = sopparms.getMinGrainSize();


    //const GA_Storage inStorgeI = GA_FeE_Type::getPreferredStorageI(outGeo0);
}



namespace SOP_FeE_GroupByMeshWinding_1_0_Namespace {

} // End SOP_FeE_GroupByMeshWinding_1_0_Namespace namespace
