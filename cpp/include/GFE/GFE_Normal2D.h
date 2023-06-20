
#pragma once

#ifndef __GFE_Normal2D_h__
#define __GFE_Normal2D_h__

#include "GFE/GFE_Normal2D.h"



#include "GFE/GFE_Normal3D.h"
#include "GFE/GFE_MeshTopology.h"
#include "GFE/GFE_GroupPromote.h"
//#include "GFE/GFE_GroupBoolean.h"
#include "GFE/GFE_GroupExpand.h"



class GFE_Normal2D : public GFE_AttribFilter {

public:
    GFE_Normal2D(
        GFE_Detail& geo,
        const SOP_NodeVerb::CookParms* const cookparms = nullptr
    )
        : GFE_AttribFilter(geo, cookparms)
        , normal3D(geo, cookparms)
    {
    }
    
    GFE_Normal2D(
        GA_Detail& geo,
        const SOP_NodeVerb::CookParms* const cookparms = nullptr
    )
        : GFE_AttribFilter(geo, cookparms)
        , normal3D(geo, cookparms)
    {
    }
    
    void
        setComputeParm(
            const bool extrapolateEnds = true,
            const bool scaleByTurns = true,
            const bool normalize = true,
            
            const fpreal64 uniScale = 1.0,
            const fpreal64 blend = 1.0,

            const exint subscribeRatio = 64,
            const exint minGrainSize = 1024
        )
    {
        setHasComputed();
        
        this->extrapolateEnds = extrapolateEnds;
        this->scaleByTurns = scaleByTurns;
        
        this->normalize    = normalize;
        this->uniScale     = uniScale;
        this->blend        = blend;

        this->subscribeRatio = subscribeRatio;
        this->minGrainSize   = minGrainSize;
    }
    
    SYS_FORCE_INLINE void setNormal3DAttrib()
    { normal3DAttrib = nullptr; }
    
    SYS_FORCE_INLINE void setNormal3DAttrib(const GFE_NormalSearchOrder normal3DSearchOrder, const UT_StringRef& attribName = "", const bool addNormal3DIfNoFind = true)
    {
        normal3DAttrib = normal3D.getOutAttribArray().findOrCreateNormal3D(true, normal3DSearchOrder, GA_STORE_INVALID, attribName);
        if (!addNormal3DIfNoFind && normal3DAttrib->isDetached())
        {
            normal3DAttrib = nullptr;
            normal3D.getOutAttribArray().clear();
        }
    }
    
    
    SYS_FORCE_INLINE void setNormal2DAttrib(const bool detached = true, const char* const attribName = "")
    { normal2DAttribName = detached ? nullptr: attribName; normal2DAttrib = nullptr }
    
    SYS_FORCE_INLINE void setNormal2DAttrib(const bool detached = true, const UT_StringRef& attribName = "")
    { setNormal2DAttrib(detached, attribName.c_str()); }
    
    SYS_FORCE_INLINE void setNormal2DAttrib(GA_Attribute* const attrib)
    {
        normal2DAttribName = nullptr;
        normal2DAttrib = GFE_Type::checkDirAttrib(attrib) ? attrib : nullptr;
    }
    


    
private:

#define __GFE_Normal2D_Temp_AttribName "__GFE_Normal2D_Temp_AttribName"
    
