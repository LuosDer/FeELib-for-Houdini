
#pragma once

#ifndef __GFE_AttribValCount_h__
#define __GFE_AttribValCount_h__

//#include "GFE/GFE_AttribValCount.h"

#include "GA/GA_Detail.h"




namespace GFE_AttribValCount {





    template<typename T>
    class AttribValCount {
    public:
        AttribValCount(const GA_Attribute* const attrib, const T& attribVal)
            : myAttrib(attrib)
            , myAttribVal(attribVal)
            , mySum(0)
        {}
        AttribValCount(AttribValCount& src, UT_Split)
            : myAttrib(src.myAttrib)
            , myAttribVal(src.myAttribVal)
            , mySum(0)
        {}
        void operator()(const GA_SplittableRange& r)
        {
            GA_PageHandleT<T, T, true, false, const GA_Attribute, const GA_ATINumeric, const GA_Detail> attrib_ph(myAttrib);
            for (GA_PageIterator pit = r.beginPages(); !pit.atEnd(); ++pit)
            {
                GA_Offset start, end;
                for (GA_Iterator it(pit.begin()); it.blockAdvance(start, end); )
                {
                    attrib_ph.setPage(start);
                    for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                    {
                        mySum += attrib_ph.value(elemoff) == myAttribVal;
                    }
                }
            }
        }

        void join(const AttribValCount& other)
        {
            mySum += other.mySum;
        }
        GA_Size getSum()
        {
            return mySum;
        }
    private:
        GA_Size mySum;
        const GA_Attribute* const myAttrib;
        const T& myAttribVal;
    };

    template<typename T>
    static GA_Size
        getAttribValCount(
            const GA_Attribute* const attribPtr,
            const T& attribVal,
            const exint subscribeRatio = 64,
            const exint minGrainSize = 1024
        )
    {
        UT_ASSERT_P(attribPtr);

        const GA_SplittableRange geoSplittableRange(GA_Range(attribPtr->getIndexMap()));


        AttribValCount<T> body(attribPtr, attribVal);
        UTparallelReduce(geoSplittableRange, body, subscribeRatio, minGrainSize);
        return body.getSum();

        //GA_Size count = 0;
        //UTparallelFor(geoSplittableRange, [attribPtr, &attribVal](const GA_SplittableRange& r)
        //{
        //    GA_PageHandleT<T, typename T::value_type, true, true, GA_Attribute, GA_ATINumeric, GA_Detail> attrib_ph(attribPtr);
        //    for (GA_PageIterator pit = r.beginPages(); !pit.atEnd(); ++pit)
        //    {
        //        GA_Offset start, end;
        //        for (GA_Iterator it(pit.begin()); it.blockAdvance(start, end); )
        //        {
        //            attrib_ph.setPage(start);
        //            for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
        //            {
        //                count += attrib_ph.value(elemoff) == attribVal;
        //            }
        //        }
        //    }
        //}, subscribeRatio, minGrainSize);

        //return count;
    }






} // End of namespace GFE_AttribValCount

#endif
