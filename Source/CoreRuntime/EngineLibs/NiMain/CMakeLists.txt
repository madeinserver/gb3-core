if (MSVC) # Fucking MSVC I hate this compiler...
    string(REPLACE "/EHsc" "/EHs-c-" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
endif()

set(MOD_EXTRA_SRC_PATH "DirectionalShadowWriteMaterialNodeLibrary;PointShadowWriteMaterialNodeLibrary;SpotShadowWriteMaterialNodeLibrary;StandardMaterialNodeLibrary;VSMBlurMaterialNodeLibrary")
set(MOD_NAME NiMain)
set(MOD_GB_LIBS NiSystem)
set(MOD_PCH NiMainPCH.h)
set(MOD_INCLUDES
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/DirectionalShadowWriteMaterialNodeLibrary
    ${CMAKE_CURRENT_LIST_DIR}/PointShadowWriteMaterialNodeLibrary
    ${CMAKE_CURRENT_LIST_DIR}/SpotShadowWriteMaterialNodeLibrary
    ${CMAKE_CURRENT_LIST_DIR}/StandardMaterialNodeLibrary
    ${CMAKE_CURRENT_LIST_DIR}/VSMBlurMaterialNodeLibrary
)

generate_project()
