
#pragma once

#ifndef __GFE_Normal3D_h__
#define __GFE_Normal3D_h__

#include "GFE/GFE_Normal3D.h"

#include "GFE/GFE_GeoFilter.h"




#include "GEO/GEO_Normal.h"


class GFE_Normal3D : public GFE_AttribFilter {

public:
    using GFE_AttribFilter::GFE_AttribFilter;


    void
        setComputeParm(
            const float cuspAngleDegrees = GEO_DEFAULT_ADJUSTED_CUSP_ANGLE,
            const GEO_NormalMethod normalMethod = GEO_NormalMethod::ANGLE_WEIGHTED,
            const bool copyOrigIfZero = false
        )
    {
        this->cuspAngleDegrees = cuspAngleDegrees;
        this->normalMethod = normalMethod;
        this->copyOrigIfZero = copyOrigIfZero;
    }

    SYS_FORCE_INLINE void setNotComputed()
    { setHasComputed(); }

    
    SYS_FORCE_INLINE void setNormal3DAttrib(const GA_Attribute* const inAttrib)
    {
        setHasComputed(true);
        normal3DAttrib = inAttrib;
    }

    SYS_FORCE_INLINE GA_Attribute* findOrCreateNormal3D(
        const bool findNormal3D,
        const GFE_NormalSearchOrder normal3DSearchOrder = GFE_NormalSearchOrder::ALL,
        const UT_StringRef& attribName = "",
        const bool addNormal3DIfNoFind = true
    )
    { return normal3DAttrib = getOutAttribArray().findOrCreateNormal3D(true,
            findNormal3D ? normal3DSearchOrder : GFE_NormalSearchOrder::PRIMITIVE, GA_STORE_INVALID,
            findNormal3D ? attribName : UT_StringHolder("")); }

    
    GA_Attribute*
    findOrCreateNormal3D(
        const bool findNormal3D = false,
        const bool detached = true,
        const GFE_NormalSearchOrder owner = GFE_NormalSearchOrder::ALL,
        const GA_Storage storage = GA_STORE_INVALID,
        const UT_StringRef& attribName = "",
        const int tupleSize = 3,
        const GA_Defaults& defaults = GA_Defaults(0.0f),
        const bool emplaceBack = true,
        const UT_Options* const create_args = nullptr,
        const GA_AttributeOptions* const attribute_options = nullptr,
        const GA_StorageClass storageClass = GA_STORECLASS_FLOAT
    )
    {
        if (findNormal3D)
        {
            const GA_Storage finalStorage = GFE_Type::getPreferredStorage(geo, storageClass, storage);
        
            GA_Attribute* const attrib = GFE_Attribute::findNormal3D(*geo, owner, attribName);
            if (GFE_Type::checkTupleAttrib(attrib, finalStorage, tupleSize, defaults))
            {
                normal3DAttrib = attrib;
                setHasComputed(true);
                return attrib;
            }
        }
        setHasComputed();
        return findOrCreateNormal3D(detached, owner, storage, attribName, tupleSize, defaults, emplaceBack, create_args, attribute_options, storageClass);
    }
    
    SYS_FORCE_INLINE GA_Attribute* findOrCreateNormal3D(
        const bool findNormal3D = false,
        const bool detached = true, 
        const GA_GroupType groupType = GA_GROUP_N,
        const GA_Storage storage = GA_STORE_INVALID,
        const UT_StringRef& attribName = "",
        const int tuple_size = 3
        )
    { return findOrCreateNormal3D(findNormal3D, detached, GFE_Attribute::toNormalSearchOrder(groupType), storage, attribName, tuple_size); }

    SYS_FORCE_INLINE GA_Attribute* findOrCreateNormal3D(
        const bool detached, 
        const GA_Group& group,
        const GA_Storage storage = GA_STORE_INVALID,
        const UT_StringRef& attribName = "",
        const int tuple_size = 3
        )
    { return findOrCreateNormal3D(detached, group.classType(), storage, attribName, tuple_size); }

    SYS_FORCE_INLINE GA_Attribute* findOrCreateNormal3D(
        const bool detached = true, 
        const GA_AttributeOwner owner = GA_ATTRIB_OWNER_N,
        const GA_Storage storage = GA_STORE_INVALID,
        const UT_StringRef& attribName = "",
        const int tuple_size = 3
        )
    { return findOrCreateNormal3D(detached, GFE_Attribute::toNormalSearchOrder(owner), storage, attribName, tuple_size); }

    
    SYS_FORCE_INLINE GA_Attribute* getAttrib() const
    {
        //UT_ASSERT_MSG(!getOutAttribArray().isEmpty(), "no attrib found");
        //return getOutAttribArray().isEmpty() ? nullptr : getOutAttribArray()[0];
        return normal3DAttrib;
    }


private:

    virtual bool
        computeCore() override
    {
        if (getHasComputed())
            return true;

        //if (getOutAttribArray().isEmpty())
        if (!normal3DAttrib)
            return false;

        if (groupParser.isEmpty())
            return true;
        
        GEOcomputeNormals(*geo->asGEO_Detail(), getOutAttribArray()[0], groupParser.getGroup(getOutAttribArray()[0]),
            cuspAngleDegrees, normalMethod, copyOrigIfZero);

        return true;
    }



public:
    fpreal cuspAngleDegrees = GEO_DEFAULT_ADJUSTED_CUSP_ANGLE;
    GEO_NormalMethod normalMethod = GEO_NormalMethod::ANGLE_WEIGHTED;
    bool copyOrigIfZero = false;

private:
    const GA_Attribute* normal3DAttrib = nullptr;
    
}; // End of class GFE_Normal3D

#endif
