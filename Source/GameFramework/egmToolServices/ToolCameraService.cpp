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

#include <efd/MessageService.h>
#include <efd/PathUtils.h>

#include <NiMath.h>
#include <NiInputKeyboard.h>
#include <NiShadowManager.h>
#include <NiViewMath.h>

#include <ecrInput/MouseMessages.h>

#include <ecr/RenderService.h>
#include <ecr/CoreRuntimeMessages.h>
#include <egf/egfLogIDs.h>

#include "SelectionService.h"

#include "InteractionService.h"
#include "StandardCameraMode.h"
#include "OrbitCameraMode.h"
#include "PanCameraMode.h"
#include "ToolServicesMessages.h"
#include "ToolSceneService.h"

#include "ToolCameraService.h"
#include <egf/ScriptContext.h>
#include <egf/StandardModelLibraryFlatModelIDs.h>
#include <egf/StandardModelLibraryPropertyIDs.h>

#include "EntitySelectionAdapter.h"

using namespace efd;
using namespace egf;
using namespace ecrInput;
using namespace ecr;
using namespace egmToolServices;

EE_IMPLEMENT_CONCRETE_CLASS_INFO(ToolCameraService);

EE_HANDLER(ToolCameraService, OnInputAction, InputActionMessage);

EE_HANDLER_WRAP(ToolCameraService, HandleEntityRemovedMessage,
                egf::EntityChangeMessage, kMSGID_OwnedEntityExitWorld);

EE_HANDLER_WRAP(ToolCameraService, HandleEntityUpdatedMessage,
                egf::EntityChangeMessage, kMSGID_OwnedEntityUpdated);

//------------------------------------------------------------------------------------------------
ToolCameraService::ToolCameraService() :
    m_activeCameraMode(NULL),
    m_pEntityManager(NULL),
    m_forward(false),
    m_backward(false),
    m_strafeLeft(false),
    m_strafeRight(false),
    m_shift(false),
    m_lastUpdateTime(0.0f),
    m_isLooking(false),
    m_isTranslating(false),
    m_isPanning(false),
    m_turboScale(2.0f),
    m_keyboardBaseMovement(1000.0f),
    m_mouseBaseMovement(30.0f),
    m_movementScale(1.0f),
    m_lookScale(1.0f),
    m_nearPlane(1.0f),
    m_farPlane(10000.0f),
    m_flyLevel(false)
{
    // If this default priority is changed, also update the service quick reference documentation
    m_defaultPriority = 2085;
}

//------------------------------------------------------------------------------------------------
ToolCameraService::~ToolCameraService()
{
    // This method intentionally left blank (all shutdown occurs in OnShutdown)
}

//------------------------------------------------------------------------------------------------
const char* ToolCameraService::GetDisplayName() const
{
    return "ToolCameraService";
}

//------------------------------------------------------------------------------------------------
SyncResult ToolCameraService::OnPreInit(efd::IDependencyRegistrar* pDependencyRegistrar)
{
    pDependencyRegistrar->AddDependency<efd::MessageService>();
    pDependencyRegistrar->AddDependency<ecr::RenderService>();
    pDependencyRegistrar->AddDependency<InteractionService>();

    m_pMessageService = m_pServiceManager->GetSystemServiceAs<efd::MessageService>();
    EE_ASSERT(m_pMessageService);

    m_pMessageService->RegisterFactoryMethod(SetActiveCameraMessage::CLASS_ID,
        SetActiveCameraMessage::FactoryMethod);

    m_spRenderService = m_pServiceManager->GetSystemServiceAs<RenderService>();
    EE_ASSERT(m_spRenderService);

    m_spToolSceneGraphService = EE_DYNAMIC_CAST(ToolSceneGraphService,
        m_pServiceManager->GetSystemServiceAs<SceneGraphService>());
    EE_ASSERT(m_spToolSceneGraphService);

    InteractionService* pInteractionService =
        m_pServiceManager->GetSystemServiceAs<InteractionService>();
    EE_ASSERT(pInteractionService);

    pInteractionService->AddInteractionDelegate(this);

    return SyncResult_Success;
}

//------------------------------------------------------------------------------------------------
AsyncResult ToolCameraService::OnInit()
{
    if (!m_spRenderService->GetRenderer())
        return AsyncResult_Pending;

    m_pEntityManager = m_pServiceManager->GetSystemServiceAs<egf::EntityManager>();
    EE_ASSERT(m_pEntityManager);
    if (!m_pEntityManager)
        return AsyncResult_Failure;

    // Will also need the message service to subscribe to input
    MessageService* pMessageService = m_pServiceManager->GetSystemServiceAs<MessageService>();

    pMessageService->Subscribe(this, kCAT_LocalMessage);

    // Set the base camera values
    Reset();

    m_cameraModeMap["Standard"] = EE_NEW StandardCameraMode(m_pServiceManager);
    m_cameraModeMap["Orbit"] = EE_NEW OrbitCameraMode(m_pServiceManager);
    m_cameraModeMap["Pan"] = EE_NEW PanCameraMode(m_pServiceManager);

    SetActiveCameraMode("Standard");
    CreateDefaultCameras();

    return AsyncResult_Complete;
}

