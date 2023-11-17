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
inline const NiFixedString& NiPSEmitterCtlr::GetEmitterName() const
{
    return m_kEmitterName;
}

//--------------------------------------------------------------------------------------------------
inline NiPSEmitter* NiPSEmitterCtlr::GetEmitterPointer() const
{
    return m_pkEmitter;
}

//--------------------------------------------------------------------------------------------------
inline void NiPSEmitterCtlr::GetEmitterPointerFromName()
{
    EE_ASSERT(m_pkTarget && m_kEmitterName.Exists());
    m_pkEmitter = ((NiPSParticleSystem*) m_pkTarget)->GetEmitterByName(
        m_kEmitterName);
    EE_ASSERT(m_pkEmitter);
}

//--------------------------------------------------------------------------------------------------
inline void NiPSEmitterCtlr::SetEmitterName(const NiFixedString& kEmitterName)
{
    EE_ASSERT(kEmitterName.Exists());
    m_kEmitterName = kEmitterName;
}

//--------------------------------------------------------------------------------------------------
