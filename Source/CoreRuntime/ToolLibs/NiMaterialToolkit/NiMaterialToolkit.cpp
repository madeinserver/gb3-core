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

#include "NiMaterialToolkit.h"
#include "NiWin32FileFinder.h"
#include <NiRenderer.h>
#include <NiShaderDesc.h>
#include <NiShaderAttributeDesc.h>
#include <NiMaterialLibrary.h>

#include <NiShaderFactory.h>
#include <NiShaderLibrary.h>
#include <NiFragmentMaterial.h>
#include <NiMediaPaths_Win32.h>

NiMaterialToolkit* NiMaterialToolkit::ms_pkThis = NULL;
bool NiMaterialToolkit::ms_bMessageBoxesEnabled = false;

//--------------------------------------------------------------------------------------------------
unsigned int NiMaterialToolkit::ShaderErrorCallback(const char* pcError,
    NiShaderError eError, bool)
{
    if (ms_bMessageBoxesEnabled &&
        (NiRenderer::GetRenderer() != NULL ||
        eError != NISHADERERR_SHADERNOTFOUND))
    {
        NiMessageBox(pcError, "Shader Error");
    }

    NiOutputDebugString("ERROR: ");
    NiOutputDebugString(pcError);

    return 0;
}

//--------------------------------------------------------------------------------------------------
NiMaterialToolkit* NiMaterialToolkit::CreateToolkit()
{
    if (ms_pkThis)
        return ms_pkThis;

    ms_pkThis = NiNew NiMaterialToolkit;

    NiShaderFactory::RegisterErrorCallback(
        NiMaterialToolkit::ShaderErrorCallback);
    return ms_pkThis;
}

//--------------------------------------------------------------------------------------------------
void NiMaterialToolkit::DestroyToolkit()
{
    NiDelete ms_pkThis;
    ms_pkThis = NULL;
}

//--------------------------------------------------------------------------------------------------
void NiMaterialToolkit::EnableMessageBoxes(bool bEnable)
{
    ms_bMessageBoxesEnabled = bEnable;
}

//--------------------------------------------------------------------------------------------------
NiMaterialToolkit::NiMaterialToolkit() :
    m_pcMaterialLibPath(NULL),
    m_pcShaderFilePath(NULL),
    m_pcShaderProgramFilePath(NULL),
    m_bAppendSubdir(false),
    m_bUpdateShaderProgramDir(false)
{ /* */ }

//--------------------------------------------------------------------------------------------------
NiMaterialToolkit::NiMaterialToolkit(NiMaterialToolkit& kToolkit) :
    m_pcShaderFilePath(NULL),
    m_pcShaderProgramFilePath(NULL),
    m_bAppendSubdir(false),
    m_bUpdateShaderProgramDir(false)
{
    size_t stLen = strlen(kToolkit.m_pcMaterialLibPath) + 1;
    m_pcMaterialLibPath = NiAlloc(char, stLen);
    NiStrcpy(m_pcMaterialLibPath, stLen, kToolkit.m_pcMaterialLibPath);
}

//--------------------------------------------------------------------------------------------------
NiMaterialToolkit::~NiMaterialToolkit()
{
    UnIndex();
    NiFree(m_pcMaterialLibPath);
    NiFree(m_pcShaderFilePath);
    NiFree(m_pcShaderProgramFilePath);

#if defined(_USRDLL)
    NiShaderFactory* pkFactory = NiShaderFactory::GetInstance();
    if (pkFactory)
    {
        pkFactory->FreeLibraryDLLs();
    }
#endif

}

//--------------------------------------------------------------------------------------------------
NiMaterialToolkit* NiMaterialToolkit::GetToolkit()
{
    EE_ASSERT(ms_pkThis != NULL);
    return ms_pkThis;
}

