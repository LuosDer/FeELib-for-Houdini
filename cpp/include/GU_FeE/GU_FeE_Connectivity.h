
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



//GU_FeE_Connectivity::connectivity(*geo, areaAttribHandle, geoPrimGroup);
//template <typename T>

static void
connectivity(
    GU_Detail* geo,
    const GA_RWHandleT<GA_Size>& attribHandle,
    const GA_ROHandleT<UT_ValArray<GA_Offset>>& adjElemsAttribHandle,
    const GA_PrimitiveGroup* geoGroup = nullptr,
    const bool makeConstant = false
)
{
    //const GA_ATINumeric* attrib = adjElemsAttribHandle.getAttribute();
    //const GA_StorageClass storageClass = attrib->getStorageClass();
    //const GA_Storage storage = attrib->getStorage();
    //const GA_AttributeOwner attribOwner = attrib->getOwner();

    if (makeConstant)
    {
#if 1
        attribHandle.makeConstant(UNREACHED_NUMBER);
#else
        attrib->myDefaults = GA_Defaults(UNREACHED_NUMBER);
        GU_FeE_Attribute::setToDefault(attribHandle);
#endif
    }
    
    GA_Size classnum = 0;
    const GA_Range range = geo->getPrimitiveRange(geoGroup);


    const GA_SplittableRange geo0SplittableRange(range);



    GA_Offset start;
    GA_Offset end;


    UT_ValArray<GA_Offset> adjElems;
    GA_Offset adjElem;

#if 1
    UT_ValArray<GA_Offset> elemHeap;
#else
    //UT_Array<GA_Offset> elemHeap;
#endif


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

            elemHeap.emplace_back(elemoff);

            while (elemHeap.size() > 0)
            {
                elemHeapLast = elemHeap.last();
                elemHeap.removeLast();

                attribHandle.set(elemHeapLast, classnum);
                adjElemsAttribHandle.get(elemHeapLast, adjElems);
                GA_Size numAdj = adjElems.size();
                for (GA_Size i = 0; i < numAdj; ++i)
                {
                    adjElem = adjElems[i];
                    attribValue = attribHandle.get(adjElem);
                    if (attribValue != UNREACHED_NUMBER)
                        continue;
                    elemHeap.emplace_back(adjElem);
                }
            } // after this while loop, elemHeap.size() == 0
            ++classnum;
        }
    }


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









