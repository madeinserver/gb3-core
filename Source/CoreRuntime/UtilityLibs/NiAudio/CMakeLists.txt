add_library(NiAudio STATIC
    NiAudio.h
    NiAudioLibType.h
    NiAudioListener.cpp
    NiAudioListener.h
    NiAudioMetrics.cpp
    NiAudioMetrics.h
    NiAudioMetrics.inl
    NiAudioSource.cpp
    NiAudioSource.h
    NiAudioSystem.cpp
    NiAudioSystem.h
    NiAudioPCH.h
)

target_link_directories(NiAudio PUBLIC ${MILES_LIB_DIR})
target_link_libraries(NiAudio PUBLIC NiMain)
target_include_directories(NiAudio PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
set_target_properties(NiAudio PROPERTIES FOLDER "${GB3_PREFIX}CoreRuntime/UtilityLibs")
target_precompile_headers(NiAudio PRIVATE NiAudioPCH.h)
