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

#include <efd/OS.h>
#include <efd/AtomicOperations.h>

namespace efd
{

inline Semaphore::Semaphore() :
    m_count(0),
    m_maxCount(1)
{
    m_hSemaphore = SDL_CreateSemaphore(0);
    EE_ASSERT(m_hSemaphore != NULL);
}
//-------------------------------------------------------------------------------------------------
inline Semaphore::Semaphore(efd::SInt32 count) :
    m_count(count),
    m_maxCount(count + 1)
{
    m_hSemaphore = SDL_CreateSemaphore(m_count);
    EE_ASSERT(m_hSemaphore != NULL);
}
//-------------------------------------------------------------------------------------------------
inline Semaphore::Semaphore(efd::SInt32 count, efd::SInt32 maxCount) :
    m_count(count),
    m_maxCount(maxCount)
{
    m_hSemaphore = SDL_CreateSemaphore(m_count);
    EE_ASSERT(m_hSemaphore != NULL);
}
//-------------------------------------------------------------------------------------------------
inline Semaphore::~Semaphore()
{
    SDL_DestroySemaphore(m_hSemaphore);
    m_hSemaphore = NULL;
}
//-------------------------------------------------------------------------------------------------
inline efd::SInt32 Semaphore::GetCount()
{
    return m_count;
}
//-------------------------------------------------------------------------------------------------
inline efd::SInt32 Semaphore::GetMaxCount()
{
    return m_maxCount;
}
//-------------------------------------------------------------------------------------------------
inline efd::SInt32 Semaphore::Signal()
{
    int count = SDL_SemPost(m_hSemaphore);
    Atomic::SetValue(&m_count, count);
    return m_count;
}
//-------------------------------------------------------------------------------------------------
inline efd::SInt32 Semaphore::Wait()
{
    int count = SDL_SemWait(m_hSemaphore);
    Atomic::SetValue(&m_count, count);
    return m_count;
}
//-------------------------------------------------------------------------------------------------
}; //namespace efd
