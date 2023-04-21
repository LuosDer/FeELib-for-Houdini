
#pragma once

#ifndef __GFE_RestVectorComponent_h__
#define __GFE_RestVectorComponent_h__

//#include "GFE/GFE_RestVectorComponent.h"

#include "GA/GA_PageHandle.h"
#include "GA/GA_PageIterator.h"



#include "GFE/GFE_GeoFilter.h"
#include "UFE/UFE_SplittableString.h"


class GFE_RestVectorComponent : public GFE_AttribCreateFilter, public GFE_GeoFilterRef {
    
#define GFE_TEMP_RESTVECTORMONPONENT_ATTRIBNAME "__GFE_TEMP_RESTVECTORMONPONENT_ATTRIBNAME"
    
public:

    //using GFE_AttribFilter::GFE_AttribFilter;

    GFE_RestVectorComponent(
        GA_Detail& geo,
        const GA_Detail* const geoRef,
        const SOP_NodeVerb::CookParms* const cookparms = nullptr
    )
        : GFE_AttribCreateFilter(geo, cookparms)
        , GFE_GeoFilterRef(geoRef, groupParser.getGOP(), cookparms)
    {
    }

    
    void
        setComputeParm(
            const int8 comp = 0,
            const bool delOrigin = false,
            const exint subscribeRatio = 64,
            const exint minGrainSize = 1024
        )
    {
        setHasComputed();
        this->comp = comp;
        this->delOrigin = delOrigin;
        this->subscribeRatio = subscribeRatio;
        this->minGrainSize = minGrainSize;
    }
    

    SYS_FORCE_INLINE void setRestAttrib(const GA_Attribute& inAttrib)
    {
        newAttribNames = inAttrib;
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

        const size_t len = getInAttribArray().size();
        for (size_t i = 0; i < len; ++i)
        {
            attribRefPtr = getInAttribArray()[i];
            attribRefPtrNonConst = getInAttribArray()[i];
            UT_ASSERT_P(attribRefPtr);
            restVectorComponent();
        }
        const size_t ref0AttribLen = getRef0AttribArray().size();
        for (size_t i = 0; i < len; ++i)
        {
            attribRefPtr = getRef0AttribArray()[i];
            attribRefPtrNonConst = nullptr;
            UT_ASSERT_P(attribRefPtr);
            restVectorComponent();
        }
        return true;
    }

    void restVectorComponent()
    {
        const GA_Attribute& attribRef = *attribRefPtr;
        
        
        const UT_StringHolder& newName = newAttribNames.getIsValid() ? newAttribNames.getNext<UT_StringHolder>() : attribRef.getName();
        const bool detached = !newName.isstring() || newName.length() == 0;

        const GA_Storage storage = GFE_Attribute::getStorage(attribRef);
        //if(!detached && !attrib.isDetached() && attrib.getName() == newName)
        if(!detached && !attribRef.isDetached() && strcmp(attribRef.getName().c_str(), newName.c_str()) == 0)
        {
            attribRestPtr = getOutAttribArray().findOrCreateTuple(
                false, attribRef.getOwner(), GA_STORECLASS_FLOAT, storage, GFE_TEMP_RESTVECTORMONPONENT_ATTRIBNAME);
            
            attribDuplicate();
            
            if (attribRefPtrNonConst)
                geo->destroyAttrib(attribRefPtrNonConst);
            geo->renameAttrib(attribRestPtr, newName);
        }
        else
        {
            attribRestPtr = getOutAttribArray().findOrCreateTuple(
                detached, attribRef.getOwner(), GA_STORECLASS_FLOAT, storage, newName);
            
            attribDuplicate();
            
            if(delOrigin && attribRefPtrNonConst)
                geo->destroyNonDetachedAttrib(attribRefPtrNonConst);
        }
    }
        

    void attribDuplicate()
    {
        const GA_AIFTuple* const aifTuple = attribRefPtr->getAIFTuple();
        if (!aifTuple)
            return;

        const int tupleSize = attribRefPtr->getTupleSize();
        if (comp >= tupleSize)
            return;
        
        const GA_Storage storage = aifTuple->getStorage(attribRefPtr);
        switch (tupleSize)
        {
        case 1:
            switch (storage)
            {
                case GA_STORE_REAL16: restFloat<fpreal16>(); break;
                case GA_STORE_REAL32: restFloat<fpreal32>(); break;
                case GA_STORE_REAL64: restFloat<fpreal64>(); break;
                default: break;
            }
        case 2:
            switch (storage)
            {
                case GA_STORE_REAL16: restVectorComponent<UT_Vector2T<fpreal16>>(); break;
                case GA_STORE_REAL32: restVectorComponent<UT_Vector2T<fpreal32>>(); break;
                case GA_STORE_REAL64: restVectorComponent<UT_Vector2T<fpreal64>>(); break;
                default: break;
            }
        break;
        case 3:
            switch (storage)
            {
                case GA_STORE_REAL16: restVectorComponent<UT_Vector3T<fpreal16>>(); break;
                case GA_STORE_REAL32: restVectorComponent<UT_Vector3T<fpreal32>>(); break;
                case GA_STORE_REAL64: restVectorComponent<UT_Vector3T<fpreal64>>(); break;
                default: break;
            }
        break;
        case 4:
            switch (storage)
            {
                case GA_STORE_REAL16: restVectorComponent<UT_Vector4T<fpreal16>>(); break;
                case GA_STORE_REAL32: restVectorComponent<UT_Vector4T<fpreal32>>(); break;
                case GA_STORE_REAL64: restVectorComponent<UT_Vector4T<fpreal64>>(); break;
                default: break;
            }
        break;
        default:
        break;
        }
    }

    
    template<typename T>
    void restVectorComponent()
    {
        UTparallelFor(groupParser.getSplittableRange(attribRefPtr), [this](const GA_SplittableRange& r)
        {
            GA_PageHandleT<T, typename T::value_type, true, true, GA_Attribute, GA_ATINumeric, GA_Detail> attrib_ph(attribRestPtr);
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
                        attrib_ph.value(elemoff) = attribRef_ph.value(elemoff)[comp];
                    }
                }
            }
        }, subscribeRatio, minGrainSize);
    }

    template<typename T>
    void restFloat()
    {
        UTparallelFor(groupParser.getSplittableRange(attribRefPtr), [this](const GA_SplittableRange& r)
        {
            GA_PageHandleT<T, T, true, true, GA_Attribute, GA_ATINumeric, GA_Detail> attrib_ph(attribRestPtr);
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


protected:
    UFE_SplittableString newAttribNames;
    
    int8 comp = 0;
    bool delOrigin = false;
private:

    GA_Attribute* attribRestPtr;
    
    GA_Attribute* attribRefPtrNonConst;
    const GA_Attribute* attribRefPtr;
    
    
    
    exint subscribeRatio = 64;
    exint minGrainSize = 1024;

#undef GFE_TEMP_RESTVECTORMONPONENT_ATTRIBNAME
    
};








#endif
