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

#include "RotationGizmo.h"

#include <ecr/PickService.h>
#include <ecr/RenderService.h>
#include <ecr/SceneGraphService.h>
#include <ecr/CameraService.h>
#include <egf/StandardModelLibraryPropertyIDs.h>

#include "GizmoService.h"
#include "ToolCameraService.h"
#include "SelectionService.h"
#include "ToolServicesMessages.h"

using namespace efd;
using namespace egf;
using namespace ecr;
using namespace egmToolServices;

//------------------------------------------------------------------------------------------------
RotationGizmo::RotationGizmo(efd::ServiceManager* pServiceManager,
                             TransformGizmoAdapter* pAdapter)
    : TransformGizmo(pServiceManager, pAdapter)
    , m_pkXAxisName(NULL)
    , m_pkYAxisName(NULL)
    , m_pkZAxisName(NULL)
    , m_pkXLineName(NULL)
    , m_pkYLineName(NULL)
    , m_pkZLineName(NULL)
    , m_mouseX(0)
    , m_mouseY(0)
    , m_mouseMoved(false)
{
    m_pkXAxisName = NiNew NiFixedString("XAxis");
    m_pkYAxisName = NiNew NiFixedString("YAxis");
    m_pkZAxisName = NiNew NiFixedString("ZAxis");
    m_pkXLineName = NiNew NiFixedString("XLine");
    m_pkYLineName = NiNew NiFixedString("YLine");
    m_pkZLineName = NiNew NiFixedString("ZLine");

    m_spGizmo = m_pSceneGraphService->LoadSceneGraph("RotateGizmo.nif");
}

//------------------------------------------------------------------------------------------------
RotationGizmo::~RotationGizmo()
{
    NiDelete m_pkXAxisName;
    NiDelete m_pkYAxisName;
    NiDelete m_pkZAxisName;
    NiDelete m_pkXLineName;
    NiDelete m_pkYLineName;
    NiDelete m_pkZLineName;
}

