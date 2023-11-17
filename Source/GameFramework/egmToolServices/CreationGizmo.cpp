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
#include "egmToolServicesLibType.h"

#include "CreationGizmo.h"

#include <efd/ServiceManager.h>
#include <egf/FlatModelManager.h>
#include <egf/EntityManager.h>
#include <egf/StandardModelLibraryPropertyIDs.h>

#include <ecr/PickService.h>
#include <ecr/RenderService.h>
#include <ecr/SceneGraphService.h>
#include <ecr/CameraService.h>

#include "GizmoService.h"
#include "SelectionService.h"
#include "GridService.h"
#include "ToolServicesMessages.h"

using namespace efd;
using namespace egf;
using namespace ecr;
using namespace egmToolServices;

//-----------------------------------------------------------------------------------------------
CreationGizmo::CreationGizmo(efd::ServiceManager* pServiceManager, CreationGizmoAdapter* pAdapter)
    : TransformGizmo(pServiceManager, pAdapter)
    , m_spCreationAdapter(pAdapter)
    , m_startingPoint(0, 0, 0)
    , m_startingRotation(0, 0, 0)
    , m_endingRotation(0, 0, 0)
    , m_fStartingRadians(0)
    , m_fCurrentRadians(0)
    , m_rotationLine(0, 0, 0)
    , m_mouseDownX(0)
    , m_mouseDownY(0)
    , m_stage(CreationGizmo::STAGE_NONE)
{
    m_pGridService = pServiceManager->GetSystemServiceAs<GridService>();
    EE_ASSERT(m_pGridService);
}

//-----------------------------------------------------------------------------------------------
CreationGizmo::~CreationGizmo()
{
}

//------------------------------------------------------------------------------------------------
efd::Bool CreationGizmo::OnMouseMove(
    ecr::RenderSurface* pSurface,
    efd::SInt32 x,
    efd::SInt32 y,
    efd::SInt32 dx,
    efd::SInt32 dy,
    efd::Bool bIsClosest)
{
    EE_UNUSED_ARG(dx);
    EE_UNUSED_ARG(dy);
    EE_UNUSED_ARG(bIsClosest);

    m_bMouseMoved = true;

    switch (m_stage)
    {
    case STAGE_NONE:
        TranslateTarget(pSurface, x, y);
        break;
    case STAGE_LOCKED:
        RotateTarget(pSurface, x, y);
        break;
    case STAGE_CREATING:
        break;
    }

    return true;
}

//-----------------------------------------------------------------------------------------------
void CreationGizmo::TranslateTarget(RenderSurface* pSurface, efd::SInt32 x, efd::SInt32 y)
{
    NiPoint3 rayOrigin;
    NiPoint3 rayDirection;

    NiCamera* pCamera = pSurface->GetCamera();

    CameraService::MouseToRay(
        (float)(x),
        (float)(y),
        pSurface->GetRenderTargetGroup()->GetWidth(0),
        pSurface->GetRenderTargetGroup()->GetHeight(0),
        pCamera,
        rayOrigin,
        rayDirection);

    PickService::PickRecordPtr spPickResult =
        m_pPickService->PerformPick(rayOrigin, rayDirection);

    bool foundIntersection = false;

    if (spPickResult)
    {
        const NiPick::Results* pPickerResults = spPickResult->GetPickResult();
        EE_ASSERT(pPickerResults);

        for (efd::UInt32 i = 0; i < pPickerResults->GetSize(); i++)
        {
            NiAVObject* pPickedObject = pPickerResults->GetAt(i)->GetAVObject()->GetRoot();

            if (!m_spCreationAdapter->IsSelected(pPickedObject))
            {
                // Retrieve surface intersection point
                m_startingPoint = pPickerResults->GetAt(i)->GetIntersection();
                foundIntersection = true;
                break;
            }
        }
    }

    // Pick against the grid if we didn't hit anything or we're within the grid interval
    float distance = Point3::VectorLength(m_startingPoint - rayOrigin);

    bool placedOnGrid = false;

    if (!foundIntersection ||
        (m_pGridService->GetUseVerticalInterval() &&
        distance > m_pGridService->GetVerticalInterval()))
    {
        m_startingPoint = m_pGridService->PickOnGrid(rayOrigin, rayDirection);
        placedOnGrid = true;
    }

    if (m_pGizmoService->TranslationSnapEnabled())
    {
        // Snap to the nearest translation increment
        GizmoService::RoundToIncrement(m_startingPoint, m_pGizmoService->TranslationSnap());
    }

    if (!placedOnGrid &&
        m_pGizmoService->GetPlacementMode() != GizmoService::PMODE_FREE_PLACEMENT)
    {
        efd::Point3 surfacePoint;
        efd::Point3 surfaceRotation;

        bool surfacePicked =
            MatchSurface(rayOrigin, rayDirection, surfacePoint, surfaceRotation);

        if (surfacePicked)
            m_startingPoint = surfacePoint;

        if (m_pGizmoService->GetPlacementMode() == GizmoService::PMODE_ALIGN_TO_SURFACE)
        {
            if (surfacePicked)
            {
                efd::Matrix3 rotation, kXRot, kYRot, kZRot;

                kXRot.MakeXRotation(surfaceRotation.x * -EE_DEGREES_TO_RADIANS);
                kYRot.MakeYRotation(surfaceRotation.y * -EE_DEGREES_TO_RADIANS);
                kZRot.MakeZRotation(surfaceRotation.z * -EE_DEGREES_TO_RADIANS);
                rotation = kXRot * kYRot * kZRot;
                rotation.Reorthogonalize();

                m_spCreationAdapter->SetRotation(0, rotation);
            }
        }
    }

    m_spCreationAdapter->SetTranslation(0, m_startingPoint);
}

