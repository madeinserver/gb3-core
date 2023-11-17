add_library(NiAnimationCompression STATIC
    NiAnimationCompression.cpp
    NiAnimationCompression.h
    NiAnimationCompressionLibType.h
)

target_link_libraries(NiAnimationCompression PUBLIC NiAnimation)
target_include_directories(NiAnimationCompression PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
set_target_properties(NiAnimationCompression PROPERTIES FOLDER "${GB3_PREFIX}CoreRuntime/ToolLibs")
