set(MOD_INCLUDES
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/NiFragmentOperationsNodeLibrary
    ${CMAKE_CURRENT_SOURCE_DIR}/NiTerrainMaterialNodeLibrary
    ${CMAKE_CURRENT_SOURCE_DIR}/NiWaterMaterialNodeLibrary
)
set(MOD_EXTRA_SRC_PATH
    NiFragmentOperationsNodeLibrary
    NiTerrainMaterialNodeLibrary
    NiWaterMaterialNodeLibrary
)

set(MOD_NAME NiTerrain)
set(MOD_GB_LIBS NiCollision)
set(MOD_PCH NiTerrainPCH.h)

generate_project()
