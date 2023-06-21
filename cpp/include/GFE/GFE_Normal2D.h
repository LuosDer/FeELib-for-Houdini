
#pragma once

#ifndef __GFE_Normal2D_h__
#define __GFE_Normal2D_h__

#include "GFE/GFE_Normal2D.h"



#include "GFE/GFE_Normal3D.h"
#include "GFE/GFE_MeshTopology.h"
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
    { normal2DAttribName = detached ? nullptr: attribName; normal2DAttrib = nullptr; }
    
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

        
        hasAttrib   = normal3DAttrib && !normal3DAttrib->isDetached() && strcmp(normal3DAttrib->getName().c_str(), normal2DAttribName)==0;
        isGroupFull = groupParser.isFull();
        

        if (normal2DAttribName)
        {
            if (hasAttrib)
            {
                if (isGroupFull)
                {
                    normal2DAttrib = getOutAttribArray().findOrCreateDir(false, GA_ATTRIB_POINT, GA_STORE_INVALID, __GFE_Normal2D_Temp_AttribName);
                }
                else
                {
                    GA_Attribute* const outNormal2DAttrib = geo->findPointAttribute(normal2DAttribName);
                    getOutAttribArray().append(outNormal2DAttrib);
                    normal2DAttrib = getOutAttribArray().findOrCreateDir(true, GA_ATTRIB_POINT, GA_STORE_INVALID);
                }
            }
            else
            {
                normal2DAttrib = geo->findPointAttribute(normal2DAttribName);
                if (GFE_Type::checkTupleAttrib(normal2DAttrib, GA_STORE_INVALID, 3))
                {
                    //onst fpreal64 constValue[] = {0.0, 0.0, 0.0};
                    //normal2DAttrib->getAIFTuple()->setRange(normal2DAttrib, groupParser.getPointRange(), constValue, 0, 3);
                    normal2DAttrib->getAIFTuple()->set(normal2DAttrib, groupParser.getPointRange(), 0.0);
                    getOutAttribArray().append(normal2DAttrib);
                }
                else
                    normal2DAttrib = getOutAttribArray().findOrCreateDir(false, GA_ATTRIB_POINT, GA_STORE_INVALID, normal2DAttribName);
            }
        }
        else
        {
            if (normal2DAttrib)
            {
                //const fpreal64 constValue(0.0);
                //normal2DAttrib->getAIFTuple()->setRange(normal2DAttrib, groupParser.getPointRange(), &constValue, 0, 3);
                normal2DAttrib->getAIFTuple()->set(normal2DAttrib, groupParser.getPointRange(), 0.0);
            }
            else
                normal2DAttrib = getOutAttribArray().findOrCreateDir(true, GA_ATTRIB_POINT, GA_STORE_INVALID);
        }
        //return true;


        
        GFE_MeshTopology meshTopology(geo, cookparms);
        unsharedVertexGroup = meshTopology.setVertexNextEquivGroup(true);
        dstptAttrib_h = meshTopology.setVertexPointDst(true);
        meshTopology.compute();

        const GA_PointGroupUPtr unsharedPointGroupUPtr = geo->createDetachedPointGroup();
        unsharedPointGroup = unsharedPointGroupUPtr.get();
        unsharedPointGroup->combine(unsharedVertexGroup);
        
        geoPointGroup = groupParser.getPointGroup();
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

        if (!normal3DAttrib)
            defaultNormal3DFinal = defaultNormal3D;
        normal3DOwner = normal3DAttrib ? normal3DAttrib->getOwner() : GA_ATTRIB_INVALID;

        const GA_Topology& topo = geo->getTopology();
        //topo.makePrimitiveRef();
        vtxPointRef = topo.getPointRef();
        vtxPrimRef  = topo.getPrimitiveRef();


        
        switch (storage_max)
        {
        case GA_STORE_REAL16: computeNormal2D<fpreal16>(); break;
        case GA_STORE_REAL32: computeNormal2D<fpreal32>(); break;
        case GA_STORE_REAL64: computeNormal2D<fpreal64>(); break;
        default: UT_ASSERT_MSG(0, "Unhandled Storage");    break;
        }

        if (normal2DAttribName && hasAttrib && isGroupFull)
            geo->forceRenameAttribute(normal2DAttrib, normal2DAttribName);

        return true;
    }


