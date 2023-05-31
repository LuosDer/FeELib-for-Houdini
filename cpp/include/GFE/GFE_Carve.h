
#pragma once

#ifndef __GFE_Carve_h__
#define __GFE_Carve_h__



//#include "GFE/GFE_Carve.h"

#include "GFE/GFE_GeoFilter.h"


#include "GFE/GFE_Attribute.h"
#include "GFE/GFE_CurveUV.h"
#include "GFE/GFE_Math.h"

enum class GFE_CarveSpace {
    CustomAttrib,
    LocalAverage,
    LocalArcLength,
    WorldAverage,
    WorldArcLength,
};


enum class GFE_CarveType {
    Start,
    End,
    Both,
};






class GFE_Carve : public GFE_AttribFilter {


public:
    GFE_GroupParser groupParser_carveStart;
    GFE_GroupParser groupParser_carveEnd;

    GFE_CarveSpace carveSpace = GFE_CarveSpace::WorldArcLength;
    bool keepOutsideStart = false;
    bool keepOutsideEnd = false;
    bool keepInside = true;

    fpreal startCarveU = 0.25;
    fpreal endCarveU = 0.25;

    bool absCarveUEnd = false;
    bool delReversedInsidePrim = false;

    bool delStartCarveUAttrib = false;
    bool delEndCarveUAttrib = false;


private:

    const GA_Attribute* uvAttrib = nullptr;
    //GA_Attribute* uvAttrib_nonConst = nullptr;
    GA_ROHandleT<fpreal> uvAttrib_h;

    const GA_Attribute* startCarveUAttrib = nullptr;
    const GA_Attribute* endCarveUAttrib = nullptr;
    GA_ROHandleT<fpreal> startCarveU_h;
    GA_ROHandleT<fpreal> endCarveU_h;
    
        
    GA_Attribute* startCarveUAttrib_nonConst = nullptr;
    GA_Attribute* endCarveUAttrib_nonConst = nullptr;
    
    GA_PointGroupUPtr carvePointGroupUPtr;
    GA_PointGroup* carvePointGroup;

    GA_PrimitiveGroupUPtr carvePrimGroupUPtr;
    GA_PrimitiveGroup* carvePrimGroup;

    GU_DetailHandle geoOrigin_h;
    const GA_Detail* geoOrigin;
    GU_Detail* geoOriginTmp;



public:
    //using GFE_AttribFilter::GFE_AttribFilter;

    GFE_Carve(
        GA_Detail& inGeo,
        //const GA_Detail* const geoOrigin = nullptr,
        const SOP_NodeVerb::CookParms* const cookparms = nullptr
    )
        : GFE_AttribFilter(inGeo, cookparms)
        , groupParser_carveStart(geo, groupParser.getGOP(), cookparms)
        , groupParser_carveEnd(geo, groupParser.getGOP(), cookparms)
    {
        carvePointGroupUPtr = geo->createDetachedPointGroup();
        carvePointGroup = carvePointGroupUPtr.get();
        carvePrimGroupUPtr = geo->createDetachedPrimitiveGroup();
        carvePrimGroup = carvePrimGroupUPtr.get();

        //if (geoOrigin)
        //{
        //    geo->replaceWith(*geoOrigin);
        //    this->geoOrigin = geoOrigin;
        //}
        //else
        //{
        //    geoOriginTmp = new GU_Detail();
        //    geoOrigin_h.allocateAndSet(geoOriginTmp);
        //    geoOriginTmp->replaceWith(*geo);

        //    this->geoOrigin = geoOriginTmp;
        //}
    }

    ~GFE_Carve()
    {
        if (delStartCarveUAttrib && startCarveUAttrib_nonConst)
        {
            geo->getAttributes().destroyAttribute(startCarveUAttrib_nonConst);
        }
        if (delEndCarveUAttrib && endCarveUAttrib_nonConst)
        {
            geo->getAttributes().destroyAttribute(endCarveUAttrib_nonConst);
        }
    }

    SYS_FORCE_INLINE void setGroup( const GA_PrimitiveGroup* const geoPrimGroup = nullptr)
    { groupParser.setGroup(geoPrimGroup); }

    SYS_FORCE_INLINE void setGroup(const UT_StringRef& primGroupName = "")
    { groupParser.setGroup(GA_GROUP_PRIMITIVE, primGroupName);}

    SYS_FORCE_INLINE void setCarveStartGroup(const GA_PrimitiveGroup* const geoPrimGroup = nullptr)
    { groupParser_carveStart.setGroup(geoPrimGroup); }

    SYS_FORCE_INLINE void setCarveEndGroup(const GA_PrimitiveGroup* const geoPrimGroup = nullptr)
    { groupParser_carveEnd.setGroup(geoPrimGroup); }

    SYS_FORCE_INLINE void setCarveStartGroup(const UT_StringRef& primGroupName = "")
    { groupParser_carveStart.setGroup(GA_GROUP_PRIMITIVE, primGroupName); }

