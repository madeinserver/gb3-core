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
inline void NiShadowClickValidator::SetCamera(NiCamera* pkCamera)
{
    m_spCamera = pkCamera;
}

//--------------------------------------------------------------------------------------------------
inline NiCamera* NiShadowClickValidator::GetCamera() const
{
    return m_spCamera;
}

//--------------------------------------------------------------------------------------------------
inline void NiShadowClickValidator::SetCullingProcess(
    NiCullingProcess* pkCullingProcess)
{
    m_spCullingProcess = pkCullingProcess;
}

//--------------------------------------------------------------------------------------------------
inline NiCullingProcess* NiShadowClickValidator::GetCullingProcess() const
{
    return m_spCullingProcess;
}

//--------------------------------------------------------------------------------------------------
