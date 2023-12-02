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

#include <efd/Asserts.h>
namespace efd
{
//-------------------------------------------------------------------------------------------------
#if defined(EE_MULTITHREADED)
inline CriticalSection::CriticalSection(const char*) :
    m_threadOwner(0), m_lockCount(0)
{
    m_criticalSection = SDL_CreateMutex();
}
#else
inline CriticalSection::CriticalSection(const char*)
{
}
#endif // #if defined(EE_MULTITHREADED)
//-------------------------------------------------------------------------------------------------
inline CriticalSection::~CriticalSection()
{
#if defined(EE_MULTITHREADED)
    SDL_DestroyMutex(m_criticalSection);
    m_criticalSection = NULL;
#endif // #if defined(EE_MULTITHREADED)
}
//-------------------------------------------------------------------------------------------------
inline void CriticalSection::Lock()
{
#if defined(EE_MULTITHREADED)
    SDL_LockMutex(m_criticalSection);
    m_threadOwner = SDL_ThreadID();
    m_lockCount++;
#endif // #if defined(EE_MULTITHREADED)
}
//-------------------------------------------------------------------------------------------------
inline void CriticalSection::Unlock()
{
#if defined(EE_MULTITHREADED)
    EE_ASSERT(m_lockCount > 0);
    EE_ASSERT(m_threadOwner == SDL_ThreadID());
    m_lockCount--;
    if (m_lockCount == 0)
        m_threadOwner = 0;
    SDL_UnlockMutex(m_criticalSection);
#endif // #if defined(EE_MULTITHREADED)
}
//-------------------------------------------------------------------------------------------------
inline efd::UInt32 CriticalSection::GetOwningThreadID() const
{
#if defined(EE_MULTITHREADED)
    return (efd::UInt32)m_threadOwner;
#else
    return 0;
#endif // #if defined(EE_MULTITHREADED)
}
//-------------------------------------------------------------------------------------------------
inline efd::UInt32 CriticalSection::GetCurrentLockCount() const
{
#if defined(EE_MULTITHREADED)
    return m_lockCount;
#else
    return 0;
#endif // #if defined(EE_MULTITHREADED)
}
//-------------------------------------------------------------------------------------------------

inline SDL_mutex* CriticalSection::GetSysMutex() { return m_criticalSection; }
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
// Fast Critical Section Methods
//-------------------------------------------------------------------------------------------------
inline FastCriticalSection::FastCriticalSection(const char*)
{
#if defined(EE_MULTITHREADED)

#if defined(EE_EFD_CONFIG_DEBUG)
    m_locked = false;
#endif

    m_criticalSection = SDL_CreateMutex();

#endif // #if defined(EE_MULTITHREADED)
}
//-------------------------------------------------------------------------------------------------
inline FastCriticalSection::~FastCriticalSection()
{
#if defined(EE_MULTITHREADED)
    SDL_DestroyMutex(m_criticalSection);
#endif // #if defined(EE_MULTITHREADED)
}
//-------------------------------------------------------------------------------------------------
inline void FastCriticalSection::Lock()
{
#if defined(EE_MULTITHREADED)
    SDL_LockMutex(m_criticalSection);
#if defined(EE_EFD_CONFIG_DEBUG)
    EE_ASSERT(m_locked == false);
    m_locked = true;
#endif
#endif // #if defined(EE_MULTITHREADED)
}
//-------------------------------------------------------------------------------------------------
inline void FastCriticalSection::Unlock()
{
#if defined(EE_MULTITHREADED)
#if defined(EE_EFD_CONFIG_DEBUG)
    m_locked = false;
#endif
   
    SDL_UnlockMutex(m_criticalSection);
#endif // #if defined(EE_MULTITHREADED)
}
//-------------------------------------------------------------------------------------------------

}; // namespace efd
