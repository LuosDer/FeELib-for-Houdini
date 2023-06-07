
#pragma once

#ifndef __GFE_Triangulate2D_h__
#define __GFE_Triangulate2D_h__

#include "GFE/GFE_Triangulate2D.h"



#include "GFE/GFE_GeoFilter.h"


//#include "GFE/GFE_AttributeCopy.h"

#include "GFE/GFE_RestVectorComponent.h"
#include "GFE/GFE_GroupElementByDirection.h"
#include "GFE/GFE_GroupUnshared.h"

//#include "SOP/SOP_Triangulate2D-3.0.h"
#include "SOP/SOP_Triangulate2D-3.0.proto.h"

class GFE_Triangulate2D : public GFE_AttribFilter {

public:
    using GFE_AttribFilter::GFE_AttribFilter;

    ~GFE_Triangulate2D()
    {
    }

    
    void
        setComputeParm(
        
            const bool preDelSharedEdge = false,
            const bool keepUnsharedSilhouette = false,
            const bool keepOutsidePrim = false,
            const bool keepHeight = false
            //,const exint subscribeRatio = 64,
            //const exint minGrainSize = 64
        )
    {
        setHasComputed();
        this->preDelSharedEdge       = preDelSharedEdge;
        this->keepUnsharedSilhouette = keepUnsharedSilhouette;
        this->keepOutsidePrim        = keepOutsidePrim;
        this->keepHeight             = keepHeight;
        
        // this->subscribeRatio = subscribeRatio;
        // this->minGrainSize   = minGrainSize;
    }

    
    SYS_FORCE_INLINE void setPreFuse(const fpreal fuseDist)
    {
        preFuse = true;
        preFuseDist = fuseDist;
    }
    SYS_FORCE_INLINE void setPreFuse()
    { preFuse = false; }


    
    SYS_FORCE_INLINE void setPostFuse(const fpreal fuseDist)
    {
        postFuse = true;
        postFuseDist = fuseDist;
    }
    SYS_FORCE_INLINE void setPostFuse()
    { postFuse = false; }


    
    SYS_FORCE_INLINE void setKeepSingleSide(const bool reverse)
    {
        keepSingleSide = true;
        reverseSide = reverse;
    }
    SYS_FORCE_INLINE void setKeepSingleSide()
    { keepSingleSide = false; }


    SYS_FORCE_INLINE void setDir(const GFE_Axis inAxis)
    {
        UT_ASSERT_MSG(inAxis != GFE_Axis::Invalid, "cant handled axis");
        axis = inAxis;
        dir = GFE_Type::axisDirD(inAxis);
    }
    
