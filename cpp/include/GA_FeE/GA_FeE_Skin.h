
#pragma once

#ifndef __GA_FeE_Skin_h__
#define __GA_FeE_Skin_h__



//#include "GA_FeE/GA_FeE_Skin.h"

#include "GA/GA_Detail.h"
#include "GEO/GEO_PrimPoly.h"

//#include "GA/GA_Types.h"

#include "GA_FeE/GA_FeE_Attribute.h"
#include "GA_FeE/GA_FeE_AttribPromote.h"
#include "GA_FeE/GA_FeE_ConvertLine.h"
#include "GA_FeE/GA_FeE_Connectivity.h"
#include "GA_FeE/GA_FeE_VertexNextEquiv.h"



namespace GA_FeE_Skin {





static void
skin(
    GA_Detail* const geo,
    const bool closed = false,
    const bool excludeSharedEdge = false,
    const GA_PrimitiveGroup* const endGroup = nullptr,

    const GA_Storage storage = GA_STORE_INVALID,
    const exint subscribeRatio = 32,
    const exint minGrainSize = 1024
)
{
    const GA_Attribute* const connectivityPointAttrib = GA_FeE_Connectivity::addAttribConnectivityPoint(geo, nullptr, nullptr, GA_STORE_INT64);
    const GA_Attribute* const connectivityPrimAttrib = GA_FeE_AttribPromote::attribPromote(geo, connectivityPointAttrib, GA_ATTRIB_PRIMITIVE);

    const GA_PrimitiveGroupUPtr endGroupTmpUPtr = geo->createDetachedPrimitiveGroup();
    GA_PrimitiveGroup* const endGroupTmp = endGroupTmpUPtr.get();
    endGroupTmp->combine(endGroup);

    const GA_VertexGroup* const unsharedGroup = excludeSharedEdge ? GA_FeE_VertexNextEquiv::addGroupVertexNextEquiv(geo, nullptr, storage) : nullptr;

    const GA_Offset primoff_first = GA_FeE_ConvertLine::convertLine(geo, true, true, false, "", true, "", "", "", "", storage);
    //GA_FeE_ConvertLine::convertLine(geo, inGeo0, true);

    const GA_ROHandleT<GA_Offset> connectivityPrim_h(connectivityPrimAttrib);
    GA_Offset val0 = connectivityPrim_h.get(geo->primitiveOffset(0));
    GA_Offset val1 = connectivityPrim_h.get(geo->primitiveOffset(1));
    GA_Offset val2 = connectivityPrim_h.get(geo->primitiveOffset(2));
    GA_Offset val3 = connectivityPrim_h.get(geo->primitiveOffset(3));
    GA_Offset val4 = connectivityPrim_h.get(geo->primitiveOffset(4));
    GA_Offset val5 = connectivityPrim_h.get(geo->primitiveOffset(5));
    GA_Offset val6 = connectivityPrim_h.get(geo->primitiveOffset(6));
    return;


    GA_Offset start, end;
    for (GA_Iterator it(GA_Range(geo->getPrimitiveMap(), primoff_first, geo->getNumPrimitiveOffsets())); it.fullBlockAdvance(start, end); )
    {
        for (GA_Offset primoff = start; primoff < end; ++primoff)
        {
            GEO_PrimPoly* const primpoly = UTverify_cast<GEO_PrimPoly*>(geo->getPrimitive(primoff));
            const GA_OffsetListRef& vertices = geo->getPrimitiveVertexList(primoff);
            const GA_Size lastIndex = vertices.size() - 1;

            GA_Size attribValCount = GA_FeE_Attribute::getAttribValCount<GA_Offset>(connectivityPointAttrib, connectivityPrim_h.get(primoff));
            if (closed)
            {
                if (endGroupTmp->contains(primoff))
                {
                    for (GA_Size i = lastIndex; i >= 0; --i)
                    {
                        primpoly->appendVertex(geo->vertexPoint(geo->vertexOffset(geo->vertexIndex(vertices[i]) + attribValCount)));
                        //primpoly->stealVertex(vtx);
                    }
                }
                else
                {
                    GA_Size primAttribValCount = GA_FeE_Attribute::getAttribValCount<GA_Offset>(connectivityPrimAttrib, connectivityPrim_h.get(primoff));
                    GA_Size depth = 0;
                    for (GA_Size i = geo->getPrimitiveMap().indexFromOffset(primoff) - primAttribValCount; i >= 0; i -= primAttribValCount)
                    {
                        if (!endGroupTmp->contains(geo->getPrimitiveMap().offsetFromIndex(i))) break;
                        ++depth;
                    }
                    attribValCount *= depth;

                    for (GA_Size i = lastIndex; i >= 0; --i)
                    {
                        primpoly->appendVertex(geo->vertexPoint(geo->vertexOffset(geo->vertexIndex(vertices[i]) - attribValCount)));
                        //primpoly->stealVertex(vtx);
                    }
                }
            }
            else
            {
                for (GA_Size i = lastIndex; i > -1; --i)
                {
                    primpoly->appendVertex(geo->vertexPoint(geo->vertexOffset(geo->vertexIndex(vertices[i]) + attribValCount)));
                    //primpoly->stealVertex(vtx);
                }
            }
        }
    }
}



} // End of namespace GA_FeE_Skin

#endif
