add_library(NiDevImageQuantizer STATIC
    AlphaMapUtility.cpp
    AlphaMapUtility.h
    AlphaMapUtility.inl
    NiDevImageQuantizer.cpp
    NiDevImageQuantizer.h
    NiDevImageQuantizerLibType.h
    QuantizeUtil.cpp
    QuantizeUtil.h
    QuantizeUtil.inl
)

target_link_libraries(NiDevImageQuantizer PUBLIC NiDX9Renderer)
target_include_directories(NiDevImageQuantizer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
set_target_properties(NiDevImageQuantizer PROPERTIES FOLDER "${GB3_PREFIX}CoreRuntime/ToolLibs")