    SYS_FORCE_INLINE void setDir(const UT_Vector3D& inDir)
    {
        axis = GFE_Axis::Invalid;
        dir = inDir;
    }
    
private:

    
    virtual bool
        computeCore() override
    {
        UT_ASSERT_MSG_P(cookparms, "must have cookparms");
        if (!cookparms)
            return false;
        
        // if (groupParser.isEmpty())
        //     return true;
        
        GU_Snap::PointSnapParms fuseParms;
        if (preFuse || postFuse)
        {
            fuseParms.myAlgorithm = GU_Snap::PointSnapParms::SnapAlgorithm::ALGORITHM_CLOSEST_POINT;
            fuseParms.myConsolidate = true;
            fuseParms.myDeleteConsolidated = true;
            fuseParms.myQPosH = geo->getP();
            fuseParms.myTPosH = geo->getP();
        }


        
        geo->keepStdAttribute("*", "", "", "");
        geo->keepStdGroup("*", "", "", "");
        
        if (keepSingleSide)
        {
            GFE_GroupElemByDir groupByDir(geo, nullptr, cookparms);
            groupByDir.up = dir;
            groupByDir.doDelGroupElement = true;
            groupByDir.setGroup.setComputeParm(reverseSide);
            groupByDir.findOrCreateGroup(true, GA_GROUP_PRIMITIVE);
            //groupByDir.normal3D.findOrCreateNormal3D(true, outGroupType, GA_STORE_INVALID, sopparms.getDirAttrib());
        }
        
        if (preFuse)
        {
            fuseParms.myDistance = preFuseDist;
            GU_Snap::snapPoints(*geo, nullptr, fuseParms);
        }
        
        if (preDelSharedEdge)
        {
            
            GFE_GroupUnshared groupUnshared(geo, cookparms);
            groupUnshared.doDelGroupElement = true;
        }
        if (keepHeight)
        {
            GFE_RestVectorComponent restVectorComponent(geo, cookparms);
            restVectorComponent.comp = axis;
            restVectorComponent.setRestAttrib(geo->getP());
            restVectorComponent.newAttribNames = "height";
            restVectorComponent.compute();
        }

        if (keepUnsharedSilhouette)
        {
            
            GFE_GroupElemByDir groupByDir(geo, nullptr, cookparms);
            groupByDir.up = dir;
            groupByDir.doDelGroupElement = true;
    
            groupByDir.setGroup.setComputeParm(sopparms.getReverseGroup());
    
            groupByDir.normal3D.findOrCreateNormal3D(true, outGroupType, GA_STORE_INVALID, sopparms.getDirAttrib());
            groupByDir.findOrCreateGroup(false, outGroupType, sopparms.getOutGroupName());
    
            
        }
        if (method == GFE_GroupElemByDirMethod::RestDir2D_AvgNormal)
            groupByDir.restDir2D.normal3D.findOrCreateNormal3D(true, normalSearchOrder, GA_STORE_INVALID, sopparms.getNormal3DAttrib());

        if (sopparms.getMatchUpDir())
            groupByDir.restDir2D.setMatchUpDir(sopparms.getUp());
        else
            groupByDir.restDir2D.setMatchUpDir();

    
        if (postFuse)
        {
            fuseParms.myDistance = postFuseDist;
            GU_Snap::snapPoints(*geo, nullptr, fuseParms);
        }

        groupByDir.groupParser.setGroup(groupType, sopparms.getGroup());
    
        groupByDir.computeAndBumpDataId();
        groupByDir.visualizeOutGroup();

        {
            UT_Array<GU_ConstDetailHandle> nodeInputs;
        
            const size_t nInputs = cookparms->nInputs();
            for (size_t i = 0; i < nInputs; ++i)
            {
                nodeInputs.emplace_back(cookparms->inputGeoHandle(i));
            }

            SOP_Node* const node = cookparms->getNode();
            const SOP_CookEngine cookEngine = node == nullptr ? SOP_COOK_COMPILED : SOP_COOK_TRADITIONAL;
        
            SOP_Triangulate2D_3_0Parms tri2DParms;
            tri2DParms.setConstrSplitPtsGrp(dir);
            tri2DParms.setDir(dir);
            SOP_NodeCache* nodeCache = nullptr;
        
            const SOP_NodeVerb::CookParms tri2dCookparms(cookparms->gdh(), nodeInputs, cookEngine, node, cookparms->getContext(),
                  &tri2DParms, nodeCache, cookparms->error(), cookparms->depnode());

            tri2dVerb->cook(tri2dCookparms);
        }

        if (keepHeight)
        {
            geo->
        }
        
        return true;
    }


    void triangulate2D(const GA_PrimitiveGroup* const geoGroup)
    {
        //UT_ASSERT_P(geoGroup);
        
    }



public:
    bool preFuse = true;
    bool postFuse = false;
    GFE_Axis axis = GFE_Axis::Y;
    UT_Vector3D dir = UT_Vector3D(0,1,0);

    bool keepSingleSide = false;
    bool reverseSide = false;
    
public:
    fpreal preFuseDist = 1e-03;
    fpreal postFuseDist = 1e-03;
    
        
    bool preDelSharedEdge = false;
    bool keepUnsharedSilhouette = false;
    bool keepOutsidePrim = false;
    bool keepHeight = false;

    
private:
    GA_PointGroup* oneNebPointGroup = nullptr;
    
    GU_DetailHandle geoRef0_h;
    GU_Detail* geoRef0Tmp;
    
    const SOP_NodeVerb* const tri2dVerb = SOP_NodeVerb::lookupVerb("triangulate2d::3.0");
    
    // exint subscribeRatio = 64;
    // exint minGrainSize = 256;

}; // End of class GFE_Triangulate2D





#endif
