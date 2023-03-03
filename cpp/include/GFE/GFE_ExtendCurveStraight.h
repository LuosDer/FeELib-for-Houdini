
#pragma once

#ifndef __GFE_ExtendCurveStraight_h__
#define __GFE_ExtendCurveStraight_h__



//#include "GFE/GFE_ExtendCurveStraight.h"

#include "GA/GA_Detail.h"
//#include "GA/GA_Types.h"

#include "GFE/GFE_GroupParse.h"





namespace GFE_ExtendCurveStraight {



template<typename FLOAT_T, bool TryFindNextPtIfOverlaping>
SYS_FORCE_INLINE
static void
extendCurveStraight(
    const GA_Detail* const geo,
    const GA_RWHandleT<UT_Vector3T<FLOAT_T>>& pos_h,
    const GA_PointGroup* const pointGroup,
    const GA_Offset primnum,
    const GA_Size vtxpnum,
    const GA_Size intArg,
    const fpreal extendCurveStraightDist
)
{
    const GA_Offset primpoint0 = geo->vertexPoint(geo->getPrimitiveVertexOffset(primnum, vtxpnum));

    if (!pointGroup->containsOffset(primpoint0))
        return;

    const UT_Vector3T<FLOAT_T>& pos = pos_h.get(primpoint0);
    UT_Vector3T<FLOAT_T> nebpos(FLOAT_T(0));

    if (TryFindNextPtIfOverlaping)
    {
        GA_Size vtxpnum_neb = vtxpnum;
        do {
            vtxpnum_neb += intArg;
            const GA_Offset primpoint1 = geo->vertexPoint(geo->getPrimitiveVertexOffset(primnum, vtxpnum_neb));
            nebpos = pos_h.get(primpoint1);
        } while (nebpos == pos);
    }
    else
    {
        const GA_Offset primpoint1 = geo->vertexPoint(geo->getPrimitiveVertexOffset(primnum, intArg));
        nebpos = pos_h.get(primpoint0);
    }

    UT_Vector3T<FLOAT_T> dir(pos - nebpos);
    dir.normalize();
    dir = pos + dir * extendCurveStraightDist;
    pos_h.set(primpoint0, dir);
}

template<typename FLOAT_T>
static void
    extendCurveStraight(
        const GA_Detail* const geo,
        const GA_RWHandleT<UT_Vector3T<FLOAT_T>>& pos_h,
        const fpreal extendCurveStraightDist,
        const GA_PrimitiveGroup* const primGroup = nullptr,
        const GA_PointGroup* const pointGroup = nullptr,
        const bool extendStartPoint = true,
        const bool extendEndPoint = true,
        const bool tryFindNextOverlapPoint = false,
        const exint subscribeRatio = 64,
        const exint minGrainSize = 64
    )
{
    if (extendCurveStraightDist == 0)
        return;

    if (!extendStartPoint && !extendEndPoint)
        return;


    const GA_SplittableRange geoSplittableRange0(geo->getPrimitiveRange(primGroup));
    UTparallelFor(geoSplittableRange0, [geo, &pos_h, pointGroup, extendStartPoint, extendEndPoint, tryFindNextOverlapPoint, extendCurveStraightDist](const GA_SplittableRange& r)
    {
        GA_Offset start, end;
        for (GA_Iterator it(r); it.blockAdvance(start, end); )
        {
            for (GA_Offset primoff = start; primoff < end; ++primoff)
            {
                const GA_Size numvtx = geo->getPrimitiveVertexCount(primoff);
                if (numvtx < 2) return;

                if (extendStartPoint) {
                    if (tryFindNextOverlapPoint)
                    {
                        extendCurveStraight<FLOAT_T, true>(geo, pos_h, pointGroup, primoff, 0, 1, extendCurveStraightDist);
                    }
                    else
                    {
                        extendCurveStraight<FLOAT_T, false>(geo, pos_h, pointGroup, primoff, 0, 1, extendCurveStraightDist);
                    }
                }

                if (extendEndPoint) {
                    if (tryFindNextOverlapPoint)
                    {
                        extendCurveStraight<FLOAT_T, true>(geo, pos_h, pointGroup, primoff, numvtx - 1, -1, extendCurveStraightDist);
                    }
                    else
                    {
                        extendCurveStraight<FLOAT_T, false>(geo, pos_h, pointGroup, primoff, numvtx - 1, -1, extendCurveStraightDist);
                    }
                }

            }
        }
    }, subscribeRatio, minGrainSize);

}

static GA_Attribute*
    extendCurveStraight(
        GA_Detail* const geo,
        const fpreal extendCurveStraightDist,
        const UT_StringHolder& posAttribName = "P",
        const GA_PrimitiveGroup* const primGroup = nullptr,
        const GA_PointGroup* const pointGroup = nullptr,
        const bool extendStartPoint = true,
        const bool extendEndPoint = true,
        const bool tryFindNextOverlapPoint = false,
        const exint subscribeRatio = 64,
        const exint minGrainSize = 64
    )
{
    GA_Attribute* posAttribPtr = geo->findPointAttribute(posAttribName);
    if(!posAttribPtr)
        posAttribPtr = geo->getP();

    UT_ASSERT_MSG(posAttribPtr, "not possible posAttrib");

    switch (posAttribPtr->getAIFTuple()->getStorage(posAttribPtr))
    {
    case GA_STORE_REAL16:
        //extendCurveStraight<fpreal16>(geo, posAttribPtr, extendCurveStraightDist, primGroup, pointGroup,
        //    extendStartPoint, extendEndPoint, tryFindNextOverlapPoint,
        //    subscribeRatio, minGrainSize);
        break;
    case GA_STORE_REAL32:
        extendCurveStraight<fpreal32>(geo, posAttribPtr, extendCurveStraightDist, primGroup, pointGroup,
            extendStartPoint, extendEndPoint, tryFindNextOverlapPoint,
            subscribeRatio, minGrainSize);
        break;
    case GA_STORE_REAL64:
        extendCurveStraight<fpreal64>(geo, posAttribPtr, extendCurveStraightDist, primGroup, pointGroup,
            extendStartPoint, extendEndPoint, tryFindNextOverlapPoint,
            subscribeRatio, minGrainSize);
        break;
    default:
        UT_ASSERT_MSG(0, "not possible storage");
        break;
    }

    return posAttribPtr;
}

static void
extendCurveStraight(
    const SOP_NodeVerb::CookParms& cookparms,
    GA_Detail* const geo,
    const fpreal extendCurveStraightDist,
    const UT_StringHolder& primGroupName,
    const UT_StringHolder& pointGroupName,
    const bool extendStartPoint = true,
    const bool extendEndPoint = true,
    const bool tryFindNextOverlapPoint = false,
    const UT_StringHolder& posAttribName = "P",
    const exint subscribeRatio = 64,
    const exint minGrainSize = 64
)
{
    if (extendCurveStraightDist == 0)
        return;

    GOP_Manager gop;
    const GA_PrimitiveGroup* const primGroup = GFE_GroupParse_Namespace::findOrParsePrimitiveGroupDetached(cookparms, geo, primGroupName, gop);
    if (primGroup && primGroup->isEmpty())
    {
        return;
    }

    const GA_PointGroup* const pointGroup = GFE_GroupParse_Namespace::findOrParsePointGroupDetached(cookparms, geo, pointGroupName, gop);
    if (pointGroup && pointGroup->isEmpty())
    {
        return;
    }

    GA_Attribute* posAttribPtr = extendCurveStraight(geo,
        extendCurveStraightDist, posAttribName, primGroup, pointGroup,
        extendStartPoint, extendEndPoint, tryFindNextOverlapPoint,
        subscribeRatio, minGrainSize);

    posAttribPtr->bumpDataId();
}






} // End of namespace GFE_ExtendCurveStraight

#endif
