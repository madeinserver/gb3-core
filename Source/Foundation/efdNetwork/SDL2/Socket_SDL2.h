// EMERGENT GAME TECHNOLOGIES PROPRIETARY INFORMATION
//
// This software is supplied under the terms of a license agreement or
// nondisclosure agreement with Emergent Game Technologies and may not 
// be copied or disclosed except in accordance with the terms of that 
// agreement.
//
//      Copyright (c) 2022-2023 Arves100/Made In Server Developers.
//      Copyright (c) 1996-2009 Emergent Game Technologies.
//      All Rights Reserved.
//
// Emergent Game Technologies, Calabasas, CA 91302
// http://www.emergent.net

#pragma once
#ifndef EE_SOCKET_SDL2_H
#define EE_SOCKET_SDL2_H

#define EE_SOCKET_ERROR(expression) (expression == SOCKET_ERROR)

#define EE_SOCKET_INVALID(expression) (expression == INVALID_SOCKET)

#ifdef EE_PLATFORM_WIN32
#define EE_SOCKET_CLOSE(expression) closesocket(expression)
#else
#define EE_SOCKET_CLOSE(expression) close(expression)
#endif

#define EE_SOCKET_LINGER(expression) static_cast<u_short>(expression)

#ifndef EAGAIN
#define EAGAIN WSAEWOULDBLOCK
#endif
#ifndef EINPROGRESS
#define EINPROGRESS WSAEWOULDBLOCK
#endif
#ifndef EALREADY
#define EALREADY WSAEALREADY
#endif
#ifndef EISCONN
#define EISCONN WSAEISCONN
#endif
#ifndef ECONNRESET
#define ECONNRESET WSAECONNRESET
#endif
#ifndef EINVAL
#define EINVAL WSAEINVAL
#endif

#include <efd/Metrics.h>
#include EE_PLATFORM_SPECIFIC_INCLUDE(efdNetwork,Socket,inl)

#endif // EE_SOCKET_WIN32_H