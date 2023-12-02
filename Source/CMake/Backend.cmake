# This file allows bootstrapping of custom backends
#

function(bootstrap_backends)
    # Attempt to bootstrap the selected backend

    if ("${GB3_TARGET_BACKEND}" STREQUAL "SDL2")
        pre_backend_operations()
        return() # no need to bootstrap the backend if it's the default one
    endif()

    if (NOT EXISTS "${GB3_ROOT}/Integrations/backend/${GB3_TARGET_BACKEND}/.cmake")
        message(FATAL_ERROR "Backend configuration \"${GB3_TARGET_BACKEND}\" not found!")
    endif()

    include(backend/${GB3_TARGET_BACKEND})

    if (NOT EXISTS "${GB3_BACKEND_ROOT}")
        message(FATAL_ERROR "Cannot find backend root")
    endif()

    pre_backend_operations()
endfunction()

if ("${GB3_TARGET_BACKEND}" STREQUAL "SDL2")
    # default backend
    
    function(add_backend_target_defines target link) # used in efd
        target_compile_definitions(${target} ${link} -DEE_PLATFORM_SDL2=1)
        target_link_libraries(${target} ${link} $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>)
    endfunction()

    function(post_backend_operations)
    endfunction()

    function(pre_backend_operations)
    endfunction()
endif()
