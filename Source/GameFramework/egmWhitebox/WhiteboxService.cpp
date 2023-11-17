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

#include "egmWhiteboxPCH.h"

#include <NiMaterialProperty.h>
#include <NiAlphaProperty.h>
#include <efd\EEMath.h>
#include <ecr\CoreRuntimeMessages.h>
#include <egf\StandardModelLibraryPropertyIDs.h>
#include <egf\StandardModelLibraryFlatModelIDs.h>

#include "WhiteboxService.h"

using namespace egmWhitebox;

EE_IMPLEMENT_CONCRETE_CLASS_INFO(WhiteboxService);

EE_HANDLER_WRAP(WhiteboxService, HandleEntityDiscoverMessage,
    egf::EntityChangeMessage, efd::kMSGID_OwnedEntityAdded);
EE_HANDLER_WRAP(WhiteboxService, HandleEntityRemovedMessage,
    egf::EntityChangeMessage, efd::kMSGID_OwnedEntityRemoved);
EE_HANDLER_WRAP(WhiteboxService, HandleEntityUpdatedMessage,
    egf::EntityChangeMessage, efd::kMSGID_OwnedEntityUpdated);

//------------------------------------------------------------------------------------------------
WhiteboxService::WhiteboxService()
    : m_toolMode(false)
    , m_spSceneGraphService(NULL)
    , m_pMessageService(NULL)
    , m_pFlatModelManager(NULL)
    , m_pEntityManager(NULL)
{
    // If this default priority is changed, also update the service quick reference documentation
    m_defaultPriority = 2000;

    // this data tells us what vertex indices are along each axis
    NiUInt16 indexAlignment[ALIGNOFFSET_SIZE] = {
        0, 1, 2, 3, 5, 7, 8, 9, 16, 18, 20, 21, // all indices that have positive x values
        0, 2, 4, 5, 6, 7, 9, 11, 13, 15, 20, 22, // all indices that have positive y values
        0, 1, 4, 5, 8, 9, 10, 11, 12, 13, 16, 17, // all indices that have positive z values
        4, 6, 10, 11, 12, 13, 14, 15, 17, 19, 22, 23, // all indices that have negative x values
        1, 3, 8, 10, 12, 14, 16, 17, 18, 19, 21, 23, // all indices that have negative y values
        2, 3, 6, 7, 14, 15, 18, 19, 20, 21, 22, 23}; // all indices that have negative z values

    efd::Memcpy(m_indexAlignment, indexAlignment, ALIGNOFFSET_SIZE * sizeof(NiUInt16));

    // initialize the snap point offsets
    m_snapMultipliers[0] = efd::Point3::UNIT_ALL;
    m_snapMultipliers[1] = efd::Point3(1.0f, 1.0f, -1.0f);
    m_snapMultipliers[2] = efd::Point3(1.0f, -1.0f, 1.0f);
    m_snapMultipliers[3] = efd::Point3(1.0f, -1.0f, -1.0f);
    m_snapMultipliers[4] = efd::Point3(-1.0f, 1.0f, 1.0f);
    m_snapMultipliers[5] = efd::Point3(-1.0f, 1.0f, -1.0f);
    m_snapMultipliers[6] = efd::Point3(-1.0f, -1.0f, 1.0f);
    m_snapMultipliers[7] = -efd::Point3::UNIT_ALL;

    // initialize our fixed string
    m_strSnapPoints = "SnapPoints";
}

//------------------------------------------------------------------------------------------------
WhiteboxService::~WhiteboxService()
{
}

//------------------------------------------------------------------------------------------------
const char* WhiteboxService::GetDisplayName() const
{
    return "WhiteboxService";
}