//------------------------------------------------------------------------------------------------
efd::Bool RotationGizmo::OnTick(efd::TimeType timeElapsed, RenderSurface* pSurface)
{
    TransformGizmo::OnTick(timeElapsed, pSurface);

    if (!IsActive())
        return true;

    if (!m_mouseMoved)
        return true;

    m_mouseMoved = false;

    NiPoint3 rayOrigin;
    NiPoint3 rayDirection;

    NiCamera* pCamera = pSurface->GetCamera();

    CameraService::MouseToRay(
        (float)(m_mouseX),
        (float)(m_mouseY),
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

    switch (m_pGizmoService->GetRelativeSpace())
    {
    case GizmoService::RSPACE_WORLD:
        {
            switch (m_eCurrentAxis)
            {
            case ROTATE_AXIS_X:
                kRotation.MakeXRotation(m_fCurrentRadians); break;
            case ROTATE_AXIS_Y:
                kRotation.MakeYRotation(m_fCurrentRadians); break;
            case ROTATE_AXIS_Z:
                kRotation.MakeZRotation(m_fCurrentRadians); break;
            }

            break;
        }
    case GizmoService::RSPACE_SCREEN:
        {
            switch (m_eCurrentAxis)
            {
            case ROTATE_AXIS_X:
                kRotation.MakeRotation(m_fCurrentRadians, pCamera->GetWorldDirection());
                break;
            case ROTATE_AXIS_Y:
                kRotation.MakeRotation(m_fCurrentRadians, pCamera->GetWorldRightVector());
                break;
            case ROTATE_AXIS_Z:
                kRotation.MakeRotation(m_fCurrentRadians, pCamera->GetWorldUpVector());
                break;
            }

            break;
        }
    }

    for (efd::UInt32 i = 0; i < m_spTransformAdapter->GetTargets(); i++)
    {
        if (m_spTransformAdapter->CanRotate(i))
        {
            if (m_pGizmoService->GetRelativeSpace() == GizmoService::RSPACE_LOCAL)
            {
                efd::Point3 kLocalAxis;

                switch (m_eCurrentAxis)
                {
                case ROTATE_AXIS_X:
                    kLocalAxis = efd::Point3::UNIT_X; break;
                case ROTATE_AXIS_Y:
                    kLocalAxis = efd::Point3::UNIT_Y; break;
                case ROTATE_AXIS_Z:
                    kLocalAxis = efd::Point3::UNIT_Z; break;
                }

                kLocalAxis = m_spTransformAdapter->GetRotationStart(i) * kLocalAxis;
                kLocalAxis.Unitize();

                kRotation.MakeRotation(m_fCurrentRadians, kLocalAxis);
            }

            NiQuaternion qStartRotation, qRotation, qCurrentRotation;
            qStartRotation.FromRotation(m_spTransformAdapter->GetRotationStart(i));
            qRotation.FromRotation(kRotation);
            qCurrentRotation = qRotation * qStartRotation;

            NiMatrix3 kCurrentRotation;
            qCurrentRotation.ToRotation(kCurrentRotation);
            kCurrentRotation.Reorthogonalize();

            m_spTransformAdapter->SetRotation(i, kCurrentRotation);
        }

        if (m_spTransformAdapter->CanTranslate(i))
        {
            if (m_pGizmoService->GetRelativeSpace() != GizmoService::RSPACE_LOCAL)
            {
                Point3 deltaTranslation = m_spTransformAdapter->GetTranslationStart(i) - m_center;
                deltaTranslation = (kRotation * deltaTranslation);

                Point3 newPosition(m_center + deltaTranslation);

                if (m_pGizmoService->TranslationPrecisionEnabled())
                {
                    // Round such that the final position matches precision.  For rotation 
                    // operations, this is only necessary when there is multiple selection.
                    GizmoService::RoundToIncrement(newPosition, 
                        m_pGizmoService->TranslationPrecision());
                }

                m_spTransformAdapter->SetTranslation(i, newPosition);
            }
        }
    }

    return true;
}

//------------------------------------------------------------------------------------------------
efd::Bool RotationGizmo::OnMouseMove(
    ecr::RenderSurface* pSurface,
    efd::SInt32 x,
    efd::SInt32 y,
    efd::SInt32 dx,
    efd::SInt32 dy,
    efd::Bool bIsClosest)
{
    EE_UNUSED_ARG(dx);
    EE_UNUSED_ARG(dy);

    if (!IsActive())
    {
        if (!bIsClosest)
        {
            if (m_eCurrentAxis != ROTATE_AXIS_NONE)
                m_eCurrentAxis = ROTATE_AXIS_NONE;

            HighLightAxis(m_eCurrentAxis);
            return false;
        }

        PickAxis(pSurface, x, y);
        return false;
    }

    if (!m_isTransforming)
    {
        if (!m_usingSpecial && m_pGizmoService->IsSpecialActive())
        {
            m_usingSpecial = true;
            m_spTransformAdapter->OnTransformClone();
            
            BeginTransform();
        }
    }

    m_mouseMoved = true;
    m_isTransforming = true;
    m_mouseX = x;
    m_mouseY = y;

    return true;
}

//------------------------------------------------------------------------------------------------
efd::Bool RotationGizmo::OnMouseDown(
    ecr::RenderSurface* pSurface,
    ecrInput::MouseMessage::MouseButton button,
    efd::SInt32 x,
    efd::SInt32 y,
    efd::Bool bIsClosest)
{
    if (button != ecrInput::MouseMessage::MBUTTON_LEFT)
        return false;

    if (!bIsClosest)
        return false;

    if (m_pCameraService->GetIsLooking() || m_pCameraService->GetIsTranslating())
        return false;

    if (m_isActive)
        return true;

    PickAxis(pSurface, x, y);

    if(m_eCurrentAxis != ROTATE_AXIS_NONE)
    {
        SetupRotation(pSurface, x, y);

        m_mouseX = x;
        m_mouseY = y;

        m_isActive = true;
        m_mouseMoved = false;
        return true;
    }

    return false;
}

//------------------------------------------------------------------------------------------------
efd::Bool RotationGizmo::OnMouseUp(
    ecr::RenderSurface* pSurface,
    ecrInput::MouseMessage::MouseButton button,
    efd::SInt32 x,
    efd::SInt32 y,
    efd::Bool bIsClosest)
{
    EE_UNUSED_ARG(pSurface);
    EE_UNUSED_ARG(x);
    EE_UNUSED_ARG(y);
    EE_UNUSED_ARG(bIsClosest);

    if (button != ecrInput::MouseMessage::MBUTTON_LEFT)
        return false;

    EndTransform(false);

    return true;
}

//------------------------------------------------------------------------------------------------
void RotationGizmo::BeginTransform()
{
    TransformGizmo::BeginTransform();

    m_center = m_spTransformAdapter->GetOrigin();
}

//------------------------------------------------------------------------------------------------
void RotationGizmo::EndTransform(efd::Bool cancel)
{
    if (!m_isActive)
        return;

    m_isActive = false;
    m_usingSpecial = false;
    m_isTransforming = false;
    m_mouseMoved = false;

    TransformGizmo::EndTransform(cancel);
}

//------------------------------------------------------------------------------------------------
efd::Bool RotationGizmo::HitTest(
    ecr::RenderSurface* pSurface,
    const efd::Point3& rayOrigin,
    const efd::Point3& rayDirection,
    float& outHitDistance)
{
    TransformToView(pSurface);

    // Issue the pick operation to the pick service.
    PickService::PickRecordPtr spPickResult = m_pPickService->PerformPick(
        rayOrigin, rayDirection, m_spGizmo, false, true, false);

    if (!spPickResult)
        return false;

    const NiPick::Results* pPickerResults = spPickResult->GetPickResult();
    NiAVObject* pkPickedObject = pPickerResults->GetAt(0)->GetAVObject();

    outHitDistance = pPickerResults->GetAt(0)->GetDistance();

    NiFixedString kName = pkPickedObject->GetName();

    if (kName == *m_pkXAxisName)
    {
        //m_eCurrentAxis = ROTATE_AXIS_X;
    }
    else if (kName == *m_pkYAxisName)
    {
        //m_eCurrentAxis = ROTATE_AXIS_Y;
    }
    else if (kName == *m_pkZAxisName)
    {
        //m_eCurrentAxis = ROTATE_AXIS_Z;
    }
    else
    {
        return false;
    }

    return true;
}

//------------------------------------------------------------------------------------------------
void RotationGizmo::SetupRotation(RenderSurface* pSurface, int x, int y)
{
    NiPoint3 rayOrigin;
    NiPoint3 rayDirection;

    BeginTransform();

    CameraService::MouseToRay(
        (float)x,
        (float)y,
        pSurface->GetRenderTargetGroup()->GetWidth(0),
        pSurface->GetRenderTargetGroup()->GetHeight(0),
        pSurface->GetCamera(),
        rayOrigin,
        rayDirection);

    if (RotateLinear(pSurface, m_eCurrentAxis))
    {
        NiPoint3 kAxis;

        switch (m_eCurrentAxis)
        {
        case ROTATE_AXIS_X:
            kAxis = NiPoint3::UNIT_X; break;
        case ROTATE_AXIS_Y:
            kAxis = NiPoint3::UNIT_Y; break;
        case ROTATE_AXIS_Z:
            kAxis = NiPoint3::UNIT_Z; break;
        }

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
                m_spGizmo->GetTranslate(),
                kAxis,
                rayOrigin,
                rayDirection);
        }

        NiPoint3 kTangent;
        // the cross product of delta and axis gives us the tangent vector
        kTangent = kAxis.Cross(kDelta);
        // now project the tangent vector parallel to the view plane
        m_pkRotationLine = kTangent - rayDirection * rayDirection.Dot(kTangent);
        m_pkRotationLine.Unitize();
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

//------------------------------------------------------------------------------------------------
float RotationGizmo::GetRadians(RenderSurface* pSurface,
                                const NiPoint3& origin,
                                const NiPoint3& direction)
{
    if (RotateLinear(pSurface, m_eCurrentAxis))
    {
        NiCamera* pkCamera = pSurface->GetCamera();
        NiPoint3 kDelta;
        kDelta = CameraService::TranslateOnAxis(
            m_spGizmo->GetTranslate(),
            m_pkRotationLine,
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
                m_spGizmo->GetTranslate()).Length() / m_fDefaultDistance) *
                pkCamera->GetViewFrustum().m_fRight * 2.0f);
        }
        float fRadians = kDelta.Dot(m_pkRotationLine) / fDistance;

        return (fRadians);
    }
    else
    {
        NiPoint3 kCenter = m_spTransformAdapter->GetOrigin();

        NiPoint3 kAxis;
        NiPoint3 kTangent;
        NiPoint3 kBiTangent;

        if (m_eCurrentAxis == ROTATE_AXIS_X)
        {
            kAxis = NiPoint3::UNIT_X;
            kTangent = NiPoint3::UNIT_Y;
            kBiTangent = NiPoint3::UNIT_Z;
        }
        else if (m_eCurrentAxis == ROTATE_AXIS_Y)
        {
            kAxis = NiPoint3::UNIT_Y;
            kTangent = NiPoint3::UNIT_Z;
            kBiTangent = NiPoint3::UNIT_X;
        }
        else if (m_eCurrentAxis == ROTATE_AXIS_Z)
        {
            kAxis = NiPoint3::UNIT_Z;
            kTangent = NiPoint3::UNIT_X;
            kBiTangent = NiPoint3::UNIT_Y;
        }

        if (m_pGizmoService->GetRelativeSpace() == GizmoService::RSPACE_LOCAL)
        {
            if (m_spTransformAdapter->GetTargets() > 0)
            {
                Matrix3 startRotation = m_spTransformAdapter->GetRotationStart(0);

                kAxis = startRotation * kAxis;
                kTangent = startRotation * kTangent;
                kBiTangent = startRotation * kBiTangent;
                kTangent = kBiTangent.Cross(kAxis);

                kAxis.Unitize();
                kTangent.Unitize();
                kBiTangent.Unitize();
            }
        }

        return (CameraService::RotateAboutAxis(
            kCenter,
            kAxis,
            kTangent,
            kBiTangent,
            origin,
            direction));
    }
}

