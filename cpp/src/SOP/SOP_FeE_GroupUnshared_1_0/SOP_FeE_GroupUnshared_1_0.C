
//#define UT_ASSERT_LEVEL 3
#include "SOP_FeE_GroupUnshared_1_0.h"


#include "SOP_FeE_GroupUnshared_1_0.proto.h"

#include "GA/GA_Detail.h"
#include "PRM/PRM_TemplateBuilder.h"
#include "UT/UT_Interrupt.h"
#include "UT/UT_DSOVersion.h"


#include "GFE/GFE_GroupUnshared.h"

using namespace SOP_FeE_GroupUnshared_1_0_Namespace;

static const char *theDsFile = R"THEDSFILE(
{
    name        parameters
    parm {
        name    "group"
        cppname "Group"
        label   "Group"
        type    string
        default { "" }
        menutoggle {
            [ "" ]
        }
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
        name    "unsharedAttribName"
        cppname "UnsharedAttribName"
        label   "Unshared Attrib Name"
        type    string
        default { "unshared" }
    }
    parm {
        name    "unsharedAttribType"
        cppname "UnsharedAttribType"
        label   "Unshared Attrib Type"
        type    ordinal
        default { "vertex" }
        menu {
            "group"     "Group"
            "int"       "Integer"
            "float"     "Float"
            "string"    "String"
        }
    }
    parm {
        name    "unsharedAttribClass"
        cppname "UnsharedAttribClass"
        label   "Unshared Attrib Class"
        type    ordinal
        default { "vertex" }
        menu {
            "prim"      "Primitive"
            "point"     "Point"
            "vertex"    "Vertex"
            "edge"      "Edge"
        }
    }


    parm {
        name    "groupUnsharedAfterFuse"
        cppname "GroupUnsharedAfterFuse"
        label   "Group Unshared after Fuse"
        type    toggle
        default { "0" }
    }
    parm {
        name    "fuseDist"
        cppname "FuseDist"
        label   "Fuse Distance"
        type    log
        default { 1e-05 }
        range   { 1e-03 10 }
        disablewhen "{ groupUnsharedAfterFuse == 0 }"
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
SOP_FeE_GroupUnshared_1_0::buildTemplates()
{
    static PRM_TemplateBuilder templ("SOP_FeE_GroupUnshared_1_0.C"_sh, theDsFile);
    if (templ.justBuilt())
    {
        templ.setChoiceListPtr("group"_sh, &SOP_Node::allGroupMenu);
    }
    return templ.templates();
}


//class SOP_FeE_GroupUnshared_1_0Cache : public SOP_NodeCache
//{
//public:
//    SOP_FeE_GroupUnshared_1_0Cache() : SOP_NodeCache(),
//        myPrevOutputDetailID(-1)
//    {}
//    ~SOP_FeE_GroupUnshared_1_0Cache() override {}
//
//    int64 myPrevOutputDetailID;
//};


class SOP_FeE_GroupUnshared_1_0Verb : public SOP_NodeVerb
{
public:
    SOP_FeE_GroupUnshared_1_0Verb() {}
    virtual ~SOP_FeE_GroupUnshared_1_0Verb() {}

    virtual SOP_NodeParms *allocParms() const { return new SOP_FeE_GroupUnshared_1_0Parms(); }
    //virtual SOP_NodeCache* allocCache() const { return new SOP_FeE_GroupUnshared_1_0Cache(); }
    virtual UT_StringHolder name() const { return SOP_FeE_GroupUnshared_1_0::theSOPTypeName; }

    virtual CookMode cookMode(const SOP_NodeParms *parms) const { return COOK_GENERIC; }

    virtual void cook(const CookParms &cookparms) const;

    static const SOP_NodeVerb::Register<SOP_FeE_GroupUnshared_1_0Verb> theVerb;
};

const SOP_NodeVerb::Register<SOP_FeE_GroupUnshared_1_0Verb> SOP_FeE_GroupUnshared_1_0Verb::theVerb;

const SOP_NodeVerb *
SOP_FeE_GroupUnshared_1_0::cookVerb() const 
{ 
    return SOP_FeE_GroupUnshared_1_0Verb::theVerb.get();
}




const UT_StringHolder SOP_FeE_GroupUnshared_1_0::theSOPTypeName("FeE::groupUnshared::1.0"_sh);

void
newSopOperator(OP_OperatorTable* table)
{
    OP_Operator* newOp = new OP_Operator(
        SOP_FeE_GroupUnshared_1_0::theSOPTypeName,
        "FeE Group Unshared",
        SOP_FeE_GroupUnshared_1_0::myConstructor,
        SOP_FeE_GroupUnshared_1_0::buildTemplates(),
        1,
        1,
        nullptr,
        OP_FLAG_GENERATOR,
        nullptr,
        1,
        "Five elements Elf/Data/Topology");

    newOp->setIconName("SOP_primitive");
    table->addOperator(newOp);

}



static GA_StorageClass
sopUnsharedStorageClass(SOP_FeE_GroupUnshared_1_0Parms::UnsharedAttribType parmgrouptype)
{
    using namespace SOP_FeE_GroupUnshared_1_0Enums;
    switch (parmgrouptype)
    {
    case UnsharedAttribType::GROUP:   return GA_STORECLASS_OTHER;    break;
    case UnsharedAttribType::INT:     return GA_STORECLASS_INT;      break;
    case UnsharedAttribType::FLOAT:   return GA_STORECLASS_REAL;     break;
    case UnsharedAttribType::STRING:  return GA_STORECLASS_STRING;   break;
    }
    UT_ASSERT_MSG(0, "Unhandled geo0Group type!");
    return GA_STORECLASS_INVALID;
}


static GA_GroupType
sopUnsharedAttribClass(SOP_FeE_GroupUnshared_1_0Parms::UnsharedAttribClass parmgrouptype)
{
    using namespace SOP_FeE_GroupUnshared_1_0Enums;
    switch (parmgrouptype)
    {
    case UnsharedAttribClass::PRIM:      return GA_GROUP_PRIMITIVE;  break;
    case UnsharedAttribClass::POINT:     return GA_GROUP_POINT;      break;
    case UnsharedAttribClass::VERTEX:    return GA_GROUP_VERTEX;     break;
    case UnsharedAttribClass::EDGE:      return GA_GROUP_EDGE;       break;
    }
    UT_ASSERT_MSG(0, "Unhandled geo0Group type!");
    return GA_GROUP_INVALID;
}



static GA_GroupType
sopGroupType(SOP_FeE_GroupUnshared_1_0Parms::GroupType parmgrouptype)
{
    using namespace SOP_FeE_GroupUnshared_1_0Enums;
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
SOP_FeE_GroupUnshared_1_0Verb::cook(const SOP_NodeVerb::CookParms &cookparms) const
{
    auto &&sopparms = cookparms.parms<SOP_FeE_GroupUnshared_1_0Parms>();
    GA_Detail* const outGeo0 = cookparms.gdh().gdpNC();
    //auto sopcache = (SOP_FeE_GroupUnshared_1_0Cache*)cookparms.cache();
    
    const GA_Detail* const inGeo0 = cookparms.inputGeo(0);

    outGeo0->replaceWith(*inGeo0);
    // outGeo0->clearAndDestroy();

    //outGeo0 = sopNodeProcess(*inGeo0);


    const UT_StringHolder& geo0AttribNames = sopparms.getUnsharedAttribName();
    if (!geo0AttribNames.isstring())
        return;


    UT_AutoInterrupt boss("Processing");
    if (boss.wasInterrupted())
        return;


    const GA_GroupType groupType = sopGroupType(sopparms.getGroupType());

    const bool groupUnsharedAfterFuse = sopparms.getGroupUnsharedAfterFuse();

    const GA_StorageClass unsharedAttribStorageClass = sopUnsharedStorageClass(sopparms.getUnsharedAttribType());
    const GA_GroupType unsharedAttribClass = sopUnsharedAttribClass(sopparms.getUnsharedAttribClass());

    const exint subscribeRatio = sopparms.getSubscribeRatio();
    const exint minGrainSize = sopparms.getMinGrainSize();


    //const GA_Precision PreferredPrecision = outGeo0->getPreferredPrecision();
    //const GA_Storage inStorageI = GFE_Type::getPreferredStorageI(PreferredPrecision);

    GFE_GroupUnshared groupUnshared(cookparms, outGeo0);

    groupUnshared.groupParser.setGroup(groupType, sopparms.getGroup());

    groupUnshared.findOrCreate(unsharedAttribClass, unsharedAttribStorageClass, GA_STORE_INVALID, false, geo0AttribNames);
    groupUnshared.setComputeParm(groupUnsharedAfterFuse, sopparms.getFuseDist(), sopparms.getOutTopoAttrib(),
        subscribeRatio, minGrainSize);

    groupUnshared.computeAndBumpDataId();

    groupUnshared.visualizeOutGroup();
    //GFE_GroupUnshared::groupUnshared(cookparms, outGeo0, groupType, sopparms.getGroup(),
    //    geo0AttribNames,
    //    unsharedAttribStorageClass, unsharedAttribClass,
    //    groupUnsharedAfterFuse, sopparms.getOutTopoAttrib(),
    //    subscribeRatio, minGrainSize);
}



namespace SOP_FeE_GroupUnshared_1_0_Namespace {

} // End SOP_FeE_GroupUnshared_1_0_Namespace namespace
