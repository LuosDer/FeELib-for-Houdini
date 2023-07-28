
#pragma once

#ifndef __GFE_VDBActivateByPoint_h__
#define __GFE_VDBActivateByPoint_h__

#include "GFE/GFE_VDBActivateByPoint.h"

#include "GFE/GFE_GeoFilter.h"

//https://github.com/jtomori/VDB_activate_from_points/blob/master/src/vdb_activate_from_points.C

/*
    GFE_VDBActivateByPoint vdbActivateByPoint(geo, cookparms);
    vdbActivateByPoint.findOrCreateTuple(true, GA_ATTRIB_POINT);
    vdbActivateByPoint.compute();
*/
    
class GFE_VDBActivateByPoint : public GFE_AttribFilter {

#define __TEMP_GFE_VDBActivateByPoint_GroupName       "__TEMP_GFE_VDBActivateByPoint_Group"
#define __TEMP_GFE_VDBActivateByPoint_PieceAttribName "__TEMP_GFE_VDBActivateByPoint_PieceAttrib"
#define __TEMP_GFE_VDBActivateByPoint_OutAttribName   "__TEMP_GFE_VDBActivateByPoint_OutAttrib"
    

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
        if (getOutAttribArray().isEmpty())
            return false;

        if (groupParser.isEmpty())
            return true;
            
        // we must lock our inputs before we try to access their geometry, OP_AutoLockInputs will automatically unlock our inputs when we return
        OP_AutoLockInputs inputs(this);
        if (inputs.lock(context) >= UT_ERROR_ABORT)
            return error();

        // duplicate our incoming geometry
        duplicateSource(0, context);

        // check for interrupt - interrupt scope closes automatically when 'progress' is destructed.
        UT_AutoInterrupt progress("Activating voxels...");

        // get pointer to geometry from second input
        const GU_Detail *points = inputGeo(1);

        // check if debug parameter is enabled, DEBUG() function is defined in header file
        if (DEBUG())
        {
            std::cout << "number of points: " << points->getNumPoints() << std::endl;
        }

        GEO_PrimVDB* vdbPrim = NULL; // empty pointer to vdb primitive

        // iterate over all incoming primitives and find the first one which is VDB
        for (GA_Iterator it(gdp->getPrimitiveRange()); !it.atEnd(); it.advance())
        {
            GEO_Primitive* prim = gdp->getGEOPrimitive(it.getOffset());
            if(dynamic_cast<const GEO_PrimVDB *>(prim))
            {
                vdbPrim = dynamic_cast<GEO_PrimVDB *>(prim);
                break;
            }
        }

        // terminate if volume is not VDB
        if(!vdbPrim)
        {
            addError(SOP_MESSAGE, "First input must contain a VDB!");
            return error();
        }

        // volume primitives in different nodes in Houdini by default share the same volume tree (for memory optimization) this will make sure that we will have our own deep copy of volume tree which we can write to 
        vdbPrim->makeGridUnique();
        
        // get grid base pointer and cast it to float grid pointer
        openvdb::GridBase::Ptr vdbPtrBase = vdbPrim->getGridPtr();
        openvdb::FloatGrid::Ptr vdbPtr = openvdb::gridPtrCast<openvdb::FloatGrid>(vdbPtrBase);

        // get accessor to the float grid
        openvdb::FloatGrid::Accessor vdb_access = vdbPtr->getAccessor();

        // get a reference to transformation of the grid
        const openvdb::math::Transform &vdbGridXform = vdbPtr->transform();

