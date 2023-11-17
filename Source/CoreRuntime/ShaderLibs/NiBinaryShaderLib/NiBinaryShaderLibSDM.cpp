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
//--------------------------------------------------------------------------------------------------
// Precompiled Header
#include "NiBinaryShaderLibPCH.h"

#include "NiBinaryShaderLib.h"
#include "NiBinaryShaderLibrary.h"
#include "NiBinaryShaderLibSDM.h"

//--------------------------------------------------------------------------------------------------
NiImplementSDMConstructor(NiBinaryShaderLib, "NiMain NiFloodgate");

//--------------------------------------------------------------------------------------------------
void NiBinaryShaderLibSDM::Init()
{
    NiImplementSDMInitCheck();
    NiShaderFactory::AddLibraryCallback("NiBinaryShaderLib",
        &NiBinaryShaderLib_LoadShaderLibrary);
    NiShaderFactory::AddLibraryCallback("NSBShaderLib",
        &NiBinaryShaderLib_LoadShaderLibrary);
}

//--------------------------------------------------------------------------------------------------
void NiBinaryShaderLibSDM::Shutdown()
{
    NiImplementSDMShutdownCheck();

    NiBinaryShaderLibrary::SetDirectoryInfo(0);

    NiShaderFactory::RemoveLibraryCallback(NiBinaryShaderLib_LoadShaderLibrary);
    NiShaderFactory::RemoveLibraryCallback(NiBinaryShaderLib_LoadShaderLibrary);
}

//--------------------------------------------------------------------------------------------------
