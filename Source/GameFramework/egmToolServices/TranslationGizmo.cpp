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

#include "TranslationGizmo.h"

#include <NiPick.h>

#include <ecr/PickService.h>
#include <ecr/RenderService.h>
#include <ecr/CameraService.h>
#include <egf/StandardModelLibraryPropertyIDs.h>

#include "GizmoService.h"
#include "ToolCameraService.h"
#include "SelectionService.h"
#include "ToolSnapService.h"
#include "ToolServicesMessages.h"

using namespace efd;
using namespace egf;
using namespace ecr;
using namespace egmToolServices;

static const NiColor kXColor(1.0f, 0.0f, 0.0f);
static const NiColor kYColor(0.0f, 1.0f, 0.0f);
static const NiColor kZColor(0.0f, 0.0f, 1.0f);
static const NiColor kHighlightColor(1.0f, 1.0f, 0.0f);

bool TranslationGizmo::m_userOctantFacing(false);

//-----------------------------------------------------------------------------------------------
TranslationGizmo::TranslationGizmo(efd::ServiceManager* pServiceManager,
                                   TransformGizmoAdapter* pAdapter)
    : TransformGizmo(pServiceManager, pAdapter)
    , m_pkXAxisName(NULL)
    , m_pkYAxisName(NULL)
    , m_pkZAxisName(NULL)
    , m_pkXYPlaneName(NULL)
    , m_pkXZPlaneName(NULL)
    , m_pkYZPlaneName(NULL)
    , m_pkXYPlanePanelName(NULL)
    , m_pkXZPlanePanelName(NULL)
    , m_pkYZPlanePanelName(NULL)
    , m_pkXArrowName(NULL)
    , m_pkYArrowName(NULL)
    , m_pkZArrowName(NULL)
    , m_pkXLineName(NULL), m_pkYLineName(NULL), m_pkZLineName(NULL)
    , m_pkXYLineName(NULL), m_pkXZLineName(NULL), m_pkYXLineName(NULL)
    , m_pkYZLineName(NULL), m_pkZXLineName(NULL), m_pkZYLineName(NULL)
    , m_mouseX(0)
    , m_mouseY(0)
    , m_mouseMoved(false)
{
    m_pkXAxisName = NiNew NiFixedString("XAxis");
    m_pkYAxisName = NiNew NiFixedString("YAxis");
    m_pkZAxisName = NiNew NiFixedString("ZAxis");
    m_pkXYPlaneName = NiNew NiFixedString("XYPlane");
    m_pkXZPlaneName = NiNew NiFixedString("XZPlane");
    m_pkYZPlaneName = NiNew NiFixedString("YZPlane");
    m_pkXYPlanePanelName = NiNew NiFixedString("XYPlanePanel");
    m_pkXZPlanePanelName = NiNew NiFixedString("XZPlanePanel");
    m_pkYZPlanePanelName = NiNew NiFixedString("YZPlanePanel");
    m_pkXArrowName = NiNew NiFixedString("XAxisArrow");
    m_pkYArrowName = NiNew NiFixedString("YAxisArrow");
    m_pkZArrowName = NiNew NiFixedString("ZAxisArrow");
    m_pkXLineName = NiNew NiFixedString("XLine");
    m_pkYLineName = NiNew NiFixedString("YLine");
    m_pkZLineName = NiNew NiFixedString("ZLine");
    m_pkXYLineName = NiNew NiFixedString("XYLine");
    m_pkXZLineName = NiNew NiFixedString("XZLine");
    m_pkYXLineName = NiNew NiFixedString("YXLine");
    m_pkYZLineName = NiNew NiFixedString("YZLine");
    m_pkZXLineName = NiNew NiFixedString("ZXLine");
    m_pkZYLineName = NiNew NiFixedString("ZYLine");

    m_spGizmo = m_pSceneGraphService->LoadSceneGraph("TranslateGizmo.nif");
}

