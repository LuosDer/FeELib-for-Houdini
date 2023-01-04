
//#define UT_ASSERT_LEVEL 3
#include "SOP_FeE_AttribCopy_4_0.h"


#include "SOP_FeE_AttribCopy_4_0.proto.h"

#include "GEO/GEO_Detail.h"
#include "PRM/PRM_TemplateBuilder.h"
#include "UT/UT_Interrupt.h"
#include "UT/UT_DSOVersion.h"


#include "GA_FeE/GA_FeE_Group.h"
#include "GA_FeE/GA_FeE_Attribute.h"
#include "GA_FeE/GA_FeE_VertexNextEquiv.h"
#include "GA_FeE/GA_FeE_Detail.h"

using namespace SOP_FeE_AttribCopy_4_0_Namespace;



static const char *theDsFile = R"THEDSFILE(
{
    name        parameters

    parm {
        name    "srcGroup"
        cppname "SrcGroup"
        label   "Source Group"
        type    string
        default { "" }
        range   { 0 1 }
        parmtag { "script_action" "import soputils\nkwargs['geometrytype'] = kwargs['node'].parmTuple('sourceClass')\nkwargs['inputindex'] = 1\nsoputils.selectGroupParm(kwargs)" }
        parmtag { "script_action_help" "Select geometry from an available viewport." }
        parmtag { "script_action_icon" "BUTTONS_reselect" }
    }
    parm {
        name    "sourceClass"
        cppname "SourceClass"
        label   "Source Class"
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
        name    "group"
        cppname "Group"
        label   "Group"
        type    string
        default { "" }
        range   { 0 1 }
        parmtag { "script_action" "import soputils\nkwargs['geometrytype'] = kwargs['node'].parmTuple('destinationClass')\nkwargs['inputindex'] = 0\nsoputils.selectGroupParm(kwargs)" }
        parmtag { "script_action_help" "Select geometry from an available viewport." }
        parmtag { "script_action_icon" "BUTTONS_reselect" }
    }
    parm {
        name    "destinationClass"
        cppname "DestinationClass"
        label   "Destination Class"
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
        name    "useIDAttrib"
        cppname "UseIDAttrib"
        label   "Match by Attribute"
        type    toggle
        nolabel
        joinnext
        default { "0" }
    }
    parm {
        name    "iDAttribName"
        cppname "IDAttribName"
        label   "ID Attribute"
        type    string
        joinnext
        default { "id" }
        disablewhen "{ useIDAttrib == 0 }"
    }
    parm {
        name    "iDAttribInput"
        cppname "IDAttribInput"
        label   "ID Attrib Input"
        type    ordinal
        nolabel
        default { "destination" }
        menu {
            "source"        "Source"
            "destination"   "Destination"
        }
    }


    groupsimple {
        name    "copyAttrib_folder"
        label   "Copy Attrib"

        parm {
            name    "copyAttribName"
            cppname "CopyAttribName"
            label   "Copy Attrib Name"
            type    string
            default { "" }
        }
        parm {
            name    "useNewCopyAttribName"
            cppname "UseNewCopyAttribName"
            label   "Use New Copy Attrib Name"
            type    toggle
            nolabel
            joinnext
            default { "0" }
        }
        parm {
            name    "newCopyAttribName"
            cppname "NewCopyAttribName"
            label   "New Copy Attrib Name"
            type    string
            default { "" }
            disablewhen "{ useNewCopyAttribName == 0 }"
        }
        parm {
            name    "attribMergeType"
            cppname "AttribMergeType"
            label   "Attrib Merge Type"
            type    ordinal
            default { "replace" }
            menu {
                "set"       "Set"
                "add"       "Add"
                "sub"       "Subtract"
                "mult"      "Multiply"
                "min"       "Min"
                "max"       "Max"
                "xor"       "Xor"
                "toggle"    "Toggle"
                "append"    "Append"
                "intersect" "Intersect"
            }
        }
    }
    
    groupsimple {
        name    "copyGroup_folder"
        label   "Copy Group"

        parm {
            name    "copyGroupName"
            cppname "CopyGroupName"
            label   "Copy Group Name"
            type    string
            default { "" }
        }
        parm {
            name    "useNewCopyGroupName"
            cppname "UseNewCopyGroupName"
            label   "Use New Copy Group Name"
            type    toggle
            nolabel
            joinnext
            default { "0" }
        }
        parm {
            name    "newCopyGroupName"
            cppname "NewCopyGroupName"
            label   "New Copy Group Name"
            type    string
            default { "" }
            disablewhen "{ useNewCopyGroupName == 0 }"
        }
        parm {
            name    "groupMergeType"
            cppname "GroupMergeType"
            label   "Group Merge Type"
            type    ordinal
            default { "replace" }
            menu {
                "replace"   "Replace Existing"
                "union"     "Union with Existing"
                "intersect" "Intersect with Existing"
                "subtract"  "Subtract from Existing"
                "toogle"    "Toogle"
            }
        }
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
SOP_FeE_AttribCopy_4_0::buildTemplates()
{
    static PRM_TemplateBuilder templ("SOP_FeE_AttribCopy_4_0.C"_sh, theDsFile);
    if (templ.justBuilt())
    {
        templ.setChoiceListPtr("iDAttribName"_sh,   &SOP_Node::allAttribMenu);
        templ.setChoiceListPtr("copyAttribName"_sh, &SOP_Node::allAttribMenu);
        templ.setChoiceListPtr("copyGroupName"_sh,  &SOP_Node::allGroupMenu);
    }
    return templ.templates();
}


const UT_StringHolder SOP_FeE_AttribCopy_4_0::theSOPTypeName("FeE::attribCopy::4.0"_sh);

void
newSopOperator(OP_OperatorTable* table)
{
    OP_Operator* newOp = new OP_Operator(
        SOP_FeE_AttribCopy_4_0::theSOPTypeName,
        "FeE Attribute Copy",
        SOP_FeE_AttribCopy_4_0::myConstructor,
        SOP_FeE_AttribCopy_4_0::buildTemplates(),
        2,
        2,
        nullptr,
        0,
        nullptr,
        1,
        "Five elements Elf/Attribute");

    newOp->setIconName("SOP_attribcopy");
    table->addOperator(newOp);

}


//class SOP_FeE_AttribCopy_4_0Cache : public SOP_NodeCache
//{
//public:
//    SOP_FeE_AttribCopy_4_0Cache() : SOP_NodeCache(),
//        myPrevOutputDetailID(-1)
//    {}
//    ~SOP_FeE_AttribCopy_4_0Cache() override {}
//
//    int64 myPrevOutputDetailID;
//};


class SOP_FeE_AttribCopy_4_0Verb : public SOP_NodeVerb
{
public:
    SOP_FeE_AttribCopy_4_0Verb() {}
    virtual ~SOP_FeE_AttribCopy_4_0Verb() {}

    virtual SOP_NodeParms *allocParms() const { return new SOP_FeE_AttribCopy_4_0Parms(); }
    //virtual SOP_NodeCache* allocCache() const { return new SOP_FeE_AttribCopy_4_0Cache(); }
    virtual UT_StringHolder name() const { return SOP_FeE_AttribCopy_4_0::theSOPTypeName; }

    virtual CookMode cookMode(const SOP_NodeParms *parms) const { return COOK_GENERIC; }

    virtual void cook(const CookParms &cookparms) const;

    static const SOP_NodeVerb::Register<SOP_FeE_AttribCopy_4_0Verb> theVerb;
};

const SOP_NodeVerb::Register<SOP_FeE_AttribCopy_4_0Verb> SOP_FeE_AttribCopy_4_0Verb::theVerb;

const SOP_NodeVerb *
SOP_FeE_AttribCopy_4_0::cookVerb() const 
{ 
    return SOP_FeE_AttribCopy_4_0Verb::theVerb.get();
}



static GA_AttributeOwner
sopAttribOwner(SOP_FeE_AttribCopy_4_0Parms::DestinationClass attribClass)
{
    using namespace SOP_FeE_AttribCopy_4_0Enums;
    switch (attribClass)
    {
    case DestinationClass::PRIM:      return GA_ATTRIB_PRIMITIVE;  break;
    case DestinationClass::POINT:     return GA_ATTRIB_POINT;      break;
    case DestinationClass::VERTEX:    return GA_ATTRIB_VERTEX;     break;
    case DestinationClass::DETAIL:    return GA_ATTRIB_DETAIL;     break;
    }
    UT_ASSERT_MSG(0, "Unhandled Geo0 Class type!");
    return GA_ATTRIB_INVALID;
}

static GA_AttributeOwner
sopAttribOwner(SOP_FeE_AttribCopy_4_0Parms::SourceClass attribClass)
{
    using namespace SOP_FeE_AttribCopy_4_0Enums;
    switch (attribClass)
    {
    case SourceClass::PRIM:      return GA_ATTRIB_PRIMITIVE;  break;
    case SourceClass::POINT:     return GA_ATTRIB_POINT;      break;
    case SourceClass::VERTEX:    return GA_ATTRIB_VERTEX;     break;
    case SourceClass::DETAIL:    return GA_ATTRIB_DETAIL;     break;
    }
    UT_ASSERT_MSG(0, "Unhandled Geo1 Class type!");
    return GA_ATTRIB_INVALID;
}

static bool
sopIDAttribInput(SOP_FeE_AttribCopy_4_0Parms::IDAttribInput iDAttribInput)
{
    using namespace SOP_FeE_AttribCopy_4_0Enums;
    switch (iDAttribInput)
    {
    case IDAttribInput::SOURCE:          return false;     break;
    case IDAttribInput::DESTINATION:     return true;      break;
    }
    UT_ASSERT_MSG(0, "Unhandled Class type!");
    return false;
}




void
SOP_FeE_AttribCopy_4_0Verb::cook(const SOP_NodeVerb::CookParms &cookparms) const
{
    auto &&sopparms = cookparms.parms<SOP_FeE_AttribCopy_4_0Parms>();
    GEO_Detail* outGeo0 = cookparms.gdh().gdpNC();
    //auto sopcache = (SOP_FeE_AttribCopy_4_0Cache*)cookparms.cache();

    const GA_Detail* const inGeo0 = cookparms.inputGeo(0);
    const GEO_Detail* const inGeo1 = cookparms.inputGeo(1);

    outGeo0->replaceWith(*inGeo0);

    
        

    const UT_StringHolder& copyAttribName = sopparms.getCopyAttribName();
    //const bool useNewCopyAttribName = sopparms.getUseNewCopyAttribName();
    const UT_StringHolder& emptyString = "";
    const UT_StringHolder& newCopyAttribName = sopparms.getUseNewCopyAttribName() ? sopparms.getNewCopyAttribName() : emptyString;
    const GA_AttributeOwner sourceClass = sopAttribOwner(sopparms.getSourceClass());
    const GA_AttributeOwner destinationClass = sopAttribOwner(sopparms.getDestinationClass());





    const GA_GroupType sourceGroupType = GA_FeE_Type::attributeOwner_groupType(sourceClass);
    const GA_GroupType destinationGroupType = GA_FeE_Type::attributeOwner_groupType(destinationClass);

    const UT_StringHolder& copyGroupName = sopparms.getCopyGroupName();
    const UT_StringHolder& newCopyGroupName = sopparms.getUseNewCopyGroupName() ? sopparms.getNewCopyGroupName() : emptyString;


    GOP_Manager gop;
    const GA_Group* geo0Group = GA_FeE_Group::findOrParseGroupDetached(cookparms, outGeo0, destinationGroupType, sopparms.getGroup(), gop);
    if (geo0Group && geo0Group->isEmpty())
        return;

    const GA_Group* geo1Group = GA_FeE_Group::findOrParseGroupDetached(cookparms, inGeo1, destinationGroupType, sopparms.getSrcGroup(), gop);
    if (geo1Group && geo1Group->isEmpty())
        return;
    //notifyGroupParmListeners(cookparms.getNode(), 0, 1, outGeo0, geo0Group);


    //GA_AttributeSet& attribSet = outGeo0->getAttributes();


    //const exint subscribeRatio = sopparms.getSubscribeRatio();
    //const exint minGrainSize = sopparms.getMinGrainSize();

    UT_AutoInterrupt boss("Processing");
    if (boss.wasInterrupted())
        return;


    const bool useIDAttrib = sopparms.getUseIDAttrib();

    const SOP_FeE_AttribCopy_4_0Parms::AttribMergeType attribMergeType = sopparms.getAttribMergeType();
    using namespace SOP_FeE_AttribCopy_4_0Enums;
    if (useIDAttrib)
    {
        const UT_StringHolder& iDAttribName = sopparms.getIDAttribName();
        const bool iDAttribInput = sopIDAttribInput(sopparms.getIDAttribInput());

        switch (attribMergeType)
        {
        case AttribMergeType::SET:
            GA_FeE_Attribute::copyAttribute(outGeo0, destinationClass, inGeo1, sourceClass, copyAttribName, iDAttribName, iDAttribInput);
            GA_FeE_Group::copyGroup(outGeo0, destinationGroupType, inGeo1, sourceGroupType, copyGroupName, iDAttribName, iDAttribInput);
            break;
        case AttribMergeType::ADD:
            break;
        case AttribMergeType::SUB:
            break;
        case AttribMergeType::MULT:
            break;
        case AttribMergeType::MIN:
            break;
        case AttribMergeType::MAX:
            break;
        case AttribMergeType::XOR:
            break;
        case AttribMergeType::TOGGLE:
            break;
        case AttribMergeType::APPEND:
            break;
        case AttribMergeType::INTERSECT:
            break;
        default:
            break;
        }
    }
    else
    {
        switch (attribMergeType)
        {
        case AttribMergeType::SET:
            GA_FeE_Attribute::copyAttribute(outGeo0, destinationClass, inGeo1, sourceClass, copyAttribName);
            GA_FeE_Group::copyGroup(outGeo0, destinationGroupType, inGeo1, sourceGroupType, copyGroupName);
            break;
        case AttribMergeType::ADD:
            break;
        case AttribMergeType::SUB:
            break;
        case AttribMergeType::MULT:
            break;
        case AttribMergeType::MIN:
            break;
        case AttribMergeType::MAX:
            break;
        case AttribMergeType::XOR:
            break;
        case AttribMergeType::TOGGLE:
            break;
        case AttribMergeType::APPEND:
            break;
        case AttribMergeType::INTERSECT:
            break;
        default:
            break;
        }
    }
    //GA_FeE_Attribute::copyAttribute(attribSet, copyAttribName, copyPointAttribName, copyVertexAttribName, copyDetailAttribName);

    //GA_FeE_Group::copyGroup(outGeo0, copyPrimGroupName, copyPointGroupName, copyVertexGroupName, copyEdgeGroupName);
}



namespace SOP_FeE_AttribCopy_4_0_Namespace {

} // End SOP_FeE_AttribCopy_4_0_Namespace namespace
