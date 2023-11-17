add_library(NiControllerExtractor STATIC
    NiCommand.cpp
    NiCommand.h
    NiControllerExtractor.cpp
    NiControllerExtractor.h
    NiControllerExtractorLibType.h
)

target_link_libraries(NiControllerExtractor PUBLIC NiParticle)
target_include_directories(NiControllerExtractor PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
set_target_properties(NiControllerExtractor PROPERTIES FOLDER "${GB3_PREFIX}CoreRuntime/ToolLibs")
