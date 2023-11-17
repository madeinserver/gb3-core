add_library(NiSettingsDialog STATIC
    NiBaseRendererDesc.h
    NiBaseRendererDesc.inl
    NiBaseRendererOptionsView.cpp
    NiBaseRendererOptionsView.h
    NiBaseRendererOptionsView.inl
    NiBaseRendererSetup.cpp
    NiBaseRendererSetup.h
    NiBaseRendererSetup.inl
    NiBaseTabController.h
    NiBaseTabController.inl
    NiRendererSettings.cpp
    NiRendererSettings.h
    NiRendererSettings.inl
    NiRendererTabController.cpp
    NiRendererTabController.h
    NiRendererTabController.inl
    NiSettingsDialog.cpp
    NiSettingsDialog.h
    NiSettingsDialogLibType.h
    NiSettingsDialogPCH.h
    NiSettingsDialogResource.hrc
    NiSettingsDialogResource.rc
)

target_link_libraries(NiSettingsDialog PUBLIC NiMain NiDX9Renderer NiD3D10Renderer ecrBGFXRenderer ecrD3D11Renderer)
target_include_directories(NiSettingsDialog PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
set_target_properties(NiSettingsDialog PROPERTIES FOLDER "${GB3_PREFIX}CoreRuntime/UtilityLibs")
target_precompile_headers(NiSettingsDialog PRIVATE NiSettingsDialogPCH.h)
