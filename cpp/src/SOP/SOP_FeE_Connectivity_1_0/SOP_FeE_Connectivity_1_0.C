
//#define UT_ASSERT_LEVEL 3
#include "SOP_FeE_Connectivity_1_0.h"



#include "SOP_FeE_Connectivity_1_0.proto.h"

#include "GA/GA_Detail.h"
#include "PRM/PRM_TemplateBuilder.h"
#include "UT/UT_Interrupt.h"
#include "UT/UT_DSOVersion.h"


#include "GU/GU_Promote.h"

#include "GFE/GFE_Attribute.h"
#include "GFE/GFE_GroupParse.h"
#include "GFE/GFE_GroupPromote.h"
#include "GFE/GFE_Connectivity.h"
#include "GFE/GFE_AttributeCast.h"



using namespace SOP_FeE_Connectivity_1_0_Namespace;



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
        name    "findInputPieceAttrib"
        cppname "FindInputPieceAttrib"
        label   "Find Input Piece Attribute"
        type    toggle
        default { "0" }
    }
    parm {
        name    "pieceAttribSearchOrder"
        cppname "PieceAttribSearchOrder"
        label   "Piece Attribute Class Search Order"
        type    ordinal
        default { "primpoint" }
        menu {
            "prim"       "Primitive"
            "point"      "Point"
            "vertex"     "Vertex"
            "primpoint"  "Prim Point"
            "pointprim"  "Point Prim"
            "all"        "All"
        }
        disablewhen "{ findInputPieceAttrib == 0 }"
    }
    parm {
        name    "promoteFromOtherClass"
        cppname "PromoteFromOtherClass"
        label   "Promote from Other Class"
        type    toggle
        default { "0" }
        disablewhen "{ findInputPieceAttrib == 0 }"
    }
    parm {
        name    "delOriginalAttrib"
        cppname "DelOriginalAttrib"
        label   "Delete Original Attrib"
        type    toggle
        default { "0" }
        disablewhen "{ findInputPieceAttrib == 0 } { promoteFromOtherClass == 0 }"
    }
    parm {
        name    "forceCastAttribType"
        cppname "ForceCastAttribType"
        label   "Force Cast Attrib Type"
        type    toggle
        default { "0" }
        disablewhen "{ findInputPieceAttrib == 0 }"
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
            "vertex"    "Vertex"
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
        name    "stringPrefix"
        cppname "StringPrefix"
        label   "String Prefix"
        type    string
        default { "piece" }
        disablewhen "{ connectivityAttribType != string }"
    }

    parm {
        name    "stringSufix"
        cppname "StringSufix"
        label   "String Sufix"
        type    string
        default { "" }
        disablewhen "{ connectivityAttribType != string }"
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
        name    "outTopoAttrib"
        cppname "OutTopoAttrib"
        label   "Output Topo Attribute"
        type    toggle
        default { "0" }
    }

    //parm {
    //    name    "subscribeRatio"
    //    cppname "SubscribeRatio"
    //    label   "Subscribe Ratio"
    //    type    integer
    //    default { 128 }
    //    range   { 0! 256 }
    //}
    //parm {
    //    name    "minGrainSize"
    //    cppname "MinGrainSize"
    //    label   "Min Grain Size"
    //    type    intlog
    //    default { 64 }
    //    range   { 0! 2048 }
    //}
}
)THEDSFILE";

PRM_Template*
SOP_FeE_Connectivity_1_0::buildTemplates()
{
    static PRM_TemplateBuilder templ("SOP_FeE_Connectivity_1_0.C"_sh, theDsFile);
    if (templ.justBuilt())
    {
        templ.setChoiceListPtr("group"_sh, &SOP_Node::allGroupMenu);
    }
    return templ.templates();
}




const UT_StringHolder SOP_FeE_Connectivity_1_0::theSOPTypeName("FeE::connectivity::1.0"_sh);

