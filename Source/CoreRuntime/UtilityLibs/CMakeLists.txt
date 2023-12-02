set(MOD_DIR "CoreRuntime/UtilityLibs")

add_subdirectory(NiAudio)
add_subdirectory(NiCursor)
add_subdirectory(NiFont)
add_subdirectory(NiInput)
add_subdirectory(NiVisualTracker)
add_subdirectory(NiMetricsOutput)
add_subdirectory(NiSettingsDialog)
add_subdirectory(NiUserInterface)
add_subdirectory(NiSceneGraphUpdateService)
add_subdirectory(NiVideo)

add_integrations("CoreRuntime/UtilityLibs")
