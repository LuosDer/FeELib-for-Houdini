
//#define UT_ASSERT_LEVEL 3
#include "SOP_FeE_GroupClosedSharedPoint_2_0.h"


#include "SOP_FeE_GroupClosedSharedPoint_2_0.proto.h"

#include "GA/GA_Detail.h"
#include "PRM/PRM_TemplateBuilder.h"
#include "UT/UT_Interrupt.h"
#include "UT/UT_DSOVersion.h"




#include "GFE/GFE_GroupClosedSharedPoint.h"




using namespace SOP_FeE_GroupClosedSharedPoint_2_0_Namespace;


static const char *theDsFile = R"THEDSFILE(
{
    name        parameters
    parm {
        name    "primGroup"
        cppname "PrimGroup"
        label   "Prim Group"
        type    string
        default { "" }
        parmtag { "script_action" "import soputils\nkwargs['geometrytype'] = (hou.geometryType.Primitives,)\nkwargs['inputindex'] = 0\nsoputils.selectGroupParm(kwargs)" }
        parmtag { "script_action_help" "Select geometry from an available viewport.\nShift-click to turn on Select Groups." }
        parmtag { "script_action_icon" "BUTTONS_reselect" }
    }
    parm {
        name    "curveUVMethod"
        cppname "CurveUVMethod"
        label   "Curve UV Method"
        type    ordinal
        default { "arcLength" }
        menu {
            "worldArcLength"  "World Arc Length"
            "worldAverage"    "World Average"
            "localArcLength"  "Local Arc Length"
            "localAverage"    "Local Average"
        }
    }
    parm {
        name    "uvAttrib"
        cppname "UVAttrib"
        label   "UV Attribute"
        type    string
        default { "uv" }
    }
    parm {
        name    "uvClass"
        cppname "UVClass"
        label   "UV Class"
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
)THEDSFILE";

PRM_Template*
SOP_FeE_GroupClosedSharedPoint_2_0::buildTemplates()
{
    static PRM_TemplateBuilder templ("SOP_FeE_GroupClosedSharedPoint_2_0.C"_sh, theDsFile);
    if (templ.justBuilt())
    {
        templ.setChoiceListPtr("uvAttrib"_sh, &SOP_Node::allTextureCoordMenu);
        templ.setChoiceListPtr("primGroup"_sh, &SOP_Node::primGroupMenu);
        
    }
    return templ.templates();
}

const UT_StringHolder SOP_FeE_GroupClosedSharedPoint_2_0::theSOPTypeName("FeE::groupClosedSharedPoint::2.0"_sh);

void
newSopOperator(OP_OperatorTable* table)
{
    OP_Operator* newOp = new OP_Operator(
        SOP_FeE_GroupClosedSharedPoint_2_0::theSOPTypeName,
        "FeE Group Closed Shared Point",
        SOP_FeE_GroupClosedSharedPoint_2_0::myConstructor,
        SOP_FeE_GroupClosedSharedPoint_2_0::buildTemplates(),
        1,
        1,
        nullptr,
        OP_FLAG_GENERATOR,
        nullptr,
        1,
        "Five elements Elf/Data/Topology");

    newOp->setIconName("SOP_ends");
    table->addOperator(newOp);

}





class SOP_FeE_GroupClosedSharedPoint_2_0Verb : public SOP_NodeVerb
{
public:
    SOP_FeE_GroupClosedSharedPoint_2_0Verb() {}
    virtual ~SOP_FeE_GroupClosedSharedPoint_2_0Verb() {}

    virtual SOP_NodeParms *allocParms() const { return new SOP_FeE_GroupClosedSharedPoint_2_0Parms(); }
    virtual UT_StringHolder name() const { return SOP_FeE_GroupClosedSharedPoint_2_0::theSOPTypeName; }

    virtual CookMode cookMode(const SOP_NodeParms *parms) const { return COOK_GENERIC; }

    virtual void cook(const CookParms &cookparms) const;
    
    /// This static data member automatically registers
    /// this verb class at library load time.
    static const SOP_NodeVerb::Register<SOP_FeE_GroupClosedSharedPoint_2_0Verb> theVerb;
};

// The static member variable definition has to be outside the class definition.
// The declaration is inside the class.
const SOP_NodeVerb::Register<SOP_FeE_GroupClosedSharedPoint_2_0Verb> SOP_FeE_GroupClosedSharedPoint_2_0Verb::theVerb;

const SOP_NodeVerb *
SOP_FeE_GroupClosedSharedPoint_2_0::cookVerb() const 
{ 
    return SOP_FeE_GroupClosedSharedPoint_2_0Verb::theVerb.get();
}




static GA_AttributeOwner
sopAttribOwner(SOP_FeE_GroupClosedSharedPoint_2_0Parms::UVClass attribClass)
{
    using namespace SOP_FeE_GroupClosedSharedPoint_2_0Enums;
    switch (attribClass)
    {
    case UVClass::AUTO:      return GA_ATTRIB_INVALID;    break;//not detail but means Auto
    case UVClass::POINT:     return GA_ATTRIB_POINT;      break;
    case UVClass::VERTEX:    return GA_ATTRIB_VERTEX;     break;
    }
    UT_ASSERT_MSG(0, "Unhandled Geo0 Class type!");
    return GA_ATTRIB_INVALID;
}


static GFE_CurveUVMethod
sopCurveUVMethod(SOP_FeE_GroupClosedSharedPoint_2_0Parms::CurveUVMethod curveUVMethod)
{
    using namespace SOP_FeE_GroupClosedSharedPoint_2_0Enums;
    switch (curveUVMethod)
    {
    case CurveUVMethod::WORLDARCLENGTH:     return GFE_CurveUVMethod::WorldArcLength;    break;
    case CurveUVMethod::WORLDAVERAGE:       return GFE_CurveUVMethod::WorldAverage;      break;
    case CurveUVMethod::LOCALARCLENGTH:     return GFE_CurveUVMethod::LocalArcLength;    break;
    case CurveUVMethod::LOCALAVERAGE:       return GFE_CurveUVMethod::LocalAverage;      break;
    }
    UT_ASSERT_MSG(0, "Unhandled CurveUVMethod!");
    return GFE_CurveUVMethod::WorldArcLength;
}



void
SOP_FeE_GroupClosedSharedPoint_2_0Verb::cook(const SOP_NodeVerb::CookParms &cookparms) const
{
    auto&& sopparms = cookparms.parms<SOP_FeE_GroupClosedSharedPoint_2_0Parms>();
    GA_Detail* const outGeo0 = cookparms.gdh().gdpNC();
    //auto sopcache = (SOP_FeE_GroupClosedSharedPoint_2_0Cache*)cookparms.cache();

    const GA_Detail* const inGeo0 = cookparms.inputGeo(0);

    outGeo0->replaceWith(*inGeo0);


    //const UT_StringHolder& primGroupName = sopparms.getPrimGroup();

    const GA_AttributeOwner uvAttribClass = sopAttribOwner(sopparms.getUVClass());
    const UT_StringHolder& uvAttribName = sopparms.getUVAttrib();

    const GFE_CurveUVMethod curveUVMethod = sopCurveUVMethod(sopparms.getCurveUVMethod());
        
    
    const exint subscribeRatio = sopparms.getSubscribeRatio();
    const exint minGrainSize = sopparms.getMinGrainSize();


    //const GA_Storage inStorageI = GFE_Type::getPreferredStorageI(outGeo0);

    UT_AutoInterrupt boss("Processing");
    if (boss.wasInterrupted())
        return;
    




    GFE_CurveUV curveUV(cookparms, outGeo0);

    curveUV.setComputeParm(curveUVMethod, subscribeRatio, minGrainSize);


    curveUV.setGroup(sopparms.getPrimGroup());
    curveUV.findOrCreateUV(uvAttribClass, GA_STORE_INVALID, false, uvAttribName, 3);

    curveUV.computeAndBumpDataId();


    //GFE_CurveUV_Namespace::curveUV(cookparms, outGeo0, primGroupName,
    //    GA_STORE_INVALID, uvAttribClass, uvAttribName, curveUVMethod,
    //    subscribeRatio, minGrainSize);
}



namespace SOP_FeE_GroupClosedSharedPoint_2_0_Namespace {

} // End SOP_FeE_GroupClosedSharedPoint_2_0_Namespace namespace
