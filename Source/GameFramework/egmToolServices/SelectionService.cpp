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

#include "egmToolServicesPCH.h"

#include <Math.h>
#include <NiInputKeyboard.h>
#include <NiMeshScreenElements.h>
#include <NiMeshCullingProcess.h>
#include <NiMeshUpdateProcess.h>
#include <NiMesh2DRenderView.h>
#include <NiSingleShaderMaterial.h>
#include <NiVertexColorProperty.h>
#include <NiStencilProperty.h>

#include <efd/IDs.h>
#include <efd/ParseHelper.h>
#include <efd/MessageService.h>
#include <efd/ServiceManager.h>

#include <efd/ILogger.h>
#include <efd/ecrLogIDs.h>

#include <egf/EntityManager.h>
#include <egf/StandardModelLibraryPropertyIDs.h>

#include <ecrInput/MouseMessages.h>

#include <ecr/PickService.h>
#include <ecr/RenderService.h>
#include <ecr/SceneGraphService.h>

#include "SelectionService.h"
#include "EntitySelectionAdapter.h"

#include "ToolCamera.h"
#include "ToolCameraService.h"
#include "ToolServicesMessages.h"
#include "CustomScreenFillingRenderView.h"

#include <NiSkinningMeshModifier.h>
#include <NiDataStreamPrimitiveLock.h>
#include <NiMeshAlgorithms.h>
#include <NiMath.h>

using namespace efd;
using namespace egf;
using namespace ecrInput;
using namespace ecr;
using namespace egmToolServices;

EE_IMPLEMENT_CONCRETE_CLASS_INFO(SelectionService);

EE_HANDLER(SelectionService, HandleInputAction, egmToolServices::InputActionMessage);

//-----------------------------------------------------------------------------------------------
SelectionService::NiSelectionViewRenderClick::NiSelectionViewRenderClick(SelectionService* pSelectionService)
{
    m_pSelectionService = pSelectionService;
}

//-----------------------------------------------------------------------------------------------
void SelectionService::NiSelectionViewRenderClick::PerformRendering(unsigned int uiFrameID)
{
    NiViewRenderClick::PerformRendering(uiFrameID);

    for (AdapterMap::iterator i = m_pSelectionService->m_adapters.begin();
         i != m_pSelectionService->m_adapters.end();
         ++i)
    {
        SelectionAdapter* pAdapter = i->second;
        pAdapter->RenderSelection(m_pSelectionService->m_spRenderService, uiFrameID);
    }
}


//------------------------------------------------------------------------------------------------
SelectionService::SelectionService()
    : m_pMessageService(NULL)
    , m_visibleArray(128, 128)
    , m_mutliSelecting(false)
    , m_alternativeSelecting(false)
{
    // If this default priority is changed, also update the service quick reference documentation
    m_defaultPriority = 2060;
}

//------------------------------------------------------------------------------------------------
SelectionService::~SelectionService()
{
}

//------------------------------------------------------------------------------------------------
const char* SelectionService::GetDisplayName() const
{
    return "SelectionService";
}

//------------------------------------------------------------------------------------------------
SyncResult SelectionService::OnPreInit(efd::IDependencyRegistrar* pDependencyRegistrar)
{
    pDependencyRegistrar->AddDependency<SceneGraphService>();
    pDependencyRegistrar->AddDependency<RenderService>();
    pDependencyRegistrar->AddDependency<egf::EntityManager>();

    m_pMessageService = m_pServiceManager->GetSystemServiceAs<efd::MessageService>();
    EE_ASSERT(m_pMessageService);

    m_pMessageService->RegisterFactoryMethod(
        KeyDownMessage::CLASS_ID,
        KeyDownMessage::FactoryMethod);
    m_pMessageService->RegisterFactoryMethod(KeyUpMessage::CLASS_ID, KeyUpMessage::FactoryMethod);

    m_spSceneGraphService = m_pServiceManager->GetSystemServiceAs<SceneGraphService>();
    EE_ASSERT(m_spSceneGraphService);

    m_spPickService = m_pServiceManager->GetSystemServiceAs<PickService>();
    EE_ASSERT(m_spPickService);

    m_spRenderService = m_pServiceManager->GetSystemServiceAs<RenderService>();
    EE_ASSERT(m_spRenderService);

    m_pMessageService->Subscribe(this, kCAT_LocalMessage);

    AddAdapter<EntitySelectionAdapter>(EE_NEW EntitySelectionAdapter(m_pServiceManager));

    return SyncResult_Success;
}

