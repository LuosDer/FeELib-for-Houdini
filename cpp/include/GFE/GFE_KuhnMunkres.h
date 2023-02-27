
#pragma once

#ifndef __GFE_KuhnMunkres_h__
#define __GFE_KuhnMunkres_h__

//#include "GFE/GFE_KuhnMunkres.h"

#if 0
#define ENABLE_ASSERTS 1
#endif

#include "GA/GA_Detail.h"


#include "GFE/GFE_Type.h"


//https://zhuanlan.zhihu.com/p/267074414
class GF_KuhnMunkres
{
public:
    int KM()
    {
        memset(match, -1, sizeof match);    // 初始每个男生都没有匹配的女生
        memset(ex_boy, 0, sizeof ex_boy);   // 初始每个男生的期望值为0

        // 每个女生的初始期望值是与她相连的男生最大的好感度
        for (int i = 0; i < N; ++i) {
            ex_girl[i] = love[i][0];
            for (int j = 1; j < N; ++j) {
                ex_girl[i] = SYSmax(ex_girl[i], love[i][j]);
            }
        }

        // 尝试为每一个女生解决归宿问题
        for (int i = 0; i < N; ++i) {

            fill(slack, slack + N, INF);    // 因为要取最小值 初始化为无穷大

            while (1) {
                // 为每个女生解决归宿问题的方法是 ：如果找不到就降低期望值，直到找到为止

                // 记录每轮匹配中男生女生是否被尝试匹配过
                memset(vis_girl, false, sizeof vis_girl);
                memset(vis_boy, false, sizeof vis_boy);

                if (dfs(i)) break;  // 找到归宿 退出

                // 如果不能找到 就降低期望值
                // 最小可降低的期望值
                int d = INF;
                for (int j = 0; j < N; ++j)
                    if (!vis_boy[j]) d = SYSmin(d, slack[j]);

                for (int j = 0; j < N; ++j) {
                    // 所有访问过的女生降低期望值
                    if (vis_girl[j]) ex_girl[j] -= d;

                    // 所有访问过的男生增加期望值
                    if (vis_boy[j]) ex_boy[j] += d;
                    // 没有访问过的boy 因为girl们的期望值降低，距离得到女生倾心又进了一步！
                    else slack[j] -= d;
                }
            }
        }

        // 匹配完成 求出所有配对的好感度的和
        int res = 0;
        for (int i = 0; i < N; ++i)
            res += love[match[i]][i];

        return res;
    }



private:
    const int MAXN = 305;
    const int INF = 0x3f3f3f3f;


    int love[MAXN][MAXN];   // 记录每个妹子和每个男生的好感度
    int ex_girl[MAXN];      // 每个妹子的期望值
    int ex_boy[MAXN];       // 每个男生的期望值
    bool vis_girl[MAXN];    // 记录每一轮匹配匹配过的女生
    bool vis_boy[MAXN];     // 记录每一轮匹配匹配过的男生
    int match[MAXN];        // 记录每个男生匹配到的妹子 如果没有则为-1
    int slack[MAXN];        // 记录每个汉子如果能被妹子倾心最少还需要多少期望值

    int N;


private:
    bool dfs(int girl)
    {
        vis_girl[girl] = true;

        for (int boy = 0; boy < N; ++boy) {

            if (vis_boy[boy]) continue; // 每一轮匹配 每个男生只尝试一次

            int gap = ex_girl[girl] + ex_boy[boy] - love[girl][boy];

            if (gap == 0) {  // 如果符合要求
                vis_boy[boy] = true;
                if (match[boy] == -1 || dfs(match[boy])) {    // 找到一个没有匹配的男生 或者该男生的妹子可以找到其他人
                    match[boy] = girl;
                    return true;
                }
            }
            else {
                slack[boy] = SYSmin(slack[boy], gap);  // slack 可以理解为该男生要得到女生的倾心 还需多少期望值 取最小值 备胎的样子【捂脸
            }
        }

        return false;
    }


};

namespace GFE_KuhnMunkres {


