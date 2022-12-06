
//#define UT_ASSERT_LEVEL 3
#include "SOP_FeE_JoinCurve_2_0.h"


// This is an automatically generated header file based on theDsFile, below,
// to provide SOP_FeE_JoinCurve_2_0Parms, an easy way to access parameter values from
// SOP_FeE_JoinCurve_2_0Verb::cook with the correct type.
#include "SOP_FeE_JoinCurve_2_0.proto.h"

#include "GEO/GEO_Detail.h"
#include "PRM/PRM_TemplateBuilder.h"
#include "UT/UT_Interrupt.h"
#include "UT/UT_DSOVersion.h"


#include "GA_FeE/GA_FeE_Attribute.h"
#include "GA_FeE/GA_FeE_VertexNextEquiv.h"
#include "GEO_FeE/GEO_FeE_Group.h"

#include "GA_FeE/GA_FeE_JoinCurve.h"


using namespace SOP_FeE_JoinCurve_2_0_Namespace;

//
// Help is stored in a "wiki" style text file.  This text file should be copied
// to $HOUDINI_PATH/help/nodes/sop/FeE.txt
//
// See the sample_install.sh file for an example.
//

/// This is the internal name of the SOP type.
/// It isn't allowed to be the same as any other SOP's type name.
const UT_StringHolder SOP_FeE_JoinCurve_2_0::theSOPTypeName("FeE::joinCurve::2.0"_sh);

