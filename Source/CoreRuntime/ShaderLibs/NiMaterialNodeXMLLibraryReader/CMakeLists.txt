add_library(NiMaterialNodeXMLLibraryReader STATIC
    NiMaterialNodeXMLLibraryReader.cpp
    NiMaterialNodeXMLLibraryReader.h
    NiMaterialNodeXMLLibraryReaderLibType.h
)

target_include_directories(NiMaterialNodeXMLLibraryReader PUBLIC ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(NiMaterialNodeXMLLibraryReader PUBLIC NiMain)
set_target_properties(NiMaterialNodeXMLLibraryReader PROPERTIES FOLDER "${GB3_PREFIX}ShaderLibs")
