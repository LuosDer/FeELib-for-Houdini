
#include "SOP_FeE_WindingNumber_1_0.h"

#include "SOP_FeE_WindingNumber_1_0.proto.h"

#include "GA/GA_Detail.h"
#include "PRM/PRM_TemplateBuilder.h"
#include "UT/UT_Interrupt.h"
#include "UT/UT_DSOVersion.h"

#include "GA_FeE/GA_FeE_WindingNumber.h"

using namespace SOP_FeE_WindingNumber_1_0_Namespace;


class SOP_FeE_WindingNumber_1_0Verb : public SOP_NodeVerb
{
public:
    virtual SOP_NodeParms *allocParms() const { return new SOP_FeE_WindingNumber_1_0Parms(); }
    virtual SOP_NodeCache *allocCache() const { return new GA_WindingNumber_Cache(); }
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
        name    "wnAttribName"
        cppname "WNAttribName"
        label   "Attribute Name"
        type    string
        default { "windingNumber" }
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
        default { "64" }
        menu {
            "auto"   "Auto"
            "_16"    "16"
            "_32"    "32"
            "_64"    "64"
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

static GA_WindingNumberType
sopWNType(SOP_FeE_WindingNumber_1_0Parms::WNType wnType)
{
    using namespace SOP_FeE_WindingNumber_1_0Enums;
    switch (wnType)
    {
    case WNType::XYZ:      return GA_WNType_XYZ;   break;
    case WNType::XY:       return GA_WNType_XY;    break;
    case WNType::YZ:       return GA_WNType_YZ;    break;
    case WNType::ZX:       return GA_WNType_ZX;    break;
    }
    UT_ASSERT_MSG(0, "Unhandled WNType!");
    return GA_WNType_XYZ;
}



/// This is the function that does the actual work.
void SOP_FeE_WindingNumber_1_0Verb::cook(const SOP_NodeVerb::CookParms& cookparms) const
{
    auto &&sopparms = cookparms.parms<SOP_FeE_WindingNumber_1_0Parms>();
    auto sopcache = (GA_WindingNumber_Cache*)cookparms.cache();

    GA_Detail* const geoPoint = cookparms.gdh().gdpNC();
    const GA_Detail* const geoRefMesh = cookparms.inputGeo(1);
    

    const GA_Storage wnStorage = sopWNStorage(sopparms.getWNPrecision());

    //const GA_Storage inStorageF = GA_FeE_Type::getPreferredStorageF(geoPoint);
    
#if 0
    GA_FeE_WindingNumber::GA_WindingNumber_Cache* sopcache = (GA_FeE_WindingNumber::GA_WindingNumber_Cache*)cookparms.cache();

    GA_Attribute* wnAttribPtr = GA_FeE_WindingNumber::addAttribWindingNumber(
        cookparms, geoPoint, geoRefMesh,
        sopparms.getQueryPoints(), sopparms.getMeshPrims(), sopcache,
        GA_STORE_INVALID, sopparms.getWNAttribName(),
        sopWNType(sopparms.getWNType()), sopparms.getFullAccuracy(), sopparms.getAccuracyScale(), sopparms.getAsSolidAngle(), sopparms.getNegate()
    );
#else
    GA_Attribute* wnAttribPtr = GA_FeE_WindingNumber::addAttribWindingNumber(
        cookparms, geoPoint, geoRefMesh,
        sopparms.getWNQueryPointGroup(), sopparms.getWNMeshPrimGroup(), sopcache,
        wnStorage, sopparms.getWNAttribName(),
        sopWNType(sopparms.getWNType()), sopparms.getWNFullAccuracy(), sopparms.getWNAccuracyScale(), sopparms.getWNAsSolidAngle(), sopparms.getWNNegate()
    );
#endif

}
