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
#ifndef NIINPUTSDL2KEYBOARD_H
#define NIINPUTSDL2KEYBOARD_H

#include "NiInputKeyboard.h"

class NIINPUT_ENTRY NiInputSDL2Keyboard : public NiInputKeyboard
{
    NiDeclareRTTI;


public:
    NiInputSDL2Keyboard(NiInputDevice::Description* pkDescription,
        SDL_Window* target, unsigned int uiUsage);
    virtual ~NiInputSDL2Keyboard();

    //
    virtual NiInputErr UpdateDevice();
    virtual NiInputErr HandleRemoval();
    virtual NiInputErr HandleInsertion();

    // *** begin Emergent internal use only ***
    //
    void UpdateKeymap();
    // *** end Emergent internal use only ***

protected:
    inline NiInputKeyboard::KeyCode SDLKeyToNiKey(SDL_Scancode button);
    void UpdateKey(SDL_Scancode scancode, efd::UInt16 mod, bool pressed);
    void InitializeMapping();


    unsigned int m_uiFlags;
    SDL_Window* m_pTarget;
    const efd::UInt8* m_anKeyStatus;
    NiInputKeyboard::KeyCode m_aeMapping[SDL_NUM_SCANCODES];

    void UnAcquire();
    NiInputErr Acquire();

};

NiSmartPointer(NiInputSDL2Keyboard);

#include "NiInputSDL2Keyboard.inl"

#endif // NIINPUTSDL2KEYBOARD_H

