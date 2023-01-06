
//#define UT_ASSERT_LEVEL 3
#include "SOP_FeE_PolyReduce2D_4_0.h"


#include "SOP_FeE_PolyReduce2D_4_0.proto.h"

#include "GEO/GEO_Detail.h"
#include "PRM/PRM_TemplateBuilder.h"
#include "UT/UT_Interrupt.h"
#include "UT/UT_DSOVersion.h"

#include "GU/GU_IntersectionAnalysis.h"

#include "GA_FeE/GA_FeE_Attribute.h"
#include "GA_FeE/GA_FeE_VertexNextEquiv.h"
#include "GEO_FeE/GEO_FeE_Group.h"



using namespace SOP_FeE_PolyReduce2D_4_0_Namespace;


static const char *theDsFile = R"THEDSFILE(
{
    name	parameters
    parm {
        name    "group"
        cppname "Group"
        label   "Group"
        type    string
        default { "" }
        range   { 0 1 }
        parmtag { "script_action" "import soputils\nkwargs['geometrytype'] = hou.geometryType.Primitives\nkwargs['inputindex'] = 0\nsoputils.selectGroupParm(kwargs)" }
        parmtag { "script_action_help" "Select geometry from an available viewport." }
        parmtag { "script_action_icon" "BUTTONS_reselect" }
    }
    parm {
        name    "delInLinePoints"
        cppname "DelInLinePoints"
        label   "Delete In-Line Points"
        type    toggle
        default { "1" }
    }
    parm {
        name    "threshold_delInLinePoints"
        cppname "Threshold_delInLinePoints"
        label   "Threshold Delete In-Line Points"
        type    angle
        default { "1e-05" }
        disablewhen "{ delInLinePoints == 0 }"
        range   { 0! 180! }
    }
    parm {
        name    "limitByGeoProperty"
        cppname "LimitByGeoProperty"
        label   "Limit by Geometry Property"
        type    toggle
        default { "1" }
    }
    parm {
        name    "geoPropertyType"
        cppname "GeoPropertyType"
        label   "Geometry Property Type"
        type    ordinal
        default { "dist" }
        menu {
            "angle" "Angle"
            "dist"  "Distance"
            "roc"   "Raidus of Curvature"
        }
    }
    parm {
        name    "maxAngle"
        cppname "MaxAngle"
        label   "Max Angle (Dot)"
        type    angle
        default { "150" }
        disablewhen "{ limitByGeoProperty == 0 } { geoPropertyType != angle }"
        range   { 90! 180! }
        parmtag { "units" "m1" }
    }
    parm {
        name    "maxDist"
        cppname "MaxDist"
        label   "Max Distance"
        type    log
        default { "0.0001" }
        disablewhen "{ limitByGeoProperty == 0 } { geoPropertyType != dist geoPropertyType != roc }"
        range   { 0.001 10 }
    }
    parm {
        name    "limitMinPoints"
        cppname "LimitMinPoints"
        label   "Minimum Points"
        type    toggle
        nolabel
        joinnext
        default { "off" }
    }
    parm {
        name    "minPoints"
        cppname "MinPoints"
        label   "Min Points"
        type    integer
        default { "10" }
        disablewhen "{ limitMinPoints == 0 }"
        range   { 1! 50 }
    }
    parm {
        name    "coverSourcePoly"
        cppname "CoverSourcePoly"
        label   "Cover Source Poly"
        type    toggle
        default { "off" }
    }
    parm {
        name    "polyReduce2D_GroupName"
        cppname "PolyReduce2D_GroupName"
        label   "Group Name"
        type    string
        default { "polyReduce2D" }
        disablewhen "{ delPoints == 1 }"
        parmtag { "script_action" "import soputils kwargs['geometrytype'] = hou.geometryType.Primitives kwargs['inputindex'] = 0 soputils.selectGroupParm(kwargs)" }
        parmtag { "script_action_help" "Select geometry from an available viewport." }
        parmtag { "script_action_icon" "BUTTONS_reselect" }
    }
    parm {
        name    "sepparm"
        label   "Separator"
        type    separator
        default { "" }
    }
    parm {
        name    "delPoints"
        cppname "DeletePoints"
        label   "Delete Points"
        type    toggle
        default { "on" }
    }
    parm {
        name    "delNonSelectedPoints"
        cppname "DelNonSelectedPoints"
        label   "Delete Non Selected"
        type    toggle
        default { "off" }
        disablewhen "{ delPoints == 0 }"
    }
    parm {
        name    "tmpAttrib_ndot"
        label   "Temp Attrib "
        type    string
        invisible
        default { [ "'__ndot_' + hou.node('.').type().nameComponents()[2]" python ] }
    }
    parm {
        name    "tmpAttrib_ndir"
        label   "Temp Attrib "
        type    string
        invisible
        default { [ "'__ndir_' + hou.node('.').type().nameComponents()[2]" python ] }
    }
    parm {
        name    "tmpAttrib_lastid"
        label   "Temp Attrib lastid"
        type    string
        invisible
        default { [ "'__lastid_' + hou.node('.').type().nameComponents()[2]" python ] }
    }
    parm {
        name    "tmpAttrib_nextid"
        label   "Temp Attrib nextid"
        type    string
        invisible
        default { [ "'__nextid_' + hou.node('.').type().nameComponents()[2]" python ] }
    }
    parm {
        name    "tmpAttrib_lastdist"
        label   "Temp Attrib "
        type    string
        invisible
        default { [ "'__lastdist_' + hou.node('.').type().nameComponents()[2]" python ] }
    }
    parm {
        name    "tmpAttrib_nextdist"
        label   "Temp Attrib "
        type    string
        invisible
        default { [ "'__nextdist_' + hou.node('.').type().nameComponents()[2]" python ] }
    }
    parm {
        name    "tmpGroup_reversedPrim"
        label   "Temp Group reversedPrim"
        type    string
        invisible
        default { [ "'__reversedPrim_' + hou.node('.').type().nameComponents()[2]" python ] }
    }
    parm {
        name    "tmpGroup_polyReduce2D"
        label   "Temp Group polyReduce2D"
        type    string
        invisible
        default { [ "'__outgrp_' + hou.node('.').type().nameComponents()[2]" python ] }
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
SOP_FeE_PolyReduce2D_4_0::buildTemplates()
{
    static PRM_TemplateBuilder templ("SOP_FeE_PolyReduce2D_4_0.C"_sh, theDsFile);
    if (templ.justBuilt())
    {
        templ.setChoiceListPtr("primGroup"_sh, &SOP_Node::primGroupMenu);
    }
    return templ.templates();
}





const UT_StringHolder SOP_FeE_PolyReduce2D_4_0::theSOPTypeName("FeE::polyReduce2d::4.0"_sh);

void
newSopOperator(OP_OperatorTable* table)
{
    OP_Operator* newOp = new OP_Operator(
        SOP_FeE_PolyReduce2D_4_0::theSOPTypeName,
        "FeE Poly Reduce 2D",
        SOP_FeE_PolyReduce2D_4_0::myConstructor,
        SOP_FeE_PolyReduce2D_4_0::buildTemplates(),
        2,
        2,
        nullptr,
        OP_FLAG_GENERATOR,
        nullptr,
        1,
        "Five elements Elf/Topo/Optimize");

    newOp->setIconName("SOP_edgedivide");
    table->addOperator(newOp);
}




class SOP_FeE_PolyReduce2D_4_0Verb : public SOP_NodeVerb
{
public:
    SOP_FeE_PolyReduce2D_4_0Verb() {}
    virtual ~SOP_FeE_PolyReduce2D_4_0Verb() {}

    virtual SOP_NodeParms *allocParms() const { return new SOP_FeE_PolyReduce2D_4_0Parms(); }
    virtual UT_StringHolder name() const { return SOP_FeE_PolyReduce2D_4_0::theSOPTypeName; }

    virtual CookMode cookMode(const SOP_NodeParms *parms) const { return COOK_GENERIC; }

    virtual void cook(const CookParms &cookparms) const;
    
    static const SOP_NodeVerb::Register<SOP_FeE_PolyReduce2D_4_0Verb> theVerb;
};

const SOP_NodeVerb::Register<SOP_FeE_PolyReduce2D_4_0Verb> SOP_FeE_PolyReduce2D_4_0Verb::theVerb;

const SOP_NodeVerb *
SOP_FeE_PolyReduce2D_4_0::cookVerb() const 
{ 
    return SOP_FeE_PolyReduce2D_4_0Verb::theVerb.get();
}


void
SOP_FeE_PolyReduce2D_4_0Verb::cook(const SOP_NodeVerb::CookParms& cookparms) const
{
    auto&& sopparms = cookparms.parms<SOP_FeE_PolyReduce2D_4_0Parms>();
    GEO_Detail* outGeo0 = cookparms.gdh().gdpNC();
    //auto sopcache = (SOP_FeE_PolyReduce2D_4_0Cache*)cookparms.cache();

    const GEO_Detail* const inGeo0 = cookparms.inputGeo(0);
    const GEO_Detail* const inGeo1 = cookparms.inputGeo(1);



    GU_DetailHandle tmpGeoH0;
    GU_Detail* tmpGeo0 = new GU_Detail();
    tmpGeoH0.allocateAndSet(tmpGeo0);
    tmpGeo0->replaceWith(*inGeo0);


    outGeo0->replaceWithPoints(*inGeo0);
    //outGeo0->replaceWith(*inGeo0);

    //GA_PointGroup* groupOneNeb = GA_FeE_TopologyReference::addGroupOneNeb(outGeo0, nullptr);

    
    GOP_Manager gop;
    const GA_PrimitiveGroup* const primGroup = GA_FeE_Group::findOrParsePrimitiveGroupDetached(cookparms, outGeo0, sopparms.getPrimGroup(), gop);
    if (primGroup && primGroup->isEmpty())
        return;


    const exint subscribeRatio = sopparms.getSubscribeRatio();
    const exint minGrainSize = sopparms.getMinGrainSize();


    const GA_Storage inStorageI = GA_FeE_Type::getPreferredStorageI(outGeo0);

    UT_AutoInterrupt boss("Processing");
    if (boss.wasInterrupted())
        return;
    


    fpreal64 threshold = sopparms.getThreshold();



    outGeo0->bumpDataIdsForAddOrRemove(1, 1, 1);
    tmpGeoH0.deleteGdp();

}



namespace SOP_FeE_PolyReduce2D_4_0_Namespace {

} // End SOP_FeE_PolyReduce2D_4_0_Namespace namespace
