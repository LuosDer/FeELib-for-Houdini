
#pragma once

#ifndef __GFE_Fuse_h__
#define __GFE_Fuse_h__

#include "GFE/GFE_Fuse.h"

#include "GFE/GFE_GeoFilter.h"


#define GFE_Fuse_UnderlyingAlgorithm_UseGU 0


#if GFE_Fuse_UnderlyingAlgorithm_UseGU
    #include "GU/GU_Snap.h"
#else
    #include "SOP/SOP_Fuse-2.0.proto.h"
    #define __TEMP_GFE_Fuse_GroupName      "__TEMP_GFE_Fuse_Group"
    #define __TEMP_GFE_Fuse_SnapGroupName  "__TEMP_GFE_Fuse_SnapGroup"
    #define __TEMP_GFE_Fuse_PosAttribName  "__TEMP_GFE_Fuse_P"
    #define __TEMP_GFE_Fuse_SpecifiedAttribName "__TEMP_GFE_Fuse_SnapSpecified"
    #define __TEMP_GFE_Fuse_SnapAttribName "__TEMP_GFE_Fuse_SnapTo"
#endif


class GFE_Fuse : public GFE_AttribCreateFilterWithRef0 {

    
public:
    
enum Method
{
    Point,
    Grid,
    Specified,
};
    
    //using GFE_AttribFilter::GFE_AttribFilter;
    
    GFE_Fuse(
        GA_Detail& geo,
        const GA_Detail* const geoRef,
        const SOP_NodeVerb::CookParms* const cookparms = nullptr
    )
        : GFE_AttribCreateFilterWithRef0(geo, geoRef, cookparms)
        , groupParserFuseRef(geoRef ? *geoRef : geo, groupParser.getGOPRef(), cookparms)
        , keepEdgeGroupArray(geo, cookparms)
    {
#if !GFE_Fuse_UnderlyingAlgorithm_UseGU
        fuseParms.setUsePositionSnapMethod(false);
        fuseParms.setRecomputenml(false);
        fuseParms.setAlgorithm(SOP_Fuse_2_0Enums::Algorithm::CLOSEST);
#endif
    }

    void
        setComputeParm(
            const Method method = Method::Point,
            const bool modifyTarget = false,
            const bool consolidate = false,
            const bool deleteConsolidated = true,
            const bool delUnusedPoint = false
        )
    {
        setHasComputed();
        
        this->method = method;
        
#if GFE_Fuse_UnderlyingAlgorithm_UseGU
        this->delUnusedPoint = delUnusedPoint;
        
        comSnapParm.myModifyBothQueryAndTarget = modifyTarget;
        comSnapParm.myConsolidate = consolidate;
        comSnapParm.myDeleteConsolidated = deleteConsolidated;
        
        pointSnapParm.myModifyBothQueryAndTarget = modifyTarget;
        pointSnapParm.myConsolidate = consolidate;
        pointSnapParm.myDeleteConsolidated = deleteConsolidated;
        
        gridSnapParm.myModifyBothQueryAndTarget = modifyTarget;
        gridSnapParm.myConsolidate = consolidate;
        gridSnapParm.myDeleteConsolidated = deleteConsolidated;
        
        specifiedSnapParm.myModifyBothQueryAndTarget = modifyTarget;
        specifiedSnapParm.myConsolidate = consolidate;
        specifiedSnapParm.myDeleteConsolidated = deleteConsolidated;
#else
        fuseParms.setModifyboth(modifyTarget);
        fuseParms.setConsolidateSnappedPoints(consolidate);
        fuseParms.setKeepConsolidatedPoints(!deleteConsolidated);
        fuseParms.setDelUnusedPoints(delUnusedPoint);
        
#endif
    }
    
#if GFE_Fuse_UnderlyingAlgorithm_UseGU
    SYS_FORCE_INLINE void setRecomputeNormal(bool b)
    { recomputeNormal = b; }
#else
    SYS_FORCE_INLINE void setRecomputeNormal(bool b)
    { fuseParms.setRecomputenml(b); }
#endif
        
