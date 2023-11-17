set(SRC
    AttachNifData.cpp
    AttachNifData.h
    AttachNifData.inl
    bapiCoreRuntime.cpp
    bapiCoreRuntime.h
    bapiCoreRuntime.i
    CameraData.cpp
    CameraData.h
    CameraService.cpp
    CameraService.h
    CameraService.inl
    CMakeLists.txt
    CoreRuntimeMessages.cpp
    CoreRuntimeMessages.h
    CoreRuntimeMessages.inl
    CoreRuntimePropertyTypes.cpp
    CoreRuntimePropertyTypes.h
    CoreRuntimePropertyTypes.inl
    ecrClassIDs.h
    ecrLibType.h
    ecrMessageIDs.h
    ecrPCH.h
    ecrPropertyIDs.h
    ecrSystemServiceIDs.h
    IAttachedObjectsProperty.cpp
    IAttachedObjectsProperty.h
    IDefaultSurfaceCreator.h
    IRenderServiceDelegate.cpp
    IRenderServiceDelegate.h
    LightService.cpp
    LightService.h
    LightService.inl
    MeshModel.cpp
    MeshModel.h
    NIFFactory.cpp
    NIFFactory.h
    NIFFactory.inl
    NIFFactoryResponse.cpp
    NIFFactoryResponse.h
    NIFFactoryResponse.inl
    PickService.cpp
    PickService.h
    PickService.inl
    RenderableModel.cpp
    RenderableModel.h
    RenderableModel.inl
    RenderContext.cpp
    RenderContext.h
    RenderContext.inl
    RenderService.cpp
    RenderService.h
    RenderService.inl
    RenderSurface.cpp
    RenderSurface.h
    RenderSurface.inl
    RenderSurfaceStep.cpp
    RenderSurfaceStep.h
    SceneGraphCache.cpp
    SceneGraphCache.h
    SceneGraphCache.inl
    SceneGraphCacheResponse.cpp
    SceneGraphCacheResponse.h
    SceneGraphCacheResponse.inl
    SceneGraphService.cpp
    SceneGraphService.h
    SceneGraphService.inl
    ServiceAllocator.cpp
    ServiceAllocator.h
    ShaderService.cpp
    ShaderService.h
    SnapshotGenerator.cpp
    SnapshotGenerator.h
    TextureRapidIterationService.cpp
    TextureRapidIterationService.h

    Win32/RenderService_Win32.cpp
)

set(SWIG_SRC
    bapiCoreRuntime.i
)

add_library(ecr STATIC ${SRC})
set_target_properties(ecr PROPERTIES FOLDER "${GB3_PREFIX}GameFramework")
target_precompile_headers(ecr PRIVATE ecrPCH.h)
target_link_libraries(ecr PUBLIC NiMain egf NiMesh NiParticle NiCollision NiDX9Renderer NiD3D10Renderer ecrD3D11Renderer NiApplication)
target_include_directories(ecr PRIVATE ${CMAKE_CURRENT_LIST_DIR})
SwigTarget(ecr ${SWIG_SRC})
