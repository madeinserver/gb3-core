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

#include <efd/ServiceManager.h>
#include <efd/StreamMessage.h>

#include <NiMeshCullingProcess.h>

#include <egf/StandardModelLibraryPropertyIDs.h>

#include <ecrInput/KeyboardMessages.h>
#include <ecrInput/MouseMessages.h>

#include <ecr/RenderService.h>
#include <ecr/CameraService.h>

#include "RotationGizmo.h"
#include "ScaleGizmo.h"
#include "SelectionGizmo.h"
#include "SelectionService.h"
#include "TranslationGizmo.h"
#include "CreationGizmo.h"
#include "ToolServicesMessages.h"
#include "ToolCameraService.h"

#include "InteractionService.h"

#include "GizmoService.h"

#include "EntityGizmoPolicy.h"
#include "CreationGizmoPolicy.h"

using namespace efd;
using namespace egf;
using namespace ecr;
using namespace ecrInput;
using namespace egmToolServices;

EE_IMPLEMENT_CONCRETE_CLASS_INFO(GizmoService);

const float GizmoService::ms_parallelThreshold = 0.99f;
const float GizmoService::ms_invParallelThreshold = 0.05f;

//------------------------------------------------------------------------------------------------
GizmoService::GizmoService()
    : m_pMessageService(NULL)
    , m_pRenderService(NULL)
    , m_visibleArray(6, 1)
    , m_spGizmoRenderView(NULL)
    , m_spCuller(NULL)
    , m_pGizmoRenderClick(NULL)
    , m_currentSpace(RSPACE_WORLD)
    , m_translationPrecisionEnabled(false)
    , m_scalePrecisionEnabled(false)
    , m_translationSnapEnabled(false)
    , m_rotationSnapEnabled(false)
    , m_scaleSnapEnabled(false)
    , m_scaleToView(true)
    , m_translationPrecision(0.1f)
    , m_scalePrecision(0.1)
    , m_translationSnap(1.0f)
    , m_rotationSnap(5.0f)
    , m_scaleSnap(0.1f)
    , m_lastUpdateTime(-1.0f)
    , m_placementMode(PMODE_FREE_PLACEMENT)
    , m_isSpecialPrimed(false)
    , m_activeSubject("Entity")
    , m_activeGizmoName("Selection")
    , m_pClosestGizmo(NULL)
{
    // If this default priority is changed, also update the service quick reference documentation
    m_defaultPriority = 2090;

    // Setup gizmo render view
    m_spGizmoRenderView = NiNew Ni3DRenderView();
}

//------------------------------------------------------------------------------------------------
GizmoService::~GizmoService()
{
}

//------------------------------------------------------------------------------------------------
const char* GizmoService::GetDisplayName() const
{
    return "GizmoService";
}

//------------------------------------------------------------------------------------------------
SyncResult GizmoService::OnPreInit(efd::IDependencyRegistrar* pDependencyRegistrar)
{
    pDependencyRegistrar->AddDependency<efd::MessageService>();
    pDependencyRegistrar->AddDependency<RenderService>();
    pDependencyRegistrar->AddDependency<PickService>();
    pDependencyRegistrar->AddDependency<SelectionService>();
    pDependencyRegistrar->AddDependency<InteractionService>();
    pDependencyRegistrar->AddDependency<ToolCameraService>();

    m_pRenderService = m_pServiceManager->GetSystemServiceAs<RenderService>();
    EE_ASSERT(m_pRenderService);

    m_pMessageService = m_pServiceManager->GetSystemServiceAs<efd::MessageService>();
    EE_ASSERT(m_pMessageService);

    m_pMessageService->RegisterFactoryMethod(
        KeyDownMessage::CLASS_ID,
        KeyDownMessage::FactoryMethod);

    m_pMessageService->RegisterFactoryMethod(SetEntitiesPropertiesMessage::CLASS_ID,
        SetEntitiesPropertiesMessage::FactoryMethod);

    m_pCameraService = m_pServiceManager->GetSystemServiceAs<ToolCameraService>();
    EE_ASSERT(m_pCameraService);

    return SyncResult_Success;
}