        // loop over all the points by handle
        int i = 0;
        GA_ROHandleV3 Phandle(points->findAttribute(GA_ATTRIB_POINT, "P")); // handle to read only attribute
        GA_Offset ptoff;
        GA_FOR_ALL_PTOFF(points, ptoff)
        {
            // test if user requested abort
            if (progress.wasInterrupted())
                return error();

            // get current pont position
            UT_Vector3 Pvalue = Phandle.get(ptoff);

            // create openvdb vector with values from houdini's vector, transform it from world space to vdb's index space (based on vdb's transformation) and activate voxel at point position
            openvdb::Vec3R p_( Pvalue[0], Pvalue[1], Pvalue[2] );
            openvdb::Coord p_xformed( vdbGridXform.worldToIndexCellCentered(p_) );
            vdb_access.setValueOn( p_xformed );
            
            if (DEBUG())
            {
                std::cout << i << ". point world space position: " << Pvalue << std::endl;
                std::cout << "  volmue index space position: " << p_xformed << std::endl;
            }

            i++;
        }

        
        const size_t size = getOutAttribArray().size();
        for (size_t i = 0; i < size; i++)
        {
            enumAttrib = getOutAttribArray()[i];
            // const GA_Storage storage = enumAttrib->getAIFTuple()->getStorage(enumAttrib);
            const GA_AIFTuple* const aifTuple = enumAttrib->getAIFTuple();
            if (aifTuple)
            {
                switch (aifTuple->getStorage(enumAttrib))
                {
                case GA_STORE_INT16:  enumerate<int16>();    break;
                case GA_STORE_INT32:  enumerate<int32>();    break;
                case GA_STORE_INT64:  enumerate<int64>();    break;
                case GA_STORE_REAL16: enumerate<fpreal16>(); break;
                case GA_STORE_REAL32: enumerate<fpreal32>(); break;
                case GA_STORE_REAL64: enumerate<fpreal64>(); break;
                default: break;
                }
            }
            else
            {
                const GA_AIFStringTuple* const aifStrTuple = enumAttrib->getAIFStringTuple();
                if (aifStrTuple)
                {
                    enumerate<UT_StringHolder>();
                }
            }
        }
        geo->forceRenameAttribute(enumAttrib, outAttribName);
        return true;
    }


    void enumerateSideFX()
    {
        enumAttrib = getOutAttribArray()[0];
        
        UT_ASSERT_P(pieceAttrib);
        UT_ASSERT_P(enumAttrib);
        
        const GA_AttributeOwner owner = enumAttrib->getOwner();
        
        UT_ASSERT_MSG(pieceAttrib->getOwner() == owner, "not same owner");

        
        GA_ElementGroup* elemGroup = nullptr;
        const GA_Group* const group = groupParser.getGroup(pieceAttrib);
        if (group)
        {
            if (group->isDetached())
            {
                elemGroup = geo->createElementGroup(pieceAttrib->getOwner(), __TEMP_GFE_VDBActivateByPoint_GroupName);
                elemGroup->combine(group);
                enumParms.setGroup(__TEMP_GFE_VDBActivateByPoint_GroupName);
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
            namedPieceAttrib = GFE_Attribute::clone(geo, pieceAttrib, __TEMP_GFE_VDBActivateByPoint_PieceAttribName);
            enumParms.setPieceAttrib(__TEMP_GFE_VDBActivateByPoint_PieceAttribName);
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
        
        enumParms.setAttribname(__TEMP_GFE_VDBActivateByPoint_OutAttribName);
        // if (enumAttrib->isDetached() || enumAttrib == pieceAttrib)
        // {
        //     //outAttrib = GFE_Attribute::clone(geo, enumAttrib, __TEMP_GFE_VDBActivateByPoint_OutAttribName);
        //     enumParms.setAttribname(__TEMP_GFE_VDBActivateByPoint_OutAttribName);
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
        
        attribCast.getInAttribArray().set(owner, __TEMP_GFE_VDBActivateByPoint_OutAttribName);
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


    


public:
    
private:
    
    
    exint subscribeRatio = 64;
    exint minGrainSize   = 1024;

private:
    
#undef __TEMP_GFE_VDBActivateByPoint_GroupName
#undef __TEMP_GFE_VDBActivateByPoint_PieceAttribName
#undef __TEMP_GFE_VDBActivateByPoint_OutAttribName

    
}; // End of class GFE_VDBActivateByPoint





#endif
