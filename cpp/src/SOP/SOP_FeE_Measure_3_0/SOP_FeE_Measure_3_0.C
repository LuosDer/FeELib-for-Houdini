
//#define UT_ASSERT_LEVEL 3
#include "SOP_FeE_Measure_3_0.h"

#include "SOP_FeE_Measure_3_0.proto.h"

#include "GA/GA_Detail.h"
#include "PRM/PRM_TemplateBuilder.h"
#include "UT/UT_Interrupt.h"
#include "UT/UT_DSOVersion.h"




#include "GFE/GFE_Measure.h"


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
            "perimeter"     "Perimeter"
            "area"          "Area"
            "volume"        "Volume"
            "meshPerimeter" "Mesh Perimeter"
            "meshArea"      "Mesh Area"
            "meshVolume"    "Mesh Volume "
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
        cppname "PosAttribName"
        label   "Position Attribute"
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
        templ.setChoiceListPtr("posAttribName"_sh, &SOP_Node::pointAttribReplaceMenu);
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


static GFE_MeasureType
sopMeasureType(SOP_FeE_Measure_3_0Parms::MeasureType parmgrouptype)
{
    using namespace SOP_FeE_Measure_3_0Enums;
    switch (parmgrouptype)
    {
    case MeasureType::PERIMETER:          return GFE_MeasureType::Perimeter;       break;
    case MeasureType::AREA:               return GFE_MeasureType::Area;            break;
    case MeasureType::VOLUME:             return GFE_MeasureType::Area;            break;
    case MeasureType::MESHPERIMETER:      return GFE_MeasureType::MeshPerimeter;   break;
    case MeasureType::MESHAREA:           return GFE_MeasureType::MeshArea;        break;
    case MeasureType::MESHVOLUME:         return GFE_MeasureType::MeshVolume;      break;
    }
    UT_ASSERT_MSG(0, "Unhandled geo0Group type!");
    return GFE_MeasureType::Area;
}



void
SOP_FeE_Measure_3_0Verb::cook(const SOP_NodeVerb::CookParms &cookparms) const
{
    auto &&sopparms = cookparms.parms<SOP_FeE_Measure_3_0Parms>();
    GA_Detail& outGeo0 = *cookparms.gdh().gdpNC();
    //auto sopcache = (SOP_FeE_Measure_3_0Cache*)cookparms.cache();

    const GA_Detail& inGeo0 = *cookparms.inputGeo(0);

    outGeo0.replaceWith(inGeo0);

    const UT_StringHolder& measureAttribName = sopparms.getMeasureAttribName();
    if (!measureAttribName.isstring())
        return;

    const GA_GroupType groupType = sopGroupType(sopparms.getGroupType());
    const GFE_MeasureType measureType = sopMeasureType(sopparms.getMeasureType());

    UT_AutoInterrupt boss("Processing");
    if (boss.wasInterrupted())
        return;

    GFE_Measure measure(outGeo0, &cookparms);
    measure.groupParser.setGroup(groupType, sopparms.getGroup());
    measure.setPositionAttrib(sopparms.getPosAttribName());
    measure.findOrCreateTuple(false, measureAttribName);
    
    measure.setComputeParm(measureType,
        sopparms.getSubscribeRatio(), sopparms.getMinGrainSize());

    measure.computeAndBumpDataId();


    //switch ()
    //{
    //case SOP_FeE_Measure_3_0Enums::MeasureType::AREA:
    //    GFE_Measure_Namespace::addAttribPrimArea(     outGeo0, geo0PosAttribName,
    //        static_cast<const GA_PrimitiveGroup*>(geo0Group), inStorageF, measureAttribName,
    //        GA_Defaults(-1.0), nullptr, nullptr, GA_ReuseStrategy(), subscribeRatio, minGrainSize);
    //    break;
    //case SOP_FeE_Measure_3_0Enums::MeasureType::PERIMETER:
    //    GFE_Measure_Namespace::addAttribPrimPerimeter(outGeo0, geo0PosAttribName,
    //        static_cast<const GA_PrimitiveGroup*>(geo0Group), inStorageF, measureAttribName,
    //        GA_Defaults(-1.0), nullptr, nullptr, GA_ReuseStrategy(), subscribeRatio, minGrainSize);
    //    break;
    //case SOP_FeE_Measure_3_0Enums::MeasureType::MESHVOLUME:
    //    GFE_Measure_Namespace::addAttribMeshVolume(outGeo0, geo0PosAttribName,
    //        static_cast<const GA_PrimitiveGroup*>(geo0Group), inStorageF, measureAttribName,
    //        GA_Defaults(-1.0), nullptr, nullptr, GA_ReuseStrategy(), subscribeRatio, minGrainSize);
    //    break;
    //default:
    //    cookparms.sopAddError(SOP_ERR_INVALID_SRC, "unsupport parm");
    //    return;
    //}


    

}



namespace SOP_FeE_Measure_3_0_Namespace {

} // End SOP_FeE_Measure_3_0_Namespace namespace
