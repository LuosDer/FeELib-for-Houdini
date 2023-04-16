
#pragma once

#ifndef __GFE_AttribDuplicate_h__
#define __GFE_AttribDuplicate_h__

//#include "GFE/GFE_AttributeDuplicate.h"

#include "GFE/GFE_GeoFilter.h"


class GFE_AttribDuplicate : public GFE_AttribFilter {

public:
    //using GFE_AttribFilter::GFE_AttribFilter;


    void
        setComputeParm(
            const bool delSrcAttrib = false,
            const exint subscribeRatio = 64,
            const exint minGrainSize = 1024
        )
    {
        setHasComputed();
        this->delSrcAttrib = delSrcAttrib;
        this->subscribeRatio = subscribeRatio;
        this->minGrainSize = minGrainSize;
    }

    SYS_FORCE_INLINE
    void
        setSourceAttribute(
            const GA_Attribute* const srcAttribPtr
        )
    {
        this->srcAttribPtr = srcAttribPtr;
        attribOwner = srcAttribPtr->getOwner();
    }

    SYS_FORCE_INLINE
    void
        setSourceAttribute(
            GA_Attribute* const srcAttribPtr
        )
    {
        this->srcAttribPtr_nonConst = srcAttribPtr;
        setSourceAttribute(static_cast<const GA_Attribute*>(srcAttribPtr));
    }


    void
        setDestinationAttribute(
            GA_Attribute* const dstAttribPtr
        )
    {
        if (dstAttribPtr->getOwner() != attribOwner)
        {
            UT_ASSERT_MSG(0, "not same owner");
            if (cookparms)
            {
            }
            return;
        }

        this->dstAttribPtr = dstAttribPtr;
        getOutAttribArray().set(dstAttribPtr);
    }

    void
        setDestinationAttribute(
            const UT_StringHolder& attribName
        )
    {
        UT_ASSERT_MSG(srcAttribPtr, "Can NOT with no srcAttribPtr");
        UT_ASSERT_MSG(geo, "Can NOT with no geo");

        if (srcAttribPtr->isDetached())
        {
            dstAttribPtr = geo->findAttribute(attribOwner, srcAttribPtr->getName());
        }
        else
        {
            dstAttribPtr = geo->findAttribute(attribOwner, srcAttribPtr->getName());
            if (!dstAttribPtr)
                dstAttribPtr = geo->getAttributes().cloneAttribute(attribOwner, srcAttribPtr->getName(), *srcAttribPtr, true);
        }
    }

    void
        setDestinationAttribute(
            const UT_StringHolder& name
        )
    {
        UT_ASSERT_MSG(srcAttribPtr, "Can NOT with no srcAttribPtr");
        UT_ASSERT_MSG(geo, "Can NOT with no geo");

        if (srcAttribPtr->isDetached())
        {
            dstAttribPtr = geo->getAttributes().cloneAttribute(attribOwner, name, *srcAttribPtr, true);
        }
        else
        {
            dstAttribPtr = geo->findAttribute(attribOwner, srcAttribPtr->getName());
            if (!dstAttribPtr)
                dstAttribPtr = geo->getAttributes().cloneAttribute(owner, name, *srcAttribPtr, true);
        }
    }


    void
        setDetachedDestinationAttribute()
    {
        UT_ASSERT_MSG(srcAttribPtr, "Can NOT with no srcAttribPtr");
        UT_ASSERT_MSG(geo, "Can NOT with no geo");


        dstAttribPtr = getOutAttribArray().createDetachedAttribute(
            attribOwner, GA_STORECLASS_FLOAT, GA_STORE_INVALID, false,
            srcAttribPtr->getType(), srcAttribPtr->getOptions().options(), &srcAttribPtr->getOptions());

    }

    SYS_FORCE_INLINE
        GA_Attribute*
        getDestinationAttribute() const
    {
        return dstAttribPtr;
    }





private:


