add_library(NiMaterialToolkit SHARED
    NiMaterialHelpers.cpp
    NiMaterialHelpers.h
    NiMaterialToolkit.cpp
    NiMaterialToolkit.h
    NiMaterialToolkitLibType.h
    NiWin32FileFinder.cpp
    NiWin32FileFinder.h
)

target_link_libraries(NiMaterialToolkit PUBLIC NiMainDLL)
target_include_directories(NiMaterialToolkit PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
set_target_properties(NiMaterialToolkit PROPERTIES FOLDER "${GB3_PREFIX}CoreRuntime/ToolLibs")
target_compile_definitions(NiMaterialToolkit PRIVATE -DNIMATERIALTOOLKIT_EXPORT -D_USRDLL INTERFACE -DNIMATERIALTOOLKIT_IMPORT)
