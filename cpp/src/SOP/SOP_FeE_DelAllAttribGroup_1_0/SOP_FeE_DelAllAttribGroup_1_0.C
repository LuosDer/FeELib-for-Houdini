
//#define UT_ASSERT_LEVEL 3
#include "SOP_FeE_DelAllAttribGroup_1_0.h"


#include "SOP_FeE_DelAllAttribGroup_1_0.proto.h"

#include "GA/GA_Detail.h"
#include "PRM/PRM_TemplateBuilder.h"
#include "UT/UT_Interrupt.h"
#include "UT/UT_DSOVersion.h"


#include "GFE/GFE_Group.h"
#include "GFE/GFE_Attribute.h"

using namespace SOP_FeE_DelAllAttribGroup_1_0_Namespace;



static const char *theDsFile = R"THEDSFILE(
{
    name        parameters

    parm {
        name    "doKeepPrimAttrib"
        cppname "DoKeepPrimAttrib"
        label   "Do Keep Prim Attrib"
        type    toggle
        default { 1 }
        nolabel
        joinnext
    }
    parm {
        name    "keepPrimAttribName"
        cppname "KeepPrimAttribName"
        label   "Keep Prim Attrib Name"
        type    string
        default { "" }
        disablewhen "{ doKeepPrimAttrib == 0 }"
    }



    parm {
        name    "doKeepPointAttrib"
        cppname "DoKeepPointAttrib"
        label   "Do Keep Point Attrib"
        type    toggle
        default { 1 }
        nolabel
        joinnext
    }
    parm {
        name    "keepPointAttribName"
        cppname "KeepPointAttribName"
        label   "Keep Point Attrib Name"
        type    string
        default { "" }
        disablewhen "{ doKeepPointAttrib == 0 }"
    }



    parm {
        name    "doKeepVertexAttrib"
        cppname "DoKeepVertexAttrib"
        label   "Do Keep Vertex Attrib"
        type    toggle
        default { 1 }
        nolabel
        joinnext
    }
    parm {
        name    "keepVertexAttribName"
        cppname "KeepVertexAttribName"
        label   "Keep Vertex Attrib Name"
        type    string
        default { "" }
        disablewhen "{ doKeepVertexAttrib == 0 }"
    }





    parm {
        name    "doKeepDetailAttrib"
        cppname "DoKeepDetailAttrib"
        label   "Do Keep Detail Attrib"
        type    toggle
        default { 1 }
        nolabel
        joinnext
    }
    parm {
        name    "keepDetailAttribName"
        cppname "KeepDetailAttribName"
        label   "Keep Detail Attrib Name"
        type    string
        default { "" }
        disablewhen "{ doKeepDetailAttrib == 0 }"
    }



    parm {
        name    "sepparm"
        label   "Separator"
        type    separator
        default { "" }
    }



    parm {
        name    "doKeepPointGroup"
        cppname "DoKeepPointGroup"
        label   "Do Keep Point Group"
        type    toggle
        default { 1 }
        nolabel
        joinnext
    }
    parm {
        name    "keepPointGroupName"
        cppname "KeepPointGroupName"
        label   "Keep Point Group Name"
        type    string
        default { "" }
        disablewhen "{ doKeepPointGroup == 0 }"
    }



    parm {
        name    "doKeepVertexGroup"
        cppname "DoKeepVertexGroup"
        label   "Do Keep Vertex Group"
        type    toggle
        default { 1 }
        nolabel
        joinnext
    }
    parm {
        name    "keepVertexGroupName"
        cppname "KeepVertexGroupName"
        label   "Keep Vertex Group Name"
        type    string
        default { "" }
        disablewhen "{ doKeepVertexGroup == 0 }"
    }



    parm {
        name    "doKeepPrimGroup"
        cppname "DoKeepPrimGroup"
        label   "Do Keep Prim Group"
        type    toggle
        default { 1 }
        nolabel
        joinnext
    }
    parm {
        name    "keepPrimGroupName"
        cppname "KeepPrimGroupName"
        label   "Keep Prim Group Name"
        type    string
        default { "" }
        disablewhen "{ doKeepPrimGroup == 0 }"
    }



    parm {
        name    "doKeepEdgeGroup"
        cppname "DoKeepEdgeGroup"
        label   "Do Keep Edge Group"
        type    toggle
        default { 1 }
        nolabel
        joinnext
    }
    parm {
        name    "keepEdgeGroupName"
        cppname "KeepEdgeGroupName"
        label   "Keep Edge Group Name"
        type    string
        default { "" }
        disablewhen "{ doKeepEdgeGroup == 0 }"
    }




    parm {
        name    "subscribeRatio"
        cppname "SubscribeRatio"
        label   "Subscribe Ratio"
        type    integer
        default { 16 }
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
SOP_FeE_DelAllAttribGroup_1_0::buildTemplates()
{
    static PRM_TemplateBuilder templ("SOP_FeE_DelAllAttribGroup_1_0.C"_sh, theDsFile);
    if (templ.justBuilt())
    {
        templ.setChoiceListPtr("keepPrimAttribName"_sh,    &SOP_Node::primAttribMenu);
        templ.setChoiceListPtr("keepPointAttribName"_sh,   &SOP_Node::pointAttribMenu);
        templ.setChoiceListPtr("keepVertexAttribName"_sh,  &SOP_Node::vertexAttribMenu);
        templ.setChoiceListPtr("keepDetailAttribName"_sh,  &SOP_Node::detailAttribMenu);

        templ.setChoiceListPtr("keepPrimGroupName"_sh,     &SOP_Node::primGroupMenu);
        templ.setChoiceListPtr("keepPointGroupName"_sh,    &SOP_Node::pointGroupMenu);
        templ.setChoiceListPtr("keepVertexGroupName"_sh,   &SOP_Node::vertexNamedGroupMenu);
        templ.setChoiceListPtr("keepEdgeGroupName"_sh,     &SOP_Node::edgeGroupMenu);
    }
    return templ.templates();
}


const UT_StringHolder SOP_FeE_DelAllAttribGroup_1_0::theSOPTypeName("FeE::delAllAttribGroup::1.0"_sh);

void
newSopOperator(OP_OperatorTable* table)
{
    OP_Operator* newOp = new OP_Operator(
        SOP_FeE_DelAllAttribGroup_1_0::theSOPTypeName,
        "FeE Delete All Attributes Groups",
        SOP_FeE_DelAllAttribGroup_1_0::myConstructor,
        SOP_FeE_DelAllAttribGroup_1_0::buildTemplates(),
        1,
        1,
        nullptr,
        0,
        nullptr,
        1,
        "Five elements Elf/Attribute");

    newOp->setIconName("SOP_clean");
    table->addOperator(newOp);

}


//class SOP_FeE_DelAllAttribGroup_1_0Cache : public SOP_NodeCache
//{
//public:
//    SOP_FeE_DelAllAttribGroup_1_0Cache() : SOP_NodeCache(),
//        myPrevOutputDetailID(-1)
//    {}
//    ~SOP_FeE_DelAllAttribGroup_1_0Cache() override {}
//
//    int64 myPrevOutputDetailID;
//};


class SOP_FeE_DelAllAttribGroup_1_0Verb : public SOP_NodeVerb
{
public:
    SOP_FeE_DelAllAttribGroup_1_0Verb() {}
    virtual ~SOP_FeE_DelAllAttribGroup_1_0Verb() {}

    virtual SOP_NodeParms *allocParms() const { return new SOP_FeE_DelAllAttribGroup_1_0Parms(); }
    //virtual SOP_NodeCache* allocCache() const { return new SOP_FeE_DelAllAttribGroup_1_0Cache(); }
    virtual UT_StringHolder name() const { return SOP_FeE_DelAllAttribGroup_1_0::theSOPTypeName; }

    virtual CookMode cookMode(const SOP_NodeParms *parms) const { return COOK_GENERIC; }

    virtual void cook(const CookParms &cookparms) const;

    static const SOP_NodeVerb::Register<SOP_FeE_DelAllAttribGroup_1_0Verb> theVerb;
};

const SOP_NodeVerb::Register<SOP_FeE_DelAllAttribGroup_1_0Verb> SOP_FeE_DelAllAttribGroup_1_0Verb::theVerb;

const SOP_NodeVerb *
SOP_FeE_DelAllAttribGroup_1_0::cookVerb() const 
{ 
    return SOP_FeE_DelAllAttribGroup_1_0Verb::theVerb.get();
}





void
SOP_FeE_DelAllAttribGroup_1_0Verb::cook(const SOP_NodeVerb::CookParms &cookparms) const
{
    auto &&sopparms = cookparms.parms<SOP_FeE_DelAllAttribGroup_1_0Parms>();
    GA_Detail& outGeo0 = cookparms.gdh().gdpNC();
    //auto sopcache = (SOP_FeE_DelAllAttribGroup_1_0Cache*)cookparms.cache();

    const GA_Detail& inGeo0 = cookparms.inputGeo(0);

    outGeo0.replaceWith(inGeo0);
    
    GA_AttributeSet& attribSet = outGeo0.getAttributes();

    //const exint subscribeRatio = sopparms.getSubscribeRatio();
    //const exint minGrainSize = sopparms.getMinGrainSize();

    UT_AutoInterrupt boss("Processing");
    if (boss.wasInterrupted())
        return;

    const UT_StringHolder& allPattern = "*";
    const UT_StringHolder& keepPrimAttribName   = sopparms.getDoKeepPrimAttrib()   ? sopparms.getKeepPrimAttribName()   : allPattern;
    const UT_StringHolder& keepPointAttribName  = sopparms.getDoKeepPointAttrib()  ? sopparms.getKeepPointAttribName()  : allPattern;
    const UT_StringHolder& keepVertexAttribName = sopparms.getDoKeepVertexAttrib() ? sopparms.getKeepVertexAttribName() : allPattern;
    const UT_StringHolder& keepDetailAttribName = sopparms.getDoKeepDetailAttrib() ? sopparms.getKeepDetailAttribName() : allPattern;
    GFE_Attribute::keepStdAttribute(attribSet, keepPrimAttribName, keepPointAttribName, keepVertexAttribName, keepDetailAttribName);

    const UT_StringHolder& keepPrimGroupName   = sopparms.getDoKeepPrimGroup()   ? sopparms.getKeepPrimGroupName()   : allPattern;
    const UT_StringHolder& keepPointGroupName  = sopparms.getDoKeepPointGroup()  ? sopparms.getKeepPointGroupName()  : allPattern;
    const UT_StringHolder& keepVertexGroupName = sopparms.getDoKeepVertexGroup() ? sopparms.getKeepVertexGroupName() : allPattern;
    const UT_StringHolder& keepEdgeGroupName   = sopparms.getDoKeepEdgeGroup()   ? sopparms.getKeepEdgeGroupName()   : allPattern;
    GFE_Group::keepStdGroup(outGeo0, keepPrimGroupName, keepPointGroupName, keepVertexGroupName, keepEdgeGroupName);
}



namespace SOP_FeE_DelAllAttribGroup_1_0_Namespace {

} // End SOP_FeE_DelAllAttribGroup_1_0_Namespace namespace
