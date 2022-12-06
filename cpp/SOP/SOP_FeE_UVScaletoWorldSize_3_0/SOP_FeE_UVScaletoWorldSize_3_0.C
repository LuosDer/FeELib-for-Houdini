
//#define UT_ASSERT_LEVEL 3
#include "SOP_FeE_UVScaletoWorldSize_3_0.h"


// This is an automatically generated header file based on theDsFile, below,
// to provide SOP_FeE_UVScaletoWorldSize_3_0Parms, an easy way to access parameter values from
// SOP_FeE_UVScaletoWorldSize_3_0Verb::cook with the correct type.
#include "SOP_FeE_UVScaletoWorldSize_3_0.proto.h"

#include "GEO/GEO_Detail.h"
#include "PRM/PRM_TemplateBuilder.h"
#include "UT/UT_Interrupt.h"
#include "UT/UT_DSOVersion.h"

#include "GU/GU_Promote.h"


#include "GA_FeE/GA_FeE_Attribute.h"
#include "GEO_FeE/GEO_FeE_Group.h"
#include "GA_FeE/GA_FeE_Adjacency.h"
#include "GA_FeE/GA_FeE_Measure.h"
#include "GA_FeE/GA_FeE_Connectivity.h"
#include "GA_FeE/GA_FeE_TopologyReference.h"


using namespace SOP_FeE_UVScaletoWorldSize_3_0_Namespace;

using attribPrecisonF = fpreal32;
using TAttribTypeV = UT_Vector3T<attribPrecisonF>;

#define FeE_UseDetachedAttrib 1

//
// Help is stored in a "wiki" style text file.  This text file should be copied
// to $HOUDINI_PATH/help/nodes/sop/FeE.txt
//
// See the sample_install.sh file for an example.
//

/// This is the internal name of the SOP type.
/// It isn't allowed to be the same as any other SOP's type name.
const UT_StringHolder SOP_FeE_UVScaletoWorldSize_3_0::theSOPTypeName("FeE::uvScaletoWorldSize::3.0"_sh);

