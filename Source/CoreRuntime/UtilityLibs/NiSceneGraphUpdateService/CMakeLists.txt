add_library(NiSceneGraphUpdateService STATIC
    NiEditorSceneGraphUpdate.cpp
    NiEditorSceneGraphUpdate.h
    NiEditorSceneGraphUpdate.inl
    NiSceneGraphUpdate.cpp
    NiSceneGraphUpdate.h
    NiSceneGraphUpdate.inl
    NiSceneGraphUpdateMessage.cpp
    NiSceneGraphUpdateMessage.h
    NiSceneGraphUpdateMessage.inl
    NiSceneGraphUpdateObject.cpp
    NiSceneGraphUpdateObject.h
    NiSceneGraphUpdateObject.inl
    NiSceneGraphUpdateObjectId.cpp
    NiSceneGraphUpdateObjectId.h
    NiSceneGraphUpdateObjectId.inl
    NiSceneGraphUpdateService.cpp
    NiSceneGraphUpdateService.h
    NiSceneGraphUpdateService.inl
    NiSceneGraphUpdateServiceLibType.h
    NiSceneGraphUpdateServicePCH.h
    NiSceneGraphUpdateSettings.cpp
    NiSceneGraphUpdateSettings.h
    NiSceneGraphUpdateSettings.inl
    NiSceneGraphUpdateStream.cpp
    NiSceneGraphUpdateStream.h
    NiViewerSceneGraphUpdate.cpp
    NiViewerSceneGraphUpdate.h
    NiViewerSceneGraphUpdate.inl
)

target_link_libraries(NiSceneGraphUpdateService PUBLIC NiParticle)
target_include_directories(NiSceneGraphUpdateService PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
set_target_properties(NiSceneGraphUpdateService PROPERTIES FOLDER "${GB3_PREFIX}CoreRuntime/UtilityLibs")
target_precompile_headers(NiSceneGraphUpdateService PRIVATE NiSceneGraphUpdateServicePCH.h)
