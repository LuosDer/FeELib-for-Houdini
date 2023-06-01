
#pragma once

#ifndef __GFE_OffsetVertexOrder_h__
#define __GFE_OffsetVertexOrder_h__

#include "GFE/GFE_OffsetVertexOrder.h"


#include "GFE/GFE_GeoFilter.h"



class GFE_OffsetVertexOrder : public GFE_AttribFilter {



public:
    using GFE_AttribFilter::GFE_AttribFilter;

    ~GFE_OffsetVertexOrder()
    {
    }

    

    SYS_FORCE_INLINE void setOffsetAttrib(const UT_StringRef& attribName)
    {
        offsetAttribNonConst = geo->findAttribute(GA_ATTRIB_PRIMITIVE, attribName);
        offsetAttrib = offsetAttribNonConst;
    }

    SYS_FORCE_INLINE void setOffsetAttrib(const GA_Attribute* const attrib)
    {
        offsetAttribNonConst = nullptr;
        offsetAttrib = attrib;
    }

    SYS_FORCE_INLINE void setOffsetAttrib(GA_Attribute* const attrib)
    {
        offsetAttribNonConst = attrib;
        offsetAttrib = attrib;
    }


    void
        setComputeParm(
            const int64 offset = 0,
            const bool treatLoopedPrimAsClosed = true,
            const bool delOffsetAttrib = false,
            const bool delUnusedPoint = true,
            const exint subscribeRatio = 64,
            const exint minGrainSize = 64
        )
    {
        setHasComputed();
        this->offset = offset;
        this->treatLoopedPrimAsClosed = treatLoopedPrimAsClosed;
        this->delOffsetAttrib = delOffsetAttrib;
        this->delUnusedPoint = delUnusedPoint;
        this->subscribeRatio = subscribeRatio;
        this->minGrainSize = minGrainSize;
    }

    
    SYS_FORCE_INLINE virtual void bumpDataId() const override
    {
        //geo->bumpDataIdsForRewire();
        geo->bumpDataIdsForAddOrRemove(false, true, false);
    }
    
private:

    // can not use in parallel unless for each GA_Detail
    virtual bool
        computeCore() override
    {
        if (offset == 0 && !offsetAttrib)
            return true;

        if (groupParser.isEmpty())
            return true;

        
        if (offsetAttrib)
            offsetVertexOrderByAttrib();
        else
            offsetVertexOrder();

        
        if (delOffsetAttrib && offsetAttribNonConst)
            geo->destroyAttrib(offsetAttribNonConst);


        return true;
    }


