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
inline NiAVObject* NiPSysPlanarCollider::GetColliderObj() const
{
    return m_pkColliderObj;
}

//--------------------------------------------------------------------------------------------------
inline float NiPSysPlanarCollider::GetWidth() const
{
    return m_fWidth;
}

//--------------------------------------------------------------------------------------------------
inline float NiPSysPlanarCollider::GetHeight() const
{
    return m_fHeight;
}

//--------------------------------------------------------------------------------------------------
inline const NiPoint3& NiPSysPlanarCollider::GetXAxis() const
{
    return m_kXAxis;
}

//--------------------------------------------------------------------------------------------------
inline const NiPoint3& NiPSysPlanarCollider::GetYAxis() const
{
    return m_kYAxis;
}

//--------------------------------------------------------------------------------------------------
#endif // #ifndef EE_REMOVE_BACK_COMPAT_STREAMING