//------------------------------------------------------------------------------------------------
efd::SyncResult WhiteboxService::OnPreInit(efd::IDependencyRegistrar* pDependencyRegistrar)
{
    pDependencyRegistrar->AddDependency<ecr::SceneGraphService>();

    m_pMessageService = m_pServiceManager->GetSystemServiceAs<efd::MessageService>();
    EE_ASSERT(m_pMessageService);

    m_spSceneGraphService = m_pServiceManager->GetSystemServiceAs<ecr::SceneGraphService>();
    EE_ASSERT(m_spSceneGraphService);

    m_pFlatModelManager = m_pServiceManager->GetSystemServiceAs<egf::FlatModelManager>();
    EE_ASSERT(m_pFlatModelManager);

    m_pEntityManager = m_pServiceManager->GetSystemServiceAs<egf::EntityManager>();
    EE_ASSERT(m_pEntityManager);

    RegisterForEntityMessages();

    return efd::SyncResult_Success;
}

//------------------------------------------------------------------------------------------------
efd::AsyncResult WhiteboxService::OnShutdown()
{
    m_pMessageService->Unsubscribe(this, efd::kCAT_LocalMessage);

    m_pMessageService = NULL;
    m_spSceneGraphService = NULL;
    m_pFlatModelManager = NULL;
    m_pEntityManager = NULL;

    return efd::AsyncResult_Complete;
}

//------------------------------------------------------------------------------------------------
void WhiteboxService::RegisterForEntityMessages()
{
    m_pMessageService->Subscribe(this, efd::kCAT_LocalMessage);
}

//------------------------------------------------------------------------------------------------
void WhiteboxService::HandleEntityDiscoverMessage(const egf::EntityChangeMessage* pMessage,
    efd::Category targetChannel)
{
    EE_ASSERT(pMessage);
    egf::Entity* pEntity = pMessage->GetEntity();
    EE_ASSERT(pEntity);

    // confirm that this entity mixes in Whitebox
    const egf::FlatModel* pFlat = pEntity->GetModel();
    if (!pFlat->ContainsModel(egf::kFlatModelID_StandardModelLibrary_Whitebox))
        return;

    // create a box and register it with the scenegraph service
    CreateAndRegisterSceneGraph(pEntity);
}

//------------------------------------------------------------------------------------------------
void WhiteboxService::HandleEntityRemovedMessage(
    const egf::EntityChangeMessage* pMessage,
    efd::Category targetChannel)
{
    EE_ASSERT(pMessage);

    egf::Entity* pEntity = pMessage->GetEntity();
    EE_ASSERT(pEntity);

    // do not consider entities that aren't whiteboxes
    const egf::FlatModel* pFlat = pEntity->GetModel();
    if (!pFlat->ContainsModel(egf::kFlatModelID_StandardModelLibrary_Whitebox))
        return;

    // since the scenegraph service considers our scenegraph external, we must force its removal
    m_spSceneGraphService->RemoveSceneGraph(pEntity, true);
}

//------------------------------------------------------------------------------------------------
void WhiteboxService::HandleEntityUpdatedMessage(
    const egf::EntityChangeMessage* pMessage,
    efd::Category targetChannel)
{
    EE_ASSERT(pMessage);

    egf::Entity* pEntity = pMessage->GetEntity();
    EE_ASSERT(pEntity);

    // do not consider entities that aren't whiteboxes
    const egf::FlatModel* pFlat = pEntity->GetModel();
    if (!pFlat->ContainsModel(egf::kFlatModelID_StandardModelLibrary_Whitebox))
        return;

    // retrieve the scene root pointer from the SceneGraph Service
    NiAVObject* pRoot = m_spSceneGraphService->GetSceneGraphFromEntity(pEntity);

    // reconstruct the whitebox using current entity properties
    if (pRoot != NULL)
        UpdateWhiteboxes(pEntity, pRoot);
}

//------------------------------------------------------------------------------------------------
void WhiteboxService::SetInToolMode(bool inToolMode)
{
    m_toolMode = inToolMode;
}

//------------------------------------------------------------------------------------------------
void WhiteboxService::CreateAndRegisterSceneGraph(egf::Entity *pEntity)
{
    // construct a new scenegraph containing an NiMesh
    NiMesh* pMesh = NiNew NiMesh();
    SetupWhiteboxDatastreams(pMesh);
    SetupWhiteboxMaterial(pMesh);
    NiNode* pRoot = NiNew NiNode();
    pRoot->AttachChild(pMesh);
    pRoot->UpdateProperties();
    SetupSnapPoints(pRoot);

    // apply entity properties to our whitebox
    UpdateWhiteboxes(pEntity, pRoot, true);

    // register our scene with the scenegraph service
    efd::vector<NiObjectPtr> objects;
    objects.push_back(NiDynamicCast(NiObject, pRoot));
    // we will be updating this ourself
    m_spSceneGraphService->CreateExternalSceneGraph(pEntity, objects, false);
}

