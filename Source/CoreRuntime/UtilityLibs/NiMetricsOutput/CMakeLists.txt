add_library(NiMetricsOutput STATIC
    NiCalculatingOutput.cpp
    NiCalculatingOutput.h
    NiCalculatingOutput.inl
    NiMetricsOutput.h
    NiMetricsOutputLibType.h
    NiMetricsOutputPCH.h
    NiVisualTrackerOutput.cpp
    NiVisualTrackerOutput.h
    NiXMLMetricsOutput.cpp
    NiXMLMetricsOutput.h
)

target_link_libraries(NiMetricsOutput PUBLIC NiVisualTracker)
target_include_directories(NiMetricsOutput PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
set_target_properties(NiMetricsOutput PROPERTIES FOLDER "${GB3_PREFIX}CoreRuntime/UtilityLibs")
target_precompile_headers(NiMetricsOutput PRIVATE NiMetricsOutputPCH.h)