//------------------------------------------------------------------------------------------------
AsyncResult ToolCameraService::OnTick()
{
    efd::TimeType currentTime = m_pServiceManager->GetServiceManagerTime();

    efd::TimeType timeElapsed = currentTime - m_lastUpdateTime;
    m_lastUpdateTime = currentTime;

    RenderSurface* pSurface = m_spRenderService->GetActiveRenderSurface();

    if (pSurface != NULL)
    {
        ToolCamera* pActiveCamera = GetActiveCamera(pSurface);

        if (pActiveCamera != NULL)
        {
            efd::Point3 lastPositon = pActiveCamera->GetWorldTranslate();
            NiMatrix3 lastRotation = pActiveCamera->GetRotate();

            m_activeCameraMode->OnTick(timeElapsed, pActiveCamera);

            if (lastPositon != pActiveCamera->GetWorldTranslate() ||
                lastRotation != pActiveCamera->GetRotate())
            {
                UpdateEntityFromCamera(pActiveCamera->m_boundEntity, pActiveCamera);
            }
        }
    }

    return AsyncResult_Pending;
}

//------------------------------------------------------------------------------------------------
AsyncResult ToolCameraService::OnShutdown()
{
    MessageService* pMessageService =
        m_pServiceManager->GetSystemServiceAs<MessageService>();

    if (pMessageService != NULL)
    {
        pMessageService->Unsubscribe(this, kCAT_LocalMessage);
    }

    InteractionService* pInteractionService =
        m_pServiceManager->GetSystemServiceAs<InteractionService>();

    if (pInteractionService != NULL)
        pInteractionService->RemoveInteractionDelegate(this);

    m_defaultCameras.clear();
    m_activeCameras.clear();
    m_savedCameras.clear();
    m_cameraModeMap.clear();

    m_spViewportFont = NULL;
    m_spViewportText = NULL;

    m_spRenderService = NULL;

    return AsyncResult_Complete;
}

//------------------------------------------------------------------------------------------------
void ToolCameraService::CreateDefaultCameras()
{
    // Default Cameras
    ToolCameraPtr spPerspective = NiNew ToolCamera();
    spPerspective->SetTranslate(0, -10, 5);
    spPerspective->SetFOV(60);
    spPerspective->SetRotate(NiQuaternion(0.5f, 0.5f, 0.5f, 0.5f));
    m_defaultCameras["Perspective"] = spPerspective;

    NiFrustum kOrthoFrustum(-0.5f, 0.5f, -0.5f, 0.5f, 1, 2, true);

    NiQuaternion kRotation;

    ToolCameraPtr spFront = NiNew ToolCamera();
    spFront->SetTranslate(-100.0f * NiPoint3::UNIT_X);
    kRotation.FromAngleAxesXYZ(NI_HALF_PI, 0.0f, NI_PI);
    spFront->SetRotate(kRotation);
    spFront->SetViewFrustum(kOrthoFrustum);
    spFront->SetLODAdjust(0.0);
    m_defaultCameras["Front"] = spFront;

    ToolCameraPtr spBack = NiNew ToolCamera();
    spBack->SetTranslate(100.0f * NiPoint3::UNIT_X);
    kRotation.FromAngleAxesXYZ(NI_HALF_PI, 0.0f, 0.0f);
    spBack->SetRotate(kRotation);
    spBack->SetViewFrustum(kOrthoFrustum);
    spBack->SetLODAdjust(0.0);
    m_defaultCameras["Back"] = spBack;

    ToolCameraPtr spLeft = NiNew ToolCamera();
    spLeft->SetTranslate(-100.0f * NiPoint3::UNIT_Y);
    kRotation.FromAngleAxesXYZ(NI_HALF_PI, 0.0f, NI_HALF_PI);
    spLeft->SetRotate(kRotation);
    spLeft->SetViewFrustum(kOrthoFrustum);
    spLeft->SetLODAdjust(0.0);
    m_defaultCameras["Left"] = spLeft;

    ToolCameraPtr spRight = NiNew ToolCamera();
    spRight->SetTranslate(100.0f * NiPoint3::UNIT_Y);
    kRotation.FromAngleAxesXYZ(NI_HALF_PI, 0.0f, NI_TWO_PI - NI_HALF_PI);
    spRight->SetRotate(kRotation);
    spRight->SetViewFrustum(kOrthoFrustum);
    spRight->SetLODAdjust(0.0);
    m_defaultCameras["Right"] = spRight;

    ToolCameraPtr spTop = NiNew ToolCamera();
    spTop->SetTranslate(-100.0f * NiPoint3::UNIT_Z);
    kRotation.FromAngleAxesXYZ(NI_HALF_PI, NI_HALF_PI, 0.0f);
    spTop->SetRotate(kRotation);
    spTop->SetViewFrustum(kOrthoFrustum);
    spTop->SetLODAdjust(0.0);
    m_defaultCameras["Top"] = spTop;

    ToolCameraPtr spBottom = NiNew ToolCamera();
    spBottom->SetTranslate(100.0f * NiPoint3::UNIT_Z);
    kRotation.FromAngleAxesXYZ(-NI_HALF_PI, -NI_HALF_PI, NI_PI);
    spBottom->SetRotate(kRotation);
    spBottom->SetViewFrustum(kOrthoFrustum);
    spBottom->SetLODAdjust(0.0);
    m_defaultCameras["Bottom"] = spBottom;

    // Cameras intended to be loaded or created from entities;
    // not created from scratch; can be either ortho or perspective
    ToolCameraPtr spCustomPerspective = NiNew ToolCamera();
    spCustomPerspective->SetTranslate(0, -10, 5);
    spCustomPerspective->SetFOV(60);
    spCustomPerspective->SetRotate(NiQuaternion(0.5f, 0.5f, 0.5f, 0.5f));
    m_defaultCameras["CustomPerspective"] = spCustomPerspective;

    ToolCameraPtr spCustomOrtho = NiNew ToolCamera();
    spCustomOrtho->SetTranslate(100.0f * NiPoint3::UNIT_Z);
    kRotation.FromAngleAxesXYZ(-NI_HALF_PI, -NI_HALF_PI, NI_PI);
    spCustomOrtho->SetRotate(kRotation);
    spCustomOrtho->SetViewFrustum(kOrthoFrustum);
    spCustomOrtho->SetLODAdjust(0.0);
    m_defaultCameras["CustomOrtho"] = spCustomOrtho;

}

