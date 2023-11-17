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
#include "NiTNodeTraversal.h"

#include <ecr/SceneGraphService.h>

#include "ToolSnapService.h"

using namespace efd;
using namespace egf;
using namespace ecr;
using namespace egmToolServices;

EE_IMPLEMENT_CONCRETE_CLASS_INFO(ToolSnapService);

EE_HANDLER(ToolSnapService, OnSceneGraphAdded, ecr::SceneGraphAddedMessage);
EE_HANDLER(ToolSnapService, OnSceneGraphRemoved, ecr::SceneGraphRemovedMessage);

//------------------------------------------------------------------------------------------------
ToolSnapService::ToolSnapService()
{
    // If this default priority is changed, also update the service quick reference documentation
    m_defaultPriority = 2040;

    m_activeNode = NULL;
    m_stickyDistance = 1.0f;
    m_snapPointEnabled = true;
    m_snapping = false;
}

//------------------------------------------------------------------------------------------------
ToolSnapService::~ToolSnapService()
{
    // This method intentionally left blank (all shutdown occurs in OnShutdown)
}

//------------------------------------------------------------------------------------------------
const char* ToolSnapService::GetDisplayName() const
{
    return "ToolSnapService";
}

//------------------------------------------------------------------------------------------------
SyncResult ToolSnapService::OnPreInit(efd::IDependencyRegistrar* pDependencyRegistrar)
{
    EE_UNUSED_ARG(pDependencyRegistrar);

    m_pSceneGraphService = m_pServiceManager->GetSystemServiceAs<SceneGraphService>();
    EE_ASSERT(m_pSceneGraphService);

    m_pMessageService = m_pServiceManager->GetSystemServiceAs<MessageService>();
    EE_ASSERT(m_pMessageService);

    m_pMessageService->Subscribe(this, kCAT_LocalMessage);

    return SyncResult_Success;
}

//------------------------------------------------------------------------------------------------
AsyncResult ToolSnapService::OnShutdown()
{
    m_spSnapPointMesh = NULL;
    m_pMessageService->Unsubscribe(this, kCAT_LocalMessage);

    m_pSceneGraphService = NULL;

    return AsyncResult_Complete;
}
//------------------------------------------------------------------------------------------------
void ToolSnapService::BeginSnap(egf::Entity* pEntity)
{
    if (!m_snapPointEnabled)
        return;

    NiNode* pInputSnapScene = NULL;
    if (!m_scenes.find(pEntity, pInputSnapScene))
        return;

    m_snapping = true;
    m_activeEntity = pEntity;
    m_activeNode = pInputSnapScene;

    if (m_activeNode)
        ShowSnapPoints(m_activeNode, true);
}
//------------------------------------------------------------------------------------------------
void ToolSnapService::EndSnap()
{
    if (!m_snapPointEnabled)
        return;

    ShowAllSnapPoints(false);

    m_snapping = false;
    m_activeEntity = NULL;
    m_activeNode = NULL;
}
//------------------------------------------------------------------------------------------------
bool ToolSnapService::SnapToPoints(
        egf::Entity* pEntity,
        const efd::Point3& startPoint,
        efd::Point3& translate)
{
    if (!m_snapPointEnabled)
        return false;

    NiNode* pInputSnapScene = m_activeNode;
    if (!pInputSnapScene)
        return false;

    const NiBound& inputBound = pInputSnapScene->GetWorldBound();
    NiNode* pParent = pInputSnapScene->GetParent();
    NiPoint3 deltaPos;
    NiPoint3 parentPos;
    if (pParent && pParent->GetParent() == NULL)
    {
        parentPos = pParent->GetTranslate();
        deltaPos = parentPos - (startPoint + translate);
    }
    else
    {
        return false;
    }

    for (EntityToSnapSceneMap::iterator i = m_scenes.begin(); i != m_scenes.end(); ++i)
    {
        egf::Entity* pSnapEntity = i->first;
        NiNode* pSnapScene = i->second;

        if (pSnapEntity == pEntity)
            continue;

        const NiBound& snapBound = pSnapScene->GetWorldBound();
        NiPoint3 vec = inputBound.GetCenter() + deltaPos - snapBound.GetCenter();
        efd::Float32 distance = vec.Length();
        distance = distance - snapBound.GetRadius() - inputBound.GetRadius();

        // Show snap points if they are near
        if (distance - m_stickyDistance*5.0f > 0.0f)
            ShowSnapPoints(pSnapScene, false);
        else
            ShowSnapPoints(pSnapScene, true);

        // If the bounding spheres aren't within the sticky distance of each other, there
        // can be no intersection.
        if (distance - m_stickyDistance > 0.0f)
            continue;

        NiAVObject* pSnapObj = NULL;
        efd::Float32 minDistance = FLT_MAX;
        NiPoint3 minInputPt;
        NiPoint3 minSnapPt;
        NiAVObject* pMinSnapObj = NULL;
        NiAVObject* pMinInputSnapObj = NULL;

        // Find the closest snap point in the scene, as well as the closest
        // snap point on the object
        for (efd::UInt32 ui = 0; ui < pSnapScene->GetArrayCount(); ui++)
        {
            pSnapObj = pSnapScene->GetAt(ui);
            if (!pSnapObj)
                continue;

            NiPoint3 snapObjPt = pSnapObj->GetWorldTranslate();

            for (efd::UInt32 uj = 0; uj < pInputSnapScene->GetArrayCount(); uj++)
            {
                NiAVObject* pInputSnapObj = pInputSnapScene->GetAt(uj);
                if (!pInputSnapObj)
                    continue;

                NiPoint3 inputPt = pInputSnapObj->GetWorldTranslate() + deltaPos;

                efd::Float32 distance = (inputPt - snapObjPt).Length();
                if (distance < minDistance)
                {
                    minDistance = distance;
                    minInputPt = inputPt;
                    minSnapPt = snapObjPt;
                    pMinSnapObj = pSnapObj;
                    pMinInputSnapObj = pInputSnapObj;
                }
            }
        }


        if (minDistance != FLT_MAX && minDistance <= m_stickyDistance)
        {
            NiPoint3 parentPosToInputPt = pMinInputSnapObj->GetWorldTranslate() - parentPos;
            translate = minSnapPt - (startPoint + parentPosToInputPt);
            return true;
        }

    }

    return false;
}