    SYS_FORCE_INLINE void setKeepEdgeGroup(const UT_StringRef& names)
    { keepEdgeGroupArray.appends(GA_GROUP_EDGE, names); }
    
    
#if GFE_Fuse_UnderlyingAlgorithm_UseGU
    SYS_FORCE_INLINE void
        setPointComputeParm(
            const bool useDistance = false,
            const fpreal distance = 1e-05,
            const GU_Snap::PointSnapParms::SnapAlgorithm algorithm = GU_Snap::PointSnapParms::SnapAlgorithm::ALGORITHM_LOWEST_POINT
        )
    {
        setHasComputed();

        pointSnapParm.myDistance = useDistance ? distance : SYS_FP64_MAX;
        pointSnapParm.myAlgorithm = algorithm;
    }
    
#else
    SYS_FORCE_INLINE void
        setPointComputeParm(
            const bool useDistance = false,
            const fpreal distance = 1e-05,
            const SOP_Fuse_2_0Enums::Algorithm algorithm = SOP_Fuse_2_0Enums::Algorithm::CLOSEST
        )
    {
        setHasComputed();
        
        fuseParms.setUseTol3D(useDistance);
        fuseParms.setTol3d(distance);
        fuseParms.setAlgorithm(algorithm);
#endif
    }
    
    
#if GFE_Fuse_UnderlyingAlgorithm_UseGU
    
    void
        setGridComputeParm(
            const int gridSnapType,
            const float linex,
            const float liney,
            const float linez,
            const float offx,
            const float offy,
            const float offz,
            const float tol
        )
    {
        setHasComputed();
        
        gridSnapParm.myGridSnapType = gridSnapType;
        gridSnapParm.myXLines = linex;
        gridSnapParm.myYLines = liney;
        gridSnapParm.myZLines = linez;
        gridSnapParm.myXOff   = offx;
        gridSnapParm.myYOff   = offy;
        gridSnapParm.myZOff   = offz;
        gridSnapParm.myTol    = tol;
    }
    
    SYS_FORCE_INLINE void
        setGridComputeParm(
            const int gridSnapType,
            const UT_Vector3D line,
            const UT_Vector3D offset,
            const float tol
        )
    {
        setGridComputeParm(gridSnapType, line[0], line[1], line[2], offset[0], offset[1], offset[2], tol);
    }

#else

    
    void
        setGridComputeParm(
            const SOP_Fuse_2_0Enums::Gridtype gridSnapType,
            const SOP_Fuse_2_0Enums::Gridround gridRound,
            const UT_Vector3D space,
            const UT_Vector3D line,
            const UT_Vector3D pow2,
            const bool useTol,
            const fpreal tol
        )
    {
        fuseParms.setGridtype(gridSnapType);
        fuseParms.setGridround(gridRound);
        
        fuseParms.setGridspacing(space);
        fuseParms.setGridlines(line);
        fuseParms.setGridpow2(pow2);
        
        fuseParms.setUseGridTol(useTol);
        fuseParms.setGridtol(tol);
    }


        
#endif
    

    SYS_FORCE_INLINE void setSpecifiedComputeParm(const GA_Attribute& attrib)
    {
#if GFE_Fuse_UnderlyingAlgorithm_UseGU
        specifiedSnapParm.myTargetOwner = attrib.getOwner();
        specifiedSnapParm.myTargetElemAttrib = &attrib;
#else
        targetClass = attrib.getOwner();
        fuseParms.setTargetClass(specifiedTargetClass(targetClass));
        if (attrib.isDetached())
        {
            GFE_Attribute::clone(*geo, attrib, __TEMP_GFE_Fuse_SpecifiedAttribName);
            fuseParms.setTargetPtAttrib(__TEMP_GFE_Fuse_SpecifiedAttribName);
        }
        else
            fuseParms.setTargetPtAttrib(attrib.getName());

#endif
    }
    
    SYS_FORCE_INLINE void setSpecifiedComputeParm(const GA_Attribute* const attrib)
    {
#if GFE_Fuse_UnderlyingAlgorithm_UseGU
        if (attrib)
            setSpecifiedComputeParm(*attrib);
        else
            specifiedSnapParm.myTargetElemAttrib = nullptr;
#else
        if (attrib)
            setSpecifiedComputeParm(*attrib);
        else
            fuseParms.setTargetPtAttrib("");
#endif
    }
    
