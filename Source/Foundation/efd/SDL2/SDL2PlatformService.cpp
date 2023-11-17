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

#include <efd/SDL2/SDL2PlatformService.h>


EE_IMPLEMENT_CONCRETE_CLASS_INFO(efd::SDL2PlatformService);

static const efd::UInt32 MAX_LOADSTRING = 100;

//------------------------------------------------------------------------------------------------
efd::SDL2PlatformService::SDL2PlatformService(
#ifdef _WIN32
    HINSTANCE hInstance,
#endif
    LPTSTR lpCmdLine,
    efd::SInt32 m_nCmdShow) :
      m_lpCmdLine(lpCmdLine)
    , m_nCmdShow(m_nCmdShow)
    , m_hWnd(0)
    , m_windowWidth(1024)
    , m_windowHeight(768)
    , m_windowX(0)
    , m_windowY(0)
    , m_isDirty(false)
    , m_maxMessagesPerTick(5)
    , m_hIcon(nullptr)
#ifdef _WIN32
    , m_hInstance(hInstance)
#endif

{
    // If this default priority is changed, also update the service quick reference documentation
    m_defaultPriority = 6500;
}

//------------------------------------------------------------------------------------------------
efd::SDL2PlatformService::~SDL2PlatformService()
{
}

//------------------------------------------------------------------------------------------------
const char* efd::SDL2PlatformService::GetDisplayName() const
{
    return "SDL2PlatformService";
}

//------------------------------------------------------------------------------------------------
bool efd::SDL2PlatformService::InitInstance(Uint32 n_nFlags)
{
    m_hWnd = SDL_CreateWindow(m_strWindowTitle.c_str(), m_windowX, m_windowY, m_windowWidth, m_windowHeight, n_nFlags);

    if (!m_hWnd)
        return false;

    SDL_SetWindowIcon(m_hWnd, m_hIcon);
    return true;
}

//------------------------------------------------------------------------------------------------
efd::WindowRef efd::SDL2PlatformService::GetWindowRef() const
{
    return m_hWnd;
}

//------------------------------------------------------------------------------------------------
void efd::SDL2PlatformService::SetWindowTitle(const efd::utf8string& strTitle)
{
    m_strWindowTitle = strTitle;
}

//------------------------------------------------------------------------------------------------
void efd::SDL2PlatformService::SetWindowTitle(efd::UInt32 titleResourceID)
{
#error a

#ifdef _WIN32
    TCHAR buffer[MAX_LOADSTRING];
    LoadString(m_hInstance, titleResourceID, buffer, MAX_LOADSTRING);
    m_strWindowTitle = buffer;
#else
#error "TODO"
#endif
}

//------------------------------------------------------------------------------------------------
void efd::SDL2PlatformService::SetWindowIcon(efd::UInt32 iconID)
{
#error a

#ifdef _WIN32
    // TODO...
#else
    return false;
#endif

/*   
    if (m_hIcon)
    {
        SDL_FreeSurface(m_hIcon);
        m_hIcon = nullptr;
    }
*/

}

//------------------------------------------------------------------------------------------------
void efd::SDL2PlatformService::SetWindowWidth(efd::UInt32 width)
{
    m_windowWidth = width;
    m_isDirty = true;
}

//------------------------------------------------------------------------------------------------
void efd::SDL2PlatformService::SetWindowHeight(efd::UInt32 height)
{
    m_windowHeight = height;
    m_isDirty = true;
}

//------------------------------------------------------------------------------------------------
void efd::SDL2PlatformService::SetWindowX(efd::UInt32 left)
{
    m_windowX = left;
    m_isDirty = true;
}

//------------------------------------------------------------------------------------------------
void efd::SDL2PlatformService::SetWindowY(efd::UInt32 top)
{
    m_windowY = top;
    m_isDirty = true;
}

//------------------------------------------------------------------------------------------------
efd::SyncResult efd::SDL2PlatformService::OnPreInit(efd::IDependencyRegistrar* pDependencyRegistrar)
{
    EE_UNUSED_ARG(pDependencyRegistrar);

    // Initialize window instance.
    if (InitInstance(m_nCmdShow))
    {
        return efd::SyncResult_Success;
    }

    return efd::SyncResult_Failure;
}

//------------------------------------------------------------------------------------------------
efd::AsyncResult efd::SDL2PlatformService::OnTick()
{
    efd::AsyncResult result = efd::AsyncResult_Pending;

    if (m_isDirty)
    {
        SDL_SetWindowPosition(m_hWnd, m_windowX, m_windowY);
        SDL_SetWindowSize(m_hWnd, m_windowWidth, m_windowHeight);
        m_isDirty = false;
    }

    UInt32 cMessage = 0;
    // message pump
    SDL_Event evt;
    while (cMessage < m_maxMessagesPerTick && SDL_PollEvent(&evt))
    {
        ++cMessage;

        if (evt.type == SDL_QUIT)
        {
            // Tell all the other services to cleanup and shut down:
            m_pServiceManager->Shutdown();

            result = efd::AsyncResult_Complete;
        }
        else if (evt.type == SDL_APP_DIDENTERBACKGROUND ||
            (evt.type == SDL_WINDOWEVENT && evt.window.event == SDL_WINDOWEVENT_SHOWN))
        {
            // Resume normal tick rate
            m_pServiceManager->UseDeactivatedSleepTime(false);
        }
        else if (evt.type == SDL_APP_DIDENTERFOREGROUND ||
            (evt.type == SDL_WINDOWEVENT && evt.window.event == SDL_WINDOWEVENT_HIDDEN))
        {
            // Reduce tick rate to share CPU with other apps
            m_pServiceManager->UseDeactivatedSleepTime(true);
        }
        else if (evt.type == SDL_KEYDOWN && evt.key.state && SDL_PRESSED && evt.key.keysym.scancode == SDL_SCANCODE_ESCAPE)
        {
            SDL_DestroyWindow(m_hWnd);
            m_hWnd = nullptr;
        }
    }

    return result;
}

//------------------------------------------------------------------------------------------------
