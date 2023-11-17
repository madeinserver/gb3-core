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

#pragma once
#ifndef NISTEPFLOATKEY_H
#define NISTEPFLOATKEY_H

#include "NiFloatKey.h"


class NIANIMATION_ENTRY NiStepFloatKey : public NiFloatKey
{
    NiDeclareAnimationStream;
public:
    // constuction
    NiStepFloatKey();
    NiStepFloatKey(float fTime, float fValue);
};

NiRegisterAnimationStream(NiStepFloatKey);

#include "NiStepFloatKey.inl"

#endif

