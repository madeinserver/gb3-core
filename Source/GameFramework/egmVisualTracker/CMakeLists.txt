set(SRC 
    egmVisualTrackerLibType.h
    egmVisualTrackerPCH.h
    VisualTrackerService.cpp
    VisualTrackerService.h
    VisualTrackerService.inl
)

add_library(egmVisualTracker STATIC ${SRC})
target_precompile_headers(egmVisualTracker PRIVATE egmVisualTrackerPCH.h)
set_target_properties(egmVisualTracker PROPERTIES FOLDER "${GB3_PREFIX}GameFramework")
target_link_libraries(egmVisualTracker PUBLIC egf ecr NiCollision)
