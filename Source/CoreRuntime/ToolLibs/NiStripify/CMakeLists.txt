add_library(NiStripify STATIC
    NiStripify.cpp
    NiStripify.h
    NiStripifyInterface.cpp
    NiStripifyInterface.h
    NiStripifyInterface.inl
    NiStripifyLibType.h
    NvTriStrip.cpp
    NvTriStrip.h
    NvTriStripObjects.cpp
    NvTriStripObjects.h
    VertexCache.cpp
    VertexCache.h
)

target_link_libraries(NiStripify PUBLIC NiMesh NiPortal)
target_include_directories(NiStripify PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
set_target_properties(NiStripify PROPERTIES FOLDER "${GB3_PREFIX}CoreRuntime/ToolLibs")
