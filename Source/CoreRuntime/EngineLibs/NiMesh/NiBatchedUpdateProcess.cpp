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

// Precompiled Header
#include "NiMeshPCH.h"

#include "NiBatchedUpdateProcess.h"

//--------------------------------------------------------------------------------------------------

NiImplementRTTI(NiBatchedUpdateProcess, NiMeshUpdateProcess);

//--------------------------------------------------------------------------------------------------
NiBatchedUpdateProcess::NiBatchedUpdateProcess(
    NiSPWorkflowManager* pkWorkflowManager)
    : NiMeshUpdateProcess(pkWorkflowManager)
{
}

//--------------------------------------------------------------------------------------------------
NiBatchedUpdateProcess::~NiBatchedUpdateProcess()
{
}

//--------------------------------------------------------------------------------------------------
void NiBatchedUpdateProcess::PreUpdate(NiAVObject*)
{
}

//--------------------------------------------------------------------------------------------------
void NiBatchedUpdateProcess::PostUpdate(NiAVObject*)
{
}

//--------------------------------------------------------------------------------------------------
void NiBatchedUpdateProcess::Flush()
{
    m_pkWorkflowManager->FlushTaskGroup(NiSyncArgs::SYNC_ANY,
        NiSyncArgs::SYNC_ANY);
}

//--------------------------------------------------------------------------------------------------
