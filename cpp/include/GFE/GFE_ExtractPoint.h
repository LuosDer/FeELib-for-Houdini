
#pragma once

#ifndef __GFE_ExtractPoint_h__
#define __GFE_ExtractPoint_h__


#include "GFE/GFE_ExtractPoint.h"



#include "GFE/GFE_GeoFilter.h"



class GFE_ExtractPoint : public GFE_GeoFilter {

public:
    //using GFE_GeoFilter::GFE_GeoFilter;
    
    GFE_ExtractPoint(
        GFE_Detail* const geo,
        const GA_Detail* const geoSrc = nullptr,
        const SOP_NodeVerb::CookParms* const cookparms = nullptr
    )
        : GFE_GeoFilter(geo, geoSrc, cookparms)
        , groupParserIn0(geoSrc ? geoSrc : static_cast<const GA_Detail*>(geo), groupParser.getGOPRef(), cookparms)
    {
    }
	
    GFE_ExtractPoint(
        GA_Detail& geo,
        const GA_Detail* const geoSrc = nullptr,
        const SOP_NodeVerb::CookParms* const cookparms = nullptr
    )
        : GFE_GeoFilter(geo, geoSrc, cookparms)
        , groupParserIn0(geoSrc ? geoSrc : &geo, groupParser.getGOPRef(), cookparms)
    {
    }
	

    void
        setComputeParm(
            const bool reverseGroup = false,
            const bool delInputGroup = true
            //,const exint subscribeRatio = 64,
            //const exint minGrainSize = 64
        )
    {
        setHasComputed();
        this->reverseGroup = reverseGroup;
        this->delInputGroup = delInputGroup;
        //this->subscribeRatio = subscribeRatio;
        //this->minGrainSize = minGrainSize;
    }
    
    void setDeleteParm(
        const GA_Detail::GA_DestroyPointMode delPointMode = GA_Detail::GA_DESTROY_DEGENERATE_INCOMPATIBLE,
        const bool guaranteeNoVertexReference = false
    )
    {
        this->delPointMode = delPointMode;
        this->guaranteeNoVertexReference = guaranteeNoVertexReference;
    }

    SYS_FORCE_INLINE void setKernel(const uint8 kernel)
    { this->kernel = kernel; }
    

    SYS_FORCE_INLINE void setGroup(const GA_GroupType groupType, const UT_StringRef& groupName)
    { groupParserIn0.setGroup(groupType, groupName); }

    SYS_FORCE_INLINE void setGroup(const UT_StringRef& groupName)
    { setGroup(GA_GROUP_POINT, groupName); }


    SYS_FORCE_INLINE virtual void bumpDataIdsForAddOrRemove() const override
    { geo->bumpDataIdsForAddOrRemove(true, false, false); }
    
    
private:

    virtual bool
        computeCore() override
    {
        pointGroup = groupParserIn0.getPointGroup();
        if (geoSrc)
            extractPointWithSource();
        else
            extractPoint();
        
        if (delInputGroup)
            groupParserIn0.delGroup();
        
        //if (delInputGroup && pointGroupNonConst)
        //    geo->destroyGroup(pointGroupNonConst);
        
        return true;
    }