    SYS_FORCE_INLINE void setCarveEndGroup(const UT_StringRef& primGroupName = "")
    { groupParser_carveEnd.setGroup(GA_GROUP_PRIMITIVE, primGroupName); }

    SYS_FORCE_INLINE void createSrcPrimAttrib(const UT_StringRef& attribName = "srcPrim", const GA_Storage storage = GA_STORE_INVALID)
    { getOutAttribArray().findOrCreateTuple(false, GA_ATTRIB_PRIMITIVE, GA_STORECLASS_INT, storage, attribName, 1, GA_Defaults(-1), false); }

    SYS_FORCE_INLINE void createSrcPointAttrib(const UT_StringRef& attribName = "srcPoint", const GA_Storage storage = GA_STORE_INVALID)
    { getOutAttribArray().findOrCreateTuple(false, GA_ATTRIB_POINT, GA_STORECLASS_INT, storage, attribName, 1, GA_Defaults(-1), false); }

    void
        setComputeParm(
            const GFE_CarveSpace carveSpace = GFE_CarveSpace::WorldArcLength,
            const bool keepOutsideStart = false,
            const bool keepOutsideEnd = false,
            const bool keepInside = true,
            const bool absCarveUEnd = false
        )
    {
        setHasComputed();
        this->carveSpace = carveSpace;
        this->keepOutsideStart = keepOutsideStart;
        this->keepOutsideEnd = keepOutsideEnd;
        this->keepInside = keepInside;
        this->absCarveUEnd = absCarveUEnd;
    }

    template<bool IsEnd>
    void setCarveU(const fpreal carveULocal, const fpreal carveUWorld)
    {
        const bool IsLocal = carveSpace == GFE_CarveSpace::LocalAverage || carveSpace == GFE_CarveSpace::LocalArcLength;
        const fpreal carveU = IsLocal ? carveULocal : carveUWorld;
        if constexpr(IsEnd)
            endCarveU = carveU;
        else
            startCarveU = carveU;
    }
    
    void setUVAttrib(const UT_StringRef& customCarveUVAttribName = "uv")
    {
        setHasComputed();
        if (carveSpace == GFE_CarveSpace::CustomAttrib)
            uvAttrib = GFE_Attribute::findUVAttributePointVertex(*geo, GA_ATTRIB_INVALID, customCarveUVAttribName);
        else
            uvAttrib = nullptr;
    }

    
    void setStartCarveUAttrib(const GA_Attribute* const carveUAttrib, const bool delCarveUAttrib)
    {
        startCarveUAttrib = carveUAttrib;
        delStartCarveUAttrib = delCarveUAttrib;
        if (startCarveUAttrib)
        {
            startCarveU_h.bind(startCarveUAttrib);
        }

        UT_ASSERT_MSG(startCarveUAttrib->getOwner() >= 3, "not correct attrib owner");
    }

    SYS_FORCE_INLINE void setStartCarveUAttrib(GA_Attribute* const carveUAttrib, const bool delCarveUAttrib)
    {
        startCarveUAttrib_nonConst = carveUAttrib;
        setStartCarveUAttrib(static_cast<const GA_Attribute*>(carveUAttrib), delCarveUAttrib);
    }

    void setStartCarveUAttrib(const UT_StringRef& carveUAttribName = "startCarveU", const bool delCarveUAttrib = false)
    {
        startCarveUAttrib_nonConst = geo->findPrimitiveAttribute(carveUAttribName);
        if (!startCarveUAttrib_nonConst)
        {
            startCarveUAttrib_nonConst = geo->findPointAttribute(carveUAttribName);
            if (!startCarveUAttrib_nonConst)
            {
                startCarveUAttrib_nonConst = geo->findVertexAttribute(carveUAttribName);
            }
        }
        if (startCarveUAttrib_nonConst)
        {
            setStartCarveUAttrib(startCarveUAttrib_nonConst, delCarveUAttrib);
        }
        else
        {
            delStartCarveUAttrib = delCarveUAttrib;
        }
    }



    
    void setEndCarveUAttrib(const GA_Attribute* const carveUAttrib, const bool delCarveUAttrib)
    {
        endCarveUAttrib = carveUAttrib;
        delEndCarveUAttrib = delCarveUAttrib;
        if (endCarveUAttrib)
        {
            endCarveU_h.bind(endCarveUAttrib);
        }

        UT_ASSERT_MSG(endCarveUAttrib->getOwner() >= 3, "not correct attrib owner");
    }

    SYS_FORCE_INLINE void setEndCarveUAttrib(GA_Attribute* const carveUAttrib, const bool delCarveUAttrib)
    {
        endCarveUAttrib_nonConst = carveUAttrib;
        setEndCarveUAttrib(static_cast<const GA_Attribute*>(carveUAttrib), delCarveUAttrib);
    }

    
    void setEndCarveUAttrib(const UT_StringRef& carveUAttribName = "endCarveU", const bool delCarveUAttrib = false)
    {
        endCarveUAttrib_nonConst = geo->findPrimitiveAttribute(carveUAttribName);
        if (!endCarveUAttrib_nonConst)
        {
            endCarveUAttrib_nonConst = geo->findPointAttribute(carveUAttribName);
            if (!endCarveUAttrib_nonConst)
            {
                endCarveUAttrib_nonConst = geo->findVertexAttribute(carveUAttribName);
            }
        }
        if (endCarveUAttrib_nonConst)
        {
            setEndCarveUAttrib(endCarveUAttrib_nonConst, delCarveUAttrib);
        }
        else
        {
            delEndCarveUAttrib = delCarveUAttrib;
        }
    }

    
    // SYS_FORCE_INLINE virtual void bumpDataIdsForAddOrRemove() const override
    // { geo->bumpDataIdsForAddOrRemove(true, true, true); }



private:

