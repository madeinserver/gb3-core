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
#include "NiFloodgatePCH.h"
#include "NiStreamProcessor.h"
#include "NiSPWorkflowImpl_SDL2.h"
#include "NiSPWorkflow.h"
#include "NiSPKernelMacros_SDL2.h"

//--------------------------------------------------------------------------------------------------
NiSPWorkflowImpl::NiSPWorkflowImpl()
{
}

//--------------------------------------------------------------------------------------------------
NiSPWorkflowImpl::~NiSPWorkflowImpl()
{
}

//--------------------------------------------------------------------------------------------------
bool NiSPWorkflowImpl::Initialize()
{
    return true;
}

//--------------------------------------------------------------------------------------------------
void NiSPWorkflowImpl::Execute(NiSPWorkflow* pkWorkflow)
{
    NiStreamProcessor::Get()->GetThreadPool()->ExecuteWorkflow(pkWorkflow);
}

//--------------------------------------------------------------------------------------------------
void NiSPWorkflowImpl::Terminate()
{
}

//--------------------------------------------------------------------------------------------------
