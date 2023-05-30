
#pragma once

#ifndef __GFE_UVGridify_h__
#define __GFE_UVGridify_h__

#include "GFE/GFE_UVGridify.h"


#include "GFE/GFE_GeoFilter.h"


enum class GFE_UVGridify_RowColMethod
{
    Uniform,
    Rows,
    Columns,
};




class GFE_UVGridify : public GFE_AttribFilter {

public:

    using GFE_AttribFilter::GFE_AttribFilter;

    void
        setComputeParm(
            const GFE_UVGridify_RowColMethod rowsOrColsNumMethod = GFE_UVGridify_RowColMethod::Uniform,
            const GA_Size rowsOrColsNum = 2,
            const bool reverseUVu = false,
            const bool reverseUVv = false,
            const bool uniScale = false,
            const exint subscribeRatio = 64,
            const exint minGrainSize = 64
        )
    {
        setHasComputed();
        this->rowsOrColsNumMethod = rowsOrColsNumMethod;
        this->rowsOrColsNum = rowsOrColsNum;
        this->reverseUVu = reverseUVu;
        this->reverseUVv = reverseUVv;
        this->uniScale = uniScale;
        this->subscribeRatio = subscribeRatio;
        this->minGrainSize = minGrainSize;
    }


private:



    virtual bool
        computeCore() override
    {
        if (getOutAttribArray().isEmpty())
            return false;
        
        if (groupParser.isEmpty())
            return true;

        const size_t size = getOutAttribArray().size();
        for (size_t i = 0; i < size; i++)
        {
            uvAttrib = getOutAttribArray()[i];
            const GA_Storage storage = uvAttrib->getAIFTuple()->getStorage(uvAttrib);
            switch (uvAttrib->getTupleSize())
            {
            case 2:
                switch (storage)
                {
                case GA_STORE_REAL16: uvGridify<UT_Vector2T<fpreal16>>(); break;
                case GA_STORE_REAL32: uvGridify<UT_Vector2T<fpreal32>>(); break;
                case GA_STORE_REAL64: uvGridify<UT_Vector2T<fpreal64>>(); break;
                default: break;
                }
                break;
            case 3:
                switch (storage)
                {
                case GA_STORE_REAL16: uvGridify<UT_Vector3T<fpreal16>>(); break;
                case GA_STORE_REAL32: uvGridify<UT_Vector3T<fpreal32>>(); break;
                case GA_STORE_REAL64: uvGridify<UT_Vector3T<fpreal64>>(); break;
                default: break;
                }
                break;
            case 4:
                switch (storage)
                {
                case GA_STORE_REAL16: uvGridify<UT_Vector4T<fpreal16>>(); break;
                case GA_STORE_REAL32: uvGridify<UT_Vector4T<fpreal32>>(); break;
                case GA_STORE_REAL64: uvGridify<UT_Vector4T<fpreal64>>(); break;
                default: break;
                }
                break;
            default: break;
            }
        }
        return true;
    }