    SYS_FORCE_INLINE void setSpecifiedComputeParm(const GA_AttributeOwner owner, const UT_StringRef& name)
    {
#if GFE_Fuse_UnderlyingAlgorithm_UseGU
        specifiedSnapParm.myTargetOwner = owner;
        specifiedSnapParm.myTargetElemAttrib = geo->findAttribute(owner, name);
#else
        targetClass = owner;
        fuseParms.setTargetClass(specifiedTargetClass(owner));
        fuseParms.setTargetPtAttrib(name);
#endif
    }

    
    
    
#if !GFE_Fuse_UnderlyingAlgorithm_UseGU
    
private:
    SYS_FORCE_INLINE SOP_Fuse_2_0Enums::TargetClass specifiedTargetClass(const GA_AttributeOwner owner)
    {
        switch (owner)
        {
        case GA_ATTRIB_PRIMITIVE: return SOP_Fuse_2_0Enums::TargetClass::PRIM;   break;
        case GA_ATTRIB_POINT:     return SOP_Fuse_2_0Enums::TargetClass::POINT;  break;
        case GA_ATTRIB_VERTEX:    return SOP_Fuse_2_0Enums::TargetClass::VERTEX; break;
        case GA_ATTRIB_GLOBAL:    return SOP_Fuse_2_0Enums::TargetClass::DETAIL; break;
        default: UT_ASSERT_MSG(0, "Unhandled Attrib Owner"); break;
        }
        return SOP_Fuse_2_0Enums::TargetClass::POINT;
    }
public:
    
#endif



    
#if GFE_Fuse_UnderlyingAlgorithm_UseGU
    SYS_FORCE_INLINE void emplaceMergeAttribs(GU_Snap::AttributeMergeData && attribMergeData)
    {
        comSnapParm.myMergeAttribs.emplace_back(attribMergeData);
        
        pointSnapParm.    myMergeAttribs.emplace_back(attribMergeData);
        gridSnapParm.     myMergeAttribs.emplace_back(attribMergeData);
        specifiedSnapParm.myMergeAttribs.emplace_back(attribMergeData);
    }
#else
    SYS_FORCE_INLINE void emplaceMergeAttribs(SOP_Fuse_2_0Parms::Numpointattribs attrib)
    { MergeAttribs.emplace_back(attrib); }

    SYS_FORCE_INLINE void emplaceMergeAttribs(SOP_Fuse_2_0Parms::Numgroups group)
    { MergeGroups.emplace_back(group); }

#endif
    
#if GFE_Fuse_UnderlyingAlgorithm_UseGU
    SYS_FORCE_INLINE void setPosMergeMethod()
    { mergePos = false; }

    SYS_FORCE_INLINE void setPosMergeMethod(const GU_Snap::AttributeMergeMethod mergeMethod)
    { mergePos = true; posMergeMethod = mergeMethod; }
#else
    SYS_FORCE_INLINE void setPosMergeMethod()
    { fuseParms.setUsePositionSnapMethod(false); }

    SYS_FORCE_INLINE void setPosMergeMethod(const SOP_Fuse_2_0Enums::PositionSnapMethod mergeMethod)
    { fuseParms.setUsePositionSnapMethod(true); fuseParms.setPositionSnapMethod(mergeMethod); }

#endif
    


    
#if GFE_Fuse_UnderlyingAlgorithm_UseGU
    GA_PointGroup* setSnappedGroup(const bool detached = true, const UT_StringRef& name = "")
    {
        GA_PointGroup* const snappedGroup = findOrCreatePointGroup(detached, name);
        comSnapParm.myOutputGroup = snappedGroup;
        
        pointSnapParm.    myOutputGroup = snappedGroup;
        gridSnapParm.     myOutputGroup = snappedGroup;
        specifiedSnapParm.myOutputGroup = snappedGroup;
        return snappedGroup;
    }
        
    GA_Attribute* setSnapAttrib(const bool detached = true, const UT_StringRef& name = "")
    {
        GA_Attribute* const snapAttrib = getOutAttribArray().findOrCreateTuple(detached, GA_ATTRIB_POINT, GA_STORECLASS_INT, GA_STORE_INVALID, name);
        comSnapParm.myOutputAttribH = snapAttrib;
        
        pointSnapParm    .myOutputAttribH = snapAttrib;
        gridSnapParm     .myOutputAttribH = snapAttrib;
        specifiedSnapParm.myOutputAttribH = snapAttrib;
        return snapAttrib;
    }
#else
    SYS_FORCE_INLINE void setSnappedGroup()
    { fuseParms.setCreatesnappedgroup(false); }
        
