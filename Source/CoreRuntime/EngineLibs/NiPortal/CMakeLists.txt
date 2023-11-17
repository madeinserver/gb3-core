add_library(NiPortal STATIC
    NiFixtureCuller.cpp
    NiFixtureCuller.h
    NiPortal.cpp
    NiPortal.h
    NiPortal.inl
    NiPortalLibType.h
    NiPortalMetrics.cpp
    NiPortalMetrics.h
    NiPortalPCH.h
    NiPortalSDM.cpp
    NiPortalSDM.h
    NiRoom.cpp
    NiRoom.h
    NiRoom.inl
    NiRoomGroup.cpp
    NiRoomGroup.h
    NiRoomGroup.inl
)

target_include_directories(NiPortal PUBLIC ${CMAKE_CURRENT_LIST_DIR})
set_target_properties(NiPortal PROPERTIES FOLDER "${GB3_PREFIX}CoreRuntime/EngineLibs")
target_precompile_headers(NiPortal PRIVATE NiPortalPCH.h)
target_link_libraries(NiPortal PUBLIC NiMesh)