//------------------------------------------------------------------------------------------------
efd::Bool RotationGizmo::RotateLinear(RenderSurface* pSurface, const RotateAxis axis)
{
    //if the linear setting is not turned on, we must determine if the
    //axis is perpendicular to the view vector
    NiPoint3 kAxisDirection;

    if (axis == ROTATE_AXIS_X)
    {
        kAxisDirection = NiPoint3::UNIT_X;
    }
    else if (axis == ROTATE_AXIS_Y)
    {
        kAxisDirection = NiPoint3::UNIT_Y;
    }
    else if (axis == ROTATE_AXIS_Z)
    {
        kAxisDirection = NiPoint3::UNIT_Z;
    }

    if (m_pGizmoService->GetRelativeSpace() == GizmoService::RSPACE_WORLD)
    {
        if (m_spTransformAdapter->GetTargets() > 0)
        {
            Matrix3 startRotation = m_spTransformAdapter->GetRotationStart(0);

            kAxisDirection = startRotation * kAxisDirection;
        }
    }

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

//------------------------------------------------------------------------------------------------
void RotationGizmo::PickAxis(RenderSurface* pSurface, int x, int y)
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

    efd::Point3 origin = efd::Point3(rayOrigin.x, rayOrigin.y, rayOrigin.z);
    efd::Point3 direction = efd::Point3(rayDirection.x, rayDirection.y, rayDirection.z);

    TransformToView(pSurface);

    // Issue the pick operation to the pick service.
    PickService::PickRecordPtr spPickResult = m_pPickService->PerformPick(
        origin, direction, m_spGizmo, false, true, false);

    if (spPickResult)
    {
        NiAVObject* pkPickedObject =
            spPickResult->GetPickResult()->GetAt(0)->GetAVObject();

        // set current axis
        NiFixedString kName = pkPickedObject->GetName();
        if (kName == *m_pkXAxisName)
            m_eCurrentAxis = ROTATE_AXIS_X;
        else if (kName == *m_pkYAxisName)
            m_eCurrentAxis = ROTATE_AXIS_Y;
        else if (kName == *m_pkZAxisName)
            m_eCurrentAxis = ROTATE_AXIS_Z;
        else //Z-Culler
            m_eCurrentAxis = ROTATE_AXIS_NONE;
    }
    else
    {
        m_eCurrentAxis = ROTATE_AXIS_NONE;
    }

    // set axis colors appropriately
    HighLightAxis(m_eCurrentAxis);
}