//------------------------------------------------------------------------------------------------
AsyncResult SelectionService::OnInit()
{
    m_spSelectionRenderView = NiNew Ni3DRenderView();

    //m_spAccumulatorProcessor = NiNew NiAccumulatorProcessor();
    m_spAlphaSortProcessor = NiNew NiAlphaSortProcessor();

    // Now we create another screen view that holds an alpha-blended selection rectangle.
    m_spSelectionRect = EE_NEW NiMeshScreenElements(false, true, 1);
    m_spSelectionRect->AddNewScreenRect(0, 0, 320, 200, 0, 0);

    m_spSelectionView = EE_NEW NiMesh2DRenderView();

    // Attach an alpha blend property to this quad.
    NiAlphaProperty* pAlphaProp = EE_NEW NiAlphaProperty();
    pAlphaProp->SetAlphaBlending(true);
    pAlphaProp->SetSrcBlendMode(NiAlphaProperty::ALPHA_SRCALPHA);
    pAlphaProp->SetDestBlendMode(NiAlphaProperty::ALPHA_INVSRCALPHA);
    pAlphaProp->SetAlphaTesting(false);
    pAlphaProp->SetTestMode(NiAlphaProperty::TEST_GREATER);
    pAlphaProp->SetTestRef(0);
    m_spSelectionRect->AttachProperty(pAlphaProp);

    // Create and add a vertex lighting mode and set vertex colors.
    NiVertexColorProperty* pSelectionVertexColorProp = EE_NEW NiVertexColorProperty();
    pSelectionVertexColorProp->SetSourceMode(NiVertexColorProperty::SOURCE_EMISSIVE);
    pSelectionVertexColorProp->SetLightingMode(NiVertexColorProperty::LIGHTING_E);
    m_spSelectionRect->AttachProperty(pSelectionVertexColorProp);

    NiColorA fillColor(1.0f, 1.0f, 1.0f, 0.2f);
    NiColorA lineColor(1.0f, 1.0f, 1.0f, 1.0f);

    m_spSelectionRect->SetColors(0, fillColor);
    m_spSelectionRect->UpdateProperties();

    // Append selection rectangle
    m_spSelectionView->AppendScreenElement(m_spSelectionRect);

    // Create four more screen elements to outline the selection rect.
    for (int i = 0; i < 4; i++)
    {
        NiMeshScreenElementsPtr spSelectionLine = EE_NEW NiMeshScreenElements(false, true, 1);
        spSelectionLine->AddNewScreenRect(0, 0, 16, 16, 0, 0);

        // Attach an alpha blend property to this quad.
        pAlphaProp = EE_NEW NiAlphaProperty();
        pAlphaProp->SetAlphaBlending(true);
        pAlphaProp->SetSrcBlendMode(NiAlphaProperty::ALPHA_SRCALPHA);
        pAlphaProp->SetDestBlendMode(NiAlphaProperty::ALPHA_INVSRCALPHA);
        pAlphaProp->SetAlphaTesting(false);
        pAlphaProp->SetTestMode(NiAlphaProperty::TEST_GREATER);
        pAlphaProp->SetTestRef(0);
        spSelectionLine->AttachProperty(pAlphaProp);

        // Create and add a vertex lighting mode and set vertex colors.
        NiVertexColorProperty* pSelectionVertexColorProp = EE_NEW NiVertexColorProperty();
        pSelectionVertexColorProp->SetSourceMode(NiVertexColorProperty::SOURCE_EMISSIVE);
        pSelectionVertexColorProp->SetLightingMode(NiVertexColorProperty::LIGHTING_E);
        spSelectionLine->AttachProperty(pSelectionVertexColorProp);

        spSelectionLine->SetColors(0, lineColor);
        spSelectionLine->UpdateProperties();

        m_spSelectionView->AppendScreenElement(spSelectionLine);
        m_spSelectionLines.push_back(spSelectionLine);
    }

    NiSPWorkflowManager* pWorkflowManager = m_spSceneGraphService->GetWorkflowManager();
    m_spCuller = EE_NEW NiMeshCullingProcess(&m_visibleArray, pWorkflowManager);
    m_spSelectionRenderView->SetCullingProcess(m_spCuller);

    m_spRenderService->AddDelegate(this);

    return AsyncResult_Complete;
}

