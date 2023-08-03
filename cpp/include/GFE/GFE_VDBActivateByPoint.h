
#pragma once

#ifndef __GFE_VDBActivateByPoint_h__
#define __GFE_VDBActivateByPoint_h__

#include "GFE/GFE_VDBActivateByPoint.h"

#include "GFE/GFE_GeoFilter.h"



#include "GU/GU_PrimVDB.h"
#include "openvdb/openvdb.h"

//#include "openvdb/tools/ChangeBackground.h"
//#include "openvdb/tools/Interpolation.h"



//https://github.com/jtomori/VDB_activate_from_points/blob/master/src/vdb_activate_from_points.C

/*
    GFE_VDBActivateByPoint vdbActivateByPoint(geo, cookparms);
    vdbActivateByPoint.findOrCreateTuple(true, GA_ATTRIB_POINT);
    vdbActivateByPoint.compute();
*/
    
class GFE_VDBActivateByPoint : public GFE_AttribFilterWithRef0 {


public:
    using GFE_AttribFilterWithRef0::GFE_AttribFilterWithRef0;


    void
        setComputeParm(
            const exint subscribeRatio = 64,
            const exint minGrainSize   = 1024
        )
    {
        setHasComputed();
        this->subscribeRatio = subscribeRatio;
        this->minGrainSize   = minGrainSize;
    }

private:

    virtual bool
        computeCore() override
    {
        if (groupParser.isEmpty())
            return true;
            
        // we must lock our inputs before we try to access their geometry, OP_AutoLockInputs will automatically unlock our inputs when we return
        //OP_AutoLockInputs inputs(this);
        //if (inputs.lock(context) >= UT_ERROR_ABORT)
        //    return error();
        
        // check for interrupt - interrupt scope closes automatically when 'progress' is destructed.
        UT_AutoInterrupt progress("Activating voxels...");


#if GFE_DEBUG_MODE
        GA_Size numpt_debug = geoRef0->getNumPoints();
#endif

        const GA_Offset vdboff = geo->getFirstVDBoff(groupParser.getPrimitiveGroup());
        if (GFE_Type::isInvalidOffset(vdboff))
        {
            cookparms->sopAddError(SOP_MESSAGE, "First input must contain a VDB!");
            return false;
        }
        GEO_PrimVDB* const vdbPrim = geo->getVDBPrimitive(vdboff);
        UT_ASSERT_P(vdbPrim);

        // volume primitives in different nodes in Houdini by default share the same volume tree (for memory optimization) this will make sure that we will have our own deep copy of volume tree which we can write to 
        vdbPrim->makeGridUnique();
        
        // get grid base pointer and cast it to float grid pointer
        openvdb::GridBase::Ptr vdbPtrBase = vdbPrim->getGridPtr();
        openvdb::FloatGrid::Ptr vdbPtr = openvdb::gridPtrCast<openvdb::FloatGrid>(vdbPtrBase);

        // get accessor to the float grid
        openvdb::FloatGrid::Accessor vdb_access = vdbPtr->getAccessor();

        // get a reference to transformation of the grid
        const openvdb::math::Transform& vdbGridXform = vdbPtr->transform();

        GFE_ROPageHandleT<UT_Vector3D> posRef0_ph(geoRef0->getP());
        GA_Offset start, end;
        for (GA_PageIterator pit = groupParserRef0.getPointSplittableRange().beginPages(); !pit.atEnd(); ++pit)
        {
            for (GA_Iterator it(pit.begin()); it.blockAdvance(start, end); )
            {
                posRef0_ph.setPage(start);
                for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                {
                    UT_Vector3D& pos = posRef0_ph.value(elemoff);
                    
                    // create openvdb vector with values from houdini's vector, transform it from world space to vdb's index space (based on vdb's transformation) and activate voxel at point position
                    const openvdb::Vec3d vdbpos(pos[0], pos[1], pos[2]);
                    const openvdb::Coord coord(vdbGridXform.worldToIndexCellCentered(vdbpos));
                    vdb_access.setValueOn(coord);
                }
            }
        }
        return true;
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
