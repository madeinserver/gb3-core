set(SRC 
    egmTerrain.i
    egmTerrainBindings.cpp
    egmTerrainBindings.h
    egmTerrainLibType.h
    egmTerrainPCH.h
    EnvironmentService.cpp
    EnvironmentService.h
    EnvironmentService.inl
    TerrainChangedMessage.cpp
    TerrainChangedMessage.h
    TerrainChangedMessage.inl
    TerrainService.cpp
    TerrainService.h
    TerrainService.inl
    TimeOfDayFile.cpp
    TimeOfDayFile.h
    TimeOfDayFile.inl
    TimeOfDayFileVersion1.cpp
    TimeOfDayFileVersion1.h
    TimeOfDayMessages.cpp
    TimeOfDayMessages.h
    TimeOfDayMessages.inl
    TTimeOfDayEntityFunctor.h
    TTimeOfDayEntityFunctor.inl
)

set(SWIG_SRC
    egmTerrain.i
)

add_library(egmTerrain STATIC ${SRC})
target_precompile_headers(egmTerrain PRIVATE egmTerrainPCH.h)
set_target_properties(egmTerrain PROPERTIES FOLDER "${GB3_PREFIX}GameFramework")
target_link_libraries(egmTerrain PUBLIC NiAnimation egf NiTerrain NiEnvironment)
SwigTarget(egmTerrain ${SWIG_SRC})
