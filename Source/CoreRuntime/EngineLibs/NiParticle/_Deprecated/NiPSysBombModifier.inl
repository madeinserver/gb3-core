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
inline NiAVObject* NiPSysBombModifier::GetBombObj() const
{
    return m_pkBombObj;
}

//--------------------------------------------------------------------------------------------------
inline const NiPoint3& NiPSysBombModifier::GetBombAxis() const
{
    return m_kBombAxis;
}

//--------------------------------------------------------------------------------------------------
inline float NiPSysBombModifier::GetDecay() const
{
    return m_fDecay;
}

//--------------------------------------------------------------------------------------------------
inline float NiPSysBombModifier::GetDeltaV() const
{
    return m_fDeltaV;
}

//--------------------------------------------------------------------------------------------------
inline NiPSysBombModifier::DecayType NiPSysBombModifier::GetDecayType() const
{
    return m_eDecayType;
}

//--------------------------------------------------------------------------------------------------
inline NiPSysBombModifier::SymmType NiPSysBombModifier::GetSymmType() const
{
    return m_eSymmType;
}

//--------------------------------------------------------------------------------------------------
#endif // #ifndef EE_REMOVE_BACK_COMPAT_STREAMING