//-----------------------------------------------------------------------------------------------
TranslationGizmo::~TranslationGizmo()
{
    NiDelete m_pkXAxisName;
    NiDelete m_pkYAxisName;
    NiDelete m_pkZAxisName;
    NiDelete m_pkXYPlaneName;
    NiDelete m_pkXZPlaneName;
    NiDelete m_pkYZPlaneName;
    NiDelete m_pkXYPlanePanelName;
    NiDelete m_pkXZPlanePanelName;
    NiDelete m_pkYZPlanePanelName;
    NiDelete m_pkXArrowName;
    NiDelete m_pkYArrowName;
    NiDelete m_pkZArrowName;
    NiDelete m_pkXLineName;
    NiDelete m_pkYLineName;
    NiDelete m_pkZLineName;
    NiDelete m_pkXYLineName;
    NiDelete m_pkXZLineName;
    NiDelete m_pkYXLineName;
    NiDelete m_pkYZLineName;
    NiDelete m_pkZXLineName;
    NiDelete m_pkZYLineName;
}

//-----------------------------------------------------------------------------------------------
bool TranslationGizmo::OnTick(efd::TimeType timeElapsed, RenderSurface* pSurface)
{
    TransformGizmo::OnTick(timeElapsed, pSurface);

    if (!IsActive())
        return true;

    if (!m_mouseMoved)
        return true;

    m_mouseMoved = false;

    NiPoint3 rayOrigin;
    NiPoint3 rayDirection;

    CameraService::MouseToRay(
        (float)(m_mouseX),
        (float)(m_mouseY),
        pSurface->GetRenderTargetGroup()->GetWidth(0),
        pSurface->GetRenderTargetGroup()->GetHeight(0),
        pSurface->GetCamera(),
        rayOrigin,
        rayDirection);

    NiPoint3 kOriginalAxis;

    // Calculate the translation for the gizmo
    if (m_eCurrentAxis == AXIS_X || m_eCurrentAxis == PLANE_YZ)
        kOriginalAxis = NiPoint3::UNIT_X;
    else if (m_eCurrentAxis == AXIS_Y || m_eCurrentAxis == PLANE_XZ)
        kOriginalAxis = NiPoint3::UNIT_Y;
    else if (m_eCurrentAxis == AXIS_Z || m_eCurrentAxis == PLANE_XY)
        kOriginalAxis = NiPoint3::UNIT_Z;

    NiPoint3 kAxis = kOriginalAxis;

    efd::UInt32 localTarget = 0;
    efd::Bool bLocalTarget = false;
    if (m_pGizmoService->GetRelativeSpace() == GizmoService::RSPACE_LOCAL)
    {
        for (efd::UInt32 i = 0; i < m_spTransformAdapter->GetTargets(); i++)
        {
            if (m_spTransformAdapter->CanRotate(i))
            {
                localTarget = i;
                bLocalTarget = true;
                kAxis = m_spTransformAdapter->GetRotationStart(i) * kOriginalAxis;
                break;
            }
        }
    }

    NiPoint3 kWorldTranslationDelta;

    switch (m_eCurrentAxis)
    {
    case AXIS_X:
    case AXIS_Y:
    case AXIS_Z:
        kWorldTranslationDelta = CameraService::TranslateOnAxis(m_gizmoStartPick, kAxis,
            rayOrigin, rayDirection);
        break;
    case PLANE_XY:
    case PLANE_XZ:
    case PLANE_YZ:
        kWorldTranslationDelta = CameraService::TranslateOnPlane(m_gizmoStartPick, kAxis,
            rayOrigin, rayDirection);
        break;
    default:
        EE_ASSERT(false);
    }

    if (m_pGizmoService->TranslationSnapEnabled())
    {
        // Snap to the nearest translation increment
        GizmoService::RoundToIncrement(kWorldTranslationDelta, m_pGizmoService->TranslationSnap());
    }

    bool kPicked = false;
    NiPoint3 intersection;
    Point3 eulerRotation;
    if (m_pGizmoService->GetPlacementMode() != GizmoService::PMODE_FREE_PLACEMENT)
    {
        kPicked = MatchSurface(rayOrigin, rayDirection, intersection, eulerRotation);
        if (kPicked)
            kWorldTranslationDelta = intersection - m_gizmoStartPoint;
    }

    NiPoint3 kLocalTranslationDelta(0, 0, 0);
    if (bLocalTarget && m_pGizmoService->GetRelativeSpace() == GizmoService::RSPACE_LOCAL)
    {
        kAxis = m_spTransformAdapter->GetRotationStart(localTarget) * kOriginalAxis;

        // Compute the inverse of the starting rotation.
        NiMatrix3 kInverseBaseRotation;
        EE_VERIFY(m_spTransformAdapter->GetRotationStart(localTarget).Inverse(kInverseBaseRotation));

        // Multiply the inverse base rotation times the world transform delta to get the local
        // transform delta.
        kLocalTranslationDelta = kInverseBaseRotation * kWorldTranslationDelta;
    }

    for (efd::UInt32 i = 0; i < m_spTransformAdapter->GetTargets(); i++)
    {
        if (m_spTransformAdapter->CanTranslate(i))
        {
            // Recalculate the translation for each entity's coordinate space
            if (m_pGizmoService->GetRelativeSpace() == GizmoService::RSPACE_LOCAL)
            {
                kWorldTranslationDelta = 
                    m_spTransformAdapter->GetRotationStart(i) * kLocalTranslationDelta;
            }

            bool bSnapped = false;
            if (m_spTransformAdapter->GetTargets() == 1)
            {
                bSnapped = m_spTransformAdapter->SnapToPoints(i, kWorldTranslationDelta);
            }

            if (!bSnapped)
            {
                // Only need to round translation if the translation is not the same for each
                // object.
                if (m_pGizmoService->TranslationSnapEnabled())
                {
                    // Snap to the nearest translation increment
                    GizmoService::RoundToIncrement(kWorldTranslationDelta, 
                        m_pGizmoService->TranslationSnap());
                }

                if (m_spTransformAdapter->GetTargets() > 1 &&
                    m_pGizmoService->GetPlacementMode() != GizmoService::PMODE_FREE_PLACEMENT)
                {
                    // Re-pick based on each entity in selection
                    kPicked = false;

                    rayDirection = m_spTransformAdapter->GetTranslationStart(i) + 
                        kWorldTranslationDelta - rayOrigin;
                    rayDirection.Unitize();

                    kPicked = MatchSurface(rayOrigin, rayDirection, intersection, eulerRotation);

                    if (kPicked)
                    {
                        kWorldTranslationDelta = intersection -
                            m_spTransformAdapter->GetTranslationStart(i);
                    }
                }

                // Apply rotation
                if (m_pGizmoService->GetPlacementMode() == GizmoService::PMODE_ALIGN_TO_SURFACE)
                {
                    if (kPicked)
                    {
                        efd::Matrix3 rotation, kXRot, kYRot, kZRot;

                        kXRot.MakeXRotation(eulerRotation.x * -EE_DEGREES_TO_RADIANS);
                        kYRot.MakeYRotation(eulerRotation.y * -EE_DEGREES_TO_RADIANS);
                        kZRot.MakeZRotation(eulerRotation.z * -EE_DEGREES_TO_RADIANS);
                        rotation = kXRot * kYRot * kZRot;
                        rotation.Reorthogonalize();

                        m_spTransformAdapter->SetRotation(i, rotation);
                    }
                    else
                    {
                        m_spTransformAdapter->SetRotation(i,
                            m_spTransformAdapter->GetRotationStart(i));
                    }
                }
            }

            Point3 newPosition(m_spTransformAdapter->GetTranslationStart(i) +
                kWorldTranslationDelta);

            // Round the position
            if (m_pGizmoService->TranslationPrecisionEnabled() && !bSnapped)
            {
                // Round such that final position matches precision
                GizmoService::RoundToIncrement(newPosition,
                    m_pGizmoService->TranslationPrecision());
            }

            // Apply position
            m_spTransformAdapter->SetTranslation(i, newPosition);
        }
    }

    return true;
}