    void offsetVertexOrderByAttrib()
    {
        UT_ASSERT_P(offsetAttrib);

        GA_Topology& topo = geo->getTopology();

        GA_Offset ptoff_orig;
        std::vector<GA_Offset> ptoffs_rest;
        ptoffs_rest.reserve(16);
        GA_PageHandleT<exint, exint, true, false, const GA_Attribute, const GA_ATINumeric, const GA_Detail> attrib_ph(offsetAttrib);
        for (GA_PageIterator pit = groupParser.getPrimitiveSplittableRange().beginPages(); !pit.atEnd(); ++pit)
        {
            GA_Offset start, end;
            for (GA_Iterator it(pit.begin()); it.blockAdvance(start, end); )
            {
                attrib_ph.setPage(start);
                for (GA_Offset primoff = start; primoff < end; ++primoff)
                {
                    int64 offsetFinal = offset + attrib_ph.value(primoff);
                    if (offsetFinal == 0)
                        continue;
                    
                    const GA_Size numvtx = geo->getPrimitiveVertexCount(primoff);
                    const bool isLooped = geo->getPrimitiveClosedFlag(primoff) ? true : (treatLoopedPrimAsClosed ? geo->isPrimitiveLooped(primoff) : false);
                    
                    if (isLooped)
                    {
                        ptoffs_rest.emplace_back();
                        offsetFinal %= numvtx;
                        if (offsetFinal > 0)
                        {
                            if (2*offsetFinal > numvtx)
                                offsetFinal -= numvtx;
                        }
                        else
                        {
                            if (-2*offsetFinal > numvtx)
                                offsetFinal += numvtx;
                        }
                        ptoffs_rest.resize(offsetFinal > 0 ? offsetFinal : -offsetFinal);
                        
                        if (offsetFinal > 0)
                        {
                            const GA_Size vtxpnum_last = numvtx-offsetFinal;
                            for (GA_Size vtxpnum = 0; vtxpnum < offsetFinal; ++vtxpnum)
                            {
                                ptoffs_rest[vtxpnum] = geo->getPrimitivePointOffset(primoff, vtxpnum);
                            }
                            for (GA_Size vtxpnum = 0; vtxpnum < vtxpnum_last; ++vtxpnum)
                            {
                                const GA_Offset vtxoff = geo->getPrimitiveVertexOffset(primoff, vtxpnum);
                                if (delUnusedPoint)
                                    ptoff_orig = geo->vertexPoint(vtxoff);

                                const GA_Offset ptoff_target = geo->getPrimitivePointOffset(primoff, vtxpnum+offsetFinal);
                                topo.wireVertexPoint(vtxoff, ptoff_target);
                            
                                if (delUnusedPoint && geo->isUnusedPoint(ptoff_orig))
                                    geo->destroyPointOffset(ptoff_orig, GA_Detail::GA_LEAVE_PRIMITIVES, true);
                            }
                            for (GA_Size vtxpnum = 0; vtxpnum < offsetFinal; ++vtxpnum)
                            {
                                const GA_Offset vtxoff = geo->getPrimitiveVertexOffset(primoff, vtxpnum+offsetFinal);
                                if (delUnusedPoint)
                                    ptoff_orig = geo->vertexPoint(vtxoff);

                                topo.wireVertexPoint(vtxoff, ptoffs_rest[vtxpnum]);
                            
                                if (delUnusedPoint && geo->isUnusedPoint(ptoff_orig))
                                    geo->destroyPointOffset(ptoff_orig, GA_Detail::GA_LEAVE_PRIMITIVES, true);
                            }
                        }
                        else
                        {
                            const GA_Size offset_positive = -offsetFinal;
                            const GA_Size vtxpnum_last = numvtx + offsetFinal;
                            for (GA_Size vtxpnum = 0; vtxpnum < offset_positive; ++vtxpnum)
                            {
                                ptoffs_rest[vtxpnum] = geo->getPrimitivePointOffset(primoff, vtxpnum_last + vtxpnum);
                            }
                            for (GA_Size vtxpnum = offset_positive; vtxpnum < numvtx; ++vtxpnum)
                            {
                                const GA_Offset vtxoff = geo->getPrimitiveVertexOffset(primoff, vtxpnum);
                                if (delUnusedPoint)
                                    ptoff_orig = geo->vertexPoint(vtxoff);
                                
                                const GA_Offset ptoff_target = geo->getPrimitivePointOffset(primoff, vtxpnum+offsetFinal);
                                topo.wireVertexPoint(vtxoff, ptoff_target);
                            
                                if (delUnusedPoint && geo->isUnusedPoint(ptoff_orig))
                                    geo->destroyPointOffset(ptoff_orig, GA_Detail::GA_LEAVE_PRIMITIVES, true);
                            }
                            for (GA_Size vtxpnum = 0; vtxpnum < offset_positive; ++vtxpnum)
                            {
                                const GA_Offset vtxoff = geo->getPrimitiveVertexOffset(primoff, vtxpnum);
                                if (delUnusedPoint)
                                    ptoff_orig = geo->vertexPoint(vtxoff);

                                topo.wireVertexPoint(vtxoff, ptoffs_rest[vtxpnum]);
                            
                                if (delUnusedPoint && geo->isUnusedPoint(ptoff_orig))
                                    geo->destroyPointOffset(ptoff_orig, GA_Detail::GA_LEAVE_PRIMITIVES, true);
                            }
                        }
                    }
                    else
                    {
                        if (offsetFinal > 0)
                        {
                            if (offsetFinal >= numvtx)
                            {
                                geo->destroyPrimitiveOffset(primoff, delUnusedPoint);
                            }
                            else
                            {
                                const GA_Size vtxpnum_last = numvtx-offsetFinal-1;
                                for (GA_Size vtxpnum = 0; vtxpnum <= vtxpnum_last; ++vtxpnum)
                                {
                                    const GA_Offset vtxoff = geo->getPrimitiveVertexOffset(primoff, vtxpnum);
                                    if (delUnusedPoint)
                                        ptoff_orig = geo->vertexPoint(vtxoff);

                                    const GA_Offset ptoff_target = geo->getPrimitivePointOffset(primoff, vtxpnum+offsetFinal);
                                    topo.wireVertexPoint(vtxoff, ptoff_target);
                            
                                    if (delUnusedPoint && geo->isUnusedPoint(ptoff_orig))
                                        geo->destroyPointOffset(ptoff_orig, GA_Detail::GA_LEAVE_PRIMITIVES, true);
                                }
                                for (GA_Size vtxpnum = numvtx-1; vtxpnum > vtxpnum_last; --vtxpnum)
                                {
                                    const GA_Offset vtxoff = geo->getPrimitiveVertexOffset(primoff, vtxpnum);
                                    geo->getPrimitive(primoff)->releaseVertex(vtxoff);
                                    topo.delVertex(vtxoff);
                                    //geo->destroyVertexOffset(vtxoff);
                                }
                            }
                        }
                        else
                        {
                            const GA_Size offset_positive = -offsetFinal;
                            if (offset_positive >= numvtx)
                            {
                                geo->destroyPrimitiveOffset(primoff, delUnusedPoint);
                            }
                            else
                            {
                                for (GA_Size vtxpnum = numvtx-1; vtxpnum >= offset_positive; --vtxpnum)
                                {
                                    const GA_Offset vtxoff = geo->getPrimitiveVertexOffset(primoff, vtxpnum);
                                    if (delUnusedPoint)
                                        ptoff_orig = geo->vertexPoint(vtxoff);
                                    
                                    const GA_Offset ptoff_target = geo->getPrimitivePointOffset(primoff, vtxpnum+offsetFinal);
                                    topo.wireVertexPoint(vtxoff, ptoff_target);
                                    
                                    if (delUnusedPoint && geo->isUnusedPoint(ptoff_orig))
                                        geo->destroyPointOffset(ptoff_orig, GA_Detail::GA_LEAVE_PRIMITIVES, true);
                                }
                                for (GA_Size vtxpnum = offset_positive-1; vtxpnum >= 0; --vtxpnum)
                                {
                                    const GA_Offset vtxoff = geo->getPrimitiveVertexOffset(primoff, vtxpnum);
                                    geo->getPrimitive(primoff)->releaseVertex(vtxoff);
                                    topo.delVertex(vtxoff);
                                    //geo->destroyVertexOffset(vtxoff);
                                }
                            }
                        }
                    }
                }
            }
        }
    }



