
#pragma once

#ifndef __GFE_VolumeRemap_h__
#define __GFE_VolumeRemap_h__

#include "GFE/GFE_VolumeRemap.h"

#include "GFE/GFE_GeoFilter.h"


/*
    GFE_VolumeRemap volumeRemap(geo, cookparms);
    volumeRemap.findOrCreateTuple(true, GA_ATTRIB_POINT);
    volumeRemap.compute();
*/


#include "GU/GU_PrimVolume.h"
//#include "UT/UT_VoxelArray.h"
    
class GFE_VolumeRemap : public GFE_AttribFilter {

#define __TEMP_GFE_VolumeRemap_GroupName       "__TEMP_GFE_VolumeRemap_Group"
#define __TEMP_GFE_VolumeRemap_PieceAttribName "__TEMP_GFE_VolumeRemap_PieceAttrib"
#define __TEMP_GFE_VolumeRemap_OutAttribName   "__TEMP_GFE_VolumeRemap_OutAttrib"
    

public:
    using GFE_AttribFilter::GFE_AttribFilter;


    void
        setComputeParm(
            const GA_Size firstIndex   = 0,
            const bool negativeIndex   = false,
            const bool outAsOffset     = true,
            const exint subscribeRatio = 64,
            const exint minGrainSize   = 64
        )
    {
        setHasComputed();
        this->firstIndex     = firstIndex;
        this->negativeIndex  = negativeIndex;
        this->outAsOffset    = outAsOffset;
        this->subscribeRatio = subscribeRatio;
        this->minGrainSize   = minGrainSize;
    }

    
private:

    virtual bool
        computeCore() override
    {
        const GFE_Detail* geoSrcTmp = geoSrc ? geoSrc : geo;
        
        volumeoff = geoSrcTmp->getFirstVolumeoff(groupParser.getPrimitiveGroup());
        if (GFE_Type::isInvalidOffset(volumeoff))
        {
            geo->clearElement();
            return true;
        }


        GU_DetailHandle geoTemp_h;
        //GU_Detail* geoTemp;
        if (geoSrc)
        {
            const GA_Size numpt = geo->getNumPoints();
            if (numpt < numVoxel)
            {
                geo->appendPointBlock(numVoxel - numpt);
            }
            else
            {
                const GA_IndexMap& indexMap = geo->getPointMap();
                //if (indexMap.isTrivialMap())
                //{
                //}
                bool flag = false;
                GA_Offset start, end;
                for (GA_Iterator it(GA_Range(indexMap, nullptr)); it.fullBlockAdvance(start, end); )
                {
                    if (flag)
                    {
                        for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                        {
                            geo->deletePointOffsetOnly(elemoff);
                        }
                    }
                    
                    if (indexMap.indexFromOffset(end-1) < numVoxel)
                        continue;

                    UT_ASSERT_P(GFE_Type::isValidOffset(indexMap, numVoxel));
                    
                    flag = true;
                    for (GA_Offset elemoff = indexMap.offsetFromIndex(numVoxel); elemoff < end; ++elemoff)
                    {
                        geo->deletePointOffsetOnly(elemoff);
                    }
                    //if (indexMap.indexFromOffset(start) >= numVoxel)
                    //{
                    //    flag = true;
                    //    for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                    //    {
                    //        geo->deletePointOffsetOnly(elemoff);
                    //    }
                    //}
                    //else
                    //{
                    //    for (GA_Offset elemoff = indexMap.offsetFromIndex(numVoxel); elemoff < end; ++elemoff)
                    //    {
                    //        geo->deletePointOffsetOnly(elemoff);
                    //    }
                    //}
                }
            }
        }
        else
        {
            geoTemp_h = GFE_DetailBase::newDetail(geo);
            geoSrcTmp = reinterpret_cast<GA_Detail*>(geoTemp_h.gdpNC());
            
            geo->clear();
            geo->appendPointBlock(numVoxel);
        }
        UT_ASSERT_P(geo->getNumPoints() == numVoxel);
        
        
        
        GU_PrimVolume volume(geoSrcTmp, volumeoff);

        
        GA_Size numVoxel;
        int rx, ry, rz;
        if (geoSrcTmp->isVDB(volumeoff))
        {
        }
        else
        {
            volume.getRes(rx, ry, rz);
            numVoxel = rx * ry * rz;
        }
        
        UT_VoxelArrayWriteHandleF vox_wh = volume.getVoxelWriteHandle();

        // Resize the array.
        vox_wh->size(rx, ry, rz);

        for (int z = 0; z < rz; z++)
        {
            for (int y = 0; y < ry; y++)
            {
                for (int x = 0; x < rx; x++)
                {
                    float v = 1;
                    vox_wh->setValue(x, y, z, v);
                }
            }
        }

        
        UT_VoxelArrayF* vox = vox_wh->getValues();
        int x, y, z;
        float total = 0.0;
        for (z = 0; z < vox->getZRes(); z++)
            for (y = 0; y < vox->getYRes(); y++)
                for (x = 0; x < vox->getXRes(); x++)
                    total += (*vox)(x, y, z);

        
        return true;
    }


