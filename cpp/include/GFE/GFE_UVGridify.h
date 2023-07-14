
#pragma once

#ifndef __GFE_UVGridify_h__
#define __GFE_UVGridify_h__

#include "GFE/GFE_UVGridify.h"

#include "GFE/GFE_GeoFilter.h"



class GFE_UVGridify : public GFE_AttribFilter {

public:

    enum RowColMethod
    {
        Uniform,
        Rows,
        Columns,
    };



    using GFE_AttribFilter::GFE_AttribFilter;

    void
        setComputeParm(
            const GFE_UVGridify::RowColMethod rowsOrColsNumMethod = GFE_UVGridify::RowColMethod::Uniform,
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
        
        setValidConstPosAttrib();
        
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
        const GA_ROHandleT<POS_VECTOR_T> pos_h(posAttrib);
        const bool isPointAttrib = uvAttrib->getOwner() == GA_ATTRIB_POINT;
        
        vtxPointRef = geo->getTopology().getPointRef();
        UTparallelFor(groupParser.getPrimitiveSplittableRange(), [this, &uv_h, &pos_h, isPointAttrib](const GA_SplittableRange& r)
        {
            value_type scale;
            bool scaleIdx = true;
            
            VECTOR_T uv(GFE_Type::getZeroVector<VECTOR_T>());
            GA_Offset start, end;
            for (GA_Iterator it(r); it.blockAdvance(start, end); )
            {
                for (GA_Offset primoff = start; primoff < end; ++primoff)
                {
                    const GA_OffsetListRef& vertices = geo->getPrimitiveVertexList(primoff);
                    const GA_Size numvtx = vertices.size();
                    if (numvtx == 0)
                        continue;
                    GA_Size rows, cols;
                    switch (rowsOrColsNumMethod)
                    {
                    case GFE_UVGridify::RowColMethod::Uniform:
                        rows = (GA_Size)ceil(numvtx / 4.0);
                        cols = (numvtx - rows - rows) / 2;
                        break;
                    case GFE_UVGridify::RowColMethod::Rows:
                        rows = rowsOrColsNum - 1;
                        cols = (numvtx - rows - rows) / 2;
                        break;
                    case GFE_UVGridify::RowColMethod::Columns:
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
                            const POS_VECTOR_T& pos0 = pos_h.get(vtxPointRef->getLink(vertices[0]));
                            const POS_VECTOR_T& pos1 = pos_h.get(vtxPointRef->getLink(vertices[rows]));
                            const POS_VECTOR_T& pos2 = pos_h.get(vtxPointRef->getLink(vertices[rows + cols]));
                            //GA_Offset a = vtxPointRef->getLink(vertices[rows + cols + rows]);
                            //GA_Offset b = geo->vertexPoint(a);
                            //const POS_VECTOR_T& pos3 = pos_h.get(b);
                            const POS_VECTOR_T& pos3 = pos_h.get(geo->vertexPoint(vtxPointRef->getLink(vertices[rows + cols + rows]));
                            const value_type dist0 = pos0.distance(pos1);
                            const value_type dist1 = pos1.distance(pos2);
                            const value_type dist2 = pos2.distance(pos3);
                            const value_type dist3 = pos3.distance(pos0);

                            scale = (dist0 + dist2) / (dist1 + dist3);

                            const value_type tmpScale = dist0 + dist2;
                            scale = dist1 + dist3;

                            scaleIdx = tmpScale > scale;
                            scale = scaleIdx ? scale / tmpScale : tmpScale / scale;
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

                        uvGridify(vertices, uv_h, uv, primoff, vtxpnum, scale, scaleIdx, isPointAttrib);
                    }
                    for (; vtxpnum < rows + cols; vtxpnum++)
                    {
                        GA_Size tmpI = cols - (rows == 0);
                        uv[0] = 1;
                        uv[1] = tmpI==0 ? 1 : 1 - (float(vtxpnum - rows) / tmpI);

                        uvGridify(vertices, uv_h, uv, primoff, vtxpnum, scale, scaleIdx, isPointAttrib);
                    }
                    const GA_Size numvtx_preCols = numvtx - cols;
                    for (; vtxpnum < numvtx_preCols; vtxpnum++)
                    {
                        GA_Size tmpI = vtxpnum - rows - cols;
                        uv[0] = tmpI == 0 ? 1 : (1 - float(tmpI) / (numvtx - rows - cols - cols - 1));
                        uv[1] = 0;

                        uvGridify(vertices, uv_h, uv, primoff, vtxpnum, scale, scaleIdx, isPointAttrib);
                    }
                    for (; vtxpnum < numvtx; vtxpnum++)
                    {
                        GA_Size tmpI = rows==0 ? numvtx - cols - 1 : cols;
                        uv[0] = 0;
                        uv[1] = tmpI==0 ? 1 : (1 - float(numvtx - vtxpnum - (rows == 0)) / tmpI);

                        uvGridify(vertices, uv_h, uv, primoff, vtxpnum, scale, scaleIdx, isPointAttrib);
                    }
                }
            }
        }, subscribeRatio, minGrainSize);
    }




    template<typename VECTOR_T>
    SYS_FORCE_INLINE void uvGridify(
        const GA_OffsetListRef& vertices,
        const GA_RWHandleT<VECTOR_T>& uv_h,
        VECTOR_T& uv,
        const GA_Offset primoff,
        const GA_Size vtxpnum,
        const typename VECTOR_T::value_type scale,
        const bool scaleIdx,
        const bool isPointAttrib
    )
    {
        if (reverseUVu)
            uv[0] = 1 - uv[0];
        
        if (reverseUVv)
            uv[1] = 1 - uv[1];
        
        if (!uniScale)
            uv[scaleIdx] *= scale;
        
        GA_Offset elemoff = vertices[vtxpnum];
        if (isPointAttrib)
            elemoff = vtxPointRef->getLink(elemoff);
        uv_h.set(elemoff, uv);
    }



    
public:
    GFE_UVGridify::RowColMethod rowsOrColsNumMethod = GFE_UVGridify::RowColMethod::Uniform;
    GA_Size rowsOrColsNum = 2;
    bool reverseUVu = false;
    bool reverseUVv = false;
    bool uniScale = false;

private:
    GA_Attribute* uvAttrib = nullptr;
    
    const GA_ATITopology* vtxPointRef;
    
    exint subscribeRatio = 64;
    exint minGrainSize = 64;
    
}; // End of Class GFE_UVGridify





#endif
