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
#include "NiInputSDL2Keyboard.h"

//--------------------------------------------------------------------------------------------------
NiImplementRTTI(NiInputSDL2Keyboard, NiInputKeyboard);

NiInputSDL2Keyboard::NiInputSDL2Keyboard(
    NiInputDevice::Description* pkDescription,
	SDL_Window* target, unsigned int uiUsage) :
    NiInputKeyboard(pkDescription),
	m_uiFlags(uiUsage),
	m_pTarget(target),
	m_anKeyStatus(NULL)
{
	InitializeMapping();
	m_eLastError = Acquire();
}

NiInputSDL2Keyboard::~NiInputSDL2Keyboard()
{
    UnAcquire();
}


NiInputErr NiInputSDL2Keyboard::Acquire()
{
	if (m_uiFlags & NiInputSystem::EXCLUSIVE)
		SDL_SetWindowKeyboardGrab(m_pTarget, SDL_TRUE);

	m_anKeyStatus = SDL_GetKeyboardState(NULL);
	UpdateKeymap();
	return NIIERR_OK;
}

void NiInputSDL2Keyboard::UnAcquire()
{
	if (m_uiFlags & NiInputSystem::EXCLUSIVE)
		SDL_SetWindowKeyboardGrab(m_pTarget, SDL_FALSE);

	SDL_ResetKeyboard();
}