void
newSopOperator(OP_OperatorTable* table)
{
    OP_Operator* newOp = new OP_Operator(
        SOP_FeE_Connectivity_1_0::theSOPTypeName,
        "FeE Connectivity",
        SOP_FeE_Connectivity_1_0::myConstructor,
        SOP_FeE_Connectivity_1_0::buildTemplates(),
        1,
        1,
        nullptr,
        OP_FLAG_GENERATOR,
        nullptr,
        1,
        "Five elements Elf/Data/Topology");

    newOp->setIconName("SOP_connectivity");
    table->addOperator(newOp);

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


static bool
sopAttribType(SOP_FeE_Connectivity_1_0Parms::ConnectivityAttribType attribType)
{
    using namespace SOP_FeE_Connectivity_1_0Enums;
    switch (attribType)
    {
    case ConnectivityAttribType::INT:        return false;      break;
    case ConnectivityAttribType::STRING:     return true;      break;
    }
    UT_ASSERT_MSG(0, "Unhandled Geo0 Attrib Type!");
    return false;
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



static GFE_PieceAttribSearchOrder
sopPieceAttribSearchOrder(SOP_FeE_Connectivity_1_0Parms::PieceAttribSearchOrder parmgrouptype)
{
    using namespace SOP_FeE_Connectivity_1_0Enums;
    switch (parmgrouptype)
    {
    case PieceAttribSearchOrder::PRIM:       return GFE_PieceAttribSearchOrder_PRIM;         break;
    case PieceAttribSearchOrder::POINT:      return GFE_PieceAttribSearchOrder_POINT;        break;
    case PieceAttribSearchOrder::VERTEX:     return GFE_PieceAttribSearchOrder_VERTEX;       break;
    case PieceAttribSearchOrder::PRIMPOINT:  return GFE_PieceAttribSearchOrder_PRIMPOINT;    break;
    case PieceAttribSearchOrder::POINTPRIM:  return GFE_PieceAttribSearchOrder_POINTPRIM;    break;
    case PieceAttribSearchOrder::ALL:        return GFE_PieceAttribSearchOrder_ALL;          break;
    }
    UT_ASSERT_MSG(0, "Unhandled Piece Attrib Search Order!");
    return GFE_PieceAttribSearchOrder_INVALID;
}


void
SOP_FeE_Connectivity_1_0Verb::cook(const SOP_NodeVerb::CookParms& cookparms) const
{
    auto&& sopparms = cookparms.parms<SOP_FeE_Connectivity_1_0Parms>();
    GA_Detail* const outGeo0 = cookparms.gdh().gdpNC();
    //auto sopcache = (GFE_Connectivity_1_0Cache*)cookparms.cache();

    const GA_Detail* const inGeo0 = cookparms.inputGeo(0);

    outGeo0->replaceWith(*inGeo0);
    // outGeo0->clearAndDestroy();

    //outGeo0 = sopNodeProcess(*inGeo0);

    const UT_StringHolder& geo0AttribNames = sopparms.getConnectivityAttribName();
    if (!geo0AttribNames.isstring())
        return;

    const GA_AttributeOwner geo0AttribClass = sopAttribOwner(sopparms.getConnectivityAttribClass());
    const bool connectivityAttribType = sopAttribType(sopparms.getConnectivityAttribType());
    //const GA_AttributeType connectivityAttribType = sopparms.getConnectivityAttribType();

    const GA_Precision preferedPrecision = outGeo0->getPreferredPrecision();

    const UT_StringHolder& stringPrefix = sopparms.getStringPrefix();
    const UT_StringHolder& stringSufix = sopparms.getStringSufix();

    if (sopparms.getFindInputPieceAttrib())
    {
        const GFE_PieceAttribSearchOrder pieceAttribSearchOrder = sopPieceAttribSearchOrder(sopparms.getFindPieceAttribClass());
        GA_Attribute* attribPtr = GFE_Attribute::findPieceAttrib(outGeo0, pieceAttribSearchOrder, geo0AttribNames);

        if (attribPtr)
        {
            const bool promoteFromOtherClass = sopparms.getPromoteFromOtherClass();
            if (promoteFromOtherClass)
            {
                if (geo0AttribClass != attribPtr->getOwner())
                {
                    attribPtr = GU_Promote::promote(*static_cast<GU_Detail*>(outGeo0), attribPtr, geo0AttribClass, sopparms.getDelOriginalAttrib(), GU_Promote::GU_PROMOTE_FIRST);
                }
            }

            const bool forceCastAttribType = sopparms.getForceCastAttribType();
            if (forceCastAttribType)
            {
                GFE_AttributeCast::attribCast(outGeo0, attribPtr, connectivityAttribType ? GA_STORECLASS_STRING : GA_STORECLASS_INT, "", preferedPrecision);
            }
            return;
        }
    }


    const GA_GroupType groupType = sopGroupType(sopparms.getGroupType());

    GOP_Manager gop;
    const GA_Group* geo0Group = GFE_GroupParse_Namespace::findOrParseGroupDetached(cookparms, outGeo0, groupType, sopparms.getGroup(), gop);
    if (geo0Group && geo0Group->isEmpty())
        return;


    const bool useUVConnectivity = sopparms.getUseUVConnectivity();

    const UT_StringHolder& uvAttribName = sopparms.getUVAttribName();
    if (useUVConnectivity && !uvAttribName.isstring())
        return;


    UT_AutoInterrupt boss("Processing");
    if (boss.wasInterrupted())
        return;



    const UT_StringHolder& seamGroupName = sopparms.getSeamGroup();
    const bool useSeamGroup = seamGroupName.isstring();


    const bool connectivityConstraint = sopConnectivityConstraint(sopparms.getConnectivityConstraint());

    //const exint subscribeRatio = sopparms.getSubscribeRatio();
    //const exint minGrainSize = sopparms.getMinGrainSize();


    const GA_Storage inStorageI = GFE_Type::getPreferredStorageI(preferedPrecision);


    const GA_GroupUPtr geo0GroupUPtr = GFE_GroupPromote::groupPromoteDetached(outGeo0, geo0Group, connectivityConstraint ? GA_GROUP_PRIMITIVE : GA_GROUP_POINT);
    const GA_Group* const geo0GroupPromoted = geo0GroupUPtr.get();

    const GA_Group* const geo0SeamGroup = GFE_GroupParse_Namespace::findOrParseGroupDetached(cookparms, outGeo0, connectivityConstraint ? GA_GROUP_VERTEX : GA_GROUP_POINT, sopparms.getGroup(), gop);


    //notifyGroupParmListeners(cookparms.getNode(), 0, 1, outGeo0, geo0Group);


    GA_Attribute* attribPtr = outGeo0->findAttribute(geo0AttribClass, geo0AttribNames);
    if (attribPtr)
        outGeo0->getAttributes().destroyAttribute(attribPtr);


    attribPtr = GFE_Connectivity::addAttribConnectivity(outGeo0,
        geo0GroupPromoted, geo0SeamGroup,
        connectivityConstraint, geo0AttribClass,
        inStorageI, geo0AttribNames
        //, subscribeRatio, minGrainSize
    );

    if (connectivityAttribType) // string type
    {
        GFE_AttributeCast::attribCast(outGeo0, attribPtr, GA_STORECLASS_STRING, "", preferedPrecision);
    }


    //timeEnd = std::chrono::steady_clock::now();
    //diff = timeEnd - timeStart;
    //timeTotal1 += diff.count();

    //outGeo0->setDetailAttributeF("timeTotal", timeTotal * 1000);
    //outGeo0->setDetailAttributeF("timeTotal1", timeTotal1 * 1000);


    GFE_TopologyReference::outTopoAttrib(outGeo0, sopparms.getOutTopoAttrib());


}



namespace SOP_FeE_Connectivity_1_0_Namespace {

} // End SOP_FeE_Connectivity_1_0_Namespace namespace
