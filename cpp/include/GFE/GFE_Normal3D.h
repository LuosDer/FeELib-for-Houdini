
#pragma once

#ifndef __GFE_Normal3D_h__
#define __GFE_Normal3D_h__

//#include "GFE/GFE_Normal3D.h"



#include "GEO/GEO_Normal.h"



#include "GFE/GFE_GeoFilter.h"



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
        setHasComputed();
        this->cuspAngleDegrees = cuspAngleDegrees;
        this->normalMethod = normalMethod;
        this->copyOrigIfZero = copyOrigIfZero;
    }

    SYS_FORCE_INLINE GA_Attribute* getAttrib() const
    {
        //UT_ASSERT_MSG(!getOutAttribArray().isEmpty(), "no attrib found");
        return getOutAttribArray().isEmpty() ? nullptr : getOutAttribArray()[0];
    }


private:

    virtual bool
        computeCore() override
    {
        if (getOutAttribArray().isEmpty())
            return false;

        if (groupParser.isEmpty())
            return true;

        GA_Attribute* const normalAttrib = getOutAttribArray()[0];

        GEOcomputeNormals(*geo->asGEO_Detail(), normalAttrib, groupParser.getGroup(normalAttrib),
            cuspAngleDegrees, normalMethod, copyOrigIfZero);

        return true;
    }



public:
    float cuspAngleDegrees = GEO_DEFAULT_ADJUSTED_CUSP_ANGLE;
    GEO_NormalMethod normalMethod = GEO_NormalMethod::ANGLE_WEIGHTED;
    bool copyOrigIfZero = false;

}; // End of class GFE_Normal3D

















