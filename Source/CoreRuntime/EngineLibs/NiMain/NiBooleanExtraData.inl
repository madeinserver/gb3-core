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
//  NiBooleanExtraData inline functions

//--------------------------------------------------------------------------------------------------
inline NiBooleanExtraData::NiBooleanExtraData()
{
    m_bValue = false;
}

//--------------------------------------------------------------------------------------------------
inline NiBooleanExtraData::NiBooleanExtraData(const bool bValue)
{
    m_bValue = bValue;
}

//--------------------------------------------------------------------------------------------------
inline NiBooleanExtraData::~NiBooleanExtraData ()
{
}

//--------------------------------------------------------------------------------------------------
inline bool NiBooleanExtraData::GetValue() const
{
    return m_bValue;
}

//--------------------------------------------------------------------------------------------------
inline void NiBooleanExtraData::SetValue(const bool bValue)
{
    m_bValue = bValue;
}
