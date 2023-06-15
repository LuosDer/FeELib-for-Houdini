
#pragma once

#ifndef __GFE_IntersectionStitch_h__
#define __GFE_IntersectionStitch_h__

#include "GFE/GFE_IntersectionStitch.h"

#include "GFE/GFE_GeoFilter.h"




#include "GFE/GFE_NodeVerb.h"
#include "SOP/SOP_IntersectionStitch.proto.h"

class GFE_IntersectionStitch : public GFE_AttribFilterWithRef {


public:
    using GFE_AttribFilterWithRef::GFE_AttribFilterWithRef;


    void
        setComputeParm(
            const bool splitCurve      = false,
            const bool repairResult    = false,
            const bool keepPointAttrib = true,
            const exint subscribeRatio = 64,
            const exint minGrainSize   = 1024
        )
    {
        setHasComputed();
        this->splitCurve      = splitCurve;
        this->repairResult    = repairResult;
        this->keepPointAttrib = keepPointAttrib;
        this->subscribeRatio = subscribeRatio;
        this->minGrainSize   = minGrainSize;
    }
    
    SYS_FORCE_INLINE void setTolerance()
    { useTol = false;  }
    
    SYS_FORCE_INLINE void setTolerance(const bool tolerance)
    { useTol = true; this->tolerance = outSecondInputGeo; }


    
    SYS_FORCE_INLINE void setInsertPoint(const bool outSecondInputGeo)
    { insertPoint = false; this->outSecondInputGeo = outSecondInputGeo; }
    
    SYS_FORCE_INLINE void setInsertPoint()
    { insertPoint = true; }


    
    SYS_FORCE_INLINE void setInputnumAttrib(const GA_Attribute* const inAttrib)
    { inputnumAttrib = inAttrib->getOwner()==GA_ATTRIB_POINT && &inAttrib->getDetail() == geoRef0 ? inAttrib : nullptr; }
    
    SYS_FORCE_INLINE const GA_Attribute* setInputnumAttrib(const UT_StringRef& name)
    { return inputnumAttrib = geoRef0->findPointAttribute(name); }

    
    SYS_FORCE_INLINE void setPrimnumAttrib(const GA_Attribute* const inAttrib)
    { primnumAttrib = inAttrib->getOwner()==GA_ATTRIB_POINT && &inAttrib->getDetail() == geoRef0 ? inAttrib : nullptr; }
    
    SYS_FORCE_INLINE const GA_Attribute* setPrimnumAttrib(const UT_StringRef& name)
    { return primnumAttrib = geoRef0->findPointAttribute(name); }

    
    SYS_FORCE_INLINE void setPrimuvwAttrib(const GA_Attribute* const inAttrib)
    { primuvwAttrib = inAttrib->getOwner()==GA_ATTRIB_POINT && &inAttrib->getDetail() == geoRef0 ? inAttrib : nullptr; }
    
    SYS_FORCE_INLINE const GA_Attribute* setPrimuvwAttrib(const UT_StringRef& name)
    { return primuvwAttrib = geoRef0->findPointAttribute(name); }
    
private:

#define __TEMP_GFE_IntersectionStitch_InGroupName  "__TEMP_GFE_IntersectionStitch_InGroup"
#define __TEMP_GFE_IntersectionStitch_RefGroupName "__TEMP_GFE_IntersectionStitch_RefGroup"
    
#define __TEMP_GFE_IntersectionStitch_inputnumAttribName  "__TEMP_GFE_IntersectionStitch_inputnumAttrib"
#define __TEMP_GFE_IntersectionStitch_primnumAttribName   "__TEMP_GFE_IntersectionStitch_primnumAttrib"
#define __TEMP_GFE_IntersectionStitch_primuvwAttribName   "__TEMP_GFE_IntersectionStitch_primuvwAttrib"
    
