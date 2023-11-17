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

#include "NiAudioPCH.h"
#include "NiAudioMetrics.h"

const char NiAudioMetrics::ms_acNames
    [NiAudioMetrics::NUM_METRICS][NIMETRICS_NAMELENGTH] =
{
    NIMETRICS_AUDIO_PREFIX "LoadedSources",
    NIMETRICS_AUDIO_PREFIX "PlayedLoops"
};

unsigned int NiAudioMetrics::ms_uiLoadedSources = 0;