    template<typename VECTOR_T>
    void uvGridify()
    {
        using value_type = typename VECTOR_T::value_type;
        using POS_VECTOR_T = UT_Vector3T<value_type>;

        
        const GA_RWHandleT<VECTOR_T> uv_h(uvAttrib);
        const GA_ROHandleT<POS_VECTOR_T> pos_h(geo->getP());
        const bool isPointAttrib = uvAttrib->getOwner() == GA_ATTRIB_POINT;
        
        UTparallelFor(groupParser.getPrimitiveSplittableRange(), [this, &uv_h, &pos_h, isPointAttrib](const GA_SplittableRange& r)
        {
            value_type scale;
            bool scaleIdx = true;
            
            VECTOR_T uv(getZeroVector<VECTOR_T>());
            GA_Offset start, end;
            for (GA_Iterator it(r); it.blockAdvance(start, end); )
            {
                for (GA_Offset primoff = start; primoff < end; ++primoff)
                {
                    const GA_Size numvtx = geo->getPrimitiveVertexCount(primoff);
                    if (numvtx == 0)
                        continue;
                    GA_Size rows, cols;
                    switch (rowsOrColsNumMethod)
                    {
                    case GFE_UVGridify_RowColMethod::Uniform:
                        rows = (GA_Size)ceil(numvtx / 4.0);
                        cols = (numvtx - rows - rows) / 2;
                        break;
                    case GFE_UVGridify_RowColMethod::Rows:
                        rows = rowsOrColsNum - 1;
                        cols = (numvtx - rows - rows) / 2;
                        break;
                    case GFE_UVGridify_RowColMethod::Columns:
                        cols = rowsOrColsNum - 1;
                        rows = (numvtx - cols - cols) / 2;
                        break;
                    default:
                        UT_ASSERT_MSG(0, "Unhandled rowsOrColsNumMethod");
                        break;
                    }
                    rows = SYSmax(rows, 0);
                    cols = SYSmax(cols, 0);


                    if (!uniScale)
                    {
                        if (numvtx > rows + rows + cols && numvtx > 2)
                        {
                            const POS_VECTOR_T& pos0 = pos_h.get(geo->vertexPoint(geo->getPrimitiveVertexOffset(primoff, 0)));
                            const POS_VECTOR_T& pos1 = pos_h.get(geo->vertexPoint(geo->getPrimitiveVertexOffset(primoff, rows)));
                            const POS_VECTOR_T& pos2 = pos_h.get(geo->vertexPoint(geo->getPrimitiveVertexOffset(primoff, rows + cols)));
                            //GA_Offset a = geo->getPrimitiveVertexOffset(primoff, rows + cols + rows);
                            //GA_Offset b = geo->vertexPoint(a);
                            //const POS_VECTOR_T& pos3 = pos_h.get(b);
                            const POS_VECTOR_T& pos3 = pos_h.get(geo->vertexPoint(geo->getPrimitiveVertexOffset(primoff, rows + rows + cols)));
                            const value_type dist0 = pos0.distance(pos1);
                            const value_type dist1 = pos1.distance(pos2);
                            const value_type dist2 = pos2.distance(pos3);
                            const value_type dist3 = pos3.distance(pos0);

                            scale = (dist0 + dist2) / (dist1 + dist3);

                            const value_type tmpScale = dist0 + dist2;
                            scale = dist1 + dist3;

                            scaleIdx = tmpScale > scale;
                            if (scaleIdx)
                            {
                                scale /= tmpScale;
                            }
                            else
                            {
                                scale = tmpScale / scale;
                            }
                        }
                        else
                        {
                            scale = 1.0;
                        }
                    }

                    GA_Size vtxpnum = 0;
                    for (; vtxpnum < rows; vtxpnum++)
                    {
                        GA_Size tmpI = rows - (cols <= 0);
                        uv[0] = tmpI==0 ? 0 : fpreal(vtxpnum) / tmpI;
                        uv[1] = 1;

                        uvGridify(uv_h, pos_h, uv, primoff, vtxpnum, rows, cols, scale, scaleIdx, isPointAttrib);
                    }
                    for (; vtxpnum < rows + cols; vtxpnum++)
                    {
                        GA_Size tmpI = cols - (rows == 0);
                        uv[0] = 1;
                        uv[1] = tmpI==0 ? 1 : 1 - (float(vtxpnum - rows) / tmpI);

                        uvGridify(uv_h, pos_h, uv, primoff, vtxpnum, rows, cols, scale, scaleIdx, isPointAttrib);
                    }
                    const GA_Size numvtx_preCols = numvtx - cols;
                    for (; vtxpnum < numvtx_preCols; vtxpnum++)
                    {
                        GA_Size tmpI = vtxpnum - rows - cols;
                        uv[0] = tmpI == 0 ? 1 : (1 - float(tmpI) / (numvtx - rows - cols - cols - 1));
                        uv[1] = 0;

                        uvGridify(uv_h, pos_h, uv, primoff, vtxpnum, rows, cols, scale, scaleIdx, isPointAttrib);
                    }
                    for (; vtxpnum < numvtx; vtxpnum++)
                    {
                        GA_Size tmpI = rows==0 ? numvtx - cols - 1 : cols;
                        uv[0] = 0;
                        uv[1] = tmpI==0 ? 1 : (1 - float(numvtx - vtxpnum - (rows == 0)) / tmpI);

                        uvGridify(uv_h, pos_h, uv, primoff, vtxpnum, rows, cols, scale, scaleIdx, isPointAttrib);
                    }
                }
            }
        }, subscribeRatio, minGrainSize);
    }




