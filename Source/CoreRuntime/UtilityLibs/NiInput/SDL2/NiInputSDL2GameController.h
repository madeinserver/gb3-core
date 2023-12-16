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
#ifndef NIINPUTSDL2GAMECONTROLLER_H
#define NIINPUTSDL2GAMECONTROLLER_H

#include "NiInputGamePad.h"

class NIINPUT_ENTRY NiInputSDL2GameController : public NiInputGamePad
{
	NiDeclareRTTI;

public:
    NiInputSDL2GameController(NiInputDevice::Description* pkDescription,
        int iStickRangeLow, int iStickRangeHigh);
    ~NiInputSDL2GameController();

    virtual NiInputErr UpdateDevice();
    //    virtual NiInputErr UpdateActionMappedDevice(
    //        NiInputSystem* pkInputSystem);
    virtual NiInputErr HandleRemoval();
    virtual NiInputErr HandleInsertion();

    virtual unsigned int GetMotorCount() const;
    virtual unsigned int GetRumbleRange(unsigned int uiMotor) const;
    virtual void SetRumbleValue(unsigned int uiMotor, unsigned int uiValue,
        bool bCommit);
    virtual void GetRumbleValue(unsigned int uiMotor, unsigned int& uiValue)
        const;
    virtual void StartRumble();
    virtual void StopRumble(bool bQuickStop = false);
};

#endif
