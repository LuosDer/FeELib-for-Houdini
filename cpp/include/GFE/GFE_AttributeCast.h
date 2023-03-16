
#pragma once

#ifndef __GFE_AttributeCast_h__
#define __GFE_AttributeCast_h__

//#include "GFE/GFE_AttributeCast.h"

#include "GA/GA_Detail.h"
#include "GFE/GFE_Type.h"

namespace GFE_AttributeCast {

    

    template<typename T>
    static void
    setAttribValue1(
        GA_Detail* const geo,
        GA_Attribute* const attrib,
        const GA_SplittableRange& geoSplittableRange,
        const exint subscribeRatio = 64,
        const exint minGrainSize = 128
    )
    {
        const GA_RWHandleT<T> attrib_h(attrib);
        UTparallelFor(geoSplittableRange, [geo, &attrib_h](const GA_SplittableRange& r)
        {
            GA_Offset start, end;
            for (GA_Iterator it(r); it.blockAdvance(start, end); )
            {
                for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                {
                    attrib_h.set(elemoff, T(1));
                }
            }
        }, subscribeRatio, minGrainSize);
    }

    static void
        setAttribStringValue1(
            GA_Detail* const geo,
            GA_Attribute* const attrib,
            const GA_SplittableRange& geoSplittableRange,
            const exint subscribeRatio = 64,
            const exint minGrainSize = 128
        )
    {
        const GA_RWHandleS attrib_h(attrib);
        UTparallelFor(geoSplittableRange, [geo, &attrib_h](const GA_SplittableRange& r)
        {
            GA_Offset start, end;
            for (GA_Iterator it(r); it.blockAdvance(start, end); )
            {
                for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                {
                    attrib_h.set(elemoff, "1");
                }
            }
        }, subscribeRatio, minGrainSize);
    }



    static bool
        attribCast(
            GA_Detail* const geo,
            const GA_Group* const group,
            const GA_StorageClass newStorageClass,
            const UT_StringHolder& newName,
            const GA_Precision precision = GA_PRECISION_INVALID,
            const exint subscribeRatio = 64,
            const exint minGrainSize = 128
        )
    {
        UT_ASSERT_P(geo);
        UT_ASSERT_P(group);

        if (newStorageClass == GA_STORECLASS_OTHER)//group
            return false;

        if (!group->isElementGroup())
            return false;

        const GA_Precision precisionFinal = precision == GA_PRECISION_INVALID ? geo->getPreferredPrecision() : precision;
        const UT_StringHolder& newNameFinal = (newName.isstring() && newName.length() != 0) ? group->getName() : newName;
        //const GA_GroupType classType = group->classType();
        const GA_AttributeOwner attribClass = GFE_Type::attributeOwner_groupType(group->classType());

        GA_SplittableRange geoSplittableRange = GFE_Group::getSplittableRangeByAnyGroup(geo, group);

        GA_Attribute* attrib = nullptr;
        switch (newStorageClass)
        {
        case GA_STORECLASS_INT:
        {
            attrib = geo->getAttributes().createTupleAttribute(attribClass, newNameFinal, GFE_Type::getPreferredStorageI(precisionFinal), 1, GA_Defaults(0));
            
            switch (precisionFinal)
            {
            case GA_PRECISION_8:
                setAttribValue1<int8> (geo, attrib, geoSplittableRange, subscribeRatio, minGrainSize);
                return true; break;
            case GA_PRECISION_16:
                setAttribValue1<int16>(geo, attrib, geoSplittableRange, subscribeRatio, minGrainSize);
                return true; break;
            case GA_PRECISION_32:
                setAttribValue1<int32>(geo, attrib, geoSplittableRange, subscribeRatio, minGrainSize);
                return true; break;
            case GA_PRECISION_64:
                setAttribValue1<int64>(geo, attrib, geoSplittableRange, subscribeRatio, minGrainSize);
                return true; break;
            default:         break;
            }
        }
            break;
        case GA_STORECLASS_REAL:
        {
            attrib = geo->getAttributes().createTupleAttribute(attribClass, newNameFinal, GFE_Type::getPreferredStorageF(precisionFinal), 1, GA_Defaults(0));
            switch (precisionFinal)
            {
            case GA_PRECISION_16:
                setAttribValue1<fpreal16>(geo, attrib, geoSplittableRange, subscribeRatio, minGrainSize);
                return true; break;
            case GA_PRECISION_32:
                setAttribValue1<fpreal32>(geo, attrib, geoSplittableRange, subscribeRatio, minGrainSize);
                return true; break;
            case GA_PRECISION_64:
                setAttribValue1<fpreal64>(geo, attrib, geoSplittableRange, subscribeRatio, minGrainSize);
                return true; break;
            default:         break;
            }
        }
            break;
        case GA_STORECLASS_STRING:
        {
            attrib = geo->getAttributes().createStringAttribute(attribClass, newNameFinal, 1);
            setAttribStringValue1(geo, attrib, geoSplittableRange, subscribeRatio, minGrainSize);
            return true;
        }
            break;
        case GA_STORECLASS_DICT:
            break;

        case GA_STORECLASS_OTHER:
            return false;
            break;
        default:
            break;
        }
        //return geo->changeAttributeStorage(attribClass, group->getName(), newStorage);
        UT_ASSERT_MSG(0, "Unhandled Precision!");
        return false;
    }

