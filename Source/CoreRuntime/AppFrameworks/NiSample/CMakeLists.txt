add_library(NiSample STATIC 
    NiSample.cpp
    NiSample.h
    NiSample.inl
    NiSamplePCH.h
)

target_link_libraries(NiSample PUBLIC NiApplication NiCursor NiUserInterface)
target_include_directories(NiSample PUBLIC ${CMAKE_CURRENT_LIST_DIR})
set_target_properties(NiSample PROPERTIES FOLDER "${GB3_PREFIX}AppFrameworks")
target_precompile_headers(NiSample PRIVATE NiSamplePCH.h)
