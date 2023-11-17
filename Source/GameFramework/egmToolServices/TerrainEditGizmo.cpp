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

#include "TerrainEditGizmo.h"
#include "ToolServicesMessages.h"

#include <efd/ServiceManager.h>
#include <efd/ecrLogIDs.h>
#include <efd/Logger.h>

#include <ecr/PickService.h>
#include <ecr/RenderService.h>
#include <ecr/SceneGraphService.h>
#include <ecr/CameraService.h> 
#include <egf/StandardModelLibraryPropertyIDs.h>

#include <NiMesh.h>

#include "GizmoService.h"
#include "ToolCameraService.h"
#include "SelectionService.h"
#include "ToolServicesMessages.h"
#include "ToolTerrainService.h"
#include "TerrainPaintingGuide.h"

using namespace efd;
using namespace egf;
using namespace ecr;
using namespace egmToolServices;

//-----------------------------------------------------------------------------------------------
TerrainEditGizmo::TerrainEditGizmo(efd::ServiceManager* pServiceManager)
    : IGizmo(NULL)
    , m_isActive(false)
    , m_isAirbrushModeEnabled(false)
    , m_isColorInverted(false)
    , m_isPainting(false)
    , m_mouseMoved(false)
    , m_pCurrIntersectedCell(0)
    , m_lastRayOrigin(efd::Point3::ZERO)
    , m_lastRayDirection(efd::Point3::ZERO)
{
    m_pMessageService = pServiceManager->GetSystemServiceAs<efd::MessageService>();
    EE_ASSERT(m_pMessageService);

    m_pTerrainService = pServiceManager->GetSystemServiceAs<ToolTerrainService>();
    EE_ASSERT(m_pTerrainService);

    m_pTerrainPaintingGuide = EE_NEW TerrainPaintingGuide(pServiceManager);

    m_spGizmo = EE_NEW NiNode();
}

//-----------------------------------------------------------------------------------------------
TerrainEditGizmo::~TerrainEditGizmo()
{
    m_spGizmo = NULL;

    EE_DELETE m_pTerrainPaintingGuide;
}

//-----------------------------------------------------------------------------------------------
void TerrainEditGizmo::Connect(Ni3DRenderView* pGizmoView)
{
    IGizmo::Connect(pGizmoView);

    if (m_spGizmo != NULL)
        pGizmoView->AppendScene(m_spGizmo);
}

//-----------------------------------------------------------------------------------------------
void TerrainEditGizmo::Disconnect(Ni3DRenderView* pGizmoView)
{
    IGizmo::Disconnect(pGizmoView);

    if (m_spGizmo != NULL)
        pGizmoView->RemoveScene(m_spGizmo);
}

//-----------------------------------------------------------------------------------------------
efd::Bool TerrainEditGizmo::OnTick(efd::TimeType timeElapsed, ecr::RenderSurface* pSurface)
{
    // Don't do anything with the base gizmo here since it deals with entities and the entity 
    // selection.
    EE_UNUSED(timeElapsed);
    if (m_isAirbrushModeEnabled && !m_mouseMoved && m_isPainting)
        PaintStroke(m_currMouse, m_currMouse, pSurface);

    m_pTerrainPaintingGuide->OnTick(NiDynamicCast(NiNode, m_spGizmo));
    
    m_mouseMoved = false;
    return true;
}

//-----------------------------------------------------------------------------------------------
efd::Bool TerrainEditGizmo::OnMouseMove(
    ecr::RenderSurface* pSurface,
    efd::SInt32 x,
    efd::SInt32 y,
    efd::SInt32 dx,
    efd::SInt32 dy,
    efd::Bool bIsClosest)
{
    EE_UNUSED_ARG(dx);
    EE_UNUSED_ARG(dy);
    EE_UNUSED_ARG(pSurface);
    EE_UNUSED_ARG(bIsClosest);

    m_lastMouse = m_currMouse;
    m_currMouse.x = (efd::Float32)x;
    m_currMouse.y = (efd::Float32)y;
    m_mouseMoved = true;

    if (!m_spGizmoMesh)
        return true;

    // Attempt to collide with the terrain
    CameraService::MouseToRay(
        m_currMouse.x,
        m_currMouse.y, 
        pSurface->GetRenderTargetGroup()->GetWidth(0),
        pSurface->GetRenderTargetGroup()->GetHeight(0),
        pSurface->GetCamera(),
        m_lastRayOrigin,
        m_lastRayDirection);
    efd::Bool currIntersectResult = CheckForRayCollision(
        m_lastRayOrigin,
        m_lastRayDirection, 
        m_currIntersectionPt,
        m_currIntersectionNormal);

    // Update the gizmo position and visibility
    m_spGizmoMesh->SetAppCulled(!currIntersectResult);
    if (currIntersectResult)
        UpdateGizmoPosition(m_currIntersectionPt);

    // Apply painting
    if (currIntersectResult && m_isPainting)
        PaintStroke(m_lastMouse, m_currMouse, pSurface);
    
    return true;
}