//------------------------------------------------------------------------------------------------
bool ToolSnapService::ClosestSnapToRay(
    egf::Entity* pEntity,
    NiPoint3 origin,
    NiPoint3 direction,
    NiPoint3 &snapPosition)
{
    if (!m_snapPointEnabled)
        return false;

    for (EntityToSnapSceneMap::iterator i = m_scenes.begin(); i != m_scenes.end(); ++i)
    {
        egf::Entity* pSnapEntity = i->first;
        NiNode* pSnapScene = i->second;

        if (pSnapEntity == pEntity)
            continue;

        const NiBound& snapBound = pSnapScene->GetWorldBound();

        // does our ray hit this bound?
        NiPoint3 delta = snapBound.GetCenter() - origin;
        NiPoint3 closestPoint = origin + direction * (delta.Dot(direction));
        efd::Float32 distSqr = (closestPoint - snapBound.GetCenter()).SqrLength();

        efd::Float32 range = snapBound.GetRadius() + m_stickyDistance;
        if (distSqr > range * range)
        {
            ShowSnapPoints(pSnapScene, false);
            continue;
        }

        ShowSnapPoints(pSnapScene, true);

        // now we must test against each snap point within the snap scene
        NiAVObject* pSnapObj = NULL;
        efd::Float32 minDistance = FLT_MAX;
        NiPoint3 minSnapPt;

        // Find the closest snap point in the scene
        for (efd::UInt32 ui = 0; ui < pSnapScene->GetArrayCount(); ui++)
        {
            pSnapObj = pSnapScene->GetAt(ui);
            if (!pSnapObj)
                continue;

            NiPoint3 snapObjPt = pSnapObj->GetWorldTranslate();

            delta = snapObjPt - origin;
            closestPoint = origin + direction * (delta.Dot(direction));
            distSqr = (closestPoint - snapObjPt).SqrLength();

            if (distSqr < minDistance)
            {
                minDistance = distSqr;
                minSnapPt = snapObjPt;
            }
        }

        if (minDistance != FLT_MAX && minDistance <= (m_stickyDistance * m_stickyDistance))
        {
            snapPosition = minSnapPt;
            return true;
        }
    }

    return false;
}

//------------------------------------------------------------------------------------------------
void ToolSnapService::OnSceneGraphAdded(
    const ecr::SceneGraphAddedMessage* pMessage,
    efd::Category targetChannel)
{
    EE_ASSERT(pMessage);

    egf::Entity* pEntity = pMessage->GetEntity();
    if (pEntity != NULL)
    {
        NiAVObject* pScene = m_pSceneGraphService->GetSceneGraphFromEntity(pEntity);
        if (pScene)
        {
            pScene = pScene->GetObjectByName("SnapPoints");
            if (NiIsKindOf(NiNode, pScene))
            {
                m_scenes[pEntity] = (NiNode*)pScene;
                RegisterSnapPoints((NiNode*)pScene);
            }
        }
    }
}

//------------------------------------------------------------------------------------------------
void ToolSnapService::OnSceneGraphRemoved(
    const ecr::SceneGraphRemovedMessage* pMessage,
    efd::Category targetChannel)
{
    EE_ASSERT(pMessage);

    egf::Entity* pEntity = pMessage->GetEntity();
    if (pEntity != NULL)
    {
        m_scenes.erase(pEntity);
    }
}

//------------------------------------------------------------------------------------------------
float ToolSnapService::GetStickinessRadius() const
{
    return m_stickyDistance;
}