    SYS_FORCE_INLINE void setSnapAttrib()
    { fuseParms.setCreatesnappedattrib(false); }
        
    SYS_FORCE_INLINE void setSnappedGroup(const UT_StringRef& name)
    {
        //snappedGroup = findOrCreatePointGroup(detached, name);
        fuseParms.setCreatesnappedgroup(true);
        fuseParms.setSnappedgroupname(name);
    }

    SYS_FORCE_INLINE void setSnapAttrib(const UT_StringRef& name)
    {
        //snapAttrib = getOutAttribArray().findOrCreateTuple(detached, GA_ATTRIB_POINT, GA_STORECLASS_INT, GA_STORE_INVALID, name);
        fuseParms.setCreatesnappedattrib(true);
        fuseParms.setSnappedattribname(name);
    }
#endif



#if GFE_Fuse_UnderlyingAlgorithm_UseGU
    
    GA_Attribute* setQRadiusAttrib(const UT_StringRef& name)
    {
        GA_Attribute* radiusAttrib = getInAttribArray().append(GA_ATTRIB_POINT, name);
        if (radiusAttrib && !radiusAttrib->getAIFTuple())
            radiusAttrib = nullptr;
        pointSnapParm.myQPscaleH = radiusAttrib;
        return radiusAttrib;
    }

    const GA_Attribute* setTRadiusAttrib(const UT_StringRef& name)
    {
        const GA_Attribute* radiusAttrib;
        if (geoRef0)
            radiusAttrib = getRef0AttribArray().append(GA_ATTRIB_POINT, name);
        else
            radiusAttrib = getInAttribArray().append(GA_ATTRIB_POINT, name);
        
        if (radiusAttrib && !radiusAttrib->getAIFTuple())
            radiusAttrib = nullptr;
        pointSnapParm.myTPscaleH = radiusAttrib;
        return radiusAttrib;
    }
        
    SYS_FORCE_INLINE void setRadiusAttrib(const UT_StringRef& name)
    {
        setQRadiusAttrib(name);
        setTRadiusAttrib(name);
    }

#else
        
    SYS_FORCE_INLINE void setQRadiusAttrib()
    { fuseParms.setUseradiusattrib(false); }

    SYS_FORCE_INLINE const GA_Attribute* setTRadiusAttrib()
    { fuseParms.setUseradiusattrib(false); }
        
    SYS_FORCE_INLINE void setQRadiusAttrib(const UT_StringRef& name)
    { fuseParms.setUseradiusattrib(true); fuseParms.setRadiusattrib(name); }

    SYS_FORCE_INLINE const GA_Attribute* setTRadiusAttrib(const UT_StringRef& name)
    { fuseParms.setUseradiusattrib(true); fuseParms.setRadiusattrib(name); }
        
        
    SYS_FORCE_INLINE void setRadiusAttrib()
    { fuseParms.setUseradiusattrib(false); }

    SYS_FORCE_INLINE void setRadiusAttrib(const UT_StringRef& name)
    { fuseParms.setUseradiusattrib(true); fuseParms.setRadiusattrib(name); }


#endif
    



#if GFE_Fuse_UnderlyingAlgorithm_UseGU
    
    
    GA_Attribute* setQMatchAttrib(const UT_StringRef& name)
    {
        GA_Attribute* matchAttrib = getInAttribArray().append(GA_ATTRIB_POINT, name);
        if (!matchAttrib)
        {
            pointSnapParm.myUseMatchAttrib = false;
            return matchAttrib;
        }
        const GA_AIFTuple* const aifTuple = matchAttrib->getAIFTuple();
        if (aifTuple)
        {
            switch (matchAttrib->getStorageClass())
            {
            case GA_STORECLASS_INT:   pointSnapParm.myQMatchIntH = matchAttrib; break;
            case GA_STORECLASS_FLOAT: pointSnapParm.myQMatchFltH = matchAttrib; break;
            }
        }
        else
        {
            const GA_AIFStringTuple* const aifStrTuple = matchAttrib->getAIFStringTuple();
            if (aifStrTuple)
            {
                pointSnapParm.myQMatchStrH = matchAttrib;
            }
            else
            {
                matchAttrib = nullptr;
            }
        }
        pointSnapParm.myUseMatchAttrib = matchAttrib;
        return matchAttrib;
    }
    
