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
inline NiUInt32 NiPSSimulatorCollidersStep::GetCollidersCount() const
{
    return m_kColliders.GetSize();
}

//--------------------------------------------------------------------------------------------------
inline NiPSCollider* NiPSSimulatorCollidersStep::GetColliderAt(
    NiUInt32 uiIndex) const
{
    EE_ASSERT(uiIndex < m_kColliders.GetSize());

    return m_kColliders.GetAt(uiIndex);
}

//--------------------------------------------------------------------------------------------------
inline void NiPSSimulatorCollidersStep::AddCollider(NiPSCollider* pkCollider)
{
    EE_ASSERT(pkCollider);

    m_kColliders.Add(pkCollider);
    m_bShouldAllocateDataBuffer = true;
}

//--------------------------------------------------------------------------------------------------
inline NiPSColliderPtr NiPSSimulatorCollidersStep::SetColliderAt(
    NiUInt32 uiIndex,
    NiPSCollider* pkCollider)
{
    EE_ASSERT(uiIndex < m_kColliders.GetSize());
    EE_ASSERT(pkCollider);

    NiPSColliderPtr spOldCollider = m_kColliders.GetAt(uiIndex);
    m_kColliders.ReplaceAt(uiIndex, pkCollider);
    m_bShouldAllocateDataBuffer = true;

    return spOldCollider;
}

//--------------------------------------------------------------------------------------------------
inline bool NiPSSimulatorCollidersStep::RemoveCollider(
    NiPSCollider* pkCollider)
{
    EE_ASSERT(pkCollider);

    NiInt32 iIndex = m_kColliders.Find(pkCollider);
    if (iIndex > -1)
    {
        m_kColliders.OrderedRemoveAt((NiUInt32) iIndex);
        m_bShouldAllocateDataBuffer = true;
        return true;
    }
    else
    {
        return false;
    }
}

//--------------------------------------------------------------------------------------------------
inline NiPSColliderPtr NiPSSimulatorCollidersStep::RemoveColliderAt(
    NiUInt32 uiIndex)
{
    EE_ASSERT(uiIndex < m_kColliders.GetSize());

    NiPSColliderPtr spOldCollider = m_kColliders.GetAt(uiIndex);
    m_kColliders.OrderedRemoveAt(uiIndex);
    m_bShouldAllocateDataBuffer = true;

    return spOldCollider;
}

//--------------------------------------------------------------------------------------------------
inline void NiPSSimulatorCollidersStep::RemoveAllColliders()
{
    m_kColliders.RemoveAll();
    m_bShouldAllocateDataBuffer = true;
}

//--------------------------------------------------------------------------------------------------
