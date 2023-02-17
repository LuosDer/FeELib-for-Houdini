
//#define UT_ASSERT_LEVEL 3
#include "SOP_FeE_Normal2D_1_0.h"


#include "SOP_FeE_Normal2D_1_0.proto.h"


#include "GA/GA_Detail.h"
#include "PRM/PRM_TemplateBuilder.h"
#include "UT/UT_Interrupt.h"
#include "UT/UT_DSOVersion.h"


#include "GA_FeE/GA_FeE_Normal.h"




using namespace SOP_FeE_Normal2D_1_0_Namespace;



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
        parmtag { "script_action_help" "Select geometry from an available viewport." }
        parmtag { "script_action_icon" "BUTTONS_reselect" }
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
        name    "posAttribName"
        cppname "PosAttribName"
        label   "Pos Attrib Name"
        type    string
        default { "P" }
    }



    parm {
        name    "normal2DAttribName"
        cppname "Normal2DAttribName"
        label   "Normal2D Attrib Name"
        type    string
        default { "N" }
    }



    parm {
        name    "extrapolateEnds"
        cppname "ExtrapolateEnds"
        label   "Extrapolate Ends"
        type    toggle
        default { "1" }
    }
    parm {
        name    "scaleByTurns"
        cppname "ScaleByTurns"
        label   "Scale by Turns"
        type    toggle
        default { "1" }
    }
    parm {
        name    "normalize"
        cppname "Normalize"
        label   "Normalize"
        type    toggle
        default { "1" }
        disablewhen "{ scaleByTurns == 1 }"
    }


    parm {
        name    "uniScale"
        cppname "UniScale"
        label   "Uniform Scale"
        type    log
        default { "1" }
        range   { 0.01 100 }
    }
    parm {
        name    "blend"
        cppname "Blend"
        label   "Blend"
        type    float
        default { "0" }
        range   { -1! 1! }
    }







    parm {
        name    "useConstantNormal3D"
        cppname "UseConstantNormal3D"
        label   "Use Constant Normal 3D"
        type    toggle
        default { "0" }
    }
    parm {
        name    "findNormal3D"
        cppname "FindNormal3D"
        label   "Find Normal 3D"
        type    toggle
        default { "0" }
        disablewhen "{ useConstantNormal3D == 1 }"
    }
    parm {
        name    "addNormal3DIfNoFind"
        cppname "AddNormal3DIfNoFind"
        label   "Add Normal 3D If No Find"
        type    toggle
        default { "1" }
        disablewhen "{ useConstantNormal3D == 1 } { findNormal3D == 0 }"
    }

    parm {
        name    "normal3DAttribClass"
        cppname "Normal3DAttribClass"
        label   "Normal 3D Attrib Class"
        type    ordinal
        default { "point" }
        disablewhen "{ useConstantNormal3D == 1 }"
        menu {
            "prim"          "Prim"
            "point"         "Point"
            "vertex"        "Vertex"
            "detail"        "Detail"
            "pointvertex"   "PointVertex"
            "all"           "ALL"
        }
    }
    parm {
        name    "normal3DAttribName"
        cppname "Normal3DAttribName"
        label   "Normal3D Attrib Name"
        type    string
        default { "N" }
        disablewhen "{ useConstantNormal3D == 1 } { findNormal3D == 0 }"
    }
    parm {
        name    "defaultNormal3D"
        cppname "DefaultNormal3D"
        label   "Default Normal 3D"
        type    vector
        size    3
        default { "0" "1" "0" }
        disablewhen "{ useConstantNormal3D == 0 addNormal3DIfNoFind == 1 } { useConstantNormal3D == 0 findNormal3D == 0 }"
        range   { -1 1 }
    }




    parm {
        name    "outTopoAttrib"
        cppname "OutTopoAttrib"
        label   "Output Topo Attribute"
        type    toggle
        default { "0" }
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
SOP_FeE_Normal2D_1_0::buildTemplates()
{
    static PRM_TemplateBuilder templ("SOP_FeE_Normal2D_1_0.C"_sh, theDsFile);
    if (templ.justBuilt())
    {
        templ.setChoiceListPtr("group"_sh, &SOP_Node::allGroupMenu);
        templ.setChoiceListPtr("posAttribName"_sh, &SOP_Node::pointAttribReplaceMenu);
        templ.setChoiceListPtr("normal3DAttribName"_sh, &SOP_Node::allAttribReplaceMenu);
    }
    return templ.templates();
}


const UT_StringHolder SOP_FeE_Normal2D_1_0::theSOPTypeName("FeE::normal2d::1.0"_sh);

void
newSopOperator(OP_OperatorTable* table)
{
    OP_Operator* newOp = new OP_Operator(
        SOP_FeE_Normal2D_1_0::theSOPTypeName,
        "FeE Normal 2D",
        SOP_FeE_Normal2D_1_0::myConstructor,
        SOP_FeE_Normal2D_1_0::buildTemplates(),
        1,
        1,
        nullptr,
        OP_FLAG_GENERATOR,
        nullptr,
        1,
        "Five elements Elf/Data/Orient");

    newOp->setIconName("SOP_normal");
    table->addOperator(newOp);
}


//class SOP_FeE_Normal2D_1_0Cache : public SOP_NodeCache
//{
//public:
//    SOP_FeE_Normal2D_1_0Cache() : SOP_NodeCache(),
//        myPrevOutputDetailID(-1)
//    {}
//    ~SOP_FeE_Normal2D_1_0Cache() override {}
//
//    int64 myPrevOutputDetailID;
//};


class SOP_FeE_Normal2D_1_0Verb : public SOP_NodeVerb
{
public:
    SOP_FeE_Normal2D_1_0Verb() {}
    virtual ~SOP_FeE_Normal2D_1_0Verb() {}

    virtual SOP_NodeParms *allocParms() const { return new SOP_FeE_Normal2D_1_0Parms(); }
    //virtual SOP_NodeCache* allocCache() const { return new SOP_FeE_Normal2D_1_0Cache(); }
    virtual UT_StringHolder name() const { return SOP_FeE_Normal2D_1_0::theSOPTypeName; }

    virtual CookMode cookMode(const SOP_NodeParms *parms) const { return COOK_GENERIC; }

    virtual void cook(const CookParms &cookparms) const;

    static const SOP_NodeVerb::Register<SOP_FeE_Normal2D_1_0Verb> theVerb;
};

const SOP_NodeVerb::Register<SOP_FeE_Normal2D_1_0Verb> SOP_FeE_Normal2D_1_0Verb::theVerb;

const SOP_NodeVerb *
SOP_FeE_Normal2D_1_0::cookVerb() const 
{ 
    return SOP_FeE_Normal2D_1_0Verb::theVerb.get();
}





static GFE_NormalSearchOrder
sopAttribSearchOrder(SOP_FeE_Normal2D_1_0Parms::Normal3DAttribClass attribClass)
{
    using namespace SOP_FeE_Normal2D_1_0Enums;
    switch (attribClass)
    {
    case Normal3DAttribClass::PRIM:          return GFE_NormalSearchOrder_PRIMITIVE;   break;
    case Normal3DAttribClass::POINT:         return GFE_NormalSearchOrder_POINT;       break;
    case Normal3DAttribClass::VERTEX:        return GFE_NormalSearchOrder_VERTEX;      break;
    case Normal3DAttribClass::DETAIL:        return GFE_NormalSearchOrder_DETAIL;      break;
    case Normal3DAttribClass::POINTVERTEX:   return GFE_NormalSearchOrder_POINTVERTEX; break;
    case Normal3DAttribClass::ALL:           return GFE_NormalSearchOrder_N;           break;
    }
    UT_ASSERT_MSG(0, "Unhandled GFE Normal Search Order!");
    return GFE_NormalSearchOrder_INVALID;
}


//static int
//sopAttribSearchOrder(SOP_FeE_Normal2D_1_0Parms::Normal3DAttribClass attribClass, GA_AttributeOwner* searchOrder)
//{
//    int size = 0;
//    using namespace SOP_FeE_Normal2D_1_0Enums;
//    switch (attribClass)
//    {
//    case Normal3DAttribClass::PRIM:          searchOrder = { GA_ATTRIB_PRIMITIVE }; size = 1; break;
//    case Normal3DAttribClass::POINT:         return GA_ATTRIB_POINT;      break;
//    case Normal3DAttribClass::VERTEX:        return GA_ATTRIB_VERTEX;     break;
//    case Normal3DAttribClass::DETAIL:        return GA_ATTRIB_DETAIL;     break;
//    case Normal3DAttribClass::POINTVERTEX:   return GA_ATTRIB_INVALID;    break;
//    case Normal3DAttribClass::ALL:           return GA_ATTRIB_OWNER_N;    break;
//    }
//    UT_ASSERT_MSG(0, "Unhandled Geo0 Class type!");
//    return GA_ATTRIB_INVALID;
//}



static GA_GroupType
sopGroupType(SOP_FeE_Normal2D_1_0Parms::GroupType parmgrouptype)
{
    using namespace SOP_FeE_Normal2D_1_0Enums;
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
SOP_FeE_Normal2D_1_0Verb::cook(const SOP_NodeVerb::CookParms &cookparms) const
{
    auto &&sopparms = cookparms.parms<SOP_FeE_Normal2D_1_0Parms>();
    GA_Detail* const outGeo0 = cookparms.gdh().gdpNC();
    //auto sopcache = (SOP_FeE_Normal2D_1_0Cache*)cookparms.cache();

    const GA_Detail* const inGeo0 = cookparms.inputGeo(0);

    outGeo0->replaceWith(*inGeo0);

    const UT_StringHolder& geo0AttribNames = sopparms.getNormal2DAttribName();
    if (!geo0AttribNames.isstring())
        return;


    GFE_NormalSearchOrder geo0Normal3DSearchOrder = sopAttribSearchOrder(sopparms.getNormal3DAttribClass());





    const bool scaleByTurns = sopparms.getScaleByTurns();
    const bool normalize = sopparms.getNormalize();
    const bool extrapolateEnds = sopparms.getExtrapolateEnds();

    const bool useConstantNormal3D = sopparms.getUseConstantNormal3D();
    const bool findNormal3D = sopparms.getFindNormal3D();
    const bool addNormal3DIfNoFind = sopparms.getAddNormal3DIfNoFind();
    
    UT_Vector3T<fpreal64> defaultNormal3D = sopparms.getDefaultNormal3D();
    const fpreal64 uniScale = sopparms.getUniScale();
    const fpreal64 blend = sopparms.getBlend();


    const exint subscribeRatio = sopparms.getSubscribeRatio();
    const exint minGrainSize = sopparms.getMinGrainSize();




    //const GA_Precision preferredPrecision = outGeo0->getPreferredPrecision();
    //const GA_Storage inStorageI = GA_FeE_Type::getPreferredStorageI(preferredPrecision);
    //const GA_Storage inStorageF = GA_FeE_Type::getPreferredStorageF(preferredPrecision);
    const GA_Storage inStorageF = GA_FeE_Type::getPreferredStorageF(outGeo0);

    UT_AutoInterrupt boss("Processing");
    if (boss.wasInterrupted())
        return;

    const UT_StringHolder& groupName0 = sopparms.getGroup();
    const GA_GroupType groupType = sopGroupType(sopparms.getGroupType());
    
    const UT_StringHolder& posAttribName = sopparms.getPosAttribName();
    
    const GA_Attribute* const posAttrib = outGeo0->findPointAttribute(sopparms.getPosAttribName());





    const UT_StringHolder& normal3DAttribName = sopparms.getNormal3DAttribName();

    const float cuspangledegrees = GEO_DEFAULT_ADJUSTED_CUSP_ANGLE;
    const GEO_NormalMethod method = GEO_NormalMethod::ANGLE_WEIGHTED;
    const bool copy_orig_if_zero = false;


    GA_Attribute* const normal2DAttrib = GA_FeE_Normal::addAttribNormal2D(cookparms, outGeo0, posAttribName,
        groupType, groupName0, inStorageF, geo0AttribNames,
        defaultNormal3D, scaleByTurns, normalize, uniScale,

        useConstantNormal3D, normal3DAttribName,
        findNormal3D, addNormal3DIfNoFind, geo0Normal3DSearchOrder,
        cuspangledegrees, method, copy_orig_if_zero,
        subscribeRatio, minGrainSize);


    GA_FeE_TopologyReference::outTopoAttrib(outGeo0, sopparms.getOutTopoAttrib());

    normal2DAttrib->bumpDataId();
}



namespace SOP_FeE_Normal2D_1_0_Namespace {

} // End SOP_FeE_Normal2D_1_0_Namespace namespace
