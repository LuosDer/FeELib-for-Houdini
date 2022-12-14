
//#define UT_ASSERT_LEVEL 3
#include "SOP_FeE_Measure_3_0.h"

#include "SOP_FeE_Measure_3_0.proto.h"

#include "GEO/GEO_Detail.h"
#include "PRM/PRM_TemplateBuilder.h"
#include "UT/UT_Interrupt.h"
#include "UT/UT_DSOVersion.h"

#include "GA_FeE/GA_FeE_Type.h"
#include "GA_FeE/GA_FeE_Attribute.h"
#include "GA_FeE/GA_FeE_Group.h"
#include "GA_FeE/GA_FeE_Measure.h"


using namespace SOP_FeE_Measure_3_0_Namespace;


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
    if (templ.justBuilt())
    {
        templ.setChoiceListPtr("group"_sh, &SOP_Node::allGroupMenu);
    }
    return templ.templates();
}





const UT_StringHolder SOP_FeE_Measure_3_0::theSOPTypeName("FeE::measure::3.0"_sh);

void
newSopOperator(OP_OperatorTable* table)
{
    OP_Operator* newOp = new OP_Operator(
        SOP_FeE_Measure_3_0::theSOPTypeName,
        "FeE Measure",
        SOP_FeE_Measure_3_0::myConstructor,
        SOP_FeE_Measure_3_0::buildTemplates(),
        1,
        1,
        nullptr,
        OP_FLAG_GENERATOR,
        nullptr,
        1,
        "Five elements Elf/Data/Property");

    newOp->setIconName("SOP_measure-2.0");
    table->addOperator(newOp);
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

    static const SOP_NodeVerb::Register<SOP_FeE_Measure_3_0Verb> theVerb;
};

const SOP_NodeVerb::Register<SOP_FeE_Measure_3_0Verb> SOP_FeE_Measure_3_0Verb::theVerb;

const SOP_NodeVerb *
SOP_FeE_Measure_3_0::cookVerb() const 
{ 
    return SOP_FeE_Measure_3_0Verb::theVerb.get();
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

    const GA_Detail* const inGeo0 = cookparms.inputGeo(0);

    outGeo0->replaceWith(*inGeo0);

    //outGeo0 = sopNodeProcess(*inGeo0);

    const UT_StringHolder& geo0PosAttribName = sopparms.getPAttribName();
    if (!geo0PosAttribName.isstring())
        return;

    const UT_StringHolder& measureAttribName = sopparms.getMeasureAttribName();
    if (!measureAttribName.isstring())
        return;


    UT_AutoInterrupt boss("Processing");
    if (boss.wasInterrupted())
        return;


    const GA_GroupType groupType = sopGroupType(sopparms.getGroupType());
    GOP_Manager gop;
    const GA_Group* geo0Group = GA_FeE_Group::findOrParseGroupDetached(cookparms, outGeo0, groupType, sopparms.getGroup(), gop);
    if (geo0Group && geo0Group->isEmpty())
        return;

    const GA_GroupType geo0finalGroupType = geo0Group ? geo0Group->classType() : GA_GROUP_INVALID;





    //const fpreal uniScale = sopparms.getUniScale();


    const exint subscribeRatio = sopparms.getSubscribeRatio();
    const exint minGrainSize = sopparms.getMinGrainSize();


    const GA_Storage inStorageF = GA_FeE_Type::getPreferredStorageF(outGeo0);
    

    //GA_Attribute* measureAttribPtr = outGeo0->addFloatTuple(GA_ATTRIB_PRIMITIVE, measureAttribName, 1, GA_Defaults(0.0), 0, 0, inStorageF);
    //GA_RWHandleT<attribPrecisonF> measureAttribHandle(measureAttribPtr);

    //const SOP_FeE_Measure_3_0Parms::MeasureType measureType = sopparms.getMeasureType();
    switch (sopparms.getMeasureType())
    {
    case SOP_FeE_Measure_3_0Enums::MeasureType::AREA:
        GA_FeE_Measure::addAttribPrimArea(     outGeo0, geo0PosAttribName,
            static_cast<const GA_PrimitiveGroup*>(geo0Group), inStorageF, measureAttribName,
            GA_Defaults(-1.0), nullptr, nullptr, GA_ReuseStrategy(), subscribeRatio, minGrainSize);
        break;
    case SOP_FeE_Measure_3_0Enums::MeasureType::PERIMETER:
        GA_FeE_Measure::addAttribPrimPerimeter(outGeo0, geo0PosAttribName,
            static_cast<const GA_PrimitiveGroup*>(geo0Group), inStorageF, measureAttribName,
            GA_Defaults(-1.0), nullptr, nullptr, GA_ReuseStrategy(), subscribeRatio, minGrainSize);
        break;
    default:
        cookparms.sopAddError(SOP_ERR_INVALID_SRC, "unsupport parm");
        return;
    }


    

}



namespace SOP_FeE_Measure_3_0_Namespace {

} // End SOP_FeE_Measure_3_0_Namespace namespace
