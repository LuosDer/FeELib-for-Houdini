
//#define UT_ASSERT_LEVEL 3
#include "SOP_FeE_GroupCombine_1_0.h"


// This is an automatically generated header file based on theDsFile, below,
// to provide SOP_FeE_GroupCombine_1_0Parms, an easy way to access parameter values from
// SOP_FeE_GroupCombine_1_0Verb::cook with the correct type.
#include "SOP_FeE_GroupCombine_1_0.proto.h"

#include <GU/GU_Detail.h>
#include <GEO/GEO_PrimPoly.h>
#include <OP/OP_Operator.h>
#include <OP/OP_OperatorTable.h>
#include <PRM/PRM_Include.h>
#include <PRM/PRM_TemplateBuilder.h>
#include <UT/UT_DSOVersion.h>
#include <UT/UT_Interrupt.h>
#include <UT/UT_StringHolder.h>
#include <SYS/SYS_Math.h>
#include <limits.h>

#include <GA/GA_Primitive.h>


#include <UT/UT_UniquePtr.h>
#include <GA/GA_SplittableRange.h>
#include <HOM/HOM_SopNode.h>


#include <GU/GU_Promote.h>

#include <GA/GA_EdgeGroup.h>

#include <GA_FeE/GA_FeE_Group.h>
//#include <GA_FeE/GA_FeE_Adjacency.h>
#include <GA_FeE/GA_FeE_VertexNextEquiv.h>


using namespace SOP_FeE_GroupCombine_1_0_Namespace;

//
// Help is stored in a "wiki" style text file.  This text file should be copied
// to $HOUDINI_PATH/help/nodes/sop/FeE.txt
//
// See the sample_install.sh file for an example.
//

/// This is the internal name of the SOP type.
/// It isn't allowed to be the same as any other SOP's type name.
const UT_StringHolder SOP_FeE_GroupCombine_1_0::theSOPTypeName("FeE::groupCombine::1.0"_sh);

/// newSopOperator is the hook that Houdini grabs from this dll
/// and invokes to register the SOP.  In this case, we add ourselves
/// to the specified operator table.
void
newSopOperator(OP_OperatorTable *table)
{
    table->addOperator(new OP_Operator(
        SOP_FeE_GroupCombine_1_0::theSOPTypeName,   // Internal name
        "FeE Group Combine",                        // UI name
        SOP_FeE_GroupCombine_1_0::myConstructor,    // How to build the SOP
        SOP_FeE_GroupCombine_1_0::buildTemplates(), // My parameters
        1,                         // Min # of sources
        1,                         // Max # of sources
        nullptr,                   // Custom local variables (none)
        OP_FLAG_GENERATOR));       // Flag it as generator
}