    void offsetVertexOrder()
    {
        GA_Topology& topo = geo->getTopology();

        GA_Offset ptoff_orig;
        std::vector<GA_Offset> ptoffs_rest;
        GA_Offset start, end;
        for (GA_Iterator it(groupParser.getPrimitiveRange()); it.blockAdvance(start, end); )
        {
            for (GA_Offset primoff = start; primoff < end; ++primoff)
            {
                int64 offsetFinal = offset;
                if (offsetFinal == 0)
                    continue;
                
                const GA_Size numvtx = geo->getPrimitiveVertexCount(primoff);
                const bool isLooped = geo->getPrimitiveClosedFlag(primoff) ? true : (treatLoopedPrimAsClosed ? geo->isPrimitiveLooped(primoff) : false);
                
                if (isLooped)
                {
                    offsetFinal %= numvtx;
                    if (offsetFinal > 0)
                    {
                        if (2*offsetFinal > numvtx)
                            offsetFinal -= numvtx;
                    }
                    else
                    {
                        if (-2*offsetFinal > numvtx)
                            offsetFinal += numvtx;
                    }
                    ptoffs_rest.resize(offsetFinal > 0 ? offsetFinal : -offsetFinal);
                    
                    if (offsetFinal > 0)
                    {
                        const GA_Size vtxpnum_last = numvtx-offsetFinal;
                        for (GA_Size vtxpnum = 0; vtxpnum < offsetFinal; ++vtxpnum)
                        {
                            ptoffs_rest[vtxpnum] = geo->getPrimitivePointOffset(primoff, vtxpnum);
                        }
                        for (GA_Size vtxpnum = 0; vtxpnum < vtxpnum_last; ++vtxpnum)
                        {
                            const GA_Offset vtxoff = geo->getPrimitiveVertexOffset(primoff, vtxpnum);
                            if (delUnusedPoint)
                                ptoff_orig = geo->vertexPoint(vtxoff);
                            
                            const GA_Offset ptoff_target = geo->getPrimitivePointOffset(primoff, vtxpnum+offsetFinal);
                            topo.wireVertexPoint(vtxoff, ptoff_target);
                            
                            if (delUnusedPoint && geo->isUnusedPoint(ptoff_orig))
                                geo->destroyPointOffset(ptoff_orig, GA_Detail::GA_LEAVE_PRIMITIVES, true);
                        }
                        for (GA_Size vtxpnum = 0; vtxpnum < offsetFinal; ++vtxpnum)
                        {
                            const GA_Offset vtxoff = geo->getPrimitiveVertexOffset(primoff, vtxpnum+offsetFinal);
                            if (delUnusedPoint)
                                ptoff_orig = geo->vertexPoint(vtxoff);
                            
                            topo.wireVertexPoint(vtxoff, ptoffs_rest[vtxpnum]);
                            
                            if (delUnusedPoint && geo->isUnusedPoint(ptoff_orig))
                                geo->destroyPointOffset(ptoff_orig, GA_Detail::GA_LEAVE_PRIMITIVES, true);
                        }
                    }
                    else
                    {
                        const GA_Size offset_positive = -offsetFinal;
                        const GA_Size vtxpnum_last = numvtx + offsetFinal;
                        for (GA_Size vtxpnum = 0; vtxpnum < offset_positive; ++vtxpnum)
                        {
                            ptoffs_rest[vtxpnum] = geo->getPrimitivePointOffset(primoff, vtxpnum_last + vtxpnum);
                        }
                        for (GA_Size vtxpnum = offset_positive; vtxpnum < numvtx; ++vtxpnum)
                        {
                            const GA_Offset vtxoff = geo->getPrimitiveVertexOffset(primoff, vtxpnum);
                            if (delUnusedPoint)
                                ptoff_orig = geo->vertexPoint(vtxoff);
                            
                            const GA_Offset ptoff_target = geo->getPrimitivePointOffset(primoff, vtxpnum+offsetFinal);
                            topo.wireVertexPoint(vtxoff, ptoff_target);
                            
                            if (delUnusedPoint && geo->isUnusedPoint(ptoff_orig))
                                geo->destroyPointOffset(ptoff_orig, GA_Detail::GA_LEAVE_PRIMITIVES, true);
                        }
                        for (GA_Size vtxpnum = 0; vtxpnum < offset_positive; ++vtxpnum)
                        {
                            const GA_Offset vtxoff = geo->getPrimitiveVertexOffset(primoff, vtxpnum);
                            if (delUnusedPoint)
                                ptoff_orig = geo->vertexPoint(vtxoff);

                            topo.wireVertexPoint(vtxoff, ptoffs_rest[vtxpnum]);
                            
                            if (delUnusedPoint && geo->isUnusedPoint(ptoff_orig))
                                geo->destroyPointOffset(ptoff_orig, GA_Detail::GA_LEAVE_PRIMITIVES, true);
                        }
                    }
                }
                else
                {
                    if (offsetFinal > 0)
                    {
                        if (offsetFinal >= numvtx)
                        {
                            geo->destroyPrimitiveOffset(primoff, delUnusedPoint);
                        }
                        else
                        {
                            const GA_Size vtxpnum_last = numvtx-offsetFinal-1;
                            for (GA_Size vtxpnum = 0; vtxpnum <= vtxpnum_last; ++vtxpnum)
                            {
                                const GA_Offset vtxoff = geo->getPrimitiveVertexOffset(primoff, vtxpnum);
                                if (delUnusedPoint)
                                    ptoff_orig = geo->vertexPoint(vtxoff);

                                const GA_Offset ptoff_target = geo->getPrimitivePointOffset(primoff, vtxpnum+offsetFinal);
                                topo.wireVertexPoint(vtxoff, ptoff_target);
                            
                                if (delUnusedPoint && geo->isUnusedPoint(ptoff_orig))
                                    geo->destroyPointOffset(ptoff_orig, GA_Detail::GA_LEAVE_PRIMITIVES, true);
                            }
                            for (GA_Size vtxpnum = numvtx-1; vtxpnum > vtxpnum_last; --vtxpnum)
                            {
                                const GA_Offset vtxoff = geo->getPrimitiveVertexOffset(primoff, vtxpnum);
                                geo->getPrimitive(primoff)->releaseVertex(vtxoff);
                                topo.delVertex(vtxoff);
                                //geo->destroyVertexOffset(vtxoff);
                            }
                        }
                    }
                    else
                    {
                        const GA_Size offset_positive = -offsetFinal;
                        if (offset_positive >= numvtx)
                        {
                            geo->destroyPrimitiveOffset(primoff, delUnusedPoint);
                        }
                        else
                        {
                            for (GA_Size vtxpnum = numvtx-1; vtxpnum >= offset_positive; --vtxpnum)
                            {
                                const GA_Offset vtxoff = geo->getPrimitiveVertexOffset(primoff, vtxpnum);
                                if (delUnusedPoint)
                                    ptoff_orig = geo->vertexPoint(vtxoff);
                                
                                const GA_Offset ptoff_target = geo->getPrimitivePointOffset(primoff, vtxpnum+offsetFinal);
                                topo.wireVertexPoint(vtxoff, ptoff_target);
                            
                                if (delUnusedPoint && geo->isUnusedPoint(ptoff_orig))
                                    geo->destroyPointOffset(ptoff_orig, GA_Detail::GA_LEAVE_PRIMITIVES, true);
                            }
                            for (GA_Size vtxpnum = offset_positive-1; vtxpnum >= 0; --vtxpnum)
                            {
                                const GA_Offset vtxoff = geo->getPrimitiveVertexOffset(primoff, vtxpnum);
                                geo->getPrimitive(primoff)->releaseVertex(vtxoff);
                                topo.delVertex(vtxoff);
                                //geo->destroyVertexOffset(vtxoff);
                            }
                        }
                    }
                }
            }
        }
    }
    