//--------------------------------------------------------------------------------------------------
bool NiMaterialToolkit::ParseNPShaders(const char* pcLibraryPath,
    const char* pcShaderPath)
{
    NiShaderFactory* pkFactory = NiShaderFactory::GetInstance();
    EE_ASSERT(pkFactory);
    pkFactory->GetRendererString();

    // The "9" is used for all D3D renderers.
#ifdef NIRELEASE
    const char* pcExt = ".np9";
#else
#ifdef NISHIPPING
    const char* pcExt = ".sp9";
#else
    const char* pcExt = ".dp9";
#endif
#endif

    NiWin32FileFinder kFinder(pcLibraryPath, true, pcExt);

    bool bErrors = false;
    char strErrors[1024];
    NiSprintf(strErrors, 1024, "No shaders were parsed for the following shader libraries:\n");
    while (kFinder.HasMoreFiles())
    {
        NiWin32FoundFile* pkFoundFile = kFinder.GetNextFile();
        if (pkFoundFile)
        {
            unsigned int uiParsed = pkFactory->LoadAndRunParserLibrary(pkFoundFile->m_strPath,
                pcShaderPath, false);
            char strTemp[256];
            NiSprintf(strTemp, 256, "Parsed %d shaders!\n", uiParsed);
            NiOutputDebugString(strTemp);
            if (uiParsed == 0)
            {
                bErrors = true;
                NiSprintf(strErrors, 1024, "%s%s\n", strErrors, pkFoundFile->m_strPath);
            }
        }
    }

    if (bErrors == true)
    {
        NiOutputDebugString("WARNING:  Failure Parsing Files For Shader Libraries.  -  ");
        NiOutputDebugString(strErrors);
        NiOutputDebugString("\n");
    }

    return true;
}

//--------------------------------------------------------------------------------------------------
bool NiMaterialToolkit::LoadFromDLL(const char* pcMaterialPath)
{
    bool bReturnValue = true;

    EE_ASSERT(pcMaterialPath && strlen(pcMaterialPath) != 0);

    NiFree(m_pcMaterialLibPath);
    size_t stLen = strlen(pcMaterialPath) + 1;
    m_pcMaterialLibPath = NiAlloc(char, stLen);
    NiStrcpy(m_pcMaterialLibPath, stLen, pcMaterialPath);

    bReturnValue = LoadShaders();
    LoadMaterials();

    return bReturnValue;
}

//--------------------------------------------------------------------------------------------------
void NiMaterialToolkit::ReloadShaders()
{
    if (ms_pkThis)
    {
        NiMaterial::UnloadShadersForAllMaterials();
        ms_pkThis->LoadShaders();
    }
}

//--------------------------------------------------------------------------------------------------
void NiMaterialToolkit::UnloadShaders()
{
    if (ms_pkThis)
    {
        ms_pkThis->UnIndex();

        NiShaderFactory::GetInstance()->UnregisterAllLibraries();
        NiMaterial::UnloadShadersForAllMaterials();
    }
}

//--------------------------------------------------------------------------------------------------
void NiMaterialToolkit::UnIndex()
{
    unsigned int uiCount =  0;
    NiTMapIterator kIter = m_kMaterialDescCache.GetFirstPos();
    while (kIter != NULL)
    {
        const char* pcValue;
        NiMaterialDescContainer* pkContainer;
        m_kMaterialDescCache.GetNext(kIter, pcValue, pkContainer);
        if (pkContainer != NULL)
            NiDelete pkContainer;
        uiCount++;
    }

    m_kMaterialDescCache.RemoveAll();
}

//--------------------------------------------------------------------------------------------------
void NiMaterialToolkit::LoadMaterials()
{
    if (m_pcMaterialLibPath == NULL)
    {
        NiOutputDebugString("WARNING:  Failure Loading Shader Libraries."
            "  No material-capable renderer libraries have been loaded.\n");
        return;
    }

    char acLibraryPath[_MAX_PATH];
    EE_ASSERT(strlen(m_pcMaterialLibPath) < _MAX_PATH);

    NiStrcpy(acLibraryPath, _MAX_PATH, m_pcMaterialLibPath);

#if _MSC_VER == 1310 //VC7.1
    NiStrcat(acLibraryPath, _MAX_PATH, "\\VC71\\");
#elif _MSC_VER == 1400 //VC8.0
    NiStrcat(acLibraryPath, _MAX_PATH, "\\VC80\\");
#elif _MSC_VER == 1500 //VC9.0
    NiStrcat(acLibraryPath, _MAX_PATH, "\\VC90\\");
#elif _MSC_VER >= 1930
    NiStrcat(acLibraryPath, _MAX_PATH, "\\VC143\\");
#else
    #error Unsupported version of Visual Studio
#endif

#ifdef NIRELEASE
    const char* pcExt = ".nlm";
#else
#ifdef NISHIPPING
    const char* pcExt = ".slm";
#else
    const char* pcExt = ".dlm";
#endif
#endif

    NiWin32FileFinder kFinder(acLibraryPath, true, pcExt);

    bool bErrors = false;
    char strErrors[1024];
    NiSprintf(strErrors, 1024,
        "The following Gamebryo Material Libraries failed to load:\n");

    while (kFinder.HasMoreFiles())
    {
        NiWin32FoundFile* pkFoundFile = kFinder.GetNextFile();
        if (pkFoundFile)
        {
            if (!NiMaterialLibrary::LoadMaterialLibraryDLL(
                pkFoundFile->m_strPath))
            {
                NiOutputDebugString("Failed to load material library!");
                bErrors = true;
                NiSprintf(strErrors, 1024, "%s%s\n", strErrors,
                    pkFoundFile->m_strPath);
                continue;
            }
        }
    }

    if (bErrors == true)
    {
        NiOutputDebugString(
            "WARNING:  Failure Loading Material Libraries.  -  ");
        NiOutputDebugString(strErrors);
        NiOutputDebugString("\n");
    }
}