//------------------------------------------------------------------------------------------------
AsyncResult GizmoService::OnInit()
{
    m_pMessageService->Subscribe(this, kCAT_LocalMessage);

    InteractionService* pInteractionService =
        m_pServiceManager->GetSystemServiceAs<InteractionService>();
    EE_ASSERT(pInteractionService);

    m_pRenderService->AddDelegate(this);
    pInteractionService->AddInteractionDelegate(this);

    AddGizmoPolicy(EE_NEW EntityGizmoPolicy(m_pServiceManager));
    AddGizmoPolicy(EE_NEW CreationGizmoPolicy(m_pServiceManager));

    return AsyncResult_Complete;
}

//------------------------------------------------------------------------------------------------
AsyncResult GizmoService::OnTick()
{
    // Reset this each frame, so it can be used by multiple tests
    m_pClosestGizmo = NULL;

    efd::TimeType currentTime = efd::GetCurrentTimeInSec();

    efd::TimeType timeElapsed = currentTime - m_lastUpdateTime;
    m_lastUpdateTime = currentTime;

    RenderSurface* pSurface = m_pRenderService->GetActiveRenderSurface();

    if (pSurface == NULL)
        return AsyncResult_Pending;

    for (GizmoPolicyVector::iterator i = m_activePolicies.begin(); i != m_activePolicies.end(); ++i)
    {
        GizmoPolicy* pPolicy = *i;
        pPolicy->OnTick(this, timeElapsed, pSurface);
    }

    for (Gizmos::iterator it = m_gizmos.begin(); it != m_gizmos.end(); ++it)
    {
        IGizmo* pGizmo = *it;
        pGizmo->OnTick(timeElapsed, pSurface);
    }

    return AsyncResult_Pending;
}

//------------------------------------------------------------------------------------------------
AsyncResult GizmoService::OnShutdown()
{
    DeactivateActivePolicies();

    while (m_gizmos.begin() != m_gizmos.end())
        RemoveGizmo(*m_gizmos.begin());

    while (!m_policies.empty())
        RemoveGizmoPolicy(m_policies.begin()->second);

    m_pRenderService->RemoveDelegate(this);

    if (m_pMessageService != NULL)
    {
        m_pMessageService->Unsubscribe(this, kCAT_LocalMessage);
    }

    InteractionService* pInteractionService =
        m_pServiceManager->GetSystemServiceAs<InteractionService>();
    if (pInteractionService != NULL)
        pInteractionService->RemoveInteractionDelegate(this);

    m_spGizmoRenderView = NULL;

    m_pMessageService = NULL;
    m_pRenderService = NULL;
    m_pSelectionService = NULL;
    m_pCameraService = NULL;

    return AsyncResult_Complete;
}

//------------------------------------------------------------------------------------------------
void GizmoService::OnSurfaceAdded(ecr::RenderService* pService, ecr::RenderSurface* pSurface)
{
    EE_UNUSED_ARG(pService);

    NiSPWorkflowManager* pWorkflowManager = NULL;
    SceneGraphService* pSceneGraphService =
        m_pRenderService->GetServiceManager()->GetSystemServiceAs<ecr::SceneGraphService>();

    if (pSceneGraphService)
        pWorkflowManager = pSceneGraphService->GetWorkflowManager();

    m_spCuller = EE_NEW NiMeshCullingProcess(&m_visibleArray, pWorkflowManager);
    m_spGizmoRenderView->SetCullingProcess(m_spCuller);

    // Create gizmo render click, which will render the gizmos to the render view.
    m_pGizmoRenderClick = NiNew NiViewRenderClick();
    m_pGizmoRenderClick->SetName("GizmoClick");
    m_pGizmoRenderClick->AppendRenderView(m_spGizmoRenderView);
    m_pGizmoRenderClick->SetRenderTargetGroup(pSurface->GetRenderTargetGroup());
    m_pGizmoRenderClick->SetClearDepthBuffer(true);

    // Create main scene render step
    NiDefaultClickRenderStep* pDefaultRenderStep =
        (NiDefaultClickRenderStep*)pSurface->GetRenderStep();
    EE_ASSERT(pDefaultRenderStep);

    pDefaultRenderStep->AppendRenderClick(m_pGizmoRenderClick);
}

//------------------------------------------------------------------------------------------------
void GizmoService::OnSurfaceRemoved(
        ecr::RenderService* pService,
        ecr::RenderSurface* pSurface)
{
    EE_UNUSED_ARG(pService);
    EE_UNUSED_ARG(pSurface);
}

