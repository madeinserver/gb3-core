add_library(NiVisualTracker STATIC
    Courier16.h
    NiCommonGraphCallbackObjects.cpp
    NiCommonGraphCallbackObjects.h
    NiCourierPixelData.inc
    NiSymbolPixelData.inc
    NiVisualTracker.cpp
    NiVisualTracker.h
    NiVisualTracker.inl
    NiVisualTrackerLibType.h
    NiVisualTrackerPCH.h
    NiVisualTrackerRenderClick.cpp
    NiVisualTrackerRenderClick.h
    NiVisualTrackerRenderClick.inl
)

target_link_libraries(NiVisualTracker PUBLIC NiMesh)
target_include_directories(NiVisualTracker PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
set_target_properties(NiVisualTracker PROPERTIES FOLDER "${GB3_PREFIX}CoreRuntime/UtilityLibs")
target_precompile_headers(NiVisualTracker PRIVATE NiVisualTrackerPCH.h)
