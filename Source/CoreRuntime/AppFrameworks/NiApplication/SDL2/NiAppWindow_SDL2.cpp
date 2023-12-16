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

// Precompiled Header
#include "NiApplicationPCH.h"

#include <NiOS.h>

#include "NiAppWindow_SDL2.h"
#include "NiApplication.h"

char NiAppWindow::ms_acWindowClassName[] = "Gamebryo Application";

//--------------------------------------------------------------------------------------------------
NiAppWindow::NiAppWindow(const char* pcWindowCaption, unsigned int uiWidth,
    unsigned int uiHeight, bool bOnTop, efd::UInt32 ulWindowFlags)
{
    // text for window caption
    if (pcWindowCaption && pcWindowCaption[0])
    {
        size_t stLen = strlen(pcWindowCaption) + 1;
        m_pcWindowCaption = NiAlloc(char, stLen);
        NiStrcpy(m_pcWindowCaption, stLen, pcWindowCaption);
    }
    else
    {
        size_t stLen = strlen(ms_acWindowClassName) + 1;
        m_pcWindowCaption = NiAlloc(char, stLen);
        NiStrcpy(m_pcWindowCaption, stLen, ms_acWindowClassName);
    }

    // window references
    m_pWnd = NULL;

    // client window dimensions
    m_uiWidth = uiWidth;
    m_uiHeight = uiHeight;

    m_unWindowFlags = ulWindowFlags;
    m_bOnTop = bOnTop;
}

//--------------------------------------------------------------------------------------------------
NiAppWindow::~NiAppWindow()
{
    NiFree(m_pcWindowCaption);

    m_pWnd = NULL;
}

//--------------------------------------------------------------------------------------------------
void NiAppWindow::SetWindowCaption(char* pcCaption)
{
    NiFree(m_pcWindowCaption);
    const size_t stStrLength = strlen(pcCaption) + 1;
    m_pcWindowCaption = NiAlloc(char, stStrLength);
    NiStrcpy(m_pcWindowCaption, stStrLength, pcCaption);

    // Call the appropriate function to set the window title
    SDL_SetWindowTitle(m_pWnd, m_pcWindowCaption);
}

//--------------------------------------------------------------------------------------------------
NiWindowRef NiAppWindow::CreateMainWindow(int, NiWindowRef hWnd)
{
    // If we have passed in the 'always on top' option on the command line
    // it should over ride the values.
    NiCommand* pkCommand = NiApplication::ms_pkApplication->GetCommand();
    if (pkCommand)
    {
        bool bValue = false;
        if (pkCommand->Boolean("ontop", bValue))
        {
            SetWindowOnTop(bValue);
        }
    }

    NiWindowRef pWnd = NULL;

    SDL_Point wh, xy;

#ifdef EE_PLATFORM_WIN32
    DWORD dwMsStyles = 0;
    if (!(GetWindowFlags() & SDL_WINDOW_BORDERLESS) && !(GetWindowFlags() & SDL_WINDOW_FULLSCREEN))
        dwMsStyles |= WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_OVERLAPPED;
    if (GetWindowFlags() & SDL_WINDOW_RESIZABLE)
        dwMsStyles |= WS_OVERLAPPEDWINDOW;

    // require the renderer window to have the specified client area
    RECT kRect = { 0, 0, (LONG)GetWidth(), (LONG)GetHeight() };
    AdjustWindowRectEx(&kRect, dwMsStyles,
        FALSE,
        0);

    wh.x = kRect.right - kRect.left;
    wh.y = kRect.bottom - kRect.top;
#else
    wh.x = GetWidth();
    wh.y = GetHeight();
#endif

    if (GetWindowFlags() & SDL_WINDOW_FULLSCREEN || GetWindowFlags() & SDL_WINDOW_FULLSCREEN_DESKTOP)
    {
        xy.x = 0;
        xy.y = 0;
    }
    else
    {
        xy.x = SDL_WINDOWPOS_CENTERED;
        xy.y = SDL_WINDOWPOS_CENTERED;
    }

    pWnd = SDL_CreateWindow(m_pcWindowCaption, xy.x, xy.y, wh.x, wh.y, GetWindowFlags());
    SetWindowReference(pWnd);

    if (GetWindowOnTop())
        SDL_SetWindowAlwaysOnTop(pWnd, SDL_TRUE);

    return pWnd;
}

//--------------------------------------------------------------------------------------------------

