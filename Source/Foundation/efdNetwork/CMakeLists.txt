add_library(efdNetwork STATIC
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

    Win32/HostInfo_Win32.cpp
    Win32/NetLib_Win32.cpp
    Win32/Net_Win32.h
    Win32/Socket_Win32.cpp
    Win32/Socket_Win32.h
    Win32/Socket_Win32.inl
    Win32/TCPSocket_Win32.cpp
)

target_link_libraries(efdNetwork PUBLIC efd)
set_target_properties(efdNetwork PROPERTIES FOLDER "${GB3_PREFIX}Foundation")
target_precompile_headers(efdNetwork PRIVATE efdNetworkPCH.h)
target_include_directories(efdNetwork PRIVATE ${CMAKE_CURRENT_LIST_DIR})
