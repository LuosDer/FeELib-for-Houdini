
#pragma once

#ifndef __GFE_GroupUnshared_h__
#define __GFE_GroupUnshared_h__

//#include "GFE/GFE_GroupUnshared.h"

#include "GFE/GFE_GeoFilter.h"

#include "GFE/GFE_MeshTopology.h"
#include "GFE/GFE_GroupUnion.h"


#include "GU/GU_Snap.h"




class GFE_GroupUnshared : public GFE_AttribCreateFilter {

public:
    // using GFE_AttribCreateFilter::GFE_AttribCreateFilter;
    
    GFE_GroupUnshared(
        GFE_Detail* const geo,
        const SOP_NodeVerb::CookParms* const cookparms = nullptr
    )
        : GFE_AttribCreateFilter(geo, cookparms)
        , meshTopology(geo, cookparms)
    {
    }

    GFE_GroupUnshared(
        GA_Detail& geo,
        const SOP_NodeVerb::CookParms* const cookparms = nullptr
    )
        : GFE_AttribCreateFilter(geo, cookparms)
        , meshTopology(geo, cookparms)
    {
    }

    void
        setComputeParm(
            const bool preFusePoint = false,
            const fpreal fuseDist = 1e-05,
            const exint subscribeRatio = 64,
            const exint minGrainSize = 1024
        )
    {
        setHasComputed();

        this->preFusePoint = preFusePoint;
        this->fuseDist = fuseDist;
        //this->subscribeRatio = subscribeRatio;
        //this->minGrainSize = minGrainSize;
        
        meshTopology.setComputeParm(true, subscribeRatio, minGrainSize);
    }


    void findOrCreateGroup(
        const bool detached = false,
        const GA_GroupType groupType = GA_GROUP_VERTEX,
        const UT_StringRef& name = "__topo_unshared"
    )
    {
        switch (groupType)
        {
        case GA_GROUP_PRIMITIVE:
            break;
        case GA_GROUP_POINT:
            break;
        case GA_GROUP_VERTEX:
            findOrCreateVertexGroup(detached, name);
            break;
        case GA_GROUP_EDGE:
            findOrCreateEdgeGroup(detached, name);
            break;
        }
    }

    void findOrCreateGroup(
        const bool detached = false,
        const bool outVertexGroup = true,
        const UT_StringRef& name = "__topo_unshared"
    )
    {
        if(outVertexGroup)
        {
            findOrCreateVertexGroup(detached, name);
        }
        else
        {
            findOrCreateEdgeGroup(detached, name);
        }
    }

    SYS_FORCE_INLINE GA_VertexGroup* findOrCreateVertexGroup(const bool detached = false, const UT_StringRef& name = "__topo_unshared")
    {
        unsharedVertexGroup = meshTopology.setVertexNextEquivGroup(detached, name);
        return unsharedVertexGroup;
    }

    SYS_FORCE_INLINE GA_EdgeGroup* findOrCreateEdgeGroup(const bool detached = false, const UT_StringRef& name = "__topo_unshared")
    {
        unsharedEdgeGroup = meshTopology.setUnsharedEdgeGroup(detached, name);
        return unsharedEdgeGroup;
    }

    SYS_FORCE_INLINE GA_VertexGroup* getVertexGroup() const
    { return meshTopology.getVertexNextEquivGroup(); }

    SYS_FORCE_INLINE GA_EdgeGroup* getEdgeGroup() const
    { return meshTopology.getUnsharedEdgeGroup(); }









private:


    virtual bool
        computeCore() override
    {
        if (groupParser.isEmpty())
            return true;

        GU_DetailHandle geoOrigin_h;
        GU_Detail* geoOriginTmp = nullptr;
        if (preFusePoint)
        {
            geoOriginTmp = new GU_Detail();
            geoOrigin_h.allocateAndSet(geoOriginTmp);
            geoOriginTmp->replaceWith(*geo);

            GU_Snap::PointSnapParms fuseParms;
            fuseParms.myDistance = fuseDist;
            fuseParms.myAlgorithm = GU_Snap::PointSnapParms::SnapAlgorithm::ALGORITHM_CLOSEST_POINT;
            fuseParms.myConsolidate = true;
            fuseParms.myDeleteConsolidated = true;
            fuseParms.myQPosH = geoOriginTmp->getP();
            fuseParms.myTPosH = geoOriginTmp->getP();
            GU_Snap::snapPoints(*geoOriginTmp, nullptr, fuseParms);
            //GU_Snap::snapPoints(*geoOriginTmp, static_cast<const GU_Detail*>(geo), fuseParms);
            
            meshTopology.reset(geoOriginTmp, cookparms);
        }
        else
        {
            geoOriginTmp = geo->asGU_Detail();
            
            meshTopology.reset(geo, cookparms);
        }
        
        getOutGroupArray()
        meshTopology.groupParser.setGroup(groupParser.getVertexGroup());
        
        meshTopology.compute();
        
        //GA_VertexGroup* unsharedGroup = GFE_VertexNextEquiv_Namespace::addGroupVertexNextEquiv(geoOriginTmp, groupParser.getVertexGroup(), GA_STORE_INVALID);

        //GFE_GroupPromote::groupPromote(geoOriginTmp, unsharedGroup, unsharedAttribClass, unsharedAttribName, true);
        GFE_GroupUnion::groupUnion(getOutGroupArray()[0], unsharedGroup);
        GFE_GroupUnion::groupUnion_topoAttrib(geo, getOutGroupArray()[0], unsharedGroup);
    

        return true;
    }




public:
    bool preFusePoint = false;
    fpreal fuseDist = 1e-05;

private:
    GFE_MeshTopology meshTopology;
    
    
    GA_VertexGroup* unsharedVertexGroup = nullptr;
    GA_EdgeGroup*   unsharedEdgeGroup   = nullptr;

