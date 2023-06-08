
#pragma once

#ifndef __GFE_Triangulate2D_h__
#define __GFE_Triangulate2D_h__

#include "GFE/GFE_Triangulate2D.h"



#include "GFE/GFE_GeoFilter.h"


#include "GFE/GFE_NodeVerb.h"

//#include "GFE/GFE_AttributeCopy.h"

#include "GFE/GFE_RestVectorComponent.h"
#include "GFE/GFE_SetVectorComponent.h"
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
            GFE_GroupElemByDir groupElemByDir(geo, nullptr, cookparms);
            groupElemByDir.up = dir;
            groupElemByDir.doDelGroupElement = true;
            groupElemByDir.setGroup.setComputeParm(reverseSide);
            groupElemByDir.findOrCreateGroup(true, GA_GROUP_PRIMITIVE);
            //groupElemByDir.normal3D.findOrCreateNormal3D(true, outGroupType, GA_STORE_INVALID, sopparms.getDirAttrib());
        }
        
        if (preFuse)
        {
            fuseParms.myDistance = preFuseDist;
            GU_Snap::snapPoints(*geo->asGU_Detail(), nullptr, fuseParms);
        }
        
        if (preDelSharedEdge)
        {
            
            GFE_GroupUnshared groupUnshared(geo, cookparms);
            groupUnshared.doDelGroupElement = true;
        }
        
        GA_Attribute* restAttrib;
        if (keepHeight && axis != GFE_Axis::Invalid)
        {
            GFE_RestVectorComponent restVectorComponent(geo, nullptr, cookparms);
            restVectorComponent.comp = static_cast<int8>(axis);
            restVectorComponent.setRestAttrib(geo->getP());
            //restVectorComponent.newAttribNames = "height";
            restVectorComponent.compute();
            
            restAttrib = restVectorComponent.getOutAttribArray()[0];
        }

        if (keepUnsharedSilhouette)
        {
            
            GFE_GroupElemByDir groupElemByDir(geo, nullptr, cookparms);
            groupElemByDir.up = dir;
            groupElemByDir.doDelGroupElement = true;
    
            groupElemByDir.setGroup.setParm(reverseSide);
    
            groupElemByDir.normal3D.findOrCreateNormal3D(true, GFE_NormalSearchOrder::PRIMITIVE, GA_STORE_INVALID);
            groupElemByDir.findOrCreateGroup(true, GA_GROUP_PRIMITIVE);
            
            groupElemByDir.groupParser.setGroup(groupParser);
    
            groupElemByDir.compute();
        }
        
        triangulate2D();

        if (postFuse)
        {
            fuseParms.myDistance = postFuseDist;
            GU_Snap::snapPoints(*geo->asGU_Detail(), nullptr, fuseParms);
        }
        
        if (keepHeight && axis != GFE_Axis::Invalid)
        {
            GFE_SetVectorComponent setVectorComponent(geo, nullptr, cookparms);
    
            setVectorComponent.groupParser.setGroup(groupParser);
            setVectorComponent.comp = static_cast<int8>(axis);
            
            setVectorComponent.getOutAttribArray().set(geo->getP());

            setVectorComponent.setRefAttrib(restAttrib);
    
            setVectorComponent.compute();
        }
        
        return true;
    }


    void triangulate2D()
    {
        //tri2DParms.setConstrSplitPtsGrp(dir);
        tri2DParms.setDir(dir);
            
        GU_DetailHandle geoTmp_h;
        geoTmp = new GU_Detail;
        geoTmp_h.allocateAndSet(geoTmp);
        geoTmp->replaceWith(*geo);

        inputgdh.clear();
        inputgdh.emplace_back(geoTmp_h);
        
        geo->clear();
        
        const auto tri2dCookparms = GFE_NodeVerb::newCookParms(cookparms, tri2DParms, nullptr, &inputgdh);
        tri2dVerb->cook(tri2dCookparms);
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
    
    GU_DetailHandle geoTmp_h;
    GU_Detail* geoTmp;
    
    UT_Array<GU_ConstDetailHandle> inputgdh;
    SOP_Triangulate2D_3_0Parms tri2DParms;
    const SOP_NodeVerb* const tri2dVerb = SOP_NodeVerb::lookupVerb("triangulate2d::3.0");
    
    // exint subscribeRatio = 64;
    // exint minGrainSize = 256;

}; // End of class GFE_Triangulate2D





#endif
