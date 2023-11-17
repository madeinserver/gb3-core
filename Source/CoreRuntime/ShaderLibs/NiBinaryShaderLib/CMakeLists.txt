add_library(NiBinaryShaderLib STATIC
    NiBinaryShader.cpp
    NiBinaryShader.h
    NiBinaryShader.inl
    NiBinaryShaderLib.cpp
    NiBinaryShaderLib.h
    NiBinaryShaderLibLibType.h
    NiBinaryShaderLibPCH.h
    NiBinaryShaderLibrary.cpp
    NiBinaryShaderLibrary.h
    NiBinaryShaderLibSDM.cpp
    NiBinaryShaderLibSDM.h
    NSBShaderLib.h

    Win32/NiBinaryShaderLibrary_DX9.cpp
)

target_include_directories(NiBinaryShaderLib PUBLIC ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(NiBinaryShaderLib PUBLIC NiDX9Renderer NSBShaderLib)
set_target_properties(NiBinaryShaderLib PROPERTIES FOLDER "${GB3_PREFIX}ShaderLibs")
target_precompile_headers(NiBinaryShaderLib PRIVATE NiBinaryShaderLibPCH.h)
