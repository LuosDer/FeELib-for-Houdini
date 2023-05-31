
//#define UT_ASSERT_LEVEL 3
#include "SOP_FeE_Triangulate2D_2_0.h"


#include "SOP_FeE_Triangulate2D_2_0.proto.h"

#include "GA/GA_Detail.h"
#include "PRM/PRM_TemplateBuilder.h"
#include "UT/UT_Interrupt.h"
#include "UT/UT_DSOVersion.h"




#include "GFE/GFE_Triangulate2D.h"




using namespace SOP_FeE_Triangulate2D_2_0_Namespace;


static const char *theDsFile = R"THEDSFILE(
{
    name        parameters


    parm {
        name    "outSrcPrims"
        cppname "OutSrcPrims"
        label   "Source Prims"
        type    toggle
        default { 0 }
        nolabel
        joinnext
    }
    parm {
        name    "srcPrimsAttribName"
        cppname "SrcPrimsAttribName"
        label   "Source Prims Attrib Name"
        type    string
        default { "srcPrims" }
        disablewhen "{ outSrcPrims == 0 }"
    }



    parm {
        name    "primType"
        cppname "PrimType"
        label   "Prim Type"
        type    ordinal
        default { "polyline" }
        menu {
            "polyline"  "Polyline"
            "poly"      "Poly"
        }
    }

    parm {
        name    "excludeSharedEdge"
        cppname "ExcludeSharedEdge"
        label   "Exclude Shared Edge"
        type    toggle
        default { "0" }
    }
    parm {
        name    "projectplane"
        label   "Project Plane"
        type    ordinal
        default { "zx" }
        menu {
            "xy"    "XY Plane"
            "yz"    "YZ Plane"
            "zx"    "ZX Plane"
        }
        range   { 0 1 }
    }
    parm {
        name    "constraint_unshared"
        label   "Constraint Unshared"
        type    toggle
        default { "0" }
    }
    parm {
        name    "keepOutside"
        label   "Keep Outside"
        type    toggle
        default { "off" }
    }
    parm {
        name    "copyFirstElemAttribname"
        label   "Copy First Elem Attrib Name"
        type    string
        default { "" }
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
SOP_FeE_Triangulate2D_2_0::buildTemplates()
{
    static PRM_TemplateBuilder templ("SOP_FeE_Triangulate2D_2_0.C"_sh, theDsFile);
    if (templ.justBuilt())
    {
        //templ.setChoiceListPtr("primGroup"_sh, &SOP_Node::primGroupMenu);
    }
    return templ.templates();
}

const UT_StringHolder SOP_FeE_Triangulate2D_2_0::theSOPTypeName("FeE::triangulate2d::2.0"_sh);

void
newSopOperator(OP_OperatorTable* table)
{
    OP_Operator* newOp = new OP_Operator(
        SOP_FeE_Triangulate2D_2_0::theSOPTypeName,
        "FeE Triangulate 2D",
        SOP_FeE_Triangulate2D_2_0::myConstructor,
        SOP_FeE_Triangulate2D_2_0::buildTemplates(),
        1,
        1,
        nullptr,
        OP_FLAG_GENERATOR,
        nullptr,
        1,
        "Five elements Elf/Filter/Topo/Artificial Line");

    newOp->setIconName("SOP_triangulate2d-2.0");
    table->addOperator(newOp);

}





class SOP_FeE_Triangulate2D_2_0Verb : public SOP_NodeVerb
{
public:
    SOP_FeE_Triangulate2D_2_0Verb() {}
    virtual ~SOP_FeE_Triangulate2D_2_0Verb() {}

    virtual SOP_NodeParms *allocParms() const { return new SOP_FeE_Triangulate2D_2_0Parms(); }
    virtual UT_StringHolder name() const { return SOP_FeE_Triangulate2D_2_0::theSOPTypeName; }

    virtual CookMode cookMode(const SOP_NodeParms *parms) const { return COOK_GENERIC; }

    virtual void cook(const CookParms &cookparms) const;
    
    /// This static data member automatically registers
    /// this verb class at library load time.
    static const SOP_NodeVerb::Register<SOP_FeE_Triangulate2D_2_0Verb> theVerb;
};

// The static member variable definition has to be outside the class definition.
// The declaration is inside the class.
const SOP_NodeVerb::Register<SOP_FeE_Triangulate2D_2_0Verb> SOP_FeE_Triangulate2D_2_0Verb::theVerb;

const SOP_NodeVerb *
SOP_FeE_Triangulate2D_2_0::cookVerb() const 
{ 
    return SOP_FeE_Triangulate2D_2_0Verb::theVerb.get();
}






void
SOP_FeE_Triangulate2D_2_0Verb::cook(const SOP_NodeVerb::CookParms &cookparms) const
{
    auto&& sopparms = cookparms.parms<SOP_FeE_Triangulate2D_2_0Parms>();
    GA_Detail& outGeo0 = *cookparms.gdh().gdpNC();
    //auto sopcache = (SOP_FeE_Triangulate2D_2_0Cache*)cookparms.cache();

    const GA_Detail& inGeo0 = *cookparms.inputGeo(0);

    outGeo0.replaceWith(inGeo0);


    GU_DetailHandle geoTmp_h;
    GU_Detail* geoTmp = new GU_Detail();
    geoTmp_h.allocateAndSet(geoTmp);
    geoTmp->replaceWith(inGeo0);

    UT_AutoInterrupt boss("Processing");
    if (boss.wasInterrupted())
        return;


    GFE_Triangulate2D triangulat2D(outGeo0, cookparms);

    triangulat2D.setComputeParm(sopparms.getPreFusePoint(), sopparms.getFuseDist(),
        sopparms.getSubscribeRatio(), sopparms.getMinGrainSize());

    
    triangulat2D.setGroup.setComputeParm(groupMergeType, sopparms.getGroup());
    triangulat2D.groupParser.setGroup(groupType, sopparms.getGroup());
    triangulat2D.findOrCreatePointGroup(false, sopparms.getOneNebGroupName());

    triangulat2D.computeAndBumpDataId();



}


