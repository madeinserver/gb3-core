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

#include "NiPrimitiveType.h"
#include "NiMeshLib.h"
#include "NiMaterialProperty.h"
#include "NiVertexColorProperty.h"

#include <ecr/SceneGraphService.h>

#include "GridService.h"

using namespace efd;
using namespace egf;
using namespace ecr;
using namespace egmToolServices;

EE_IMPLEMENT_CONCRETE_CLASS_INFO(GridService);

//------------------------------------------------------------------------------------------------
GridService::GridService()
    : m_displayGrid(false)
    , m_spMinorLines(NULL)
    , m_spMajorLines(NULL)
    , m_minorLineColor(0.3f, 0.3f, 0.3f)
    , m_majorLineColor(0.0f, 0.0f, 0.0f)
    , m_minorLineSpacing(1.0f)
    , m_majorLineSpacing(10.0f)
    , m_extents(100.0f)
    , m_useVerticalInterval(false)
    , m_verticalInterval(200)
    , m_rebuildGridRequested(false)
{
    // If this default priority is changed, also update the service quick reference documentation
    m_defaultPriority = 2050;
}

//------------------------------------------------------------------------------------------------
GridService::~GridService()
{
    // This method intentionally left blank (all shutdown occurs in OnShutdown)
}

//------------------------------------------------------------------------------------------------
const char* GridService::GetDisplayName() const
{
    return "GridService";
}

//------------------------------------------------------------------------------------------------
SyncResult GridService::OnPreInit(efd::IDependencyRegistrar* pDependencyRegistrar)
{
    pDependencyRegistrar->AddDependency<RenderService>();
    pDependencyRegistrar->AddDependency<PickService>();

    m_pRenderService = m_pServiceManager->GetSystemServiceAs<RenderService>();
    EE_ASSERT(m_pRenderService);

    m_spSceneGraphService = EE_DYNAMIC_CAST(ToolSceneGraphService,
        m_pServiceManager->GetSystemServiceAs<SceneGraphService>());

    EE_ASSERT(m_spSceneGraphService);

    // Construct grid lines
    RebuildGrid();

    return SyncResult_Success;
}

//------------------------------------------------------------------------------------------------
AsyncResult GridService::OnInit()
{
    if (!m_pRenderService->GetRenderer())
        return AsyncResult_Pending;

    m_pRenderService->AddDelegate(this);

    return AsyncResult_Complete;
}

//------------------------------------------------------------------------------------------------
AsyncResult GridService::OnTick()
{
    if (m_rebuildGridRequested)
    {
        RebuildGrid();
    }

    return AsyncResult_Pending;
}

//------------------------------------------------------------------------------------------------
AsyncResult GridService::OnShutdown()
{
    DeleteGrid();

    m_spSceneGraphService = NULL;

    return AsyncResult_Complete;
}

//------------------------------------------------------------------------------------------------
void GridService::DeleteGrid()
{
    if (m_spMajorLines)
    {
        m_spSceneGraphService->RemoveSceneGraph(m_majorLinesHandle);
        m_spMajorLines = NULL;
    }

    if (m_spMinorLines)
    {
        m_spSceneGraphService->RemoveSceneGraph(m_minorLinesHandle);
        m_spMinorLines = NULL;
    }

    for (efd::vector<GridService::GridSection*>::iterator iter = m_gridSections.begin();
         iter != m_gridSections.end();
         ++iter)
    {
        GridService::GridSection* pGridSection = *iter;

        if (pGridSection->MajorLines)
        {
            m_spSceneGraphService->RemoveSceneGraph(pGridSection->MajorLinesHandle);
            pGridSection->MajorLines = NULL;
        }

        if (pGridSection->MinorLines)
        {
            m_spSceneGraphService->RemoveSceneGraph(pGridSection->MinorLinesHandle);
            pGridSection->MinorLines = NULL;
        }

        EE_EXTERNAL_DELETE pGridSection;
    }

    m_gridSections.clear();
}

