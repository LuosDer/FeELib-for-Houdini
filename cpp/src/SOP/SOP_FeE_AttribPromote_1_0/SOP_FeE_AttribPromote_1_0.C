
//#define UT_ASSERT_LEVEL 3
#include "SOP_FeE_AttribPromote_1_0.h"

#include "SOP_FeE_AttribPromote_1_0.proto.h"

#include "GA/GA_Detail.h"
#include "PRM/PRM_TemplateBuilder.h"
#include "UT/UT_Interrupt.h"
#include "UT/UT_DSOVersion.h"


#include "GFE/GFE_AttributePromote.h"



using namespace SOP_FeE_AttribPromote_1_0_Namespace;


static const char *theDsFile = R"THEDSFILE(
{
    name        parameters

    parm {
        name    "srcAttribClass"
        cppname "SrcAttribClass"
        label   "Source Attribute Class"
        type    ordinal
        default { "point" }
        menu {
            "prim"      "Primitive"
            "point"     "Point"
            "vertex"    "Vertex"
            "detail"    "Detail"
        }
    }

    parm {
        name    "srcAttrib"
        cppname "SrcAttrib"
        label   "Source Attribute"
        type    string
        default { "" }
    }

    parm {
        name    "dstAttribClass"
        cppname "DstAttribClass"
        label   "Destination Attribute Class"
        type    ordinal
        default { "point" }
        menu {
            "prim"      "Primitive"
            "point"     "Point"
            "vertex"    "Vertex"
            "detail"    "Detail"
        }
    }

    parm {
        name    "renameAttrib"
        cppname "RenameAttrib"
        label   "Rename Attrib"
        type    toggle
        nolabel
        joinnext
        default { "0" }
    }
    parm {
        name    "newAttribName"
        cppname "NewAttribName"
        label   "New Attrib Name"
        type    string
        default { "" }
        disablewhen "{ renameAttrib == 0 }"
    }



    // parm {
    //     name    "renameGroup"
    //     cppname "RenameGroup"
    //     label   "Rename Group"
    //     type    toggle
    //     nolabel
    //     joinnext
    //     default { "0" }
    // }
    // parm {
    //     name    "newGroupName"
    //     cppname "NewGroupName"
    //     label   "New Group Name"
    //     type    string
    //     default { "" }
    //     disablewhen "{ renameGroup == 0 }"
    // }

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
SOP_FeE_AttribPromote_1_0::buildTemplates()
{
    static PRM_TemplateBuilder templ("SOP_FeE_AttribPromote_1_0.C"_sh, theDsFile);
    if (templ.justBuilt())
    {
        templ.setChoiceListPtr("srcAttrib"_sh, &SOP_Node::allAttribMenu);
    }
    return templ.templates();
}

const UT_StringHolder SOP_FeE_AttribPromote_1_0::theSOPTypeName("FeE::attribPromote::1.0"_sh);

void
newSopOperator(OP_OperatorTable* table)
{
    OP_Operator* newOp = new OP_Operator(
        SOP_FeE_AttribPromote_1_0::theSOPTypeName,
        "FeE Attribute Promote",
        SOP_FeE_AttribPromote_1_0::myConstructor,
        SOP_FeE_AttribPromote_1_0::buildTemplates(),
        1,
        1,
        nullptr,
        OP_FLAG_GENERATOR,
        nullptr,
        1,
        "Five elements Elf/Attribute");

    newOp->setIconName("SOP_attribpromote");
    table->addOperator(newOp);
}

//class SOP_FeE_AttribPromote_1_0Cache : public SOP_NodeCache
//{
//public:
//    SOP_FeE_AttribPromote_1_0Cache() : SOP_NodeCache(),
//        myPrevOutputDetailID(-1)
//    {}
//    ~SOP_FeE_AttribPromote_1_0Cache() override {}
//
//    int64 myPrevOutputDetailID;
//};


class SOP_FeE_AttribPromote_1_0Verb : public SOP_NodeVerb
{
public:
    SOP_FeE_AttribPromote_1_0Verb() {}
    virtual ~SOP_FeE_AttribPromote_1_0Verb() {}

    virtual SOP_NodeParms *allocParms() const { return new SOP_FeE_AttribPromote_1_0Parms(); }
    //virtual SOP_NodeCache* allocCache() const { return new SOP_FeE_AttribPromote_1_0Cache(); }
    virtual UT_StringHolder name() const { return SOP_FeE_AttribPromote_1_0::theSOPTypeName; }

    virtual CookMode cookMode(const SOP_NodeParms *parms) const { return COOK_GENERIC; }

    virtual void cook(const CookParms &cookparms) const;

    /// This static data member automatically registers
    /// this verb class at library ldir0d time.
    static const SOP_NodeVerb::Register<SOP_FeE_AttribPromote_1_0Verb> theVerb;
};

// The static member variable definition has to be outside the class definition.
// The declaration is inside the class.
const SOP_NodeVerb::Register<SOP_FeE_AttribPromote_1_0Verb> SOP_FeE_AttribPromote_1_0Verb::theVerb;

const SOP_NodeVerb *
SOP_FeE_AttribPromote_1_0::cookVerb() const 
{ 
    return SOP_FeE_AttribPromote_1_0Verb::theVerb.get();
}




static GA_AttributeOwner
sopAttribOwner(SOP_FeE_AttribPromote_1_0Parms::SrcAttribClass attribClass)
{
    using namespace SOP_FeE_AttribPromote_1_0Enums;
    switch (attribClass)
    {
    case SrcAttribClass::PRIM:      return GA_ATTRIB_PRIMITIVE;  break;
    case SrcAttribClass::POINT:     return GA_ATTRIB_POINT;      break;
    case SrcAttribClass::VERTEX:    return GA_ATTRIB_VERTEX;     break;
    case SrcAttribClass::DETAIL:    return GA_ATTRIB_DETAIL;     break;
    }
    UT_ASSERT_MSG(0, "Unhandled Geo0 Class type!");
    return GA_ATTRIB_INVALID;
}

static GA_AttributeOwner
sopAttribOwner(SOP_FeE_AttribPromote_1_0Parms::DstAttribClass attribClass)
{
    using namespace SOP_FeE_AttribPromote_1_0Enums;
    switch (attribClass)
    {
    case DstAttribClass::PRIM:      return GA_ATTRIB_PRIMITIVE;  break;
    case DstAttribClass::POINT:     return GA_ATTRIB_POINT;      break;
    case DstAttribClass::VERTEX:    return GA_ATTRIB_VERTEX;     break;
    case DstAttribClass::DETAIL:    return GA_ATTRIB_DETAIL;     break;
    }
    UT_ASSERT_MSG(0, "Unhandled Geo0 Class type!");
    return GA_ATTRIB_INVALID;
}


void
SOP_FeE_AttribPromote_1_0Verb::cook(const SOP_NodeVerb::CookParms& cookparms) const
{
    auto&& sopparms = cookparms.parms<SOP_FeE_AttribPromote_1_0Parms>();
    GA_Detail& outGeo0 = *cookparms.gdh().gdpNC();
    //auto sopcache = (SOP_FeE_AttribPromote_1_0Cache*)cookparms.cache();

    const GA_Detail& inGeo0 = *cookparms.inputGeo(0);

    outGeo0.replaceWith(inGeo0);


    UT_AutoInterrupt boss("Processing");
    if (boss.wasInterrupted())
        return;

    const GA_AttributeOwner srcAttribClass = sopAttribOwner(sopparms.getSrcAttribClass());
    const GA_AttributeOwner dstAttribClass = sopAttribOwner(sopparms.getDstAttribClass());
    
    GFE_AttribPromote attribPromote(outGeo0, cookparms);

    attribPromote.setSourceAttribute(srcAttribClass, sopparms.getSrcAttrib());
    attribPromote.setDestinationAttribute(dstAttribClass);
    attribPromote.setComputeParm(sopparms.getSubscribeRatio(), sopparms.getMinGrainSize());

    //GA_Attribute* const finalAttribPtr = attribPromote.getDestinationAttribute();
    attribPromote.computeAndBumpDataId();

    
}