/// This is a multi-line raw string specifying the parameter interface
/// for this SOP.
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
        menu {
            "guess"     "Guess from Group"
            "prim"      "Primitive"
            "point"     "Point"
            "vertex"    "Vertex"
            "edge"      "Edge"
        }
    }
    
    
    parm {
        name    "combineGroupName"
        cppname "CombineGroupName"
        label   "Combine Group Name"
        type    string
        default { "" }
    }
    parm {
        name    "combineGroupType"
        cppname "CombineGroupType"
        label   "Combine Group Type"
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
SOP_FeE_GroupCombine_1_0::buildTemplates()
{
    static PRM_TemplateBuilder templ("SOP_FeE_GroupCombine_1_0.C"_sh, theDsFile);
    return templ.templates();
}


//class SOP_FeE_GroupCombine_1_0Cache : public SOP_NodeCache
//{
//public:
//    SOP_FeE_GroupCombine_1_0Cache() : SOP_NodeCache(),
//        myPrevOutputDetailID(-1)
//    {}
//    ~SOP_FeE_GroupCombine_1_0Cache() override {}
//
//    int64 myPrevOutputDetailID;
//};


class SOP_FeE_GroupCombine_1_0Verb : public SOP_NodeVerb
{
public:
    SOP_FeE_GroupCombine_1_0Verb() {}
    virtual ~SOP_FeE_GroupCombine_1_0Verb() {}

    virtual SOP_NodeParms *allocParms() const { return new SOP_FeE_GroupCombine_1_0Parms(); }
    //virtual SOP_NodeCache* allocCache() const { return new SOP_FeE_GroupCombine_1_0Cache(); }
    virtual UT_StringHolder name() const { return SOP_FeE_GroupCombine_1_0::theSOPTypeName; }

    virtual CookMode cookMode(const SOP_NodeParms *parms) const { return COOK_GENERIC; }

    virtual void cook(const CookParms &cookparms) const;

    /// This static data member automatically registers
    /// this verb class at library ldir0d time.
    static const SOP_NodeVerb::Register<SOP_FeE_GroupCombine_1_0Verb> theVerb;
};

// The static member variable definition has to be outside the class definition.
// The declaration is inside the class.
const SOP_NodeVerb::Register<SOP_FeE_GroupCombine_1_0Verb> SOP_FeE_GroupCombine_1_0Verb::theVerb;

const SOP_NodeVerb *
SOP_FeE_GroupCombine_1_0::cookVerb() const 
{ 
    return SOP_FeE_GroupCombine_1_0Verb::theVerb.get();
}







static GA_GroupType
sopCombineGroupType(SOP_FeE_GroupCombine_1_0Parms::CombineGroupType parmgrouptype)
{
    using namespace SOP_FeE_GroupCombine_1_0Enums;
    switch (parmgrouptype)
    {
    case CombineGroupType::GUESS:     return GA_GROUP_INVALID;    break;
    case CombineGroupType::PRIM:      return GA_GROUP_PRIMITIVE;  break;
    case CombineGroupType::POINT:     return GA_GROUP_POINT;      break;
    case CombineGroupType::VERTEX:    return GA_GROUP_VERTEX;     break;
    case CombineGroupType::EDGE:      return GA_GROUP_EDGE;       break;
    }
    UT_ASSERT_MSG(0, "Unhandled geo0Group type!");
    return GA_GROUP_INVALID;
}




static GA_GroupType
sopGroupType(SOP_FeE_GroupCombine_1_0Parms::GroupType parmgrouptype)
{
    using namespace SOP_FeE_GroupCombine_1_0Enums;
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
SOP_FeE_GroupCombine_1_0Verb::cook(const SOP_NodeVerb::CookParms &cookparms) const
{
    auto &&sopparms = cookparms.parms<SOP_FeE_GroupCombine_1_0Parms>();
    GEO_Detail* outGeo0 = cookparms.gdh().gdpNC();
    //auto sopcache = (SOP_FeE_GroupCombine_1_0Cache*)cookparms.cache();

    const GEO_Detail* const inGeo0 = cookparms.inputGeo(0);

    outGeo0->replaceWith(*inGeo0);
    // outGeo0->clearAndDestroy();

    //outGeo0 = sopNodeProcess(*inGeo0);


    const UT_StringHolder& geo0AttribNames = sopparms.getCombineGroupName();
    if (!geo0AttribNames.isstring())
        return;


    GOP_Manager gop;
    const GA_GroupType groupType = sopGroupType(sopparms.getGroupType());
    const GA_Group* geo0Group = GA_FeE_Group::parseGroupDetached(cookparms, outGeo0, groupType, sopparms.getGroup(), gop);
    if (!geo0Group)
        return;
    
    //if (!geo0Group || geo0Group->isEmpty())
    if (!geo0Group || GA_FeE_Group::isEmpty(geo0Group, groupType))
        return;


    const GA_GroupType combineGroupType = sopCombineGroupType(sopparms.getCombineGroupType());
    GA_Group* combineGroup = GA_FeE_Group::findGroup(outGeo0, combineGroupType, geo0AttribNames);
    if (!combineGroup)
        return;

    UT_AutoInterrupt boss("Processing");
    if (boss.wasInterrupted())
        return;

    
    const exint subscribeRatio = sopparms.getSubscribeRatio();
    const exint minGrainSize = sopparms.getMinGrainSize();
    //const exint minGrainSize = pow(2, 8);
    //const exint minGrainSize = pow(2, 4);


    //const GA_Storage& inStorgeF = SYSisSame<T, fpreal32>() ? GA_STORE_REAL32 : GA_STORE_REAL64;
    //const GA_Storage inStorgeF = GA_STORE_REAL32;
    const GA_Storage inStorgeI = GA_STORE_INT32;


    //const GA_AttributeOwner combineAttribType = GA_FeE_Group::attributeOwner_groupType(combineGroupType);
    //GA_ElementGroup* combineGroup = outGeo0->findElementGroup(combineAttribType, geo0AttribNames);
    //combineGroup->combine(geo0Group);

    //bool success = true;
    //GOP_Manager gop;
    //const GA_Group* geo0EdgeGroup = gop.parseEdgeDetached(sopparms.getGroup(), outGeo0, true, success);

    //const GA_EdgeGroup* geo0EdgeGroup = static_cast<const GA_EdgeGroup*>(geo0Group);
    GA_FeE_Group::combine(outGeo0, combineGroup, geo0Group);

    //for (auto it = static_cast<const GA_EdgeGroup*>(geo0EdgeGroup)->begin(); !it.atEnd(); it.advance())
    //{
    //    const GA_Edge& edge = it.getEdge();
    //    static_cast<GA_EdgeGroup*>(combineGroup)->add(edge.p0(), edge.p1());
    //}


    //GA_VertexGroup* unsharedGroup = GA_FeE_VertexNextEquiv::addGroupVertexNextEquiv(outGeo0, "__topo_unshared_SOP_FeE_GroupCombine_1_0", geo0VtxGroup);
    //GA_Group* unshared_promoGroup = GA_FeE_Group::groupPromote(outGeo0, unsharedGroup, unsharedAttribClass, geo0AttribNames, false, true);

    //combineGroup->bumpDataId();
    GA_FeE_Group::bumpDataId(combineGroup);

    //GA_EdgeGroup* combineEdgeGroup = static_cast<GA_EdgeGroup*>(combineGroup);
    // 
    //static_cast<GA_EdgeGroup*>(combineGroup)->makeAllEdgesValid();
    



}



namespace SOP_FeE_GroupCombine_1_0_Namespace {

} // End SOP_FeE_GroupCombine_1_0_Namespace namespace