//------------------------------------------------------------------------------------------------
void WhiteboxService::UpdateWhiteboxes(egf::Entity *pEntity, NiAVObject *pRoot, bool init)
{
    // determine what properties we care about have changed
    bool updateTransform = false;
    updateTransform = pEntity->IsDirty(egf::kPropertyID_StandardModelLibrary_Position);
    updateTransform |= pEntity->IsDirty(egf::kPropertyID_StandardModelLibrary_Rotation);
    updateTransform |= pEntity->IsDirty(egf::kPropertyID_StandardModelLibrary_Scale);

    if (updateTransform || init)
    {
        // get the entity properties that contribute to the geometry
        efd::Point3 position;
        efd::Point3 rotation;
        efd::Float32 scale;
        egf::PropertyResult result = pEntity->GetPropertyValue<efd::Point3>(
            egf::kPropertyID_StandardModelLibrary_Position, position);
        EE_ASSERT(result == egf::PropertyResult_OK);
        result = pEntity->GetPropertyValue(
            egf::kPropertyID_StandardModelLibrary_Rotation, rotation);
        NiMatrix3 rotMatrix;
        rotation *= -efd::EE_DEGREES_TO_RADIANS;
        rotMatrix.FromEulerAnglesXYZ(rotation.x, rotation.y, rotation.z);
        EE_ASSERT(result == egf::PropertyResult_OK);
        result = pEntity->GetPropertyValue(
            egf::kPropertyID_StandardModelLibrary_Scale, scale);
        EE_ASSERT(result == egf::PropertyResult_OK);
        // update the root node's transform
        pRoot->SetTranslate(position);
        pRoot->SetRotate(rotMatrix);
        pRoot->SetScale(scale);
    }
    // do we need to toggle snap points
    bool updateSnap = pEntity->IsDirty(egf::kPropertyID_StandardModelLibrary_SnapPoints);
    bool snapAdded = false;
    if (updateSnap || init)
    {
        EE_VERIFY(pEntity->GetPropertyValue<efd::Bool>(
            egf::kPropertyID_StandardModelLibrary_SnapPoints, snapAdded) == egf::PropertyResult_OK);
        // only create snap points in tool mode
        snapAdded = snapAdded && m_toolMode;
        SetupSnapPoints(pRoot, snapAdded);
        // let the tool snap service know the scene structure has changed
        if (!init)
            SendUpdateMessages(pEntity);
    }
    // update each mesh and snap point in our scenegraph with the new dimensions
    bool updateMesh = pEntity->IsDirty(egf::kPropertyID_StandardModelLibrary_ShapeDimensions);
    if (updateMesh || snapAdded || init)
    {
        efd::Point3 dimensions;
        EE_VERIFY(pEntity->GetPropertyValue<efd::Point3>(
            egf::kPropertyID_StandardModelLibrary_ShapeDimensions, dimensions) ==
            egf::PropertyResult_OK);
        UpdateMeshesRecursive(dimensions, pRoot);
    }
    // update our material property
    bool updateMaterial = pEntity->IsDirty(egf::kPropertyID_StandardModelLibrary_Diffuse);
    if (updateMaterial || init)
    {
        efd::ColorA diffuse;
        EE_VERIFY(pEntity->GetPropertyValue<efd::ColorA>(
            egf::kPropertyID_StandardModelLibrary_Diffuse, diffuse) == egf::PropertyResult_OK);
        UpdateMaterialsRecursive(diffuse, pRoot);
    }
    // if we are in a running game and runtime visibility is off, set the appculled flag
    bool updateRenderVisibility = pEntity->IsDirty(
        egf::kPropertyID_StandardModelLibrary_RuntimeRender);
    if (updateRenderVisibility || init)
    {
        efd::Bool visible;
        EE_VERIFY(pEntity->GetPropertyValue<efd::Bool>(
            egf::kPropertyID_StandardModelLibrary_RuntimeRender, visible) ==
            egf::PropertyResult_OK);

        NiNode* pNode = NiDynamicCast(NiNode, pRoot);

        // Runtime Render is set the children of the root so it does not conflict with
        // IsVisible property
        if (pNode)
        {
            for (unsigned int i = 0; i < pNode->GetChildCount(); i++)
            {
                pNode->GetAt(i)->SetAppCulled(!visible && !m_toolMode);
            }
        }
    }

    bool updateVisibility = pEntity->IsDirty(egf::kPropertyID_StandardModelLibrary_IsVisible);
    if (updateVisibility || init)
    {
        efd::Bool visible;
        EE_VERIFY(pEntity->GetPropertyValue<efd::Bool>(
            egf::kPropertyID_StandardModelLibrary_IsVisible, visible) == egf::PropertyResult_OK);

        pRoot->SetAppCulled(!visible);
    }
    // determine if we need to use the walkability naming convention
    bool updateWalkability = pEntity->IsDirty(egf::kPropertyID_StandardModelLibrary_Walkable);
    if (updateWalkability || init)
    {
        efd::Bool walkable;
        EE_VERIFY(pEntity->GetPropertyValue<efd::Bool>(
            egf::kPropertyID_StandardModelLibrary_Walkable, walkable) == egf::PropertyResult_OK);
        if (walkable)
            pRoot->SetName("Walkable");
        else
            pRoot->SetName("");
        // let the walkable service know we've changed the scenegraph
        if (!init)
            SendUpdateMessages(pEntity);
    }
    if (updateMesh || updateTransform || updateMaterial || updateRenderVisibility || updateSnap ||
        updateWalkability || updateVisibility || init)
    {
        pRoot->Update(0.0f);
    }

    return;
}

