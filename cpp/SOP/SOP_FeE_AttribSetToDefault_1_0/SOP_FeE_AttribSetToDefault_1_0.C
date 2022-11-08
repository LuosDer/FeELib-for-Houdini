
//#define UT_ASSERT_LEVEL 3
#include "SOP_FeE_AttribSetToDefault_1_0.h"


// This is an automatically generated header file based on theDsFile, below,
// to provide SOP_FeE_AttribSetToDefault_1_0Parms, an easy way to access parameter values from
// SOP_FeE_AttribSetToDefault_1_0Verb::cook with the correct type.
#include "SOP_FeE_AttribSetToDefault_1_0.proto.h"

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
#include <GU/GU_Promote.h>
#include <GEO/GEO_SplitPoints.h>

#include "GU_FeE/GU_FeE_Attribute.h"


using namespace SOP_FeE_AttribSetToDefault_1_0_Namespace;

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
const UT_StringHolder SOP_FeE_AttribSetToDefault_1_0::theSOPTypeName("FeE::attribSetToDefault::1.0"_sh);

/// newSopOperator is the hook that Houdini grabs from this dll
/// and invokes to register the SOP.  In this case, we add ourselves
/// to the specified operator table.
void
newSopOperator(OP_OperatorTable *table)
{
    table->addOperator(new OP_Operator(
        SOP_FeE_AttribSetToDefault_1_0::theSOPTypeName,   // Internal name
        "FeE Attribute Set to Default",                     // UI name
        SOP_FeE_AttribSetToDefault_1_0::myConstructor,    // How to build the SOP
        SOP_FeE_AttribSetToDefault_1_0::buildTemplates(), // My parameters
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
        name    "attribClass"
        cppname "AttribClass"
        label   "Attribute Class"
        type    ordinal
        default { "point" }
        menu {
            "prim"      "Primitive"
            "point"     "Point"
            "vertex"    "Vertex"
            "detail"    "Detail"
        }
    }
    parm {
        name    "attribNames"
        cppname "AttribNames"
        label   "Attrib Names"
        type    string
        default { "N" }
    }
    parm {
        name    "uniScale"
        cppname "UniScale"
        label   "Uniform Scale"
        type    float
        default { 1 }
        range   { 0! 10 }
    }
    parm {
        name    "subscribeRatio"
        cppname "SubscribeRatio"
        label   "Subscribe Ratio"
        type    integer
        default { 16 }
        range   { 0! 256 }
    }
}
)THEDSFILE";






PRM_Template*
SOP_FeE_AttribSetToDefault_1_0::buildTemplates()
{
    static PRM_TemplateBuilder templ("SOP_FeE_AttribSetToDefault_1_0.C"_sh, theDsFile);
    return templ.templates();
}


//class SOP_FeE_AttribSetToDefault_1_0Cache : public SOP_NodeCache
//{
//public:
//    SOP_FeE_AttribSetToDefault_1_0Cache() : SOP_NodeCache(),
//        myPrevOutputDetailID(-1)
//    {}
//    ~SOP_FeE_AttribSetToDefault_1_0Cache() override {}
//
//    int64 myPrevOutputDetailID;
//};


class SOP_FeE_AttribSetToDefault_1_0Verb : public SOP_NodeVerb
{
public:
    SOP_FeE_AttribSetToDefault_1_0Verb() {}
    virtual ~SOP_FeE_AttribSetToDefault_1_0Verb() {}

    virtual SOP_NodeParms *allocParms() const { return new SOP_FeE_AttribSetToDefault_1_0Parms(); }
    //virtual SOP_NodeCache* allocCache() const { return new SOP_FeE_AttribSetToDefault_1_0Cache(); }
    virtual UT_StringHolder name() const { return SOP_FeE_AttribSetToDefault_1_0::theSOPTypeName; }

    virtual CookMode cookMode(const SOP_NodeParms *parms) const { return COOK_GENERIC; }

    virtual void cook(const CookParms &cookparms) const;

    /// This static data member automatically registers
    /// this verb class at library ldir0d time.
    static const SOP_NodeVerb::Register<SOP_FeE_AttribSetToDefault_1_0Verb> theVerb;
};

// The static member variable definition has to be outside the class definition.
// The declaration is inside the class.
const SOP_NodeVerb::Register<SOP_FeE_AttribSetToDefault_1_0Verb> SOP_FeE_AttribSetToDefault_1_0Verb::theVerb;

