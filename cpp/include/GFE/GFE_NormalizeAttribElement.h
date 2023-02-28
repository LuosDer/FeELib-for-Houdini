
#pragma once

#ifndef __GFE_NormalizeAttribElement_h__
#define __GFE_NormalizeAttribElement_h__

//#include "GFE/GFE_NormalizeAttribElement.h"

#include "GA/GA_Detail.h"


#include "GA/GA_AttributeFilter.h"
#include "GA/GA_PageHandle.h"
#include "GA/GA_PageIterator.h"

#include "GFE/GFE_GroupParse.h"
#include "GFE/GFE_Range.h"

#if 1



class GFE_NormalizeAttribElement {


public:

    GFE_NormalizeAttribElement(
        GA_Detail* const geo,
        const SOP_NodeVerb::CookParms* const cookparms = nullptr
    )
        : geo(geo)
        , cookparms(cookparms)
    {
        UT_ASSERT_MSG(geo, "do not find geo");
    }

    GFE_NormalizeAttribElement(
        const SOP_NodeVerb::CookParms& cookparms,
        GA_Detail* const geo
    )
        : geo(geo)
        , cookparms(&cookparms)
    {
        UT_ASSERT_MSG(geo, "do not find geo");
    }

GFE_NormalizeAttribElement(
    const GA_Detail* const geo,
    const GA_ElementGroup* const geoGroup,
    const GA_AttributeOwner attribClass,
    const bool doNormalize = true,
    const fpreal64 uniScale = 1
)
    : geo(geo)
    , geoGroup(geoGroup)
    , attribClass(attribClass)
    , doNormalize(doNormalize)
    , uniScale(uniScale)
{
}


~GFE_NormalizeAttribElement()
{
}



void
setAttrib(
    const GA_AttributeOwner attribClass,
    const UT_StringHolder& attribPattern,
    const bool doNormalize = true,
    const fpreal64 uniScale = 1.0,
    const exint subscribeRatio = 64,
    const exint minGrainSize = 64
)
{
    if (!attribPattern.isstring() || attribPattern.length() == 0)
        return;

    hasParm_outAttrib = true;

    this->attribClass = attribClass;
    //this->attribPattern = attribPattern;
    this->doNormalize = doNormalize;
    this->uniScale = uniScale;
    this->subscribeRatio = subscribeRatio;
    this->minGrainSize = minGrainSize;


    GA_Attribute* attribPtr = nullptr;
    for (GA_AttributeDict::iterator it = geo->getAttributes().begin(attribClass); !it.atEnd(); ++it)
    {
        attribPtr = it.attrib();
        if (!attribPtr->getName().multiMatch(attribPattern))
            continue;
        attribArray.emplace_back(attribPtr);
    }
}



void
setRange(
    const GA_SplittableRange& geoSplittableRange
)
{
    hasParm_geoSplittableRange = true;
    this->geoSplittableRange = geoSplittableRange;
}

void
setRange()
{
    if (!hasParm_inGroup)
        setInGroup();
    hasParm_geoSplittableRange = true;
    //const GA_SplittableRange& geoSplittableRange(GA_Range(geo->getIndexMap(attribClass), geoGroup));
    //const GA_SplittableRange geo0SplittableRange = GFE_Range::getSplittableRangeByAnyGroup(geo, geoGroup, attribClass);
    this->geoSplittableRange = GFE_Range::getSplittableRangeByAnyGroup(geo, geoGroup, attribClass);
}


void
setInGroup(
    const GA_Group* const geoGroup = nullptr
)
{
    hasParm_inGroup = true;
    this->geoGroup = geoGroup;
}

void
setInGroup(
    const GA_GroupType groupType,
    const UT_StringHolder& groupName
)
{
    hasParm_inGroup = true;
    groupParse.setParm(cookparms);
    groupParse.setParm(geo);
    groupParse.setParm(groupName);
    geoGroup = groupParse.parseConst(groupType);
    //geoGroup = GFE_Group::findOrParseGroupDetached(cookparms, geo, groupType, groupName, gop);
    setRange();
}








void
compute()
{
    if (!hasParm_geoSplittableRange)
        setRange();
    if (!hasParm_outAttrib)
        return;

    if (geoGroup && geoGroup->isEmpty())
        return;

    for (int i = 0; i < attribArray.size(); i++)
    {
        normalizeAttribElement(attribArray[i]);
    }
}

void
bumpDataId() const
{
    for (int i = 0; i < attribArray.size(); i++)
    {
        attribArray[i]->bumpDataId();
    }
}


const std::vector<GA_Attribute*>&
getAttrib() const
{
    return attribArray;
}


protected:

    template<typename VECTOR_T>
    void
        normalizeAttribElement(
            GA_Attribute* const attribPtr
        ) const
    {
        UTparallelFor(geoSplittableRange, [attribPtr, this](const GA_SplittableRange& r)
        {
            GA_PageHandleT<VECTOR_T, typename VECTOR_T::value_type, true, true, GA_Attribute, GA_ATINumeric, GA_Detail> attrib_ph(attribPtr);
            for (GA_PageIterator pit = r.beginPages(); !pit.atEnd(); ++pit)
            {
                GA_Offset start, end;
                for (GA_Iterator it(pit.begin()); it.blockAdvance(start, end); )
                {
                    attrib_ph.setPage(start);
                    for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                    {
                        if (doNormalize)
                            attrib_ph.value(elemoff).normalize();
                        attrib_ph.value(elemoff) *= uniScale;
                    }
                }
            }
        }, subscribeRatio, minGrainSize);
    }


    //#if SYS_VERSION_MAJOR_INT > 19 || ( SYS_VERSION_MAJOR_INT == 19 && SYS_VERSION_MINOR_INT == 5 )
    //template<typename VECTOR_T>
    //static void
    //normalizeAttribElement(
    //    GA_Attribute* const attribPtr,
    //    const GA_Range& geoRange,
    //    const typename VECTOR_T::value_type uniScale = 1
    //)
    //{
    //    GAparallelForEachPage(geoRange, true, [this](GA_PageIterator pit)
    //    {
    //        GA_PageHandleT<VECTOR_T, typename VECTOR_T::value_type, true, true, GA_Attribute, GA_ATINumeric, GA_Detail> attrib_ph(attribPtr);
    //        GAforEachPageBlock(pit, [&attrib_ph, this](GA_Offset start, GA_Offset end)
    //        {
    //            attrib_ph.setPage(start);
    //            for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
    //            {
    //                if (doNormalize)
    //                    attrib_ph.value(elemoff).normalize();
    //                attrib_ph.value(elemoff) *= uniScale;
    //            }
    //        });
    //    });
    //}
    //
    //#endif


    bool
        normalizeAttribElement(
            GA_Attribute* const attribPtr
        ) const
    {
        UT_ASSERT_P(attribPtr);
        const GA_Storage storage = attribPtr->getAIFTuple()->getStorage(attribPtr);
        switch (attribPtr->getAIFTuple()->getTupleSize(attribPtr))
        {
        case 2:
            switch (storage)
            {
            case GA_STORE_REAL16:
                normalizeAttribElement<UT_Vector2T<fpreal16>>(attribPtr);
                break;
            case GA_STORE_REAL32:
                normalizeAttribElement<UT_Vector2T<fpreal32>>(attribPtr);
                break;
            case GA_STORE_REAL64:
                normalizeAttribElement<UT_Vector2T<fpreal64>>(attribPtr);
                break;
            default:
                return false;
                break;
            }
            break;
        case 3:
            switch (storage)
            {
            case GA_STORE_REAL16:
                normalizeAttribElement<UT_Vector3T<fpreal16>>(attribPtr);
                break;
            case GA_STORE_REAL32:
                normalizeAttribElement<UT_Vector3T<fpreal32>>(attribPtr);
                break;
            case GA_STORE_REAL64:
                normalizeAttribElement<UT_Vector3T<fpreal64>>(attribPtr);
                break;
            default:
                return false;
                break;
            }
            break;
        case 4:
            switch (storage)
            {
            case GA_STORE_REAL16:
                normalizeAttribElement<UT_Vector4T<fpreal16>>(attribPtr);
                break;
            case GA_STORE_REAL32:
                normalizeAttribElement<UT_Vector4T<fpreal32>>(attribPtr);
                break;
            case GA_STORE_REAL64:
                normalizeAttribElement<UT_Vector4T<fpreal64>>(attribPtr);
                break;
            default:
                return false;
                break;
            }
            break;
        default:
            return false;
            break;
        }
        return true;
    }




private:
    const SOP_NodeVerb::CookParms* cookparms;
    const GA_Detail* geo;