//--------------------------------------------------------------------------------------------------
bool NiMaterialToolkit::LoadShaders()
{
    if (m_pcMaterialLibPath == NULL)
    {
        NiOutputDebugString("WARNING:  Failure Loading Shader Libraries."
            "  No shader-capable renderer libraries have been loaded.\n");
        return false;
    }

    char acLibraryPath[_MAX_PATH];
    EE_ASSERT(strlen(m_pcMaterialLibPath) < _MAX_PATH);

    NiStrcpy(acLibraryPath, _MAX_PATH, m_pcMaterialLibPath);

#if _MSC_VER == 1310 //VC7.1
    NiStrcat(acLibraryPath, _MAX_PATH, "\\VC71\\");
#elif _MSC_VER == 1400 //VC8.0
    NiStrcat(acLibraryPath, _MAX_PATH, "\\VC80\\");
#elif _MSC_VER == 1500 //VC9.0
    NiStrcat(acLibraryPath, _MAX_PATH, "\\VC90\\");
#elif _MSC_VER >= 1930
    NiStrcat(acLibraryPath, _MAX_PATH, "\\VC143\\");
#else
    #error Unsupported version of Visual Studio
#endif

    NiShaderFactory* pkFactory = NiShaderFactory::GetInstance();
    if (pkFactory == NULL)
    {
        NiOutputDebugString("WARNING:  Failure Loading Shader Libraries."
            "  No shader-capable renderer libraries have been loaded.\n");
        return false;
    }

    ParseNPShaders(acLibraryPath, m_pcShaderFilePath);

    // The "9" is used for all D3D renderers.
#ifdef NIRELEASE
    const char* pcExt = ".nl9";
#else
#ifdef NISHIPPING
    const char* pcExt = ".sl9";
#else
    const char* pcExt = ".dl9";
#endif
#endif

    pkFactory->GetRendererString();

    unsigned int uiNumDirs = 1;
    NiWin32FileFinder kFinder(acLibraryPath, true, pcExt);
    const char* apcDirectories[1];
    apcDirectories[0] = m_pcShaderFilePath;

    bool bErrors = false;
    char strErrors[1024];
    NiSprintf(strErrors, 1024, "The following Gamebryo Shader Libraries failed to load:\n");

    while (kFinder.HasMoreFiles())
    {
        NiWin32FoundFile* pkFoundFile = kFinder.GetNextFile();
        if (pkFoundFile)
        {
            if (!pkFactory->LoadAndRegisterShaderLibrary(pkFoundFile->m_strPath, uiNumDirs,
                apcDirectories, false))
            {
                NiOutputDebugString("Failed to load shader library!");
                bErrors = true;
                NiSprintf(strErrors, 1024, "%s%s\n", strErrors, pkFoundFile->m_strPath);
                continue;
            }
        }
    }

    if (bErrors == true)
    {
        NiOutputDebugString("WARNING:  Failure Loading Shader Libraries.  -  ");
        NiOutputDebugString(strErrors);
        NiOutputDebugString("\n");
        return false;
    }

    return true;
}

//--------------------------------------------------------------------------------------------------
NiShaderDesc* NiMaterialToolkit::GetMaterialDesc(const char* pcName)
{
    NiMaterialDescContainer* pkContainer = NULL;

    m_kMaterialDescCache.GetAt(pcName, pkContainer);

    if (pkContainer)
    {
        return pkContainer->m_pkDesc;
    }

    for (unsigned int ui = 0;
        ui < NiMaterialLibrary::GetMaterialLibraryCount(); ui++)
    {
        NiMaterialLibrary* pkLibrary =
            NiMaterialLibrary::GetMaterialLibrary(ui);
        if (!pkLibrary)
            continue;

        NiShaderDesc* pkDesc = pkLibrary->GetFirstMaterialDesc();
        while (pkDesc)
        {
            if (strcmp(pkDesc->GetName(), pcName) == 0)
            {
                pkContainer = NiNew NiMaterialDescContainer();
                pkContainer->m_pkDesc = pkDesc;
                m_kMaterialDescCache.SetAt(pkDesc->GetName(), pkContainer);
                return pkDesc;
            }

            pkDesc = pkLibrary->GetNextMaterialDesc();
        }
    }

    return NULL;
}

