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
#include "NSFParserLibPCH.h"

#include "NSFLoader.h"
#include "NSFParsedShader.h"
#include <NiFilename.h>

#include <NiShaderLibraryDesc.h>
#include <NiShaderDesc.h>

//--------------------------------------------------------------------------------------------------
NSFLoaderPtr NSFLoader::ms_spLoader;
extern NiTPointerList<NSFParsedShader*> g_kParsedShaderList;
extern int ParseShader(const char* pcFileName);
extern void ResetParser();
extern void CleanupParser();

//--------------------------------------------------------------------------------------------------
NSFLoader::NSFLoader()
{
    m_kNSFTextList.RemoveAll();
}

//--------------------------------------------------------------------------------------------------
NSFLoader::~NSFLoader()
{
    NiTListIterator kIter = m_kNSFTextList.GetHeadPos();
    while (kIter)
    {
        char* pszName = m_kNSFTextList.GetNext(kIter);
        if (pszName)
            NiFree(pszName);
    }
    m_kNSFTextList.RemoveAll();
}

//--------------------------------------------------------------------------------------------------
NiShaderParser* NSFLoader::Create()
{
    if (!ms_spLoader)
        ms_spLoader = NiNew NSFLoader();

    return ms_spLoader;
}

//--------------------------------------------------------------------------------------------------
void NSFLoader::Destroy()
{
    ms_spLoader = NULL;

    NiTListIterator iter = g_kParsedShaderList.GetHeadPos();
    while (iter)
        NiDelete g_kParsedShaderList.GetNext(iter);

    g_kParsedShaderList.RemoveAll();
}

//--------------------------------------------------------------------------------------------------
void NSFLoader::ParseAllFiles(const char* pszDirectory,
    bool bRecurseDirectories, unsigned int& uiCount,
    NiTObjectArray<NiFixedString>* pkFileNames)
{
    FindAllNSFTextFiles(pszDirectory, bRecurseDirectories);

    uiCount = 0;
    uiCount += LoadAllNSFTextFiles(pkFileNames);
}

//--------------------------------------------------------------------------------------------------
bool NSFLoader::ParseFile(const char* pcFile, unsigned int& uiCount,
    NiTObjectArray<NiFixedString>* pkFileNames)
{
    uiCount = 0;
    NILOG(NIMESSAGE_GENERAL_0, "Attempting to parse %s\n", pcFile);

    ResetParser();
    int iResult = ParseShader(pcFile);
    CleanupParser();
    if (iResult != 0)
    {
        NILOG(NIMESSAGE_GENERAL_0, "Failed to parse %s\n", pcFile);
    }
    else
    {
        // We will write the binary format out, and then let the binary
        // loading step actually add it...
        // This will 'auto-recompile' any shader files...
        // However, it requires that text files get loaded
        // prior to the binary list being created.
        NiFilename kFilename(pcFile);

        // Grab each shader out of the list
        NSFParsedShader* pkParsedShader;
        NSBShaderPtr spNSB;

        NiTListIterator iter;
        iter = g_kParsedShaderList.GetHeadPos();
        while (iter)
        {
            pkParsedShader = g_kParsedShaderList.GetNext(iter);
            if (!pkParsedShader)
                continue;

            spNSB = pkParsedShader->GetShader();
            if (spNSB)
            {
                NILOG(NIMESSAGE_GENERAL_0,
                    "Parsed Shader %s\n", spNSB->GetName());

                NILOG(NIMESSAGE_GENERAL_0,
                    "Storing binary version of shader %s\n",
                    spNSB->GetName());

                kFilename.SetFilename(spNSB->GetName());
                kFilename.SetExt(".nsb");

                char acFullPath[NI_MAX_PATH];
                kFilename.GetFullPath(acFullPath, NI_MAX_PATH);

                if (spNSB->Save(acFullPath))
                    uiCount++;

                spNSB = 0;

                if (pkFileNames)
                    pkFileNames->Add(acFullPath);
            }
        }
    }

    NiTListIterator iter = g_kParsedShaderList.GetHeadPos();
    while (iter)
    {
        NSFParsedShader* pkParsedShader =
            g_kParsedShaderList.GetNext(iter);
        if (pkParsedShader)
            NiDelete pkParsedShader;
    }
    g_kParsedShaderList.RemoveAll();

    return uiCount > 0;
}

//--------------------------------------------------------------------------------------------------
unsigned int NSFLoader::GetTextFileCount()
{
    return m_kNSFTextList.GetSize();
}

//--------------------------------------------------------------------------------------------------
const char* NSFLoader::GetFirstTextFile(NiTListIterator& kIter)
{
    kIter = m_kNSFTextList.GetHeadPos();

    if (kIter)
        return m_kNSFTextList.GetNext(kIter);
    return 0;
}

//--------------------------------------------------------------------------------------------------
const char* NSFLoader::GetNextTextFile(NiTListIterator& kIter)
{
    if (kIter)
        return m_kNSFTextList.GetNext(kIter);
    return 0;
}

//--------------------------------------------------------------------------------------------------
void NSFLoader::FindAllNSFTextFiles(const char* pszDirectory,
    bool bRecurseDirectories)
{
    LoadAllNSFFilesInDirectory(pszDirectory, ".NSF", bRecurseDirectories,
        &m_kNSFTextList);
}

//--------------------------------------------------------------------------------------------------
bool NSFLoader::ProcessNSFFile(const char* pcFilename, const char* pszExt,
    NiTPointerList<char*>* pkFileList)
{
    if (!pcFilename || (strcmp(pcFilename, "") == 0))
        return false;

    NiFilename kFilename(pcFilename);
    if (NiStricmp(kFilename.GetExt(), pszExt) == 0)
    {
        NILOG(NIMESSAGE_GENERAL_0, "        Found %s File %s\n", pszExt,
            pcFilename);

        // Add it to the list
        size_t stLen = strlen(pcFilename) + 1;
        char* pszNew = NiAlloc(char, stLen);
        EE_ASSERT(pszNew);

        NiStrcpy(pszNew, stLen, pcFilename);

        pkFileList->AddTail(pszNew);
        return true;
    }

    return false;
}

//--------------------------------------------------------------------------------------------------
unsigned int NSFLoader::LoadAllNSFTextFiles(
    NiTObjectArray<NiFixedString>* pkFileNames)
{
    unsigned int uiCount = 0;
    NiTListIterator kIter = 0;
    const char* pcFile = GetFirstTextFile(kIter);

    while (pcFile)
    {
        ResetParser();

        unsigned int uiParsed = 0;
        ParseFile(pcFile, uiParsed, pkFileNames);
        uiCount += uiParsed;

        pcFile = GetNextTextFile(kIter);
        CleanupParser();
    }

    return uiCount;
}

//--------------------------------------------------------------------------------------------------
unsigned int NSFLoader::GetNumSupportedMimeTypes() const
{
    return 1;
}

//--------------------------------------------------------------------------------------------------
const char* NSFLoader::GetSupportedMimeType(unsigned int uiIdx) const
{
    EE_ASSERT_MESSAGE(!uiIdx, ("Invalid index"));
    EE_UNUSED_ARG(uiIdx);
    return "gamebryo-text-shader";
}

//--------------------------------------------------------------------------------------------------
const char* NSFLoader::GetOutputMimeType() const
{
    return "gamebryo-binary-shader";
}

//--------------------------------------------------------------------------------------------------