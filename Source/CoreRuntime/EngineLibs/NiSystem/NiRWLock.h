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
#ifndef NIRWLOCK_H
#define NIRWLOCK_H

#include "NiSystemLibType.h"
#include "NiCriticalSection.h"
#include "NiUniversalTypes.h"

// NISYSTEM_ENTRY is intentionally not listed here.  Since this class is
// completely inline and is not referred to anywhere else in NiSystem,
// the VC++ compiler does not seem to include it in the exported DLL,
// even though __declspec(dllexport) is defined.  However, other applications
// referencing this class see the __declspec(dllexport) and expect the
// definitions to be found in the DLL, causing a linker error.
class NiRWLock
{
public:
    NiRWLock();
    ~NiRWLock();

    inline void LockRead();
    inline void LockWrite();
    inline void UnlockRead();
    inline void UnlockWrite();
protected:
    volatile efd::UAtomic m_uiNumReaders;
#ifdef EE_PLATFORM_PS3
    efd::CriticalSection m_kWriteLock;
    sys_lwcond_t m_kNoReadersCond;
#elif EE_PLATFORM_XBOX360
    efd::CriticalSection m_kWriteLock;
    HANDLE m_hNoReadersEvent;
#elif EE_PLATFORM_SDL2
    efd::CriticalSection m_kWriteLock;
    SDL_cond* m_kNoReadersCond;
#else
    #error Attempt to create reader-writer lock on undefined platform.
#endif
};

#include EE_PLATFORM_SPECIFIC_INCLUDE(NiSystem,NiRWLock,inl)

#endif // #ifndef NIRWLOCK_H
