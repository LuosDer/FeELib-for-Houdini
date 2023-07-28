
#include "SOP_FeE_VolumeProject_1_0.h"
#include "SOP_FeE_VolumeProject_1_0.proto.h"

#include <UT/UT_VoxelArray.h>
#include <UT/UT_ParallelUtil.h>
#include <UT/UT_StopWatch.h>

#include <GU/GU_Detail.h>
#include <GU/GU_PrimVolume.h>

#include <PRM/PRM_Include.h>
#include <PRM/PRM_TemplateBuilder.h>
#include <UT/UT_DSOVersion.h>



#include "GFE/GFE_VolumeProject.h"


using namespace SOP_FeE_VolumeProject_1_0_Namespace;

void
newSopOperator(OP_OperatorTable *table)
{
	OP_Operator* newOp = new OP_Operator(
		SOP_FeE_VolumeProject_1_0::theSOPTypeName,
		"FeE Volume Project",
		SOP_FeE_VolumeProject_1_0::myConstructor,
		SOP_FeE_VolumeProject_1_0::buildTemplates(),
		3,
		4,
		nullptr,
		OP_FLAG_GENERATOR,
		nullptr,
		1,
		"Five elements Elf/Data/Orient");

	newOp->setIconName("SOP_uvtransform-2.0");
	table->addOperator(newOp);
}

static const char *theDsFile = R"THEDSFILE(
{
    name	volumefft

    parm {
		name	"group"
		label	"Velocity Volumes"
		type	string
		default	{ "" }
		parmtag	{ "script_action" "import soputils\nkwargs['geometrytype'] = (hou.geometryType.Primitives,)\nkwargs['inputindex'] = 0\nsoputils.selectGroupParm(kwargs)" }
		parmtag	{ "script_action_help" "Select geometry from an available viewport.\nShift-click to turn on Select Groups." }
		parmtag	{ "script_action_icon" "BUTTONS_reselect" }
    }
    parm {
		name	"divgroup"
		cppname	"DivGroup"
		label	"Divergence Volume"
		type	string
		default	{ "" }
		parmtag	{ "script_action" "import soputils\nkwargs['geometrytype'] = (hou.geometryType.Primitives,)\nkwargs['inputindex'] = 1\nsoputils.selectGroupParm(kwargs)" }
		parmtag	{ "script_action_help" "Select geometry from an available viewport.\nShift-click to turn on Select Groups." }
		parmtag	{ "script_action_icon" "BUTTONS_reselect" }
    }
    parm {
		name	"lutgroup"
		cppname	"LUTGroup"
		label	"LUT Volumes"
		type	string
		default	{ "" }
		parmtag	{ "script_action" "import soputils\nkwargs['geometrytype'] = (hou.geometryType.Primitives,)\nkwargs['inputindex'] = 2\nsoputils.selectGroupParm(kwargs)" }
		parmtag	{ "script_action_help" "Select geometry from an available viewport.\nShift-click to turn on Select Groups." }
		parmtag	{ "script_action_icon" "BUTTONS_reselect" }
    }
    parm {
		name	"activegroup"
		cppname	"ActiveGroup"
		label	"Active Volume"
		type	string
		default	{ "" }
		parmtag	{ "script_action" "import soputils\nkwargs['geometrytype'] = (hou.geometryType.Primitives,)\nkwargs['inputindex'] = 3\nsoputils.selectGroupParm(kwargs)" }
		parmtag	{ "script_action_help" "Select geometry from an available viewport.\nShift-click to turn on Select Groups." }
		parmtag	{ "script_action_icon" "BUTTONS_reselect" }
    }
    parm {
		name	"lutcenter"
		cppname "LUTCenter"
		label	"LUT Center"
		type	integer
		default	{ 10 }
    }
    parm {
		name	"lutrad"
		label	"LUT Rad"
		cppname	LUTRad
		type	integer
		default	{ 1 }
    }
    parm {
		name	"lutmagic"
		label	"LUT Magic"
		cppname	LUTMagic
		type	float
		default	{ 1 }
    }
    parm {
		name	"lutround"
		label	"LUT Round Pattern"
		cppname	LUTRound
		type	toggle
		default	{ "1" }
    }
    parm {
		name	"domip"
		label	"Do MIP MAP"
		cppname	DoMIP
		type 	toggle
		default	{ "1" }
    }
    parm {
		name	"mipby4"
		label	"Mip MAP by 4"
		cppname	MipBy4
		type 	toggle
		default	{ "1" }
    }
    parm {
		name	"mipmagic"
		label	"MIP Magic"
		cppname	MIPMagic
		type	float
		default	{ 1 }
    }

    parm {
		name	"zeroinactive"
		label	"Zero Inactive"
		cppname	ZeroInactive
		type	toggle
		default	{ 0 }
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
        default { 1024 }
        range   { 0! 2048 }
    }
}
)THEDSFILE";