//-----------------------------------------------------------------------------------------------
efd::Bool TerrainEditGizmo::OnMouseDown(
    ecr::RenderSurface* pSurface,
    ecrInput::MouseMessage::MouseButton button,
    efd::SInt32 x,
    efd::SInt32 y,
    efd::Bool bIsClosest)
{
    if (button != ecrInput::MouseMessage::MBUTTON_LEFT)
        return false;

    CameraService::MouseToRay(
        (float)x,
        (float)y,
        pSurface->GetRenderTargetGroup()->GetWidth(0),
        pSurface->GetRenderTargetGroup()->GetHeight(0),
        pSurface->GetCamera(),
        m_lastRayOrigin,
        m_lastRayDirection);

    // If we continue to intersect the terrain, generate source paint data if we are painting.
    float hitDistance;
    if (!HitTest(pSurface, m_lastRayOrigin, m_lastRayDirection, hitDistance))
    {
        m_isActive = true;
        return false;
    }

    // Left clicking the mouse button when in terrain edit mode begins a paint operation so inform
    // the tools painting is about to begin.
    m_isActive = true;
    m_currMouse.x = (efd::Float32)x;
    m_currMouse.y = (efd::Float32)y;
    m_lastMouse = m_currMouse;
    m_remainingDistance = 0.0f;
    m_isPainting = true;

    // Begin painting
    IMessagePtr spBeginPaintMsg = EE_NEW TerrainPaintingBeginMessageType();
    m_pMessageService->SendImmediate(spBeginPaintMsg);

    // Apply the first stroke
    return OnMouseMove(pSurface, x, y, 0, 0, bIsClosest);
}

//-----------------------------------------------------------------------------------------------
efd::Bool TerrainEditGizmo::OnMouseUp(
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

    if (!m_isActive)
        return false;

    // Releasing the left mouse button causes the paint operation to stop so inform the tools.
    m_isActive = true;
    m_remainingDistance = 0.0f;
    m_isPainting = false;
    m_mouseMoved = false;

    IMessagePtr spEndPaintMsg = EE_NEW TerrainPaintingEndMessageType();
    m_pMessageService->SendImmediate(spEndPaintMsg);
    return true;
}

//-----------------------------------------------------------------------------------------------
efd::Bool TerrainEditGizmo::OnMouseScroll(
    ecr::RenderSurface* pSurface,
    efd::SInt32 x,
    efd::SInt32 y,
    efd::SInt32 dScroll,
    efd::Bool bIsClosest)
{
    EE_UNUSED_ARG(pSurface);
    EE_UNUSED_ARG(dScroll);
    EE_UNUSED_ARG(x);
    EE_UNUSED_ARG(y);
    EE_UNUSED_ARG(bIsClosest);

    return false;
}

//-----------------------------------------------------------------------------------------------
void TerrainEditGizmo::BeginTransform()
{
}

//-----------------------------------------------------------------------------------------------
void TerrainEditGizmo::EndTransform(efd::Bool cancel)
{
    EE_UNUSED_ARG(cancel);
}

//-----------------------------------------------------------------------------------------------
efd::Bool TerrainEditGizmo::HitTest(
    ecr::RenderSurface* pSurface,
    const efd::Point3& rayOrigin,
    const efd::Point3& rayDirection,
    float& outHitDistance)
{
    EE_UNUSED_ARG(pSurface);

    outHitDistance = 0;

    return CheckForRayCollision(
        NiPoint3(rayOrigin),
        NiPoint3(rayDirection),
        m_currIntersectionPt,
        m_currIntersectionNormal);
}

//-----------------------------------------------------------------------------------------------
TerrainPaintingGuide* TerrainEditGizmo::GetPaintingGuide()
{
    return m_pTerrainPaintingGuide;
}