//------------------------------------------------------------------------------------------------
void GizmoService::OnSurfacePreDraw(RenderService* pService, RenderSurface* pSurface)
{
    EE_UNUSED_ARG(pService);
    EE_UNUSED_ARG(pSurface);

    for (Gizmos::iterator it = m_gizmos.begin(); it != m_gizmos.end(); ++it)
    {
        IGizmo* pGizmo = *it;
        pGizmo->TransformToView(pSurface);
    }
}

//-----------------------------------------------------------------------------------------------
Ni3DRenderView* GizmoService::GetGizmoView()
{
    return m_spGizmoRenderView;
}

//------------------------------------------------------------------------------------------------
template< typename T >
void GizmoService::AddGizmoPolicy(GizmoPolicy* pGizmoPolicy)
{
    m_policies[T::CLASS_ID] = pGizmoPolicy;

    pGizmoPolicy->OnAdded(this);
    ActivateGizmoPolicy(pGizmoPolicy);
}

//------------------------------------------------------------------------------------------------
void GizmoService::AddGizmoPolicy(GizmoPolicy* pGizmoPolicy)
{
    m_policies[pGizmoPolicy->GetClassID()] = pGizmoPolicy;

    pGizmoPolicy->OnAdded(this);
    ActivateGizmoPolicy(pGizmoPolicy);
}

//------------------------------------------------------------------------------------------------
template< typename T >
void GizmoService::RemoveGizmoPolicy(GizmoPolicy* pGizmoPolicy)
{
    pGizmoPolicy->OnRemoved(this);

    m_policies.erase(T::CLASS_ID);

    if (m_activePolicies.size() > 0)
    {
        GizmoPolicyVector::iterator itr = m_activePolicies.find(pGizmoPolicy);
        if (itr != m_activePolicies.end())
        {
            m_activePolicies.erase(itr);
        }
    }
}

//------------------------------------------------------------------------------------------------
void GizmoService::RemoveGizmoPolicy(GizmoPolicy* pGizmoPolicy)
{
    pGizmoPolicy->OnRemoved(this);

    m_policies.erase(pGizmoPolicy->GetClassID());
    
    if (m_activePolicies.size() > 0)
    {
        GizmoPolicyVector::iterator itr = m_activePolicies.find(pGizmoPolicy);
        if (itr != m_activePolicies.end())
        {
            m_activePolicies.erase(itr);
        }
    }
}

//------------------------------------------------------------------------------------------------
const efd::utf8string& GizmoService::GetActivePolicySubject()
{
    return m_activeSubject;
}

//------------------------------------------------------------------------------------------------
void GizmoService::SetActivePolicySubject(const efd::utf8string& subjectName)
{
    InternalSetActivePolicySubject(subjectName, true);
}

//------------------------------------------------------------------------------------------------
void GizmoService::PushActivePolicySubject(const efd::utf8string& subjectName)
{
    m_activeSubjectStack.push(m_activeSubject);
    InternalSetActivePolicySubject(subjectName, false);
}

//------------------------------------------------------------------------------------------------
void GizmoService::PopActivePolicySubject()
{
    EE_ASSERT(!m_activeSubjectStack.empty());

    if (!m_activeSubjectStack.empty())
    {
        InternalSetActivePolicySubject(m_activeSubjectStack.top(), false);
        m_activeSubjectStack.pop();
    }
}

//------------------------------------------------------------------------------------------------
void GizmoService::InternalSetActivePolicySubject(
    const efd::utf8string& subjectName,
    efd::Bool clearStack)
{
    DeactivateActivePolicies();

    m_activeSubject = subjectName;

    for (GizmoPolicyMap::iterator i = m_policies.begin(); i != m_policies.end(); ++i)
    {
        GizmoPolicyPtr spPolicy = i->second;
        ActivateGizmoPolicy(spPolicy);
    }

    if (clearStack)
    {
        while (!m_activeSubjectStack.empty())
            m_activeSubjectStack.pop();
    }
}

//------------------------------------------------------------------------------------------------
const efd::utf8string& GizmoService::GetActiveGizmo()
{
    return m_activeGizmoName;
}