//-----------------------------------------------------------------------------------------------
void CreationGizmo::RotateTarget(RenderSurface* pSurface, int x, int y)
{
    NiPoint3 rayOrigin;
    NiPoint3 rayDirection;

    NiCamera* pCamera = pSurface->GetCamera();

    CameraService::MouseToRay(
        (float)(x),
        (float)(y),
        pSurface->GetRenderTargetGroup()->GetWidth(0),
        pSurface->GetRenderTargetGroup()->GetHeight(0),
        pCamera,
        rayOrigin,
        rayDirection);

    float fCurrentRadians;
    fCurrentRadians = GetRadians(pSurface, rayOrigin, rayDirection);
    m_fCurrentRadians = m_fStartingRadians - fCurrentRadians;

    if (m_pGizmoService->RotationSnapEnabled())
    {
        // If snap is on, we need to construct a rotation such that it is
        // rounded to the nearest snap increment relative to the starting
        // rotation
        double snapIncrementRadians = m_pGizmoService->RotationSnap() * -EE_DEGREES_TO_RADIANS;
        GizmoService::RoundToIncrement(m_fCurrentRadians, snapIncrementRadians);
    }

    NiMatrix3 kRotation;
    kRotation.MakeZRotation(m_fCurrentRadians);

    NiMatrix3 kXRot, kYRot, kZRot;
    kXRot.MakeXRotation(m_startingRotation.x * -EE_DEGREES_TO_RADIANS);
    kYRot.MakeYRotation(m_startingRotation.y * -EE_DEGREES_TO_RADIANS);
    kZRot.MakeZRotation(m_startingRotation.z * -EE_DEGREES_TO_RADIANS);
    NiMatrix3 kStartRotation = kXRot * kYRot * kZRot;
    kStartRotation.Reorthogonalize();

    NiQuaternion qStartRotation, qRotation, qCurrentRotation;
    qStartRotation.FromRotation(kStartRotation);
    qRotation.FromRotation(kRotation);
    qCurrentRotation = qRotation * qStartRotation;

    NiMatrix3 kCurrentRotation;
    qCurrentRotation.ToRotation(kCurrentRotation);
    kCurrentRotation.Reorthogonalize();

    m_spCreationAdapter->SetRotation(0, kCurrentRotation);
}

//-----------------------------------------------------------------------------------------------
efd::Bool CreationGizmo::OnMouseDown(
    ecr::RenderSurface* pSurface,
    ecrInput::MouseMessage::MouseButton button,
    efd::SInt32 x,
    efd::SInt32 y,
    efd::Bool bIsClosest)
{
    EE_UNUSED_ARG(bIsClosest);

    if (button != ecrInput::MouseMessage::MBUTTON_LEFT)
        return false;

    m_isActive = true;

    m_mouseDownX = x;
    m_mouseDownY = y;

    m_spCreationAdapter->BeginTransform();

    SetupRotation(pSurface, x, y);

    TranslateTarget(pSurface, x, y);

    m_stage = STAGE_LOCKED;

    return true;
}

//-----------------------------------------------------------------------------------------------
efd::Bool CreationGizmo::OnMouseUp(
    ecr::RenderSurface* pSurface,
    ecrInput::MouseMessage::MouseButton button,
    efd::SInt32 x,
    efd::SInt32 y,
    efd::Bool bIsClosest)
{
    if (button != ecrInput::MouseMessage::MBUTTON_LEFT)
        return false;

    EE_UNUSED_ARG(pSurface);
    EE_UNUSED_ARG(x);
    EE_UNUSED_ARG(y);
    EE_UNUSED_ARG(bIsClosest);

    if (!m_isActive)
        return false;

    m_isActive = false;

    m_stage = STAGE_CREATING;

    m_spCreationAdapter->EndTransform(false);

    m_spCreationAdapter->Create();

    m_startingRotation = m_endingRotation;

    m_stage = STAGE_NONE;

    return true;
}

//------------------------------------------------------------------------------------------------
void CreationGizmo::BeginTransform()
{
    TransformGizmo::BeginTransform();
}

//------------------------------------------------------------------------------------------------
void CreationGizmo::EndTransform(bool cancel)
{
    if (!m_isActive)
        return;

    m_spCreationAdapter->EndTransform(cancel);

    m_isActive = false;
}