    template<typename VECTOR_T, typename POS_VECTOR_T>
    void uvGridify(
        const GA_RWHandleT<VECTOR_T>& uv_h,
        const GA_ROHandleT<POS_VECTOR_T>& pos_h,
        VECTOR_T& uv,
        const GA_Offset primoff,
        const GA_Size vtxpnum,
        const exint rows,
        const exint cols,
        const typename POS_VECTOR_T::value_type scale,
        const bool scaleIdx,
        const bool isPointAttrib
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
            if (scaleIdx)
            {
                uv[1] *= scale;
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
    SYS_FORCE_INLINE VECTOR_T getZeroVector() const
    {
        if constexpr(VECTOR_T::tuple_size == 4)
            return VECTOR_T(0, 0, 0, 0);
        else
            return VECTOR_T(0.0);
    }


    
public:
    GFE_UVGridify_RowColMethod rowsOrColsNumMethod = GFE_UVGridify_RowColMethod::Uniform;
    GA_Size rowsOrColsNum = 2;
    bool reverseUVu = false;
    bool reverseUVv = false;
    bool uniScale = false;

private:
    GA_Attribute* uvAttrib = nullptr;
    
    exint subscribeRatio = 64;
    exint minGrainSize = 64;
    
}; // End of Class GFE_UVGridify
































//
//namespace GFE_UVGridify_Namespace {
//
//
//template<typename VECTOR_T, typename POS_VECTOR_T>
//SYS_FORCE_INLINE
//static void
//uvGridify(
//    const GA_Detail* const geo,
//    const GA_RWHandleT<VECTOR_T>& uv_h,
//    const GA_ROHandleT<POS_VECTOR_T>& pos_h,
//    VECTOR_T& uv,
//    const GA_Offset primoff,
//    const GA_Size vtxpnum,
//    const exint rows,
//    const exint cols,
//    const bool isPointAttrib = false,
//    const bool reverseUVu = false,
//    const bool reverseUVv = false,
//    const bool uniScale = false
//)
//{
//    if (reverseUVu)
//    {
//        uv[0] = 1 - uv[0];
//    }
//    if (reverseUVv)
//    {
//        uv[1] = 1 - uv[1];
//    }
//    if (!uniScale)
//    {
//        const POS_VECTOR_T& pos0 = pos_h.get(geo->vertexPoint(geo->getPrimitiveVertexOffset(primoff, 0)));
//        const POS_VECTOR_T& pos1 = pos_h.get(geo->vertexPoint(geo->getPrimitiveVertexOffset(primoff, rows)));
//        const POS_VECTOR_T& pos2 = pos_h.get(geo->vertexPoint(geo->getPrimitiveVertexOffset(primoff, rows + cols)));
//        const POS_VECTOR_T& pos3 = pos_h.get(geo->vertexPoint(geo->getPrimitiveVertexOffset(primoff, rows + cols + rows)));
//        const typename POS_VECTOR_T::value_type dist0 = pos0.distance(pos1);
//        const typename POS_VECTOR_T::value_type dist1 = pos1.distance(pos2);
//        const typename POS_VECTOR_T::value_type dist2 = pos2.distance(pos3);
//        const typename POS_VECTOR_T::value_type dist3 = pos3.distance(pos0);
//
//        const typename POS_VECTOR_T::value_type scale = (dist0 + dist2) / (dist1 + dist3);
//        if (scale > 1.0)
//        {
//            uv[1] *= 1.0 / scale;
//        }
//        else
//        {
//            uv[0] *= scale;
//        }
//    }
//    GA_Offset elemoff = geo->getPrimitiveVertexOffset(primoff, vtxpnum);
//    if (isPointAttrib)
//        elemoff = geo->vertexPoint(elemoff);
//    uv_h.set(elemoff, uv);
//}
//
//template<typename VECTOR_T>
//static void
//    uvGridify(
//        const GA_Detail* const geo,
//        const GA_RWHandleT<VECTOR_T>& uv_h,
//        const GA_PrimitiveGroup* const geoPrimGroup = nullptr,
//        const GFE_UVGridify_RowColMethod rowsOrColsNumMethod = GFE_UVGridify_RowColMethod::Uniform,
//        const GA_Size rowsOrColsNum = 2,
//        const bool reverseUVu = false,
//        const bool reverseUVv = false,
//        const bool uniScale = false,
//        const exint subscribeRatio = 64,
//        const exint minGrainSize = 64
//    )
//{
//    const GA_ROHandleT<UT_Vector3T<fpreal64>> pos_h(geo->getP());
//    const bool isPointAttrib = uv_h.getAttribute()->getOwner() == GA_ATTRIB_POINT;
//
//    const GA_SplittableRange geoSplittableRange(geo->getPrimitiveRange(geoPrimGroup));
//    UTparallelFor(geoSplittableRange, [geo, &uv_h, &pos_h, isPointAttrib, rowsOrColsNumMethod, rowsOrColsNum, reverseUVu, reverseUVv, uniScale](const GA_SplittableRange& r)
//    {
//        //const typename VECTOR_T::value_type zero = 0;
//        VECTOR_T uv(typename VECTOR_T::value_type(0));
//        GA_Offset start, end;
//        for (GA_Iterator it(r); it.blockAdvance(start, end); )
//        {
//            for (GA_Offset primoff = start; primoff < end; ++primoff)
//            {
//                GA_Size numvtx = geo->getPrimitiveVertexCount(primoff);
//
//
//
//                GA_Size rows, cols;
//                switch (rowsOrColsNumMethod)
//                {
//                case GFE_UVGridify_RowColMethod::Uniform:
//                    rows = (GA_Size)ceil(numvtx / 4.0);
//                    cols = (numvtx - rows - rows) / 2;
//                    break;
//                case GFE_UVGridify_RowColMethod::Rows:
//                    rows = rowsOrColsNum - 1;
//                    cols = (numvtx - rows - rows) / 2;
//                    break;
//                case GFE_UVGridify_RowColMethod::Columns:
//                    cols = rowsOrColsNum - 1;
//                    rows = (numvtx - cols - cols) / 2;
//                    break;
//                default:
//                    UT_ASSERT_MSG(0, "Unhandled rowsOrColsNumMethod");
//                    break;
//                }
//                rows = SYSmax(rows, 0);
//                cols = SYSmax(cols, 0);
//
//
//
//
//
//
//                GA_Size vtxpnum = 0;
//                for (;vtxpnum < rows; vtxpnum++)
//                {
//                    uv[0] = fpreal(vtxpnum) / (rows - (cols <= 0));
//                    uv[1] = 1;
//
//                    uvGridify(geo, uv_h, pos_h, uv, primoff, vtxpnum, rows, cols, isPointAttrib, reverseUVu, reverseUVv, uniScale);
//                }
//                for (; vtxpnum < rows + cols; vtxpnum++)
//                {
//                    uv[0] = 1;
//                    uv[1] = 1 - (float(vtxpnum - rows) / (cols - (rows == 0)));
//
//                    uvGridify(geo, uv_h, pos_h, uv, primoff, vtxpnum, rows, cols, isPointAttrib, reverseUVu, reverseUVv, uniScale);
//                }
//                const GA_Size numvtx_preCols = numvtx - cols;
//                for (; vtxpnum < numvtx_preCols; vtxpnum++)
//                {
//                    uv[0] = (vtxpnum - rows - cols == 0) ? 1 : (1 - float(vtxpnum - rows - cols) / (numvtx - rows - cols - cols - 1));
//                    uv[1] = 0;
//
//                    uvGridify(geo, uv_h, pos_h, uv, primoff, vtxpnum, rows, cols, isPointAttrib, reverseUVu, reverseUVv, uniScale);
//                }
//                for (; vtxpnum < numvtx; vtxpnum++)
//                {
//                    uv[0] = 0;
//                    uv[1] = ((rows == 0 ? numvtx - cols - 1 : cols)==0) ? 1 : (1 - float(numvtx - vtxpnum - (rows == 0)) / (rows == 0 ? numvtx - cols - 1 : cols));
//
//                    uvGridify(geo, uv_h, pos_h, uv, primoff, vtxpnum, rows, cols, isPointAttrib, reverseUVu, reverseUVv, uniScale);
//                }
//            }
//        }
//    }, subscribeRatio, minGrainSize);
//
//}
//
//
//static bool
//    uvGridify(
//        const GA_Detail* const geo,
//        GA_Attribute* const uvAttrib,
//        const GA_PrimitiveGroup* const geoPrimGroup,
//        const GFE_UVGridify_RowColMethod rowsOrColsNumMethod = GFE_UVGridify_RowColMethod::Uniform,
//        const GA_Size rowsOrColsNum = 2,
//        const bool reverseUVu = false,
//        const bool reverseUVv = false,
//        const bool uniScale = false,
//        const exint subscribeRatio = 64,
//        const exint minGrainSize = 64
//    )
//{
//    switch (uvAttrib->getTupleSize())
//    {
//    case 2:
//        switch (uvAttrib->getAIFTuple()->getStorage(uvAttrib))
//        {
//        case GA_STORE_REAL16:
//            uvGridify(geo, GA_RWHandleT<UT_Vector2T<fpreal16>>(uvAttrib), geoPrimGroup,
//                rowsOrColsNumMethod, rowsOrColsNum,
//                reverseUVu, reverseUVv, uniScale,
//                subscribeRatio, minGrainSize);
//            break;
//        case GA_STORE_REAL32:
//            uvGridify(geo, GA_RWHandleT<UT_Vector2T<fpreal32>>(uvAttrib), geoPrimGroup,
//                rowsOrColsNumMethod, rowsOrColsNum,
//                reverseUVu, reverseUVv, uniScale,
//                subscribeRatio, minGrainSize);
//            break;
//        case GA_STORE_REAL64:
//            uvGridify(geo, GA_RWHandleT<UT_Vector2T<fpreal64>>(uvAttrib), geoPrimGroup,
//                rowsOrColsNumMethod, rowsOrColsNum,
//                reverseUVu, reverseUVv, uniScale,
//                subscribeRatio, minGrainSize);
//            break;
//        default:
//            return false;
//            break;
//        }
//        break;
//    case 3:
//        switch (uvAttrib->getAIFTuple()->getStorage(uvAttrib))
//        {
//        case GA_STORE_REAL16:
//            uvGridify(geo, GA_RWHandleT<UT_Vector3T<fpreal16>>(uvAttrib), geoPrimGroup,
//                rowsOrColsNumMethod, rowsOrColsNum,
//                reverseUVu, reverseUVv, uniScale,
//                subscribeRatio, minGrainSize);
//            break;
//        case GA_STORE_REAL32:
//            uvGridify(geo, GA_RWHandleT<UT_Vector3T<fpreal32>>(uvAttrib), geoPrimGroup,
//                rowsOrColsNumMethod, rowsOrColsNum,
//                reverseUVu, reverseUVv, uniScale,
//                subscribeRatio, minGrainSize);
//            break;
//        case GA_STORE_REAL64:
//            uvGridify(geo, GA_RWHandleT<UT_Vector3T<fpreal64>>(uvAttrib), geoPrimGroup,
//                rowsOrColsNumMethod, rowsOrColsNum,
//                reverseUVu, reverseUVv, uniScale,
//                subscribeRatio, minGrainSize);
//            break;
//        default:
//            return false;
//            break;
//        }
//        break;
//    case 4:
//        return false;
//        //switch (uvAttrib->getAIFTuple()->getStorage(uvAttrib))
//        //{
//        //case GA_STORE_REAL16:
//        //    uvGridify(geo, GA_RWHandleT<UT_Vector4T<fpreal16>>(uvAttrib), geoPrimGroup,
//        //        rowsOrColsNumMethod, rowsOrColsNum,
//        //        reverseUVu, reverseUVv, uniScale,
//        //        subscribeRatio, minGrainSize);
//        //    break;
//        //case GA_STORE_REAL32:
//        //    uvGridify(geo, GA_RWHandleT<UT_Vector4T<fpreal32>>(uvAttrib), geoPrimGroup,
//        //        rowsOrColsNumMethod, rowsOrColsNum,
//        //        reverseUVu, reverseUVv, uniScale,
//        //        subscribeRatio, minGrainSize);
//        //    break;
//        //case GA_STORE_REAL64:
//        //    uvGridify(geo, GA_RWHandleT<UT_Vector4T<fpreal64>>(uvAttrib), geoPrimGroup,
//        //        rowsOrColsNumMethod, rowsOrColsNum,
//        //        reverseUVu, reverseUVv, uniScale,
//        //        subscribeRatio, minGrainSize);
//        //    break;
//        //default:
//        //    return false;
//        //    break;
//        //}
//        //break;
//    default:
//        return false;
//        break;
//    }
//    return true;
//}
//
//
//
//static void
//uvGridify(
//    const GA_Detail* const geo,
//    GA_Attribute* const uvAttrib,
//    const GA_Group* const geoGroup,
//    const GFE_UVGridify_RowColMethod rowsOrColsNumMethod = GFE_UVGridify_RowColMethod::Uniform,
//    const GA_Size rowsOrColsNum = 2,
//    const bool reverseUVu = false,
//    const bool reverseUVv = false,
//    const bool uniScale = false,
//    const exint subscribeRatio = 64,
//    const exint minGrainSize = 64
//)
//{
//    GA_PrimitiveGroupUPtr geoPrimGroupUPtr = GFE_GroupPromote::groupPromotePrimitiveDetached(geo, geoGroup);
//    uvGridify(geo, uvAttrib, geoPrimGroupUPtr.get(),
//        rowsOrColsNumMethod, rowsOrColsNum,
//        reverseUVu, reverseUVv, uniScale,
//        subscribeRatio, minGrainSize);
//}
//
//SYS_FORCE_INLINE
//static GA_Attribute*
//uvGridify(
//    GA_Detail* const geo,
//    const GA_AttributeOwner uvAttribClass = GA_ATTRIB_INVALID,
//    const UT_StringHolder& uvAttribName = "uv",
//    const GA_Group* const geoGroup = nullptr,
//    const GFE_UVGridify_RowColMethod rowsOrColsNumMethod = GFE_UVGridify_RowColMethod::Uniform,
//    const GA_Size rowsOrColsNum = 2,
//    const bool reverseUVu = false,
//    const bool reverseUVv = false,
//    const bool uniScale = false,
//    const exint subscribeRatio = 64,
//    const exint minGrainSize = 64
//)
//{
//    GA_Attribute* uvAttrib = GFE_Attribute::findOrCreateUVAttributePointVertex(geo, uvAttribClass, uvAttribName);
//
//    uvGridify(geo, uvAttrib, geoGroup,
//        rowsOrColsNumMethod, rowsOrColsNum,
//        reverseUVu, reverseUVv, uniScale,
//        subscribeRatio, minGrainSize);
//
//    return uvAttrib;
//}
//
//SYS_FORCE_INLINE
//static GA_Attribute*
//uvGridify(
//    const SOP_NodeVerb::CookParms& cookparms,
//    GA_Detail* const geo,
//    const GA_GroupType groupType = GA_GROUP_INVALID,
//    const UT_StringHolder& groupName = "",
//    const GA_AttributeOwner uvAttribClass = GA_ATTRIB_INVALID,
//    const UT_StringHolder& uvAttribName = "uv",
//    const GFE_UVGridify_RowColMethod rowsOrColsNumMethod = GFE_UVGridify_RowColMethod::Uniform,
//    const GA_Size rowsOrColsNum = 2,
//    const bool reverseUVu = false,
//    const bool reverseUVv = false,
//    const bool uniScale = false,
//    const exint subscribeRatio = 64,
//    const exint minGrainSize = 64
//)
//{
//    GOP_Manager gop;
//    const GA_Group* const geoGroup = GFE_GroupParser_Namespace::findOrParseGroupDetached(cookparms, geo, groupType, groupName, gop);
//
//    GA_Attribute* const uvAttrib = uvGridify(geo,
//        uvAttribClass, uvAttribName, geoGroup,
//        rowsOrColsNumMethod, rowsOrColsNum,
//        reverseUVu, reverseUVv, uniScale,
//        subscribeRatio, minGrainSize);
//
//    if(uvAttrib)
//        uvAttrib->bumpDataId();
//
//    return uvAttrib;
//}
//
//
//
//} // End of namespace GFE_UVGridify

#endif
