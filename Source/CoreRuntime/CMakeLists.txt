add_subdirectory(EngineLibs)
add_subdirectory(ToolLibs)
add_subdirectory(UtilityLibs)
add_subdirectory(AppFrameworks)
add_subdirectory(ShaderLibs)

if (GB3_ENABLE_PHYSX)
    add_subdirectory(PhysXLibs)
endif()