const SOP_NodeVerb *
SOP_FeE_AttribSetToDefault_1_0::cookVerb() const 
{ 
    return SOP_FeE_AttribSetToDefault_1_0Verb::theVerb.get();
}



static GA_AttributeOwner
sopAttribOwner(SOP_FeE_AttribSetToDefault_1_0Parms::AttribClass attribClass)
{
    using namespace SOP_FeE_AttribSetToDefault_1_0Enums;
    switch (attribClass)
    {
    case AttribClass::PRIM:      return GA_ATTRIB_PRIMITIVE;  break;
    case AttribClass::POINT:     return GA_ATTRIB_POINT;      break;
    case AttribClass::VERTEX:    return GA_ATTRIB_VERTEX;     break;
    case AttribClass::DETAIL:    return GA_ATTRIB_DETAIL;     break;
    }
    UT_ASSERT_MSG(0, "Unhandled Geo0 Class type!");
    return GA_ATTRIB_INVALID;
}

static GA_GroupType
sopGroupType(SOP_FeE_AttribSetToDefault_1_0Parms::GroupType parmgrouptype)
{
    using namespace SOP_FeE_AttribSetToDefault_1_0Enums;
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
SOP_FeE_AttribSetToDefault_1_0Verb::cook(const SOP_NodeVerb::CookParms &cookparms) const
{
    auto &&sopparms = cookparms.parms<SOP_FeE_AttribSetToDefault_1_0Parms>();
    GU_Detail* outGeo0 = cookparms.gdh().gdpNC();
    //auto sopcache = (SOP_FeE_AttribSetToDefault_1_0Cache*)cookparms.cache();

    const GEO_Detail* const inGeo0 = cookparms.inputGeo(0);

    outGeo0->replaceWith(*inGeo0);
    // outGeo0->clearAndDestroy();

    //outGeo0 = sopNodeProcess(*inGeo0);


    const UT_StringHolder& geo0AttribNames = sopparms.getAttribNames();
    if (!geo0AttribNames.isstring())
        return;



    GOP_Manager gop;
    //const attribPrecisonF uniScale = sopparms.getUniScale();
    const GA_AttributeOwner geo0AttribClass = sopAttribOwner(sopparms.getAttribClass());
    const int minGrainSize = pow(2, 4);
    const exint subscribeRatio = sopparms.getSubscribeRatio();


    //const GA_Storage fpreal_storage = SYSisSame<T, fpreal32>() ? GA_STORE_REAL32 : GA_STORE_REAL64;
    const GA_Storage fpreal_storage = GA_STORE_REAL32;


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
    //notifyGroupParmListeners(cookparms.getNode(), 0, 1, outGeo0, geo0Group);

    if (geo0Group && geo0Group->isEmpty())
        return;

    GA_GroupType geo0finalGroupType;
    if (geo0Group)
        geo0finalGroupType = geo0Group->classType();
    else
        geo0finalGroupType = GA_GROUP_INVALID;






    UT_StringHolder geo0AttribNameSub = geo0AttribNames;

    GA_AttributeOwner geo0AttribClassFinal;
    GA_Attribute* attribPtr;
    if (geo0AttribClass == GA_ATTRIB_DETAIL)//detail means Auto
    {
        attribPtr = outGeo0->findFloatTuple(GA_ATTRIB_VERTEX, GA_SCOPE_PUBLIC, geo0AttribNameSub);
        if (attribPtr)
            geo0AttribClassFinal = GA_ATTRIB_VERTEX;
        else
        {
            attribPtr = outGeo0->findFloatTuple(GA_ATTRIB_POINT, GA_SCOPE_PUBLIC, geo0AttribNameSub);
            if (attribPtr)
                geo0AttribClassFinal = GA_ATTRIB_POINT;
            else
                return;
            //continue;
        }
    }
    else
    {
        attribPtr = outGeo0->findFloatTuple(geo0AttribClass, GA_SCOPE_PUBLIC, geo0AttribNameSub);
        if (!attribPtr)
        {
            return;
            //continue;
        }
        geo0AttribClassFinal = geo0AttribClass;
    }
    GA_RWHandleT<TAttribTypeV> attribHandle(attribPtr);


    GU_FeE_Attribute::setToDefault(outGeo0, attribHandle, geo0Group);




}



namespace SOP_FeE_AttribSetToDefault_1_0_Namespace {

} // End SOP_FeE_AttribSetToDefault_1_0_Namespace namespace
