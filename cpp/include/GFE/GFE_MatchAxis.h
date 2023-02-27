
#pragma once

#ifndef __GFE_MatchAxis_h__
#define __GFE_MatchAxis_h__

//#include "GFE/GFE_MatchAxis.h"

#include "GA/GA_Detail.h"


//#include "GFE/GFE_Type.h"
#include "GFE/GFE_Group.h"
#include "GFE/GFE_XformByAttrib.h"
#include "GFE/GFE_Attribute.h"



namespace GFE_MatchAxis {


template<typename FLOAT_T>
static UT_Matrix3T<FLOAT_T>
    lookat_matrix(
        const UT_Vector3T<FLOAT_T>& fromVec,
        const UT_Vector3T<FLOAT_T>& toVec,
        const UT_Vector3T<FLOAT_T>& upVec
    ) {
    //UT_Vector3T<FLOAT_T>& zeroVec = UT_Vector3T<FLOAT_T>(0);
    UT_Matrix3T<FLOAT_T> xform;
    xform.lookat(fromVec, toVec, upVec);
    return xform;
    //return invert(lookat(0, fromVec)) * lookat(0, toVec);
}


template<typename FLOAT_T>
static UT_Matrix3T<FLOAT_T>
matchAxis(
    const UT_Vector3T<FLOAT_T>& fromVec,
    const UT_Vector3T<FLOAT_T>& toVec
) {
    UT_Vector3T<FLOAT_T> zeroVec(0,0,0);
    UT_Matrix3T<FLOAT_T> xform0, xform1;
    xform0.lookat(zeroVec, fromVec);
    xform0.invert();
    xform1.lookat(zeroVec, toVec);
    return xform0 * xform1;
    //return invert(lookat(0, fromVec)) * lookat(0, toVec);
}




template<typename FLOAT_T>
static void
matchAxis(
    GA_Detail* const geo,
    const GA_PointGroup* const pointGroup,
    GA_Attribute* const attribPtr,
    const UT_Vector3T<FLOAT_T>& fromVec,
    const UT_Vector3T<FLOAT_T>& toVec
    //, const UT_Vector3T<FLOAT_T>& upVec
    ) {
    UT_ASSERT_P(geo);
    UT_ASSERT_P(attribPtr);
    //UT_Vector3T<FLOAT_T> upVec(0, 1, 0);
    //const UT_Matrix3T<FLOAT_T>& xform = lookat_matrix(fromVec, toVec, upVec);
    const UT_Matrix3T<FLOAT_T>& xform = matchAxis(fromVec, toVec);
    GFE_XformByAttrib::xformByAttrib(geo, pointGroup, xform, attribPtr);
}





template<typename FLOAT_T>
static void
matchAxis(
    GA_Detail* const geo,
    const GA_PointGroup* const pointGroup,
    const GA_AttributeOwner owner,
    const UT_StringHolder& attribName,
    const UT_Vector3T<FLOAT_T>& fromVec,
    const UT_Vector3T<FLOAT_T>& toVec
    //, const UT_Vector3T<FLOAT_T>& upVec
) {
    if (pointGroup && pointGroup->isEmpty())
        return;
    GA_Attribute* const attribPtr = geo->findAttribute(owner, attribName);
    //matchAxis(geo, pointGroup, attribPtr, fromVec, toVec, upVec);
    matchAxis(geo, pointGroup, attribPtr, fromVec, toVec);
}









template<typename FLOAT_T>
static void
matchAxis(
    const SOP_NodeVerb::CookParms& cookparms,
    GA_Detail* const geo,
    const UT_StringHolder& pointGroupName,
    const GA_AttributeOwner owner,
    const UT_StringHolder& attribName,
    const UT_Vector3T<FLOAT_T>& fromVec,
    const UT_Vector3T<FLOAT_T>& toVec
    //, const UT_Vector3T<FLOAT_T>& upVec
) {
    GOP_Manager gop;
    const GA_PointGroup* const pointGroup = GFE_Group::findOrParsePointGroupDetached(cookparms, geo, pointGroupName, gop);
    //matchAxis(geo, pointGroup, owner, attribName, fromVec, toVec, upVec);
    matchAxis(geo, pointGroup, owner, attribName, fromVec, toVec);

    GFE_Attribute::bumpDataId(geo, owner, attribName);
}




} // End of namespace GFE_MatchAxis

#endif
