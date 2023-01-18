
#pragma once

#ifndef __GA_FeE_AttribPromote_h__
#define __GA_FeE_AttribPromote_h__

//#include "GA_FeE/GA_FeE_AttribPromote.h"

#include "GA/GA_Detail.h"





#include "GA_FeE/GA_FeE_TopologyReference.h"

//#include "GA_FeE/GA_FeE_Type.h"

namespace GA_FeE_AttribPromote {



static void
    attribPromote(
        const GA_Detail* const geo,
        GA_Attribute* const dstAttribPtr,
        const GA_Attribute* const srcAttribPtr,
        const exint subscribeRatio = 16,
        const exint minGrainSize = 1024
    )
{
    UT_ASSERT_P(geo);
    UT_ASSERT_P(dstAttribPtr);
    UT_ASSERT_P(srcAttribPtr);
    UT_ASSERT_P(dstAttribPtr != srcAttribPtr);

    const GA_AttributeOwner dstClass = dstAttribPtr->getOwner();
    const GA_AttributeOwner srcClass = srcAttribPtr->getOwner();

    //GA_RWHandleT<> newAttrib_h(newAttribPtr);
    //GA_ROHandleT<> attrib_h(attribPtr);
    const GA_SplittableRange geoSplittableRange(GA_Range(geo->getIndexMap(dstClass)));
    UTparallelFor(geoSplittableRange, [geo, dstAttribPtr, srcAttribPtr, srcClass, dstClass](const GA_SplittableRange& r)
    {
        GA_Offset start, end;
        for (GA_Iterator it(r); it.blockAdvance(start, end); )
        {
            for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
            {
                //dstAttribPtr->copy(elemoff, *srcAttribPtr, GA_FeE_TopologyReference::offsetPromote(geo, srcClass, dstClass, elemoff));
                dstAttribPtr->copy(elemoff, *srcAttribPtr, GA_FeE_TopologyReference::offsetPromote(geo, dstClass, srcClass, elemoff));
            }
        }
    }, subscribeRatio, minGrainSize);
}







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
        return nullptr;

    GA_AttributeUPtr newAttribUPtr = geo->getAttributes().createDetachedAttribute(newClass, attribPtr->getType(), attribPtr->getOptions().options(), &attribPtr->getOptions());
    attribPromote(geo, newAttribUPtr.get(), attribPtr, subscribeRatio, minGrainSize);

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











} // End of namespace GA_FeE_AttribPromote

#endif
