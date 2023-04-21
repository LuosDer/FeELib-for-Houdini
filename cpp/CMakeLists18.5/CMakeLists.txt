cmake_minimum_required( VERSION 3.6 )

project( HDK_FeELib_Project )

# CMAKE_PREFIX_PATH must contain the path to the toolkit/cmake subdirectory of
# the Houdini installation. See the "Compiling with CMake" section of the HDK
# documentation for more details, which describes several options for
# specifying this path.
list( APPEND CMAKE_PREFIX_PATH "$ENV{HFS18.5}/toolkit/cmake" )

# Locate Houdini's libraries and header files.
# Registers an imported library target named 'Houdini'.
find_package( Houdini REQUIRED )


function ( houdini_sop_add_library_std )
    cmake_parse_arguments( H_LIBRARY "" "OUTPUT_VAR;API" "FILES" ${ARGN} )

    foreach ( library_name ${H_LIBRARY_FILES} )
    
        # Code generation for the embedded DS file in ${library_name}.C.
        houdini_generate_proto_headers( FILES "../src/SOP/${library_name}/${library_name}.C" )
        
        # Add a library and its source files.
        add_library( ${library_name} SHARED
            "../src/SOP/${library_name}/${library_name}.C"
            "../src/SOP/${library_name}/${library_name}.h"
        )
        
        # Link against the Houdini libraries, and add required include directories and
        # compile definitions.
        target_link_libraries( ${library_name} Houdini )
        
        # Include ${CMAKE_CURRENT_BINARY_DIR} for the generated header.
        target_include_directories( ${library_name} PRIVATE
            ${CMAKE_CURRENT_BINARY_DIR}
            "../include"
        )
        
        # Sets several common target properties, such as the library's output directory.
        houdini_configure_target( ${library_name} )
    
    endforeach ()

endfunction ()




function ( houdini_dm_add_library_std )
    cmake_parse_arguments( H_LIBRARY "" "OUTPUT_VAR;API" "FILES" ${ARGN} )

    foreach ( library_name ${H_LIBRARY_FILES} )
    
        # Add a library and its source files.
        add_library( ${library_name} SHARED
            "../src/DM/${library_name}/${library_name}.C"
        )
        
        # Link against the Houdini libraries, and add required include directories and
        # compile definitions.
        target_link_libraries( ${library_name} Houdini )
        
        # Include ${CMAKE_CURRENT_BINARY_DIR} for the generated header.
        target_include_directories( ${library_name} PRIVATE
            ${CMAKE_CURRENT_BINARY_DIR}
            "../include"
        )
        
        # Sets several common target properties, such as the library's output directory.
        houdini_configure_target( ${library_name} )
    
    endforeach ()

endfunction ()




# file( GLOB HDK_FEELIB_FILE_NAMES LIST_DIRECT false /src/SOP/* )
# aux_source_directory( ../src/SOP/ HDK_FEELIB_FILE_NAMES )

# houdini_sop_add_library_std( FILES ${HDK_FEELIB_FILE_NAMES} )

houdini_sop_add_library_std( FILES
    SOP_FeE_Adjacency_1_0
    SOP_FeE_AttribCast_1_0
    SOP_FeE_AttribCopy_4_0
    #SOP_FeE_AttribCopyFast_1_0
    SOP_FeE_AttribCopyFromSingleElem_1_0
    SOP_FeE_AttribDuplicate_1_0
    SOP_FeE_AttribInterpRay_2_0
    SOP_FeE_AttribPromote_1_0
    SOP_FeE_AttribScale_1_0
    #SOP_FeE_AttribSetToDefault_1_0
    SOP_FeE_Carve_1_0
    SOP_FeE_Connectivity_1_0
    SOP_FeE_ConvertLine_1_0
    SOP_FeE_CurveExpand2d_3_0
    SOP_FeE_CurveSetExpand2d_3_0
    SOP_FeE_CurveUV_1_0
    SOP_FeE_DelAllAttribGroup_1_0
    SOP_FeE_DelAndUnpack_1_0
    SOP_FeE_DelByGroup_1_0
    SOP_FeE_DelElement_1_0
    SOP_FeE_DelVertex_1_0
    SOP_FeE_EdgeGroupTransfer_3_0
    SOP_FeE_Enumerate_1_0
    SOP_FeE_ExtendCurveStraight_1_0
    SOP_FeE_ExtremePrim_1_0
    SOP_FeE_ExtrudeBorder2D_1_0
    SOP_FeE_ExtractPoint_1_0
    SOP_FeE_GroupAttribBoundary_1_0
    SOP_FeE_GroupByMeshWinding_1_0
    SOP_FeE_GroupByPolyWinding_1_0
    SOP_FeE_GroupClosedSharedPoint_2_0
    SOP_FeE_GroupCombine_1_0
    SOP_FeE_GroupCurveEnds_2_0
    SOP_FeE_GroupExpand_1_0
    SOP_FeE_GroupNewEdge_1_0
    SOP_FeE_GroupOneNebPoint_1_0
    SOP_FeE_GroupReverse_1_0
    SOP_FeE_GroupUnshared_1_0
    SOP_FeE_GroupVis_2_0
    SOP_FeE_HasGroup_1_0
    SOP_FeE_JoinCurve_2_0
    SOP_FeE_KuhnMunkres_2_0
    SOP_FeE_LeaveElement_1_0
    SOP_FeE_MatchAxis_1_0
    SOP_FeE_MatchBBox_2_0
    SOP_FeE_MatchDirOrient_1_0
    SOP_FeE_Measure_3_0
    SOP_FeE_OffsetVertexOrder_2_0
    SOP_FeE_OnPoly_1_0
    SOP_FeE_OrientAlongCurve_1_0
    SOP_FeE_Normal2D_1_0
    SOP_FeE_OffsetAttribToIndex_1_0
    SOP_FeE_PointInBBox_2_0
    SOP_FeE_PointInMesh_WN_2_0
    SOP_FeE_PolyCut_3_0
    SOP_FeE_PolyReduce2D_4_0
    SOP_FeE_PrimInlinePoint_Fast_2_0
    SOP_FeE_RestDir2D_1_0
    SOP_FeE_RestVectorComponent_1_0
    SOP_FeE_SetVectorComponent_1_0
    SOP_FeE_Skin_1_0
    SOP_FeE_SkinIntersectCurve_2_0
    SOP_FeE_SkinPiece_4_0
    SOP_FeE_SmallPiece_1_0
    SOP_FeE_SplitPoint_1_0
    SOP_FeE_Triangulate2D_2_0
    SOP_FeE_UVGridify_1_0
    SOP_FeE_UVRectify_2_0
    SOP_FeE_UVScaletoWorldSize_0_5
    SOP_FeE_UVScaletoWorldSize_3_0
    SOP_FeE_WindingNumber_1_0
    SOP_FeE_XformByAttrib_1_0
)


houdini_dm_add_library_std( FILES
    DM_InfoHook
    DM_LightBloomHook
)