    // can not use in parallel unless for each GA_Detail
    virtual bool
        computeCore() override
    {
        if (groupParser.isEmpty())
            return true;

        GFE_CurveUV curveUV(geo, cookparms);
        if (carveSpace != GFE_CarveSpace::CustomAttrib)
        {
            switch (carveSpace)
            {
            case GFE_CarveSpace::LocalAverage:   curveUV.curveUVMethod = GFE_CurveUVMethod::LocalAverage;   break;
            case GFE_CarveSpace::LocalArcLength: curveUV.curveUVMethod = GFE_CurveUVMethod::LocalArcLength; break;
            case GFE_CarveSpace::WorldAverage:   curveUV.curveUVMethod = GFE_CurveUVMethod::WorldAverage;   break;
            case GFE_CarveSpace::WorldArcLength:
            default:                             curveUV.curveUVMethod = GFE_CurveUVMethod::WorldArcLength; break;
            }
            curveUV.setGroup(groupParser.getPrimitiveGroup());
            uvAttrib = curveUV.findOrCreateUV(true, GA_ATTRIB_VERTEX, GA_STORE_INVALID);
            curveUV.compute();
        }
        if (!uvAttrib)
            return false;

        uvAttrib_h = uvAttrib;

        carve();

        return true;
    }


    void
        carve()
    {


    }


    template<typename T>
    void
        setAttribVal(
            const GA_RWHandleT<T>& attrib_h,
            const GA_Offset ptnum0,
            const GA_Offset ptnum1,
            const fpreal bias
        )
    {
        T attrib0 = attrib_h.get(ptnum0);
        T attrib1 = attrib_h.get(ptnum1);
        attrib0 = GFE_Math::vlerp(attrib0, attrib1, bias);
        attrib_h.set(ptnum0, attrib0);
    }

    //template<>
    //void
    //    setAttribVal<UT_Vector3T<fpreal>>(
    //        const GA_RWHandleT<UT_Vector3T<fpreal>>& attrib_h,
    //        const GA_Offset ptnum0,
    //        const GA_Offset ptnum1,
    //        const fpreal bias
    //        )
    //{
    //    UT_Vector3T<fpreal> attrib0 = attrib_h.get(ptnum0);
    //    const UT_Vector3T<fpreal> attrib1 = attrib_h.get(ptnum1);
    //    attrib0 = GFE_Math::vlerp<UT_Vector3T<fpreal>, fpreal>(attrib0, attrib1, bias);
    //    attrib_h.set(ptnum0, attrib0);
    //}

    //template<>
    //void
    //    setAttribVal<fpreal>(
    //        const GA_RWHandleT<fpreal>& attrib_h,
    //        const GA_Offset ptnum0,
    //        const GA_Offset ptnum1,
    //        const fpreal bias
    //    )
    //{
    //    fpreal attrib0 = attrib_h.get(ptnum0);
    //    const fpreal attrib1 = attrib_h.get(ptnum1);
    //    attrib0 = GFE_Math::vlerp(attrib0, attrib1, bias);
    //    attrib_h.set(ptnum0, attrib0);
    //}

    //template<>
    //void
    //    setAttribVal<int>(
    //        const GA_RWHandleT<int>& attrib_h,
    //        const GA_Offset ptnum0,
    //        const GA_Offset ptnum1,
    //        const fpreal bias
    //        )
    //{
    //    int attrib0 = attrib_h.get(ptnum0);
    //    const int attrib1 = attrib_h.get(ptnum1);
    //    attrib0 = GFE_Math::vlerp(attrib0, attrib1, bias);
    //    attrib_h.set(ptnum0, attrib0);
    //}

    //template<>
    //void
    //    setAttribVal<bool>(
    //        const GA_RWHandleT<bool>& attrib_h,
    //        const GA_Offset ptnum0,
    //        const GA_Offset ptnum1,
    //        const fpreal bias
    //        )
    //{
    //    const bool attrib0 = attrib_h.get(ptnum0);
    //    const bool attrib1 = attrib_h.get(ptnum1);
    //    if (attrib0 != attrib1)
    //    {
    //        attrib_h.set(ptnum0, bias > 0.5 ? attrib1 : attrib0);
    //    }
    //}


