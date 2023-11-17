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

#include "RenderContext.h"
#include "RenderService.h"
#include <NiMeshCullingProcess.h>
#include <NiAlphaSortProcessor.h>
#include <NiShaderSortProcessor.h>
#include <NiShadowManager.h>
#include <efd/ServiceManager.h>

using namespace efd;
using namespace egf;
using namespace ecr;

//--------------------------------------------------------------------------------------------------
RenderContext::RenderContext(RenderService* pRenderService, Ni3DRenderView* pRenderView) :
    m_pRenderService(pRenderService),
    m_visibleArray(1024, 1024),
    m_backgroundColor(0.5f, 0.5f, 0.5f, 1.0f),
    m_invalid(true),
    m_validate(false)
{
    // Setup main render view
    if (pRenderView)
        m_spMainRenderView = pRenderView;
    else
        m_spMainRenderView = EE_NEW Ni3DRenderView();

    NiSPWorkflowManager* pWorkflowManager = NULL;
    SceneGraphService* pSceneGraphService =
        m_pRenderService->GetServiceManager()->GetSystemServiceAs<ecr::SceneGraphService>();
    if (pSceneGraphService)
        pWorkflowManager = pSceneGraphService->GetWorkflowManager();

    m_spCuller = EE_NEW NiMeshCullingProcess(&m_visibleArray, pWorkflowManager);
    m_spMainRenderView->SetCullingProcess(m_spCuller);
}
//--------------------------------------------------------------------------------------------------
RenderContext::~RenderContext()
{
    efd::UInt32 count = static_cast<efd::UInt32>(m_renderSurfaces.size());
    for (efd::UInt32 ui = 0; ui < count; ++ui)
    {
        m_renderSurfaces[ui] = 0;
    }

    m_spMainRenderView = 0;
    m_spRenderListProcessor = 0;
    m_spCuller = 0;
}
//--------------------------------------------------------------------------------------------------
void RenderContext::Draw(efd::Float32 currentTime)
{
    if (m_validate && !m_invalid)
        return;

    // Get renderer pointer.
    NiRenderer* pkRenderer = NiRenderer::GetRenderer();
    EE_ASSERT(pkRenderer);

    // Manually open the rendering frame. Since each render surface has a unique
    // NiRenderFrame, each render frame has the usage of BeginFrame(), EndFrame(),
    // and DisplayFrame() disabled. This prevents us from calling
    // DisplayFrame() multiple times for one frame in the case of multiple
    // RenderSurfaces (split screen). Because of this we have to manually
    // call BeginFrame(), EndFrame(), or DisplayFrame()
    // If the render frame can't begin because of, say, a lost device, don't
    // attempt to render.
    if (!pkRenderer->BeginFrame())
        return;

    m_invalid = false;

    efd::UInt32 count = static_cast<efd::UInt32>(m_renderSurfaces.size());

    for (efd::UInt32 ui = 0; ui < count; ++ui)
    {
        RenderSurface* pSurface = m_renderSurfaces[ui];

        if (pSurface->m_spCamera == NULL)
            continue;

        pSurface->m_spCamera->Update(currentTime);

        EE_ASSERT(pSurface->m_spRenderFrame);

        NiRenderStep* pRenderStep = pSurface->GetRenderStep();
        EE_ASSERT(pRenderStep && NiIsKindOf(NiDefaultClickRenderStep, pRenderStep));
        NiDefaultClickRenderStep* pClickRenderStep = (NiDefaultClickRenderStep*)pRenderStep;
        EE_ASSERT(pClickRenderStep->GetOutputRenderTargetGroup());

        // Setup all the clicks to draw with appropriate viewport and camera information.
        const NiTPointerList<NiRenderClickPtr>& kRenderClicks =
            pClickRenderStep->GetRenderClickList();

        NiCamera* pCamera = pSurface->m_spCamera;
        const NiRect<float>& kViewport = pCamera->GetViewPort();

        // Inform the shadowing system of the active camera for Render Surface we are
        // about to draw.
        NiShadowManager::SetSceneCamera(pCamera);

        NiTListIterator kClickIter = kRenderClicks.GetHeadPos();
        while (kClickIter)
        {
            const NiRenderClickPtr& kClick = kRenderClicks.GetNext(kClickIter);
            kClick->SetViewport(kViewport);

            NiViewRenderClickPtr kViewClick = NiDynamicCast(NiViewRenderClick, kClick);
            if (kViewClick != NULL)
            {
                const NiTPointerList<NiRenderViewPtr>& kRenderViews =
                    kViewClick->GetRenderViews();

                NiTListIterator kViewIter = kRenderViews.GetHeadPos();
                while (kViewIter)
                {
                    Ni3DRenderViewPtr kView =
                        NiDynamicCast(Ni3DRenderView, kRenderViews.GetNext(kViewIter));

                    if (kView)
                        kView->SetCamera(pCamera);
                }
            }
        }

        RaisePreDrawEvent(pSurface);
        pSurface->Draw(currentTime);
        RaisePostDrawEvent(pSurface);
    }

    // Manually close the rendering frame.
    pkRenderer->EndFrame();

    // Manually display the frame.
    pkRenderer->DisplayFrame();
}
//--------------------------------------------------------------------------------------------------
void RenderContext::AddRenderSurface(RenderSurfacePtr spSurface, 
    NiRenderListProcessor* pCustomRenderListProcessor /* = NULL */)
{
    // Set up the scene render click for rendering the scene graphs in the render view.
    NiViewRenderClick* pRenderClick = spSurface->GetSceneRenderClick();

    pRenderClick->AppendRenderView(m_spMainRenderView);
    pRenderClick->SetUseRendererBackgroundColor(false);
    pRenderClick->SetPersistBackgroundColorToRenderer(true);
    pRenderClick->SetBackgroundColor(m_backgroundColor);
    pRenderClick->SetClearAllBuffers(true);

    if (pCustomRenderListProcessor != NULL)
    {
        m_spRenderListProcessor = pCustomRenderListProcessor;
    }
    else
    {
        // Has list processor been previous initialized?
        if (m_spRenderListProcessor == NULL)
        {
            m_spRenderListProcessor = EE_NEW NiShaderSortProcessor();
        }
    }
    pRenderClick->SetProcessor(m_spRenderListProcessor);

    spSurface->m_pRenderContext = this;

    m_renderSurfaces.push_back(spSurface);

    RaiseSurfaceAdded(spSurface);
}
//--------------------------------------------------------------------------------------------------
void RenderContext::RemoveRenderSurface(efd::WindowRef window)
{
    RemoveRenderSurface(GetRenderSurface(window));
}
//--------------------------------------------------------------------------------------------------
void RenderContext::RemoveRenderSurface(RenderSurface* pSurface)
{
    RenderSurfacePtr spSurface = pSurface;

    m_renderSurfaces.erase(m_renderSurfaces.find(spSurface));

    spSurface->m_pRenderContext = NULL;

    RaiseSurfaceRemoved(spSurface);
}
//--------------------------------------------------------------------------------------------------
RenderSurface* RenderContext::GetRenderSurface(efd::WindowRef window) const
{
    efd::UInt32 count = static_cast<efd::UInt32>(m_renderSurfaces.size());
    for (efd::UInt32 i = 0; i < count; i++)
    {
        if (m_renderSurfaces[i]->GetWindowRef() == window)
            return m_renderSurfaces[i];
    }

    return NULL;
}
//--------------------------------------------------------------------------------------------------
RenderSurface* RenderContext::GetRenderSurfaceAt(efd::UInt32 index) const
{
    EE_ASSERT(index < static_cast<efd::UInt32>(m_renderSurfaces.size()));

    return m_renderSurfaces[index];
}
//--------------------------------------------------------------------------------------------------
void RenderContext::SetBackgroundColor(const NiColorA& kColor)
{
    m_backgroundColor = kColor;

    efd::UInt32 count = static_cast<efd::UInt32>(m_renderSurfaces.size());
    for (efd::UInt32 ui = 0; ui < count; ++ui)
    {
        NiViewRenderClick* pRenderClick = m_renderSurfaces[ui]->GetSceneRenderClick();
        pRenderClick->SetBackgroundColor(m_backgroundColor);
    }
}
//--------------------------------------------------------------------------------------------------
void RenderContext::AddRenderedEntity(egf::Entity* pEntity, NiAVObject* pAVObject)
{
    EE_UNUSED_ARG(pEntity);

    m_spMainRenderView->AppendScene(pAVObject);
}
//--------------------------------------------------------------------------------------------------
void RenderContext::RemoveRenderedEntity(egf::Entity* pEntity, NiAVObject* pAVObject)
{
    EE_UNUSED_ARG(pEntity);

    m_spMainRenderView->RemoveScene(pAVObject);
}
//--------------------------------------------------------------------------------------------------
void RenderContext::AddRenderedObject(SceneGraphService::SceneGraphHandle handle,
    NiAVObject* pAVObject)
{
    EE_UNUSED_ARG(handle);

    m_spMainRenderView->AppendScene(pAVObject);
}
//--------------------------------------------------------------------------------------------------
void RenderContext::RemoveRenderedObject(SceneGraphService::SceneGraphHandle handle,
    NiAVObject* pAVObject)
{
    EE_UNUSED_ARG(handle);

    m_spMainRenderView->RemoveScene(pAVObject);
}
//--------------------------------------------------------------------------------------------------
void RenderContext::RaiseSurfaceAdded(RenderSurface* pSurface)
{
    m_pRenderService->RaiseSurfaceAdded(pSurface);
}
//--------------------------------------------------------------------------------------------------
void RenderContext::RaiseSurfaceRemoved(RenderSurface* pSurface)
{
    m_pRenderService->RaiseSurfaceRemoved(pSurface);
}
//--------------------------------------------------------------------------------------------------
void RenderContext::RaisePreDrawEvent(RenderSurface* pSurface)
{
    m_pRenderService->RaisePreDrawEvent(pSurface);
}
//--------------------------------------------------------------------------------------------------
void RenderContext::RaisePostDrawEvent(RenderSurface* pSurface)
{
    m_pRenderService->RaisePostDrawEvent(pSurface);
}
//--------------------------------------------------------------------------------------------------
