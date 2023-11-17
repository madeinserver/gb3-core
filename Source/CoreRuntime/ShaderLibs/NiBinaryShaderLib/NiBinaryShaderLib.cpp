// EMERGENT GAME TECHNOLOGIES PROPRIETARY INFORMATION
//
// This software is supplied under the terms of a license agreement or
// nondisclosure agreement with Emergent Game Technologies and may not
// be copied or disclosed except in accordance with the terms of that
// agreement.
//
//      Copyright (c) 1996-2009 Emergent Game Technologies.
//      All Rights Reserved.
//
// Emergent Game Technologies, Calabasas, CA 91302
// http://www.emergent.net
//--------------------------------------------------------------------------------------------------
// Precompiled Header
#include "NiBinaryShaderLibPCH.h"

#include "NiBinaryShaderLib.h"
#include "NiBinaryShaderLibrary.h"

#include "NSBLoader.h"
#include "NiBinaryShaderLibLibType.h"
#include "NiBinaryShaderLibSDM.h"

#include <NiShaderAttributeDesc.h>
#include <NiShaderDesc.h>
#include <NiShaderLibraryDesc.h>

//--------------------------------------------------------------------------------------------------
#if defined(_USRDLL)

static NiBinaryShaderLibSDM NiBinaryShaderLibSDMObject;

BOOL WINAPI DllMain(HINSTANCE, ULONG fdwReason, LPVOID)
{
    NiOutputDebugString("NiBinaryShaderLib> DLLMain CALL - ");

    switch (fdwReason)
    {
    case DLL_PROCESS_ATTACH:
        {
            //  Initialize anything needed here
            //  If failed, return FALSE
            NiOutputDebugString("PROCESS ATTACH!\n");
            NiStaticDataManager::ProcessAccumulatedLibraries();
        }
        break;
    case DLL_THREAD_ATTACH:
        {
            NiOutputDebugString("THREAD ATTACH!\n");
        }
        break;
    case DLL_PROCESS_DETACH:
        {
            //  Shutdown anything needed here
            NiOutputDebugString("PROCESS DETACH!\n");
            NiStaticDataManager::RemoveLibrary("NiBinaryShaderLib");
        }
        break;
    case DLL_THREAD_DETACH:
        {
            NiOutputDebugString("THREAD DETACH!\n");
        }
        break;
    }

    return (TRUE);
}

//--------------------------------------------------------------------------------------------------
NIBINARYSHADERLIB_ENTRY bool LoadShaderLibrary(NiRenderer* pkRenderer,
    int iDirectoryCount, const char* apcDirectories[], bool bRecurseSubFolders,
    NiShaderLibrary** ppkLibrary)
{
    return NiBinaryShaderLib_LoadShaderLibrary(
        pkRenderer,
        iDirectoryCount,
        apcDirectories,
        bRecurseSubFolders,
        ppkLibrary);
}

//--------------------------------------------------------------------------------------------------
NIBINARYSHADERLIB_ENTRY unsigned int GetCompilerVersion(void)
{
    return (_MSC_VER);
}

//--------------------------------------------------------------------------------------------------
#endif  //#if defined(_USRDLL)

//--------------------------------------------------------------------------------------------------
bool NSBShaderLib_LoadShaderLibrary(NiRenderer* pkRenderer,
    int iDirectoryCount, const char* apcDirectories[],
    bool bRecurseSubFolders, NiShaderLibrary** ppkLibrary)
{
    return NiBinaryShaderLib_LoadShaderLibrary(
        pkRenderer, 
        iDirectoryCount, 
        apcDirectories,
        bRecurseSubFolders,
        ppkLibrary);
}
//--------------------------------------------------------------------------------------------------
bool NiBinaryShaderLib_LoadShaderLibrary(NiRenderer* pkRenderer,
    int iDirectoryCount, const char* apcDirectories[],
    bool bRecurseSubFolders, NiShaderLibrary** ppkLibrary)
{
    *ppkLibrary = NULL;

#if defined(WIN32)
    NiBinaryShaderLibrary* pkLibrary = NiBinaryShaderLibrary::Create(
        iDirectoryCount, 
        apcDirectories, 
        bRecurseSubFolders);
    if (pkLibrary)
        pkLibrary->SetRenderer((NiD3DRenderer*)pkRenderer);

    *ppkLibrary = pkLibrary;
    return (*ppkLibrary != NULL);
#elif defined(_XENON)
    NiBinaryShaderLibrary* pkLibrary = NiBinaryShaderLibrary::Create(
        iDirectoryCount,
        apcDirectories, 
        bRecurseSubFolders);
    if (pkLibrary)
        pkLibrary->SetRenderer((NiD3DRenderer*)pkRenderer);

    *ppkLibrary = pkLibrary;
    return (*ppkLibrary != NULL);
#elif defined(_PS3)
    NiBinaryShaderLibrary* pkLibrary = NiBinaryShaderLibrary::Create(
        iDirectoryCount,
        apcDirectories, bRecurseSubFolders);

    *ppkLibrary = pkLibrary;
    return (*ppkLibrary != NULL);
#else
    #error "Unknown platform";
    return false;
#endif
}

//--------------------------------------------------------------------------------------------------
