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

#ifndef EE_REMOVE_BACK_COMPAT_STREAMING

//--------------------------------------------------------------------------------------------------
inline unsigned short NiPSysSpawnModifier::GetNumSpawnGenerations() const
{
    return m_usNumSpawnGenerations;
}

//--------------------------------------------------------------------------------------------------
inline float NiPSysSpawnModifier::GetPercentageSpawned() const
{
    return m_fPercentageSpawned;
}

//--------------------------------------------------------------------------------------------------
inline unsigned short NiPSysSpawnModifier::GetMinNumToSpawn() const
{
    return m_usMinNumToSpawn;
}

//--------------------------------------------------------------------------------------------------
inline unsigned short NiPSysSpawnModifier::GetMaxNumToSpawn() const
{
    return m_usMaxNumToSpawn;
}

//--------------------------------------------------------------------------------------------------
inline float NiPSysSpawnModifier::GetSpawnSpeedChaos() const
{
    return m_fSpawnSpeedChaos;
}

//--------------------------------------------------------------------------------------------------
inline float NiPSysSpawnModifier::GetSpawnDirChaos() const
{
    return m_fSpawnDirChaos;
}

//--------------------------------------------------------------------------------------------------
inline float NiPSysSpawnModifier::GetLifeSpan() const
{
    return m_fLifeSpan;
}

//--------------------------------------------------------------------------------------------------
inline float NiPSysSpawnModifier::GetLifeSpanVar() const
{
    return m_fLifeSpanVar;
}

//--------------------------------------------------------------------------------------------------
#endif // #ifndef EE_REMOVE_BACK_COMPAT_STREAMING
