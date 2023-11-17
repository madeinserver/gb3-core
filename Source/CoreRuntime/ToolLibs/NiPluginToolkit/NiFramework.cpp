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

#include "NiFramework.h"
#include <NiStaticDataManager.h>
#include "NiSharedDataList.h"
#include "NiPluginManager.h"
#include <NiVersion.h>
#include "NiScriptTemplateManager.h"
#include "NiDefaultScriptReader.h"
#include "NiDefaultScriptWriter.h"
#include "NiPluginToolkitSDM.h"
#include <NiMemTracker.h>
#include <NiStandardAllocator.h>

static NiPluginToolkitSDM NiPluginToolkitSDMObject;

NiImplementRootRTTI(NiFramework);

NiFramework* NiFramework::ms_pkThis = NULL;

//--------------------------------------------------------------------------------------------------
NiFramework& NiFramework::GetFramework()
{
    EE_ASSERT(ms_pkThis != NULL);
    return *ms_pkThis;
}

//--------------------------------------------------------------------------------------------------
void NiFramework::InitFramework(const char* pcAppPath, const char* pcAppName,
    const char* pcAppVersion, bool bFloodgateParallelExecution)
{
    EE_ASSERT(ms_pkThis == NULL);

    NiInitOptions* pkOptions = NiExternalNew NiInitOptions();

    pkOptions->SetFloodgateParallelExecution(bFloodgateParallelExecution);

    NiInit(pkOptions);

    NiMemTracker* pkTracker = NiMemTracker::Get();
    if (pkTracker)
    {
        // Disable the full memory log, as it can significantly slow down plugin loading times
        // It can be enabled when the overview reports
        pkTracker->SetWriteToLog(false);
    }

    NiSharedDataList::CreateInstance();
    NiPluginManager::CreateInstance();
    NiScriptTemplateManager::CreateInstance();
    NiScriptTemplateManager* pkScriptManager =
        NiScriptTemplateManager::GetInstance();
    pkScriptManager->AddScriptReader(NiNew NiDefaultScriptReader());
    pkScriptManager->AddScriptWriter(NiNew NiDefaultScriptWriter());

    ms_pkThis = NiNew NiFramework(pcAppPath, pcAppName, pcAppVersion);
}

//--------------------------------------------------------------------------------------------------
void NiFramework::ShutdownFramework()
{
    EE_ASSERT(ms_pkThis != NULL);

    NiDelete ms_pkThis;
    ms_pkThis = NULL;

    NiScriptTemplateManager::DestroyInstance();
    NiSharedDataList::DestroyInstance();
    NiPluginManager::DestroyInstance();
    const NiInitOptions* pkOptions = NiStaticDataManager::GetInitOptions();
    NiShutdown();
    NiExternalDelete pkOptions;
}

//--------------------------------------------------------------------------------------------------
NiFramework::NiFramework(const char* pcAppPath, const char* pcAppName,
    const char* pcAppVersion)
{
    m_strAppPath = pcAppPath;
    m_strAppName = pcAppName;
    m_strAppVersion = pcAppVersion;
}

//--------------------------------------------------------------------------------------------------
const NiString& NiFramework::GetAppPath()
{
    return m_strAppPath;
}

//--------------------------------------------------------------------------------------------------
const NiString& NiFramework::GetAppName()
{
    return m_strAppName;
}

//--------------------------------------------------------------------------------------------------
const NiString& NiFramework::GetAppVersion()
{
    return m_strAppVersion;
}

//--------------------------------------------------------------------------------------------------
const char* NiFramework::GetNiVersion()
{
    return GAMEBRYO_SDK_VERSION_STRING;
}

//--------------------------------------------------------------------------------------------------
NiSharedDataList& NiFramework::GetSharedDataList() const
{
    NiSharedDataList* pkSharedDataList = NiSharedDataList::GetInstance();
    EE_ASSERT(pkSharedDataList);
    return *pkSharedDataList;
}

//--------------------------------------------------------------------------------------------------
NiBatchExecutionResultPtr NiFramework::ExecuteScript(NiScriptInfo* pkScript)
{
    NiPluginManager* pkPluginManager = NiPluginManager::GetInstance();
    EE_ASSERT(pkPluginManager);
    return pkPluginManager->ExecuteScript(pkScript);
}

//--------------------------------------------------------------------------------------------------
