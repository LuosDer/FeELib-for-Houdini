
//#define UT_ASSERT_LEVEL 3
#include "SOP_FeE_Normal_2D_1_0.h"


// This is an automatically generated header file based on theDsFile, below,
// to provide SOP_FeE_Normal_2D_1_0Parms, an easy way to access parameter values from
// SOP_FeE_Normal_2D_1_0Verb::cook with the correct type.
#include "SOP_FeE_Normal_2D_1_0.proto.h"


#include <GA/GA_Detail.h>
#include <PRM/PRM_TemplateBuilder.h>
#include <UT/UT_Interrupt.h>
#include <UT/UT_DSOVersion.h>


#include <GA_FeE/GA_FeE_Measure.h>
#include <GA_FeE/GA_FeE_Connectivity.h>
#include <GA_FeE/GA_FeE_TopologyReference.h>
#include <GEO_FeE/GEO_FeE_Group.h>
#include <GEO_FeE/GEO_FeE_GroupExpand.h>


using namespace SOP_FeE_Normal_2D_1_0_Namespace;


//
// Help is stored in a "wiki" style text file.  This text file should be copied
// to $HOUDINI_PATH/help/nodes/sop/FeE.txt
//
// See the sample_install.sh file for an example.
//

/// This is the internal name of the SOP type.
/// It isn't allowed to be the same as any other SOP's type name.
const UT_StringHolder SOP_FeE_Normal_2D_1_0::theSOPTypeName("FeE::normal2D::1.0"_sh);

/// newSopOperator is the hook that Houdini grabs from this dll
/// and invokes to register the SOP.  In this case, we add ourselves
/// to the specified operator table.
void
newSopOperator(OP_OperatorTable *table)
{
    table->addOperator(new OP_Operator(
        SOP_FeE_Normal_2D_1_0::theSOPTypeName,   // Internal name
        "FeE Normal 2D",                     // UI name
        SOP_FeE_Normal_2D_1_0::myConstructor,    // How to build the SOP
        SOP_FeE_Normal_2D_1_0::buildTemplates(), // My parameters
        1,                         // Min # of sources
        1,                         // Max # of sources
        nullptr,                   // Custom local variables (none)
        OP_FLAG_GENERATOR));       // Flag it as generator
}