    void enumerateSideFX()
    {
        enumAttrib = getOutAttribArray()[0];
        
        UT_ASSERT_P(pieceAttrib);
        UT_ASSERT_P(enumAttrib);
        
        const GA_AttributeOwner owner = enumAttrib->getOwner();


        
        GA_Offset start, end;
        for (GA_Iterator it(pit.begin()); it.blockAdvance(start, end); )
        {
            attrib_ph.setPage(start);
            //const GA_Offset baseOff = start - GAgetPageOff(start);
            for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
            {
                attrib_ph.value(elemoff) = firstIndex + elemoff;
            }
        }

        
        GA_ElementGroup* elemGroup = nullptr;
        const GA_Group* const group = groupParser.getGroup(pieceAttrib);
        if (group)
        {
            if (group->isDetached())
            {
                elemGroup = geo->createElementGroup(pieceAttrib->getOwner(), __TEMP_GFE_VolumeRemap_GroupName);
                elemGroup->combine(group);
                enumParms.setGroup(__TEMP_GFE_VolumeRemap_GroupName);
            }
            else
            {
                enumParms.setGroup(group->getName());
            }
        }
        else
        {
            enumParms.setGroup("");
        }

        GA_Attribute* namedPieceAttrib = nullptr;
        if (pieceAttrib->isDetached())
        {
            namedPieceAttrib = GFE_Attribute::clone(geo, pieceAttrib, __TEMP_GFE_VolumeRemap_PieceAttribName);
            enumParms.setPieceAttrib(__TEMP_GFE_VolumeRemap_PieceAttribName);
            //namedPieceAttrib->bumpDataId();
        }
        else
        {
            enumParms.setPieceAttrib(pieceAttrib->getName());
        }

        SOP_EnumerateEnums::GroupType enumGroupType;
        switch (owner)
        {
        case GA_ATTRIB_PRIMITIVE: enumGroupType = SOP_EnumerateEnums::GroupType::PRIMITIVE; break;
        case GA_ATTRIB_POINT:     enumGroupType = SOP_EnumerateEnums::GroupType::POINT;     break;
        case GA_ATTRIB_VERTEX:    enumGroupType = SOP_EnumerateEnums::GroupType::VERTEX;    break;
        default: UT_ASSERT_P("unhandled enumAttrib class"); break;
        }
        enumParms.setGroupType(enumGroupType);
        enumParms.setUsePieceAttrib(true);
        enumParms.setPieceMode(enumeratePieceElem ? SOP_EnumerateEnums::PieceMode::ELEMENTS : SOP_EnumerateEnums::PieceMode::PIECES);

        //GA_Attribute* outAttrib = nullptr;
        //if (enumAttrib->isDetached() || GFE_Type::stringEqual(enumAttrib->getName(), pieceAttrib->getName()))
        
        enumParms.setAttribname(__TEMP_GFE_VolumeRemap_OutAttribName);
        // if (enumAttrib->isDetached() || enumAttrib == pieceAttrib)
        // {
        //     //outAttrib = GFE_Attribute::clone(geo, enumAttrib, __TEMP_GFE_VolumeRemap_OutAttribName);
        //     enumParms.setAttribname(__TEMP_GFE_VolumeRemap_OutAttribName);
        // }
        // else
        // {
        //     enumParms.setAttribname(enumAttrib->getName());
        // }

        
        //enumParms.setAttribType(SOP_EnumerateEnums::AttribType::INT);
        //enumParms.setPrefix("");
        
        destgdh.allocateAndSet(geo->asGU_Detail(), false);
        
        inputgdh.clear();
        GU_DetailHandle input_h;
        if (geoSrc)
        {
            GU_Detail* const geoSrcGU = new GU_Detail;
            geoSrcGU->replaceWith(*geoSrc);
            input_h.allocateAndSet(geoSrcGU);
        }
        else
        {
            input_h.allocateAndSet(geo->asGU_Detail(), false);
        }
        inputgdh.emplace_back(input_h);
        
        SOP_NodeCache* const nodeCache = enumVerb->allocCache();
        const auto enumCookparms = GFE_NodeVerb::newCookParms(cookparms, enumParms, nodeCache, &destgdh, &inputgdh);
        
        GFE_AttribCast attribCast(geo, cookparms);
        attribCast.newStorageClass = enumAttrib->getStorageClass();
        attribCast.newPrecision    = GFE_Attribute::getPrecision(enumAttrib);
        attribCast.newAttribNames  = enumAttrib->isDetached() ? "" : outAttribName;
        
        enumVerb->cook(enumCookparms);
        
        attribCast.getInAttribArray().set(owner, __TEMP_GFE_VolumeRemap_OutAttribName);
        if (attribCast.newStorageClass == GA_STORECLASS_STRING)
        {
            attribCast.prefix = prefix;
            attribCast.sufix  = sufix;
        }
        attribCast.compute();

        geo->destroyElementGroup(elemGroup);
        geo->destroyAttrib(namedPieceAttrib);
        
        getOutAttribArray().set(attribCast.getOutAttribArray());
    }


    
    template<typename T>
    void enumerate()
    {
        UT_ASSERT_P(enumAttrib);
        const GA_SplittableRange geoSplittableRange0(groupParser.getRange(enumAttrib));
        if(outAsOffset)
        {
            UTparallelFor(geoSplittableRange0, [this](const GA_SplittableRange& r)
            {
                GA_PageHandleT<T, T, true, true, GA_Attribute, GA_ATINumeric, GA_Detail> attrib_ph(enumAttrib);
                for (GA_PageIterator pit = r.beginPages(); !pit.atEnd(); ++pit)
                {
                    GA_Offset start, end;
                    for (GA_Iterator it(pit.begin()); it.blockAdvance(start, end); )
                    {
                        attrib_ph.setPage(start);
                        //const GA_Offset baseOff = start - GAgetPageOff(start);
                        for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                        {
                            attrib_ph.value(elemoff) = firstIndex + elemoff;
                        }
                    }
                }
            }, subscribeRatio, minGrainSize);
        }
        else
        {
            const GA_IndexMap& indexMap = enumAttrib->getIndexMap();
            if (negativeIndex)
            {
                UTparallelFor(geoSplittableRange0, [this, &indexMap](const GA_SplittableRange& r)
                {
                    GA_PageHandleT<T, T, true, true, GA_Attribute, GA_ATINumeric, GA_Detail> attrib_ph(enumAttrib);
                    for (GA_PageIterator pit = r.beginPages(); !pit.atEnd(); ++pit)
                    {
                        GA_Offset start, end;
                        for (GA_Iterator it(pit.begin()); it.blockAdvance(start, end); )
                        {
                            attrib_ph.setPage(start);
                            for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                            {
                                attrib_ph.value(elemoff) = firstIndex - indexMap.indexFromOffset(elemoff);
                            }
                        }
                    }
                }, subscribeRatio, minGrainSize);
            }
            else
            {
                UTparallelFor(geoSplittableRange0, [this, &indexMap](const GA_SplittableRange& r)
                {
                    GA_PageHandleT<T, T, true, true, GA_Attribute, GA_ATINumeric, GA_Detail> attrib_ph(enumAttrib);
                    for (GA_PageIterator pit = r.beginPages(); !pit.atEnd(); ++pit)
                    {
                        GA_Offset start, end;
                        for (GA_Iterator it(pit.begin()); it.blockAdvance(start, end); )
                        {
                            attrib_ph.setPage(start);
                            for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                            {
                                attrib_ph.value(elemoff) = firstIndex + indexMap.indexFromOffset(elemoff);
                            }
                        }
                    }
                }, subscribeRatio, minGrainSize);
            }
        }
    }


