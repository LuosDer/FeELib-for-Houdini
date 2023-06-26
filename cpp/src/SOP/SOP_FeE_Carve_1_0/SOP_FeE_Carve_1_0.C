
//#define UT_ASSERT_LEVEL 3
#include "SOP_FeE_Carve_1_0.h"


#include "SOP_FeE_Carve_1_0.proto.h"

#include "GA/GA_Detail.h"
#include "PRM/PRM_TemplateBuilder.h"
#include "UT/UT_Interrupt.h"
#include "UT/UT_DSOVersion.h"



#include "GFE/GFE_Carve.h"


using namespace SOP_FeE_Carve_1_0_Namespace;


static const char *theDsFile = R"THEDSFILE(
{
    name	parameters
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
        default { "prim" }
        menu {
            "guess"     "Guess from Group"
            "prim"      "Primitive"
            "point"     "Point"
            "vertex"    "Vertex"
            "edge"      "Edge"
        }
    }
    parm {
        name    "carveStartPrimGroup"
        cppname "CarveStartPrimGroup"
        label   "Carve Start Prim Group"
        type    string
        default { "" }
        parmtag { "script_action" "import soputils\nkwargs['geometrytype'] = (hou.geometryType.Primitives,)\nkwargs['inputindex'] = 0\nsoputils.selectGroupParm(kwargs)" }
        parmtag { "script_action_help" "Select geometry from an available viewport.\nShift-click to turn on Select Groups." }
        parmtag { "script_action_icon" "BUTTONS_reselect" }
    }
    parm {
        name    "carveEndPrimGroup"
        cppname "CarveEndPrimGroup"
        label   "Carve End Prim Group"
        type    string
        default { "" }
        parmtag { "script_action" "import soputils\nkwargs['geometrytype'] = (hou.geometryType.Primitives,)\nkwargs['inputindex'] = 0\nsoputils.selectGroupParm(kwargs)" }
        parmtag { "script_action_help" "Select geometry from an available viewport.\nShift-click to turn on Select Groups." }
        parmtag { "script_action_icon" "BUTTONS_reselect" }
    }

    parm {
        name    "carveSpace"
        cppname "CarveSpace"
        label   "Carve Space"
        type    ordinal
        default { "worldArcLength" }
        menu {
            "customAttrib"       "CustomAttrib"
            "localAverage"       "Local Average"
            "localArcLength"     "Local Arc Length"
            "worldAverage"       "World Average"
            "worldArcLength"     "World Arc Length"
        }
    }
    parm {
        name    "customCarveUVAttribName"
        cppname "CustomCarveUVAttribName"
        label   "Custom Carve UV Attrib Name"
        type    string
        default { "" }
        disablewhen "{ carveSpace != customAttrib }"
    }
    parm {
        name    "sepparm"
        label   "Separator"
        type    separator
        default { "" }
    }
    parm {
        name    "startCarveULocal"
        cppname "StartCarveULocal"
        label   "Start Carve U Local"
        type    float
        default { "0.25" }
        hidewhen "{ carveSpace != localAverage carveSpace != localArcLength }"
        range   { 0! 1! }
    }
    parm {
        name    "startCarveUWorld"
        cppname "StartCarveUWorld"
        label   "Start Carve U Local World"
        type    log
        default { "0.25" }
        hidewhen "{ carveSpace == localAverage } { carveSpace == localArcLength }"
        range   { 0.01 100 }
    }
    parm {
        name    "startCarveUAttrib"
        cppname "StartCarveUAttrib"
        label   "Start CarveU Attrib"
        type    string
        default { "startCarveU" }
    }
    parm {
        name    "absCarveUEnd"
        cppname "AbsCarveUEnd"
        label   "Absolute Carve U End"
        type    toggle
        default { "0" }
    }
    parm {
        name    "endCarveULocal"
        cppname "EndCarveULocal"
        label   "End Carve U Local"
        type    float
        default { "0.75" }
        hidewhen "{ carveSpace != localAverage carveSpace != localArcLength }"
        range   { 0! 1! }
    }
    parm {
        name    "endCarveUWorld"
        cppname "EndCarveUWorld"
        label   "End Carve U World"
        type    log
        default { "0.25" }
        hidewhen "{ carveSpace == localAverage } { carveSpace == localArcLength }"
        range   { 0.01 100 }
    }
    parm {
        name    "useEndCarveUAttrib"
        cppname "UseEndCarveUAttrib"
        label   "Use End CarveU Attrib"
        type    toggle
        default { "0" }
    }
    parm {
        name    "endCarveUAttrib"
        cppname "EndCarveUAttrib"
        label   "End CarveU Attrib"
        type    string
        hidewhen "{ useEndCarveUAttrib == 1 }"
        default { "endCarveU" }
    }

    parm {
        name    "sepparm3"
        label   "Separator"
        type    separator
        default { "" }
    }





    parm {
        name    "keepOutsideStart"
        cppname "KeepOutsideStart"
        label   "Keep Outside Start"
        type    toggle
        default { "0" }
    }
    groupsimple {
        name    "outsideStart_folder"
        label   "Outside Start"
        disablewhen "{ keepOutsideStart == 0 }"
        grouptag { "group_type" "simple" }

        parm {
            name    "outBreakPointGroupOutsideStart"
            cppname "OutBreakPointGroupOutsideStart"
            label   "Output Break Point Group Outside Start"
            type    toggle
            nolabel
            joinnext
            default { "0" }
        }
        parm {
            name    "breakPointGroupOutsideStart"
            cppname "BreakPointGroupOutsideStart"
            label   "Break Point Group Outside Start"
            type    string
            default { "breakpt" }
            disablewhen "{ outBreakPointGroupOutsideStart == 0 }"
        }
        parm {
            name    "outPrimGroupOutsideStart"
            cppname "OutPrimGroupOutsideStart"
            label   "Output Prim Group Outside Start"
            type    toggle
            nolabel
            joinnext
            default { "0" }
        }
        parm {
            name    "primGroupOutsideStart"
            cppname "PrimGroupOutsideStart"
            label   "Prim Group Outside Start"
            type    string
            default { "outside" }
            disablewhen "{ outPrimGroupOutsideStart == 0 }"
        }
        parm {
            name    "reverseOutsideStart"
            cppname "ReverseOutsideStart"
            label   "Reverse Outside Start"
            type    toggle
            default { "0" }
        }
    }

    parm {
        name    "keepOutsideEnd"
        cppname "KeepOutsideEnd"
        label   "Keep Outside End"
        type    toggle
        default { "0" }
    }
    groupsimple {
        name    "outsideEnd_folder"
        label   "Outside End"
        disablewhen "{ keepOutsideEnd == 0 }"
        grouptag { "group_type" "simple" }

        parm {
            name    "outBreakPointGroupOutsideEnd"
            cppname "OutBreakPointGroupOutsideEnd"
            label   "Output Break Point Group Outside End"
            type    toggle
            nolabel
            joinnext
            default { "ch('outBreakPointGroupOutsideStart')" }
        }
        parm {
            name    "breakPointGroupOutsideEnd"
            cppname "BreakPointGroupOutsideEnd"
            label   "Break Point Group Outside End"
            type    string
            default { "chs('outBreakPointGroupOutsideStart')" }
            disablewhen "{ outBreakPointGroupOutsideEnd == 0 }"
        }
        parm {
            name    "outPrimGroupOutsideEnd"
            cppname "OutPrimGroupOutsideEnd"
            label   "Output Prim Group Outside End"
            type    toggle
            nolabel
            joinnext
            default { "ch('outPrimGroupOutsideStart')" }
        }
        parm {
            name    "primGroupOutsideEnd"
            cppname "PrimGroupOutsideEnd"
            label   "Prim Group Outside End"
            type    string
            default { "chs('primGroupOutsideStart')" }
            disablewhen "{ outPrimGroupOutsideEnd == 0 }"
        }
        parm {
            name    "reverseOutsideEnd"
            cppname "ReverseOutsideEnd"
            label   "Reverse Outside End"
            type    toggle
            default { "0" }
        }
    }

    parm {
        name    "keepInside"
        cppname "KeepInside"
        label   "Keep Inside"
        type    toggle
        default { "1" }
    }
    groupsimple {
        name    "inside_folder"
        label   "Inside"
        disablewhen "{ keepInside == 0 }"
        grouptag { "group_type" "simple" }

        parm {
            name    "outBreakPointGroupInsideStart"
            cppname "OutBreakPointGroupInsideStart"
            label   "Output Break Point Group Inside Start"
            type    toggle
            nolabel
            joinnext
            default { "ch('outBreakPointGroupOutsideStart')" }
        }
        parm {
            name    "breakPointGroupInsideStart"
            cppname "BreakPointGroupInsideStart"
            label   "Break Point Group Inside Start"
            type    string
            default { "chs('breakPointGroupOutsideStart')" }
            disablewhen "{ outBreakPointGroupInsideStart == 0 }"
        }
        parm {
            name    "outBreakPointGroupInsideEnd"
            cppname "OutBreakPointGroupInsideEnd"
            label   "Output Break Point Group Inside End"
            type    toggle
            nolabel
            joinnext
            default { "ch('outBreakPointGroupInsideStart')" }
        }
        parm {
            name    "breakPointGroupInsideEnd"
            cppname "BreakPointGroupInsideEnd"
            label   "Break Point Group Inside End"
            type    string
            default { "chs('breakPointGroupInsideStart')" }
            disablewhen "{ outBreakPointGroupInsideEnd == 0 }"
        }
        parm {
            name    "outPrimGroupInside"
            cppname "OutPrimGroupInside"
            label   "Output Prim Group Inside"
            type    toggle
            nolabel
            joinnext
            default { "ch('outPrimGroupOutsideStart')" }
        }
        parm {
            name    "primGroupInside"
            cppname "PrimGroupInside"
            label   "Output Prim Group Inside"
            type    string
            default { "inside" }
            disablewhen "{ outPrimGroupInside == 0 }"
        }
        parm {
            name    "delReversedInsidePrim"
            cppname "DelReversedInsidePrim"
            label   "Delete Reversed Inside Prim"
            type    toggle
            default { "0" }
            disablewhen "{ keepInside == 0 }"
        }
    }



    parm {
        name    "outSrcPointAttrib"
        cppname "OutSrcPointAttrib"
        label   "Output Source Point Attrib"
        type    toggle
        nolabel
        joinnext
        default { "0" }
    }
    parm {
        name    "srcPointAttribName"
        cppname "SrcPointAttribName"
        label   "Source Point Attrib Name"
        type    string
        default { "srcPoint" }
        disablewhen "{ outSrcPointAttrib == 0 }"
    }

    parm {
        name    "outSrcPrimAttrib"
        cppname "OutSrcPrimAttrib"
        label   "Output Source Prim Attrib"
        type    toggle
        nolabel
        joinnext
        default { "0" }
    }
    parm {
        name    "srcPrimAttribName"
        cppname "SrcPrimAttribName"
        label   "Source Prim Attrib Name"
        type    string
        default { "srcPrim" }
        disablewhen "{ outSrcPrimAttrib == 0 }"
    }


    parm {
        name    "interpAttrib"
        cppname "InterpAttrib"
        label   "Interp Attrib"
        type    string
        default { "P" }
    }



    parm {
        name    "sortPointOrder"
        cppname "SortPointOrder"
        label   "Sort Points Order"
        type    toggle
        default { "0" }
    }
    parm {
        name    "sortPrimOrder"
        cppname "SortPrimOrder"
        label   "Sort Primitive Order"
        type    toggle
        default { "0" }
    }
    parm {
        name    "delStartCarveUAttrib"
        cppname "DelStartCarveUAttrib"
        label   "Delete Start Carve U Attrib"
        type    toggle
        default { "1" }
    }
    parm {
        name    "delEndCarveUAttrib"
        cppname "DelEndCarveUAttrib"
        label   "Delete End CarveU Attrib"
        type    toggle
        default { "ch('del_startCarveU_attrib')" }
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
SOP_FeE_Carve_1_0::buildTemplates()
{
    static PRM_TemplateBuilder templ("SOP_FeE_Carve_1_0.C"_sh, theDsFile);
    if (templ.justBuilt())
    {
        templ.setChoiceListPtr("primGroup"_sh,            &SOP_Node::primGroupMenu);
        templ.setChoiceListPtr("carveStartPrimGroup"_sh,  &SOP_Node::primGroupMenu);
        templ.setChoiceListPtr("carveEndPrimGroup"_sh,    &SOP_Node::primGroupMenu);

        templ.setChoiceListPtr("interpAttrib"_sh,         &SOP_Node::allAttribReplaceMenu);
    }
    return templ.templates();
}





const UT_StringHolder SOP_FeE_Carve_1_0::theSOPTypeName("FeE::carve::1.0"_sh);

void
newSopOperator(OP_OperatorTable* table)
{
    OP_Operator* newOp = new OP_Operator(
        SOP_FeE_Carve_1_0::theSOPTypeName,
        "FeE Carve",
        SOP_FeE_Carve_1_0::myConstructor,
        SOP_FeE_Carve_1_0::buildTemplates(),
        1,
        1,
        nullptr,
        OP_FLAG_GENERATOR,
        nullptr,
        1,
        "Five elements Elf/Fracture/Clip");

    newOp->setIconName("SOP_carve");
    table->addOperator(newOp);
}




class SOP_FeE_Carve_1_0Verb : public SOP_NodeVerb
{
public:
    SOP_FeE_Carve_1_0Verb() {}
    virtual ~SOP_FeE_Carve_1_0Verb() {}

    virtual SOP_NodeParms *allocParms() const { return new SOP_FeE_Carve_1_0Parms(); }
    virtual UT_StringHolder name() const { return SOP_FeE_Carve_1_0::theSOPTypeName; }

    virtual CookMode cookMode(const SOP_NodeParms *parms) const { return COOK_GENERIC; }

    virtual void cook(const CookParms &cookparms) const;
    
    static const SOP_NodeVerb::Register<SOP_FeE_Carve_1_0Verb> theVerb;
};

const SOP_NodeVerb::Register<SOP_FeE_Carve_1_0Verb> SOP_FeE_Carve_1_0Verb::theVerb;

const SOP_NodeVerb *
SOP_FeE_Carve_1_0::cookVerb() const 
{ 
    return SOP_FeE_Carve_1_0Verb::theVerb.get();
}





 

static GA_GroupType
sopGroupType(SOP_FeE_Carve_1_0Parms::GroupType parmGroupType)
{
    using namespace SOP_FeE_Carve_1_0Enums;
    switch (parmGroupType)
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


static GFE_CarveSpace
sopCarveSpace(SOP_FeE_Carve_1_0Parms::CarveSpace parmgrouptype)
{
    using namespace SOP_FeE_Carve_1_0Enums;
    switch (parmgrouptype)
    {
    case CarveSpace::CUSTOMATTRIB:     return GFE_CarveSpace::CustomAttrib;    break;
    case CarveSpace::LOCALAVERAGE:     return GFE_CarveSpace::LocalAverage;    break;
    case CarveSpace::LOCALARCLENGTH:   return GFE_CarveSpace::LocalArcLength;  break;
    case CarveSpace::WORLDAVERAGE:     return GFE_CarveSpace::WorldAverage;    break;
    case CarveSpace::WORLDARCLENGTH:   return GFE_CarveSpace::WorldArcLength;  break;
    }
    UT_ASSERT_MSG(0, "Unhandled Carve Space!");
    return GFE_CarveSpace::WorldArcLength;
}


void
SOP_FeE_Carve_1_0Verb::cook(const SOP_NodeVerb::CookParms& cookparms) const
{
    auto&& sopparms = cookparms.parms<SOP_FeE_Carve_1_0Parms>();
    GA_Detail& outGeo0 = *cookparms.gdh().gdpNC();
    //auto sopcache = (SOP_FeE_Carve_1_0Cache*)cookparms.cache();

    const GA_Detail& inGeo0 = *cookparms.inputGeo(0);

    outGeo0.replaceWith(inGeo0);

    const GFE_CarveSpace carveSpace = sopCarveSpace(sopparms.getCarveSpace());
    
    const UT_StringHolder& customCarveUVAttribName = sopparms.getCustomCarveUVAttribName();

    const GA_GroupType groupType = sopGroupType(sopparms.getGroupType());
    
    UT_AutoInterrupt boss("Processing");
    if (boss.wasInterrupted())
        return;
    
    GFE_Carve carve(outGeo0, &cookparms);
    
    carve.setGroup(sopparms.getPrimGroup());

    carve.setComputeParm(carveSpace,
        sopparms.getKeepOutsideStart(), sopparms.getKeepOutsideEnd(), sopparms.getKeepInside(),
        sopparms.getAbsCarveUEnd());

    carve.setCarveU<true>(sopparms.getEndCarveULocal(), sopparms.getEndCarveUWorld());
    carve.setCarveU<false>(sopparms.getStartCarveULocal(), sopparms.getStartCarveUWorld());

    carve.setUVAttrib(customCarveUVAttribName);

    carve.setStartCarveUAttrib(sopparms.getStartCarveUAttrib(), sopparms.getDelStartCarveUAttrib());
    carve.setEndCarveUAttrib(  sopparms.getEndCarveUAttrib(), sopparms.getDelEndCarveUAttrib());

    carve.setCarveStartGroup(sopparms.getCarveStartPrimGroup());
    carve.setCarveEndGroup(sopparms.getCarveEndPrimGroup());

    if (sopparms.getOutSrcPrimAttrib())
        carve.createSrcPrimAttrib(sopparms.getSrcPrimAttribName());

    if (sopparms.getOutSrcPointAttrib())
        carve.createSrcPointAttrib(sopparms.getSrcPrimAttribName());

    carve.getOutAttribArray().appendPointVertexs(sopparms.getInterpAttrib());

    carve.computeAndBumpDataIdsForAddOrRemove();

    carve.visualizeOutGroup();
    


}