//-----------------------------------------------------------------------------------------------
ToolCamera* ToolCameraService::GetActiveCamera()
{
    RenderSurface* pSurface = m_spRenderService->GetActiveRenderSurface();

    if (pSurface == NULL)
        return NULL;

    return GetActiveCamera(pSurface);
}

//------------------------------------------------------------------------------------------------
ToolCamera* ToolCameraService::GetActiveCamera(RenderSurface* pSurface)
{
    ToolCameraPtr spCamera;
    if (m_activeCameras.find(pSurface->GetWindowRef(), spCamera))
        return spCamera;

    return NULL;
}

//-----------------------------------------------------------------------------------------------
void ToolCameraService::SetActiveCamera(const efd::utf8string& cameraName, efd::WindowRef window)
{
    ToolCameraPtr spDefaultCamera;
    egf::EntityID boundEntity = egf::kENTITY_INVALID;

    // It's possible an entity was made invisible for this camera because the user was
    // possessing it; make sure it's visible
    ToolCameraPtr spOldCamera;
    if (m_activeCameras.find(window, spOldCamera))
    {
        UnbindCamera(spOldCamera);
    }

    // If the camera is not found in the list of default cameras, the user has
    // chosen to 'possess' a scene camera.
    if (!m_defaultCameras.find(cameraName, spDefaultCamera))
    {
        spDefaultCamera = m_defaultCameras["CustomPerspective"];
        boundEntity = GetEntityBinding(cameraName);

        // If the possessed camera is orthographic, use an ortho camera as a basis
        if (IsEntityOrtho(boundEntity))
            spDefaultCamera = m_defaultCameras["CustomOrtho"];

        if (spOldCamera != NULL)
        {
            ToolCameraPtr spDummyCamera;
            if (m_defaultCameras.find(spOldCamera->m_displayName, spDummyCamera))
            {
                m_savedCameras[window] = spOldCamera;
            }
        }
    }
    else
    {
        m_savedCameras.erase(window);
    }

    RenderSurface* pSurface = m_spRenderService->GetRenderSurface(window);

    if (pSurface == NULL)
        return;

    // Clone the default camera
    ToolCameraPtr spNewCamera = NiNew ToolCamera();

    spNewCamera->m_pRenderSurface = pSurface;
    spNewCamera->m_boundEntity = boundEntity;
    spNewCamera->m_displayName = cameraName;
    spNewCamera->SetTranslate(spDefaultCamera->GetTranslate());
    spNewCamera->SetRotate(spDefaultCamera->GetRotate());
    spNewCamera->SetScale(spDefaultCamera->GetScale());
    spNewCamera->SetFOV(spDefaultCamera->GetFOV());

    spNewCamera->SetLODAdjust(spDefaultCamera->GetLODAdjust());
    spNewCamera->SetMinNearPlaneDist(spDefaultCamera->GetMinNearPlaneDist());
    spNewCamera->SetMaxFarNearRatio(spDefaultCamera->GetMaxFarNearRatio());

    spNewCamera->SetViewFrustum(spDefaultCamera->GetViewFrustum());

    InitializeCameraViewFrustum(spNewCamera, pSurface);

    pSurface->SetCamera(spNewCamera);
    m_activeCameras[window] = spNewCamera;

    if (m_activeCameraMode != NULL)
        m_activeCameraMode->Reset(spNewCamera);

    if (boundEntity != egf::kENTITY_INVALID)
    {
        BindCamera(boundEntity, spNewCamera);
    }

    m_spRenderService->InvalidateRenderContexts();
}

//------------------------------------------------------------------------------------------------
void ToolCameraService::InitializeCameraViewFrustum(
    ToolCameraPtr spCamera,
    RenderSurface* pSurface)
{
    NiRenderTargetGroup* pRenderTarget = pSurface->GetRenderTargetGroup();
    EE_ASSERT(pRenderTarget);

    if (spCamera->GetViewFrustum().m_bOrtho)
    {
        float fWidth = (float)pRenderTarget->GetWidth(0);
        float fHeight = (float)pRenderTarget->GetHeight(0);

        float worldScale = m_spToolSceneGraphService->GetWorldScale();

        if (worldScale <= 0)
            worldScale = 1.0f;

        NiFrustum frustum(-fWidth * worldScale, fWidth * worldScale, fHeight * worldScale,
            -fHeight * worldScale, 1.0f, 2.0f, true);

        spCamera->SetViewFrustum(frustum);
    }
    else
    {
        efd::Float32 aspectRatio = (efd::Float32)pRenderTarget->GetWidth(0) /
            (efd::Float32)pRenderTarget->GetHeight(0);

        efd::Float32 verticalFieldOfViewRad = spCamera->GetFOV() * EE_DEGREES_TO_RADIANS;
        efd::Float32 viewPlaneHalfHeight = tanf(verticalFieldOfViewRad * 0.5f);
        efd::Float32 viewPlaneHalfWidth = viewPlaneHalfHeight * aspectRatio;

        NiFrustum frustum(-viewPlaneHalfWidth, viewPlaneHalfWidth,
                          viewPlaneHalfHeight, -viewPlaneHalfHeight,
                          0.5f, 10000.0f, false);

        spCamera->SetViewFrustum(frustum);
    }

    m_spRenderService->InvalidateRenderContexts();
}