//-----------------------------------------------------------------------------------------------
efd::Bool TranslationGizmo::OnMouseMove(
    ecr::RenderSurface* pSurface,
    efd::SInt32 x,
    efd::SInt32 y,
    efd::SInt32 dx,
    efd::SInt32 dy,
    efd::Bool bIsClosest)
{
    EE_UNUSED_ARG(dx);
    EE_UNUSED_ARG(dy);

    EE_ASSERT(pSurface);

    if (!IsActive())
    {
        if (!bIsClosest)
        {
            if (m_eCurrentAxis != NONE)
            {
                m_eCurrentAxis = NONE;

                HighLightAxis(m_eCurrentAxis, false);
            }

            return false;
        }

        PickAxis(pSurface, x, y);
        return false;
    }

    m_mouseMoved = true;

    m_mouseX = x;
    m_mouseY = y;

    if (m_isTransforming)
        return true;

    m_isTransforming = true;

    if (!m_usingSpecial && m_pGizmoService->IsSpecialActive())
    {
        m_usingSpecial = true;
        m_spTransformAdapter->OnTransformClone();

        BeginTransform();
    }

    return true;
}

//------------------------------------------------------------------------------------------------
efd::Bool TranslationGizmo::OnMouseDown(
    ecr::RenderSurface* pSurface,
    ecrInput::MouseMessage::MouseButton button,
    efd::SInt32 x,
    efd::SInt32 y,
    efd::Bool bIsClosest)
{
    if (button != ecrInput::MouseMessage::MBUTTON_LEFT)
        return false;

    if(!bIsClosest)
        return false;

    if (m_pCameraService->GetIsLooking() || m_pCameraService->GetIsTranslating())
        return false;

    PickService::PickRecordPtr spPickResult = PickAxis(pSurface, x, y);

    if (!spPickResult)
        return false;

    m_gizmoStartPick = spPickResult->GetPickResult()->GetAt(0)->GetIntersection();

    BeginTransform();

    m_mouseX = x;
    m_mouseY = y;
    m_mouseMoved = false;

    m_isActive = true;

    return true;
}