    template<>
    void enumerate<UT_StringHolder>()
    {
        UT_ASSERT_P(enumAttrib);

        const GA_RWHandleS attrib_h(enumAttrib);
        const GA_SplittableRange geoSplittableRange0(groupParser.getRange(enumAttrib->getOwner()));
        if(outAsOffset)
        {
            UTparallelFor(geoSplittableRange0, [this, &attrib_h](const GA_SplittableRange& r)
            {
                GA_Offset start, end;
                for (GA_Iterator it(r); it.blockAdvance(start, end); )
                {
                    for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                    {
                        char buffer[100];
                        sprintf(buffer, "%s%I64d%s", prefix, firstIndex+elemoff, sufix);
                        attrib_h.set(elemoff, buffer);
                    }
                }
            }, subscribeRatio, minGrainSize);
        }
        else
        {
            const GA_IndexMap& indexMap = enumAttrib->getIndexMap();
            if(negativeIndex)
            {
                UTparallelFor(geoSplittableRange0, [this, &attrib_h, &indexMap](const GA_SplittableRange& r)
                {
                    GA_Offset start, end;
                    for (GA_Iterator it(r); it.blockAdvance(start, end); )
                    {
                        for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                        {
                            char buffer[100];
                            sprintf(buffer, "%s%I64d%s", prefix, firstIndex-indexMap.indexFromOffset(elemoff), sufix);
                            attrib_h.set(elemoff, buffer);
                        }
                    }
                }, subscribeRatio, minGrainSize);
            }
            else
            {
                UTparallelFor(geoSplittableRange0, [this, &attrib_h, &indexMap](const GA_SplittableRange& r)
                {
                    GA_Offset start, end;
                    for (GA_Iterator it(r); it.blockAdvance(start, end); )
                    {
                        for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                        {
                            char buffer[100];
                            sprintf(buffer, "%s%I64d%s", prefix, firstIndex+indexMap.indexFromOffset(elemoff), sufix);
                            attrib_h.set(elemoff, buffer);
                        }
                    }
                }, subscribeRatio, minGrainSize);
            }
        }
    }

    


public:

    GA_Offset volumeoff = GFE_INVALID_OFFSET;
    bool enumeratePieceElem = false;
    
    
private:
    const char* outAttribName = nullptr;
    GA_Attribute* enumAttrib;
    
    
    exint subscribeRatio = 64;
    exint minGrainSize   = 1024;


#undef __TEMP_GFE_VolumeRemap_GroupName
#undef __TEMP_GFE_VolumeRemap_PieceAttribName
#undef __TEMP_GFE_VolumeRemap_OutAttribName

    
}; // End of class GFE_VolumeRemap





#endif