/// This is a multi-line raw string specifying the parameter interface
/// for this SOP.
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
        parmtag { "script_action" "import soputils\nkwargs['geometrytype'] = kwargs['node'].parmTuple('grouptype')\nkwargs['inputindex'] = 0\nsoputils.selectGroupParm(kwargs)" }
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
        menureplace {
            [ "" ]
            [ "import fee_buildMenu" ]
            [ "" ]
            [ "node = kwargs['node']" ]
            [ "return fee_buildMenu.buildAttribsMenu(node, 0, 'point', 'all')" ]
            language python
        }
    }
    parm {
        name    "input3DNormal"
        cppname "Input3DNormal"
        label   "Input 3D Normal"
        type    toggle
        default { "0" }
    }
    parm {
        name    "inputConstant3DNormal"
        cppname "InputConstant3DNormal"
        label   "Input Constant 3D Normal"
        type    toggle
        default { "0" }
    }
    parm {
        name    "normal3DAttribName"
        cppname "Normal3DAttribName"
        label   "Normal3D Attrib Name"
        type    string
        default { "N" }
        disablewhen "{ input3DNormal == 0 }"
        hidewhen "{ inputConstant3DNormal == 1 }"
        menureplace {
            [ "" ]
            [ "import fee_buildMenu" ]
            [ "" ]
            [ "node = kwargs['node']" ]
            [ "classType = node.parm('class').evalAsString()" ]
            [ "return fee_buildMenu.buildAttribsMenu(node, 0, classType, 'all')" ]
            language python
        }
    }
    parm {
        name    "constant3DNormal"
        cppname "Constant3DNormal"
        label   "Constant 3D Normal"
        type    vector
        size    3
        default { "0" "1" "0" }
        hidewhen "{ inputConstant3DNormal == 0 }"
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
        name    "normal2DAttribClass"
        cppname "Normal2DAttribClass"
        label   "Normal 2D Attrib Class"
        type    ordinal
        default { "point" }
        menu {
            "point"     "Point"
            "vertex"    "Vertex"
        }
    }
    parm {
        name    "extrapolateEnds"
        cppname "ExtrapolateEnds"
        label   "Extrapolate Ends"
        type    toggle
        default { "on" }
    }
    parm {
        name    "halfScale"
        cppname "HalfScale"
        label   "Half Scale"
        type    toggle
        default { "0" }
    }
    parm {
        name    "scaleByTurns"
        cppname "ScaleByTurns"
        label   "Scale by Turns"
        type    toggle
        default { "0" }
    }
    parm {
        name    "normalize"
        cppname "Normalize"
        label   "Normalize"
        type    toggle
        default { "1" }
        disablewhen "{ extrapolateEnds == 1 } { halfScale == 1 } { scaleByTurns == 1 }"
    }
    parm {
        name    "uniScale"
        cppname "UniScale"
        label   "Uniform Scale"
        type    float
        default { "1" }
        range   { 0 4 }
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
SOP_FeE_Normal_2D_1_0::buildTemplates()
{
    static PRM_TemplateBuilder templ("SOP_FeE_Normal_2D_1_0.C"_sh, theDsFile);
    return templ.templates();
}


//class SOP_FeE_Normal_2D_1_0Cache : public SOP_NodeCache
//{
//public:
//    SOP_FeE_Normal_2D_1_0Cache() : SOP_NodeCache(),
//        myPrevOutputDetailID(-1)
//    {}
//    ~SOP_FeE_Normal_2D_1_0Cache() override {}
//
//    int64 myPrevOutputDetailID;
//};


class SOP_FeE_Normal_2D_1_0Verb : public SOP_NodeVerb
{
public:
    SOP_FeE_Normal_2D_1_0Verb() {}
    virtual ~SOP_FeE_Normal_2D_1_0Verb() {}

    virtual SOP_NodeParms *allocParms() const { return new SOP_FeE_Normal_2D_1_0Parms(); }
    //virtual SOP_NodeCache* allocCache() const { return new SOP_FeE_Normal_2D_1_0Cache(); }
    virtual UT_StringHolder name() const { return SOP_FeE_Normal_2D_1_0::theSOPTypeName; }

    virtual CookMode cookMode(const SOP_NodeParms *parms) const { return COOK_GENERIC; }

    virtual void cook(const CookParms &cookparms) const;

    /// This static data member automatically registers
    /// this verb class at library ldir0d time.
    static const SOP_NodeVerb::Register<SOP_FeE_Normal_2D_1_0Verb> theVerb;
};

// The static member variable definition has to be outside the class definition.
// The declaration is inside the class.
const SOP_NodeVerb::Register<SOP_FeE_Normal_2D_1_0Verb> SOP_FeE_Normal_2D_1_0Verb::theVerb;

const SOP_NodeVerb *
SOP_FeE_Normal_2D_1_0::cookVerb() const 
{ 
    return SOP_FeE_Normal_2D_1_0Verb::theVerb.get();
}







static GA_AttributeOwner
sopAttribOwner(SOP_FeE_Normal_2D_1_0Parms::Normal2DAttribClass attribClass)
{
    using namespace SOP_FeE_Normal_2D_1_0Enums;
    switch (attribClass)
    {
    case Normal2DAttribClass::POINT:     return GA_ATTRIB_POINT;      break;
    case Normal2DAttribClass::VERTEX:    return GA_ATTRIB_VERTEX;     break;
    }
    UT_ASSERT_MSG(0, "Unhandled Geo0 Class type!");
    return GA_ATTRIB_INVALID;
}


static GA_GroupType
sopGroupType(SOP_FeE_Normal_2D_1_0Parms::GroupType parmgrouptype)
{
    using namespace SOP_FeE_Normal_2D_1_0Enums;
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
SOP_FeE_Normal_2D_1_0Verb::cook(const SOP_NodeVerb::CookParms &cookparms) const
{
    auto &&sopparms = cookparms.parms<SOP_FeE_Normal_2D_1_0Parms>();
    GU_Detail* outGeo0 = cookparms.gdh().gdpNC();
    //auto sopcache = (SOP_FeE_Normal_2D_1_0Cache*)cookparms.cache();

    const GEO_Detail* const inGeo0 = cookparms.inputGeo(0);

    outGeo0->replaceWith(*inGeo0);
    // outGeo0->clearAndDestroy();

    //outGeo0 = sopNodeProcess(*inGeo0);




    const UT_StringHolder& geo0AttribNames = sopparms.getNormal2DAttribName();
    if (!geo0AttribNames.isstring())
        return;
    const GA_AttributeOwner geo03dNAttribClass = sopAttribOwner(sopparms.getNormal2DAttribClass());





    const bool scaleByTurns = sopparms.getScaleByTurns();
    const bool normalize = sopparms.getNormalize();
    const bool extrapolateEnds = sopparms.getExtrapolateEnds();
    const bool input3DNormal = sopparms.getInput3DNormal();
    const bool inputConstant3DNormal = sopparms.getInputConstant3DNormal();

    UT_Vector3T<fpreal64> constant3DNormal = sopparms.getConstant3DNormal();
    const fpreal64 uniScale = sopparms.getUniScale();
    const fpreal64 blend = sopparms.getBlend();


    const exint subscribeRatio = sopparms.getSubscribeRatio();
    const exint minGrainSize = sopparms.getMinGrainSize();




    const GA_Precision PreferredPrecision = outGeo0->getPreferredPrecision();
    const GA_Storage inStorageI = GA_FeE_Type::getPreferredStorageI(PreferredPrecision);
    const GA_Storage inStorageF = GA_FeE_Type::getPreferredStorageF(PreferredPrecision);

    UT_AutoInterrupt boss("Processing");
    if (boss.wasInterrupted())
        return;


    const GA_Attribute* posHAttrib = outGeo0->findPointAttribute(sopparms.getPosAttribName());
    if (!posHAttrib)
        return;
    const GA_ROHandleT<UT_Vector3T<fpreal64>> posH(posHAttrib);



    GOP_Manager gop;
    const UT_StringHolder& groupName0 = sopparms.getGroup();
    const GA_GroupType groupType = sopGroupType(sopparms.getGroupType());
    const GA_Group* geo0Group = GA_FeE_Group::findOrParseGroupDetached(cookparms, outGeo0, groupType, groupName0, gop);
    const bool hasGroup = !!geo0Group;

    if (hasGroup && geo0Group->isEmpty())
        return;

    GA_PointGroup* geo0PointGroup = const_cast<GA_PointGroup*>(GEO_FeE_Group::groupPromotePointDetached(outGeo0, geo0Group));
    UT_UniquePtr<GA_PointGroup> geo0PointGroupUPtr(geo0PointGroup);

    GA_VertexGroup* geo0VtxGroup = const_cast<GA_VertexGroup*>(GEO_FeE_Group::groupPromoteVertexDetached(outGeo0, geo0Group));

    //GA_PointGroup* expandGroup = GA_FeE_Group::newDetachedGroup(outGeo0, geo0Group);
    GA_PointGroup* expandGroup = outGeo0->newDetachedPointGroup();
    UT_UniquePtr<GA_Group> expandGroupUPtr(expandGroup);

    GEO_FeE_GroupExpand::groupExpand(outGeo0, expandGroup, geo0PointGroup, GA_GROUP_EDGE);
    //notifyGroupParmListeners(cookparms.getNode(), 0, 1, outGeo0, geo0Group);

    const GA_GroupType geo0finalGroupType = geo0Group->classType();






    GA_VertexGroup* unsharedGroup = GA_FeE_VertexNextEquiv::addGroupVertexNextEquiv(outGeo0, geo0VtxGroup);
    //GA_PointGroup* unsharedPointGroup = const_cast<GA_PointGroup*>(GEO_FeE_Group::groupPromote(outGeo0, unsharedGroup, GA_GROUP_POINT, geo0AttribNames, true));

    GA_PointGroup* unsharedPointGroup = const_cast<GA_PointGroup*>(GEO_FeE_Group::groupPromotePoint(outGeo0, unsharedGroup, geo0AttribNames, true));


    const GA_Attribute* dstptAttrib = GA_FeE_TopologyReference::addAttribVertexPointDst(outGeo0, geo0VtxGroup);
    const GA_ROHandleT<GA_Offset> dstptAttribH(dstptAttrib);

    GA_Attribute* normal3DAttrib = nullptr;
    if (input3DNormal)
    {
        const UT_StringHolder& normal3DAttribName = sopparms.getNormal3DAttribName();
        switch (geo03dNAttribClass)
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
    else
    {
        //if(geo03dNAttribClass != )
        //normal3DAttrib = outGeo0->addFloatTuple(geo03dNAttribClass, "__N_SOP_FeE_Normal2D_1_0", 3, GA_Defaults(0.0), nullptr, nullptr, inStorageF);
        //normal3DAttrib = addAttribNormal3D(outGeo0, geo03dNAttribClass, inStorageF, "__N_SOP_FeE_Normal2D_1_0", GA_Defaults(0.0));
    }
    const GA_AttributeOwner n3dFinalAttribClass = normal3DAttrib->getOwner();
    const GA_ROHandleT<UT_Vector3T<fpreal64>> n3dH(normal3DAttrib);

    if (!inputConstant3DNormal && n3dFinalAttribClass == GA_ATTRIB_DETAIL)
    {
        constant3DNormal = n3dH.get(0);
    }
    //outGeo0->setPrimitiveClosedFlag(elemoff);


    //GA_Attribute* n2dAttrib = outGeo0->addFloatTuple(geo0AttribClass, geo0AttribNames, 3, GA_Defaults(0.0), nullptr, nullptr, inStorageF);
    GA_Attribute* n2dAttrib = outGeo0->addFloatTuple(GA_ATTRIB_POINT, geo0AttribNames, 3, GA_Defaults(0.0), nullptr, nullptr, inStorageF);
    GA_RWHandleT<UT_Vector3T<fpreal64>> n2dAttribH(n2dAttrib);

    GA_Topology& topo = outGeo0->getTopology();
    const GA_ATITopology* vtxPointRef = topo.getPointRef();
    const GA_ATITopology* pointVtxRef = topo.getVertexRef();
    const GA_ATITopology* vtxPrimRef = topo.getPrimitiveRef();

    if (extrapolateEnds)
    {
        ;
    }
    const GA_SplittableRange geoVertexSplittableRange(outGeo0->getVertexRange(geo0VtxGroup));
    UTparallelFor(geoVertexSplittableRange, [&outGeo0, &posH, &dstptAttribH, &n2dAttribH, &n3dH,
        &vtxPointRef, &vtxPrimRef, 
        &inputConstant3DNormal, &constant3DNormal, &n3dFinalAttribClass](const GA_SplittableRange& r)
    {
        UT_Vector3T<fpreal64> dir;
        GA_Offset start, end;
        for (GA_Iterator it(r); it.blockAdvance(start, end); )
        {
            for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
            {
                GA_Offset ptoff = vtxPointRef->getLink(elemoff);
                GA_Offset dstpt = dstptAttribH.get(elemoff);
                
                dir = posH.get(dstpt) - posH.get(ptoff);
                if (!inputConstant3DNormal && n3dFinalAttribClass != GA_ATTRIB_DETAIL)
                {
                    switch (n3dFinalAttribClass)
                    {
                    case GA_ATTRIB_PRIMITIVE:
                        constant3DNormal = n3dH.get(vtxPrimRef->getLink(elemoff));
                        break;
                    case GA_ATTRIB_POINT:
                        constant3DNormal = n3dH.get(ptoff);
                        break;
                    case GA_ATTRIB_VERTEX:
                        constant3DNormal = n3dH.get(elemoff);
                        break;
                    }
                }
                dir.cross(constant3DNormal);
                dir.normalize();

                n2dAttribH.add(ptoff, dir);
                n2dAttribH.add(dstpt, dir);
            }
        }
    }, subscribeRatio, minGrainSize);


    //const GA_SplittableRange geoPointSplittableRange(outGeo0->getPointRange(geo0PointGroup));
    //UTparallelFor(geoPointSplittableRange, [&outGeo0, &posH, &uniScale, &dstptAttribH, &vtxPointRef, &n2dAttribH, &inputConstant3DNormal, &constant3DNormal, &scaleByTurns, &normalize](const GA_SplittableRange& r)
    //    {
    //        UT_Vector3T<fpreal64> dir;
    //        GA_Offset start, end;
    //        for (GA_Iterator it(r); it.blockAdvance(start, end); )
    //        {
    //            for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
    //            {
    //                if (scaleByTurns)
    //                {
    //                    n2dAttribPH.value(elemoff) *= 4 * uniScale / n2dAttribPH.value(elemoff).length2();
    //                }
    //                else if (normalize)
    //                {
    //                    n2dAttribPH.value(elemoff).normalize();
    //                    n2dAttribPH.value(elemoff) *= uniScale;
    //                }
    //            }
    //        }
    //    }, subscribeRatio, minGrainSize);




    GA_FeE_TopologyReference::outTopoAttrib(outGeo0, sopparms.getOutTopoAttrib());



}



namespace SOP_FeE_Normal_2D_1_0_Namespace {

} // End SOP_FeE_Normal_2D_1_0_Namespace namespace
