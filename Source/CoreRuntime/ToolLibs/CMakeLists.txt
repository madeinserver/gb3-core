set(MOD_DIR "CoreRuntime/ToolLibs")

add_subdirectory(NiPNGReader)
add_subdirectory(NiAnimationCompression)
add_subdirectory(NiControllerExtractor)
add_subdirectory(NiDevImageQuantizer)

if (FBXSDK_FOUND)
    add_subdirectory(NiFBXConverter)
endif()

add_subdirectory(NiHDRReader)

if (GB3_BUILD_TOOLS)
    add_subdirectory(NiMaterialToolkit) 
    add_subdirectory(NiPluginToolkit)
    add_subdirectory(NiProgramLauncher)
    add_subdirectory(NiStandardSharedData)
endif()

add_subdirectory(NiMeshTools)
add_subdirectory(NiNBTGenerator)
add_subdirectory(NiOpenEXRReader)
add_subdirectory(NiMeshProfileProcessor)
add_subdirectory(NimeshProfileXMLParser)
add_subdirectory(NiOptimization)
add_subdirectory(NiViewMath)
add_subdirectory(NiStripify)

add_integrations("CoreRuntime/ToolLibs")
