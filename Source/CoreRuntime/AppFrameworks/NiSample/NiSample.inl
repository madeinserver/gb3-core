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
inline bool NiSample::RegisterShaderParsers()
{
    return true;
}

//--------------------------------------------------------------------------------------------------
inline bool NiSample::RegisterShaderLibraries()
{
    return true;
}

//--------------------------------------------------------------------------------------------------
inline void NiSample::SetUseNavSystem(bool bUseNavSystem)
{
    m_bUseNavSystem = bUseNavSystem;
}

//--------------------------------------------------------------------------------------------------
inline bool NiSample::GetUseNavSystem()
{
    return m_bUseNavSystem;
}

//--------------------------------------------------------------------------------------------------