//------------------------------------------------------------------------------------------------
efd::Bool TranslationGizmo::OnMouseUp(
    ecr::RenderSurface* pSurface,
    ecrInput::MouseMessage::MouseButton button,
    efd::SInt32 x,
    efd::SInt32 y,
    efd::Bool bIsClosest)
{
    EE_UNUSED_ARG(x);
    EE_UNUSED_ARG(y);
    EE_UNUSED_ARG(pSurface);
    EE_UNUSED_ARG(bIsClosest);

    if (button != ecrInput::MouseMessage::MBUTTON_LEFT)
        return false;

    EndTransform(false);

    return true;
}

//------------------------------------------------------------------------------------------------
void TranslationGizmo::BeginTransform()
{
    TransformGizmo::BeginTransform();

    m_gizmoStartPoint = m_spTransformAdapter->GetOrigin();
}

//------------------------------------------------------------------------------------------------
void TranslationGizmo::EndTransform(bool cancel)
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
void TranslationGizmo::TransformToView(ecr::RenderSurface* pSurface)
{
    TransformGizmo::TransformToView(pSurface);

    if (m_userOctantFacing)
    {
        NiCamera* pCamera = pSurface->GetCamera();

        NiPoint3 selectionDir = m_spTransformAdapter->GetOrigin() - pCamera->GetTranslate();
        NiPoint3::UnitizeVector(selectionDir);
        float rad_xy = efd::FastATan2(selectionDir.y, selectionDir.x) + EE_PI;
        float quad = efd::Floor(rad_xy / EE_HALF_PI) * EE_HALF_PI;
        bool reverse_xy = (((int)efd::Floor(rad_xy / EE_HALF_PI)) % 2 != 0);
        bool reverse_z = selectionDir.z > 0;

        NiMatrix3 gizX, gizY, gizZ;
        gizX = NiMatrix3::IDENTITY;
        gizY = NiMatrix3::IDENTITY;
        if (reverse_z)
        {
            if (reverse_xy)
                gizY.MakeYRotation(EE_PI);
            else
                gizX.MakeXRotation(EE_PI);

            gizZ.MakeZRotation(quad - EE_HALF_PI);
        }
        else
        {
            gizZ.MakeZRotation(EE_TWO_PI - quad);
        }
        m_spGizmo->SetRotate(gizZ * gizX * gizY);

        HighLightAxis(m_eCurrentAxis, (reverse_z) ? !reverse_xy : reverse_xy);

        m_spGizmo->UpdateWorldBound();
        m_spGizmo->UpdateProperties();
        m_spGizmo->Update(0.0f);
    }
}

//-----------------------------------------------------------------------------------------------
bool TranslationGizmo::HitTest(
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
    outHitDistance = pPickerResults->GetAt(0)->GetDistance();

    return true;
}