/// newSopOperator is the hook that Houdini grabs from this dll
/// and invokes to register the SOP.  In this case, we add ourselves
/// to the specified operator table.
void
newSopOperator(OP_OperatorTable *table)
{
    table->addOperator(new OP_Operator(
        SOP_FeE_JoinCurve_2_0::theSOPTypeName,   // Internal name
        "FeE Join Curve",     // UI name
        SOP_FeE_JoinCurve_2_0::myConstructor,    // How to build the SOP
        SOP_FeE_JoinCurve_2_0::buildTemplates(), // My parameters
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
        name    "stopPointGroup"
        cppname "StopPointGroup"
        label   "Stop Point Group"
        type    string
        default { "" }
        range   { 0 1 }
        parmtag { "script_action" "import soputils\nkwargs['geometrytype'] = kwargs['node'].parmTuple('grouptype')\nkwargs['inputindex'] = 0\nsoputils.selectGroupParm(kwargs)" }
        parmtag { "script_action_help" "Select geometry from an available viewport." }
        parmtag { "script_action_icon" "BUTTONS_reselect" }
    }
    parm {
        name    "sortPtnum"
        label   "Sort ptnum"
        type    toggle
        default { "0" }
    }


    parm {
        name    "copyAttribNameFromFirstElem"
        label   "Copy Attribute Name from First Elem"
        type    string
        default { "" }
    }
    parm {
        name    "uniqueneb_srcprims"
        label   "Unique Neb Source Prims"
        type    toggle
        default { "1" }
        disablewhen "{ outSrcPrims == 0 }"
    }
    parm {
        name    "keepEdgeGroup"
        label   "Keep Edge Group"
        type    toggle
        default { "1" }
    }
    parm {
        name    "removeDuplicateLoop"
        label   "Remove Duplicate Loop"
        type    toggle
        default { "1" }
    }


    parm {
        name    "primGroup"
        cppname "PrimGroup"
        label   "Prim Group"
        type    string
        default { "" }
        parmtag { "script_action" "import soputils\nkwargs['geometrytype'] = (hou.geometryType.Primitives,)\nkwargs['inputindex'] = 0\nsoputils.selectGroupParm(kwargs)" }
        parmtag { "script_action_help" "Select geometry from an available viewport.\nShift-click to turn on Select Groups." }
        parmtag { "script_action_icon" "BUTTONS_reselect" }
        parmtag { "sop_input" "0" }
    }
    parm {
        name    "pointGroup"
        cppname "PointGroup"
        label   "Point Group"
        type    string
        default { "" }
        parmtag { "script_action" "import soputils\nkwargs['geometrytype'] = (hou.geometryType.Points,)\nkwargs['inputindex'] = 0\nsoputils.selectGroupParm(kwargs)" }
        parmtag { "script_action_help" "Select geometry from an available viewport.\nShift-click to turn on Select Groups." }
        parmtag { "script_action_icon" "BUTTONS_reselect" }
        parmtag { "sop_input" "0" }
    }
    parm {
        name    "vertexGroup"
        cppname "VertexGroup"
        label   "Vertex Group"
        type    string
        default { "" }
        parmtag { "script_action" "import soputils\nkwargs['geometrytype'] = (hou.geometryType.Vertices,)\nkwargs['inputindex'] = 0\nsoputils.selectGroupParm(kwargs)" }
        parmtag { "script_action_help" "Select geometry from an available viewport.\nShift-click to turn on Select Groups." }
        parmtag { "script_action_icon" "BUTTONS_reselect" }
        parmtag { "sop_input" "0" }
    }
    parm {
        name    "edgeGroup"
        cppname "EdgeGroup"
        label   "Edge Group"
        type    string
        default { "" }
        parmtag { "script_action" "import soputils\nkwargs['geometrytype'] = (hou.geometryType.Edges,)\nkwargs['inputindex'] = 0\nsoputils.selectGroupParm(kwargs)" }
        parmtag { "script_action_help" "Select geometry from an available viewport.\nShift-click to turn on Select Groups." }
        parmtag { "script_action_icon" "BUTTONS_reselect" }
        parmtag { "sop_input" "0" }
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
       name    "outSrcPrims"
       cppname "OutSrcPrims"
       label   "Output Source Prims"
       type    toggle
       default { 0 }
       nolabel
       joinnext
    }
    parm {
        name    "srcPrimsAttribName"
        cppname "SrcPrimsAttribName"
        label   "Source Prims Attrib Name"
        type    string
        default { "srcPrims" }
        disablewhen "{ outSrcPrims == 0 }"
    }



    parm {
        name    "primType"
        cppname "PrimType"
        label   "Prim Type"
        type    ordinal
        default { "polyline" }
        menu {
            "polyline"  "Polyline"
            "poly"      "Poly"
        }
    }

    parm {
        name    "keepOrder"
        cppname "KeepOrder"
        label   "Keep Order"
        type    toggle
        default { "0" }
    }
    parm {
       name    "keepLoop"
       cppname "KeepLoop"
       label   "Keep Loop"
       type    toggle
       default { 1 }
    }
    parm {
        name    "closeLoop"
        cppname "CloseLoop"
        label   "Close Loop"
        type    toggle
        default { "0" }
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
       name    "checkInputError"
       cppname "CheckInputError"
       label   "Check Input Error"
       type    toggle
       default { 0 }
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
SOP_FeE_JoinCurve_2_0::buildTemplates()
{
    static PRM_TemplateBuilder templ("SOP_FeE_JoinCurve_2_0.C"_sh, theDsFile);
    return templ.templates();
}

class SOP_FeE_JoinCurve_2_0Verb : public SOP_NodeVerb
{
public:
    SOP_FeE_JoinCurve_2_0Verb() {}
    virtual ~SOP_FeE_JoinCurve_2_0Verb() {}

    virtual SOP_NodeParms *allocParms() const { return new SOP_FeE_JoinCurve_2_0Parms(); }
    virtual UT_StringHolder name() const { return SOP_FeE_JoinCurve_2_0::theSOPTypeName; }

    virtual CookMode cookMode(const SOP_NodeParms *parms) const { return COOK_GENERIC; }

    virtual void cook(const CookParms &cookparms) const;
    
    /// This static data member automatically registers
    /// this verb class at library load time.
    static const SOP_NodeVerb::Register<SOP_FeE_JoinCurve_2_0Verb> theVerb;
};

// The static member variable definition has to be outside the class definition.
// The declaration is inside the class.
const SOP_NodeVerb::Register<SOP_FeE_JoinCurve_2_0Verb> SOP_FeE_JoinCurve_2_0Verb::theVerb;

const SOP_NodeVerb *
SOP_FeE_JoinCurve_2_0::cookVerb() const 
{ 
    return SOP_FeE_JoinCurve_2_0Verb::theVerb.get();
}




static bool
sopPrimPolyIsClosed(SOP_FeE_JoinCurve_2_0Parms::PrimType parmgrouptype)
{
    using namespace SOP_FeE_JoinCurve_2_0Enums;
    switch (parmgrouptype)
    {
    case PrimType::POLYLINE:   return 0;    break;
    case PrimType::POLY:       return 1;    break;
    }
    UT_ASSERT_MSG(0, "Unhandled Prim type!");
    return 0;
}


void
SOP_FeE_JoinCurve_2_0Verb::cook(const SOP_NodeVerb::CookParms& cookparms) const
{
    auto&& sopparms = cookparms.parms<SOP_FeE_JoinCurve_2_0Parms>();
    GEO_Detail* outGeo0 = cookparms.gdh().gdpNC();
    //auto sopcache = (SOP_FeE_JoinCurve_2_0Cache*)cookparms.cache();

    const GA_Detail* const inGeo0 = cookparms.inputGeo(0);

    //outGeo0->replaceWithPoints(*inGeo0);
    outGeo0->replaceWith(*inGeo0);

    if (sopparms.getCheckInputError())
    {
        GA_FeE_JoinCurve::joinCurveCheckInputError(cookparms, outGeo0);
    }

    //GA_PointGroup* groupOneNeb = GA_FeE_TopologyReference::addGroupOneNeb(outGeo0, nullptr);

    const UT_StringHolder& primGroupName = sopparms.getPrimGroup();
    const UT_StringHolder& pointGroupName = sopparms.getPointGroup();
    const UT_StringHolder& vertexGroupName = sopparms.getVertexGroup();
    const UT_StringHolder& edgeGroupName = sopparms.getEdgeGroup();


    GOP_Manager gop;
    const GA_PointGroup* stopPointGroup = GA_FeE_Group::findOrParsePointGroupDetached(cookparms, outGeo0, sopparms.getStopPointGroup(), gop);

    
    const bool keepOrder = sopparms.getKeepOrder();
    const bool keepLoop = sopparms.getKeepLoop();
    const bool closeLoop = sopparms.getCloseLoop();
    


    //const GA_PointGroupUPtr passedPointGroupUPtr = outGeo0->createDetachedPointGroup();
    //GA_PointGroup* passedPointGroup = passedPointGroupUPtr.get();


    //GA_PrimitiveGroupUPtr passedPrimitiveGroupUPtr;
    //GA_PrimitiveGroup* passedPrimitiveGroup = nullptr;

    //if (keepLoop)
    //{
    //    passedPrimitiveGroupUPtr = outGeo0->createDetachedPrimitiveGroup();
    //    passedPrimitiveGroup = passedPrimitiveGroupUPtr.get();
    //}
    



    const bool hasInputGroup = primGroupName.isstring() || pointGroupName.isstring() || vertexGroupName.isstring() || edgeGroupName.isstring();
    GA_VertexGroup* geo0VtxGroup = nullptr;
    GA_VertexGroupUPtr geo0vtxGroupUPtr;
    if (hasInputGroup)
    {
        //geo0vtxGroupUPtr = tmpGeo0->createDetachedVertexGroup();
        geo0VtxGroup = geo0vtxGroupUPtr.get();
        if (primGroupName.isstring())
        {

        }

        if (pointGroupName.isstring())
        {

        }

        if (vertexGroupName.isstring())
        {

        }

        if (edgeGroupName.isstring())
        {

        }
    }

    //const bool isClosed = sopPrimPolyIsClosed(sopparms.getPrimType());
    const UT_StringHolder& srcPrimsAttribName = sopparms.getSrcPrimsAttribName();
    const bool outSrcPrims = sopparms.getOutSrcPrims() && srcPrimsAttribName.isstring();

    const exint kernel = sopparms.getKernel();

    //const exint subscribeRatio = sopparms.getSubscribeRatio();
    //const exint minGrainSize = sopparms.getMinGrainSize();
    

    const GA_Storage inStorageI = GA_FeE_Type::getPreferredStorageI(outGeo0);


    GA_Attribute* srcPrimsAttrib = nullptr;
    if (outSrcPrims)
    {
        srcPrimsAttrib = outGeo0->addIntArray(GA_ATTRIB_PRIMITIVE, srcPrimsAttribName, 1, nullptr, nullptr, inStorageI);
    }

    UT_AutoInterrupt boss("Processing");
    if (boss.wasInterrupted())
        return;



    switch (kernel)
    {
    case 0:
        GA_FeE_JoinCurve::joinCurve(outGeo0, stopPointGroup, keepOrder, keepLoop, closeLoop, srcPrimsAttrib);
        break;
    case 1:
//        for (GA_Iterator it(outGeo0->getPointRange(groupOneNeb)); it.fullBlockAdvance(start, end); )
//        {
//            for (GA_Offset ptoff = start; ptoff < end; ++ptoff)
//            {
//                if (KeepOrder)
//                {
//                }
//                if (passedPointGroup->contains(ptoff))
//                    continue;
//                if (stopPointGroup && stopPointGroup->contains(ptoff))
//                    continue;
//                const GA_Primitive* newPrim = outGeo0->appendPrimitive(GA_PRIMPOLY);
//
//                GA_Offset newPrimoff = newPrim->getMapOffset();
//
//
//                GA_Offset primoff_neb = newPrim->getMapOffset();
//#if 0
//                while (1) {
//#else
//                for (int LOOPCOUNT = 0; LOOPCOUNT <= MAXLOOPCOUNT; ++LOOPCOUNT) {
//                    if (LOOPCOUNT == MAXLOOPCOUNT)
//                    {
//                        UT_ASSERT("Unsupport Input Geo so arrive death loop");
//                    }
//#endif
//
//
//                }
//                if (keepLoop)
//                {
//                    passedPrimitiveGroup->setElement(primoff, true);
//                }
//                passedPointGroup->setElement(ptoff, true);
//                if (outSrcPrims)
//                {
//                    srcPrims.emplace_back();
//                    srcPrimsAttribH.set(newPrimoff, srcPrims);
//                }
//                if (outSrcPrims)
//                {
//                    srcPrims.clear();
//                }
//            }
//        }
        break;
    }


    outGeo0->bumpDataIdsForAddOrRemove(0, 1, 1);
    //outGeo0->bumpDataIdsForRewire();

    //tmpGeoH0.deleteGdp();
    
}



namespace SOP_FeE_JoinCurve_2_0_Namespace {

} // End SOP_FeE_JoinCurve_2_0_Namespace namespace
