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
#include "ecrPCH.h"

#include "RenderSurface.h"
#include "RenderService.h"
#include "NiRenderFrame.h"
#include "RenderSurfaceStep.h"

using namespace ecr;

//--------------------------------------------------------------------------------------------------
RenderSurface::RenderSurface(efd::WindowRef windowHandle, RenderService* pService) :
    m_windowRef(windowHandle), m_pRenderService(pService)
{
    m_spCamera = NiNew NiCamera(); // Dummy camera

    m_spSceneRenderClick = NiNew NiViewRenderClick;
    m_spSceneRenderClick->SetName("SceneGraphClick");

    NiDefaultClickRenderStep* pClickRenderStep = NiNew RenderSurfaceStep();
    m_spRenderStep = pClickRenderStep;
    pClickRenderStep->AppendRenderClick(m_spSceneRenderClick);

    // Create a new NiRenderFrame, but disable the usage of BeginFrame(), EndFrame()
    // and DisplayFrame() since these will be manually called in the RenderContext.
    m_spRenderFrame = NiNew NiRenderFrame(false);
    m_spRenderFrame->AppendRenderStep(m_spRenderStep);
}
//--------------------------------------------------------------------------------------------------
RenderSurface::~RenderSurface()
{
    m_spCamera = 0;
    m_spRenderFrame = 0;
    m_spRenderStep = 0;
    m_spSceneRenderClick = 0;
}
//--------------------------------------------------------------------------------------------------
void RenderSurface::SetMainRenderClick(NiViewRenderClick* pRenderClick)
{
    EE_ASSERT(pRenderClick);

    NiDefaultClickRenderStep* pClickRenderStep =
        NiDynamicCast(NiDefaultClickRenderStep, m_spRenderStep);

    if (pClickRenderStep)
    {
        // Copy necessary information from the previous main render click
        // This is information that other main scene render clicks likely wouldn't change.
        if (m_spSceneRenderClick)
        {
            pRenderClick->SetRenderTargetGroup(
                m_spSceneRenderClick->GetRenderTargetGroup());

            NiColorA backgroundColor;
            m_spSceneRenderClick->GetBackgroundColor(backgroundColor);
            pRenderClick->SetBackgroundColor(backgroundColor);

            const NiTPointerList<NiRenderViewPtr>& views = m_spSceneRenderClick->GetRenderViews();
            NiTListIterator viewItr = views.GetHeadPos();
            while (viewItr)
            {
                pRenderClick->AppendRenderView(views.Get(viewItr));
                viewItr = views.GetNextPos(viewItr);
            }
        }

        pRenderClick->SetName("SceneGraphClick");

        // Swap out the old render click for the new one
        NiTPointerList<NiRenderClickPtr>& clickList = pClickRenderStep->GetRenderClickList();
        NiTListIterator itr = pClickRenderStep->GetRenderClickPosByName("SceneGraphClick");
        clickList.InsertAfter(itr, (NiRenderClickPtr)pRenderClick);
        clickList.RemovePos(itr);

        m_spSceneRenderClick = pRenderClick;
    }
}
//--------------------------------------------------------------------------------------------------
void RenderSurface::Draw(efd::Float32 currentTime)
{
    EE_UNUSED_ARG(currentTime);

    m_pRenderService->RaisePreDrawEvent(this);

    m_spRenderFrame->Draw();
    m_spRenderFrame->Display();

    m_pRenderService->RaisePostDrawEvent(this);
}
//--------------------------------------------------------------------------------------------------
NiRenderStep* RenderSurface::GetRenderStep()
{
    return m_spRenderStep;
}
//--------------------------------------------------------------------------------------------------