    bool hasParm_inGroup = false;
    const GA_Group* geoGroup = nullptr;
    GFE_GroupParse groupParse;


    bool hasParm_geoSplittableRange = false;
    GA_SplittableRange geoSplittableRange;


    bool hasParm_outAttrib = false;
    GA_AttributeOwner attribClass;
    //UT_StringHolder attribPattern;
    std::vector<GA_Attribute*> attribArray;
    bool doNormalize;
    fpreal64 uniScale;

    exint subscribeRatio;
    exint minGrainSize;

};

































namespace GFE_NormalizeAttribElement_Namespace {


    //
    //template<typename T>
    //static void
    //normalizeAttribElement(
    //    const GA_RWHandleT<UT_Vector3T<T>>& attrib_h,
    //    const GA_SplittableRange& geoSplittableRange,
    //    const bool doNormalize = true,
    //    const T uniScale = 1,
    //    const exint subscribeRatio = 64,
    //    const exint minGrainSize = 64
    //)
    //{
    //    UTparallelFor(geoSplittableRange, [&attrib_h, doNormalize, uniScale](const GA_SplittableRange& r)
    //    {
    //        UT_Vector3T<T> attribValue;
    //        GA_Offset start, end;
    //        for (GA_Iterator it(r); it.blockAdvance(start, end); )
    //        {
    //            for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
    //            {
    //                attribValue = attrib_h.get(elemoff);
    //                if (doNormalize)
    //                    attribValue.normalize();
    //                attribValue *= uniScale;
    //                attrib_h.set(elemoff, attribValue);
    //            }
    //        }
    //    }, subscribeRatio, minGrainSize);
    //}


    template<typename T>
    static void
        normalizeAttribElement2T(
            GA_Attribute* const attribPtr,
            const GA_SplittableRange& geoSplittableRange,
            const bool doNormalize = true,
            const T uniScale = 1,
            const exint subscribeRatio = 64,
            const exint minGrainSize = 64
        )
    {
        UTparallelFor(geoSplittableRange, [attribPtr, doNormalize, uniScale](const GA_SplittableRange& r)
        {
            UT_Vector2T<T> attribValue;
            GA_PageHandleT<UT_Vector2T<T>, typename UT_Vector2T<T>::value_type, true, true, GA_Attribute, GA_ATINumeric, GA_Detail> attrib_ph(attribPtr);
            for (GA_PageIterator pit = r.beginPages(); !pit.atEnd(); ++pit)
            {
                GA_Offset start, end;
                for (GA_Iterator it(pit.begin()); it.blockAdvance(start, end); )
                {
                    attrib_ph.setPage(start);
                    for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                    {
                        if (doNormalize)
                            attrib_ph.value(elemoff).normalize();
                        attrib_ph.value(elemoff) *= uniScale;
                    }
                }
            }
        }, subscribeRatio, minGrainSize);
    }




#if SYS_VERSION_MAJOR_INT > 19 || ( SYS_VERSION_MAJOR_INT == 19 && SYS_VERSION_MINOR_INT == 5 )
    template<typename T>
    static void
        normalizeAttribElement3T(
            GA_Attribute* const attribPtr,
            const GA_Range& geoRange,
            const bool doNormalize = true,
            const T uniScale = 1,
            const exint subscribeRatio = 64,
            const exint minGrainSize = 64
        )
    {
        GAparallelForEachPage(geoRange, true, [attribPtr, doNormalize, uniScale](GA_PageIterator pit)
        {
            GA_PageHandleT<UT_Vector3T<T>, typename UT_Vector3T<T>::value_type, true, true, GA_Attribute, GA_ATINumeric, GA_Detail> attrib_ph(attribPtr);
            GAforEachPageBlock(pit, [&attrib_ph, doNormalize, uniScale](GA_Offset start, GA_Offset end)
            {
                attrib_ph.setPage(start);
                for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                {
                    if (doNormalize)
                        attrib_ph.value(elemoff).normalize();
                    attrib_ph.value(elemoff) *= uniScale;
                }
            });
        });
    }
#endif