    SYS_FORCE_INLINE
    static bool
    attribCast(
        GA_Detail* const geo,
        GA_Group* const group,
        const GA_StorageClass newStorageClass,
        const UT_StringHolder& newName,
        const GA_Precision precision = GA_PRECISION_INVALID,
        const bool delOriginal = false,
        const exint subscribeRatio = 64,
        const exint minGrainSize = 128
    )
    {
        const bool success = attribCast(geo, static_cast<const GA_Group*>(group), newStorageClass, newName, precision, subscribeRatio, minGrainSize);
        if (delOriginal)
        {
            geo->destroyGroup(group);
        }
        return success;
    }


    

    static bool
        changeAttribStorageClass(
            GA_Detail* const geo,
            const GA_AttributeOwner attribClass,
            const UT_StringHolder& attribName,
            const GA_StorageClass newStorageClass,
            const GA_Precision precision = GA_PRECISION_INVALID
        )
    {
        GEO_Detail* const geoGEO = static_cast<GEO_Detail*>(geo);
        const GA_Precision precisionFinal = precision == GA_PRECISION_INVALID ? geo->getPreferredPrecision() : precision;

        //GA_Attribute* attrib = nullptr;
        switch (newStorageClass)
        {
        case GA_STORECLASS_INT:
        {
            //attrib = geo->getAttributes().createTupleAttribute(attribClass, newNameFinal, GFE_Type::getPreferredStorageI(precisionFinal), 1, GA_Defaults(0));

            switch (precision)
            {
            case GA_PRECISION_8:
                geoGEO->changeAttributeStorage(attribClass, attribName, GA_STORE_INT8);
                break;
            case GA_PRECISION_16:
                geoGEO->changeAttributeStorage(attribClass, attribName, GA_STORE_INT16);
                break;
            case GA_PRECISION_32:
                geoGEO->changeAttributeStorage(attribClass, attribName, GA_STORE_INT32);
                break;
            case GA_PRECISION_64:
                geoGEO->changeAttributeStorage(attribClass, attribName, GA_STORE_INT64);
                break;
            default:         break;
            }
        }
        break;
        case GA_STORECLASS_REAL:
        {
            //attrib = geo->getAttributes().createTupleAttribute(attribClass, newNameFinal, GFE_Type::getPreferredStorageF(precisionFinal), 1, GA_Defaults(0));

            switch (precision)
            {
            case GA_PRECISION_16:
                geoGEO->changeAttributeStorage(attribClass, attribName, GA_STORE_REAL16);
                break;
            case GA_PRECISION_32:
                geoGEO->changeAttributeStorage(attribClass, attribName, GA_STORE_REAL32);
                break;
            case GA_PRECISION_64:
                geoGEO->changeAttributeStorage(attribClass, attribName, GA_STORE_REAL64);
                break;
            default:         break;
            }
        }
        break;
        case GA_STORECLASS_STRING:
        {
            geoGEO->changeAttributeStorage(attribClass, attribName, GA_STORE_STRING);
            return true;
        }
        break;
        case GA_STORECLASS_DICT:
            geoGEO->changeAttributeStorage(attribClass, attribName, GA_STORE_DICT);
            return false;
            break;
        case GA_STORECLASS_OTHER:
            return false;
            break;
        default:
            UT_ASSERT_MSG(0, "Unhandled Precision!");
            break;
        }
        //return geo->changeAttributeStorage(attribClass, group->getName(), newStorage);
        return true;
    }

    static bool
        attribCast(
            GA_Detail* const geo,
            const GA_Attribute* const attribPtr,
            const GA_StorageClass newStorageClass,
            const UT_StringHolder& newName,
            const GA_Precision precision = GA_PRECISION_INVALID,
            const exint subscribeRatio = 64,
            const exint minGrainSize = 128
        )
    {
        UT_ASSERT_P(geo);
        UT_ASSERT_P(attribPtr);
        
        const GA_StorageClass inStorageClass = attribPtr->getStorageClass();
        if (newStorageClass == inStorageClass)
            return false;

        const GA_AttributeOwner attribClass = attribPtr->getOwner();
        const UT_StringHolder& attribName = attribPtr->getName();

        const UT_StringHolder& newNameFinal = (newName.isstring() && newName.length() != 0) ? attribName : newName;

        return changeAttribStorageClass(geo, attribClass, attribName, newStorageClass, precision);
    }

    //SYS_FORCE_INLINE
    //    static bool
    //    attribCast(
    //        GA_Detail* geo,
    //        GA_Attribute* attribPtr,
    //        const GA_StorageClass newStorageClass,
    //        const UT_StringHolder& newName,
    //        const GA_Precision precision = GA_PRECISION_INVALID,
    //        const bool delOriginal = false,
    //        const exint subscribeRatio = 64,
    //        const exint minGrainSize = 128
    //    )
    //{
    //    const bool success = attribCast(geo, static_cast<const GA_Attribute*>(attribPtr), newStorageClass, newName, precision, subscribeRatio, minGrainSize);
    //    if (delOriginal) {
    //        GFE_Attribute::destroyAttribute(attribPtr);
    //    }
    //    return success;
    //}















} // End of namespace GFE_AttributeCast

#endif
