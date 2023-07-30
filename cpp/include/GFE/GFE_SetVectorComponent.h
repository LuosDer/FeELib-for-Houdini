
#pragma once

#ifndef __GFE_SetVectorComponent_h__
#define __GFE_SetVectorComponent_h__

#include "GFE/GFE_SetVectorComponent.h"

#include "GFE/GFE_GeoFilter.h"



#include "GFE/GFE_RestVectorComponent.h"


class GFE_SetVectorComponent : public GFE_AttribFilterWithRef0 {

public:

    //using GFE_AttribFilterWithRef0::GFE_AttribFilterWithRef0;

    GFE_SetVectorComponent(
        GFE_Detail* const geo,
        const GA_Detail* const geoRef,
        const SOP_NodeVerb::CookParms* const cookparms = nullptr
    )
        : GFE_AttribFilterWithRef0(geo, geoRef, cookparms)
        , restVecComp(geo, nullptr, cookparms)
    {
    }

    GFE_SetVectorComponent(
        GA_Detail& geo,
        const GA_Detail* const geoRef,
        const SOP_NodeVerb::CookParms* const cookparms = nullptr
    )
        : GFE_AttribFilterWithRef0(geo, geoRef, cookparms)
        , restVecComp(geo, nullptr, cookparms)
    {
    }

    
    void
        setComputeParm(
            const int8 comp = 1,
            const fpreal attribValF = 0,
            const bool delRefAttrib = true,
            const exint subscribeRatio = 64,
            const exint minGrainSize   = 1024
        )
    {
        setHasComputed();
        this->comp = comp;
        this->attribValF = attribValF;
        this->delRefAttrib = delRefAttrib;
        this->subscribeRatio = subscribeRatio;
        this->minGrainSize   = minGrainSize;
    }
    
    SYS_FORCE_INLINE void setComp(const int8 v = 1)
    { comp = v; }
    
    SYS_FORCE_INLINE void setComp(const GFE_Axis axis = GFE_Axis::Y)
    { comp = static_cast<int8>(axis); }
    
    void setRefAttrib(const GA_Attribute* const attrib)
    {
        if (getOutAttribArray().isEmpty() || attrib->getOwner() != getOutAttribArray()[0]->getOwner())
        {
            if (cookparms)
                cookparms->sopAddWarning(0, "must add out attrib before add ref attrib");
            return;
        }
        attribRef = attrib;
        refAttribNames = "";
        UT_ASSERT(getOutAttribArray().isEmpty() ? 1 : attrib->getOwner() == getOutAttribArray()[0]->getOwner());
    }
    
    SYS_FORCE_INLINE void setRefAttrib(GA_Attribute* const attrib)
    {
        attribRefPtrNonConst = attrib;
        setRefAttrib(static_cast<const GA_Attribute*>(attrib));
    }
    
    // SYS_FORCE_INLINE GA_Attribute* setRefAttrib(const UT_StringHolder& attribPattern)
    // {
    //     // if (!attribPattern.isstring() || attribPattern.length() == 0)
    //     //     attribRef = nullptr;
    //     if (getOutAttribArray().isEmpty())
    //         return nullptr;
    //     const GFE_Detail* const inGeo = geoRef0 ? geoRef0 : geo;
    //     attribRef = inGeo->findAttribute(getOutAttribArray()[0]->getOwner(), attribPattern);
    //     return attribRef;
    // }
    
    SYS_FORCE_INLINE void setRefAttrib(const char* const attribPattern)
    { refAttribNames = attribPattern; }

    //SYS_FORCE_INLINE void setRefAttrib(const UT_StringRef& attribPattern)
    //{ refAttribNames = attribPattern; }


    
    // SYS_FORCE_INLINE void setRestAttrib(GA_Attribute* const attribPtr)
    // {
    //     attribRestPtr = attribPtr;
    //     UT_ASSERT(getOutAttribArray().isEmpty() ? 1 : attribRestPtr->getOwner() == getOutAttribArray()[0]->getOwner());
    // }
    // SYS_FORCE_INLINE GA_Attribute* setRestAttrib(const UT_StringHolder& attribPattern)
    // {
    //     // if (!attribPattern.isstring() || attribPattern.length() == 0)
    //     //     attribRef = nullptr;
    //     
    //     // UT_ASSERT(!getOutAttribArray().isEmpty());
    //     
    //     if (getOutAttribArray().isEmpty())
    //         return nullptr;
    //     attribRestPtr = geo->findAttribute(getOutAttribArray()[0]->getOwner(), attribPattern);
    //     return attribRestPtr;
    // }
    