//------------------------------------------------------------------------------------------------
void WhiteboxService::UpdateMeshesRecursive(const efd::Point3& dimensions, NiAVObject* pObject)
{
    if (NiIsKindOf(NiNode, pObject))
    {
        NiNode* pNode = (NiNode*)pObject;
        // if this the snap point parent, update children but don't recurse
        if (pNode->GetName() == m_strSnapPoints)
        {
            // ensure we have exactly one child per box corner
            EE_ASSERT(pNode->GetChildCount() == 8);
            EE_ASSERT(pNode->GetArrayCount() == 8);

            for (unsigned int ui = 0; ui < pNode->GetArrayCount(); ui++)
            {
                NiAVObject* pChild = pNode->GetAt(ui);
                pChild->SetTranslate(
                    efd::Point3::ComponentProduct(dimensions, m_snapMultipliers[ui]) * 0.5f);
            }
            return;
        }
        // if it's just a normal node, recurse over children
        for (unsigned int i = 0; i < pNode->GetArrayCount(); i++)
        {
            NiAVObject* pChild = pNode->GetAt(i);
            UpdateMeshesRecursive(dimensions, pChild);
        }
    }
    else if (NiIsKindOf(NiMesh, pObject))
    {
        NiMesh* pMesh = (NiMesh*)pObject;
        // get an iterator to the mesh's position data
        NiDataStreamElementLock posLock(
            pMesh,
            NiCommonSemantics::POSITION(),
            0,
            NiDataStreamElement::F_FLOAT32_3);
        EE_ASSERT(posLock.IsLocked());
        NiTStridedRandomAccessIterator<NiPoint3> posIter = posLock.begin<NiPoint3>(0);

        // modify dimensions to represent the postive delta from the origin and non-zero
        efd::Point3 halfDimensions = dimensions;
        halfDimensions.x = efd::Max(efd::Abs(dimensions.x / 2.0f), 0.01f);
        halfDimensions.y = efd::Max(efd::Abs(dimensions.y / 2.0f), 0.01f);
        halfDimensions.z = efd::Max(efd::Abs(dimensions.z / 2.0f), 0.01f);

        // set new positions based off of dimensions
        for (int i = ALIGNOFFSET_POS_X; i < ALIGNOFFSET_POS_Y; i++)
        {
            posIter[m_indexAlignment[i]].x = halfDimensions.x;
        }
        for (int i = ALIGNOFFSET_POS_Y; i < ALIGNOFFSET_POS_Z; i++)
        {
            posIter[m_indexAlignment[i]].y = halfDimensions.y;
        }
        for (int i = ALIGNOFFSET_POS_Z; i < ALIGNOFFSET_NEG_X; i++)
        {
            posIter[m_indexAlignment[i]].z = halfDimensions.z;
        }
        for (int i = ALIGNOFFSET_NEG_X; i < ALIGNOFFSET_NEG_Y; i++)
        {
            posIter[m_indexAlignment[i]].x = -halfDimensions.x;
        }
        for (int i = ALIGNOFFSET_NEG_Y; i < ALIGNOFFSET_NEG_Z; i++)
        {
            posIter[m_indexAlignment[i]].y = -halfDimensions.y;
        }
        for (int i = ALIGNOFFSET_NEG_Z; i < ALIGNOFFSET_SIZE; i++)
        {
            posIter[m_indexAlignment[i]].z = -halfDimensions.z;
        }

        // update the mesh's bounds
        pMesh->RecomputeBounds();
    }

    return;
}

