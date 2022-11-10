
#pragma once

#ifndef __GEO_FeE_Adjacent_h__
#define __GEO_FeE_Adjacent_h__

//#include <GEO_FeE/GEO_FeE_Adjacent.h>

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


#include <GA/GA_Defaults.h>



namespace GEO_FeE_Adjacent {




    //GEO_FeE_Adjacent::setToDefault(attribHandle)


    static void
    getEdgeAdjacentPolygons(GEO_EdgeAdjArray& adjacencies,
            GA_Offset poly_off) const;
    (

        )
    {

    }


} // End of namespace GEO_FeE_Adjacent

#endif