    SYS_FORCE_INLINE void clearRestAttrib()
    { restVecComp.getOutAttribArray().clear(); }
    
    SYS_FORCE_INLINE void setRestAttrib()
    { rest = false; }
    
    SYS_FORCE_INLINE void setRestAttrib(const char* const attribPattern)
    { rest = true; restVecComp.newAttribNames = attribPattern; }
    
    SYS_FORCE_INLINE const GFE_AttributeArray& getRestAttrib() const
    { return restVecComp.getOutAttribArray(); }
    
    SYS_FORCE_INLINE virtual void bumpDataId() const override
    {
        GFE_AttribFilterWithRef0::bumpDataId();
        restVecComp.bumpDataId();
    }
    
private:


    virtual bool
        computeCore() override
    {
        UT_ASSERT(comp >= 0);
        UT_ASSERT(comp <= 3);
        if (comp < 0 || comp > 3)
            return false;
        
        if (groupParser.isEmpty())
            return true;

        if (rest)
        {
            restVecComp.groupParser.setGroup(groupParser);
            restVecComp.comp = comp;
        }
        
        const size_t len = getOutAttribArray().size();
        for (size_t i = 0; i < len; ++i)
        {
            attribPtr = getOutAttribArray()[i];
            UT_ASSERT_P(attribPtr);

            if (rest)
            {
                restVecComp.setRestAttrib(attribPtr);
                restVecComp.compute();
            }
            
            if (refAttribNames.getIsValid())
            {
                if (geoRef0)
                {
                    attribRef = geoRef0->findAttribute(attribPtr->getOwner(), refAttribNames.getNext<UT_StringHolder>());
                    if (attribRef && !attribRef->getAIFTuple())
                        attribRef = nullptr;
                }
                else
                {
                    attribRefPtrNonConst = geo->findAttribute(attribPtr->getOwner(), refAttribNames.getNext<UT_StringHolder>());
                    if (attribRefPtrNonConst && !attribRefPtrNonConst->getAIFTuple())
                        attribRefPtrNonConst = nullptr;
                    
                    attribRef = attribRefPtrNonConst;
                }
            }
            
            const GA_AIFTuple* const aifTuple = attribPtr->getAIFTuple();
            if (!aifTuple)
                continue;

            const int tupleSize = attribPtr->getTupleSize();
            if (comp >= tupleSize)
                continue;
            
            const GA_Storage storage = aifTuple->getStorage(attribPtr);
            switch (tupleSize)
            {
            case 1:
                switch (storage)
                {
                case GA_STORE_REAL16: setFloat<fpreal16>(); break;
                case GA_STORE_REAL32: setFloat<fpreal32>(); break;
                case GA_STORE_REAL64: setFloat<fpreal64>(); break;
                default: break;
                }
            break;
            case 2:
                switch (storage)
                {
                case GA_STORE_REAL16: setVectorComponent<UT_Vector2T<fpreal16>>(); break;
                case GA_STORE_REAL32: setVectorComponent<UT_Vector2T<fpreal32>>(); break;
                case GA_STORE_REAL64: setVectorComponent<UT_Vector2T<fpreal64>>(); break;
                default: break;
                }
            break;
            case 3:
                switch (storage)
                {
                case GA_STORE_REAL16: setVectorComponent<UT_Vector3T<fpreal16>>(); break;
                case GA_STORE_REAL32: setVectorComponent<UT_Vector3T<fpreal32>>(); break;
                case GA_STORE_REAL64: setVectorComponent<UT_Vector3T<fpreal64>>(); break;
                default: break;
                }
            break;
            case 4:
                switch (storage)
                {
                case GA_STORE_REAL16: setVectorComponent<UT_Vector4T<fpreal16>>(); break;
                case GA_STORE_REAL32: setVectorComponent<UT_Vector4T<fpreal32>>(); break;
                case GA_STORE_REAL64: setVectorComponent<UT_Vector4T<fpreal64>>(); break;
                default: break;
                }
            break;
            default:break;
            }
        }
        
        if (delRefAttrib && !geoRef0 && attribRefPtrNonConst)
            geo->destroyAttrib(attribRefPtrNonConst);
        return true;
    }