//
//namespace GFE_Normal3D_Namespace {
//
//    //#define GFE_Temp_Normal3DAttribName "__Normal3D_SOP_FeE_Normal"
//    #define GFE_Temp_Normal2DAttribName "__Normal2D_SOP_FeE_Normal"
//
//    //constexpr UT_StringHolder& GFE_Temp_Normal3DAttribName = "__Normal3D_SOP_FeE_Normal";
//
//
//    SYS_FORCE_INLINE
//    static void
//        computeNormal3D(
//            GA_Detail* const geo,
//            const GA_ROHandleT<UT_Vector3T<fpreal32>>& posAttrib_h,
//            const GA_RWHandleT<UT_Vector3T<fpreal32>>& normal3DAttrib_h,
//            const GA_Group* const geoGroup = nullptr,
//            const float cuspangledegrees = GEO_DEFAULT_ADJUSTED_CUSP_ANGLE,
//            const GEO_NormalMethod normalMethod = GEO_NormalMethod::ANGLE_WEIGHTED,
//            const bool copy_orig_if_zero = false
//        )
//    {
//        GEOcomputeNormals(*static_cast<GEO_Detail*>(geo), posAttrib_h, normal3DAttrib_h, geoGroup, cuspangledegrees, normalMethod, copy_orig_if_zero);
//    }
//
//#if SYS_VERSION_MAJOR_INT > 19 || ( SYS_VERSION_MAJOR_INT == 19 && SYS_VERSION_MINOR_INT == 5 )
//    static void
//        computeNormal3D(
//            GA_Detail* const geo,
//            const GA_ROHandleT<UT_Vector3T<fpreal64>>& posAttrib_h,
//            const GA_RWHandleT<UT_Vector3T<fpreal64>>& normal3DAttrib_h,
//            const GA_Group* const geoGroup = nullptr,
//            const float cuspangledegrees = GEO_DEFAULT_ADJUSTED_CUSP_ANGLE,
//            const GEO_NormalMethod normalMethod = GEO_NormalMethod::ANGLE_WEIGHTED,
//            const bool copy_orig_if_zero = false
//        )
//    {
//        GEOcomputeNormals(*static_cast<GEO_Detail*>(geo), posAttrib_h, normal3DAttrib_h, geoGroup, cuspangledegrees, normalMethod, copy_orig_if_zero);
//    }
//#endif
//
//    SYS_FORCE_INLINE
//    static void
//        computeNormal3D(
//            GA_Detail* const geo,
//            const GA_Attribute* const posAttrib,
//            GA_Attribute* const normal3DAttrib,
//            const GA_Group* const geoGroup = nullptr,
//            const GA_Storage storage = GA_STORE_INVALID,
//            const float cuspangledegrees = GEO_DEFAULT_ADJUSTED_CUSP_ANGLE,
//            const GEO_NormalMethod normalMethod = GEO_NormalMethod::ANGLE_WEIGHTED,
//            const bool copy_orig_if_zero = false
//        )
//    {
//        UT_ASSERT_P(posAttrib);
//        UT_ASSERT_P(normal3DAttrib);
//
//#if SYS_VERSION_MAJOR_INT > 19 || ( SYS_VERSION_MAJOR_INT == 19 && SYS_VERSION_MINOR_INT == 5 )
//        const GA_Storage finalStorage = storage == GA_STORE_INVALID ? posAttrib->getAIFTuple()->getStorage(posAttrib) : storage;
//        if (finalStorage == GA_STORE_REAL64)
//        {
//            computeNormal3D(geo, GA_ROHandleV3D(posAttrib), normal3DAttrib, geoGroup, cuspangledegrees, normalMethod, copy_orig_if_zero);
//        }
//        else
//        {
//            computeNormal3D(geo, GA_ROHandleV3(posAttrib), normal3DAttrib, geoGroup, cuspangledegrees, normalMethod, copy_orig_if_zero);
//        }
//#else
//        computeNormal3D(geo, GA_ROHandleV3(posAttrib), normal3DAttrib, geoGroup, cuspangledegrees, normalMethod, copy_orig_if_zero);
//#endif
//    }
//
//    //GFE_Normal3D::addAttribNormal3D(geo, nullptr, attribClass, GA_STORE_REAL32, "N", GEO_DEFAULT_ADJUSTED_CUSP_ANGLE, GEO_NormalMethod::ANGLE_WEIGHTED, false, nullptr);
//    //GFE_Normal3D::addAttribNormal3D(geo, geoGroup, attribClass, storage, normal3DAttribName, cuspangledegrees, normalMethod, copy_orig_if_zero, posAttrib);
//
//    static GA_Attribute*
//        addAttribNormal3D(
//            GA_Detail* const geo,
//            const GA_Attribute* posAttrib = nullptr,
//            const GA_Group* const geoGroup = nullptr,
//            const GA_Storage storage = GA_STORE_INVALID,
//            const UT_StringHolder& normal3DAttribName = "N",
//
//            const GA_AttributeOwner attribClass = GA_ATTRIB_VERTEX,
//            const float cuspangledegrees = GEO_DEFAULT_ADJUSTED_CUSP_ANGLE,
//            const GEO_NormalMethod normalMethod = GEO_NormalMethod::ANGLE_WEIGHTED,
//            const bool copy_orig_if_zero = false
//        )
//    {
//        UT_ASSERT_P(attribClass != GA_ATTRIB_OWNER_N && attribClass != GA_ATTRIB_INVALID);
//
//        if (!posAttrib)
//            posAttrib = geo->findPointAttribute("P");
//
//        UT_ASSERT_P(posAttrib);
//        UT_ASSERT_P(posAttrib->getAIFTuple());
//
//        GA_Attribute* normal3DAttrib = geo->findAttribute(attribClass, normal3DAttribName);
//        if (normal3DAttrib)
//            geo->getAttributes().destroyAttribute(normal3DAttrib);
//
//        const GA_Storage finalStorage = storage == GA_STORE_INVALID ? posAttrib->getAIFTuple()->getStorage(posAttrib) : storage;
//
//        UT_ASSERT_P(
//            finalStorage == GA_STORE_REAL16 ||
//            finalStorage == GA_STORE_REAL32 ||
//            finalStorage == GA_STORE_REAL64);
//
//        normal3DAttrib = geo->getAttributes().createTupleAttribute(attribClass, normal3DAttribName, finalStorage, 3, GA_Defaults(0.0));
//
//        computeNormal3D(geo, posAttrib, normal3DAttrib, geoGroup, finalStorage, cuspangledegrees, normalMethod, copy_orig_if_zero);
//
//        return normal3DAttrib;
//    }
//
//
//    static GA_AttributeUPtr
//        addDetachedAttribNormal3D(
//            GA_Detail* const geo,
//            const GA_Attribute* posAttrib = nullptr,
//            const GA_Group* const geoGroup = nullptr,
//            const GA_Storage storage = GA_STORE_INVALID,
//
//            const GA_AttributeOwner attribClass = GA_ATTRIB_VERTEX,
//            const float cuspangledegrees = GEO_DEFAULT_ADJUSTED_CUSP_ANGLE,
//            const GEO_NormalMethod normalMethod = GEO_NormalMethod::ANGLE_WEIGHTED,
//            const bool copy_orig_if_zero = false
//        )
//    {
//        UT_ASSERT_P(attribClass != GA_ATTRIB_OWNER_N && attribClass != GA_ATTRIB_INVALID);
//
//        if (!posAttrib)
//            posAttrib = geo->findPointAttribute("P");
//
//        UT_ASSERT_P(posAttrib);
//        UT_ASSERT_P(posAttrib->getAIFTuple());
//
//        const GA_Storage finalStorage = storage == GA_STORE_INVALID ? posAttrib->getAIFTuple()->getStorage(posAttrib) : storage;
//
//        UT_ASSERT_P(
//            finalStorage == GA_STORE_REAL16 ||
//            finalStorage == GA_STORE_REAL32 ||
//            finalStorage == GA_STORE_REAL64);
//
//        GA_AttributeUPtr normal3DAttribUPtr = geo->getAttributes().createDetachedTupleAttribute(attribClass, finalStorage, 3, GA_Defaults(0.0));
//        GA_Attribute* const normal3DAttrib = normal3DAttribUPtr.get();
//
//        computeNormal3D(geo, posAttrib, normal3DAttrib, geoGroup, finalStorage, cuspangledegrees, normalMethod, copy_orig_if_zero);
//
//        return normal3DAttribUPtr;
//    }
//
//    //GFE_Normal3D::findAndAddAttribNormal3D();
//
//    //!!!!! WARNING !!!!! this func may return a detached attrib
//    static GA_Attribute*
//        findOrAddAttribNormal3D(
//            GA_Detail* const geo,
//            GA_AttributeUPtr& normal3DAttribUPtr,
//            const GA_Attribute* const posAttrib = nullptr,
//            const GA_Group* const geoGroup = nullptr,
//            const GA_Storage storage = GA_STORE_INVALID,
//            const UT_StringHolder& normal3DAttribName = "N",
//
//            const bool findNormal3D = false,
//            const bool addNormal3DIfNoFind = true,
//            const GFE_NormalSearchOrder normalSearchOrder = GFE_NormalSearchOrder::INVALID,
//            const float cuspangledegrees = GEO_DEFAULT_ADJUSTED_CUSP_ANGLE,
//            const GEO_NormalMethod normalMethod = GEO_NormalMethod::ANGLE_WEIGHTED,
//            const bool copy_orig_if_zero = false
//        )
//    {
//        GA_Attribute* normal3DAttrib = nullptr;
//        if (findNormal3D)
//        {
//            normal3DAttrib = GFE_Attribute::findNormal3D(geo, normalSearchOrder, normal3DAttribName);
//        }
//
//        if (!findNormal3D || (addNormal3DIfNoFind && !normal3DAttrib))
//        {
//            GA_AttributeOwner geoNormal3DAttribClass;
//            switch (normalSearchOrder)
//            {
//            case GFE_NormalSearchOrder::PRIMITIVE:
//                geoNormal3DAttribClass = GA_ATTRIB_PRIMITIVE;
//                break;
//            case GFE_NormalSearchOrder::POINT:
//                geoNormal3DAttribClass = GA_ATTRIB_POINT;
//                break;
//            case GFE_NormalSearchOrder::VERTEX:
//                geoNormal3DAttribClass = GA_ATTRIB_VERTEX;
//                break;
//            default:
//                return nullptr;
//                break;
//            }
//            //if (!findNormal3D)
//            //{
//            //    GA_Attribute* normal3DAttrib = geo->findAttribute(geoNormal3DAttribClass, normal3DAttribName);
//            //    if (normal3DAttrib)
//            //        return normal3DAttrib;
//            //}
//            normal3DAttribUPtr = addDetachedAttribNormal3D(geo, posAttrib, geoGroup, storage,
//                geoNormal3DAttribClass,
//                cuspangledegrees, normalMethod, copy_orig_if_zero);
//            normal3DAttrib = normal3DAttribUPtr.get();
//        }
//
//        //if (normal3DAttrib && normal3DAttrib->getOwner() == GA_ATTRIB_DETAIL)
//        //{
//        //    const GA_ROHandleT<UT_Vector3T<fpreal64>> normal3D_h(normal3DAttrib);
//        //    defaultNormal3D = normal3D_h.get(0);
//        //}
//
//        return normal3DAttrib;
//    }
//
//
//    static GA_Attribute*
//        findOrAddAttribNormal3D(
//            GA_Detail* const geo,
//            const GA_Attribute* posAttrib = nullptr,
//            const GA_Group* const geoGroup = nullptr,
//            const GA_Storage storage = GA_STORE_INVALID,
//            const UT_StringHolder& normal3DAttribName = "N",
//
//            const bool findNormal3D = false,
//            const bool addNormal3DIfNoFind = true,
//            const GFE_NormalSearchOrder normalSearchOrder = GFE_NormalSearchOrder::INVALID,
//            const float cuspangledegrees = GEO_DEFAULT_ADJUSTED_CUSP_ANGLE,
//            const GEO_NormalMethod normalMethod = GEO_NormalMethod::ANGLE_WEIGHTED,
//            const bool copy_orig_if_zero = false
//        )
//    {
//        GA_Attribute* normal3DAttrib = nullptr;
//        if (findNormal3D)
//        {
//            normal3DAttrib = GFE_Attribute::findNormal3D(geo, normalSearchOrder, normal3DAttribName);
//        }
//
//        if (!findNormal3D || (addNormal3DIfNoFind && !normal3DAttrib))
//        {
//            GA_AttributeOwner geoNormal3DAttribClass;
//            switch (normalSearchOrder)
//            {
//            case GFE_NormalSearchOrder::PRIMITIVE:
//                geoNormal3DAttribClass = GA_ATTRIB_PRIMITIVE;
//                break;
//            case GFE_NormalSearchOrder::POINT:
//                geoNormal3DAttribClass = GA_ATTRIB_POINT;
//                break;
//            case GFE_NormalSearchOrder::VERTEX:
//                geoNormal3DAttribClass = GA_ATTRIB_VERTEX;
//                break;
//            default:
//                return nullptr;
//                break;
//            }
//            //if (!findNormal3D)
//            //{
//            //    GA_Attribute* normal3DAttrib = geo->findAttribute(geoNormal3DAttribClass, normal3DAttribName);
//            //    if (normal3DAttrib)
//            //        return normal3DAttrib;
//            //}
//            normal3DAttrib = addAttribNormal3D(geo, posAttrib, geoGroup, storage, normal3DAttribName, geoNormal3DAttribClass,
//                cuspangledegrees, normalMethod, copy_orig_if_zero);
//        }
//        //if (normal3DAttrib && normal3DAttrib->getOwner() == GA_ATTRIB_DETAIL)
//        //{
//        //    const GA_ROHandleT<UT_Vector3T<fpreal64>> normal3D_h(normal3DAttrib);
//        //    defaultNormal3D = normal3D_h.get(0);
//        //}
//
//        return normal3DAttrib;
//    }
//
//
//
//
//
//
//
//} // End of namespace GFE_Normal3D
//
//





#endif
