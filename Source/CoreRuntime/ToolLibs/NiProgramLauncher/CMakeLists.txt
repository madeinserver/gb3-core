add_library(NiProgramLauncher SHARED
    NiOutputWindow.cpp
    NiOutputWindow.h
    NiOutputWindow.inl
    NiProgramLauncher.cpp
    NiProgramLauncher.h
    NiProgramLauncher.rc
    NiProgramLauncherLibType.h
    NiProgramLauncherPCH.h
    NiProgramLauncherRes.h
    NiProgramRunner.cpp
    NiProgramRunner.h
    NiProgramRunnerFactory.h
    NiWinProgramRunner.cpp
    NiWinProgramRunner.h
    NiWinProgramRunnerFactory.cpp
    NiWinProgramRunnerFactory.h
)

target_link_libraries(NiProgramLauncher PUBLIC NiPluginToolkit)
target_include_directories(NiProgramLauncher PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
set_target_properties(NiProgramLauncher PROPERTIES FOLDER "${GB3_PREFIX}CoreRuntime/ToolLibs")
target_precompile_headers(NiProgramLauncher PRIVATE NiProgramLauncherPCH.h)
target_compile_definitions(NiProgramLauncher PRIVATE -DNIPROGRAMLAUNCHER_EXPORT -D_USRDLL INTERFACE NIPROGRAMLAUNCHER_IMPORT)
