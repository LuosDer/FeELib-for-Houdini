
//#define UT_ASSERT_LEVEL 3
#include "SOP_FeE_ConvertLine_1_0.h"


// This is an automatically generated header file based on theDsFile, below,
// to provide SOP_FeE_ConvertLine_1_0Parms, an easy way to access parameter values from
// SOP_FeE_ConvertLine_1_0Verb::cook with the correct type.
#include "SOP_FeE_ConvertLine_1_0.proto.h"

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


#include <GEO_FeE/GEO_FeE_Adjacency.h>

using namespace SOP_FeE_ConvertLine_1_0_Namespace;

using attribPrecisonF = fpreal32;
using TAttribTypeV = UT_Vector3T<attribPrecisonF>;

//
// Help is stored in a "wiki" style text file.  This text file should be copied
// to $HOUDINI_PATH/help/nodes/sop/FeE.txt
//
// See the sample_install.sh file for an example.
//

/// This is the internal name of the SOP type.
/// It isn't allowed to be the same as any other SOP's type name.
const UT_StringHolder SOP_FeE_ConvertLine_1_0::theSOPTypeName("FeE::convertLine::1.0"_sh);

/// newSopOperator is the hook that Houdini grabs from this dll
/// and invokes to register the SOP.  In this case, we add ourselves
/// to the specified operator table.
void
newSopOperator(OP_OperatorTable *table)
{
    table->addOperator(new OP_Operator(
        SOP_FeE_ConvertLine_1_0::theSOPTypeName,   // Internal name
        "FeE Convert Line",                     // UI name
        SOP_FeE_ConvertLine_1_0::myConstructor,    // How to build the SOP
        SOP_FeE_ConvertLine_1_0::buildTemplates(), // My parameters
        1,                         // Min # of sources
        1,                         // Max # of sources
        nullptr,                   // Custom local variables (none)
        OP_FLAG_GENERATOR));       // Flag it as generator
}

/// This is a multi-line raw string specifying the parameter interface
/// for this SOP.
static const char *theDsFile = R"THEDSFILE(
{
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
       label   "Source Prims"
       type    toggle
       default { 0 }
    }
    parm {
        name    "srcPrimsAttribName"
        cppname "SrcPrimsAttribName"
        label   "Source Prims Attrib Name"
        type    string
        default { "srcPrims" }
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
SOP_FeE_ConvertLine_1_0::buildTemplates()
{
    static PRM_TemplateBuilder templ("SOP_FeE_ConvertLine_1_0.C"_sh, theDsFile);
    return templ.templates();
}


//class SOP_FeE_ConvertLine_1_0Cache : public SOP_NodeCache
//{
//public:
//    SOP_FeE_ConvertLine_1_0Cache() : SOP_NodeCache(),
//        myPrevOutputDetailID(-1)
//    {}
//    ~SOP_FeE_ConvertLine_1_0Cache() override {}
//
//    int64 myPrevOutputDetailID;
//};


class SOP_FeE_ConvertLine_1_0Verb : public SOP_NodeVerb
{
public:
    SOP_FeE_ConvertLine_1_0Verb() {}
    virtual ~SOP_FeE_ConvertLine_1_0Verb() {}

    virtual SOP_NodeParms *allocParms() const { return new SOP_FeE_ConvertLine_1_0Parms(); }
    //virtual SOP_NodeCache* allocCache() const { return new SOP_FeE_ConvertLine_1_0Cache(); }
    virtual UT_StringHolder name() const { return SOP_FeE_ConvertLine_1_0::theSOPTypeName; }

    virtual CookMode cookMode(const SOP_NodeParms *parms) const { return COOK_GENERIC; }

    virtual void cook(const CookParms &cookparms) const;

    /// This static data member automatically registers
    /// this verb class at library ldir0d time.
    static const SOP_NodeVerb::Register<SOP_FeE_ConvertLine_1_0Verb> theVerb;
};

// The static member variable definition has to be outside the class definition.
// The declaration is inside the class.
const SOP_NodeVerb::Register<SOP_FeE_ConvertLine_1_0Verb> SOP_FeE_ConvertLine_1_0Verb::theVerb;

const SOP_NodeVerb *
SOP_FeE_ConvertLine_1_0::cookVerb() const 
{ 
    return SOP_FeE_ConvertLine_1_0Verb::theVerb.get();
}





static bool
sopPrimPolyIsClosed(SOP_FeE_ConvertLine_1_0Parms::PrimType parmgrouptype)
{
    using namespace SOP_FeE_ConvertLine_1_0Enums;
    switch (parmgrouptype)
    {
    case PrimType::POLYLINE:   return 0;    break;
    case PrimType::POLY:       return 1;    break;
    }
    UT_ASSERT_MSG(0, "Unhandled Prim type!");
    return 0;
}



void
SOP_FeE_ConvertLine_1_0Verb::cook(const SOP_NodeVerb::CookParms& cookparms) const
{
    auto&& sopparms = cookparms.parms<SOP_FeE_ConvertLine_1_0Parms>();
    GU_Detail* outGeo0 = cookparms.gdh().gdpNC();
    //auto sopcache = (SOP_FeE_ConvertLine_1_0Cache*)cookparms.cache();

    const GEO_Detail* const inGeo0 = cookparms.inputGeo(0);

    outGeo0->replaceWithPoints(*inGeo0);
    // outGeo0->clearAndDestroy();

    //outGeo0 = sopNodeProcess(*inGeo0);

    const UT_StringHolder& primGroupName = sopparms.getPrimGroup();
    const UT_StringHolder& pointGroupName = sopparms.getPointGroup();
    const UT_StringHolder& vertexGroupName = sopparms.getVertexGroup();
    const UT_StringHolder& edgeGroupName = sopparms.getEdgeGroup();

    const GA_PointGroupUPtr groupDeleter = outGeo0->createDetachedPointGroup();
    //const GA_PointGroup group = outGeo0->newDetachedPointGroup();
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

    GOP_Manager gop;

    const bool isClosed = sopPrimPolyIsClosed(sopparms.getPrimType());

    const exint subscribeRatio = sopparms.getSubscribeRatio();
    const exint minGrainSize = sopparms.getMinGrainSize();
    //const exint minGrainSize = pow(2, 8);
    //const exint minGrainSize = pow(2, 4);

    //const GA_Storage inStorage = SYSisSame<T, fpreal32>() ? GA_STORE_REAL32 : GA_STORE_REAL64;
    const GA_Storage inStorage = GA_STORE_INT64;


    UT_AutoInterrupt boss("Processing");
    if (boss.wasInterrupted())
        return;

    GA_ATINumericUPtr  a = outGeo0->createDetachedTupleAttribute(GA_ATTRIB_PRIMITIVE, )
    
    GA_RWHandleT<UT_ValArray<exint>> attribHandle;
    GA_Attribute* attribPtr = outGeo0->addIntArray(GA_ATTRIB_POINT, pointPointEdgeAttribName, 1, 0, 0, inStorage);
    attribHandle.bind(attribPtr);
    GEO_FeE_Adjacency::pointPointEdge(outGeo0, attribHandle,
        static_cast<const GA_PointGroup*>(geo0Group), nullptr,
        subscribeRatio, minGrainSize);

    //notifyGroupParmListeners(cookparms.getNode(), 0, 1, outGeo0, geo0Group);


    outGeo0->appendPrimitiveBlock();


}



namespace SOP_FeE_ConvertLine_1_0_Namespace {

} // End SOP_FeE_ConvertLine_1_0_Namespace namespace
