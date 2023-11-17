add_library(NSBShaderLib STATIC
    NSBAttributeDesc.cpp
    NSBAttributeDesc.h
    NSBAttributeTable.cpp
    NSBAttributeTable.h
    NSBConstantMap.cpp
    NSBConstantMap.h
    NSBConstantMap.inl
    NSBImplementation.cpp
    NSBImplementation.h
    NSBImplementation.inl
    NSBLoader.cpp
    NSBLoader.h
    NSBObjectTable.cpp
    NSBObjectTable.h
    NSBObjectTable.inl
    NSBPackingDef.cpp
    NSBPackingDef.h
    NSBPackingDef.inl
    NSBPass.cpp
    NSBPass.h
    NSBPass.inl
    NSBRenderStates.cpp
    NSBRenderStates.h
    NSBRequirements.cpp
    NSBRequirements.h
    NSBShader.cpp
    NSBShader.h
    NSBShader.inl
    NSBShaderLibLibType.h
    NSBShaderLibPCH.h
    NSBStageAndSamplerStates.cpp
    NSBStageAndSamplerStates.h
    NSBStateGroup.cpp
    NSBStateGroup.h
    NSBStateGroup.inl
    NSBTexture.cpp
    NSBTexture.h
    NSBTexture.inl
    NSBTextureStage.cpp
    NSBTextureStage.h
    NSBTextureStage.inl
    NSBUserDefinedDataBlock.cpp
    NSBUserDefinedDataBlock.h
    NSBUserDefinedDataBlock.inl
    NSBUserDefinedDataSet.cpp
    NSBUserDefinedDataSet.h
    NSBUtility.cpp
    NSBUtility.h

    Win32/NSBLoader_Win32.cpp
)

target_precompile_headers(NSBShaderLib PRIVATE NSBShaderLibPCH.h)
target_include_directories(NSBShaderLib PUBLIC ${CMAKE_CURRENT_LIST_DIR} ${CMAKE_CURRENT_LIST_DIR}/Win32)
target_link_libraries(NSBShaderLib PUBLIC NiMain)
set_target_properties(NSBShaderLib PROPERTIES FOLDER "${GB3_PREFIX}ShaderLibs")