    const GA_Attribute* setTMatchAttrib(const UT_StringRef& name)
    {
        const GA_Attribute* matchAttrib = geoRef0 ? getRef0AttribArray().append(GA_ATTRIB_POINT, name) : getInAttribArray().append(GA_ATTRIB_POINT, name);
        if (!matchAttrib)
        {
            pointSnapParm.myUseMatchAttrib = false;
            return matchAttrib;
        }
        const GA_AIFTuple* const aifTuple = matchAttrib->getAIFTuple();
        if (aifTuple)
        {
            switch (matchAttrib->getStorageClass())
            {
            case GA_STORECLASS_INT:   pointSnapParm.myTMatchIntH = matchAttrib; break;
            case GA_STORECLASS_FLOAT: pointSnapParm.myTMatchFltH = matchAttrib; break;
            }
        }
        else
        {
            const GA_AIFStringTuple* const aifStrTuple = matchAttrib->getAIFStringTuple();
            if (aifStrTuple)
            {
                pointSnapParm.myTMatchStrH = matchAttrib;
            }
            else
            {
                matchAttrib = nullptr;
            }
        }
        pointSnapParm.myUseMatchAttrib = matchAttrib;
        return matchAttrib;
    }
    
    SYS_FORCE_INLINE void setMatchAttrib(const UT_StringRef& name)
    {
        setQMatchAttrib(name);
        setTMatchAttrib(name);
        pointSnapParm.myUseMatchAttrib &= bool(pointSnapParm.myQMatchStrH.getAttribute());
    }
    
#else
        
    SYS_FORCE_INLINE void setQMatchAttrib()
    { fuseParms.setUsematchattrib(false); }
    
    SYS_FORCE_INLINE void setQMatchAttrib(const UT_StringRef& name)
    {
        fuseParms.setUsematchattrib(true);
        fuseParms.setMatchattrib(name);
    }
    
    SYS_FORCE_INLINE void setTMatchAttrib()
    { fuseParms.setUsematchattrib(false); }
    
    SYS_FORCE_INLINE void setTMatchAttrib(const UT_StringRef& name)
    {
        fuseParms.setUsematchattrib(true);
        fuseParms.setMatchattrib(name);
    }
    
    SYS_FORCE_INLINE void setMatchAttrib()
    { fuseParms.setUsematchattrib(false); }
        
    SYS_FORCE_INLINE void setMatchAttrib(const UT_StringRef& name)
    {
        fuseParms.setUsematchattrib(true);
        fuseParms.setMatchattrib(name);
    }
    
    

#endif


    SYS_FORCE_INLINE void
        setPointMatchComputeParm(
            const fpreal tol = 0,
            const bool mismatch = false
        )
    {
        setHasComputed();
        
#if GFE_Fuse_UnderlyingAlgorithm_UseGU
        pointSnapParm.myMatchTol = tol;
        pointSnapParm.myMismatch = mismatch;
#else
        fuseParms.setMatchTol(tol);
        fuseParms.setMatchtype(mismatch ? SOP_Fuse_2_0Enums::Matchtype::MISMATCH : SOP_Fuse_2_0Enums::Matchtype::MATCH);
#endif
    }

        