//-----------------------------------------------------------------------------------------------
void egmToolServices::TerrainEditGizmo::UpdateGizmoTransform(NiTerrain* pTerrain)
{
    if (!pTerrain || ! m_spGizmo)
        return ;

    NiTransform transform = pTerrain->GetWorldTransform();
    if (m_spGizmo->GetLocalTransform() != transform)
    {
        m_spGizmo->SetLocalTransform(transform);
        m_spGizmo->Update(0.0f);
    }
}

//-----------------------------------------------------------------------------------------------
void egmToolServices::TerrainEditGizmo::InvertBrushColor(bool invert)
{
    if (m_isColorInverted != invert)
    {
        m_isColorInverted = invert;

        // Update the Gizmo's shape (adjust all the Z coordinates of the mesh's verts)
        NiNode* pNode = NiDynamicCast(NiNode, m_spGizmoMesh);
        EE_ASSERT(pNode && pNode->GetChildCount() == 1);
        NiMesh* pMesh = NiDynamicCast(NiMesh, pNode->GetAt(0));
        EE_ASSERT(pMesh);

        // Fetch the data stream
        NiDataStreamElementLock colorLock(
            pMesh,
            NiCommonSemantics::COLOR(),
            0,
            NiDataStreamElement::F_NORMUINT8_4,
            NiDataStream::LOCK_WRITE | NiDataStream::LOCK_READ);
        EE_ASSERT(colorLock.IsLocked());
        NiTStridedRandomAccessIterator<NiRGBA> colorsIter = colorLock.begin<NiRGBA>();

        // Loop through the iterator:
        static efd::UInt32 offset = 0;
        efd::UInt32 numVerts = colorLock.count(0);
        for (efd::UInt32 vert = 0; vert < numVerts; ++vert)
        {
            colorsIter[vert] = NiRGBA(
                255 - colorsIter[vert].r(),
                255 - colorsIter[vert].g(),
                255 - colorsIter[vert].b(),
                colorsIter[vert].a());
        }
        colorsIter++;

        colorLock.Unlock();
        pMesh->Update(0.0f);

        m_spGizmo->Update(0.0f);
    }
}

//-----------------------------------------------------------------------------------------------
void TerrainEditGizmo::GetGizmoBoundingBox(
    const NiPoint3& intersectionPt,
    efd::SInt32& x, 
    efd::SInt32& y,
    efd::UInt32& width,
    efd::UInt32& height)
{
    // Given the current intersection point in world space project that position into height-map 
    // space and using the current gizmo radius, create a bounding box.
    EE_ASSERT(m_pTerrainService);

    NiTerrain* pTerrain = m_pTerrainService->GetNiTerrain();
    EE_ASSERT(pTerrain);

    NiTransform transform = pTerrain->GetWorldTransform();
    NiTransform inverseTransform;
    transform.Invert(inverseTransform);
    NiPoint3 hmLocation = inverseTransform * intersectionPt;

    // Origin is the center of the height-map.
    efd::UInt32 heightMapSize = pTerrain->GetCalcSectorSize();
    efd::Float32 heightMapCenter = heightMapSize / 2.0f;

    // Calculate the bounding box
    x = (efd::SInt32)(heightMapCenter + hmLocation.x - m_brushRadius);
    y = (efd::SInt32)(heightMapCenter + hmLocation.y - m_brushRadius);
    width = (efd::UInt32)(m_brushRadius * 2.0f);
    height = (efd::UInt32)(m_brushRadius * 2.0f);
}

//-----------------------------------------------------------------------------------------------
void TerrainEditGizmo::GetTerrainSpacePaintArea(const NiPoint3& intersectionPt,
    efd::Point2& center, efd::Point2& dimensions)
{
    // Given the current intersection point in world space project that position into height-map 
    // space and using the current gizmo radius, create a bounding box.
    EE_ASSERT(m_pTerrainService);

    NiTerrain* pTerrain = m_pTerrainService->GetNiTerrain();
    EE_ASSERT(pTerrain);

    NiTransform transform = pTerrain->GetWorldTransform();
    NiTransform inverseTransform;
    transform.Invert(inverseTransform);
    NiPoint3 hmLocation = inverseTransform * intersectionPt;

    // Origin is the center of the height-map.
    efd::UInt32 heightMapSize = pTerrain->GetCalcSectorSize() - 1;
    efd::Float32 heightMapCenter = float(heightMapSize) / 2.0f;

    // This is then the center of the area
    center.x = heightMapCenter + hmLocation.x;
    center.y = heightMapCenter + hmLocation.y;

    // And return the dimensions
    dimensions.x = 2.0f * m_brushRadius;
    dimensions.y = 2.0f * m_brushRadius;
}

