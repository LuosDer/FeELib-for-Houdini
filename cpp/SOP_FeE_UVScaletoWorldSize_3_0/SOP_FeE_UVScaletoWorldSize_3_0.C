
//#define UT_ASSERT_LEVEL 3
#include "SOP_FeE_UVScaletoWorldSize_3_0.h"


// This is an automatically generated header file based on theDsFile, below,
// to provide SOP_FeE_UVScaletoWorldSize_3_0Parms, an easy way to access parameter values from
// SOP_FeE_UVScaletoWorldSize_3_0Verb::cook with the correct type.
#include "SOP_FeE_UVScaletoWorldSize_3_0.proto.h"

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


#include <UT/UT_UniquePtr.h>
#include <GA/GA_SplittableRange.h>
#include <HOM/HOM_SopNode.h>



using namespace SOP_FeE_UVScaletoWorldSize_3_0_Namespace;

//
// Help is stored in a "wiki" style text file.  This text file should be copied
// to $HOUDINI_PATH/help/nodes/sop/FeE.txt
//
// See the sample_install.sh file for an example.
//

/// This is the internal name of the SOP type.
/// It isn't allowed to be the same as any other SOP's type name.
const UT_StringHolder SOP_FeE_UVScaletoWorldSize_3_0::theSOPTypeName("FeE::attribScale::1.0"_sh);

/// newSopOperator is the hook that Houdini grabs from this dll
/// and invokes to register the SOP.  In this case, we add ourselves
/// to the specified operator table.
void
newSopOperator(OP_OperatorTable *table)
{
    table->addOperator(new OP_Operator(
        SOP_FeE_UVScaletoWorldSize_3_0::theSOPTypeName,   // Internal name
        "FeE Attribute Scale",     // UI name
        SOP_FeE_UVScaletoWorldSize_3_0::myConstructor,    // How to build the SOP
        SOP_FeE_UVScaletoWorldSize_3_0::buildTemplates(), // My parameters
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
        name    "uvAttribClass"
        cppname "UVAttribClass"
        label   "UV Attribute Class"
        type    ordinal
        default { "auto" }
        menu {
            "auto"      "Auto"
            "point"     "Point"
            "vertex"    "Vertex"
        }
    }
    parm {
        name    "uvAttribNames"
        cppname "UVAttribNames"
        label   "UV Attribute Names"
        type    string
        default { "uv" }
    }
    parm {
        name    "computeUVAreaInPiece"
        cppname "ComputeUVAreaInPiece"
        label   "Compute UV Area In Pieces"
        type    toggle
        default { "1" }
    }
    parm {
        name    "doUVScalex"
        cppname "DoUVScalex"
        label   "Do Scale X"
        type    toggle
        default { "1" }
    }
    parm {
        name    "doUVScaley"
        cppname "DoUVScaley"
        label   "Do Scale Y"
        type    toggle
        default { "1" }
    }
    parm {
        name    "doUVScalez"
        cppname "DoUVScalez"
        label   "Do Scale Z"
        type    toggle
        default { "1" }
    }
    parm {
        name    "uvScalex"
        cppname "UVScalex"
        label   "UV Scale X"
        type    float
        default { 1 }
        range   { 0! 10 }
    }
    parm {
        name    "uvScaley"
        cppname "UVScaley"
        label   "UV Scale Y"
        type    float
        default { 1 }
        range   { 0! 10 }
    }
    parm {
        name    "uvScalez"
        cppname "UVScalez"
        label   "UV Scale Z"
        type    float
        default { 1 }
        range   { 0! 10 }
    }
    parm {
        name    "uvScale"
        cppname "UVScale"
        label   "UV Scale"
        type    float
        default { 1 }
        range   { 0! 10 }
    }
}
)THEDSFILE";

PRM_Template*
SOP_FeE_UVScaletoWorldSize_3_0::buildTemplates()
{
    static PRM_TemplateBuilder templ("SOP_FeE_UVScaletoWorldSize_3_0.C"_sh, theDsFile);
    return templ.templates();
}