//------------------------------------------------------------------------------------------------
void GridService::ConstructStaticGrid()
{
    float extents = m_spSceneGraphService->GetWorldScale() * m_extents;
    float majorLineSpacing = m_spSceneGraphService->GetWorldScale() * m_majorLineSpacing;
    float minorLineSpacing = m_spSceneGraphService->GetWorldScale() * m_minorLineSpacing;

    // Determine if major lines overlap minor lines
    bool inPhase = (Fmod(majorLineSpacing, minorLineSpacing) < 1e-06f &&
        majorLineSpacing >= minorLineSpacing);

    // Take into account symmetry, the origin line, and both axes
    int majorLinesOneExtent = (int)(extents / majorLineSpacing);
    int majorLineCount = (majorLinesOneExtent * 2 + 1) * 2;
    int majorVertCount = majorLineCount * 2;
    int minorLinesOneExtent = (int)(extents / minorLineSpacing);
    int minorLineCount = (minorLinesOneExtent * 2 + 1) * 2;
    int minorVertCount = minorLineCount * 2;
    if (inPhase)
        minorVertCount -= majorVertCount;

    // Construct major lines mesh
    m_spMajorLines = NiNew NiMesh();
    m_spMajorLines->SetPrimitiveType(NiPrimitiveType::PRIMITIVE_LINES);
    NiDataStreamRef* pMajorStreamRef =
        m_spMajorLines->AddStream(NiCommonSemantics::POSITION(), 0,
        NiDataStreamElement::F_FLOAT32_3, majorVertCount,
        NiDataStream::ACCESS_GPU_READ | NiDataStream::ACCESS_CPU_READ |
        NiDataStream::ACCESS_CPU_WRITE_MUTABLE,
        NiDataStream::USAGE_VERTEX, NULL, false);
    EE_ASSERT(pMajorStreamRef);
    NiDataStream* pMajorStream = pMajorStreamRef->GetDataStream();
    NiPoint3* pMajorVerts = (NiPoint3*)pMajorStream->Lock(
        NiDataStream::LOCK_WRITE);

    // Add major lines
    int vertCount = 0;
    int line;
    for (line = -majorLinesOneExtent; line <= majorLinesOneExtent; line++)
    {
        // X axis line
        pMajorVerts[vertCount] = NiPoint3(-extents,
            line * majorLineSpacing, 0.0f);
        pMajorVerts[vertCount + 1] = NiPoint3(extents,
            line * majorLineSpacing, 0.0f);

        // Y axis line
        pMajorVerts[vertCount + 2] = NiPoint3(line * majorLineSpacing,
            -extents, 0.0f);
        pMajorVerts[vertCount + 3] = NiPoint3(line * majorLineSpacing,
            extents, 0.0f);

        vertCount += 4;
    }
    pMajorStream->Unlock(NiDataStream::LOCK_WRITE);

    // Construct minor lines mesh
    m_spMinorLines = NiNew NiMesh();
    m_spMinorLines->SetPrimitiveType(NiPrimitiveType::PRIMITIVE_LINES);
    NiDataStreamRef* pMinorStreamRef =
        m_spMinorLines->AddStream(NiCommonSemantics::POSITION(), 0,
        NiDataStreamElement::F_FLOAT32_3, minorVertCount,
        NiDataStream::ACCESS_GPU_READ | NiDataStream::ACCESS_CPU_READ |
        NiDataStream::ACCESS_CPU_WRITE_MUTABLE,
        NiDataStream::USAGE_VERTEX, NULL, false);
    EE_ASSERT(pMinorStreamRef);
    NiDataStream* pMinorStream = pMinorStreamRef->GetDataStream();
    NiPoint3* pMinorVerts = (NiPoint3*)pMinorStream->Lock(
        NiDataStream::LOCK_WRITE);

    // Add minor lines
    vertCount = 0;
    for (line = -minorLinesOneExtent; line <= minorLinesOneExtent; line++)
    {
        if (inPhase &&
            line % (int)(majorLineSpacing / minorLineSpacing) == 0)
        {
            // Major line here already
            continue;
        }

        // X axis line
        pMinorVerts[vertCount] = NiPoint3(-extents, line * minorLineSpacing, 0.0f);
        pMinorVerts[vertCount + 1] = NiPoint3(extents, line * minorLineSpacing, 0.0f);

        // Y axis line
        pMinorVerts[vertCount + 2] = NiPoint3(line * minorLineSpacing, -extents, 0.0f);
        pMinorVerts[vertCount + 3] = NiPoint3(line * minorLineSpacing, extents, 0.0f);

        vertCount += 4;
    }
    pMinorStream->Unlock(NiDataStream::LOCK_WRITE);

    // Setup major line properties
    NiMaterialProperty* pMajorMaterial = NiNew NiMaterialProperty();
    pMajorMaterial->SetAmbientColor(NiColor::BLACK);
    pMajorMaterial->SetDiffuseColor(NiColor::BLACK);
    pMajorMaterial->SetSpecularColor(NiColor::BLACK);
    pMajorMaterial->SetEmittance(m_majorLineColor);
    pMajorMaterial->SetShineness(0.0f);
    pMajorMaterial->SetAlpha(1.0f);
    m_spMajorLines->AttachProperty(pMajorMaterial);

    NiVertexColorProperty* pMajorVCProp = NiNew NiVertexColorProperty();
    pMajorVCProp->SetLightingMode(NiVertexColorProperty::LIGHTING_E);
    m_spMajorLines->AttachProperty(pMajorVCProp);

    m_spMajorLines->RecomputeBounds();
    m_spMajorLines->UpdateProperties();
    m_spMajorLines->UpdateEffects();
    m_spMajorLines->Update(0.0f);

    // Setup minor line properties
    NiMaterialProperty* pMinorMaterial = NiNew NiMaterialProperty();
    pMinorMaterial->SetAmbientColor(NiColor::BLACK);
    pMinorMaterial->SetDiffuseColor(NiColor::BLACK);
    pMinorMaterial->SetSpecularColor(NiColor::BLACK);
    pMinorMaterial->SetEmittance(m_minorLineColor);
    pMinorMaterial->SetShineness(0.0f);
    pMinorMaterial->SetAlpha(1.0f);
    m_spMinorLines->AttachProperty(pMinorMaterial);

    NiVertexColorProperty* pMinorVCProp = NiNew NiVertexColorProperty();
    pMinorVCProp->SetLightingMode(NiVertexColorProperty::LIGHTING_E);
    m_spMinorLines->AttachProperty(pMinorVCProp);

    m_spMinorLines->RecomputeBounds();
    m_spMinorLines->UpdateProperties();
    m_spMinorLines->UpdateEffects();
    m_spMinorLines->Update(0.0f);
}

