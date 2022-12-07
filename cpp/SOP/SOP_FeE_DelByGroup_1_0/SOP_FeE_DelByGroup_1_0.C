
//#define UT_ASSERT_LEVEL 3
#include "SOP_FeE_DelByGroup_1_0.h"


#include "SOP_FeE_DelByGroup_1_0.proto.h"

#include "GEO/GEO_Detail.h"
#include "PRM/PRM_TemplateBuilder.h"
#include "UT/UT_Interrupt.h"
#include "UT/UT_DSOVersion.h"


#include "GEO_FeE/GEO_FeE_Group.h"
#include "GA_FeE/GA_FeE_VertexNextEquiv.h"
#include "GA_FeE/GA_FeE_Detail.h"

using namespace SOP_FeE_DelByGroup_1_0_Namespace;



static const char *theDsFile = R"THEDSFILE(
{
    name        parameters
    
    parm {
       name    "delByGroup"
       cppname "DelByGroup"
       label   "Delete By Group"
       type    toggle
       default { 1 }
    }
    parm {
        name    "group"
        cppname "Group"
        label   "Group"
        type    string
        default { "" }
        disablewhen "{ delByGroup == 0 }"
        parmtag { "script_action" "import soputils\nkwargs['geometrytype'] = kwargs['node'].parmTuple('grouptype')\nkwargs['inputindex'] = 0\nsoputils.selectGroupParm(kwargs)" }
        parmtag { "script_action_help" "Select geometry from an available viewport." }
        parmtag { "script_action_icon" "BUTTONS_reselect" }
    }
    parm {
        name    "groupType"
        cppname "GroupType"
        label   "Group Type"
        type    ordinal
        default { "guess" }
        disablewhen "{ delByGroup == 0 }"
        menu {
            "guess"     "Guess from Group"
            "prim"      "Primitive"
            "point"     "Point"
            "vertex"    "Vertex"
            "edge"      "Edge"
        }

    }
    

    parm {
        name    "subscribeRatio"
        cppname "SubscribeRatio"
        label   "Subscribe Ratio"
        type    integer
        default { 16 }
        range   { 0! 256 }
        disablewhen "{ delByGroup == 0 }"
    }
    parm {
        name    "minGrainSize"
        cppname "MinGrainSize"
        label   "Min Grain Size"
        type    intlog
        default { 1024 }
        range   { 0! 2048 }
        disablewhen "{ delByGroup == 0 }"
    }

}
)THEDSFILE";



PRM_Template*
SOP_FeE_DelByGroup_1_0::buildTemplates()
{
    static PRM_TemplateBuilder templ("SOP_FeE_DelByGroup_1_0.C"_sh, theDsFile);
    return templ.templates();
}


const UT_StringHolder SOP_FeE_DelByGroup_1_0::theSOPTypeName("FeE::delByGroup::1.0"_sh);

void
newSopOperator(OP_OperatorTable* table)
{
    OP_Operator* newOp = new OP_Operator(
        SOP_FeE_DelByGroup_1_0::theSOPTypeName,
        "FeE Delete by Group",
        SOP_FeE_DelByGroup_1_0::myConstructor,
        SOP_FeE_DelByGroup_1_0::buildTemplates(),
        1,
        1,
        nullptr,
        OP_FLAG_GENERATOR,
        nullptr,
        1,
        "Five elements Elf/Group");

    newOp->setIconName("SOP_blast");
    table->addOperator(newOp);

}


//class SOP_FeE_DelByGroup_1_0Cache : public SOP_NodeCache
//{
//public:
//    SOP_FeE_DelByGroup_1_0Cache() : SOP_NodeCache(),
//        myPrevOutputDetailID(-1)
//    {}
//    ~SOP_FeE_DelByGroup_1_0Cache() override {}
//
//    int64 myPrevOutputDetailID;
//};


class SOP_FeE_DelByGroup_1_0Verb : public SOP_NodeVerb
{
public:
    SOP_FeE_DelByGroup_1_0Verb() {}
    virtual ~SOP_FeE_DelByGroup_1_0Verb() {}

    virtual SOP_NodeParms *allocParms() const { return new SOP_FeE_DelByGroup_1_0Parms(); }
    //virtual SOP_NodeCache* allocCache() const { return new SOP_FeE_DelByGroup_1_0Cache(); }
    virtual UT_StringHolder name() const { return SOP_FeE_DelByGroup_1_0::theSOPTypeName; }

    virtual CookMode cookMode(const SOP_NodeParms *parms) const { return COOK_GENERIC; }

    virtual void cook(const CookParms &cookparms) const;

    /// This static data member automatically registers
    /// this verb class at library ldir0d time.
    static const SOP_NodeVerb::Register<SOP_FeE_DelByGroup_1_0Verb> theVerb;
};

// The static member variable definition has to be outside the class definition.
// The declaration is inside the class.
const SOP_NodeVerb::Register<SOP_FeE_DelByGroup_1_0Verb> SOP_FeE_DelByGroup_1_0Verb::theVerb;

const SOP_NodeVerb *
SOP_FeE_DelByGroup_1_0::cookVerb() const 
{ 
    return SOP_FeE_DelByGroup_1_0Verb::theVerb.get();
}






static GA_GroupType
sopGroupType(SOP_FeE_DelByGroup_1_0Parms::GroupType parmgrouptype)
{
    using namespace SOP_FeE_DelByGroup_1_0Enums;
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
SOP_FeE_DelByGroup_1_0Verb::cook(const SOP_NodeVerb::CookParms &cookparms) const
{
    auto &&sopparms = cookparms.parms<SOP_FeE_DelByGroup_1_0Parms>();
    GA_Detail* outGeo0 = cookparms.gdh().gdpNC();
    //auto sopcache = (SOP_FeE_DelByGroup_1_0Cache*)cookparms.cache();

    const GA_Detail* const inGeo0 = cookparms.inputGeo(0);

    outGeo0->replaceWith(*inGeo0);

    if (!sopparms.getDelByGroup())
        return;


    const GA_GroupType groupType = sopGroupType(sopparms.getGroupType());
    GA_Group* geo0Group = GA_FeE_Group::findGroup(outGeo0, groupType, sopparms.getGroup());
    if (!geo0Group)
        return;


    UT_AutoInterrupt boss("Processing");
    if (boss.wasInterrupted())
        return;


    GA_FeE_Group::delByGroup();

    //const exint subscribeRatio = sopparms.getSubscribeRatio();
    //const exint minGrainSize = sopparms.getMinGrainSize();
    //const exint minGrainSize = pow(2, 8);
    //const exint minGrainSize = pow(2, 4);


    //const GA_Storage& inStorgeF = SYSisSame<T, fpreal32>() ? GA_STORE_REAL32 : GA_STORE_REAL64;
    //const GA_Storage inStorgeF = GA_STORE_REAL32;
    //const GA_Storage inStorgeI = GA_FeE_Type::getPreferredStorageI(outGeo0);
    
}



namespace SOP_FeE_DelByGroup_1_0_Namespace {

} // End SOP_FeE_DelByGroup_1_0_Namespace namespace