//------------------------------------------------------------------------------------------------
void GizmoService::SetActiveGizmo(const efd::utf8string& gizmoName)
{
    InternalSetActiveGizmo(gizmoName, true);
}

//------------------------------------------------------------------------------------------------
void GizmoService::PushActiveGizmo(const efd::utf8string& gizmoName)
{
    m_activeGizmoStack.push(m_activeGizmoName);
    InternalSetActiveGizmo(gizmoName, false);
}

//------------------------------------------------------------------------------------------------
void GizmoService::PopActiveGizmo()
{
    EE_ASSERT(!m_activeGizmoStack.empty());

    if (!m_activeGizmoStack.empty())
    {
        InternalSetActiveGizmo(m_activeGizmoStack.top(), false);
        m_activeGizmoStack.pop();
    }
}

//------------------------------------------------------------------------------------------------
void GizmoService::InternalSetActiveGizmo(const efd::utf8string& gizmoName, efd::Bool clearStack)
{
    m_activeGizmoName = gizmoName;

    for (GizmoPolicyVector::iterator i = m_activePolicies.begin();
         i != m_activePolicies.end();
         ++i)
    {
        (*i)->SetActiveGizmo(this, gizmoName);
    }

    if (clearStack)
    {
        while (!m_activeGizmoStack.empty())
            m_activeGizmoStack.pop();
    }
}

//------------------------------------------------------------------------------------------------
void GizmoService::AddGizmo(IGizmo* pGizmo)
{
    // Make sure we don't add the same gizmo twice.
    if (m_gizmos.find(pGizmo) == m_gizmos.end())
    {
        m_gizmos.push_back(pGizmo);
        pGizmo->Connect(m_spGizmoRenderView);
    }
}

//------------------------------------------------------------------------------------------------
void GizmoService::RemoveGizmo(IGizmo* pGizmo)
{
    Gizmos::iterator it = m_gizmos.find(pGizmo);
    if (it != m_gizmos.end())
    {
        IGizmo* pGizmo = *it;
        pGizmo->Disconnect(m_spGizmoRenderView);

        m_gizmos.erase(it);
    }
}

//------------------------------------------------------------------------------------------------
efd::Bool GizmoService::ActivateGizmoPolicy(GizmoPolicy* pGizmoPolicy)
{
    if (pGizmoPolicy->IsSubjectCovered(this, m_activeSubject))
    {
        m_activePolicies.push_back(pGizmoPolicy);
        pGizmoPolicy->Activate(this);

        return true;
    }

    return false;
}

//------------------------------------------------------------------------------------------------
void GizmoService::DeactivateActivePolicies()
{
    for (GizmoPolicyVector::iterator i = m_activePolicies.begin(); i != m_activePolicies.end(); ++i)
        (*i)->Deactivate(this);

    m_activePolicies.clear();
}

//------------------------------------------------------------------------------------------------
efd::SInt32 GizmoService::GetInteractionPriority()
{
    return 200;
}

//------------------------------------------------------------------------------------------------
bool GizmoService::OnMouseScroll(
        efd::Bool handled,
        efd::SInt32 x,
        efd::SInt32 y,
        efd::SInt32 dScroll)
{
    EE_UNUSED_ARG(handled);

    RenderSurface* pSurface = m_pRenderService->GetActiveRenderSurface();

    if (pSurface == NULL)
        return false;

    efd::Bool gizmoHandled = false;

    UpdateClosestGizmo(x, y, pSurface);

    for (Gizmos::iterator it = m_gizmos.begin(); it != m_gizmos.end(); ++it)
    {
        IGizmo* pGizmo = *it;
        gizmoHandled |= pGizmo->OnMouseScroll(pSurface, x, y, dScroll, (pGizmo == m_pClosestGizmo));
    }

    return gizmoHandled;
}

//------------------------------------------------------------------------------------------------
bool GizmoService::OnMouseMove(
        efd::Bool handled,
        efd::SInt32 x,
        efd::SInt32 y,
        efd::SInt32 dx,
        efd::SInt32 dy)
{
    EE_UNUSED_ARG(handled);

    RenderSurface* pSurface = m_pRenderService->GetActiveRenderSurface();

    if (pSurface == NULL)
        return false;

    m_pRenderService->InvalidateRenderContexts();

    efd::Bool gizmoHandled = false;

    UpdateClosestGizmo(x, y, pSurface);

    for (Gizmos::iterator it = m_gizmos.begin(); it != m_gizmos.end(); ++it)
    {
        IGizmo* pGizmo = *it;
        
        gizmoHandled |= pGizmo->OnMouseMove(pSurface, x, y, dx, dy, (pGizmo == m_pClosestGizmo));
    }


    return gizmoHandled;
}