//------------------------------------------------------------------------------------------------
void WhiteboxService::UpdateMaterialsRecursive(const efd::ColorA& diffuse, NiAVObject *pObject)
{
    if (NiIsKindOf(NiNode, pObject))
    {
        NiNode* pNode = (NiNode*)pObject;
        for (unsigned int i = 0; i < pNode->GetArrayCount(); i++)
        {
            NiAVObject* pChild = pNode->GetAt(i);
            UpdateMaterialsRecursive(diffuse, pChild);
        }
    }
    else if (NiIsKindOf(NiMesh, pObject))
    {
        NiMaterialProperty* pMaterial = NiDynamicCast(NiMaterialProperty,
            pObject->GetProperty(NiProperty::MATERIAL));
        NiAlphaProperty* pAlpha = NiDynamicCast(NiAlphaProperty,
            pObject->GetProperty(NiProperty::ALPHA));
        if (pMaterial)
        {
            NiColor diffuseMat = NiColor(diffuse.r, diffuse.g, diffuse.b);
            pMaterial->SetDiffuseColor(diffuseMat);
            pMaterial->SetAlpha(diffuse.a);
        }
        if (pAlpha)
            pAlpha->SetAlphaBlending(diffuse.a < 1.0f);
    }
}

//------------------------------------------------------------------------------------------------
void WhiteboxService::SetupWhiteboxDatastreams(NiMesh* pMesh)
{
    EE_ASSERT(pMesh);

    pMesh->SetPrimitiveType(NiPrimitiveType::PRIMITIVE_TRIANGLES);
    pMesh->SetSubmeshCount(1);

    // add position data stream
    NiDataStreamElementLock posLock = pMesh->AddStreamGetLock(
        NiCommonSemantics::POSITION(),
        0,
        NiDataStreamElement::F_FLOAT32_3,
        24,
        NiDataStream::ACCESS_CPU_READ | NiDataStream::ACCESS_CPU_WRITE_MUTABLE |
        NiDataStream::ACCESS_GPU_READ,
        NiDataStream::USAGE_VERTEX);
    EE_ASSERT(posLock.IsLocked());
    // add normal data stream
    NiDataStreamElementLock normLock = pMesh->AddStreamGetLock(
        NiCommonSemantics::NORMAL(),
        0,
        NiDataStreamElement::F_FLOAT32_3,
        24,
        NiDataStream::ACCESS_CPU_WRITE_STATIC | NiDataStream::ACCESS_GPU_READ,
        NiDataStream::USAGE_VERTEX);
    EE_ASSERT(normLock.IsLocked());
    // add index data stream
    NiDataStreamElementLock indexLock = pMesh->AddStreamGetLock(
        NiCommonSemantics::INDEX(),
        0,
        NiDataStreamElement::F_UINT16_1,
        36,
        NiDataStream::ACCESS_CPU_READ | NiDataStream::ACCESS_CPU_WRITE_STATIC |
        NiDataStream::ACCESS_GPU_READ,
        NiDataStream::USAGE_VERTEX_INDEX);
    EE_ASSERT(indexLock.IsLocked());

    NiTStridedRandomAccessIterator<NiPoint3> posIter = posLock.begin<NiPoint3>(0);
    NiTStridedRandomAccessIterator<NiPoint3> normIter = normLock.begin<NiPoint3>(0);
    NiTStridedRandomAccessIterator<NiUInt16> indexIter = indexLock.begin<NiUInt16>(0);

    // positive x face
    posIter[0] = NiPoint3(1.0f, 1.0f, 1.0f);
    normIter[0] = NiPoint3::UNIT_X;
    posIter[1] = NiPoint3(1.0f, -1.0f, 1.0f);
    normIter[1] = NiPoint3::UNIT_X;
    posIter[2] = NiPoint3(1.0f, 1.0f, -1.0f);
    normIter[2] = NiPoint3::UNIT_X;
    posIter[3] = NiPoint3(1.0f, -1.0f, -1.0f);
    normIter[3] = NiPoint3::UNIT_X;
    indexIter[0] = 0;
    indexIter[1] = 1;
    indexIter[2] = 2;
    indexIter[3] = 1;
    indexIter[4] = 3;
    indexIter[5] = 2;
    // postitive y face
    posIter[4] = NiPoint3(1.0f, 1.0f, 1.0f);
    normIter[4] = NiPoint3::UNIT_Y;
    posIter[5] = NiPoint3(-1.0f, 1.0f, 1.0f);
    normIter[5] = NiPoint3::UNIT_Y;
    posIter[6] = NiPoint3(1.0f, 1.0f, -1.0f);
    normIter[6] = NiPoint3::UNIT_Y;
    posIter[7] = NiPoint3(-1.0f, 1.0f, -1.0f);
    normIter[7] = NiPoint3::UNIT_Y;
    indexIter[6] = 4;
    indexIter[7] = 5;
    indexIter[8] = 6;
    indexIter[9] = 5;
    indexIter[10] = 7;
    indexIter[11] = 6;
    // positive z face
    posIter[8] = NiPoint3(1.0f, -1.0f, 1.0f);
    normIter[8] = NiPoint3::UNIT_Z;
    posIter[9] = NiPoint3(1.0f, 1.0f, 1.0f);
    normIter[9] = NiPoint3::UNIT_Z;
    posIter[10] = NiPoint3(-1.0f, -1.0f, 1.0f);
    normIter[10] = NiPoint3::UNIT_Z;
    posIter[11] = NiPoint3(-1.0f, 1.0f, 1.0f);
    normIter[11] = NiPoint3::UNIT_Z;
    indexIter[12] = 8;
    indexIter[13] = 9;
    indexIter[14] = 10;
    indexIter[15] = 9;
    indexIter[16] = 11;
    indexIter[17] = 10;
    // negative x face
    posIter[12] = NiPoint3(-1.0f, -1.0f, 1.0f);
    normIter[12] = -NiPoint3::UNIT_X;
    posIter[13] = NiPoint3(-1.0f, 1.0f, 1.0f);
    normIter[13] = -NiPoint3::UNIT_X;
    posIter[14] = NiPoint3(-1.0f, -1.0f, -1.0f);
    normIter[14] = -NiPoint3::UNIT_X;
    posIter[15] = NiPoint3(-1.0f, 1.0f, -1.0f);
    normIter[15] = -NiPoint3::UNIT_X;
    indexIter[18] = 12;
    indexIter[19] = 13;
    indexIter[20] = 14;
    indexIter[21] = 13;
    indexIter[22] = 15;
    indexIter[23] = 14;
    // negative y face
    posIter[16] = NiPoint3(-1.0f, -1.0f, 1.0f);
    normIter[16] = -NiPoint3::UNIT_Y;
    posIter[17] = NiPoint3(1.0f, -1.0f, 1.0f);
    normIter[17] = -NiPoint3::UNIT_Y;
    posIter[18] = NiPoint3(-1.0f, -1.0f, -1.0f);
    normIter[18] = -NiPoint3::UNIT_Y;
    posIter[19] = NiPoint3(1.0f, -1.0f, -1.0f);
    normIter[19] = -NiPoint3::UNIT_Y;
    indexIter[24] = 16;
    indexIter[25] = 17;
    indexIter[26] = 18;
    indexIter[27] = 17;
    indexIter[28] = 19;
    indexIter[29] = 18;
    // negative z face
    posIter[20] = NiPoint3(1.0f, 1.0f, -1.0f);
    normIter[20] = -NiPoint3::UNIT_Z;
    posIter[21] = NiPoint3(1.0f, -1.0f, -1.0f);
    normIter[21] = -NiPoint3::UNIT_Z;
    posIter[22] = NiPoint3(-1.0f, 1.0f, -1.0f);
    normIter[22] = -NiPoint3::UNIT_Z;
    posIter[23] = NiPoint3(-1.0f, -1.0f, -1.0f);
    normIter[23] = -NiPoint3::UNIT_Z;
    indexIter[30] = 20;
    indexIter[31] = 21;
    indexIter[32] = 22;
    indexIter[33] = 21;
    indexIter[34] = 23;
    indexIter[35] = 22;
}

