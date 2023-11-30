set(SRC
    BridgeService.cpp
    BridgeService.h
    ChannelManagerConfig.h
    ChannelManagerConfig.inl
    ChannelManagerService.cpp
    ChannelManagerService.h
    ConnectionTCP.cpp
    ConnectionTCP.h
    ConnectionTCP.inl
    efdNetworkLibType.h
    efdNetworkPCH.h
    efdNetworkSDM.cpp
    efdNetworkSDM.h
    HostInfo.h
    IConnection.cpp
    IConnection.h
    INetLib.cpp
    INetLib.h
    INetLib.inl
    INetTransport.h
    NameResolutionMessage.cpp
    NameResolutionMessage.h
    NameResolutionMessage.inl
    NameResolutionService.cpp
    NameResolutionService.h
    Net.h
    NetCategory.h
    NetErrorMessage.cpp
    NetErrorMessage.h
    NetErrorMessage.inl
    NetLib.cpp
    NetLib.h
    NetListener.h
    NetListener.inl
    NetService.cpp
    NetService.h
    NetService.inl
    NetTransportTCP.cpp
    NetTransportTCP.h
    NetVersion.cpp
    NetVersion.h
    NetworkMessages.cpp
    NetworkMessages.h
    NetworkMessages.inl
    Socket.cpp
    Socket.h
    SocketErrors.h
    TCPSocket.h
    TransportFactory.cpp
    TransportFactory.h
)


set(SDL2_SRC
    SDL2/Net_SDL2.h
    SDL2/HostInfo_SDL2.cpp
    SDL2/Socket_SDL2.h
    SDL2/Socket_SDL2.inl
    SDL2/TCPSocket_SDL2.cpp
    SDL2/NetLib_SDL2.cpp
)


source_group("SDL2" FILES ${SDL2_SRC})
add_library(efdNetwork STATIC ${SDL2_SRC} ${SRC})
set_target_properties(efdNetwork PROPERTIES FOLDER "${GB3_PREFIX}Foundation")
target_precompile_headers(efdNetwork PRIVATE efdNetworkPCH.h)
target_include_directories(efdNetwork PRIVATE ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(efdNetwork PUBLIC efd)
