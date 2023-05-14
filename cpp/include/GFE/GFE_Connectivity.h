
#pragma once

#ifndef __GFE_Connectivity_h__
#define __GFE_Connectivity_h__

//#include "GFE/GFE_Connectivity.h"


#include "GFE/GFE_GeoFilter.h"

#include "GFE/GFE_MeshTopology.h"
#include "GFE/GFE_AttributePromote.h"
#include "GFE/GFE_AttributeCast.h"




class GFE_Connectivity : public GFE_AttribFilter
{

//#define GFE_TEMP_ConnectivityAttribName "__TEMP_GFE_ConnectivityAttrib"

public:

GFE_Connectivity(
    GFE_Detail* const geo,
    const SOP_NodeVerb::CookParms* const cookparms = nullptr
)
    : GFE_AttribFilter(geo, cookparms)
    , groupParserSeam(geo, groupParser.getGOP(), cookparms)
{
}

GFE_Connectivity(
    GA_Detail& geo,
    const SOP_NodeVerb::CookParms* const cookparms = nullptr
)
    : GFE_AttribFilter(geo, cookparms)
    , groupParserSeam(geo, groupParser.getGOP(), cookparms)
{
}
    
~GFE_Connectivity()
{
}



    
    void
    setComputeParm(
        const bool connectivityConstraint = false,
        const bool outTopoAttrib = true
    )
    {
        setHasComputed();

        this->outTopoAttrib = outTopoAttrib;
        this->connectivityConstraint = connectivityConstraint;
    }

    void
        findOrCreateTuple(
            const bool detached = false,
            const GA_AttributeOwner attribOwner = GA_ATTRIB_PRIMITIVE,
            const GA_StorageClass storageClass = GA_STORECLASS_INT,
            const GA_Storage storage = GA_STORE_INVALID,
            const UT_StringHolder& name = "__topo_connectivity"
        )
    {
        getOutAttribArray().clear();

        const GA_AttributeOwner connectivityOwner = connectivityConstraint ? GA_ATTRIB_PRIMITIVE : GA_ATTRIB_POINT;
        const bool isDetached = connectivityOwner != attribOwner || storageClass != GA_STORECLASS_INT;
#if 1
        if (isDetached)
            getOutAttribArray().findOrCreateTuple(detached, attribOwner, storageClass, storage, name, 1, GA_Defaults(GFE_INVALID_OFFSET));
    
        connectivityAttribPtr = getOutAttribArray().findOrCreateTuple(isDetached, connectivityOwner, GA_STORECLASS_INT, GA_STORE_INVALID,
            isDetached ? UT_StringRef("") : name, 1, GA_Defaults(GFE_INVALID_OFFSET));
#else
        connectivityAttribPtr = getOutAttribArray().findOrCreateTuple(true, connectivityOwner, GA_STORECLASS_INT, storage, isDetached ? GFE_TEMP_ConnectivityAttribName : name);
        if (isDetached)
            getOutAttribArray().findOrCreateTuple(detached, attribOwner, storageClass, storage, name);
#endif
    }

    SYS_FORCE_INLINE GA_Attribute* getConnectivityAttrib() const
    { return getOutAttribArray().isEmpty() ? nullptr : getOutAttribArray()[0]; }
            







private:


    virtual bool
        computeCore() override
    {
        if (getOutAttribArray().isEmpty())
            return false;
        
        if (groupParser.isEmpty())
            return true;

        connectivity();

        if (getOutAttribArray().size() > 1)
        {
            GA_Attribute* const outAttribPtr = getOutAttribArray()[0];
            const GA_AttributeOwner connectivityOriginOwner = connectivityConstraint ? GA_ATTRIB_PRIMITIVE : GA_ATTRIB_POINT;
            //const bool isDetached = connectivityOwner != outAttrib->get || storageClass != GA_STORECLASS_INT;

            const bool shouldPromote = outAttribPtr->getOwner() != connectivityOriginOwner;
            const bool shouldCast = outAttribPtr->getStorageClass() != GA_STORECLASS_INT;
        
            GFE_AttribPromote attribPromote(geo);
            if (shouldPromote)
            {
                //GA_Storage sto = GFE_Attribute::getStorage(connectivityAttribPtr);
                attribPromote.setSourceAttribute(connectivityAttribPtr);
                
                if (shouldCast)
                    attribPromote.setDetachedDestinationAttribute(outAttribPtr->getOwner());
                else
                    attribPromote.setDestinationAttribute(outAttribPtr);
                
                attribPromote.compute();
            }
            if (shouldCast)
            {
                GA_Attribute* const inAttribPtr = shouldPromote ? attribPromote.getDestinationAttribute() : connectivityAttribPtr;
                //GA_StorageClass storage537 = inAttribPtr->getStorageClass();
                //GA_Storage storage124 = GFE_Attribute::getStorage(inAttribPtr);
                GFE_AttribCast attribCast(geo);
                attribCast.getInAttribArray().set(inAttribPtr);
                attribCast.setDestinationAttribute(*outAttribPtr);
                //attribCast.newStorageClass = outAttribStorageClass;
                //attribCast.precision = GA_PRECISION_INVALID;
                attribCast.compute();
                //GA_Attribute* const attrib = attribCast.getOutAttribArray()[0];
                //const char* a = attrib->getName().c_str();
                //const char* b = outAttribPtr->getName().c_str();
            }
            //geo->forceRenameAttribute(outAttribPtr, outAttribPtr->getName());
        }

        return true;
    }