//--------------------------------------------------------------------------------------------------
const char* NiMaterialToolkit::GetAppStringForMaterialDesc(const char* pcName)
{
    NiMaterialDescContainer* pkContainer = NULL;
    m_kMaterialDescCache.GetAt(pcName, pkContainer);
    if (pkContainer)
    {
        return (const char*) pkContainer->m_pcApplicationDescription;
    }
    else
    {
        return NULL;
    }
}

//--------------------------------------------------------------------------------------------------
bool NiMaterialToolkit::SetAppStringForMaterialDesc(const char* pcName,
    const char* pcAppDesc)
{
    NiMaterialDescContainer* pkContainer = NULL;
    m_kMaterialDescCache.GetAt(pcName, pkContainer);
    if (pkContainer && pcAppDesc && strlen(pcAppDesc) > 0)
    {
        NiFree(pkContainer->m_pcApplicationDescription);

        size_t stLen = strlen(pcAppDesc) + 1;
        pkContainer->m_pcApplicationDescription = NiAlloc(char, stLen);
        NiStrcpy(pkContainer->m_pcApplicationDescription, stLen, pcAppDesc);
        return true;
    }

    return false;
}

//--------------------------------------------------------------------------------------------------
NiMaterialToolkit::NiMaterialDescContainer::NiMaterialDescContainer()
{
    m_pkDesc = NULL;
    m_pcApplicationDescription = NULL;
}

//--------------------------------------------------------------------------------------------------
NiMaterialToolkit::NiMaterialDescContainer::~NiMaterialDescContainer()
{
    NiFree(m_pcApplicationDescription);
    m_pkDesc = NULL;
    m_pcApplicationDescription = NULL;
}

//--------------------------------------------------------------------------------------------------
void NiMaterialToolkit::SetShaderFileDirectory(const char* pcDir)
{
    NiFree(m_pcShaderFilePath);
    if (pcDir != NULL && *pcDir != '\0')
    {
        size_t stLen = strlen(pcDir) + 1;
        m_pcShaderFilePath = NiAlloc(char, stLen);
        NiStrcpy(m_pcShaderFilePath, stLen, pcDir);
    }
    else
    {
        m_pcShaderFilePath = NULL;
    }
}

//--------------------------------------------------------------------------------------------------
void NiMaterialToolkit::SetShaderProgramFileDirectory(const char* pcDir,
    bool bAppend)
{
    NiFree(m_pcShaderProgramFilePath);
    if (pcDir != NULL && *pcDir != '\0')
    {
        size_t stLen = strlen(pcDir) + 1;
        m_pcShaderProgramFilePath = NiAlloc(char, stLen);
        NiStrcpy(m_pcShaderProgramFilePath, stLen, pcDir);
    }
    else
    {
        m_pcShaderProgramFilePath = NULL;
    }

    m_bAppendSubdir = bAppend;

    m_bUpdateShaderProgramDir = true;
}

//--------------------------------------------------------------------------------------------------
void NiMaterialToolkit::UpdateShaderProgramFileDirectory()
{
    if (ms_pkThis->m_bUpdateShaderProgramDir)
    {
        if (ms_pkThis->m_bAppendSubdir)
        {
            NiShaderFactory* pkFactory = NiShaderFactory::GetInstance();
            if (pkFactory)
            {
                char acNewDir[_MAX_PATH];
                NiSprintf(acNewDir, _MAX_PATH, "%s\\%s",
                    ms_pkThis->m_pcShaderProgramFilePath,
                    pkFactory->GetRendererString());
                NiShaderFactory::AddShaderProgramFileDirectory(acNewDir);
            }
        }
        else
        {
            NiShaderFactory::AddShaderProgramFileDirectory(
                ms_pkThis->m_pcShaderProgramFilePath);
        }

        char acNewDir[_MAX_PATH];
        NiSprintf(acNewDir, _MAX_PATH, "%s\\Generated\\",
            ms_pkThis->m_pcShaderProgramFilePath);
        NiMaterial::SetDefaultWorkingDirectory(acNewDir);
        ms_pkThis->SetWorkingDirectoryForMaterials(acNewDir);
        ms_pkThis->CreateFragmentMaterialCaches();
        ms_pkThis->LoadFragmentMaterialCaches();
    }
}