void NiInputSDL2Keyboard::InitializeMapping()
{
    memset((void*)m_aeMapping, KEY_NOKEY, sizeof(m_aeMapping));

    m_aeMapping[SDL_SCANCODE_ESCAPE] = KEY_ESCAPE;
    m_aeMapping[SDL_SCANCODE_1] = KEY_1;
    m_aeMapping[SDL_SCANCODE_2] = KEY_2;
    m_aeMapping[SDL_SCANCODE_3] = KEY_3;
    m_aeMapping[SDL_SCANCODE_4] = KEY_4;
    m_aeMapping[SDL_SCANCODE_5] = KEY_5;
    m_aeMapping[SDL_SCANCODE_6] = KEY_6;
    m_aeMapping[SDL_SCANCODE_7] = KEY_7;
    m_aeMapping[SDL_SCANCODE_8] = KEY_8;
    m_aeMapping[SDL_SCANCODE_9] = KEY_9;
    m_aeMapping[SDL_SCANCODE_0] = KEY_0;
    m_aeMapping[SDL_SCANCODE_MINUS] = KEY_MINUS;
    m_aeMapping[SDL_SCANCODE_EQUALS] = KEY_EQUALS;
    m_aeMapping[SDL_SCANCODE_BACKSPACE] = KEY_BACK;
    m_aeMapping[SDL_SCANCODE_TAB] = KEY_TAB;
    m_aeMapping[SDL_SCANCODE_Q] = KEY_Q;
    m_aeMapping[SDL_SCANCODE_W] = KEY_W;
    m_aeMapping[SDL_SCANCODE_E] = KEY_E;
    m_aeMapping[SDL_SCANCODE_R] = KEY_R;
    m_aeMapping[SDL_SCANCODE_T] = KEY_T;
    m_aeMapping[SDL_SCANCODE_Y] = KEY_Y;
    m_aeMapping[SDL_SCANCODE_U] = KEY_U;
    m_aeMapping[SDL_SCANCODE_I] = KEY_I;
    m_aeMapping[SDL_SCANCODE_O] = KEY_O;
    m_aeMapping[SDL_SCANCODE_P] = KEY_P;
    m_aeMapping[SDL_SCANCODE_LEFTBRACKET] = KEY_LBRACKET;
    m_aeMapping[SDL_SCANCODE_RIGHTBRACKET] = KEY_RBRACKET;
    m_aeMapping[SDL_SCANCODE_RETURN] = KEY_RETURN;
    m_aeMapping[SDL_SCANCODE_LCTRL] = KEY_LCONTROL;
    m_aeMapping[SDL_SCANCODE_A] = KEY_A;
    m_aeMapping[SDL_SCANCODE_S] = KEY_S;
    m_aeMapping[SDL_SCANCODE_D] = KEY_D;
    m_aeMapping[SDL_SCANCODE_F] = KEY_F;
    m_aeMapping[SDL_SCANCODE_G] = KEY_G;
    m_aeMapping[SDL_SCANCODE_H] = KEY_H;
    m_aeMapping[SDL_SCANCODE_J] = KEY_J;
    m_aeMapping[SDL_SCANCODE_K] = KEY_K;
    m_aeMapping[SDL_SCANCODE_L] = KEY_L;
    m_aeMapping[SDL_SCANCODE_SEMICOLON] = KEY_SEMICOLON;
    m_aeMapping[SDL_SCANCODE_APOSTROPHE] = KEY_APOSTROPHE;
    m_aeMapping[SDL_SCANCODE_GRAVE] = KEY_GRAVE;
    m_aeMapping[SDL_SCANCODE_LSHIFT] = KEY_LSHIFT;
    m_aeMapping[SDL_SCANCODE_BACKSLASH] = KEY_BACKSLASH;
    m_aeMapping[SDL_SCANCODE_Z] = KEY_Z;
    m_aeMapping[SDL_SCANCODE_X] = KEY_X;
    m_aeMapping[SDL_SCANCODE_C] = KEY_C;
    m_aeMapping[SDL_SCANCODE_V] = KEY_V;
    m_aeMapping[SDL_SCANCODE_B] = KEY_B;
    m_aeMapping[SDL_SCANCODE_N] = KEY_N;
    m_aeMapping[SDL_SCANCODE_M] = KEY_M;
    m_aeMapping[SDL_SCANCODE_COMMA] = KEY_COMMA;
    m_aeMapping[SDL_SCANCODE_PERIOD] = KEY_PERIOD;
    m_aeMapping[SDL_SCANCODE_SLASH] = KEY_SLASH;
    m_aeMapping[SDL_SCANCODE_RSHIFT] = KEY_RSHIFT;
    m_aeMapping[SDL_SCANCODE_KP_MULTIPLY] = KEY_MULTIPLY;
    m_aeMapping[SDL_SCANCODE_LALT] = KEY_LMENU;
    m_aeMapping[SDL_SCANCODE_SPACE] = KEY_SPACE;
    m_aeMapping[SDL_SCANCODE_CAPSLOCK] = KEY_CAPITAL;
    m_aeMapping[SDL_SCANCODE_F1] = KEY_F1;
    m_aeMapping[SDL_SCANCODE_F2] = KEY_F2;
    m_aeMapping[SDL_SCANCODE_F3] = KEY_F3;
    m_aeMapping[SDL_SCANCODE_F4] = KEY_F4;
    m_aeMapping[SDL_SCANCODE_F5] = KEY_F5;
    m_aeMapping[SDL_SCANCODE_F6] = KEY_F6;
    m_aeMapping[SDL_SCANCODE_F7] = KEY_F7;
    m_aeMapping[SDL_SCANCODE_F8] = KEY_F8;
    m_aeMapping[SDL_SCANCODE_F9] = KEY_F9;
    m_aeMapping[SDL_SCANCODE_F10] = KEY_F10;
    m_aeMapping[SDL_SCANCODE_NUMLOCKCLEAR] = KEY_NUMLOCK;
    m_aeMapping[SDL_SCANCODE_SCROLLLOCK] = KEY_SCROLL;
    m_aeMapping[SDL_SCANCODE_KP_7] = KEY_NUMPAD7;
    m_aeMapping[SDL_SCANCODE_KP_8] = KEY_NUMPAD8;
    m_aeMapping[SDL_SCANCODE_KP_9] = KEY_NUMPAD9;
    m_aeMapping[SDL_SCANCODE_KP_MINUS] = KEY_SUBTRACT;
    m_aeMapping[SDL_SCANCODE_KP_4] = KEY_NUMPAD4;
    m_aeMapping[SDL_SCANCODE_KP_5] = KEY_NUMPAD5;
    m_aeMapping[SDL_SCANCODE_KP_6] = KEY_NUMPAD6;
    m_aeMapping[SDL_SCANCODE_KP_PLUS] = KEY_ADD;
    m_aeMapping[SDL_SCANCODE_KP_1] = KEY_NUMPAD1;
    m_aeMapping[SDL_SCANCODE_KP_2] = KEY_NUMPAD2;
    m_aeMapping[SDL_SCANCODE_KP_3] = KEY_NUMPAD3;
    m_aeMapping[SDL_SCANCODE_KP_0] = KEY_NUMPAD0;
    m_aeMapping[SDL_SCANCODE_KP_DECIMAL] = KEY_DECIMAL;
    //m_aeMapping[SDL_SCANCODE_OEM_102] = KEY_OEM_102;
    m_aeMapping[SDL_SCANCODE_F11] = KEY_F11;
    m_aeMapping[SDL_SCANCODE_F12] = KEY_F12;
    m_aeMapping[SDL_SCANCODE_F13] = KEY_F13;
    m_aeMapping[SDL_SCANCODE_F14] = KEY_F14;
    m_aeMapping[SDL_SCANCODE_F15] = KEY_F15;
    m_aeMapping[SDL_SCANCODE_LANG3] = KEY_KANA;
    //m_aeMapping[SDL_SCANCODE_ABNT_C1] = KEY_ABNT_C1;
    //m_aeMapping[SDL_SCANCODE_CONVERT] = KEY_CONVERT;
    //m_aeMapping[SDL_SCANCODE_NOCONVERT] = KEY_NOCONVERT;
    m_aeMapping[SDL_SCANCODE_INTERNATIONAL3] = KEY_YEN;
    //m_aeMapping[SDL_SCANCODE_ABNT_C2] = KEY_ABNT_C2;
    m_aeMapping[SDL_SCANCODE_KP_EQUALS] = KEY_NUMPADEQUALS;
    //m_aeMapping[SDL_SCANCODE_PREVTRACK] = KEY_PREVTRACK;
    //m_aeMapping[SDL_SCANCODE_AT] = KEY_AT;
    //m_aeMapping[SDL_SCANCODE_COLON] = KEY_COLON;
    //m_aeMapping[SDL_SCANCODE_UNDERLINE] = KEY_UNDERLINE;
    //m_aeMapping[SDL_SCANCODE_KANJI] = KEY_KANJI;
    //m_aeMapping[SDL_SCANCODE_JP_STOP] = KEY_STOP;
    //m_aeMapping[SDL_SCANCODE_AX] = KEY_AX;
    //m_aeMapping[SDL_SCANCODE_UNLABELED] = KEY_UNLABELED;
    //m_aeMapping[SDL_SCANCODE_NEXTTRACK] = KEY_NEXTTRACK;
    m_aeMapping[SDL_SCANCODE_KP_ENTER] = KEY_NUMPADENTER;
    m_aeMapping[SDL_SCANCODE_RCTRL] = KEY_RCONTROL;
    m_aeMapping[SDL_SCANCODE_MUTE] = KEY_MUTE;
    m_aeMapping[SDL_SCANCODE_CALCULATOR] = KEY_CALCULATOR;
    m_aeMapping[SDL_SCANCODE_AUDIOPLAY] = KEY_PLAYPAUSE;
    m_aeMapping[SDL_SCANCODE_AUDIOSTOP] = KEY_MEDIASTOP;
    m_aeMapping[SDL_SCANCODE_VOLUMEDOWN] = KEY_VOLUMEDOWN;
    m_aeMapping[SDL_SCANCODE_VOLUMEUP] = KEY_VOLUMEUP;
    m_aeMapping[SDL_SCANCODE_AC_HOME] = KEY_WEBHOME;
    m_aeMapping[SDL_SCANCODE_KP_COMMA] = KEY_NUMPADCOMMA;
    m_aeMapping[SDL_SCANCODE_KP_DIVIDE] = KEY_DIVIDE;
    m_aeMapping[SDL_SCANCODE_PRINTSCREEN] = KEY_SYSRQ;
    m_aeMapping[SDL_SCANCODE_RALT] = KEY_RMENU;
    m_aeMapping[SDL_SCANCODE_PAUSE] = KEY_PAUSE;
    m_aeMapping[SDL_SCANCODE_HOME] = KEY_HOME;
    m_aeMapping[SDL_SCANCODE_UP] = KEY_UP;
    m_aeMapping[SDL_SCANCODE_PRIOR] = KEY_PRIOR;
    m_aeMapping[SDL_SCANCODE_LEFT] = KEY_LEFT;
    m_aeMapping[SDL_SCANCODE_RIGHT] = KEY_RIGHT;
    m_aeMapping[SDL_SCANCODE_END] = KEY_END;
    m_aeMapping[SDL_SCANCODE_DOWN] = KEY_DOWN;
    m_aeMapping[SDL_SCANCODE_PAGEDOWN] = KEY_NEXT;
    m_aeMapping[SDL_SCANCODE_INSERT] = KEY_INSERT;
    m_aeMapping[SDL_SCANCODE_DELETE] = KEY_DELETE;
    m_aeMapping[SDL_SCANCODE_LGUI] = KEY_LWIN;
    m_aeMapping[SDL_SCANCODE_RGUI] = KEY_RWIN;
    m_aeMapping[SDL_SCANCODE_APPLICATION] = KEY_APPS;
    m_aeMapping[SDL_SCANCODE_POWER] = KEY_POWER;
    m_aeMapping[SDL_SCANCODE_SLEEP] = KEY_SLEEP;
    //m_aeMapping[SDL_SCANCODE_WAKE] = KEY_WAKE;
    m_aeMapping[SDL_SCANCODE_AC_SEARCH] = KEY_WEBSEARCH;
    m_aeMapping[SDL_SCANCODE_AC_BOOKMARKS] = KEY_WEBFAVORITES;
    m_aeMapping[SDL_SCANCODE_AC_REFRESH] = KEY_WEBREFRESH;
    m_aeMapping[SDL_SCANCODE_AC_STOP] = KEY_WEBSTOP;
    m_aeMapping[SDL_SCANCODE_AC_FORWARD] = KEY_WEBFORWARD;
    m_aeMapping[SDL_SCANCODE_AC_BACK] = KEY_WEBBACK;
    m_aeMapping[SDL_SCANCODE_COMPUTER] = KEY_MYCOMPUTER;
    m_aeMapping[SDL_SCANCODE_MAIL] = KEY_MAIL;
    m_aeMapping[SDL_SCANCODE_MEDIASELECT] = KEY_MEDIASELECT;
}

