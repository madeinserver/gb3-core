add_library(NiVideo STATIC
    NiVideo.h
    NiVideoDecoder.cpp
    NiVideoDecoder.h
    NiVideoDecoder.inl
    NiVideoLibType.h
    NiVideoPCH.h
    NiVideoSDM.cpp
    NiVideoSDM.h

    NiBinkVideo/NiVideoDecoder_Bink.cpp
    NiBinkVideo/NiVideoDecoder_Bink.h
    NiBinkVideo/NiVideo_Bink.h
)

target_link_libraries(NiVideo PUBLIC NiMain blinkw32)
target_include_directories(NiVideo PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/NiBinkVideo)
set_target_properties(NiVideo PROPERTIES FOLDER "${GB3_PREFIX}CoreRuntime/UtilityLibs")
target_precompile_headers(NiVideo PRIVATE NiVideoPCH.h)