    void
        connectivity()
    {
#if 1
        GFE_MeshTopology meshTopology(geo, cookparms);
        meshTopology.outAsOffset = false;
        meshTopology.outIntermediateAttrib = true;
        
        if (connectivityConstraint)
        {
            meshTopology.groupParser.setGroup(groupParser.getPrimitiveGroup());
            meshTopology.setPrimPrimEdge(true);
            meshTopology.compute();
            adjElemsAttrib_h.bind(meshTopology.getPrimPrimEdge());
        }
        else
        {
            meshTopology.groupParser.setGroup(groupParser.getPointGroup());
            meshTopology.setPointPointEdge(true);
            meshTopology.compute();
            adjElemsAttrib_h.bind(meshTopology.getPointPointEdge());
        }
#else
        GA_Attribute* adjElemsAttrib = nullptr;
    if (connectivityConstraint)
    {
        adjElemsAttrib = GFE_Adjacency_Namespace::addAttribPrimPrimEdge(geo, groupParser.getPrimitiveGroup(), groupParserSeam.getVertexGroup());
    }
    else
    {
        adjElemsAttrib = GFE_Adjacency_Namespace::addAttribPointPointEdge(geo, groupParser.getPointGroup(), groupParserSeam.getPointGroup());
    }
    adjElemsAttrib_h.bind(adjElemsAttrib);
#endif

    //double timeTotal = 0;
    //double timeTotal1 = 0;
    //auto timeStart = std::chrono::steady_clock::now();
    //auto timeEnd = std::chrono::steady_clock::now();
    //timeStart = std::chrono::steady_clock::now();
    //timeEnd = std::chrono::steady_clock::now();
    //std::chrono::duration<double> diff;

    //::std::vector<::std::vector<GA_Offset>> adjElems;

    //const GA_PrimitiveGroup* const geoGroup = groupParser.getPrimitiveGroup();

    const GA_AttributeOwner connectivityOwner = connectivityConstraint ? GA_ATTRIB_PRIMITIVE : GA_ATTRIB_POINT;
    
    const GA_Size nelems = connectivityConstraint ? geo->getNumPrimitives() : geo->getNumPoints();

    ::std::vector<GA_Offset> elemHeap;
    //elemHeap.reserve(__min(maxElemHeapSize, nelems));
    elemHeap.reserve(SYSmin(maxElemHeapSize, nelems));

    ::std::vector<GA_Offset> classnumArray(nelems, GFE_INVALID_OFFSET);

    UT_PackedArrayOfArrays<GA_Offset> packedArray;
    adjElemsAttrib_h->getAIFNumericArray()->getPackedArrayFromIndices(adjElemsAttrib_h.getAttribute(), 0, nelems, packedArray);

    const UT_Array<GA_Size>& rawOffsets = packedArray.rawOffsets();
    UT_Array<GA_Offset>& rawData = packedArray.rawData();
    
    // if (connectivityConstraint)
    // {
    //     if (rawData.size() < pow(2, 12))
    //     {
    //         for (GA_Offset elemoff = 0; elemoff < rawData.size(); ++elemoff)
    //         {
    //             rawData[elemoff] = geo->primitiveIndex(rawData[elemoff]);
    //         }
    //     }
    //     else
    //     {
    //         UTparallelFor(UT_BlockedRange<GA_Size>(0, rawData.size()), [this, &rawData](const UT_BlockedRange<GA_Size>& r) {
    //             for (GA_Offset elemoff = r.begin(); elemoff != r.end(); ++elemoff)
    //             {
    //                 rawData[elemoff] = geo->primitiveIndex(rawData[elemoff]);
    //             }
    //         }, 16, 1024);
    //     }
    // }
    // else
    // {
    //     if (rawData.size() < pow(2, 12))
    //     {
    //         for (GA_Offset elemoff = 0; elemoff < rawData.size(); ++elemoff)
    //         {
    //             rawData[elemoff] = geo->pointIndex(rawData[elemoff]);
    //         }
    //     }
    //     else
    //     {
    //         UTparallelFor(UT_BlockedRange<GA_Size>(0, rawData.size()), [this, &rawData](const UT_BlockedRange<GA_Size>& r) {
    //             for (GA_Offset elemoff = r.begin(); elemoff != r.end(); ++elemoff)
    //             {
    //                 rawData[elemoff] = geo->pointIndex(rawData[elemoff]);
    //             }
    //         }, 16, 1024);
    //     }
    // }


    //timeEnd = std::chrono::steady_clock::now();
    //diff = timeEnd - timeStart;
    //timeTotal += diff.count();
    //timeStart = std::chrono::steady_clock::now();


    GA_Index classnum = startClassnum;
    for (GA_Size elemoff = 0; elemoff < nelems; ++elemoff)
    {
        if (classnumArray[elemoff] != GFE_INVALID_OFFSET)
            continue;
        classnumArray[elemoff] = classnum;
        elemHeap.emplace_back(elemoff);
        while (!elemHeap.empty())
        {
            const GA_Size lastElem = elemHeap[elemHeap.size() - 1];
            elemHeap.pop_back();
            const GA_Size rawOffsetEnd = rawOffsets[lastElem + 1];
            for (GA_Size i = rawOffsets[lastElem]; i < rawOffsetEnd; ++i)
            {
                const GA_Offset rawDataVal = rawData[i];
                if (classnumArray[rawDataVal] != GFE_INVALID_OFFSET)
                    continue;
                classnumArray[rawDataVal] = classnum;
                elemHeap.emplace_back(rawDataVal);
            }
        }
        ++classnum;
    }
    
    GA_Attribute* const connectivityAttribPtr = this->connectivityAttribPtr;
    const GA_IndexMap& indexMap = geo->getIndexMap(connectivityOwner);
    UTparallelFor(groupParser.getSplittableRange(connectivityOwner),
        [&indexMap, &classnumArray, connectivityAttribPtr](const GA_SplittableRange& r)
    {
        GA_PageHandleScalar<GA_Offset>::RWType attrib_ph(connectivityAttribPtr);
        for (GA_PageIterator pit = r.beginPages(); !pit.atEnd(); ++pit)
        {
            GA_Offset start, end;
            for (GA_Iterator it(pit.begin()); it.blockAdvance(start, end); )
            {
                attrib_ph.setPage(start);
                for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                {
                    attrib_ph.value(elemoff) = classnumArray[indexMap.indexFromOffset(elemoff)];
                }
            }
        }
    }, 16, 1024);
    //geo->setAttributeFromArray(attrib_h.getAttribute(), GA_Range(), UT_Array<GA_Size>(classnumArray));


    //timeEnd = std::chrono::steady_clock::now();
    //diff = timeEnd - timeStart;
    //timeTotal1 += diff.count();
    //timeStart = std::chrono::steady_clock::now();

    //geo->setDetailAttributeF("time", timeTotal * 1000);
    //geo->setDetailAttributeF("time1", timeTotal1 * 1000);
}



//#undef GFE_TEMP_ConnectivityAttribName
    
public:
    GFE_GroupParser groupParserSeam;
    GA_Size startClassnum = 0;
    bool connectivityConstraint = false; // false means point  and  true means edge 
    //GA_AttributeOwner connectivityOwner = GA_ATTRIB_PRIMITIVE;
    // bool outAsOffset = true;

private:
    GA_Attribute* connectivityAttribPtr;
    GA_ROHandleT<UT_ValArray<GA_Offset>> adjElemsAttrib_h;


    
    const GA_Size maxElemHeapSize = pow(2, 15);
    const UT_StringHolder GFE_TEMP_ConnectivityAttribName = "__TEMP_GFE_ConnectivityAttrib";

    
}; // End of class GFE_Connectivity
















