//------------------------------------------------------------------------------------------------
void WhiteboxService::SetupWhiteboxMaterial(NiMesh* pMesh)
{
    NiMaterialProperty* pMaterial = NiNew NiMaterialProperty();
    pMaterial->SetAmbientColor(NiColor::WHITE);
    pMaterial->SetDiffuseColor(NiColor::WHITE);
    pMaterial->SetSpecularColor(NiColor::WHITE);
    pMaterial->SetEmittance(NiColor::BLACK);
    pMaterial->SetShineness(0.0f);
    pMaterial->SetAlpha(1.0f);

    NiAlphaProperty* pAlpha = NiNew NiAlphaProperty();
    pAlpha->SetAlphaBlending(false);

    pMesh->AttachProperty(pMaterial);
    pMesh->AttachProperty(pAlpha);
}

//------------------------------------------------------------------------------------------------
void WhiteboxService::SetupSnapPoints(NiAVObject *pRoot, bool add)
{
    if (add)
    {
        NiNode* pRootNode = NiDynamicCast(NiNode, pRoot);
        EE_ASSERT(pRootNode);
        // add a node named "SnapPoints" below the root
        NiNode* pSnapRoot = NiNew NiNode(8);
        pSnapRoot->SetName(m_strSnapPoints);

        // add a child for each corner, initial transforms will be updated in updatemeshesrecursive
        for (unsigned int ui = 0; ui < 8; ui++)
        {
            pSnapRoot->AttachChild(NiNew NiNode());
        }
        pRootNode->AttachChild(pSnapRoot);
    }
    else
    {
        // recursively find any nodes named "SnapPoints" and remove them from the scenegraph
        if (pRoot->GetName() == m_strSnapPoints)
        {
            NiNode* pParent = pRoot->GetParent();
            EE_ASSERT(pParent);
            pParent->DetachChild(pRoot);
        }
        else if (NiIsKindOf(NiNode, pRoot))
        {
            NiNode* pNode = (NiNode*)pRoot;
            for (unsigned int ui = 0; ui < pNode->GetArrayCount(); ui++)
            {
                NiAVObject* pChild = pNode->GetAt(ui);
                if (pChild)
                    SetupSnapPoints(pChild, add);
            }
        }
    }
}

//------------------------------------------------------------------------------------------------
void WhiteboxService::SendUpdateMessages(egf::Entity *pEntity)
{
    // other services only expect structural scenegraph changes when scenes were completely reloaded
    // so we trick them into thinking that this one is different
    ecr::SceneGraphRemovedMessagePtr spRemoveMessage =
        EE_NEW ecr::SceneGraphRemovedMessage(pEntity);
    m_pMessageService->SendImmediate(spRemoveMessage);
    ecr::SceneGraphAddedMessagePtr spAddMessage = EE_NEW ecr::SceneGraphAddedMessage(pEntity);
    m_pMessageService->SendImmediate(spAddMessage);
}

//------------------------------------------------------------------------------------------------
