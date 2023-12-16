// EMERGENT GAME TECHNOLOGIES PROPRIETARY INFORMATION
//
// This software is supplied under the terms of a license agreement or
// nondisclosure agreement with Emergent Game Technologies and may not
// be copied or disclosed except in accordance with the terms of that
// agreement.
//
//      Copyright (c) 2022 Arves100/Made In Server Developers.
//      Copyright (c) 1996-2009 Emergent Game Technologies.
//      All Rights Reserved.
//
// Emergent Game Technologies, Calabasas, CA 91302
// http://www.emergent.net

#pragma once
#ifndef EE_UNIVERSALTYPES_SDL2_H
#define EE_UNIVERSALTYPES_SDL2_H

#include <efd/SDL2/RTLib_SDL2.h>

/** @file UniversalTypes_SDL2.h
    Define our Universal types in terms of platform specific types
*/

namespace efd
{
/// @name UniversalTypes
/// Define our Universal types in terms of platform specific types. 
//@{

typedef bool                Bool;

typedef char                Char;

#ifdef EE_PLATFORM_WIN32
typedef unsigned short      WChar;
#else
typedef unsigned int        WChar;
#endif

typedef int8_t              SInt8;
typedef uint8_t             UInt8;

#define EE_SINT8_MAX        (127)
#define EE_UINT8_MAX        (255)

typedef int16_t             SInt16;
typedef uint16_t    UInt16;

#define EE_SINT16_MAX       (32767)
#define EE_UINT16_MAX       (65535)

typedef int32_t             SInt32;
typedef uint32_t    UInt32;

#define EE_SINT32_MAX       (2147483647)
#define EE_UINT32_MAX       (4294967295)

typedef int64_t             SInt64;
typedef uint64_t    UInt64;

#define EE_SINT64_MAX       (9223372036854775807ll)
#define EE_UINT64_MAX       (18446744073709551615ull)

typedef float               Float32;
typedef double              Float64;

#if !defined(SWIG)

#ifdef EE_PLATFORM_WIN32
// Special windows types
typedef HINSTANCE InstanceRef;
typedef HACCEL AcceleratorRef;
typedef LPCREATESTRUCT CreateStructRef;
typedef HMENU MenuRef;
typedef HDC ContextRef;
#else
// dummy types
typedef void* InstanceRef;
typedef void* AcceleratorRef;
typedef void* ContextRef;
typedef void* MenuRef;
typedef void* CreateStructRef;
#endif

typedef SDL_Window* WindowRef;
typedef SDL_Window* StatusWindowRef;
typedef SDL_GLContext* OglRenderContextRef;
typedef void* ModuleRef;

typedef struct
{
    SDL_Event* evt;
} EventRecord, *EventRecordPtr;

#endif

//@}
}

#endif
