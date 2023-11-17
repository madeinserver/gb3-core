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
// NSBUserDefinedData inline functions
//--------------------------------------------------------------------------------------------------
inline NSBUserDefinedDataBlock::NSBUserDefinedDataBlock(const char* pcName):
    NSBConstantMap(),
    m_pcName(0)
{
    SetName(pcName);
}

//--------------------------------------------------------------------------------------------------
inline NSBUserDefinedDataBlock::~NSBUserDefinedDataBlock()
{
    NiFree(m_pcName);
}

//--------------------------------------------------------------------------------------------------
inline const char* NSBUserDefinedDataBlock::GetName() const
{
    return m_pcName;
}

//--------------------------------------------------------------------------------------------------
inline void NSBUserDefinedDataBlock::SetName(const char* pcName)
{
    NSBUtility::SetString(m_pcName, 0, pcName);
}

//--------------------------------------------------------------------------------------------------
