add_library(NiViewMath STATIC
    NiViewMath.cpp
    NiViewMath.h
    NiViewMathLibType.h
)

target_link_libraries(NiViewMath PUBLIC NiMain)
target_include_directories(NiViewMath PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
set_target_properties(NiViewMath PROPERTIES FOLDER "${GB3_PREFIX}CoreRuntime/ToolLibs")