PRM_Template *
SOP_FeE_VolumeProject_1_0::buildTemplates()
{
    static PRM_TemplateBuilder	templ("SOP_FeE_VolumeProject_1_0.C"_sh, theDsFile);
    if (templ.justBuilt())
    {
	templ.setChoiceListPtr("group", &SOP_Node::primGroupMenu);
	templ.setChoiceListPtr("divgroup", &SOP_Node::primGroupMenu);
    }
    return templ.templates();
}


OP_Node *
SOP_FeE_VolumeProject_1_0::myConstructor(OP_Network *dad, const char *name, OP_Operator *op)
{
    return new SOP_FeE_VolumeProject_1_0(dad, name, op);
}

SOP_FeE_VolumeProject_1_0::SOP_FeE_VolumeProject_1_0(OP_Network *dad, const char *name, OP_Operator *op)
	: SOP_Node(dad, name, op)
{
}

SOP_FeE_VolumeProject_1_0::~SOP_FeE_VolumeProject_1_0()
{
}

OP_ERROR
SOP_FeE_VolumeProject_1_0::cookMySop(OP_Context &context)
{
    return cookMyselfAsVerb(context);
}

class SOP_FeE_VolumeProject_1_0Verb : public SOP_NodeVerb
{
public:
    SOP_FeE_VolumeProject_1_0Verb() 
    {
    }
    virtual ~SOP_FeE_VolumeProject_1_0Verb() {}

    virtual SOP_NodeParms	*allocParms() const { return new SOP_FeE_VolumeProject_1_0Parms(); }
    virtual UT_StringHolder	 name() const { return "FeE::volumeProject::1.0"_sh; }

    virtual CookMode		 cookMode(const SOP_NodeParms *parms)  const { return COOK_INPLACE; }

    virtual void	cook(const CookParms &cookparms) const;
};


static SOP_NodeVerb::Register<SOP_FeE_VolumeProject_1_0Verb> theSOPVolumeProjectVerb;

const SOP_NodeVerb *
SOP_FeE_VolumeProject_1_0::cookVerb() const 
{ 
    return theSOPVolumeProjectVerb.get();
}


void
SOP_FeE_VolumeProject_1_0Verb::cook(const SOP_NodeVerb::CookParms &cookparms) const
{
    auto&& sopparms = cookparms.parms<SOP_FeE_VolumeProject_1_0Parms>();
    GA_Detail& geo = *cookparms.gdh().gdpNC();



	
	GFE_VolumeProject volumeProject(geo, cookparms);

	//volumeProject.groupParser.setGroup(groupType, sopparms.getGroup());
	//volumeProject.getOutAttribArray().set(geo0AttribClass, geo0AttribNames);
	volumeProject.setComputeParm(
		sopparms.getSubscribeRatio(), sopparms.getMinGrainSize());

	volumeProject.computeAndBumpDataId();

}
