
#pragma once

#ifndef __GFE_FlatEdge_h__
#define __GFE_FlatEdge_h__

#include "GFE/GFE_FlatEdge.h"

#include "GFE/GFE_GeoFilter.h"


#include "GFE/GFE_Math.h"
#include "GFE/GFE_MeshTopology.h"
#include "GFE/GFE_Normal3D.h"


class GFE_FlatEdge : public GFE_AttribFilter {

public:
    enum ManifoldEdgeOp
    {
        Min,
        Max,
    };
    //using GFE_AttribFilter::GFE_AttribFilter;

    GFE_FlatEdge(
        GA_Detail* const geo,
        const SOP_NodeVerb::CookParms* const cookparms = nullptr
    )
        : GFE_AttribFilter(geo, cookparms)
        , normal3D(geo, cookparms)
    {
    }

    GFE_FlatEdge(
        GA_Detail& geo,
        const SOP_NodeVerb::CookParms* const cookparms = nullptr
    )
        : GFE_AttribFilter(geo, cookparms)
        , normal3D(geo, cookparms)
    {
    }

    ~GFE_FlatEdge()
    {
    }

    void
        setComputeParm(
            const bool includeUnsharedEdge = true,
            const ManifoldEdgeOp manifoldEdge,
            const bool outAsVertexGroup = true,
            const fpreal flatEdgeAngleThreshold = 1e-05,
            const bool absoluteDot = true,
            const exint subscribeRatio = 64,
            const exint minGrainSize = 1024
        )
    {
        setHasComputed();
        this->includeUnsharedEdge = includeUnsharedEdge;
        this->manifoldEdge = manifoldEdge;
        this->outAsVertexGroup = outAsVertexGroup;
        this->flatEdgeAngleThreshold = flatEdgeAngleThreshold;
        this->absoluteDot = absoluteDot;
        
        this->subscribeRatio = subscribeRatio;
        this->minGrainSize = minGrainSize;
    }


    SYS_FORCE_INLINE void findOrCreateGroup(const UT_StringRef& groupName = "")
    { getOutGroupArray().findOrCreateEdge(doDelGroupElement, groupName); }
    
private:

    // can not use in parallel unless for each GA_Detail
    virtual bool
        computeCore() override
    {
        if (getOutGroupArray().isEmpty())
            return false;

        if (groupParser.isEmpty())
            return true;

        if (!normalAttrib || (normalAttrib->getOwner() != GA_ATTRIB_VERTEX && normalAttrib->getOwner() != GA_ATTRIB_PRIMITIVE))
        {
            if (normal3D.getOutAttribArray().isEmpty())
            {
                normal3D.getOutAttribArray().findOrCreateNormal3D(true);
            }
            normal3D.compute();
            normalAttrib = normal3D.getAttrib();
        }


        groupSetter = getOutGroupArray()[0];
        
        GFE_MeshTopology meshTopology(geo, cookparms);
        //const GA_Attribute* const vertexNextEquivNoLoopAttrib = adjacency.setVertexNextEquivNoLoop();
        const GA_Attribute* const vertexNextEquivAttrib = meshTopology.setVertexNextEquiv();
        meshTopology.compute();

        GA_VertexGroup* const outVertexGroup = getOutGroupArray()[0];
        GA_EdgeGroup*   const outEdgeGroup   = getOutGroupArray()[0];

        
        switch (normalAttrib->getAIFTuple()->getStorage(normalAttrib))
        {
        case GA_STORE_REAL16: flatEdge<fpreal16>(); break;
        case GA_STORE_REAL32: flatEdge<fpreal32>(); break;
        case GA_STORE_REAL64: flatEdge<fpreal64>(); break;
        default: break;
        }

        if (doDelGroupElement)
            delGroupElement();
        
        return true;
    }


