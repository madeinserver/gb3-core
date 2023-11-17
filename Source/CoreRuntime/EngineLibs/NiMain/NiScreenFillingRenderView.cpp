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

// Precompiled Header
#include "NiMainPCH.h"

#include "NiScreenFillingRenderView.h"
#include "NiRenderer.h"
#include "NiDynamicEffectState.h"

NiImplementRTTI(NiScreenFillingRenderView, NiRenderView);

NiScreenFillingRenderView* (*NiScreenFillingRenderView::CreateFunc)() = NULL;

//--------------------------------------------------------------------------------------------------
NiScreenFillingRenderView::NiScreenFillingRenderView() :
    m_bEffectsChanged(false)
{
}

//--------------------------------------------------------------------------------------------------
NiScreenFillingRenderView::~NiScreenFillingRenderView()
{
}

//--------------------------------------------------------------------------------------------------
void NiScreenFillingRenderView::SetCameraData(const NiRect<float>& kViewport)
{
    // Get renderer pointer.
    NiRenderer* pkRenderer = NiRenderer::GetRenderer();
    EE_ASSERT(pkRenderer);

    // Set screen-space camera data using viewport.
    pkRenderer->SetScreenSpaceCameraData(&kViewport);
}

//--------------------------------------------------------------------------------------------------
