
//#define UT_ASSERT_LEVEL 3
#include "SOP_FeE_Adjacency_1_0.h"


// This is an automatically generated header file based on theDsFile, below,
// to provide SOP_FeE_Adjacency_1_0Parms, an easy way to access parameter values from
// SOP_FeE_Adjacency_1_0Verb::cook with the correct type.
#include "SOP_FeE_Adjacency_1_0.proto.h"

#include "GA/GA_Detail.h"
#include "PRM/PRM_TemplateBuilder.h"
#include "UT/UT_Interrupt.h"
#include "UT/UT_DSOVersion.h"


#include "GFE/GFE_TopologyReference.h"
#include "GFE/GFE_Adjacency.h"
#include "GFE/GFE_GroupParser.h"
#include "GFE/GFE_GroupUnion.h"



using namespace SOP_FeE_Adjacency_1_0_Namespace;


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
        default { "__topo_vtxpnum" }
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
        default { "__topo_vtxPrimPrev" }
        disablewhen "{ calVertexVertexPrim == 0 }"
    }
    parm {
        name    "vertexVertexPrimNextAttribName"
        cppname "VertexVertexPrimNextAttribName"
        label   "Vertex Vertex Prim Next Attribute Name"
        type    string
        default { "__topo_vtxPrimNext" }
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
        default { "__topo_dstpt" }
        disablewhen "{ calVertexPointDst == 0 }"
    }

    parm {
        name    "calVertexNextEquiv"
        cppname "CalVertexNextEquiv"
        label   "Calculate Vertex Next Equiv"
        type    toggle
        default { "0" }
        nolabel
        joinnext
    }
    parm {
        name    "vertexNextEquivAttribName"
        cppname "VertexNextEquivAttribName"
        label   "Vertex Next Equiv Attribute Name"
        type    string
        default { "__topo_nextEquiv" }
        disablewhen "{ calVertexNextEquiv == 0 }"
    }

    parm {
        name    "calVertexNextEquivNoLoop"
        cppname "CalVertexNextEquivNoLoop"
        label   "Calculate Vertex Next Equiv No Loop"
        type    toggle
        default { "0" }
        nolabel
        joinnext
    }
    parm {
        name    "vertexNextEquivNoLoopAttribName"
        cppname "VertexNextEquivNoLoopAttribName"
        label   "Vertex Next Equiv No Loop Attribute Name"
        type    string
        default { "__topo_nextEquivNoLoop" }
        disablewhen "{ calVertexNextEquivNoLoop == 0 }"
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
        default { "__topo_nebs" }
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
        default { "__topo_nebsPrim" }
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
        default { "__topo_nebs" }
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
        default { "__topo_nebsPoint" }
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
SOP_FeE_Adjacency_1_0::buildTemplates()
{
    static PRM_TemplateBuilder templ("SOP_FeE_Adjacency_1_0.C"_sh, theDsFile);
    if (templ.justBuilt())
    {
        templ.setChoiceListPtr("group"_sh, &SOP_Node::allGroupMenu);
    }
    return templ.templates();
}

const UT_StringHolder SOP_FeE_Adjacency_1_0::theSOPTypeName("FeE::adjacency::1.0"_sh);

void
newSopOperator(OP_OperatorTable* table)
{
    OP_Operator* newOp = new OP_Operator(
        SOP_FeE_Adjacency_1_0::theSOPTypeName,
        "FeE Adjacency",
        SOP_FeE_Adjacency_1_0::myConstructor,
        SOP_FeE_Adjacency_1_0::buildTemplates(),
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
    GA_Detail* const outGeo0 = cookparms.gdh().gdpNC();
    //auto sopcache = (SOP_FeE_Adjacency_1_0Cache*)cookparms.cache();

    const GA_Detail* const inGeo0 = cookparms.inputGeo(0);

    outGeo0->replaceWith(*inGeo0);
    // outGeo0->clearAndDestroy();

    //outGeo0 = sopNodeProcess(*inGeo0);

    bool calVertexPrimIndex       = sopparms.getCalVertexPrimIndex();
    bool calVertexVertexPrim      = sopparms.getCalVertexVertexPrim();
    bool calVertexPointDst        = sopparms.getCalVertexPointDst();
    bool calVertexNextEquiv       = sopparms.getCalVertexNextEquiv();
    bool calVertexNextEquivNoLoop = sopparms.getCalVertexNextEquivNoLoop();
    bool calPointPointEdge        = sopparms.getCalPointPointEdge();
    bool calPointPointPrim        = sopparms.getCalPointPointPrim();
    bool calPrimPrimEdge          = sopparms.getCalPrimPrimEdge();
    bool calPrimPrimPoint         = sopparms.getCalPrimPrimPoint();

#if 1
    const UT_StringHolder& vertexPrimIndexAttribName             = sopparms.getVertexPrimIndexAttribName();
    const UT_StringHolder& vertexVertexPrimNextAttribName        = sopparms.getVertexVertexPrimNextAttribName();
    const UT_StringHolder& vertexPointDstAttribName              = sopparms.getVertexPointDstAttribName();
    const UT_StringHolder& vertexNextEquivAttribName             = sopparms.getVertexNextEquivAttribName();
    const UT_StringHolder& vertexNextEquivNoLoopAttribName       = sopparms.getVertexNextEquivNoLoopAttribName();
    const UT_StringHolder& pointPointEdgeAttribName              = sopparms.getPointPointEdgeAttribName();
    const UT_StringHolder& pointPointPrimAttribName              = sopparms.getPointPointPrimAttribName();
    const UT_StringHolder& primPrimEdgeAttribName                = sopparms.getPrimPrimEdgeAttribName();
    const UT_StringHolder& primPrimPointAttribName               = sopparms.getPrimPrimPointAttribName();
    const UT_StringHolder& vertexVertexPrimPrevAttribName        = sopparms.getVertexVertexPrimPrevAttribName();

    calVertexPrimIndex         = calVertexPrimIndex       && vertexPrimIndexAttribName.isstring()        && vertexPrimIndexAttribName.length() != 0;
    calVertexVertexPrim        = calVertexVertexPrim      && ((vertexVertexPrimPrevAttribName.isstring() && vertexVertexPrimPrevAttribName.length() != 0) || (vertexVertexPrimNextAttribName.isstring() && vertexVertexPrimNextAttribName.length() != 0));
    calVertexPointDst          = calVertexPointDst        && vertexPointDstAttribName.isstring()         && vertexPointDstAttribName.length() != 0;
    calVertexNextEquiv         = calVertexNextEquiv       && vertexNextEquivAttribName.isstring()        && vertexNextEquivAttribName.length() != 0;
    calVertexNextEquivNoLoop   = calVertexNextEquivNoLoop && vertexNextEquivNoLoopAttribName.isstring()  && vertexNextEquivNoLoopAttribName.length() != 0;
    calPointPointEdge          = calPointPointEdge        && pointPointEdgeAttribName.isstring()         && pointPointEdgeAttribName.length() != 0;
    calPointPointPrim          = calPointPointPrim        && pointPointPrimAttribName.isstring()         && pointPointPrimAttribName.length() != 0;
    calPrimPrimEdge            = calPrimPrimEdge          && primPrimEdgeAttribName.isstring()           && primPrimEdgeAttribName.length() != 0;
    calPrimPrimPoint           = calPrimPrimPoint         && primPrimPointAttribName.isstring()          && primPrimPointAttribName.length() != 0;
#else
    UT_StringHolder& pointPointEdgeAttribName;
    UT_StringHolder& pointPointPrimAttribName;
    UT_StringHolder& primPrimEdgeAttribName;
    UT_StringHolder& primPrimPointAttribName;
    UT_StringHolder& vertexNextEquivAttribName;
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
    if (calVertexNextEquiv)
    {
        calVertexNextEquiv = sopparms.getVertexNextEquivAttribName();
        calVertexNextEquiv = calVertexNextEquiv.isstring();
    }
#endif
    if (
        !calVertexPrimIndex &&
        !calVertexVertexPrim &&
        !calVertexPointDst &&
        !calVertexNextEquiv &&
        !calVertexNextEquivNoLoop &&
        !calPointPointEdge &&
        !calPointPointPrim &&
        !calPrimPrimEdge &&
        !calPrimPrimPoint
        )
        return;




    const UT_StringHolder& pointSeamGroupName = sopparms.getPointSeamGroup();
    //const bool& usepointSeamGroup = pointSeamGroupName.isstring() && (calPointPointEdge || calPointPointPrim || calPrimPrimPoint);

    const UT_StringHolder& vertexEdgeSeamGroupName = sopparms.getVertexEdgeSeamGroup();
    //const bool& useVertexEdgeSeamGroup = vertexEdgeSeamGroupName.isstring() && calPrimPrimEdge;

    const UT_StringHolder& edgeSeamGroupName = sopparms.getEdgeSeamGroup();
    //const bool& useEdgeSeamGroup = edgeSeamGroupName.isstring() && calPrimPrimEdge;



    const exint kernel = sopparms.getKernel();
    //const bool& outAsOffset = sopparms.getOutAsOffset();
    const exint subscribeRatio = sopparms.getSubscribeRatio();
    const exint minGrainSize = sopparms.getMinGrainSize();

    const GA_Storage inStorageI = GFE_Type::getPreferredStorageI(outGeo0);


    UT_AutoInterrupt boss("Processing");
    if (boss.wasInterrupted())
        return;





    const GA_GroupType groupType = sopGroupType(sopparms.getGroupType());

    GOP_Manager gop;
    const GA_Group* geo0Group = GFE_GroupParser_Namespace::findOrParseGroupDetached(cookparms, outGeo0, groupType, sopparms.getGroup(), gop);
    //notifyGroupParmListeners(cookparms.getNode(), 0, 1, outGeo0, geo0Group);
    if (geo0Group && geo0Group->isEmpty())
        return;

    //const GA_Range geo0Range = GFE_Group::groupPromoteRange(outGeo0, geo0Group, geo0AttribClass);
    //const GA_SplittableRange geo0SplittableRange(GFE_Group::groupPromoteRange(outGeo0, geo0Group, geo0AttribClass));
    //const GA_SplittableRange geo0SplittableRange = GFE_Group::groupPromoteSplittableRange(outGeo0, geo0Group, geo0AttribClass);


    GA_VertexGroup* const vertexEdgeSeamGroup = const_cast<GA_VertexGroup*>(GFE_GroupParser_Namespace::findOrParseVertexGroupDetached(cookparms, outGeo0, vertexEdgeSeamGroupName, gop));

    const GA_PointGroup* const pointSeamGroup = GFE_GroupParser_Namespace::findOrParsePointGroupDetached(cookparms, outGeo0, pointSeamGroupName, gop);
    //const GA_ElementGroup* edgeSeamGroup = nullptr;

    const GA_EdgeGroup* const edgeSeamGroup = GFE_GroupParser_Namespace::findOrParseEdgeGroupDetached(cookparms, outGeo0, edgeSeamGroupName, gop);
    //GFE_Group::combineGroup<GA_VertexGroup, GA_EdgeGroup>(outGeo0, vertexEdgeSeamGroup, edgeSeamGroup);
    //GFE_Group::combineVertexFromEdgeGroup(outGeo0, vertexEdgeSeamGroup, edgeSeamGroup);
    if(vertexEdgeSeamGroup)
        GFE_GroupUnion::groupUnion(vertexEdgeSeamGroup, edgeSeamGroup);



    GA_ATINumericUPtr vtxpnumAttribUPtr;
    GA_RWHandleT<GA_Size> vtxpnumAttribHandle;
    if (calVertexPrimIndex)
    {
        GA_Attribute* attribPtr = outGeo0->getAttributes().createTupleAttribute(GA_ATTRIB_VERTEX, vertexPrimIndexAttribName, inStorageI, 1, GA_Defaults(-1));
        vtxpnumAttribHandle.bind(attribPtr);
    }
    else if (calVertexPointDst || calVertexNextEquiv || calVertexNextEquivNoLoop || calPointPointEdge || calPrimPrimEdge)
    {
        vtxpnumAttribUPtr = outGeo0->createDetachedTupleAttribute(GA_ATTRIB_VERTEX, inStorageI, 1, GA_Defaults(-1));
        //GA_ATINumeric* vtxpnumATI = vtxpnumAttribUPtr.get();
        vtxpnumAttribHandle.bind(vtxpnumAttribUPtr.get());
    }
    if (calVertexPrimIndex || calVertexPointDst || calVertexNextEquiv || calVertexNextEquivNoLoop || calPointPointEdge || calPrimPrimEdge)
    {
        GFE_TopologyReference::vertexPrimIndex(outGeo0, vtxpnumAttribHandle,
            static_cast<const GA_VertexGroup*>(geo0Group),
            subscribeRatio, minGrainSize);
        //GFE_TopologyReference::vertexPrimIndex(outGeo0, vtxpnumAttribHandle, static_cast<const GA_VertexGroup*>(geo0Group), subscribeRatio, minGrainSize);
        vtxpnumAttribHandle.bumpDataId();
    }



    GA_ATINumericUPtr dstptAttribUPtr;
    GA_RWHandleT<GA_Offset> dstptAttribHandle;
    if (calVertexPointDst)
    {
        GA_Attribute* attribPtr = outGeo0->getAttributes().createTupleAttribute(GA_ATTRIB_VERTEX, vertexPointDstAttribName, inStorageI, 1, GA_Defaults(-1));
        dstptAttribHandle.bind(attribPtr);
    }
    else if (calVertexNextEquiv || calVertexNextEquivNoLoop || calPrimPrimEdge)
    {
        dstptAttribUPtr = outGeo0->createDetachedTupleAttribute(GA_ATTRIB_VERTEX, inStorageI, 1, GA_Defaults(-1));
        //GA_ATINumeric* dstptATI = dstptAttribUPtr.get();
        dstptAttribHandle.bind(dstptAttribUPtr.get());
    }
    if (calVertexPointDst || calVertexNextEquiv || calVertexNextEquivNoLoop || calPrimPrimEdge)
    {
        switch ((calVertexNextEquiv || calVertexNextEquivNoLoop || calPrimPrimEdge) ? 0 : kernel)
        {
        case 0:
            GFE_TopologyReference::vertexPointDstByVtxpnum(outGeo0, dstptAttribHandle, vtxpnumAttribHandle,
                static_cast<const GA_VertexGroup*>(geo0Group),
                subscribeRatio, minGrainSize);
            break;
        case 1:
            GFE_TopologyReference::vertexPointDst(outGeo0, dstptAttribHandle.getAttribute(), vtxpnumAttribHandle.getAttribute(),
                static_cast<const GA_VertexGroup*>(geo0Group),
                subscribeRatio, minGrainSize);
            break;
        default:
            break;
        }
        dstptAttribHandle.bumpDataId();
    }



    GA_ATINumericUPtr vtxPrevAttribUPtr, vtxNextAttribUPtr;
    GA_RWHandleT<GA_Offset> vtxPrevAttribHandle, vtxNextAttribHandle;
    if (calVertexVertexPrim)
    {
        vtxPrevAttribHandle = outGeo0->getAttributes().createTupleAttribute(GA_ATTRIB_VERTEX, vertexVertexPrimPrevAttribName, inStorageI, 1, GA_Defaults(-1));
        vtxNextAttribHandle = outGeo0->getAttributes().createTupleAttribute(GA_ATTRIB_VERTEX, vertexVertexPrimNextAttribName, inStorageI, 1, GA_Defaults(-1));
    }
    else if (calPointPointEdge)
    {
        vtxPrevAttribUPtr = outGeo0->createDetachedTupleAttribute(GA_ATTRIB_VERTEX, inStorageI, 1, GA_Defaults(-1));
        vtxNextAttribUPtr = outGeo0->createDetachedTupleAttribute(GA_ATTRIB_VERTEX, inStorageI, 1, GA_Defaults(-1));
        //GA_ATINumeric* dstptATI = dstptAttribUPtr.get();
        vtxPrevAttribHandle = vtxPrevAttribUPtr.get();
        vtxNextAttribHandle = vtxNextAttribUPtr.get();
    }
    if (calVertexVertexPrim || calPointPointEdge)
    {
        switch (calPointPointEdge ? 0 : kernel)
        {
        case 0:
            GFE_TopologyReference::vertexVertexPrim(outGeo0, vtxPrevAttribHandle, vtxNextAttribHandle,
                static_cast<const GA_VertexGroup*>(geo0Group),
                subscribeRatio, minGrainSize);
            break;
        case 1:
            GFE_TopologyReference::vertexVertexPrim1(outGeo0, vtxPrevAttribHandle, vtxNextAttribHandle,
                static_cast<const GA_VertexGroup*>(geo0Group),
                subscribeRatio, minGrainSize);
            break;
        default:
            break;
        }
        vtxPrevAttribHandle.bumpDataId();
        vtxNextAttribHandle.bumpDataId();
    }




    GA_ATINumericUPtr vtxNextEquivAttribUPtr;
    GA_RWHandleT<GA_Offset> vtxNextEquivAttribHandle;
    if (calVertexNextEquiv)
    {
        vtxNextEquivAttribHandle = outGeo0->getAttributes().createTupleAttribute(GA_ATTRIB_VERTEX, vertexNextEquivAttribName, inStorageI, 1, GA_Defaults(-1));
    }
    else if (calPrimPrimEdge)
    {
        vtxNextEquivAttribUPtr = outGeo0->createDetachedTupleAttribute(GA_ATTRIB_VERTEX, inStorageI, 1, GA_Defaults(-1));
        vtxNextEquivAttribHandle = vtxNextEquivAttribUPtr.get();
    }
    if (calVertexNextEquiv || calPrimPrimEdge)
    {
        GFE_VertexNextEquiv::vertexNextEquiv(outGeo0, vtxNextEquivAttribHandle, dstptAttribHandle,
            static_cast<const GA_VertexGroup*>(geo0Group),
            subscribeRatio, minGrainSize);
        vtxNextEquivAttribHandle.bumpDataId();
    }




    GA_RWHandleT<GA_Offset> intAttribHandle;
    GA_RWHandleT<UT_ValArray<GA_Offset>> attribHandle;

    if (calVertexNextEquivNoLoop)
    {
        GA_Attribute* attribPtr = outGeo0->getAttributes().createTupleAttribute(GA_ATTRIB_VERTEX, vertexNextEquivNoLoopAttribName, inStorageI, 1, GA_Defaults(-1));
        intAttribHandle.bind(attribPtr);
        GFE_VertexNextEquiv::vertexNextEquivNoLoop(outGeo0, intAttribHandle, dstptAttribHandle,
            static_cast<const GA_VertexGroup*>(geo0Group),
            subscribeRatio, minGrainSize);
        attribPtr->bumpDataId();
    }

    if (calPointPointEdge)
    {
        //GA_Attribute* attribPtr = outGeo0->getAttributes().createTupleAttribute(GA_ATTRIB_POINT, pointPointEdgeAttribName, 1, GA_Defaults(-1));
        GA_Attribute* attribPtr = outGeo0->getAttributes().createArrayAttribute(GA_ATTRIB_POINT, GA_SCOPE_PUBLIC, pointPointEdgeAttribName, inStorageI, 1);
        attribHandle.bind(attribPtr);
        switch (kernel)
        {
        case 0:
            GFE_Adjacency::pointPointEdge(outGeo0, attribHandle, vtxpnumAttribHandle,
                static_cast<const GA_PointGroup*>(geo0Group), nullptr,
                subscribeRatio, minGrainSize);
            break;
        case 1:
            GFE_Adjacency::pointPointEdge(outGeo0, attribHandle, vtxPrevAttribHandle, vtxNextAttribHandle,
                static_cast<const GA_PointGroup*>(geo0Group), nullptr,
                subscribeRatio, minGrainSize);
            break;
        case 2:
            GFE_Adjacency::pointPointEdge(outGeo0, attribHandle,
                static_cast<const GA_PointGroup*>(geo0Group), nullptr,
                subscribeRatio, minGrainSize);
            break;
        default:
            break;
        }
        attribPtr->bumpDataId();
    }

    if (calPointPointPrim)
    {
        GA_Attribute* attribPtr = outGeo0->getAttributes().createArrayAttribute(GA_ATTRIB_POINT, GA_SCOPE_PUBLIC, pointPointPrimAttribName, inStorageI, 1);
        //GA_Attribute* attribPtr = outGeo0->getAttributes().createTupleAttribute(GA_ATTRIB_POINT, pointPointPrimAttribName, 1, GA_Defaults(-1));
        attribHandle.bind(attribPtr);
        //GFE_Adjacency::pointPointPrim(outGeo0, attribHandle,
        //    static_cast<const GA_PointGroup*>(geo0Group), nullptr,
        //    subscribeRatio, minGrainSize);
        attribPtr->bumpDataId();
    }

    if (calPrimPrimEdge)
    {
        if (0 && kernel==0)
        {
            GFE_Adjacency::addAttribPrimPrimEdge(outGeo0,
                static_cast<const GA_PrimitiveGroup*>(geo0Group), vertexEdgeSeamGroup, 
                inStorageI, primPrimEdgeAttribName,
                nullptr, nullptr, GA_ReuseStrategy(),
                subscribeRatio, minGrainSize);
        }
        else
        {
            GA_Attribute* attribPtr = outGeo0->getAttributes().createArrayAttribute(GA_ATTRIB_PRIMITIVE, GA_SCOPE_PUBLIC, primPrimEdgeAttribName, inStorageI, 1);
            //GA_Attribute* attribPtr = outGeo0->getAttributes().createTupleAttribute(GA_ATTRIB_PRIMITIVE, primPrimEdgeAttribName, 1, GA_Defaults(-1));
            attribHandle.bind(attribPtr);
            switch (kernel)
            {
            case 0:
                GFE_Adjacency::primPrimEdge(outGeo0, attribHandle, vtxNextEquivAttribHandle,
                    static_cast<const GA_PrimitiveGroup*>(geo0Group), vertexEdgeSeamGroup,
                    subscribeRatio, minGrainSize);
                break;
            case 1:
                GFE_Adjacency::primPrimEdge1(outGeo0, attribHandle, dstptAttribHandle,
                    static_cast<const GA_PrimitiveGroup*>(geo0Group), vertexEdgeSeamGroup,
                    subscribeRatio, minGrainSize);
                break;
            case 2:
                GFE_Adjacency::primPrimEdge2(outGeo0, attribHandle, dstptAttribHandle,
                    static_cast<const GA_PrimitiveGroup*>(geo0Group), vertexEdgeSeamGroup,
                    subscribeRatio, minGrainSize);
                break;
            case 3:
                GFE_Adjacency::primPrimEdge3(outGeo0, attribHandle, dstptAttribHandle,
                    static_cast<const GA_PrimitiveGroup*>(geo0Group), vertexEdgeSeamGroup,
                    subscribeRatio, minGrainSize);
                break;
            case 4:
                GFE_Adjacency::primPrimEdge4(outGeo0, attribHandle,
                    static_cast<const GA_PrimitiveGroup*>(geo0Group), vertexEdgeSeamGroup,
                    subscribeRatio, minGrainSize);
                break;
            default:
                break;
            }
            attribPtr->bumpDataId();
        }
    }

    if (calPrimPrimPoint)
    {
        GA_Attribute* attribPtr = outGeo0->getAttributes().createArrayAttribute(GA_ATTRIB_PRIMITIVE, GA_SCOPE_PUBLIC, primPrimPointAttribName, inStorageI, 1);
        //GA_Attribute* attribPtr = outGeo0->getAttributes().createTupleAttribute(GA_ATTRIB_PRIMITIVE, primPrimPointAttribName, 1, GA_Defaults(-1));
        attribHandle.bind(attribPtr);
        GFE_Adjacency::primPrimPoint(outGeo0, attribHandle,
            static_cast<const GA_PrimitiveGroup*>(geo0Group), pointSeamGroup,
            subscribeRatio, minGrainSize);
        attribPtr->bumpDataId();
    }


}



namespace SOP_FeE_Adjacency_1_0_Namespace {

} // End SOP_FeE_Adjacency_1_0_Namespace namespace
