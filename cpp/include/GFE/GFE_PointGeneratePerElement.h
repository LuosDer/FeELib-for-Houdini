
#pragma once

#ifndef __GFE_PointGeneratePerElement_h__
#define __GFE_PointGeneratePerElement_h__

#include "GFE/GFE_PointGeneratePerElement.h"

#include "GFE/GFE_GeoFilter.h"


#include "GFE/GFE_AttributeCopy.h"
#include "GFE/GFE_Math.h"
#include "GFE/GFE_MeshTopology.h"

//#include "SOP/SOP_Scatter-2.0.proto.h"


class GFE_PointGenPerElem : public GFE_AttribFilter {

#define __TEMP_GFE_PointGenPerElem_GroupName "__TEMP_GFE_PointGenPerElem_GROUP"

public:
    using GFE_AttribFilter::GFE_AttribFilter;


    void
        setComputeParm(
            const GA_GroupType elemType = GA_GROUP_PRIMITIVE,
            const bool setPositionOnElem = true,
            const exint subscribeRatio = 64,
            const exint minGrainSize = 1024
        )
    {
        setHasComputed();
        this->elemType           = elemType;
        this->setPositionOnElem  = setPositionOnElem;
        this->subscribeRatio     = subscribeRatio;
        this->minGrainSize       = minGrainSize;
    }

    SYS_FORCE_INLINE void setNumPointAttrib(const GA_Attribute* const inAttrib = nullptr)
    { numPointAttrib = inAttrib; }

    const GA_Attribute* setNumPointAttrib(const UT_StringRef& numPointAttribName)
    {
        numPointAttrib = (geoSrc ? geoSrc : geo)->findAttribute(GFE_Type::attributeOwner_groupType(elemType), numPointAttribName);
        if (numPointAttrib && !(numPointAttrib->getAIFTuple() &&
                               numPointAttrib->getTupleSize() == 1 &&
                               numPointAttrib->getStorageClass() == GA_STORECLASS_INT))
            numPointAttrib = nullptr;
        return numPointAttrib;
    }


    
    //SYS_FORCE_INLINE GA_Attribute* setSrcElemoffAttrib(const GA_Attribute* const inAttrib = nullptr)
    //{ srcElemoffAttrib = inAttrib; }

    const char* setSrcElemoffAttrib(const char* const inString)
    { return srcElemoffAttribName = inString; }
    
    
    
private:

    // can not use in parallel unless for each GA_Detail
    virtual bool
        computeCore() override
    {
        elemOwner = GFE_Type::attributeOwner_groupType(elemType);
        
        UT_ASSERT_MSG(!numPointAttrib || numPointAttrib->getOwner() == elemOwner, "not correct numPointAttrib owner");
        UT_ASSERT_MSG(!numPointAttrib ||
                      (numPointAttrib->getAIFTuple() &&
                       numPointAttrib->getTupleSize() == 1 &&
                       numPointAttrib->getStorageClass() == GA_STORECLASS_INT),
              "not correct numPointAttrib type");
        
        if (groupParser.isEmpty())
            return true;
        
        const bool isElementGroup = GFE_Type::isElementGroup(elemOwner);
        
        GU_DetailHandle geoTmp_h;
        geoTmp = new GU_Detail();
        geoTmp_h.allocateAndSet(geoTmp);
        geoTmp->replaceWith(geoSrc ? *geoSrc : *geo);

        geoGroup = groupParser.isDetached() ? groupParser.getGroup(elemType) : geoTmp->getGroupTable(elemType)->find(groupParser.getName());
        if (isElementGroup)
        {
            const GA_Range r(geoTmp->getIndexMap(elemOwner), static_cast<const GA_ElementGroup*>(geoGroup));
            geoSplittableRange = GA_SplittableRange(r);
        }

        GA_AttributeUPtr numPointAttribMidUPtr;
        if (isElementGroup && numPointAttrib)
        {
            if (numPointAttrib->isDetached())
            {
                numPointAttribMidUPtr = GFE_Attribute::clone(*geoTmp, *numPointAttrib);
                numPointAttribMid = numPointAttribMidUPtr.get();
            }
            else
            {
                numPointAttribMid = geoTmp->findAttribute(elemOwner, numPointAttrib->getName());
            }
            GFE_Attribute::accumulateT<exint>(*numPointAttribMid, geoSplittableRange);
        }
        
        GA_Size numpt;
        if (isElementGroup)
        {
            numpt = geoGroup ? static_cast<const GA_ElementGroup*>(geoGroup)->entries() : geoTmp->getIndexMap(elemOwner).indexSize();
            if (numPointAttribMid)
                numPointAttribMid->getAIFTuple()->get(numPointAttribMid, numpt-1, numpt);
        }
        else
        {
            if (geoGroup)
            {
                numpt = static_cast<const GA_EdgeGroup*>(geoGroup)->entries();
            }
            else
            {
                GFE_MeshTopology meshTopology(geoTmp, cookparms);
                meshTopology.outIntermediateAttrib = false;
                meshTopology.groupParser.setGroup(groupParser);
                creatingGroup = meshTopology.setVertexNextEquivNoLoopGroup(true);
                dstptAttrib = meshTopology.setVertexPointDst(true);
                meshTopology.compute();
        
                numpt = creatingGroup->entries();
            }
        }

        geo->clear();
        geo->appendPointBlock(numpt);
        
        if (srcElemoffAttribName)
            srcElemoffAttrib = getOutAttribArray().findOrCreateTuple(false, GA_ATTRIB_POINT, GA_STORECLASS_INT, GA_STORE_INVALID, srcElemoffAttribName, 1, GA_Defaults(-1));

        numPoint_h   = numPointAttribMid;
        srcElemoff_h = srcElemoffAttrib;
        pos_h        = geo->getP();
        posRef_h     = geoTmp->getP();

        if (!srcElemoffAttrib && !setPositionOnElem)
            return true;
        
        switch (elemType)
        {
        case GA_GROUP_PRIMITIVE: pointGenPerPrim();   break;
        case GA_GROUP_POINT:     pointGenPerPoint();  break;
        case GA_GROUP_VERTEX:    pointGenPerVertex(); break;
        case GA_GROUP_EDGE:      pointGenPerEdge();   break;
        default:                 return false;        break;
        }

        return true;
    }

