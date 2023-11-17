find_package(OpenEXR CONFIG REQUIRED)

add_library(NiOpenEXRReader STATIC
    NiFileIStream.cpp
    NiFileIStream.h
    NiOpenEXRReader.cpp
    NiOpenEXRReader.h
    NiOpenEXRReaderLibType.h
)

target_link_libraries(NiOpenEXRReader PUBLIC NiMain OpenEXR::OpenEXR)
target_include_directories(NiOpenEXRReader PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
set_target_properties(NiOpenEXRReader PROPERTIES FOLDER "${GB3_PREFIX}CoreRuntime/ToolLibs")