//------------------------------------------------------------------------------------------------
AsyncResult SelectionService::OnShutdown()
{
    EE_ASSERT(m_pServiceManager);

    while (m_adapters.begin() != m_adapters.end())
    {
        RemoveAdapter(m_adapters.begin()->second);
    }

    m_pMessageService = m_pServiceManager->GetSystemServiceAs<efd::MessageService>();

    if (m_pMessageService != NULL)
    {
        m_pMessageService->Unsubscribe(this, kCAT_LocalMessage);
        m_pMessageService = NULL;
    }

    if (m_spRenderService)
    {
        m_spRenderService->RemoveDelegate(this);
        m_spRenderService = NULL;
    }

    m_spPickService = NULL;
    m_spSceneGraphService = NULL;

    m_spSelectionView = NULL;
    m_spSelectionRect = NULL;
    m_spSelectionLines.clear();

    m_spSelectionRenderView = NULL;
    m_spCuller = NULL;
    m_spAlphaSortProcessor = NULL;

    return AsyncResult_Complete;
}

//-----------------------------------------------------------------------------------------------
void SelectionService::OnSurfaceAdded(RenderService* pService, RenderSurface* pSurface)
{
    EE_UNUSED_ARG(pService);

    SetupScreenFillingRenderViews(pSurface);
    CreateRenderedTextureTargets(pSurface);
}

//-----------------------------------------------------------------------------------------------
void SelectionService::OnSurfaceRecreated(RenderService* pService, RenderSurface* pSurface)
{
    EE_UNUSED_ARG(pService);
    EE_ASSERT(pSurface);

    CreateRenderedTextureTargets(pSurface);
}

//-----------------------------------------------------------------------------------------------
void SelectionService::OnSurfaceRemoved(RenderService* pService, RenderSurface* pSurface)
{
    EE_UNUSED_ARG(pService);
    EE_UNUSED_ARG(pSurface);
}

//-----------------------------------------------------------------------------------------------
Ni3DRenderView* SelectionService::GetSelectionView()
{
    return m_spSelectionRenderView;
}

//-----------------------------------------------------------------------------------------------
bool SelectionService::IsSelectingMultiple() const
{
    return m_mutliSelecting;
}

//-----------------------------------------------------------------------------------------------
bool SelectionService::IsSelectingAlternative() const
{
    return m_alternativeSelecting;
}

//-----------------------------------------------------------------------------------------------
void SelectionService::HandleInputAction(const InputActionMessage* pMsg, efd::Category targetChannel)
{
    EE_ASSERT(m_pServiceManager);

    const utf8string& kName = pMsg->GetName();

    if (kName == "Multi Select")
        m_mutliSelecting = pMsg->GetActive();
    else if (kName == "Alternative Select")
        m_alternativeSelecting = pMsg->GetActive();
}

