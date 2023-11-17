add_library(NSFParserLib STATIC
    NSFGrammar.cpp
    NSFLexer.cpp
    NSFLexSymbol.h
    NSFLoader.cpp
    NSFLoader.h
    NSFParsedShader.cpp
    NSFParsedShader.h
    NSFParserLib.cpp
    NSFParserLib.h
    NSFParserLibLibType.h
    NSFParserLibPCH.h
    NSFParserLibSDM.cpp
    NSFParserLibSDM.h
    NSFTextFile.cpp
    NSFTextFile.h
    unistd.h

    Win32/NSFLoader_Win32.cpp
)

target_precompile_headers(NSFParserLib PRIVATE NSFParserLibPCH.h)
target_include_directories(NSFParserLib PUBLIC ${CMAKE_CURRENT_LIST_DIR} ${CMAKE_CURRENT_LIST_DIR}/Win32)
target_link_libraries(NSFParserLib PUBLIC NSBShaderLib)
set_target_properties(NSFParserLib PROPERTIES FOLDER "${GB3_PREFIX}ShaderLibs")
