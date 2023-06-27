
//#define UT_ASSERT_LEVEL 3
#include "SOP_FeE_Fuse_1_0.h"


#include "SOP_FeE_Fuse_1_0.proto.h"

#include "GA/GA_Detail.h"
#include "PRM/PRM_TemplateBuilder.h"
#include "UT/UT_Interrupt.h"
#include "UT/UT_DSOVersion.h"




#include "GFE/GFE_Fuse.h"




using namespace SOP_FeE_Fuse_1_0_Namespace;


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
        name    "keepEdgeGroup"
        cppname "KeepEdgeGroup"
        label   "Keep Edge Group"
        type    string
        default { "*" }
    }
    parm {
        name    "queryGroup"
        cppname "QueryGroup"
        label   "Group"
        type    string
        default { "" }
    }
    parm {
        name    "useTargetGroup"
        cppname "UseTargetGroup"
        label   "Target Group"
        type    toggle
        nolabel
        joinnext
        default { "off" }
        disablewhen "{ snapType != distance } { hasinput(1) == 1 }"
    }
    parm {
        name    "targetGroup"
        cppname "TargetGroup"
        label   "Target Group"
        type    string
        default { "" }
        disablewhen "{ snapType != distance } { hasinput(1) == 0 usetargetgroup == 0 } { hasinput(1) == 0 }"
    }
    parm {
        name    "modifyTarget"
        cppname "ModifyTarget"
        label   "Modify Target"
        type    toggle
        default { "off" }
        disablewhen "{ snapType != distance } { usetargetgroup == 0 }"
    }
    parm {
        name    "posAttrib"
        cppname "PosAttrib"
        label   "Position Attribute"
        type    string
        default { "P" }
        parmtag { "sop_input" "0" }
    }
    groupsimple {
        name    "snapGroup"
        label   "Snap"
        grouptag { "group_type" "simple" }

        parm {
            name    "snapType"
            cppname "SnapType"
            label   "Snap Type"
            type    ordinal
            joinnext
            default { "distance" }
            menu {
                "distance"    "Near Points"
                "grid"        "Grid"
                "specified"   "Specified Points"
            }
        }
        parm {
            name    "algorithm"
            cppname "Algorithm"
            label   "Algorithm"
            type    ordinal
            default { "lowest" }
            hidewhen "{ snapType != distance }"
            menu {
                "lowest"    "Least Target Point Number (for cloud reduction)"
                "closest"   "Closest Target Point (for disjoint pieces)"
            }
        }
        parm {
            name    "useSnapDist"
            cppname "UseSnapDist"
            label   "Use Snap Distance"
            type    toggle
            nolabel
            joinnext
            default { "on" }
            hidewhen "{ snapType != distance }"
        }
        parm {
            name    "snapDist"
            cppname "SnapDist"
            label   "Snap Distance"
            type    log
            default { "0" }
            disablewhen "{ snapType != distance } { usetol3d == 0 }"
            hidewhen "{ snapType != distance }"
            range   { 0.001 10 }
        }
        parm {
            name    "usePositionSnapMethod"
            cppname "UsePositionSnapMethod"
            label   "Snap Positions"
            type    toggle
            nolabel
            joinnext
            default { "off" }
            hidewhen "{ snapType != distance }"
        }
        parm {
            name    "positionSnapMethod"
            cppname "PositionSnapMethod"
            label   "Output Positions"
            type    ordinal
            default { "average" }
            disablewhen "{ snapType != distance } { usePositionSnapMethod == 0 }"
            hidewhen "{ snapType != distance }"
            menu {
                "average"   "Average Value"
                "lowest"    "Least Point Number"
                "highest"   "Greatest Point Number"
                "max"       "Maximum Value"
                "min"       "Minimum Value"
                "mode"      "Mode"
                "median"    "Median"
                "sum"       "Sum"
                "sumsquare" "Sum of Squares"
                "rms"       "Root Mean Square"
            }
        }
        parm {
            name    "useRadiusAttrib"
            cppname "UseRadiusAttrib"
            label   "Radius Attribute"
            type    toggle
            nolabel
            joinnext
            default { "off" }
            hidewhen "{ snapType != distance }"
        }
        parm {
            name    "radiusAttrib"
            cppname "RadiusAttrib"
            label   "Radius Attribute"
            type    string
            default { "pscale" }
            disablewhen "{ useradiusattrib == 0 }"
            hidewhen "{ snapType != distance }"
            parmtag { "sop_input" "1" }
        }
        parm {
            name    "useMatchAttrib"
            cppname "UseMatchAttrib"
            label   "Match Attribute"
            type    toggle
            nolabel
            joinnext
            default { "off" }
            hidewhen "{ snapType != distance }"
        }
        parm {
            name    "matchAttrib"
            cppname "MatchAttrib"
            label   "Match Attribute"
            type    string
            default { "name" }
            disablewhen "{ useMatchAttrib == 0 }"
            hidewhen "{ snapType != distance }"
            parmtag { "sop_input" "1" }
        }
        parm {
            name    "matchType"
            cppname "MatchType"
            label   "Match Condition"
            type    ordinal
            default { "match" }
            disablewhen "{ useMatchAttrib == 0 }"
            hidewhen "{ snapType != distance }"
            menu {
                "match"     "Equal Attribute Values"
                "mismatch"  "Unequal Attribute Values"
            }
        }
        parm {
            name    "matchTol"
            cppname "MatchTol"
            label   "Match Tolerance"
            type    float
            default { "0" }
            disablewhen "{ useMatchAttrib == 0 }"
            hidewhen "{ snapType != distance }"
            range   { 0 1 }
        }
        parm {
            name    "gridType"
            cppname "Gridtype"
            label   "Grid Type"
            type    ordinal
            default { "spacing" }
            hidewhen "{ snapType != grid }"
            menu {
                "spacing"   "Grid Spacing"
                "lines"     "Grid Lines"
                "pow2"      "Power of 2 Grid Lines"
            }
        }
        parm {
            name    "gridSpacing"
            cppname "GridSpacing"
            label   "Grid Spacing"
            type    vector
            size    3
            default { "0.1" "0.1" "0.1" }
            disablewhen "{ gridType != spacing }"
            hidewhen "{ snapType != grid }"
            range   { -1 1 }
        }
        parm {
            name    "gridLines"
            cppname "GridLines"
            label   "Grid Lines"
            type    vector
            size    3
            default { "10" "10" "10" }
            disablewhen "{ gridType != lines }"
            hidewhen "{ snapType != grid }"
            range   { -1 1 }
        }
        parm {
            name    "gridPow"
            cppname "GridPow"
            label   "Grid Power 2"
            type    integer
            size    3
            default { "3" "3" "3" }
            disablewhen "{ gridType != pow2 }"
            hidewhen "{ snapType != grid }"
            range   { 0 10 }
        }
        parm {
            name    "gridOffset"
            cppname "GridOffset"
            label   "Grid Offset"
            type    vector
            size    3
            default { "0" "0" "0" }
            hidewhen "{ snapType != grid }"
            range   { -1 1 }
        }
        parm {
            name    "gridRounding"
            cppname "GridRounding"
            label   "Grid Rounding"
            type    ordinal
            default { "nearest" }
            hidewhen "{ snapType != grid }"
            menu {
                "nearest"   "Nearest"
                "down"      "Down"
                "up"        "Up"
            }
        }
        parm {
            name    "useGridTol"
            cppname "UseGridTol"
            label   "Use Grid Tolerance"
            type    toggle
            nolabel
            joinnext
            default { "on" }
            hidewhen "{ snapType != grid }"
        }
        parm {
            name    "gridTol"
            cppname "GridTol"
            label   "Grid Tolerance"
            type    float
            default { "10" }
            disablewhen "{ snapType != grid } { useGridTol == 0 }"
            hidewhen "{ snapType != grid }"
            range   { 0.001 10 }
        }
    }

    groupsimple {
        name    "fuse_folder"
        label   "Fuse"
        grouptag { "group_type" "simple" }

        parm {
            name    "fuseSnappedPoint"
            cppname "FuseSnappedPoint"
            label   "Fuse Snapped Points"
            type    toggle
            default { "on" }
        }
        parm {
            name    "keepFusedPoint"
            cppname "KeepFusedPoint"
            label   "Keep Fused Points"
            type    toggle
            default { "off" }
            disablewhen "{ fuseSnappedPoint == 0 }"
        }
        parm {
            name    "delDegenerate"
            cppname "DelDegenerate"
            label   "Remove Repeated Vertices and Degenerate Primitives"
            type    toggle
            default { "on" }
            disablewhen "{ fuseSnappedPoint == 0 }"
        }
        parm {
            name    "delDegenPoint"
            cppname "DelDegenPoint"
            label   "Remove Unused Points from Degenerate Primitives"
            type    toggle
            default { "on" }
            disablewhen "{ fuseSnappedPoint == 0 } { delDegenerate == 0 }"
        }
        parm {
            name    "delUnusedPoint"
            cppname "DelUnusedPoint"
            label   "Remove All Unused Point"
            type    toggle
            default { "off" }
            disablewhen "{ fuseSnappedPoint == 0 }"
        }
    }

    groupsimple {
        name    "outputGroup"
        label   "Output Attributes and Groups"
        grouptag { "group_type" "simple" }

        parm {
            name    "recomputeNormal"
            cppname "RecomputeNormal"
            label   "Recompute Affected Normals"
            type    toggle
            default { "off" }
        }
        parm {
            name    "createSnappedGroup"
            cppname "CreateSnappedGroup"
            label   "Create Snapped Points Group"
            type    toggle
            nolabel
            joinnext
            default { "off" }
        }
        parm {
            name    "snappedGroupName"
            cppname "SnappedGroupName"
            label   "Snapped Point Group"
            type    string
            default { "snapped" }
            disablewhen "{ createSnappedGroup == 0 }"
        }
        parm {
            name    "createSnappedAttrib"
            cppname "CreateSnappedAttrib"
            label   "Create Snapped Destination Attribute"
            type    toggle
            nolabel
            joinnext
            default { "off" }
            disablewhen "{ snapType != distance }"
        }
        parm {
            name    "snappedAttribName"
            cppname "SnappedAttribName"
            label   "Snapped Destination Attribute"
            type    string
            default { "snapTo" }
            disablewhen "{ createSnappedAttrib == 0 } { snapType != distance }"
        }
        multiparm {
            name    "numPointAttrib"
            cppname "NumPointAttrib"
            label   "Attributes to Snap"
            default 0

            parm {
                name    "attribSnapMethod#"
                cppname "AttribSnapMethod#"
                label   "Output Values"
                type    ordinal
                default { "8" }
                menu {
                    "max"       "Maximum"
                    "min"       "Minimum"
                    "mean"      "Average"
                    "mode"      "Mode"
                    "median"    "Median"
                    "sum"       "Sum"
                    "sumsquare" "Sum of Squares"
                    "rms"       "Root Mean Square"
                    "first"     "First Match"
                    "last"      "Last Match"
                }
            }
            parm {
                name    "pointAttribs#"
                cppname "PointAttribs#"
                label   "Point Attribute"
                type    string
                default { "" }
                parmtag { "sop_input" "1" }
            }
        }

        multiparm {
            name    "numGroups"
            cppname "NumGroups"
            label   "Groups to Snap"
            default 0

            parm {
                name    "groupPropagation#"
                cppname "GroupPropagation#"
                label   "Group Propagation"
                type    ordinal
                default { "0" }
                menu {
                    "leastPointNumber"      "Least Point Number"
                    "greatestPointNumber"   "Greatest Point Number"
                    "union"                 "Union"
                    "intersect"             "Intersect"
                    "mode"                  "Most Common"
                }
            }
            parm {
                name    "pointGroups#"
                cppname "PointGroups#"
                label   "Point Group"
                type    string
                default { "" }
                parmtag { "sop_input" "1" }
            }
        }
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
SOP_FeE_Fuse_1_0::buildTemplates()
{
    static PRM_TemplateBuilder templ("SOP_FeE_Fuse_1_0.C"_sh, theDsFile);
    if (templ.justBuilt())
    {
        templ.setChoiceListPtr("group"_sh, &SOP_Node::groupMenu);
    }
    return templ.templates();
}

const UT_StringHolder SOP_FeE_Fuse_1_0::theSOPTypeName("FeE::fuse::1.0"_sh);

void
newSopOperator(OP_OperatorTable* table)
{
    OP_Operator* newOp = new OP_Operator(
        SOP_FeE_Fuse_1_0::theSOPTypeName,
        "FeE Fuse",
        SOP_FeE_Fuse_1_0::myConstructor,
        SOP_FeE_Fuse_1_0::buildTemplates(),
        1,
        2,
        nullptr,
        OP_FLAG_GENERATOR,
        nullptr,
        1,
        "Five elements Elf/Topo/Optimize");

    newOp->setIconName("SOP_fuse-2.0");
    table->addOperator(newOp);

}





class SOP_FeE_Fuse_1_0Verb : public SOP_NodeVerb
{
public:
    SOP_FeE_Fuse_1_0Verb() {}
    virtual ~SOP_FeE_Fuse_1_0Verb() {}

    virtual SOP_NodeParms *allocParms() const { return new SOP_FeE_Fuse_1_0Parms(); }
    virtual UT_StringHolder name() const { return SOP_FeE_Fuse_1_0::theSOPTypeName; }

    virtual CookMode cookMode(const SOP_NodeParms *parms) const { return COOK_GENERIC; }

    virtual void cook(const CookParms &cookparms) const;
    
    /// This static data member automatically registers
    /// this verb class at library load time.
    static const SOP_NodeVerb::Register<SOP_FeE_Fuse_1_0Verb> theVerb;
};

// The static member variable definition has to be outside the class definition.
// The declaration is inside the class.
const SOP_NodeVerb::Register<SOP_FeE_Fuse_1_0Verb> SOP_FeE_Fuse_1_0Verb::theVerb;

const SOP_NodeVerb *
SOP_FeE_Fuse_1_0::cookVerb() const 
{ 
    return SOP_FeE_Fuse_1_0Verb::theVerb.get();
}





static GA_GroupType
sopPositionSnapMethod(SOP_FeE_Fuse_1_0Parms::PositionSnapMethod parmPositionSnapMethod)
{
    using namespace SOP_FeE_Fuse_1_0Enums;
    switch (parmPositionSnapMethod)
    {
    case PositionSnapMethod::GUESS:     return GA_GROUP_INVALID;    break;
    case PositionSnapMethod::PRIM:      return GA_GROUP_PRIMITIVE;  break;
    case PositionSnapMethod::POINT:     return GA_GROUP_POINT;      break;
    case PositionSnapMethod::VERTEX:    return GA_GROUP_VERTEX;     break;
    case PositionSnapMethod::EDGE:      return GA_GROUP_EDGE;       break;
    }
    UT_ASSERT_MSG(0, "Unhandled geo0Group type!");
    return GA_GROUP_INVALID;
}


void
SOP_FeE_Fuse_1_0Verb::cook(const SOP_NodeVerb::CookParms &cookparms) const
{
    auto&& sopparms = cookparms.parms<SOP_FeE_Fuse_1_0Parms>();
    GA_Detail& outGeo0 = *cookparms.gdh().gdpNC();
    //auto sopcache = (SOP_FeE_Fuse_1_0Cache*)cookparms.cache();

    const GA_Detail& inGeo0 = *cookparms.inputGeo(0);

    outGeo0.replaceWith(inGeo0);



    const GA_AttributeOwner attribClass = sopAttribOwner(sopparms.getClass());
    const GA_GroupType parmPositionSnapMethod = sopPositionSnapMethod(sopparms.getPositionSnapMethod());


    UT_AutoInterrupt boss("Processing");
    if (boss.wasInterrupted())
        return;
    
/*
    GFE_Fuse fuse(geo, cookparms);
    fuse.findOrCreateTuple(true, GA_ATTRIB_POINT);
    fuse.compute();
*/
    
    GFE_Fuse fuse(outGeo0, cookparms);
    fuse.setComputeParm(sopparms.getFirstIndex(), sopparms.getNegativeIndex(), sopparms.getOutAsOffset(),
        sopparms.getSubscribeRatio(), sopparms.getMinGrainSize());

    fuse.groupParser.setGroup(groupType, sopparms.getGroup());

    
    fuse.findOrCreateTuple(false, attribClass, storageClass, GA_STORE_INVALID, sopparms.getAttribName());

    fuse.computeAndBumpDataId();
    

}