//-----------------------------------------------------------------------------------------------
PickService::PickRecordPtr TranslationGizmo::PickAxis(RenderSurface* pSurface, int x, int y)
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

    TransformToView(pSurface);

    // Issue the pick operation to the pick service.
    PickService::PickRecordPtr spPickResult = m_pPickService->PerformPick(
        rayOrigin, rayDirection, m_spGizmo, false, true, true);
   
    if (spPickResult)
    {
        bool reverse_xy = false;

        if (m_userOctantFacing)
        {

            NiCamera* pCamera = pSurface->GetCamera();

            NiPoint3 selectionDir = m_spGizmo->GetTranslate() - pCamera->GetTranslate();
            NiPoint3::UnitizeVector(selectionDir);
            float rad = efd::FastATan2(selectionDir.y, selectionDir.x) + EE_PI;
            bool reverse_xy = (((int)efd::Floor(rad / EE_HALF_PI)) % 2 != 0);
            bool reverse_z = selectionDir.z > 0;
            if (reverse_z)
                reverse_xy = !reverse_xy;

        }

        NiAVObject* pkPickedObject =
            spPickResult->GetPickResult()->GetAt(0)->GetAVObject();

        NiFixedString kName = pkPickedObject->GetName();

        if (kName == *m_pkXAxisName || kName == *m_pkXLineName || kName == *m_pkXArrowName)
        {
            m_eCurrentAxis = reverse_xy ? AXIS_Y : AXIS_X;
        }
        else if (kName == *m_pkYAxisName || kName == *m_pkYLineName || kName == *m_pkYArrowName)
        {
            m_eCurrentAxis = reverse_xy ? AXIS_X : AXIS_Y;
        }
        else if (kName == *m_pkZAxisName || kName == *m_pkZLineName || kName == *m_pkZArrowName)
        {
            m_eCurrentAxis = AXIS_Z;
        }
        else if (kName == *m_pkXYPlaneName || kName == *m_pkXYPlanePanelName)
        {
            m_eCurrentAxis = PLANE_XY;
        }
        else if (kName == *m_pkXZPlaneName || kName == *m_pkXZPlanePanelName)
        {
            m_eCurrentAxis = reverse_xy ? PLANE_YZ : PLANE_XZ;
        }
        else if (kName == *m_pkYZPlaneName || kName == *m_pkYZPlanePanelName)
        {
            m_eCurrentAxis = reverse_xy ? PLANE_XZ : PLANE_YZ;
        }

        HighLightAxis(m_eCurrentAxis, reverse_xy);
    }
    else
    {
        m_eCurrentAxis = NONE;

        HighLightAxis(m_eCurrentAxis, false);
    }

    return spPickResult;
}

