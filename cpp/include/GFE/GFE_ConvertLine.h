
#pragma once

#ifndef __GFE_ConvertLine_h__
#define __GFE_ConvertLine_h__

//#include "GFE/GFE_ConvertLine.h"

#include "GFE/GFE_GeoFilter.h"

#include "GFE/GFE_VertexNextEquiv.h"





class GFE_ConvertLine : public GFE_AttribFilter {

public:
    GFE_ConvertLine(
        GA_Detail& geo,
        const GA_Detail* const geoOrigin = nullptr,
        const SOP_NodeVerb::CookParms* const cookparms = nullptr
    )
        : GFE_AttribFilter(geo, cookparms)
    {
        geoOriginTmp = new GU_Detail();
        geoOrigin_h.allocateAndSet(geoOriginTmp);

        if (geoOrigin)
        {
            hasInGeo = true;
            geo.replaceWith(*geoOrigin);
            this->geoOrigin = geoOrigin;
        }
        else
        {
            hasInGeo = false;
            this->geoOrigin = geoOriginTmp;
        }
        geoOriginTmp->replaceWith(*geo);
    }

    ~GFE_ConvertLine()
    {
    }


    void
        setGroup(
            const GA_PrimitiveGroup* const geoPrimGroup = nullptr
        )
    {
        groupParser.setGroup(geoPrimGroup);
    }

    void
        setGroup(
            const UT_StringHolder& primGroupName = ""
        )
    {
        groupParser.setPrimitiveGroup(primGroupName);
    }


    void
        createSrcPrimAttrib(
            const bool detached = false,
            const GA_Storage storage = GA_STORE_INVALID,
            const UT_StringHolder& srcPrimAttribName = "srcPrims"
        )
    {
        srcPrimAttribDetached = detached;
        srcPrimAttribStorage = storage;
        outSrcPrimAttrib = true;
        this->srcPrimAttribName = srcPrimAttribName;
    }

    void
        setComputeParm(
            const bool isClosed = false,
            const bool keepSourcePrim = false
        )
    {
        setHasComputed();
        primoff_first = -1;
        this->isClosed = isClosed;
        this->keepSourcePrim = keepSourcePrim;
    }

    void
        setCopyPrimitiveAttrib(
            const bool copyAttrib = false,
            const UT_StringHolder& copyAttribName = "*"
        )
    {
        copyPrimAttrib = copyAttrib;
        copyPrimAttribName = copyAttribName;
    }

    void
        setCopyVertexAttrib(
            const bool copyAttrib = false,
            const UT_StringHolder& copyAttribName = "*"
        )
    {
        copyVertexAttrib = copyAttrib;
        copyVertexAttribName = copyAttribName;
    }


    virtual void
        bumpDataIdsForAddOrRemove() const override
    {
        geo->bumpDataIdsForAddOrRemove(false, true, true);
    }

    GA_Offset
        get_primoff_first() const
    {
        return primoff_first;
    }

    


private:

    // can not use in parallel unless for each GA_Detail
    virtual bool
        computeCore() override
    {
        if (groupParser.isEmpty())
            return true;

        if (!keepSourcePrim)
        {
            geo->replaceWithPoints(*geoOrigin);
        }
        //if (hasInGeo)
        //{
        //}
        //else
        //{
        //}
        GA_AttributeSet& geoAttribSet = geo->getAttributes();
        GA_Attribute* attribPtr = nullptr;
        std::vector<GA_Attribute*> copyPrimAttribArray;
        std::vector<GA_Attribute*> copyVertexAttribArray;
        copyPrimAttribArray.reserve(16);
        copyVertexAttribArray.reserve(16);

        const GA_AttributeFilter allAttribFilter = GA_AttributeFilter::selectPublic();

        if (copyPrimAttrib)
        {
            for (GA_AttributeDict::iterator it = geoAttribSet.begin(GA_ATTRIB_PRIMITIVE); !it.atEnd(); ++it)
            {
                attribPtr = it.attrib();
                //if (attribPtr->getScope() != GA_SCOPE_PUBLIC)
                //    continue;
                if (attribPtr->getName().multiMatch(copyPrimAttribName))
                    copyPrimAttribArray.emplace_back(attribPtr);
                else
                    geoAttribSet.destroyAttribute(attribPtr);
            }
        }
        else
        {
            geoAttribSet.destroyAttributes(GA_ATTRIB_PRIMITIVE, allAttribFilter);
        }

        if (copyVertexAttrib)
        {
            for (GA_AttributeDict::iterator it = geoOriginTmp->getAttributes().begin(GA_ATTRIB_VERTEX); !it.atEnd(); ++it)
            {
                attribPtr = it.attrib();

                if (attribPtr->getScope() != GA_SCOPE_PUBLIC)
                    continue;
                //const char* name = srcAttribPtr->getName().c_str();
                if (attribPtr->getName().multiMatch(copyVertexAttribName))
                    copyVertexAttribArray.emplace_back(attribPtr);
            }
        }
        geoAttribSet.destroyAttributes(GA_ATTRIB_VERTEX, allAttribFilter);


        //const bool hasInputGroup = !groupParser.isEmpty();


        const GA_VertexGroup* const creatingGroup = GFE_VertexNextEquiv::addGroupVertexNextEquivNoLoop(geoOriginTmp, groupParser.getVertexGroup());
        //const GA_Attribute* dstptAttrib = geoPoint->findVertexAttribute("__topo_dstpt");
        const GA_RWHandleT<GA_Offset> dstpt_h = GFE_TopologyReference::addAttribVertexPointDst(geoOriginTmp, groupParser.getVertexGroup());
        //const GA_RWHandleT<GA_Offset> dstpt_h = geoOriginTmp->findVertexAttribute(GFE_TOPO_SCOPE, "__topo_dstpt");

        UT_ASSERT_P(dstpt_h.getAttribute());


        const GA_Size entries = creatingGroup->getGroupEntries();

        GA_Offset vtxoff_first;
        primoff_first = geo->appendPrimitivesAndVertices(GA_PrimitiveTypeId(1), entries, 2, vtxoff_first, isClosed);
        //geo->appendPrimitivesAndVertices(GA_PrimitiveTypeId(1), entries, 2, vtxoff_first, isClosed);

        const bool createSrcPrimAttrib = copyPrimAttrib || outSrcPrimAttrib;

        GA_Attribute* srcPrimAttrib = nullptr;
        GA_Attribute* srcVtxAttrib = nullptr;
        if (createSrcPrimAttrib)
            srcPrimAttrib = getOutAttribArray().
            findOrCreateTuple(srcPrimAttribDetached || !outSrcPrimAttrib,
                GA_ATTRIB_PRIMITIVE, GA_STORECLASS_INT,
                srcPrimAttribStorage, srcPrimAttribName);

        if (copyVertexAttrib)
            srcVtxAttrib = getOutAttribArray().findOrCreateTuple(true, GA_ATTRIB_PRIMITIVE, GA_STORECLASS_INT);

        GA_RWHandleT<GA_Offset> srcPrim_h(srcPrimAttrib);
        GA_RWHandleT<GA_Offset> srcVtx_h(srcVtxAttrib);



        GA_Topology& topo = geo->getTopology();
        const GA_ATITopology* const vtxPrimRef = topo.getPrimitiveRef();


        const GA_Topology& topo_tmpGeo0 = geoOriginTmp->getTopology();
        //topo_tmpGeo0.makePrimitiveRef();
        const GA_ATITopology* const vtxPointRef_geoTmp = topo_tmpGeo0.getPointRef();
        const GA_ATITopology* const vtxPrimRef_geoTmp = topo_tmpGeo0.getPrimitiveRef();

        GA_Offset start, end;
        for (GA_Iterator it(geoOriginTmp->getVertexRange(creatingGroup)); it.fullBlockAdvance(start, end); )
        {
            for (GA_Offset vtxoff = start; vtxoff < end; ++vtxoff)
            {
                if (createSrcPrimAttrib)
                {
                    srcPrim_h.set(vtxPrimRef->getLink(vtxoff_first), vtxPrimRef_geoTmp->getLink(vtxoff));
                }
                if (copyVertexAttrib)
                {
                    srcVtx_h.set(vtxPrimRef->getLink(vtxoff_first), vtxoff);
                }
                topo.wireVertexPoint(vtxoff_first, vtxPointRef_geoTmp->getLink(vtxoff));
                ++vtxoff_first;
                topo.wireVertexPoint(vtxoff_first, dstpt_h.get(vtxoff));
                ++vtxoff_first;
            }
        }

        //////////////////// copy attributes //////////////////////

        if (copyPrimAttrib || copyVertexAttrib)
        {
            const GA_Offset primoff_last = primoff_first + entries;

            if (copyPrimAttrib)
            {
                const size_t size = copyPrimAttribArray.size();
                for (size_t i = 0; i < size; i++)
                {
                    attribPtr = copyPrimAttribArray[i];
                    attribPtr->bumpDataId();

                    if (keepSourcePrim)
                    {
                        for (GA_Offset primoff = primoff_first; primoff < primoff_last; ++primoff)
                        {
                            //const GA_Offset srcPrimVal = srcPrim_h.get(primoff);
                            attribPtr->copy(primoff, srcPrim_h.get(primoff));
                        }
                    }
                    else
                    {
                        GA_Attribute& srcAttrib = *geoOriginTmp->findPrimitiveAttribute(attribPtr->getName());
                        for (GA_Offset primoff = primoff_first; primoff < primoff_last; ++primoff)
                        {
                            //const GA_Offset srcPrimVal = srcPrim_h.get(primoff);
                            attribPtr->copy(primoff, srcAttrib, srcPrim_h.get(primoff));
                        }
                    }
                }
            }

            GA_Attribute* srcAttribPtr = nullptr;
            if (copyVertexAttrib)
            {
                const size_t size = copyVertexAttribArray.size();
                for (size_t i = 0; i < size; i++)
                {
                    srcAttribPtr = copyVertexAttribArray[i];

                    attribPtr = geo->findPrimitiveAttribute(srcAttribPtr->getName());
                    if (attribPtr)
                        continue;

                    GA_Attribute& srcAttrib = *srcAttribPtr;
                    attribPtr = geoAttribSet.cloneAttribute(GA_ATTRIB_PRIMITIVE, srcAttribPtr->getName(), srcAttrib, true);
                    for (GA_Offset primoff = primoff_first; primoff < primoff_last; ++primoff)
                    {
                        //const GA_Offset srcVtxVal = srcVtx_h.get(primoff);
                        attribPtr->copy(primoff, srcAttrib, srcVtx_h.get(primoff));
                    }
                }
            }
        }


        //geoTmp_h.deleteGdp();

        return true;
    }



public:
    bool isClosed = false;
    bool copyPrimAttrib = false;
    bool copyVertexAttrib = false;
    bool keepSourcePrim = false;
    UT_StringHolder copyPrimAttribName;
    UT_StringHolder copyVertexAttribName;
    
protected:
    bool hasInGeo;

private:
    