    virtual void visualizeOutGroup() const override
    {
        if (doDelGroupElement)
            return;
        if (!keepEdgeGroupArray.isEmpty())
            visualizeGroup(keepEdgeGroupArray[0]);
        else if (!getOutGroupArray().isEmpty())
            visualizeGroup(getOutGroupArray()[0]);
    }







        








        
private:

        
virtual bool
    computeCore() override
{
    if (groupParser.isEmpty())
        return true;

    
    setValidPosAttrib();
    setValidPosRef0Attrib();
    

#if GFE_Fuse_UnderlyingAlgorithm_UseGU
    const bool keepEdgeGroup = comSnapParm.myConsolidate && !keepEdgeGroupArray.isEmpty();
    
    GA_AttributeUPtr posRestUPtr;
    GA_Attribute* posRest = posAttribNonConst;
    if (!geoRef0 && mergePos)
    {
        //posRestUPtr = GFE_Attribute::clone(*geo, *posAttrib);
        //posRest = posRestUPtr.get();
        
        posRest = GFE_Attribute::clone(*geo, *posAttrib, "abc");
    }
    
    GU_Detail& geoGU = *geo->asGU_Detail();

    comSnapParm.myQGroup = groupParser.getPointGroup();
    comSnapParm.myTGroup = groupParserFuseRef.getPointGroup();
    
    pointSnapParm    .myQGroup = groupParser.getPointGroup();
    gridSnapParm     .myQGroup = groupParser.getPointGroup();
    specifiedSnapParm.myQGroup = groupParser.getPointGroup();
    
    pointSnapParm    .myTGroup = groupParserFuseRef.getPointGroup();
    gridSnapParm     .myTGroup = groupParserFuseRef.getPointGroup();
    specifiedSnapParm.myTGroup = groupParserFuseRef.getPointGroup();
    
    if (keepEdgeGroup)
        fuseKeepEdgeGroup();
    else
        fuse();
    
    //posAttribNonConst->bumpDataId();
    
    if (delUnusedPoint)
        geo->destroyUnusedPoints();

#else
    const bool keepEdgeGroup = fuseParms.getConsolidateSnappedPoints() && !keepEdgeGroupArray.isEmpty();
    
    const GA_PointGroup* const geoGroup = groupParser.getPointGroup();
    GA_PointGroup* geoGroupRest = nullptr;
    if (geoGroup)
    {
        if (geoGroup->isDetached())
        {
            geoGroupRest = geo->newPointGroup(__TEMP_GFE_Fuse_SnapGroupName);
            geoGroupRest->combine(geoGroup);
            fuseParms.setQuerygroup(__TEMP_GFE_Fuse_SnapGroupName);
        }
        else
        {
            
            fuseParms.setQuerygroup(geoGroup->getName());
        }
    }
    else
    {
        fuseParms.setQuerygroup("");
    }

    
    GA_Attribute* posRest;
    if (posAttribNonConst->isDetached())
    {
        posRest = GFE_Attribute::clone(*geo, *posAttribNonConst, __TEMP_GFE_Fuse_PosAttribName);
        fuseParms.setPosAttrib(__TEMP_GFE_Fuse_PosAttribName);
    }
    else
    {
        fuseParms.setPosAttrib(posAttribNonConst->getName());
    }

    if (posAttribNonConst->isDetached())
    {
        GFE_Attribute::clone(*posAttribNonConst, *posRest);
    }


    GU_DetailHandle geoTmp_h;
    geoTmp = new GU_Detail();
    geoTmp_h.allocateAndSet(geoTmp);
    geoTmp->replaceWith(geoSrc ? *geoSrc : *geo);
        
    inputgdh.clear();
    inputgdh.emplace_back(geoTmp_h);

    GU_DetailHandle geoRef0Tmp_h;
    if (geoRef0)
    {
        GU_Detail* const geoRef0Tmp = new GU_Detail();
        geoRef0Tmp_h.allocateAndSet(geoRef0Tmp);
        geoRef0Tmp->replaceWith(*geoRef0);
        inputgdh.emplace_back(geoRef0Tmp_h);
    }
    else
    {
        inputgdh.emplace_back(geoRef0Tmp_h);
    }

        
    destgdh.allocateAndSet(geo->asGU_Detail(), false);


    fuseParms.setNumpointattribs(MergeAttribs);
    fuseParms.setNumgroups(MergeGroups);
    if (keepEdgeGroup)
        fuseKeepEdgeGroup();
    else
        fuse();
    
    if (geoGroupRest)
        geo->destroyGroup(geoGroupRest);
    
    if (method == Method::Specified)
        geo->destroyAttrib(geo->findAttribute(targetClass, __TEMP_GFE_Fuse_SpecifiedAttribName));
    
    
#endif

    
    return true;
}

#if GFE_Fuse_UnderlyingAlgorithm_UseGU
            