//-----------------------------------------------------------------------------------------------
void TerrainEditGizmo::UpdateGizmoPosition(const efd::Point3& centerPoint)
{
    // Get the gizmo's transform (should be the same as the terrain)
    NiTransform transform = m_spGizmo->GetLocalTransform();

    // Transform out the world transform on the intersection point
    NiTransform inverseTransform;
    transform.Invert(inverseTransform);
    NiPoint3 localPoint = inverseTransform * centerPoint;

    // Transform the gizmo mesh
    m_spGizmoMesh->SetScale(1.0f);
    m_spGizmoMesh->SetTranslate(localPoint);

    // Figure out the bounding box of the gizmo
    efd::SInt32 x;
    efd::SInt32 y;
    efd::UInt32 width;
    efd::UInt32 height;
    GetGizmoBoundingBox(centerPoint, x, y, width, height);

    // Generate the world space region of heightmap that we want
    // Remember this region is 'inclusive'
    NiRect<efd::SInt32> worldSpaceRegion;
    worldSpaceRegion.m_left = x;
    worldSpaceRegion.m_bottom = y;
    worldSpaceRegion.m_right = x + width - 1;
    worldSpaceRegion.m_top = y + height - 1;
    EE_ASSERT(worldSpaceRegion.GetWidth() + 1 == efd::SInt32(width));
    EE_ASSERT(worldSpaceRegion.GetHeight() + 1 == efd::SInt32(height));

    // Fetch the data from terrain
    EE_ASSERT(m_pTerrainService);
    NiTerrain* pTerrain = m_pTerrainService->GetNiTerrain();
    EE_ASSERT(pTerrain);
    HeightMapBuffer heightMap;
    pTerrain->GetHeightMap(worldSpaceRegion, &heightMap);

    // Update the shape of the gizmo
    UpdateGizmoShape(&heightMap);

    // Update
    m_spGizmo->Update(0.0f);
}

//-----------------------------------------------------------------------------------------------
void TerrainEditGizmo::UpdateGizmoShape(HeightMapBuffer* pHeightMap)
{
    EE_ASSERT(pHeightMap);

    // Update the Gizmo's shape (adjust all the Z coordinates of the mesh's verts)
    NiNode* pNode = NiDynamicCast(NiNode, m_spGizmoMesh);
    EE_ASSERT(pNode && pNode->GetChildCount() == 1);
    NiMesh* pMesh = NiDynamicCast(NiMesh, pNode->GetAt(0));
    EE_ASSERT(pMesh);

    // Fetch the data stream
    NiDataStreamElementLock posLock(
        pMesh,
        NiCommonSemantics::POSITION(),
        0,
        NiDataStreamElement::F_FLOAT32_3,
        NiDataStream::LOCK_WRITE | NiDataStream::LOCK_READ);
    EE_ASSERT(posLock.IsLocked());
    NiTStridedRandomAccessIterator<efd::Point3> iter = posLock.begin<efd::Point3>();

    // Fetch the heightmap related to this region of the terrain
    NiTerrain* pTerrain = m_pTerrainService->GetNiTerrain();
    float minHeight = 0.0f;
    float maxHeight = 0.0f;
    if (pTerrain)
    {
        minHeight = pTerrain->GetMinHeight();
        maxHeight = pTerrain->GetMaxHeight();
    }

    // Loop through the iterator:
    static efd::UInt32 offset = 0;
    efd::UInt32 numVerts = posLock.count(0);
    for (efd::UInt32 vert = 0; vert < numVerts; ++vert)
    {
        efd::Point3& vertPoint = iter[vert];

        efd::UInt32 sampleX = efd::UInt32(vertPoint.x + m_brushRadius);
        efd::UInt32 sampleY = efd::UInt32(vertPoint.y + m_brushRadius);
        sampleX = efd::Clamp(sampleX, 0, pHeightMap->GetWidth() - 1);
        sampleY = efd::Clamp(sampleY, 0, pHeightMap->GetHeight() - 1);

        // Find the z coordinate for this X/Y position
        efd::UInt16 sample = pHeightMap->SamplePoint(sampleX, sampleY);
        vertPoint.z = NiLerp(float(sample) / float(efd::UInt16(-1)), minHeight, maxHeight); 
        vertPoint.z -= m_spGizmoMesh->GetTranslate().z;
    }
    offset++;

    posLock.Unlock();
    pMesh->Update(0.0f);
}

