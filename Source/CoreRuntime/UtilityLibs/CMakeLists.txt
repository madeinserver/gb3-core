set(MOD_DIR "CoreRuntime/UtilityLibs")

add_subdirectory(NiAudio)
add_subdirectory(NiCursor)
add_subdirectory(NiFont)
add_subdirectory(NiInput)
add_subdirectory(NiVisualTracker)
add_subdirectory(NiMetricsOutput)

if (GB3_WIN32)
    add_subdirectory(NiSettingsDialog)
endif()

add_subdirectory(NiUserInterface)
add_subdirectory(NiSceneGraphUpdateService)
add_subdirectory(NiVideo)

add_integrations("CoreRuntime/UtilityLibs")
