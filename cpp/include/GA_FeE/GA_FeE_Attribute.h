
#pragma once

#ifndef __GA_FeE_Attribute_h__
#define __GA_FeE_Attribute_h__

//#include <GA_FeE/GA_FeE_Attribute.h>

#include <GA/GA_Detail.h>


namespace GA_FeE_Attribute {


static GA_Storage
getPreferredStorageI(
        const GA_Detail* geo
    )
{
    switch (geo->getPreferredPrecision())
    {
    case GA_PRECISION_INVALID:
        break;
    case GA_PRECISION_1:
        break;
    case GA_PRECISION_8:    return  GA_STORE_INT8;    break;
    case GA_PRECISION_16:   return  GA_STORE_INT16;   break;
    case GA_PRECISION_32:   return  GA_STORE_INT32;   break;
    case GA_PRECISION_64:   return  GA_STORE_INT64;   break;
    default:
        break;
    }
    UT_ASSERT_MSG(0, "Unhandled Precision!");
}

static GA_Storage
getPreferredStorageF(
    const GA_Detail* geo
)
{
    switch (geo->getPreferredPrecision())
    {
    case GA_PRECISION_INVALID:   break;
    case GA_PRECISION_1:         break;
    case GA_PRECISION_8:         break;
    case GA_PRECISION_16:        return  GA_STORE_REAL16;   break;
    case GA_PRECISION_32:        return  GA_STORE_REAL32;   break;
    case GA_PRECISION_64:        return  GA_STORE_REAL64;   break;
    default:                     break;
    }
    UT_ASSERT_MSG(0, "Unhandled Precision!");
}





static bool
attribCast(
    const GEO_Detail* geo,
    const GA_Group* group,
    const GA_StorageClass newStorageClass,
    const bool detached = false,
    const bool delOriginal = false
)
{
    const GA_GroupType classType = group->classType();
    //if (newStorageClass == GA_STORECLASS_OTHER && group->classType() == GA_GROUP_EDGE)
    //    return static_cast<GA_Attribute*>(static_cast<GA_ElementGroup*>(group));
    switch (newStorageClass)
    {
    case GA_STORECLASS_INVALID:     break;
    case GA_STORECLASS_INT:
        if (!group->isElementGroup())
            return false;
        static_cast<GA_ElementGroup*>group

        break;
    case GA_STORECLASS_REAL:
        break;
    case GA_STORECLASS_STRING:
        break;
    case GA_STORECLASS_DICT:
        break;
    case GA_STORECLASS_OTHER:
        return changeAttributeStorage(, group->getName(), newStorageClass);
        switch (classType)
        {
        case GA_GROUP_POINT:
            break;
        case GA_GROUP_PRIMITIVE:
            break;
        case GA_GROUP_EDGE:
            break;
        case GA_GROUP_VERTEX:
            break;
        default:
            break;
        }
        break;
    default:
        break;
    }
    UT_ASSERT_MSG(0, "Unhandled Precision!");
    return false;
}







static 
sopUnsharedAttribType(SOP_FeE_GroupUnshared_1_0Parms::UnsharedAttribType parmgrouptype)
{
    using namespace SOP_FeE_GroupUnshared_1_0Enums;
    switch (parmgrouptype)
    {
    case UnsharedAttribType::GROUP:   return GA_STORECLASS_OTHER;    break;
    case UnsharedAttribType::INT:     return GA_STORECLASS_INT;      break;
    case UnsharedAttribType::FLOAT:   return GA_STORECLASS_REAL;     break;
    case UnsharedAttribType::STRING:  return GA_STORECLASS_STRING;   break;
    }
    UT_ASSERT_MSG(0, "Unhandled geo0Group type!");
    return GA_STORECLASS_INVALID;
}





//GA_FeE_Attribute::findFloatTuplePointVertex(outGeo0, geo0AttribClass, geo0AttribNameSub, attribPtr, geo0AttribClassFinal)
//GA_FeE_Attribute::findFloatTuplePointVertex(geo, attribOwner, GA_SCOPE_PUBLIC, attribName, attribPtr, attribOwnerFianl);

static bool
    findFloatTuplePointVertex(
        GEO_Detail* const geo,
        const GA_AttributeOwner attribOwner,
        const GA_AttributeScope scope,
        const UT_StringRef& attribName,
        GA_Attribute*& attribPtr,
        GA_AttributeOwner& attribOwnerFinal,
        const int min_size = 1,
        const int max_size = -1
    )
{
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
                return false;
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
            return false;
        }
        attribOwnerFinal = attribOwner;
    }
    return true;
}

//GA_FeE_Attribute::findFloatTuplePointVertex(outGeo0, geo0AttribClass, geo0AttribNameSub, attribPtr, geo0AttribClassFinal)
//GA_FeE_Attribute::findFloatTuplePointVertex(geo, attribOwner, attribName, attribPtr, attribOwnerFianl);
static bool
findFloatTuplePointVertex(
    GEO_Detail* const geo,
    const GA_AttributeOwner attribOwner,
    const UT_StringRef& attribName,
    GA_Attribute*& attribPtr,
    GA_AttributeOwner& attribOwnerFianl,
    const int min_size = 1,
    const int max_size = -1
)
{
    return GA_FeE_Attribute::findFloatTuplePointVertex(geo, attribOwner, GA_SCOPE_PUBLIC, attribName, attribPtr, attribOwnerFianl, min_size, max_size);
}


//GA_FeE_Attribute::normalizeElementAttrib(geo0SplittableRange, attribPtr,
//    doNormalize, uniScale,
//    subscribeRatio, minGrainSize);


static void
normalizeElementAttrib(
    const GA_SplittableRange geoSplittableRange,
    GA_Attribute* attribPtr,
    const bool doNormalize = 1,
    const fpreal64 uniScale = 1,
    const exint subscribeRatio = 64,
    const exint minGrainSize = 64
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




} // End of namespace GA_FeE_Attribute

#endif
