// EMERGENT GAME TECHNOLOGIES PROPRIETARY INFORMATION
//
// This software is supplied under the terms of a license agreement or
// nondisclosure agreement with Emergent Game Technologies and may not
// be copied or disclosed except in accordance with the terms of that
// agreement.
//
//		Copyright (c) 2023 Arves100/Made In Server developers.
//      Copyright (c) 1996-2009 Emergent Game Technologies.
//      All Rights Reserved.
//
// Emergent Game Technologies, Calabasas, CA 91302
// http://www.emergent.net

#pragma once
#ifndef NISDL2INPUTSYSTEM_H
#define NISDL2INPUTSYSTEM_H

#include "NiInputSystem.h"

#include <NiSystem.h>

class NiInputSDL2Mouse;
class NiInputSDL2Keyboard;

class NIINPUT_ENTRY NiSDL2InputSystem : public NiInputSystem
{
    NiDeclareRTTI;

public:
    typedef bool (NiSDL2InputSystem::*SDL2InputCallback)(SDL_Event* evt);

    class NIINPUT_ENTRY SDL2CreateParams : public NiInputSystem::CreateParams
    {
        NiDeclareRTTI;

    public:
        SDL2CreateParams();
        SDL2CreateParams(SDL2InputCallback* pCb, SDL_Window* pWindow, 
            NiActionMap* pkActionMap, unsigned int uiKeyboard,
            unsigned int uiMouse, unsigned int uiGamePads, int iAxisRangeLow,
            int iAxisRangeHigh);
        virtual ~SDL2CreateParams();

        inline SDL2InputCallback* GetInputCallbackReference() const;
        inline SDL_Window* GetOwnerWindow() const;

        // *** begin Emergent internal use only ***
        inline void SetOwnerWindow(SDL_Window* hWnd);
        inline void SetInputCallbackReference(SDL2InputCallback* pCb);
        // *** end Emergent internal use only ***

    protected:
        SDL2InputCallback* m_pCB;
        SDL_Window* m_pWindow;
    };

    class SDL2Description : public NiInputDevice::Description
    {
    public:
        SDL2Description();
        SDL2Description(NiInputDevice::Type eType, unsigned int uiPort,
            unsigned int uiSlot, const char* pcName);
        ~SDL2Description();
    };

public:
    // The constructor should NEVER be called by anyone but the
    // static NiInputSystem::Create function! It's public due to
    // the static function needing to access it.
    NiSDL2InputSystem();

    virtual ~NiSDL2InputSystem();


    // Gamepad - will attempt to open the gamepad at the given
    // port and slot
    virtual NiInputGamePad* OpenGamePad(unsigned int uiPort,
        unsigned int uiSlot);

    // Mouse - will return the first mouse found, searching each port
    // in numerical order.
    virtual NiInputMouse* OpenMouse();

    // Keyboard - will return the first keyboard found, searching each
    // port in numerical order.
    virtual NiInputKeyboard* OpenKeyboard();

    //
    virtual NiInputErr UpdateAllDevices();
    virtual NiInputErr UpdateActionMap();
    virtual NiInputErr HandleDeviceChanges();
    virtual NiInputErr HandleRemovals();
    virtual NiInputErr HandleInsertions();

    virtual NiInputErr CheckEnumerationStatus();

    virtual void SuspendInput();
    virtual void ResumeInput();

    // *** begin Emergent internal use only ***
    //
    inline void CopySDLProcCallbackToOutput(SDL2InputCallback* pCb);
    inline void SetOwnerWindow(SDL_Window* hWnd);

    virtual NiInputErr Initialize(CreateParams* pkParams);
    virtual NiInputErr Shutdown();
    // *** end Emergent internal use only ***
protected:
    bool SDLProc(SDL_Event* evt);
    SDL_Window* m_pOwnerWnd;

    friend NiInputSystem* NiInputSystem::Create(CreateParams* pkParams);
};

NiSmartPointer(NiSDL2InputSystem);

#include "NiSDL2InputSystem.inl"

#endif  //#ifndef NISDL2INPUTSYSTEM_H