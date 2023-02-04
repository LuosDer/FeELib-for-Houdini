
//#define UT_ASSERT_LEVEL 3
#include "SOP_FeE_RestDir2D_1_0.h"


#include "SOP_FeE_RestDir2D_1_0.proto.h"

#include "GA/GA_Detail.h"
#include "PRM/PRM_TemplateBuilder.h"
#include "UT/UT_Interrupt.h"
#include "UT/UT_DSOVersion.h"


#include "GA_FeE/GA_FeE_RestDir2D.h"

using namespace SOP_FeE_RestDir2D_1_0_Namespace;



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
        name    "restDir2DMethod"
        cppname "RestDir2DMethod"
        label   "Method"
        type    ordinal
        default { "avgNormal" }
        menu {
            "avgNormal"  "AvgNormal"
            "houOBB"     "Houdini OBB"
        }
    }

    parm {
        name    "restDir2DAttribName"
        cppname "RestDir2DAttribName"
        label   "Rest Dir 2D Attribute Name"
        type    string
        default { "restDir2D" }
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
        default { "0" }
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
SOP_FeE_RestDir2D_1_0::buildTemplates()
{
    static PRM_TemplateBuilder templ("SOP_FeE_RestDir2D_1_0.C"_sh, theDsFile);
    if (templ.justBuilt())
    {
        templ.setChoiceListPtr("group"_sh, &SOP_Node::allGroupMenu);
    }
    return templ.templates();
}


const UT_StringHolder SOP_FeE_RestDir2D_1_0::theSOPTypeName("FeE::groupByPolyWinding::1.0"_sh);

void
newSopOperator(OP_OperatorTable* table)
{
    OP_Operator* newOp = new OP_Operator(
        SOP_FeE_RestDir2D_1_0::theSOPTypeName,
        "FeE Group by Polygon Winding",
        SOP_FeE_RestDir2D_1_0::myConstructor,
        SOP_FeE_RestDir2D_1_0::buildTemplates(),
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

//class SOP_FeE_RestDir2D_1_0Cache : public SOP_NodeCache
//{
//public:
//    SOP_FeE_RestDir2D_1_0Cache() : SOP_NodeCache(),
//        myPrevOutputDetailID(-1)
//    {}
//    ~SOP_FeE_RestDir2D_1_0Cache() override {}
//
//    int64 myPrevOutputDetailID;
//};


class SOP_FeE_RestDir2D_1_0Verb : public SOP_NodeVerb
{
public:
    SOP_FeE_RestDir2D_1_0Verb() {}
    virtual ~SOP_FeE_RestDir2D_1_0Verb() {}

    virtual SOP_NodeParms *allocParms() const { return new SOP_FeE_RestDir2D_1_0Parms(); }
    //virtual SOP_NodeCache* allocCache() const { return new SOP_FeE_RestDir2D_1_0Cache(); }
    virtual UT_StringHolder name() const { return SOP_FeE_RestDir2D_1_0::theSOPTypeName; }

    virtual CookMode cookMode(const SOP_NodeParms *parms) const { return COOK_GENERIC; }

    virtual void cook(const CookParms &cookparms) const;

    /// This static data member automatically registers
    /// this verb class at library ldir0d time.
    static const SOP_NodeVerb::Register<SOP_FeE_RestDir2D_1_0Verb> theVerb;
};

// The static member variable definition has to be outside the class definition.
// The declaration is inside the class.
const SOP_NodeVerb::Register<SOP_FeE_RestDir2D_1_0Verb> SOP_FeE_RestDir2D_1_0Verb::theVerb;

const SOP_NodeVerb *
SOP_FeE_RestDir2D_1_0::cookVerb() const 
{ 
    return SOP_FeE_RestDir2D_1_0Verb::theVerb.get();
}






static GA_GroupType
sopGroupType(SOP_FeE_RestDir2D_1_0Parms::GroupType parmgrouptype)
{
    using namespace SOP_FeE_RestDir2D_1_0Enums;
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


static GA_FeE_RestDir2D_Method
sopMethod(SOP_FeE_RestDir2D_1_0Parms::RestDir2DMethod parmgrouptype)
{
    using namespace SOP_FeE_RestDir2D_1_0Enums;
    switch (parmgrouptype)
    {
    case RestDir2DMethod::AVGNORMAL:   return GA_FeE_RestDir2D_AvgNormal;   break;
    case RestDir2DMethod::HOUOBB:      return GA_FeE_RestDir2D_HouOBB;   break;
    }
    UT_ASSERT_MSG(0, "Unhandled RestDir2DMethod!");
    return GA_FeE_RestDir2D_AvgNormal;
}


void
SOP_FeE_RestDir2D_1_0Verb::cook(const SOP_NodeVerb::CookParms& cookparms) const
{
    auto&& sopparms = cookparms.parms<SOP_FeE_RestDir2D_1_0Parms>();
    GA_Detail* const outGeo0 = cookparms.gdh().gdpNC();
    //auto sopcache = (SOP_FeE_RestDir2D_1_0Cache*)cookparms.cache();

    const GA_Detail* const inGeo0 = cookparms.inputGeo(0);

    outGeo0->replaceWith(*inGeo0);

    UT_AutoInterrupt boss("Processing");
    if (boss.wasInterrupted())
        return;

    const GA_GroupType groupType = sopGroupType(sopparms.getGroupType());
    const UT_StringHolder& groupName = sopparms.getGroup();

    const bool runOverPieces = sopparms.getRunOverPieces();
    const bool findInputPieceAttrib = sopparms.getFindInputPieceAttrib();
    const UT_StringHolder& pieceAttribName = sopparms.getPieceAttribName();

    const UT_StringHolder& restDir2DAttribName = sopparms.getRestDir2DAttribName();
    
    const GA_FeE_RestDir2D_Method method = sopMethod(sopparms.getRestDir2DMethod());

    const GA_Storage inStorgeF = GA_FeE_Type::getPreferredStorageF(outGeo0);
    const exint subscribeRatio = sopparms.getSubscribeRatio();
    const exint minGrainSize = sopparms.getMinGrainSize();


    GA_Attribute* const restDir2DAttrib = GA_FeE_RestDir2D::addAttribRestDir2D(cookparms, outGeo0, groupType, groupName,
        method, inStorgeF, restDir2DAttribName,
        nullptr, nullptr, GA_ReuseStrategy(),
        subscribeRatio, minGrainSize);

    restDir2DAttrib->bumpDataId();


}



namespace SOP_FeE_RestDir2D_1_0_Namespace {

} // End SOP_FeE_RestDir2D_1_0_Namespace namespace