//------------------------------------------------------------------------------------------------
class Functor
{
public:
    float m_fScale;

    bool operator()(NiAVObject* pObj)
    {
        if (NiIsKindOf(NiMesh, pObj))
            pObj->SetScale(m_fScale);

        return true;
    }
};

//------------------------------------------------------------------------------------------------
void ToolSnapService::SetStickinessRadius(efd::Float32 radius)
{
    m_stickyDistance = radius;

    Functor kFunctor;
    kFunctor.m_fScale = radius;

    // Force the snap points to match the scene scale
    for (EntityToSnapSceneMap::iterator i = m_scenes.begin(); i != m_scenes.end(); ++i)
    {
        NiNode* pSnapScene = i->second;

        NiTNodeTraversal::DepthFirst_AllObjects(pSnapScene, kFunctor);
        pSnapScene->Update(0.0f);
    }
}

//------------------------------------------------------------------------------------------------
bool ToolSnapService::GetSnapPointEnabled() const
{
    return m_snapPointEnabled;
}

//------------------------------------------------------------------------------------------------
void ToolSnapService::SetSnapPointEnabled(bool enabled)
{
    m_snapPointEnabled = enabled;
}

//------------------------------------------------------------------------------------------------
void ToolSnapService::RegisterSnapPoints(NiNode* pScene)
{
    CreateMasterMesh();

    for (NiUInt32 ui = 0; ui < pScene->GetArrayCount(); ui++)
    {
        NiAVObject* pkObj = pScene->GetAt(ui);
        if (pkObj && NiIsKindOf(NiNode, pkObj))
        {
            NiNode* pkNode = (NiNode*) pkObj;
            NiAVObject* pkClone = (NiAVObject*)m_spSnapPointMesh->Clone();
            pkClone->SetScale(m_stickyDistance);
            pkNode->AttachChild(pkClone);
        }
    }

    pScene->Update(0.0f);
    pScene->UpdateEffects();
    pScene->UpdateProperties();
    ShowSnapPoints(pScene, false);
}

//------------------------------------------------------------------------------------------------
void ToolSnapService::ShowSnapPoints(NiNode* pScene, bool bShow)
{
    pScene->SetAppCulled(!bShow);
}

//------------------------------------------------------------------------------------------------
void ToolSnapService::ShowAllSnapPoints(bool bShow)
{
    for (EntityToSnapSceneMap::iterator i = m_scenes.begin(); i != m_scenes.end(); ++i)
    {
        NiNode* pSnapScene = i->second;
        ShowSnapPoints(pSnapScene, bShow);
    }
}

//------------------------------------------------------------------------------------------------
void ToolSnapService::CreateMasterMesh()
{
    if (NULL != m_spSnapPointMesh)
        return;

    m_spSnapPointMesh = NiNew NiMesh();
    m_spSnapPointMesh->SetPrimitiveType(NiPrimitiveType::PRIMITIVE_LINES);
    m_spSnapPointMesh->SetSubmeshCount(1);

    NiPoint3 akPoints[6];
    akPoints[0] =  NiPoint3::UNIT_X;
    akPoints[1] = -NiPoint3::UNIT_X;
    akPoints[2] =  NiPoint3::UNIT_Y;
    akPoints[3] = -NiPoint3::UNIT_Y;
    akPoints[4] =  NiPoint3::UNIT_Z;
    akPoints[5] = -NiPoint3::UNIT_Z;

    m_spSnapPointMesh->AddStream(NiCommonSemantics::POSITION(), 0,
        NiDataStreamElement::F_FLOAT32_3, 6,
        NiDataStream::ACCESS_CPU_READ | NiDataStream::ACCESS_GPU_READ |
        NiDataStream::ACCESS_CPU_WRITE_STATIC,
        NiDataStream::USAGE_VERTEX, akPoints);
    m_spSnapPointMesh->SetTranslate(NiPoint3::ZERO);
    NiMatrix3 kIdentity;
    kIdentity.MakeIdentity();
    m_spSnapPointMesh->SetRotate(kIdentity);
    m_spSnapPointMesh->SetScale(1.0f);
    NiBound kBound;
    kBound.SetCenterAndRadius(NiPoint3::ZERO, 1.0f);
    m_spSnapPointMesh->SetModelBound(kBound);

    NiMaterialProperty* pkMatProp = NiNew NiMaterialProperty();
    pkMatProp->SetEmittance(NiColor(0.0f, 1.0f, 0.0f));
    m_spSnapPointMesh->AttachProperty(pkMatProp);

    NiVertexColorProperty* pkVCProp = NiNew NiVertexColorProperty();
    pkVCProp->SetLightingMode(NiVertexColorProperty::LIGHTING_E);
    m_spSnapPointMesh->AttachProperty(pkVCProp);

    m_spSnapPointMesh->Update(0.0f);
    m_spSnapPointMesh->UpdateProperties();
    m_spSnapPointMesh->UpdateEffects();
}

//------------------------------------------------------------------------------------------------
