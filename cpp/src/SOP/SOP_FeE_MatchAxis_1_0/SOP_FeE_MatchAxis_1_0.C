
//#define UT_ASSERT_LEVEL 3
#include "SOP_FeE_MatchAxis_1_0.h"


#include "SOP_FeE_MatchAxis_1_0.proto.h"

#include "GA/GA_Detail.h"
#include "PRM/PRM_TemplateBuilder.h"
#include "UT/UT_Interrupt.h"
#include "UT/UT_DSOVersion.h"




#include "GA_FeE/GA_FeE_MatchAxis.h"




using namespace SOP_FeE_MatchAxis_1_0_Namespace;


static const char *theDsFile = R"THEDSFILE(
{
    name        parameters
    parm {
        name    "pointGroup"
        cppname "PointGroup"
        label   "Point Group"
        type    string
        default { "" }
        parmtag { "script_action" "import soputils\nkwargs['geometrytype'] = hou.geometryType.Points\nkwargs['inputindex'] = 0\nsoputils.selectGroupParm(kwargs)" }
        parmtag { "script_action_help" "Select geometry from an available viewport." }
        parmtag { "script_action_icon" "BUTTONS_reselect" }
    }
    parm {
        name    "matchAxisAttribClass"
        cppname "MatchAxisAttribClass"
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
        name    "matchAxisAttribName"
        cppname "MatchAxisAttribName"
        label   "Match Axis Attrib Name"
        type    string
        default { "P" }
    }
    parm {
        name    "fromVec"
        cppname "FromVec"
        label   "From Vector"
        type    direction
        size    3
        default { "0" "0" "1" }
        range   { -1 1 }
    }
    parm {
        name    "toVec"
        cppname "ToVec"
        label   "To Vector"
        type    direction
        size    3
        default { "0" "1" "0" }
        range   { -1 1 }
    }
    //parm {
    //    name    "upVec"
    //    cppname "UpVec"
    //    label   "Up Vector"
    //    type    direction
    //    size    3
    //    default { "0" "1" "0" }
    //    range   { -1 1 }
    //}
    parm {
        name    "center"
        cppname "Center"
        label   "Center"
        type    vector
        size    3
        default { "0" "0" "0" }
        disablewhen "{ hasinput(1) == 1 }"
        range   { -1 1 }
    }
    parm {
        name    "bias"
        cppname "Bias"
        label   "Bias"
        type    float
        default { "1" }
        range   { 0! 1! }
    }
    parm {
        name    "sepparm"
        cppname "Sepparm"
        label   "Separator"
        type    separator
        default { "" }
    }
    

    //parm {
    //    name    "subscribeRatio"
    //    cppname "SubscribeRatio"
    //    label   "Subscribe Ratio"
    //    type    integer
    //    default { 64 }
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
SOP_FeE_MatchAxis_1_0::buildTemplates()
{
    static PRM_TemplateBuilder templ("SOP_FeE_MatchAxis_1_0.C"_sh, theDsFile);
    if (templ.justBuilt())
    {
        templ.setChoiceListPtr("pointGroup"_sh, &SOP_Node::edgeGroupMenu);
        
    }
    return templ.templates();
}

const UT_StringHolder SOP_FeE_MatchAxis_1_0::theSOPTypeName("FeE::matchAxis::1.0"_sh);

void
newSopOperator(OP_OperatorTable* table)
{
    OP_Operator* newOp = new OP_Operator(
        SOP_FeE_MatchAxis_1_0::theSOPTypeName,
        "FeE Match Axis",
        SOP_FeE_MatchAxis_1_0::myConstructor,
        SOP_FeE_MatchAxis_1_0::buildTemplates(),
        1,
        3,
        nullptr,
        OP_FLAG_GENERATOR,
        nullptr,
        1,
        "Five elements Elf/Filter/Match");

    newOp->setIconName("SOP_matchaxis");
    table->addOperator(newOp);

}





class SOP_FeE_MatchAxis_1_0Verb : public SOP_NodeVerb
{
public:
    SOP_FeE_MatchAxis_1_0Verb() {}
    virtual ~SOP_FeE_MatchAxis_1_0Verb() {}

    virtual SOP_NodeParms *allocParms() const { return new SOP_FeE_MatchAxis_1_0Parms(); }
    virtual UT_StringHolder name() const { return SOP_FeE_MatchAxis_1_0::theSOPTypeName; }

    virtual CookMode cookMode(const SOP_NodeParms *parms) const { return COOK_GENERIC; }

    virtual void cook(const CookParms &cookparms) const;
    
    /// This static data member automatically registers
    /// this verb class at library load time.
    static const SOP_NodeVerb::Register<SOP_FeE_MatchAxis_1_0Verb> theVerb;
};

// The static member variable definition has to be outside the class definition.
// The declaration is inside the class.
const SOP_NodeVerb::Register<SOP_FeE_MatchAxis_1_0Verb> SOP_FeE_MatchAxis_1_0Verb::theVerb;

const SOP_NodeVerb *
SOP_FeE_MatchAxis_1_0::cookVerb() const 
{ 
    return SOP_FeE_MatchAxis_1_0Verb::theVerb.get();
}






static GA_AttributeOwner
sopAttribOwner(SOP_FeE_MatchAxis_1_0Parms::MatchAxisAttribClass attribClass)
{
    using namespace SOP_FeE_MatchAxis_1_0Enums;
    switch (attribClass)
    {
    case MatchAxisAttribClass::PRIM:      return GA_ATTRIB_PRIMITIVE;  break;
    case MatchAxisAttribClass::POINT:     return GA_ATTRIB_POINT;      break;
    case MatchAxisAttribClass::VERTEX:    return GA_ATTRIB_VERTEX;     break;
    case MatchAxisAttribClass::DETAIL:    return GA_ATTRIB_DETAIL;     break;
    }
    UT_ASSERT_MSG(0, "Unhandled Geo0 Class type!");
    return GA_ATTRIB_INVALID;
}



void
SOP_FeE_MatchAxis_1_0Verb::cook(const SOP_NodeVerb::CookParms &cookparms) const
{
    auto&& sopparms = cookparms.parms<SOP_FeE_MatchAxis_1_0Parms>();
    GA_Detail* const outGeo0 = cookparms.gdh().gdpNC();
    //auto sopcache = (SOP_FeE_MatchAxis_1_0Cache*)cookparms.cache();

    const GA_Detail* const inGeo0 = cookparms.inputGeo(0);

    outGeo0->replaceWith(*inGeo0);

            
    //const exint subscribeRatio = sopparms.getSubscribeRatio();
    //const exint minGrainSize = sopparms.getMinGrainSize();
    const UT_Vector3D fromVec = sopparms.getFromVec();
    const UT_Vector3D toVec = sopparms.getToVec();
    //const UT_Vector3D upVec = sopparms.getUpVec();
    

    //const GA_Storage inStorageI = GA_FeE_Type::getPreferredStorageI(outGeo0);

    UT_AutoInterrupt boss("Processing");
    if (boss.wasInterrupted())
        return;

    const GA_AttributeOwner owner = sopAttribOwner(sopparms.getMatchAxisAttribClass());
    //GA_FeE_MatchAxis::matchAxis(cookparms, outGeo0, sopparms.getPointGroup(), owner, sopparms.getMatchAxisAttribName(), fromVec, toVec, upVec);
    GA_FeE_MatchAxis::matchAxis(cookparms, outGeo0, sopparms.getPointGroup(), owner, sopparms.getMatchAxisAttribName(), fromVec, toVec);


}



namespace SOP_FeE_MatchAxis_1_0_Namespace {

} // End SOP_FeE_MatchAxis_1_0_Namespace namespace
