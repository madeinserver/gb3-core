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
#include "NSBShaderLibPCH.h"

#include "NSBLoader.h"
#include "NSBShader.h"

#include <NiFilename.h>

//--------------------------------------------------------------------------------------------------
NSBLoader* NSBLoader::ms_pkLoader = 0;
efd::CriticalSection NSBLoader::m_kCriticalSection;

//--------------------------------------------------------------------------------------------------
NSBLoader::NSBLoader() :
    m_uiRefCount(0)
{
    m_kBinaryList.RemoveAll();
}

//--------------------------------------------------------------------------------------------------
NSBLoader::~NSBLoader()
{
    NiTListIterator kIter = m_kBinaryList.GetHeadPos();
    while (kIter)
    {
        char* pcName = m_kBinaryList.GetNext(kIter);
        if (pcName)
            NiFree(pcName);
    }
    m_kBinaryList.RemoveAll();
}

//--------------------------------------------------------------------------------------------------
NSBLoader* NSBLoader::GetInstance()
{
    m_kCriticalSection.Lock();

    if (!ms_pkLoader)
        ms_pkLoader = NiNew NSBLoader();

    ms_pkLoader->m_uiRefCount++;

    m_kCriticalSection.Unlock();

    return ms_pkLoader;
}

//--------------------------------------------------------------------------------------------------
void NSBLoader::Release()
{
    m_kCriticalSection.Lock();

    if (ms_pkLoader)
    {
        if (--ms_pkLoader->m_uiRefCount == 0)
        {
            NiDelete ms_pkLoader;
            ms_pkLoader = 0;
        }
    }

    m_kCriticalSection.Unlock();
}

//--------------------------------------------------------------------------------------------------
bool NSBLoader::LoadAllNSBFiles(const char* pcDirectory, bool bRecurseSubDirs)
{
    FindAllNSBFiles(pcDirectory, bRecurseSubDirs);
    if (!LoadAllNSBFiles())
        return false;

    return true;
}

//--------------------------------------------------------------------------------------------------
unsigned int NSBLoader::GetBinaryFileCount()
{
    return m_kBinaryList.GetSize();
}

//--------------------------------------------------------------------------------------------------
const char* NSBLoader::GetFirstBinaryFile(NiTListIterator& kIter)
{
    kIter = m_kBinaryList.GetHeadPos();

    if (kIter)
        return m_kBinaryList.GetNext(kIter);
    return 0;
}

//--------------------------------------------------------------------------------------------------
const char* NSBLoader::GetNextBinaryFile(NiTListIterator& kIter)
{
    if (kIter)
        return m_kBinaryList.GetNext(kIter);
    return 0;
}

//--------------------------------------------------------------------------------------------------
void NSBLoader::FindAllNSBFiles(const char* pcDirectory,
    bool bRecurseDirectories)
{
    LoadAllNSBFilesInDirectory(pcDirectory, ".NSB", bRecurseDirectories,
        &m_kBinaryList);
}

//--------------------------------------------------------------------------------------------------
bool NSBLoader::ProcessNSBFile(const char* pcFilename, const char* pcExt,
    NiTPointerList<char*>* pkFileList)
{
    if (!pcFilename || (strcmp(pcFilename, "") == 0))
        return false;

    NiFilename kFilename(pcFilename);
    if (NiStricmp(kFilename.GetExt(), pcExt) == 0)
    {
        NILOG(NIMESSAGE_GENERAL_0,
            "        Found %s File %s\n", pcExt, pcFilename);

        // Add it to the list
        size_t stLen = strlen(pcFilename) + 1;
        char* pcNew = NiAlloc(char, stLen);
        EE_ASSERT(pcNew);

        NiStrcpy(pcNew, stLen, pcFilename);

        pkFileList->AddTail(pcNew);
        return true;
    }

    return false;
}

//--------------------------------------------------------------------------------------------------
bool NSBLoader::LoadAllNSBFiles()
{
    bool bResult = false;

    NiTListIterator kIter = 0;
    const char* pcFile = GetFirstBinaryFile(kIter);
    while (pcFile)
    {
        bResult |= LoadNSBFile(pcFile);
        pcFile = GetNextBinaryFile(kIter);
    }

    return bResult;
}

//--------------------------------------------------------------------------------------------------
bool NSBLoader::LoadNSBFile(const char* pcFile)
{
    // Parse the name and see if it's in the list.
    NiFilename kFilename(pcFile);

    if (GetNSBShader(kFilename.GetFilename()))
    {
        NILOG(NIMESSAGE_GENERAL_0, "%s (%s) already loaded?\n",
            kFilename.GetFilename(), pcFile);
    }
    else
    {
        NILOG(NIMESSAGE_GENERAL_0, "Attempting to load %s\n", pcFile);

        NSBShader* pkNSBShader = NiNew NSBShader();
        if (pkNSBShader)
        {
            if (pkNSBShader->Load(pcFile))
            {
                NILOG(NIMESSAGE_GENERAL_0, "Loaded %s\n", pcFile);
                InsertNSBShaderIntoList(pkNSBShader);
                return true;
            }
            else
            {
                NILOG(NIMESSAGE_GENERAL_0, "Failed to load %s\n",
                    pcFile);
                NiDelete pkNSBShader;
            }
        }
    }

    return false;
}

//--------------------------------------------------------------------------------------------------
bool NSBLoader::InsertNSBShaderIntoList(NSBShader* pkNSBShader)
{
    if (!pkNSBShader)
        return false;

    NSBShader* pkCheckShader = GetNSBShader(pkNSBShader->GetName());
    if (pkCheckShader)
    {
        NILOG(NIMESSAGE_GENERAL_0, "Shader %s already in list??\n");
        return false;
    }

    m_kNSBShaderList.AddTail(pkNSBShader);

    return true;
}

//--------------------------------------------------------------------------------------------------
unsigned int NSBLoader::GetNSBShaderCount()
{
    return m_kNSBShaderList.GetSize();
}

//--------------------------------------------------------------------------------------------------
NSBShader* NSBLoader::GetFirstNSBShader(NiTListIterator& kIter)
{
    kIter = m_kNSBShaderList.GetHeadPos();
    return GetNextNSBShader(kIter);
}

//--------------------------------------------------------------------------------------------------
NSBShader* NSBLoader::GetNextNSBShader(NiTListIterator& kIter)
{
    if (kIter)
        return m_kNSBShaderList.GetNext(kIter);
    return 0;
}

//--------------------------------------------------------------------------------------------------
NSBShader* NSBLoader::GetNSBShader(const char* pcName)
{
    NiTListIterator kIter = m_kNSBShaderList.GetHeadPos();
    while (kIter)
    {
        NSBShader* pkNSBShader = m_kNSBShaderList.GetNext(kIter);
        if (!pkNSBShader)
            continue;
        if (strcmp(pkNSBShader->GetName(), pcName) == 0)
        {
            // Found it
            return pkNSBShader;
        }
    }

    return 0;
}

//--------------------------------------------------------------------------------------------------

