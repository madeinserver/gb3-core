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

#include <efd/TimeType.h>
#include <SDL2/SDL_timer.h>

using namespace efd;

// File scope globals used by both GetCurrentTimeInSec() and SetInitialTimeInSec()
bool gs_appTimeInitialized = false;
Uint64 gs_freq;
Uint64 gs_appTimeOffset;

//------------------------------------------------------------------------------------------------
void efd::SetInitialTimeInSec(float offsetInSeconds /* = 0.0f */)
{
    gs_freq = SDL_GetPerformanceFrequency();
    gs_appTimeOffset = SDL_GetPerformanceCounter();
    
    gs_appTimeOffset -= (Uint64)((long double)offsetInSeconds * (long double)gs_freq);

    gs_appTimeInitialized = true;
}

//------------------------------------------------------------------------------------------------
efd::TimeType efd::GetCurrentTimeInSec()
{
    if (!gs_appTimeInitialized)
    {
        SetInitialTimeInSec();
    }

    Uint64 counter = SDL_GetPerformanceCounter();
    return efd::TimeType((long double)(counter - gs_appTimeOffset) /
        (long double)gs_freq);
}

//------------------------------------------------------------------------------------------------
// Implementation of efd::RealTimeClock
//------------------------------------------------------------------------------------------------
TimeType RealTimeClock::GetCurrentTime()
{
    // DT32308 This is only has 1 sec accuracy, we need to use a better method
    time_t long_time;
    time(&long_time);
    return (efd::Float64)long_time;
}

//------------------------------------------------------------------------------------------------
// Implementation of efd::HighPrecisionClock
//------------------------------------------------------------------------------------------------
TimeType HighPrecisionClock::GetCurrentTime() const
{
    // This method is very similar to GetCurrentTimeInSec(), but it always returns time since app
    // start instead of time since the last SetInitialTimeInSec() call.
    static bool bFirst = true;
    static Uint64 freq;
    static Uint64 initial;

    if (bFirst)
    {
        freq = SDL_GetPerformanceFrequency();
        initial = SDL_GetPerformanceCounter();
        bFirst = false;
    }

    Uint64 counter = SDL_GetPerformanceCounter();

    efd::Float64 current = (efd::Float64)((efd::Float64)(counter - initial) /
        (efd::Float64)freq);

    current += (efd::Float64)m_syncOffset;

    return current;
}

//------------------------------------------------------------------------------------------------
void HighPrecisionClock::SetSynchronizationOffset(TimeType i_offset)
{
    m_syncOffset = i_offset;
}

//------------------------------------------------------------------------------------------------
