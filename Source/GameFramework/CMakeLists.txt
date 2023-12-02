set(MOD_DIR "GameFramework")

add_subdirectory(egf)
add_subdirectory(ecr)
add_subdirectory(ecrInput)
add_subdirectory(egmAnimation)
add_subdirectory(egmTerrain)
add_subdirectory(egmVisualizers)
add_subdirectory(egmVisualTracker)
add_subdirectory(egmWhitebox)
add_subdirectory(egmToolServices)

add_integrations("GameFramework")
