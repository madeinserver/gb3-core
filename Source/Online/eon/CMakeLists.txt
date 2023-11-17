set(SRC
    bapiOnline.cpp
    bapiOnline.h
    bapiOnline.i
    DiscoveryRequest.cpp
    DiscoveryRequest.h
    EntityMessage.cpp
    EntityMessage.h
    eonClassIDs.h
    eonLogIDs.h
    eonMessageIDs.h
    eonPCH.h
    eonSDM.cpp
    eonSDM.h
    eonSystemServiceIDs.h
    GroupUpdatePolicy.cpp
    GroupUpdatePolicy.h
    IReplicationGroupPolicy.cpp
    IReplicationGroupPolicy.h
    IReplicationGroupPolicy.inl
    OnlineEntity.h
    OnlineEntityChangeMessage.h
    OnlineEntityManager.cpp
    OnlineEntityManager.h
    OnlineEntry.h
    OnlineEnums.h
    ReplicatingEntity.cpp
    ReplicatingEntity.h
    ReplicationConsumerEntity.cpp
    ReplicationConsumerEntity.h
    ReplicationConsumerEntity.inl
    ReplicationProducerEntity.cpp
    ReplicationProducerEntity.h
    ReplicationProducerEntity.inl
    ReplicationService.cpp
    ReplicationService.h
    ReplicationService.inl
    ServiceAllocator.cpp
    ServiceAllocator.h
    ViewEventMessage.cpp
    ViewEventMessage.h
)

set(SWIG_SRC
    bapiOnline.i
)

add_library(eon STATIC ${SRC})
set_target_properties(eon PROPERTIES FOLDER "${GB3_PREFIX}Online")
target_precompile_headers(eon PRIVATE eonPCH.h)
target_link_libraries(eon PUBLIC egf)
target_include_directories(eon PUBLIC ${CMAKE_CURRENT_LIST_DIR}/..)
SwigTarget(eon ${SWIG_SRC})

