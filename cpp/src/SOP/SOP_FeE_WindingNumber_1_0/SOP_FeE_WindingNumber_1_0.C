
#include "SOP_FeE_WindingNumber_1_0.h"

#include "SOP_FeE_WindingNumber_1_0.proto.h"

#include "GA/GA_Detail.h"
#include "PRM/PRM_TemplateBuilder.h"
#include "UT/UT_Interrupt.h"
#include "UT/UT_DSOVersion.h"

#include "GFE/GFE_WindingNumber.h"




using namespace SOP_FeE_WindingNumber_1_0_Namespace;


class SOP_FeE_WindingNumber_1_0Verb : public SOP_NodeVerb
{
public:
    virtual SOP_NodeParms *allocParms() const { return new SOP_FeE_WindingNumber_1_0Parms(); }
    virtual SOP_NodeCache *allocCache() const { return new GFE_WindingNumber_Cache(); }
    virtual UT_StringHolder name() const { return theSOPTypeName; }

    /// This SOP wouldn't get any benefit from the results of the previous cook,
    /// (except for what's stored in SOP_FeE_WindingNumber_1_0Cache), and it would
    /// always duplicate its input at the start of the cook anyway, so it might
    /// as well use COOK_INPLACE, (which always either steals the first input's
    /// detail for the output detail or duplicates it into the output detail),
    /// instead of COOK_GENERIC, (which would let us have an output detail
    /// that's separate from the input detail and might be the same output
    /// detail as on a previous cook).
    virtual CookMode cookMode(const SOP_NodeParms *parms) const { return COOK_INPLACE; }

    virtual void cook(const CookParms &cookparms) const;

    /// This is the internal name of the SOP type.
    /// It isn't allowed to be the same as any other SOP's type name.
    static const UT_StringHolder theSOPTypeName;

    /// This static data member automatically registers
    /// this verb class at library load time.
    static const SOP_NodeVerb::Register<SOP_FeE_WindingNumber_1_0Verb> theVerb;

    /// This is the parameter interface string, below.
    static const char *const theDsFile;
};

// The static member variable definitions have to be outside the class definition.
// The declarations are inside the class.
const UT_StringHolder SOP_FeE_WindingNumber_1_0Verb::theSOPTypeName("FeE::windingNumber::1.0"_sh);
const SOP_NodeVerb::Register<SOP_FeE_WindingNumber_1_0Verb> SOP_FeE_WindingNumber_1_0Verb::theVerb;

//******************************************************************************
//*                 Parameters                                                 *
//******************************************************************************

/// This is a multi-line raw string specifying the parameter interface for this SOP.