    template<typename T>
    static void
        normalizeAttribElement3T(
            GA_Attribute* const attribPtr,
            const GA_SplittableRange& geoSplittableRange,
            const bool doNormalize = true,
            const T uniScale = 1,
            const exint subscribeRatio = 64,
            const exint minGrainSize = 64
        )
    {
        UTparallelFor(geoSplittableRange, [attribPtr, doNormalize, uniScale](const GA_SplittableRange& r)
        {
            UT_Vector3T<T> attribValue;
            GA_PageHandleT<UT_Vector3T<T>, typename UT_Vector3T<T>::value_type, true, true, GA_Attribute, GA_ATINumeric, GA_Detail> attrib_ph(attribPtr);
            for (GA_PageIterator pit = r.beginPages(); !pit.atEnd(); ++pit)
            {
                GA_Offset start, end;
                for (GA_Iterator it(pit.begin()); it.blockAdvance(start, end); )
                {
                    attrib_ph.setPage(start);
                    for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                    {
                        if (doNormalize)
                            attrib_ph.value(elemoff).normalize();
                        attrib_ph.value(elemoff) *= uniScale;
                    }
                }
            }
        }, subscribeRatio, minGrainSize);
    }

    template<typename T>
    static void
        normalizeAttribElement4T(
            GA_Attribute* const attribPtr,
            const GA_SplittableRange& geoSplittableRange,
            const bool doNormalize = true,
            const T uniScale = 1,
            const exint subscribeRatio = 64,
            const exint minGrainSize = 64
        )
    {
        UTparallelFor(geoSplittableRange, [attribPtr, doNormalize, uniScale](const GA_SplittableRange& r)
        {
            UT_Vector4T<T> attribValue;
            GA_PageHandleT<UT_Vector4T<T>, typename UT_Vector4T<T>::value_type, true, true, GA_Attribute, GA_ATINumeric, GA_Detail> attrib_ph(attribPtr);
            for (GA_PageIterator pit = r.beginPages(); !pit.atEnd(); ++pit)
            {
                GA_Offset start, end;
                for (GA_Iterator it(pit.begin()); it.blockAdvance(start, end); )
                {
                    attrib_ph.setPage(start);
                    for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                    {
                        if (doNormalize)
                            attrib_ph.value(elemoff).normalize();
                        attrib_ph.value(elemoff) *= uniScale;
                    }
                }
            }
        }, subscribeRatio, minGrainSize);
    }


    //GFE_Attribute::normalizeAttribElement(attribPtr, geo0SplittableRange,
    //    doNormalize, uniScale,
    //    subscribeRatio, minGrainSize);

    static void
        normalizeAttribElement(
            GA_Attribute* const attribPtr,
            const GA_SplittableRange& geoSplittableRange,
            const bool doNormalize = true,
            const fpreal64 uniScale = 1,
            const exint subscribeRatio = 64,
            const exint minGrainSize = 64
        )
    {
        UT_ASSERT_P(attribPtr);
        const GA_Storage storage = attribPtr->getAIFTuple()->getStorage(attribPtr);
        switch (attribPtr->getAIFTuple()->getTupleSize(attribPtr))
        {
        case 2:
            switch (storage)
            {
            case GA_STORE_REAL16:
                normalizeAttribElement2T<fpreal16>(attribPtr, geoSplittableRange, doNormalize, uniScale, subscribeRatio, minGrainSize);
                break;
            case GA_STORE_REAL32:
                normalizeAttribElement2T<fpreal32>(attribPtr, geoSplittableRange, doNormalize, uniScale, subscribeRatio, minGrainSize);
                break;
            case GA_STORE_REAL64:
                normalizeAttribElement2T<fpreal64>(attribPtr, geoSplittableRange, doNormalize, uniScale, subscribeRatio, minGrainSize);
                break;
            default:
                break;
            }
            break;
        case 3:
            switch (storage)
            {
            case GA_STORE_REAL16:
                normalizeAttribElement3T<fpreal16>(attribPtr, geoSplittableRange, doNormalize, uniScale, subscribeRatio, minGrainSize);
                break;
            case GA_STORE_REAL32:
                normalizeAttribElement3T<fpreal32>(attribPtr, geoSplittableRange, doNormalize, uniScale, subscribeRatio, minGrainSize);
                break;
            case GA_STORE_REAL64:
                normalizeAttribElement3T<fpreal64>(attribPtr, geoSplittableRange, doNormalize, uniScale, subscribeRatio, minGrainSize);
                break;
            default:
                break;
            }
            break;
        case 4:
            switch (storage)
            {
            case GA_STORE_REAL16:
                normalizeAttribElement4T<fpreal16>(attribPtr, geoSplittableRange, doNormalize, uniScale, subscribeRatio, minGrainSize);
                break;
            case GA_STORE_REAL32:
                normalizeAttribElement4T<fpreal32>(attribPtr, geoSplittableRange, doNormalize, uniScale, subscribeRatio, minGrainSize);
                break;
            case GA_STORE_REAL64:
                normalizeAttribElement4T<fpreal64>(attribPtr, geoSplittableRange, doNormalize, uniScale, subscribeRatio, minGrainSize);
                break;
            default:
                break;
            }
            break;
        default:
            break;
        }
    }