    virtual bool
        computeCore() override
    {
        UT_ASSERT_MSG_P(interStitchVerb, "Does not have interStitch Verb");
        
        UT_ASSERT_MSG_P(cookparms, "Does not have cookparms");
        if (!cookparms)
            return false;
        
        if (insertPoint && !(inputnumAttrib && primnumAttrib && primuvwAttrib))
            return false;
            
        if (groupParser.isEmpty())
            return true;
        
        const bool repairResultFinal = repairResult && (insertPoint || !outSecondInputGeo);
        if (repairResultFinal)
        {
            ptnumNewLast = geo->getNumPoints();
            numprimInput = geo->getNumPrimitives();
        }
        
        GU_DetailHandle geoTmp_h;
        geoTmp = new GU_Detail();
        geoTmp_h.allocateAndSet(geoTmp);
        geoTmp->replaceWith(*geo);
        
        GU_DetailHandle geoRefTmp_h;
        geoRefTmp = new GU_Detail();
        geoRefTmp_h.allocateAndSet(geoRefTmp);
        geoRefTmp->replaceWith(*geoRef0);
        
        inputgdh.clear();
        inputgdh.emplace_back(geoTmp_h);
        if (insertPoint)
        {
            inputgdh.emplace_back(GU_DetailHandle());
            inputgdh.emplace_back(geoRefTmp_h);
        }
        else
        {
            inputgdh.emplace_back(geoRefTmp_h);
            inputgdh.emplace_back(GU_DetailHandle());
        }
        
        geo->clear();
        destgdh.allocateAndSet(geo->asGU_Detail(), false);
        
        intersectionStitch();

        if (repairResultFinal)
            repair();
        
        //geo->getGroupTable(GA_GROUP_PRIMITIVE)->destroy(__TEMP_GFE_IntersectionStitch_InGroupName);
        //geo->getGroupTable(insertPoint ? GA_GROUP_POINT : GA_GROUP_PRIMITIVE)->destroy(__TEMP_GFE_IntersectionStitch_RefGroupName);
        
        return true;
    }



    void repair()
    {
        ptnumNewLast = geo->getNumPoints() - ptnumNewLast;
        UTparallelFor(groupParser.getPrimitiveSplittableRange(), [this](const GA_SplittableRange& r)
        {
            GA_Offset start, end;
            for (GA_Iterator it(r); it.blockAdvance(start, end); )
            {
                for (GA_Offset primoff = start; primoff < end; ++primoff)
                {
                    const GA_Size numvtx = geo->getPrimitiveVertexCount(primoff);
                    if (numvtx <= 0)
                        continue;
                    const GA_Offset primvtxoff_last = geo->getPrimitiveVertexOffset(primoff, numvtx-1);
                    const GA_Offset primptoff_last   = geo->vertexPoint(primvtxoff_last);
                    const GA_Index  primptnum_last  = geo->pointIndex(primptoff_last);
                    
                    if  ( primptnum_last >= ptnumNewLast ) return;
                    

                    const GA_Offset primvtxoff0 = geo->getPrimitiveVertexOffset(primoff, 0);
                    const GA_Offset primptoff0  = geo->vertexPoint(primvtxoff0);
                    
                    GA_Offset nextPrimoff;
                    for (GA_Offset pointVtxoff = geo->pointVertex(primptoff0); GFE_Type::isValidOffset(pointVtxoff); pointVtxoff = geo->vertexToNextVertex(pointVtxoff))
                    {
                        nextPrimoff = geo->vertexPrimitive(pointVtxoff);
                        if ( geo->primitiveIndex(nextPrimoff) >= numprimInput ) break;
                    }
                    
                    // const GA_Size numvtx_nextPrim = geo->getPrimitiveVertexCount(nextPrimoff);
                    for (GA_Size vtxpnum = 1; vtxpnum < numvtx; ++vtxpnum)
                    {
                        const GA_Offset nextPrimvtx   = geo->getPrimitiveVertexOffset(primoff, vtxpnum);
                        const GA_Offset nextPrimptoff = geo->vertexPoint(nextPrimvtx);
                        const GA_Offset newvtx = geo->appendVertex();
                        geo->getTopology().wireVertexPrimitive(newvtx, nextPrimoff);
                        geo->getTopology().wireVertexPoint(newvtx, nextPrimptoff);
                    }
                    
                    geo->destroyPrimitiveOffset(primoff);
                }
            }
            
        }, subscribeRatio, minGrainSize);
    }
    
