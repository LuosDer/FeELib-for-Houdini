
#pragma once

#ifndef __GA_FeE_Attribute_h__
#define __GA_FeE_Attribute_h__

//#include "GA_FeE/GA_FeE_Attribute.h"

#include "GA/GA_Detail.h"


#include "GA/GA_AttributeFilter.h"
#include "GA/GA_PageHandle.h"
#include "GA/GA_PageIterator.h"



#include "GA_FeE/GA_FeE_Type.h"


namespace GA_FeE_Attribute {

    

template<typename T>
class AttribValCount {
public:
    AttribValCount(const GA_Attribute* const attrib, const T& attribVal)
        : myAttrib(attrib)
        , myAttribVal(attribVal)
        , mySum(0)
    {}
    AttribValCount(AttribValCount& src, UT_Split)
        : myAttrib(src.myAttrib)
        , myAttribVal(src.myAttribVal)
        , mySum(0)
    {}
    void operator()(const GA_SplittableRange& r)
    {
        GA_PageHandleT<T, T, true, false, const GA_Attribute, const GA_ATINumeric, const GA_Detail> attrib_ph(myAttrib);
        for (GA_PageIterator pit = r.beginPages(); !pit.atEnd(); ++pit)
        {
            GA_Offset start, end;
            for (GA_Iterator it(pit.begin()); it.blockAdvance(start, end); )
            {
                attrib_ph.setPage(start);
                for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                {
                    mySum += attrib_ph.value(elemoff) == myAttribVal;
                }
            }
        }
    }

