
#pragma once

#ifndef __GFE_AttributePromote_h__
#define __GFE_AttributePromote_h__

//#include "GFE/GFE_AttributePromote.h"

#include "GA/GA_Detail.h"





#include "GFE/GFE_TopologyReference.h"

//#include "GFE/GFE_Type.h"

namespace GFE_AttributePromote {


static void
    attribPromote(
        const GA_Detail* const geo,
        GA_Attribute* const dstAttribPtr,
        const GA_Attribute* const srcAttribPtr,
        const exint subscribeRatio = 16,
        const exint minGrainSize = 1024
    )
{
    UT_ASSERT_P(dstAttribPtr);
    UT_ASSERT_P(srcAttribPtr);
    UT_ASSERT_P(dstAttribPtr != srcAttribPtr);

    UT_ASSERT_P(geo == &dstAttribPtr->getDetail());
    UT_ASSERT_P(geo == &srcAttribPtr->getDetail());

    const GA_AttributeOwner dstClass = dstAttribPtr->getOwner();
    const GA_AttributeOwner srcClass = srcAttribPtr->getOwner();

    //GA_RWHandleT<> newAttrib_h(newAttribPtr);
    //GA_ROHandleT<> attrib_h(attribPtr);
    const GA_SplittableRange geoSplittableRange(GA_Range(dstAttribPtr->getIndexMap()));
    UTparallelFor(geoSplittableRange, [geo, dstAttribPtr, srcAttribPtr, srcClass, dstClass](const GA_SplittableRange& r)
    {
        GA_Offset start, end;
        for (GA_Iterator it(r); it.blockAdvance(start, end); )
        {
            for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
            {
                //dstAttribPtr->copy(elemoff, *srcAttribPtr, GFE_TopologyReference::offsetPromote(geo, srcClass, dstClass, elemoff));
                dstAttribPtr->copy(elemoff, *srcAttribPtr, GFE_TopologyReference::offsetPromote(geo, dstClass, srcClass, elemoff));
            }
        }
    }, subscribeRatio, minGrainSize);
}


SYS_FORCE_INLINE
static void
    attribPromote(
        GA_Attribute* const dstAttribPtr,
        const GA_Attribute* const srcAttribPtr,
        const exint subscribeRatio = 16,
        const exint minGrainSize = 1024
    )
{
    UT_ASSERT_P(srcAttribPtr);
    attribPromote(&srcAttribPtr->getDetail(), dstAttribPtr, srcAttribPtr, subscribeRatio, minGrainSize);
}







SYS_FORCE_INLINE
static GA_AttributeUPtr
attribPromoteDetached(
    const GA_Detail* const geo,
    const GA_Attribute* const attribPtr,
    const GA_AttributeOwner newClass,
    const exint subscribeRatio = 16,
    const exint minGrainSize = 1024
)
{
    UT_ASSERT_P(geo);
    if (!attribPtr)
        return GA_AttributeUPtr();
#if 0
    GA_Attribute* const newAttribPtr = geo->getAttributes().cloneTempAttribute(newClass, *attribPtr, true);
    GA_AttributeUPtr newAttribUPtr(newAttribPtr);
    attribPromote(newAttribPtr, attribPtr, subscribeRatio, minGrainSize);
#else
    GA_AttributeUPtr newAttribUPtr = geo->getAttributes().createDetachedAttribute(newClass, attribPtr->getType(), attribPtr->getOptions().options(), &attribPtr->getOptions());
    attribPromote(geo, newAttribUPtr.get(), attribPtr, subscribeRatio, minGrainSize);
#endif

    return newAttribUPtr;
}

SYS_FORCE_INLINE
static GA_AttributeUPtr
attribPromoteDetached(
    const GA_Attribute* const attribPtr,
    const GA_AttributeOwner newClass,
    const exint subscribeRatio = 16,
    const exint minGrainSize = 1024
)
{
    if (!attribPtr)
        return GA_AttributeUPtr();
#if 0
    GA_Attribute* const newAttribPtr = attribPtr->getDetail().getAttributes().cloneTempAttribute(newClass, *attribPtr, true);
    GA_AttributeUPtr newAttribUPtr(newAttribPtr);
    attribPromote(newAttribPtr, attribPtr, subscribeRatio, minGrainSize);
#else
    GA_AttributeUPtr newAttribUPtr = attribPtr->getDetail().getAttributes().createDetachedAttribute(newClass, attribPtr->getType(), attribPtr->getOptions().options(), &attribPtr->getOptions());
    attribPromote(newAttribUPtr.get(), attribPtr, subscribeRatio, minGrainSize);
#endif

    return newAttribUPtr;
}



static GA_Attribute*
    attribPromote(
        GA_Detail* const geo,
        const GA_Attribute* const attribPtr,
        const GA_AttributeOwner newClass,
        const exint subscribeRatio = 16,
        const exint minGrainSize = 1024
    )
{
    UT_ASSERT_P(geo);
    if (!attribPtr)
        return nullptr;

    GA_Attribute* const newAttribPtr = geo->getAttributes().cloneAttribute(newClass, attribPtr->getName(), *attribPtr, true);
    attribPromote(geo, newAttribPtr, attribPtr, subscribeRatio, minGrainSize);

    return newAttribPtr;
}




static GA_ConstAttributeUPtr
attribFindPromoteDetached(
    const GA_Detail* const geo,
    const GA_Attribute* const attribPtr,
    const GA_AttributeOwner newClass,
    const exint subscribeRatio = 16,
    const exint minGrainSize = 1024
)
{
    UT_ASSERT_P(geo);
    if (!attribPtr)
        return nullptr;

    if (attribPtr->getOwner() == newClass)
        return UT_UniquePtr<const GA_Attribute>(attribPtr);

    GA_AttributeUPtr newAttribUPtr = geo->getAttributes().createDetachedAttribute(newClass, attribPtr->getType(), attribPtr->getOptions().options(), &attribPtr->getOptions());
    attribPromote(geo, newAttribUPtr.get(), attribPtr, subscribeRatio, minGrainSize);

    return newAttribUPtr;
}

static const GA_Attribute*
attribFindPromote(
    GA_Detail* const geo,
    const GA_Attribute* const attribPtr,
    const GA_AttributeOwner newClass,
    const exint subscribeRatio = 16,
    const exint minGrainSize = 1024
)
{
    UT_ASSERT_P(geo);
    if (!attribPtr)
        return nullptr;

    if (attribPtr->getOwner() == newClass)
        return attribPtr;

    GA_Attribute* const newAttribPtr = geo->getAttributes().cloneAttribute(newClass, attribPtr->getName(), *attribPtr, true);
    attribPromote(geo, newAttribPtr, attribPtr, subscribeRatio, minGrainSize);

    return newAttribPtr;
}


static GA_AttributeUPtr
attribFindPromoteDetached(
    const GA_Detail* const geo,
    GA_Attribute* const attribPtr,
    const GA_AttributeOwner newClass,
    const exint subscribeRatio = 16,
    const exint minGrainSize = 1024
)
{
    UT_ASSERT_P(geo);
    if (!attribPtr)
        return nullptr;

    if (attribPtr->getOwner() == newClass)
        return UT_UniquePtr<GA_Attribute>(attribPtr);

    GA_AttributeUPtr newAttribUPtr = geo->getAttributes().createDetachedAttribute(newClass, attribPtr->getType(), attribPtr->getOptions().options(), &attribPtr->getOptions());
    attribPromote(geo, newAttribUPtr.get(), attribPtr, subscribeRatio, minGrainSize);

    return newAttribUPtr;
}

static GA_Attribute*
attribFindPromote(
    GA_Detail* const geo,
    GA_Attribute* const attribPtr,
    const GA_AttributeOwner newClass,
    const exint subscribeRatio = 16,
    const exint minGrainSize = 1024
)
{
    UT_ASSERT_P(geo);
    if (!attribPtr)
        return nullptr;

    if (attribPtr->getOwner() == newClass)
        return attribPtr;

    GA_Attribute* const newAttribPtr = geo->getAttributes().cloneAttribute(newClass, attribPtr->getName(), *attribPtr, true);
    attribPromote(geo, newAttribPtr, attribPtr, subscribeRatio, minGrainSize);

    return newAttribPtr;
}











} // End of namespace GFE_AttributePromote

#endif