    bool outSrcPrimAttrib = false;
    bool srcPrimAttribDetached = false;
    UT_StringHolder srcPrimAttribName;
    GA_Storage srcPrimAttribStorage = GA_STORE_INVALID;

    GA_Offset primoff_first = -1;

    GU_DetailHandle geoOrigin_h;
    const GA_Detail* geoOrigin;
    GU_Detail* geoOriginTmp;
}; // End of class GFE_ConvertLine


















































namespace GFE_ConvertLine_Namespace {


#define TMP_GFE_ConvertLine_UseDetached_SrcPrimAttrib 1

#if !TMP_GFE_ConvertLine_UseDetached_SrcPrimAttrib
#define TMP_GFE_ConvertLine_SrcPrimAttribName "GFE_ConvertLine_srcPrim"
#endif


static GA_Offset
convertLine(
    GA_Detail* const geoPoint,
    const GA_Detail* const geoFull,
    const bool isClosed = false,
    const bool copyPrimAttrib = false,
    bool outSrcPrimAttrib = false,
    const UT_StringHolder& srcPrimAttribName = "",
    const bool keepSourcePrim = false,
    const UT_StringHolder& primGroupName = "",
    const UT_StringHolder& pointGroupName = "",
    const UT_StringHolder& vertexGroupName = "",
    const UT_StringHolder& edgeGroupName = "",
    const GA_Storage storage = GA_STORE_INVALID
)
{
    UT_ASSERT_P(geoPoint);
    UT_ASSERT_P(geoFull);

    if (keepSourcePrim)
    {
        geoPoint->replaceWith(*geoFull);
    }
    else
    {
        geoPoint->replaceWithPoints(*geoFull);
    }

    GU_DetailHandle geoTmp_h;
    GU_Detail* const geoTmp = new GU_Detail();
    geoTmp_h.allocateAndSet(geoTmp);
    geoTmp->replaceWith(*geoFull);


    const bool hasInputGroup = primGroupName.isstring() || pointGroupName.isstring() || vertexGroupName.isstring() || edgeGroupName.isstring();

    GA_VertexGroupUPtr geo0vtxGroupUPtr;
    GA_VertexGroup* geoVtxGroup = nullptr;
    if (hasInputGroup)
    {
        geo0vtxGroupUPtr = geoTmp->createDetachedVertexGroup();
        geoVtxGroup = geo0vtxGroupUPtr.get();
        if (primGroupName.isstring())
        {

        }

        if (pointGroupName.isstring())
        {

        }

        if (vertexGroupName.isstring())
        {

        }

        if (edgeGroupName.isstring())
        {

        }
    }

    const GA_Storage finalStorage = storage == GA_STORE_INVALID ? GFE_Type::getPreferredStorageI(geoPoint) : storage;
    //const GA_VertexGroup* creatingGroup = GFE_VertexNextEquiv::addGroupVertexNextEquivNoLoop(geoTmp, "__topo_nextEquivValid", geoVtxGroup, subscribeRatio, minGrainSize);
    const GA_VertexGroup* const creatingGroup = GFE_VertexNextEquiv::addGroupVertexNextEquivNoLoop(geoTmp, geoVtxGroup, finalStorage);
    //const GA_Attribute* dstptAttrib = geoPoint->findVertexAttribute("__topo_dstpt");
    const GA_RWHandleT<GA_Offset> dstpt_h = geoTmp->findVertexAttribute(GFE_TOPO_SCOPE, "__topo_dstpt");

    UT_ASSERT_P(dstpt_h.getAttribute());


    GA_Size entries = creatingGroup->getGroupEntries();

    GA_Offset vtxoff_first;
    GA_Offset primoff_first = geoPoint->appendPrimitivesAndVertices(GA_PrimitiveTypeId(1), entries, 2, vtxoff_first, isClosed);
    //geoPoint->appendPrimitivesAndVertices(GA_PrimitiveTypeId(1), entries, 2, vtxoff_first, isClosed);



#if TMP_GFE_ConvertLine_UseDetached_SrcPrimAttrib
    GA_AttributeUPtr srcPrimAttribUPtr;
#endif
    GA_Attribute* srcPrimAttrib = nullptr;
    if (copyPrimAttrib || outSrcPrimAttrib)
    {
        if (outSrcPrimAttrib)
        {
            if (srcPrimAttribName.isstring() && !srcPrimAttribName.isEmpty())
            {
                srcPrimAttrib = geoPoint->getAttributes().createTupleAttribute(GA_ATTRIB_PRIMITIVE, srcPrimAttribName, finalStorage, 1, GA_Defaults(-1));
            }
            else
            {
                if (copyPrimAttrib)
                {
#if TMP_GFE_ConvertLine_UseDetached_SrcPrimAttrib
                    srcPrimAttribUPtr = geoPoint->getAttributes().createDetachedTupleAttribute(GA_ATTRIB_PRIMITIVE, finalStorage, 1, GA_Defaults(0));
                    srcPrimAttrib = srcPrimAttribUPtr.get();
#else
                    srcPrimAttrib = geoPoint->getAttributes().createTupleAttribute(GA_ATTRIB_PRIMITIVE, TMP_GFE_ConvertLine_SrcPrimAttribName, finalStorage, 1, GA_Defaults(-1));
                    //srcPrimAttribUPtr.reset(srcPrimAttrib);
#endif
                }
                outSrcPrimAttrib = false;
                UT_ASSERT_MSG(0, "srcPrimAttribName invliad");
            }
        }
        else
        {
#if TMP_GFE_ConvertLine_UseDetached_SrcPrimAttrib
            srcPrimAttribUPtr = geoPoint->getAttributes().createDetachedTupleAttribute(GA_ATTRIB_PRIMITIVE, finalStorage, 1, GA_Defaults(0));
            srcPrimAttrib = srcPrimAttribUPtr.get();
#else
            srcPrimAttrib = geoPoint->getAttributes().createTupleAttribute(GA_ATTRIB_PRIMITIVE, TMP_GFE_ConvertLine_SrcPrimAttribName, finalStorage, 1, GA_Defaults(-1));
            //srcPrimAttribUPtr.reset(srcPrimAttrib);
#endif
        }
    }
    //if (srcPrimAttrib)
    //{
    //    exint indexSize = srcPrimAttrib->getIndexMap().indexSize();
    //    exint offsetSize = srcPrimAttrib->getIndexMap().offsetSize();
    //}
    GA_RWHandleT<GA_Offset> srcPrim_h(srcPrimAttrib);



    GA_Topology& topo = geoPoint->getTopology();
    const GA_ATITopology* const vtxPrimRef = topo.getPrimitiveRef();


    const GA_Topology& topo_tmpGeo0 = geoTmp->getTopology();
    //topo_tmpGeo0.makePrimitiveRef();
    const GA_ATITopology* const vtxPointRef_geoTmp = topo_tmpGeo0.getPointRef();
    const GA_ATITopology* const vtxPrimRef_geoTmp = topo_tmpGeo0.getPrimitiveRef();

    GA_Offset start, end;
    for (GA_Iterator it(geoTmp->getVertexRange(creatingGroup)); it.fullBlockAdvance(start, end); )
    {
        for (GA_Offset vtxoff = start; vtxoff < end; ++vtxoff)
        {
            if (copyPrimAttrib || outSrcPrimAttrib)
            {
                srcPrim_h.set(vtxPrimRef->getLink(vtxoff_first), vtxPrimRef_geoTmp->getLink(vtxoff));
            }
            topo.wireVertexPoint(vtxoff_first, vtxPointRef_geoTmp->getLink(vtxoff));
            ++vtxoff_first;
            topo.wireVertexPoint(vtxoff_first, dstpt_h.get(vtxoff));
            ++vtxoff_first;
        }
    }

    //////////////////// copy attributes //////////////////////

    if (copyPrimAttrib)
    {
#if 1
        GA_Offset primoff_last = primoff_first + entries;

        for (GA_AttributeDict::iterator it = geoPoint->getAttributes().begin(GA_ATTRIB_PRIMITIVE); !it.atEnd(); ++it)
        {
            GA_Attribute* const attribPtr = it.attrib();
            if (attribPtr == srcPrimAttrib)
                continue;
            attribPtr->bumpDataId();
            const UT_StringHolder& name = attribPtr->getName();
            //if (attribPtr->isDetached())
            //    continue;
            if (keepSourcePrim)
            {
                for (GA_Offset primoff = primoff_first; primoff < primoff_last; ++primoff)
                {
                    const GA_Offset srcPrimVal = srcPrim_h.get(primoff);
                    attribPtr->copy(primoff, srcPrim_h.get(primoff));
                }
            }
            else
            {
                GA_Attribute* const srcAttribPtr = geoTmp->findPrimitiveAttribute(it.name());
                for (GA_Offset primoff = primoff_first; primoff < primoff_last; ++primoff)
                {
                    const GA_Offset srcPrimVal = srcPrim_h.get(primoff);
                    attribPtr->copy(primoff, *srcAttribPtr, srcPrim_h.get(primoff));
                }
            }
        }
#else
        GA_Offset start, end;
        for (GA_Iterator it(GA_Range(geoPoint->getPrimitiveMap(), primoff_first, geoPoint->getNumPrimitiveOffsets())); it.fullBlockAdvance(start, end); )
        {
            for (GA_Offset primoff = start; primoff < end; ++primoff)
            {
            }
        }
#endif

#if !TMP_GFE_ConvertLine_UseDetached_SrcPrimAttrib
        if (!outSrcPrimAttrib)
        {
            geoPoint->getAttributes().destroyAttribute(srcPrimAttrib);
        }
#endif
    }



    //geoTmp_h.deleteGdp();

    return primoff_first;
}


#if 1



SYS_FORCE_INLINE
static GA_Offset
convertLine(
    GA_Detail* const geoPoint,
    const bool isClosed = false,
    const bool copyPrimAttrib = false,
    bool outSrcPrimAttrib = false,
    const UT_StringHolder& srcPrimAttribName = "",
    const bool keepSourcePrim = false,
    const UT_StringHolder& primGroupName = "",
    const UT_StringHolder& pointGroupName = "",
    const UT_StringHolder& vertexGroupName = "",
    const UT_StringHolder& edgeGroupName = "",
    const GA_Storage storage = GA_STORE_INVALID
)
{
    UT_ASSERT_P(geoPoint);

    GU_DetailHandle geoFullTmp_h;
    GU_Detail* const geoFullTmp = new GU_Detail();
    geoFullTmp_h.allocateAndSet(geoFullTmp);
    geoFullTmp->replaceWith(*geoPoint);

#if 1
    if (!keepSourcePrim)
    {
        geoPoint->replaceWithPoints(*geoFullTmp);
    }
#else
    if (keepSourcePrim)
    {
        geoPoint->replaceWith(*geoFullTmp);
    }
    else
    {
        geoPoint->replaceWithPoints(*geoFullTmp);
    }
#endif


    const bool hasInputGroup = primGroupName.isstring() || pointGroupName.isstring() || vertexGroupName.isstring() || edgeGroupName.isstring();

    GA_VertexGroupUPtr geo0vtxGroupUPtr;
    GA_VertexGroup* geoVtxGroup = nullptr;
    if (hasInputGroup)
    {
        geo0vtxGroupUPtr = geoFullTmp->createDetachedVertexGroup();
        geoVtxGroup = geo0vtxGroupUPtr.get();
        if (primGroupName.isstring())
        {

        }

        if (pointGroupName.isstring())
        {

        }

        if (vertexGroupName.isstring())
        {

        }

        if (edgeGroupName.isstring())
        {

        }
}

    const GA_Storage finalStorage = storage == GA_STORE_INVALID ? GFE_Type::getPreferredStorageI(geoPoint) : storage;
    //const GA_VertexGroup* creatingGroup = GFE_VertexNextEquiv::addGroupVertexNextEquivNoLoop(geoFullTmp, "__topo_nextEquivValid", geoVtxGroup, subscribeRatio, minGrainSize);
    const GA_VertexGroup* const creatingGroup = GFE_VertexNextEquiv::addGroupVertexNextEquivNoLoop(geoFullTmp, geoVtxGroup, finalStorage);
    //const GA_Attribute* dstptAttrib = geoPoint->findVertexAttribute("__topo_dstpt");
    const GA_RWHandleT<GA_Offset> dstpt_h = geoFullTmp->findVertexAttribute(GFE_TOPO_SCOPE, "__topo_dstpt");

    UT_ASSERT_P(dstpt_h.getAttribute());


    GA_Size entries = creatingGroup->getGroupEntries();

    GA_Offset vtxoff_first;
    GA_Offset primoff_first = geoPoint->appendPrimitivesAndVertices(GA_PrimitiveTypeId(1), entries, 2, vtxoff_first, isClosed);
    //geoPoint->appendPrimitivesAndVertices(GA_PrimitiveTypeId(1), entries, 2, vtxoff_first, isClosed);



#if TMP_GFE_ConvertLine_UseDetached_SrcPrimAttrib
    GA_AttributeUPtr srcPrimAttribUPtr;
#endif
    GA_Attribute* srcPrimAttrib = nullptr;
    if (copyPrimAttrib || outSrcPrimAttrib)
    {
        if (outSrcPrimAttrib)
        {
            if (srcPrimAttribName.isstring() && !srcPrimAttribName.isEmpty())
            {
                srcPrimAttrib = geoPoint->getAttributes().createTupleAttribute(GA_ATTRIB_PRIMITIVE, srcPrimAttribName, finalStorage, 1, GA_Defaults(-1));
            }
            else
            {
                if (copyPrimAttrib)
                {
#if TMP_GFE_ConvertLine_UseDetached_SrcPrimAttrib
                    srcPrimAttribUPtr = geoPoint->getAttributes().createDetachedTupleAttribute(GA_ATTRIB_PRIMITIVE, finalStorage, 1, GA_Defaults(0));
                    srcPrimAttrib = srcPrimAttribUPtr.get();
#else
                    srcPrimAttrib = geoPoint->getAttributes().createTupleAttribute(GA_ATTRIB_PRIMITIVE, TMP_GFE_ConvertLine_SrcPrimAttribName, finalStorage, 1, GA_Defaults(-1));
                    //srcPrimAttribUPtr.reset(srcPrimAttrib);
#endif
                }
                outSrcPrimAttrib = false;
                UT_ASSERT_MSG(0, "srcPrimAttribName invliad");
            }
        }
        else
        {
#if TMP_GFE_ConvertLine_UseDetached_SrcPrimAttrib
            srcPrimAttribUPtr = geoPoint->getAttributes().createDetachedTupleAttribute(GA_ATTRIB_PRIMITIVE, finalStorage, 1, GA_Defaults(0));
            srcPrimAttrib = srcPrimAttribUPtr.get();
#else
            srcPrimAttrib = geoPoint->getAttributes().createTupleAttribute(GA_ATTRIB_PRIMITIVE, TMP_GFE_ConvertLine_SrcPrimAttribName, finalStorage, 1, GA_Defaults(-1));
            //srcPrimAttribUPtr.reset(srcPrimAttrib);
#endif
        }
    }
    //if (srcPrimAttrib)
    //{
    //    exint indexSize = srcPrimAttrib->getIndexMap().indexSize();
    //    exint offsetSize = srcPrimAttrib->getIndexMap().offsetSize();
    //}
    GA_RWHandleT<GA_Offset> srcPrim_h(srcPrimAttrib);



    GA_Topology& topo = geoPoint->getTopology();
    const GA_ATITopology* const vtxPrimRef = topo.getPrimitiveRef();


    const GA_Topology& topo_tmpGeo0 = geoFullTmp->getTopology();
    //topo_tmpGeo0.makePrimitiveRef();
    const GA_ATITopology* const vtxPointRef_geoTmp = topo_tmpGeo0.getPointRef();
    const GA_ATITopology* const vtxPrimRef_geoTmp = topo_tmpGeo0.getPrimitiveRef();

    GA_Offset start, end;
    for (GA_Iterator it(geoFullTmp->getVertexRange(creatingGroup)); it.fullBlockAdvance(start, end); )
    {
        for (GA_Offset vtxoff = start; vtxoff < end; ++vtxoff)
        {
            if (copyPrimAttrib || outSrcPrimAttrib)
            {
                srcPrim_h.set(vtxPrimRef->getLink(vtxoff_first), vtxPrimRef_geoTmp->getLink(vtxoff));
            }
            topo.wireVertexPoint(vtxoff_first, vtxPointRef_geoTmp->getLink(vtxoff));
            ++vtxoff_first;
            topo.wireVertexPoint(vtxoff_first, dstpt_h.get(vtxoff));
            ++vtxoff_first;
        }
    }

    //////////////////// copy attributes //////////////////////

    if (copyPrimAttrib)
    {
#if 1
        GA_Offset primoff_last = primoff_first + entries;

        for (GA_AttributeDict::iterator it = geoPoint->getAttributes().begin(GA_ATTRIB_PRIMITIVE); !it.atEnd(); ++it)
        {
            GA_Attribute* const attribPtr = it.attrib();
            if (attribPtr == srcPrimAttrib)
                continue;
            //attribPtr->bumpDataId();
            //const UT_StringHolder& name = attribPtr->getName();
            //if (attribPtr->isDetached())
            //    continue;
            if (keepSourcePrim)
            {
                for (GA_Offset primoff = primoff_first; primoff < primoff_last; ++primoff)
                {
                    //const GA_Offset srcPrimVal = srcPrim_h.get(primoff);
                    attribPtr->copy(primoff, srcPrim_h.get(primoff));
                }
            }
            else
            {
                GA_Attribute* const srcAttribPtr = geoFullTmp->findPrimitiveAttribute(it.name());
                for (GA_Offset primoff = primoff_first; primoff < primoff_last; ++primoff)
                {
                    //const GA_Offset srcPrimVal = srcPrim_h.get(primoff);
                    attribPtr->copy(primoff, *srcAttribPtr, srcPrim_h.get(primoff));
                }
            }
        }
#else
        GA_Offset start, end;
        for (GA_Iterator it(GA_Range(geoPoint->getPrimitiveMap(), primoff_first, geoPoint->getNumPrimitiveOffsets())); it.fullBlockAdvance(start, end); )
        {
            for (GA_Offset primoff = start; primoff < end; ++primoff)
            {
            }
        }
#endif

#if !TMP_GFE_ConvertLine_UseDetached_SrcPrimAttrib
        if (!outSrcPrimAttrib)
        {
            geoPoint->getAttributes().destroyAttribute(srcPrimAttrib);
        }
#endif
    }



    //geoTmp_h.deleteGdp();

    return primoff_first;
}


#else


SYS_FORCE_INLINE
static GA_Offset
convertLine(
    GA_Detail* const geoFull,
    const bool isClosed = false,
    const bool copyPrimAttrib = false,
    const bool outSrcPrimAttrib = false,
    const UT_StringHolder& srcPrimAttribName = "",
    const bool keepSourcePrim = false,
    const UT_StringHolder& primGroupName = "",
    const UT_StringHolder& pointGroupName = "",
    const UT_StringHolder& vertexGroupName = "",
    const UT_StringHolder& edgeGroupName = "",
    const GA_Storage storage = GA_STORE_INVALID
)
{
    UT_ASSERT_P(geoFull);

    GU_DetailHandle geoFullTmp_h;
    GU_Detail* const geoFullTmp = new GU_Detail();
    geoFullTmp_h.allocateAndSet(geoFullTmp);
    geoFullTmp->replaceWith(*geoFull);

    geoFull->replaceWithPoints(*geoFullTmp);

    return convertLine(geoFull, geoFullTmp, isClosed, copyPrimAttrib, outSrcPrimAttrib, srcPrimAttribName, keepSourcePrim, primGroupName, pointGroupName, vertexGroupName, edgeGroupName, GA_STORE_INVALID);
}



#endif



SYS_FORCE_INLINE
static GU_DetailHandle
convertLine(
    const GA_Detail* const geoFull,
    const bool isClosed = false,
    const bool copyPrimAttrib = false,
    const bool outSrcPrimAttrib = false,
    const UT_StringHolder& srcPrimAttribName = "",
    const bool keepSourcePrim = false,
    const UT_StringHolder& primGroupName = "",
    const UT_StringHolder& pointGroupName = "",
    const UT_StringHolder& vertexGroupName = "",
    const UT_StringHolder& edgeGroupName = "",
    const GA_Storage storage = GA_STORE_INVALID
)
{
    UT_ASSERT_P(geoFull);

    GU_DetailHandle geoOut_h;
    GU_Detail* const geoOut = new GU_Detail();
    geoOut_h.allocateAndSet(geoOut);

    convertLine(geoOut, geoFull, isClosed, copyPrimAttrib, outSrcPrimAttrib, srcPrimAttribName, keepSourcePrim, primGroupName, pointGroupName, vertexGroupName, edgeGroupName, GA_STORE_INVALID);
    return geoOut_h;
}



} // End of namespace GFE_ConvertLine

#endif