    template<typename FLOAT_T>
    void flatEdge()
    {
        GA_ROHandleT<UT_Vector3T<FLOAT_T>> normal_h(normalAttrib);

        flatEdgeThreshold = -cos(GFE_Math::radians(flatEdgeAngleThreshold));

        UTparallelFor(groupParser.getVertexSplittableRange(), [this, outVertexGroup, outEdgeGroup, vertexNextEquivAttrib, &normal_h](const GA_SplittableRange& r)
        {
            // GA_PageHandleT<UT_Vector3T<FLOAT_T>, T, true, true, GA_Attribute, GA_ATINumeric, GA_Detail> normal_ph(normalAttrib);
            // GA_PageHandleT<GA_Offset, GA_Offset, true, true, GA_Attribute, GA_ATINumeric, GA_Detail> vertexNextEquivNoLoop_ph(vertexNextEquivNoLoopAttrib);
            GA_PageHandleT<GA_Offset, GA_Offset, true, true, GA_Attribute, GA_ATINumeric, GA_Detail> vertexNextEquiv_ph(vertexNextEquivAttrib);
            for (GA_PageIterator pit = r.beginPages(); !pit.atEnd(); ++pit)
            {
                GA_Offset start, end;
                for (GA_Iterator it(pit.begin()); it.blockAdvance(start, end); )
                {
                    //normal_ph.setPage(start);
                    for (GA_Offset vtxoff = start; vtxoff < end; ++vtxoff)
                    {
                        if (includeUnsharedEdge)
                        {
                            normal_h.get(vtxoff);
                            if (vtxoff < vertexNextEquiv_ph.value(vtxoff) || vertexNextEquiv_ph.value(vtxoff) != GFE_INVALID_OFFSET)
                            {
                                outVertexGroup->setElement(vtxoff, false);
                                continue;//includeunshared
                            }
                        }
                        else
                        {
                            if (vtxoff <= vertexNextEquiv_ph.value(vtxoff))
                            {
                                outVertexGroup->setElement(vtxoff, false);
                                continue;//excludeunshared
                            }
                        }

                        if (normalAttrib->getOwner() == GA_ATTRIB_VERTEX && !inedgegroup(0, chs("../group"), @ptnum, vertexpoint(0, hedge_nextequiv)))
                        {
                            outVertexGroup->setElement(vtxoff, false);
                            return;//includeunshared
                        }

                        UT_Vector3T<FLOAT_T> N0, N1;
                        if (normalAttrib->getOwner() == GA_ATTRIB_VERTEX)
                        {
                            N0 = v@opinput1_N;
                            N1 = vertex(1, 'N', hedge_nextequiv);
                        }
                        else
                        {
                            N0 = prim(1, 'N', @primnum);
                            N1 = prim(1, 'N', vertexprim(0, hedge_nextequiv));
                        }
                        FLOAT_T dot = N0.dot(N1);
                        if (absoluteDot && dot < 0)
                            dot *= -1;
                        /*
                        if ( @ptnum == 1 && vertexpoint(0, hedge_nextequiv)==13 ) cout(dot >= chf('../flatEdgeAngleThreshold'));
                        if ( @ptnum == 13 && vertexpoint(0, hedge_nextequiv)==1 ) cout(dot >= chf('../flatEdgeAngleThreshold'));
                        if ( @ptnum == 1 && vertexpoint(0, hedge_nextequiv)==13 ) cout(i@outgrp);
                        if ( @ptnum == 13 && vertexpoint(0, hedge_nextequiv)==1 ) cout(i@outgrp);
                        */
                        /*
                        if ( @ptnum == 15 && vertexpoint(0, hedge_nextequiv)==14 ) cout(dot);
                        if ( @ptnum == 14 && vertexpoint(0, hedge_nextequiv)==15 ) cout(dot);
                        */
                        /*
                        if ( @ptnum == 7 && vertexpoint(0, hedge_nextequiv)==14 ) cout(dot);
                        if ( @ptnum == 14 && vertexpoint(0, hedge_nextequiv)==7 ) cout(dot);
                        if ( @ptnum == 7 && vertexpoint(0, hedge_nextequiv)==14 ) cout(i@outgrp);
                        if ( @ptnum == 14 && vertexpoint(0, hedge_nextequiv)==7 ) cout(i@outgrp);
                        */

                        const bool inGroup = (dot >= flatEdgeThreshold) ^ reverseGroup;
                        if (outAsVertexGroup)
                        {
                            outVertexGroup->setElement(vtxoff, inGroup);
                        }
                        else if (inGroup)
                        {
                            outEdgeGroup->add(ptoff, vertexpoint(0, hedge_nextequiv));
                        }


                        /*
                        if ( chi("../manifoldEdge") < 2 )
                        {
                            //none
                            //all
                            i@outgrp = chi("../manifoldEdge");
                        }
                        else if ( chi("../manifoldEdge") < 4 )
                        {
                            //min
                            //max
                            float exdot = chi("../manifoldEdge")==3 ? -MAXF32 : MAXF32;

                            vector Ns[];
                            resize(Ns, hedge_equivcount);
                            hedge_nextequiv = vtxoff;
                            for (int i = 0; i < hedge_equivcount; ++i)
                            {
                                Ns[i] = prim(1, 'N', vertexprim(0, hedge_nextequiv));
                                hedge_nextequiv = hedge_nextequiv(0, hedge_nextequiv);
                            }

                            for (int j = 0; j < hedge_equivcount; ++j)
                            {
                                for (int i = j+1; i < hedge_equivcount; ++i)
                                {
                                    float dot = dot(Ns[i], Ns[j]);
                                    dot = chi('../absoluteDot') ? abs(dot) : dot;
                                    exdot = chi("../manifoldEdge")==3 ? max(exdot, dot) : min(exdot, dot);
                                }
                            }
                            i@outgrp = (exdot < chf('../flatEdgeAngleThreshold')) ^ chi('../reverse');
                        }
                        else
                        {
                            error('unsupport val');
                            return;
                        }
                        */


                    }
                }
            }
        }, subscribeRatio, minGrainSize);
    }




public:
    GFE_Normal3D normal3D;
    
    bool includeUnsharedEdge = true;
    ManifoldEdgeOp manifoldEdge;
    bool outAsVertexGroup = true;
    fpreal flatEdgeAngleThreshold = 1e-05;
    bool absoluteDot = true;
    
private:
    GA_Attribute* normalAttrib = nullptr;
    fpreal flatEdgeThreshold;
    exint subscribeRatio = 64;
    exint minGrainSize = 1024;


}; // End of class GFE_FlatEdge



#endif
