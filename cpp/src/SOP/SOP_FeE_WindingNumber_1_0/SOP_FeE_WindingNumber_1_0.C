/*
 * Copyright (c) 2022
 *      Side Effects Software Inc.  All rights reserved.
 *
 * Redistribution and use of Houdini Development Kit samples in source and
 * binary forms, with or without modification, are permitted provided that the
 * following conditions are met:
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. The name of Side Effects Software may not be used to endorse or
 *    promote products derived from this software without specific prior
 *    written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY SIDE EFFECTS SOFTWARE `AS IS' AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN
 * NO EVENT SHALL SIDE EFFECTS SOFTWARE BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *----------------------------------------------------------------------------
 * This SOP computes the winding number induced by a mesh at the specified points.
 */

#include "SOP_FeE_WindingNumber_1_0.h"

// This is an automatically generated header file based on theDsFile, below,
// to provide SOP_FeE_WindingNumber_1_0Parms, an easy way to access parameter values from
// SOP_FeE_WindingNumber_1_0Verb::cook with the correct type.
#include "SOP_FeE_WindingNumber_1_0.proto.h"

#include "GA/GA_Detail.h"
#include "PRM/PRM_TemplateBuilder.h"
#include "UT/UT_Interrupt.h"
#include "UT/UT_DSOVersion.h"



//#include "GA_FeE/GA_FeE_Type.h"
#include "GA_FeE/GA_FeE_WindingNumber.h"

using namespace SOP_FeE_WindingNumber_1_0_Namespace;


class SOP_FeE_WindingNumber_1_0Verb : public SOP_NodeVerb
{
public:
    virtual SOP_NodeParms *allocParms() const { return new SOP_FeE_WindingNumber_1_0Parms(); }
    virtual SOP_NodeCache *allocCache() const { return new GA_FeE_WindingNumber::GA_WindingNumber_Cache(); }
    virtual UT_StringHolder name() const { return theSOPTypeName; }

    /// This SOP wouldn't get any benefit from the results of the previous cook,
    /// (except for what's stored in SOP_FeE_WindingNumber_1_0Cache), and it would
    /// always duplicate its input at the start of the cook anyway, so it might
    /// as well use COOK_INPLACE, (which always either steals the first input's
    /// detail for the output detail or duplicates it into the output detail),
    /// instead of COOK_GENERIC, (which would let us have an output detail
    /// that's separate from the input detail and might be the same output
    /// detail as on a previous cook).
    virtual CookMode cookMode(const SOP_NodeParms *parms) const { return COOK_INPLACE; }

    virtual void cook(const CookParms &cookparms) const;

    /// This is the internal name of the SOP type.
    /// It isn't allowed to be the same as any other SOP's type name.
    static const UT_StringHolder theSOPTypeName;

    /// This static data member automatically registers
    /// this verb class at library load time.
    static const SOP_NodeVerb::Register<SOP_FeE_WindingNumber_1_0Verb> theVerb;

    /// This is the parameter interface string, below.
    static const char *const theDsFile;
};

// The static member variable definitions have to be outside the class definition.
// The declarations are inside the class.
const UT_StringHolder SOP_FeE_WindingNumber_1_0Verb::theSOPTypeName("FeE::windingNumber::1.0"_sh);
const SOP_NodeVerb::Register<SOP_FeE_WindingNumber_1_0Verb> SOP_FeE_WindingNumber_1_0Verb::theVerb;

//******************************************************************************
//*                 Parameters                                                 *
//******************************************************************************

/// This is a multi-line raw string specifying the parameter interface for this SOP.

static const char* theDsFile = R"THEDSFILE(
{
    name        parameters
    parm {
        name    "queryPoints"
        cppname "QueryPoints"
        label   "Query Points"
        type    string
        default { "" }
        parmtag { "script_action" "import soputils\nkwargs['geometrytype'] = (hou.geometryType.Points,)\nkwargs['inputindex'] = 0\nsoputils.selectGroupParm(kwargs)" }
        parmtag { "script_action_help" "Select geometry from an available viewport.\nShift-click to turn on Select Groups." }
        parmtag { "script_action_icon" "BUTTONS_reselect" }
    }
    parm {
        name    "meshPrims"
        cppname "MeshPrims"
        label   "Mesh Primitives"
        type    string
        default { "" }
        parmtag { "script_action" "import soputils\nkwargs['geometrytype'] = (hou.geometryType.Primitives,)\nkwargs['inputindex'] = 1\nsoputils.selectGroupParm(kwargs)" }
        parmtag { "script_action_help" "Select geometry from an available viewport.\nShift-click to turn on Select Groups." }
        parmtag { "script_action_icon" "BUTTONS_reselect" }
        parmtag { "sop_input" "1" }
    }
    parm {
        name    "wnType"
        cppname "WNType"
        label   "Winding Number Type"
        type    ordinal
        default { "0" }
        menu {
            "xyz"   "3D"
            "xy"    "2D in XY Plane"
            "yz"    "2D in YZ Plane"
            "zx"    "2D in ZX Plane"
        }
    }
    parm {
        name    "wnAttribName"
        cppname "WNAttribName"
        label   "Attribute Name"
        type    string
        default { "windingNumber" }
    }
    parm {
        name    "asSolidAngle"
        cppname "AsSolidAngle"
        label   "Scale to Solid Angle"
        type    toggle
        default { "0" }
        joinnext
    }
    parm {
        name    "negate"
        cppname "Negate"
        label   "Negate Value (Reverse)"
        type    toggle
        default { "0" }
    }
    parm {
        name    "fullAccuracy"
        cppname "FullAccuracy"
        label   "Full Accuracy (Slow)"
        type    toggle
        default { "0" }
    }
    parm {
        name    "accuracyScale"
        cppname "AccuracyScale"
        label   "Accuracy Scale"
        type    float
        default { "2" }
        range   { 1! 20 }
        disablewhen "{ fullAccuracy == 1 }"
    }
}
)THEDSFILE";

