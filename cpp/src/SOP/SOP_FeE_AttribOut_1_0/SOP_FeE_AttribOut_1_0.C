//#define UT_ASSERT_LEVEL 3
#include "SOP_FeE_AttribOut_1_0.h"

#include "SOP_FeE_AttribOut_1_0.proto.h"

#include "GA/GA_Detail.h"
#include "PRM/PRM_TemplateBuilder.h"
#include "UT/UT_Interrupt.h"
#include "UT/UT_DSOVersion.h"


#include "GFE/GFE_Detail.h"


using namespace SOP_FeE_AttribOut_1_0_Namespace;


static const char* theDsFile = R"THEDSFILE(
{
    name        parameters

    parm {
        name    "outAttrib#"
        cppname "OutAttrib#"
        label   "Out Attrib"
        type    toggle
        default { 1 }
        nolabel
        joinnext
    }
    parm {
        name    "primAttrib"
        cppname "PrimAttrib"
        label   "Prim Attrib"
        type    string
        default { "" }
        disablewhen "{ outPrimAttrib == 0 }"
    }
    parm {
        name    "newPrimAttribName"
        cppname "NewPrimAttribName"
        label   "New Prim Attrib Name"
        type    string
        default { "" }
        disablewhen "{ outPrimAttrib == 1 }"
    }




}
)THEDSFILE";


PRM_Template*
SOP_FeE_AttribOut_1_0::buildTemplates()
{
    static PRM_TemplateBuilder templ("SOP_FeE_AttribOut_1_0.C"_sh, theDsFile);
    if (templ.justBuilt())
    {
        templ.setChoiceListPtr("delPrimAttrib"_sh,    &SOP_Node::primAttribMenu);
        templ.setChoiceListPtr("delPointAttrib"_sh,   &SOP_Node::pointAttribMenu);
        templ.setChoiceListPtr("delVertexAttrib"_sh,  &SOP_Node::vertexAttribMenu);
        templ.setChoiceListPtr("delDetailAttrib"_sh,  &SOP_Node::detailAttribMenu);

        templ.setChoiceListPtr("delPrimGroup"_sh,     &SOP_Node::primNamedGroupMenu);
        templ.setChoiceListPtr("delPointGroup"_sh,    &SOP_Node::pointNamedGroupMenu);
        templ.setChoiceListPtr("delVertexGroup"_sh,   &SOP_Node::vertexNamedGroupMenu);
        templ.setChoiceListPtr("delEdgeGroup"_sh,     &SOP_Node::edgeNamedGroupMenu);
    }
    return templ.templates();
}

const UT_StringHolder SOP_FeE_AttribOut_1_0::theSOPTypeName("FeE::attribOut::1.0"_sh);

void
newSopOperator(OP_OperatorTable* table)
{
    OP_Operator* newOp = new OP_Operator(
        SOP_FeE_AttribOut_1_0::theSOPTypeName,
        "FeE Attribute Output",
        SOP_FeE_AttribOut_1_0::myConstructor,
        SOP_FeE_AttribOut_1_0::buildTemplates(),
        1,
        1,
        nullptr,
        OP_FLAG_GENERATOR,
        nullptr,
        1,
        "Five elements Elf/Attribute");

    newOp->setIconName("SOP_attribute");
    table->addOperator(newOp);
}

//class SOP_FeE_AttribOut_1_0Cache : public SOP_NodeCache
//{
//public:
//    SOP_FeE_AttribOut_1_0Cache() : SOP_NodeCache(),
//        myPrevOutputDetailID(-1)
//    {}
//    ~SOP_FeE_AttribOut_1_0Cache() override {}
//
//    int64 myPrevOutputDetailID;
//};


class SOP_FeE_AttribOut_1_0Verb : public SOP_NodeVerb
{
public:
    SOP_FeE_AttribOut_1_0Verb()
    {
    }

    virtual ~SOP_FeE_AttribOut_1_0Verb()
    {
    }