    virtual bool
        computeCore() override
    {
        //if (getOutAttribArray().isEmpty())
        //    return false;

        if (groupParser.isEmpty())
            return true;

        if (!posAttrib)
            posAttrib = geo->getP();

        if (normal3DAttrib && normal3DAttrib->isDetached())
            normal3D.compute();
        
        GA_Storage storage_max = posAttrib->getAIFTuple()->getStorage(posAttrib);
        if (normal3DAttrib)
        {
            const GA_Storage storage_normal3D = normal3DAttrib->getAIFTuple()->getStorage(normal3DAttrib);
            storage_max = storage_max >= storage_normal3D ? storage_max : storage_normal3D;
        }

        
        isGroupFull = groupParser.isFull();
        const bool shouldRenameAttrib = normal3DAttrib && !normal3DAttrib->isDetached() && strcmp(normal3DAttrib->getName().c_str(), normal2DAttribName)==0;
        if (normal2DAttribName)
        {
            if (shouldRenameAttrib)
                getOutAttribArray().findOrCreateDir(false, GA_ATTRIB_POINT, GA_STORE_INVALID, __GFE_Normal2D_Temp_AttribName);
            else
            {
                getOutAttribArray().findOrCreateDir(false, GA_ATTRIB_POINT, GA_STORE_INVALID, normal2DAttribName);
                if (isGroupFull)
                {
                    geo->destroyAttribute(GA_ATTRIB_POINT, normal2DAttribName);
                    getOutAttribArray().findOrCreateDir(false, GA_ATTRIB_POINT, GA_STORE_INVALID, normal2DAttribName);
                }
                else
                {
                    geo->destroyAttribute(GA_ATTRIB_POINT, normal2DAttribName);
                    getOutAttribArray().findOrCreateDir(false, GA_ATTRIB_POINT, GA_STORE_INVALID, normal2DAttribName);
                }
            }
        }
        else
        {
            if (!normal2DAttrib)
                normal2DAttrib = getOutAttribArray().findOrCreateDir(true, GA_ATTRIB_POINT, GA_STORE_INVALID);
        }
        
        switch (storage_max)
        {
        case GA_STORE_REAL16: computeNormal2D<fpreal16>(); break;
        case GA_STORE_REAL32: computeNormal2D<fpreal32>(); break;
        case GA_STORE_REAL64: computeNormal2D<fpreal64>(); break;
        default: UT_ASSERT_MSG(0, "Unhandled Storage");    break;
        }

        if (shouldRenameAttrib)
            geo->forceRenameAttribute(getOutAttribArray()[0], normal2DAttribName);

        return true;
    }


#undef __GFE_Normal2D_Temp_AttribName