//------------------------------------------------------------------------------------------------
void RotationGizmo::HighLightAxis(const RotateAxis axis)
{
    // first, reset everything to it's default
    NiAVObject* pkLineX;
    NiAVObject* pkLineY;
    NiAVObject* pkLineZ;

    pkLineX = m_spGizmo->GetObjectByName(*m_pkXLineName);
    pkLineY = m_spGizmo->GetObjectByName(*m_pkYLineName);
    pkLineZ = m_spGizmo->GetObjectByName(*m_pkZLineName);
    EE_ASSERT(pkLineX);
    EE_ASSERT(pkLineY);
    EE_ASSERT(pkLineZ);

    NiMaterialProperty* pkMaterial;
    pkMaterial = NiDynamicCast(NiMaterialProperty,
        pkLineX->GetProperty(NiProperty::MATERIAL));
    EE_ASSERT(pkMaterial);
    pkMaterial->SetEmittance(NiColor(1.0f, 0.0f, 0.0f));

    pkMaterial = NiDynamicCast(NiMaterialProperty,
        pkLineY->GetProperty(NiProperty::MATERIAL));
    EE_ASSERT(pkMaterial);
    pkMaterial->SetEmittance(NiColor(0.0f, 1.0f, 0.0f));

    pkMaterial = NiDynamicCast(NiMaterialProperty,
        pkLineZ->GetProperty(NiProperty::MATERIAL));
    EE_ASSERT(pkMaterial);
    pkMaterial->SetEmittance(NiColor(0.0f, 0.0f, 1.0f));

    if (axis == ROTATE_AXIS_X)
    {
        pkMaterial = NiDynamicCast(NiMaterialProperty,
            pkLineX->GetProperty(NiProperty::MATERIAL));
        pkMaterial->SetEmittance(NiColor(1.0f, 1.0f, 0.0f));
    }
    else if (axis == ROTATE_AXIS_Y)
    {
        pkMaterial = NiDynamicCast(NiMaterialProperty,
            pkLineY->GetProperty(NiProperty::MATERIAL));
        pkMaterial->SetEmittance(NiColor(1.0f, 1.0f, 0.0f));
    }
    else if (axis == ROTATE_AXIS_Z)
    {
        pkMaterial = NiDynamicCast(NiMaterialProperty,
            pkLineZ->GetProperty(NiProperty::MATERIAL));
        pkMaterial->SetEmittance(NiColor(1.0f, 1.0f, 0.0f));
    }

    m_spGizmo->UpdateProperties();
}

//------------------------------------------------------------------------------------------------
