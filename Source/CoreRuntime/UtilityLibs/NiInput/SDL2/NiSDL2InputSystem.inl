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
//--------------------------------------------------------------------------------------------------
// NiSDL2InputSystem inline functions
//--------------------------------------------------------------------------------------------------
// SDL2CreateParams
//--------------------------------------------------------------------------------------------------
inline NiSDL2InputSystem::SDL2CreateParams::SDL2CreateParams() :
    NiInputSystem::CreateParams(0, false, false, MAX_GAMEPADS),
    m_pCB(NULL), m_pWindow(NULL)
{
}

//--------------------------------------------------------------------------------------------------
inline NiSDL2InputSystem::SDL2CreateParams::SDL2CreateParams(SDL2InputCallback* pCb, SDL_Window* pWindow,
    NiActionMap* pkActionMap, unsigned int uiKeyboard,
    unsigned int uiMouse, unsigned int uiGamePads, int iAxisRangeLow,
    int iAxisRangeHigh) :
    NiInputSystem::CreateParams(pkActionMap, uiKeyboard, uiMouse, uiGamePads,
    iAxisRangeLow, iAxisRangeHigh),
    m_pCB(pCb), m_pWindow(pWindow)
{
}

//--------------------------------------------------------------------------------------------------
inline NiSDL2InputSystem::SDL2CreateParams::~SDL2CreateParams()
{
}

//--------------------------------------------------------------------------------------------------
inline NiSDL2InputSystem::SDL2InputCallback* NiSDL2InputSystem::SDL2CreateParams::GetInputCallbackReference()
const
{
    return m_pCB;
}

//--------------------------------------------------------------------------------------------------
inline SDL_Window* NiSDL2InputSystem::SDL2CreateParams::GetOwnerWindow()
const
{
    return m_pWindow;
}

//--------------------------------------------------------------------------------------------------
inline void NiSDL2InputSystem::SDL2CreateParams::SetOwnerWindow(SDL_Window* hWnd)
{
    m_pWindow = hWnd;
}

//--------------------------------------------------------------------------------------------------
inline void NiSDL2InputSystem::SDL2CreateParams::SetInputCallbackReference(SDL2InputCallback* pCb)
{
    m_pCB = pCb;
}

//--------------------------------------------------------------------------------------------------
// SDL2Description
//--------------------------------------------------------------------------------------------------
inline NiSDL2InputSystem::SDL2Description::SDL2Description() :
    NiInputDevice::Description()
{
    
}

//--------------------------------------------------------------------------------------------------
inline NiSDL2InputSystem::SDL2Description::SDL2Description(
    NiInputDevice::Type eType, unsigned int uiPort, unsigned int uiSlot,
    const char* pcName) :
    NiInputDevice::Description(eType, uiPort, uiSlot, pcName)
{
}

//--------------------------------------------------------------------------------------------------
inline NiSDL2InputSystem::SDL2Description::~SDL2Description()
{
}

//--------------------------------------------------------------------------------------------------
inline void NiSDL2InputSystem::CopySDLProcCallbackToOutput(NiSDL2InputSystem::SDL2InputCallback* pCb)
{
    *pCb = &NiSDL2InputSystem::SDLProc;
}

//--------------------------------------------------------------------------------------------------
inline void NiSDL2InputSystem::SetOwnerWindow(SDL_Window* hWnd)
{
    m_pOwnerWnd = hWnd;
}

//--------------------------------------------------------------------------------------------------
