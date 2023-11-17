add_library(efdLogService STATIC
    efdLogServiceLibType.h
    efdLogServicePCH.h
    LogService.cpp
    LogService.h
    LogServiceMessages.cpp
    LogServiceMessages.h
    NetDestination.cpp
    NetDestination.h
)

target_link_libraries(efdLogService PUBLIC efd)
set_target_properties(efdLogService PROPERTIES FOLDER "${GB3_PREFIX}Foundation")
target_precompile_headers(efdLogService PRIVATE efdLogServicePCH.h)
