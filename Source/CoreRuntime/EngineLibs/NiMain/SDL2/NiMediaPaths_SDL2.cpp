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

// Precompiled Header
#include "NiMainPCH.h"

#include <efd/Utilities.h>
#include <efd/StringUtilities.h>

#include "NiMediaPaths_SDL2.h"

#ifdef EE_PLATFORM_WIN32
#define PLATFORM_NAME "Win32"
#elif defined EE_PLATFORM_MACOSX
#define PLATFORM_NAME "MacOSX"
#elif defined EE_PLATFORM_LINUX
#define PLATFORM_NAME "Linux"
#else
#error "Unsupported media path"
#endif

//--------------------------------------------------------------------------------------------------
bool NiMediaPaths_SDL2::GetEmergentPath(char* pBuffer, size_t bufferSize)
{
    EE_ASSERT((pBuffer != NULL) && (bufferSize > 0));

    size_t destLength;
    efd::GetEnvironmentVariable(&destLength, pBuffer, bufferSize, "EMERGENT_PATH");

    if (destLength == 0)
    {
        // Environment variable not found
        pBuffer[0] = 0;
        return false;
    }

    if (destLength > bufferSize)
    {
        // Buffer too small
        pBuffer[0] = 0;
        return false;
    }

    return true;
}
//--------------------------------------------------------------------------------------------------
void NiMediaPaths_SDL2::AppendShaderPath(char* pBuffer, size_t bufferSize)
{
    efd::Strcat(pBuffer, bufferSize, "\\Media\\Shaders\\Data");
}
//--------------------------------------------------------------------------------------------------
bool NiMediaPaths_SDL2::FetchAbsoluteShaderPath(NiString& outAbsoluteShaderPath)
{
    char acBuffer[2048];

    if (!NiMediaPaths_SDL2::GetEmergentPath(acBuffer, 2048))
        return false;
    AppendShaderPath(acBuffer, 2048);
    
    outAbsoluteShaderPath = acBuffer;
    return true;
}
//--------------------------------------------------------------------------------------------------
void NiMediaPaths_SDL2::AppendShaderLibPath(char* pBuffer, size_t bufferSize)
{
    efd::Strcat(pBuffer, bufferSize, "\\SDK\\" PLATFORM_NAME "\\DLL\\ShaderLibs");
}
//--------------------------------------------------------------------------------------------------
bool NiMediaPaths_SDL2::FetchAbsoluteShaderLibPath(NiString& outAbsoluteShaderLibPath)
{
    char acBuffer[2048];

    if (!NiMediaPaths_SDL2::GetEmergentPath(acBuffer, 2048))
        return false;
    AppendShaderLibPath(acBuffer, 2048);
    
    outAbsoluteShaderLibPath = acBuffer;
    return true;
}
//--------------------------------------------------------------------------------------------------
void NiMediaPaths_SDL2::AppendShaderDefaultTexturePath(char* pBuffer, size_t bufferSize)
{
    efd::Strcat(pBuffer, bufferSize, "\\Media\\Shaders\\Data\\Textures");
}
//--------------------------------------------------------------------------------------------------
bool NiMediaPaths_SDL2::FetchAbsoluteShaderDefaultTexturePath(NiString& outAbsoluteTexturePath)
{
    char acBuffer[2048];

    if (!NiMediaPaths_SDL2::GetEmergentPath(acBuffer, 2048))
        return false;
    AppendShaderDefaultTexturePath(acBuffer, 2048);
    
    outAbsoluteTexturePath = acBuffer;
    return true;
}
//--------------------------------------------------------------------------------------------------
void NiMediaPaths_SDL2::AppendMeshProfilePath(char* pBuffer, size_t bufferSize)
{
    efd::Strcat(pBuffer, bufferSize, "\\Media\\ToolPluinData\\MeshProfiles");
}
//--------------------------------------------------------------------------------------------------
void NiMediaPaths_SDL2::AppendToolPluginsPath(char* pBuffer, size_t bufferSize)
{
    efd::Strcat(pBuffer, bufferSize, "\\SDK\\" PLATFORM_NAME "\\DLL\\ToolPlugins");
}
//--------------------------------------------------------------------------------------------------
