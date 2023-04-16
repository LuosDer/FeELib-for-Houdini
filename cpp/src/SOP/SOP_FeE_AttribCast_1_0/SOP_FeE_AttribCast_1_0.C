
//#define UT_ASSERT_LEVEL 3
#include "SOP_FeE_AttribCast_1_0.h"
#include "SOP_FeE_AttribCast_1_0.proto.h"


#include "GA/GA_Detail.h"
#include "PRM/PRM_TemplateBuilder.h"
#include "UT/UT_Interrupt.h"
#include "UT/UT_DSOVersion.h"


#include "GFE/GFE_AttributeCast.h"


using namespace SOP_FeE_AttribCast_1_0_Namespace;



static const char* theDsFile = R"THEDSFILE(
{
    name        parameters
    parm {
        name    "attribClass"
        cppname "AttribClass"
        label   "Attrib Class"
        type    ordinal
        default { "points" }
        menu {
            "detail"    "Detail"
            "prim"      "Primitive"
            "point"     "Point"
            "vertex"    "Vertex"
        }
    }
    parm {
        name    "attribName"
        cppname "AttribName"
        label   "Attrib Name"
        type    string
        default { "" }
    }
    parm {
        name    "groupName"
        cppname "GroupName"
        label   "Group Name"
        type    string
        default { "" }
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
        name    "newName"
        cppname "NewName"
        label   "New Name"
        type    string
        default { "" }
        disablewhen "{ renameAttrib == 0 }"
    }
    parm {
        name    "attribType"
        cppname "AttribType"
        label   "Attrib Type"
        type    ordinal
        default { "string" }
        menu {
            "bool"      "Bool"
            "int"       "Int"
            "float"     "Float"
            "string"    "String"
            "vector2"   "Vector2"
            "vector"    "Vector"
            "vector4"   "Vector4"
            "matrix2"   "Matrix2"
            "matrix"    "Matrix"
            "matrix4"   "Matrix4"
        }
    }
    parm {
        name    "prefix"
        cppname "Prefix"
        label   "Prefix"
        type    string
        default { "" }
        hidewhen "{ attribType != string }"
    }
    parm {
        name    "sufix"
        cppname "Sufix"
        label   "Sufix"
        type    string
        default { "" }
        hidewhen "{ attribType != string }"
    }
    parm {
        name    "delOriginAttrib"
        cppname "DelOriginAttrib"
        label   "Delete Origin Attrib"
        type    toggle
        default { "1" }
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
SOP_FeE_AttribCast_1_0::buildTemplates()
{
    static PRM_TemplateBuilder templ("SOP_FeE_AttribCast_1_0.C"_sh, theDsFile);
    if (templ.justBuilt())
    {
        templ.setChoiceListPtr("attribName"_sh, &SOP_Node::allAttribMenu);
        templ.setChoiceListPtr("groupName"_sh, &SOP_Node::allGroupMenu);
    }
    return templ.templates();
}

const UT_StringHolder SOP_FeE_AttribCast_1_0::theSOPTypeName("FeE::attribCast::1.0"_sh);

void
newSopOperator(OP_OperatorTable* table)
{
    OP_Operator* newOp = new OP_Operator(
        SOP_FeE_AttribCast_1_0::theSOPTypeName,
        "FeE Attribute Cast",
        SOP_FeE_AttribCast_1_0::myConstructor,
        SOP_FeE_AttribCast_1_0::buildTemplates(),
        1,
        1,
        nullptr,
        OP_FLAG_GENERATOR,
        nullptr,
        1,
        "Five elements Elf/Attribute");

    newOp->setIconName("SOP_attribcast");
    table->addOperator(newOp);
}

class SOP_FeE_AttribCast_1_0Verb : public SOP_NodeVerb
{
public:
    SOP_FeE_AttribCast_1_0Verb() {}
    virtual ~SOP_FeE_AttribCast_1_0Verb() {}

    virtual SOP_NodeParms* allocParms() const { return new SOP_FeE_AttribCast_1_0Parms(); }
    virtual UT_StringHolder name() const { return SOP_FeE_AttribCast_1_0::theSOPTypeName; }

    virtual CookMode cookMode(const SOP_NodeParms* parms) const { return COOK_GENERIC; }

    virtual void cook(const CookParms& cookparms) const;

    static const SOP_NodeVerb::Register<SOP_FeE_AttribCast_1_0Verb> theVerb;
};

const SOP_NodeVerb::Register<SOP_FeE_AttribCast_1_0Verb> SOP_FeE_AttribCast_1_0Verb::theVerb;

const SOP_NodeVerb*
SOP_FeE_AttribCast_1_0::cookVerb() const
{
    return SOP_FeE_AttribCast_1_0Verb::theVerb.get();
}




static GA_AttributeOwner
sopAttribOwner(SOP_FeE_AttribCast_1_0Parms::AttribClass attribClass)
{
    using namespace SOP_FeE_AttribCast_1_0Enums;
    switch (attribClass)
    {
    case AttribClass::PRIM:      return GA_ATTRIB_PRIMITIVE;  break;
    case AttribClass::POINT:     return GA_ATTRIB_POINT;      break;
    case AttribClass::VERTEX:    return GA_ATTRIB_VERTEX;     break;
    case AttribClass::DETAIL:    return GA_ATTRIB_DETAIL;     break;
    }
    UT_ASSERT_MSG(0, "Unhandled Geo0 Class type!");
    return GA_ATTRIB_INVALID;
}


static GA_StorageClass
sopStorageClass(SOP_FeE_AttribCast_1_0Parms::AttribType attribType)
{
    using namespace SOP_FeE_AttribCast_1_0Enums;
    switch (attribType)
    {
    case AttribType::BOOL:      return GA_STORECLASS_OTHER;      break;
    case AttribType::INT:       return GA_STORECLASS_INT;        break;
    case AttribType::FLOAT:     return GA_STORECLASS_FLOAT;      break;
    case AttribType::STRING:    return GA_STORECLASS_STRING;     break;
    }
    UT_ASSERT_MSG(0, "Unhandled Geo0 Attrib Type!");
    return GA_STORECLASS_INVALID;
}




void
SOP_FeE_AttribCast_1_0Verb::cook(const SOP_NodeVerb::CookParms &cookparms) const
{
    auto &&sopparms = cookparms.parms<SOP_FeE_AttribCast_1_0Parms>();
    GA_Detail& outGeo0 = *cookparms.gdh().gdpNC();
    
    //GFE_Detail& outGeoGFE0 = static_cast<GFE_Detail&>(outGeo0);

    const GA_Detail& inGeo0 = *cookparms.inputGeo(0);

    outGeo0.replaceWith(inGeo0);
    
    const GA_AttributeOwner geo0AttribClass = sopAttribOwner(sopparms.getAttribClass());
    //const GA_GroupType groupType = sopGroupType(sopparms.getGroupType());
        
    const exint subscribeRatio = sopparms.getSubscribeRatio();
    const exint minGrainSize = sopparms.getMinGrainSize();
    
    GFE_AttribCast attribCast(outGeo0, cookparms);
    GFE_AttribDuplicate
    attribCast.getInAttribArray().set(geo0AttribClass, sopparms.getAttribName());
    attribCast.getInGroupArray(). set(geo0AttribClass, sopparms.getGroupName());

    attribCast.newStorageClass = sopStorageClass(sopparms.getAttribType());
    if(attribCast.newStorageClass == GA_STORECLASS_STRING)
    {
        attribCast.getOutAttribArray().set(geo0AttribClass, sopparms.getAttribName());
        attribCast.prefix = sopparms.getPrefix();
        attribCast.sufix  = sopparms.getSufix();
    }
    
    attribCast.setComputeParm(sopparms.getRenameAttrib(), sopparms.getDelOriginAttrib(), subscribeRatio, minGrainSize);
    attribCast.computeAndBumpDataId();


}