    void join(const AttribValCount& other)
    {
        mySum += other.mySum;
    }
    GA_Size getSum()
    {
        return mySum;
    }
private:
    GA_Size mySum;
    const GA_Attribute* const myAttrib;
    const T& myAttribVal;
};

template<typename T>
static GA_Size
getAttribValCount(
    const GA_Attribute* const attribPtr,
    const T& attribVal,
    const exint subscribeRatio = 64,
    const exint minGrainSize = 1024
    )
{
    UT_ASSERT_P(attribPtr);

    const GA_SplittableRange geoSplittableRange(GA_Range(attribPtr->getIndexMap()));


    AttribValCount<T> body(attribPtr, attribVal);
    UTparallelReduce(geoSplittableRange, body, subscribeRatio, minGrainSize);
    return body.getSum();

    //GA_Size count = 0;
    //UTparallelFor(geoSplittableRange, [attribPtr, &attribVal](const GA_SplittableRange& r)
    //{
    //    GA_PageHandleT<T, typename T::value_type, true, true, GA_Attribute, GA_ATINumeric, GA_Detail> attrib_ph(attribPtr);
    //    for (GA_PageIterator pit = r.beginPages(); !pit.atEnd(); ++pit)
    //    {
    //        GA_Offset start, end;
    //        for (GA_Iterator it(pit.begin()); it.blockAdvance(start, end); )
    //        {
    //            attrib_ph.setPage(start);
    //            for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
    //            {
    //                count += attrib_ph.value(elemoff) == attribVal;
    //            }
    //        }
    //    }
    //}, subscribeRatio, minGrainSize);

    //return count;
}





static void
bumpDataId(
    GA_Detail* const geo,
    const GA_AttributeOwner owner,
    const UT_StringHolder& attribPattern
)
{
    GA_AttributeFilter filter = GA_AttributeFilter::selectByPattern(attribPattern);
    UT_Array<GA_Attribute*> attribList;
    geo->getAttributes().matchAttributes(filter, owner, attribList);
    for (GA_Size i = 0; i < attribList.size(); ++i)
    {
        attribList[i]->bumpDataId();
    }
}







/*
template<typename T>
static void
    copyAttributeNumeric(
        GA_Attribute* const attribNew,
        const GA_Attribute* const attribRef,
        const exint subscribeRatio,
        const exint minGrainSize
    )
{
    UT_ASSERT_P(attribNew);
    UT_ASSERT_P(attribRef);
    UT_ASSERT(attribNew->matchesStorage(attribRef));

    GA_ROHandleT<T> attribRef_h(attribRef);
    GA_RWHandleT<T> attribNew_h(attribNew);
    const GA_SplittableRange geoSplittableRange(GA_Range(attribNew->getIndexMap()));
    UTparallelFor(geoSplittableRange, [&attribNew_h, &attribRef_h](const GA_SplittableRange& r)
    {
        GA_Offset start, end;
        for (GA_Iterator it(r); it.blockAdvance(start, end); )
        {
            for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
            {
                T value = attribRef_h.get(elemoff);
                attribNew_h.set(elemoff, value);
            }
        }
    }, subscribeRatio, minGrainSize);
}

static void
copyAttributeString(
    GA_Attribute* const attribNew,
    const GA_Attribute* const attribRef,
    const exint subscribeRatio,
    const exint minGrainSize
)
{
    UT_ASSERT_P(attribNew);
    UT_ASSERT_P(attribRef);
    UT_ASSERT(attribNew->matchesStorage(attribRef));

    GA_ROHandleS attribRef_h(attribRef);
    GA_RWHandleS attribNew_h(attribNew);
    const GA_SplittableRange geoSplittableRange(GA_Range(attribNew->getIndexMap()));
    UTparallelFor(geoSplittableRange, [&attribNew_h, &attribRef_h](const GA_SplittableRange& r)
    {
        GA_Offset start, end;
        for (GA_Iterator it(r); it.blockAdvance(start, end); )
        {
            for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
            {
                UT_StringHolder value = attribRef_h.get(elemoff);
                attribNew_h.set(elemoff, value);
            }
        }
    }, subscribeRatio, minGrainSize);
}

static void
copyAttributeAnyType(
    GA_Attribute* const attribNew,
    const GA_Attribute* const attribRef,
    const exint subscribeRatio,
    const exint minGrainSize
)
{
    UT_ASSERT_P(attribNew);
    UT_ASSERT_P(attribRef);
    UT_ASSERT(attribNew->matchesStorage(attribRef));

    switch (attribNew->getStorageClass())
    {
    case GA_STORECLASS_INT:
        copyAttributeNumeric<int>(attribNew, attribRef, subscribeRatio, minGrainSize);
        break;
    case GA_STORECLASS_FLOAT:
        copyAttributeNumeric<float>(attribNew, attribRef, subscribeRatio, minGrainSize);
        break;
    case GA_STORECLASS_STRING:
        copyAttributeString(attribNew, attribRef, subscribeRatio, minGrainSize);
        break;
    case GA_STORECLASS_DICT:
        break;
    default:
        break;
    }
}

static void
    copyAttribute(
        GA_Detail* const geo,
        const GA_AttributeOwner owner,
        const GA_Detail* const geoRef,
        const GA_AttributeOwner ownerRef,
        const UT_StringHolder& attribPattern,
        const exint subscribeRatio = 8,
        const exint minGrainSize = 1024
    )
{
    if (attribPattern.length()==0)
        return;

    GA_AttributeSet& attribsDst = geo->getAttributes();
    GA_Attribute* attribNew = nullptr;
    GA_Attribute* attribRef = nullptr;

    GA_Range rangeSrc(geo->getIndexMap(owner));
    GA_Range rangeDst(geoRef->getIndexMap(ownerRef));

    for (GA_AttributeDict::iterator it = geoRef->getAttributes().begin(ownerRef); !it.atEnd(); ++it)
    {
        attribRef = it.attrib();
        if (attribRef->isDetached())
            continue;
        if (attribRef->getScope() != GA_SCOPE_PUBLIC)
            continue;
        const UT_StringHolder& attribName = attribRef->getName();
        const UT_StringHolder& attribNameNew = attribName;
        
        if (!attribName.match(attribPattern))
            continue;

        attribNew = attribsDst.findAttribute(owner, attribNameNew);
        //if (attribNew)
        //{
        //    attribsDst.destroyAttribute(attribNew);
        //    attribNew = nullptr;
        //}
        if (attribNew)
        {
            attribNew->bumpDataId();
        }
        else
        {
            //attrib = attribsDst.createAttribute(owner, attribRef, nullptr, nullptr, attribRef->getType(), GA_ReuseStrategy());
            attribNew = attribsDst.cloneAttribute(owner, attribNameNew, *attribRef, true);
        }
        attribNew->copy(rangeDst, *attribRef, rangeSrc);
        //copyAttributeAnyType(attribNew, attribRef, subscribeRatio, minGrainSize);
    }
}
*/


template<typename T>
static void
copyAttributeRef(
    const GA_SplittableRange& geoSplittableRange,
    GA_Attribute* const attribNew,
    const GA_Attribute* const attribRef,
    const GA_Attribute* const attribID = nullptr,
    const exint subscribeRatio = 8,
    const exint minGrainSize = 1024
)
{
    const GA_RWHandleT<T> attribNew_h(attribNew);
    const GA_ROHandleT<T> attribRef_h(attribRef);
    if (attribID)
    {
        const GA_ROHandleT<exint> attribID_h(attribID);
        UTparallelFor(geoSplittableRange, [&attribNew_h, &attribRef_h, &attribID_h](const GA_SplittableRange& r)
        {
            GA_Offset start, end;
            for (GA_Iterator it(r); it.blockAdvance(start, end); )
            {
                for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                {
                    exint id = attribID_h.get(elemoff);
                    T value = attribRef_h.get(elemoff);
                    attribNew_h.set(id, value);
                }
            }
        }, subscribeRatio, minGrainSize);
    }
    else
    {
        UTparallelFor(geoSplittableRange, [&attribNew_h, &attribRef_h](const GA_SplittableRange& r)
        {
            GA_Offset start, end;
            for (GA_Iterator it(r); it.blockAdvance(start, end); )
            {
                for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                {
                    T value = attribRef_h.get(elemoff);
                    attribNew_h.set(elemoff, value);
                }
            }
        }, subscribeRatio, minGrainSize);
    }
}


static void
copyAttribute2(
    GA_Detail* const geo,
    const GA_AttributeOwner owner,
    const GA_Detail* const geoRef,
    const GA_AttributeOwner ownerRef,
    const UT_StringHolder& attribPattern,
    const UT_StringHolder& iDAttribName = "",
    const bool iDAttribInput = false,
    const exint subscribeRatio = 8,
    const exint minGrainSize = 1024
)
{
    if (attribPattern.length() == 0)
        return;

    GA_AttributeSet& attribsDst = geo->getAttributes();
    GA_Attribute* attribNew = nullptr;
    const GA_Attribute* attribRef = nullptr;

    //GA_OffsetList offs;
    //if (!group && indexMap.isTrivialMap())
    //{
    //    if (!reverse)
    //    {
    //        offs.setTrivial(GA_Offset(0), indexMap.indexSize());
    //    }
    //}
    //else
    //{
    //    GA_Offset start, end;
    //    for (GA_Iterator it(GA_Range(indexMap, group, reverse)); it.fullBlockAdvance(start, end); )
    //    {
    //        offs.setTrivialRange(offs.size(), start, end - start);
    //    }
    //}
    //GA_OffsetList offList = GA_FeE_Detail::getOffsetList(geo, owner, group);


    //GA_Range rangeSrc(attribNew->getIndexMap(), offs);
    //GA_Range rangeDst(attribRef->getIndexMap(), offs);
    //if (iDAttribInput)
    //{
    //    rangeSrc = GA_Range(attribNew->getIndexMap(), offs);
    //    rangeDst = GA_Range(attribRef->getIndexMap(), offs);
    //}
    //else
    //{
    //    rangeSrc = GA_Range(attribNew->getIndexMap(), offs);
    //    rangeDst = GA_Range(attribRef->getIndexMap(), offs);
    //}

    const GA_Attribute* iDAttrib = nullptr;
    GA_Range range;
    if (iDAttribInput)//DESTINATION
    {
        if (iDAttribName.length() > 0)
            iDAttrib = attribsDst.findAttribute(owner, iDAttribName);
        range = GA_Range(geo->getIndexMap(owner));
    }
    else//Src
    {
        if (iDAttribName.length() > 0)
            iDAttrib = geoRef->getAttributes().findAttribute(ownerRef, iDAttribName);
        range = GA_Range(geoRef->getIndexMap(ownerRef));
    }
    const GA_SplittableRange geoSplittableRange(range);



    for (GA_AttributeDict::iterator it = geoRef->getAttributes().begin(ownerRef); !it.atEnd(); ++it)
    {
        attribRef = it.attrib();
        if (attribRef->isDetached())
            continue;
        if (attribRef->getScope() != GA_SCOPE_PUBLIC)
            continue;
        const UT_StringHolder& attribName = attribRef->getName();

        if (!attribName.multiMatch(attribPattern, true, " "))
            continue;

        const UT_StringHolder& attribNameNew = attribName;

        attribNew = attribsDst.findAttribute(owner, attribNameNew);
        if (attribNew)
        {
            attribNew->bumpDataId();
        }
        else
        {
            //attrib = attribsDst.createAttribute(owner, attribRef, nullptr, nullptr, attribRef->getType(), GA_ReuseStrategy());
            attribNew = attribsDst.cloneAttribute(owner, attribNameNew, *attribRef, true);
        }

        if (iDAttribInput)//DESTINATION
        {
        }
        else//Src
        {
            switch (attribNew->getType().getTypeId())
            {
            case 0:
                break;
            case 1:
                break;
            case 2:
                break;
            case 3:
                break;
            default:
                break;
            }
            switch (attribNew->getStorageClass())
            {
            case GA_STORECLASS_INT:
                break;
            case GA_STORECLASS_REAL:
                copyAttributeRef<float>(geoSplittableRange, attribNew, attribRef, iDAttrib, subscribeRatio, minGrainSize);
                break;
            case GA_STORECLASS_STRING:
                break;
            case GA_STORECLASS_DICT:
                break;
            default:
                break;
            }
        }
    }
}


template<typename T>
static void
copyAttributeDst(
    const GA_SplittableRange& geoSplittableRange,
    const GA_RWHandleT<T>& attribNew_h,
    const GA_ElementGroup* const attribRef,
    const GA_Attribute* const attribID = nullptr,
    const exint subscribeRatio = 8,
    const exint minGrainSize = 1024
)
{
    if (attribID)
    {
        const GA_ROHandleT<exint> attribID_h(attribID);
        UTparallelFor(geoSplittableRange, [&attribNew_h, attribRef, &attribID_h](const GA_SplittableRange& r)
        {
            GA_Offset start, end;
            for (GA_Iterator it(r); it.blockAdvance(start, end); )
            {
                for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                {
                    exint id = attribID_h.get(elemoff);
                    attribNew_h.set(elemoff, (T)attribRef->contains(id));
                }
            }
        }, subscribeRatio, minGrainSize);
    }
    else
    {
        UTparallelFor(geoSplittableRange, [&attribNew_h, attribRef](const GA_SplittableRange& r)
        {
            GA_Offset start, end;
            for (GA_Iterator it(r); it.blockAdvance(start, end); )
            {
                for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                {
                    attribNew_h.set(elemoff, (T)attribRef->contains(elemoff));
                }
            }
        }, subscribeRatio, minGrainSize);
    }
}

static bool
copyAttributeDst(
    const GA_SplittableRange& geoSplittableRange,
    GA_Attribute* const attribNew,
    const GA_ElementGroup* const attribRef,
    const GA_Attribute* const attribID = nullptr,
    const exint subscribeRatio = 8,
    const exint minGrainSize = 1024
)
{
    switch (attribNew->getAIFTuple()->getStorage(attribNew))
    {
    //case GA_STORE_BOOL:
    //    copyAttributeDst(geoSplittableRange, GA_RWHandleT<bool>(attribNew),
    //        UTverify_cast<const GA_ElementGroup*>(attribRef), attribID, subscribeRatio, minGrainSize);
    //    break;
    case GA_STORE_UINT8:
        copyAttributeDst(geoSplittableRange, GA_RWHandleT<uint8>(attribNew),
            UTverify_cast<const GA_ElementGroup*>(attribRef), attribID, subscribeRatio, minGrainSize);
        break;
    case GA_STORE_INT8:
        copyAttributeDst(geoSplittableRange, GA_RWHandleT<int8>(attribNew),
            UTverify_cast<const GA_ElementGroup*>(attribRef), attribID, subscribeRatio, minGrainSize);
        break;
    case GA_STORE_INT16:
        copyAttributeDst(geoSplittableRange, GA_RWHandleT<int16>(attribNew),
            UTverify_cast<const GA_ElementGroup*>(attribRef), attribID, subscribeRatio, minGrainSize);
        break;
    case GA_STORE_INT32:
        copyAttributeDst(geoSplittableRange, GA_RWHandleT<int32>(attribNew),
            UTverify_cast<const GA_ElementGroup*>(attribRef), attribID, subscribeRatio, minGrainSize);
        break;
    case GA_STORE_INT64:
        copyAttributeDst(geoSplittableRange, GA_RWHandleT<int64>(attribNew),
            UTverify_cast<const GA_ElementGroup*>(attribRef), attribID, subscribeRatio, minGrainSize);
        break;
    case GA_STORE_REAL16:
        copyAttributeDst(geoSplittableRange, GA_RWHandleT<fpreal16>(attribNew),
            UTverify_cast<const GA_ElementGroup*>(attribRef), attribID, subscribeRatio, minGrainSize);
        break;
    case GA_STORE_REAL32:
        copyAttributeDst(geoSplittableRange, GA_RWHandleT<fpreal32>(attribNew),
            UTverify_cast<const GA_ElementGroup*>(attribRef), attribID, subscribeRatio, minGrainSize);
        break;
    case GA_STORE_REAL64:
        copyAttributeDst(geoSplittableRange, GA_RWHandleT<fpreal64>(attribNew),
            UTverify_cast<const GA_ElementGroup*>(attribRef), attribID, subscribeRatio, minGrainSize);
        break;
    default:
        return false;
        break;
    }
    return true;
}

template<typename T>
SYS_FORCE_INLINE
static void
copyAttributeDst(
    const GA_SplittableRange& geoSplittableRange,
    const GA_RWHandleT<T>& attribNew_h,
    const GA_Group* const attribRef,
    const GA_Attribute* const attribID = nullptr,
    const exint subscribeRatio = 8,
    const exint minGrainSize = 1024
)
{
    copyAttributeDst<T>(geoSplittableRange, attribNew_h, UTverify_cast<const GA_ElementGroup*>(attribRef), attribID, subscribeRatio, minGrainSize);
}

SYS_FORCE_INLINE
static void
copyAttributeDst(
    const GA_SplittableRange& geoSplittableRange,
    GA_Attribute* const attribNew,
    const GA_Group* const attribRef,
    const GA_Attribute* const attribID = nullptr,
    const exint subscribeRatio = 8,
    const exint minGrainSize = 1024
)
{
    copyAttributeDst(geoSplittableRange, attribNew, UTverify_cast<const GA_ElementGroup*>(attribRef), attribID, subscribeRatio, minGrainSize);
}


static void
copyAttributeDst(
    const GA_SplittableRange& geoSplittableRange,
    GA_Attribute* const attribNew,
    const GA_Attribute* const attribRef,
    const GA_Attribute* const attribID = nullptr,
    const exint subscribeRatio = 8,
    const exint minGrainSize = 1024
)
{
    if (attribID)
    {
        const GA_ROHandleT<exint> attribID_h(attribID);
        UTparallelFor(geoSplittableRange, [attribNew, attribRef, &attribID_h](const GA_SplittableRange& r)
        {
            GA_Offset start, end;
            for (GA_Iterator it(r); it.blockAdvance(start, end); )
            {
                for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                {
                    exint id = attribID_h.get(elemoff);
                    attribNew->copy(elemoff, *attribRef, id);
                }
            }
        }, subscribeRatio, minGrainSize);
    }
    else
    {
        UTparallelFor(geoSplittableRange, [attribNew, attribRef](const GA_SplittableRange& r)
        {
            GA_Offset start, end;
            for (GA_Iterator it(r); it.blockAdvance(start, end); )
            {
                for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                {
                    attribNew->copy(elemoff, *attribRef, elemoff);
                }
            }
        }, subscribeRatio, minGrainSize);
    }
}



template<typename T>
static void
copyAttributeRef(
    const GA_SplittableRange& geoSplittableRange,
    const GA_RWHandleT<T>& attribNew_h,
    const GA_ElementGroup* const attribRef,
    const GA_Attribute* const attribID = nullptr,
    const exint subscribeRatio = 8,
    const exint minGrainSize = 1024
)
{
    if (attribID)
    {
        const GA_ROHandleT<exint> attribID_h(attribID);
        UTparallelFor(geoSplittableRange, [&attribNew_h, attribRef, &attribID_h](const GA_SplittableRange& r)
        {
            GA_Offset start, end;
            for (GA_Iterator it(r); it.blockAdvance(start, end); )
            {
                for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                {
                    exint id = attribID_h.get(elemoff);
                    attribNew_h.set(id, (T)attribRef->contains(elemoff));
                }
            }
        }, subscribeRatio, minGrainSize);
    }
    else
    {
        UTparallelFor(geoSplittableRange, [&attribNew_h, attribRef](const GA_SplittableRange& r)
        {
            GA_Offset start, end;
            for (GA_Iterator it(r); it.blockAdvance(start, end); )
            {
                for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                {
                    attribNew_h.set(elemoff, (T)attribRef->contains(elemoff));
                }
            }
        }, subscribeRatio, minGrainSize);
    }
}


static bool
copyAttributeRef(
    const GA_SplittableRange& geoSplittableRange,
    GA_Attribute* const attribNew,
    const GA_ElementGroup* const attribRef,
    const GA_Attribute* const attribID = nullptr,
    const exint subscribeRatio = 8,
    const exint minGrainSize = 1024
)
{
    switch (attribNew->getAIFTuple()->getStorage(attribNew))
    {
    //case GA_STORE_BOOL:
    //    copyAttributeRef(geoSplittableRange, GA_RWHandleT<bool>(attribNew),
    //        UTverify_cast<const GA_ElementGroup*>(attribRef), attribID, subscribeRatio, minGrainSize);
    //    break;
    case GA_STORE_UINT8:
        copyAttributeRef(geoSplittableRange, GA_RWHandleT<uint8>(attribNew),
            UTverify_cast<const GA_ElementGroup*>(attribRef), attribID, subscribeRatio, minGrainSize);
        break;
    case GA_STORE_INT8:
        copyAttributeRef(geoSplittableRange, GA_RWHandleT<int8>(attribNew),
            UTverify_cast<const GA_ElementGroup*>(attribRef), attribID, subscribeRatio, minGrainSize);
        break;
    case GA_STORE_INT16:
        copyAttributeRef(geoSplittableRange, GA_RWHandleT<int16>(attribNew),
            UTverify_cast<const GA_ElementGroup*>(attribRef), attribID, subscribeRatio, minGrainSize);
        break;
    case GA_STORE_INT32:
        copyAttributeRef(geoSplittableRange, GA_RWHandleT<int32>(attribNew),
            UTverify_cast<const GA_ElementGroup*>(attribRef), attribID, subscribeRatio, minGrainSize);
        break;
    case GA_STORE_INT64:
        copyAttributeRef(geoSplittableRange, GA_RWHandleT<int64>(attribNew),
            UTverify_cast<const GA_ElementGroup*>(attribRef), attribID, subscribeRatio, minGrainSize);
        break;
    case GA_STORE_REAL16:
        copyAttributeRef(geoSplittableRange, GA_RWHandleT<fpreal16>(attribNew),
            UTverify_cast<const GA_ElementGroup*>(attribRef), attribID, subscribeRatio, minGrainSize);
        break;
    case GA_STORE_REAL32:
        copyAttributeRef(geoSplittableRange, GA_RWHandleT<fpreal32>(attribNew),
            UTverify_cast<const GA_ElementGroup*>(attribRef), attribID, subscribeRatio, minGrainSize);
        break;
    case GA_STORE_REAL64:
        copyAttributeRef(geoSplittableRange, GA_RWHandleT<fpreal64>(attribNew),
            UTverify_cast<const GA_ElementGroup*>(attribRef), attribID, subscribeRatio, minGrainSize);
        break;
    default:
        return false;
        break;
    }
    return true;
}

template<typename T>
SYS_FORCE_INLINE
static void
copyAttributeRef(
    const GA_SplittableRange& geoSplittableRange,
    const GA_RWHandleT<T>& attribNew_h,
    const GA_Group* const attribRef,
    const GA_Attribute* const attribID = nullptr,
    const exint subscribeRatio = 8,
    const exint minGrainSize = 1024
)
{
    copyAttributeRef<T>(geoSplittableRange, attribNew_h, UTverify_cast<const GA_ElementGroup*>(attribRef), attribID, subscribeRatio, minGrainSize);
}

template<typename T>
SYS_FORCE_INLINE
static void
copyAttributeRef(
    const GA_SplittableRange& geoSplittableRange,
    GA_Attribute* const attribNew,
    const GA_Group* const attribRef,
    const GA_Attribute* const attribID = nullptr,
    const exint subscribeRatio = 8,
    const exint minGrainSize = 1024
)
{
    copyAttributeRef<T>(geoSplittableRange, attribNew, UTverify_cast<const GA_ElementGroup*>(attribRef), attribID, subscribeRatio, minGrainSize);
}




static void
copyAttributeRef(
    const GA_SplittableRange& geoSplittableRange,
    GA_Attribute* const attribNew,
    const GA_Attribute* const attribRef,
    const GA_Attribute* const attribID = nullptr,
    const exint subscribeRatio = 8,
    const exint minGrainSize = 1024
)
{
    if (attribID)
    {
        const GA_ROHandleT<exint> attribID_h(attribID);
        UTparallelFor(geoSplittableRange, [attribNew, attribRef, &attribID_h](const GA_SplittableRange& r)
        {
            GA_Offset start, end;
            for (GA_Iterator it(r); it.blockAdvance(start, end); )
            {
                for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                {
                    exint id = attribID_h.get(elemoff);
                    attribNew->copy(id, *attribRef, elemoff);
                }
            }
        }, subscribeRatio, minGrainSize);
    }
    else
    {
        UTparallelFor(geoSplittableRange, [attribNew, attribRef](const GA_SplittableRange& r)
        {
            GA_Offset start, end;
            for (GA_Iterator it(r); it.blockAdvance(start, end); )
            {
                for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                {
                    attribNew->copy(elemoff, *attribRef, elemoff);
                }
            }
        }, subscribeRatio, minGrainSize);
    }
}


static void
copyAttribute(
    GA_Detail* const geo,
    const GA_AttributeOwner owner,
    const GA_Detail* const geoRef,
    const GA_AttributeOwner ownerRef,
    const UT_StringHolder& attribPattern,
    const UT_StringHolder& iDAttribName = "",
    const bool iDAttribInput = false,
    const exint subscribeRatio = 8,
    const exint minGrainSize = 1024
)
{
    if (attribPattern.length() == 0)
        return;

    GA_AttributeSet& attribsDst = geo->getAttributes();
    GA_Attribute* attribNew = nullptr;
    const GA_Attribute* attribRef = nullptr;


    const GA_Attribute* iDAttrib = nullptr;
    
    GA_Range range;
    if (iDAttribInput)//DESTINATION
    {
        if(iDAttribName.length() > 0)
            iDAttrib = attribsDst.findAttribute(owner, iDAttribName);
        range = GA_Range(geo->getIndexMap(owner));
    }
    else//Src
    {
        if (iDAttribName.length() > 0)
            iDAttrib = geoRef->getAttributes().findAttribute(ownerRef, iDAttribName);
        range = GA_Range(geoRef->getIndexMap(ownerRef));
    }
    const GA_SplittableRange geoSplittableRange(range);



    for (GA_AttributeDict::iterator it = geoRef->getAttributes().begin(ownerRef); !it.atEnd(); ++it)
    {
        attribRef = it.attrib();
        if (attribRef->isDetached())
            continue;
        if (attribRef->getScope() != GA_SCOPE_PUBLIC)
            continue;
        const UT_StringHolder& attribName = attribRef->getName();

        if (!attribName.multiMatch(attribPattern, true, " "))
            continue;

        const UT_StringHolder& attribNameNew = attribName;

        attribNew = attribsDst.findAttribute(owner, attribNameNew);
        if (attribNew)
        {
            attribNew->bumpDataId();
        }
        else
        {
            //attrib = attribsDst.createAttribute(owner, attribRef, nullptr, nullptr, attribRef->getType(), GA_ReuseStrategy());
            attribNew = attribsDst.cloneAttribute(owner, attribNameNew, *attribRef, true);
        }

        if (iDAttribInput)//DESTINATION
        {
            copyAttributeDst(geoSplittableRange, attribNew, attribRef, iDAttrib, subscribeRatio, minGrainSize);
        }
        else//Src
        {
            copyAttributeRef(geoSplittableRange, attribNew, attribRef, iDAttrib, subscribeRatio, minGrainSize);
        }
    }
}




SYS_FORCE_INLINE
static bool
copyAttribute(
    GA_Attribute* const attribNew,
    const GA_ElementGroup* const attribRef,
    const GA_Attribute* const attribID = nullptr,
    const exint subscribeRatio = 8,
    const exint minGrainSize = 1024
)
{
    const GA_SplittableRange geoSplittableRange(GA_Range(attribNew->getIndexMap()));
    return copyAttributeDst(geoSplittableRange, attribNew, UTverify_cast<const GA_ElementGroup*>(attribRef), attribID, subscribeRatio, minGrainSize);
}




















































static void
    copyAttribute(
        GA_Attribute* const attrib,
        const GA_Attribute* const attribRef
    )
{
    UT_ASSERT(attrib);
    UT_ASSERT(attribRef);
    return;
}


static void
copyAttribute(
    GA_AttributeSet& attribSet,
    const GA_AttributeSet& attribSetRef,
    const GA_AttributeOwner owner,
    const UT_StringHolder& attribPattern
)
{
    if (attribPattern == "")
        return;
    for (GA_AttributeDict::iterator it = attribSet.begin(owner); !it.atEnd(); ++it)
    {
        GA_Attribute* attrib = it.attrib();
        if (attrib->isDetached())
            continue;
        if (!attrib->getName().match(attribPattern))
            continue;
        
        copyAttribute(attrib, attribSetRef.findAttribute(owner, attrib->getName()));
    }
}




SYS_FORCE_INLINE
static void
copyAttribute(
    GA_AttributeSet& attribSet,
    const GA_AttributeSet& attribSetRef,
    const GA_AttributeOwner owner,
    const GA_AttributeScope scope,
    const UT_StringHolder& attribName
)
{
    return copyAttribute(attribSet.findAttribute(owner, scope, attribName), attribSetRef.findAttribute(owner, scope, attribName));
}

SYS_FORCE_INLINE
static void
copyAttribute(
    GA_Detail* const geo,
    const GA_Detail* const geoRef,
    const GA_AttributeOwner owner,
    const GA_AttributeScope scope,
    const UT_StringHolder& attribName
)
{
    return copyAttribute(geo->getAttributes(), geoRef->getAttributes(), owner, scope, attribName);
}




SYS_FORCE_INLINE
static void
copyAttribute(
    GA_AttributeSet& attribSet,
    const GA_AttributeSet& attribSetRef,
    const UT_StringHolder& primAttribPattern,
    const UT_StringHolder& pointAttribPattern,
    const UT_StringHolder& vertexAttribPattern,
    const UT_StringHolder& detailAttribPattern
)
{
    copyAttribute(attribSet, attribSetRef, GA_ATTRIB_PRIMITIVE, GA_SCOPE_PUBLIC, primAttribPattern);
    copyAttribute(attribSet, attribSetRef, GA_ATTRIB_POINT,     GA_SCOPE_PUBLIC, pointAttribPattern);
    copyAttribute(attribSet, attribSetRef, GA_ATTRIB_VERTEX,    GA_SCOPE_PUBLIC, vertexAttribPattern);
    copyAttribute(attribSet, attribSetRef, GA_ATTRIB_GLOBAL,    GA_SCOPE_PUBLIC, detailAttribPattern);
}

SYS_FORCE_INLINE
static void
copyAttribute(
    GA_Detail* const geo,
    const GA_Detail* const geoRef,
    const UT_StringHolder& primAttribPattern,
    const UT_StringHolder& pointAttribPattern,
    const UT_StringHolder& vertexAttribPattern,
    const UT_StringHolder& detailAttribPattern
)
{
    UT_ASSERT(geo);
    UT_ASSERT(geoRef);
    copyAttribute(geo->getAttributes(), geoRef->getAttributes(), primAttribPattern, pointAttribPattern, vertexAttribPattern, detailAttribPattern);
}






static void
delStdAttribute(
    GA_AttributeSet& attribSet,
    const UT_StringHolder& primAttribPattern,
    const UT_StringHolder& pointAttribPattern,
    const UT_StringHolder& vertexAttribPattern,
    const UT_StringHolder& detailAttribPattern
)
{
    GA_AttributeFilter attribFilter;
    const GA_AttributeFilter attribFilterPublic = GA_AttributeFilter::selectPublic();
    const GA_AttributeFilter attribFilterStd = GA_AttributeFilter::selectAnd(attribFilterPublic, GA_AttributeFilter::selectStandard());

    attribFilter = GA_AttributeFilter::selectByPattern(primAttribPattern);
    attribFilter = GA_AttributeFilter::selectAnd(attribFilter, attribFilterStd);
    attribSet.destroyAttributes(GA_ATTRIB_PRIMITIVE, attribFilter);

    attribFilter = GA_AttributeFilter::selectByPattern(pointAttribPattern);
    attribFilter = GA_AttributeFilter::selectAnd(attribFilter, attribFilterStd);
    attribSet.destroyAttributes(GA_ATTRIB_POINT, attribFilter);

    attribFilter = GA_AttributeFilter::selectByPattern(vertexAttribPattern);
    attribFilter = GA_AttributeFilter::selectAnd(attribFilter, attribFilterStd);
    attribSet.destroyAttributes(GA_ATTRIB_VERTEX, attribFilter);

    attribFilter = GA_AttributeFilter::selectByPattern(detailAttribPattern);
    attribFilter = GA_AttributeFilter::selectAnd(attribFilter, attribFilterStd);
    attribSet.destroyAttributes(GA_ATTRIB_DETAIL, attribFilter);
}


SYS_FORCE_INLINE
    static void
    delStdAttribute(
        GA_Detail* const geo,
        const UT_StringHolder& primAttribPattern,
        const UT_StringHolder& pointAttribPattern,
        const UT_StringHolder& vertexAttribPattern,
        const UT_StringHolder& detailAttribPattern
    )
{
    return delStdAttribute(geo->getAttributes(), primAttribPattern, pointAttribPattern, vertexAttribPattern, detailAttribPattern);
}




static void
keepStdAttribute(
    GA_AttributeSet& attribSet,
    const UT_StringHolder& primAttribPattern,
    const UT_StringHolder& pointAttribPattern,
    const UT_StringHolder& vertexAttribPattern,
    const UT_StringHolder& detailAttribPattern
)
{
    GA_AttributeFilter attribFilter;
    const GA_AttributeFilter attribFilterPublic = GA_AttributeFilter::selectPublic();
    const GA_AttributeFilter attribFilterStd = GA_AttributeFilter::selectAnd(attribFilterPublic, GA_AttributeFilter::selectStandard());

    attribFilter = GA_AttributeFilter::selectByPattern(primAttribPattern);
    attribFilter = GA_AttributeFilter::selectNot(attribFilter);
    attribFilter = GA_AttributeFilter::selectAnd(attribFilter, attribFilterStd);
    attribSet.destroyAttributes(GA_ATTRIB_PRIMITIVE, attribFilter);

    attribFilter = GA_AttributeFilter::selectByPattern(pointAttribPattern);
    attribFilter = GA_AttributeFilter::selectNot(attribFilter);
    attribFilter = GA_AttributeFilter::selectAnd(attribFilter, attribFilterStd);
    attribSet.destroyAttributes(GA_ATTRIB_POINT, attribFilter);

    attribFilter = GA_AttributeFilter::selectByPattern(vertexAttribPattern);
    attribFilter = GA_AttributeFilter::selectNot(attribFilter);
    attribFilter = GA_AttributeFilter::selectAnd(attribFilter, attribFilterStd);
    attribSet.destroyAttributes(GA_ATTRIB_VERTEX, attribFilter);

    attribFilter = GA_AttributeFilter::selectByPattern(detailAttribPattern);
    attribFilter = GA_AttributeFilter::selectNot(attribFilter);
    attribFilter = GA_AttributeFilter::selectAnd(attribFilter, attribFilterStd);
    attribSet.destroyAttributes(GA_ATTRIB_DETAIL, attribFilter);
}


SYS_FORCE_INLINE
static void
keepStdAttribute(
    GA_Detail* const geo,
    const UT_StringHolder& primAttribPattern,
    const UT_StringHolder& pointAttribPattern,
    const UT_StringHolder& vertexAttribPattern,
    const UT_StringHolder& detailAttribPattern
)
{
    return keepStdAttribute(geo->getAttributes(), primAttribPattern, pointAttribPattern, vertexAttribPattern, detailAttribPattern);
}



static void
keepAttribute(
    GA_AttributeSet& attribSet,
    const GA_AttributeOwner attribClass,
    const GA_AttributeFilter& attribFilter
)
{
    for (GA_AttributeDict::iterator it = attribSet.begin(attribClass); !it.atEnd(); ++it)
    {
        GA_Attribute* attribPtr = it.attrib();
        UT_StringHolder name = attribPtr->getName();
        //if (attribPtr->isDetached())
        //    continue;
        if (attribFilter.match(attribPtr))
            continue;
        //attribPtr->bumpDataId();
        attribSet.destroyAttribute(attribPtr);
    }
}


SYS_FORCE_INLINE
static void
keepStdAttribute(
    GA_AttributeSet& attribSet,
    const GA_AttributeOwner attribClass,
    const GA_AttributeFilter& attribFilter
)
{
    GA_AttributeFilter attribFilterPublic = GA_AttributeFilter::selectAnd(attribFilter, GA_AttributeFilter::selectPublic());
    attribFilterPublic = GA_AttributeFilter::selectOr(attribFilterPublic, GA_AttributeFilter::selectNot(GA_AttributeFilter::selectStandard()));
    //attribFilterPublic = GA_AttributeFilter::selectOr(attribFilterPublic, GA_AttributeFilter::selectNot(GA_AttributeFilter::selectFactory()));
    GA_FeE_Attribute::keepAttribute(attribSet, attribClass, attribFilterPublic);
}



SYS_FORCE_INLINE
    static bool
    deleteAttribute(
        GA_Attribute* const attrib
    )
{
    UT_ASSERT_P(attrib);
    return attrib->getDetail().getAttributes().destroyAttribute(attrib);
}



SYS_FORCE_INLINE
static bool
attributeDelete(
    GA_Attribute* const attrib
)
{
    UT_ASSERT_P(attrib);
    return attrib->getDetail().getAttributes().destroyAttribute(attrib);
}

SYS_FORCE_INLINE
static bool
attribDelete(
    GA_Attribute* const attrib
)
{
    UT_ASSERT_P(attrib);
    return attrib->getDetail().getAttributes().destroyAttribute(attrib);
}


SYS_FORCE_INLINE
static bool
    destroyAttribute(
        GA_Attribute* const attrib
    )
{
    UT_ASSERT_P(attrib);
    return attrib->getDetail().getAttributes().destroyAttribute(attrib);
}




SYS_FORCE_INLINE
static bool
renameAttribute(
    GA_Detail* const geo,
    const GA_Attribute* const attrib,
    const UT_StringHolder& newName
)
{
    UT_ASSERT_P(geo);
    UT_ASSERT_P(attrib);
    return geo->renameAttribute(attrib->getOwner(), attrib->getScope(), attrib->getName(), newName);
}



SYS_FORCE_INLINE
static bool
renameAttribute(
    GA_Attribute* const attrib,
    const UT_StringHolder& newName
)
{
    UT_ASSERT_P(attrib);
    return attrib->getDetail().renameAttribute(attrib->getOwner(), attrib->getScope(), attrib->getName(), newName);
}





static bool
forceRenameAttribute(
    GA_Detail* geo,
    GA_Attribute* const attrib,
    const UT_StringHolder& newName
)
{
    GA_Attribute* const existAttribute = geo->findAttribute(attrib->getOwner(), newName);
    if (existAttribute)
        geo->getAttributes().destroyAttribute(existAttribute);
    return geo->renameAttribute(attrib->getOwner(), GA_SCOPE_PUBLIC, attrib->getName(), newName);
}


SYS_FORCE_INLINE
static bool
forceRenameAttribute(
    GA_Attribute* const attrib,
    const UT_StringHolder& newName
)
{
    return forceRenameAttribute(&(attrib->getDetail()), attrib, newName);
    //GA_Detail& geo = attrib->getDetail();
    //return forceRenameAttribute(&geo, attrib, newName);
}






//GA_FeE_Attribute::findAttributePointVertex(outGeo0, geo0AttribClass, geo0AttribNameSub, geo0AttribClassFinal)
//GA_FeE_Attribute::findAttributePointVertex(geo, attribOwner, GA_SCOPE_PUBLIC, attribName, attribOwnerFianl);

static const GA_Attribute*
findAttributePointVertex(
    const GA_Detail* const geo,
    const GA_AttributeOwner attribOwner,
    const GA_AttributeScope scope,
    const UT_StringRef& attribName,
    GA_AttributeOwner& attribOwnerFinal
)
{
    const GA_Attribute* attribPtr = nullptr;
    const GA_AttributeSet& geoAttribs = geo->getAttributes();
    if (attribOwner < 0 || attribOwner >= GA_ATTRIB_PRIMITIVE)//not point or vertex means Auto
    {
        attribPtr = geoAttribs.findAttribute(GA_ATTRIB_VERTEX, scope, attribName);
        if (attribPtr)
            attribOwnerFinal = GA_ATTRIB_VERTEX;
        else
        {
            attribPtr = geoAttribs.findAttribute(GA_ATTRIB_POINT, scope, attribName);
            if (!attribPtr)
            {
                attribOwnerFinal = GA_ATTRIB_INVALID;
                return nullptr;
            }
            attribOwnerFinal = GA_ATTRIB_POINT;
        }
    }
    else
    {
        attribPtr = geoAttribs.findAttribute(attribOwner, scope, attribName);
        if (!attribPtr)
        {
            attribOwnerFinal = GA_ATTRIB_INVALID;
            return nullptr;
        }
        attribOwnerFinal = attribOwner;
    }
    return attribPtr;
}

static const GA_Attribute*
findAttributePointVertex(
    const GA_Detail* const geo,
    const GA_AttributeOwner attribOwner,
    const GA_AttributeScope scope,
    const UT_StringRef& attribName
)
{
    const GA_Attribute* attribPtr = nullptr;
    const GA_AttributeSet& geoAttribs = geo->getAttributes();
    if (attribOwner < 0 || attribOwner >= GA_ATTRIB_PRIMITIVE)//not point or vertex means Auto
    {
        attribPtr = geoAttribs.findAttribute(GA_ATTRIB_VERTEX, scope, attribName);
        if (!attribPtr)
        {
            attribPtr = geoAttribs.findAttribute(GA_ATTRIB_POINT, scope, attribName);
            if (!attribPtr)
            {
                return nullptr;
            }
        }
    }
    else
    {
        attribPtr = geoAttribs.findAttribute(attribOwner, scope, attribName);
        if (!attribPtr)
        {
            return nullptr;
        }
    }
    return attribPtr;
}





static GA_Attribute*
findAttributePointVertex(
    GA_Detail* const geo,
    const GA_AttributeOwner attribOwner,
    const GA_AttributeScope scope,
    const UT_StringRef& attribName,
    GA_AttributeOwner& attribOwnerFinal
)
{
    GA_Attribute* attribPtr = nullptr;
    GA_AttributeSet& geoAttribs = geo->getAttributes();
    if (attribOwner < 0 || attribOwner >= GA_ATTRIB_PRIMITIVE)//not point or vertex means Auto
    {
        attribPtr = geoAttribs.findAttribute(GA_ATTRIB_VERTEX, scope, attribName);
        if (attribPtr)
            attribOwnerFinal = GA_ATTRIB_VERTEX;
        else
        {
            attribPtr = geoAttribs.findAttribute(GA_ATTRIB_POINT, scope, attribName);
            if (!attribPtr)
            {
                attribOwnerFinal = GA_ATTRIB_INVALID;
                return nullptr;
            }
            attribOwnerFinal = GA_ATTRIB_POINT;
        }
    }
    else
    {
        attribPtr = geoAttribs.findAttribute(attribOwner, scope, attribName);
        if (!attribPtr)
        {
            attribOwnerFinal = GA_ATTRIB_INVALID;
            return nullptr;
        }
        attribOwnerFinal = attribOwner;
    }
    return attribPtr;
}



static GA_Attribute*
findAttributePointVertex(
    GA_Detail* const geo,
    const GA_AttributeOwner attribOwner,
    const GA_AttributeScope scope,
    const UT_StringRef& attribName
)
{
    GA_Attribute* attribPtr = nullptr;
    GA_AttributeSet& geoAttribs = geo->getAttributes();
    if (attribOwner < 0 || attribOwner >= GA_ATTRIB_PRIMITIVE)//not point or vertex means Auto
    {
        attribPtr = geoAttribs.findAttribute(GA_ATTRIB_VERTEX, scope, attribName);
        if (!attribPtr)
        {
            attribPtr = geoAttribs.findAttribute(GA_ATTRIB_POINT, scope, attribName);
            if (!attribPtr)
            {
                return nullptr;
            }
        }
    }
    else
    {
        attribPtr = geoAttribs.findAttribute(attribOwner, scope, attribName);
        if (!attribPtr)
        {
            return nullptr;
        }
    }
    return attribPtr;
}




//GA_FeE_Attribute::findAttributePointVertex(outGeo0, geo0AttribClass, geo0AttribNameSub, geo0AttribClassFinal)
//GA_FeE_Attribute::findAttributePointVertex(geo, attribOwner, attribName, attribOwnerFianl);



SYS_FORCE_INLINE
static const GA_Attribute*
findAttributePointVertex(
    const GA_Detail* const geo,
    const GA_AttributeOwner attribOwner,
    const UT_StringRef& attribName,
    GA_AttributeOwner& attribOwnerFianl
)
{
    return findAttributePointVertex(geo, attribOwner, GA_SCOPE_PUBLIC, attribName, attribOwnerFianl);
}

SYS_FORCE_INLINE
static const GA_Attribute*
findAttributePointVertex(
    const GA_Detail* const geo,
    const GA_AttributeOwner attribOwner,
    const UT_StringRef& attribName
)
{
    return findAttributePointVertex(geo, attribOwner, GA_SCOPE_PUBLIC, attribName);
}



SYS_FORCE_INLINE
static GA_Attribute*
findAttributePointVertex(
    GA_Detail* const geo,
    const GA_AttributeOwner attribOwner,
    const UT_StringRef& attribName,
    GA_AttributeOwner& attribOwnerFianl
)
{
    return findAttributePointVertex(geo, attribOwner, GA_SCOPE_PUBLIC, attribName, attribOwnerFianl);
}

SYS_FORCE_INLINE
static GA_Attribute*
findAttributePointVertex(
    GA_Detail* const geo,
    const GA_AttributeOwner attribOwner,
    const UT_StringRef& attribName
)
{
    return findAttributePointVertex(geo, attribOwner, GA_SCOPE_PUBLIC, attribName);
}




static GA_Attribute*
findUVAttributePointVertex(
    GA_Detail* const geo,
    const GA_AttributeOwner uvAttribClass = GA_ATTRIB_INVALID,
    const UT_StringRef& uvAttribName = "uv",
    const GA_Storage storage = GA_STORE_INVALID
)
{
    GA_Attribute* uvAttribPtr = findAttributePointVertex(geo, uvAttribClass, uvAttribName);
    if (uvAttribPtr)
    {
        int tupleSize = uvAttribPtr->getTupleSize();
        if (tupleSize < 2 || tupleSize > 4)
        {
            //geo->getAttributes().destroyAttribute(uvAttribPtr);
            uvAttribPtr = nullptr;
        }
    }
    return uvAttribPtr;
}



static GA_Attribute*
findOrCreateUVAttributePointVertex(
    GA_Detail* const geo,
    const GA_AttributeOwner uvAttribClass = GA_ATTRIB_INVALID,
    const UT_StringRef& uvAttribName = "uv",
    const GA_Storage storage = GA_STORE_INVALID
)
{
    GA_Attribute* uvAttribPtr = findAttributePointVertex(geo, uvAttribClass, uvAttribName);
    if (uvAttribPtr)
    {
        int tupleSize = uvAttribPtr->getTupleSize();
        if (tupleSize < 2 || tupleSize > 4)
        {
            geo->getAttributes().destroyAttribute(uvAttribPtr);
            uvAttribPtr = nullptr;
        }
    }
    if (!uvAttribPtr)
    {
        const GA_Storage finalStorageF = storage == GA_STORE_INVALID ? GA_FeE_Type::getPreferredStorageF(geo) : storage;
#if 1
        uvAttribPtr = static_cast<GEO_Detail*>(geo)->addTextureAttribute(uvAttribClass == GA_ATTRIB_POINT ? GA_ATTRIB_POINT : GA_ATTRIB_VERTEX, finalStorageF);
#else
        uvAttribPtr = geo->getAttributes().createTupleAttribute(
            uvAttribClass == GA_ATTRIB_POINT ? GA_ATTRIB_POINT : GA_ATTRIB_VERTEX, uvAttribName, finalStorageF, 3, GA_Defaults(0));
#endif
    }
    return uvAttribPtr;
}


//
//template<typename T>
//static void
//normalizeAttribElement(
//    const GA_RWHandleT<UT_Vector3T<T>>& attrib_h,
//    const GA_SplittableRange& geoSplittableRange,
//    const bool doNormalize = 1,
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
    const bool doNormalize = 1,
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
    const bool doNormalize = 1,
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
    const bool doNormalize = 1,
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
    const bool doNormalize = 1,
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


//GA_FeE_Attribute::normalizeAttribElement(attribPtr, geo0SplittableRange,
//    doNormalize, uniScale,
//    subscribeRatio, minGrainSize);

static void
normalizeAttribElement(
    GA_Attribute* const attribPtr,
    const GA_SplittableRange& geoSplittableRange,
    const bool doNormalize = 1,
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

//GA_FeE_Attribute::normalizeAttribElement(attribPtr,
//    doNormalize, uniScale,
//    subscribeRatio, minGrainSize);

SYS_FORCE_INLINE
static void
normalizeAttribElement(
    GA_Attribute* const attribPtr,
    const bool doNormalize = 1,
    const fpreal64 uniScale = 1,
    const exint subscribeRatio = 64,
    const exint minGrainSize = 64
)
{
    normalizeAttribElement(attribPtr, GA_SplittableRange(GA_Range(attribPtr->getIndexMap())), doNormalize, uniScale, subscribeRatio, minGrainSize);
}





//GA_FeE_Attribute::normalizeAttribElement(geo, attribClass, attribPattern,
//    doNormalize, uniScale,
//    subscribeRatio, minGrainSize);

static void
normalizeAttribElement(
    const GA_Detail* const geo,
    const GA_ElementGroup* const geoGroup,
    const GA_AttributeOwner attribClass,
    const UT_StringHolder& attribPattern,
    const bool doNormalize = 1,
    const fpreal64 uniScale = 1,
    const exint subscribeRatio = 64,
    const exint minGrainSize = 64
)
{
    if (!attribPattern.isstring() || attribPattern.length() == 0)
        return;

    const GA_SplittableRange& geoSplittableRange(GA_Range(geo->getIndexMap(attribClass), geoGroup));
    //const GA_SplittableRange geo0SplittableRange = GA_FeE_Range::getSplittableRangeByAnyGroup(geo, geoGroup, attribClass);

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

static void
copyAttribFromSingleElement(
    const GA_Detail* const geo,
    const GA_ElementGroup* const geoGroup,
    const GA_AttributeOwner attribClass,
    const UT_StringHolder& attribPattern,
    const bool doNormalize = 1,
    const fpreal64 uniScale = 1,
    const exint subscribeRatio = 64,
    const exint minGrainSize = 64
)
{
    if (!attribPattern.isstring() || attribPattern.length() == 0)
        return;

    const GA_SplittableRange& geoSplittableRange(GA_Range(geo->getIndexMap(attribClass), geoGroup));
    //const GA_SplittableRange geo0SplittableRange = GA_FeE_Range::getSplittableRangeByAnyGroup(geo, geoGroup, attribClass);

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



} // End of namespace GA_FeE_Attribute

#endif