    void extractPointWithSource()
    {
        if (!pointGroup)
        {
            if (reverseGroup)
                geo->clearElement();
            else
                geo->replaceWithPoints(*geoSrc);
            return;
        }
        if (pointGroup->isEmpty())
        {
            if (reverseGroup)
                geo->replaceWithPoints(*geoSrc);
            else
                geo->clearElement();
            return;
        }
        if (pointGroup->entries() == pointGroup->getIndexMap().indexSize())
        {
            if (reverseGroup)
                geo->clearElement();
            else
                geo->replaceWithPoints(*geoSrc);
            return;
        }
        
        switch (kernel)
        {
        case 0:
            //geo->asGEO_Detail()->mergePoints(*srcGeo, pointGroup, false, false);
            geo->asGEO_Detail()->mergePoints(static_cast<const GEO_Detail&>(*geoSrc), GA_Range(geoSrc->getPointMap(), pointGroup, reverseGroup));
            break;
        case 1:
            geo->replaceWithPoints(*geoSrc);
            geo->destroyPointOffsets(GA_Range(geo->getPointMap(), pointGroup, !reverseGroup), GA_Detail::GA_DestroyPointMode::GA_LEAVE_PRIMITIVES, true);
            break;
        case 2:
        {
            geo->replaceWithPoints(*geoSrc);
            GA_OffsetList offList = geo->getOffsetList(pointGroup, !reverseGroup);
            GA_IndexMap& ptmap = geo->getIndexMap(GA_ATTRIB_POINT);

            for (GA_Size arrayi = 0; arrayi < offList.size(); ++arrayi)
            {
                ptmap.destroyOffset(offList[arrayi]);
            }
        }
            break;
        case 3:
        {
            if (reverseGroup)
            {
                if (pointGroup->entries() <= 4096)
                {
                    geo->replaceWithPoints(*geoSrc);
                    geo->destroyPointOffsets(GA_Range(geo->getPointMap(), pointGroup, !reverseGroup), GA_Detail::GA_DestroyPointMode::GA_LEAVE_PRIMITIVES, true);
                }
                else
                {
                    //geo->clearElement();
                    geo->asGEO_Detail()->mergePoints(*static_cast<const GEO_Detail*>(geoSrc), pointGroup, false, false);
                }
            }
            else
            {
                if (pointGroup->entries() >= geo->getNumPoints() - 4096)
                {
                    geo->replaceWithPoints(*geoSrc);
                    geo->destroyPointOffsets(GA_Range(geo->getPointMap(), pointGroup, !reverseGroup), GA_Detail::GA_DestroyPointMode::GA_LEAVE_PRIMITIVES, true);
                }
                else
                {
                    //geo->clearElement();
                    geo->asGEO_Detail()->mergePoints(*static_cast<const GEO_Detail*>(geoSrc), pointGroup, false, false);
                }
            }
        }
            break;
        default:
            break;
        }
        //geo->destroyPointOffsets(GA_Range(geo->getPointMap(), pointGroup), GA_Detail::GA_DestroyPointMode::GA_LEAVE_PRIMITIVES, true);
        //geo->destroyUnusedPoints(pointGroup);
    }



    // can not use in parallel unless for each GA_Detail
    void extractPoint()
    {
        if (!pointGroup)
        {
            if (reverseGroup)
                geo->clearElement();
            // else
            //     geo->replaceWithPoints(*geo);
            return;
        }
        if (pointGroup->isEmpty())
        {
            if (!reverseGroup)
                geo->clearElement();
            // else
            //     geo->replaceWithPoints(*geo);
            return;
        }
        if (pointGroup->entries() == pointGroup->getIndexMap().indexSize())
        {
            if (reverseGroup)
                geo->clearElement();
            // else
            //     geo->replaceWithPoints(*geo);
            return;
        }
        
        switch (kernel)
        {
        case 0:
            geo->destroyPointOffsets(GA_Range(geo->getPointMap(), pointGroup, !reverseGroup), delPointMode, guaranteeNoVertexReference);
        break;
        case 1:
        {
            GA_OffsetList offList = geo->getOffsetList(pointGroup, !reverseGroup);
            GA_IndexMap& ptmap = geo->getIndexMap(GA_ATTRIB_POINT);

            for (GA_Size arrayi = 0; arrayi < offList.size(); ++arrayi)
            {
                ptmap.destroyOffset(offList[arrayi]);
            }
        }
        break;
        case 2:
        {
            if (reverseGroup)
            {
                if (pointGroup->entries() <= 4096)
                {
                    geo->destroyPointOffsets(GA_Range(geo->getPointMap(), pointGroup, !reverseGroup), delPointMode, guaranteeNoVertexReference);
                }
                else
                {
                    //geo->asGEO_Detail()->mergePoints(static_cast<const GEO_Detail&>(*geo->asGEO_Detail()), pointGroup, false, false);
                }
            }
            else
            {
                if (pointGroup->entries() >= geo->getNumPoints() - 4096)
                {
                    geo->destroyPointOffsets(GA_Range(geo->getPointMap(), pointGroup, !reverseGroup), delPointMode, guaranteeNoVertexReference);
                }
                else
                {
                    //geo->asGEO_Detail()->mergePoints(static_cast<const GEO_Detail&>(*geo->asGEO_Detail()), pointGroup, false, false);
                }
            }
        }
            break;
        default:
            break;
        }
        //geo->destroyPointOffsets(GA_Range(geo->getPointMap(), pointGroup), GA_Detail::GA_DestroyPointMode::GA_LEAVE_PRIMITIVES, true);
        //geo->destroyUnusedPoints(pointGroup);
    }


public:
    bool reverseGroup = false;
    bool delInputGroup = true;
    
    GA_Detail::GA_DestroyPointMode delPointMode = GA_Detail::GA_DESTROY_DEGENERATE_INCOMPATIBLE;
    bool guaranteeNoVertexReference = false;

private:
    GFE_GroupParser groupParserIn0;

    const GA_PointGroup* pointGroup;
    //GA_PointGroup* pointGroupNonConst = nullptr;
    //bool hasSetGroup = false;

    uint8 kernel = 0;

    //exint subscribeRatio = 64;
    //exint minGrainSize = 1024;


}; // End of class GFE_ExtractPoint





#endif