//-----------------------------------------------------------------------------------------------
SelectionService::SelectionMode SelectionService::GetSelectionMode() const
{
    if (IsSelectingMultiple() && IsSelectingAlternative())
        return SELECTIONMODE_SUBTRACT;
    else if (!IsSelectingMultiple() && IsSelectingAlternative())
        return SELECTIONMODE_ADD;
    else if (IsSelectingMultiple())
        return SELECTIONMODE_INVERT;
    else
        return SELECTIONMODE_REPLACE;
}

//-----------------------------------------------------------------------------------------------
void SelectionService::Select(ecr::RenderSurface* pSurface, efd::SInt32 x, efd::SInt32 y)
{
    NiPoint3 rayOrigin;
    NiPoint3 rayDirection;

    CameraService::MouseToRay((float)x, (float)y,
        pSurface->GetRenderTargetGroup()->GetWidth(0),
        pSurface->GetRenderTargetGroup()->GetHeight(0),
        pSurface->GetCamera(), rayOrigin, rayDirection);

    efd::Point3 origin = efd::Point3(rayOrigin.x, rayOrigin.y, rayOrigin.z);
    efd::Point3 direction = efd::Point3(rayDirection.x, rayDirection.y, rayDirection.z);

    for (AdapterMap::iterator it = m_adapters.begin(); it != m_adapters.end(); ++it)
    {
        SelectionAdapter* pAdapter = it->second;
        pAdapter->Select(pSurface, x, y, origin, direction);
    }
}

//-----------------------------------------------------------------------------------------------
void SelectionService::CreateRenderedTextureTargets(RenderSurface* pSurface)
{
    efd::UInt32 surfaceWidth = 1;
    efd::UInt32 surfaceHeight = 1;

    if (pSurface)
    {
        surfaceWidth = pSurface->GetRenderTargetGroup()->GetWidth(0);
        surfaceHeight = pSurface->GetRenderTargetGroup()->GetHeight(0);
    }

    // Create a render texture to hold the highlighted entity selection.
    NiTexture::FormatPrefs kPrefs;
    kPrefs.m_ePixelLayout = NiTexture::FormatPrefs::TRUE_COLOR_32;
    kPrefs.m_eMipMapped = NiTexture::FormatPrefs::NO;
    kPrefs.m_eAlphaFmt = NiTexture::FormatPrefs::SMOOTH;

    NiDefaultClickRenderStep* pDefaultRenderStep =
        (NiDefaultClickRenderStep*)pSurface->GetRenderStep();
    EE_ASSERT(pDefaultRenderStep);

    NiRenderedTexturePtr spSelectedTextureTarget = NiRenderedTexture::Create(
        surfaceWidth, surfaceHeight,
        m_spRenderService->GetRenderer(), kPrefs);

    if (spSelectedTextureTarget == NULL)
    {
        EE_LOG(efd::kGamebryoGeneral0,
            efd::ILogger::kERR0,
            ("Unable to create selection texture render target.  This could have occured "
            "because there is insufficient contiguous video memory available."));

        spSelectedTextureTarget = NULL;

        return;
    }

    NiDepthStencilBuffer* pDSB = pSurface->GetRenderTargetGroup()->GetDepthStencilBuffer();
    EE_ASSERT(pDSB);

    // Give the depth/stencil buffer from the first render target to the second
    // render target.
    NiRenderTargetGroupPtr spSelectedRenderGroup = NiRenderTargetGroup::Create(
        spSelectedTextureTarget->GetBuffer(),
        m_spRenderService->GetRenderer(),
        pDSB);

    // Now we need to attach the render groups and rendered textures to the
    // render frame associated with the render surface.

    NiViewRenderClick* pSelectedTextureClick = (NiViewRenderClick*)
        pDefaultRenderStep->GetRenderClickByName("SelectedTextureClick");

    // Attach the selected texture to the texturing property.
    NiScreenFillingRenderView* pSelectedView = (NiScreenFillingRenderView*)
        pSelectedTextureClick->GetRenderViewByName("SelectedView");

    NiTexturingProperty* pSelectedTexturingProperty = (NiTexturingProperty*)
        pSelectedView->GetProperty(NiTexturingProperty::GetType());

    pSelectedTexturingProperty->SetBaseTexture(spSelectedTextureTarget);

    NiViewRenderClick* pBlitClearAlphaClick = (NiViewRenderClick*)
        pDefaultRenderStep->GetRenderClickByName("BlitClearAlphaClick");

    pBlitClearAlphaClick->SetRenderTargetGroup(spSelectedRenderGroup);
}