    // void offsetVertexOrderByAttrib()
    // {
    //     UT_ASSERT_P(offsetAttrib);
    //
    //     UTparallelFor(groupParser.getPrimitiveSplittableRange(), [this](const GA_SplittableRange& r)
    //     {
    //         GA_PageHandleT<exint, exint, true, false, const GA_Attribute, const GA_ATINumeric, const GA_Detail> attrib_ph(offsetAttrib);
    //         for (GA_PageIterator pit = r.beginPages(); !pit.atEnd(); ++pit)
    //         {
    //             GA_Offset start, end;
    //             for (GA_Iterator it(pit.begin()); it.blockAdvance(start, end); )
    //             {
    //                 attrib_ph.setPage(start);
    //                 for (GA_Offset primoff = start; primoff < end; ++primoff)
    //                 {
    //                     const GA_Size offsetFinal = offset + attrib_ph.value(primoff);
    //
    //                     const GA_Size numvtx = geo->getPrimitiveVertexCount(primoff);
    //
    //                     const bool isClosed = geo->getPrimitiveClosedFlag(primoff);
    //                     const bool isPrimLooped = geo->isPrimitiveLooped(primoff);
    //                     
    //                     for (GA_Size vtxpnum = 0; vtxpnum < numvtx; ++vtxpnum)
    //                     {
    //                         const GA_Offset vtxoff = geo->getPrimitiveVertexOffset(primoff, vtxpnum);
    //
    //                         GA_Size vtxpnum_next = vtxpnum + offsetFinal;
    //                         if (vtxpnum_next >= numvtx || vtxpnum_next < 0)
    //                         {
    //                             if (isPrimLooped)
    //                             {
    //                                 if (isClosed)
    //                                 {
    //                                     vtxpnum_next %= numvtx;
    //                                 }
    //                                 else
    //                                 {
    //                                     vtxpnum_next %= numvtx - 1;
    //                                 }
    //                             }
    //                             else
    //                             {
    //                                 geo->destroyVertexOffset(vtxoff);
    //                                 continue;
    //                             }
    //                         }
    //                         geo->getTopology().wireVertexPoint(vtxoff, geo->getPrimitivePointOffset(primoff, vtxpnum_next));
    //                     }
    //                 }
    //             }
    //         }
    //     }, subscribeRatio, minGrainSize);
    // }
    //
    //
    //
    // void offsetVertexOrder()
    // {
    //
    //     UTparallelFor(groupParser.getPrimitiveSplittableRange(), [this](const GA_SplittableRange& r)
    //     {
    //         for (GA_PageIterator pit = r.beginPages(); !pit.atEnd(); ++pit)
    //         {
    //             GA_Offset start, end;
    //             for (GA_Iterator it(pit.begin()); it.blockAdvance(start, end); )
    //             {
    //                 for (GA_Offset primoff = start; primoff < end; ++primoff)
    //                 {
    //                     const GA_Size numvtx = geo->getPrimitiveVertexCount(primoff);
    //
    //                     const bool isClosed = geo->getPrimitiveClosedFlag(primoff);
    //                     const bool isPrimLooped = geo->isPrimitiveLooped(primoff);
    //
    //                     for (GA_Size vtxpnum = 0; vtxpnum < numvtx; ++vtxpnum)
    //                     {
    //                         const GA_Offset vtxoff = geo->getPrimitiveVertexOffset(primoff, vtxpnum);
    //
    //                         GA_Size vtxpnum_next = vtxpnum + offset;
    //                         if (vtxpnum_next >= numvtx || vtxpnum_next < 0)
    //                         {
    //                             if (isPrimLooped)
    //                             {
    //                                 if (isClosed)
    //                                 {
    //                                     vtxpnum_next %= numvtx;
    //                                 }
    //                                 else
    //                                 {
    //                                     vtxpnum_next %= numvtx - 1;
    //                                 } 
    //                             }
    //                             else
    //                             {
    //                                 geo->destroyVertexOffset(vtxoff);
    //                                 continue;
    //                             }
    //                         }
    //                         geo->getTopology().wireVertexPoint(vtxoff, geo->getPrimitivePointOffset(primoff, vtxpnum_next));
    //                     }
    //                 }
    //             }
    //         }
    //     }, subscribeRatio, minGrainSize);
    // }
    //


public:
    int64 offset = 0;
    bool treatLoopedPrimAsClosed = true;
    bool delUnusedPoint = true;
    bool delOffsetAttrib = false;
    
private:
    
    const GA_Attribute* offsetAttrib = nullptr;
    GA_Attribute* offsetAttribNonConst = nullptr;

    exint subscribeRatio = 64;
    exint minGrainSize = 64;


}; // End of Class GFE_OffsetVertexOrder









#endif
