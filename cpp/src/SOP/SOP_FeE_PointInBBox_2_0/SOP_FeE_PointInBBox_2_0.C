
//#define UT_ASSERT_LEVEL 3
#include "SOP_FeE_PointInBBox_2_0.h"


#include "SOP_FeE_PointInBBox_2_0.proto.h"

#include "GA/GA_Detail.h"
#include "PRM/PRM_TemplateBuilder.h"
#include "UT/UT_Interrupt.h"
#include "UT/UT_DSOVersion.h"




#include "GA_FeE/GA_FeE_PointInBBox.h"




using namespace SOP_FeE_PointInBBox_1_0_Namespace;


static const char *theDsFile = R"THEDSFILE(
{
    name        parameters
    parm {
        name    "group"
        cppname "Group"
        label   "Group"
        type    string
        default { "" }
        range   { 0 1 }
        parmtag { "script_action" "import soputils\nkwargs['geometrytype'] = kwargs['node'].parmTuple('groupType')\nkwargs['inputindex'] = 0\nsoputils.selectGroupParm(kwargs)" }
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
        name    "groupRef"
        cppname "GroupRef"
        label   "Group Ref"
        type    string
        default { "" }
        range   { 0 1 }
        parmtag { "script_action" "import soputils\nkwargs['geometrytype'] = kwargs['node'].parmTuple('groupTypeRef')\nkwargs['inputindex'] = 0\nsoputils.selectGroupParm(kwargs)" }
        parmtag { "script_action_help" "Select geometry from an available viewport." }
        parmtag { "script_action_icon" "BUTTONS_reselect" }
    }
    parm {
        name    "groupTypeRef"
        cppname "GroupTypeRef"
        label   "Group Type Ref"
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
        name    "groupName"
        cppname "GroupName"
        label   "Group Name"
        type    string
        default { "`opname('.')`" }
    }
    parm {
        name    "groupMergeType"
        cppname "GroupMergeType"
        label   "Group Merge Type"
        type    ordinal
        default { "replace" }
        menu {
            "replace"   "Replace Existing"
            "union"     "Union with Existing"
            "intersect" "Intersect with Existing"
            "subtract"  "Subtract from Existing"
        }
    }
    parm {
        name    "intersectNum"
        cppname "IntersectNum"
        label   "Intersect Num"
        type    integer
        default { "0" }
        range   { 0! 2! }
    }
    parm {
        name    "xn"
        cppname "XN"
        label   "XN"
        type    toggle
        nolabel
        joinnext
        default { "0" }
    }
    parm {
        name    "xnThreshold"
        cppname "XNThreshold"
        label   "X Negative"
        type    log
        default { "0" }
        disablewhen "{ xn == 0 }"
        range   { 1e-06 10 }
    }
    parm {
        name    "xp"
        cppname "XP"
        label   "XP"
        type    toggle
        nolabel
        joinnext
        default { "0" }
    }
    parm {
        name    "xpThreshold"
        cppname "XPThreshold"
        label   "X Positive"
        type    log
        default { "0" }
        disablewhen "{ xp == 0 }"
        range   { 1e-06 10 }
    }
    parm {
        name    "yn"
        cppname "YN"
        label   "YN"
        type    toggle
        nolabel
        joinnext
        default { "0" }
    }
    parm {
        name    "ynThreshold"
        cppname "YNThreshold"
        label   "Y Negative"
        type    log
        default { "0" }
        disablewhen "{ yn == 0 }"
        range   { 1e-06 10 }
    }
    parm {
        name    "yp"
        cppname "YP"
        label   "YP"
        type    toggle
        nolabel
        joinnext
        default { "0" }
    }
    parm {
        name    "ypThreshold"
        cppname "YPThreshold"
        label   "Y Positive"
        type    log
        default { "0" }
        disablewhen "{ yp == 0 }"
        range   { 1e-06 10 }
    }
    parm {
        name    "zn"
        cppname "ZN"
        label   "ZN"
        type    toggle
        nolabel
        joinnext
        default { "0" }
    }
    parm {
        name    "znThreshold"
        cppname "ZNThreshold"
        label   "Z Negative"
        type    log
        default { "0" }
        disablewhen "{ zn == 0 }"
        range   { 1e-06 10 }
    }
    parm {
        name    "zp"
        cppname "ZP"
        label   "ZP"
        type    toggle
        nolabel
        joinnext
        default { "0" }
    }
    parm {
        name    "zpThreshold"
        cppname "ZPThreshold"
        label   "Z Positive"
        type    log
        default { "0" }
        disablewhen "{ zp == 0 }"
        range   { 1e-06 10 }
    }
    parm {
        name    "numingroup_min"
        cppname "SubscribeRatio"
        label   "Num in Group Min"
        type    integer
        default { "1" }
        disablewhen "{ onlyfull == 1 }"
        range   { 1! 10 }
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
SOP_FeE_PointInBBox_2_0::buildTemplates()
{
    static PRM_TemplateBuilder templ("SOP_FeE_PointInBBox_2_0.C"_sh, theDsFile);
    if (templ.justBuilt())
    {
        //templ.setChoiceListPtr("group"_sh, &SOP_Node::groupMenu);
        templ.setChoiceListPtr("posAttribName"_sh, &SOP_Node::allTextureCoordMenu);
        
    }
    return templ.templates();
}

const UT_StringHolder SOP_FeE_PointInBBox_2_0::theSOPTypeName("FeE::pointInBBox::2.0"_sh);

void
newSopOperator(OP_OperatorTable* table)
{
    OP_Operator* newOp = new OP_Operator(
        SOP_FeE_PointInBBox_2_0::theSOPTypeName,
        "FeE Point in Boundin Box",
        SOP_FeE_PointInBBox_2_0::myConstructor,
        SOP_FeE_PointInBBox_2_0::buildTemplates(),
        1,
        2,
        nullptr,
        OP_FLAG_GENERATOR,
        nullptr,
        1,
        "Five elements Elf/Group/Bound");

    newOp->setIconName("SOP_bound");
    table->addOperator(newOp);

}





class SOP_FeE_PointInBBox_1_0Verb : public SOP_NodeVerb
{
public:
    SOP_FeE_PointInBBox_1_0Verb() {}
    virtual ~SOP_FeE_PointInBBox_1_0Verb() {}

    virtual SOP_NodeParms *allocParms() const { return new SOP_FeE_PointInBBox_1_0Parms(); }
    virtual UT_StringHolder name() const { return SOP_FeE_PointInBBox_2_0::theSOPTypeName; }

    virtual CookMode cookMode(const SOP_NodeParms *parms) const { return COOK_GENERIC; }

    virtual void cook(const CookParms &cookparms) const;
    
    /// This static data member automatically registers
    /// this verb class at library load time.
    static const SOP_NodeVerb::Register<SOP_FeE_PointInBBox_1_0Verb> theVerb;
};

// The static member variable definition has to be outside the class definition.
// The declaration is inside the class.
const SOP_NodeVerb::Register<SOP_FeE_PointInBBox_1_0Verb> SOP_FeE_PointInBBox_1_0Verb::theVerb;

const SOP_NodeVerb *
SOP_FeE_PointInBBox_2_0::cookVerb() const 
{ 
    return SOP_FeE_PointInBBox_1_0Verb::theVerb.get();
}





//// Calls functor on every active offset in this index map.
//template<typename FUNCTOR>
//SYS_FORCE_INLINE
//void forEachOffset(GA_IndexMap& idxmap, FUNCTOR&& functor)
//{
//    if (idxmap.isTrivialMap())
//    {
//        const GA_Offset end = GA_Offset(GA_Size(idxmap.indexSize()));
//        for (GA_Offset off(0); off != end; ++off)
//        {
//            functor(off, off);
//        }
//    }
//    else
//    {
//        const GA_Offset veryend(idxmap.myMaxOccupiedOffset + 1);
//        GA_Size idx(0);
//        GA_Offset off(0);
//        while (true)
//        {
//            off = idxmap.findActiveOffset(off, veryend);
//            GA_Offset end = idxmap.findInactiveOffset(off, veryend);
//            if (off == end)
//                break;
//            do
//            {
//                functor(off, idx);
//                ++off;
//                ++idx;
//            } while (off != end);
//        }
//    }
//}




/*
template<typename FUNCTOR>
static void forEachOffset(FUNCTOR&& functor, const GA_IndexMap& index_map, const GA_ElementGroup* group = nullptr, bool complement = false)
{
    // Fall back to regular case if no group.
    //if (!group)
    //{
    //    if (!complement)
    //        index_map.forEachOffset(functor);
    //    return;
    //}

    // Group order is only relevant if not complemented.
    if (!complement)
    {
        const GA_ElementGroupOrder* order = group->getOrdered();
        if (order)
        {
            GA_Size idx(0);
            for (GA_ElementGroupOrderIndex i(0), n(order->entries()); i != n; ++i)
            {
                GA_Offset off = order->getElement(i);
                functor(off, idx);
                ++idx;
            }
            return;
        }
    }

    // We have a group, treated as unordered.
    const GA_Offset veryend = index_map.offsetSize();
    GA_Size idx(0);
    GA_Offset off(0);
    while (true)
    {
        bool value;
        GA_Size span_size;
        group->getConstantSpan(off, veryend, span_size, value);
        if (span_size == 0)
            break;
        if (value == complement)
        {
            off += span_size;
            continue;
        }
        const GA_Offset span_end = off + span_size;
        while (true)
        {
            off = index_map.findActiveOffset(off, span_end);
            GA_Offset end = index_map.findInactiveOffset(off, span_end);
            if (off == end)
                break;
            do
            {
                functor(off, idx);
                ++off;
                ++idx;
            } while (off != end);
        }
    }
}

template<typename FUNCTOR>
SYS_FORCE_INLINE
void forEachPrimitive(GA_Detail* geo, const GA_PrimitiveGroup* group, bool complement, FUNCTOR&& functor)
{
    forEachOffset(functor, geo->getPrimitiveMap(), group, complement);
}

*/


//template<typename FUNCTOR>
//SYS_FORCE_INLINE
//void forEachVertex(GA_Detail* geo, const GA_VertexGroup* group, bool complement, FUNCTOR&& functor)
//{
//    forEachOffset(functor, geo->getVertexMap(), group, complement);
//}






void
SOP_FeE_PointInBBox_1_0Verb::cook(const SOP_NodeVerb::CookParms &cookparms) const
{
    auto&& sopparms = cookparms.parms<SOP_FeE_PointInBBox_1_0Parms>();
    GA_Detail* const outGeo0 = cookparms.gdh().gdpNC();
    //auto sopcache = (SOP_FeE_PointInBBox_1_0Cache*)cookparms.cache();

    const GA_Detail* const inGeo0 = cookparms.inputGeo(0);
    const GA_Detail* const inGeo1 = cookparms.inputGeo(1);
    const GA_Detail* const inGeo2 = cookparms.inputGeo(2);

    outGeo0->replaceWith(*inGeo0);

    const bool xn = sopparms.getXN();
    const bool xp = sopparms.getXP();
    const bool yn = sopparms.getYN();
    const bool yp = sopparms.getYP();
    const bool zn = sopparms.getZN();
    const bool zp = sopparms.getZP();

    const fpreal xnThreshold = sopparms.getXNThreshold();
    const fpreal xpThreshold = sopparms.getXPThreshold();
    const fpreal ynThreshold = sopparms.getYNThreshold();
    const fpreal ypThreshold = sopparms.getYPThreshold();
    const fpreal znThreshold = sopparms.getZNThreshold();
    const fpreal zpThreshold = sopparms.getZPThreshold();

    const exint subscribeRatio = sopparms.getSubscribeRatio();
    const exint minGrainSize = sopparms.getMinGrainSize();

    const UT_StringHolder& groupName = sopparms.getSeamGroup();
    const UT_StringHolder& posAttribName = sopparms.getPosAttribName();

    //const GA_Storage inStorageI = GA_FeE_Type::getPreferredStorageI(outGeo0);

    UT_AutoInterrupt boss("Processing");
    if (boss.wasInterrupted())
        return;



    GA_PointGroup* groupPtr = GA_FeE_PointInBBox::addGroupPointInBBox(cookparms, outGeo0, inGeo1, xn, xp, yn, yp, zn, zp);
}



namespace SOP_FeE_PointInBBox_1_0_Namespace {

} // End SOP_FeE_PointInBBox_1_0_Namespace namespace