//------------------------------------------------------------------------------------------------
bool GizmoService::OnMouseDown(
        efd::Bool handled,
        MouseMessage::MouseButton eButton,
        efd::SInt32 x,
        efd::SInt32 y)
{
    EE_UNUSED_ARG(handled);

    RenderSurface* pSurface = m_pRenderService->GetActiveRenderSurface();

    if (pSurface == NULL)
        return false;

    UpdateClosestGizmo(x, y, pSurface);

    m_pRenderService->InvalidateRenderContexts();

    efd::Bool gizmoHandled = false;

    for (Gizmos::iterator it = m_gizmos.begin(); it != m_gizmos.end(); ++it)
    {
        IGizmo* pGizmo = *it;
        gizmoHandled |= pGizmo->OnMouseDown(pSurface, eButton, x, y, (pGizmo == m_pClosestGizmo));
    }

    return gizmoHandled;
}

//------------------------------------------------------------------------------------------------
bool GizmoService::OnMouseUp(
        efd::Bool handled,
        MouseMessage::MouseButton eButton,
        efd::SInt32 x,
        efd::SInt32 y)
{
    EE_UNUSED_ARG(handled);

    if (eButton != MouseMessage::MBUTTON_LEFT)
        return false;

    RenderSurface* pSurface = m_pRenderService->GetActiveRenderSurface();

    if (pSurface == NULL)
        return false;

    m_pRenderService->InvalidateRenderContexts();

    efd::Bool gizmoHandled = false;

    UpdateClosestGizmo(x, y, pSurface);

    for (Gizmos::iterator it = m_gizmos.begin(); it != m_gizmos.end(); ++it)
    {
        IGizmo* pGizmo = *it;
        gizmoHandled |= pGizmo->OnMouseUp(pSurface, eButton, x, y, (m_pClosestGizmo == pGizmo));
    }

    return gizmoHandled;
}

//------------------------------------------------------------------------------------------------
efd::Bool GizmoService::HitTest(RenderSurface* pSurface, efd::SInt32 x, efd::SInt32 y)
{
    EE_ASSERT(pSurface);

    NiPoint3 rayOrigin;
    NiPoint3 rayDirection;

    CameraService::MouseToRay(
        (float)x,
        (float)y,
        pSurface->GetRenderTargetGroup()->GetWidth(0),
        pSurface->GetRenderTargetGroup()->GetHeight(0),
        pSurface->GetCamera(),
        rayOrigin,
        rayDirection);

    for (Gizmos::iterator it = m_gizmos.begin(); it != m_gizmos.end(); ++it)
    {
        IGizmo* pGizmo = *it;

        float outHitDistance;
        if (pGizmo->HitTest(pSurface, rayOrigin, rayDirection, outHitDistance))
        {
            return true;
        }
    }

    return false;
}

//------------------------------------------------------------------------------------------------
void GizmoService::UpdateClosestGizmo(int x, int y, RenderSurface* pSurface)
{
    if (m_pClosestGizmo == NULL)
    {
        NiPoint3 rayOrigin;
        NiPoint3 rayDirection;

        CameraService::MouseToRay(
            (float)x,
            (float)y,
            pSurface->GetRenderTargetGroup()->GetWidth(0),
            pSurface->GetRenderTargetGroup()->GetHeight(0),
            pSurface->GetCamera(),
            rayOrigin,
            rayDirection);

        float closestHitDistance = FLT_MAX;

        for (Gizmos::iterator it = m_gizmos.begin(); it != m_gizmos.end(); ++it)
        {
            IGizmo* pGizmo = *it;

            float outHitDistance = 0; // Default to 0 in case gizmo doesn't set the hit distance.
            if (pGizmo->HitTest(pSurface, rayOrigin, rayDirection, outHitDistance))
            {
                if (outHitDistance < closestHitDistance)
                {
                    closestHitDistance = outHitDistance;
                    m_pClosestGizmo = pGizmo;
                }
            }
        }
    }
}

