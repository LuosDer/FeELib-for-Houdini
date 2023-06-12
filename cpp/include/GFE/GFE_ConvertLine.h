
#pragma once

#ifndef __GFE_ConvertLine_h__
#define __GFE_ConvertLine_h__

#include "GFE/GFE_ConvertLine.h"

#include "GFE/GFE_GeoFilter.h"



#include "GFE/GFE_MeshTopology.h"


class GFE_ConvertLine : public GFE_AttribFilter {

public:
    //using GFE_AttribFilter::GFE_AttribFilter;
    
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
        geoOriginTmp->replaceWith(geo);
    }

    ~GFE_ConvertLine()
    {
    }

    
    SYS_FORCE_INLINE void createSrcPrimAttrib(
        const bool detached = false,
        const GA_Storage storage = GA_STORE_INVALID,
        const UT_StringRef& srcPrimAttribName = "srcPrim"
    )
    {
        srcPrimAttribDetached = detached;
        srcPrimAttribStorage = storage;
        outSrcPrimAttrib = true;
        this->srcPrimAttribName = srcPrimAttribName;
    }

    void
        setComputeParm(
            const bool isClosed       = false,
            const bool keepSourcePrim = false
        )
    {
        setHasComputed();
        primoff_first = GFE_INVALID_OFFSET;
        
        this->isClosed       = isClosed;
        this->keepSourcePrim = keepSourcePrim;
    }

    SYS_FORCE_INLINE void setCopyPrimitiveAttrib(const bool copyAttrib = false,const UT_StringRef& copyAttribName = "*")
    {
        copyPrimAttrib = copyAttrib;
        copyPrimAttribName = copyAttribName;
    }

    SYS_FORCE_INLINE void setCopyVertexAttrib(const bool copyAttrib = false,const UT_StringRef& copyAttribName = "*")
    {
        copyVertexAttrib = copyAttrib;
        copyVertexAttribName = copyAttribName;
    }
    
    virtual SYS_FORCE_INLINE void bumpDataIdsForAddOrRemove() const override
    { geo->bumpDataIdsForAddOrRemove(false, true, true); }
    
    SYS_FORCE_INLINE GA_Offset get_primoff_first() const
    { return primoff_first; }

    


private:

    // can not use in parallel unless for each GA_Detail
    virtual bool
        computeCore() override
    {
        if (groupParser.isEmpty())
            return true;

        if (!keepSourcePrim)
            geo->replaceWithPoints(*geoOrigin);
        
        GA_AttributeSet& geoAttribSet = geo->getAttributes();
        GA_Attribute* attribPtr = nullptr;
        ::std::vector<GA_Attribute*> copyPrimAttribArray;
        ::std::vector<GA_Attribute*> copyVertexAttribArray;
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

        
        GFE_MeshTopology meshTopology(geoOriginTmp);
        meshTopology.outIntermediateAttrib = false;
        meshTopology.groupParser.setGroup(groupParser.getVertexGroup());
        const GA_VertexGroup* const creatingGroup = meshTopology.setVertexNextEquivNoLoopGroup(true);
        const GA_ROHandleT<GA_Offset> dstpt_h(meshTopology.setVertexPointDst(true));
        meshTopology.compute();
        
        const GA_Size entries = creatingGroup->entries();

        UT_ASSERT_P(dstpt_h.getAttribute());

        GA_Offset vtxoff_first;
        primoff_first = geo->appendPrimitivesAndVertices(GA_PrimitiveTypeId(1), entries, 2, vtxoff_first, isClosed);
        //geo->appendPrimitivesAndVertices(GA_PrimitiveTypeId(1), entries, 2, vtxoff_first, isClosed);

        const bool createSrcPrimAttrib = copyPrimAttrib || outSrcPrimAttrib;

        GA_Attribute* srcPrimAttrib = nullptr;
        GA_Attribute* srcVtxAttrib  = nullptr;
        if (createSrcPrimAttrib)
            srcPrimAttrib = getOutAttribArray().findOrCreateTuple(
                srcPrimAttribDetached || !outSrcPrimAttrib,
                GA_ATTRIB_PRIMITIVE, GA_STORECLASS_INT,
                srcPrimAttribStorage, srcPrimAttribName, 1, GA_Defaults(GFE_INVALID_OFFSET));

        if (copyVertexAttrib)
            srcVtxAttrib = getOutAttribArray().findOrCreateTuple(true, GA_ATTRIB_PRIMITIVE, GA_STORECLASS_INT);

        GA_RWHandleT<GA_Offset> srcPrim_h(srcPrimAttrib);
        GA_RWHandleT<GA_Offset> srcVtx_h(srcVtxAttrib);


        GA_Topology& topo = geo->getTopology();
        const GA_ATITopology& vtxPrimRef = *topo.getPrimitiveRef();


        const GA_Topology& topo_tmpGeo0 = geoOriginTmp->getTopology();
        //topo_tmpGeo0.makePrimitiveRef();
        const GA_ATITopology& vtxPointRef_geoTmp = *topo_tmpGeo0.getPointRef();
        const GA_ATITopology& vtxPrimRef_geoTmp  = *topo_tmpGeo0.getPrimitiveRef();

        GA_Offset start, end;
        for (GA_Iterator it(geoOriginTmp->getVertexRange(creatingGroup)); it.fullBlockAdvance(start, end); )
        {
            for (GA_Offset vtxoff = start; vtxoff < end; ++vtxoff)
            {
                if (createSrcPrimAttrib)
                {
                    srcPrim_h.set(vtxPrimRef.getLink(vtxoff_first), vtxPrimRef_geoTmp.getLink(vtxoff));
                }
                if (copyVertexAttrib)
                {
                    srcVtx_h.set(vtxPrimRef.getLink(vtxoff_first), vtxoff);
                }
                topo.wireVertexPoint(vtxoff_first, vtxPointRef_geoTmp.getLink(vtxoff));
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

            if (copyVertexAttrib)
            {
                const size_t size = copyVertexAttribArray.size();
                for (size_t i = 0; i < size; i++)
                {
                    GA_Attribute& srcAttrib = *copyVertexAttribArray[i];

                    attribPtr = geo->findPrimitiveAttribute(srcAttrib.getName());
                    if (attribPtr)
                        continue;

                    attribPtr = geoAttribSet.cloneAttribute(GA_ATTRIB_PRIMITIVE, srcAttrib.getName(), srcAttrib, true);
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
    UT_StringRef copyPrimAttribName;
    UT_StringRef copyVertexAttribName;
    
protected:
    bool hasInGeo;

private:
    
    bool outSrcPrimAttrib = false;
    bool srcPrimAttribDetached = false;
    UT_StringRef srcPrimAttribName;
    GA_Storage srcPrimAttribStorage = GA_STORE_INVALID;

    GA_Offset primoff_first = GFE_INVALID_OFFSET;

    GU_DetailHandle geoOrigin_h;
    const GA_Detail* geoOrigin;
    GU_Detail* geoOriginTmp;
}; // End of class GFE_ConvertLine







#endif