//-----------------------------------------------------------------------------------------------
void ToolCameraService::SetActiveCamera(const efd::utf8string& cameraName)
{
    RenderSurface* pSurface = m_spRenderService->GetActiveRenderSurface();

    if (pSurface == NULL)
        return;

    SetActiveCamera(cameraName, pSurface->GetWindowRef());
}

//-----------------------------------------------------------------------------------------------
void ToolCameraService::SetActiveCameraMode(const efd::utf8string& modeName)
{
    ICameraMode* pActiveMode = m_cameraModeMap[modeName];
    if (!pActiveMode)
        return;

    m_activeCameraMode = pActiveMode;

    m_spRenderService->InvalidateRenderContexts();
}

//------------------------------------------------------------------------------------------------
void ToolCameraService::LookAtSelection()
{
    ToolCamera* pCamera = GetActiveCamera();

    if (!pCamera)
        return;

    SelectionService* pSelectionService = m_pServiceManager->GetSystemServiceAs<SelectionService>();
    EE_ASSERT(pSelectionService);

    if (!pSelectionService->GetAdapter<EntitySelectionAdapter>()->HasSelection())
        return;

    // DT32635 LookAtSelection does not work correctly for ortho cameras, we would just force
    // the code to do the same thing as MoveToSelection, when in ortho mode.

    EntitySelectionAdapter* pAdapter = pSelectionService->GetAdapter<EntitySelectionAdapter>();
    NiBound kSelectionBound = pAdapter->GetBound();
    const NiPoint3& kSelectionBoundCenter = kSelectionBound.GetCenter();

    pCamera->LookAtWorldPoint(kSelectionBoundCenter, NiPoint3::UNIT_Z);

    m_spRenderService->InvalidateRenderContexts();
}

//------------------------------------------------------------------------------------------------
void ToolCameraService::MoveToSelection()
{
    ToolCamera* pCamera = GetActiveCamera();

    if (!pCamera)
        return;

    SelectionService* pSelectionService = m_pServiceManager->GetSystemServiceAs<SelectionService>();
    EE_ASSERT(pSelectionService);

    if (!pSelectionService->GetAdapter<EntitySelectionAdapter>()->HasSelection())
        return;

    EntitySelectionAdapter* pAdapter = pSelectionService->GetAdapter<EntitySelectionAdapter>();
    NiBound kSelectionBound = pAdapter->GetBound();
    ZoomToExtents(pCamera, kSelectionBound);
}

//------------------------------------------------------------------------------------------------
void ToolCameraService::ZoomToExtents(ToolCamera* pCamera, const NiBound& kBound)
{
    NiMatrix3 kRotation = pCamera->GetRotate();
    NiFrustum kFrustum = pCamera->GetViewFrustum();
    //NiPoint3 kDestPoint = NiViewMath::PanTo(kBound, kRotation, kFrustum);

    // A modified version of NiViewMath::PanTo, this one does not multiply the radius by 2, that was
    // doubling the actual distance needed.
    NiPoint3 kLook;
    kRotation.GetCol(0, kLook);
    float fFrustumEdge = (kFrustum.m_fRight > kFrustum.m_fTop) ?
        kFrustum.m_fTop : kFrustum.m_fRight;
    float fDistanceToCenter = kBound.GetRadius() / fFrustumEdge;

    NiPoint3 kDestPoint = (kBound.GetCenter() - fDistanceToCenter * kLook);

    //NiBound kBoundWithGrid = *(MFramework::Instance->BoundManager->GetToolSceneBound(pmViewport));
    if (kFrustum.m_bOrtho)
    {
        NiPoint3 kLook;
        kRotation.GetCol(0, kLook);
        float fAspect;
        fAspect = kFrustum.m_fRight / kFrustum.m_fTop;

        // add the grid if it is static
        //kDestPoint = kDestPoint - (kBoundWithGrid.GetRadius() +
        //    (kDestPoint - kBoundWithGrid.GetCenter()).Dot(kLook)) * kLook;
        if (kBound.GetRadius() > 0.0f)
        {
            if (fAspect >= 1.0f)
            {
                // if we have a wide aspect, fit the top of the frustum to
                // the bounds
                kFrustum.m_fTop = kBound.GetRadius();
                kFrustum.m_fBottom = -kFrustum.m_fTop;
                kFrustum.m_fRight = kFrustum.m_fTop * fAspect;
                kFrustum.m_fLeft = -kFrustum.m_fRight;
            }
            else
            {
                // if we have a tall aspect, fit the right to the bounds
                kFrustum.m_fRight = kBound.GetRadius();
                kFrustum.m_fLeft = -kFrustum.m_fRight;
                kFrustum.m_fTop = kFrustum.m_fRight / fAspect;
                kFrustum.m_fBottom = -kFrustum.m_fTop;
            }

            pCamera->SetViewFrustum(kFrustum);
        }
    }

    pCamera->SetTranslate(kDestPoint);
    pCamera->SetRotate(kRotation);

    if (kFrustum.m_bOrtho)
        AdjustOrthoDistance(pCamera);

    m_spRenderService->InvalidateRenderContexts();
}

//------------------------------------------------------------------------------------------------
void ToolCameraService::Reset()
{
    EE_ASSERT(m_pServiceManager);

    RenderSurface* pSurface = m_spRenderService->GetActiveRenderSurface();

    if (pSurface != NULL)
    {
        ToolCamera* activeCamera = GetActiveCamera(pSurface);

        if (m_activeCameraMode != NULL)
            m_activeCameraMode->Reset(activeCamera);
    }
}

