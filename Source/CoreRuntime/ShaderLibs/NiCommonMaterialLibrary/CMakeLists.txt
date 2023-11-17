add_library(NiCommonMaterialLibrary STATIC
    AdditiveLightMapsMaterial.cpp
    AdditiveLightMapsMaterial.h
    NiCommonMaterialLib.cpp
    NiCommonMaterialLib.h
    NiCommonMaterialLibPCH.h
    NiCommonMaterialLibrary.cpp
    NiCommonMaterialLibrary.h
    NiCommonMaterialLibType.h
    NiCommonMaterialSDM.cpp
    NiCommonMaterialSDM.h
    NiFlatWireframeMaterial.cpp
    NiFlatWireframeMaterial.h
    TextureBlendMaterial.cpp
    TextureBlendMaterial.h
)

target_include_directories(NiCommonMaterialLibrary PUBLIC ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(NiCommonMaterialLibrary PUBLIC NiMesh)
set_target_properties(NiCommonMaterialLibrary PROPERTIES FOLDER "${GB3_PREFIX}ShaderLibs")
target_precompile_headers(NiCommonMaterialLibrary PRIVATE NiCommonMaterialLibPCH.h)
