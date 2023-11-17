add_library(NiApplication STATIC 
    NiApplication.cpp
    NiApplication.h
    NiApplication.inl
    NiApplicationMetrics.cpp
    NiApplicationMetrics.h
    NiApplicationPCH.h
    NiCommand.cpp
    NiCommand.h
    NiFrameRate.cpp
    NiFrameRate.h
    NiFrameRate.inl
    NiTurret.cpp
    NiTurret.h

    Win32/NiApplication_Win32.cpp
    Win32/NiAppMain.cpp
    Win32/NiAppWindow.cpp
    Win32/NiAppWindow.h
    Win32/NiAppWindow.inl
)

target_link_libraries(NiApplication PUBLIC NiInput NiVisualTracker NiSettingsDialog)
target_include_directories(NiApplication PUBLIC ${CMAKE_CURRENT_LIST_DIR} ${CMAKE_CURRENT_LIST_DIR}/Win32)
set_target_properties(NiApplication PROPERTIES FOLDER "${GB3_PREFIX}AppFrameworks")
target_precompile_headers(NiApplication PRIVATE NiApplicationPCH.h)
