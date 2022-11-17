
//#define UT_ASSERT_LEVEL 3
#include "SOP_FeE_Adjacency_1_0.h"


// This is an automatically generated header file based on theDsFile, below,
// to provide SOP_FeE_Adjacency_1_0Parms, an easy way to access parameter values from
// SOP_FeE_Adjacency_1_0Verb::cook with the correct type.
#include "SOP_FeE_Adjacency_1_0.proto.h"

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


#include <chrono>

#include <GEO_FeE/GEO_FeE_Adjacency.h>

using namespace SOP_FeE_Adjacency_1_0_Namespace;

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
const UT_StringHolder SOP_FeE_Adjacency_1_0::theSOPTypeName("FeE::adjacency::1.0"_sh);

/// newSopOperator is the hook that Houdini grabs from this dll
/// and invokes to register the SOP.  In this case, we add ourselves
/// to the specified operator table.
void
newSopOperator(OP_OperatorTable *table)
{
    table->addOperator(new OP_Operator(
        SOP_FeE_Adjacency_1_0::theSOPTypeName,   // Internal name
        "FeE Adjacency",                     // UI name
        SOP_FeE_Adjacency_1_0::myConstructor,    // How to build the SOP
        SOP_FeE_Adjacency_1_0::buildTemplates(), // My parameters
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
        name    "pointSeamGroup"
        cppname "PointSeamGroup"
        label   "Point Seam Group"
        type    string
        default { "" }
        parmtag { "script_action" "import soputils\nkwargs['geometrytype'] = (hou.geometryType.Points,)\nkwargs['inputindex'] = 0\nsoputils.selectGroupParm(kwargs)" }
        parmtag { "script_action_help" "Select geometry from an available viewport.\nShift-click to turn on Select Groups." }
        parmtag { "script_action_icon" "BUTTONS_reselect" }
        parmtag { "sop_input" "0" }
    }
    parm {
        name    "vertexEdgeSeamGroup"
        cppname "VertexEdgeSeamGroup"
        label   "Vertex Edge Seam Group"
        type    string
        default { "" }
        parmtag { "script_action" "import soputils\nkwargs['geometrytype'] = (hou.geometryType.Vertices,)\nkwargs['inputindex'] = 0\nsoputils.selectGroupParm(kwargs)" }
        parmtag { "script_action_help" "Select geometry from an available viewport.\nShift-click to turn on Select Groups." }
        parmtag { "script_action_icon" "BUTTONS_reselect" }
        parmtag { "sop_input" "0" }
    }
    parm {
        name    "edgeSeamGroup"
        cppname "EdgeSeamGroup"
        label   "Edge Seam Group"
        type    string
        default { "" }
        parmtag { "script_action" "import soputils\nkwargs['geometrytype'] = (hou.geometryType.Edges,)\nkwargs['inputindex'] = 0\nsoputils.selectGroupParm(kwargs)" }
        parmtag { "script_action_help" "Select geometry from an available viewport.\nShift-click to turn on Select Groups." }
        parmtag { "script_action_icon" "BUTTONS_reselect" }
        parmtag { "sop_input" "0" }
    }





    parm {
        name    "calVertexPrimIndex"
        cppname "CalVertexPrimIndex"
        label   "Calculate Vertex Prim Offset"
        type    toggle
        default { "0" }
        nolabel
        joinnext
    }
    parm {
        name    "vertexPrimIndexAttribName"
        cppname "VertexPrimIndexAttribName"
        label   "Vertex Prim Index Attribute Name"
        type    string
        default { "vertexPrimIndex" }
        disablewhen "{ calVertexPrimIndex == 0 }"
    }

    parm {
        name    "calVertexVertexPrim"
        cppname "CalVertexVertexPrim"
        label   "Calculate Vertex Vertex Prim"
        type    toggle
        default { "0" }
    }
    parm {
        name    "vertexVertexPrimPrevAttribName"
        cppname "VertexVertexPrimPrevAttribName"
        label   "Vertex Vertex Prim Prev Attribute Name"
        type    string
        default { "vertexPrev" }
        disablewhen "{ calVertexVertexPrim == 0 }"
    }
    parm {
        name    "vertexVertexPrimNextAttribName"
        cppname "VertexVertexPrimNextAttribName"
        label   "Vertex Vertex Prim Next Attribute Name"
        type    string
        default { "vertexNext" }
        disablewhen "{ calVertexVertexPrim == 0 }"
    }



    parm {
        name    "calVertexPointDst"
        cppname "CalVertexPointDst"
        label   "Calculate Vertex Point Destination"
        type    toggle
        default { "0" }
        nolabel
        joinnext
    }
    parm {
        name    "vertexPointDstAttribName"
        cppname "VertexPointDstAttribName"
        label   "Vertex Point Destination Attribute Name"
        type    string
        default { "vertexPointDst" }
        disablewhen "{ calVertexPointDst == 0 }"
    }

    parm {
        name    "calVertexVertexNextEquiv"
        cppname "CalVertexVertexNextEquiv"
        label   "Calculate Vertex Vertex Next Equiv"
        type    toggle
        default { "0" }
        nolabel
        joinnext
    }
    parm {
        name    "vertexVertexNextEquivAttribName"
        cppname "VertexVertexNextEquivAttribName"
        label   "Vertex Vertex Next Equiv Attribute Name"
        type    string
        default { "vertexVertexNextEquiv" }
        disablewhen "{ calVertexVertexNextEquiv == 0 }"
    }

    parm {
        name    "calPointPointEdge"
        cppname "CalPointPointEdge"
        label   "Calculate Point Point Edge"
        type    toggle
        default { "0" }
        nolabel
        joinnext
    }
    parm {
        name    "pointPointEdgeAttribName"
        cppname "PointPointEdgeAttribName"
        label   "Point Point Edge Attribute Name"
        type    string
        default { "pointPointEdge" }
        disablewhen "{ calPointPointEdge == 0 }"
    }

    parm {
        name    "calPointPointPrim"
        cppname "CalPointPointPrim"
        label   "Calculate Point Point Prim"
        type    toggle
        default { "0" }
        nolabel
        joinnext
    }
    parm {
        name    "pointPointPrimAttribName"
        cppname "PointPointPrimAttribName"
        label   "Point Point Prim Attribute Name"
        type    string
        default { "pointPointPrim" }
        disablewhen "{ calPointPointPrim == 0 }"
    }

    parm {
        name    "calPrimPrimEdge"
        cppname "CalPrimPrimEdge"
        label   "Calculate Prim Prim Edge"
        type    toggle
        default { "0" }
        nolabel
        joinnext
    }
    parm {
        name    "primPrimEdgeAttribName"
        cppname "PrimPrimEdgeAttribName"
        label   "Prim Prim Edge Attribute Name"
        type    string
        default { "primPrimEdge" }
        disablewhen "{ calPrimPrimEdge == 0 }"
    }

    parm {
        name    "calPrimPrimPoint"
        cppname "CalPrimPrimPoint"
        label   "Calculate Prim Prim Point"
        type    toggle
        default { "0" }
        nolabel
        joinnext
    }
    parm {
        name    "primPrimPointAttribName"
        cppname "PrimPrimPointAttribName"
        label   "Prim Prim Point Attribute Name"
        type    string
        default { "primPrimPoint" }
        disablewhen "{ calPrimPrimPoint == 0 }"
    }





    parm {
       name    "kernel"
       cppname "Kernel"
       label   "Kernel"
       type    integer
       default { 0 }
       range   { 0! 4 }
    }

    //parm {
    //    name    "outAsOffset"
    //    cppname "OutAsOffset"
    //    label   "Output as Offset"
    //    type    toggle
    //    default { "1" }
    //}

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
SOP_FeE_Adjacency_1_0::buildTemplates()
{
    static PRM_TemplateBuilder templ("SOP_FeE_Adjacency_1_0.C"_sh, theDsFile);
    return templ.templates();
}


//class SOP_FeE_Adjacency_1_0Cache : public SOP_NodeCache
//{
//public:
//    SOP_FeE_Adjacency_1_0Cache() : SOP_NodeCache(),
//        myPrevOutputDetailID(-1)
//    {}
//    ~SOP_FeE_Adjacency_1_0Cache() override {}
//
//    int64 myPrevOutputDetailID;
//};


class SOP_FeE_Adjacency_1_0Verb : public SOP_NodeVerb
{
public:
    SOP_FeE_Adjacency_1_0Verb() {}
    virtual ~SOP_FeE_Adjacency_1_0Verb() {}

    virtual SOP_NodeParms *allocParms() const { return new SOP_FeE_Adjacency_1_0Parms(); }
    //virtual SOP_NodeCache* allocCache() const { return new SOP_FeE_Adjacency_1_0Cache(); }
    virtual UT_StringHolder name() const { return SOP_FeE_Adjacency_1_0::theSOPTypeName; }

    virtual CookMode cookMode(const SOP_NodeParms *parms) const { return COOK_GENERIC; }

    virtual void cook(const CookParms &cookparms) const;

    /// This static data member automatically registers
    /// this verb class at library ldir0d time.
    static const SOP_NodeVerb::Register<SOP_FeE_Adjacency_1_0Verb> theVerb;
};

// The static member variable definition has to be outside the class definition.
// The declaration is inside the class.
const SOP_NodeVerb::Register<SOP_FeE_Adjacency_1_0Verb> SOP_FeE_Adjacency_1_0Verb::theVerb;

const SOP_NodeVerb *
SOP_FeE_Adjacency_1_0::cookVerb() const 
{ 
    return SOP_FeE_Adjacency_1_0Verb::theVerb.get();
}





static GA_GroupType
sopGroupType(SOP_FeE_Adjacency_1_0Parms::GroupType parmgrouptype)
{
    using namespace SOP_FeE_Adjacency_1_0Enums;
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
SOP_FeE_Adjacency_1_0Verb::cook(const SOP_NodeVerb::CookParms& cookparms) const
{
    auto&& sopparms = cookparms.parms<SOP_FeE_Adjacency_1_0Parms>();
    GU_Detail* outGeo0 = cookparms.gdh().gdpNC();
    //auto sopcache = (SOP_FeE_Adjacency_1_0Cache*)cookparms.cache();

    const GEO_Detail* const inGeo0 = cookparms.inputGeo(0);

    outGeo0->replaceWith(*inGeo0);
    // outGeo0->clearAndDestroy();

    //outGeo0 = sopNodeProcess(*inGeo0);

    bool calVertexPrimIndex = sopparms.getCalVertexPrimIndex();
    bool calVertexPointDst = sopparms.getCalVertexPointDst();
    bool calVertexVertexNextEquiv = sopparms.getCalVertexVertexNextEquiv();
    bool calPointPointEdge = sopparms.getCalPointPointEdge();
    bool calPointPointPrim = sopparms.getCalPointPointPrim();
    bool calPrimPrimEdge = sopparms.getCalPrimPrimEdge();
    bool calPrimPrimPoint = sopparms.getCalPrimPrimPoint();
    bool calVertexVertexPrim = sopparms.getCalVertexVertexPrim();

#if 1
    const UT_StringHolder& vertexPrimIndexAttribName = sopparms.getVertexPrimIndexAttribName();
    const UT_StringHolder& vertexPointDstAttribName = sopparms.getVertexPointDstAttribName();
    const UT_StringHolder& vertexVertexNextEquivAttribName = sopparms.getVertexVertexNextEquivAttribName();
    const UT_StringHolder& pointPointEdgeAttribName = sopparms.getPointPointEdgeAttribName();
    const UT_StringHolder& pointPointPrimAttribName = sopparms.getPointPointPrimAttribName();
    const UT_StringHolder& primPrimEdgeAttribName = sopparms.getPrimPrimEdgeAttribName();
    const UT_StringHolder& primPrimPointAttribName = sopparms.getPrimPrimPointAttribName();
    const UT_StringHolder& vertexVertexPrimPrevAttribName = sopparms.getVertexVertexPrimPrevAttribName();
    const UT_StringHolder& vertexVertexPrimNextAttribName = sopparms.getVertexVertexPrimNextAttribName();

    calVertexPrimIndex = calVertexPrimIndex && vertexPrimIndexAttribName.isstring();
    calVertexPointDst = calVertexPointDst && vertexVertexNextEquivAttribName.isstring();
    calVertexVertexNextEquiv = calVertexVertexNextEquiv && vertexPointDstAttribName.isstring();
    calPointPointEdge = calPointPointEdge && pointPointEdgeAttribName.isstring();
    calPointPointPrim = calPointPointPrim && pointPointPrimAttribName.isstring();
    calPrimPrimEdge = calPrimPrimEdge && primPrimEdgeAttribName.isstring();
    calPrimPrimPoint = calPrimPrimPoint && primPrimPointAttribName.isstring();
    calVertexVertexPrim = calVertexVertexPrim && (vertexVertexPrimPrevAttribName.isstring() || vertexVertexPrimNextAttribName.isstring());
#else
    UT_StringHolder& pointPointEdgeAttribName;
    UT_StringHolder& pointPointPrimAttribName;
    UT_StringHolder& primPrimEdgeAttribName;
    UT_StringHolder& primPrimPointAttribName;
    UT_StringHolder& vertexVertexNextEquivAttribName;
    if (calPointPointEdge)
    {
        pointPointEdgeAttribName = sopparms.getPointPointEdgeAttribName();
        calPointPointEdge = pointPointEdgeAttribName.isstring();
    }
    if (calPointPointPrim)
    {
        pointPointPrimAttribName = sopparms.getPointPointPrimAttribName();
        calPointPointPrim = pointPointPrimAttribName.isstring();
    }
    if (calPrimPrimEdge)
    {
        primPrimEdgeAttribName = sopparms.getPrimPrimEdgeAttribName();
        calPrimPrimEdge = primPrimEdgeAttribName.isstring();
    }
    if (calPrimPrimPoint)
    {
        primPrimPointAttribName = sopparms.getPrimPrimPointAttribName();
        calPrimPrimPoint = primPrimPointAttribName.isstring();
    }
    if (calVertexVertexNextEquiv)
    {
        calVertexVertexNextEquiv = sopparms.getVertexVertexNextEquivAttribName();
        calVertexVertexNextEquiv = calVertexVertexNextEquiv.isstring();
    }
#endif

    if (
        !calVertexPrimIndex &&
        !calVertexPointDst &&
        !calVertexVertexNextEquiv &&
        !calPointPointEdge &&
        !calPointPointPrim &&
        !calPrimPrimEdge &&
        !calPrimPrimPoint &&
        !calVertexVertexPrim
        )
        return;




    const UT_StringHolder& pointSeamGroupName = sopparms.getPointSeamGroup();
    const bool usepointSeamGroup = pointSeamGroupName.isstring() && (calPointPointEdge || calPointPointPrim || calPrimPrimPoint);

    const UT_StringHolder& vertexEdgeSeamGroupName = sopparms.getVertexEdgeSeamGroup();
    const bool useVertexEdgeSeamGroup = vertexEdgeSeamGroupName.isstring() && calPrimPrimEdge;

    const UT_StringHolder& edgeSeamGroupName = sopparms.getEdgeSeamGroup();
    const bool useEdgeSeamGroup = edgeSeamGroupName.isstring() && calPrimPrimEdge;



    const exint& kernel = sopparms.getKernel();
    //const bool& outAsOffset = sopparms.getOutAsOffset();
    const exint& subscribeRatio = sopparms.getSubscribeRatio();
    const exint& minGrainSize = sopparms.getMinGrainSize();
    //const exint minGrainSize = pow(2, 8);
    //const exint minGrainSize = pow(2, 4);

    //const GA_Storage inStorageF = SYSisSame<T, fpreal32>() ? GA_STORE_REAL32 : GA_STORE_REAL64;
    const GA_Storage inStorageI = GA_STORE_INT32;


    UT_AutoInterrupt boss("Processing");
    if (boss.wasInterrupted())
        return;







    const GA_ElementGroup* geo0Group = nullptr;
    const UT_StringHolder& groupName0 = sopparms.getGroup();

    if (groupName0.isstring())
    {
        GOP_Manager gop;
        GA_GroupType groupType = sopGroupType(sopparms.getGroupType());

        bool ok = true;
        const GA_Group* anyGroup = gop.parseGroupDetached(groupName0, groupType, outGeo0, true, false, ok);

        if (!ok || (anyGroup && !anyGroup->isElementGroup()))
        {
            cookparms.sopAddWarning(SOP_ERR_BADGROUP, groupName0);
        }
        if (anyGroup && anyGroup->isElementGroup())
        {
            geo0Group = UTverify_cast<const GA_ElementGroup*>(anyGroup);
        }
    }
    //notifyGroupParmListeners(cookparms.getNode(), 0, 1, outGeo0, geo0Group);

    /*
    if (geo0Group && geo0Group->isEmpty())
        return;

    GA_GroupType geo0finalGroupType;
    if (geo0Group)
        geo0finalGroupType = geo0Group->classType();
    else
        geo0finalGroupType = GA_GROUP_INVALID;

    */


    const GA_PointGroup* pointSeamGroup = nullptr;
    const GA_VertexGroup* vertexEdgeSeamGroup = nullptr;
    //const GA_ElementGroup* edgeSeamGroup = nullptr;

    if (vertexEdgeSeamGroupName.isstring())
    {
        bool ok = true;
        const GA_Group* anyGroup = gop.parseGroupDetached(vertexEdgeSeamGroupName, GA_GROUP_VERTEX, outGeo0, true, false, ok);

        if (!ok || (anyGroup && !anyGroup->isElementGroup()))
        {
            cookparms.sopAddWarning(SOP_ERR_BADGROUP, vertexEdgeSeamGroupName);
        }
        if (anyGroup && anyGroup->isElementGroup())
        {
            vertexEdgeSeamGroup = UTverify_cast<const GA_VertexGroup*>(anyGroup);
        }
    }
    if (edgeSeamGroupName.isstring())
    {
        bool ok = true;
        const GA_Group* anyGroup = gop.parseGroupDetached(edgeSeamGroupName, GA_GROUP_EDGE, outGeo0, true, false, ok);

        if (!ok || (anyGroup && !anyGroup->isElementGroup()))
        {
            cookparms.sopAddWarning(SOP_ERR_BADGROUP, edgeSeamGroupName);
        }
        if (anyGroup && anyGroup->isElementGroup())
        {
            if (vertexEdgeSeamGroup)
            {
                GA_VertexGroupUPtr groupDeleter;
                if (vertexEdgeSeamGroup)
                {
                    GA_VertexGroup* newDetachedGroup = new GA_VertexGroup(*outGeo0);
                    groupDeleter.reset(newDetachedGroup);
                    newDetachedGroup->combine(vertexEdgeSeamGroup);
                    vertexEdgeSeamGroup = newDetachedGroup;
                }
            }
            else
            {
                vertexEdgeSeamGroup = UTverify_cast<const GA_VertexGroup*>(anyGroup);
            }
        }
    }

    GA_ATINumericUPtr vtxpnumAttribUPtr;
    GA_RWHandleT<GA_Size> vtxpnumAttribHandle;
    if (calVertexPrimIndex)
    {
        GA_Attribute* attribPtr = outGeo0->addIntTuple(GA_ATTRIB_VERTEX, vertexPrimIndexAttribName, 1, GA_Defaults(-1), 0, 0, inStorageI);
        vtxpnumAttribHandle.bind(attribPtr);
    }
    else if (calVertexPointDst || calVertexVertexNextEquiv || calPointPointEdge || calPrimPrimEdge)
    {
        vtxpnumAttribUPtr = outGeo0->createDetachedTupleAttribute(GA_ATTRIB_VERTEX, inStorageI, 1, GA_Defaults(-1));
        //GA_ATINumeric* vtxpnumATI = vtxpnumAttribUPtr.get();
        vtxpnumAttribHandle.bind(vtxpnumAttribUPtr.get());
    }
    if (calVertexPrimIndex || calVertexPointDst || calVertexVertexNextEquiv || calPointPointEdge || calPrimPrimEdge)
    {
        GEO_FeE_Adjacency::vertexPrimIndex(outGeo0, vtxpnumAttribHandle,
            static_cast<const GA_VertexGroup*>(geo0Group),
            subscribeRatio, minGrainSize);
        //GEO_FeE_Adjacency::vertexPrimIndex(outGeo0, vtxpnumAttribHandle, static_cast<const GA_VertexGroup*>(geo0Group), subscribeRatio, minGrainSize);
    }



    GA_ATINumericUPtr dstptAttribUPtr;
    GA_RWHandleT<GA_Offset> dstptAttribHandle;
    if (calVertexPointDst)
    {
        GA_Attribute* attribPtr = outGeo0->addIntTuple(GA_ATTRIB_VERTEX, vertexPointDstAttribName, 1, GA_Defaults(-1), 0, 0, inStorageI);
        dstptAttribHandle.bind(attribPtr);
    }
    else if (calVertexVertexNextEquiv || calPrimPrimEdge)
    {
        dstptAttribUPtr = outGeo0->createDetachedTupleAttribute(GA_ATTRIB_VERTEX, inStorageI, 1, GA_Defaults(-1));
        //GA_ATINumeric* dstptATI = dstptAttribUPtr.get();
        dstptAttribHandle.bind(dstptAttribUPtr.get());
    }
    if (calVertexPointDst || calVertexVertexNextEquiv || calPrimPrimEdge)
    {
        GEO_FeE_Adjacency::vertexPointDst(outGeo0, dstptAttribHandle, vtxpnumAttribHandle,
            static_cast<const GA_VertexGroup*>(geo0Group),
            subscribeRatio, minGrainSize);
    }



    GA_RWHandleT<GA_Offset> intAttribHandle;
    GA_RWHandleT<UT_ValArray<GA_Offset>> attribHandle;


    if (calVertexVertexPrim)
    {
        GA_Attribute* attribPtr = outGeo0->addIntTuple(GA_ATTRIB_VERTEX, vertexVertexPrimPrevAttribName, 1, GA_Defaults(-1), 0, 0, inStorageI);
        intAttribHandle.bind(attribPtr);

        attribPtr = outGeo0->addIntTuple(GA_ATTRIB_VERTEX, vertexVertexPrimNextAttribName, 1, GA_Defaults(-1), 0, 0, inStorageI);
        GA_RWHandleT<GA_Offset> intAttribHandle1;
        intAttribHandle1.bind(attribPtr);

        switch (kernel)
        {
        case 0:
            GEO_FeE_Adjacency::vertexVertexPrim(outGeo0, intAttribHandle, intAttribHandle1,
                static_cast<const GA_VertexGroup*>(geo0Group),
                subscribeRatio, minGrainSize);
            break;
        case 1:
            GEO_FeE_Adjacency::vertexVertexPrim1(outGeo0, intAttribHandle, intAttribHandle1,
                static_cast<const GA_VertexGroup*>(geo0Group),
                subscribeRatio, minGrainSize);
            break;
        default:
            break;
        }
    }


    if (calVertexVertexNextEquiv)
    {
        GA_Attribute* attribPtr = outGeo0->addIntTuple(GA_ATTRIB_VERTEX, vertexVertexNextEquivAttribName, 1, GA_Defaults(-1), 0, 0, inStorageI);
        intAttribHandle.bind(attribPtr);
        GEO_FeE_Adjacency::vertexVertexNextEquiv(outGeo0, intAttribHandle, dstptAttribHandle,
            static_cast<const GA_VertexGroup*>(geo0Group),
            subscribeRatio, minGrainSize);
    }

    if (calPointPointEdge)
    {
        //GA_Attribute* attribPtr = outGeo0->addIntTuple(GA_ATTRIB_POINT, pointPointEdgeAttribName, 1, GA_Defaults(-1), 0, 0, inStorageI);
        GA_Attribute* attribPtr = outGeo0->addIntArray(GA_ATTRIB_POINT, pointPointEdgeAttribName, 1, 0, 0, inStorageI);
        attribHandle.bind(attribPtr);
        switch (kernel)
        {
        case 0:
            GEO_FeE_Adjacency::pointPointEdge(outGeo0, attribHandle, vtxpnumAttribHandle,
                static_cast<const GA_PointGroup*>(geo0Group), nullptr,
                subscribeRatio, minGrainSize);
            break;
        case 1:
            GEO_FeE_Adjacency::pointPointEdge(outGeo0, attribHandle,
                static_cast<const GA_PointGroup*>(geo0Group), nullptr,
                subscribeRatio, minGrainSize);
            break;
        default:
            break;
        }
    }

    if (calPointPointPrim)
    {
        GA_Attribute* attribPtr = outGeo0->addIntArray(GA_ATTRIB_POINT, pointPointPrimAttribName, 1, 0, 0, inStorageI);
        //GA_Attribute* attribPtr = outGeo0->addIntTuple(GA_ATTRIB_POINT, pointPointPrimAttribName, 1, GA_Defaults(-1), 0, 0, inStorageI);
        attribHandle.bind(attribPtr);
        //GEO_FeE_Adjacency::pointPointPrim(outGeo0, attribHandle,
        //    static_cast<const GA_PointGroup*>(geo0Group), nullptr,
        //    subscribeRatio, minGrainSize);
    }

    if (calPrimPrimEdge)
    {
        GA_Attribute* attribPtr = outGeo0->addIntArray(GA_ATTRIB_PRIMITIVE, primPrimEdgeAttribName, 1, 0, 0, inStorageI);
        //GA_Attribute* attribPtr = outGeo0->addIntTuple(GA_ATTRIB_PRIMITIVE, primPrimEdgeAttribName, 1, GA_Defaults(-1), 0, 0, inStorageI);
        attribHandle.bind(attribPtr);
        switch (kernel)
        {
        case 0:
            GEO_FeE_Adjacency::primPrimEdge(outGeo0, attribHandle, dstptAttribHandle,
                static_cast<const GA_PrimitiveGroup*>(geo0Group), vertexEdgeSeamGroup,
                subscribeRatio, minGrainSize);
            break;
        case 1:
            GEO_FeE_Adjacency::primPrimEdge1(outGeo0, attribHandle, dstptAttribHandle,
                static_cast<const GA_PrimitiveGroup*>(geo0Group), vertexEdgeSeamGroup,
                subscribeRatio, minGrainSize);
            break;
        case 2:
            GEO_FeE_Adjacency::primPrimEdge2(outGeo0, attribHandle, dstptAttribHandle,
                static_cast<const GA_PrimitiveGroup*>(geo0Group), vertexEdgeSeamGroup,
                subscribeRatio, minGrainSize);
            break;
        case 3:
            GEO_FeE_Adjacency::primPrimEdge3(outGeo0, attribHandle,
                static_cast<const GA_PrimitiveGroup*>(geo0Group), vertexEdgeSeamGroup,
                subscribeRatio, minGrainSize);
            break;
        default:
            break;
        }
    }

    if (calPrimPrimPoint)
    {
        GA_Attribute* attribPtr = outGeo0->addIntArray(GA_ATTRIB_PRIMITIVE, primPrimPointAttribName, 1, 0, 0, inStorageI);
        //GA_Attribute* attribPtr = outGeo0->addIntTuple(GA_ATTRIB_PRIMITIVE, primPrimPointAttribName, 1, GA_Defaults(-1), 0, 0, inStorageI);
        attribHandle.bind(attribPtr);
        GEO_FeE_Adjacency::primPrimPoint(outGeo0, attribHandle,
            static_cast<const GA_PrimitiveGroup*>(geo0Group), pointSeamGroup,
            subscribeRatio, minGrainSize);
    }


}



namespace SOP_FeE_Adjacency_1_0_Namespace {

} // End SOP_FeE_Adjacency_1_0_Namespace namespace