//-----------------------------------------------------------------------------------------------
void TerrainEditGizmo::PaintStroke(
    const NiPoint2& startPoint,
    const NiPoint2& endPoint,
    ecr::RenderSurface* pSurface)
{
    // Step size is the size of one pixel in the map to be painted.
    efd::Float32 stepSize = 2.0f;

    // Get the distance along the line to determine how many paint steps to take.
    efd::Point2 direction = endPoint - startPoint;
    efd::Float32 d = direction.Unitize();
    efd::UInt32 numSteps = (efd::UInt32)(d / stepSize);
    if (numSteps == 0)
        numSteps = 1; // Ensure at least one step is performed.

    m_remainingDistance = efd::Max(0.0f, d - (stepSize * numSteps));

    // Build up a list of paint messages to send for this stroke.
    NiPoint3 intersectionPt;
    NiPoint3 intersectionNormal;

    CameraService::MouseToRay(
        startPoint.x,
        startPoint.y,
        pSurface->GetRenderTargetGroup()->GetWidth(0),
        pSurface->GetRenderTargetGroup()->GetHeight(0),
        pSurface->GetCamera(),
        m_lastRayOrigin,
        m_lastRayDirection);

    efd::Bool currIntersectResult = CheckForRayCollision(
        m_lastRayOrigin,
        m_lastRayDirection, 
        intersectionPt,
        intersectionNormal);
    if (!currIntersectResult)
        return;

    // Get the bounding box based on the brush location and radius.
    efd::SInt32 x;
    efd::SInt32 y;
    efd::UInt32 width;
    efd::UInt32 height;
    GetGizmoBoundingBox(intersectionPt, x, y, width, height);

    // Get terrain space center and dimensions
    efd::Point2 center, dimensions;
    GetTerrainSpacePaintArea(intersectionPt, center, dimensions);

    HeightMapBuffer heightMap; 
    efd::Point2 dxdy = direction * stepSize;
    for (efd::UInt32 currStep = 0; currStep < 1; currStep++)
    {
        // Allocate a paint message for storing the source pixel data. The necessary buffers for 
        // storing the source data are automatically created when creating the message.
        SourcePaintDataMessagePtr spPaintDataMsg = 
            EE_NEW SourcePaintDataMessage(x, y, width, height, center, dimensions);

        m_pMessageService->SendImmediate(spPaintDataMsg);

        x = (efd::UInt32)(x + dxdy.x);
        y = (efd::UInt32)(y + dxdy.y);
    }
}

//-----------------------------------------------------------------------------------------------
efd::Bool TerrainEditGizmo::CheckForRayCollision(
    const NiPoint3& rayOrigin, 
    const NiPoint3& rayDirection,
    NiPoint3& collisionPt,
    NiPoint3& collisionNormal)
{
    m_pCurrIntersectedCell = 0;

    if (!m_pTerrainService)
        return false;

    NiTerrain* pTerrain = m_pTerrainService->GetNiTerrain();
    if (!pTerrain)
        return false;

    NiRay ray(rayOrigin, rayDirection);
    if (pTerrain->Collide(ray))
    {
        ray.GetIntersection(collisionPt, collisionNormal);
        m_pCurrIntersectedCell = ray.GetCollidedCell();

        return true;
    }
    else
    {
        // Work out A point on the terrain plain (we will use the terrain's position)
        NiPoint3 terrainPos = pTerrain->GetWorldTranslate();

        // Work out the terrain plane Normal:
        efd::Matrix3 invRotation;
        pTerrain->GetWorldRotate().Inverse(invRotation);
        efd::Point3 planeNormal = efd::Point3::UNIT_Z * invRotation;
        planeNormal.Unitize();

        // Calculate the intersection point
        float projectedDirection = planeNormal.Dot(rayDirection);
        efd::Point3 projectedOrigin = terrainPos - rayOrigin;
        
        // Is the ray colliding with the plane?
        if (projectedDirection == 0)
            return false;
        
        // Calculate the distance to the plane
        float distance = (planeNormal.Dot(projectedOrigin)) / projectedDirection;
        if (distance < 0)
            return false;
        
        efd::Point3 intersectPoint = rayOrigin + distance * rayDirection;

        // Assign the collision points
        collisionPt = intersectPoint;
        collisionNormal = planeNormal;

        return false;
    }
}

//-----------------------------------------------------------------------------------------------
const NiTerrainCell* egmToolServices::TerrainEditGizmo::GetLastIntersectedCell() const
{
    return m_pCurrIntersectedCell;
}

//-----------------------------------------------------------------------------------------------