PRM_Template*
SOP_FeE_WindingNumber_1_0::buildTemplates()
{
    static PRM_TemplateBuilder templ("SOP_FeE_WindingNumber_1_0.C"_sh, theDsFile);
    if (templ.justBuilt())
    {
        templ.setChoiceListPtr("queryPoints"_sh, &SOP_Node::pointGroupMenu);
        templ.setChoiceListPtr("meshPrims"_sh, &SOP_Node::primGroupMenu);
    }
    return templ.templates();
}

const SOP_NodeVerb *SOP_FeE_WindingNumber_1_0::cookVerb() const
{
    return SOP_FeE_WindingNumber_1_0Verb::theVerb.get();
}

/// newSopOperator is the hook that Houdini grabs from this dll
/// and invokes to register the SOP.  In this case, we add ourselves
/// to the specified operator table.
void
newSopOperator(OP_OperatorTable *table)
{
    OP_Operator* newOp = new OP_Operator(
        SOP_FeE_WindingNumber_1_0Verb::theSOPTypeName,
        "FeE Winding Number",
        SOP_FeE_WindingNumber_1_0::myConstructor,
        SOP_FeE_WindingNumber_1_0::buildTemplates(),
        2,
        2,
        nullptr,
        0,  // No flags: not a generator, no merge input, not an output
        nullptr,
        1,
        "Five elements Elf/Data");

    newOp->setIconName("SOP_windingnumber");
    table->addOperator(newOp);
}



static GA_WindingNumberType
sopWNType(SOP_FeE_WindingNumber_1_0Parms::WNType wnType)
{
    using namespace SOP_FeE_WindingNumber_1_0Enums;
    switch (wnType)
    {
    case WNType::XYZ:      return GA_WNType_XYZ;   break;
    case WNType::XY:       return GA_WNType_XY;    break;
    case WNType::YZ:       return GA_WNType_YZ;    break;
    case WNType::ZX:       return GA_WNType_ZX;    break;
    }
    UT_ASSERT_MSG(0, "Unhandled WNType!");
    return GA_WNType_XYZ;
}



/// This is the function that does the actual work.
void SOP_FeE_WindingNumber_1_0Verb::cook(const SOP_NodeVerb::CookParms& cookparms) const
{
    auto &&sopparms = cookparms.parms<SOP_FeE_WindingNumber_1_0Parms>();

    GA_Detail* const geoPoint = cookparms.gdh().gdpNC();
    const GA_Detail* const geoRefMesh = cookparms.inputGeo(1);
    

    //const GA_Storage inStorageF = GA_FeE_Type::getPreferredStorageF(geoPoint);
    
#if 0
    GA_FeE_WindingNumber::GA_WindingNumber_Cache* sopcache = (GA_FeE_WindingNumber::GA_WindingNumber_Cache*)cookparms.cache();

    GA_Attribute* wnAttribPtr = GA_FeE_WindingNumber::addAttribWindingNumber(
        cookparms, geoPoint, geoRefMesh,
        sopparms.getQueryPoints(), sopparms.getMeshPrims(), sopcache,
        GA_STORE_INVALID, sopparms.getWNAttribName(),
        sopWNType(sopparms.getWNType()), sopparms.getFullAccuracy(), sopparms.getAccuracyScale(), sopparms.getAsSolidAngle(), sopparms.getNegate()
    );
#else
    GA_Attribute* wnAttribPtr = GA_FeE_WindingNumber::addAttribWindingNumber(
        cookparms, geoPoint, geoRefMesh,
        sopparms.getQueryPoints(), sopparms.getMeshPrims(), nullptr,
        GA_STORE_INVALID, sopparms.getWNAttribName(),
        sopWNType(sopparms.getWNType()), sopparms.getFullAccuracy(), sopparms.getAccuracyScale(), sopparms.getAsSolidAngle(), sopparms.getNegate()
    );
#endif

    wnAttribPtr->bumpDataId();
}