    virtual bool
        computeCore() override
    {
        const bool isAttribEmpty = getOutAttribArray().isEmpty();

        if (isAttribEmpty && getOutGroupArray().isEmpty())
            return false;

        //if (groupParser.isEmpty())
        //    return true;

        //const GA_AttributeOwner owner =
        //    isAttribEmpty ? 
        //    GFE_Type::attributeOwner_groupType(getRef0GroupArray()[0]->classType()) :
        //    getRef0AttribArray()[0]->getOwner();

        //const GA_GroupType groupType =
        //    isAttribEmpty ?
        //    getRef0GroupArray()[0]->classType() :
        //    GFE_Type::attributeOwner_groupType(getRef0AttribArray()[0]->getOwner());


        const GA_IndexMap& indexMap_runOver = geo->getIndexMap(attribOwner);

        const GA_Range geoRange(indexMap_runOver);
        const GA_SplittableRange geoSplittableRange(geoRange);



        const GA_GroupType groupTypeDst = GFE_Type::attributeOwner_groupType(ownerDst);

        GA_GroupTable* const groupDst = geo->getGroupTable(groupTypeDst);

        const size_t sizeRefGroup = getRef0GroupArray().size();
        for (size_t i = 0; i < sizeRefGroup; i++)
        {
            const GA_Group& groupRef = *getRef0GroupArray()[i];

            const UT_StringHolder& attribName = groupRef.getName();
            const UT_StringHolder& attribNameNew = attribName;

            GA_Group* groupNew = groupDst->find(attribNameNew);
            if (groupNew)
            {
                //GFE_Group::groupBumpDataId(groupNew);
                if (groupTypeDst == GA_GROUP_EDGE)
                {
                    static_cast<GA_EdgeGroup*>(groupNew)->bumpDataId();
                }
                else
                {
                    static_cast<GA_ElementGroup*>(groupNew)->bumpDataId();
                }
            }
            else
            {
                groupNew = groupDst->newGroup(attribNameNew);
                //if (groupType == GA_GROUP_EDGE)
                //{
                //    groupNew = groupDst->newGroup(attribNameNew);
                //}
                //else
                //{
                //    groupNew = static_cast<GA_ElementGroupTable*>(groupDst)->cloneAttribute(owner, attribNameNew, *attribRef, true);
                //}
            }
            getOutGroupArray().append(groupNew);
            groupDuplicate(*groupNew, groupRef);
        }

        GA_AttributeSet& attribDst = geo->getAttributes();

        const size_t sizeRefAttrib = getRef0AttribArray().size();
        for (size_t i = 0; i < sizeRefAttrib; i++)
        {
            const GA_Attribute& attribRef = *getRef0AttribArray()[i];

            const UT_StringHolder& attribName = attribRef.getName();
            const UT_StringHolder& attribNameNew = attribName;

            GA_Attribute* attribNew = attribDst.findAttribute(ownerDst, attribNameNew);
            if (attribNew)
                attribNew->bumpDataId();
            else
                attribNew = attribDst.cloneAttribute(ownerDst, attribNameNew, attribRef, true);

            getOutAttribArray().append(attribNew);
            attribDuplicate(geoSplittableRange, *attribNew, attribRef);
        }

        UT_ASSERT_MSG(!delSrcAttrib || srcAttribPtr_nonConst, "cant destory Src Attrib");
        if (delSrcAttrib && srcAttribPtr_nonConst)
        {
            geo->destroyAttrib(srcAttribPtr_nonConst);
        }

        return true;
    }


    SYS_FORCE_INLINE
    void
        groupDuplicate(
            GA_EdgeGroup& attribNew,
            const GA_EdgeGroup& attribRef
        )
    {
        attribNew = attribRef;
    }

    SYS_FORCE_INLINE
    void
        groupDuplicate(
            GA_ElementGroup& attribNew,
            const GA_ElementGroup& attribRef
        )
    {
        attribNew.copyMembership(attribRef);
    }

    SYS_FORCE_INLINE
    void
        groupDuplicate(
            GA_Group& attribNew,
            const GA_Group& attribRef
        )
    {
        if (attribNew.classType() == GA_GROUP_EDGE)
        {
            groupDuplicate(static_cast<GA_EdgeGroup&>(attribNew), static_cast<const GA_EdgeGroup&>(attribRef));
        }
        else
        {
            groupDuplicate(static_cast<GA_ElementGroup&>(attribNew), static_cast<const GA_ElementGroup&>(attribRef));
        }
    }



    void
        attribDuplicate(
            const GA_SplittableRange& geoSplittableRange,
            GA_Attribute& attribNew,
            const GA_Attribute& attribRef
        )
    {
        //const GA_IndexMap& indexMap = geo->getIndexMap(GFE_Type::attributeOwner_groupType(attribNew.classType()));
        const GA_IndexMap& indexMap_target = iDAttribInput ? attribRef.getIndexMap() : attribNew.getIndexMap();
        UTparallelFor(geoSplittableRange, [&attribNew, &attribRef, &indexMap_target](const GA_SplittableRange& r)
        {
            GA_Offset start, end;
            for (GA_Iterator it(r); it.blockAdvance(start, end); )
            {
                for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                {
                    //attribNew.copy(elemoff, attribRef, elemoff);
                    if (!indexMap_target.isOffsetActive(elemoff) && !indexMap_target.isOffsetTransient(elemoff))
                        continue;
                    attribNew.copy(elemoff, attribRef, elemoff);
                }
            }
        }, subscribeRatio, minGrainSize);
    }




public:
    bool delSrcAttrib = false;
    GA_Attribute* dstAttribPtr;

private:
    const GA_Attribute* srcAttribPtr;
    GA_Attribute* srcAttribPtr_nonConst;
    GA_AttributeOwner attribOwner;

    int subscribeRatio = 8;
    int minGrainSize = 1024;

}; // End of class GFE_AttribDuplicate







#endif
