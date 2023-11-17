add_library(NiMeshTools STATIC
    NiMeshTools.cpp
    NiMeshTools.h
    NiMeshTools.inl
    NiMeshToolsLibType.h
    NiSkinningUtilities.cpp
    NiSkinningUtilities.h
    NiToolPipelineCloneHelper.cpp
    NiToolPipelineCloneHelper.h
)

target_link_libraries(NiMeshTools PUBLIC NiMesh)
target_include_directories(NiMeshTools PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
set_target_properties(NiMeshTools PROPERTIES FOLDER "${GB3_PREFIX}CoreRuntime/ToolLibs")