    //exint subscribeRatio = 64;
    //exint minGrainSize = 1024;
    
};// End of class GFE_GroupUnshared











//
//
//
//
//namespace GFE_GroupUnshared_Namespace {
//
//
//
//
//
//    static GA_Group*
//        groupUnshared(
//            GA_Detail* const geo,
//            const GA_VertexGroup* const geoVtxGroup,
//            const UT_StringRef& unsharedAttribName = "unshared",
//            const GA_StorageClass unsharedAttribStorageClass = GA_STORECLASS_REAL,
//            const GA_GroupType unsharedAttribClass = GA_GROUP_VERTEX,
//            const bool preFusePoint = false,
//            const GA_Storage inStorageI = GA_STORE_INVALID,
//            const bool outTopoAttrib = false,
//            const exint subscribeRatio = 64,
//            const exint minGrainSize = 64
//        )
//    {
//        UT_ASSERT_P(geo);
//
//        const GA_Storage finalStorageI = GFE_Type::getPreferredStorageI(geo);
//
//        //GA_VertexGroup* const unsharedGroup = GFE_VertexNextEquiv_Namespace::addGroupVertexNextEquiv(geo, geoVtxGroup, inStorageI, "__topo_unshared_SOP_FeE_GroupUnshared_1_0");
//        GA_VertexGroup* unsharedGroup = GFE_VertexNextEquiv_Namespace::addGroupVertexNextEquiv(geo, geoVtxGroup, inStorageI, "__topo_unshared", subscribeRatio, minGrainSize);
//        GA_Group* const unshared_promoGroup = GFE_GroupPromote::groupPromote(geo, unsharedGroup, unsharedAttribClass, unsharedAttribName, true);
//        return unshared_promoGroup;
//    }
//
//
//
//
//    static GA_Group*
//        groupUnshared(
//            const GA_Detail* const geo,
//            const GA_Group* const geoGroup,
//            const UT_StringRef& unsharedAttribName = "unshared",
//            const GA_StorageClass unsharedAttribStorageClass = GA_STORECLASS_REAL,
//            const GA_GroupType unsharedAttribClass = GA_GROUP_VERTEX,
//            const bool preFusePoint = false,
//            const GA_Storage inStorageI = GA_STORE_INVALID,
//            const bool outTopoAttrib = false,
//            const exint subscribeRatio = 64,
//            const exint minGrainSize = 64
//        )
//    {
//        UT_ASSERT_P(geo);
//
//        if (geoGroup && geoGroup->get == )
//        {
//            return groupUnshared(cookparms, geo, groupType, sopparms.getGroup()
//                geo0AttribNames
//                unsharedAttribStorageClass, unsharedAttribClass
//                preFusePoint, sopparms.getOutTopoAttrib(),
//                subscribeRatio, minGrainSize
//            );
//        }
//        const GA_VertexGroupUPtr geoVtxGroupUPtr = GFE_GroupPromote::groupPromoteVertexDetached(geo, geoGroup);
//        //GA_VertexGroup* geoVtxGroup = static_cast<GA_VertexGroup*>(geoVtxGroupUPtr.get());
//        const GA_VertexGroup* const geoVtxGroup = geoVtxGroupUPtr.get();
//
//
//        return groupUnshared(geo, geoVtxGroup,
//            geo0AttribNames
//            unsharedAttribStorageClass, unsharedAttribClass
//            preFusePoint, sopparms.getOutTopoAttrib(),
//            subscribeRatio, minGrainSize
//        );
//
//
//    }
//
//
//
//
//
//static GA_Group*
//    groupUnshared(
//    const SOP_NodeVerb::CookParms& cookparms,
//    const GA_Detail* const geo,
//    const GA_GroupType groupType,
//    const UT_StringRef& groupName,
//    const UT_StringRef& unsharedAttribName = "unshared",
//    const GA_StorageClass unsharedAttribStorageClass = GA_STORECLASS_REAL,
//    const GA_GroupType unsharedAttribClass = GA_GROUP_VERTEX,
//    const bool preFusePoint = false,
//    const GA_Storage inStorageI = GA_STORE_INVALID,
//    const bool outTopoAttrib = false,
//    const exint subscribeRatio = 64,
//    const exint minGrainSize = 64
//    )
//{
//    UT_ASSERT_P(geo);
//    UT_ASSERT_P(group);
//
//
//    GOP_Manager gop;
//    const GA_Group* const geoGroup = GFE_GroupParser_Namespace::findOrParseGroupDetached(cookparms, geo, groupType, sopparms.getGroup(), gop);
//    if (geoGroup && geoGroup->isEmpty())
//        return;
//
//    groupUnshared(cookparms, geo, groupType, sopparms.getGroup(),
//        geo0AttribNames,
//        unsharedAttribStorageClass, unsharedAttribClass
//        preFusePoint, sopparms.getOutTopoAttrib(),
//        subscribeRatio, minGrainSize
//    );
//
//
//    GFE_TopologyReference::outTopoAttrib(geo, outTopoAttrib);
//
//    if (geoGroup)
//    {
//        cookparms.getNode()->setHighlight(true);
//        cookparms.select(*unshared_promoGroup);
//    }
//    return unsharedGroup;
//}
//
//
//
//} // End of namespace GFE_GroupUnshared
//
//
//
//



#endif