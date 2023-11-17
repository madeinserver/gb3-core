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
inline NiAVObjectPtr NiRoomGroup::GetShell() const
{
    return m_spShell;
}

//--------------------------------------------------------------------------------------------------
inline const NiRoomList &NiRoomGroup::GetRoomList() const
{
    return m_kRooms;
}

//--------------------------------------------------------------------------------------------------
inline NiRoomPtr NiRoomGroup::GetLastRoom() const
{
    return m_spLastRoom;
}

//--------------------------------------------------------------------------------------------------
inline void NiRoomGroup::SetLastRoom(NiRoom* pkLastRoom)
{
    m_spLastRoom = pkLastRoom;
}

//--------------------------------------------------------------------------------------------------
inline bool NiRoomGroup::GetPortallingDisabled()
{
    return ms_bPortallingDisabled;
}

//--------------------------------------------------------------------------------------------------
inline void NiRoomGroup::SetPortallingDisabled(bool bIsDisabled)
{
    ms_bPortallingDisabled = bIsDisabled;
}

//--------------------------------------------------------------------------------------------------