//-----------------------------------------------------------------------------------------------
bool CreationGizmo::IsActive()
{
    return m_isActive;
}

//-----------------------------------------------------------------------------------------------
bool CreationGizmo::OnTick(efd::TimeType timeElapsed, RenderSurface* pSurface)
{
    IGizmo::OnTick(timeElapsed, pSurface);

    return IsActive();
}

//-----------------------------------------------------------------------------------------------
void CreationGizmo::SetupRotation(RenderSurface* pSurface, int x, int y)
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

    if (RotateLinear(pSurface))
    {
        NiPoint3 kAxis = NiPoint3::UNIT_Z;

        // project our mouse click on to gizmo axis plane and find the
        // delta vector between the gizmo center and projection result
        NiPoint3 kLook;
        float fCosine;
        NiPoint3 kDelta;

        pSurface->GetCamera()->GetRotate().GetCol(0, (float*)&kLook);
        fCosine = kLook.Dot(kAxis);

        // check if the rotation axis is perpendicular to the view
        if ((fCosine <= GizmoService::ms_invParallelThreshold) &&
            (fCosine >= -GizmoService::ms_invParallelThreshold))
        {
            kDelta = -kLook;
        }
        else
        {
            kDelta = CameraService::TranslateOnPlane(
                m_startingPoint,
                kAxis,
                rayOrigin,
                rayDirection);
        }

        NiPoint3 kTangent;
        // the cross product of delta and axis gives us the tangent vector
        kTangent = kAxis.Cross(kDelta);
        // now project the tangent vector parallel to the view plane
        m_rotationLine = kTangent - rayDirection * rayDirection.Dot(kTangent);
        m_rotationLine.Unitize();
        // the initial linear offset accounts for the place that the user
        // initially clicks is not at the origin
        m_fStartingRadians = GetRadians(pSurface, rayOrigin, rayDirection);
    }
    else
    {
        // record the most recent angle
        m_fStartingRadians = GetRadians(pSurface, rayOrigin, rayDirection);
    }
}

//-----------------------------------------------------------------------------------------------
bool CreationGizmo::HitTest(
    ecr::RenderSurface* pSurface,
    const efd::Point3& rayOrigin,
    const efd::Point3& rayDirection,
    float& outHitDistance)
{
    EE_UNUSED_ARG(pSurface);
    EE_UNUSED_ARG(rayOrigin);
    EE_UNUSED_ARG(rayDirection);

    outHitDistance = 0;
    return true;
}

//-----------------------------------------------------------------------------------------------
float CreationGizmo::GetRadians(RenderSurface* pSurface,
                                const efd::Point3& origin,
                                const efd::Point3& direction)
{
    if (RotateLinear(pSurface))
    {
        // Clamp very small values to "zero rotation"
        if (m_rotationLine.SqrLength() < 0.000001f)
            return 0;

        NiCamera* pkCamera = pSurface->GetCamera();
        NiPoint3 kDelta;
        kDelta = CameraService::TranslateOnAxis(
            m_startingPoint,
            m_rotationLine,
            origin,
            direction);

        // reduce delta proportional to gizmo scale
        float fDistance;
        if (pkCamera->GetViewFrustum().m_bOrtho)
        {
            NiFrustum pkFrustum = pkCamera->GetViewFrustum();
            fDistance = ((pkFrustum.m_fRight * 2.0f) / m_fDefaultDistance);
        }
        else
        {
            fDistance = (((pkCamera->GetTranslate() -
                m_startingPoint).Length() / m_fDefaultDistance) *
                pkCamera->GetViewFrustum().m_fRight * 2.0f);
        }
        float fRadians = kDelta.Dot(m_rotationLine) / fDistance;

        return (fRadians);
    }
    else
    {
        NiPoint3 kAxis;
        NiPoint3 kTangent;
        NiPoint3 kBiTangent;

        kAxis = NiPoint3::UNIT_Z;
        kTangent = NiPoint3::UNIT_X;
        kBiTangent = NiPoint3::UNIT_Y;

        return (CameraService::RotateAboutAxis(
            m_startingPoint,
            kAxis,
            kTangent,
            kBiTangent,
            origin,
            direction));
    }
}

//-----------------------------------------------------------------------------------------------
bool CreationGizmo::RotateLinear(RenderSurface* pSurface)
{
    //if the linear setting is not turned on, we must determine if the
    //axis is perpendicular to the view vector
    NiPoint3 kAxisDirection = NiPoint3::UNIT_Z;

    NiCamera* pkCam;
    NiPoint3 kLook;
    float fCosine;

    pkCam = pSurface->GetCamera();
    pkCam->GetRotate().GetCol(0, (float*)&kLook);
    fCosine = kLook.Dot(kAxisDirection);

    // check if the rotation axis is perpendicular to the view
    if ((fCosine <= GizmoService::ms_invParallelThreshold) &&
        (fCosine >= -GizmoService::ms_invParallelThreshold))
    {
        return true;
    }

    return false;
}

//-----------------------------------------------------------------------------------------------