/*




namespace GFE_Connectivity_Namespace {



#if 0
    #define UNREACHED_NUMBER SYS_EXINT_MAX
#else
    #define UNREACHED_NUMBER -1
#endif


static void
connectivityPoint(
    const GA_Detail& geo,
    const GA_RWHandleT<GA_Size>& attrib_h,
    const GA_ROHandleT<UT_ValArray<GA_Offset>>& adjElemsAttrib_h,
    const GA_PointGroup* const geoGroup = nullptr
)
{
    //double timeTotal = 0;
    //double timeTotal1 = 0;
    //auto timeStart = std::chrono::steady_clock::now();
    //auto timeEnd = std::chrono::steady_clock::now();
    //timeStart = std::chrono::steady_clock::now();
    //timeEnd = std::chrono::steady_clock::now();
    //std::chrono::duration<double> diff;

    //::std::vector<::std::vector<GA_Offset>> adjElems;
    GA_Size nelems = geo.getNumPoints();

    ::std::vector<GA_Offset> elemHeap;
    elemHeap.reserve(__min(pow(2, 15), nelems));

    ::std::vector<GA_Offset> classnumArray(nelems, UNREACHED_NUMBER);

    UT_PackedArrayOfArrays<GA_Offset> packedArray;
    adjElemsAttrib_h->getAIFNumericArray()->getPackedArrayFromIndices(adjElemsAttrib_h.getAttribute(), 0, nelems, packedArray);

    const UT_Array<GA_Size>& rawOffsets = packedArray.rawOffsets();
    UT_Array<GA_Offset>& rawData = packedArray.rawData();

    if (rawData.size() < pow(2, 12))
    {
        for (GA_Offset elemoff = 0; elemoff < rawData.size(); ++elemoff)
        {
            rawData[elemoff] = geo.pointIndex(rawData[elemoff]);
        }
    }
    else
    {
        UTparallelFor(UT_BlockedRange<GA_Size>(0, rawData.size()), [&geo, &rawData](const UT_BlockedRange<GA_Size>& r) {
            for (GA_Offset elemoff = r.begin(); elemoff != r.end(); ++elemoff)
            {
                rawData[elemoff] = geo.pointIndex(rawData[elemoff]);
            }
        }, 16, 1024);
    }


    //timeEnd = std::chrono::steady_clock::now();
    //diff = timeEnd - timeStart;
    //timeTotal += diff.count();
    //timeStart = std::chrono::steady_clock::now();


    GA_Size classnum = 0;
    for (GA_Size elemoff = 0; elemoff < nelems; ++elemoff)
    {
        if (classnumArray[elemoff] != UNREACHED_NUMBER)
            continue;
        classnumArray[elemoff] = classnum;
        elemHeap.emplace_back(elemoff);
        while (!elemHeap.empty())
        {
            GA_Size lastElem = elemHeap[elemHeap.size() - 1];
            elemHeap.pop_back();
            GA_Size rawOffsetEnd = rawOffsets[lastElem + 1];
            for (GA_Size i = rawOffsets[lastElem]; i < rawOffsetEnd; ++i)
            {
                GA_Offset rawDataVal = rawData[i];
                if (classnumArray[rawDataVal] != UNREACHED_NUMBER)
                    continue;
                classnumArray[rawDataVal] = classnum;
                elemHeap.emplace_back(rawDataVal);
            }
        }
        ++classnum;
    }

    const GA_SplittableRange geoSplittableRange0(geo.getPointRange());
    //const GA_SplittableRange geoSplittableRange0(geo.getPrimitiveRange());
    UTparallelFor(geoSplittableRange0, [&geo, &attrib_h, &classnumArray](const GA_SplittableRange& r) {
        GA_Offset start, end;
        for (GA_Iterator it(r); it.blockAdvance(start, end); )
        {
            for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
            {
                attrib_h.set(elemoff, classnumArray[geo.pointIndex(elemoff)]);
            }
        }
    }, 16, 1024);
    //geo.setAttributeFromArray(attrib_h.getAttribute(), GA_Range(), UT_Array<GA_Size>(classnumArray));



    //timeEnd = std::chrono::steady_clock::now();
    //diff = timeEnd - timeStart;
    //timeTotal1 += diff.count();
    //timeStart = std::chrono::steady_clock::now();

    //geo.setDetailAttributeF("time", timeTotal * 1000);
    //geo.setDetailAttributeF("time1", timeTotal1 * 1000);
}



static void
connectivityPrim(
    const GA_Detail& geo,
    const GA_RWHandleT<GA_Size>& attrib_h,
    const GA_ROHandleT<UT_ValArray<GA_Offset>>& adjElemsAttrib_h,
    const GA_PrimitiveGroup* const geoGroup = nullptr
)
{
    //double timeTotal = 0;
    //double timeTotal1 = 0;
    //auto timeStart = std::chrono::steady_clock::now();
    //auto timeEnd = std::chrono::steady_clock::now();
    //timeStart = std::chrono::steady_clock::now();
    //timeEnd = std::chrono::steady_clock::now();
    //std::chrono::duration<double> diff;

    //::std::vector<::std::vector<GA_Offset>> adjElems;
    GA_Size nelems = geo.getNumPrimitives();

    ::std::vector<GA_Offset> elemHeap;
    elemHeap.reserve(__min(pow(2, 15), nelems));

    ::std::vector<GA_Offset> classnumArray(nelems, UNREACHED_NUMBER);

    UT_PackedArrayOfArrays<GA_Offset> packedArray;
    adjElemsAttrib_h->getAIFNumericArray()->getPackedArrayFromIndices(adjElemsAttrib_h.getAttribute(), 0, nelems, packedArray);

    const UT_Array<GA_Size>& rawOffsets = packedArray.rawOffsets();
    UT_Array<GA_Offset>& rawData = packedArray.rawData();


    if (rawData.size() < pow(2, 12))
    {
        for (GA_Offset elemoff = 0; elemoff < rawData.size(); ++elemoff)
        {
            rawData[elemoff] = geo.primitiveIndex(rawData[elemoff]);
        }
    }
    else
    {
        UTparallelFor(UT_BlockedRange<GA_Size>(0, rawData.size()), [&geo, &rawData](const UT_BlockedRange<GA_Size>& r) {
            for (GA_Offset elemoff = r.begin(); elemoff != r.end(); ++elemoff)
            {
                rawData[elemoff] = geo.primitiveIndex(rawData[elemoff]);
            }
        }, 16, 1024);
    }


    GA_Size classnum = 0;
    for (GA_Size elemoff = 0; elemoff < nelems; ++elemoff)
    {
        if (classnumArray[elemoff] != UNREACHED_NUMBER)
            continue;
        classnumArray[elemoff] = classnum;
        elemHeap.emplace_back(elemoff);
        while (!elemHeap.empty())
        {
            GA_Size lastElem = elemHeap[elemHeap.size() - 1];
            elemHeap.pop_back();
            GA_Size rawOffsetEnd = rawOffsets[lastElem + 1];
            for (GA_Size i = rawOffsets[lastElem]; i < rawOffsetEnd; ++i)
            {
                GA_Offset rawDataVal = rawData[i];
                if (classnumArray[rawDataVal] != UNREACHED_NUMBER)
                    continue;
                classnumArray[rawDataVal] = classnum;
                elemHeap.emplace_back(rawDataVal);
            }
        }
        ++classnum;
    }

    const GA_SplittableRange geoSplittableRange0(geo.getPrimitiveRange());
    //const GA_SplittableRange geoSplittableRange0(geo.getPrimitiveRange());
    UTparallelFor(geoSplittableRange0, [&geo, &attrib_h, &classnumArray](const GA_SplittableRange& r) {
        GA_Offset start, end;
        for (GA_Iterator it(r); it.blockAdvance(start, end); )
        {
            for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
            {
                attrib_h.set(elemoff, classnumArray[geo.primitiveIndex(elemoff)]);
            }
        }
    }, 16, 1024);
    //geo.setAttributeFromArray(attrib_h.getAttribute(), GA_Range(), UT_Array<GA_Size>(classnumArray));
}





//return addAttribConnectivityPoint(geo, name, geoGroup, geoSeamGroup, 0, 0, storage, reuse, subscribeRatio, minGrainSize);

static GA_Attribute*
addAttribConnectivityPoint(
    GA_Detail& geo,
    const GA_PointGroup* const geoGroup = nullptr,
    const GA_PointGroup* const geoSeamGroup = nullptr,
    const GA_Storage storage = GA_STORE_INVALID,
    const UT_StringHolder& name = "__topo_connectivity",
    const UT_Options* const creation_args = nullptr,
    const GA_AttributeOptions* const attribute_options = nullptr,
    const GA_ReuseStrategy& reuse = GA_ReuseStrategy()
    //,const exint subscribeRatio = 32,
    //const exint minGrainSize = 1024
)
{
    GA_Attribute* attribPtr = geo.findPointAttribute(name);
    if (attribPtr)
        return attribPtr;

    const GA_Storage finalStorage = storage == GA_STORE_INVALID ? GFE_Type::getPreferredStorageI(geo) : storage;

    attribPtr = geo.getAttributes().createTupleAttribute(GA_ATTRIB_POINT, GFE_TOPO_SCOPE, name, finalStorage, 1, GA_Defaults(-1), creation_args, attribute_options, reuse);

    const GA_ROHandleT<UT_ValArray<GA_Offset>> adjElemsAttrib_h =
          GFE_Adjacency_Namespace::addAttribPointPointEdge(geo, geoGroup, geoSeamGroup, finalStorage);
    //    GFE_Adjacency_Namespace::addAttribPointPointEdge(geo, geoGroup, geoSeamGroup, finalStorage,
    //        "__topo_nebs", nullptr, nullptr, GA_ReuseStrategy(), subscribeRatio, minGrainSize);

    connectivityPoint(geo, attribPtr, adjElemsAttrib_h, geoGroup);
    return attribPtr;
}









//return addAttribConnectivityPrim(geo, name, geoGroup, geoSeamGroup, 0, 0, storage, reuse, subscribeRatio, minGrainSize);

static GA_Attribute*
addAttribConnectivityPrim(
    GA_Detail& geo,
    const GA_PrimitiveGroup* geoGroup = nullptr,
    const GA_VertexGroup* geoSeamGroup = nullptr,
    const GA_Storage storage = GA_STORE_INVALID,
    const UT_StringHolder& name = "__topo_connectivity",
    const UT_Options* const creation_args = nullptr,
    const GA_AttributeOptions* const attribute_options = nullptr,
    const GA_ReuseStrategy& reuse = GA_ReuseStrategy()
    //,const exint subscribeRatio = 32,
    //const exint minGrainSize = 1024
)
{
    GA_Attribute* attribPtr = geo.findAttribute(GA_ATTRIB_PRIMITIVE, GFE_TOPO_SCOPE, name);
    if (attribPtr)
        return attribPtr;
    const GA_Storage finalStorage = storage == GA_STORE_INVALID ? GFE_Type::getPreferredStorageI(geo) : storage;

    const GA_ROHandleT<UT_ValArray<GA_Offset>> adjElemsAttrib_h =
        GFE_Adjacency_Namespace::addAttribPrimPrimEdge(geo, geoGroup, geoSeamGroup, finalStorage);
    //    GFE_Adjacency_Namespace::addAttribPrimPrimEdge(geo, geoGroup, geoSeamGroup, finalStorage,
    //        "__topo_nebs", nullptr, nullptr, GA_ReuseStrategy(), subscribeRatio, minGrainSize);

    attribPtr = geo.getAttributes().createTupleAttribute(GA_ATTRIB_PRIMITIVE, GFE_TOPO_SCOPE, name, finalStorage, 1, GA_Defaults(-1), creation_args, attribute_options, reuse);

    connectivityPrim(geo, attribPtr, adjElemsAttrib_h, geoGroup);
    return attribPtr;
}



static GA_Attribute*
addAttribConnectivity(
    GA_Detail& geo,
    const GA_Group* geoGroup = nullptr,
    const GA_Group* geoSeamGroup = nullptr,
    const bool connectivityConstraint = false,
    const GA_AttributeOwner attribOwner = GA_ATTRIB_PRIMITIVE,
    const GA_Storage storage = GA_STORE_INVALID,
    const UT_StringHolder& name = "__topo_connectivity"
    //,const exint subscribeRatio = 32,
    //const exint minGrainSize = 1024
)
{
    GA_Attribute* attribPtr = nullptr;
    if (connectivityConstraint)
    {
        if (attribOwner != GA_ATTRIB_PRIMITIVE)
        {
            attribPtr = geo.getAttributes().findAttribute(attribOwner, GFE_TOPO_SCOPE, name);
            if (attribPtr)
                return attribPtr;
        }
        attribPtr = addAttribConnectivityPrim(geo,
            UTverify_cast<const GA_PrimitiveGroup*>(geoGroup),
            UTverify_cast<const GA_VertexGroup*>(geoSeamGroup),
            storage, name
            //,nullptr, nullptr, GA_ReuseStrategy()
            //,subscribeRatio, minGrainSize
            );
        if (attribOwner != GA_ATTRIB_PRIMITIVE)
        {
            //attribPtr = GU_Promote::promote(*geo, attribPtr, attribOwner, true, GU_Promote::GU_PROMOTE_FIRST);
            GA_Attribute* const origAttribPtr = attribPtr;
            attribPtr = GFE_AttributePromote::attribPromote(geo, attribPtr, attribOwner);
            geo.getAttributes().destroyAttribute(origAttribPtr);
        }
    }
    else
    {
        if (attribOwner != GA_ATTRIB_POINT)
        {
            attribPtr = geo.getAttributes().findAttribute(attribOwner, GFE_TOPO_SCOPE, name);
            if (attribPtr)
                return attribPtr;
        }
        attribPtr = addAttribConnectivityPoint(geo,
            UTverify_cast<const GA_PointGroup*>(geoGroup),
            UTverify_cast<const GA_PointGroup*>(geoSeamGroup),
            storage, name
            //,nullptr, nullptr, GA_ReuseStrategy()
            //,subscribeRatio, minGrainSize
            );
        if (attribOwner != GA_ATTRIB_POINT)
        {
            //attribPtr = GU_Promote::promote(*geo, attribPtr, attribOwner, true, GU_Promote::GU_PROMOTE_FIRST);
            GA_Attribute* const origAttribPtr = attribPtr;
            attribPtr = GFE_AttributePromote::attribPromote(geo, attribPtr, attribOwner);
            geo.getAttributes().destroyAttribute(origAttribPtr);
        }
    }
    return attribPtr;
}



} // End of namespace GFE_Connectivity



*/


#endif
