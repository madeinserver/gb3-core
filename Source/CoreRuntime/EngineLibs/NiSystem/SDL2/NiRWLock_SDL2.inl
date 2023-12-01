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

//--------------------------------------------------------------------------------------------------
inline NiRWLock::NiRWLock()
    : m_kNoReadersCond(NULL)
{
    m_kNoReadersCond = SDL_CreateCond();
}

//--------------------------------------------------------------------------------------------------
inline NiRWLock::~NiRWLock()
{
    SDL_DestroyCond(m_kNoReadersCond);
}

//--------------------------------------------------------------------------------------------------
inline void NiRWLock::LockRead()
{
    SDL_CondSignal(m_kNoReadersCond);
    m_kWriteLock.Lock();
    efd::AtomicIncrement(m_uiNumReaders);
    m_kWriteLock.Unlock();
}

//--------------------------------------------------------------------------------------------------
inline void NiRWLock::LockWrite()
{
    m_kWriteLock.Lock();
    SDL_CondWait(m_kNoReadersCond, m_kWriteLock.GetSysMutex());
}

//--------------------------------------------------------------------------------------------------
inline void NiRWLock::UnlockWrite()
{
    m_kWriteLock.Unlock();
}

//--------------------------------------------------------------------------------------------------
inline void NiRWLock::UnlockRead()
{
    // read unlock
    if (efd::AtomicDecrement(m_uiNumReaders) == 0)
    {
        SDL_CondSignal(m_kNoReadersCond);
    }
}

//--------------------------------------------------------------------------------------------------