class SOP_FeE_UVScaletoWorldSize_3_0Verb : public SOP_NodeVerb
{
public:
    SOP_FeE_UVScaletoWorldSize_3_0Verb() {}
    virtual ~SOP_FeE_UVScaletoWorldSize_3_0Verb() {}

    virtual SOP_NodeParms *allocParms() const { return new SOP_FeE_UVScaletoWorldSize_3_0Parms(); }
    virtual UT_StringHolder name() const { return SOP_FeE_UVScaletoWorldSize_3_0::theSOPTypeName; }

    virtual CookMode cookMode(const SOP_NodeParms *parms) const { return COOK_GENERIC; }

    virtual void cook(const CookParms &cookparms) const;
    
    /// This static data member automatically registers
    /// this verb class at library load time.
    static const SOP_NodeVerb::Register<SOP_FeE_UVScaletoWorldSize_3_0Verb> theVerb;
};

// The static member variable definition has to be outside the class definition.
// The declaration is inside the class.
const SOP_NodeVerb::Register<SOP_FeE_UVScaletoWorldSize_3_0Verb> SOP_FeE_UVScaletoWorldSize_3_0Verb::theVerb;

const SOP_NodeVerb *
SOP_FeE_UVScaletoWorldSize_3_0::cookVerb() const 
{ 
    return SOP_FeE_UVScaletoWorldSize_3_0Verb::theVerb.get();
}




static GA_AttributeOwner
sopAttribOwner(SOP_FeE_UVScaletoWorldSize_3_0Parms::UVAttribClass attribClass)
{
    using namespace SOP_FeE_UVScaletoWorldSize_3_0Enums;
    switch (attribClass)
    {
    case UVAttribClass::AUTO:      return GA_ATTRIB_DETAIL;     break;//not detail but means Auto
    case UVAttribClass::POINT:     return GA_ATTRIB_POINT;      break;
    case UVAttribClass::VERTEX:    return GA_ATTRIB_VERTEX;     break;
    }
    UT_ASSERT_MSG(0, "Unhandled Geo0 Class type!");
    return GA_ATTRIB_INVALID;
}