//--------------------------------------------------------------------------------------------------
unsigned int NiMaterialToolkit::GetLibraryCount()
{
    return NiMaterialLibrary::GetMaterialLibraryCount();
}

//--------------------------------------------------------------------------------------------------
NiMaterialLibrary* NiMaterialToolkit::GetLibraryAt(unsigned int ui)
{
    return NiMaterialLibrary::GetMaterialLibrary(ui);
}

//--------------------------------------------------------------------------------------------------
void NiMaterialToolkit::SetWorkingDirectoryForMaterials(const char* pcDir)
{
    NiMaterial::SetWorkingDirectoryForAllMaterials(pcDir);
}

//--------------------------------------------------------------------------------------------------
void NiMaterialToolkit::LoadFragmentMaterialCaches()
{
    NiMaterial::BeginReadMaterialList();
    NiMaterialIterator kIter = NiMaterial::GetFirstMaterialIter();
    while (kIter)
    {
        NiFragmentMaterial* pkFragMaterial = NiDynamicCast(
            NiFragmentMaterial, NiMaterial::GetNextMaterial(kIter));

        if (pkFragMaterial)
        {
            for (unsigned int ui = 0; ui < NiGPUProgram::PROGRAM_MAX; ui++)
            {
                NiGPUProgramCache* pkCache =
                    pkFragMaterial->GetProgramCache(
                    (NiGPUProgram::ProgramType) ui);

                if (pkCache)
                    pkCache->Load();
            }
        }
    }
    NiMaterial::EndReadMaterialList();
}

//--------------------------------------------------------------------------------------------------
void NiMaterialToolkit::CreateFragmentMaterialCaches()
{
    NiMaterial::BeginReadMaterialList();
    NiMaterialIterator kIter = NiMaterial::GetFirstMaterialIter();
    while (kIter)
    {
        NiFragmentMaterial* pkFragMaterial = NiDynamicCast(
            NiFragmentMaterial, NiMaterial::GetNextMaterial(kIter));

        if (pkFragMaterial)
        {
            // Only VERTEX and PIXEL program caches are checked here because
            // SetDefaultProgramCache will set either all caches or no caches.
            // Thus, it is not necessary to check GEOMETRY program caches.
            // Doing so would cause caches to be unnecessarily recreated on
            // non-D3D10 platforms.
            if (!pkFragMaterial->GetProgramCache(
                    NiGPUProgram::PROGRAM_VERTEX) ||
                !pkFragMaterial->GetProgramCache(NiGPUProgram::PROGRAM_PIXEL))
            {
                NiRenderer* pkRenderer = NiRenderer::GetRenderer();
                if (pkRenderer)
                {
                    pkRenderer->SetDefaultProgramCache(pkFragMaterial);

                    pkFragMaterial->SetWorkingDirectory(
                        NiMaterial::GetDefaultWorkingDirectory());
                }
            }
        }
    }
    NiMaterial::EndReadMaterialList();
}

//--------------------------------------------------------------------------------------------------
bool NiMaterialToolkit::InitializeUsingDefaultPaths()
{
    // Get the EMERGENT_PATH variable and verify it exists
    char acEmergentPath[2048];
    bool emergentPathSet = NiMediaPaths_Win32::GetEmergentPath(
        acEmergentPath,
        2048);

    if (!emergentPathSet)
        return false;

    // Construct paths to the default shader file location and shader library location
    char acShaderFilePath[2048];
    NiStrcpy(acShaderFilePath, 2048, acEmergentPath);
    NiMediaPaths_Win32::AppendShaderPath(acShaderFilePath, 2048);

    char acShaderLibPath[2048];
    NiStrcpy(acShaderLibPath, 2048, acEmergentPath);
    NiMediaPaths_Win32::AppendShaderLibPath(acShaderLibPath, 2048);

    // Use the paths to initialize the toolkit
    SetShaderFileDirectory(acShaderFilePath);
    SetShaderProgramFileDirectory(acShaderFilePath, true);
    UpdateShaderProgramFileDirectory();

    LoadFromDLL(acShaderLibPath);

    return true;
}

//--------------------------------------------------------------------------------------------------