    template<typename T>
    void setVectorComponent()
    {
        using value_type = typename T::value_type;
        if (attribRef)
        {
            // const GA_AIFTuple* const aifTuple = attribRef->getAIFTuple();
            UT_ASSERT_P(attribRef->getAIFTuple());

            const int tupleSize = attribRef->getTupleSize();
            if (comp >= tupleSize && tupleSize >= 2)
                return;
            
            const GA_Storage storage = attribRef->getAIFTuple()->getStorage(attribRef);
            switch (tupleSize)
            {
            case 1:
                switch (storage)
                {
                    case GA_STORE_REAL16: setVectorComponentByRefAttrib_FLOAT<T, fpreal16>(); break;
                    case GA_STORE_REAL32: setVectorComponentByRefAttrib_FLOAT<T, fpreal32>(); break;
                    case GA_STORE_REAL64: setVectorComponentByRefAttrib_FLOAT<T, fpreal64>(); break;
                    default: break;
                }
            break;
            case 2:
                switch (storage)
                {
                    case GA_STORE_REAL16: setVectorComponentByRefAttrib<T, UT_Vector2T<fpreal16>>(); break;
                    case GA_STORE_REAL32: setVectorComponentByRefAttrib<T, UT_Vector2T<fpreal32>>(); break;
                    case GA_STORE_REAL64: setVectorComponentByRefAttrib<T, UT_Vector2T<fpreal64>>(); break;
                    default: break;
                }
            break;
            case 3:
                switch (storage)
                {
                    case GA_STORE_REAL16: setVectorComponentByRefAttrib<T, UT_Vector3T<fpreal16>>(); break;
                    case GA_STORE_REAL32: setVectorComponentByRefAttrib<T, UT_Vector3T<fpreal32>>(); break;
                    case GA_STORE_REAL64: setVectorComponentByRefAttrib<T, UT_Vector3T<fpreal64>>(); break;
                    default: break;
                }
            break;
            case 4:
                switch (storage)
                {
                    case GA_STORE_REAL16: setVectorComponentByRefAttrib<T, UT_Vector4T<fpreal16>>(); break;
                    case GA_STORE_REAL32: setVectorComponentByRefAttrib<T, UT_Vector4T<fpreal32>>(); break;
                    case GA_STORE_REAL64: setVectorComponentByRefAttrib<T, UT_Vector4T<fpreal64>>(); break;
                    default: break;
                }
            break;
            default:
            break;
            }
        }
        else
        {
            const value_type attribVal = attribValF;
            UTparallelFor(groupParser.getSplittableRange(attribPtr), [this, attribVal](const GA_SplittableRange& r)
            {
                GA_PageHandleT<T, value_type, true, true, GA_Attribute, GA_ATINumeric, GA_Detail> attrib_ph(attribPtr);
                GA_Offset start, end;
                for (GA_PageIterator pit = r.beginPages(); !pit.atEnd(); ++pit)
                {
                    for (GA_Iterator it(pit.begin()); it.blockAdvance(start, end); )
                    {
                        attrib_ph.setPage(start);
                        for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                        {
                            attrib_ph.value(elemoff)[comp] = attribVal;
                        }
                    }
                }
            }, subscribeRatio, minGrainSize);
        }
    }

