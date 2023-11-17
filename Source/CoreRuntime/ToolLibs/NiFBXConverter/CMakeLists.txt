add_library(NiFBXConverter STATIC
    NiFBXConverter.cpp
    NiFBXConverter.h
    NiFBXConverterLibType.h
    NiFBXExporter.cpp
    NiFBXExporter.h
    NiFBXUtility.h
    NiFBXUtility.inl
    NiMeshFBXExporter.cpp
    NiMeshFBXExporter.h
)

target_link_libraries(NiFBXConverter PUBLIC NiMain)
target_link_libraries(NiFBXConverter PUBLIC OPTIMIZED ${FBKSDK_LIBRARY})
target_link_libraries(NiFBXConverter PUBLIC DEBUG ${FBKSDK_LIBRARY_DEBUG})
target_include_directories(NiFBXConverter PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${FBXSDK_INCLUDE_DIR})
set_target_properties(NiFBXConverter PROPERTIES FOLDER "${GB3_PREFIX}CoreRuntime/ToolLibs")