    void intersectionStitch()
    {
        interStitchParms.setUseproxtol(useTol);
        interStitchParms.setProxtol(tolerance);
        interStitchParms.setSplitcurves(splitCurve);
        interStitchParms.setKeeppointattribs(keepPointAttrib);

        
        if (groupParser.getHasGroup())
        {
            if (groupParser.isDetached() || !groupParser.isPrimitiveGroup())
            {
                GA_Group* const inGroup = geoTmp->newPrimitiveGroup(__TEMP_GFE_IntersectionStitch_InGroupName);
                inGroup->combine(groupParser.getGroup());
                interStitchParms.setAgroup(__TEMP_GFE_IntersectionStitch_InGroupName);
            }
            else
            {
                interStitchParms.setAgroup(groupParser.getName());
            }
        }
        else
        {
            interStitchParms.setAgroup("");
        }
        
        if (groupParserRef0.getHasGroup())
        {
            if (groupParserRef0.isDetached() || !groupParserRef0.isPrimitiveGroup())
            {
                GA_Group* refGroup;
                if (insertPoint)
                    refGroup = geoRefTmp->getGroupTable(GA_GROUP_POINT)    ->newGroup(__TEMP_GFE_IntersectionStitch_RefGroupName);
                else
                    refGroup = geoRefTmp->getGroupTable(GA_GROUP_PRIMITIVE)->newGroup(__TEMP_GFE_IntersectionStitch_RefGroupName);
                refGroup->combine(groupParserRef0.getGroup());
                interStitchParms.setBgroup(__TEMP_GFE_IntersectionStitch_RefGroupName);
            }
            else
            {
                interStitchParms.setBgroup(groupParserRef0.getName());
            }
        }
        else
        {
            interStitchParms.setAgroup("");
        }
        
        
        
        if (insertPoint)
        {
            UT_ASSERT_P(inputnumAttrib);
            UT_ASSERT_P(primnumAttrib);
            UT_ASSERT_P(primuvwAttrib);
            
            const bool detachedInputnum = inputnumAttrib->isDetached();
            const bool detachedPrimnum  = primnumAttrib->isDetached();
            const bool detachedPrimuvw  = primuvwAttrib->isDetached();

            if (detachedInputnum)
                GFE_Attribute::clone(*geoRefTmp, *inputnumAttrib, __TEMP_GFE_IntersectionStitch_inputnumAttribName);
            if (detachedPrimnum)
                GFE_Attribute::clone(*geoRefTmp, *primnumAttrib,  __TEMP_GFE_IntersectionStitch_primnumAttribName);
            if (detachedPrimuvw)
                GFE_Attribute::clone(*geoRefTmp, *primuvwAttrib,  __TEMP_GFE_IntersectionStitch_primuvwAttribName);
            
            interStitchParms.setInputnumattrib(detachedInputnum ? UT_StringHolder(__TEMP_GFE_IntersectionStitch_inputnumAttribName) : inputnumAttrib->getName());
            interStitchParms.setPrimnumattrib(detachedPrimnum   ? UT_StringHolder(__TEMP_GFE_IntersectionStitch_primnumAttribName)  : primnumAttrib->getName());
            interStitchParms.setPrimuvwattrib(detachedPrimuvw   ? UT_StringHolder(__TEMP_GFE_IntersectionStitch_primuvwAttribName)  : primuvwAttrib->getName());
        }
        
        SOP_NodeCache* const nodeCache = interStitchVerb->allocCache();
        const auto interStitchCookparms = GFE_NodeVerb::newCookParms(cookparms, interStitchParms, nodeCache, &destgdh, &inputgdh);
        interStitchVerb->cook(interStitchCookparms);
    }
    
public:
    bool splitCurve = false;
    bool repairResult = false;
    bool keepPointAttrib = true;
    
private:
    bool insertPoint = true;
    bool outSecondInputGeo = false;
    bool useTol = true;
    fpreal64 tolerance = 1e-05;
    
private:
    GU_Detail* geoTmp = nullptr;
    GU_Detail* geoRefTmp = nullptr;

private:
    GA_Index ptnumNewLast;
    GA_Size numprimInput;
    
    const GA_Attribute* inputnumAttrib = nullptr;
    const GA_Attribute* primnumAttrib  = nullptr;
    const GA_Attribute* primuvwAttrib  = nullptr;
    
    
    GU_DetailHandle destgdh;
    UT_Array<GU_ConstDetailHandle> inputgdh;
    SOP_IntersectionStitchParms interStitchParms;
    const SOP_NodeVerb* const interStitchVerb = SOP_NodeVerb::lookupVerb("intersectionstitch");
        
    
    exint subscribeRatio = 64;
    exint minGrainSize = 1024;

#undef __TEMP_GFE_IntersectionStitch_InGroupName
#undef __TEMP_GFE_IntersectionStitch_RefGroupName
    
#undef __TEMP_GFE_IntersectionStitch_inputnumAttribName
#undef __TEMP_GFE_IntersectionStitch_primnumAttribName
#undef __TEMP_GFE_IntersectionStitch_primuvwAttribName
    
}; // End of class GFE_IntersectionStitch





#endif
