set(SRC 
    ActorMessages.cpp
    ActorMessages.h
    ActorMessages.inl
    ActorModel.cpp
    ActorModel.h
    AnimationService.cpp
    AnimationService.h
    AnimationService.inl
    egmAnimation.i
    egmAnimationBindings.cpp
    egmAnimationBindings.h
    egmAnimationLibType.h
    egmAnimationPCH.h
    KFMCache.cpp
    KFMCache.h
    KFMCache.inl
    KFMCacheResponse.cpp
    KFMCacheResponse.h
    KFMCacheResponse.inl
    KFMFactory.cpp
    KFMFactory.h
    KFMFactoryRequest.cpp
    KFMFactoryRequest.h
    KFMFactoryResponse.cpp
    KFMFactoryResponse.h
)

set(SWIG_SRC egmAnimation.i)

add_library(egmAnimation STATIC ${SRC})
target_precompile_headers(egmAnimation PRIVATE egmAnimationPCH.h)
set_target_properties(egmAnimation PROPERTIES FOLDER "${GB3_PREFIX}GameFramework")
target_link_libraries(egmAnimation PUBLIC egf NiFloodgate NiMesh NiAnimation)
SwigTarget(egmAnimation ${SWIG_SRC})
