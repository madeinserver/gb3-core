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
#ifndef NIINPUTSDL2MOUSE_H
#define NIINPUTSDL2MOUSE_H

#include "NiInputMouse.h"

class NIINPUT_ENTRY NiInputSDL2Mouse : public NiInputMouse
{
    NiDeclareRTTI;

public:
    NiInputSDL2Mouse(NiInputDevice::Description* pkDescription, 
        SDL_Window* target, unsigned int uiUsage);
    ~NiInputSDL2Mouse();


    //
    virtual NiInputErr UpdateDevice();
    virtual NiInputErr HandleRemoval();
    virtual NiInputErr HandleInsertion();

    // *** begin Emergent internal use only ***
    //
    // *** end Emergent internal use only ***

protected:
    inline NiInputMouse::Button SDLKeyToNiKey(efd::UInt8 button);
    void InitializeMapping();

    unsigned int m_uiFlags;
    unsigned int m_uiButtonTouchMask;
    SDL_Window* m_pTarget;
    NiInputMouse::Button m_aeMapping[6];

    void UpdateButton(efd::UInt8 buttonId, bool isPressed);
    void UnAcquire();
    NiInputErr Acquire();

};

NiSmartPointer(NiInputSDL2Mouse);

#include "NiInputSDL2Mouse.inl"

#endif // NIINPUTSDL2MOUSE_H
