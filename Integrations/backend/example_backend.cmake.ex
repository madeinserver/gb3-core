# This is an example backend for Gamebryo3

#
# This function is used by efd to define the basics of the backend target
#  this can be used to adjust any linking to the backend library
#  or any specific definitions that the backend may need
#
function(add_backend_target_defines target link)
    target_compile_definitions(${target} ${link} -DEE_PLATFORM_EXAMPLE=1)
    target_link_libraries(${target} ${link} exbackend)
endmacro()

#
# This function is executed before projects have been imported and before integrations
#  this is usefull for any extra backend operation
#
function(pre_backend_operations)
    message(STATUS "Running pre-operations...")
endfunction()

#
# This function is executed after projects have been imported
#  this is usefull for any extra backend operation
#
function(post_backend_operations)
    message(STATUS "Running post-operations...")
endfunction()