//------------------------------------------------------------------------------------------------
void SelectionService::SetupScreenFillingRenderViews(RenderSurfacePtr spSurface)
{
    NiDefaultClickRenderStep* pRenderStep = (NiDefaultClickRenderStep*)spSurface->GetRenderStep();

    // Create shared z-buffer property.
    NiZBufferPropertyPtr spZBufferProp = NiNew NiZBufferProperty();
    spZBufferProp->SetZBufferTest(false);
    spZBufferProp->SetZBufferWrite(false);

    NiTexturingPropertyPtr spSelectedTexturingProperty = NiNew NiTexturingProperty();
    spSelectedTexturingProperty->SetApplyMode(NiTexturingProperty::APPLY_REPLACE);
    spSelectedTexturingProperty->SetBaseMap(NiNew NiTexturingProperty::Map());
    spSelectedTexturingProperty->SetBaseFilterMode(NiTexturingProperty::FILTER_NEAREST);
    spSelectedTexturingProperty->SetBaseClampMode(NiTexturingProperty::CLAMP_S_CLAMP_T);

    NiViewRenderClickPtr spSelectedTextureClick = NiNew NiViewRenderClick();
    spSelectedTextureClick->SetName("SelectedTextureClick");
    spSelectedTextureClick->SetClearAllBuffers(false);
    spSelectedTextureClick->SetRenderTargetGroup(spSurface->GetRenderTargetGroup());

    NiViewRenderClickPtr spBlitClearAlphaClick = NiNew NiSelectionViewRenderClick(this);
    spBlitClearAlphaClick->SetName("BlitClearAlphaClick");
    spBlitClearAlphaClick->SetUseRendererBackgroundColor(false);
    spBlitClearAlphaClick->SetPersistBackgroundColorToRenderer(true);
    spBlitClearAlphaClick->SetBackgroundColor(NiColorA(0.0f, 0.0f, 0.0f, 0.0f));
    spBlitClearAlphaClick->SetClearColorBuffers(true);
    spBlitClearAlphaClick->SetClearDepthBuffer(false);
    spBlitClearAlphaClick->SetClearStencilBuffer(false);

    // First we need a render view that takes the unselected entities as a
    // texture input and copies that to another texture while at the same time
    // setting the alpha value for each pixel to zero.
    NiScreenFillingRenderViewPtr spScreenQuad = NiNew CustomScreenFillingRenderView();
    spScreenQuad->AttachProperty(spZBufferProp);

    // Create screen quad shader material.
    NiSingleShaderMaterial* m_pTexColorNoAlphaMaterial =
        NiSingleShaderMaterial::Create("TexColorNoAlpha");
    spScreenQuad->GetScreenFillingQuad().ApplyAndSetActiveMaterial(m_pTexColorNoAlphaMaterial);
    spScreenQuad->GetScreenFillingQuad().AddExtraData("NiNoMaterialSwap", NiNew NiExtraData());

    // Create a render click bound to our second rendered texture. This will
    // then be used to blend in the selected entities.

    // The selected entity render view needs to be appended to the same click
    // as the alpha clearing quad.
    spBlitClearAlphaClick->AppendRenderView(spScreenQuad);
    spBlitClearAlphaClick->AppendRenderView(m_spSelectionRenderView);
    spBlitClearAlphaClick->SetProcessor(m_spAlphaSortProcessor);
    pRenderStep->AppendRenderClick(spBlitClearAlphaClick);

    // Now create another screen view with which we will render the selected
    // entities texture to.
    spScreenQuad = NiNew CustomScreenFillingRenderView();
    spScreenQuad->SetName("SelectedView");
    spScreenQuad->AttachProperty(spZBufferProp);
    spScreenQuad->AttachProperty(spSelectedTexturingProperty);

    // Create and add stencil property.
    NiStencilProperty* pScreenStencilProp = NiNew NiStencilProperty();
    pScreenStencilProp->SetStencilOn(true);
    pScreenStencilProp->SetStencilFunction(NiStencilProperty::TEST_ALWAYS);
    pScreenStencilProp->SetStencilReference(1);
    pScreenStencilProp->SetStencilPassAction(NiStencilProperty::ACTION_REPLACE);
    spScreenQuad->AttachProperty(pScreenStencilProp);

    // Attach an alpha blend property to this quad.
    NiAlphaProperty* pAlphaProp = NiNew NiAlphaProperty();
    pAlphaProp->SetAlphaBlending(true);
    pAlphaProp->SetSrcBlendMode(NiAlphaProperty::ALPHA_SRCALPHA);
    pAlphaProp->SetDestBlendMode(NiAlphaProperty::ALPHA_INVSRCALPHA);
    pAlphaProp->SetAlphaTesting(true);
    pAlphaProp->SetTestMode(NiAlphaProperty::TEST_GREATER);
    pAlphaProp->SetTestRef(0);
    spScreenQuad->AttachProperty(pAlphaProp);

    spSelectedTextureClick->AppendRenderView(spScreenQuad);

    // Now create another screen view with which we will use to render a
    // shaded quad on top of the selected entities thus "highlighting" them.
    spScreenQuad = NiNew CustomScreenFillingRenderView();
    spScreenQuad->AttachProperty(spZBufferProp);

    // Create and add texturing property.
    NiTexturingProperty* pScreenTexProp = NiNew NiTexturingProperty();
    pScreenTexProp->SetBaseTexture(NULL);
    pScreenTexProp->SetApplyMode(NiTexturingProperty::APPLY_MODULATE);
    spScreenQuad->AttachProperty(pScreenTexProp);

    // Create and add a stencil property.
    pScreenStencilProp = NiNew NiStencilProperty();
    pScreenStencilProp->SetStencilOn(true);
    pScreenStencilProp->SetStencilFunction(NiStencilProperty::TEST_EQUAL);
    pScreenStencilProp->SetStencilReference(1);
    pScreenStencilProp->SetStencilPassAction(NiStencilProperty::ACTION_KEEP);
    spScreenQuad->AttachProperty(pScreenStencilProp);

    // Attach an alpha blend property to this quad.
    pAlphaProp = NiNew NiAlphaProperty();
    pAlphaProp->SetAlphaBlending(true);
    pAlphaProp->SetSrcBlendMode(NiAlphaProperty::ALPHA_SRCALPHA);
    pAlphaProp->SetDestBlendMode(NiAlphaProperty::ALPHA_INVSRCALPHA);
    pAlphaProp->SetAlphaTesting(false);
    pAlphaProp->SetTestMode(NiAlphaProperty::TEST_GREATER);
    pAlphaProp->SetTestRef(0);
    spScreenQuad->AttachProperty(pAlphaProp);

    // Create and add a vertex lighting mode and set vertex colors.
    NiVertexColorProperty* pScreenVertexColorProp = NiNew NiVertexColorProperty();
    pScreenVertexColorProp->SetSourceMode(NiVertexColorProperty::SOURCE_EMISSIVE);
    pScreenVertexColorProp->SetLightingMode(NiVertexColorProperty::LIGHTING_E);
    spScreenQuad->AttachProperty(pScreenVertexColorProp);

    NiMeshScreenElements& kMeshScreenElements =
        (NiMeshScreenElements&)spScreenQuad->GetScreenFillingQuad();

    NiColorA kHighlightColor = NiColorA(1.0f, 0.0f, 0.0f, 0.3f);
    kMeshScreenElements.SetColors(0, kHighlightColor);

    spSelectedTextureClick->AppendRenderView(spScreenQuad);

    pRenderStep->AppendRenderClick(spSelectedTextureClick);
}

