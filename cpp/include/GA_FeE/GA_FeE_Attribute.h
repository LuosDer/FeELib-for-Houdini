
#pragma once

#ifndef __GA_FeE_Attribute_h__
#define __GA_FeE_Attribute_h__

//#include <GA_FeE/GA_FeE_Attribute.h>

#include <GEO/GEO_Detail.h>
#include <GEO/GEO_PrimPoly.h>
#include <OP/OP_Operator.h>
#include <OP/OP_OperatorTable.h>
#include <PRM/PRM_Include.h>
#include <PRM/PRM_TemplateBuilder.h>
#include <UT/UT_DSOVersion.h>
#include <UT/UT_Interrupt.h>
#include <UT/UT_StringHolder.h>
#include <SYS/SYS_Math.h>
#include <limits.h>


#include <GA/GA_Defaults.h>



namespace GA_FeE_Attribute {

//GA_FeE_Attribute::findFloatTuplePointVertex(outGeo0, geo0AttribClass, geo0AttribNameSub, attribPtr, geo0AttribClassFinal)
//GA_FeE_Attribute::findFloatTuplePointVertex(geo, attribOwner, GA_SCOPE_PUBLIC, attribName, attribPtr, attribOwnerFianl);

static bool
    findFloatTuplePointVertex(
        GEO_Detail* const geo,
        const GA_AttributeOwner& attribOwner,
        const GA_AttributeScope& scope,
        const UT_StringRef& attribName,
        GA_Attribute*& attribPtr,
        GA_AttributeOwner& attribOwnerFinal,
        const int& min_size = 1,
        const int& max_size = -1
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
    const GA_AttributeOwner& attribOwner,
    const UT_StringRef& attribName,
    GA_Attribute*& attribPtr,
    GA_AttributeOwner& attribOwnerFianl,
    const int& min_size = 1,
    const int& max_size = -1
)
{
    return GA_FeE_Attribute::findFloatTuplePointVertex(geo, attribOwner, GA_SCOPE_PUBLIC, attribName, attribPtr, attribOwnerFianl, min_size, max_size);
}




} // End of namespace GA_FeE_Attribute

#endif
