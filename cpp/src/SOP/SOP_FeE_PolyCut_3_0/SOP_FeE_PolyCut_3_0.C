
//#define UT_ASSERT_LEVEL 3
#include "SOP_FeE_PolyCut_3_0.h"


#include "SOP_FeE_PolyCut_3_0.proto.h"

#include "GEO/GEO_Detail.h"
#include "PRM/PRM_TemplateBuilder.h"
#include "UT/UT_Interrupt.h"
#include "UT/UT_DSOVersion.h"


#include "GA_FeE/GA_FeE_Attribute.h"
#include "GA_FeE/GA_FeE_VertexNextEquiv.h"
#include "GEO_FeE/GEO_FeE_Group.h"

#include "GA_FeE/GA_FeE_PolyCut.h"


using namespace SOP_FeE_PolyCut_3_0_Namespace;


static const char *theDsFile = R"THEDSFILE(
{
    name	parameters
    parm {
        name    "primGroup"
        cppname "PrimGroup"
        label   "Prim Group"
        type    string
        default { "" }
        parmtag { "script_action" "import soputils\nkwargs['geometrytype'] = (hou.geometryType.Primitives, )\nkwargs['inputindex'] = 0\nsoputils.selectGroupParm(kwargs)" }
        parmtag { "script_action_help" "Select geometry from an available viewport.\nShift-click to turn on Select Groups." }
        parmtag { "script_action_icon" "BUTTONS_reselect" }
    }
    parm {
        name    "cutPointGroup"
        cppname "CutPointGroup"
        label   "Cut Point Group"
        type    string
        default { "" }
        parmtag { "script_action" "import soputils\nkwargs['geometrytype'] = (hou.geometryType.Points, )\nkwargs['inputindex'] = 0\nsoputils.selectGroupParm(kwargs)" }
        parmtag { "script_action_help" "Select geometry from an available viewport.\nShift-click to turn on Select Groups." }
        parmtag { "script_action_icon" "BUTTONS_reselect" }
    }
    parm {
        name    "mergePrimEndsIfClosed"
        cppname "MergePrimEndsIfClosed"
        label   "Merge Prim Ends if Closed "
        type    toggle
        default { "1" }
    }
    parm {
        name    "cutPoint"
        cppname "CutPoint"
        label   "Cut Point"
        type    toggle
        default { "0" }
    }
    parm {
        name    "primType"
        cppname "PrimType"
        label   "Prim Type"
        type    ordinal
        default { "auto" }
        menu {
            "auto"          "Auto Detect"
            "polyline"      "Poly Line"
            "poly"          "Poly"
        }
    }

    parm {
        name    "createOrigPrimAttrib"
        cppname "CreateOrigPrimAttrib"
        label   "Create Origin Primitive Attribute"
        type    toggle
        nolabel
        joinnext
        default { "0" }
    }
    parm {
        name    "origPrimAttribName"
        cppname "OrigPrimAttribName"
        label   "Origin Primitive Attribute Name"
        type    string
        default { "origPrim" }
        disablewhen "{ createOrigPrimAttrib == 0 }"
    }
    parm {
        name    "createOrigPointAttrib"
        cppname "CreateOrigPointAttrib"
        label   "Create Origin Point Attribute"
        type    toggle
        nolabel
        joinnext
        default { "off" }
        disablewhen "{ cutPoint == 0 }"
    }
    parm {
        name    "origPointAttribName"
        cppname "OrigPointAttribName"
        label   "Origin Point Attribute Name"
        type    string
        default { "origPoint" }
        disablewhen "{ createOrigPointAttrib == 0 } { cutPoint == 0 }"
    }
    parm {
        name    "sepparm"
        label   "Separator"
        type    separator
        default { "" }
    }

    parm {
        name    "keepPrimAttribName"
        cppname "KeepPrimAttribName"
        label   "Keep Primitive Attribute Name"
        type    string
        default { "" }
    }
    parm {
        name    "keepPointAttribName"
        cppname "KeepPointAttribName"
        label   "Keep Point Attribute Name"
        type    string
        default { "*" }
        disablewhen "{ cutPoint == 0 }"
    }

    parm {
        name    "keepPrimGroupName"
        cppname "KeepPrimGroupName"
        label   "Keep Primitive Group Name"
        type    string
        default { "" }
    }
    parm {
        name    "keepPointGroupName"
        cppname "KeepPointGroupName"
        label   "Keep Point Group Name"
        type    string
        default { "*" }
        disablewhen "{ cutPoint == 0 }"
    }
    parm {
        name    "keepEdgeGroupName"
        cppname "KeepEdgeGroupName"
        label   "Keep Edge Group Name"
        type    string
        default { "" }
        disablewhen "{ cutPoint == 0 }"
    }

    parm {
        name    "delInputPointGroup"
        cppname "DelInputPointGroup"
        label   "Delete Input Point Group"
        type    toggle
        default { "0" }
    }


    parm {
        name    "kernel"
        cppname "Kernel"
        label   "Kernel"
        type    integer
        default { 0 }
        range   { 0! 1! }
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
SOP_FeE_PolyCut_3_0::buildTemplates()
{
    static PRM_TemplateBuilder templ("SOP_FeE_PolyCut_3_0.C"_sh, theDsFile);
    if (templ.justBuilt())
    {
        templ.setChoiceListPtr("cutPointGroup"_sh, &SOP_Node::pointGroupMenu);
        templ.setChoiceListPtr("primGroup"_sh, &SOP_Node::primGroupMenu);

        templ.setChoiceListPtr("keepPrimAttribName"_sh, &SOP_Node::primAttribMenu);
        templ.setChoiceListPtr("keepPointAttribName"_sh, &SOP_Node::pointAttribMenu);
        templ.setChoiceListPtr("keepPrimGroupName"_sh, &SOP_Node::primGroupMenu);
        templ.setChoiceListPtr("keepPointGroupName"_sh, &SOP_Node::pointGroupMenu);
        templ.setChoiceListPtr("keepEdgeGroupName"_sh, &SOP_Node::edgeGroupMenu);
    }
    return templ.templates();
}





const UT_StringHolder SOP_FeE_PolyCut_3_0::theSOPTypeName("FeE::polyCut::3.0"_sh);

void
newSopOperator(OP_OperatorTable* table)
{
    OP_Operator* newOp = new OP_Operator(
        SOP_FeE_PolyCut_3_0::theSOPTypeName,
        "FeE Poly Cut",
        SOP_FeE_PolyCut_3_0::myConstructor,
        SOP_FeE_PolyCut_3_0::buildTemplates(),
        1,
        1,
        nullptr,
        OP_FLAG_GENERATOR,
        nullptr,
        1,
        "Five elements Elf/Topo/Primitive");

    newOp->setIconName("SOP_polycut");
    table->addOperator(newOp);
}




class SOP_FeE_PolyCut_3_0Verb : public SOP_NodeVerb
{
public:
    SOP_FeE_PolyCut_3_0Verb() {}
    virtual ~SOP_FeE_PolyCut_3_0Verb() {}

    virtual SOP_NodeParms *allocParms() const { return new SOP_FeE_PolyCut_3_0Parms(); }
    virtual UT_StringHolder name() const { return SOP_FeE_PolyCut_3_0::theSOPTypeName; }

    virtual CookMode cookMode(const SOP_NodeParms *parms) const { return COOK_GENERIC; }

    virtual void cook(const CookParms &cookparms) const;
    
    static const SOP_NodeVerb::Register<SOP_FeE_PolyCut_3_0Verb> theVerb;
};

const SOP_NodeVerb::Register<SOP_FeE_PolyCut_3_0Verb> SOP_FeE_PolyCut_3_0Verb::theVerb;

const SOP_NodeVerb *
SOP_FeE_PolyCut_3_0::cookVerb() const 
{ 
    return SOP_FeE_PolyCut_3_0Verb::theVerb.get();
}



static int
sopPrimType(SOP_FeE_PolyCut_3_0Parms::PrimType parmgrouptype)
{
    using namespace SOP_FeE_PolyCut_3_0Enums;
    switch (parmgrouptype)
    {
    case PrimType::AUTO:       return 0;    break;
    case PrimType::POLYLINE:   return 1;    break;
    case PrimType::POLY:       return 2;    break;
    }
    UT_ASSERT_MSG(0, "Unhandled Prim type!");
    return -1;
}


void
SOP_FeE_PolyCut_3_0Verb::cook(const SOP_NodeVerb::CookParms& cookparms) const
{
    auto&& sopparms = cookparms.parms<SOP_FeE_PolyCut_3_0Parms>();
    GEO_Detail* outGeo0 = cookparms.gdh().gdpNC();
    //auto sopcache = (SOP_FeE_PolyCut_3_0Cache*)cookparms.cache();

    const GA_Detail* const inGeo0 = cookparms.inputGeo(0);

    GU_DetailHandle tmpGeoH0;
    GU_Detail* tmpGeo0 = new GU_Detail();
    tmpGeoH0.allocateAndSet(tmpGeo0);
    tmpGeo0->replaceWith(*inGeo0);

    outGeo0->replaceWithPoints(*inGeo0);
    //outGeo0->replaceWith(*inGeo0);

    //GA_PointGroup* groupOneNeb = GA_FeE_TopologyReference::addGroupOneNeb(outGeo0, nullptr);

    
    GOP_Manager gop;
    const GA_PointGroup* const cutPointGroup = GA_FeE_Group::findOrParsePointGroupDetached(cookparms, outGeo0, sopparms.getCutPointGroup(), gop);
    if (cutPointGroup && cutPointGroup->isEmpty())
        return;

    const GA_PrimitiveGroup* const primGroup = GA_FeE_Group::findOrParsePrimitiveGroupDetached(cookparms, tmpGeo0, sopparms.getPrimGroup(), gop);
    if (primGroup && primGroup->isEmpty())
        return;

    const bool mergePrimEndsIfClosed = sopparms.getMergePrimEndsIfClosed();

    const bool cutPoint = sopparms.getCutPoint();

    const int primType = sopPrimType(sopparms.getPrimType());

    const exint kernel = sopparms.getKernel();


    const exint subscribeRatio = sopparms.getSubscribeRatio();
    const exint minGrainSize = sopparms.getMinGrainSize();


    const GA_Storage inStorageI = GA_FeE_Type::getPreferredStorageI(outGeo0);

    UT_AutoInterrupt boss("Processing");
    if (boss.wasInterrupted())
        return;
    

    const UT_StringHolder& keepPrimAttribName  = sopparms.getKeepPrimAttribName();
    const UT_StringHolder& keepPointAttribName = sopparms.getKeepPointAttribName();
    const UT_StringHolder& keepPrimGroupName   = sopparms.getKeepPrimGroupName();
    const UT_StringHolder& keepPointGroupName  = sopparms.getKeepPointGroupName();
    const UT_StringHolder& keepEdgeGroupName   = sopparms.getKeepEdgeGroupName();



    GA_Attribute* srcPrimsAttrib = nullptr;
    GA_Attribute* srcPointsAttrib = nullptr;

    const bool createOrigPrimAttrib = sopparms.getCreateOrigPrimAttrib();
    //if (createOrigPrimAttrib || keepPrimAttribName.length() > 0 || keepPrimGroupName.length() > 0)
    if (createOrigPrimAttrib)
    {
        const UT_StringHolder& origPrimAttribName = sopparms.getOrigPrimAttribName();
        srcPrimsAttrib = outGeo0->addIntTuple(GA_ATTRIB_PRIMITIVE, origPrimAttribName, 1, GA_Defaults(-1), nullptr, nullptr, inStorageI);
    }
    const bool createOrigPointAttrib = sopparms.getCreateOrigPointAttrib();
    //if (cutPoint)
    //{
    //    if (createOrigPointAttrib || keepPointAttribName.length() > 0 || keepPointGroupName.length() > 0)
    //    {
    //        const UT_StringHolder& origPointAttribName = sopparms.getOrigPointAttribName();
    //        srcPointsAttrib = outGeo0->addIntTuple(GA_ATTRIB_POINT, origPointAttribName, 1, GA_Defaults(-1), nullptr, nullptr, inStorageI);
    //    }
    //}
    if (cutPoint && createOrigPointAttrib)
    {
        const UT_StringHolder& origPointAttribName = sopparms.getOrigPointAttribName();
        srcPointsAttrib = outGeo0->addIntTuple(GA_ATTRIB_POINT, origPointAttribName, 1, GA_Defaults(-1), nullptr, nullptr, inStorageI);
    }

    GA_FeE_PolyCut::polyCut(outGeo0, tmpGeo0, cutPointGroup, cutPoint, primGroup, mergePrimEndsIfClosed, primType);

    //if (!createOrigPointAttrib && srcPointsAttrib)
    //{
    //    outGeo0->getAttributes().destroyAttribute(srcPointsAttrib);
    //}

    //if (!createOrigPrimAttrib && srcPrimsAttrib)
    //{
    //    outGeo0->getAttributes().destroyAttribute(srcPrimsAttrib);
    //}

    const bool delInputPointGroup = sopparms.getDelInputPointGroup();
    if (delInputPointGroup && cutPointGroup && !cutPointGroup->isDetached())
    {
        outGeo0->destroyGroup(const_cast<GA_PointGroup*>(cutPointGroup));
    }


    outGeo0->bumpDataIdsForAddOrRemove(1, 1, 1);
    tmpGeoH0.deleteGdp();

}



namespace SOP_FeE_PolyCut_3_0_Namespace {

} // End SOP_FeE_PolyCut_3_0_Namespace namespace