    void pointGenPerPrim()
    {
        UTparallelFor(geoSplittableRange, [this](const GA_SplittableRange& r)
        {
            bool unCorrectVtxpnum;
            UT_Vector3T<fpreal> pos, pos0, pos1, pos2;
            GA_Offset start, end;
            for (GA_Iterator it(r.begin()); it.blockAdvance(start, end); )
            {
                for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                {
                    if (setPositionOnElem)
                    {
                        const GA_Size numvtx = geoTmp->getPrimitiveVertexCount(elemoff);
                        if (numvtx > 3)
                        {
                            const GA_Size numvtx2 = numvtx-2;
                            for (GA_Size vtxpnum = 0; vtxpnum < numvtx; ++vtxpnum)
                            {
                                unCorrectVtxpnum = false;
                                const bool isEndVtx = vtxpnum >= numvtx2;
                                // const GA_Offset vtxoff = geoTmp->getPrimitiveVertexOffset(elemoff, vtxpnum);
                                // const GA_Offset ptoff = geoTmp->vertexPoint(vtxoff);
                                pos0 = posRef_h.get(getPrimitivePointOffset(geoTmp, elemoff, vtxpnum));
                                pos1 = posRef_h.get(getPrimitivePointOffset(geoTmp, elemoff, isEndVtx ? (vtxpnum == numvtx2 ? numvtx-1 : 0) : vtxpnum + 1));
                                pos2 = posRef_h.get(getPrimitivePointOffset(geoTmp, elemoff, isEndVtx ? (vtxpnum == numvtx2 ? 0 : 1)        : vtxpnum + 2));
                                
                                for (GA_Size vtxpnum1 = isEndVtx ? (vtxpnum >= numvtx2 ? 2 : 1) : 0; vtxpnum1 < vtxpnum; ++vtxpnum1)
                                {
                                    if (!GFE_Math::pointInTriangleT<fpreal>(posRef_h.get(getPrimitivePointOffset(geoTmp, elemoff, vtxpnum1)), pos0, pos1, pos2))
                                    {
                                        unCorrectVtxpnum = true;
                                        break;
                                    }
                                }
                                if (unCorrectVtxpnum)
                                    continue;
                                for (GA_Size vtxpnum1 = vtxpnum+2; vtxpnum1 < numvtx; ++vtxpnum1)
                                {
                                    if (!GFE_Math::pointInTriangleT<fpreal>(posRef_h.get(getPrimitivePointOffset(geoTmp, elemoff, vtxpnum1)), pos0, pos1, pos2))
                                    {
                                        unCorrectVtxpnum = true;
                                        break;
                                    }
                                }
                                if (unCorrectVtxpnum)
                                    continue;
                                pos = (pos0 + pos1 + pos2) / 3.0;
                                break;
                            }
                            UT_ASSERT_MSG(!unCorrectVtxpnum, "not possible");
                        }
                        else
                        {
                            switch (numvtx)
                            {
                            default:
                            case 0: pos = 0;                                                         break;
                            case 1: pos = posRef_h.get(getPrimitivePointOffset(geoTmp, elemoff, 0)); break;
                            case 2: 
                                pos0 = posRef_h.get(getPrimitivePointOffset(geoTmp, elemoff, 0));
                                pos1 = posRef_h.get(getPrimitivePointOffset(geoTmp, elemoff, 1));
                                pos = (pos0 + pos1) / 2.0;
                            break;
                            case 3:
                                pos0 = posRef_h.get(getPrimitivePointOffset(geoTmp, elemoff, 0));
                                pos1 = posRef_h.get(getPrimitivePointOffset(geoTmp, elemoff, 1));
                                pos2 = posRef_h.get(getPrimitivePointOffset(geoTmp, elemoff, 2));
                                pos = (pos0 + pos1 + pos2) / 3.0;
                            break;
                            }
                        }
                    }
                    if (numPointAttribMid)
                    {
                        for (GA_Size ptoff = elemoff ? numPoint_h.get(elemoff-1) : 0; ptoff < numPoint_h.get(elemoff); ++ptoff)
                        {
                            if (setPositionOnElem)
                                pos_h.set(ptoff, pos);
                            if (srcElemoffAttrib)
                                srcElemoff_h.set(ptoff, elemoff);
                        }
                    }
                    else
                    {
                        if (setPositionOnElem)
                            pos_h.set(elemoff, pos);
                        if (srcElemoffAttrib)
                            srcElemoff_h.set(elemoff, elemoff);
                    }
                    
                }
            }
        }, subscribeRatio, minGrainSize);
    }

