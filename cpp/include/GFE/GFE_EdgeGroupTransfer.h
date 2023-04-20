
#pragma once

#ifndef __GFE_EdgeGroupTransfer_h__
#define __GFE_EdgeGroupTransfer_h__

//#include "GFE/GFE_EdgeGroupTransfer.h"

#include "GU/GU_Snap.h"
#include "GFE/GFE_GeoFilter.h"




class GFE_EdgeGroupTransfer : public GFE_AttribFilter, public GFE_GeoFilterRef {

public:
    using GFE_AttribFilter::GFE_AttribFilter;
    
    ~GFE_EdgeGroupTransfer()
    {
    }


    SYS_FORCE_INLINE void setGroup(const GA_PrimitiveGroup* const geoPrimGroup = nullptr)
    {
        groupParser.setGroup(geoPrimGroup);
    }
    SYS_FORCE_INLINE void setGroup(const UT_StringHolder& primGroupName = "")
    {
        groupParser.setPrimitiveGroup(primGroupName);
    }

    SYS_FORCE_INLINE void createSrcPrimAttrib(
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

    SYS_FORCE_INLINE void setCopyPrimitiveAttrib(const bool copyAttrib = false,const UT_StringHolder& copyAttribName = "*")
    {
        copyPrimAttrib = copyAttrib;
        copyPrimAttribName = copyAttribName;
    }

    SYS_FORCE_INLINE void setCopyVertexAttrib(const bool copyAttrib = false,const UT_StringHolder& copyAttribName = "*")
    {
        copyVertexAttrib = copyAttrib;
        copyVertexAttribName = copyAttribName;
    }
    
    virtual SYS_FORCE_INLINE void bumpDataIdsForAddOrRemove() const override
    {
        geo->bumpDataIdsForAddOrRemove(false, true, true);
    }
    SYS_FORCE_INLINE GA_Offset get_primoff_first() const
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

        
        GFE_Adjacency adjacency(geoOriginTmp);
        //adjacency.outAsOffset = false;
        adjacency.outIntermediateAttrib = false;
        adjacency.groupParser.setGroup(groupParser.getVertexGroup());
        
        const GA_VertexGroup* const creatingGroup = adjacency.setVertexNextEquivNoLoopGroup(true);
        const GA_RWHandleT<GA_Offset> dstpt_h(adjacency.setVertexPointDst(true));
        
        adjacency.compute();
        // const GA_VertexGroup* const creatingGroup = adjacency.getVertexNextEquivNoLoopGroup();
        // const GA_RWHandleT<GA_Offset> dstpt_h(adjacency.getVertexPointDst());
        
        //const GA_VertexGroup* const creatingGroup = GFE_VertexNextEquiv::addGroupVertexNextEquivNoLoop(geoOriginTmp, groupParser.getVertexGroup());
        //const GA_RWHandleT<GA_Offset> dstpt_h = GFE_TopologyReference::addAttribVertexPointDst(geoOriginTmp, groupParser.getVertexGroup());

        UT_ASSERT_P(dstpt_h.getAttribute());


        const GA_Size entries = creatingGroup->getGroupEntries();

        GA_Offset vtxoff_first;
        primoff_first = geo->appendPrimitivesAndVertices(GA_PrimitiveTypeId(1), entries, 2, vtxoff_first, isClosed);
        //geo->appendPrimitivesAndVertices(GA_PrimitiveTypeId(1), entries, 2, vtxoff_first, isClosed);

        const bool createSrcPrimAttrib = copyPrimAttrib || outSrcPrimAttrib;

        GA_Attribute* srcPrimAttrib = nullptr;
        GA_Attribute* srcVtxAttrib  = nullptr;
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
    
protected:
    bool hasInGeo;

private:
    GA_EdgeGroup* const edgeGroup,

    const bool useSnapDist = true,
    const fpreal snapDist = 0.001,
    const bool useNewGroupName = false,
    const UT_StringHolder& newGroupName = "",
    const bool reverseGroup = false,
    const bool outAsVertexGroup = false,
    const exint subscribeRatio = 64,
    const exint minGrainSize = 64
    
}; // End of class GFE_EdgeGroupTransfer




namespace GFE_EdgeGroupTransfer {


static void
edgeGroupTransfer(
    GA_Detail* const geo,
    const GA_Detail* const geoRef,
    const GA_VertexGroup* const geoVtxGroup,
    GA_EdgeGroup* const edgeGroup,

    const bool useSnapDist = true,
    const fpreal snapDist = 0.001,
    const bool useNewGroupName = false,
    const UT_StringHolder& newGroupName = "",
    const bool reverseGroup = false,
    const bool outAsVertexGroup = false,
    const exint subscribeRatio = 64,
    const exint minGrainSize = 64
    )
{
    UT_ASSERT_P(geo);
    UT_ASSERT_P(geoRef);

    GA_Attribute* snapDstPointAttribPtr = geo->getAttributes().createTupleAttribute(GA_ATTRIB_POINT, "snapDstPoint", GA_STORE_INT64, 1, GA_Defaults(-1));
    const GA_RWHandleID snapDstPoint_h(snapDstPointAttribPtr);


    GU_Snap::PointSnapParms pointSnapParms;
    pointSnapParms.myAlgorithm = GU_Snap::PointSnapParms::ALGORITHM_CLOSEST_POINT;
    pointSnapParms.myDistance = useSnapDist ? snapDist : SYS_FP64_MAX;
    pointSnapParms.myOutputAttribH = snapDstPoint_h;


    GU_Snap::snapPoints(*static_cast<GU_Detail*>(geo), static_cast<const GU_Detail*>(geoRef), pointSnapParms);


    
    
    int vtxpnum = vertexprimindex(0, @vtxnum);
    int nextvtx = vtxpnum + 1;
    if ( nextvtx == @numvtx && primintrinsic(0, 'closed', @primnum) ) nextvtx = 0;
    int nextpt = primpoint(0, @primnum, nextvtx);

    int lastpt_id = point(0, chs("../tmpGroup_snapped_to"), @ptnum);
    int nextpt_id = point(0, chs("../tmpGroup_snapped_to"), nextpt);



    if ( chi("../outAsVertexGroup") ) {
        if ( find(string[](detailintrinsic(1, 'edgegroups')), chs("../groupName")) >= 0 ) {
            i@outgrp = inedgegroup(1, chs("../groupName"), lastpt_id, nextpt_id) ^ chi("../reverseGroup");
        } else {
            i@outgrp = chi("../reverseGroup");
            int pointprims1[] = pointprims(1, lastpt_id);
            for (int vtxnum = pointvertex(1, nextpt_id); vtxnum != -1; vtxnum = vertexnext(1, vtxnum)) {
                int primnum = vertexprim(1, vtxnum);
                if ( find(pointprims1, primnum) >= 0 && inprimgroup(1, chs("../groupName"), primnum) ) {
                    i@outgrp = !chi("../reverseGroup");
                    break;
                }
            }
        }
    } else {
        if ( find(string[](detailintrinsic(1, 'edgegroups')), chs("../groupName")) >= 0 ) {
            setedgegroup(0, chs("vex_selectiongroup"), @ptnum, nextpt, inedgegroup(1, chs("../groupName"), lastpt_id, nextpt_id) ^ chi("../reverseGroup"));
        } else {
            int flag = chi("../reverseGroup");
            int pointprims1[] = pointprims(1, lastpt_id);
            // printf('\n pointprims1 %d', pointprims1);
            for (int vtxnum = pointvertex(1, nextpt_id); vtxnum != -1; vtxnum = vertexnext(1, vtxnum)) {
                int primnum = vertexprim(1, vtxnum);
                // printf('\n primnum %d', primnum);
                if ( find(pointprims1, primnum) >= 0 && inprimgroup(1, chs("../groupName"), primnum) ) {
                    flag = !chi("../reverseGroup");
                    break;
                }
            }
            setedgegroup(0, chs("vex_selectiongroup"), @ptnum, nextpt, flag);
        }
    }



    
}


SYS_FORCE_INLINE
    static void
    edgeGroupTransfer(
        GA_Detail* const geo,
        const GA_Detail* const geoRef,
        const GA_VertexGroup* const geoVtxGroup,

        const UT_StringHolder& edgeGroupName,
        const bool useSnapDist = true,
        const fpreal snapDist = 0.001,
        const bool useNewGroupName = false,
        const UT_StringHolder& newGroupName = "",
        const bool reverseGroup = false,
        const bool outAsVertexGroup = false,
        const exint subscribeRatio = 64,
        const exint minGrainSize = 64
    )
{
    UT_ASSERT_P(geo);
    UT_ASSERT_P(geoRef);

    if (!edgeGroupName.isstring() || edgeGroupName.length() == 0)
        return;

    for (GA_GroupTable::iterator<GA_Group> it = geo->getGroupTable(GA_GROUP_EDGE)->beginTraverse(); !it.atEnd(); ++it)
    {
        GA_EdgeGroup* const group = UTverify_cast<GA_EdgeGroup*>(it.group());
        //if (group->isDetached())
        //    continue;
        if (!group->getName().multiMatch(edgeGroupName))
            continue;

        edgeGroupTransfer(geo, geoRef,
            geoVtxGroup, group,
            useSnapDist, snapDist,
            useNewGroupName, newGroupName,
            reverseGroup, outAsVertexGroup,
            subscribeRatio, minGrainSize);
    }

}

SYS_FORCE_INLINE
static void
edgeGroupTransfer(
    GA_Detail* const geo,
    const GA_Detail* const geoRef,
    const GA_Group* const geoGroup,

    const UT_StringHolder& edgeGroupName,
    const bool useSnapDist = true,
    const fpreal snapDist = 0.001,
    const bool useNewGroupName = false,
    const UT_StringHolder& newGroupName = "",
    const bool reverseGroup = false,
    const bool outAsVertexGroup = false,
    const exint subscribeRatio = 64,
    const exint minGrainSize = 64
)
{
    UT_ASSERT_P(geo);
    UT_ASSERT_P(geoRef);
    
    const GA_VertexGroup* const geoVtxGroup = GFE_GroupPromote::groupFindPromoteVertexDetached(geo, geoGroup);

    edgeGroupTransfer(geo, geoRef,
        geoVtxGroup, edgeGroupName,
        useSnapDist, snapDist,
        useNewGroupName, newGroupName,
        reverseGroup, outAsVertexGroup,
        subscribeRatio, minGrainSize);

    if (geoVtxGroup->isDetached())
        delete geoVtxGroup;
}


SYS_FORCE_INLINE
static void
edgeGroupTransfer(
    const SOP_NodeVerb::CookParms& cookparms,
    GA_Detail* const geo,
    const GA_Detail* const geoRef,
    const GA_GroupType groupType,
    const UT_StringHolder& groupName,

    const UT_StringHolder& edgeGroupName,
    const bool useSnapDist = true,
    const fpreal snapDist = 0.001,
    const bool useNewGroupName = false,
    const UT_StringHolder& newGroupName = "",
    const bool reverseGroup = false,
    const bool outAsVertexGroup = false,
    const exint subscribeRatio = 64,
    const exint minGrainSize = 64
    )
{
    UT_ASSERT_P(geo);
    UT_ASSERT_P(geoRef);

    if (!edgeGroupName.isstring() || edgeGroupName.length() == 0)
        return;

    GOP_Manager gop;
    const GA_Group* const geoGroup = GFE_GroupParse_Namespace::findOrParseGroupDetached(cookparms, geo, groupType, groupName, gop);
    if (geoGroup && geoGroup->isEmpty())
        return;

    edgeGroupTransfer(geo, geoRef,
        geoGroup, edgeGroupName,
        useSnapDist, snapDist,
        useNewGroupName, newGroupName,
        reverseGroup, outAsVertexGroup,
        subscribeRatio, minGrainSize);

    GFE_Group::groupBumpDataId(geo, GA_GROUP_EDGE, edgeGroupName);
}




} // End of namespace GFE_EdgeGroupTransfer

#endif