inline NiInputErr NiInputSDL2Keyboard::UpdateDevice()
{
	NiInputKeyboard::UpdateDevice();

    UpdateKeymap();
    return NIIERR_OK;
}


void NiInputSDL2Keyboard::UpdateKey(SDL_Scancode scancode, efd::UInt16 mod, bool pressed)
{
	NiInputKeyboard::KeyCode btn = SDLKeyToNiKey(scancode);
	if (btn == NiInputKeyboard::KEY_NOKEY)
		return;

	if (pressed)
		RecordKeyPress(btn);
	else
		RecordKeyRelease(btn);

    if (mod & ::KMOD_LSHIFT)
        AddModifiers(NiInputKeyboard::KMOD_LSHIFT);
    if (mod & ::KMOD_RSHIFT)
        AddModifiers(NiInputKeyboard::KMOD_RSHIFT);
    if (mod & ::KMOD_LCTRL)
        AddModifiers(NiInputKeyboard::KMOD_LCONTROL);
    if (mod & ::KMOD_RCTRL)
        AddModifiers(NiInputKeyboard::KMOD_RCONTROL);
    if (mod & ::KMOD_LALT)
        AddModifiers(KMOD_LMENU);
    if (mod & ::KMOD_RALT)
        AddModifiers(KMOD_RMENU);
    if (mod & ::KMOD_LGUI)
        AddModifiers(KMOD_LWIN);
    if (mod & ::KMOD_RGUI)
        AddModifiers(KMOD_RWIN);
    if (mod & ::KMOD_CAPS)
        AddModifiers(KMOD_CAPS_LOCK);
}

void NiInputSDL2Keyboard::UpdateKeymap()
{
    for (efd::UInt16 i = 0; i < SDL_NUM_SCANCODES; i++)
    {
        if (m_anKeyStatus[i])
            RecordKeyPress(SDLKeyToNiKey((SDL_Scancode)i));
        else
            RecordKeyRelease(SDLKeyToNiKey((SDL_Scancode)i));
    }
}
