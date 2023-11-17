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

#include "egmToolServicesPCH.h"

#include "IInteractionDelegate.h"

using namespace efd;
using namespace egf;
using namespace ecr;
using namespace egmToolServices;

//-----------------------------------------------------------------------------------------------
bool IInteractionDelegate::OnPreMouseScroll(
        efd::SInt32 x,
        efd::SInt32 y,
        efd::SInt32 dScroll)
{
    EE_UNUSED_ARG(x);
    EE_UNUSED_ARG(y);
    EE_UNUSED_ARG(dScroll);

    return false;
}

//-----------------------------------------------------------------------------------------------
bool IInteractionDelegate::OnPreMouseMove(
        efd::SInt32 x,
        efd::SInt32 y,
        efd::SInt32 dx,
        efd::SInt32 dy)
{
    EE_UNUSED_ARG(x);
    EE_UNUSED_ARG(y);
    EE_UNUSED_ARG(dx);
    EE_UNUSED_ARG(dy);

    return false;
}

//-----------------------------------------------------------------------------------------------
bool IInteractionDelegate::OnPreMouseDown(
        ecrInput::MouseMessage::MouseButton eButton,
        efd::SInt32 x,
        efd::SInt32 y)
{
    EE_UNUSED_ARG(eButton);
    EE_UNUSED_ARG(x);
    EE_UNUSED_ARG(y);

    return false;
}

//-----------------------------------------------------------------------------------------------
bool IInteractionDelegate::OnPreMouseUp(
        ecrInput::MouseMessage::MouseButton eButton,
        efd::SInt32 x,
        efd::SInt32 y)
{
    EE_UNUSED_ARG(eButton);
    EE_UNUSED_ARG(x);
    EE_UNUSED_ARG(y);

    return false;
}
//-----------------------------------------------------------------------------------------------
