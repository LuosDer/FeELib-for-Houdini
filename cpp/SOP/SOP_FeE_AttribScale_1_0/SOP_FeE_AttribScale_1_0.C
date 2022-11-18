
//#define UT_ASSERT_LEVEL 3
#include "SOP_FeE_AttribScale_1_0.h"


// This is an automatically generated header file based on theDsFile, below,
// to provide SOP_FeE_AttribScale_1_0Parms, an easy way to access parameter values from
// SOP_FeE_AttribScale_1_0Verb::cook with the correct type.
#include "SOP_FeE_AttribScale_1_0.proto.h"

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
#include <GA/GA_PageHandle.h>
#include <GA/GA_PageIterator.h>


#include <GA_FeE/GA_FeE_Group.h>


using namespace SOP_FeE_AttribScale_1_0_Namespace;

//
// Help is stored in a "wiki" style text file.  This text file should be copied
// to $HOUDINI_PATH/help/nodes/sop/FeE.txt
//
// See the sample_install.sh file for an example.
//

/// This is the internal name of the SOP type.
/// It isn't allowed to be the same as any other SOP's type name.
const UT_StringHolder SOP_FeE_AttribScale_1_0::theSOPTypeName("FeE::attribScale::1.0"_sh);

/// newSopOperator is the hook that Houdini grabs from this dll
/// and invokes to register the SOP.  In this case, we add ourselves
/// to the specified operator table.
void
newSopOperator(OP_OperatorTable *table)
{
    table->addOperator(new OP_Operator(
        SOP_FeE_AttribScale_1_0::theSOPTypeName,   // Internal name
        "FeE Attribute Scale",     // UI name
        SOP_FeE_AttribScale_1_0::myConstructor,    // How to build the SOP
        SOP_FeE_AttribScale_1_0::buildTemplates(), // My parameters
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
        name    "normalize"
        cppname "Normalize"
        label   "Normalize"
        type    toggle
        default { "1" }
    }
    parm {
        name    "uniScale"
        cppname "UniScale"
        label   "Uniform Scale"
        type    float
        default { 1 }
        range   { -100 100 }
    }


    parm {
        name    "kernel"
        cppname "Kernel"
        label   "Kernel"
        type    integer
        default { 0 }
        range   { 0! 2 }
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
SOP_FeE_AttribScale_1_0::buildTemplates()
{
    static PRM_TemplateBuilder templ("SOP_FeE_AttribScale_1_0.C"_sh, theDsFile);
    return templ.templates();
}

class SOP_FeE_AttribScale_1_0Verb : public SOP_NodeVerb
{
public:
    SOP_FeE_AttribScale_1_0Verb() {}
    virtual ~SOP_FeE_AttribScale_1_0Verb() {}

    virtual SOP_NodeParms *allocParms() const { return new SOP_FeE_AttribScale_1_0Parms(); }
    virtual UT_StringHolder name() const { return SOP_FeE_AttribScale_1_0::theSOPTypeName; }

    virtual CookMode cookMode(const SOP_NodeParms *parms) const { return COOK_GENERIC; }

    virtual void cook(const CookParms &cookparms) const;
    
    /// This static data member automatically registers
    /// this verb class at library load time.
    static const SOP_NodeVerb::Register<SOP_FeE_AttribScale_1_0Verb> theVerb;
};

// The static member variable definition has to be outside the class definition.
// The declaration is inside the class.
const SOP_NodeVerb::Register<SOP_FeE_AttribScale_1_0Verb> SOP_FeE_AttribScale_1_0Verb::theVerb;

const SOP_NodeVerb *
SOP_FeE_AttribScale_1_0::cookVerb() const 
{ 
    return SOP_FeE_AttribScale_1_0Verb::theVerb.get();
}




static GA_AttributeOwner
sopAttribOwner(SOP_FeE_AttribScale_1_0Parms::AttribClass attribClass)
{
    using namespace SOP_FeE_AttribScale_1_0Enums;
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
sopGroupType(SOP_FeE_AttribScale_1_0Parms::GroupType parmgrouptype)
{
    using namespace SOP_FeE_AttribScale_1_0Enums;
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
SOP_FeE_AttribScale_1_0Verb::cook(const SOP_NodeVerb::CookParms &cookparms) const
{
    auto &&sopparms = cookparms.parms<SOP_FeE_AttribScale_1_0Parms>();
    GU_Detail* outGeo0 = cookparms.gdh().gdpNC();

    const GEO_Detail* const inGeo0 = cookparms.inputGeo(0);

    outGeo0->replaceWith(*inGeo0);


    const UT_StringHolder& geo0AttribNames = sopparms.getAttribNames();
    if (!geo0AttribNames.isstring())
        return;


    const fpreal& uniScale = sopparms.getUniScale();
    const bool& doNormalize = sopparms.getNormalize();

    if (!doNormalize && uniScale==1)
        return;



    const GA_GroupType& groupType = sopGroupType(sopparms.getGroupType());
    const GA_ElementGroup* geo0Group = GA_FeE_Group::parseGroupDetached(cookparms, outGeo0, groupType, sopparms.getGroup());
    if (geo0Group && geo0Group->isEmpty())
        return;
    //notifyGroupParmListeners(cookparms.getNode(), 0, 1, outGeo0, geo0Group);



    const GA_AttributeOwner& geo0AttribClass = sopAttribOwner(sopparms.getAttribClass());

    const exint& kernel = sopparms.getKernel();
    const exint& subscribeRatio = sopparms.getSubscribeRatio();
    const exint& minGrainSize = sopparms.getMinGrainSize();
    //const exint minGrainSize = pow(2, 13);


    const GA_Range geo0Range = GA_FeE_Group::groupPromoteRange(outGeo0, geo0Group, geo0AttribClass);
    //const GA_SplittableRange geo0SplittableRange(GA_FeE_Group::groupPromoteRange(outGeo0, geo0Group, geo0AttribClass));
    const GA_SplittableRange geo0SplittableRange = GA_FeE_Group::groupPromoteSplittableRange(outGeo0, geo0Group, geo0AttribClass);





    const UT_StringHolder& geo0AttribNameSub = geo0AttribNames;



    GA_Attribute* attribPtr = outGeo0->findFloatTuple(geo0AttribClass, GA_SCOPE_PUBLIC, geo0AttribNameSub, 3, 3);

    if (!attribPtr)
        return;

    const int attribSize = attribPtr->getTupleSize();

    //template <typename T>
    switch (attribSize)
    {
    case 3:
        break;
    default:
        UT_ASSERT_MSG(0, "Unhandled outGeo0 Attrib Size");
    }

    switch (kernel)
    {
    case 0:
    {
        //const GA_RWAttributeRef attrib_rwRef(attribPtr);
        UTparallelFor(geo0SplittableRange, [&attribPtr, &doNormalize, &uniScale](const GA_SplittableRange& r)
        {
            GA_PageHandleV<UT_Vector3F>::RWType attrib_ph(attribPtr);
            //GA_RWPageHandleV3 attrib_ph(attrib_rwRef.getAttribute());

            for (GA_PageIterator pit = r.beginPages(); !pit.atEnd(); ++pit)
            {
                GA_Offset start, end;
                for (GA_Iterator it(pit.begin()); it.blockAdvance(start, end); )
                {
                    attrib_ph.setPage(start);
                    for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                    {
                        if (doNormalize)
                            attrib_ph.value(elemoff).normalize();
                        attrib_ph.value(elemoff) *= uniScale;
                    }
                }
            }
        }, subscribeRatio, minGrainSize);
    }
    break;
    case 1:
    {
        GA_RWHandleT<UT_Vector3F> attribHandle(attribPtr);
        UTparallelFor(geo0SplittableRange, [&attribHandle, doNormalize, uniScale](const GA_SplittableRange& r)
        {
            GA_Offset start, end;
            for (GA_Iterator it(r); it.blockAdvance(start, end); )
            {
                for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                {
                    UT_Vector3F attribValue = attribHandle.get(elemoff);
                    if (doNormalize)
                        attribValue.normalize();
                    attribValue *= uniScale;
                    attribHandle.set(elemoff, attribValue);
                }
            }
        }, subscribeRatio, minGrainSize);
    }
    break;
    case 2:
    {
        //const GA_RWAttributeRef attrib_rwRef(attribPtr);

        //GAparallelForEachPage(geo0Range, true, [&attrib_rwRef, &doNormalize, &uniScale](GA_PageIterator pit)
        //{
        //    GA_RWPageHandleV3 attrib_ph(attrib_rwRef.getAttribute());

        //    GAforEachPageBlock(pit, [&](GA_Offset start, GA_Offset end)
        //    {
        //        attrib_ph.setPage(start);
        //        for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
        //        {
        //            //UT_Vector3F attribValue = attribHandle.get(elemoff);
        //            if (doNormalize)
        //                attrib_ph.value(elemoff).normalize();
        //            //UT_Vector3 N = v_ph.get(i);
        //            //N.normalize();
        //            //v_ph.set(i, N);
        //        }
        //    }
        //});
    }
    break;
    }







}



namespace SOP_FeE_AttribScale_1_0_Namespace {

} // End SOP_FeE_AttribScale_1_0_Namespace namespace