//-----------------------------------------------------------------------------------------------
void TranslationGizmo::HighLightAxis(const TranslateAxis eAxis, bool reverse_xy)
{
    // first, reset everything to it's default
    NiAVObject* pArrowX;
    NiAVObject* pArrowY;
    NiAVObject* pArrowZ;
    NiAVObject* pLineX;
    NiAVObject* pLineY;
    NiAVObject* pLineZ;
    NiAVObject* pLineXY;
    NiAVObject* pLineXZ;
    NiAVObject* pLineYX;
    NiAVObject* pLineYZ;
    NiAVObject* pLineZX;
    NiAVObject* pLineZY;

    if (reverse_xy)
    {
        pArrowY = m_spGizmo->GetObjectByName(*m_pkXArrowName);
        pLineY  = m_spGizmo->GetObjectByName(*m_pkXLineName);
        pLineYX = m_spGizmo->GetObjectByName(*m_pkXYLineName);
        pLineYZ = m_spGizmo->GetObjectByName(*m_pkXZLineName);

        pArrowX = m_spGizmo->GetObjectByName(*m_pkYArrowName);
        pLineX  = m_spGizmo->GetObjectByName(*m_pkYLineName);
        pLineXY = m_spGizmo->GetObjectByName(*m_pkYXLineName);
        pLineXZ = m_spGizmo->GetObjectByName(*m_pkYZLineName);

        pLineZY = m_spGizmo->GetObjectByName(*m_pkZXLineName);
        pLineZX = m_spGizmo->GetObjectByName(*m_pkZYLineName);
    }
    else
    {
        pArrowX = m_spGizmo->GetObjectByName(*m_pkXArrowName);
        pLineX  = m_spGizmo->GetObjectByName(*m_pkXLineName);
        pLineXY = m_spGizmo->GetObjectByName(*m_pkXYLineName);
        pLineXZ = m_spGizmo->GetObjectByName(*m_pkXZLineName);

        pArrowY = m_spGizmo->GetObjectByName(*m_pkYArrowName);
        pLineY  = m_spGizmo->GetObjectByName(*m_pkYLineName);
        pLineYX = m_spGizmo->GetObjectByName(*m_pkYXLineName);
        pLineYZ = m_spGizmo->GetObjectByName(*m_pkYZLineName);

        pLineZX = m_spGizmo->GetObjectByName(*m_pkZXLineName);
        pLineZY = m_spGizmo->GetObjectByName(*m_pkZYLineName);
    }

    pArrowZ = m_spGizmo->GetObjectByName(*m_pkZArrowName);
    pLineZ = m_spGizmo->GetObjectByName(*m_pkZLineName);

    EE_ASSERT(pArrowX);
    EE_ASSERT(pArrowY);
    EE_ASSERT(pArrowZ);
    EE_ASSERT(pLineX);
    EE_ASSERT(pLineY);
    EE_ASSERT(pLineZ);
    EE_ASSERT(pLineXY);
    EE_ASSERT(pLineXZ);
    EE_ASSERT(pLineYX);
    EE_ASSERT(pLineYZ);
    EE_ASSERT(pLineZX);
    EE_ASSERT(pLineZY);

    SetEmittance(pArrowX, kXColor);
    SetEmittance(pLineX, kXColor);
    SetEmittance(pLineXY, kXColor);
    SetEmittance(pLineXZ, kXColor);

    SetEmittance(pArrowY, kYColor);
    SetEmittance(pLineY, kYColor);
    SetEmittance(pLineYX, kYColor);
    SetEmittance(pLineYZ, kYColor);

    SetEmittance(pArrowZ, kZColor);
    SetEmittance(pLineZ, kZColor);
    SetEmittance(pLineZX, kZColor);
    SetEmittance(pLineZY, kZColor);

    if (eAxis == AXIS_X)
    {
        SetEmittance(pArrowX, kHighlightColor);
        SetEmittance(pLineX, kHighlightColor);
    }
    else if (eAxis == AXIS_Y)
    {
        SetEmittance(pArrowY, kHighlightColor);
        SetEmittance(pLineY, kHighlightColor);
    }
    else if (eAxis == AXIS_Z)
    {
        SetEmittance(pArrowZ, kHighlightColor);
        SetEmittance(pLineZ, kHighlightColor);
    }
    else if (eAxis == PLANE_XY)
    {
        SetEmittance(pArrowX, kHighlightColor);
        SetEmittance(pLineX, kHighlightColor);
        SetEmittance(pArrowY, kHighlightColor);
        SetEmittance(pLineY, kHighlightColor);
        SetEmittance(pLineXY, kHighlightColor);
        SetEmittance(pLineYX, kHighlightColor);
    }
    else if (eAxis == PLANE_XZ)
    {
        SetEmittance(pArrowX, kHighlightColor);
        SetEmittance(pLineX, kHighlightColor);
        SetEmittance(pArrowZ, kHighlightColor);
        SetEmittance(pLineZ, kHighlightColor);
        SetEmittance(pLineXZ, kHighlightColor);
        SetEmittance(pLineZX, kHighlightColor);
    }
    else if (eAxis == PLANE_YZ)
    {
        SetEmittance(pArrowZ, kHighlightColor);
        SetEmittance(pLineZ, kHighlightColor);
        SetEmittance(pArrowY, kHighlightColor);
        SetEmittance(pLineY, kHighlightColor);
        SetEmittance(pLineZY, kHighlightColor);
        SetEmittance(pLineYZ, kHighlightColor);
    }

    m_spGizmo->UpdateProperties();
}

//-----------------------------------------------------------------------------------------------
void TranslationGizmo::SetEmittance(NiAVObject* pObject, const NiColor& kColor)
{
    NiMaterialProperty* pMaterial = NiDynamicCast(NiMaterialProperty,
        pObject->GetProperty(NiProperty::MATERIAL));
    pMaterial->SetEmittance(kColor);
}

//-----------------------------------------------------------------------------------------------