    template<typename T, typename T_REF>
    void setVectorComponentByRefAttrib()
    {
        UT_ASSERT_P(attribRef);
        UT_ASSERT(attribPtr->getOwner() == attribRef->getOwner());
        UTparallelFor(groupParser.getSplittableRange(attribPtr), [this](const GA_SplittableRange& r)
        {
            GA_PageHandleT<T,     typename T::value_type,     true, true, GA_Attribute, GA_ATINumeric, GA_Detail> attrib_ph(attribPtr);
            GA_PageHandleT<T_REF, typename T_REF::value_type, true, false, const GA_Attribute, const GA_ATINumeric, const GA_Detail> attribRef_ph(attribRef);
            GA_Offset start, end;
            for (GA_PageIterator pit = r.beginPages(); !pit.atEnd(); ++pit)
            {
                for (GA_Iterator it(pit.begin()); it.blockAdvance(start, end); )
                {
                    attrib_ph.setPage(start);
                    attribRef_ph.setPage(start);
                    for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                    {
                        attrib_ph.value(elemoff)[comp] = attribRef_ph.value(elemoff)[comp];
                    }
                }
            }
        }, subscribeRatio, minGrainSize);
    }

    
    template<typename T, typename T_REF>
    void setVectorComponentByRefAttrib_FLOAT()
    {
        UT_ASSERT_P(attribRef);
        UT_ASSERT(attribPtr->getOwner() == attribRef->getOwner());
        UTparallelFor(groupParser.getSplittableRange(attribPtr), [this](const GA_SplittableRange& r)
        {
            GA_PageHandleT<T, typename T::value_type, true, true, GA_Attribute, GA_ATINumeric, GA_Detail> attrib_ph(attribPtr);
            GA_PageHandleT<T_REF, T_REF, true, false, const GA_Attribute, const GA_ATINumeric, const GA_Detail> attribRef_ph(attribRef);
            GA_Offset start, end;
            for (GA_PageIterator pit = r.beginPages(); !pit.atEnd(); ++pit)
            {
                for (GA_Iterator it(pit.begin()); it.blockAdvance(start, end); )
                {
                    attrib_ph.setPage(start);
                    attribRef_ph.setPage(start);
                    for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                    {
                        attrib_ph.value(elemoff)[comp] = attribRef_ph.value(elemoff);
                    }
                }
            }
        }, subscribeRatio, minGrainSize);
    }
    
    template<typename T>
    void setFloat()
    {
        if (attribRef)
        {
            UTparallelFor(groupParser.getSplittableRange(attribPtr), [this](const GA_SplittableRange& r)
            {
                GA_PageHandleT<T, T, true, true, GA_Attribute, GA_ATINumeric, GA_Detail> attrib_ph(attribPtr);
                GA_PageHandleT<T, T, true, false, const GA_Attribute, const GA_ATINumeric, const GA_Detail> attribRef_ph(attribRef);
                GA_Offset start, end;
                for (GA_PageIterator pit = r.beginPages(); !pit.atEnd(); ++pit)
                {
                    for (GA_Iterator it(pit.begin()); it.blockAdvance(start, end); )
                    {
                        attrib_ph.setPage(start);
                        attribRef_ph.setPage(start);
                        for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                        {
                            attrib_ph.value(elemoff) = attribRef_ph.value(elemoff);
                        }
                    }
                }
            }, subscribeRatio, minGrainSize);
        }
        else
        {
            const T attribVal = static_cast<const T>(attribValF);
            UTparallelFor(groupParser.getSplittableRange(attribPtr), [this, attribVal](const GA_SplittableRange& r)
            {
                GA_PageHandleT<T, T, true, true, GA_Attribute, GA_ATINumeric, GA_Detail> attrib_ph(attribPtr);
                GA_Offset start, end;
                for (GA_PageIterator pit = r.beginPages(); !pit.atEnd(); ++pit)
                {
                    for (GA_Iterator it(pit.begin()); it.blockAdvance(start, end); )
                    {
                        attrib_ph.setPage(start);
                        for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                        {
                            attrib_ph.value(elemoff) = attribVal;
                        }
                    }
                }
            }, subscribeRatio, minGrainSize);
        }
    }


public:
    UFE_SplittableString refAttribNames;
    // UFE_SplittableString restAttribNames;
    
    int8 comp = 1;
    fpreal attribValF = 0;
    
    bool delRefAttrib = false;
    // bool restAttrib = false;

private:

    GA_Attribute* attribPtr;
    const GA_Attribute* attribRef = nullptr;
    GA_Attribute* attribRefPtrNonConst = nullptr;
    //GA_Attribute* attribRestPtr;
    
    bool rest = false;
    GFE_RestVectorComponent restVecComp;
    
    
    exint subscribeRatio = 64;
    exint minGrainSize = 1024;
};









#endif
