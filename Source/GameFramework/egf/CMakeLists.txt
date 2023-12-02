set(MOD_NAME egf)
set(MOD_PCH egfPCH.h)
set(MOD_INCLUDES ${CMAKE_CURRENT_LIST_DIR}/..)
set(MOD_SWIG TRUE)
set(MOD_GB_LIBS NiMain)

generate_project()