//-----------------------------------------------------------------------------------------------
efd::SInt32 ToolCameraService::GetInteractionPriority()
{
    return 100;
}

//-----------------------------------------------------------------------------------------------
bool ToolCameraService::OnPreMouseScroll(efd::SInt32 x, efd::SInt32 y, efd::SInt32 dScroll)
{
    EE_UNUSED_ARG(x);
    EE_UNUSED_ARG(y);
    EE_UNUSED_ARG(dScroll);

    return m_isLooking || m_isTranslating || m_isPanning;
}

//------------------------------------------------------------------------------------------------
bool ToolCameraService::OnPreMouseMove(efd::SInt32 x, efd::SInt32 y, efd::SInt32 dx, efd::SInt32 dy)
{
    EE_UNUSED_ARG(x);
    EE_UNUSED_ARG(y);
    EE_UNUSED_ARG(dx);
    EE_UNUSED_ARG(dy);

    return m_isLooking || m_isTranslating || m_isPanning;
}

//------------------------------------------------------------------------------------------------
bool ToolCameraService::OnPreMouseDown(ecrInput::MouseMessage::MouseButton eButton, efd::SInt32 x,
                                       efd::SInt32 y)
{
    EE_UNUSED_ARG(eButton);
    EE_UNUSED_ARG(x);
    EE_UNUSED_ARG(y);

    return m_isLooking || m_isTranslating || m_isPanning;
}

//------------------------------------------------------------------------------------------------
bool ToolCameraService::OnPreMouseUp(ecrInput::MouseMessage::MouseButton eButton, efd::SInt32 x,
                                     efd::SInt32 y)
{
    EE_UNUSED_ARG(eButton);
    EE_UNUSED_ARG(x);
    EE_UNUSED_ARG(y);

    return m_isLooking || m_isTranslating || m_isPanning;
}

//------------------------------------------------------------------------------------------------
bool ToolCameraService::OnMouseScroll(efd::Bool handled, efd::SInt32 x, efd::SInt32 y,
                                      efd::SInt32 dScroll)
{
    EE_UNUSED_ARG(handled);

    if (m_activeCameraMode != NULL)
    {
        ToolCamera* pActiveCamera = GetActiveCamera();
        if (pActiveCamera != NULL)
        {
            efd::Point3 lastPositon = pActiveCamera->GetWorldTranslate();
            NiMatrix3 lastRotation = pActiveCamera->GetRotate();

            bool res = m_activeCameraMode->OnMouseScroll(x, y, dScroll, pActiveCamera);

            if (lastPositon != pActiveCamera->GetWorldTranslate() ||
                lastRotation != pActiveCamera->GetRotate())
            {
                UpdateEntityFromCamera(pActiveCamera->m_boundEntity, pActiveCamera);
            }

            return res;
        }
    }

    return false;
}

//------------------------------------------------------------------------------------------------
bool ToolCameraService::OnMouseMove(efd::Bool handled, efd::SInt32 x, efd::SInt32 y,
                                    efd::SInt32 dx, efd::SInt32 dy)
{
    EE_UNUSED_ARG(handled);

    if (m_activeCameraMode != NULL)
    {
        ToolCamera* pActiveCamera = GetActiveCamera();
        if (pActiveCamera != NULL)
        {
            efd::Point3 lastPositon = pActiveCamera->GetWorldTranslate();
            NiMatrix3 lastRotation = pActiveCamera->GetRotate();

            bool res = m_activeCameraMode->OnMouseMove(x, y, dx, dy, pActiveCamera);

            if (lastPositon != pActiveCamera->GetWorldTranslate() ||
                lastRotation != pActiveCamera->GetRotate())
            {
                UpdateEntityFromCamera(pActiveCamera->m_boundEntity, pActiveCamera);
            }

            return res;
        }
    }

    return false;
}

//------------------------------------------------------------------------------------------------
bool ToolCameraService::OnMouseDown(efd::Bool handled, ecrInput::MouseMessage::MouseButton eButton,
                                    efd::SInt32 x, efd::SInt32 y)
{
    EE_UNUSED_ARG(handled);

    bool altDown = false;
#ifdef EE_PLATFORM_SDL2
    altDown = (SDL_GetModState() & ::KMOD_ALT) != 0;
#elif defined(EE_PLATFORM_WIN32)
    altDown = (GetKeyState(VK_MENU) & ~1) != 0;
#endif

    // DT32228 Remove hardcoded reference to Alt modifier when we make World Builder camera
    // controls more configurable. Remember to adjust C# code as well to inform ToolCameraService
    // of the changes.
    if (eButton == MouseUpMessage::MBUTTON_LEFT && !altDown)
    {
        m_isLooking = true;
    }
    else if (eButton == MouseUpMessage::MBUTTON_RIGHT && !altDown)
    {
        m_isTranslating = true;
    }
    else if (eButton == MouseDownMessage::MBUTTON_MIDDLE)
    {
        m_isPanning = true;
    }

    if (m_activeCameraMode != NULL)
    {
        ToolCamera* pActiveCamera = GetActiveCamera();
        if (pActiveCamera != NULL)
        {
            efd::Point3 lastPositon = pActiveCamera->GetWorldTranslate();
            NiMatrix3 lastRotation = pActiveCamera->GetRotate();

            bool res = m_activeCameraMode->OnMouseDown(eButton, x, y, pActiveCamera);

            if (lastPositon != pActiveCamera->GetWorldTranslate() ||
                lastRotation != pActiveCamera->GetRotate())
            {
                UpdateEntityFromCamera(pActiveCamera->m_boundEntity, pActiveCamera);
            }

            return res;
        }
    }

    return false;
}

