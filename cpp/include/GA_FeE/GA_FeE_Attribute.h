
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


/*
template<typename T>
static void
    copyAttributeNumeric(
        GA_Attribute* const attribNew,
        const GA_Attribute* const attribRef,
        const int subscribeRatio,
        const int minGrainSize
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
    const int subscribeRatio,
    const int minGrainSize
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
    const int subscribeRatio,
    const int minGrainSize
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
        const int subscribeRatio = 8,
        const int minGrainSize = 1024
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
    const int subscribeRatio = 8,
    const int minGrainSize = 1024
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
    const int subscribeRatio = 8,
    const int minGrainSize = 1024
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






static void
copyAttributeDst(
    const GA_SplittableRange& geoSplittableRange,
    GA_Attribute* const attribNew,
    const GA_Attribute* const attribRef,
    const GA_Attribute* const attribID = nullptr,
    const int subscribeRatio = 8,
    const int minGrainSize = 1024
)
{
    if (attribID)
    {
        const GA_ROHandleT<exint> attribID_h(attribID);
        UTparallelFor(geoSplittableRange, [&attribNew, &attribRef, &attribID_h](const GA_SplittableRange& r)
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
        UTparallelFor(geoSplittableRange, [&attribNew, &attribRef](const GA_SplittableRange& r)
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
copyAttributeRef(
    const GA_SplittableRange& geoSplittableRange,
    GA_Attribute* const attribNew,
    const GA_Attribute* const attribRef,
    const GA_Attribute* const attribID = nullptr,
    const int subscribeRatio = 8,
    const int minGrainSize = 1024
)
{
    if (attribID)
    {
        const GA_ROHandleT<exint> attribID_h(attribID);
        UTparallelFor(geoSplittableRange, [&attribNew, &attribRef, &attribID_h](const GA_SplittableRange& r)
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
        UTparallelFor(geoSplittableRange, [&attribNew, &attribRef](const GA_SplittableRange& r)
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
    const int subscribeRatio = 8,
    const int minGrainSize = 1024
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
        GA_Attribute* attrib
    )
{
    UT_ASSERT_P(attrib);
    return attrib->getDetail().getAttributes().destroyAttribute(attrib);
}



SYS_FORCE_INLINE
static bool
attributeDelete(
    GA_Attribute* attrib
)
{
    UT_ASSERT_P(attrib);
    return attrib->getDetail().getAttributes().destroyAttribute(attrib);
}

SYS_FORCE_INLINE
static bool
attribDelete(
    GA_Attribute* attrib
)
{
    UT_ASSERT_P(attrib);
    return attrib->getDetail().getAttributes().destroyAttribute(attrib);
}


SYS_FORCE_INLINE
static bool
    destroyAttribute(
        GA_Attribute* attrib
    )
{
    UT_ASSERT_P(attrib);
    return attrib->getDetail().getAttributes().destroyAttribute(attrib);
}




SYS_FORCE_INLINE
static bool
renameAttribute(
    GA_Detail* geo,
    const GA_Attribute* attrib,
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
    GA_Attribute* attrib,
    const UT_StringHolder& newName
)
{
    UT_ASSERT_P(attrib);
    return attrib->getDetail().renameAttribute(attrib->getOwner(), attrib->getScope(), attrib->getName(), newName);
}





static bool
forceRenameAttribute(
    GA_Detail* geo,
    GA_Attribute* attrib,
    const UT_StringHolder& newName
)
{
    GA_Attribute* existAttribute = geo->findAttribute(attrib->getOwner(), newName);
    if (existAttribute)
        geo->getAttributes().destroyAttribute(existAttribute);
    return geo->renameAttribute(GA_ATTRIB_POINT, GA_SCOPE_PUBLIC, attrib->getName(), newName);
}


SYS_FORCE_INLINE
static bool
forceRenameAttribute(
    GA_Attribute* attrib,
    const UT_StringHolder& newName
)
{
    GA_Detail& geo = attrib->getDetail();
    return forceRenameAttribute(&geo, attrib, newName);
}






//GA_FeE_Attribute::findFloatTuplePointVertex(outGeo0, geo0AttribClass, geo0AttribNameSub, geo0AttribClassFinal)
//GA_FeE_Attribute::findFloatTuplePointVertex(geo, attribOwner, GA_SCOPE_PUBLIC, attribName, attribOwnerFianl);

static const GA_Attribute*
findFloatTuplePointVertex(
    const GEO_Detail* geo,
    const GA_AttributeOwner attribOwner,
    const GA_AttributeScope scope,
    const UT_StringRef& attribName,
    GA_AttributeOwner& attribOwnerFinal,
    const int min_size = 1,
    const int max_size = -1
)
{
    const GA_Attribute* attribPtr = nullptr;
    if (attribOwner >= GA_ATTRIB_PRIMITIVE)//means Auto
    {
        attribPtr = geo->findFloatTuple(GA_ATTRIB_VERTEX, scope, attribName, min_size, max_size);
        if (attribPtr)
            attribOwnerFinal = GA_ATTRIB_VERTEX;
        else
        {
            attribPtr = geo->findFloatTuple(GA_ATTRIB_POINT, scope, attribName, min_size, max_size);
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
        attribPtr = geo->findFloatTuple(attribOwner, scope, attribName, min_size, max_size);
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
findFloatTuplePointVertex(
    GEO_Detail* geo,
    const GA_AttributeOwner attribOwner,
    const GA_AttributeScope scope,
    const UT_StringRef& attribName,
    GA_AttributeOwner& attribOwnerFinal,
    const int min_size = 1,
    const int max_size = -1
)
{
    GA_Attribute* attribPtr = nullptr;
    if (attribOwner >= GA_ATTRIB_PRIMITIVE)//means Auto
    {
        attribPtr = geo->findFloatTuple(GA_ATTRIB_VERTEX, scope, attribName, min_size, max_size);
        if (attribPtr)
            attribOwnerFinal = GA_ATTRIB_VERTEX;
        else
        {
            attribPtr = geo->findFloatTuple(GA_ATTRIB_POINT, scope, attribName, min_size, max_size);
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
        attribPtr = geo->findFloatTuple(attribOwner, scope, attribName, min_size, max_size);
        if (!attribPtr)
        {
            attribOwnerFinal = GA_ATTRIB_INVALID;
            return nullptr;
        }
        attribOwnerFinal = attribOwner;
    }
    return attribPtr;
}




//GA_FeE_Attribute::findFloatTuplePointVertex(outGeo0, geo0AttribClass, geo0AttribNameSub, geo0AttribClassFinal)
//GA_FeE_Attribute::findFloatTuplePointVertex(geo, attribOwner, attribName, attribOwnerFianl);



SYS_FORCE_INLINE
static const GA_Attribute*
findFloatTuplePointVertex(
    const GEO_Detail* geo,
    const GA_AttributeOwner attribOwner,
    const UT_StringRef& attribName,
    GA_AttributeOwner& attribOwnerFianl,
    const int min_size = 1,
    const int max_size = -1
)
{
    return findFloatTuplePointVertex(geo, attribOwner, GA_SCOPE_PUBLIC, attribName, attribOwnerFianl, min_size, max_size);
}


SYS_FORCE_INLINE
static GA_Attribute*
findFloatTuplePointVertex(
    GEO_Detail* geo,
    const GA_AttributeOwner attribOwner,
    const UT_StringRef& attribName,
    GA_AttributeOwner& attribOwnerFianl,
    const int min_size = 1,
    const int max_size = -1
)
{
    return findFloatTuplePointVertex(geo, attribOwner, GA_SCOPE_PUBLIC, attribName, attribOwnerFianl, min_size, max_size);
}





//GA_FeE_Attribute::normalizeElementAttrib(geo0SplittableRange, attribPtr,
//    doNormalize, uniScale,
//    subscribeRatio, minGrainSize);


static void
normalizeAttribElement(
    const GA_SplittableRange& geoSplittableRange,
    GA_Attribute* attribPtr,
    const bool doNormalize = 1,
    const fpreal64 uniScale = 1,
    const int subscribeRatio = 64,
    const int minGrainSize = 64
)
{
    UTparallelFor(geoSplittableRange, [&attribPtr, &doNormalize, &uniScale](const GA_SplittableRange& r)
    {
        GA_PageHandleV<UT_Vector3F>::RWType attrib_ph(attribPtr);
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


static void
normalizeAttribElement(
    const GA_SplittableRange& geoSplittableRange,
    const GA_RWHandleT<UT_Vector3F>& attribHandle,
    const bool doNormalize = 1,
    const fpreal64 uniScale = 1,
    const int subscribeRatio = 64,
    const int minGrainSize = 64
)
{
    UTparallelFor(geoSplittableRange, [&attribHandle, &doNormalize, &uniScale](const GA_SplittableRange& r)
    {
        GA_Offset start, end;
        for (GA_Iterator it(r); it.blockAdvance(start, end); )
        {
            for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
            {
                UT_Vector3F attribValue = attribHandle.get(elemoff);
                if (doNormalize)
                    attribValue.normalize();
                attribValue *= uniScale;
                attribHandle.set(elemoff, attribValue);
            }
        }
    }, subscribeRatio, minGrainSize);
}




} // End of namespace GA_FeE_Attribute

#endif
