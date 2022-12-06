
//#define UT_ASSERT_LEVEL 3
#include "SOP_FeE_DelVertex_1_0.h"


// This is an automatically generated header file based on theDsFile, below,
// to provide SOP_FeE_DelVertex_1_0Parms, an easy way to access parameter values from
// SOP_FeE_DelVertex_1_0Verb::cook with the correct type.
#include "SOP_FeE_DelVertex_1_0.proto.h"

#include "GEO/GEO_Detail.h"
#include "PRM/PRM_TemplateBuilder.h"
#include "UT/UT_Interrupt.h"
#include "UT/UT_DSOVersion.h"


//#include "GA_FeE/GA_FeE_Attribute.h"
//#include "GA_FeE/GA_FeE_VertexNextEquiv.h"
#include "GEO_FeE/GEO_FeE_Group.h"
#include "GA_FeE/GA_FeE_GroupPromote.h"



using namespace SOP_FeE_DelVertex_1_0_Namespace;

//
// Help is stored in a "wiki" style text file.  This text file should be copied
// to $HOUDINI_PATH/help/nodes/sop/FeE.txt
//
// See the sample_install.sh file for an example.
//

/// This is the internal name of the SOP type.
/// It isn't allowed to be the same as any other SOP's type name.
const UT_StringHolder SOP_FeE_DelVertex_1_0::theSOPTypeName("FeE::delVertex::1.0"_sh);

