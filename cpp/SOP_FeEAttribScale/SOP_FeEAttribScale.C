
//#define UT_ASSERT_LEVEL 3
#include "SOP_FeEAttribScale.h"


// This is an automatically generated header file based on theDsFile, below,
// to provide SOP_FeEAttribScaleParms, an easy way to access parameter values from
// SOP_FeEAttribScaleVerb::cook with the correct type.
#include "SOP_FeEAttribScale.proto.h"

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



using namespace SOP_FeEAttribScale_Namespace;

//
// Help is stored in a "wiki" style text file.  This text file should be copied
// to $HOUDINI_PATH/help/nodes/sop/FeE.txt
//
// See the sample_install.sh file for an example.
//

/// This is the internal name of the SOP type.
/// It isn't allowed to be the same as any other SOP's type name.
const UT_StringHolder SOP_FeEAttribScale::theSOPTypeName("FeE::attribScale::1.0"_sh);

/// newSopOperator is the hook that Houdini grabs from this dll
/// and invokes to register the SOP.  In this case, we add ourselves
/// to the specified operator table.
void
newSopOperator(OP_OperatorTable *table)
{
    table->addOperator(new OP_Operator(
        SOP_FeEAttribScale::theSOPTypeName,   // Internal name
        "FeE Attribute Scale",     // UI name
        SOP_FeEAttribScale::myConstructor,    // How to build the SOP
        SOP_FeEAttribScale::buildTemplates(), // My parameters
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
    }
}
)THEDSFILE";

PRM_Template*
SOP_FeEAttribScale::buildTemplates()
{
    static PRM_TemplateBuilder templ("SOP_FeEAttribScale.C"_sh, theDsFile);
    return templ.templates();
}

class SOP_FeEAttribScaleVerb : public SOP_NodeVerb
{
public:
    SOP_FeEAttribScaleVerb() {}
    virtual ~SOP_FeEAttribScaleVerb() {}

    virtual SOP_NodeParms *allocParms() const { return new SOP_FeEAttribScaleParms(); }
    virtual UT_StringHolder name() const { return SOP_FeEAttribScale::theSOPTypeName; }

    virtual CookMode cookMode(const SOP_NodeParms *parms) const { return COOK_GENERIC; }

    virtual void cook(const CookParms &cookparms) const;
    
    /// This static data member automatically registers
    /// this verb class at library load time.
    static const SOP_NodeVerb::Register<SOP_FeEAttribScaleVerb> theVerb;
};

// The static member variable definition has to be outside the class definition.
// The declaration is inside the class.
const SOP_NodeVerb::Register<SOP_FeEAttribScaleVerb> SOP_FeEAttribScaleVerb::theVerb;

const SOP_NodeVerb *
SOP_FeEAttribScale::cookVerb() const 
{ 
    return SOP_FeEAttribScaleVerb::theVerb.get();
}




static GA_AttributeOwner
sopAttribOwner(SOP_FeEAttribScaleParms::AttribClass attribClass)
{
    using namespace SOP_FeEAttribScaleEnums;
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
sopGroupType(SOP_FeEAttribScaleParms::GroupType parmgrouptype)
{
    using namespace SOP_FeEAttribScaleEnums;
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
SOP_FeEAttribScaleVerb::cook(const SOP_NodeVerb::CookParms &cookparms) const
{
    auto &&sopparms = cookparms.parms<SOP_FeEAttribScaleParms>();
    GU_Detail* outGeo0 = cookparms.gdh().gdpNC();

    const GEO_Detail* const inGeo0 = cookparms.inputGeo(0);

    outGeo0->replaceWith(*inGeo0);

    fpreal uniScale = sopparms.getUniScale();

    const UT_StringHolder& geo0AttribNames = sopparms.getAttribNames();
    UT_StringHolder geo0AttribNameSub = geo0AttribNames;

    if (!geo0AttribNames.isstring())
        return;


    // outGeo0->clearAndDestroy();

    GOP_Manager gop;

    GA_AttributeOwner geo0AttribClass = sopAttribOwner(sopparms.getAttribClass());

    GA_Attribute* attribPtr = outGeo0->findFloatTuple(geo0AttribClass, GA_SCOPE_PUBLIC, geo0AttribNameSub);

    if (!attribPtr)
        return;

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







    /*
    switch (geo0AttribClass)
    {
    case GA_ATTRIB_PRIM:




        break;
    case GA_ATTRIB_POINT:



        break;
    case GA_ATTRIB_VERTEX:




        break;
    default:
        UT_ASSERT_MSG(0, "Unhandled outGeo0 class type!");
    }
    */




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

    const int minGrainSize = pow(2, 13);

    switch (geo0AttribClass)
    {
    case GA_ATTRIB_PRIMITIVE:
    {
        const GA_PrimitiveGroup* geo0PromotedGroup = nullptr;
        GA_PrimitiveGroupUPtr groupDeleter;
        if (geo0Group)
        {
            if (geo0finalGroupType == GA_GROUP_PRIMITIVE)
                geo0PromotedGroup = static_cast<const GA_PrimitiveGroup*>(geo0Group);
            else if (geo0finalGroupType == GA_GROUP_VERTEX || geo0finalGroupType == GA_GROUP_POINT || geo0finalGroupType == GA_GROUP_EDGE)
            {
                GA_PrimitiveGroup* newDetachedGroup = new GA_PrimitiveGroup(*outGeo0);
                groupDeleter.reset(newDetachedGroup);
                newDetachedGroup->combine(geo0Group);
                geo0PromotedGroup = newDetachedGroup;
            }
        }

        const GA_SplittableRange geo0SplittableRange(outGeo0->getPrimitiveRange(geo0PromotedGroup));
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


#if 0
        if (boss.wasInterrupted())
            return;
        GA_Offset start;
        GA_Offset end;
        for (GA_Iterator it(outGeo0->getPointRange(geo0PromotedGroup)); it.blockAdvance(start, end); )
        {
            for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
            {
                TAttribType attribValue = attribHandle.get(elemoff);
                attribValue.normalize();
                attribValue *= uniScale;
                attribHandle.set(elemoff, attribValue);
            }
        }
#else
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
#endif
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



namespace SOP_FeEAttribScale_Namespace {

} // End SOP_FeEAttribScale_Namespace namespace
