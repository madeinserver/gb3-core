set(MOD_NAME NiFloodgate)
set(MOD_GB_LIBS NiMain)
set(MOD_INCLUDES ${CMAKE_CURRENT_LIST_DIR} ${CMAKE_CURRENT_LIST_DIR}/Kernels/NiNopKernel)
set(MOD_EXTRA_SRC_PATH Kernels/NiNopKernel)
set(MOD_PCH NiFloodgatePCH.h)

generate_project()
