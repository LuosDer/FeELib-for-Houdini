
//#define UT_ASSERT_LEVEL 3
#include "SOP_FeE_Normal2D_1_0.h"


#include "SOP_FeE_Normal2D_1_0.proto.h"


#include "GA/GA_Detail.h"
#include "PRM/PRM_TemplateBuilder.h"
#include "UT/UT_Interrupt.h"
#include "UT/UT_DSOVersion.h"


#include "GA_FeE/GA_FeE_Attribute.h"
#include "GA_FeE/GA_FeE_Measure.h"
#include "GA_FeE/GA_FeE_Connectivity.h"
#include "GA_FeE/GA_FeE_TopologyReference.h"
#include "GEO_FeE/GEO_FeE_Group.h"
#include "GEO_FeE/GEO_FeE_GroupExpand.h"
#include "GEO_FeE/GEO_FeE_Normal.h"

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
        menutoggle {
            [ "" ]
        }
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
        name    "inputNormal3D"
        cppname "InputNormal3D"
        label   "Input Normal 3D"
        type    toggle
        default { "0" }
    }
    parm {
        name    "normal3DAttribClass"
        cppname "Normal3DAttribClass"
        label   "Normal 3D Attrib Class"
        type    ordinal
        default { "point" }
        disablewhen " { inputNormal3D == 1 useConstantNormal3D == 1 }"
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
        name    "useConstantNormal3D"
        cppname "UseConstantNormal3D"
        label   "Use Constant Normal 3D"
        type    toggle
        default { "0" }
        hidewhen "{ inputNormal3D == 1 }"
    }
    parm {
        name    "normal3DAttribName"
        cppname "Normal3DAttribName"
        label   "Normal3D Attrib Name"
        type    string
        default { "N" }
        hidewhen "{ inputNormal3D == 0 } { inputNormal3D == 1 useConstantNormal3D == 1 }"
    }
    parm {
        name    "defaultNormal3D"
        cppname "DefaultNormal3D"
        label   "Default Normal 3D"
        type    vector
        size    3
        default { "0" "1" "0" }
        disablewhen "{ useConstantNormal3D == 0 }"
        range   { -1 1 }
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


const UT_StringHolder SOP_FeE_Normal2D_1_0::theSOPTypeName("FeE::normal2D::1.0"_sh);

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







static GA_AttributeOwner
sopAttribOwner(SOP_FeE_Normal2D_1_0Parms::Normal3DAttribClass attribClass)
{
    using namespace SOP_FeE_Normal2D_1_0Enums;
    switch (attribClass)
    {
    case Normal3DAttribClass::PRIM:          return GA_ATTRIB_PRIMITIVE;  break;
    case Normal3DAttribClass::POINT:         return GA_ATTRIB_POINT;      break;
    case Normal3DAttribClass::VERTEX:        return GA_ATTRIB_VERTEX;     break;
    case Normal3DAttribClass::DETAIL:        return GA_ATTRIB_DETAIL;     break;
    case Normal3DAttribClass::POINTVERTEX:   return GA_ATTRIB_INVALID;    break;
    case Normal3DAttribClass::ALL:           return GA_ATTRIB_OWNER_N;    break;
    }
    UT_ASSERT_MSG(0, "Unhandled Geo0 Class type!");
    return GA_ATTRIB_INVALID;
}


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
    GU_Detail* outGeo0 = cookparms.gdh().gdpNC();
    //auto sopcache = (SOP_FeE_Normal2D_1_0Cache*)cookparms.cache();

    const GEO_Detail* const inGeo0 = cookparms.inputGeo(0);

    outGeo0->replaceWith(*inGeo0);
    // outGeo0->clearAndDestroy();

    //outGeo0 = sopNodeProcess(*inGeo0);

#if 1
    const UT_StringHolder& N2DAttribName = "__N2D_SOP_FeE_Normal2D_1_0";
    const UT_StringHolder& N3DAttribName = "__N2D_SOP_FeE_Normal2D_1_0";
#else
    #define N2DAttribName "__N2D_SOP_FeE_Normal2D_1_0"
    #define N3DAttribName "__N3D_SOP_FeE_Normal2D_1_0"
#endif

    const UT_StringHolder& geo0AttribNames = sopparms.getNormal2DAttribName();
    if (!geo0AttribNames.isstring())
        return;
    GA_AttributeOwner geo0N3DAttribClass = sopAttribOwner(sopparms.getNormal3DAttribClass());





    const bool scaleByTurns = sopparms.getScaleByTurns();
    const bool normalize = sopparms.getNormalize();
    const bool extrapolateEnds = sopparms.getExtrapolateEnds();
    const bool inputNormal3D = sopparms.getInputNormal3D();
    const bool useConstantNormal3D = sopparms.getUseConstantNormal3D();

    UT_Vector3T<fpreal64> defaultNormal3D = sopparms.getDefaultNormal3D();
    const fpreal64 uniScale = sopparms.getUniScale();
    const fpreal64 blend = sopparms.getBlend();


    const exint subscribeRatio = sopparms.getSubscribeRatio();
    const exint minGrainSize = sopparms.getMinGrainSize();




    const GA_Precision PreferredPrecision = outGeo0->getPreferredPrecision();
    //const GA_Storage inStorageI = GA_FeE_Type::getPreferredStorageI(PreferredPrecision);
    const GA_Storage inStorageF = GA_FeE_Type::getPreferredStorageF(PreferredPrecision);

    UT_AutoInterrupt boss("Processing");
    if (boss.wasInterrupted())
        return;


    const GA_Attribute* posAttrib = outGeo0->findPointAttribute(sopparms.getPosAttribName());
    if (!posAttrib)
        return;
    //const GA_ROHandleT<UT_Vector3T<fpreal64>> posH(posAttrib);



    GOP_Manager gop;
    const UT_StringHolder& groupName0 = sopparms.getGroup();
    const GA_GroupType groupType = sopGroupType(sopparms.getGroupType());
    const GA_Group* geo0Group = GA_FeE_Group::findOrParseGroupDetached(cookparms, outGeo0, groupType, groupName0, gop);
    const bool hasGroup = !!geo0Group;

    if (hasGroup && geo0Group->isEmpty())
        return;

    GA_PointGroup* geo0PointGroup = const_cast<GA_PointGroup*>(GA_FeE_GroupPromote::groupPromotePointDetached(outGeo0, geo0Group));
    UT_UniquePtr<GA_PointGroup> geo0PointGroupUPtr(geo0PointGroup);

    GA_VertexGroup* geo0VtxGroup = const_cast<GA_VertexGroup*>(GA_FeE_GroupPromote::groupPromoteVertexDetached(outGeo0, geo0Group));

    //GA_PointGroup* expandGroup = GA_FeE_Group::newDetachedGroup(outGeo0, geo0Group);
    GA_PointGroup* expandGroup = outGeo0->newDetachedPointGroup();
    UT_UniquePtr<GA_Group> expandGroupUPtr(expandGroup);

    GEO_FeE_GroupExpand::groupExpand(outGeo0, expandGroup, geo0PointGroup, GA_GROUP_EDGE);
    //notifyGroupParmListeners(cookparms.getNode(), 0, 1, outGeo0, geo0Group);

    //const GA_GroupType geo0FinalGroupType = geo0Group ? geo0Group->classType() : GA_GROUP_INVALID;






    GA_Attribute* normal3DAttrib = nullptr;
    if (inputNormal3D)
    {
        const UT_StringHolder& normal3DAttribName = sopparms.getNormal3DAttribName();
        switch (geo0N3DAttribClass)
        {
        case GA_ATTRIB_PRIMITIVE:
            normal3DAttrib = outGeo0->findPrimitiveAttribute(normal3DAttribName);
            if (!normal3DAttrib)
                return;
            break;
        case GA_ATTRIB_POINT:
            normal3DAttrib = outGeo0->findPointAttribute(normal3DAttribName);
            if (!normal3DAttrib)
                return;
            break;
        case GA_ATTRIB_VERTEX:
            normal3DAttrib = outGeo0->findVertexAttribute(normal3DAttribName);
            if (!normal3DAttrib)
                return;
            break;
        case GA_ATTRIB_DETAIL:
            normal3DAttrib = outGeo0->findGlobalAttribute(normal3DAttribName);
            if (!normal3DAttrib)
                return;
            break;
        case GA_ATTRIB_OWNER_N:
            normal3DAttrib = outGeo0->findPrimitiveAttribute(normal3DAttribName);
            if (!normal3DAttrib)
            {
                normal3DAttrib = outGeo0->findPointAttribute(normal3DAttribName);
                if (!normal3DAttrib)
                {
                    normal3DAttrib = outGeo0->findVertexAttribute(normal3DAttribName);
                    if (!normal3DAttrib)
                    {
                        normal3DAttrib = outGeo0->findGlobalAttribute(normal3DAttribName);
                        if (!normal3DAttrib)
                            return;
                    }
                }
            }
            
            break;
        case GA_ATTRIB_INVALID:
            normal3DAttrib = outGeo0->findPointAttribute(normal3DAttribName);
            if (!normal3DAttrib)
            {
                normal3DAttrib = outGeo0->findVertexAttribute(normal3DAttribName);
                if (!normal3DAttrib)
                {
                    return;
                }
            }
            break;
        default:
            return;
            break;
        }
    }
    else if (!useConstantNormal3D)
    {
        if (geo0N3DAttribClass == GA_ATTRIB_OWNER_N || geo0N3DAttribClass == GA_ATTRIB_INVALID)
            geo0N3DAttribClass = GA_ATTRIB_PRIMITIVE;

        normal3DAttrib = GEO_FeE_Normal::addAttribNormal3D(outGeo0, nullptr, geo0N3DAttribClass, inStorageF, N3DAttribName,
            GEO_DEFAULT_ADJUSTED_CUSP_ANGLE, GEO_NormalMethod::ANGLE_WEIGHTED, false, posAttrib);
    }
    const GA_AttributeOwner N3DFinalAttribClass = normal3DAttrib ? normal3DAttrib->getOwner() : GA_ATTRIB_INVALID;
    const GA_ROHandleT<UT_Vector3T<fpreal64>> N3DH(normal3DAttrib);

    if (!useConstantNormal3D && N3DFinalAttribClass == GA_ATTRIB_DETAIL)
    {
        defaultNormal3D = N3DH.get(0);
    }
    //outGeo0->setPrimitiveClosedFlag(elemoff);


    //GA_Attribute* N2DAttrib = outGeo0->addFloatTuple(geo0AttribClass, N2DAttribName, 3, GA_Defaults(0.0), nullptr, nullptr, inStorageF);
    //GA_Attribute* N2DAttrib = outGeo0->addFloatTuple(GA_ATTRIB_POINT, N2DAttribName, 3, GA_Defaults(0.0), nullptr, nullptr, inStorageF);
    GA_Attribute* N2DAttrib = outGeo0->addFloatTuple(GA_ATTRIB_POINT, N2DAttribName, 3, GA_Defaults(0.0), nullptr, nullptr, inStorageF);
    //GA_RWHandleT<UT_Vector3T<fpreal64>> N2DAttribH(N2DAttrib);


    //geo0PointGroup->setOrdered(false);
    //geo0PointGroup->makeUnordered();
    GEO_FeE_Normal::computeNormal2D(outGeo0, geo0PointGroup,
        N2DAttrib, posAttrib, normal3DAttrib, defaultNormal3D,
        scaleByTurns, normalize, uniScale,
        subscribeRatio, minGrainSize);


    GA_FeE_Attribute::forceRenameAttribute(outGeo0, N2DAttrib, geo0AttribNames);
    //GA_AttributeOwner search_order[] = { GA_ATTRIB_POINT };
    //GA_Attribute* existAttribute = outGeo0->findAttribute(GA_ATTRIB_POINT, geo0AttribNames);
    //if(existAttribute)
    //    outGeo0->getAttributes().destroyAttribute(existAttribute);
    //outGeo0->renameAttribute(GA_ATTRIB_POINT, GA_SCOPE_PUBLIC, N2DAttribName, geo0AttribNames);

    if(!inputNormal3D && !useConstantNormal3D && normal3DAttrib)
        GA_FeE_Attribute::destroyAttribute(normal3DAttrib);
    
    
    GA_FeE_TopologyReference::outTopoAttrib(outGeo0, sopparms.getOutTopoAttrib());

    N2DAttrib->bumpDataId();
    //N2DAttrib->bumpDataId();
    outGeo0->bumpAllDataIds();


}



namespace SOP_FeE_Normal2D_1_0_Namespace {

} // End SOP_FeE_Normal2D_1_0_Namespace namespace