    template<typename FLOAT_T>
    void computeNormal2D()
    {
        const GA_PointGroup* const geoPointGroup = groupParser.getPointGroup();

        GFE_MeshTopology meshTopology(geo, cookparms);
        GA_VertexGroup* const unsharedVertexGroup = meshTopology.setVertexNextEquivGroup(true);
        const GA_Attribute* const dstptAttrib = meshTopology.setVertexPointDst(true);
        meshTopology.compute();

        const GA_PointGroupUPtr unsharedPointGroupUPtr = geo->createDetachedPointGroup();
        GA_PointGroup* const unsharedPointGroup = unsharedPointGroupUPtr.get();
        unsharedPointGroup->combine(unsharedVertexGroup);
        
        if (geoPointGroup)
        {
            //GA_PointGroupUPtr expandGroupUPtr = geo->createDetachedPointGroup();
            //expandGroupUPtr.get();
            
            GFE_GroupExpand groupExpand(geo, cookparms);
            groupExpand.setBaseGroup(geoPointGroup);
            GA_PointGroup* const geoExpandPointGroup = static_cast<GA_PointGroup*>(groupExpand.setExpandGroup(true));
            groupExpand.compute();
            
            *unsharedPointGroup &= *geoExpandPointGroup;
            
            //const GA_PointGroupUPtr expandPointGroupUPtr = geo->createDetachedPointGroup();
            //GA_PointGroup* const expandPointGroup = expandPointGroupUPtr.get();
            //expandPointGroup->combine(unsharedVertexGroup);
            geo->groupIntersect(*unsharedVertexGroup, geoExpandPointGroup);
            //*unsharedVertexGroup &= *geoExpandPointGroup;
        }

        const GA_ROHandleT<GA_Offset> dstptAttrib_h(dstptAttrib);

        const GA_RWHandleT<UT_Vector3T<FLOAT_T>> normal2D_h(getOutAttribArray()[0]);
        const GA_ROHandleT<UT_Vector3T<FLOAT_T>> pos_h(posAttrib);
        const GA_ROHandleT<UT_Vector3T<FLOAT_T>> normal3D_h(normal3DAttrib);

        if (!normal3DAttrib)
            defaultNormal3DFinal = defaultNormal3D;
        const GA_AttributeOwner normal3DOwner = normal3DAttrib ? normal3DAttrib->getOwner() : GA_ATTRIB_INVALID;
        if (normal3DOwner == GA_ATTRIB_GLOBAL)
            defaultNormal3DFinal = normal3D_h.get(0);
        
        
        
        const GA_Topology& topo = geo->getTopology();
        //topo.makePrimitiveRef();
        const GA_ATITopology& vtxPointRef = *topo.getPointRef();
        const GA_ATITopology& vtxPrimRef  = *topo.getPrimitiveRef();

        UTparallelFor(GA_SplittableRange(geo->getVertexRange(unsharedVertexGroup)), [this, &dstptAttrib_h, &normal2D_h, &pos_h, &normal3D_h,
            &vtxPointRef, &vtxPrimRef, normal3DOwner](const GA_SplittableRange& r)
        {
            UT_Vector3T<FLOAT_T> dir;
            GA_Offset start, end;
            for (GA_Iterator it(r); it.blockAdvance(start, end); )
            {
                for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                {
                    const GA_Offset ptoff = vtxPointRef.getLink(elemoff);
                    const GA_Offset dstpt = dstptAttrib_h.get(elemoff);

                    dir = pos_h.get(ptoff) - pos_h.get(dstpt);
                    switch (normal3DOwner)
                    {
                    case GA_ATTRIB_PRIMITIVE:
                        dir.cross(normal3D_h.get(vtxPrimRef.getLink(elemoff)));
                        break;
                    case GA_ATTRIB_POINT:
                        dir.cross(normal3D_h.get(ptoff));
                        break;
                    case GA_ATTRIB_VERTEX:
                        dir.cross(normal3D_h.get(elemoff));
                        break;
                    default:
                        dir.cross(defaultNormal3DFinal);
                        break;
                    }
                    dir.normalize();
                    normal2D_h.add(ptoff, dir);
                    normal2D_h.add(dstpt, dir);
                }
            }
        }, subscribeRatio, minGrainSize);



        const GA_SplittableRange geoPointSplittableRange(geo->getPointRange(unsharedPointGroup));
        
        GA_Attribute* const outNormal2DAttrib = geo->findPointAttribute(normal2DAttribName);
        if (outNormal2DAttrib && groupParser.getHasGroup())
        {
            getOutAttribArray().append(outNormal2DAttrib);
            UTparallelFor(geoPointSplittableRange, [this, outNormal2DAttrib](const GA_SplittableRange& r)
            {
                GA_Offset start, end;
                GA_PageHandleT<UT_Vector3T<FLOAT_T>, FLOAT_T, true, true, GA_Attribute, GA_ATINumeric, GA_Detail> normal2D_ph(outNormal2DAttrib);
                GA_PageHandleT<UT_Vector3T<FLOAT_T>, FLOAT_T, true, true, GA_Attribute, GA_ATINumeric, GA_Detail> normal2D_ph(getOutAttribArray()[0]);
                for (GA_PageIterator pit = r.beginPages(); !pit.atEnd(); ++pit)
                {
                    for (GA_Iterator it(pit.begin()); it.blockAdvance(start, end); )
                    {
                        normal2D_ph.setPage(start);
                        for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                        {
                            if (scaleByTurns)
                            {
                                normal2D_ph.value(elemoff) *= 2 * uniScale / normal2D_ph.value(elemoff).length2();
                            }
                            else if (normalize)
                            {
                                normal2D_ph.value(elemoff).normalize();
                                normal2D_ph.value(elemoff) *= uniScale;
                            }
                            normal2D_ph.value(elemoff) = normal2DTemp_ph.value(elemoff);
                        }
                    }
                }
            }, subscribeRatio, minGrainSize);
        }
        else
        {
            UTparallelFor(geoPointSplittableRange, [this](const GA_SplittableRange& r)
            {
                GA_Offset start, end;
                GA_PageHandleT<UT_Vector3T<FLOAT_T>, FLOAT_T, true, true, GA_Attribute, GA_ATINumeric, GA_Detail> normal2D_ph(getOutAttribArray()[0]);
                for (GA_PageIterator pit = r.beginPages(); !pit.atEnd(); ++pit)
                {
                    for (GA_Iterator it(pit.begin()); it.blockAdvance(start, end); )
                    {
                        normal2D_ph.setPage(start);
                        for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                        {
                            if (scaleByTurns)
                            {
                                normal2D_ph.value(elemoff) *= 2 * uniScale / normal2D_ph.value(elemoff).length2();
                            }
                            else if (normalize)
                            {
                                normal2D_ph.value(elemoff).normalize();
                                normal2D_ph.value(elemoff) *= uniScale;
                            }
                        }
                    }
                }
            }, subscribeRatio, minGrainSize);
        }
    }






public:
    GFE_Normal3D normal3D;


    UT_Vector3T<fpreal64> defaultNormal3D = {0,1,0};
    
    bool extrapolateEnds = true;
    bool scaleByTurns    = true;
    bool normalize       = true;
    fpreal64 uniScale = 1.0;
    fpreal64 blend = 1.0;
    
    bool findNormal3D = false;
    bool addNormal3DIfNoFind = true;

private:
    UT_Vector3T<fpreal64> defaultNormal3DFinal;
    const GA_Attribute* normal3DAttrib = nullptr;
    
    GA_Attribute* normal2DAttrib = nullptr;
    const char* normal2DAttribName = nullptr;
    
    bool isGroupFull;

    exint subscribeRatio = 64;
    exint minGrainSize = 1024;
    
}; // End of class GFE_Normal2D



#endif