    template<typename FLOAT_T>
    static void
        km(
            GA_Detail* const geo,
            const GA_Detail* const refGeo,
            const GA_RWHandleT<GA_Offset>& kmMatchedPoint_h,
            const GA_ROHandleT<FLOAT_T>& weight
        )
    {
        UT_ASSERT_P(geo);
        UT_ASSERT_P(refGeo);


    }




template<typename VECTOR_T>
static void
km(
    GA_Detail* const geo,
    const GA_Detail* const refGeo,
    const GA_RWHandleT<GA_Offset>& kmMatchedPoint_h,
    const GA_ROHandleT<VECTOR_T>& pos_h,
    const GA_ROHandleT<VECTOR_T>& posRef_h
)
{
    UT_ASSERT_P(geo);
    UT_ASSERT_P(refGeo);

    GA_Index indexSize    = pos_h   .getAttribute()->getIndexMap().indexSize();
    GA_Index indexSizeRef = posRef_h.getAttribute()->getIndexMap().indexSize();
    
    std::vector<indexSizeRef>
    const GA_ROHandleT<FLOAT_T>& weight_h
    km<UT_Vector3T<fpreal16>>(geo, refGeo, kmMatchedPointAttribPtr, posAttribPtr, posRefAttribPtr);
}



static GA_Attribute*
    km(
        GA_Detail* const geo,
        const GA_Detail* const refGeo,
        const UT_StringHolder& kmMatchedPointName = "kmMatchedPoint",
        const GA_Storage storage = GA_STORE_INVALID,
        const UT_StringHolder& posAttribName = "P",
        const UT_StringHolder& refPosAttribName = "P"
    ) {
    UT_ASSERT_P(geo);
    UT_ASSERT_P(refGeo);

    if (!kmMatchedPointName.isstring() || kmMatchedPointName.isEmpty())
        return nullptr;


    const GA_Storage finalStorage = storage == GA_STORE_INVALID ? GFE_Type::getPreferredStorageI(geo) : storage;

    GA_Attribute* kmMatchedPointAttribPtr = geo->findPointAttribute(kmMatchedPointName);
    if (kmMatchedPointAttribPtr && kmMatchedPointAttribPtr->getStorageClass() != GA_STORECLASS_INT)
    {
        geo->getAttributes().destroyAttribute(kmMatchedPointAttribPtr);
        kmMatchedPointAttribPtr = nullptr;
    }

    if(!kmMatchedPointAttribPtr)
        kmMatchedPointAttribPtr = geo->getAttributes().createTupleAttribute(GA_ATTRIB_POINT, kmMatchedPointName, finalStorage, 1, GA_Defaults(-1));

    const GA_Attribute* posAttribPtr = geo->findPointAttribute(kmMatchedPointName);
    if (!posAttribPtr)
        posAttribPtr = geo->getP();

    const GA_Attribute* posRefAttribPtr = refGeo->findPointAttribute(kmMatchedPointName);
    if (!posRefAttribPtr)
        posRefAttribPtr = refGeo->getP();

    switch (posAttribPtr->getAIFTuple()->getStorage(posAttribPtr))
    {
    case GA_STORE_REAL16:
        km<UT_Vector3T<fpreal16>>(geo, refGeo, kmMatchedPointAttribPtr, posAttribPtr, posRefAttribPtr);
        break;
    case GA_STORE_REAL32:
        km<UT_Vector3T<fpreal32>>(geo, refGeo, kmMatchedPointAttribPtr, posAttribPtr, posRefAttribPtr);
        break;
    case GA_STORE_REAL64:
        km<UT_Vector3T<fpreal64>>(geo, refGeo, kmMatchedPointAttribPtr, posAttribPtr, posRefAttribPtr);
        break;
    default:
        break;
    }

    return kmMatchedPointAttribPtr;
}





static GA_Attribute*
    km(
        const SOP_NodeVerb::CookParms& cookparms,
        GA_Detail* const geo,
        const GA_Detail* const refGeo,
        const UT_StringHolder& kmMatchedPointName = "kmMatchedPoint",
        const GA_Storage storage = GA_STORE_INVALID,
        const UT_StringHolder& posAttribName = "P",
        const UT_StringHolder& refPosAttribName = "P"
    ) {
    UT_ASSERT_P(geo);
    UT_ASSERT_P(refGeo);

    GA_Attribute* kmMatchedPointAttribPtr = km(geo, refGeo, kmMatchedPointName, storage, posAttribName, refPosAttribName);

    kmMatchedPointAttribPtr->bumpDataId();
    return kmMatchedPointAttribPtr;
}







} // End of namespace GFE_KuhnMunkres

#endif
