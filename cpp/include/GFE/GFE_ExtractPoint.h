
#pragma once

#ifndef __GFE_ExtractPoint_h__
#define __GFE_ExtractPoint_h__


#include "GFE/GFE_ExtractPoint.h"



#include "GFE/GFE_GeoFilter.h"



class GFE_ExtractPoint : public GFE_GeoFilter {


public:

    using GFE_GeoFilter::GFE_GeoFilter;
    //
    // GFE_ExtractPoint(
    //     GA_Detail& geo,
    //     const GA_Detail* const geoSrc = nullptr,
    //     const SOP_NodeVerb::CookParms* const cookparms = nullptr
    // )
    //     : GFE_GeoFilter(geo, cookparms)
    //     , geoSrc(geoSrc)
    // {
    // }
    //
    // GFE_ExtractPoint(
    //     GA_Detail& geo,
    //     const GA_Detail& geoSrc,
    //     const SOP_NodeVerb::CookParms* const cookparms = nullptr
    // )
    //     : GFE_GeoFilter(geo, cookparms)
    //     , geoSrc(&geoSrc)
    // {
    // }


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

    SYS_FORCE_INLINE void setKernel(const char kernel)
    { this->kernel = kernel; }
    



    void setGroup()
    {
        pointGroup = groupParser.getPointGroup();
        if (pointGroup && groupParser.isPointGroup() && !pointGroup->isDetached())
        {
            pointGroupNonConst = geo->findPointGroup(pointGroup->getName());
        }

        hasSetGroup = true;
    }

    void setGroup(const GA_GroupType groupType, const UT_StringRef& groupName)
    {
        groupParser.setGroup(groupType, groupName);

        if (groupType == GA_GROUP_POINT)
        {
            pointGroupNonConst = geo->findPointGroup(groupName);
            pointGroup = pointGroupNonConst;
        }
        else
        {
            pointGroup = groupParser.getPointGroup();
        }

        hasSetGroup = true;
    }

    SYS_FORCE_INLINE void setGroup(const UT_StringRef& groupName)
    { setGroup(GA_GROUP_POINT, groupName); }


    SYS_FORCE_INLINE virtual void bumpDataIdsForAddOrRemove() const override
    { geo->bumpDataIdsForAddOrRemove(true, false, false); }
    
    
private:

    virtual bool
        computeCore() override
    {
        if (groupParser.isEmpty())
            return true;

        if (!hasSetGroup)
            setGroup();

        geo->delStdAttribute(delPrimAttrib, delPointAttrib, delVertexAttrib, delDetailAttrib);

        if (geoSrc)
        {
            geo->replaceWithPoints(*geoSrc);
        }
        else
        {
            if (pointGroup)
            {
                geo->delStdGroup(delPrimGroup, "", delVertexGroup, delEdgeGroup);

                GA_GroupTable& groupTable = *geo->getGroupTable(GA_GROUP_POINT);
                for (GA_GroupTable::iterator<GA_Group> it = groupTable.beginTraverse(); !it.atEnd(); ++it)
                {
                    GA_PointGroup* const geoGroup = static_cast<GA_PointGroup*>(it.group());
                    //if (geoGroup->getName() == group->getName())
                    if (geoGroup == group)
                        continue;
                    if (geoGroup->getName().match(delPointGroup))
                        continue;
                    groupTable.destroy(geoGroup);
                }
            }
            else
            {
                geo->delStdGroup(delPrimGroup, delPointGroup, delVertexGroup, delEdgeGroup);
            }
        }


        if (geoSrc)
        {
            extractPointWithSource();
        }
        else
        {
            extractPoint();
        }

        if (delInputGroup && pointGroupNonConst)
        {
            geo->destroyGroup(pointGroupNonConst);
        }
        return true;
    }

    // can not use in parallel unless for each GA_Detail
    void extractPointWithSource()
    {
        if (pointGroup)
        {
            if (pointGroup->isEmpty())
            {
                if (reverseGroup)
                {
                    return;
                }
                else
                {
                    geo->clearElement();
                    return;
                }
            }
            else if (pointGroup->entries() == geo->getNumPoints())
            {
                if (reverseGroup)
                {
                    geo->clearElement();
                    return;
                }
                else
                {
                    return;
                }
            }
        }
        else
        {
            if (reverseGroup)
            {
                return;
            }
            else
            {
                geo->clearElement();
                return;
            }
        }

        switch (kernel)
        {
        case 0:
            geo->clear();
            //geo->asGEO_Detail()->mergePoints(*srcGeo, pointGroup, false, false);
            geo->asGEO_Detail()->mergePoints(static_cast<const GEO_Detail&>(*geoSrc), GA_Range(geoSrc->getPointMap(), pointGroup, reverseGroup));
            break;
        case 1:
            geo->destroyPointOffsets(GA_Range(geo->getPointMap(), pointGroup, !reverseGroup), GA_Detail::GA_DestroyPointMode::GA_LEAVE_PRIMITIVES, true);
            break;
        case 2:
        {
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
                    geo->destroyPointOffsets(GA_Range(geo->getPointMap(), pointGroup, !reverseGroup), GA_Detail::GA_DestroyPointMode::GA_LEAVE_PRIMITIVES, true);
                }
                else
                {
                    geo->clearElement();
                    geo->asGEO_Detail()->mergePoints(*static_cast<const GEO_Detail*>(geoSrc), pointGroup, false, false);
                }
            }
            else
            {
                if (pointGroup->entries() >= geo->getNumPoints() - 4096)
                {
                    geo->destroyPointOffsets(GA_Range(geo->getPointMap(), pointGroup, !reverseGroup), GA_Detail::GA_DestroyPointMode::GA_LEAVE_PRIMITIVES, true);
                }
                else
                {
                    geo->clearElement();
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
        if (pointGroup)
        {
            if (pointGroup->isEmpty())
            {
                if (reverseGroup)
                {
                    return;
                }
                else
                {
                    geo->clearElement();
                    return;
                }
            }
            else if (pointGroup->entries() == geo->getNumPoints())
            {
                if (reverseGroup)
                {
                    geo->clearElement();
                    return;
                }
                else
                {
                    return;
                }
            }
        }
        else
        {
            if (reverseGroup)
            {
                return;
            }
            else
            {
                geo->clearElement();
                return;
            }
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
                    //geo->merge();
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

    const GA_PointGroup* pointGroup = nullptr;
    GA_PointGroup* pointGroupNonConst = nullptr;
    bool hasSetGroup = false;

    char kernel = 0;

    //exint subscribeRatio = 64;
    //exint minGrainSize = 1024;


}; // End of class GFE_ExtractPoint





#endif