//------------------------------------------------------------------------------------------------
bool ToolCameraService::OnMouseUp(efd::Bool handled, MouseMessage::MouseButton eButton,
                                  efd::SInt32 x, efd::SInt32 y)
{
    EE_UNUSED_ARG(handled);

    if (eButton == MouseUpMessage::MBUTTON_LEFT)
    {
        m_isLooking = false;
    }
    else if (eButton == MouseUpMessage::MBUTTON_RIGHT)
    {
        m_isTranslating = false;
    }
    else if (eButton == MouseUpMessage::MBUTTON_MIDDLE)
    {
        m_isPanning = false;
    }

    if (m_activeCameraMode != NULL)
    {
        ToolCamera* pActiveCamera = GetActiveCamera();
        if (pActiveCamera != NULL)
        {
            efd::Point3 lastPositon = pActiveCamera->GetWorldTranslate();
            NiMatrix3 lastRotation = pActiveCamera->GetRotate();

            bool res = m_activeCameraMode->OnMouseUp(eButton, x, y, pActiveCamera);

            if (lastPositon != pActiveCamera->GetWorldTranslate() ||
                lastRotation != pActiveCamera->GetRotate())
            {
                UpdateEntityFromCamera(pActiveCamera->m_boundEntity, pActiveCamera);
            }

            return res;
        }
    }

    return false;
}

//------------------------------------------------------------------------------------------------
void ToolCameraService::HandleSurfaceRemoved(RenderSurface* pSurface)
{
    ///@todo NDarnell
    // replace callback function with callback class that can have context

    ToolCameraService* pCameraService =
        g_bapiContext.GetSystemServiceAs<ToolCameraService>();

    if (pCameraService)
        pCameraService->m_activeCameras.erase(pSurface->GetWindowRef());
}

//------------------------------------------------------------------------------------------------
void ToolCameraService::OnInputAction(const InputActionMessage* pMsg, efd::Category targetChannel)
{
    EE_ASSERT(m_pServiceManager);

    const utf8string& kName = pMsg->GetName();

    if (kName == "Forward")
        m_forward = pMsg->GetActive();
    else if (kName == "Backward")
        m_backward = pMsg->GetActive();
    else if (kName == "Left")
        m_strafeLeft = pMsg->GetActive();
    else if (kName == "Right")
        m_strafeRight = pMsg->GetActive();
    else if (kName == "Turbo")
        m_shift = pMsg->GetActive();
    else if (kName == "Unbind Entity")
    {
        UnbindCamera(GetActiveCamera());
    }
}

//------------------------------------------------------------------------------------------------
void ToolCameraService::UpdateEntityFromCamera(const egf::EntityID& entityID, ToolCamera* pCamera)
{
    if (entityID == egf::kENTITY_INVALID)
        return;

    egf::Entity* pEntity = m_pEntityManager->LookupEntity(entityID);
    if (!pEntity)
        return;

    efd::Point3 position = pCamera->GetWorldLocation();

    efd::Point3 rotation;
    pCamera->GetRotate().ToEulerAnglesXYZ(rotation.x, rotation.y, rotation.z);

    rotation.x = rotation.x * -EE_RADIANS_TO_DEGREES;
    rotation.y = rotation.y * -EE_RADIANS_TO_DEGREES;
    rotation.z = rotation.z * -EE_RADIANS_TO_DEGREES;

    pEntity->SetPropertyValue(egf::kPropertyID_StandardModelLibrary_Position, position);
    pEntity->SetPropertyValue(egf::kPropertyID_StandardModelLibrary_Rotation, rotation);

    pCamera->m_boundEntityDirty = true;
}

//------------------------------------------------------------------------------------------------
void ToolCameraService::UpdateCameraFromEntity(const egf::EntityID& entityID, ToolCamera* pCamera)
{
    if (pCamera->m_boundEntityDirty)
    {
        pCamera->m_boundEntityDirty = false;
        return;
    }

    egf::Entity* pEntity = m_pEntityManager->LookupEntity(entityID);
    if (!pEntity)
        return;

    if (pEntity->GetModel()->ContainsModel(egf::kFlatModelID_StandardModelLibrary_Camera))
    {
        float FOV;
        float nearPlane;
        float farPlane;
        float minimumNearPlane;
        float maximumFarToNearRatio;
        bool isOrtho;
        float LODAdjust;

        pEntity->GetPropertyValue(egf::kPropertyID_StandardModelLibrary_FOV, FOV);
        pEntity->GetPropertyValue(egf::kPropertyID_StandardModelLibrary_NearPlane, nearPlane);
        pEntity->GetPropertyValue(egf::kPropertyID_StandardModelLibrary_FarPlane, farPlane);
        pEntity->GetPropertyValue(egf::kPropertyID_StandardModelLibrary_MinimumNearPlane, minimumNearPlane);
        pEntity->GetPropertyValue(egf::kPropertyID_StandardModelLibrary_MaximumFarToNearRatio, maximumFarToNearRatio);
        pEntity->GetPropertyValue(egf::kPropertyID_StandardModelLibrary_IsOrthographic, isOrtho);
        pEntity->GetPropertyValue(egf::kPropertyID_StandardModelLibrary_LODAdjust, LODAdjust);

        pCamera->SetFOV(FOV);
        pCamera->SetMinNearPlaneDist(minimumNearPlane);
        pCamera->SetMaxFarNearRatio(maximumFarToNearRatio);
        pCamera->SetLODAdjust(LODAdjust);

        NiRenderTargetGroup* pRenderTarget = pCamera->m_pRenderSurface->GetRenderTargetGroup();
        EE_ASSERT(pRenderTarget);

        efd::Float32 aspectRatio = (efd::Float32)pRenderTarget->GetWidth(0) /
            (efd::Float32)pRenderTarget->GetHeight(0);

        efd::Float32 verticalFieldOfViewRad = EE_DEGREES_TO_RADIANS * FOV;
        efd::Float32 viewPlaneHalfHeight = tanf(verticalFieldOfViewRad * 0.5f);
        efd::Float32 viewPlaneHalfWidth = viewPlaneHalfHeight * aspectRatio;
        NiFrustum frustum(-viewPlaneHalfWidth, viewPlaneHalfWidth,
                          viewPlaneHalfHeight, -viewPlaneHalfHeight,
                          nearPlane, farPlane, isOrtho);
        pCamera->SetViewFrustum(frustum);
    }

    efd::Point3 position;
    efd::Point3 rotation;
    pEntity->GetPropertyValue(egf::kPropertyID_StandardModelLibrary_Position, position);
    pEntity->GetPropertyValue(egf::kPropertyID_StandardModelLibrary_Rotation, rotation);

    pCamera->SetTranslate(position);

    NiMatrix3 kXRot, kYRot, kZRot;
    kXRot.MakeXRotation(rotation.x * -EE_DEGREES_TO_RADIANS);
    kYRot.MakeYRotation(rotation.y * -EE_DEGREES_TO_RADIANS);
    kZRot.MakeZRotation(rotation.z * -EE_DEGREES_TO_RADIANS);
    pCamera->SetRotate(kXRot * kYRot * kZRot);

    pCamera->Update(0.0f);

    m_spRenderService->InvalidateRenderContexts();
}

