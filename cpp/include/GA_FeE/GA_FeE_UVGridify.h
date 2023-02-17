
#pragma once

#ifndef __GA_FeE_UVScaletoWorldSize_h__
#define __GA_FeE_UVScaletoWorldSize_h__

//#include "GA_FeE/GA_FeE_UVGridify.h"

#include "GA/GA_Detail.h"


#include "GA_FeE/GA_FeE_Type.h"
#include "GA_FeE/GA_FeE_Attribute.h"
#include "GA_FeE/GA_FeE_Group.h"
#include "GA_FeE/GA_FeE_GroupPromote.h"


enum UVGridify_RowsOrColsNumMethod
{
    UVGridifyMethod_Uniform,
    UVGridifyMethod_Rows,
    UVGridifyMethod_Columns,
};

namespace GA_FeE_UVGridify {


template<typename VECTOR_T, typename POS_VECTOR_T>
SYS_FORCE_INLINE
static void
uvGridify(
    const GA_Detail* const geo,
    const GA_RWHandleT<VECTOR_T>& uv_h,
    const GA_ROHandleT<POS_VECTOR_T>& pos_h,
    VECTOR_T& uv,
    const GA_Offset primoff,
    const GA_Size vtxpnum,
    const exint rows,
    const exint cols,
    const bool isPointAttrib = false,
    const bool reverseUVu = false,
    const bool reverseUVv = false,
    const bool uniScale = false
)
{
    if (reverseUVu)
    {
        uv[0] = 1 - uv[0];
    }
    if (reverseUVv)
    {
        uv[1] = 1 - uv[1];
    }
    if (!uniScale)
    {
        const POS_VECTOR_T& pos0 = pos_h.get(geo->vertexPoint(geo->getPrimitiveVertexOffset(primoff, 0)));
        const POS_VECTOR_T& pos1 = pos_h.get(geo->vertexPoint(geo->getPrimitiveVertexOffset(primoff, rows)));
        const POS_VECTOR_T& pos2 = pos_h.get(geo->vertexPoint(geo->getPrimitiveVertexOffset(primoff, rows + cols)));
        const POS_VECTOR_T& pos3 = pos_h.get(geo->vertexPoint(geo->getPrimitiveVertexOffset(primoff, rows + cols + rows)));
        const typename POS_VECTOR_T::value_type dist0 = pos0.distance(pos1);
        const typename POS_VECTOR_T::value_type dist1 = pos1.distance(pos2);
        const typename POS_VECTOR_T::value_type dist2 = pos2.distance(pos3);
        const typename POS_VECTOR_T::value_type dist3 = pos3.distance(pos0);

        const typename POS_VECTOR_T::value_type scale = (dist0 + dist2) / (dist1 + dist3);
        if (scale > 1.0)
        {
            uv[1] *= 1.0 / scale;
        }
        else
        {
            uv[0] *= scale;
        }
    }
    GA_Offset elemoff = geo->getPrimitiveVertexOffset(primoff, vtxpnum);
    if (isPointAttrib)
        elemoff = geo->vertexPoint(elemoff);
    uv_h.set(elemoff, uv);
}

template<typename VECTOR_T>
static void
    uvGridify(
        const GA_Detail* const geo,
        const GA_RWHandleT<VECTOR_T>& uv_h,
        const GA_PrimitiveGroup* const geoPrimGroup = nullptr,
        const UVGridify_RowsOrColsNumMethod rowsOrColsNumMethod = UVGridifyMethod_Uniform,
        const GA_Size rowsOrColsNum = 2,
        const bool reverseUVu = false,
        const bool reverseUVv = false,
        const bool uniScale = false,
        const exint subscribeRatio = 64,
        const exint minGrainSize = 64
    )
{
    const GA_ROHandleT<UT_Vector3T<fpreal64>> pos_h(geo->getP());
    const bool isPointAttrib = uv_h.getAttribute()->getOwner() == GA_ATTRIB_POINT;

    const GA_SplittableRange geoSplittableRange(geo->getPrimitiveRange(geoPrimGroup));
    UTparallelFor(geoSplittableRange, [geo, &uv_h, &pos_h, isPointAttrib, rowsOrColsNumMethod, rowsOrColsNum, reverseUVu, reverseUVv, uniScale](const GA_SplittableRange& r)
    {
        //const typename VECTOR_T::value_type zero = 0;
        VECTOR_T uv(typename VECTOR_T::value_type(0));
        GA_Offset start, end;
        for (GA_Iterator it(r); it.blockAdvance(start, end); )
        {
            for (GA_Offset primoff = start; primoff < end; ++primoff)
            {
                GA_Size numvtx = geo->getPrimitiveVertexCount(primoff);



                GA_Size rows, cols;
                switch (rowsOrColsNumMethod)
                {
                case UVGridifyMethod_Uniform:
                    rows = (GA_Size)ceil(numvtx / 4.0);
                    cols = (numvtx - rows - rows) / 2;
                    break;
                case UVGridifyMethod_Rows:
                    rows = rowsOrColsNum - 1;
                    cols = (numvtx - rows - rows) / 2;
                    break;
                case UVGridifyMethod_Columns:
                    cols = rowsOrColsNum - 1;
                    rows = (numvtx - cols - cols) / 2;
                    break;
                default:
                    UT_ASSERT_MSG(0, "Unhandled rowsOrColsNumMethod");
                    break;
                }
                rows = SYSmax(rows, 0);
                cols = SYSmax(cols, 0);






                GA_Size vtxpnum = 0;
                for (;vtxpnum < rows; vtxpnum++)
                {
                    uv[0] = fpreal(vtxpnum) / (rows - (cols <= 0));
                    uv[1] = 1;

                    uvGridify(geo, uv_h, pos_h, uv, primoff, vtxpnum, rows, cols, isPointAttrib, reverseUVu, reverseUVv, uniScale);
                }
                for (; vtxpnum < rows + cols; vtxpnum++)
                {
                    uv[0] = 1;
                    uv[1] = 1 - (float(vtxpnum - rows) / (cols - (rows == 0)));

                    uvGridify(geo, uv_h, pos_h, uv, primoff, vtxpnum, rows, cols, isPointAttrib, reverseUVu, reverseUVv, uniScale);
                }
                const GA_Size numvtx_preCols = numvtx - cols;
                for (; vtxpnum < numvtx_preCols; vtxpnum++)
                {
                    uv[0] = (vtxpnum - rows - cols == 0) ? 1 : (1 - float(vtxpnum - rows - cols) / (numvtx - rows - cols - cols - 1));
                    uv[1] = 0;

                    uvGridify(geo, uv_h, pos_h, uv, primoff, vtxpnum, rows, cols, isPointAttrib, reverseUVu, reverseUVv, uniScale);
                }
                for (; vtxpnum < numvtx; vtxpnum++)
                {
                    uv[0] = 0;
                    uv[1] = ((rows == 0 ? numvtx - cols - 1 : cols)==0) ? 1 : (1 - float(numvtx - vtxpnum - (rows == 0)) / (rows == 0 ? numvtx - cols - 1 : cols));

                    uvGridify(geo, uv_h, pos_h, uv, primoff, vtxpnum, rows, cols, isPointAttrib, reverseUVu, reverseUVv, uniScale);
                }
            }
        }
    }, subscribeRatio, minGrainSize);

}


static bool
    uvGridify(
        const GA_Detail* const geo,
        GA_Attribute* const uvAttribPtr,
        const GA_PrimitiveGroup* const geoPrimGroup,
        const UVGridify_RowsOrColsNumMethod rowsOrColsNumMethod = UVGridifyMethod_Uniform,
        const GA_Size rowsOrColsNum = 2,
        const bool reverseUVu = false,
        const bool reverseUVv = false,
        const bool uniScale = false,
        const exint subscribeRatio = 64,
        const exint minGrainSize = 64
    )
{
    switch (uvAttribPtr->getTupleSize())
    {
    case 2:
        switch (uvAttribPtr->getAIFTuple()->getStorage(uvAttribPtr))
        {
        case GA_STORE_REAL16:
            uvGridify(geo, GA_RWHandleT<UT_Vector2T<fpreal16>>(uvAttribPtr), geoPrimGroup,
                rowsOrColsNumMethod, rowsOrColsNum,
                reverseUVu, reverseUVv, uniScale,
                subscribeRatio, minGrainSize);
            break;
        case GA_STORE_REAL32:
            uvGridify(geo, GA_RWHandleT<UT_Vector2T<fpreal32>>(uvAttribPtr), geoPrimGroup,
                rowsOrColsNumMethod, rowsOrColsNum,
                reverseUVu, reverseUVv, uniScale,
                subscribeRatio, minGrainSize);
            break;
        case GA_STORE_REAL64:
            uvGridify(geo, GA_RWHandleT<UT_Vector2T<fpreal64>>(uvAttribPtr), geoPrimGroup,
                rowsOrColsNumMethod, rowsOrColsNum,
                reverseUVu, reverseUVv, uniScale,
                subscribeRatio, minGrainSize);
            break;
        default:
            return false;
            break;
        }
        break;
    case 3:
        switch (uvAttribPtr->getAIFTuple()->getStorage(uvAttribPtr))
        {
        case GA_STORE_REAL16:
            uvGridify(geo, GA_RWHandleT<UT_Vector3T<fpreal16>>(uvAttribPtr), geoPrimGroup,
                rowsOrColsNumMethod, rowsOrColsNum,
                reverseUVu, reverseUVv, uniScale,
                subscribeRatio, minGrainSize);
            break;
        case GA_STORE_REAL32:
            uvGridify(geo, GA_RWHandleT<UT_Vector3T<fpreal32>>(uvAttribPtr), geoPrimGroup,
                rowsOrColsNumMethod, rowsOrColsNum,
                reverseUVu, reverseUVv, uniScale,
                subscribeRatio, minGrainSize);
            break;
        case GA_STORE_REAL64:
            uvGridify(geo, GA_RWHandleT<UT_Vector3T<fpreal64>>(uvAttribPtr), geoPrimGroup,
                rowsOrColsNumMethod, rowsOrColsNum,
                reverseUVu, reverseUVv, uniScale,
                subscribeRatio, minGrainSize);
            break;
        default:
            return false;
            break;
        }
        break;
    case 4:
        return false;
        //switch (uvAttribPtr->getAIFTuple()->getStorage(uvAttribPtr))
        //{
        //case GA_STORE_REAL16:
        //    uvGridify(geo, GA_RWHandleT<UT_Vector4T<fpreal16>>(uvAttribPtr), geoPrimGroup,
        //        rowsOrColsNumMethod, rowsOrColsNum,
        //        reverseUVu, reverseUVv, uniScale,
        //        subscribeRatio, minGrainSize);
        //    break;
        //case GA_STORE_REAL32:
        //    uvGridify(geo, GA_RWHandleT<UT_Vector4T<fpreal32>>(uvAttribPtr), geoPrimGroup,
        //        rowsOrColsNumMethod, rowsOrColsNum,
        //        reverseUVu, reverseUVv, uniScale,
        //        subscribeRatio, minGrainSize);
        //    break;
        //case GA_STORE_REAL64:
        //    uvGridify(geo, GA_RWHandleT<UT_Vector4T<fpreal64>>(uvAttribPtr), geoPrimGroup,
        //        rowsOrColsNumMethod, rowsOrColsNum,
        //        reverseUVu, reverseUVv, uniScale,
        //        subscribeRatio, minGrainSize);
        //    break;
        //default:
        //    return false;
        //    break;
        //}
        //break;
    default:
        return false;
        break;
    }
    return true;
}



static void
uvGridify(
    const GA_Detail* const geo,
    GA_Attribute* const uvAttribPtr,
    const GA_Group* const geoGroup,
    const UVGridify_RowsOrColsNumMethod rowsOrColsNumMethod = UVGridifyMethod_Uniform,
    const GA_Size rowsOrColsNum = 2,
    const bool reverseUVu = false,
    const bool reverseUVv = false,
    const bool uniScale = false,
    const exint subscribeRatio = 64,
    const exint minGrainSize = 64
)
{
    GA_PrimitiveGroupUPtr geoPrimGroupUPtr = GA_FeE_GroupPromote::groupPromotePrimitiveDetached(geo, geoGroup);
    uvGridify(geo, uvAttribPtr, geoPrimGroupUPtr.get(),
        rowsOrColsNumMethod, rowsOrColsNum,
        reverseUVu, reverseUVv, uniScale,
        subscribeRatio, minGrainSize);
}

SYS_FORCE_INLINE
static GA_Attribute*
uvGridify(
    GA_Detail* const geo,
    const GA_AttributeOwner uvAttribClass = GA_ATTRIB_INVALID,
    const UT_StringHolder& uvAttribName = "uv",
    const GA_Group* const geoGroup = nullptr,
    const UVGridify_RowsOrColsNumMethod rowsOrColsNumMethod = UVGridifyMethod_Uniform,
    const GA_Size rowsOrColsNum = 2,
    const bool reverseUVu = false,
    const bool reverseUVv = false,
    const bool uniScale = false,
    const exint subscribeRatio = 64,
    const exint minGrainSize = 64
)
{
    GA_Attribute* uvAttribPtr = GA_FeE_Attribute::findOrCreateUVAttributePointVertex(geo, uvAttribClass, uvAttribName);

    uvGridify(geo, uvAttribPtr, geoGroup,
        rowsOrColsNumMethod, rowsOrColsNum,
        reverseUVu, reverseUVv, uniScale,
        subscribeRatio, minGrainSize);

    return uvAttribPtr;
}

SYS_FORCE_INLINE
static GA_Attribute*
uvGridify(
    const SOP_NodeVerb::CookParms& cookparms,
    GA_Detail* const geo,
    const GA_GroupType groupType = GA_GROUP_INVALID,
    const UT_StringHolder& groupName = "",
    const GA_AttributeOwner uvAttribClass = GA_ATTRIB_INVALID,
    const UT_StringHolder& uvAttribName = "uv",
    const UVGridify_RowsOrColsNumMethod rowsOrColsNumMethod = UVGridifyMethod_Uniform,
    const GA_Size rowsOrColsNum = 2,
    const bool reverseUVu = false,
    const bool reverseUVv = false,
    const bool uniScale = false,
    const exint subscribeRatio = 64,
    const exint minGrainSize = 64
)
{
    GOP_Manager gop;
    const GA_Group* const geoGroup = GA_FeE_Group::findOrParseGroupDetached(cookparms, geo, groupType, groupName, gop);

    GA_Attribute* const uvAttribPtr = uvGridify(geo,
        uvAttribClass, uvAttribName, geoGroup,
        rowsOrColsNumMethod, rowsOrColsNum,
        reverseUVu, reverseUVv, uniScale,
        subscribeRatio, minGrainSize);

    if(uvAttribPtr)
        uvAttribPtr->bumpDataId();

    return uvAttribPtr;
}



} // End of namespace GA_FeE_UVGridify

#endif
