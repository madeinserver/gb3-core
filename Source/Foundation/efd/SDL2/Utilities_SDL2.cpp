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
//#include "efdPCH.h"

#include <efd/Utilities.h>
#include <efd/TimeType.h>

using namespace efd;

//--------------------------------------------------------------------------------------------------
efd::UInt64 efd::GetPerformanceCounter()
{
    return SDL_GetPerformanceCounter();
}

//--------------------------------------------------------------------------------------------------
// _rotr operated on efd::UInt32 (4 bytes on a PC).  So the non-Windows
// version should be 32 bits as well.
efd::UInt32 efd::Rotr(efd::UInt32 x, efd::SInt32 n)
{
#ifdef EE_PLATFORM_WIN32
    return _rotr(x, n);
#else
    __asm__("rorl %%cl, %0" : "+r" (x) : "c" (n));
    return x;
#endif
}

//--------------------------------------------------------------------------------------------------
void efd::GetEnvironmentVariable(size_t* pstDestLength, efd::Char* pcDest, size_t stDestSize,
    const efd::Char* pcSrc)
{
#if _MSC_VER >= 1400 && defined (EE_PLATFORM_WIN32)
    getenv_s(pstDestLength, pcDest, stDestSize, pcSrc);
#else // #if _MSC_VER >= 1400

    EE_ASSERT(pstDestLength != 0 && pcDest != 0 && stDestSize != 0);

    char* pcResult = getenv(pcSrc);
    if (pcResult)
    {
        efd::Strcpy(pcDest, stDestSize, pcResult);
        *pstDestLength = strlen(pcDest);
    }
    else
    {
        *pstDestLength = 0;
    }
#endif // #if _MSC_VER >= 1400
}

//--------------------------------------------------------------------------------------------------
efd::UInt32 efd::MakeDir(const efd::Char* path)
{
#if defined (EE_PLATFORM_WIN32)
    return efd::UInt32(_mkdir(path));
#else
    return efd::UInt32(mkdir(path));
#endif
}

//--------------------------------------------------------------------------------------------------
void efd::InitTestEnvironment()
{
#if defined (EE_PLATFORM_WIN32)
    DWORD dwMode = SetErrorMode(0);
    SetErrorMode(dwMode | SEM_NOGPFAULTERRORBOX);
#endif
}

//--------------------------------------------------------------------------------------------------
