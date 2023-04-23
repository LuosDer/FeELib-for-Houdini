
#pragma once

#ifndef __GFE_SetVectorComponent_h__
#define __GFE_SetVectorComponent_h__

//#include "GFE/GFE_SetVectorComponent.h"

#include "GA/GA_PageHandle.h"
#include "GA/GA_PageIterator.h"



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
        , GFE_GeoFilterRef(geoRef, groupParser.getGOP(), cookparms)
    {
    }

    
    void
        setComputeParm(
            const int8 comp = 0,
            const fpreal attribValF = 0,
            const exint subscribeRatio = 64,
            const exint minGrainSize = 1024
        )
    {
        setHasComputed();
        this->comp = comp;
        this->attribValF = attribValF;
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
        restAttribNames = attribPattern;
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

        const size_t len = getOutAttribArray().size();
        for (size_t i = 0; i < len; ++i)
        {
            attribPtr = getOutAttribArray()[i];
            UT_ASSERT_P(attribPtr);

            if (!attribRefPtr)
            {
                const GFE_Detail* const attribRefGeo = geoRef0 ? geoRef0 : geo;
                const UT_StringHolder& attribRefName = refAttribNames.getIsValid() ? refAttribNames.getNext<UT_StringHolder>() : UT_StringHolder("");
                attribRefPtr = attribRefGeo->findAttribute(attribPtr->getOwner(), attribRefName);
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
        return true;
    }

    template<typename T>
    void setVectorComponent()
    {
        const typename T::value_type attribVal = attribValF;
        const GA_SplittableRange& geoSplittableRange = groupParser.getSplittableRange(attribPtr);
        
        if (attribRefPtr)
        {
            UTparallelFor(geoSplittableRange, [this, attribVal](const GA_SplittableRange& r)
            {
                GA_PageHandleT<T, typename T::value_type, true, true, GA_Attribute, GA_ATINumeric, GA_Detail> attrib_ph(attribPtr);
                GA_PageHandleT<T, typename T::value_type, true, false, const GA_Attribute, const GA_ATINumeric, const GA_Detail> attribRef_ph(attribRefPtr);
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
        else
        {
            UTparallelFor(geoSplittableRange, [this, attribVal](const GA_SplittableRange& r)
            {
                GA_PageHandleT<T, typename T::value_type, true, true, GA_Attribute, GA_ATINumeric, GA_Detail> attrib_ph(attribPtr);
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

    template<typename T>
    void setFloat()
    {
        const T attribVal = static_cast<const T>(attribValF);
        
        const GA_SplittableRange& geoSplittableRange = groupParser.getSplittableRange(attribPtr);
        if (attribRefPtr)
        {
            UTparallelFor(geoSplittableRange, [this, attribVal](const GA_SplittableRange& r)
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
            UTparallelFor(geoSplittableRange, [this, attribVal](const GA_SplittableRange& r)
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
    UFE_SplittableString restAttribNames;
    
    
    int8 comp = 0;
    fpreal attribValF = 0;
    bool restAttrib = false;

private:

    GA_Attribute* attribPtr;
    const GA_Attribute* attribRefPtr = nullptr;
    //GA_Attribute* attribRestPtr;
    
    
    
    exint subscribeRatio = 64;
    exint minGrainSize = 1024;
};









#endif