#undef __GFE_Normal2D_Temp_AttribName


    template<typename FLOAT_T>
    void computeNormal2D()
    {
        const GA_RWHandleT<UT_Vector3T<FLOAT_T>> normal2D_h(normal2DAttrib);
        const GA_ROHandleT<UT_Vector3T<FLOAT_T>> pos_h(posAttrib);
        const GA_ROHandleT<UT_Vector3T<FLOAT_T>> normal3D_h(normal3DAttrib);
        if (normal3DOwner == GA_ATTRIB_GLOBAL)
            defaultNormal3DFinal = normal3D_h.get(0);
        

        UTparallelFor(GA_SplittableRange(geo->getVertexRange(unsharedVertexGroup)), [this, &normal2D_h, &pos_h, &normal3D_h](const GA_SplittableRange& r)
        {
            UT_Vector3T<FLOAT_T> dir;
            GA_Offset start, end;
            for (GA_Iterator it(r); it.blockAdvance(start, end); )
            {
                for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                {
                    const GA_Offset ptoff = vtxPointRef->getLink(elemoff);
                    const GA_Offset dstpt = dstptAttrib_h.get(elemoff);

                    dir = pos_h.get(ptoff) - pos_h.get(dstpt);
                    switch (normal3DOwner)
                    {
                    case GA_ATTRIB_PRIMITIVE:
                        dir.cross(normal3D_h.get(vtxPrimRef->getLink(elemoff)));
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
                    if (!geoPointGroup || geoPointGroup->contains(ptoff))
                        normal2D_h.add(ptoff, dir);
                    if (!geoPointGroup || geoPointGroup->contains(dstpt))
                        normal2D_h.add(dstpt, dir);
                }
            }
        }, subscribeRatio, minGrainSize);


        if (geoPointGroup)
            *unsharedPointGroup &= *geoPointGroup;
        const GA_SplittableRange geoPointSplittableRange(geo->getPointRange(unsharedPointGroup));
        
        if (normal2DAttribName && hasAttrib && !isGroupFull)
        {
            GA_Attribute* const outNormal2DAttrib = geo->findPointAttribute(normal2DAttribName);
            UT_ASSERT_P(outNormal2DAttrib);
            UTparallelFor(geoPointSplittableRange, [this, outNormal2DAttrib](const GA_SplittableRange& r)
            {
                GA_Offset start, end;
                GA_PageHandleT<UT_Vector3T<FLOAT_T>, FLOAT_T, true, true, GA_Attribute, GA_ATINumeric, GA_Detail> normal2D_ph(outNormal2DAttrib);
                GA_PageHandleT<UT_Vector3T<FLOAT_T>, FLOAT_T, true, false, const GA_Attribute, const GA_ATINumeric, const GA_Detail> normal2DTmp_ph(normal2DAttrib);
                for (GA_PageIterator pit = r.beginPages(); !pit.atEnd(); ++pit)
                {
                    for (GA_Iterator it(pit.begin()); it.blockAdvance(start, end); )
                    {
                        normal2D_ph.setPage(start);
                        normal2DTmp_ph.setPage(start);
                        for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                        {
                            normal2D_ph.value(elemoff) = normal2DTmp_ph.value(elemoff);
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
        else
        {
            UTparallelFor(geoPointSplittableRange, [this](const GA_SplittableRange& r)
            {
                GA_Offset start, end;
                GA_PageHandleT<UT_Vector3T<FLOAT_T>, FLOAT_T, true, true, GA_Attribute, GA_ATINumeric, GA_Detail> normal2D_ph(normal2DAttrib);
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
    GA_AttributeOwner normal3DOwner;
    
    GA_Attribute* normal2DAttrib = nullptr;
    const char* normal2DAttribName = nullptr;
    
    bool hasAttrib;
    bool isGroupFull;

    const GA_PointGroup* geoPointGroup;
    GA_VertexGroup* unsharedVertexGroup;
    GA_PointGroup*  unsharedPointGroup;
    GA_ROHandleT<GA_Offset> dstptAttrib_h;
    const GA_ATITopology* vtxPointRef;
    const GA_ATITopology* vtxPrimRef;
    
    exint subscribeRatio = 64;
    exint minGrainSize = 1024;
    
}; // End of class GFE_Normal2D



#endif
