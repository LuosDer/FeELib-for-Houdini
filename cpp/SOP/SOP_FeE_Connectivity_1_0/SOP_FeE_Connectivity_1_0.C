
//#define UT_ASSERT_LEVEL 3
#include "SOP_FeE_Connectivity_1_0.h"


// This is an automatically generated header file based on theDsFile, below,
// to provide SOP_FeE_Connectivity_1_0Parms, an easy way to access parameter values from
// SOP_FeE_Connectivity_1_0Verb::cook with the correct type.
#include "SOP_FeE_Connectivity_1_0.proto.h"

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
#include <GA/GA_PageHandle.h>
#include <GA/GA_AIFNumericArray.h>

#include <UT/UT_UniquePtr.h>
#include <GA/GA_SplittableRange.h>
#include <HOM/HOM_SopNode.h>


#include <GU/GU_Measure.h>
#include <GU/GU_Promote.h>

#include <GU_FeE/GU_FeE_Connectivity.h>

#include <GEO_FeE/GEO_FeE_Adjacency.h>

using namespace SOP_FeE_Connectivity_1_0_Namespace;

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
const UT_StringHolder SOP_FeE_Connectivity_1_0::theSOPTypeName("FeE::connectivity::1.0"_sh);

/// newSopOperator is the hook that Houdini grabs from this dll
/// and invokes to register the SOP.  In this case, we add ourselves
/// to the specified operator table.
void
newSopOperator(OP_OperatorTable *table)
{
    table->addOperator(new OP_Operator(
        SOP_FeE_Connectivity_1_0::theSOPTypeName,   // Internal name
        "FeE Connectivity",                     // UI name
        SOP_FeE_Connectivity_1_0::myConstructor,    // How to build the SOP
        SOP_FeE_Connectivity_1_0::buildTemplates(), // My parameters
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
        name    "connectivityConstraint"
        cppname "ConnectivityConstraint"
        label   "Connectivity Constraint"
        type    ordinal
        default { "point" }
        menu {
            "point"     "Point"
            "edge"      "Edge"
        }
    }
    parm {
        name    "connectivityAttribClass"
        cppname "ConnectivityAttribClass"
        label   "Connectivity Attribute Class"
        type    ordinal
        default { "prim" }
        menu {
            "prim"      "Primitive"
            "point"     "Point"
        }
    }
    parm {
        name    "connectivityAttribName"
        cppname "ConnectivityAttribName"
        label   "Connectivity Attribute Name"
        type    string
        default { "class" }
    }
    parm {
        name    "connectivityAttribType"
        cppname "ConnectivityAttribType"
        label   "Connectivity Attribute Type"
        type    ordinal
        default { "int" }
        menu {
            "int"        "Integer"
            "string"     "String"
        }
    }


    parm {
        name    "seamGroup"
        cppname "SeamGroup"
        label   "Seam Group"
        type    string
        default { "" }
        parmtag { "script_action" "import soputils\nkwargs['geometrytype'] = (hou.geometryType.Edges,)\nkwargs['inputindex'] = 0\nsoputils.selectGroupParm(kwargs)" }
        parmtag { "script_action_help" "Select geometry from an available viewport.\nShift-click to turn on Select Groups." }
        parmtag { "script_action_icon" "BUTTONS_reselect" }
        parmtag { "sop_input" "0" }
    }
    parm {
        name    "useUVConnectivity"
        cppname "UseUVConnectivity"
        label   "Use UV Connectivity"
        type    toggle
        default { "0" }
    }
    parm {
        name    "uvAttribName"
        cppname "UVAttribName"
        label   "UV Attrib Name"
        type    string
        default { "uv" }
        parmtag { "script_action" "import soputils\nkwargs['geometrytype'] = (hou.geometryType.Points,)\nkwargs['inputindex'] = 0\nsoputils.selectGroupParm(kwargs)" }
        parmtag { "script_action_help" "Select geometry from an available viewport.\nShift-click to turn on Select Groups." }
        parmtag { "script_action_icon" "BUTTONS_reselect" }
        parmtag { "sop_input" "0" }
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
SOP_FeE_Connectivity_1_0::buildTemplates()
{
    static PRM_TemplateBuilder templ("SOP_FeE_Connectivity_1_0.C"_sh, theDsFile);
    return templ.templates();
}


//class SOP_FeE_Connectivity_1_0Cache : public SOP_NodeCache
//{
//public:
//    SOP_FeE_Connectivity_1_0Cache() : SOP_NodeCache(),
//        myPrevOutputDetailID(-1)
//    {}
//    ~SOP_FeE_Connectivity_1_0Cache() override {}
//
//    int64 myPrevOutputDetailID;
//};


class SOP_FeE_Connectivity_1_0Verb : public SOP_NodeVerb
{
public:
    SOP_FeE_Connectivity_1_0Verb() {}
    virtual ~SOP_FeE_Connectivity_1_0Verb() {}

    virtual SOP_NodeParms *allocParms() const { return new SOP_FeE_Connectivity_1_0Parms(); }
    //virtual SOP_NodeCache* allocCache() const { return new SOP_FeE_Connectivity_1_0Cache(); }
    virtual UT_StringHolder name() const { return SOP_FeE_Connectivity_1_0::theSOPTypeName; }

    virtual CookMode cookMode(const SOP_NodeParms *parms) const { return COOK_GENERIC; }

    virtual void cook(const CookParms &cookparms) const;

    /// This static data member automatically registers
    /// this verb class at library ldir0d time.
    static const SOP_NodeVerb::Register<SOP_FeE_Connectivity_1_0Verb> theVerb;
};

// The static member variable definition has to be outside the class definition.
// The declaration is inside the class.
const SOP_NodeVerb::Register<SOP_FeE_Connectivity_1_0Verb> SOP_FeE_Connectivity_1_0Verb::theVerb;

const SOP_NodeVerb *
SOP_FeE_Connectivity_1_0::cookVerb() const 
{ 
    return SOP_FeE_Connectivity_1_0Verb::theVerb.get();
}






static bool
sopConnectivityConstraint(SOP_FeE_Connectivity_1_0Parms::ConnectivityConstraint parmConnectivityConstraint)
{
    using namespace SOP_FeE_Connectivity_1_0Enums;
    switch (parmConnectivityConstraint)
    {
    case ConnectivityConstraint::POINT:    return false;     break;
    case ConnectivityConstraint::EDGE:     return true;      break;
    }
    UT_ASSERT_MSG(0, "Unhandled Connectivity Constraint!");
    return false;
}


static exint
sopAttribType(SOP_FeE_Connectivity_1_0Parms::ConnectivityAttribType attribType)
{
    using namespace SOP_FeE_Connectivity_1_0Enums;
    switch (attribType)
    {
    case ConnectivityAttribType::INT:        return 0;      break;
    case ConnectivityAttribType::STRING:     return 1;      break;
    }
    UT_ASSERT_MSG(0, "Unhandled Geo0 Attrib Type!");
    return -1;
}


static GA_AttributeOwner
sopAttribOwner(SOP_FeE_Connectivity_1_0Parms::ConnectivityAttribClass attribClass)
{
    using namespace SOP_FeE_Connectivity_1_0Enums;
    switch (attribClass)
    {
    case ConnectivityAttribClass::PRIM:      return GA_ATTRIB_PRIMITIVE;  break;
    case ConnectivityAttribClass::POINT:     return GA_ATTRIB_POINT;      break;
    }
    UT_ASSERT_MSG(0, "Unhandled Geo0 Class type!");
    return GA_ATTRIB_INVALID;
}

static GA_GroupType
sopGroupType(SOP_FeE_Connectivity_1_0Parms::GroupType parmgrouptype)
{
    using namespace SOP_FeE_Connectivity_1_0Enums;
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
SOP_FeE_Connectivity_1_0Verb::cook(const SOP_NodeVerb::CookParms& cookparms) const
{
    auto&& sopparms = cookparms.parms<SOP_FeE_Connectivity_1_0Parms>();
    GU_Detail* outGeo0 = cookparms.gdh().gdpNC();
    //auto sopcache = (SOP_FeE_Connectivity_1_0Cache*)cookparms.cache();

    const GEO_Detail* const inGeo0 = cookparms.inputGeo(0);

    outGeo0->replaceWith(*inGeo0);
    // outGeo0->clearAndDestroy();

    //outGeo0 = sopNodeProcess(*inGeo0);

    const UT_StringHolder& geo0AttribNames = sopparms.getConnectivityAttribName();
    if (!geo0AttribNames.isstring())
        return;

    const bool useUVConnectivity = sopparms.getUseUVConnectivity();

    const UT_StringHolder& uvAttribName = sopparms.getUVAttribName();
    if (useUVConnectivity && !uvAttribName.isstring())
        return;

    const UT_StringHolder& seamGroupName = sopparms.getSeamGroup();
    const bool useSeamGroup = seamGroupName.isstring();


    GOP_Manager gop;
    const GA_AttributeOwner geo0AttribClass = sopAttribOwner(sopparms.getConnectivityAttribClass());
    const exint connectivityAttribType = sopAttribType(sopparms.getConnectivityAttribType());
    //const GA_AttributeType connectivityAttribType = sopparms.getConnectivityAttribType();
    const bool connectivityConstraint = sopConnectivityConstraint(sopparms.getConnectivityConstraint());

    const exint subscribeRatio = sopparms.getSubscribeRatio();
    //const exint minGrainSize = pow(2, 8);
    //const exint minGrainSize = pow(2, 4);
    const exint minGrainSize = sopparms.getMinGrainSize();


    //const GA_Storage inStorageI = SYSisSame<T, fpreal32>() ? GA_STORE_REAL32 : GA_STORE_REAL64;
    const GA_Storage inStorageI = GA_STORE_INT32;


    UT_AutoInterrupt boss("Processing");
    if (boss.wasInterrupted())
        return;







    const GA_ElementGroup* geo0Group = nullptr;
    const UT_StringHolder& groupName0 = sopparms.getGroup();

    if (groupName0.isstring())
    {
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


    /// <summary>
    /// ///////////////////////////////
    /// </summary>
    /// <param name="cookparms"></param>

    GA_ATINumericUPtr vtxpnumAttribUPtr;
    GA_RWHandleT<GA_Size> vtxpnumAttribHandle;

    vtxpnumAttribUPtr = outGeo0->createDetachedTupleAttribute(GA_ATTRIB_VERTEX, inStorageI, 1, GA_Defaults(-1));
    //GA_ATINumeric* vtxpnumATI = vtxpnumAttribUPtr.get();
    vtxpnumAttribHandle.bind(vtxpnumAttribUPtr.get());

    GEO_FeE_Adjacency::vertexPrimIndex(outGeo0, vtxpnumAttribHandle,
        static_cast<const GA_VertexGroup*>(geo0Group),
        subscribeRatio, minGrainSize);




    /// <summary>
    /// ///////////////////////////////
    /// </summary>
    /// <param name="cookparms"></param>

    GA_ATINumericUPtr dstptAttribUPtr;
    GA_RWHandleT<GA_Offset> dstptAttribHandle;
    if (connectivityConstraint)
    {
        dstptAttribUPtr = outGeo0->createDetachedTupleAttribute(GA_ATTRIB_VERTEX, inStorageI, 1, GA_Defaults(-1));
        //GA_ATINumeric* dstptATI = dstptAttribUPtr.get();

        dstptAttribHandle.bind(dstptAttribUPtr.get());
        GEO_FeE_Adjacency::vertexPointDst(outGeo0, dstptAttribHandle, vtxpnumAttribHandle,
            static_cast<const GA_VertexGroup*>(geo0Group),
            subscribeRatio, minGrainSize);
    }
    



    /// <summary>
    /// ///////////////////////////////
    /// </summary>
    /// <param name="cookparms"></param>

#if 0
    const UT_StringHolder attribTypeName = "adjElems";
    const UT_StringRef attribTypeNameRef = "adjElems";
    //const GA_AttributeType attribType(attribTypeName);
    //const UT_Options options("int iarray", 0, nullptr);
    const UT_Options options("int64 i64array", int64(0), nullptr);

    const GA_AttributeOwner connectivityConstraintOwner = connectivityConstraint ? GA_ATTRIB_PRIMITIVE : GA_ATTRIB_POINT;

    GA_AttributeUPtr adjElemsAttribPtr = outGeo0->createDetachedAttribute(connectivityConstraintOwner, attribTypeNameRef, &options, nullptr);
    GA_RWHandleT<UT_ValArray<GA_Offset>> adjElemsAttribHandle(adjElemsAttribPtr.get());

#else
    GA_Attribute* adjElemsAttribPtr = outGeo0->addIntArray(connectivityConstraint ? GA_ATTRIB_PRIMITIVE : GA_ATTRIB_POINT, "__adjElems_SOP_FeE_Connectivity_1_0", 1, 0, 0, inStorageI);
    GA_RWHandleT<UT_ValArray<GA_Offset>> adjElemsAttribHandle(adjElemsAttribPtr);
#endif

    if (connectivityConstraint)
        GEO_FeE_Adjacency::primPrimEdge(outGeo0, adjElemsAttribHandle, dstptAttribHandle,
            static_cast<const GA_PrimitiveGroup*>(geo0Group), nullptr,
            subscribeRatio, minGrainSize);
    else
        GEO_FeE_Adjacency::pointPointEdge(outGeo0, adjElemsAttribHandle, vtxpnumAttribHandle,
            static_cast<const GA_PointGroup*>(geo0Group), nullptr,
            subscribeRatio, minGrainSize);







    /// <summary>
    /// ///////////////////////////////
    /// </summary>
    /// <param name="cookparms"></param>


    GA_Attribute* attribPtr = outGeo0->addIntTuple(geo0AttribClass, geo0AttribNames, 1, GA_Defaults(-1), 0, 0, inStorageI);
    GA_RWHandleT<GA_Size> attribHandle(attribPtr);

    if (connectivityConstraint)
        GU_FeE_Connectivity::connectivity(outGeo0, attribHandle, adjElemsAttribHandle, static_cast<const GA_PrimitiveGroup*>(geo0Group));
    else
        GU_FeE_Connectivity::connectivity(outGeo0, attribHandle, adjElemsAttribHandle, static_cast<const GA_PointGroup*>(geo0Group));




    GA_Attribute* attrib = outGeo0->findFloatTuple(GA_ATTRIB_POINT, "N", 3);
    const GA_AIFTuple* tuple = attrib->getAIFTuple();
    if (tuple)
    {
        UT_Vector3 N;
        for (GA_Iterator it(outGeo0->getPointRange()); !it.atEnd(); it.advance())
        {
            GA_Offset offset = it.getOffset();
            tuple->get(attrib; offset, N.data(), 3);
            N.normalize();
            tuple->set(attrib, offset, N.data(), 3);
        }
    }
    // Blind data struct
    class Struct { int a; };
    // Add a "Struct" object for every point
    // Note that we set the scope to GA_SCOPE_PRIVATE so that the
    // attribute isn't visible to users or saved/loaded.
    GA_RWAttributeRef   blind_gah = outGeo0->createAttribute(
        GA_ATTRIB_POINT,
        GA_SCOPE_PRIVATE,
        "attrib_name",
        NULL, NULL,
        "blinddata");

    if (blind_gah.isValid())
    {
        GA_Attribute* a = blind_gah.getAttribute();
        const GA_AIFBlindData* aif = a->getAIFBlindData();
        if (aif)
        {
            Struct      default_value();        // Default value
            aif->setDataSize(a, sizeof(Struct), &default_value);
        }
    }

    GA_RWAttributeRef    blind_gah = outGeo0->findPointAttribute(
        GA_SCOPE_PRIVATE,
        "attrib_name");
    const GA_Attribute* a = blind_gah.getAttribute();
    const GA_AIFBlindData* aif;
    if (aif = a->getAIFBlindData())
    {
        for (GA_Iterator it = range.begin(); !it.atEnd(); ++it)
        {
            const Struct& pt_value = aif->getValue<GA_SCOPE_PRIVATE>(a, *it);
            ...
        }
    }



    GA_ROPageHandleV3 v_ph(outGeo0, GA_ATTRIB_POINT, "v");
    GA_RWPageHandleV3 p_ph(outGeo0->getP());
    if (v_ph.isValid() && p_ph.isValid())
    {
        GA_Offset start, end;
        for (GA_Iterator it(outGeo0->getPointRange()); it.blockAdvance(start, end); )
        {
            v_ph.setPage(start);
            p_ph.setPage(start);
#if 1
            // Use Vector Math library
            VM_Math::madd((fpreal32*)&p_ph.value(start),
                (const fpreal32*)&v_ph.value(start),
                0.1, (end - start) * 3);
#else
            for (GA_Offset pt = start; pt < end; ++pt)
            {
                p_ph.value(pt) += v_ph.get(pt) * 0.1;
            }
#endif
        }
    }

}



namespace SOP_FeE_Connectivity_1_0_Namespace {

} // End SOP_FeE_Connectivity_1_0_Namespace namespace