    template<typename FLOAT_T>
    void
        setAttribDir(
            const GA_RWHandleT<UT_Vector3T<FLOAT_T>>& attrib_h,
            const GA_Offset ptnum0,
            const GA_Offset ptnum1,
            const fpreal bias
        )
    {
        UT_Vector3T<FLOAT_T> attrib0 = attrib_h.get(ptnum0);
        UT_Vector3T<FLOAT_T> attrib1 = attrib_h.get(ptnum1);

        typename FLOAT_T length0 = length2(attrib0);
        const typename FLOAT_T length1 = length2(attrib1);
        length0 = sqrt(GFE_Math::vlerp(length0, length1, bias));

        attrib0.normalize();
        attrib1.normalize();

        UT_QuaternionT<fpreal> q;
        q.updateFromVectors(attrib0, attrib1);
        q = UT_QuaternionT<fpreal>(0, 0, 0, 1).interpolate(q, bias);

        attrib0 = q.rotate(attrib0);
        attrib0 *= length0;

        attrib_h.set(ptnum0, attrib0);
    }


    void
        interpAttribs(
            const std::vector<GA_Attribute*>& attribs,
            const GA_Offset ptnum0,
            const GA_Offset ptnum1,
            const fpreal bias
        )
    {
        const size_t size = attribs.size();
        UT_ASSERT_MSG(size, "not correct size");

        for (size_t i = 0; i < size; i++)
        {
            GA_Attribute* const attribPtr = attribs[i];

            const GA_AIFTuple* const aifTuple = attribPtr->getAIFTuple();
            if (aifTuple)
            {
                const int tupleSize = aifTuple->getTupleSize(attribPtr);
                const GA_Storage storage = aifTuple->getStorage(attribPtr);

                if (tupleSize == 3)
                {
                    const GA_TypeInfo typeInfo = attribPtr->getTypeInfo();
                    if (typeInfo == GA_TYPE_VECTOR || typeInfo == GA_TYPE_NORMAL)
                    {
                        switch (storage)
                        {
                        case GA_STORE_BOOL:
                            break;
                        case GA_STORE_UINT8:
                            break;
                        case GA_STORE_INT8:
                            break;
                        case GA_STORE_INT16:
                            break;
                        case GA_STORE_INT32:
                            break;
                        case GA_STORE_INT64:
                            break;
                        case GA_STORE_REAL16:
                            setAttribDir<fpreal16>(attribPtr, ptnum0, ptnum1, bias);
                            continue;
                            break;
                        case GA_STORE_REAL32:
                            setAttribDir<fpreal32>(attribPtr, ptnum0, ptnum1, bias);
                            continue;
                            break;
                        case GA_STORE_REAL64:
                            setAttribDir<fpreal64>(attribPtr, ptnum0, ptnum1, bias);
                            continue;
                            break;
                        case GA_STORE_STRING:
                            break;
                        case GA_STORE_DICT:
                            break;
                        case GA_STORE_INVALID:
                            break;
                        default:
                            break;
                        }
                    }
                }

                switch (storage)
                {
                case GA_STORE_BOOL:
                    setAttribVal<bool>(attribPtr, ptnum0, ptnum1, bias);
                    continue;
                    break;
                case GA_STORE_UINT8:
                    setAttribVal<uint8>(attribPtr, ptnum0, ptnum1, bias);
                    continue;
                    break;
                case GA_STORE_INT8:
                    setAttribVal<int8>(attribPtr, ptnum0, ptnum1, bias);
                    continue;
                    break;
                case GA_STORE_INT16:
                    setAttribVal<int16>(attribPtr, ptnum0, ptnum1, bias);
                    continue;
                    break;
                case GA_STORE_INT32:
                    setAttribVal<int32>(attribPtr, ptnum0, ptnum1, bias);
                    continue;
                    break;
                case GA_STORE_INT64:
                    setAttribVal<int64>(attribPtr, ptnum0, ptnum1, bias);
                    continue;
                    break;
                case GA_STORE_REAL16:
                    setAttribVal<fpreal16>(attribPtr, ptnum0, ptnum1, bias);
                    continue;
                    break;
                case GA_STORE_REAL32:
                    setAttribVal<fpreal32>(attribPtr, ptnum0, ptnum1, bias);
                    continue;
                    break;
                case GA_STORE_REAL64:
                    setAttribVal<fpreal64>(attribPtr, ptnum0, ptnum1, bias);
                    continue;
                    break;
                case GA_STORE_STRING:
                    continue;
                    break;
                case GA_STORE_DICT:
                    continue;
                    break;
                case GA_STORE_INVALID:
                    break;
                default:
                    break;
                }
            }
            else if (attribPtr->getAIFNumericArray())
            {
            }
            else
            {
                UT_ASSERT_MSG(size, "not correct attrib type");
            }
            UT_ASSERT_MSG(size, "not correct attrib type");
        }
    }


//
//
//#define getUVAttribV(datatype, getUVAttrib_uv)\
//        if ( hasVertexAttrib_uv ) {\
//            datatype attrib0 = vertex(0, uvAttribName, vtxnum);\
//            getUVAttrib_uv = attrib0.x;\
//        } else {\
//            datatype attrib0 = point(0, uvAttribName, vertexpoint(0, vtxnum));\
//            getUVAttrib_uv = attrib0.x;\
//        }\
//
//
//
//#define getUVAttribV_primIndex(datatype, getUVAttrib_uv, getUVAttribV_primIndex_vtxnum)\
//        if ( hasVertexAttrib_uv ) {\
//            datatype attrib0 = vertex(0, uvAttribName, vertexindex(0, @primnum, getUVAttribV_primIndex_vtxnum));\
//            getUVAttrib_uv = attrib0.x;\
//        } else {\
//            datatype attrib0 = point(0, uvAttribName, primpoint(0, @primnum, getUVAttribV_primIndex_vtxnum));\
//            getUVAttrib_uv = attrib0.x;\
//        }\
//
//
//#define getUVAttribF(getUVAttribF_attribName)\
//        if ( hasVertexAttrib_uv ) {\
//            getUVAttribF_attribName = vertex(0, uvAttribName, vtxnum);\
//        } else {\
//            getUVAttribF_attribName = point(0, uvAttribName, vertexpoint(0, vtxnum));\
//        }\
//
//
//#define getUVAttribF_primIndex(getUVAttribF_attribName, getUVAttribV_primIndex_vtxnum)\
//        if ( hasVertexAttrib_uv ) {\
//            getUVAttribF_attribName = vertex(0, uvAttribName, vertexindex(0, @primnum, getUVAttribV_primIndex_vtxnum));\
//        } else {\
//            getUVAttribF_attribName = point(0, uvAttribName, primpoint(0, @primnum, getUVAttribV_primIndex_vtxnum));\
//        }\
//


