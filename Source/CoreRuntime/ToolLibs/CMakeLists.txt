set(MOD_DIR "CoreRuntime/ToolLibs")

add_subdirectory(NiPNGReader)
add_subdirectory(NiAnimationCompression)
add_subdirectory(NiControllerExtractor)
add_subdirectory(NiDevImageQuantizer)
add_subdirectory(NiHDRReader)
add_subdirectory(NiMeshTools)
add_subdirectory(NiNBTGenerator)
add_subdirectory(NiOpenEXRReader)
add_subdirectory(NiMeshProfileProcessor)
add_subdirectory(NimeshProfileXMLParser)
add_subdirectory(NiOptimization)
add_subdirectory(NiViewMath)
add_subdirectory(NiStripify)

# DLL only
add_subdirectory(NiMaterialToolkit) 
add_subdirectory(NiPluginToolkit)
add_subdirectory(NiProgramLauncher)
add_subdirectory(NiStandardSharedData)


add_integrations("CoreRuntime/ToolLibs")