//-----------------------------------------------------------------------------------------------
void SelectionService::SetSelectionRectangle(int x0, int y0, int x1, int y1, int width, int height)
{
    RenderSurface* pActiveSurface = m_spRenderService->GetActiveRenderSurface();
    EE_ASSERT(pActiveSurface != NULL);

    NiDefaultClickRenderStep* pDefaultRenderStep =
        (NiDefaultClickRenderStep*)pActiveSurface->GetRenderStep();
    NiViewRenderClick* pSelectedTextureClick =
        (NiViewRenderClick*)pDefaultRenderStep->GetRenderClickByName("SelectedTextureClick");
    EE_ASSERT(pSelectedTextureClick != NULL);

    pSelectedTextureClick->RemoveRenderView(m_spSelectionView);
    pSelectedTextureClick->AppendRenderView(m_spSelectionView);

    if (x1 < x0)
        EE_STL_NAMESPACE::swap(x0, x1);
    if (y1 < y0)
        EE_STL_NAMESPACE::swap(y0, y1);

    float ndcTop = y0 / (float) height;
    float ndcLeft = x0 / (float) width;
    float ndcWidth = (x1 - x0) / (float) width;
    float ndcHeight = (y1 - y0) / (float) height;

    float ndcHoriz = 1.0f / width;
    float ndcVert = 1.0f / height;

    m_spSelectionRect->SetRectangle(0, ndcLeft, ndcTop, ndcWidth, ndcHeight);
    m_spSelectionRect->SetAppCulled(false);
    m_spSelectionRect->Update(0);

    // Top line
    m_spSelectionLines[0]->SetRectangle(
        0,
        ndcLeft - ndcHoriz,
        ndcTop - ndcVert,
        ndcWidth + 2 * ndcHoriz,
        ndcVert);

    // Left line
    m_spSelectionLines[1]->SetRectangle(
        0,
        ndcLeft - ndcHoriz,
        ndcTop,
        ndcHoriz,
        ndcHeight);

    // Right line
    m_spSelectionLines[2]->SetRectangle(
        0,
        ndcLeft + ndcWidth,
        ndcTop,
        ndcHoriz,
        ndcHeight);

    // Bottom line
    m_spSelectionLines[3]->SetRectangle(
        0,
        ndcLeft - ndcHoriz,
        ndcTop + ndcHeight,
        ndcWidth + 2 * ndcHoriz,
        ndcVert);

    // Set colors
    for (int i = 0; i < 4; i++)
    {
        m_spSelectionLines[i]->SetAppCulled(false);
        m_spSelectionLines[i]->Update(0);
    }

    m_spRenderService->InvalidateRenderContexts();
}

