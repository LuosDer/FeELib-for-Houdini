
#pragma once

#ifndef __GU_FeE_Connectivity_h__
#define __GU_FeE_Connectivity_h__

//#include <GU_FeE/GU_FeE_Connectivity.h>

#include <GU/GU_Detail.h>
#include <GEO/GEO_PrimPoly.h>
#include <OP/OP_Operator.h>
#include <OP/OP_OperatorTable.h>
#include <PRM/PRM_Include.h>
#include <PRM/PRM_TemplateBuilder.h>
#include <UT/UT_DSOVersion.h>
#include <UT/UT_Interrupt.h>
#include <UT/UT_StringHolder.h>
#include <SYS/SYS_Math.h>
#include <limits.h>

#include <GEO/GEO_AdjPolyIterator.h>
//#include <GA/GA_Handle.h>
//#include <GA/GA_ATINumeric.h>

//#include <GEO_FeE/GEO_FeE_Adjacency.h>
#include <GU_FeE/GU_FeE_Attribute.h>

#include <time.h>
#include <chrono>


namespace GU_FeE_Connectivity
{


#if 0
    #define UNREACHED_NUMBER SYS_EXINT_MAX
#else
    #define UNREACHED_NUMBER -1
#endif



using attribPrecisonF = fpreal32;
using TAttribTypeV = UT_Vector3T<attribPrecisonF>;



//template <typename T>
static void
connectivity(
    GU_Detail* geo,
    const GA_RWHandleT<exint>& attribHandle,
    const GA_PrimitiveGroup* geoPrimGroup = nullptr
)
{
    UT_AutoInterrupt boss("Compute Connectivity");

    const GA_ATINumeric* attrib = attribHandle.getAttribute();
    const GA_StorageClass storageClass = attrib->getStorageClass();
    const GA_Storage storage = attrib->getStorage();
    const GA_AttributeOwner attribOwner = attrib->getOwner();
    //GU_FeE_Connectivity::connectivity(*geo, areaAttribHandle, geoPrimGroup);

#if 1
    attribHandle.makeConstant(UNREACHED_NUMBER);
#else
    attrib->myDefaults = GA_Defaults(UNREACHED_NUMBER);
    GU_FeE_Attribute::setToDefault(attribHandle);
#endif
    
    GA_Size classnum = 0;
    const GA_Range range = geo->getPrimitiveRange(geoPrimGroup);
    const GA_SplittableRange geo0SplittableRange(range);

    GA_Offset start;
    GA_Offset end;


    //for (GA_Iterator it(range); it.blockAdvance(start, end); )
    //{
    //    for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
    //    {
    //        attribHandle.set(elemoff, UNREACHED_NUMBER);
    //    }
    //}

    //using timeTtype = std::milli;
    //time_t timeTotal, timeSub, timeStart, timeEnd;

    //time(&timeTotal);
    double timeTotal = 0;
    //_CHRONO time_point<steady_clock> timeTotal = 0;
    GEO_Detail::GEO_EdgeAdjArray adjElems;
    GA_Offset adjElem;
#if 1
    UT_ValArray<GA_Offset> elemHeap;
#else
    //UT_Array<GA_Offset> elemHeap;
#endif



#if 0
    geo->forEachPrimitive([&geo, &attribHandle, &classnum, &adjElems, &adjElem, &timeTotal, &elemHeap](const GA_Offset& elemoff)
    {
        auto start = std::chrono::steady_clock::now();
        auto end = std::chrono::steady_clock::now();
        start = std::chrono::steady_clock::now();
        GA_Offset attribValue = attribHandle.get(elemoff);
        if (attribValue != UNREACHED_NUMBER)
            return;
        //if (elemHeap.size() == 0)
        //elemHeap.append(elemoff);

        elemHeap.emplace_back(elemoff);

        end = std::chrono::steady_clock::now();
        //timeTotal += std::chrono::duration<double, timeTtype>(end - start).count();

        //while (elemHeap.capacity() > 0)
        while (elemHeap.size() > 0)
        {

            GA_Offset elemHeapLast = elemHeap.last();
            elemHeap.removeLast();

            attribHandle.set(elemHeapLast, classnum);

            //time(&timeStart);
            start = std::chrono::steady_clock::now();
            GA_Size numAdj = geo->getEdgeAdjacentPolygons(adjElems, elemHeapLast);
            end = std::chrono::steady_clock::now();
            timeTotal += std::chrono::duration<double, timeTtype>(end - start).count();

            //time(&timeEnd);

            //difftime(timeEnd, timeStart);
            //GA_Offset* nebs = getPointPointEdgeAdjacent();
            for (GA_Size i = 0; i < numAdj; ++i)
            {
                //GEO_Detail::EdgeAdjacencyData adjElem = adjElems[i];
                //GA_Offset adjElem = adjElem.myAdjacentPolygon;
                adjElem = adjElems[i].myAdjacentPolygon;
                attribValue = attribHandle.get(adjElem);
                if (attribValue != UNREACHED_NUMBER)
                    continue;
                //elemHeap.append(adjElem);

                elemHeap.emplace_back(adjElem);

            }
        } // after this while loop, elemHeap.size() == 0
        ++classnum;
    });
#else
    GA_Offset attribValue;
    GA_Offset elemHeapLast;
    //for (GA_Iterator it(range); it.blockAdvance(start, end); )
    for (GA_Iterator it(range); it.fullBlockAdvance(start, end); )
    {
        for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
        {
            attribValue = attribHandle.get(elemoff);
            if (attribValue != UNREACHED_NUMBER)
                continue;
            //if (elemHeap.size() == 0)
            //elemHeap.append(elemoff);

            elemHeap.emplace_back(elemoff);


            //while (elemHeap.capacity() > 0)
            while (elemHeap.size() > 0)
            {
                elemHeapLast = elemHeap.last();
                elemHeap.removeLast();

                attribHandle.set(elemHeapLast, classnum);

                //time(&timeStart);
                GA_Size numAdj = geo->getEdgeAdjacentPolygons(adjElems, elemHeapLast);

                //time(&timeEnd);
                
                //difftime(timeEnd, timeStart);
                //GA_Offset* nebs = getPointPointEdgeAdjacent();
                for (GA_Size i = 0; i < numAdj; ++i)
                {
                    //GEO_Detail::EdgeAdjacencyData adjElem = adjElems[i];
                    //GA_Offset adjElem = adjElem.myAdjacentPolygon;
                    adjElem = adjElems[i].myAdjacentPolygon;
                    attribValue = attribHandle.get(adjElem);
                    if (attribValue != UNREACHED_NUMBER)
                        continue;
                    //elemHeap.append(adjElem);

                    elemHeap.emplace_back(adjElem);

                }
            } // after this while loop, elemHeap.size() == 0
            ++classnum;
        }
    }
#endif
    attribHandle->bumpDataId();

#if 1
#if 1
    //GA_Attribute* attribPtrDebug = geo->createAttribute(GA_ATTRIB_DETAIL, "time", nullptr, nullptr, nullptr);
    //GA_RWHandleT<fpreal> attribHandleDebug(attribPtrDebug);
    //geo->setDetailAttributeF("time", timeTotal);
    //char *asStr2;
    //sprintf(asStr2, "%lf", timeTotal);
    //geo->setDetailAttributeS("timestr", asStr2);
#else
    GA_Attribute* attribPtrDebug = geo->addFloatTuple(GA_ATTRIB_GLOBAL, "time", 1, GA_Defaults(-1.0), 0, 0, GA_STORE_REAL64);
    GA_RWHandleT<fpreal> attribHandleDebug(attribPtrDebug);
#endif
#else
    GA_Attribute* attribPtrDebug = geo->addIntTuple(GA_ATTRIB_GLOBAL, "time", 1, GA_Defaults(0), 0, 0, GA_STORE_INT64);
    GA_RWHandleT<exint> attribHandleDebug(attribPtrDebug);
    attribHandleDebug.set(0, timeTotal);
    attribHandleDebug->bumpDataId();
#endif
}




} // End of namespace GU_FeE_Connectivity

#endif
