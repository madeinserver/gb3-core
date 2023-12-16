set(MOD_NAME ecr)
set(MOD_PCH ecrPCH.h)
set(MOD_SWIG TRUE)
set(MOD_GB_LIBS NiMain egf NiMesh NiParticle NiCollision NiApplication)
set(MOD_INCLUDES ${CMAKE_CURRENT_LIST_DIR}/..)
set(MOD_P_INCLUDES ${CMAKE_CURRENT_LIST_DIR})

generate_project()
