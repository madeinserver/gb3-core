# Copyright (C) 2022-2023 Arves100/Made In Server Development team
#
# How integrations works:
#  Add a custom cmake file inside Integrations, this file has to 
#   define the following functions:
#   - bootstrap_integration_(filename)(result_variable)
#   - integration_(filename)_add_(directoryname)()
#  Example for integration Sample.cmake:
#   - bootstrap_integration_sample(result_variable)
#   - integration_sample_add_gameframework()
#

# All the integrations found
set(GB3_INTEGRATIONS )
# All the integrations requested
set(GB3_INTEGRATIONS_WANTED )
# All the integrations that were enabled
set(GB3_INTEGRATIONS_ENABLED )
# All the integrations that were enabled sorted by name
set(GB3_INTEGRATIONS_ENABLED_BY_NAME )

function(bootstrap_integrations)
    file(GLOB INTEGRATIONS "${GB3_INTEGRATION_ROOT}/*.cmake")

    foreach(INT_NAME_RAW IN ITEMS ${INTEGRATIONS})
        string(REPLACE "${GB3_INTEGRATION_ROOT}/" "" INT_NAME ${INT_NAME_RAW})
        string(REPLACE ".cmake" "" INT_NAME ${INT_NAME})
        string(TOUPPER ${INT_NAME} INT_NAME_UPPER)

        list(APPEND GB3_INTEGRATIONS ${INT_NAME})

        if (GB3_ENABLE_INTEGRATION_${INT_NAME_UPPER})
            list(APPEND GB3_INTEGRATIONS_WANTED 1)

            include(${INT_NAME}) # this will import the integration

            string(TOLOWER ${INT_NAME} INT_NAME_LOW)
            if (COMMAND bootstrap_integration_${INT_NAME_LOW})
                # this will bootstrap the integration
                cmake_language(CALL "bootstrap_integration_${INT_NAME_LOW}" INT_RESULT)

                if (NOT DEFINED INT_RESULT)
                    message(WARNING "Integration ${INT_NAME} didn't return a proper return code\nAssuming it failed...")
                    set(INT_RESULT 0)
                endif()

                if ("${INT_RESULT}" STREQUAL "1")
                    list(APPEND GB3_INTEGRATIONS_ENABLED 1)
                    list(APPEND GB3_INTEGRATIONS_ENABLED_BY_NAME ${INT_NAME})
                else()
                    list(APPEND GB3_INTEGRATIONS_ENABLED 0)
                endif()
            else()
                message(WARNING "Integration ${INT_NAME} is invalid...")
            endif()
        else()
            list(APPEND GB3_INTEGRATIONS_WANTED 0)
            list(APPEND GB3_INTEGRATIONS_ENABLED 0)
        endif()

        set(GB3_INTEGRATIONS "${GB3_INTEGRATIONS}" PARENT_SCOPE)
        set(GB3_INTEGRATIONS_ENABLED_BY_NAME "${GB3_INTEGRATIONS_ENABLED_BY_NAME}" PARENT_SCOPE)
        set(GB3_INTEGRATIONS_WANTED "${GB3_INTEGRATIONS_WANTED}" PARENT_SCOPE)
        set(GB3_INTEGRATIONS_ENABLED "${GB3_INTEGRATIONS_ENABLED}" PARENT_SCOPE)
    endforeach()
endfunction()

function(add_integrations dir_name)
    string(TOLOWER ${dir_name} DIR_NAME_LOW)
    string(REPLACE "/" "_" DIR_NAME_LOW ${DIR_NAME_LOW})

    foreach(INT_NAME IN ITEMS ${GB3_INTEGRATIONS_ENABLED_BY_NAME})
        if (COMMAND integration_${INT_NAME}_add_${DIR_NAME_LOW})
            cmake_language(CALL "integration_${INT_NAME}_add_${DIR_NAME_LOW}")
        endif()
    endforeach()
endfunction()

function(have_integration integration_name output_var)
    string(TOUPPER ${integration_name} INTEGRATION_NAME_UP)

    list(FIND GB3_INTEGRATIONS_ENABLED_BY_NAME ${INTEGRATION_NAME_UP} IS_INTEGRATION_FOUND)
    if ("${IS_INTEGRATION_FOUND}" STREQUAL "-1")
        set(${output_var} 0 PARENT_SCOPE)
    else()
        set(${output_var} TRUE PARENT_SCOPE)
    endif()
endfunction()

function(print_integrations)
    foreach (INT IN ITEMS ${GB3_INTEGRATIONS})
        list(POP_FRONT GB3_INTEGRATIONS_WANTED IS_INT_WANTED)
        list(POP_FRONT GB3_INTEGRATIONS_ENABLED IS_INT_ENABLED)

        if ("${IS_INT_ENABLED}" STREQUAL "1")
            set(INT_ENABLING "Yes")
        else()
            set(INT_ENABLING "No")
        endif()

        if ("${IS_INT_WANTED}" STREQUAL "1")
            set(INT_WANTED "Yes")
        else()
            set(INT_WANTED "No")
        endif()

        message(STATUS "- ${INT}: ${INT_ENABLING} (Wanted: ${INT_WANTED})")
    endforeach()
endfunction()
