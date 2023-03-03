
#pragma once

#ifndef __GFE_SetVectorComponent_h__
#define __GFE_SetVectorComponent_h__

//#include "GFE/GFE_SetVectorComponent.h"

#include "GA/GA_Detail.h"

#include "GA/GA_PageHandle.h"



#include "GFE/GFE_GeoFilter.h"
//#include "GFE/GFE_GroupParse.h"
#include "GFE/GFE_Range.h"

#if 1

class GFE_SetVectorComponent : public GFE_AttribFilter {

public:

    using GFE_AttribFilter::GFE_AttribFilter;

    ~GFE_SetVectorComponent()
    {
    }

    void
        setComputeParm(
            const int comp = 0,
            const fpreal attribValF = 0,
            const exint subscribeRatio = 64,
            const exint minGrainSize = 1024
        )
    {
        hasParm_computeParm = true;
        this->comp = comp;
        this->attribValF = attribValF;
        this->subscribeRatio = subscribeRatio;
        this->minGrainSize = minGrainSize;
    }


    void
        computeCore()
    {
        for (int i = 0; i < attribArray.size(); i++)
        {
            GA_Attribute* const attribPtr = attribArray[i];
            switch (attribPtr->getTupleSize())
            {
            case 2:
                switch (attribPtr->getAIFTuple()->getStorage(attribPtr))
                {
                case GA_STORE_REAL16:
                    setVectorComponent<UT_Vector2T<fpreal16>>(attribPtr, comp, attribValF,
                        subscribeRatio, minGrainSize);
                    break;
                case GA_STORE_REAL32:
                    setVectorComponent<UT_Vector2T<fpreal32>>(attribPtr, comp, attribValF,
                        subscribeRatio, minGrainSize);
                    break;
                case GA_STORE_REAL64:
                    setVectorComponent<UT_Vector2T<fpreal64>>(attribPtr, comp, attribValF,
                        subscribeRatio, minGrainSize);
                    break;
                default:
                    break;
                }
                break;
            case 3:
                switch (attribPtr->getAIFTuple()->getStorage(attribPtr))
                {
                case GA_STORE_REAL16:
                    setVectorComponent<UT_Vector3T<fpreal16>>(attribPtr, comp, attribValF,
                        subscribeRatio, minGrainSize);
                    break;
                case GA_STORE_REAL32:
                    setVectorComponent<UT_Vector3T<fpreal32>>(attribPtr, comp, attribValF,
                        subscribeRatio, minGrainSize);
                    break;
                case GA_STORE_REAL64:
                    setVectorComponent<UT_Vector3T<fpreal64>>(attribPtr, comp, attribValF,
                        subscribeRatio, minGrainSize);
                    break;
                default:
                    break;
                }
                break;
            case 4:
                //switch (attribPtr->getAIFTuple()->getStorage(attribPtr))
                //{
                //case GA_STORE_REAL16:
                //    uvGridify(geo, GA_RWHandleT<UT_Vector4T<fpreal16>>(attribPtr), geoPrimGroup,
                //        rowsOrColsNumMethod, rowsOrColsNum,
                //        reverseUVu, reverseUVv, uniScale,
                //        subscribeRatio, minGrainSize);
                //    break;
                //case GA_STORE_REAL32:
                //    uvGridify(geo, GA_RWHandleT<UT_Vector4T<fpreal32>>(attribPtr), geoPrimGroup,
                //        rowsOrColsNumMethod, rowsOrColsNum,
                //        reverseUVu, reverseUVv, uniScale,
                //        subscribeRatio, minGrainSize);
                //    break;
                //case GA_STORE_REAL64:
                //    uvGridify(geo, GA_RWHandleT<UT_Vector4T<fpreal64>>(attribPtr), geoPrimGroup,
                //        rowsOrColsNumMethod, rowsOrColsNum,
                //        reverseUVu, reverseUVv, uniScale,
                //        subscribeRatio, minGrainSize);
                //    break;
                //default:
                //    return false;
                //    break;
                //}
                //break;
            default:
                break;
            }
        }
    }

protected:

    template<typename T>
    void
        setVectorComponent(
            GA_Attribute* const attribPtr,
            const int comp,
            const typename T::value_type attribVal,
            const exint subscribeRatio = 64,
            const exint minGrainSize = 1024
        )
    {
        UT_ASSERT_P(attribPtr);
        const GA_SplittableRange geoSplittableRange = GFE_Range::getSplittableRangeByAnyGroup(&attribPtr->getDetail(), geoGroup, attribPtr->getOwner());
        //const GA_SplittableRange geoSplittableRange(GA_Range(attribPtr->getIndexMap(), geoGroup));
        UTparallelFor(geoSplittableRange, [attribPtr, comp, attribVal](const GA_SplittableRange& r)
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

    template<typename T>
    void
        setVectorComponent(
            GA_Attribute* const attribPtr,
            const GA_Attribute* const attribRefPtr,
            const int comp,
            const typename T::value_type attribVal,
            const exint subscribeRatio = 64,
            const exint minGrainSize = 1024
        )
    {
        UT_ASSERT_P(attribPtr);
        if (!attribRefPtr)
            return setVectorComponent(attribPtr, attribVal, subscribeRatio, minGrainSize);

        const GA_SplittableRange geoSplittableRange = GFE_Range::getSplittableRangeByAnyGroup(&attribPtr->getDetail(), geoGroup, attribPtr->getOwner());
        //const GA_SplittableRange geoSplittableRange(GA_Range(attribPtr->getIndexMap(), geoGroup));
        UTparallelFor(geoSplittableRange, [attribPtr, comp, attribVal, attribRefPtr](const GA_SplittableRange& r)
        {
            GA_PageHandleT<T, typename T::value_type, true, true, GA_Attribute, GA_ATINumeric, GA_Detail> attrib_ph(attribPtr);
            GA_PageHandleT<T, typename T::value_type, true, true, GA_Attribute, GA_ATINumeric, GA_Detail> attribRef_ph(attribRefPtr);
            for (GA_PageIterator pit = r.beginPages(); !pit.atEnd(); ++pit)
            {
                GA_Offset start, end;
                for (GA_Iterator it(pit.begin()); it.blockAdvance(start, end); )
                {
                    attrib_ph.setPage(start);
                    attribRef_ph.setPage(start);
                    for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                    {
                        attrib_ph.value(elemoff)[comp] = attribVal;
                    }
                }
            }
        }, subscribeRatio, minGrainSize);
    }

protected:
    int comp;
    fpreal attribValF;
    exint subscribeRatio;
    exint minGrainSize;
};



#else

class GFE_SetVectorComponent {



public:

    GFE_SetVectorComponent(
        GA_Detail* const geo,
        const SOP_NodeVerb::CookParms* const cookparms = nullptr
    )
        : geo(geo)
        , cookparms(cookparms)
    {
        UT_ASSERT_MSG(geo, "do not find geo");
    }

    GFE_SetVectorComponent(
        const SOP_NodeVerb::CookParms& cookparms,
        GA_Detail* const geo
    )
        : geo(geo)
        , cookparms(&cookparms)
    {
        UT_ASSERT_MSG(geo, "do not find geo");
    }

    ~GFE_SetVectorComponent()
    {
    }



    void
        setInGroup(
            const GA_Group* const geoGroup
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
        geoGroup = groupParse.parse(groupType);
    }




    void
        setComputeParm(
            const int comp,
            const fpreal attribValF,
            const exint subscribeRatio = 64,
            const exint minGrainSize = 1024
        )
    {
        hasParm_computeParm = true;
        this->comp = comp;
        this->attribValF = attribValF;
        this->subscribeRatio = subscribeRatio;
        this->minGrainSize = minGrainSize;
    }




    void
        setOutAttrib(
            GA_Attribute* attribPtr
        )
    {
        if (!attribPtr)
            return;

        attribArray.clear();
        attribArray.emplace_back(attribPtr);
    }

    void
        addOutAttrib(
            GA_Attribute* attribPtr
        )
    {
        if (!attribPtr)
            return;

        if (attribArray.size() == 0)
            attribArray.emplace_back(attribPtr);
        return;

        attribArray.emplace_back(attribPtr);
    }

    void
        setOutAttrib(
            const GA_AttributeOwner attribClass,
            const UT_StringHolder& attribPattern
        )
    {
        if (!attribPattern.isstring() || attribPattern.length() == 0)
            return;

        attribArray.clear();
        GA_Attribute* attribPtr = geo->findAttribute(attribClass, attribPattern);
        if (attribPtr)
            attribArray.emplace_back(attribPtr);
    }

    void
        addOutAttrib(
            const GA_AttributeOwner attribClass,
            const UT_StringHolder& attribPattern
        )
    {
        if (!attribPattern.isstring() || attribPattern.length() == 0)
            return;

        GA_Attribute* attribPtr = geo->findAttribute(attribClass, attribPattern);
        if (attribPtr)
            attribArray.emplace_back(attribPtr);
    }

    void
        addOutAttribs(
            const GA_AttributeOwner attribClass,
            const UT_StringHolder& attribPattern
        )
    {
        if (!attribPattern.isstring() || attribPattern.length() == 0)
            return;

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
        compute()
    {
        if (attribArray.size() == 0)
            return;

        if (geoGroup && geoGroup->isEmpty())
            return;

        for (int i = 0; i < attribArray.size(); i++)
        {
            GA_Attribute* const attribPtr = attribArray[i];
            switch (attribPtr->getTupleSize())
            {
            case 2:
                switch (attribPtr->getAIFTuple()->getStorage(attribPtr))
                {
                case GA_STORE_REAL16:
                    setVectorComponent<UT_Vector2T<fpreal16>>(attribPtr, comp, attribValF,
                        subscribeRatio, minGrainSize);
                    break;
                case GA_STORE_REAL32:
                    setVectorComponent<UT_Vector2T<fpreal32>>(attribPtr, comp, attribValF,
                        subscribeRatio, minGrainSize);
                    break;
                case GA_STORE_REAL64:
                    setVectorComponent<UT_Vector2T<fpreal64>>(attribPtr, comp, attribValF,
                        subscribeRatio, minGrainSize);
                    break;
                default:
                    break;
                }
                break;
            case 3:
                switch (attribPtr->getAIFTuple()->getStorage(attribPtr))
                {
                case GA_STORE_REAL16:
                    setVectorComponent<UT_Vector3T<fpreal16>>(attribPtr, comp, attribValF,
                        subscribeRatio, minGrainSize);
                    break;
                case GA_STORE_REAL32:
                    setVectorComponent<UT_Vector3T<fpreal32>>(attribPtr, comp, attribValF,
                        subscribeRatio, minGrainSize);
                    break;
                case GA_STORE_REAL64:
                    setVectorComponent<UT_Vector3T<fpreal64>>(attribPtr, comp, attribValF,
                        subscribeRatio, minGrainSize);
                    break;
                default:
                    break;
                }
                break;
            case 4:
                //switch (attribPtr->getAIFTuple()->getStorage(attribPtr))
                //{
                //case GA_STORE_REAL16:
                //    uvGridify(geo, GA_RWHandleT<UT_Vector4T<fpreal16>>(attribPtr), geoPrimGroup,
                //        rowsOrColsNumMethod, rowsOrColsNum,
                //        reverseUVu, reverseUVv, uniScale,
                //        subscribeRatio, minGrainSize);
                //    break;
                //case GA_STORE_REAL32:
                //    uvGridify(geo, GA_RWHandleT<UT_Vector4T<fpreal32>>(attribPtr), geoPrimGroup,
                //        rowsOrColsNumMethod, rowsOrColsNum,
                //        reverseUVu, reverseUVv, uniScale,
                //        subscribeRatio, minGrainSize);
                //    break;
                //case GA_STORE_REAL64:
                //    uvGridify(geo, GA_RWHandleT<UT_Vector4T<fpreal64>>(attribPtr), geoPrimGroup,
                //        rowsOrColsNumMethod, rowsOrColsNum,
                //        reverseUVu, reverseUVv, uniScale,
                //        subscribeRatio, minGrainSize);
                //    break;
                //default:
                //    return false;
                //    break;
                //}
                //break;
            default:
                break;
            }
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

    template<typename T>
    void
        setVectorComponent(
            GA_Attribute* const attribPtr,
            const int comp,
            const typename T::value_type attribVal,
            const exint subscribeRatio = 64,
            const exint minGrainSize = 1024
        )
    {
        UT_ASSERT_P(attribPtr);
        const GA_SplittableRange geoSplittableRange = GFE_Range::getSplittableRangeByAnyGroup(&attribPtr->getDetail(), geoGroup, attribPtr->getOwner());
        //const GA_SplittableRange geoSplittableRange(GA_Range(attribPtr->getIndexMap(), geoGroup));
        UTparallelFor(geoSplittableRange, [attribPtr, comp, attribVal](const GA_SplittableRange& r)
        {
            GA_PageHandleT<T, typename T::value_type , true, true, GA_Attribute, GA_ATINumeric, GA_Detail> attrib_ph(attribPtr);
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

    template<typename T>
    void
        setVectorComponent(
            GA_Attribute* const attribPtr,
            const GA_Attribute* const attribRefPtr,
            const int comp,
            const typename T::value_type attribVal,
            const exint subscribeRatio = 64,
            const exint minGrainSize = 1024
        )
    {
        UT_ASSERT_P(attribPtr);
        if (!attribRefPtr)
            return setVectorComponent(attribPtr, attribVal, subscribeRatio, minGrainSize);

        const GA_SplittableRange geoSplittableRange = GFE_Range::getSplittableRangeByAnyGroup(&attribPtr->getDetail(), geoGroup, attribPtr->getOwner());
        //const GA_SplittableRange geoSplittableRange(GA_Range(attribPtr->getIndexMap(), geoGroup));
        UTparallelFor(geoSplittableRange, [attribPtr, comp, attribVal, attribRefPtr](const GA_SplittableRange& r)
        {
            GA_PageHandleT<T, typename T::value_type , true, true, GA_Attribute, GA_ATINumeric, GA_Detail> attrib_ph(attribPtr);
            GA_PageHandleT<T, typename T::value_type , true, true, GA_Attribute, GA_ATINumeric, GA_Detail> attribRef_ph(attribRefPtr);
            for (GA_PageIterator pit = r.beginPages(); !pit.atEnd(); ++pit)
            {
                GA_Offset start, end;
                for (GA_Iterator it(pit.begin()); it.blockAdvance(start, end); )
                {
                    attrib_ph.setPage(start);
                    attribRef_ph.setPage(start);
                    for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                    {
                        attrib_ph.value(elemoff)[comp] = attribVal;
                    }
                }
            }
        }, subscribeRatio, minGrainSize);
    }




private:
    const SOP_NodeVerb::CookParms* cookparms;
    GA_Detail* geo;


    bool hasParm_inGroup = false;
    GFE_GroupParse groupParse;
    const GA_Group* geoGroup = nullptr;

    //UT_StringHolder attribPattern;
    std::vector<GA_Attribute*> attribArray;

    bool hasParm_computeParm = false;
    int comp;
    fpreal attribValF;
    exint subscribeRatio;
    exint minGrainSize;

};


#endif


















namespace GFE_SetVectorComponent_Namespace {

template<typename T>
static void
setVectorComponent(
    GA_Attribute* const attribPtr,
    const int comp,
    const typename T::value_type attribVal,
    const exint subscribeRatio = 64,
    const exint minGrainSize = 1024
)
{
    UT_ASSERT_P(attribPtr);
    const GA_SplittableRange geoSplittableRange(GA_Range(attribPtr->getIndexMap()));
    UTparallelFor(geoSplittableRange, [attribPtr, &attribVal](const GA_SplittableRange& r)
    {
        GA_PageHandleT<T, typename T::value_type , true, true, GA_Attribute, GA_ATINumeric, GA_Detail> attrib_ph(attribPtr);
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

template<typename T>
static void
setVectorComponent(
    GA_Attribute* const attribPtr,
    const GA_Attribute* const attribRefPtr,
    const int comp,
    const typename T::value_type attribVal,
    const exint subscribeRatio = 64,
    const exint minGrainSize = 1024
    )
{
    UT_ASSERT_P(attribPtr);
    if (!attribRefPtr)
        return setVectorComponent(attribPtr, attribVal, subscribeRatio, minGrainSize);

    const GA_SplittableRange geoSplittableRange(GA_Range(attribPtr->getIndexMap()));
    UTparallelFor(geoSplittableRange, [attribPtr, &attribVal](const GA_SplittableRange& r)
    {
        GA_PageHandleT<T, typename T::value_type, true, true, GA_Attribute, GA_ATINumeric, GA_Detail> attrib_ph(attribPtr);
        GA_PageHandleT<T, typename T::value_type, true, true, GA_Attribute, GA_ATINumeric, GA_Detail> attribRef_ph(attribRefPtr);
        for (GA_PageIterator pit = r.beginPages(); !pit.atEnd(); ++pit)
        {
            GA_Offset start, end;
            for (GA_Iterator it(pit.begin()); it.blockAdvance(start, end); )
            {
                attrib_ph.setPage(start);
                attribRef_ph.setPage(start);
                for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                {
                    attrib_ph.value(elemoff)[comp] = attribVal;
                }
            }
        }
    }, subscribeRatio, minGrainSize);
}













} // End of namespace GFE_SetVectorComponent

#endif
