
//#define UT_ASSERT_LEVEL 3
#include "SOP_FeE_SkinIntersectCurve_2_0.h"


#include "SOP_FeE_SkinIntersectCurve_2_0.proto.h"

#include "GA/GA_Detail.h"
#include "PRM/PRM_TemplateBuilder.h"
#include "UT/UT_Interrupt.h"
#include "UT/UT_DSOVersion.h"

#include "GU/GU_IntersectionAnalysis.h"
#include "GU/GU_PrimMesh.h"




#include "GFE/GFE_GroupParser.h"



using namespace SOP_FeE_SkinIntersectCurve_2_0_Namespace;


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
    }
    parm {
        name    "origPointAttribName"
        cppname "OrigPointAttribName"
        label   "Origin Point Attribute Name"
        type    string
        default { "origPoint" }
        disablewhen "{ createOrigPointAttrib == 0 }"
    }
    parm {
        name    "sepparm"
        label   "Separator"
        type    separator
        default { "" }
    }


    parm {
        name    "delInputPointGroup"
        cppname "DelInputPointGroup"
        label   "Delete Input Point Group"
        type    toggle
        default { "0" }
    }

    parm {
        name    "threshold"
        cppname "Threshold"
        label   "Threshold"
        type    log
        default { "1e-05" }
        range { 0! 1 }
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
SOP_FeE_SkinIntersectCurve_2_0::buildTemplates()
{
    static PRM_TemplateBuilder templ("SOP_FeE_SkinIntersectCurve_2_0.C"_sh, theDsFile);
    if (templ.justBuilt())
    {
        templ.setChoiceListPtr("primGroup"_sh, &SOP_Node::primGroupMenu);
    }
    return templ.templates();
}





const UT_StringHolder SOP_FeE_SkinIntersectCurve_2_0::theSOPTypeName("FeE::skinIntersectCurve::2.0"_sh);

void
newSopOperator(OP_OperatorTable* table)
{
    OP_Operator* newOp = new OP_Operator(
        SOP_FeE_SkinIntersectCurve_2_0::theSOPTypeName,
        "FeE Skin Intersect Curve",
        SOP_FeE_SkinIntersectCurve_2_0::myConstructor,
        SOP_FeE_SkinIntersectCurve_2_0::buildTemplates(),
        2,
        2,
        nullptr,
        OP_FLAG_GENERATOR,
        nullptr,
        1,
        "Five elements Elf/Generator/Curve to Poly");

    newOp->setIconName("SOP_skin");
    table->addOperator(newOp);
}




class SOP_FeE_SkinIntersectCurve_2_0Verb : public SOP_NodeVerb
{
public:
    SOP_FeE_SkinIntersectCurve_2_0Verb() {}
    virtual ~SOP_FeE_SkinIntersectCurve_2_0Verb() {}

    virtual SOP_NodeParms *allocParms() const { return new SOP_FeE_SkinIntersectCurve_2_0Parms(); }
    virtual UT_StringHolder name() const { return SOP_FeE_SkinIntersectCurve_2_0::theSOPTypeName; }

    virtual CookMode cookMode(const SOP_NodeParms *parms) const { return COOK_GENERIC; }

    virtual void cook(const CookParms &cookparms) const;
    
    static const SOP_NodeVerb::Register<SOP_FeE_SkinIntersectCurve_2_0Verb> theVerb;
};

const SOP_NodeVerb::Register<SOP_FeE_SkinIntersectCurve_2_0Verb> SOP_FeE_SkinIntersectCurve_2_0Verb::theVerb;

const SOP_NodeVerb *
SOP_FeE_SkinIntersectCurve_2_0::cookVerb() const 
{ 
    return SOP_FeE_SkinIntersectCurve_2_0Verb::theVerb.get();
}


void
SOP_FeE_SkinIntersectCurve_2_0Verb::cook(const SOP_NodeVerb::CookParms& cookparms) const
{
    auto&& sopparms = cookparms.parms<SOP_FeE_SkinIntersectCurve_2_0Parms>();
    GA_Detail* const outGeo0 = cookparms.gdh().gdpNC();
    //auto sopcache = (SOP_FeE_SkinIntersectCurve_2_0Cache*)cookparms.cache();

    const GA_Detail* const inGeo0 = cookparms.inputGeo(0);
    const GA_Detail* const inGeo1 = cookparms.inputGeo(1);



    GU_DetailHandle tmpGeoH0;
    GU_Detail* tmpGeo0 = new GU_Detail();
    tmpGeoH0.allocateAndSet(tmpGeo0);
    tmpGeo0->replaceWith(*inGeo0);


    outGeo0->replaceWithPoints(*inGeo0);
    //outGeo0->replaceWith(*inGeo0);

    //GA_PointGroup* groupOneNeb = GFE_TopologyReference::addGroupOneNeb(outGeo0, nullptr);

    
    GOP_Manager gop;
    const GA_PrimitiveGroup* const primGroup = GFE_GroupParser_Namespace::findOrParsePrimitiveGroupDetached(cookparms, outGeo0, sopparms.getPrimGroup(), gop);
    if (primGroup && primGroup->isEmpty())
        return;


    const exint subscribeRatio = sopparms.getSubscribeRatio();
    const exint minGrainSize = sopparms.getMinGrainSize();


    const GA_Storage inStorageI = GFE_Type::getPreferredStorageI(outGeo0);

    UT_AutoInterrupt boss("Processing");
    if (boss.wasInterrupted())
        return;
    


    fpreal64 threshold = sopparms.getThreshold();

    //UT_Array<UT_Int32Array>*    input_num_arr = nullptr;
    //UT_Array<UT_Int64Array>*    prim_num_arr  = nullptr;
    //UT_Array<UT_Fpreal32Array>* prim_uvw_arr  = nullptr;
    UT_Array<UT_Int32Array>    input_num_arr;
    UT_Array<UT_Int64Array>    prim_num_arr;
    UT_Array<UT_Fpreal32Array> prim_uvw_arr;
    UT_Array<UT_Int64Array>*    pt_num_arr    = nullptr;
    UT_Vector3DArray intersectionPoses = GU_IntersectionAnalysis::findIntersectionPoints(
        static_cast<const GEO_Detail*>(inGeo0), NULL,
        static_cast<const GEO_Detail*>(inGeo1), NULL,
        threshold,
        //input_num_arr,
        //prim_num_arr,
        //prim_uvw_arr,
        &input_num_arr,
        &prim_num_arr,
        &prim_uvw_arr,
        pt_num_arr
    );

    for (exint i = intersectionPoses.size() - 1; i >= 0; i--)
    {
        float u = prim_uvw_arr[0][0];
        if (u < 1 && u > 0) continue;
        intersectionPoses.removeIndex(i);
        input_num_arr.removeIndex(i);
        prim_num_arr.removeIndex(i);
        prim_uvw_arr.removeIndex(i);
    }




    outGeo0->bumpDataIdsForAddOrRemove(1, 1, 1);
    tmpGeoH0.deleteGdp();

}



namespace SOP_FeE_SkinIntersectCurve_2_0_Namespace {

} // End SOP_FeE_SkinIntersectCurve_2_0_Namespace namespace