    void fuseKeepEdgeGroup()
    {
    }

            
    void fuse()
    {
        switch (method)
        {
        case Method::Point:
            if (mergePos)
            {
                emplaceMergeAttribs(GU_Snap::AttributeMergeData(posMergeMethod, geoRef0 ? posRef0Attrib : posAttribNonConst, posAttribNonConst));
                pointSnapParm.myQPosH = geoRef0 ? posAttribNonConst : posRest;
            }
            else
            {
                pointSnapParm.myQPosH = posAttribNonConst;
                
            }
            //pointSnapParm.myQPosH = mergePos ? posAttribNonConst : nullptr;
            pointSnapParm.myTPosH = geoRef0 ? posRef0Attrib : posRest;
            GU_Snap::snapPoints(geoGU, geoRef0->asGU_Detail(), pointSnapParm);
        break;
            
            
        case Method::Grid:
            gridSnapParm.myPosH = posAttribNonConst;
            GU_Snap::snapGrid(geoGU, gridSnapParm);
        break;
            
            
        case Method::Specified:
            specifiedSnapParm.myTargetOwner = GA_ATTRIB_POINT;
            if (mergePos)
                emplaceMergeAttribs(GU_Snap::AttributeMergeData(posMergeMethod, geoRef0 ? posRef0Attrib : posAttribNonConst, posAttribNonConst));
            
            GU_Snap::snapByAttrib(geoGU, geoRef0->asGU_Detail(), specifiedSnapParm);
        break;

            
        default: UT_ASSERT_MSG(0, "Unhandled Fuse Method");                             
        break;
        }
    }

#else
        
void fuseKeepEdgeGroup()
{
    UT_ASSERT_P(fuseParms.getConsolidateSnappedPoints());
    UT_ASSERT_P(!keepEdgeGroupArray.isEmpty());
    
    SOP_NodeCache* const nodeCache = fuseVerb->allocCache();
    
    SOP_Fuse_2_0Parms fuseParms1(fuseParms);
    fuseParms1.setRecomputenml(false);
    fuseParms1.setConsolidateSnappedPoints(false);
    fuseParms1.setCreatesnappedattrib(true);
    fuseParms1.setCreatesnappedgroup(false);
    fuseParms1.setAlgorithm(SOP_Fuse_2_0Parms::Algorithm::LOWEST);
    //fuseParms1.setSnappedgroupname();
    fuseParms1.setSnappedattribname(__TEMP_GFE_Fuse_SnapAttribName);

    const auto fuseCookparms1 = GFE_NodeVerb::newCookParms(cookparms, fuseParms1, nodeCache, &destgdh, &inputgdh);
    fuseVerb->cook(fuseCookparms1);






    
    GU_DetailHandle geoPreFuse_h;
    geoPreFuse = new GU_Detail;
    geoPreFuse_h.allocateAndSet(geoPreFuse);
    geoPreFuse->replaceWith(*geo);
    
    ::std::vector<GA_Offset> flagArray(geo->getNumPoints(), GFE_INVALID_OFFSET);
    
    GU_DetailHandle geo_h;
    geo_h.allocateAndSet(geo->asGU_Detail(), false);
    
    inputgdh.clear();
    inputgdh.emplace_back(geo_h);
    inputgdh.emplace_back(GU_ConstDetailHandle());
    
    SOP_Fuse_2_0Parms fuseParms2(fuseParms);
    fuseParms2.setSnaptype(SOP_Fuse_2_0Enums::Snaptype::SPECIFIED);
    //fuseParms2.setConsolidateSnappedPoints(true);
    fuseParms2.setUsePositionSnapMethod(false);
    fuseParms2.setTargetClass(SOP_Fuse_2_0Enums::TargetClass::POINT);
    fuseParms2.setTargetPtAttrib(__TEMP_GFE_Fuse_SnapAttribName);
    const auto fuseCookparms2 = GFE_NodeVerb::newCookParms(cookparms, fuseParms2, nodeCache, &destgdh, &inputgdh);
    
    fuseVerb->cook(fuseCookparms2);
    
    const GA_Attribute* const snapToAttrib    = geo->findPointAttribute(__TEMP_GFE_Fuse_SnapAttribName);
    const GA_Attribute* const snapToAttribRef = geoPreFuse->findPointAttribute(__TEMP_GFE_Fuse_SnapAttribName);
    const GA_ROHandleT<GA_Offset> snapTo_h(snapToAttrib);
    const GA_ROHandleT<GA_Offset> snapToRef_h(snapToAttribRef);
    
    GA_PageHandleT<GA_Offset, GA_Offset, true, false, const GA_Attribute, const GA_ATINumeric, const GA_Detail> snapTo_ph(snapToAttrib);
    for (GA_PageIterator pit = geo->getPointSplittableRange().beginPages(); !pit.atEnd(); ++pit)
    {
        GA_Offset start, end;
        for (GA_Iterator it(pit.begin()); it.blockAdvance(start, end); )
        {
            snapTo_ph.setPage(start);
            for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
            {
                flagArray[snapTo_ph.value(elemoff)] = elemoff;
            }
        }
    }
    
    const size_t numEdgeGroups = keepEdgeGroupArray.size();
    for (size_t i = 0; i < numEdgeGroups; ++i)
    {
        GA_EdgeGroup* const edgeGroup = keepEdgeGroupArray.getEdgeGroup(i);
        const GA_EdgeGroup* const refEdgeGroup = geoPreFuse->findEdgeGroup(edgeGroup->getName());
        if (!refEdgeGroup)
            continue;
        
        GA_Offset start, end;
        for (GA_EdgeGroup::const_iterator it = refEdgeGroup->begin(); !it.atEnd(); ++it)
        {
            const GA_Edge& edge = *it;
            GA_Offset p0 = snapToRef_h.get(edge.p0());
            if (GFE_Type::isInvalidOffset(p0))
                p0 = edge.p0();
            UT_ASSERT_P(p0 < flagArray.size());
            
            GA_Offset p1 = snapToRef_h.get(edge.p1());
            if (GFE_Type::isInvalidOffset(p1))
                p1 = edge.p1();
            UT_ASSERT_P(p1 < flagArray.size());
            edgeGroup->add(flagArray[p0], flagArray[p1]);
        }
    }
    geo->destroyAttrib(geo->findAttribute(GA_ATTRIB_POINT, __TEMP_GFE_Fuse_SnapAttribName));

    
}

        
void fuse()
{
    SOP_NodeCache* const nodeCache = fuseVerb->allocCache();
    const auto fuseCookparms = GFE_NodeVerb::newCookParms(cookparms, fuseParms, nodeCache, &destgdh, &inputgdh);
    fuseVerb->cook(fuseCookparms);
}

#endif


    
public:
    
