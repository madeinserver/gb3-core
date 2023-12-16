set(MOD_GB_LIBS NiInput NiVisualTracker NiMesh)
set(MOD_NAME NiApplication)
set(MOD_PCH NiApplicationPCH.h)
set(MOD_INCLUDES ${CMAKE_CURRENT_LIST_DIR} ${CMAKE_CURRENT_LIST_DIR}/../)

if (WIN32)
	list(APPEND MOD_GB_LIBS NiSettingsDialog)
endif()

generate_project()