    virtual SOP_NodeParms* allocParms() const { return new SOP_FeE_AttribOut_1_0Parms(); }
    //virtual SOP_NodeCache* allocCache() const { return new SOP_FeE_AttribOut_1_0Cache(); }
    virtual UT_StringHolder name() const { return SOP_FeE_AttribOut_1_0::theSOPTypeName; }

    virtual CookMode cookMode(const SOP_NodeParms* parms) const { return COOK_GENERIC; }

    virtual void cook(const CookParms& cookparms) const;

    /// This static data member automatically registers
    /// this verb class at library ldir0d time.
    static const SOP_NodeVerb::Register<SOP_FeE_AttribOut_1_0Verb> theVerb;
};

// The static member variable definition has to be outside the class definition.
// The declaration is inside the class.
const SOP_NodeVerb::Register<SOP_FeE_AttribOut_1_0Verb> SOP_FeE_AttribOut_1_0Verb::theVerb;

const SOP_NodeVerb*
SOP_FeE_AttribOut_1_0::cookVerb() const
{
    return SOP_FeE_AttribOut_1_0Verb::theVerb.get();
}





void
SOP_FeE_AttribOut_1_0Verb::cook(const SOP_NodeVerb::CookParms& cookparms) const
{
    auto&& sopparms = cookparms.parms<SOP_FeE_AttribOut_1_0Parms>();
    GA_Detail& outGeo0 = *cookparms.gdh().gdpNC();
    //auto sopcache = (SOP_FeE_AttribOut_1_0Cache*)cookparms.cache();

    const GA_Detail& inGeo0 = *cookparms.inputGeo(0);

    outGeo0.replaceWith(inGeo0);


    UT_AutoInterrupt boss("Processing");
    if (boss.wasInterrupted())
        return;


    GFE_Detail& outGFE0 = static_cast<GFE_Detail&>(outGeo0);

    for ()
    outGFE0.outAttrib(delPrimGroup, delPointGroup, delVertexGroup, delEdgeGroup);

    const UT_StringHolder& allPattern = "*";
    const UT_StringHolder& delPrimAttrib   = sopparms.getDoDelPrimAttrib()   ? sopparms.getDelPrimAttrib()   : allPattern;
    const UT_StringHolder& delPointAttrib  = sopparms.getDoDelPointAttrib()  ? sopparms.getDelPointAttrib()  : allPattern;
    const UT_StringHolder& delVertexAttrib = sopparms.getDoDelVertexAttrib() ? sopparms.getDelVertexAttrib() : allPattern;
    const UT_StringHolder& delDetailAttrib = sopparms.getDoDelDetailAttrib() ? sopparms.getDelDetailAttrib() : allPattern;
    outGFE0.delStdAttribute(delPrimAttrib, delPointAttrib, delVertexAttrib, delDetailAttrib);
    //GA_AttributeSet& attribSet = outGeo0.getAttributes();
    //GFE_Attribute::delStdAttribute(attribSet, attribSet, delPrimAttrib, delPointAttrib, delVertexAttrib, delDetailAttrib);

    const UT_StringHolder& delPrimGroup   = sopparms.getDoDelPrimGroup()   ? sopparms.getDelPrimGroup()   : allPattern;
    const UT_StringHolder& delPointGroup  = sopparms.getDoDelPointGroup()  ? sopparms.getDelPointGroup()  : allPattern;
    const UT_StringHolder& delVertexGroup = sopparms.getDoDelVertexGroup() ? sopparms.getDelVertexGroup() : allPattern;
    const UT_StringHolder& delEdgeGroup   = sopparms.getDoDelEdgeGroup()   ? sopparms.getDelEdgeGroup()   : allPattern;
    outGFE0.delStdGroup(delPrimGroup, delPointGroup, delVertexGroup, delEdgeGroup);
}


namespace SOP_FeE_AttribOut_1_0_Namespace
{
} // End SOP_FeE_AttribOut_1_0_Namespace namespace
