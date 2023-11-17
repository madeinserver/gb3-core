add_library(NiNBTGenerator STATIC
    NiNBTGenerator.cpp
    NiNBTGenerator.h
    NiNBTGeneratorLibType.h

    ATI/NmFileIO.cpp
    ATI/NmFileIO.h
)

target_link_libraries(NiNBTGenerator PUBLIC NiMeshTools)
target_include_directories(NiNBTGenerator PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/ATI)
set_target_properties(NiNBTGenerator PROPERTIES FOLDER "${GB3_PREFIX}CoreRuntime/ToolLibs")