    GFE_GroupParser groupParserFuseRef;

    
    Method method = Method::Point;
    bool mergePos = false;
    bool delUnusedPoint = false;
    
    #if GFE_Fuse_UnderlyingAlgorithm_UseGU
        bool recomputeNormal = false;
        GU_Snap::AttributeMergeMethod posMergeMethod = GU_Snap::AttributeMergeMethod::MERGE_ATTRIBUTE_MEAN;
    #else
        SOP_Fuse_2_0Parms fuseParms;
    #endif

private:
    GFE_GroupArray keepEdgeGroupArray;
        
    GU_Detail* geoPreFuse = nullptr;
    
    #if GFE_Fuse_UnderlyingAlgorithm_UseGU
        GU_Snap::CommonSnapParms comSnapParm;
        GU_Snap::PointSnapParms  pointSnapParm;
        GU_Snap::GridSnapParms   gridSnapParm;
        GU_Snap::AttribSnapParms specifiedSnapParm;
    #else
        GA_AttributeOwner targetClass;
        GU_Detail* geoTmp = nullptr;
    #endif
        
    //const GA_PointGroup* pointGroup;
    
    //exint subscribeRatio = 64;
    //exint minGrainSize   = 1024;

private:

    
#if !GFE_Fuse_UnderlyingAlgorithm_UseGU
    GA_PointGroup* snappedGroup;
    GA_Attribute* snapAttrib;
        
    GU_DetailHandle destgdh;
    UT_Array<GU_ConstDetailHandle> inputgdh;
    const SOP_NodeVerb* const fuseVerb = SOP_NodeVerb::lookupVerb("fuse::2.0");
    
    UT_Array<SOP_Fuse_2_0Parms::Numpointattribs> MergeAttribs;
    UT_Array<SOP_Fuse_2_0Parms::Numgroups> MergeGroups;
    
    #undef __TEMP_GFE_Fuse_GroupName
    #undef __TEMP_GFE_Fuse_PosAttribName
    #undef __TEMP_GFE_Fuse_SpecifiedAttribName
    #undef __TEMP_GFE_Fuse_SnapAttribName
    #undef __TEMP_GFE_Fuse_SnapGroupName
#endif

}; // End of class GFE_Fuse


//#undef GFE_Fuse_UnderlyingAlgorithm_UseGU





#endif
