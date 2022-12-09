
//#define UT_ASSERT_LEVEL 3
#include "SOP_FeE_DelAllAttribGroup_1_0.h"


#include "SOP_FeE_DelAllAttribGroup_1_0.proto.h"

#include "GEO/GEO_Detail.h"
#include "PRM/PRM_TemplateBuilder.h"
#include "UT/UT_Interrupt.h"
#include "UT/UT_DSOVersion.h"


#include "GEO_FeE/GEO_FeE_Group.h"
#include "GA_FeE/GA_FeE_Attribute.h"
#include "GA_FeE/GA_FeE_VertexNextEquiv.h"
#include "GA_FeE/GA_FeE_Detail.h"

using namespace SOP_FeE_DelAllAttribGroup_1_0_Namespace;



static const char *theDsFile = R"THEDSFILE(
{
    name        parameters

    parm {
        name    "doKeepPrimAttrib"
        cppname "DoKeepPrimAttrib"
        label   "Do Keep Prim Attrib"
        type    toggle
        default { 1 }
        nolabel
        joinnext
    }
    parm {
        name    "keepPrimAttribName"
        cppname "KeepPrimAttribName"
        label   "Keep Prim Attrib Name"
        type    string
        default { "" }
        disablewhen "{ doKeepPrimAttrib == 0 }"
    }



    parm {
        name    "doKeepPointAttrib"
        cppname "DoKeepPointAttrib"
        label   "Do Keep Point Attrib"
        type    toggle
        default { 1 }
        nolabel
        joinnext
    }
    parm {
        name    "keepPointAttribName"
        cppname "KeepPointAttribName"
        label   "Keep Point Attrib Name"
        type    string
        default { "" }
        disablewhen "{ doKeepPointAttrib == 0 }"
    }



    parm {
        name    "doKeepVertexAttrib"
        cppname "DoKeepVertexAttrib"
        label   "Do Keep Vertex Attrib"
        type    toggle
        default { 1 }
        nolabel
        joinnext
    }
    parm {
        name    "keepVertexAttribName"
        cppname "KeepVertexAttribName"
        label   "Keep Vertex Attrib Name"
        type    string
        default { "" }
        disablewhen "{ doKeepVertexAttrib == 0 }"
    }





    parm {
        name    "doKeepDetailAttrib"
        cppname "DoKeepDetailAttrib"
        label   "Do Keep Detail Attrib"
        type    toggle
        default { 1 }
        nolabel
        joinnext
    }
    parm {
        name    "keepDetailAttribName"
        cppname "KeepDetailAttribName"
        label   "Keep Detail Attrib Name"
        type    string
        default { "" }
        disablewhen "{ doKeepDetailAttrib == 0 }"
    }



    parm {
        name    "sepparm"
        label   "Separator"
        type    separator
        default { "" }
    }



    parm {
        name    "doKeepPointGroup"
        cppname "DoKeepPointGroup"
        label   "Do Keep Point Group"
        type    toggle
        default { 1 }
        nolabel
        joinnext
    }
    parm {
        name    "keepPointGroupName"
        cppname "KeepPointGroupName"
        label   "Keep Point Group Name"
        type    string
        default { "" }
        disablewhen "{ doKeepPointGroup == 0 }"
    }



    parm {
        name    "doKeepVertexGroup"
        cppname "DoKeepVertexGroup"
        label   "Do Keep Vertex Group"
        type    toggle
        default { 1 }
        nolabel
        joinnext
    }
    parm {
        name    "keepVertexGroupName"
        cppname "KeepVertexGroupName"
        label   "Keep Vertex Group Name"
        type    string
        default { "" }
        disablewhen "{ doKeepVertexGroup == 0 }"
    }



    parm {
        name    "doKeepPrimGroup"
        cppname "DoKeepPrimGroup"
        label   "Do Keep Prim Group"
        type    toggle
        default { 1 }
        nolabel
        joinnext
    }
    parm {
        name    "keepPrimGroupName"
        cppname "KeepPrimGroupName"
        label   "Keep Prim Group Name"
        type    string
        default { "" }
        disablewhen "{ doKeepPrimGroup == 0 }"
    }



    parm {
        name    "doKeepEdgeGroup"
        cppname "DoKeepEdgeGroup"
        label   "Do Keep Edge Group"
        type    toggle
        default { 1 }
        nolabel
        joinnext
    }
    parm {
        name    "keepEdgeGroupName"
        cppname "KeepEdgeGroupName"
        label   "Keep Edge Group Name"
        type    string
        default { "" }
        disablewhen "{ doKeepEdgeGroup == 0 }"
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
SOP_FeE_DelAllAttribGroup_1_0::buildTemplates()
{
    static PRM_TemplateBuilder templ("SOP_FeE_DelAllAttribGroup_1_0.C"_sh, theDsFile);
    return templ.templates();
}


const UT_StringHolder SOP_FeE_DelAllAttribGroup_1_0::theSOPTypeName("FeE::delAllAttribGroup::1.0"_sh);

void
newSopOperator(OP_OperatorTable* table)
{
    OP_Operator* newOp = new OP_Operator(
        SOP_FeE_DelAllAttribGroup_1_0::theSOPTypeName,
        "FeE Delete All Attributes Groups",
        SOP_FeE_DelAllAttribGroup_1_0::myConstructor,
        SOP_FeE_DelAllAttribGroup_1_0::buildTemplates(),
        1,
        1,
        nullptr,
        0,
        nullptr,
        1,
        "Five elements Elf/Attribute");

    newOp->setIconName("SOP_clean");
    table->addOperator(newOp);

}


//class SOP_FeE_DelAllAttribGroup_1_0Cache : public SOP_NodeCache
//{
//public:
//    SOP_FeE_DelAllAttribGroup_1_0Cache() : SOP_NodeCache(),
//        myPrevOutputDetailID(-1)
//    {}
//    ~SOP_FeE_DelAllAttribGroup_1_0Cache() override {}
//
//    int64 myPrevOutputDetailID;
//};


class SOP_FeE_DelAllAttribGroup_1_0Verb : public SOP_NodeVerb
{
public:
    SOP_FeE_DelAllAttribGroup_1_0Verb() {}
    virtual ~SOP_FeE_DelAllAttribGroup_1_0Verb() {}

    virtual SOP_NodeParms *allocParms() const { return new SOP_FeE_DelAllAttribGroup_1_0Parms(); }
    //virtual SOP_NodeCache* allocCache() const { return new SOP_FeE_DelAllAttribGroup_1_0Cache(); }
    virtual UT_StringHolder name() const { return SOP_FeE_DelAllAttribGroup_1_0::theSOPTypeName; }

    virtual CookMode cookMode(const SOP_NodeParms *parms) const { return COOK_GENERIC; }

    virtual void cook(const CookParms &cookparms) const;

    static const SOP_NodeVerb::Register<SOP_FeE_DelAllAttribGroup_1_0Verb> theVerb;
};

const SOP_NodeVerb::Register<SOP_FeE_DelAllAttribGroup_1_0Verb> SOP_FeE_DelAllAttribGroup_1_0Verb::theVerb;

const SOP_NodeVerb *
SOP_FeE_DelAllAttribGroup_1_0::cookVerb() const 
{ 
    return SOP_FeE_DelAllAttribGroup_1_0Verb::theVerb.get();
}





void
SOP_FeE_DelAllAttribGroup_1_0Verb::cook(const SOP_NodeVerb::CookParms &cookparms) const
{
    auto &&sopparms = cookparms.parms<SOP_FeE_DelAllAttribGroup_1_0Parms>();
    GU_Detail* outGeo0 = cookparms.gdh().gdpNC();
    //auto sopcache = (SOP_FeE_DelAllAttribGroup_1_0Cache*)cookparms.cache();

    const GA_Detail* const inGeo0 = cookparms.inputGeo(0);

    outGeo0->replaceWith(*inGeo0);
    GA_AttributeSet& attribSet = outGeo0->getAttributes();





    //GA_AttributeFilter   primGroupFilter = GA_AttributeFilter::selectByName(sopparms.getKeepPrimGroupName());
    //GA_AttributeFilter  pointGroupFilter = GA_AttributeFilter::selectByName(sopparms.getKeepPointGroupName());
    //GA_AttributeFilter vertexGroupFilter = GA_AttributeFilter::selectByName(sopparms.getKeepVertexGroupName());
    //GA_AttributeFilter   edgeGroupFilter = GA_AttributeFilter::selectByName(sopparms.getKeepEdgeGroupName());

    //GA_AttributeFilter groupFilter;
    //groupFilter = GA_AttributeFilter::selectOr(groupFilter,   primGroupFilter);
    //groupFilter = GA_AttributeFilter::selectOr(groupFilter,  pointGroupFilter);
    //groupFilter = GA_AttributeFilter::selectOr(groupFilter, vertexGroupFilter);
    //groupFilter = GA_AttributeFilter::selectOr(groupFilter,   edgeGroupFilter);


    //GA_AttributeFilter   primAttribFilter = GA_AttributeFilter::selectByName(sopparms.getKeepPrimAttribName());
    //GA_AttributeFilter  pointAttribFilter = GA_AttributeFilter::selectByName(sopparms.getKeepPointAttribName());
    //GA_AttributeFilter vertexAttribFilter = GA_AttributeFilter::selectByName(sopparms.getKeepVertexAttribName());
    //GA_AttributeFilter detailAttribFilter = GA_AttributeFilter::selectByName(sopparms.getKeepDetailAttribName());

    //GA_AttributeFilter attribFilter;
    //attribFilter = GA_AttributeFilter::selectOr(attribFilter,   primAttribFilter);
    //attribFilter = GA_AttributeFilter::selectOr(attribFilter,  pointAttribFilter);
    //attribFilter = GA_AttributeFilter::selectOr(attribFilter, vertexAttribFilter);
    //attribFilter = GA_AttributeFilter::selectOr(attribFilter, detailAttribFilter);





    //groupBaseFilter = GA_AttributeFilter::selectNot(groupBaseFilter);
    //attribFilter = GA_AttributeFilter::selectAnd(attribFilter, groupBaseFilter);

    //GA_AttributeFilter finalFilter = GA_AttributeFilter::selectOr(attribFilter, groupFilter);
    //outGeo0->replaceWith(*inGeo0, &finalFilter);

    //const GA_GroupType groupType = sopGroupType(sopparms.getGroupType());


    //const exint subscribeRatio = sopparms.getSubscribeRatio();
    //const exint minGrainSize = sopparms.getMinGrainSize();

    UT_AutoInterrupt boss("Processing");
    if (boss.wasInterrupted())
        return;

#if 1
    if (sopparms.getDoKeepPrimAttrib())
    {
        GA_AttributeFilter attribFilter = GA_AttributeFilter::selectByName(sopparms.getKeepPrimAttribName());
        GA_FeE_Attribute::keepStdAttribute(attribSet, GA_ATTRIB_PRIMITIVE, attribFilter);
    }
    if (sopparms.getDoKeepPointAttrib())
    {
        GA_AttributeFilter attribFilter = GA_AttributeFilter::selectByName(sopparms.getKeepPointAttribName());
        GA_FeE_Attribute::keepStdAttribute(attribSet, GA_ATTRIB_POINT, attribFilter);
    }
    if (sopparms.getDoKeepVertexAttrib())
    {
        GA_AttributeFilter attribFilter = GA_AttributeFilter::selectByName(sopparms.getKeepVertexAttribName());
        GA_FeE_Attribute::keepStdAttribute(attribSet, GA_ATTRIB_VERTEX, attribFilter);
    }
    if (sopparms.getDoKeepDetailAttrib())
    {
        GA_AttributeFilter attribFilter = GA_AttributeFilter::selectByName(sopparms.getKeepDetailAttribName());
        GA_FeE_Attribute::keepStdAttribute(attribSet, GA_ATTRIB_DETAIL, attribFilter);
    }
#else
    if (sopparms.getDoKeepPrimAttrib())
    {
        GA_AttributeFilter   primAttribFilter = GA_AttributeFilter::selectByName(sopparms.getKeepPrimAttribName());
        primAttribFilter = GA_AttributeFilter::selectNot(primAttribFilter);
        attribSet.destroyAttributes(GA_ATTRIB_PRIMITIVE, primAttribFilter);
        attribSet.bumpAllDataIds(GA_ATTRIB_PRIMITIVE);
    }
    if (sopparms.getDoKeepPointAttrib())
    {
        GA_AttributeFilter   pointAttribFilter = GA_AttributeFilter::selectByName(sopparms.getKeepPointAttribName());
        pointAttribFilter = GA_AttributeFilter::selectNot(pointAttribFilter);
        attribSet.destroyAttributes(GA_ATTRIB_POINT, pointAttribFilter);
        attribSet.bumpAllDataIds(GA_ATTRIB_POINT);
    }
    if (sopparms.getDoKeepVertexAttrib())
    {
        GA_AttributeFilter   vertexAttribFilter = GA_AttributeFilter::selectByName(sopparms.getKeepVertexAttribName());
        vertexAttribFilter = GA_AttributeFilter::selectNot(vertexAttribFilter);
        attribSet.destroyAttributes(GA_ATTRIB_VERTEX, vertexAttribFilter);
        attribSet.bumpAllDataIds(GA_ATTRIB_VERTEX);
    }
    if (sopparms.getDoKeepDetailAttrib())
    {
        GA_AttributeFilter   detailAttribFilter = GA_AttributeFilter::selectByName(sopparms.getKeepDetailAttribName());
        detailAttribFilter = GA_AttributeFilter::selectNot(detailAttribFilter);
        attribSet.destroyAttributes(GA_ATTRIB_DETAIL, detailAttribFilter);
        attribSet.bumpAllDataIds(GA_ATTRIB_DETAIL);
    }
#endif



    /*
    GA_AttributeFilter groupBaseFilter = GA_AttributeFilter::selectGroup();

    if (sopparms.getDoKeepPrimGroup())
    {
        GA_AttributeFilter   primGroupFilter = GA_AttributeFilter::selectByName(sopparms.getKeepPrimGroupName());
        primAttribFilter = GA_AttributeFilter::selectNot(primAttribFilter);
        attribSet.destroyAttributes(GA_ATTRIB_PRIMITIVE, primAttribFilter);
        attribSet.bumpAllDataIds(GA_ATTRIB_PRIMITIVE);
    }
    if (sopparms.getDoKeepPointGroup())
    {
        GA_AttributeFilter   pointGroupFilter = GA_AttributeFilter::selectByName(sopparms.getKeepPointGroupName());
        pointAttribFilter = GA_AttributeFilter::selectNot(pointAttribFilter);
        attribSet.destroyAttributes(GA_ATTRIB_POINT, pointAttribFilter);
        attribSet.bumpAllDataIds(GA_ATTRIB_POINT);
    }
    if (sopparms.getDoKeepVertexGroup())
    {
        GA_AttributeFilter   vertexGroupFilter = GA_AttributeFilter::selectByName(sopparms.getKeepVertexGroupName());
        vertexAttribFilter = GA_AttributeFilter::selectNot(vertexAttribFilter);
        attribSet.destroyAttributes(GA_ATTRIB_VERTEX, vertexAttribFilter);
        attribSet.bumpAllDataIds(GA_ATTRIB_VERTEX);
    }
    if (sopparms.getDoKeepEdgeGroup())
    {
        GA_AttributeFilter   edgeGroupFilter = GA_AttributeFilter::selectByName(sopparms.getKeepEdgeGroupName());
        detailAttribFilter = GA_AttributeFilter::selectNot(detailAttribFilter);
        attribSet.destroyAttributes(GA_ATTRIB_DETAIL, detailAttribFilter);
        attribSet.bumpAllDataIds(GA_ATTRIB_DETAIL);
    }
    */
    /*
    for (GA_GroupType groupType : {GA_GROUP_PRIMITIVE, GA_GROUP_POINT, GA_GROUP_VERTEX, GA_GROUP_EDGE})
    {
        GA_GroupTable* groupTable = outGeo0->getGroupTable(groupType);
        //for (GA_GroupTable::iterator it = groupTable->beginTraverse(); !it.atEnd(); it.operator++())
        for (GA_GroupTable::iterator it = groupTable->beginTraverse(); !it.atEnd(); ++it)
        {
            GA_Group* group = it.group();
            if (group->isDetached())
                continue;
            if (!group->getName().startsWith("__topo_"))
                continue;
            groupTable->destroy(group);
        }
    }
    */

}



namespace SOP_FeE_DelAllAttribGroup_1_0_Namespace {

} // End SOP_FeE_DelAllAttribGroup_1_0_Namespace namespace
