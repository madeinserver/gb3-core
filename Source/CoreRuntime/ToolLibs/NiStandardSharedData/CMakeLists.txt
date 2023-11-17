add_library(NiStandardSharedData SHARED
    NiControllerExtractorSharedData.cpp
    NiControllerExtractorSharedData.h
    NiExporterOptionsSharedData.cpp
    NiExporterOptionsSharedData.h
    NiExtraObjectsSharedData.cpp
    NiExtraObjectsSharedData.h
    NiPhysicsSharedData.cpp
    NiPhysicsSharedData.h
    NiRendererSharedData.cpp
    NiRendererSharedData.h
    NiSceneGraphSharedData.cpp
    NiSceneGraphSharedData.h
    NiStandardSharedData.h
    NiStandardSharedDataLibType.h
    NiTimerSharedData.cpp
    NiTimerSharedData.h
)

target_link_libraries(NiStandardSharedData PUBLIC NiPluginToolkit)
target_include_directories(NiStandardSharedData PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
set_target_properties(NiStandardSharedData PROPERTIES FOLDER "${GB3_PREFIX}CoreRuntime/ToolLibs")
target_compile_definitions(NiStandardSharedData PRIVATE -DNISTANDARDSHAREDDATA_EXPORT -D_USRDLL)
