
//#define UT_ASSERT_LEVEL 3
#include "SOP_FeE_AttribDuplicate_1_0.h"


// This is an automatically generated header file based on theDsFile, below,
// to provide SOP_FeE_AttribDuplicate_1_0Parms, an easy way to access parameter values from
// SOP_FeE_AttribDuplicate_1_0Verb::cook with the correct type.
#include "SOP_FeE_AttribDuplicate_1_0.proto.h"

#include "GA/GA_Detail.h"
#include "PRM/PRM_TemplateBuilder.h"
#include "UT/UT_Interrupt.h"
#include "UT/UT_DSOVersion.h"


#include "GFE/GFE_AttributeDuplicate.h"



using namespace SOP_FeE_AttribDuplicate_1_0_Namespace;


static const char *theDsFile = R"THEDSFILE(
{
    name        parameters

    parm {
        name    "srcAttribClass"
        cppname "SrcAttribClass"
        label   "Source Attribute Class"
        type    toggle
        default { "" }
    }

    parm {
        name    "srcAttrib"
        cppname "SrcAttrib"
        label   "Source Attribute"
        type    toggle
        default { "" }
    }

    parm {
        name    "outAsOffset"
        cppname "OutAsOffset"
        label   "Output as Offset"
        type    toggle
        default { "1" }
    }



    parm {
        name    "outTopoAttrib"
        cppname "OutTopoAttrib"
        label   "Output Topo Attribute"
        type    toggle
        default { "0" }
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
SOP_FeE_AttribDuplicate_1_0::buildTemplates()
{
    static PRM_TemplateBuilder templ("SOP_FeE_AttribDuplicate_1_0.C"_sh, theDsFile);
    if (templ.justBuilt())
    {
        templ.setChoiceListPtr("srcAttrib"_sh, &SOP_Node::allAttribMenu);
    }
    return templ.templates();
}

const UT_StringHolder SOP_FeE_AttribDuplicate_1_0::theSOPTypeName("FeE::attribDuplicate::1.0"_sh);

void
newSopOperator(OP_OperatorTable* table)
{
    OP_Operator* newOp = new OP_Operator(
        SOP_FeE_AttribDuplicate_1_0::theSOPTypeName,
        "FeE Attribute Duplicate",
        SOP_FeE_AttribDuplicate_1_0::myConstructor,
        SOP_FeE_AttribDuplicate_1_0::buildTemplates(),
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

//class SOP_FeE_AttribDuplicate_1_0Cache : public SOP_NodeCache
//{
//public:
//    SOP_FeE_AttribDuplicate_1_0Cache() : SOP_NodeCache(),
//        myPrevOutputDetailID(-1)
//    {}
//    ~SOP_FeE_AttribDuplicate_1_0Cache() override {}
//
//    int64 myPrevOutputDetailID;
//};


class SOP_FeE_AttribDuplicate_1_0Verb : public SOP_NodeVerb
{
public:
    SOP_FeE_AttribDuplicate_1_0Verb() {}
    virtual ~SOP_FeE_AttribDuplicate_1_0Verb() {}

    virtual SOP_NodeParms *allocParms() const { return new SOP_FeE_AttribDuplicate_1_0Parms(); }
    //virtual SOP_NodeCache* allocCache() const { return new SOP_FeE_AttribDuplicate_1_0Cache(); }
    virtual UT_StringHolder name() const { return SOP_FeE_AttribDuplicate_1_0::theSOPTypeName; }

    virtual CookMode cookMode(const SOP_NodeParms *parms) const { return COOK_GENERIC; }

    virtual void cook(const CookParms &cookparms) const;

    /// This static data member automatically registers
    /// this verb class at library ldir0d time.
    static const SOP_NodeVerb::Register<SOP_FeE_AttribDuplicate_1_0Verb> theVerb;
};

// The static member variable definition has to be outside the class definition.
// The declaration is inside the class.
const SOP_NodeVerb::Register<SOP_FeE_AttribDuplicate_1_0Verb> SOP_FeE_AttribDuplicate_1_0Verb::theVerb;

const SOP_NodeVerb *
SOP_FeE_AttribDuplicate_1_0::cookVerb() const 
{ 
    return SOP_FeE_AttribDuplicate_1_0Verb::theVerb.get();
}





static GA_GroupType
sopGroupType(SOP_FeE_AttribDuplicate_1_0Parms::GroupType parmgrouptype)
{
    using namespace SOP_FeE_AttribDuplicate_1_0Enums;
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
SOP_FeE_AttribDuplicate_1_0Verb::cook(const SOP_NodeVerb::CookParms& cookparms) const
{
    auto&& sopparms = cookparms.parms<SOP_FeE_AttribDuplicate_1_0Parms>();
    GA_Detail& outGeo0 = *cookparms.gdh().gdpNC();
    //auto sopcache = (SOP_FeE_AttribDuplicate_1_0Cache*)cookparms.cache();

    const GA_Detail& inGeo0 = *cookparms.inputGeo(0);

    outGeo0.replaceWith(inGeo0);


    UT_AutoInterrupt boss("Processing");
    if (boss.wasInterrupted())
        return;

    GFE_AttribDuplicate attribDuplicate(outGeo0, cookparms);

    attribDuplicate.setSourceAttribute(connectivityAttribPtr);
    attribDuplicate.setDestinationAttribute(outAttribPtr);
    attribDuplicate.compute();

    GA_Attribute* const finalAttribPtr = attribPromote.getDestinationAttribute();
    attribDuplicate.computeAndBumpDataId();

    
}



namespace SOP_FeE_AttribDuplicate_1_0_Namespace {

} // End SOP_FeE_AttribDuplicate_1_0_Namespace namespace
