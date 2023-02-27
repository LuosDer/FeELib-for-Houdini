
#pragma once

#ifndef __GFE_Array_h__
#define __GFE_Array_h__

//#include "GFE/GFE_Array.h"

#include "GA/GA_Detail.h"
//#include "GA/GA_SplittableRange.h"



//#include "GFE/GFE_Type.h


namespace GFE_Array {





template<typename T>
std::vector<int>
argsort(
    const std::vector<T>& inArray
)
{
    std::vector<int> indexs(inArray.size());
    for (int i = 0; i < inArray.size(); ++i)
        indexs[i] = i;

    std::sort(indexs.begin(), indexs.end(),
        [&inArray](int pos1, int pos2) {return (inArray[pos1] < inArray[pos2]); });

    return indexs;
}



template<typename T>
UT_Array<int>
argsort_UT_Array(
    const UT_Array<T>& inArray
)
{
    UT_Array<int> indexs(inArray.size());
    for (int i = 0; i < inArray.size(); ++i)
        indexs[i] = i;

    std::sort(indexs.begin(), indexs.end(),
        [&inArray](int pos1, int pos2) {return (inArray[pos1] < inArray[pos2]); });

    return indexs;
}




//UT_Array<int>
//argsort_UT_Array(
//    const UT_Array<fpreal>& inArray
//)
//{
//    UT_Array<int> indexs(inArray.size());
//    for (int i = 0; i < inArray.size(); ++i)
//        indexs[i] = i;
//
//    std::sort(indexs.begin(), indexs.end(),
//        [&inArray](int pos1, int pos2) {return (inArray[pos1] < inArray[pos2]); });
//
//    return indexs;
//}











} // End of namespace GFE_Array

#endif