/// newSopOperator is the hook that Houdini grabs from this dll
/// and invokes to register the SOP.  In this case, we add ourselves
/// to the specified operator table.
void
newSopOperator(OP_OperatorTable *table)
{
    table->addOperator(new OP_Operator(
        SOP_FeE_UVScaletoWorldSize_3_0::theSOPTypeName,   // Internal name
        "FeE UV Scale to World Size",                     // UI name
        SOP_FeE_UVScaletoWorldSize_3_0::myConstructor,    // How to build the SOP
        SOP_FeE_UVScaletoWorldSize_3_0::buildTemplates(), // My parameters
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
        name    "uvAttribClass"
        cppname "UVAttribClass"
        label   "UV Attribute Class"
        type    ordinal
        default { "auto" }
        menu {
            "auto"      "Auto"
            "point"     "Point"
            "vertex"    "Vertex"
        }
    }
    parm {
        name    "uvAttribNames"
        cppname "UVAttribNames"
        label   "UV Attribute Names"
        type    string
        default { "uv" }
    }
    parm {
        name    "computeUVAreaInPiece"
        cppname "ComputeUVAreaInPiece"
        label   "Compute UV Area In Pieces"
        type    toggle
        default { "1" }
    }



    parm {
        name    "doUVScalex"
        cppname "DoUVScalex"
        label   "Do Scale X"
        type    toggle
        default { "1" }
        nolabel
        joinnext
    }
    parm {
        name    "uvScalex"
        cppname "UVScalex"
        label   "UV Scale X"
        type    float
        default { 1 }
        range   { -1 10 }
        disablewhen "{ doUVScalex == 0 }"
    }



    parm {
        name    "doUVScaley"
        cppname "DoUVScaley"
        label   "Do Scale Y"
        type    toggle
        default { "1" }
        nolabel
        joinnext
    }
    parm {
        name    "uvScaley"
        cppname "UVScaley"
        label   "UV Scale Y"
        type    float
        default { 1 }
        range   { -1 10 }
        disablewhen "{ doUVScaley == 0 }"
    }



    parm {
        name    "doUVScalez"
        cppname "DoUVScalez"
        label   "Do Scale Z"
        type    toggle
        default { "1" }
        nolabel
        joinnext
    }
    parm {
        name    "uvScalez"
        cppname "UVScalez"
        label   "UV Scale Z"
        type    float
        default { 1 }
        range   { -1 10 }
        disablewhen "{ doUVScalez == 0 }"
    }

    parm {
        name    "uvScale"
        cppname "UVScale"
        label   "UV Scale"
        type    float
        default { 1 }
        range   { -10 10 }
        disablewhen "{ doUVScalex == 0 doUVScaley == 0 doUVScalez == 0 }"
    }



    parm {
        name    "uvSplitDistThreshold"
        cppname "UVSplitDistThreshold"
        label   "UV Split Distance Threshold"
        type    log
        default { 1e-05 }
        range   { 0! 10 }
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
SOP_FeE_UVScaletoWorldSize_3_0::buildTemplates()
{
    static PRM_TemplateBuilder templ("SOP_FeE_UVScaletoWorldSize_3_0.C"_sh, theDsFile);
    return templ.templates();
}


//class SOP_FeE_UVScaletoWorldSize_3_0Cache : public SOP_NodeCache
//{
//public:
//    SOP_FeE_UVScaletoWorldSize_3_0Cache() : SOP_NodeCache(),
//        myPrevOutputDetailID(-1)
//    {}
//    ~SOP_FeE_UVScaletoWorldSize_3_0Cache() override {}
//
//    int64 myPrevOutputDetailID;
//};


class SOP_FeE_UVScaletoWorldSize_3_0Verb : public SOP_NodeVerb
{
public:
    SOP_FeE_UVScaletoWorldSize_3_0Verb() {}
    virtual ~SOP_FeE_UVScaletoWorldSize_3_0Verb() {}

    virtual SOP_NodeParms *allocParms() const { return new SOP_FeE_UVScaletoWorldSize_3_0Parms(); }
    //virtual SOP_NodeCache* allocCache() const { return new SOP_FeE_UVScaletoWorldSize_3_0Cache(); }
    virtual UT_StringHolder name() const { return SOP_FeE_UVScaletoWorldSize_3_0::theSOPTypeName; }

    virtual CookMode cookMode(const SOP_NodeParms *parms) const { return COOK_GENERIC; }

    virtual void cook(const CookParms &cookparms) const;

    /// This static data member automatically registers
    /// this verb class at library ldir0d time.
    static const SOP_NodeVerb::Register<SOP_FeE_UVScaletoWorldSize_3_0Verb> theVerb;
};

// The static member variable definition has to be outside the class definition.
// The declaration is inside the class.
const SOP_NodeVerb::Register<SOP_FeE_UVScaletoWorldSize_3_0Verb> SOP_FeE_UVScaletoWorldSize_3_0Verb::theVerb;

const SOP_NodeVerb *
SOP_FeE_UVScaletoWorldSize_3_0::cookVerb() const 
{ 
    return SOP_FeE_UVScaletoWorldSize_3_0Verb::theVerb.get();
}




static GA_AttributeOwner
sopAttribOwner(SOP_FeE_UVScaletoWorldSize_3_0Parms::UVAttribClass attribClass)
{
    using namespace SOP_FeE_UVScaletoWorldSize_3_0Enums;
    switch (attribClass)
    {
    case UVAttribClass::AUTO:      return GA_ATTRIB_DETAIL;     break;//not detail but means Auto
    case UVAttribClass::POINT:     return GA_ATTRIB_POINT;      break;
    case UVAttribClass::VERTEX:    return GA_ATTRIB_VERTEX;     break;
    }
    UT_ASSERT_MSG(0, "Unhandled Geo0 Class type!");
    return GA_ATTRIB_INVALID;
}

static GA_GroupType
sopGroupType(SOP_FeE_UVScaletoWorldSize_3_0Parms::GroupType parmgrouptype)
{
    using namespace SOP_FeE_UVScaletoWorldSize_3_0Enums;
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
SOP_FeE_UVScaletoWorldSize_3_0Verb::cook(const SOP_NodeVerb::CookParms &cookparms) const
{
    auto &&sopparms = cookparms.parms<SOP_FeE_UVScaletoWorldSize_3_0Parms>();
    GEO_Detail* outGeo0 = cookparms.gdh().gdpNC();
    //auto sopcache = (SOP_FeE_UVScaletoWorldSize_3_0Cache*)cookparms.cache();

    const GEO_Detail* const inGeo0 = cookparms.inputGeo(0);

    outGeo0->replaceWith(*inGeo0);
    // outGeo0->clearAndDestroy();

    //outGeo0 = sopNodeProcess(*inGeo0);


    const bool& doUVScalex = sopparms.getDoUVScalex();
    const bool& doUVScaley = sopparms.getDoUVScaley();
    const bool& doUVScalez = sopparms.getDoUVScalez();
    if (!doUVScalex && !doUVScaley && !doUVScalez)
        return;

    const UT_StringHolder& geo0AttribNames = sopparms.getUVAttribNames();
    if (!geo0AttribNames.isstring())
        return;


    const GA_GroupType& groupType = sopGroupType(sopparms.getGroupType());

    GOP_Manager gop;
    const GA_Group* geo0Group = GA_FeE_Group::findOrParseGroupDetached(cookparms, outGeo0, groupType, sopparms.getGroup(), gop);
    if (geo0Group && geo0Group->isEmpty())
        return;

    const GA_GroupType& geo0finalGroupType = geo0Group ? geo0Group->classType() : GA_GROUP_INVALID;

    UT_AutoInterrupt boss("Processing");
    if (boss.wasInterrupted())
        return;




    const attribPrecisonF& uniScale = sopparms.getUVScale();
    const attribPrecisonF& uvScalex = sopparms.getUVScalex();
    const attribPrecisonF& uvScaley = sopparms.getUVScaley();
    const attribPrecisonF& uvScalez = sopparms.getUVScalez();

    TAttribTypeV uvScale(doUVScalex, doUVScaley, doUVScalez);
    //TAttribTypeV uvScale(uvScalex, uvScaley, uvScalez);
    uvScale *= TAttribTypeV(uvScalex, uvScaley, uvScalez) * uniScale;

    
    const bool& computeUVAreaInPiece = sopparms.getComputeUVAreaInPiece();
    const GA_AttributeOwner& geo0AttribClass = sopAttribOwner(sopparms.getUVAttribClass());
    //fpreal uvSplitDistThreshold = sopparms.getUVSplitDistThreshold();
    const attribPrecisonF& uvSplitDistThreshold = sopparms.getUVSplitDistThreshold();

    const exint& subscribeRatio = sopparms.getSubscribeRatio();
    const exint& minGrainSize = sopparms.getMinGrainSize();
    //const exint minGrainSize = pow(2, 8);
    //const exint minGrainSize = pow(2, 4);


    const GA_Storage& inStorageI = GA_STORE_INT32;
    //const GA_Storage inStorageF = SYSisSame<T, fpreal32>() ? GA_STORE_REAL32 : GA_STORE_REAL64;
    const GA_Storage& inStorageF = GA_STORE_REAL32;
    







    const UT_StringHolder& geo0AttribNameSub = geo0AttribNames;
    GA_AttributeOwner geo0AttribClassFinal;
    GA_Attribute* attribPtr = GA_FeE_Attribute::findFloatTuplePointVertex(outGeo0, geo0AttribClass, geo0AttribNameSub, geo0AttribClassFinal);
    if (!attribPtr)
        return;

    GA_RWHandleT<TAttribTypeV> attribHandle(attribPtr);


#if FeE_UseDetachedAttrib
#if 0
    GA_ATINumericUPtr areaATI_deleter = outGeo0->createDetachedTupleAttribute(GA_ATTRIB_PRIMITIVE, inStorageF, 1);
    GA_ATINumericUPtr areaUVATI_deleter = outGeo0->createDetachedTupleAttribute(GA_ATTRIB_PRIMITIVE, inStorageF, 1);
    GA_ATINumericUPtr uvScaleATI_deleter = outGeo0->createDetachedTupleAttribute(GA_ATTRIB_PRIMITIVE, inStorageF, 3);
    GA_ATINumeric* areaATIPtr = areaATI_deleter.get();
    GA_ATINumeric* areaUVATIPtr = areaUVATI_deleter.get();
    GA_ATINumeric* uvScaleATIPtr = uvScaleATI_deleter.get();
#else

    GA_Attribute* areaATIPtr   = GA_FeE_Measure::addAttribPrimArea(outGeo0,               "__area_SOP_FeE_UVScaletoWorldSize_3_0",   static_cast<const GA_PrimitiveGroup*>(geo0Group), GA_Defaults(-1.0), inStorageF, subscribeRatio, minGrainSize);
    GA_Attribute* areaUVATIPtr = GA_FeE_Measure::addAttribPrimArea(outGeo0, attribHandle, "__areaUV_SOP_FeE_UVScaletoWorldSize_3_0", static_cast<const GA_PrimitiveGroup*>(geo0Group), GA_Defaults(-1.0), inStorageF, subscribeRatio, minGrainSize);

    if (!areaUVATIPtr)
        return;
    //GA_Attribute* areaATIPtr   = outGeo0->addFloatTuple(GA_ATTRIB_PRIMITIVE, "__area_SOP_FeE_UVScaletoWorldSize_3_0",   1, GA_Defaults(0.0), 0, 0, inStorageF);
    //GA_Attribute* areaUVATIPtr = outGeo0->addFloatTuple(GA_ATTRIB_PRIMITIVE, "__areaUV_SOP_FeE_UVScaletoWorldSize_3_0", 1, GA_Defaults(0.0), 0, 0, inStorageF);
    GA_ATINumericUPtr uvScaleATI_deleter = outGeo0->createDetachedTupleAttribute(GA_ATTRIB_PRIMITIVE, inStorageF, 3);
    GA_ATINumeric* uvScaleATIPtr = uvScaleATI_deleter.get();
#endif
#else
    GA_Attribute* areaATIPtr    = outGeo0->addFloatTuple(GA_ATTRIB_PRIMITIVE, "__area_SOP_FeE_UVScaletoWorldSize_3_0",    1, GA_Defaults(0.0), 0, 0, inStorageF);
    GA_Attribute* areaUVATIPtr  = outGeo0->addFloatTuple(GA_ATTRIB_PRIMITIVE, "__areaUV_SOP_FeE_UVScaletoWorldSize_3_0",  1, GA_Defaults(0.0), 0, 0, inStorageF);
    GA_Attribute* uvScaleATIPtr = outGeo0->addFloatTuple(GA_ATTRIB_PRIMITIVE, "__uvScale_SOP_FeE_UVScaletoWorldSize_3_0", 3, GA_Defaults(1.0), 0, 0, inStorageF);
#endif

    GA_RWHandleT<attribPrecisonF> areaAttribHandle(areaATIPtr);
    GA_RWHandleT<attribPrecisonF> areaUVAttribHandle(areaUVATIPtr);
    GA_RWHandleT<TAttribTypeV> uvScaleAttribHandle(uvScaleATIPtr);

    if (computeUVAreaInPiece)
    {
        GA_ATINumericUPtr vtxpnumAttribUPtr;
        GA_RWHandleT<GA_Size> vtxpnumAttribHandle;

        vtxpnumAttribUPtr = outGeo0->createDetachedTupleAttribute(GA_ATTRIB_VERTEX, inStorageI, 1, GA_Defaults(-1));
        //GA_ATINumeric* vtxpnumATI = vtxpnumAttribUPtr.get();
        vtxpnumAttribHandle.bind(vtxpnumAttribUPtr.get());

        GA_FeE_TopologyReference::vertexPrimIndex(outGeo0, vtxpnumAttribHandle,
            static_cast<const GA_VertexGroup*>(geo0Group),
            subscribeRatio, minGrainSize);


        /// <summary>
        /// ///////////////////////////////
        /// </summary>
        /// <param name="cookparms"></param>


        GA_Attribute* adjElemsAttribPtr = outGeo0->addIntArray(GA_ATTRIB_POINT, "__adjElems_SOP_FeE_UVScaletoWorldSize_3_0", 1, 0, 0, inStorageI);
        GA_RWHandleT<UT_ValArray<GA_Offset>> adjElemsAttribHandle(adjElemsAttribPtr);

        GA_FeE_Adjacency::pointPointEdge(outGeo0, adjElemsAttribHandle, vtxpnumAttribHandle,
            static_cast<const GA_PointGroup*>(geo0Group), nullptr,
            subscribeRatio, minGrainSize);


#if FeE_UseDetachedAttrib
        GA_ATINumericUPtr connectivityATI_deleter = outGeo0->createDetachedTupleAttribute(GA_ATTRIB_POINT, inStorageI, 1, GA_Defaults(-1));
        GA_Attribute* connectivityATIPtr = connectivityATI_deleter.get();
#else
        GA_Attribute* connectivityATIPtr = outGeo0->addIntTuple(GA_ATTRIB_POINT, "__connectivity_SOP_FeE_UVScaletoWorldSize_3_0", 1, GA_Defaults(-1), 0, 0, inStorageI);
#endif
        GA_RWHandleT<GA_Size> connectivityAttribHandle(connectivityATIPtr);
        GA_FeE_Connectivity::connectivityPoint(outGeo0, connectivityAttribHandle, adjElemsAttribHandle, static_cast<const GA_PointGroup*>(geo0Group));



#if FeE_UseDetachedAttrib
        GA_AttributeUPtr connectivityAPtr_deleter = GU_Promote::create(GA_ATTRIB_PRIMITIVE, *connectivityATIPtr, GU_Promote::GU_PROMOTE_FIRST);
        connectivityATIPtr = connectivityAPtr_deleter.get();
#else
        connectivityATIPtr = GU_Promote::promote(*static_cast<GU_Detail*>(outGeo0), connectivityATIPtr, GA_ATTRIB_PRIMITIVE, true, GU_Promote::GU_PROMOTE_FIRST, NULL);
#endif
        connectivityAttribHandle = connectivityATIPtr;




#if FeE_UseDetachedAttrib
        areaAttribHandle   = GU_Promote::promote(*static_cast<GU_Detail*>(outGeo0), areaATIPtr,   GA_ATTRIB_PRIMITIVE, false, GU_Promote::GU_PROMOTE_SUM, NULL, connectivityATIPtr);
        areaUVAttribHandle = GU_Promote::promote(*static_cast<GU_Detail*>(outGeo0), areaUVATIPtr, GA_ATTRIB_PRIMITIVE, false, GU_Promote::GU_PROMOTE_SUM, NULL, connectivityATIPtr);
#else
        areaAttribHandle   = GU_Promote::promote(*static_cast<GU_Detail*>(outGeo0), areaATIPtr,   GA_ATTRIB_PRIMITIVE, true, GU_Promote::GU_PROMOTE_SUM, NULL, connectivityATIPtr);
        areaUVAttribHandle = GU_Promote::promote(*static_cast<GU_Detail*>(outGeo0), areaUVATIPtr, GA_ATTRIB_PRIMITIVE, true, GU_Promote::GU_PROMOTE_SUM, NULL, connectivityATIPtr);
#endif

    }

    const int& attribSize = attribPtr->getTupleSize();
    //template <typename T>
    switch (attribSize)
    {
    case 3:
        break;
    default:
        UT_ASSERT_MSG(0, "Unhandled outGeo0 Attrib Size");
    }



    {
        const GA_SplittableRange geo0SplittableRange(outGeo0->getPrimitiveRange());
        UTparallelFor(geo0SplittableRange, [&outGeo0, &uvScaleAttribHandle, &areaUVAttribHandle, &areaAttribHandle, &uvScale, &doUVScalex, &doUVScaley, &doUVScalez](const GA_SplittableRange& r)
        {
            GA_Offset start, end;
            for (GA_Iterator it(r); it.blockAdvance(start, end); )
            {
                for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                {
                    const attribPrecisonF& areaUV = areaUVAttribHandle.get(elemoff);
                    const attribPrecisonF& area = areaAttribHandle.get(elemoff);
                    TAttribTypeV uvS = uvScale * sqrt(area / areaUV);
                    uvS[0] = doUVScalex ? uvS[0] : 1;
                    uvS[1] = doUVScaley ? uvS[1] : 1;
                    uvS[2] = doUVScalez ? uvS[2] : 1;
                    uvScaleAttribHandle.set(elemoff, uvS);
                }
            }
        }, subscribeRatio, minGrainSize);
    }

    {
        //const GA_Range geo0Range = GA_FeE_Group::getRangeByAnyGroup(outGeo0, geo0Group, geo0AttribClassFinal);
        const GA_SplittableRange geo0SplittableRange = GEO_FeE_Group::getSplittableRangeByAnyGroup(outGeo0, geo0Group, geo0AttribClassFinal);
        UTparallelFor(geo0SplittableRange, [&outGeo0, &attribHandle, &uvScaleAttribHandle, &geo0AttribClassFinal](const GA_SplittableRange& r)
        {
            GA_Offset start, end;
            for (GA_Iterator it(r); it.blockAdvance(start, end); )
            {
                for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                {
                    const GA_Offset& primoff = geo0AttribClassFinal == GA_ATTRIB_POINT ? outGeo0->vertexPrimitive(outGeo0->pointVertex(elemoff)) : outGeo0->vertexPrimitive(elemoff);
                    TAttribTypeV uv = attribHandle.get(elemoff) * uvScaleAttribHandle.get(primoff);
                    attribHandle.set(elemoff, uv);
                }
            }
        }, subscribeRatio, minGrainSize);
    }

    attribPtr->bumpDataId();


#if FeE_UseDetachedAttrib
    outGeo0->destroyAttribute(GA_ATTRIB_PRIMITIVE, "__area_SOP_FeE_UVScaletoWorldSize_3_0");
    outGeo0->destroyAttribute(GA_ATTRIB_PRIMITIVE, "__areaUV_SOP_FeE_UVScaletoWorldSize_3_0");
    outGeo0->destroyAttribute(GA_ATTRIB_POINT, "__adjElems_SOP_FeE_UVScaletoWorldSize_3_0");
#endif
}



namespace SOP_FeE_UVScaletoWorldSize_3_0_Namespace {

} // End SOP_FeE_UVScaletoWorldSize_3_0_Namespace namespace
