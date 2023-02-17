
//#define UT_ASSERT_LEVEL 3
#include "SOP_FeE_UVGridify_1_0.h"


#include "SOP_FeE_UVGridify_1_0.proto.h"

#include "GA/GA_Detail.h"
#include "PRM/PRM_TemplateBuilder.h"
#include "UT/UT_Interrupt.h"
#include "UT/UT_DSOVersion.h"




#include "GA_FeE/GA_FeE_UVGridify.h"




using namespace SOP_FeE_UVGridify_1_0_Namespace;


static const char *theDsFile = R"THEDSFILE(
{
    name        parameters
    parm {
        name    "group"
        cppname "Group"
        label   "Group"
        type    string
        default { "" }
        range   { 0 1 }
        parmtag { "script_action" "import soputils\nkwargs['geometrytype'] = kwargs['node'].parmTuple('groupType')\nkwargs['inputindex'] = 0\nsoputils.selectGroupParm(kwargs)" }
        parmtag { "script_action_help" "Select geometry from an available viewport." }
        parmtag { "script_action_icon" "BUTTONS_reselect" }
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
    }
    parm {
        name    "uvAttribClass"
        cppname "UVAttribClass"
        label   "UV Attribute Class"
        type    ordinal
        default { "auto" }
        menu {
            "auto"      "Auto"
            "point"     "Point"
            "vertex"    "Vertex"
        }
    }
    parm {
        name    "uvAttrib"
        cppname "UVAttrib"
        label   "UV Attribute"
        type    string
        default { "uv" }
    }
    parm {
        name    "rowsOrColsNumMethod"
        cppname "RowsOrColsNumMethod"
        label   "Rows Or Columns num Method"
        type    ordinal
        default { "uniform" }
        menu {
            "uniform"   "Uniform"
            "rows"      "Rows"
            "cols"      "Columns"
        }
    }
    parm {
        name    "rowsOrColsNum"
        cppname "RowsOrColsNum"
        label   "Rows or Columns num"
        type    intlog
        default { "2" }
        disablewhen "{ rowsOrColsNumMethod == uniform }"
        range   { 2! 30 }
    }
    parm {
        name    "reverseUVu"
        cppname "ReverseUVu"
        label   "Reverse UV U"
        type    toggle
        default { "0" }
    }
    parm {
        name    "reverseUVv"
        cppname "ReverseUVv"
        label   "Reverse UV V"
        type    toggle
        default { "0" }
    }
    parm {
        name    "uniScale"
        cppname "UniScale"
        label   "Uniform Scale"
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
SOP_FeE_UVGridify_1_0::buildTemplates()
{
    static PRM_TemplateBuilder templ("SOP_FeE_UVGridify_1_0.C"_sh, theDsFile);
    if (templ.justBuilt())
    {
        //templ.setChoiceListPtr("group"_sh, &SOP_Node::groupMenu);
        templ.setChoiceListPtr("posAttribName"_sh, &SOP_Node::allTextureCoordMenu);
        
    }
    return templ.templates();
}

const UT_StringHolder SOP_FeE_UVGridify_1_0::theSOPTypeName("FeE::uvGridify::1.0"_sh);

void
newSopOperator(OP_OperatorTable* table)
{
    OP_Operator* newOp = new OP_Operator(
        SOP_FeE_UVGridify_1_0::theSOPTypeName,
        "FeE UV Gridify",
        SOP_FeE_UVGridify_1_0::myConstructor,
        SOP_FeE_UVGridify_1_0::buildTemplates(),
        1,
        1,
        nullptr,
        OP_FLAG_GENERATOR,
        nullptr,
        1,
        "Five elements Elf/UV");

    newOp->setIconName("DATATYPES_uv");
    table->addOperator(newOp);

}





class SOP_FeE_UVGridify_1_0Verb : public SOP_NodeVerb
{
public:
    SOP_FeE_UVGridify_1_0Verb() {}
    virtual ~SOP_FeE_UVGridify_1_0Verb() {}

    virtual SOP_NodeParms *allocParms() const { return new SOP_FeE_UVGridify_1_0Parms(); }
    virtual UT_StringHolder name() const { return SOP_FeE_UVGridify_1_0::theSOPTypeName; }

    virtual CookMode cookMode(const SOP_NodeParms *parms) const { return COOK_GENERIC; }

    virtual void cook(const CookParms &cookparms) const;
    
    /// This static data member automatically registers
    /// this verb class at library load time.
    static const SOP_NodeVerb::Register<SOP_FeE_UVGridify_1_0Verb> theVerb;
};

// The static member variable definition has to be outside the class definition.
// The declaration is inside the class.
const SOP_NodeVerb::Register<SOP_FeE_UVGridify_1_0Verb> SOP_FeE_UVGridify_1_0Verb::theVerb;

const SOP_NodeVerb *
SOP_FeE_UVGridify_1_0::cookVerb() const 
{ 
    return SOP_FeE_UVGridify_1_0Verb::theVerb.get();
}




static GA_AttributeOwner
sopAttribOwner(SOP_FeE_UVGridify_1_0Parms::UVAttribClass attribClass)
{
    using namespace SOP_FeE_UVGridify_1_0Enums;
    switch (attribClass)
    {
    case UVAttribClass::AUTO:      return GA_ATTRIB_INVALID;    break;//not detail but means Auto
    case UVAttribClass::POINT:     return GA_ATTRIB_POINT;      break;
    case UVAttribClass::VERTEX:    return GA_ATTRIB_VERTEX;     break;
    }
    UT_ASSERT_MSG(0, "Unhandled Geo0 Class type!");
    return GA_ATTRIB_INVALID;
}


static UVGridify_RowsOrColsNumMethod
sopRowsOrColsNumMethod(SOP_FeE_UVGridify_1_0Parms::RowsOrColsNumMethod parmgrouptype)
{
    using namespace SOP_FeE_UVGridify_1_0Enums;
    switch (parmgrouptype)
    {
    case RowsOrColsNumMethod::UNIFORM:     return UVGridifyMethod_Uniform;    break;
    case RowsOrColsNumMethod::ROWS:        return UVGridifyMethod_Rows;       break;
    case RowsOrColsNumMethod::COLS:        return UVGridifyMethod_Columns;    break;
    }
    UT_ASSERT_MSG(0, "Unhandled UVGridify Rows Or Cols Num Method!");
    return UVGridifyMethod_Uniform;
}


static GA_GroupType
sopGroupType(SOP_FeE_UVGridify_1_0Parms::GroupType parmgrouptype)
{
    using namespace SOP_FeE_UVGridify_1_0Enums;
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
SOP_FeE_UVGridify_1_0Verb::cook(const SOP_NodeVerb::CookParms &cookparms) const
{
    auto&& sopparms = cookparms.parms<SOP_FeE_UVGridify_1_0Parms>();
    GA_Detail* const outGeo0 = cookparms.gdh().gdpNC();
    //auto sopcache = (SOP_FeE_UVGridify_1_0Cache*)cookparms.cache();

    const GA_Detail* const inGeo0 = cookparms.inputGeo(0);

    outGeo0->replaceWith(*inGeo0);


    const GA_GroupType groupType = sopGroupType(sopparms.getGroupType());
    const UT_StringHolder& groupName = sopparms.getGroup();

    const GA_AttributeOwner uvAttribClass = sopAttribOwner(sopparms.getUVAttribClass());
    const UT_StringHolder& uvAttribName = sopparms.getUVAttrib();
    const UVGridify_RowsOrColsNumMethod rowsOrColsNumMethod = sopRowsOrColsNumMethod(sopparms.getRowsOrColsNumMethod());
    const exint rowsOrColsNum = sopparms.getRowsOrColsNum();
    const bool reverseUVu = sopparms.getReverseUVu();
    const bool reverseUVv = sopparms.getReverseUVv();

    const bool uniScale = sopparms.getUniScale();
    
    const exint subscribeRatio = sopparms.getSubscribeRatio();
    const exint minGrainSize = sopparms.getMinGrainSize();


    //const GA_Storage inStorageI = GA_FeE_Type::getPreferredStorageI(outGeo0);

    UT_AutoInterrupt boss("Processing");
    if (boss.wasInterrupted())
        return;
    
    GA_Attribute* posAttribPtr = GA_FeE_UVGridify::uvGridify(cookparms, outGeo0, groupType, groupName,
        uvAttribClass, uvAttribName,
        rowsOrColsNumMethod, rowsOrColsNum, 
        reverseUVu, reverseUVv, uniScale,
        subscribeRatio, minGrainSize
    );
}



namespace SOP_FeE_UVGridify_1_0_Namespace {

} // End SOP_FeE_UVGridify_1_0_Namespace namespace
