add_library(NiEnvironment STATIC
    NiAtmosphere.cpp
    NiAtmosphere.h
    NiAtmosphere.inl
    NiCubeMapRenderStep.cpp
    NiCubeMapRenderStep.h
    NiCubeMapRenderStep.inl
    NiEnvironment.cpp
    NiEnvironment.h
    NiEnvironment.inl
    NiEnvironmentLib.h
    NiEnvironmentLibType.h
    NiEnvironmentPCH.h
    NiSky.cpp
    NiSky.h
    NiSky.inl
    NiSkyBlendStage.cpp
    NiSkyBlendStage.h
    NiSkyBlendStage.inl
    NiSkyDome.cpp
    NiSkyDome.h
    NiSkyDome.inl
    NiSkyMaterial.cpp
    NiSkyMaterial.h
    NiSkyMaterialDescriptor.h
    NiSkyMaterialPixelDescriptor.cpp
    NiSkyMaterialPixelDescriptor.h
    NiSkyMaterialVertexDescriptor.cpp
    NiSkyMaterialVertexDescriptor.h
    NiSkyRenderView.cpp
    NiSkyRenderView.h
    NiTimeOfDay.cpp
    NiTimeOfDay.h
    NiTimeOfDay.inl
    NiTimeOfDayController.h
    NiTimeOfDayController.inl
    NiTTimeOfDayDefaultFunctor.h
    NiTTimeOfDayDefaultFunctor.inl

    NiSkyMaterialNodeLibrary/NiSkyMaterialNodeLibrary.cpp
    NiSkyMaterialNodeLibrary/NiSkyMaterialNodeLibrary.h
    NiAtmosphereMaterialNodeLibrary/NiAtmosphereMaterialNodeLibrary.h
    NiAtmosphereMaterialNodeLibrary/NiAtmosphereMaterialNodeLibrary.cpp
)

target_link_libraries(NiEnvironment PUBLIC NiAnimation)
target_include_directories(NiEnvironment PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/NiAtmosphereMaterialNodeLibrary
    ${CMAKE_CURRENT_LIST_DIR}/NiSkyMaterialNodeLibrary
)
set_target_properties(NiEnvironment PROPERTIES FOLDER "${GB3_PREFIX}CoreRuntime/EngineLibs")
target_precompile_headers(NiEnvironment PRIVATE NiEnvironmentPCH.h)
