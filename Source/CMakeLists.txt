add_subdirectory(3rdParty)
add_subdirectory(Foundation)
add_subdirectory(CoreRuntime)
add_subdirectory(GameFramework)
add_subdirectory(Online)

if (GB3_ENABLE_UNIT_TESTS)
    add_subdirectory(UnitTests)
endif()
