
#pragma once

#ifndef __GFE_SetVectorComponent_h__
#define __GFE_SetVectorComponent_h__

#include "GFE/GFE_SetVectorComponent.h"


#include "GFE/GFE_GeoFilter.h"

#include "GFE/GFE_RestVectorComponent.h"


class GFE_SetVectorComponent : public GFE_AttribFilter, public GFE_GeoFilterRef {

public:

    //using GFE_AttribFilter::GFE_AttribFilter;

    GFE_SetVectorComponent(
        GA_Detail& geo,
        const GA_Detail* const geoRef,
        const SOP_NodeVerb::CookParms* const cookparms = nullptr
    )
        : GFE_AttribFilter(geo, cookparms)
        , GFE_GeoFilterRef(geoRef, groupParser.getGOPRef(), cookparms)
        , restVectorComponent(geo, nullptr, cookparms)
    {
    }

    
    void
        setComputeParm(
            const int8 comp = 0,
            const fpreal attribValF = 0,
            const bool delRefAttrib = true,
            const exint subscribeRatio = 64,
            const exint minGrainSize = 1024
        )
    {
        setHasComputed();
        this->comp = comp;
        this->attribValF = attribValF;
        this->delRefAttrib = delRefAttrib;
        this->subscribeRatio = subscribeRatio;
        this->minGrainSize = minGrainSize;
    }
    
    SYS_FORCE_INLINE void setRefAttrib(const GA_Attribute* const attribPtr)
    {
        if(getOutAttribArray().isEmpty() || attribRefPtr->getOwner() != getOutAttribArray()[0]->getOwner())
        {
            if (cookparms)
                cookparms->sopAddWarning(0, "must add out attrib before add ref attrib");
        }
        attribRefPtr = attribPtr;
        refAttribNames = "";
        UT_ASSERT(getOutAttribArray().isEmpty() ? 1 : attribRefPtr->getOwner() == getOutAttribArray()[0]->getOwner());
    }
    // SYS_FORCE_INLINE GA_Attribute* setRefAttrib(const UT_StringHolder& attribPattern)
    // {
    //     // if (!attribPattern.isstring() || attribPattern.length() == 0)
    //     //     attribRefPtr = nullptr;
    //     if (getOutAttribArray().isEmpty())
    //         return nullptr;
    //     const GFE_Detail* const inGeo = geoRef0 ? geoRef0 : geo;
    //     attribRefPtr = inGeo->findAttribute(getOutAttribArray()[0]->getOwner(), attribPattern);
    //     return attribRefPtr;
    // }
    SYS_FORCE_INLINE void setRefAttrib(const UT_StringHolder& attribPattern)
    {
        refAttribNames = attribPattern;
    }


    
    // SYS_FORCE_INLINE void setRestAttrib(GA_Attribute* const attribPtr)
    // {
    //     attribRestPtr = attribPtr;
    //     UT_ASSERT(getOutAttribArray().isEmpty() ? 1 : attribRestPtr->getOwner() == getOutAttribArray()[0]->getOwner());
    // }
    // SYS_FORCE_INLINE GA_Attribute* setRestAttrib(const UT_StringHolder& attribPattern)
    // {
    //     // if (!attribPattern.isstring() || attribPattern.length() == 0)
    //     //     attribRefPtr = nullptr;
    //     
    //     // UT_ASSERT(!getOutAttribArray().isEmpty());
    //     
    //     if (getOutAttribArray().isEmpty())
    //         return nullptr;
    //     attribRestPtr = geo->findAttribute(getOutAttribArray()[0]->getOwner(), attribPattern);
    //     return attribRestPtr;
    // }
    SYS_FORCE_INLINE void setRestAttrib(const UT_StringHolder& attribPattern)
    {
        restVectorComponent.newAttribNames = attribPattern;
    }
    
private:


    virtual bool
        computeCore() override
    {
        UT_ASSERT(comp >= 0);
        if (comp < 0)
            return false;
        
        if (groupParser.isEmpty())
            return true;

        if(restVectorComponent.newAttribNames.getIsValid())
        {
            restVectorComponent.groupParser.copy(groupParser);
            restVectorComponent.comp = comp;
        }
        
        const size_t len = getOutAttribArray().size();
        for (size_t i = 0; i < len; ++i)
        {
            attribPtr = getOutAttribArray()[i];
            UT_ASSERT_P(attribPtr);

            if(restVectorComponent.newAttribNames.getIsValid())
            {
                restVectorComponent.setRestAttrib(attribPtr);
                restVectorComponent.compute();
            }
            
            if (refAttribNames.getIsValid())
            {
                if (geoRef0)
                {
                    attribRefPtr = geoRef0->findAttribute(attribPtr->getOwner(), refAttribNames.getNext<UT_StringHolder>());
                    if (attribRefPtr && !attribRefPtr->getAIFTuple())
                        attribRefPtr = nullptr;
                }
                else
                {
                    attribRefPtrNonConst = geo->findAttribute(attribPtr->getOwner(), refAttribNames.getNext<UT_StringHolder>());
                    if (attribRefPtrNonConst && !attribRefPtrNonConst->getAIFTuple())
                        attribRefPtrNonConst = nullptr;
                    
                    attribRefPtr = attribRefPtrNonConst;
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
            default:
                break;
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
        if (attribRefPtr)
        {
            // const GA_AIFTuple* const aifTuple = attribRefPtr->getAIFTuple();
            UT_ASSERT_P(attribRefPtr->getAIFTuple());

            const int tupleSize = attribRefPtr->getTupleSize();
            if (comp >= tupleSize && tupleSize >= 2)
                return;
            
            const GA_Storage storage = attribRefPtr->getAIFTuple()->getStorage(attribRefPtr);
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
                for (GA_PageIterator pit = r.beginPages(); !pit.atEnd(); ++pit)
                {
                    GA_Offset start, end;
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
        UT_ASSERT_P(attribRefPtr);
        UT_ASSERT(attribPtr->getOwner() == attribRefPtr->getOwner());
        UTparallelFor(groupParser.getSplittableRange(attribPtr), [this](const GA_SplittableRange& r)
        {
            GA_PageHandleT<T,     typename T::value_type,     true, true, GA_Attribute, GA_ATINumeric, GA_Detail> attrib_ph(attribPtr);
            GA_PageHandleT<T_REF, typename T_REF::value_type, true, false, const GA_Attribute, const GA_ATINumeric, const GA_Detail> attribRef_ph(attribRefPtr);
            for (GA_PageIterator pit = r.beginPages(); !pit.atEnd(); ++pit)
            {
                GA_Offset start, end;
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
        UT_ASSERT_P(attribRefPtr);
        UT_ASSERT(attribPtr->getOwner() == attribRefPtr->getOwner());
        UTparallelFor(groupParser.getSplittableRange(attribPtr), [this](const GA_SplittableRange& r)
        {
            GA_PageHandleT<T, typename T::value_type, true, true, GA_Attribute, GA_ATINumeric, GA_Detail> attrib_ph(attribPtr);
            GA_PageHandleT<T_REF, T_REF, true, false, const GA_Attribute, const GA_ATINumeric, const GA_Detail> attribRef_ph(attribRefPtr);
            for (GA_PageIterator pit = r.beginPages(); !pit.atEnd(); ++pit)
            {
                GA_Offset start, end;
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
        if (attribRefPtr)
        {
            UTparallelFor(groupParser.getSplittableRange(attribPtr), [this](const GA_SplittableRange& r)
            {
                GA_PageHandleT<T, T, true, true, GA_Attribute, GA_ATINumeric, GA_Detail> attrib_ph(attribPtr);
                GA_PageHandleT<T, T, true, false, const GA_Attribute, const GA_ATINumeric, const GA_Detail> attribRef_ph(attribRefPtr);
                for (GA_PageIterator pit = r.beginPages(); !pit.atEnd(); ++pit)
                {
                    GA_Offset start, end;
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
                for (GA_PageIterator pit = r.beginPages(); !pit.atEnd(); ++pit)
                {
                    GA_Offset start, end;
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


protected:
    UFE_SplittableString refAttribNames;
    // UFE_SplittableString restAttribNames;
    
    int8 comp = 0;
    fpreal attribValF = 0;
    
    bool delRefAttrib = false;
    // bool restAttrib = false;

private:

    GA_Attribute* attribPtr;
    const GA_Attribute* attribRefPtr = nullptr;
    GA_Attribute* attribRefPtrNonConst = nullptr;
    //GA_Attribute* attribRestPtr;
    
    GFE_RestVectorComponent restVectorComponent;
    
    
    exint subscribeRatio = 64;
    exint minGrainSize = 1024;
};









#endif