    //GFE_Attribute::normalizeAttribElement(attribPtr,
    //    doNormalize, uniScale,
    //    subscribeRatio, minGrainSize);

    SYS_FORCE_INLINE
        static void
        normalizeAttribElement(
            GA_Attribute* const attribPtr,
            const bool doNormalize = true,
            const fpreal64 uniScale = 1,
            const exint subscribeRatio = 64,
            const exint minGrainSize = 64
        )
    {
        normalizeAttribElement(attribPtr, GA_SplittableRange(GA_Range(attribPtr->getIndexMap())), doNormalize, uniScale, subscribeRatio, minGrainSize);
    }





    //GFE_Attribute::normalizeAttribElement(geo, attribClass, attribPattern,
    //    doNormalize, uniScale,
    //    subscribeRatio, minGrainSize);

    static void
        normalizeAttribElement(
            const GA_Detail* const geo,
            const GA_ElementGroup* const geoGroup,
            const GA_AttributeOwner attribClass,
            const UT_StringHolder& attribPattern,
            const bool doNormalize = true,
            const fpreal64 uniScale = 1,
            const exint subscribeRatio = 64,
            const exint minGrainSize = 64
        )
    {
        if (!attribPattern.isstring() || attribPattern.length() == 0)
            return;

        const GA_SplittableRange& geoSplittableRange(GA_Range(geo->getIndexMap(attribClass), geoGroup));
        //const GA_SplittableRange geo0SplittableRange = GFE_Range::getSplittableRangeByAnyGroup(geo, geoGroup, attribClass);

        GA_Attribute* attribPtr = nullptr;
        for (GA_AttributeDict::iterator it = geo->getAttributes().begin(attribClass); !it.atEnd(); ++it)
        {
            attribPtr = it.attrib();
            if (attribPtr->isDetached())
                continue;
            if (!attribPtr->getName().multiMatch(attribPattern))
                continue;
            normalizeAttribElement(attribPtr, geoSplittableRange, doNormalize, uniScale, subscribeRatio, minGrainSize);
            attribPtr->bumpDataId();
        }
    }





    //GEO_FeE_Attribute::normalizeAttribElement(outGeo0, geo0Group, geo0AttribClass, attribPtr,
    //    doNormalize, uniScale,
    //    subscribeRatio, minGrainSize);

    //static void
    //    normalizeAttribElement(
    //        const GA_Detail* const geo,
    //        const GA_Group* const geoGroup,
    //        const GA_AttributeOwner attribClass,
    //        GA_Attribute* const attribPtr,
    //        const bool doNormalize = true,
    //        const fpreal64 uniScale = 1,
    //        const exint subscribeRatio = 64,
    //        const exint minGrainSize = 64
    //    )
    //{
    //    UT_ASSERT_P(geo);
    //    UT_ASSERT_P(geoGroup);
    //    const GA_SplittableRange geoSplittableRange = GFE_Range::getSplittableRangeByAnyGroup(geo, geoGroup, attribClass);
    //    normalizeAttribElement(geoSplittableRange, attribPtr,
    //        doNormalize, uniScale,
    //        subscribeRatio, minGrainSize);
    //}








} // End of namespace GFE_NormalizeAttribElement





#endif


#endif





