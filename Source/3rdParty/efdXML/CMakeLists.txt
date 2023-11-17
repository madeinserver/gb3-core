add_library(efdXML STATIC 
    tinystr.h
    tinystr.cpp
    tinyxml.h
    tinyxml.cpp
    tinyxmlerror.cpp
    tinyxmlparser.cpp
)
set_target_properties(efdXML PROPERTIES FOLDER "${GB3_PREFIX}3rdParty")
target_include_directories(efdXML PUBLIC ${CMAKE_CURRENT_LIST_DIR}/../)
target_link_libraries(efdXML efdBase)
