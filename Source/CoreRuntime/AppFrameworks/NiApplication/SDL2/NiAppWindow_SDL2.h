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

#pragma once
#ifndef NIAPPWINDOW_SDL2_H
#define NIAPPWINDOW_SDL2_H

#include <NiUniversalTypes.h>
#include <NiMemManager.h>
#include <NiMemObject.h>

//--------------------------------------------------------------------------------------------------
class NiAppWindow : public NiMemObject
{
public:
    NiAppWindow(
        const char* pcWindowCaption,       // caption of window
        unsigned int uiWidth,              // client window width
        unsigned int uiHeight,             // client window height
        bool bOnTop = false,
        efd::UInt32 ulWindowFlags =      // client window style
            SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_OPENGL);
    ~NiAppWindow();

    void SetWindowReference(NiWindowRef pWnd);
    NiWindowRef GetWindowReference();

    static const char* GetWindowClassName();
    efd::UInt32 GetWindowFlags();
    void SetWindowFlags(efd::UInt32 ulWindowStyle);

    // window caption
    void SetWindowCaption(char* pcCaption);
    const char* GetWindowCaption() const;

    // window size
    void SetWidth(unsigned int uiWidth);
    unsigned int GetWidth() const;
    void SetHeight(unsigned int uiHeight);
    unsigned int GetHeight() const;

    void SetWindowOnTop(bool bWindowOnTop);
    bool GetWindowOnTop() const;

    // Window creation
    virtual NiWindowRef CreateMainWindow(int iWinMode,
        NiWindowRef hWnd = NULL);

protected:
    NiWindowRef m_pWnd;

    static char ms_acWindowClassName[];

    char* m_pcWindowCaption;
    unsigned int m_uiWidth;
    unsigned int m_uiHeight;
    efd::UInt32 m_unWindowFlags;
    bool m_bOnTop;
};

//--------------------------------------------------------------------------------------------------

#include "NiAppWindow_SDL2.inl"

#endif // NIAPPWINDOW_SDL2_H