//------------------------------------------------------------------------------------------------
void SelectionService::ClearSelectionRectangle()
{
    RenderSurface* pActiveSurface = m_spRenderService->GetActiveRenderSurface();

    if (pActiveSurface == NULL)
        return;

    NiDefaultClickRenderStep* pDefaultRenderStep =
        (NiDefaultClickRenderStep*)pActiveSurface->GetRenderStep();

    if (pDefaultRenderStep == NULL)
        return;

    NiViewRenderClick* pSelectedTextureClick =
        (NiViewRenderClick*)pDefaultRenderStep->GetRenderClickByName("SelectedTextureClick");

    if (pSelectedTextureClick == NULL)
        return;

    pSelectedTextureClick->RemoveRenderView(m_spSelectionView);


    m_spSelectionRect->SetRectangle(0, 0, 0, 0, 0);
    m_spSelectionRect->SetAppCulled(true);
    m_spSelectionRect->Update(0);
    m_spSelectionRect->UpdateProperties();

    for (int i = 0; i < 4; i++)
    {
        m_spSelectionLines[i]->SetAppCulled(true);
        m_spSelectionLines[i]->Update(0);
        m_spSelectionLines[i]->UpdateProperties();
    }
}

//-----------------------------------------------------------------------------------------------
void SelectionService::AddSelectionRectangle()
{
    float left, top, width, height;
    m_spSelectionRect->GetRectangle(0, left, top, width, height);

    // create frustum
    ToolCameraServicePtr spToolCameraService =
        m_pServiceManager->GetSystemServiceAs<ToolCameraService>();
    ToolCamera* pToolCamera = spToolCameraService->GetActiveCamera();

    NiFrustum cullFrustum, viewFrustum;
    float viewWidth, viewHeight;
    viewFrustum = pToolCamera->GetViewFrustum();
    viewWidth = viewFrustum.m_fRight - viewFrustum.m_fLeft;
    viewHeight = viewFrustum.m_fBottom - viewFrustum.m_fTop;

    cullFrustum.m_bOrtho = viewFrustum.m_bOrtho;
    cullFrustum.m_fNear = viewFrustum.m_fNear;
    cullFrustum.m_fFar = viewFrustum.m_fFar;
    cullFrustum.m_fLeft = left * viewWidth + viewFrustum.m_fLeft;
    cullFrustum.m_fRight = (left + width) * viewWidth + viewFrustum.m_fLeft;
    cullFrustum.m_fBottom = (top + height) * viewHeight + viewFrustum.m_fTop;
    cullFrustum.m_fTop = top * viewHeight + viewFrustum.m_fTop;

    SceneGraphService* pSceneGraphService = m_pServiceManager->GetSystemServiceAs<SceneGraphService>();
    EE_ASSERT(pSceneGraphService);

    NiSPWorkflowManager* pWorkflowManager = pSceneGraphService->GetWorkflowManager();
    NiMeshUpdateProcess updater(pWorkflowManager);

    // create camera
    NiCameraPtr spCamera = EE_NEW NiCamera();
    spCamera->SetLocalFromWorldTransform(pToolCamera->GetWorldTransform());
    spCamera->SetViewFrustum(cullFrustum);
    spCamera->Update(updater);

    NiVisibleArray visibles(16, 16);
    NiMeshCullingProcessPtr spCuller =
        EE_NEW NiMeshCullingProcess(&m_visibleArray, pWorkflowManager);

    for (AdapterMap::iterator it = m_adapters.begin(); it != m_adapters.end(); ++it)
    {
        SelectionAdapter* pAdapter = it->second;
        pAdapter->SelectFrustum(spCamera, spCuller);
    }
}

//-----------------------------------------------------------------------------------------------
void SelectionService::OnSurfacePreDraw(RenderService* pService, RenderSurface* pSurface)
{
    EE_UNUSED_ARG(pService);

    EE_ASSERT(pSurface);
    EE_ASSERT(pSurface->GetRenderStep());

    // Make sure that the BlitClearAlphaClick's processor always matches the main scene's
    // processor.  Otherwise, selected entities can show up different than deselected entities.
    NiRenderListProcessor* pProcessor = pSurface->GetSceneRenderClick()->GetProcessor();

    NiDefaultClickRenderStep* pRenderStep =
        NiDynamicCast(NiDefaultClickRenderStep, pSurface->GetRenderStep());

    if (pRenderStep)
    {
        NiViewRenderClick* pRenderClick =
            NiDynamicCast(NiViewRenderClick,
            pRenderStep->GetRenderClickByName("BlitClearAlphaClick"));

        if (pRenderClick)
        {
            pRenderClick->SetProcessor(pProcessor);
        }
    }
}

//------------------------------------------------------------------------------------------------