/// newSopOperator is the hook that Houdini grabs from this dll
/// and invokes to register the SOP.  In this case, we add ourselves
/// to the specified operator table.
void
newSopOperator(OP_OperatorTable *table)
{
    table->addOperator(new OP_Operator(
        SOP_FeE_DelVertex_1_0::theSOPTypeName,   // Internal name
        "FeE Delete Vertex",     // UI name
        SOP_FeE_DelVertex_1_0::myConstructor,    // How to build the SOP
        SOP_FeE_DelVertex_1_0::buildTemplates(), // My parameters
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
        parmtag { "script_action" "import soputils\nkwargs['geometrytype'] = kwargs['node'].parmTuple('groupType')\nkwargs['inputindex'] = 0\nsoputils.selectGroupParm(kwargs)" }
        parmtag { "script_action_help" "Select geometry from an available viewport.\nShift-click to turn on Select Groups." }
        parmtag { "script_action_icon" "BUTTONS_reselect" }
        parmtag { "sop_input" "0" }
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
        name    "attribFromVertex"
        cppname "AttribFromVertex"
        label   "Attrib from Vertex"
        type    string
        default { "" }
    }
    parm {
        name    "attribFromPrim"
        cppname "AttribFromPrim"
        label   "Attrib from Prim"
        type    string
        default { "" }
    }
    parm {
        name    "groupFromVertex"
        cppname "GroupFromVertex"
        label   "Group from Vertex"
        type    string
        default { "" }
    }
    parm {
        name    "groupFromPrim"
        cppname "GroupFromPrim"
        label   "Group from Prim"
        type    string
        default { "" }
    }
    parm {
        name    "groupFromEdge"
        cppname "GroupFromEdge"
        label   "Group from Edge"
        type    string
        default { "" }
    }

    parm {
        name    "promoteEdgeGroupToPrim"
        cppname "PromoteEdgeGroupToPrim"
        label   "Promote Edge Group to Prim"
        type    string
        default { "" }
    }


    parm {
       name    "delDegeneratePrims"
       cppname "DelDegeneratePrims"
       label   "Delete Degenerate Primitives"
       type    toggle
       default { 1 }
    }

    parm {
       name    "delUnusedPoints"
       cppname "DelUnusedPoints"
       label   "Delete Unused Points"
       type    toggle
       default { 1 }
    }




    parm {
       name    "kernel"
       cppname "Kernel"
       label   "Kernel"
       type    integer
       default { 0 }
       range   { 0! 1! }
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
SOP_FeE_DelVertex_1_0::buildTemplates()
{
    static PRM_TemplateBuilder templ("SOP_FeE_DelVertex_1_0.C"_sh, theDsFile);
    return templ.templates();
}

class SOP_FeE_DelVertex_1_0Verb : public SOP_NodeVerb
{
public:
    SOP_FeE_DelVertex_1_0Verb() {}
    virtual ~SOP_FeE_DelVertex_1_0Verb() {}

    virtual SOP_NodeParms *allocParms() const { return new SOP_FeE_DelVertex_1_0Parms(); }
    virtual UT_StringHolder name() const { return SOP_FeE_DelVertex_1_0::theSOPTypeName; }

    virtual CookMode cookMode(const SOP_NodeParms *parms) const { return COOK_GENERIC; }

    virtual void cook(const CookParms &cookparms) const;
    
    /// This static data member automatically registers
    /// this verb class at library load time.
    static const SOP_NodeVerb::Register<SOP_FeE_DelVertex_1_0Verb> theVerb;
};

// The static member variable definition has to be outside the class definition.
// The declaration is inside the class.
const SOP_NodeVerb::Register<SOP_FeE_DelVertex_1_0Verb> SOP_FeE_DelVertex_1_0Verb::theVerb;

const SOP_NodeVerb *
SOP_FeE_DelVertex_1_0::cookVerb() const 
{ 
    return SOP_FeE_DelVertex_1_0Verb::theVerb.get();
}










static GA_GroupType
sopGroupType(SOP_FeE_DelVertex_1_0Parms::GroupType parmgrouptype)
{
    using namespace SOP_FeE_DelVertex_1_0Enums;
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
SOP_FeE_DelVertex_1_0Verb::cook(const SOP_NodeVerb::CookParms& cookparms) const
{
    auto&& sopparms = cookparms.parms<SOP_FeE_DelVertex_1_0Parms>();
    GEO_Detail* outGeo0 = cookparms.gdh().gdpNC();
    //auto sopcache = (SOP_FeE_DelVertex_1_0Cache*)cookparms.cache();

    const GA_Detail* const inGeo0 = cookparms.inputGeo(0);

    //outGeo0->replaceWithPoints(*inGeo0);
    outGeo0->replaceWith(*inGeo0);




    GOP_Manager gop;
    const GA_GroupType groupType = sopGroupType(sopparms.getGroupType());
    //const GA_Group* geo0Group = GA_FeE_Group::findOrParseGroupDetached(cookparms, outGeo0, groupType, sopparms.getGroup(), gop);
    const GA_Group* geo0Group = GA_FeE_Group::parseGroupDetached(cookparms, outGeo0, groupType, sopparms.getGroup(), gop);
    if (geo0Group && geo0Group->isEmpty())
        return;

    //const exint subscribeRatio = sopparms.getSubscribeRatio();
    //const exint minGrainSize = sopparms.getMinGrainSize();

    //if (geo0Group)
    //{
    //    GA_GroupType type = geo0Group->classType();
    //    int a = 1;
    //}
    //const GA_Storage inStorageI = GA_FeE_Type::getPreferredStorageI(outGeo0);



    UT_AutoInterrupt boss("Processing");
    if (boss.wasInterrupted())
        return;



    //const GA_VertexGroup* geo0VtxGroup = GA_FeE_GroupPromote::groupFindPromoteVertexDetached(outGeo0, geo0Group);

    const GA_VertexGroup* geo0VtxGroup = GA_FeE_GroupPromote::groupPromoteVertexDetached(outGeo0, geo0Group);

    const bool delDegeneratePrims = sopparms.getDelDegeneratePrims();
    const bool delUnusedPoints = sopparms.getDelUnusedPoints();

    //outGeo0->destroyVertexOffsets(outGeo0->getVertexRange(geo0VtxGroup));

    GA_Topology& topo = outGeo0->getTopology();
    const GA_ATITopology* vtxPrimRef = topo.getPrimitiveRef();

    GA_Offset start, end;
    for (GA_Iterator it(outGeo0->getVertexRange(geo0VtxGroup)); it.fullBlockAdvance(start, end); )
    {
        for (GA_Offset vtxoff = start; vtxoff < end; ++vtxoff)
        {
            GEO_Primitive* prim = outGeo0->getGEOPrimitive(vtxPrimRef->getLink(vtxoff));
            prim->releaseVertex(vtxoff);
            topo.delVertex(vtxoff);
            //outGeo0->destroyVertexOffset(vtxoff);
        }
    }
    if (delDegeneratePrims)
        outGeo0->destroyDegeneratePrimitives();
    if (delUnusedPoints)
        outGeo0->destroyUnusedPoints();


    //if (geo0VtxGroup->isDetached())
    //    outGeo0->destroyGroup(const_cast<GA_VertexGroup*>(geo0VtxGroup));


    //outGeo0->bumpAllDataIds();
    if (delDegeneratePrims)
    outGeo0->bumpDataIdsForAddOrRemove(1, 1, 1);
    //outGeo0->bumpDataIdsForRewire();


}



namespace SOP_FeE_DelVertex_1_0_Namespace {

} // End SOP_FeE_DelVertex_1_0_Namespace namespace
