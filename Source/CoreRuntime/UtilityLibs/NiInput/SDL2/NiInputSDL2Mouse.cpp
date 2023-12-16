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
// Precompiled Header
#include "NiInputPCH.h"
#include "NiInputSDL2Mouse.h"

//--------------------------------------------------------------------------------------------------
NiImplementRTTI(NiInputSDL2Mouse, NiInputMouse);

NiInputSDL2Mouse::NiInputSDL2Mouse(NiInputDevice::Description* pkDescription,
	SDL_Window* target, unsigned int uiUsage) :
	NiInputMouse(pkDescription),
	m_uiFlags(uiUsage),
	m_pTarget(target)
{
	InitializeMapping();
	m_eLastError = Acquire();
}

NiInputSDL2Mouse::~NiInputSDL2Mouse()
{
	UnAcquire();
}

NiInputErr NiInputSDL2Mouse::UpdateDevice()
{
	NiInputMouse::UpdateDevice();

	int x, y;
	efd::UInt32 btns = SDL_GetMouseState(&x, &y);

	for (int i = 1; i < 6; i++)
	{
		UpdateButton(i, (btns & SDL_BUTTON(i)) );
	}

	// Touch the controls that weren't...
	// The positions are touchs in the UpdateImmediatePositionData call
	// above, so we don't have to worry about them.
	if (m_uiButtonTouchMask)
	{
		for (unsigned int ui = 0; ui < NIM_NUM_BUTTONS; ui++)
		{
			if ((m_uiButtonTouchMask & (1 << ui)) == 0)
				m_akButtons[ui].TouchValue();
		}
	}
	else
	{
		for (unsigned int ui = 0; ui < NIM_NUM_BUTTONS; ui++)
			m_akButtons[ui].TouchValue();
	}

	return NIIERR_OK;
}

void NiInputSDL2Mouse::UnAcquire()
{
	if (m_uiFlags & NiInputSystem::EXCLUSIVE)
		SDL_SetWindowGrab(m_pTarget, SDL_FALSE);
	else if (m_uiFlags & NiInputSystem::BACKGROUND)
		SDL_CaptureMouse(SDL_FALSE);
}

NiInputErr NiInputSDL2Mouse::Acquire()
{
	if (m_uiFlags & NiInputSystem::EXCLUSIVE)
		SDL_SetWindowGrab(m_pTarget, SDL_TRUE);
	else if (m_uiFlags & NiInputSystem::BACKGROUND)
	{
		if (SDL_CaptureMouse(SDL_TRUE) == -1)
		{
			return NIIERR_UNSUPPORTED;
		}
	}

	return NIIERR_OK;
}

void NiInputSDL2Mouse::UpdateButton(efd::UInt8 buttonId, bool isPressed)
{
	NiInputMouse::Button btn = SDLKeyToNiKey(buttonId);

	if (isPressed)
		RecordButtonPress(btn);
	else
		RecordButtonRelease(btn);

	m_uiButtonTouchMask |= (1 << btn);
}

void NiInputSDL2Mouse::InitializeMapping()
{
	m_aeMapping[0] = NiInputMouse::NIM_NONE;
	m_aeMapping[SDL_BUTTON_LEFT] = NiInputMouse::NIM_LEFT;
	m_aeMapping[SDL_BUTTON_RIGHT] = NiInputMouse::NIM_RIGHT;
	m_aeMapping[SDL_BUTTON_MIDDLE] = NiInputMouse::NIM_MIDDLE;
	m_aeMapping[SDL_BUTTON_X1] = NiInputMouse::NIM_X1;
	m_aeMapping[SDL_BUTTON_X2] = NiInputMouse::NIM_X2;
}
