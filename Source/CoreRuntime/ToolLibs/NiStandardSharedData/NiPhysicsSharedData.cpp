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

#include "NiPhysicsSharedData.h"

NiImplementRTTI(NiPhysicsSharedData, NiSharedData);

//--------------------------------------------------------------------------------------------------
NiPhysicsSharedData::NiPhysicsSharedData() : m_scaleFactor(1.0f)
{
}

//--------------------------------------------------------------------------------------------------
NiPhysicsSharedData::~NiPhysicsSharedData()
{
}

//--------------------------------------------------------------------------------------------------
efd::Float32 NiPhysicsSharedData::GetScaleFactor()
{
    return m_scaleFactor;
}

//--------------------------------------------------------------------------------------------------
void NiPhysicsSharedData::SetScaleFactor(efd::Float32 scaleFactor)
{
    m_scaleFactor = scaleFactor;
}

//--------------------------------------------------------------------------------------------------
