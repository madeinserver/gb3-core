set(SRC 
    AttenuationPropertyVisualizer.cpp
    AttenuationPropertyVisualizer.h
    ConnectionPropertyVisualizer.cpp
    ConnectionPropertyVisualizer.h
    CubePropertyVisualizer.cpp
    CubePropertyVisualizer.h
    egmVisualizersLibType.h
    egmVisualizersPCH.h
    EntitySplinePropertyVisualizer.cpp
    EntitySplinePropertyVisualizer.h
    FrustumPropertyVisualizer.cpp
    FrustumPropertyVisualizer.h
    IPropertyVisualizer.h
    PhysXBoxPropertyVisualizer.cpp
    PhysXBoxPropertyVisualizer.h
    PointCloudVolumeVisualizer.cpp
    PointCloudVolumeVisualizer.h
    PointPropertyVisualizer.cpp
    PointPropertyVisualizer.h
    PropertyVisualizationHelpers.cpp
    PropertyVisualizationHelpers.h
    PropertyVisualizationHelpers.inl
    PropertyVisualizationService.cpp
    PropertyVisualizationService.h
    PropertyVisualizerFactory.cpp
    PropertyVisualizerFactory.h
    ProxyService.cpp
    ProxyService.h
    RadiusPropertyVisualizer.cpp
    RadiusPropertyVisualizer.h
    SpherePropertyVisualizer.cpp
    SpherePropertyVisualizer.h
    SplinePropertyVisualizer.cpp
    SplinePropertyVisualizer.h
    VisualizerConnectionTracker.cpp
    VisualizerConnectionTracker.h
    VisualizerVisibilityTracker.cpp
    VisualizerVisibilityTracker.h
)

add_library(egmVisualizers STATIC ${SRC})
target_precompile_headers(egmVisualizers PRIVATE egmVisualizersPCH.h)
set_target_properties(egmVisualizers PROPERTIES FOLDER "${GB3_PREFIX}GameFramework")
target_link_libraries(egmVisualizers PUBLIC egf NiMesh NiParticle NiCollision ecr)
