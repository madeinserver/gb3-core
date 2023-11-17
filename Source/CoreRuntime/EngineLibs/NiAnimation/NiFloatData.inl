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
inline NiFloatData::NiFloatData()
{
    m_uiNumKeys = 0;
    m_pkKeys = 0;
    m_eType = NiAnimationKey::NOINTERP;
    m_ucKeySize = 0;
}

//--------------------------------------------------------------------------------------------------
inline unsigned int NiFloatData::GetNumKeys() const
{
    return m_uiNumKeys;
}

//--------------------------------------------------------------------------------------------------
inline NiFloatKey* NiFloatData::GetAnim(unsigned int& uiNumKeys,
    NiFloatKey::KeyType& eType, unsigned char& ucSize) const
{
    uiNumKeys = m_uiNumKeys;
    eType = m_eType;
    ucSize = m_ucKeySize;
    return m_pkKeys;
}

//--------------------------------------------------------------------------------------------------