    // #define getUVAttribFuvMax(getUVAttribF_attribName)\
        //         if ( hasVertexAttrib_uv ) {\
        //             getUVAttribF_attribName = vertex(0, uvAttribName, vtxnum);\
        //         } else {\
        //             getUVAttribF_attribName = point(0, uvAttribName, vertexpoint(0, vtxnum));\
        //         }\



    template<GFE_CarveType CARVE_TYPE>
    void
        carve()
    {
        const bool carveSpace_isWorld = carveSpace == GFE_CarveSpace::CustomAttrib  ||
                                        carveSpace == GFE_CarveSpace::WorldAverage  ||
                                        carveSpace == GFE_CarveSpace::WorldArcLength;

        UTparallelFor(groupParser.getPrimitiveSplittableRange(), [this, carveSpace_isWorld](const GA_SplittableRange& r)
        {
            fpreal domainUMin, domainUMax, uvmax;
            GA_Offset primvtx0, primpt0, primvtx1, primpt1;
            GA_Offset start, end;
            for (GA_Iterator it(r); it.blockAdvance(start, end); )
            {
                for (GA_Offset primoff = start; primoff < end; ++primoff)
                {
                    const GA_Size numvtx = geo->getPrimitiveVertexCount(primoff);

                    if constexpr(CARVE_TYPE != GFE_CarveType::End)
                    {
                        primvtx0 = geo->getPrimitiveVertexOffset(primoff, 0);
                        primpt0 = geo->vertexPrimitive(primvtx0);
                    }
                    if constexpr(CARVE_TYPE == GFE_CarveType::Start)
                    {
                        if (!groupParser_carveStart.getPrimitiveGroup()->contains(primoff))
                        {
                            carvePrimGroup->setElement(primoff, true);
                            continue;
                        }
                    }

                    if constexpr(CARVE_TYPE != GFE_CarveType::Start)
                    {
                        primvtx1 = geo->getPrimitiveVertexOffset(primoff, numvtx-1);
                        primpt1 = geo->vertexPrimitive(primvtx0);
                    }

                    if constexpr(CARVE_TYPE == GFE_CarveType::End)
                    {
                        if (!groupParser_carveEnd.getPrimitiveGroup()->contains(primoff))
                        {
                            carvePrimGroup->setElement(primoff, true);
                            continue;
                        }
                    }

                    if constexpr(CARVE_TYPE != GFE_CarveType::End)
                    {
                        if (startCarveUAttrib)
                        {
                            switch (startCarveUAttrib->getOwner())
                            {
                            case GA_ATTRIB_PRIMITIVE: domainUMin = startCarveU_h.get(primoff);  break;
                            case GA_ATTRIB_POINT:     domainUMin = startCarveU_h.get(primpt0);  break;
                            case GA_ATTRIB_VERTEX:    domainUMin = startCarveU_h.get(primvtx0); break;
                            default: UT_ASSERT_MSG(0, "unhandled carveU Attrib class");         break;
                            }
                        }
                        else
                        {
                            domainUMin = startCarveU;
                        }

                        if constexpr(CARVE_TYPE == GFE_CarveType::Both)
                        {
                            if (carveSpace_isWorld)
                            {
                                switch (uvAttrib->getOwner())
                                {
                                case GA_ATTRIB_POINT:  uvmax = uvAttrib_h.get(primpt1);  break;
                                case GA_ATTRIB_VERTEX: uvmax = uvAttrib_h.get(primvtx1); break;
                                default: UT_ASSERT_MSG(0, "unhandled uv class");         break;
                                }

                                if (groupParser_carveStart.getPrimitiveGroup()->contains(primoff) && domainUMin >= uvmax)
                                {
                                    carvePrimGroup->setElement(primoff, true);
                                    continue;
                                }
                            }
                            else
                            {
                                uvmax = 1;
                                if (groupParser_carveStart.getPrimitiveGroup()->contains(primoff) && domainUMin >= 1)
                                {
                                    carvePrimGroup->setElement(primoff, true);
                                    continue;
                                }
                            }
                        }
                        else if (domainUMin <= 0)
                        {
                            carvePrimGroup->setElement(primoff, true);
                            continue;
                        }
                    }
                


                    if constexpr(CARVE_TYPE != GFE_CarveType::Start) {
                        if (endCarveUAttrib)
                        {
                            switch (endCarveUAttrib->getOwner())
                            {
                            case GA_ATTRIB_PRIMITIVE: domainUMax = endCarveU_h.get(primoff);  break;
                            case GA_ATTRIB_POINT:     domainUMax = endCarveU_h.get(primpt0);  break;
                            case GA_ATTRIB_VERTEX:    domainUMax = endCarveU_h.get(primvtx0); break;
                            default: break;
                            }
                        }
                        else
                        {
                            domainUMax = endCarveU;
                        }
                        
                        // cout(domainUMax);
                        if (!absCarveUEnd)
                        {
                            if constexpr(CARVE_TYPE != GFE_CarveType::Both)
                            {
                                // if ( domainUMax <= domainUMin ) {
                                if (domainUMax <= 0)
                                {
                                    carvePrimGroup->setElement(primoff, true);
                                    continue;
                                }
                            }
                            if (carveSpace_isWorld)
                            {
                                fpreal uvmax2;
                                switch (uvAttrib->getOwner())
                                {
                                case GA_ATTRIB_POINT:  uvmax2 = uvAttrib_h.get(primpt1);  break;
                                case GA_ATTRIB_VERTEX: uvmax2 = uvAttrib_h.get(primvtx1); break;
                                default: UT_ASSERT_MSG(0, "unhandled uv class"); break;
                                }
                                domainUMax = uvmax2 - domainUMax;
                            }
                            else
                            {
                                domainUMax = 1.0 - domainUMax;
                            }
                        }
                        // cout(domainUMax);
                        if constexpr(CARVE_TYPE == GFE_CarveType::Both)
                        {
                            // if ( inpointgroup(0, chs("../secondptgroup"), primpt1) && domainUMax <= domainUMin ) {
                            if (groupParser_carveEnd.getPrimitiveGroup()->contains(primoff) && domainUMax <= 0)
                            {
                                carvePrimGroup->setElement(primoff, true);
                                continue;
                            }
                        }
                    }


                    if constexpr(CARVE_TYPE == GFE_CarveType::Both)
                    {
                        if (delReversedInsidePrim && domainUMax < domainUMin)
                        {
                            carvePrimGroup->setElement(primoff, true);
                            continue;
                        }
                    }


                    fpreal uv_rest = 0;

                    fpreal uv;
                    fpreal step;
                    if (carveSpace == GFE_CarveSpace::LocalAverage)
                    {
                        uv = CARVE_TYPE;
                        step = 1.0 / (numvtx - 1);
                    }


                }
            }
#if 0
#if 0
#if 0
#if 0

        /////////////////// start ///////////////////

        if constexpr(CARVE_TYPE != GFE_CarveType::End) {

#define carveKernelType 0



            {
                if constexpr(CARVE_TYPE == GFE_CarveType::Both) {
                    if (inpointgroup(0, chs("../firstptgroup"), primpt0)) {
#if carveKernelType
                        if (domainUMax <= uvmax) {
#else
                        if (domainUMin >= 0) {
#endif
#endif
                            uv = carveKernelType;
#if carveKernelType
                            for (int vtxpnum = @numvtx - 1; vtxpnum >= 0; --vtxpnum)
                            {
#else
                            for (int vtxpnum = 0; vtxpnum < @numvtx; ++vtxpnum)
                            {
#endif
                                int vtxnum = vertexindex(0, @primnum, vtxpnum);

                                if (chi("../carveSpace") != 1) {
                                    if (uvAttribSize == 3) { // float
                                        getUVAttribV(vector, uv)
                                    }
                                    else if (uvAttribSize == 2) {
                                        getUVAttribV(vector2, uv)
                                    }
                                    else if (uvAttribSize == 4) {
                                        getUVAttribV(vector4, uv)
                                    }
                                    else {
                                        getUVAttribF(uv)
                                    }
                                }

#if carveKernelType
                                if (uv <= domainUMax) {
                                    int breakvtxpnum = vtxpnum + (uv != domainUMax);
                                    if constexpr(CARVE_TYPE == GFE_CarveType::Both) {
                                        for (int vtxpnum_prev = @numvtx - 1; vtxpnum_prev > breakvtxpnum; --vtxpnum_prev) {
                                        }
                                    else
                                    {
                                        for (int vtxpnum_prev = 0; vtxpnum_prev < breakvtxpnum - 1; ++vtxpnum_prev) {
                                        }
                                        setvertexgroup(0, chs("../tmpGroup_blastVertices"), @primnum, vtxpnum_prev, 1);
                                    }

                                    if (vtxpnum == @numvtx - 1 && uv == domainUMax) break;
                                    if constexpr(CARVE_TYPE == GFE_CarveType::Both) {
                                        if (chi('../outBreakPtsGrp_inside1')) {
                                            setpointgroup(0, chs("../breakPtsGrp_inside1"), vertexpoint(0, vertexindex(0, @primnum, breakvtxpnum)), 1);
                                        }
                                    }
                                    else
                                    {
                                        if (chi('../outBreakPtsGrp_outside2')) {
                                            setpointgroup(0, chs("../breakPtsGrp_outside2"), vertexpoint(0, vtxnum), 1);
                                        }
                                    }
                                    if (uv == domainUMax) break;


                                    if constexpr(CARVE_TYPE == GFE_CarveType::Both) {
                                        interpAttribs(0, attribNames, vertexpoint(0, vertexindex(0, @primnum, breakvtxpnum)), vertexpoint(0, vtxnum), (domainUMax - uv_rest) / (uv - uv_rest));
                                    }
                                    else {
                                        interpAttribs(0, attribNames, vertexpoint(0, vtxnum), vertexpoint(0, vertexindex(0, @primnum, breakvtxpnum)), 1 - (domainUMax - uv_rest) / (uv - uv_rest));
                                    }
                                    break;
                                    }
                                }
                                else {
                                    if (uv >= domainUMin)
                                    {
                                        int breakvtxpnum = vtxpnum - (uv != domainUMin);
                                        if constexpr(CARVE_TYPE == GFE_CarveType::Both) {
                                            for (int vtxpnum_prev = 0; vtxpnum_prev < breakvtxpnum; ++vtxpnum_prev)
                                            {
                            }
                                        else {
                                            for (int vtxpnum_prev = @numvtx - 1; vtxpnum_prev > breakvtxpnum + 1; --vtxpnum_prev)
                                            {
                                            }
                                            setvertexgroup(0, chs("../tmpGroup_blastVertices"), @primnum, vtxpnum_prev, 1);
                                        }

                                        if (vtxpnum == 0 && uv == domainUMin) break;
                                        if constexpr(CARVE_TYPE == GFE_CarveType::Both) {
                                            if (chi('../outBreakPtsGrp_inside2'))
                                            {
                                                setpointgroup(0, chs("../breakPtsGrp_inside2"), vertexpoint(0, vertexindex(0, @primnum, breakvtxpnum)), 1);
                                            }
                                        }
                                        else {
                                            if (chi('../outBreakPtsGrp_outside1'))
                                            {
                                                setpointgroup(0, chs("../breakPtsGrp_outside1"), vertexpoint(0, vtxnum), 1);
                                            }
                                        }
                                        if (uv == domainUMin) break;


                                        if constexpr(CARVE_TYPE == GFE_CarveType::Both) {
                                            // printf('\n%d', vertexpoint(0, vertexindex(0, @primnum, breakvtxpnum)));
                                            // printf('\n%d', vertexpoint(0, vtxnum));
                                            // printf('\n%f', (domainUMin - uv_rest) / (uv - uv_rest));
                                            interpAttribs(0, attribNames, vertexpoint(0, vertexindex(0, @primnum, breakvtxpnum)), vertexpoint(0, vtxnum), (domainUMin - uv_rest) / (uv - uv_rest));
                                        }
                                        else {
                                            // cout(vertexpoint(0, vtxnum));
                                            // cout('       ');
                                            // cout((domainUMin - uv_rest) / (uv - uv_rest));
                                            interpAttribs(0, attribNames, vertexpoint(0, vtxnum), vertexpoint(0, vertexindex(0, @primnum, breakvtxpnum)), 1 - (domainUMin - uv_rest) / (uv - uv_rest));
                                        }
                                        break;
                        }
#endif

                                        uv_rest = uv;
                                        if (chi("../carveSpace") == 1) {
#if carveKernelType
                                            uv -= step;
#else
                                            uv += step;
#endif
                                        }
                    }
                                    if constexpr(CARVE_TYPE == GFE_CarveType::Both) {
                                    }
                }
            }
        }

#endif
    }



        /////////////////// end ///////////////////

if(CARVE_TYPE!=0) {

#undef carveKernelType
#define carveKernelType 1

        {
if(CARVE_TYPE == GFE_CarveType::Both){
    if (inpointgroup(0, chs("../secondptgroup"), primpt1)) {
        // cout(1);
#if carveKernelType
        if (domainUMax <= uvmax) {
#else
        if (domainUMin >= 0) {
#endif
        }
    }
}
                    uv = carveKernelType;
#if carveKernelType
                    for (int vtxpnum = @numvtx - 1; vtxpnum >= 0; --vtxpnum) {
#else
                    for (int vtxpnum = 0; vtxpnum < @numvtx; ++vtxpnum) {
#endif
                        int vtxnum = vertexindex(0, @primnum, vtxpnum);

                        if (chi("../carveSpace") != 1) {
                            if (uvAttribSize == 3) { // float
                                getUVAttribV(vector, uv)
                            }
                            else if (uvAttribSize == 2) {
                                getUVAttribV(vector2, uv)
                            }
                            else if (uvAttribSize == 4) {
                                getUVAttribV(vector4, uv)
                            }
                            else {
                                getUVAttribF(uv)
                            }
                        }

#if carveKernelType
                        if (uv <= domainUMax) {
                            int breakvtxpnum = vtxpnum + (uv != domainUMax);
                            if constexpr(CARVE_TYPE == GFE_CarveType::Both) {
                                for (int vtxpnum_prev = @numvtx - 1; vtxpnum_prev > breakvtxpnum; --vtxpnum_prev) {
#else
                        for (int vtxpnum_prev = 0; vtxpnum_prev < breakvtxpnum - 1; ++vtxpnum_prev) {
#endif
                            setvertexgroup(0, chs("../tmpGroup_blastVertices"), @primnum, vtxpnum_prev, 1);
                                }

                        if (vtxpnum == @numvtx - 1 && uv == domainUMax) break;
                        if constexpr(CARVE_TYPE == GFE_CarveType::Both) {
                            if (chi('../outBreakPtsGrp_inside1')) {
                                setpointgroup(0, chs("../breakPtsGrp_inside1"), vertexpoint(0, vertexindex(0, @primnum, breakvtxpnum)), 1);
                            }
                        }
                        else {
                            if (chi('../outBreakPtsGrp_outside2')) {
                                setpointgroup(0, chs("../breakPtsGrp_outside2"), vertexpoint(0, vtxnum), 1);
                            }
                        }
                        if (uv == domainUMax) break;


                        if constexpr(CARVE_TYPE == GFE_CarveType::Both) {
                            interpAttribs(0, attribNames, vertexpoint(0, vertexindex(0, @primnum, breakvtxpnum)), vertexpoint(0, vtxnum), (domainUMax - uv_rest) / (uv - uv_rest));
                        }
                        else {
                            interpAttribs(0, attribNames, vertexpoint(0, vtxnum), vertexpoint(0, vertexindex(0, @primnum, breakvtxpnum)), 1 - (domainUMax - uv_rest) / (uv - uv_rest));
                        }
                        break;
                            }
#else
if (uv >= domainUMin) {
    int breakvtxpnum = vtxpnum - (uv != domainUMin);
    if constexpr(CARVE_TYPE == GFE_CarveType::Both) {
        for (int vtxpnum_prev = 0; vtxpnum_prev < breakvtxpnum; ++vtxpnum_prev) {
        }
    else {
        for (int vtxpnum_prev = @numvtx - 1; vtxpnum_prev > breakvtxpnum + 1; --vtxpnum_prev) {
        }
        setvertexgroup(0, chs("../tmpGroup_blastVertices"), @primnum, vtxpnum_prev, 1);
    }

    if (vtxpnum == 0 && uv == domainUMin) break;
    if constexpr(CARVE_TYPE == GFE_CarveType::Both) {
        if (chi('../outBreakPtsGrp_inside2')) {
            setpointgroup(0, chs("../breakPtsGrp_inside2"), vertexpoint(0, vertexindex(0, @primnum, breakvtxpnum)), 1);
        }
    }
    else {
        if (chi('../outBreakPtsGrp_outside1')) {
            setpointgroup(0, chs("../breakPtsGrp_outside1"), vertexpoint(0, vtxnum), 1);
        }
    }
    if (uv == domainUMin) break;


    if constexpr(CARVE_TYPE == GFE_CarveType::Both) {
        // printf('\n%d', vertexpoint(0, vertexindex(0, @primnum, breakvtxpnum)));
        // printf('\n%d', vertexpoint(0, vtxnum));
        // printf('\n%f', (domainUMin - uv_rest) / (uv - uv_rest));
        interpAttribs(0, attribNames, vertexpoint(0, vertexindex(0, @primnum, breakvtxpnum)), vertexpoint(0, vtxnum), (domainUMin - uv_rest) / (uv - uv_rest));
    }
    else {
        // cout(vertexpoint(0, vtxnum));
        // cout('       ');
        // cout((domainUMin - uv_rest) / (uv - uv_rest));
        interpAttribs(0, attribNames, vertexpoint(0, vtxnum), vertexpoint(0, vertexindex(0, @primnum, breakvtxpnum)), 1 - (domainUMin - uv_rest) / (uv - uv_rest));
    }
    break;
    }
#endif

    uv_rest = uv;
    if (chi("../carveSpace") == 1) {
#if carveKernelType
        uv -= step;
#else
        uv += step;
#endif
    }
}
if constexpr(CARVE_TYPE == GFE_CarveType::Both) {
}
                        }
                    }
                    }



#endif


                    }, 1, 1);
    }



}; // End of class GFE_Carve

#endif