static const char* theDsFile = R"THEDSFILE(
{
    name        parameters
    parm {
        name    "wnQueryPointGroup"
        cppname "WNQueryPointGroup"
        label   "Query Point Group"
        type    string
        default { "" }
        parmtag { "script_action" "import soputils\nkwargs['geometrytype'] = (hou.geometryType.Points,)\nkwargs['inputindex'] = 0\nsoputils.selectGroupParm(kwargs)" }
        parmtag { "script_action_help" "Select geometry from an available viewport.\nShift-click to turn on Select Groups." }
        parmtag { "script_action_icon" "BUTTONS_reselect" }
    }
    parm {
        name    "wnMeshPrimGroup"
        cppname "WNMeshPrimGroup"
        label   "Mesh Primitive Group"
        type    string
        default { "" }
        parmtag { "script_action" "import soputils\nkwargs['geometrytype'] = (hou.geometryType.Primitives,)\nkwargs['inputindex'] = 1\nsoputils.selectGroupParm(kwargs)" }
        parmtag { "script_action_help" "Select geometry from an available viewport.\nShift-click to turn on Select Groups." }
        parmtag { "script_action_icon" "BUTTONS_reselect" }
        parmtag { "sop_input" "1" }
    }
    parm {
        name    "wnType"
        cppname "WNType"
        label   "Winding Number Type"
        type    ordinal
        default { "0" }
        menu {
            "xyz"   "3D"
            "xy"    "2D in XY Plane"
            "yz"    "2D in YZ Plane"
            "zx"    "2D in ZX Plane"
        }
    }
    parm {
        name    "outWN"
        cppname "OutWN"
        label   "Output Winding Number"
        type    toggle
        nolabel
        joinnext
        default { "0" }
    }

    parm {
        name    "wnAttribClass"
        cppname "WNAttribClass"
        label   "Attribute Class"
        type    ordinal
        default { "point" }
        menu {
            "prim"      "Primitive"
            "point"     "Point"
            "vertex"    "Vertex"
        }
    }
    parm {
        name    "wnAttribName"
        cppname "WNAttribName"
        label   "Attribute Name"
        type    string
        default { "windingNumber" }
        disablewhen "{ outWN == 0 }"
    }
    parm {
        name    "wnAsSolidAngle"
        cppname "WNAsSolidAngle"
        label   "Scale to Solid Angle"
        type    toggle
        default { "0" }
        joinnext
    }
    parm {
        name    "wnNegate"
        cppname "WNNegate"
        label   "Negate Value (Reverse)"
        type    toggle
        default { "0" }
    }
    parm {
        name    "wnFullAccuracy"
        cppname "WNFullAccuracy"
        label   "Full Accuracy (Slow)"
        type    toggle
        default { "0" }
    }
    parm {
        name    "wnAccuracyScale"
        cppname "WNAccuracyScale"
        label   "Accuracy Scale"
        type    float
        default { "2" }
        range   { 1! 20 }
        disablewhen "{ wnFullAccuracy == 1 }"
    }
    parm {
        name    "wnPrecision"
        cppname "WNPrecision"
        label   "Precision"
        type    ordinal
        default { "auto" }
        menu {
            "auto"   "Auto"
            "_16"    "16"
            "_32"    "32"
            "_64"    "64"
        }
    }

    parm {
        name    "outPointInMeshGroup"
        cppname "OutPointInMeshGroup"
        label   "Output Point in Mesh Group"
        type    toggle
    //    nolabel
    //    joinnext
        default { "0" }
    }
    groupsimple {
        name    "pointInMeshGroup_folder"
        label   "Point in Mesh Group"
        grouptag { "group_type" "simple" }
        disablewhen "{ outPointInMeshGroup == 0 }"

        parm {
            name    "pointInMeshGroupType"
            cppname "PointInMeshGroupType"
            label   "Point in Mesh Group Type"
            type    ordinal
            default { "point" }
            menu {
                "prim"      "Primitive"
                "point"     "Point"
                "vertex"    "Vertex"
                "edge"      "Edge"
            }
        }
        parm {
            name    "pointInMeshGroupName"
            cppname "PointInMeshGroupName"
            label   "Point in Mesh Group Name"
            type    string
            default { "inMesh" }
        }
        parm {
            name    "groupMergeType"
            cppname "GroupMergeType"
            label   "Group Merge Type"
            type    ordinal
            default { "replace" }
            menu {
                "replace"   "Replace Existing"
                "union"     "Union with Existing"
                "intersect" "Intersect with Existing"
                "subtract"  "Subtract from Existing"
            }
        }
        parm {
            name    "groupInGeoPoint"
            cppname "GroupInGeoPoint"
            label   "In Geo"
            type    toggle
            default { "on" }
        }
        parm {
            name    "groupOnGeoPoint"
            cppname "GroupOnGeoPoint"
            label   "On Geo"
            type    toggle
            default { "on" }
        }
        parm {
            name    "pointInMeshThreshold"
            cppname "PointInMeshThreshold"
            label   "Point in Mesh Threshold"
            type    log
            default { "1e-05" }
            range   { 1e-05 1 }
        }
        parm {
            name    "reverseGroup"
            cppname "ReverseGroup"
            label   "Reverse Group"
            type    toggle
            default { "0" }
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
}
)THEDSFILE";

PRM_Template*
SOP_FeE_WindingNumber_1_0::buildTemplates()
{
    static PRM_TemplateBuilder templ("SOP_FeE_WindingNumber_1_0.C"_sh, theDsFile);
    if (templ.justBuilt())
    {
        templ.setChoiceListPtr("wnQueryPointGroup"_sh, &SOP_Node::pointGroupMenu);
        templ.setChoiceListPtr("wnMeshPrimGroup"_sh, &SOP_Node::primGroupMenu);
        templ.setChoiceListPtr("wnAttribName"_sh, &SOP_Node::pointAttribReplaceMenu);
    }
    return templ.templates();
}

const SOP_NodeVerb *SOP_FeE_WindingNumber_1_0::cookVerb() const
{
    return SOP_FeE_WindingNumber_1_0Verb::theVerb.get();
}

/// newSopOperator is the hook that Houdini grabs from this dll
/// and invokes to register the SOP.  In this case, we add ourselves
/// to the specified operator table.
void
newSopOperator(OP_OperatorTable *table)
{
    OP_Operator* newOp = new OP_Operator(
        SOP_FeE_WindingNumber_1_0Verb::theSOPTypeName,
        "FeE Winding Number",
        SOP_FeE_WindingNumber_1_0::myConstructor,
        SOP_FeE_WindingNumber_1_0::buildTemplates(),
        2,
        2,
        nullptr,
        0,  // No flags: not a generator, no merge input, not an output
        nullptr,
        1,
        "Five elements Elf/Data");

    newOp->setIconName("SOP_windingnumber");
    table->addOperator(newOp);
}




static GA_Storage
sopWNStorage(SOP_FeE_WindingNumber_1_0Parms::WNPrecision wnPrecision)
{
    using namespace SOP_FeE_WindingNumber_1_0Enums;
    switch (wnPrecision)
    {
    case WNPrecision::AUTO:     return GA_STORE_INVALID;   break;
    case WNPrecision::_16:      return GA_STORE_REAL16;    break;
    case WNPrecision::_32:      return GA_STORE_REAL32;    break;
    case WNPrecision::_64:      return GA_STORE_REAL64;    break;
    }
    UT_ASSERT_MSG(0, "Unhandled WNPrecision!");
    return GA_STORE_INVALID;
}

static GFE_WNType
sopWNType(SOP_FeE_WindingNumber_1_0Parms::WNType wnType)
{
    using namespace SOP_FeE_WindingNumber_1_0Enums;
    switch (wnType)
    {
    case WNType::XYZ:      return GFE_WNType::XYZ;   break;
    case WNType::XY:       return GFE_WNType::XY;    break;
    case WNType::YZ:       return GFE_WNType::YZ;    break;
    case WNType::ZX:       return GFE_WNType::ZX;    break;
    }
    UT_ASSERT_MSG(0, "Unhandled WNType!");
    return GFE_WNType::XYZ;
}


static GA_AttributeOwner
sopAttribOwner(SOP_FeE_WindingNumber_1_0Parms::WNAttribClass parmAttribClass)
{
    using namespace SOP_FeE_WindingNumber_1_0Enums;
    switch (parmAttribClass)
    {
    case WNAttribClass::PRIM:      return GA_ATTRIB_PRIMITIVE;  break;
    case WNAttribClass::POINT:     return GA_ATTRIB_POINT;      break;
    case WNAttribClass::VERTEX:    return GA_ATTRIB_VERTEX;     break;
    }
    UT_ASSERT_MSG(0, "Unhandled Geo0 Class type!");
    return GA_ATTRIB_INVALID;
}

static GA_GroupType
sopGroupType(SOP_FeE_WindingNumber_1_0Parms::PointInMeshGroupType parmGroupType)
{
    using namespace SOP_FeE_WindingNumber_1_0Enums;
    switch (parmGroupType)
    {
    case PointInMeshGroupType::PRIM:      return GA_GROUP_PRIMITIVE;  break;
    case PointInMeshGroupType::POINT:     return GA_GROUP_POINT;      break;
    case PointInMeshGroupType::VERTEX:    return GA_GROUP_VERTEX;     break;
    case PointInMeshGroupType::EDGE:      return GA_GROUP_EDGE;       break;
    }
    UT_ASSERT_MSG(0, "Unhandled Point in Mesh Group Type!");
    return GA_GROUP_INVALID;
}



static GFE_GroupMergeType
sopGroupMergeType(SOP_FeE_WindingNumber_1_0Parms::GroupMergeType groupMergeType)
{
    using namespace SOP_FeE_WindingNumber_1_0Enums;
    switch (groupMergeType)
    {
    case GroupMergeType::REPLACE:     return GFE_GroupMergeType::Replace;    break;
    case GroupMergeType::UNION:       return GFE_GroupMergeType::Union;      break;
    case GroupMergeType::INTERSECT:   return GFE_GroupMergeType::Intersect;  break;
    case GroupMergeType::SUBTRACT:    return GFE_GroupMergeType::Subtract;   break;
    }
    UT_ASSERT_MSG(0, "Unhandled Group Merge Type!");
    return GFE_GroupMergeType::Replace;
}


/// This is the function that does the actual work.
void SOP_FeE_WindingNumber_1_0Verb::cook(const SOP_NodeVerb::CookParms& cookparms) const
{
    auto &&sopparms = cookparms.parms<SOP_FeE_WindingNumber_1_0Parms>();
    auto sopcache = (GFE_WindingNumber_Cache*)cookparms.cache();

    GA_Detail& geoPoint = *cookparms.gdh().gdpNC();
    const GA_Detail& geoRefMesh = *cookparms.inputGeo(1);
    

    const GA_Storage wnStorage = sopWNStorage(sopparms.getWNPrecision());
    const GFE_GroupMergeType groupMergeType = sopGroupMergeType(sopparms.getGroupMergeType());


    
    GFE_WindingNumber wn(geoPoint, geoRefMesh, sopcache, &cookparms);

    wn.setInGroup(sopparms.getWNQueryPointGroup(), sopparms.getWNMeshPrimGroup());

    const GA_AttributeOwner attribOwner = sopAttribOwner(sopparms.getWNAttribClass());
            
    if (sopparms.getOutPointInMeshGroup() || sopparms.getOutWN())
        wn.findOrCreateTuple(!sopparms.getOutWN(), attribOwner, wnStorage, sopparms.getWNAttribName());

    if (sopparms.getOutPointInMeshGroup())
    {
        wn.groupSetter.setParm(groupMergeType, sopparms.getReverseGroup());
        wn.setPointInMeshComputeParm(
            sopparms.getGroupInGeoPoint(),
            sopparms.getGroupOnGeoPoint(),
            sopparms.getPointInMeshThreshold());
        
        const GA_GroupType groupType = sopGroupType(sopparms.getPointInMeshGroupType());
            
        wn.findOrCreateGroup(false, groupType, sopparms.getPointInMeshGroupName());
    }

    wn.setComputeParm(sopWNType(sopparms.getWNType()),
        sopparms.getWNFullAccuracy(), sopparms.getWNAccuracyScale(),
        sopparms.getWNAsSolidAngle(), sopparms.getWNNegate());

    wn.computeAndBumpDataId();
    wn.visualizeOutGroup();

}