//------------------------------------------------------------------------------------------------
void GizmoService::IsSpecialActive(bool value)
{
    m_isSpecialPrimed = value;
}

//------------------------------------------------------------------------------------------------
bool GizmoService::IsSpecialActive() const
{
    return m_isSpecialPrimed;
}

//------------------------------------------------------------------------------------------------
void GizmoService::FindAlignToSurfaceRotation(const Point3& kNormal, Point3& kRotation)
{
    //DT32771 Algorithm should be clarified and converted to use efd types

    unsigned short alignFacingAxis = 2;
    unsigned short alignUpAxis = 1;

    efd::Point3 normal = kNormal;

    NiPoint3 upAxis(NiPoint3::UNIT_Z);
    // First check if model's up and facing axis are parallel
    if ((alignUpAxis - alignFacingAxis) % 3 == 0)
        alignUpAxis += 1;

    if ((normal.Dot(upAxis) > ms_parallelThreshold) ||
        (normal.Dot(upAxis) < -ms_parallelThreshold))
    {
        if ((normal.Dot(NiPoint3::UNIT_Z) < ms_parallelThreshold) &&
            (normal.Dot(NiPoint3::UNIT_Z) > -ms_parallelThreshold))
        {
            upAxis = NiPoint3::UNIT_Z;
        }
        else if ((normal.Dot(NiPoint3::UNIT_Y) < ms_parallelThreshold) &&
                 (normal.Dot(NiPoint3::UNIT_Y) > -ms_parallelThreshold))
        {
            upAxis = NiPoint3::UNIT_Y;
        }
        else
        {
            upAxis = NiPoint3::UNIT_X;
        }
    }

    while (alignFacingAxis > 5)
        alignFacingAxis -= 6;

    while (alignUpAxis > 5)
        alignUpAxis -= 6;

    if (alignFacingAxis > 2)
    {
        alignFacingAxis -= 3;
        normal = -normal;
    }

    if (alignUpAxis > 2)
    {
        alignUpAxis -= 3;
        upAxis = -upAxis;
    }

    // calculate rotation matrix
    NiPoint3 frameX;
    NiPoint3 frameY;
    NiPoint3 frameZ;
    if (alignFacingAxis == 0)
    {
        if (alignUpAxis == 1)
        {
            // we want X axis to face N and Y to face up
            frameX = normal;
            frameZ = frameX.Cross(upAxis);
            frameY = frameZ.Cross(frameX);
        }
        else
        {
            // we want X axis to face N and Z to face up
            frameX = normal;
            frameY = upAxis.Cross(frameX);
            frameZ = frameX.Cross(frameY);
        }
    }
    else if (alignFacingAxis == 1)
    {
        if (alignUpAxis == 0)
        {
            // we want Y axis to face N and X to face up
            frameY = normal;
            frameZ = upAxis.Cross(frameY);
            frameX = frameY.Cross(frameZ);
        }
        else
        {
            // we want Y axis to face N and Z to face up
            frameY = normal;
            frameX = frameY.Cross(upAxis);
            frameZ = frameX.Cross(frameY);
        }
    }
    else
    {
        if (alignUpAxis == 0)
        {
            // we want Z axis to face N and X to face up
            frameZ = normal;
            frameY = frameZ.Cross(upAxis);
            frameX = frameY.Cross(frameZ);
        }
        else
        {
            // we want Z axis to face N and Y to face up
            frameZ = normal;
            frameX = upAxis.Cross(frameZ);
            frameY = frameZ.Cross(frameX);
        }
    }

    NiMatrix3 rotationMatrix;
    rotationMatrix.SetCol(0, frameX);
    rotationMatrix.SetCol(1, frameY);
    rotationMatrix.SetCol(2, frameZ);
    rotationMatrix.Reorthogonalize();

    rotationMatrix.ToEulerAnglesXYZ(kRotation.x, kRotation.y, kRotation.z);
    kRotation.x *= -EE_RADIANS_TO_DEGREES;
    kRotation.y *= -EE_RADIANS_TO_DEGREES;
    kRotation.z *= -EE_RADIANS_TO_DEGREES;
}

//------------------------------------------------------------------------------------------------
