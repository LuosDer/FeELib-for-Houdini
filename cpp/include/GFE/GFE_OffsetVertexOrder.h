
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
            const bool loopPrimAsClosed = false,
            const bool delOffsetAttrib = false,
            const exint subscribeRatio = 64,
            const exint minGrainSize = 64
        )
    {
        setHasComputed();
        this->offset = offset;
        this->loopPrimAsClosed = loopPrimAsClosed;
        this->delOffsetAttrib = delOffsetAttrib;
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
        
        GA_PageHandleT<exint, exint, true, false, const GA_Attribute, const GA_ATINumeric, const GA_Detail> attrib_ph(offsetAttrib);
        for (GA_PageIterator pit = groupParser.getPrimitiveSplittableRange().beginPages(); !pit.atEnd(); ++pit)
        {
            GA_Offset start, end;
            for (GA_Iterator it(pit.begin()); it.blockAdvance(start, end); )
            {
                attrib_ph.setPage(start);
                for (GA_Offset primoff = start; primoff < end; ++primoff)
                {
                    const int64 offsetFinal = offset + attrib_ph.value(primoff);
                    if (offsetFinal > 0)
                    {
                        
                        for (GA_Size vtxpnum = numvtx-1; vtxpnum > numvtx; --vtxpnum)
                        {
                            const GA_Offset vtxoff = geo->getPrimitiveVertexOffset(primoff, vtxpnum);

                            GA_Size vtxpnum_next = vtxpnum + offsetFinal;
                            if (vtxpnum_next >= numvtx || vtxpnum_next < 0)
                            {
                                loopPrimAsClosed
                                if (isPrimLooped)
                                {
                                    if (isClosed)
                                    {
                                        vtxpnum_next %= numvtx;
                                    }
                                    else
                                    {
                                        vtxpnum_next %= numvtx - 1;
                                    }
                                }
                                else
                                {
                                    geo->getPrimitive(primoff)->releaseVertex(vtxoff);
                                    topo.delVertex(vtxoff);
                                    //geo->destroyVertexOffset(vtxoff);
                                    continue;
                                }
                            }
                    }
                    else
                    {
                        
                    }
                    const GA_Size numvtx = geo->getPrimitiveVertexCount(primoff);

                    const bool isClosed = geo->getPrimitiveClosedFlag(primoff);
                    const bool isPrimLooped = geo->isPrimitiveLooped(primoff);
                    
                    for (GA_Size vtxpnum = 0; vtxpnum < numvtx; ++vtxpnum)
                    {
                        const GA_Offset vtxoff = geo->getPrimitiveVertexOffset(primoff, vtxpnum);

                        GA_Size vtxpnum_next = vtxpnum + offsetFinal;
                        if (vtxpnum_next >= numvtx || vtxpnum_next < 0)
                        {
                            loopPrimAsClosed
                            if (isPrimLooped)
                            {
                                if (isClosed)
                                {
                                    vtxpnum_next %= numvtx;
                                }
                                else
                                {
                                    vtxpnum_next %= numvtx - 1;
                                }
                            }
                            else
                            {
                                geo->getPrimitive(primoff)->releaseVertex(vtxoff);
                                topo.delVertex(vtxoff);
                                //geo->destroyVertexOffset(vtxoff);
                                continue;
                            }
                        }
                        topo.wireVertexPoint(vtxoff, geo->getPrimitivePointOffset(primoff, vtxpnum_next));
                    }
                }
            }
        }
    }



    void offsetVertexOrder()
    {
        GA_Topology& topo = geo->getTopology();
        GA_Offset start, end;
        for (GA_Iterator it(groupParser.getPrimitiveRange()); it.blockAdvance(start, end); )
        {
            for (GA_Offset primoff = start; primoff < end; ++primoff)
            {
                const GA_Size numvtx = geo->getPrimitiveVertexCount(primoff);

                const bool isClosed = geo->getPrimitiveClosedFlag(primoff);
                const bool isPrimLooped = geo->isPrimitiveLooped(primoff);


                for (GA_Size vtxpnum = 0; vtxpnum < numvtx; ++vtxpnum)
                {
                    GA_Size vtxpnum_next = vtxpnum + offset;
                    const GA_Offset vtxoff = geo->getPrimitiveVertexOffset(primoff, vtxpnum);
                    if (vtxpnum_next >= numvtx || vtxpnum_next < 0)
                    {
                        if (isPrimLooped)
                        {
                            if (isClosed)
                            {
                                vtxpnum_next %= numvtx;
                            }
                            else
                            {
                                vtxpnum_next %= numvtx - 1;
                            } 
                        }
                        else
                        {
                            geo->getPrimitive(primoff)->releaseVertex(vtxoff);
                            topo.delVertex(vtxoff);
                            //geo->destroyVertexOffset(vtxoff);
                            continue;
                        }
                    }
                    topo.wireVertexPoint(vtxoff, geo->getPrimitivePointOffset(primoff, vtxpnum_next));
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
    bool loopPrimAsClosed = false;
    bool delOffsetAttrib = false;

private:
    
    const GA_Attribute* offsetAttrib = nullptr;
    GA_Attribute* offsetAttribNonConst = nullptr;

    exint subscribeRatio = 64;
    exint minGrainSize = 64;


}; // End of Class GFE_OffsetVertexOrder









#endif
