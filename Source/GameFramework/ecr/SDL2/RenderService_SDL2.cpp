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
#include "ecrPCH.h"

#include "../RenderService.h"

#include "efd/SDL2/SDL2PlatformService.h"

#include <efd/IConfigManager.h>

#include <NiRendererSettings.h>

#ifdef EE_PLATFORM_WIN32
#include <NiSettingsDialog.h>
#include <NiBaseRendererSetup.h>
#include <SDL_syswm.h>
#endif

using namespace egf;
using namespace efd;
using namespace ecr;

//------------------------------------------------------------------------------------------------
void RenderService::InternalDestructor()
{
    m_spRenderer->RemoveLostDeviceNotificationFunc(&RenderService::OnDeviceLost);
    m_spRenderer->RemoveResetNotificationFunc(&RenderService::OnDeviceReset);
}

//------------------------------------------------------------------------------------------------
bool RenderService::CreateRenderer()
{
    // If you are using the SDL2PlatformService for your main message loop then we can
    // automatically figure out the correct parent window handle to use.
    SDL2PlatformService* pWin32 =
        m_pServiceManager->GetSystemServiceAs<SDL2PlatformService>();
    if (!m_parentHandle && pWin32)
    {
        m_parentHandle = pWin32->GetWindowRef();
    }

    IConfigManager* pConfigManager = m_pServiceManager->GetSystemServiceAs<IConfigManager>();

    NiRendererSettings settings;
    char settingsFile[EE_MAX_PATH];
    bool validFile = efd::PathUtils::GetExecutableDirectory(settingsFile, sizeof(settingsFile));

    // Config manager/command line override settings file settings.
    if (validFile)
    {
        NiStrcat(settingsFile, sizeof(settingsFile), "AppSettings.ini");
        settings.LoadSettings(settingsFile);
    }
    settings.LoadFromConfigManager(pConfigManager);

    if (settings.m_bRendererDialog && pWin32)
    {
        SDL_SysWMinfo wmInfo;
        SDL_VERSION(&wmInfo.version);
        SDL_GetWindowWMInfo(m_parentHandle, &wmInfo);
        HWND hwnd = wmInfo.info.win.window;

        NiSettingsDialog dialog(&settings);
        if (dialog.InitDialog(GetModuleHandle(NULL)) &&
            dialog.ShowDialog(hwnd, (NiAcceleratorRef)m_parentHandle))
        {
            if (settings.m_bSaveSettings && validFile)
                settings.SaveSettings(settingsFile);
        }
    }

    if (m_parentHandle && settings.m_uiScreenHeight && settings.m_uiScreenWidth)
    {
        SDL_SetWindowSize(m_parentHandle, settings.m_uiScreenWidth, settings.m_uiScreenHeight);
    }

    m_spRenderer = NiBaseRendererSetup::CreateRenderer(
        &settings,
        m_parentHandle,
        m_parentHandle);

    m_spRenderer->AddLostDeviceNotificationFunc(&RenderService::OnDeviceLost, this);
    m_spRenderer->AddResetNotificationFunc(&RenderService::OnDeviceReset, this);


    return (m_spRenderer != NULL);
}

//------------------------------------------------------------------------------------------------
RenderSurfacePtr RenderService::CreateRenderSurface(NiWindowRef windowHandle)
{
    if (!windowHandle)
        windowHandle = m_parentHandle;

    if (windowHandle != m_parentHandle &&
        !m_spRenderer->CreateWindowRenderTargetGroup(windowHandle))
    {
        return false;
    }


    // Create a new render surface entry for the back-buffer.
    RenderSurfacePtr spSurface = NiNew RenderSurface(windowHandle, this);

    if (windowHandle == m_parentHandle)
    {
        spSurface->GetSceneRenderClick()->SetRenderTargetGroup(
            m_spRenderer->GetDefaultRenderTargetGroup());
    }
    else
    {
        spSurface->GetSceneRenderClick()->SetRenderTargetGroup(
            m_spRenderer->GetWindowRenderTargetGroup(windowHandle));
    }

    if (!m_pActiveSurface)
        SetActiveRenderSurface(spSurface);

    return spSurface;
}

//------------------------------------------------------------------------------------------------
bool RenderService::DestroyRenderSurface(RenderSurface* pSurface)
{
    m_spRenderer->ReleaseWindowRenderTargetGroup(pSurface->GetWindowRef());

    return true;
}

//------------------------------------------------------------------------------------------------
bool RenderService::RecreateRenderSurface(RenderSurface* pSurface)
{
    EE_ASSERT(pSurface);

    // If the specified surface maps to the back-buffer of the device then
    // tell the NiRenderer to re-create the device. If the surface maps to a
    // swap chain, destroy the swap chain and re-create it.

    if (pSurface->GetRenderTargetGroup() == m_spRenderer->GetDefaultRenderTargetGroup())
    {
        // Do nothing; the device has already recreated itself and its swap chain.
    }
    else
    {
        if (!m_spRenderer->LostDeviceRestore())
            return false;

        // Swap chain.
        if (!m_spRenderer->RecreateWindowRenderTargetGroup(pSurface->GetWindowRef()))
        {
            return false;
        }
    }

    // Make sure to fix the aspect ratio on the default camera if it's present.
    NiRenderTargetGroup* pRenderTarget = pSurface->GetRenderTargetGroup();
    EE_ASSERT(pRenderTarget);

    NiCamera* pCamera = pSurface->GetCamera();
    if (pCamera)
    {
        float width = (float)pRenderTarget->GetWidth(0);
        float height = (float)pRenderTarget->GetHeight(0);

        float aspectRatio = width / height;

        pCamera->AdjustAspectRatio(aspectRatio);
    }

    // Notify any callbacks that the render surface has been recreated.
    for (DelegateList::iterator i = m_renderServiceDelegates.begin();
         i != m_renderServiceDelegates.end(); ++i)
    {
        (*i)->OnSurfaceRecreated(this, pSurface);
    }

    return true;
}

//------------------------------------------------------------------------------------------------
