set(SRC 
    egmWhiteboxLibType.h
    egmWhiteboxPCH.h
    WhiteboxService.cpp
    WhiteboxService.h
)

add_library(egmWhitebox STATIC ${SRC})
target_precompile_headers(egmWhitebox PRIVATE egmWhiteboxPCH.h)
set_target_properties(egmWhitebox PROPERTIES FOLDER "${GB3_PREFIX}GameFramework")
target_link_libraries(egmWhitebox PUBLIC egf ecr)
