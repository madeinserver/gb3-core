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
//--------------------------------------------------------------------------------------------------
// Precompiled Header
#include "NiInputPCH.h"
#include "NiSDL2InputSystem.h"
#include "NiInputSDL2Mouse.h"
#include "NiInputSDL2GameController.h"
#include "NiInputSDL2Keyboard.h"
#include "NiInputSDL2Joystick.h"

//--------------------------------------------------------------------------------------------------
// THIS FUNCTION MUST BE PRESENT FOR EACH PLATFORM-SPECIFIC IMPLEMENTATION.
//--------------------------------------------------------------------------------------------------
NiInputSystem* NiInputSystem::Create(CreateParams* pkParams)
{
    NiInputSystem* pkInputSys = 0;

    if (NiIsKindOf(NiSDL2InputSystem::SDL2CreateParams, pkParams))
    {
        NiSDL2InputSystem::SDL2CreateParams* pkCreateParams =
            (NiSDL2InputSystem::SDL2CreateParams*)pkParams;
        NiSDL2InputSystem* pkIS = NiNew NiSDL2InputSystem();
        if (pkIS)
        {
            // We need to copy the pointer of the callback to be called by NiApplication
            pkIS->CopySDLProcCallbackToOutput(pkCreateParams->GetInputCallbackReference());
            pkIS->SetOwnerWindow(pkCreateParams->GetOwnerWindow());

            // Initialize it
            if (pkIS->Initialize(pkParams) != NIIERR_OK)
            {
                NiDelete pkIS;
                return NULL;
            }
            pkInputSys = (NiInputSystem*)pkIS;

            // Create 4 NiInputXInputGamePad objects, and check the initial
            // status of each

            /*for (ui = 0; ui < MAX_XINPUT_GAMEPADS; ui++)
            {
                // Need to create it.
                NiInputDevice::Description* pkNiDesc =
                    pkIS->CreateNiInputDescriptionForXInput(ui, 0);
                EE_ASSERT(pkNiDesc);

                pkDI8IS->AddAvailableDevice(pkNiDesc);

                if (pkNiDesc->GetType() != NiInputDevice::NIID_GAMEPAD)
                {
                    pkDI8IS->m_eLastError = NIIERR_INVALIDDEVICE;
                    continue;
                }

                // Create an NiInputXInputGamePad instance.
                NiInputXInputGamePad* pkXIGP = NiNew NiInputXInputGamePad(
                    pkDI8IS, pkNiDesc, pkDI8IS->m_iAxisRangeLow,
                    pkDI8IS->m_iAxisRangeHigh);
                EE_ASSERT(pkXIGP);
                pkXIGP->SetStatus(NiInputDevice::REMOVED);
                pkDI8IS->m_aspGamePads[ui] = pkXIGP;

                pkDI8IS->MapNiActionsToGamePad(pkXIGP);

                // Get initial state of device
                pkDI8IS->ReadXInputGamePad(ui);
            }*/
        }
    }
    else
    {
        EE_FAIL("Initializing NiSDL2InputSystem w/ incorrect CreateParams!");
    }

    return pkInputSys;
}

//--------------------------------------------------------------------------------------------------
NiImplementRTTI(NiSDL2InputSystem, NiInputSystem);

//--------------------------------------------------------------------------------------------------
NiImplementRTTI(NiSDL2InputSystem::SDL2CreateParams,
    NiInputSystem::CreateParams);

NiSDL2InputSystem::NiSDL2InputSystem()  :
    m_pOwnerWnd(NULL)
{
}

NiSDL2InputSystem::~NiSDL2InputSystem()
{
    Shutdown();
}

//--------------------------------------------------------------------------------------------------

NiInputMouse* NiSDL2InputSystem::OpenMouse()
{
    if (m_spMouse)
        return m_spMouse;

    NiInputDevice::Description* pkNiMouseDesc = EE_NEW SDL2Description(NiInputDevice::NIID_MOUSE, 0, 0, "Mouse");
    NiInputSDL2Mouse* pMouse = EE_NEW NiInputSDL2Mouse(pkNiMouseDesc, m_pOwnerWnd, m_uiMouse);

    pMouse->SetStatus(NiInputDevice::READY);
    MapNiActionsToMouse();

    m_spMouse = (NiInputMouse*)pMouse;

    // we can only open one mouse in SDL2
    return pMouse;
}

//--------------------------------------------------------------------------------------------------
NiInputKeyboard* NiSDL2InputSystem::OpenKeyboard()
{
    if (m_spKeyboard)
        return m_spKeyboard;

    NiInputDevice::Description* pkNiKeyDesc = EE_NEW SDL2Description(NiInputDevice::NIID_KEYBOARD, 0, 0, "Keyboard");

    NiInputSDL2Keyboard* pKb = EE_NEW NiInputSDL2Keyboard(pkNiKeyDesc, m_pOwnerWnd, m_uiKeyboard);
    pKb->SetStatus(NiInputDevice::READY);

    m_spKeyboard = (NiInputKeyboard*)pKb;

    // we can only open one keyboard in SDL2
    return pKb;
}