static GA_GroupType
sopGroupType(SOP_FeE_UVScaletoWorldSize_3_0Parms::GroupType parmgrouptype)
{
    using namespace SOP_FeE_UVScaletoWorldSize_3_0Enums;
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
SOP_FeE_UVScaletoWorldSize_3_0Verb::cook(const SOP_NodeVerb::CookParms &cookparms) const
{
    auto &&sopparms = cookparms.parms<SOP_FeE_UVScaletoWorldSize_3_0Parms>();
    GU_Detail* outGeo0 = cookparms.gdh().gdpNC();

    const GEO_Detail* const inGeo0 = cookparms.inputGeo(0);

    outGeo0->replaceWith(*inGeo0);
    // outGeo0->clearAndDestroy();



    const bool doUVScalex = sopparms.getDoUVScalex();
    const bool doUVScaley = sopparms.getDoUVScaley();
    const bool doUVScalez = sopparms.getDoUVScalez();
    if (!doUVScalex && !doUVScaley && !doUVScalez)
        return;


    const UT_StringHolder& geo0AttribNames = sopparms.getUVAttribNames();
    if (!geo0AttribNames.isstring())
        return;



    GOP_Manager gop;
    fpreal uniScale = sopparms.getUVScale();
    GA_AttributeOwner geo0AttribClass = sopAttribOwner(sopparms.getUVAttribClass());
    const int minGrainSize = pow(2, 13);






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



    UT_AutoInterrupt boss("Processing");
    if (boss.wasInterrupted())
        return;







    UT_StringHolder geo0AttribNameSub = geo0AttribNames;

    GA_AttributeOwner geo0AttribClassFinal;
    GA_Attribute* attribPtr;
    if (geo0AttribClass == GA_ATTRIB_DETAIL)//not detail but means Auto
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
            return;
            //continue;
        geo0AttribClassFinal = geo0AttribClass;
    }

    int attribSize = attribPtr->getTupleSize();


    using TAttribType = UT_Vector3F;


    GA_RWHandleT<TAttribType> attribHandle(attribPtr);

    //template <typename T>
    switch (attribSize)
    {
    case 3:
        break;
    default:
        UT_ASSERT_MSG(0, "Unhandled outGeo0 Attrib Size");
    }




    switch (geo0AttribClassFinal)
    {
    case GA_ATTRIB_POINT:
    {
        const GA_PointGroup* geo0PromotedGroup = nullptr;
        GA_PointGroupUPtr groupDeleter;
        if (geo0Group)
        {
            if (geo0finalGroupType == GA_GROUP_POINT)
                geo0PromotedGroup = static_cast<const GA_PointGroup*>(geo0Group);
            else if (geo0finalGroupType == GA_GROUP_VERTEX || geo0finalGroupType == GA_GROUP_PRIMITIVE || geo0finalGroupType == GA_GROUP_EDGE)
            {
                GA_PointGroup* newDetachedGroup = new GA_PointGroup(*outGeo0);
                groupDeleter.reset(newDetachedGroup);
                newDetachedGroup->combine(geo0Group);
                geo0PromotedGroup = newDetachedGroup;
            }
        }

        const GA_SplittableRange geo0SplittableRange(outGeo0->getPointRange(geo0PromotedGroup));
        UTparallelFor(geo0SplittableRange, [&attribHandle, &boss, uniScale](const GA_SplittableRange& r)
            {
                if (boss.wasInterrupted())
                    return;
                GA_Offset start;
                GA_Offset end;
                for (GA_Iterator it(r); it.blockAdvance(start, end); )
                {
                    for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                    {
                        TAttribType attribValue = attribHandle.get(elemoff);
                        attribValue.normalize();
                        attribValue *= uniScale;
                        attribHandle.set(elemoff, attribValue);
                    }
                }
            }, 1, minGrainSize);
    }
    break;
    case GA_ATTRIB_VERTEX:
    {
        const GA_VertexGroup* geo0PromotedGroup = nullptr;
        GA_VertexGroupUPtr groupDeleter;
        if (geo0Group)
        {
            if (geo0finalGroupType == GA_GROUP_VERTEX)
                geo0PromotedGroup = static_cast<const GA_VertexGroup*>(geo0Group);
            else if (geo0finalGroupType == GA_GROUP_PRIMITIVE || geo0finalGroupType == GA_GROUP_POINT || geo0finalGroupType == GA_GROUP_EDGE)
            {
                GA_VertexGroup* newDetachedGroup = new GA_VertexGroup(*outGeo0);
                groupDeleter.reset(newDetachedGroup);
                newDetachedGroup->combine(geo0Group);
                geo0PromotedGroup = newDetachedGroup;
            }
        }

        const GA_SplittableRange geo0SplittableRange(outGeo0->getVertexRange(geo0PromotedGroup));
        UTparallelFor(geo0SplittableRange, [&attribHandle, &boss, uniScale](const GA_SplittableRange& r)
            {
                if (boss.wasInterrupted())
                    return;
                GA_Offset start;
                GA_Offset end;
                for (GA_Iterator it(r); it.blockAdvance(start, end); )
                {
                    for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                    {
                        TAttribType attribValue = attribHandle.get(elemoff);
                        attribValue.normalize();
                        attribValue *= uniScale;
                        attribHandle.set(elemoff, attribValue);
                    }
                }
            }, 1, minGrainSize);
    }

    break;
    default:
        UT_ASSERT_MSG(0, "Unhandled outGeo0 class type!");
    }


    attribHandle->bumpDataId();
    //why bumpDataId() while winding number.c did it


}



namespace SOP_FeE_UVScaletoWorldSize_3_0_Namespace {

} // End SOP_FeE_UVScaletoWorldSize_3_0_Namespace namespace