//------------------------------------------------------------------------------------------------
void GridService::RequestRebuildGrid()
{
    m_rebuildGridRequested = true;
}

//------------------------------------------------------------------------------------------------
void GridService::RebuildGrid()
{
    m_rebuildGridRequested = false;

    DeleteGrid();

    if (m_displayGrid && m_spSceneGraphService)
    {
        // Add updated grid meshes
        ConstructStaticGrid();

        m_gridSections.push_back(CreateGridSection());
        m_gridSections.push_back(CreateGridSection());
    }
}

//------------------------------------------------------------------------------------------------
static NiPoint3 GetBestPlane(const NiPoint3& pkLook)
{
    float fCosine = pkLook.Dot(NiPoint3::UNIT_Z);

    if ((fCosine <= 0.01f) && (fCosine >= -0.01f))
    {
        // the default up-axis is no good - find another one
        // we want the axis who's normal is closest to the look vector
        float fXBias = NiAbs(pkLook.Dot(NiPoint3::UNIT_X));
        float fYBias = NiAbs(pkLook.Dot(NiPoint3::UNIT_Y));
        float fZBias = NiAbs(pkLook.Dot(NiPoint3::UNIT_Z));

        if ((fXBias >= fYBias) && (fXBias >= fZBias))
        {
            return NiPoint3::UNIT_X;
        }
        else if (fYBias >= fZBias)
        {
            return NiPoint3::UNIT_Y;
        }
        else
        {
            return NiPoint3::UNIT_Y;
        }
    }

    return NiPoint3::UNIT_Z;
}

//------------------------------------------------------------------------------------------------
NiPoint3 GridService::PickOnGrid(const NiPoint3& rayOrigin, const NiPoint3& rayDirection)
{
    NiPoint3 kPlaneNormal = GetBestPlane(rayDirection);

    float z = 0;

    if (kPlaneNormal == NiPoint3::UNIT_Z)
    {
        float fCosine = rayDirection.Dot(NiPoint3::UNIT_Z);

        bool pickFloor = fCosine <= 0;

        if (m_useVerticalInterval)
        {
            if (rayOrigin.z > 0)
            {
                if (pickFloor)
                    z = m_verticalInterval * (int)(rayOrigin.z / m_verticalInterval);
                else
                    z = m_verticalInterval * ((int)(rayOrigin.z / m_verticalInterval) + 1);
            }
            else if (rayOrigin.z < 0)
            {
                if (pickFloor)
                    z = m_verticalInterval * ((int)(rayOrigin.z / m_verticalInterval) - 1);
                else
                    z = m_verticalInterval * (int)(rayOrigin.z / m_verticalInterval);
            }
        }

        if (!pickFloor)
            kPlaneNormal = -NiPoint3::UNIT_Z;
    }

    NiPoint3 startPoint = CameraService::TranslateOnPlane(
        NiPoint3(0, 0, z),
        kPlaneNormal,
        rayOrigin,
        rayDirection);

    startPoint.z = z;

    return startPoint;
}

