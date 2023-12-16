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

inline NiInputErr NiInputSDL2Mouse::HandleRemoval()
{
	UnAcquire();
	return NIIERR_OK;
}

inline NiInputErr NiInputSDL2Mouse::HandleInsertion()
{
	return Acquire();
}

inline NiInputMouse::Button NiInputSDL2Mouse::SDLKeyToNiKey(efd::UInt8 button)
{
	return m_aeMapping[button];
}
