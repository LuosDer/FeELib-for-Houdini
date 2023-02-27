
#pragma once

#ifndef __GFE_AttributeCopy_h__
#define __GFE_AttributeCopy_h__

//#include "GFE/GFE_AttributeCopy.h"

#include "GA/GA_Detail.h"


#include "GA/GA_AttributeFilter.h"
#include "GA/GA_PageHandle.h"
#include "GA/GA_PageIterator.h"



#include "GFE/GFE_Type.h"


namespace GFE_AttributeCopy {

    


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
    //GA_OffsetList offList = GFE_Detail::getOffsetList(geo, owner, group);


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




} // End of namespace GFE_AttributeCopy

#endif