static void
connectivity(
    GU_Detail* geo,
    const GA_RWHandleT<GA_Size>& attribHandle,
    const GA_ROHandleT<UT_ValArray<GA_Offset>>& adjElemsAttribHandle,
    const GA_PointGroup* geoGroup = nullptr,
    const bool makeConstant = false
)
{
    //const GA_ATINumeric* attrib = adjElemsAttribHandle.getAttribute();
    //const GA_StorageClass storageClass = attrib->getStorageClass();
    //const GA_Storage storage = attrib->getStorage();
    //const GA_AttributeOwner attribOwner = attrib->getOwner();

    if (makeConstant)
    {
#if 1
        attribHandle.makeConstant(UNREACHED_NUMBER);
#else
        attrib->myDefaults = GA_Defaults(UNREACHED_NUMBER);
        GU_FeE_Attribute::setToDefault(attribHandle);
#endif
    }

    double timeTotal = 0;
    double timeTotal1 = 0;
    auto timeStart = std::chrono::steady_clock::now();
    auto timeEnd = std::chrono::steady_clock::now();
    timeStart = std::chrono::steady_clock::now();
    timeEnd = std::chrono::steady_clock::now();



    GA_Size classnum = 0;
    //UT_ValArray<GA_Offset> adjElems(64, 0);
    ::std::vector<GA_Offset> elemHeap;
    //UT_ValArray<GAOffsetStorageType> elemHeap;
    GA_Offset elemHeapLast;
    GA_Size numAdj;
    std::chrono::duration<double> diff;

    const int elemHeapSize = pow(2, 15);
    elemHeap.reserve(elemHeapSize);
    //adjElems.reserve(16);

    if (1)
    {

        timeStart = std::chrono::steady_clock::now();

        const GA_Range range = geo->getPointRange(geoGroup);
        GA_Offset start;
        GA_Offset end;
        for (GA_Iterator it(range); it.fullBlockAdvance(start, end); )
        {
            for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
            {
                if (attribHandle.get(elemoff) != UNREACHED_NUMBER)
                    continue;

                elemHeap.emplace_back(elemoff);
                while (!elemHeap.empty())
                {
                    //elemHeapLast = elemHeap.erase();
                    elemHeapLast = elemHeap[elemHeap.size() - 1];
                    elemHeap.pop_back();

                    attribHandle.set(elemHeapLast, classnum);


                    //timeStart = std::chrono::steady_clock::now();

                    UT_ValArray<GA_Offset> adjElems;
                    adjElemsAttribHandle.get(elemHeapLast, adjElems);

                    //timeEnd = std::chrono::steady_clock::now();
                    //diff = std::chrono::duration_cast<std::chrono::duration<double>>(timeEnd - timeStart);
                    //timeTotal += diff.count() * 1000;



                    numAdj = adjElems.size();
                    for (GA_Size i = 0; i < numAdj; ++i)
                    {
                        if (attribHandle.get(adjElems[i]) != UNREACHED_NUMBER)
                            continue;
                        elemHeap.emplace_back(adjElems[i]);
                    }

                } // after this while loop, elemHeap.size() == 0
                ++classnum;
            }
        }
        timeEnd = std::chrono::steady_clock::now();
        diff = std::chrono::duration_cast<std::chrono::duration<double>>(timeEnd - timeStart);
        timeTotal1 += diff.count() * 1000;

    }
    else
    {
        UT_ValArray<GA_Offset> adjElems(64, 0);
        geo->forEachPoint([&](GA_Offset elemoff) {
            if (attribHandle.get(elemoff) == UNREACHED_NUMBER)
            {
                elemHeap.emplace_back(elemoff);
                while (!elemHeap.empty())
                {
                    //elemHeapLast = elemHeap.erase();
                    elemHeapLast = elemHeap[elemHeap.size() - 1];
                    elemHeap.pop_back();

                    attribHandle.set(elemHeapLast, classnum);

                    timeStart = std::chrono::steady_clock::now();


                    adjElemsAttribHandle.get(elemHeapLast, adjElems);

                    timeEnd = std::chrono::steady_clock::now();
                    auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(timeEnd - timeStart);
                    timeTotal += diff.count();

                    numAdj = adjElems.size();
                    for (GA_Size i = 0; i < numAdj; ++i)
                    {
                        if (attribHandle.get(adjElems[i]) != UNREACHED_NUMBER)
                            continue;
                        elemHeap.emplace_back(adjElems[i]);
                    }

                } // after this while loop, elemHeap.size() == 0
                ++classnum;
            }
        });
    }

    geo->setDetailAttributeF("time", timeTotal);
    geo->setDetailAttributeF("time1", timeTotal1);

}



static void
connectivity1(
    GU_Detail* geo,
    const GA_RWHandleT<GA_Size>& attribHandle,
    const GA_ROHandleT<UT_ValArray<GA_Offset>>& adjElemsAttribHandle,
    const GA_PointGroup* geoGroup = nullptr,
    const bool makeConstant = false
)
{
    //const GA_ATINumeric* attrib = adjElemsAttribHandle.getAttribute();
    //const GA_StorageClass storageClass = attrib->getStorageClass();
    //const GA_Storage storage = attrib->getStorage();
    //const GA_AttributeOwner attribOwner = attrib->getOwner();

    if (makeConstant)
    {
#if 1
        attribHandle.makeConstant(UNREACHED_NUMBER);
#else
        attrib->myDefaults = GA_Defaults(UNREACHED_NUMBER);
        GU_FeE_Attribute::setToDefault(attribHandle);
#endif
    }

    GA_Size classnum = 0;
    UT_ValArray<GA_Offset> adjElems;
    UT_ValArray<GA_Offset> elemHeap;
    GA_Offset elemHeapLast;
    GA_Size numAdj;


    const GA_Range range = geo->getPointRange(geoGroup);
    GA_Offset start;
    GA_Offset end;
    for (GA_Iterator it(range); it.fullBlockAdvance(start, end); )
    {
        for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
        {
            if (attribHandle.get(elemoff) != UNREACHED_NUMBER)
                continue;
            elemHeap.emplace_back(elemoff);
            while (elemHeap.size() > 0)
            {
                attribHandle.set(elemHeap.last(), classnum);
                adjElemsAttribHandle.get(elemHeap.last(), adjElems);
                elemHeap.removeLast();

                numAdj = adjElems.size();
                for (GA_Size i = 0; i < numAdj; ++i)
                {
                    if (attribHandle.get(adjElems[i]) != UNREACHED_NUMBER)
                        continue;
                    elemHeap.emplace_back(adjElems[i]);
                }
            } // after this while loop, elemHeap.size() == 0
            ++classnum;
        }
    }
}





} // End of namespace GU_FeE_Connectivity

#endif
