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

#include "NiSceneGraphUpdateServicePCH.h"

#include "NiEditorSceneGraphUpdate.h"
#include "NiSceneGraphUpdateService.h"

#include "NiSourceTexture.h"

#include <efdNetwork/ChannelManagerService.h>

#include <efd/ServiceManager.h>

#if defined(WIN32)
#include <efdNetwork/BridgeService.h>
#include <efd/DynamicModule.h>
#endif

//--------------------------------------------------------------------------------------------------
NiEditorSceneGraphUpdate::NiEditorSceneGraphUpdate()
{
    m_uiProcessingMessageCount = 0;
}

//--------------------------------------------------------------------------------------------------
NiEditorSceneGraphUpdate::~NiEditorSceneGraphUpdate()
{
}

//--------------------------------------------------------------------------------------------------
void NiEditorSceneGraphUpdate::InitializeEditor(
    NiEditorSceneGraphUpdate* pEditorSceneGraphUpdate /* = NULL */,
    efd::ServiceManager* pServiceManager /* = NULL */)
{
    NiSourceTexture::SetDestroyAppDataFlag(false);

    // Allocate the editor if one is not already provided
    if (!pEditorSceneGraphUpdate)
    {
        pEditorSceneGraphUpdate = NiNew NiEditorSceneGraphUpdate;
    }

    NiSceneGraphUpdate::Initialize(pEditorSceneGraphUpdate, pServiceManager);

    if (!pServiceManager)
    {
        pServiceManager = GetInstance()->m_pServiceManager;
    }

    // Register dependent services
    efd::ChannelManagerService* pChannelManagerService =
        pServiceManager->GetSystemServiceAs<efd::ChannelManagerService>();
    if (!pChannelManagerService)
    {
        pServiceManager->RegisterSystemService(EE_NEW efd::ChannelManagerService());
    }

    // Register the update service
    pServiceManager->RegisterSystemService(EE_NEW NiSceneGraphUpdateService(false));
}

//--------------------------------------------------------------------------------------------------
void NiEditorSceneGraphUpdate::ShutdownEditor()
{
    NiSceneGraphUpdate::GetInstance()->SendImmediate(NiSceneGraphUpdate::MESSAGE_EDITOR_STOPPED);

    NiSceneGraphUpdate::Shutdown();
}

//--------------------------------------------------------------------------------------------------
bool NiEditorSceneGraphUpdate::Update()
{
    // If we own the service manager we need to update it.
    if (m_bOwnsServiceManager)
    {
        if (!UpdateServiceManager())
            return false;
    }

    if (!IsReady())
        return false;

    return true;
}

//--------------------------------------------------------------------------------------------------