    SYS_FORCE_INLINE GA_Offset getPrimitivePointOffset(const GA_Detail* const geo, const GA_Offset primoff, const GA_Size vtxpnum)
    { return geoTmp->vertexPoint(geoTmp->getPrimitiveVertexOffset(primoff, vtxpnum)); }



    void pointGenPerPoint()
    {
        
    }
    void pointGenPerVertex()
    {
        
    }
    
    void pointGenPerEdge()
    {
        UT_ASSERT_P(dstptAttrib);
        
        UT_Vector3T<fpreal> pos0, pos1;
        GA_Offset ptoff_first = 0;
        GA_PageHandleT<GA_Offset, GA_Offset, true, false, const GA_Attribute, const GA_ATINumeric, const GA_Detail> dstpt_ph(dstptAttrib);
        const GA_SplittableRange geoSplittableRange(geoTmp->getVertexRange(creatingGroup));
        for (GA_PageIterator pit = geoSplittableRange.beginPages(); !pit.atEnd(); ++pit)
        {
            GA_Offset start, end;
            for (GA_Iterator it(pit.begin()); it.blockAdvance(start, end); )
            {
                dstpt_ph.setPage(start);
                for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                {
                    if (srcElemoffAttrib)
                        srcElemoff_h.set(ptoff_first, elemoff);
                    
                    if (setPositionOnElem)
                    {
                        pos0 = posRef_h.get(geoTmp->vertexPoint(elemoff));
                        pos1 = posRef_h.get(dstpt_ph.value(elemoff));
                        pos_h.set(ptoff_first, (pos0 + pos1) * 0.5);
                    }
                    ++ptoff_first;
                }
            }
        }
    }
    

    //void scatter()
    //{
    //    scatterParms.setGroup(__TEMP_GFE_PointGenPerElem_GroupName);
    //    //scatterParms.setPrimNumAttrib();
    //    //scatterParms.setPrimNumAttrib();
    //    
    //    SOP_NodeCache* const nodeCache = scatterVerb->allocCache();
    //    const auto scatterCookparms = GFE_NodeVerb::newCookParms(cookparms, scatterParms, nodeCache, nullptr, &inputgdh);
    //    
    //    scatterVerb->cook(scatterCookparms);
    //}

public:
    GA_GroupType elemType = GA_GROUP_PRIMITIVE;
    bool setPositionOnElem = true;
    const char* srcElemoffAttribName = nullptr;
    
    
private:
    const GA_Attribute* numPointAttrib = nullptr;
    exint subscribeRatio = 64;
    exint minGrainSize = 1024;


private:
    const GA_Group* geoGroup;
    GA_AttributeOwner elemOwner;
    
    GA_ROHandleT<exint> numPoint_h;
    GA_RWHandleT<exint> srcElemoff_h;
    GA_RWHandleT<UT_Vector3T<fpreal>> pos_h;
    GA_RWHandleT<UT_Vector3T<fpreal>> posRef_h;
    
    const GA_Attribute* dstptAttrib;
    const GA_VertexGroup* creatingGroup;
    
    GA_Attribute* numPointAttribMid = nullptr;
    GA_Attribute* srcElemoffAttrib = nullptr;
    
    GA_SplittableRange geoSplittableRange;
    
    GU_Detail* geoTmp;


private:
    
    //UT_Array<GU_ConstDetailHandle> inputgdh;
    //SOP_Scatter_2_0Parms scatterParms;
    //const SOP_NodeVerb* const scatterVerb = SOP_NodeVerb::lookupVerb("scatter::2.0");
    
#undef __TEMP_GFE_PointGenPerElem_GroupName
    
}; // End of class GFE_PointGenPerElem


















#endif
