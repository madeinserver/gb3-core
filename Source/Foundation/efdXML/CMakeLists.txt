set(MOD_INCLUDES ${CMAKE_CURRENT_LIST_DIR}/..)
set(MOD_NAME efdXML)
set(MOD_DEFINES
    # Debug configuration
    $<$<CONFIG:DEBUG>:EE_CONFIG_DEBUG>;
    $<$<CONFIG:DEBUG>:EE_EFD_CONFIG_DEBUG>;
    $<$<CONFIG:DEBUG>:EE_USE_ASSERT_STACKTRACE>;
    $<$<CONFIG:DEBUG>:EE_USE_MEMORY_MANAGEMENT>;
    # Release configuration (QA)
    $<$<CONFIG:RELEASE>:EE_CONFIG_RELEASE>;
    $<$<CONFIG:RELEASE>:EE_EFD_CONFIG_RELEASE>;
    # Shipping configuration
    $<$<CONFIG:SHIPPING>:EE_CONFIG_SHIPPING>;
    $<$<CONFIG:SHIPPING>:EE_EFD_CONFIG_SHIPPING>;
    $<$<CONFIG:SHIPPING>:EE_DISABLE_LOGGING>;
)

generate_project()

# custom add of backend target defines
add_backend_target_defines(efdXML PUBLIC)