//------------------------------------------------------------------------------------------------
void ToolCameraService::ChangeEntityVisibility(const egf::EntityID& entityID, bool isVisible)
{
    if (entityID == egf::kENTITY_INVALID)
        return;

    egf::Entity* pEntity = m_pEntityManager->LookupEntity(entityID);
    if (!pEntity)
        return;

    efd::ID128 persistentID = pEntity->GetDataFileID();

    efd::StreamMessagePtr spStreamMessage = EE_NEW ToolVisibilityRequest;

    if (isVisible)
        *spStreamMessage << (efd::UInt32)1;
    else
        *spStreamMessage << (efd::UInt32)0;

    *spStreamMessage << (efd::UInt32)2; //VISIBILITY_CAMERA_BOUND
    *spStreamMessage << (efd::UInt32)1;
    *spStreamMessage << persistentID;

    m_pMessageService->SendImmediate(spStreamMessage,
        efd::kCAT_LocalMessage);
}

//------------------------------------------------------------------------------------------------
egf::EntityID ToolCameraService::GetEntityBinding(const efd::utf8string& cameraName)
{
    egf::EntityID entityID;
    efd::ID128 id;
    efd::ParseHelper<efd::ID128>::FromString(cameraName, id);
    egf::Entity* pEntity = m_pEntityManager->LookupEntityByDataFileID(id);
    if (pEntity)
    {
        entityID = pEntity->GetEntityID();
    }

    return entityID;
}

//------------------------------------------------------------------------------------------------
bool ToolCameraService::IsEntityOrtho(const egf::EntityID& entityID)
{
    if (entityID == egf::kENTITY_INVALID)
        return false;

    egf::Entity* pEntity = m_pEntityManager->LookupEntity(entityID);
    if (!pEntity)
        return false;

    bool isOrtho = false;
    egf::PropertyResult res =
        pEntity->GetPropertyValue(egf::kPropertyID_StandardModelLibrary_IsOrthographic, isOrtho);

    if (res != egf::PropertyResult_OK)
        return false;

    return isOrtho;
}

//------------------------------------------------------------------------------------------------
void ToolCameraService::BindCamera(const egf::EntityID& entityID, ToolCamera* pCamera)
{
    if (entityID == egf::kENTITY_INVALID)
        return;

    Entity* pEntity = m_pEntityManager->LookupEntity(entityID);
    if (!pEntity)
        return;

    UpdateCameraFromEntity(entityID, pCamera);
    ChangeEntityVisibility(entityID, false);
}

//------------------------------------------------------------------------------------------------
void ToolCameraService::UnbindCamera(ToolCamera* pCamera)
{
    if (pCamera->m_boundEntity == egf::kENTITY_INVALID)
        return;

    Entity* pEntity = m_pEntityManager->LookupEntity(pCamera->m_boundEntity);
    if (!pEntity)
        return;

    ChangeEntityVisibility(pCamera->m_boundEntity, true);

    pCamera->m_boundEntityDirty = false;
    pCamera->m_boundEntity = egf::kENTITY_INVALID;

    efd::Point3 position, rotation;
    pEntity->GetPropertyValue(egf::kPropertyID_StandardModelLibrary_Position, position);
    pEntity->GetPropertyValue(egf::kPropertyID_StandardModelLibrary_Rotation, rotation);

    SetEntitiesPropertiesMessagePtr spSetProps = EE_NEW SetEntitiesPropertiesMessage();
    utf8string positionStr, rotationStr;
    efd::ParseHelper<efd::Point3>::ToString(position, positionStr);
    efd::ParseHelper<efd::Point3>::ToString(rotation, rotationStr);
    spSetProps->AddEntry(pEntity->GetDataFileID(), "Position", positionStr);
    spSetProps->AddEntry(pEntity->GetDataFileID(), "Rotation", rotationStr);
    m_pMessageService->SendImmediate(spSetProps, 
        ToolMessagesConstants::ms_fromFrameworkCategory);

    efd::WindowRef kWindow = pCamera->m_pRenderSurface->GetWindowRef();
    ToolCameraPtr spSavedCamera;
    if (m_savedCameras.find(kWindow, spSavedCamera))
    {
        RenderSurface* pSurface = m_spRenderService->GetRenderSurface(kWindow);

        if (pSurface != NULL)
        {
            InitializeCameraViewFrustum(spSavedCamera, pSurface);

            pSurface->SetCamera(spSavedCamera);
            m_activeCameras[kWindow] = spSavedCamera;

            m_spRenderService->InvalidateRenderContexts();
        }
    }
}

