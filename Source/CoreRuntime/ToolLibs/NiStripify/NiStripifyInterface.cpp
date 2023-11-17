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

#include "NiStripifyInterface.h"

NiStripifyInterface::Platform_Settings
NiStripifyInterface::ms_akDefaultSettings[] =
{
    { PLATFORM_WIN32,     24},
    { PLATFORM_PS3,       24},
    { PLATFORM_XBOX360,   15},
};

//--------------------------------------------------------------------------------------------------
NiStripifyInterface::NiStripifyInterface(Platform ePlatform)
{
    EE_ASSERT(ePlatform < PLATFORM_COUNT);

    m_kSettings = ms_akDefaultSettings[ePlatform];

    EE_ASSERT(m_kSettings.m_ePlatform == ePlatform);
}

//--------------------------------------------------------------------------------------------------
void NiStripifyInterface::Stripify_Object(NiNode* pkNode)
{
    NiStripify::Stripify(pkNode, m_kSettings.m_uiCacheSize);
}

//--------------------------------------------------------------------------------------------------
