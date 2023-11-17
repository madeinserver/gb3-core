add_library(NiOptimization STATIC
    NiOptimization.h
    NiOptimizationLibType.h
    NiOptimize.cpp
    NiOptimize.h
    NiOptimizeABV.cpp
    NiOptimizeABV.h
    NiOptimizeAnimation.cpp
    NiOptimizeAnimation.h
    NiOptimizeWeldSkin.cpp
    NiOptimize_HelpFunc.cpp
    NiOptimize_HelpFunc.inl
    NiOptimize_OptFunc.cpp
    NiOptimize_OptFunc.inl
)

target_link_libraries(NiOptimization PUBLIC NiMeshTools NiCollision NiParticle NiPortal)
target_include_directories(NiOptimization PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
set_target_properties(NiOptimization PROPERTIES FOLDER "${GB3_PREFIX}CoreRuntime/ToolLibs")
