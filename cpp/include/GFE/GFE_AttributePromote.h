
#pragma once

#ifndef __GFE_AttributePromote_h__
#define __GFE_AttributePromote_h__

//#include "GFE/GFE_AttributePromote.h"

#include "GA/GA_Detail.h"


#include "GFE/GFE_GeoFilter.h"




//#include "GFE/GFE_Type.h"


class GFE_AttribPromote : public GFE_AttribFilter {

public:
    using GFE_AttribFilter::GFE_AttribFilter;

    void
        setComputeParm(
            const bool outTopoAttrib = false,
            const exint subscribeRatio = 64,
            const exint minGrainSize = 1024
        )
    {
        setHasComputed();

        this->outTopoAttrib = outTopoAttrib;
        this->subscribeRatio = subscribeRatio;
        this->minGrainSize = minGrainSize;
    }

    void
        setSourceAttribute(
            const GA_Attribute* const srcAttribPtr
        )
    {
        this->srcAttribPtr = srcAttribPtr;
    }

    void
        setSourceAttribute(
            GA_Attribute* const srcAttribPtr
        )
    {
        this->srcAttribPtr = srcAttribPtr;
        this->srcAttribPtr_nonConst = srcAttribPtr;
    }


    void
        setDestinationAttribute(
            GA_Attribute* const dstAttribPtr
        )
    {
        dstOwner = dstAttribPtr->getOwner();
        this->dstAttribPtr = dstAttribPtr;
        getOutAttribArray().set(dstAttribPtr);
    }

    void
        createDestinationAttribute(
            const GA_AttributeOwner owner
        )
    {
        UT_ASSERT_MSG(srcAttribPtr, "Can NOT with no srcAttribPtr");
        UT_ASSERT_MSG(geo, "Can NOT with no geo");

        if (srcAttribPtr->getOwner() == owner)
            return;

        if (srcAttribPtr->isDetached())
        {
            return;
        }
        else
        {
            dstAttribPtr = geo->findAttribute(owner, srcAttribPtr->getName());
            if (!dstAttribPtr)
                dstAttribPtr = geo->getAttributes().cloneAttribute(owner, srcAttribPtr->getName(), *srcAttribPtr, true);
        }
        dstOwner = owner;
    }

    void
        createDestinationAttribute(
            const GA_AttributeOwner owner,
            const UT_StringHolder& name
        )
    {
        UT_ASSERT_MSG(srcAttribPtr, "Can NOT with no srcAttribPtr");
        UT_ASSERT_MSG(geo, "Can NOT with no geo");

        if (srcAttribPtr->getOwner() == owner)
            return;

        if (srcAttribPtr->isDetached())
        {
            dstAttribPtr = geo->getAttributes().cloneAttribute(owner, name, *srcAttribPtr, true);
        }
        else
        {
            dstAttribPtr = geo->findAttribute(owner, srcAttribPtr->getName());
            if (!dstAttribPtr)
                dstAttribPtr = geo->getAttributes().cloneAttribute(owner, name, *srcAttribPtr, true);
        }
        dstOwner = owner;
    }


    void
        createDetachedDestinationAttribute(
            const GA_AttributeOwner owner
        )
    {
        UT_ASSERT_MSG(srcAttribPtr, "Can NOT with no srcAttribPtr");
        UT_ASSERT_MSG(geo,          "Can NOT with no geo");
        
        if (srcAttribPtr->getOwner() == owner)
            return;
        
        dstAttribPtr = getOutAttribArray().createDetachedAttribute(
            owner, GA_STORECLASS_FLOAT, GA_STORE_INVALID, false,
            srcAttribPtr->getType(), srcAttribPtr->getOptions().options(), &srcAttribPtr->getOptions());

        dstOwner = owner;
    }

    SYS_FORCE_INLINE
    GA_Attribute*
        getDestinationAttribute() const
    {
        return dstAttribPtr;
    }



    



    


private:


    virtual bool
        computeCore() override
    {
        UT_ASSERT_P(dstAttribPtr);
        UT_ASSERT_P(srcAttribPtr);

        if (getOutAttribArray().isEmpty())
            return true;

        if (!srcAttribPtr || !dstAttribPtr)
            return false;

        if (dstAttribPtr == srcAttribPtr)
            return false;

        if (!geo)
            setDetail(*dstAttribPtr);

        UT_ASSERT_P(geo == &dstAttribPtr->getDetail());
        UT_ASSERT_P(geo == &srcAttribPtr->getDetail());

        if (groupParser.isEmpty())
            return true;

        attribPromote();

        return true;
    }


