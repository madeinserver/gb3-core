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
#ifndef EE_OS_SDL2_H
#define EE_OS_SDL2_H

#include <SDL2/SDL.h>

#if EE_COMPILER_MSVC
// WIN32 system headers demote/disable some useful warnings, so push
// warning state here and pop it after the include
#pragma warning(push, 3)
#endif

#if EE_PLATFORM_WIN32
#include <winsock2.h>
#include <windows.h>

#define EE_ASM_DECOR(x) x
#else
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <libgen.h>

#ifdef EE_PLATFORM_LINUX
#include <sched.h>
#endif

#ifdef LOCK_READ
#undef LOCK_READ
#endif

#ifdef LOCK_WRITE
#undef LOCK_WRITE
#endif

#define EE_ASM_DECOR(x) _##x
#endif

#if EE_COMPILER_MSVC
#pragma warning(pop)
#endif

/// Helper macro for unused warnings
#define EE_UNUSED

/// A helper macro that declares the argument as unused. Useful when building at warning level-4
/// to indicate the unused argument is known and accepted.
#define EE_UNUSED_ARG(arg) ((void)(arg))

#if EE_COMPILER_MSVC
/// Attempt to force the compiler to inline the function.
#define EE_FORCEINLINE __forceinline

/// Attempt to force the compiler to never inline the function.
#define EE_NOINLINE __declspec(noinline)

#if (_MSC_VER >= 1400) //VC8.0
    /// Restrict pointer overlapping
    #define EE_RESTRICT __restrict
    
    /// Enable "_s" functions (only on Windows)
    #define EE_HAVE_SECURE_FUNCTIONS 1
#else
    /// Restrict pointer overlapping
    #define EE_RESTRICT
#endif

#ifndef _WIN64
    #define EE_ASM_CALL __cdecl
#endif

#elif EE_COMPILER_GCC || EE_COMPILER_CLANG
/// Attempt to force the compiler to inline the function.
#define EE_FORCEINLINE inline __attribute__((always_inline))

/// Attempt to force the compiler to never inline the function.
#define EE_NOINLINE __attribute__ ((noinline))

/// Restrict pointer overlapping
#define EE_RESTRICT __restrict__

#ifdef _WIN32
    /// Enable "_s" functions (only on Windows)
    #define EE_HAVE_SECURE_FUNCTIONS 1

    #ifndef _WIN64
        #define EE_ASM_CALL __cdecl
    #endif
#endif

#endif

#ifndef EE_ASM_CALL
#define EE_ASM_CALL
#endif

/// throw an exception
#define EE_EMPTY_THROW throw()

#if EE_COMPILER_MSVC
// We call many CRT methods that MSVC has marked as deprecated
// DT22211 We should remove calls to deprecated methods so this isn't needed
#pragma warning(disable : 4996)
#endif


#endif
