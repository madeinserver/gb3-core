set(MOD_NAME NiEnvironment)
set(MOD_GB_LIBS NiAnimation)
set(MOD_EXTRA_SRC_PATH
    NiAtmosphereMaterialNodeLibrary
    NiSkyMaterialNodeLibrary
)
set(MOD_INCLUDES
${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/NiAtmosphereMaterialNodeLibrary
    ${CMAKE_CURRENT_LIST_DIR}/NiSkyMaterialNodeLibrary
)

set(MOD_PCH NiEnvironmentPCH.h)

generate_project()