    void
        attribPromote()
    {
        const GA_Attribute& srcAttrib = *srcAttribPtr;
        srcOwner = srcAttribPtr->getOwner();

        const GA_SplittableRange geoSplittableRange(GA_Range(dstAttribPtr->getIndexMap()));
        UTparallelFor(geoSplittableRange, [this, &srcAttrib](const GA_SplittableRange& r)
        {
            GA_Offset start, end;
            for (GA_Iterator it(r); it.blockAdvance(start, end); )
            {
                for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                {
                    //dstAttribPtr->copy(elemoff, *srcAttribPtr, GFE_TopologyReference_Namespace::offsetPromote(geo, srcOwner, dstOwner, elemoff));
                    dstAttribPtr->copy(elemoff, srcAttrib, offsetPromote(elemoff));
                }
            }
        }, subscribeRatio, minGrainSize);
    }


    GA_Offset
        offsetPromote(
            const GA_Offset elemoff
        )
    {
        //const GA_Topology& topo = geo->getTopology();
        switch (srcOwner)
        {
        case GA_ATTRIB_PRIMITIVE:
            switch (dstOwner)
            {
            case GA_ATTRIB_PRIMITIVE:
                return elemoff;
                break;
            case GA_ATTRIB_POINT:
                return geo->vertexPoint(geo->getPrimitiveVertexOffset(elemoff, 0));
                break;
            case GA_ATTRIB_VERTEX:
                return geo->getPrimitiveVertexOffset(elemoff, 0);
                break;
            default:
                UT_ASSERT_MSG(0, "Unhandled newClass!");
                break;
            }
            break;
        case GA_ATTRIB_POINT:
            switch (dstOwner)
            {
            case GA_ATTRIB_PRIMITIVE:
                return geo->vertexPrimitive(geo->pointVertex(elemoff));
                break;
            case GA_ATTRIB_POINT:
                return elemoff;
                break;
            case GA_ATTRIB_VERTEX:
                return geo->pointVertex(elemoff);
                break;
            default:
                UT_ASSERT_MSG(0, "Unhandled newClass!");
                break;
            }
            break;
        case GA_ATTRIB_VERTEX:
            switch (dstOwner)
            {
            case GA_ATTRIB_PRIMITIVE:
                return geo->vertexPrimitive(elemoff);
                break;
            case GA_ATTRIB_POINT:
                return geo->vertexPoint(elemoff);
                break;
            case GA_ATTRIB_VERTEX:
                return elemoff;
                break;
            default:
                UT_ASSERT_MSG(0, "Unhandled newClass!");
                break;
            }
            break;
        default:
            UT_ASSERT_MSG(0, "Unhandled origClass!");
            break;
        }
        return -1;
    }




public:
    GA_Attribute* dstAttribPtr;

private:
    const GA_Attribute* srcAttribPtr;
    GA_Attribute* srcAttribPtr_nonConst;

    GA_AttributeOwner srcOwner;
    GA_AttributeOwner dstOwner;

    exint subscribeRatio = 64;
    exint minGrainSize = 1024;

}; // End of class GFE_AttribPromote























