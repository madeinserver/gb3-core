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
inline NiParticleInfo* NiPSysData::GetParticleInfo()
{
    return m_pkParticleInfo;
}

//--------------------------------------------------------------------------------------------------
inline const NiParticleInfo* NiPSysData::GetParticleInfo() const
{
    return m_pkParticleInfo;
}

//--------------------------------------------------------------------------------------------------
inline float* NiPSysData::GetRotationSpeeds()
{
    return m_pfRotationSpeeds;
}

//--------------------------------------------------------------------------------------------------
inline const float* NiPSysData::GetRotationSpeeds() const
{
    return m_pfRotationSpeeds;
}

//--------------------------------------------------------------------------------------------------
inline unsigned short NiPSysData::GetMaxNumParticles() const
{
    return m_usVertices;
}

//--------------------------------------------------------------------------------------------------
inline unsigned short NiPSysData::GetNumParticles() const
{
    return GetActiveVertexCount();
}

//--------------------------------------------------------------------------------------------------
#endif // #ifndef EE_REMOVE_BACK_COMPAT_STREAMING
