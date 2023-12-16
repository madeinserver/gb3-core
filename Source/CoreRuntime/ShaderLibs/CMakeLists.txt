set(MOD_DIR "CoreRuntime/ShaderLibs")

add_subdirectory(NSBShaderLib)
add_subdirectory(NSFParserLib)
add_subdirectory(NiCommonMaterialLibrary)
add_subdirectory(NiMaterialNodeXMLLibraryReader)

add_integrations("CoreRuntime/ShaderLibs")
