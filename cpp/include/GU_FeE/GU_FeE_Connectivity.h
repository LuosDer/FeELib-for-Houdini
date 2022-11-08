
#pragma once

#ifndef __GU_FeE_Connectivity_h__
#define __GU_FeE_Connectivity_h__


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

#include <GU_FeE/GU_FeE_Attribute.h>

namespace GU_FeE_Connectivity
{


#if 0
    #define UNREACHED_NUMBER SYS_EXINT_MAX
#else
    #define UNREACHED_NUMBER -1
#endif



using attribPrecisonF = fpreal32;
using TAttribTypeV = UT_Vector3T<attribPrecisonF>;



template <typename T>
static GA_Attribute*
connectivity(
    const GU_Detail* geo,
    const GA_RWHandleT<T>& attribHandle,
    const GA_PrimitiveGroup* geoPrimGroup = nullptr
)
{
    UT_AutoInterrupt boss("Compute Connectivity");

    const GA_ATINumeric* attrib = attribHandle.getAttribute();
    const GA_StorageClass storageClass = attrib->getStorageClass();
    const GA_Storage storage = attrib->getStorage();
    const GA_AttributeOwner attribOwner = attrib->getOwner();
    const GA_AttributeOwner attribOwner = attrib->getOwner();
    //GU_FeE_Connectivity::connectivity(*geo, areaAttribHandle, geoPrimGroup);

#if 1
    attribHandle->makeConstant(UNREACHED_NUMBER);
#else
    attrib->myDefaults = GA_Defaults(UNREACHED_NUMBER);
    GU_FeE_Attribute::setToDefault(attribHandle);
#endif
    
    GA_Size classnum = 0;

    //const GA_Range range = geo->getPrimitiveRange();
    //const GA_SplittableRange geo0SplittableRange(geo->getPrimitiveRange());

    //GA_Offset start;
    //GA_Offset end;
    //for (GA_Iterator it(range); it.blockAdvance(start, end); )
    //{
    //    for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
    //    {
    //        attribHandle.set(elemoff, UNREACHED_NUMBER);
    //    }
    //}


    UT_Array<GA_Offset> processHeap;
    for (GA_Iterator it(r); it.fullBlockAdvance(start, end); )
    {
        for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
        {
            if (processHeap.size() == 0)
                processHeap.append(elemoff);
            //while (processHeap.capacity() > 0)
            while (processHeap.size() > 0)
            {
                GA_Offset attribValue = attribHandle.get(elemoff);
                if (attribValue == UNREACHED_NUMBER)
                    continue;

                GA_Offset* nebs = geo->neighbour();
                for (GA_Size i = start; i < len(nebs); ++i)
                {
                    processHeap.append(nebs[i]);
                    attribHandle.set(nebs[i], classnum);
                }
                attribHandle.set(elemoff, classnum);
            }
            ++classnum;
        }
    }

    attribHandle->bumpDataId();
}




} // End of namespace GU_FeE_Connectivity

#endif
