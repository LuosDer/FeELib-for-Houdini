
//#define UT_ASSERT_LEVEL 3
#include "SOP_FeE_Measure_3_0.h"


// This is an automatically generated header file based on theDsFile, below,
// to provide SOP_FeE_Measure_3_0Parms, an easy way to access parameter values from
// SOP_FeE_Measure_3_0Verb::cook with the correct type.
#include "SOP_FeE_Measure_3_0.proto.h"

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


#include <GU/GU_Measure.h>
#include <GEO/GEO_SplitPoints.h>

#include <GA_FeE/GA_FeE_Type.h>
#include <GA_FeE/GA_FeE_Attribute.h>
#include <GA_FeE/GA_FeE_Group.h>
#include <GA_FeE/GA_FeE_Measure.h>


using namespace SOP_FeE_Measure_3_0_Namespace;

//
// Help is stored in a "wiki" style text file.  This text file should be copied
// to $HOUDINI_PATH/help/nodes/sop/FeE.txt
//
// See the sample_install.sh file for an example.
//

/// This is the internal name of the SOP type.
/// It isn't allowed to be the same as any other SOP's type name.
const UT_StringHolder SOP_FeE_Measure_3_0::theSOPTypeName("FeE::measure::3.0"_sh);

/// newSopOperator is the hook that Houdini grabs from this dll
/// and invokes to register the SOP.  In this case, we add ourselves
/// to the specified operator table.
void
newSopOperator(OP_OperatorTable *table)
{
    table->addOperator(new OP_Operator(
        SOP_FeE_Measure_3_0::theSOPTypeName,   // Internal name
        "FeE Measure",                     // UI name
        SOP_FeE_Measure_3_0::myConstructor,    // How to build the SOP
        SOP_FeE_Measure_3_0::buildTemplates(), // My parameters
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
        name    "measureType"
        cppname "MeasureType"
        label   "Measure Type"
        type    ordinal
        default { "area" }
        menu {
            "area"       "Area"
            "perimeter"  "Perimeter"
        }
    }
    parm {
        name    "measureAttribName"
        cppname "MeasureAttribName"
        label   "Measure Attribute Name"
        type    string
        default { "area" }
    }
    parm {
        name    "posAttribName"
        cppname "PAttribName"
        label   "Position Attribute Name"
        type    string
        default { "P" }
    }
    parm {
        name    "posAttribClass"
        cppname "PAttribClass"
        label   "Position Attribute Class"
        type    ordinal
        default { "auto" }
        menu {
            "auto"      "Auto"
            "point"     "Point"
            "vertex"    "Vertex"
        }
    }
    parm {
        name    "uniScale"
        cppname "UniScale"
        label   "Uniform Scale"
        type    float
        default { 1 }
        range   { -10! 10 }
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
SOP_FeE_Measure_3_0::buildTemplates()
{
    static PRM_TemplateBuilder templ("SOP_FeE_Measure_3_0.C"_sh, theDsFile);
    return templ.templates();
}


//class SOP_FeE_Measure_3_0Cache : public SOP_NodeCache
//{
//public:
//    SOP_FeE_Measure_3_0Cache() : SOP_NodeCache(),
//        myPrevOutputDetailID(-1)
//    {}
//    ~SOP_FeE_Measure_3_0Cache() override {}
//
//    int64 myPrevOutputDetailID;
//};


class SOP_FeE_Measure_3_0Verb : public SOP_NodeVerb
{
public:
    SOP_FeE_Measure_3_0Verb() {}
    virtual ~SOP_FeE_Measure_3_0Verb() {}

    virtual SOP_NodeParms *allocParms() const { return new SOP_FeE_Measure_3_0Parms(); }
    //virtual SOP_NodeCache* allocCache() const { return new SOP_FeE_Measure_3_0Cache(); }
    virtual UT_StringHolder name() const { return SOP_FeE_Measure_3_0::theSOPTypeName; }

    virtual CookMode cookMode(const SOP_NodeParms *parms) const { return COOK_GENERIC; }

    virtual void cook(const CookParms &cookparms) const;

    /// This static data member automatically registers
    /// this verb class at library ldir0d time.
    static const SOP_NodeVerb::Register<SOP_FeE_Measure_3_0Verb> theVerb;
};

// The static member variable definition has to be outside the class definition.
// The declaration is inside the class.
const SOP_NodeVerb::Register<SOP_FeE_Measure_3_0Verb> SOP_FeE_Measure_3_0Verb::theVerb;

const SOP_NodeVerb *
SOP_FeE_Measure_3_0::cookVerb() const 
{ 
    return SOP_FeE_Measure_3_0Verb::theVerb.get();
}




static GA_AttributeOwner
sopAttribOwner(SOP_FeE_Measure_3_0Parms::PAttribClass attribClass)
{
    using namespace SOP_FeE_Measure_3_0Enums;
    switch (attribClass)
    {
    case PAttribClass::AUTO:      return GA_ATTRIB_DETAIL;     break;//not detail but means Auto
    case PAttribClass::POINT:     return GA_ATTRIB_POINT;      break;
    case PAttribClass::VERTEX:    return GA_ATTRIB_VERTEX;     break;
    }
    UT_ASSERT_MSG(0, "Unhandled Geo0 Class type!");
    return GA_ATTRIB_INVALID;
}

static GA_GroupType
sopGroupType(SOP_FeE_Measure_3_0Parms::GroupType parmgrouptype)
{
    using namespace SOP_FeE_Measure_3_0Enums;
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
SOP_FeE_Measure_3_0Verb::cook(const SOP_NodeVerb::CookParms &cookparms) const
{
    auto &&sopparms = cookparms.parms<SOP_FeE_Measure_3_0Parms>();
    GEO_Detail* outGeo0 = cookparms.gdh().gdpNC();
    //auto sopcache = (SOP_FeE_Measure_3_0Cache*)cookparms.cache();

    const GEO_Detail* const inGeo0 = cookparms.inputGeo(0);

    outGeo0->replaceWith(*inGeo0);

    //outGeo0 = sopNodeProcess(*inGeo0);

    const UT_StringHolder& geo0AttribName = sopparms.getPAttribName();
    if (!geo0AttribName.isstring())
        return;

    const UT_StringHolder& measureAttribName = sopparms.getMeasureAttribName();
    if (!measureAttribName.isstring())
        return;


    UT_AutoInterrupt boss("Processing");
    if (boss.wasInterrupted())
        return;


    const GA_GroupType groupType = sopGroupType(sopparms.getGroupType());
    GOP_Manager gop;
    const GA_Group* geo0Group = GA_FeE_Group::parseGroupDetached(cookparms, outGeo0, groupType, sopparms.getGroup(), gop);
    if (geo0Group && geo0Group->isEmpty())
        return;

    const GA_GroupType geo0finalGroupType = geo0Group ? geo0Group->classType() : GA_GROUP_INVALID;




    const SOP_FeE_Measure_3_0Parms::MeasureType measureType = sopparms.getMeasureType();

    const GA_AttributeOwner geo0AttribClass = sopAttribOwner(sopparms.getPAttribClass());
    const fpreal uniScale = sopparms.getUniScale();


    const exint subscribeRatio = sopparms.getSubscribeRatio();
    const exint minGrainSize = sopparms.getMinGrainSize();
    //const exint minGrainSize = pow(2, 8);
    //const exint minGrainSize = pow(2, 4);
    // 
    //const GA_Storage inStorageF = SYSisSame<T, fpreal32>() ? GA_STORE_REAL32 : GA_STORE_REAL64;
    //const GA_Storage inStorageF = GA_STORE_REAL32;
    const GA_Storage inStorageF = GA_FeE_Type::getPreferredStorageF(outGeo0);
    

    //GA_Attribute* measureAttribPtr = outGeo0->addFloatTuple(GA_ATTRIB_PRIMITIVE, measureAttribName, 1, GA_Defaults(0.0), 0, 0, inStorageF);
    //GA_RWHandleT<attribPrecisonF> measureAttribHandle(measureAttribPtr);

    switch (measureType)
    {
    case SOP_FeE_Measure_3_0Enums::MeasureType::AREA:
        GA_FeE_Measure::addAttribPrimArea(     outGeo0, geo0AttribClass, geo0AttribName, measureAttribName, static_cast<const GA_PrimitiveGroup*>(geo0Group), GA_Defaults(-1.0), inStorageF, subscribeRatio, minGrainSize);
        break;
    case SOP_FeE_Measure_3_0Enums::MeasureType::PERIMETER:
        GA_FeE_Measure::addAttribPrimPerimeter(outGeo0, geo0AttribClass, geo0AttribName, measureAttribName, static_cast<const GA_PrimitiveGroup*>(geo0Group), GA_Defaults(-1.0), inStorageF, subscribeRatio, minGrainSize);
        break;
    default:
        cookparms.sopAddError(SOP_ERR_INVALID_SRC, "unsupport parm");
        return;
    }
    /*
    if (geo0AttribName == "P")
    {
        switch (measureType)
        {
        case SOP_FeE_Measure_3_0Enums::MeasureType::AREA:
            GA_FeE_Measure::primArea(outGeo0, measureAttribHandle, static_cast<const GA_PrimitiveGroup*>(geo0Group), subscribeRatio, minGrainSize);
            break;
        case SOP_FeE_Measure_3_0Enums::MeasureType::PERIMETER:
            GA_FeE_Measure::primPerimeter(outGeo0, measureAttribHandle, static_cast<const GA_PrimitiveGroup*>(geo0Group), subscribeRatio, minGrainSize);
            break;
        default:
            cookparms.sopAddError(SOP_ERR_INVALID_SRC, "unsupport parm");
            return;
        }
    }
    else
    {
        GA_AttributeOwner geo0AttribClassFinal;
        GA_Attribute* attribPtr;
        if (!GA_FeE_Attribute::findFloatTuplePointVertex(outGeo0, geo0AttribClass, geo0AttribName, attribPtr, geo0AttribClassFinal))
        {
            cookparms.sopAddError(SOP_ERR_INVALID_SRC, "unsupport pos attrib");
            return;
        }
        GA_RWHandleT<TAttribTypeV> attribHandle(attribPtr);


        switch (measureType)
        {
        case SOP_FeE_Measure_3_0Enums::MeasureType::AREA:
            GA_FeE_Measure::primArea(outGeo0, measureAttribHandle, attribHandle, static_cast<const GA_PrimitiveGroup*>(geo0Group), subscribeRatio, minGrainSize);
            break;
        case SOP_FeE_Measure_3_0Enums::MeasureType::PERIMETER:
            GA_FeE_Measure::primPerimeter(outGeo0, measureAttribHandle, attribHandle, static_cast<const GA_PrimitiveGroup*>(geo0Group), subscribeRatio, minGrainSize);
            break;
        default:
            cookparms.sopAddError(SOP_ERR_INVALID_SRC, "unsupport parm");
            return;
        }
    }
    */


    

}



namespace SOP_FeE_Measure_3_0_Namespace {

} // End SOP_FeE_Measure_3_0_Namespace namespace