//--------------------------------------------------------------------------------------------------
NiInputErr NiSDL2InputSystem::Initialize(CreateParams* pkParams)
{
    if (NiInputSystem::Initialize(pkParams) != NIIERR_OK)
        return m_eLastError;

    return NIIERR_OK;
}

NiInputGamePad* NiSDL2InputSystem::OpenGamePad(unsigned int uiPort,
    unsigned int uiSlot)
{
    // TODO: Add gamepad support
    m_eLastError = NIIERR_UNSUPPORTED;
    return NULL;
}

bool NiSDL2InputSystem::SDLProc(SDL_Event* evt)
{
    // casting is done this way to avoid RTTI calls...
    switch (evt->type)
    {
    case SDL_KEYMAPCHANGED:
        if (m_spKeyboard)
            ((NiInputSDL2Keyboard*)m_spKeyboard.data())->UpdateKeymap();
        break;
    case SDL_MOUSEWHEEL: // this is required!
        if (!m_spMouse)
            break;

        if (evt->wheel.direction == SDL_MOUSEWHEEL_FLIPPED)
        {
            ((NiInputSDL2Mouse*)m_spMouse.data())->RecordPositionChange(2, evt->wheel.y * -1);
            //((NiInputSDL2Mouse*)m_spMouse.data())->RecordPositionChange(3, evt->wheel.x * -1);
        }
        else
        {
            ((NiInputSDL2Mouse*)m_spMouse.data())->RecordPositionChange(2, evt->wheel.y);
            //((NiInputSDL2Mouse*)m_spMouse.data())->RecordPositionChange(3, evt->wheel.x);
        }
        break;
    default:
        return false;
    }

    return true;
}

//--------------------------------------------------------------------------------------------------
NiInputErr NiSDL2InputSystem::UpdateAllDevices()
{
    return NiInputSystem::UpdateAllDevices();
}

//--------------------------------------------------------------------------------------------------
NiInputErr NiSDL2InputSystem::UpdateActionMap()
{
    return NiInputSystem::UpdateActionMap();
}

//--------------------------------------------------------------------------------------------------
NiInputErr NiSDL2InputSystem::HandleRemovals()
{
    return NIIERR_OK;
}

//--------------------------------------------------------------------------------------------------
NiInputErr NiSDL2InputSystem::HandleInsertions()
{
    return NIIERR_OK;
}

//--------------------------------------------------------------------------------------------------
NiInputErr NiSDL2InputSystem::Shutdown()
{
    // We need to release the devices before we can release the interface!
    NiInputSystem::Shutdown();
    return NIIERR_OK;
}

//--------------------------------------------------------------------------------------------------
void NiSDL2InputSystem::SuspendInput()
{
}

//--------------------------------------------------------------------------------------------------
void NiSDL2InputSystem::ResumeInput()
{
}

//--------------------------------------------------------------------------------------------------
NiInputErr NiSDL2InputSystem::CheckEnumerationStatus()
{
    // enumeration isn't done in a secondary thread in SDL2,
    // we can always assume we are never enumerating as we will know
    // about joystick changes by the event pump
    return NIIERR_ENUM_NOTRUNNING;
}

//--------------------------------------------------------------------------------------------------
NiInputErr NiSDL2InputSystem::HandleDeviceChanges()
{
    m_eLastError = NIIERR_OK;

    //
    bool bLostDevice = false;

    // Keyboard
    if (m_spKeyboard)
    {
        NiInputErr eErr = m_spKeyboard->HandleInsertion();
        if (eErr == NIIERR_DEVICELOST)
            bLostDevice = true;
        m_spKeyboard->SetLastError(eErr);
    }

    // Mouse
    if (m_spMouse)
    {
        NiInputErr eErr = m_spMouse->HandleInsertion();
        if (eErr == NIIERR_DEVICELOST)
            bLostDevice = true;
        m_spMouse->SetLastError(eErr);
    }

    // Gamepads
    for (unsigned int ui = 0; ui < MAX_GAMEPADS; ui++)
    {
        if (m_aspGamePads[ui])
        {
            NiInputErr eErr = m_aspGamePads[ui]->HandleInsertion();
            if (eErr == NIIERR_DEVICELOST)
                bLostDevice = true;
            m_aspGamePads[ui]->SetLastError(eErr);
        }
    }

    // See if any devices were lost
    if (bLostDevice)
        m_eLastError = NIIERR_DEVICELOST;

    return m_eLastError;
}