//------------------------------------------------------------------------------------------------
void GridService::OnSurfacePreDraw(RenderService* pService, RenderSurface* pSurface)
{
    EE_UNUSED_ARG(pService);

    if (m_spSceneGraphService == NULL)
        return;

    if (m_gridSections.size() == 0)
        return;

    NiCamera* pCamera = pSurface->GetCamera();
    if (pCamera == NULL)
        return;

    GridService::GridSection* pBottomSection = m_gridSections[0];
    GridService::GridSection* pTopSection = m_gridSections[1];

    if (pCamera->GetViewFrustum().m_bOrtho)
    {
        pBottomSection->MajorLines->SetTranslate(0, 0, 0);
        pBottomSection->MinorLines->SetTranslate(0, 0, 0);
        pTopSection->MajorLines->SetTranslate(0, 0, 0);
        pTopSection->MinorLines->SetTranslate(0, 0, 0);
        pTopSection->MajorLines->SetAppCulled(true);
        pTopSection->MinorLines->SetAppCulled(true);

        pBottomSection->MajorLines->Update(0);
        pBottomSection->MinorLines->Update(0);
        pTopSection->MajorLines->Update(0);
        pTopSection->MinorLines->Update(0);
    }
    else
    {
        const NiPoint3 rayOrigin = pCamera->GetWorldLocation();

        float z_top = 0;
        float z_bottom = 0;

        if (m_useVerticalInterval)
        {
            if (rayOrigin.z >= 0)
            {
                z_bottom = m_verticalInterval * (int)(rayOrigin.z / m_verticalInterval);
                z_top = m_verticalInterval * ((int)(rayOrigin.z / m_verticalInterval) + 1);
            }
            else if (rayOrigin.z < 0)
            {
                z_bottom = m_verticalInterval * ((int)(rayOrigin.z / m_verticalInterval) - 1);
                z_top = m_verticalInterval * (int)(rayOrigin.z / m_verticalInterval);
            }
        }

        float gridOffset = 0.01f * m_spSceneGraphService->GetWorldScale();

        pBottomSection->MajorLines->SetTranslate(0, 0, z_bottom + gridOffset);
        pBottomSection->MinorLines->SetTranslate(0, 0, z_bottom + gridOffset);
        pTopSection->MajorLines->SetTranslate(0, 0, z_top - gridOffset);
        pTopSection->MinorLines->SetTranslate(0, 0, z_top - gridOffset);

        if (z_top == 0 && z_bottom == 0)
        {
            pTopSection->MajorLines->SetAppCulled(true);
            pTopSection->MinorLines->SetAppCulled(true);
        }
        else
        {
            pTopSection->MajorLines->SetAppCulled(false);
            pTopSection->MinorLines->SetAppCulled(false);
        }

        pBottomSection->MajorLines->Update(0);
        pBottomSection->MinorLines->Update(0);
        pTopSection->MajorLines->Update(0);
        pTopSection->MinorLines->Update(0);
    }
}

//------------------------------------------------------------------------------------------------
GridService::GridSection* GridService::CreateGridSection()
{
    NiCloningProcess cloningProcess;
    cloningProcess.m_eCopyType = NiObjectNET::COPY_EXACT;
    cloningProcess.m_eAffectedNodeRelationBehavior = NiCloningProcess::CLONE_RELATION_CLONEDONLY;

    GridSection* pGridSection = EE_EXTERNAL_NEW GridSection();
    pGridSection->MajorLines = (NiMeshPtr)(NiMesh*)m_spMajorLines->Clone(cloningProcess);
    pGridSection->MinorLines = (NiMeshPtr)(NiMesh*)m_spMinorLines->Clone(cloningProcess);

    EE_ASSERT(pGridSection->MajorLines);
    EE_ASSERT(pGridSection->MinorLines);

    efd::vector<NiObjectPtr> objects;
    objects.push_back(NiDynamicCast(NiObject, pGridSection->MinorLines));
    pGridSection->MinorLinesHandle = m_spSceneGraphService->AddSceneGraph(objects, false, true);
    objects[0] = NiDynamicCast(NiObject, pGridSection->MajorLines);
    pGridSection->MajorLinesHandle = m_spSceneGraphService->AddSceneGraph(objects, false, true);

    return pGridSection;
}

//------------------------------------------------------------------------------------------------
