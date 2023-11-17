add_library(NiMeshProfileXMLParser STATIC
    NiMeshProfile.cpp
    NiMeshProfile.h
    NiMeshProfile.inl
    NiMeshProfileComponent.cpp
    NiMeshProfileComponent.h
    NiMeshProfileComponent.inl
    NiMeshProfileElement.cpp
    NiMeshProfileElement.h
    NiMeshProfileElement.inl
    NiMeshProfileGroup.cpp
    NiMeshProfileGroup.h
    NiMeshProfileGroup.inl
    NiMeshProfileStream.cpp
    NiMeshProfileStream.h
    NiMeshProfileStream.inl
    NiMeshProfileXMLParser.cpp
    NiMeshProfileXMLParser.h
    NiMeshProfileXMLParserLibType.h
)

target_link_libraries(NiMeshProfileXMLParser PUBLIC NiMeshProfileProcessor)
target_include_directories(NiMeshProfileXMLParser PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
set_target_properties(NiMeshProfileXMLParser PROPERTIES FOLDER "${GB3_PREFIX}CoreRuntime/ToolLibs")
