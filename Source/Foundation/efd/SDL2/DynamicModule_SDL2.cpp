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

#include <efd/DynamicModule.h>

using namespace efd;

//------------------------------------------------------------------------------------------------
DynamicModule::DynamicModule()
    : m_pData(NULL)
{
}

//------------------------------------------------------------------------------------------------
DynamicModule::DynamicModule(const DynamicModule& i_other)
    : m_pData(i_other.m_pData)
{
    if (m_pData)
    {
        m_pData->IncRefCount();
    }
}

//------------------------------------------------------------------------------------------------
DynamicModule::~DynamicModule()
{
    UnloadModule();
}

//------------------------------------------------------------------------------------------------
DynamicModule& DynamicModule::operator=(const DynamicModule& i_rhs)
{
    if (m_pData != i_rhs.m_pData)
    {
        if (m_pData)
        {
            m_pData->DecRefCount();
        }
        m_pData = i_rhs.m_pData;
        if (m_pData)
        {
            m_pData->IncRefCount();
        }
    }
    return *this;
}

//------------------------------------------------------------------------------------------------
utf8string CreateSuffix(const char* suffix)
{
#if !defined(EE_GAMEBRYO_VERSION)
    // The above define must exist to calculate the correct library suffix.
    EE_COMPILETIME_ASSERT(false);
#endif
    utf8string rval;
    rval.sprintf("%d%s", EE_GAMEBRYO_VERSION, suffix);
    return rval;
}

//------------------------------------------------------------------------------------------------
static void* GetGamebryoLibraryHandle(
    const utf8string& i_strName)
{

#if defined(EE_EFD_CONFIG_DEBUG)
    utf8string suffix = CreateSuffix("D");
#elif defined(EE_EFD_CONFIG_SHIPPING)
    utf8string suffix = CreateSuffix("S");
#else
    utf8string suffix = CreateSuffix("R");
#endif

    utf8string qualifiedName = i_strName;
    qualifiedName.append(suffix);

    void* libHandle = NULL;

    libHandle = SDL_LoadObject(qualifiedName.c_str());    

    return libHandle;
}

//------------------------------------------------------------------------------------------------
bool DynamicModule::LoadModule(const utf8string& i_strName, bool loadLibraryAsDatafile)
{
    UnloadModule();

    void* libHandle = NULL;

    // Apply naming convention to select appropriate file
    utf8string qualifiedName = i_strName;
    if (i_strName.rfind(".") == utf8string::npos)
    {
        libHandle = GetGamebryoLibraryHandle(i_strName);
    }
    else
    {
        libHandle = SDL_LoadObject(qualifiedName.c_str());
    }

    if (libHandle == NULL)
    {
        return false;
    }

    m_pData = EE_EXTERNAL_NEW ModuleData();
    // our ref counts start at zero and I'm not using a smart pointer so manually increment:
    m_pData->IncRefCount();

    m_pData->m_handle = (ModuleHandle)libHandle;
    m_pData->m_strName = i_strName;

    return true;
}

//------------------------------------------------------------------------------------------------
bool DynamicModule::UnloadModule()
{
    if (m_pData)
    {
        m_pData->DecRefCount();
        m_pData = NULL;
        return true;
    }
    return false;
}

//------------------------------------------------------------------------------------------------
bool DynamicModule::IsLoaded() const
{
    if (m_pData)
    {
        EE_ASSERT(m_pData->m_handle);
        return true;
    }
    return false;
}

//------------------------------------------------------------------------------------------------
MethodHandle DynamicModule::GetMethod(const utf8string& i_strName) const
{
    if (!m_pData)
    {
        return NULL;
    }
    EE_ASSERT(m_pData->m_handle);

    MethodHandle pMethod = SDL_LoadFunction(m_pData->m_handle, i_strName.c_str());
    return pMethod;
}

//------------------------------------------------------------------------------------------------
const utf8string& DynamicModule::GetModuleName() const
{
    if (m_pData)
    {
        return m_pData->m_strName;
    }
    return utf8string::NullString();
}

//------------------------------------------------------------------------------------------------
ModuleHandle DynamicModule::GetModuleHandle() const
{
    if (m_pData)
    {
        EE_ASSERT(m_pData->m_handle);
        return m_pData->m_handle;
    }
    return NULL;
}

//------------------------------------------------------------------------------------------------
DynamicModule::ModuleData::ModuleData()
: m_handle(NULL)
{
}

//------------------------------------------------------------------------------------------------
DynamicModule::ModuleData::~ModuleData()
{
    if (m_handle)
    {
        SDL_UnloadObject(m_handle);
        m_handle = NULL;
    }
}

//------------------------------------------------------------------------------------------------
const char* DynamicModuleNameRules::GetPlatformPrefix()
{
    return "";
}

//------------------------------------------------------------------------------------------------
const char* DynamicModuleNameRules::GetPlatformExtension()
{
#ifdef EE_PLATFORM_WIN32
    return ".dll";
#elif defined (EE_PLATFORM_MACOSX)
    return ".dynlib";
#else
    return ".so";
#endif
}

//------------------------------------------------------------------------------------------------
const utf8string DynamicModuleNameRules::GetPlatformSpecificName(const utf8string& i_strBaseName)
{
    utf8string strResult = GetPlatformPrefix() + i_strBaseName + GetPlatformExtension();
    return strResult;
}

