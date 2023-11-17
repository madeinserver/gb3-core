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
inline NiBoolData::NiBoolData(): m_uiNumKeys(0),
    m_pkKeys(NULL),  m_eType(NiBoolKey::NOINTERP), m_ucKeySize(0)
{

}

//--------------------------------------------------------------------------------------------------
inline unsigned int NiBoolData::GetNumKeys() const
{
    return m_uiNumKeys;
}

//--------------------------------------------------------------------------------------------------
inline NiBoolKey* NiBoolData::GetAnim(unsigned int& uiNumKeys,
    NiBoolKey::KeyType& eType, unsigned char& ucSize) const
{
    uiNumKeys = m_uiNumKeys;
    eType = m_eType;
    ucSize = m_ucKeySize;
    return m_pkKeys;
}

//--------------------------------------------------------------------------------------------------