//------------------------------------------------------------------------------------------------
bool ToolCameraService::IsDefaultCamera(const efd::utf8string& cameraName)
{
    ToolCameraPtr spCamera;
    return (m_defaultCameras.find(cameraName, spCamera));
}

//------------------------------------------------------------------------------------------------
bool ToolCameraService::IsEntityBoundToAnyCamera(const egf::EntityID& entityID)
{
    efd::map<efd::WindowRef, ToolCameraPtr>::const_iterator itr = m_activeCameras.begin();
    while (itr != m_activeCameras.end())
    {
        if (itr->second != NULL &&
            itr->second->m_boundEntity != egf::kENTITY_INVALID &&
            itr->second->m_boundEntity == entityID)
        {
            return true;
        }
        ++itr;
    }
    return false;
}

//------------------------------------------------------------------------------------------------
void ToolCameraService::HandleEntityRemovedMessage(
    const egf::EntityChangeMessage* pMessage,
    efd::Category targetChannel)
{
    const egf::EntityID& entityID = pMessage->GetEntity()->GetEntityID();

    efd::map<efd::WindowRef, ToolCameraPtr>::iterator cameraItr;
    for (cameraItr = m_activeCameras.begin(); cameraItr != m_activeCameras.end(); cameraItr++)
    {
        if (entityID == cameraItr->second->m_boundEntity)
        {
            // Unbind entity without updating its property values since it's being deleted
            cameraItr->second->m_boundEntity = egf::kENTITY_INVALID;
        }
    }
}

//------------------------------------------------------------------------------------------------
void ToolCameraService::HandleEntityUpdatedMessage(const egf::EntityChangeMessage* pMessage,
                                                   efd::Category targetChannel)
{
    Entity* pEntity = pMessage->GetEntity();

    efd::map<efd::WindowRef, ToolCameraPtr>::iterator cameraItr;
    for (cameraItr = m_activeCameras.begin(); cameraItr != m_activeCameras.end(); cameraItr++)
    {
        ToolCamera* pCamera = cameraItr->second;

        if (pCamera->m_boundEntity != pEntity->GetEntityID())
            continue;

        UpdateCameraFromEntity(pEntity->GetEntityID(), pCamera);
    }
}

//------------------------------------------------------------------------------------------------
bool ToolCameraService::IsCameraEntityBound(ToolCamera* pCamera)
{
    return pCamera->m_boundEntity != egf::kENTITY_INVALID;
}

//------------------------------------------------------------------------------------------------
void ToolCameraService::SyncBoundEntities()
{
    efd::map<efd::WindowRef, ToolCameraPtr>::iterator cameraItr;
    for (cameraItr = m_activeCameras.begin(); cameraItr != m_activeCameras.end(); cameraItr++)
    {
        ToolCamera* pCamera = cameraItr->second;

        // No need to sync if there's no entity bound to the camera or if it can't be found
        if (pCamera->m_boundEntity == egf::kENTITY_INVALID)
            continue;

        egf::Entity* pEntity = m_pEntityManager->LookupEntity(pCamera->m_boundEntity);
        if (!pEntity)
            continue;

        efd::Point3 position, rotation;
        pEntity->GetPropertyValue(egf::kPropertyID_StandardModelLibrary_Position, position);
        pEntity->GetPropertyValue(egf::kPropertyID_StandardModelLibrary_Rotation, rotation);

        SetEntitiesPropertiesMessagePtr spSetProps = EE_NEW SetEntitiesPropertiesMessage();
        utf8string positionStr, rotationStr;
        efd::ParseHelper<efd::Point3>::ToString(position, positionStr);
        efd::ParseHelper<efd::Point3>::ToString(rotation, rotationStr);

        spSetProps->AddEntry(pEntity->GetDataFileID(), "Position", positionStr, NULL);
        spSetProps->AddEntry(pEntity->GetDataFileID(), "Rotation", rotationStr, NULL);

        m_pMessageService->SendImmediate(spSetProps,
            ToolMessagesConstants::ms_fromFrameworkCategory);
    }
}

//------------------------------------------------------------------------------------------------
void ToolCameraService::AdjustOrthoDistance(ToolCamera* pCamera)
{
    const NiPoint3& origin = pCamera->GetTranslate();
    const NiMatrix3& rotation = pCamera->GetRotate();

    // Fit the near and far planes to match the new angle of the camera with the scene.
    const NiBound& sceneBounds = m_spToolSceneGraphService->GetSceneBounds();

    NiPoint3 dir;
    rotation.GetCol(0, dir);

    // project a pt onto the plane tangent to the sphere and our dir
    NiPoint3 tangentPoint = sceneBounds.GetCenter() - sceneBounds.GetRadius() * dir;
    Float32 distance = (tangentPoint - origin).Dot(dir);
    NiPoint3 destination = origin + distance * dir;

    // Only adjust the distance if this is not a user camera
    if (pCamera->m_boundEntity == egf::kENTITY_INVALID)
        pCamera->SetTranslate(destination);

    pCamera->FitNearAndFarToBound(sceneBounds);
    pCamera->Update(0);
}

//------------------------------------------------------------------------------------------------
