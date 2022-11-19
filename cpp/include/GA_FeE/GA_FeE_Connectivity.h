
#pragma once

#ifndef __GA_FeE_Connectivity_h__
#define __GA_FeE_Connectivity_h__

//#include <GU_FeE/GA_FeE_Connectivity.h>

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

//#include <GA_FeE/GA_FeE_Adjacency.h>
#include <GU_FeE/GU_FeE_Attribute.h>

#include <time.h>
#include <chrono>


namespace GA_FeE_Connectivity
{


#if 0
    #define UNREACHED_NUMBER SYS_EXINT_MAX
#else
    #define UNREACHED_NUMBER -1
#endif


//#define FUNC_DEFINE_connectivity(DEFParm_GA_ElementGroup, DEFParm_GA_Range)
static void
connectivity(
    GEO_Detail* geo,
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
    std::chrono::duration<double> diff;

    const GA_Range range = geo->getPointRange(geoGroup);
    GA_Size classnum = 0;
    //::std::vector<::std::vector<GA_Offset>> adjElems;
    UT_ValArray<GA_Offset> adjElems(32);
    ::std::vector<GA_Offset> elemHeap;
    elemHeap.reserve(pow(2, 15));
    //adjElems.reserve(16);


    GA_Offset start, end;
    for (GA_Iterator it(range); it.fullBlockAdvance(start, end); )
    {
        for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
        {
            if (attribHandle.get(elemoff) != UNREACHED_NUMBER)
                continue;
            attribHandle.set(elemoff, classnum);
            elemHeap.emplace_back(elemoff);
            while (!elemHeap.empty())
            {
                //timeStart = std::chrono::steady_clock::now();

                adjElemsAttribHandle.get(elemHeap[elemHeap.size() - 1], adjElems);
                //adjElems = adjElemsAttribHandle.get(elemHeap[elemHeap.size() - 1]);

                //timeEnd = std::chrono::steady_clock::now();
                //diff = std::chrono::duration_cast<std::chrono::duration<double>>(timeEnd - timeStart);
                //timeTotal += diff.count();

                elemHeap.pop_back();
                for (GA_Size i = 0; i < adjElems.size(); ++i)
                {
                    if (attribHandle.get(adjElems[i]) != UNREACHED_NUMBER)
                        continue;
                    attribHandle.set(adjElems[i], classnum);
                    elemHeap.emplace_back(adjElems[i]);
                }
            }
            ++classnum;
        }
    }
    //geo->setDetailAttributeF("time", timeTotal * 1000);
    //geo->setDetailAttributeF("time1", timeTotal1 * 1000);
}


//#define FUNC_DEFINE_connectivity(DEFParm_GA_ElementGroup, DEFParm_GA_Range)
static void
connectivityPrim(
    GEO_Detail* geo,
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

    double timeTotal = 0;
    double timeTotal1 = 0;
    auto timeStart = std::chrono::steady_clock::now();
    auto timeEnd   = std::chrono::steady_clock::now();
    timeStart = std::chrono::steady_clock::now();
    timeEnd = std::chrono::steady_clock::now();
    std::chrono::duration<double> diff;

    const GA_Range range = geo->getPrimitiveRange(geoGroup);
    GA_Size classnum = 0;
    //::std::vector<::std::vector<GA_Offset>> adjElems;
    UT_ValArray<GA_Offset> adjElems(32);
    ::std::vector<GA_Offset> elemHeap;
    elemHeap.reserve(pow(2, 15));
    //adjElems.reserve(16);


    GA_Offset start, end;
    for (GA_Iterator it(range); it.fullBlockAdvance(start, end); )
    {
        for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
        {
            if (attribHandle.get(elemoff) != UNREACHED_NUMBER)
                continue;
            attribHandle.set(elemoff, classnum);
            elemHeap.emplace_back(elemoff);
            while (!elemHeap.empty())
            {
                //timeStart = std::chrono::steady_clock::now();

                adjElemsAttribHandle.get(elemHeap[elemHeap.size() - 1], adjElems);
                //adjElems = adjElemsAttribHandle.get(elemHeap[elemHeap.size() - 1]);

                //timeEnd = std::chrono::steady_clock::now();
                //diff = std::chrono::duration_cast<std::chrono::duration<double>>(timeEnd - timeStart);
                //timeTotal += diff.count();

                elemHeap.pop_back();
                for (GA_Size i = 0; i < adjElems.size(); ++i)
                {
                    if (attribHandle.get(adjElems[i]) != UNREACHED_NUMBER)
                        continue;
                    attribHandle.set(adjElems[i], classnum);
                    elemHeap.emplace_back(adjElems[i]);
                }
            }
            ++classnum;
        }
    }
    //geo->setDetailAttributeF("time", timeTotal * 1000);
    //geo->setDetailAttributeF("time1", timeTotal1 * 1000);
}

//FUNC_DEFINE_connectivity(GA_PointGroup, GA_PointGroup)



} // End of namespace GA_FeE_Connectivity

#endif