/*


namespace GFE_AttributePromote {


static void
    attribPromote(
        const GA_Detail& geo,
        GA_Attribute& dstAttrib,
        const GA_Attribute& srcAttrib,
        const exint subscribeRatio = 16,
        const exint minGrainSize = 1024
    )
{
    UT_ASSERT_P(&dstAttrib != &srcAttrib);

    UT_ASSERT_P(&geo == &dstAttrib.getDetail());
    UT_ASSERT_P(&geo == &srcAttrib.getDetail());

    const GA_AttributeOwner dstOwner = dstAttrib.getOwner();
    const GA_AttributeOwner srcOwner = srcAttrib.getOwner();

    //GA_RWHandleT<> newAttrib_h(newAttribPtr);
    //GA_ROHandleT<> attrib_h(attribPtr);
    const GA_SplittableRange geoSplittableRange(GA_Range(dstAttrib.getIndexMap()));
    UTparallelFor(geoSplittableRange, [&geo, &dstAttrib, &srcAttrib, srcOwner, dstOwner](const GA_SplittableRange& r)
    {
        GA_Offset start, end;
        for (GA_Iterator it(r); it.blockAdvance(start, end); )
        {
            for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
            {
                //dstAttribPtr->copy(elemoff, *srcAttribPtr, GFE_TopologyReference_Namespace::offsetPromote(geo, srcOwner, dstOwner, elemoff));
                dstAttrib.copy(elemoff, srcAttrib, GFE_TopologyReference_Namespace::offsetPromote(geo, dstOwner, srcOwner, elemoff));
            }
        }
    }, subscribeRatio, minGrainSize);
}


SYS_FORCE_INLINE
static void
    attribPromote(
        GA_Attribute& dstAttribPtr,
        const GA_Attribute& srcAttribPtr,
        const exint subscribeRatio = 16,
        const exint minGrainSize = 1024
    )
{
    attribPromote(srcAttribPtr.getDetail(), dstAttribPtr, srcAttribPtr, subscribeRatio, minGrainSize);
}







SYS_FORCE_INLINE
static GA_AttributeUPtr
attribPromoteDetached(
    const GA_Detail& geo,
    const GA_Attribute* const attribPtr,
    const GA_AttributeOwner newClass,
    const exint subscribeRatio = 16,
    const exint minGrainSize = 1024
)
{
    if (!attribPtr)
        return GA_AttributeUPtr();
#if 0
    GA_Attribute* const newAttribPtr = geo->getAttributes().cloneTempAttribute(newClass, *attribPtr, true);
    GA_AttributeUPtr newAttribUPtr(newAttribPtr);
    attribPromote(newAttribPtr, attribPtr, subscribeRatio, minGrainSize);
#else
    GA_AttributeUPtr newAttribUPtr = geo.getAttributes().createDetachedAttribute(newClass, attribPtr->getType(), attribPtr->getOptions().options(), &attribPtr->getOptions());
    attribPromote(geo, *newAttribUPtr.get(), *attribPtr, subscribeRatio, minGrainSize);
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
    attribPromote(*newAttribUPtr.get(), *attribPtr, subscribeRatio, minGrainSize);
#endif

    return newAttribUPtr;
}



static GA_Attribute*
    attribPromote(
        GA_Detail& geo,
        const GA_Attribute* const attribPtr,
        const GA_AttributeOwner newClass,
        const exint subscribeRatio = 16,
        const exint minGrainSize = 1024
    )
{
    if (!attribPtr)
        return nullptr;

    GA_Attribute* const newAttribPtr = geo.getAttributes().cloneAttribute(newClass, attribPtr->getName(), *attribPtr, true);
    attribPromote(geo, *newAttribPtr, *attribPtr, subscribeRatio, minGrainSize);

    return newAttribPtr;
}




static GA_ConstAttributeUPtr
attribFindPromoteDetached(
    const GA_Detail& geo,
    const GA_Attribute* const attribPtr,
    const GA_AttributeOwner newClass,
    const exint subscribeRatio = 16,
    const exint minGrainSize = 1024
)
{
    if (!attribPtr)
        return nullptr;

    if (attribPtr->getOwner() == newClass)
        return UT_UniquePtr<const GA_Attribute>(attribPtr);

    GA_AttributeUPtr newAttribUPtr = geo.getAttributes().createDetachedAttribute(newClass, attribPtr->getType(), attribPtr->getOptions().options(), &attribPtr->getOptions());
    attribPromote(geo, *newAttribUPtr.get(), *attribPtr, subscribeRatio, minGrainSize);

    return newAttribUPtr;
}

static const GA_Attribute*
attribFindPromote(
    GA_Detail& geo,
    const GA_Attribute* const attribPtr,
    const GA_AttributeOwner newClass,
    const exint subscribeRatio = 16,
    const exint minGrainSize = 1024
)
{
    if (!attribPtr)
        return nullptr;

    if (attribPtr->getOwner() == newClass)
        return attribPtr;

    GA_Attribute* const newAttribPtr = geo.getAttributes().cloneAttribute(newClass, attribPtr->getName(), *attribPtr, true);
    attribPromote(geo, *newAttribPtr, *attribPtr, subscribeRatio, minGrainSize);

    return newAttribPtr;
}


static GA_AttributeUPtr
attribFindPromoteDetached(
    const GA_Detail& geo,
    GA_Attribute* const attribPtr,
    const GA_AttributeOwner newClass,
    const exint subscribeRatio = 16,
    const exint minGrainSize = 1024
)
{
    if (!attribPtr)
        return nullptr;

    if (attribPtr->getOwner() == newClass)
        return UT_UniquePtr<GA_Attribute>(attribPtr);

    GA_AttributeUPtr newAttribUPtr = geo.getAttributes().createDetachedAttribute(newClass, attribPtr->getType(), attribPtr->getOptions().options(), &attribPtr->getOptions());
    attribPromote(geo, *newAttribUPtr.get(), *attribPtr, subscribeRatio, minGrainSize);

    return newAttribUPtr;
}

static GA_Attribute*
attribFindPromote(
    GA_Detail& geo,
    GA_Attribute* const attribPtr,
    const GA_AttributeOwner newClass,
    const exint subscribeRatio = 16,
    const exint minGrainSize = 1024
)
{
    if (!attribPtr)
        return nullptr;

    if (attribPtr->getOwner() == newClass)
        return attribPtr;

    GA_Attribute* const newAttribPtr = geo.getAttributes().cloneAttribute(newClass, attribPtr->getName(), *attribPtr, true);
    attribPromote(geo, *newAttribPtr, *attribPtr, subscribeRatio, minGrainSize);

    return newAttribPtr;
}











} // End of namespace GFE_AttributePromote





*/






#endif
