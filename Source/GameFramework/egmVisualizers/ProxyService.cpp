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

#include "egmVisualizersPCH.h"

#include "ProxyService.h"
#include <ecr/RenderService.h>
#include <ecr/SceneGraphService.h>
#include <NiBillboardNode.h>

#include <efd/MessageService.h>
#include <efd/PathUtils.h>
#include <efd/IDs.h>
#include <efd/ServiceManager.h>
#include <egf/EntityManager.h>
#include <NiSourceTexture.h>
#include <NiVertexColorProperty.h>
#include <NiMesh.h>

#include <efd/ecrLogIDs.h>
#include <egf/StandardModelLibraryPropertyIDs.h>
#include <egf/StandardModelLibraryFlatModelIDs.h>

using namespace efd;
using namespace egf;
using namespace ecr;
using namespace egmVisualizers;

EE_IMPLEMENT_CONCRETE_CLASS_INFO(ProxyService);

EE_HANDLER_WRAP(ProxyService, HandleEntityDiscoverMessage, EntityChangeMessage,
                kMSGID_OwnedEntityEnterWorld);

EE_HANDLER_WRAP(ProxyService, HandleEntityRemovedMessage, EntityChangeMessage,
                kMSGID_OwnedEntityExitWorld);

EE_HANDLER_WRAP(ProxyService, HandleEntityUpdatedMessage, EntityChangeMessage,
                kMSGID_OwnedEntityUpdated);

// Register for asset response messages:
// dwest: NOTE: this will not cause *all* AssetLocatorResponse subclasses to be handled.
EE_HANDLER_WRAP(ProxyService, HandleAssetLocatorResponse, AssetLocatorResponse,
                kMSGID_AssetLocatorResponse);


//--------------------------------------------------------------------------------------------------
ProxyService::ProxyService()
: m_asset_lookup_callback(kCAT_INVALID)
{
    // If this default priority is changed, also update the service quick reference documentation
    m_defaultPriority = 2070;
}

//--------------------------------------------------------------------------------------------------
ProxyService::~ProxyService()
{
    // SceneGraphService will clean up outstanding proxies.
}

//--------------------------------------------------------------------------------------------------
const char* ProxyService::GetDisplayName() const
{
    return "ProxyService";
}

//--------------------------------------------------------------------------------------------------
SyncResult ProxyService::OnPreInit(efd::IDependencyRegistrar* pDependencyRegistrar)
{
    pDependencyRegistrar->AddDependency<SceneGraphService>();

    m_pMessageService = m_pServiceManager->GetSystemServiceAs<efd::MessageService>();
    EE_ASSERT(m_pMessageService);

    m_pEntityManager = m_pServiceManager->GetSystemServiceAs<egf::EntityManager>();
    EE_ASSERT(m_pEntityManager);

    m_pSceneGraphService = m_pServiceManager->GetSystemServiceAs<SceneGraphService>();
    EE_ASSERT(m_pSceneGraphService);

    m_pAssetLocator = m_pServiceManager->GetSystemServiceAs<AssetLocatorService>();
    EE_ASSERT(m_pAssetLocator);

    m_pMessageService->Subscribe(this, kCAT_LocalMessage);
    m_asset_lookup_callback = GenerateAssetResponseCategory();
    m_pMessageService->Subscribe(this, m_asset_lookup_callback);

    return SyncResult_Success;
}

//--------------------------------------------------------------------------------------------------
AsyncResult ProxyService::OnShutdown()
{
    if (m_pMessageService != NULL)
    {
        m_pMessageService->Unsubscribe(this, kCAT_LocalMessage);
        m_pMessageService->Unsubscribe(this, m_asset_lookup_callback);
    }

    m_pSceneGraphService = NULL;
    m_proxySceneGraphs.clear();

    return AsyncResult_Complete;
}

//--------------------------------------------------------------------------------------------------
void ProxyService::HandleEntityDiscoverMessage(
    const egf::EntityChangeMessage* pMessage,
    efd::Category targetChannel)
{
    EE_ASSERT(pMessage);

    Entity* pEntity = pMessage->GetEntity();
    EE_ASSERT(pEntity);

    const egf::FlatModel* pModel = pEntity->GetModel();

#if !defined(EE_DISABLE_LOGGING)
    if (pModel->ContainsModel(kFlatModelID_StandardModelLibrary_3DProxy) && 
        pModel->ContainsModel(kFlatModelID_StandardModelLibrary_2DProxy))
    {
         EE_LOG(efd::kGamebryoGeneral0, efd::ILogger::kERR2,
            ("Entity mixes in both 3DProxy and 2DProxy. This is not supported. 3D Proxy"
            " will be used.\n"));
    }
#endif

    // The scene graph service can take over management of 3D proxy geometry since it treats it as
    // as scene graph for the given entity. We just use the proxy service in this case to instruct
    // the scene graph service to begin management in the first place.
    if (pModel->ContainsModel(kFlatModelID_StandardModelLibrary_3DProxy))
    {
        efd::AssetID sceneGraphName;
        if (pEntity->GetPropertyValue(kPropertyID_StandardModelLibrary_ProxyNifAsset,
            sceneGraphName) == PropertyResult_OK)
        {
            m_pSceneGraphService->CreateSceneGraphAssetID(pEntity, sceneGraphName, false, true);
        }
    }
    else if (pModel->ContainsModel(kFlatModelID_StandardModelLibrary_2DProxy))
    {
        // In the case of 2D proxies, the proxy service manages the scene graph references since we
        // create special proxy geometry and perform custom logic in the proxy service.
        efd::AssetID textureName;
        if (pEntity->GetPropertyValue(kPropertyID_StandardModelLibrary_ProxyTextureAsset,
            textureName) == PropertyResult_OK)
        {
            egf::EntityID entityID = pEntity->GetEntityID();
            if (!textureName.IsValidURN())
                return;

            m_pendingAssetLoads[entityID] = textureName;
            m_pAssetLocator->AssetLocate(textureName, m_asset_lookup_callback);
        }
    }
}

//--------------------------------------------------------------------------------------------------
void ProxyService::HandleEntityRemovedMessage(
    const egf::EntityChangeMessage* pMessage,
    efd::Category cat)
{
    EE_UNUSED_ARG(cat);

    Entity* pEntity = pMessage->GetEntity();
    EE_ASSERT(pEntity);

    const egf::FlatModel* pModel = pEntity->GetModel();

    // Remove any registered proxy scene graphs
    if (pModel->ContainsModel(kFlatModelID_StandardModelLibrary_3DProxy))
    {
        m_pSceneGraphService->RemoveSceneGraph(pEntity, true);
    }
    else if (pModel->ContainsModel(kFlatModelID_StandardModelLibrary_2DProxy))
    {
        SceneGraphService::SceneGraphHandle handle;
        if (m_proxySceneGraphs.find(pEntity->GetEntityID(), handle))
        {
            m_proxySceneGraphs.erase(pEntity->GetEntityID());
            m_pSceneGraphService->RemoveSceneGraph(handle);
        }
    }
}

//--------------------------------------------------------------------------------------------------
void ProxyService::HandleEntityUpdatedMessage(
    const egf::EntityChangeMessage* pMessage,
    efd::Category)
{
    Entity* pEntity = pMessage->GetEntity();
    EE_ASSERT(pEntity);

    const egf::FlatModel* pModel = pEntity->GetModel();

    if (pModel->ContainsModel(kFlatModelID_StandardModelLibrary_3DProxy))
    {
        efd::AssetID sceneGraphName;
        if (pEntity->IsDirty(kPropertyID_StandardModelLibrary_ProxyNifAsset) &&
            pEntity->GetPropertyValue(kPropertyID_StandardModelLibrary_ProxyNifAsset,
            sceneGraphName) == PropertyResult_OK)
        {
            m_pSceneGraphService->RecreateSceneGraph(pEntity, sceneGraphName);
        }
    }
    else if (pModel->ContainsModel(kFlatModelID_StandardModelLibrary_2DProxy))
    {
        SceneGraphService::SceneGraphHandle handle = SceneGraphService::kInvalidHandle;
        m_proxySceneGraphs.find(pEntity->GetEntityID(), handle);

        efd::AssetID textureName;
        if (pEntity->IsDirty(kPropertyID_StandardModelLibrary_ProxyTextureAsset) &&
            pEntity->GetPropertyValue(kPropertyID_StandardModelLibrary_ProxyTextureAsset,
            textureName) == PropertyResult_OK)
        {
            bool valid = textureName.IsValidURN();
            if (handle != SceneGraphService::kInvalidHandle && !valid)
            {
                m_proxySceneGraphs.erase(pEntity->GetEntityID());
                m_pSceneGraphService->RemoveSceneGraph(handle);
                return;
            }

            if (valid)
            {
                m_pendingAssetLoads[pEntity->GetEntityID()] = textureName;
                m_pAssetLocator->AssetLocate(textureName, m_asset_lookup_callback);
            }
        }

        if (handle != SceneGraphService::kInvalidHandle)
        {
            NiAVObject* pScene = m_pSceneGraphService->GetSceneGraphFromHandle(handle);
            EE_ASSERT(pScene);
            UpdateProperties(pEntity, pScene);
        }
    }
}
//--------------------------------------------------------------------------------------------------
void ProxyService::HandleAssetLocatorResponse(
    const efd::AssetLocatorResponse* pMessage,
    efd::Category targetChannel)
{
    const utf8string& uri = pMessage->GetResponseURI();

    const AssetLocatorResponse::AssetURLMap& assets = pMessage->GetAssetURLMap();
    AssetLocatorResponse::AssetURLMap::const_iterator it = assets.begin();

    for (; it != assets.end(); ++it)
    {
        utf8string path = it->second.url;

        // Iterate through the pending asset load map
        EntityStringMap::iterator iter = m_pendingAssetLoads.begin();
        while (iter != m_pendingAssetLoads.end())
        {
            EntityStringMap::iterator cur = iter++;

            if (uri != cur->second)
            {
                continue;
            }

            EntityID id = cur->first;
            m_pendingAssetLoads.erase(cur);
            if (!id.IsValid())
                continue;

            Entity* pEntity = m_pEntityManager->LookupEntity(id);
            if (!pEntity)
                continue;

            SceneGraphService::SceneGraphHandle handle;
            if (m_proxySceneGraphs.find(id, handle))
                RecreateSceneGraph(handle, path);
            else
                CreateSceneGraph(pEntity, path);
        }
    }
}
//--------------------------------------------------------------------------------------------------
Bool ProxyService::RecreateSceneGraph(
    SceneGraphService::SceneGraphHandle handle,
    const utf8string& path)
{
    NiSourceTexture* pProxyTexture = NiSourceTexture::Create(NiFixedString(path.c_str()));
    if (!pProxyTexture)
    {
        EE_LOG(efd::kGamebryoGeneral0,
            efd::ILogger::kERR2,
            ("Failed to create texture from '%s'.", path.c_str()));
        return false;
    }

    NiAVObject* pScene = m_pSceneGraphService->GetSceneGraphFromHandle(handle);
    EE_ASSERT(pScene);
    if (!pScene)
    {
        EE_LOG(efd::kGamebryoGeneral0,
            efd::ILogger::kERR2,
            ("Failed to get scene graph handle '%d'.", (UInt32)handle));
        return false;
    }

    NiNode* pNode = NiDynamicCast(NiNode, pScene);
    EE_ASSERT(pNode);

    if (pNode->GetChildCount() != 1)
    {
        EE_LOG(efd::kGamebryoGeneral0, efd::ILogger::kERR2, ("Invalid proxy scenegraph."));
        return false;
    }

    NiMesh* pMesh = NiDynamicCast(NiMesh, pNode->GetAt(0));
    if (!pMesh)
    {
        EE_LOG(efd::kGamebryoGeneral0, efd::ILogger::kERR2, ("Invalid proxy scenegraph."));
        return false;
    }

    NiTexturingProperty* pTex = (NiTexturingProperty*)pMesh->GetProperty(NiProperty::TEXTURING);
    EE_ASSERT(pTex);

    pTex->SetBaseMap(NiNew NiTexturingProperty::Map(pProxyTexture, 0));

    return true;
}
//--------------------------------------------------------------------------------------------------
Bool ProxyService::CreateSceneGraph(Entity* pEntity, const utf8string& path)
{
    EE_ASSERT(pEntity);

    NiSourceTexture* pProxyTexture = NiSourceTexture::Create(NiFixedString(path.c_str()));
    if (!pProxyTexture)
    {
        EE_LOG(efd::kGamebryoGeneral0,
            efd::ILogger::kERR2,
            ("Failed to create texture from '%s'.", path.c_str()));
        return false;
    }

    const efd::UInt32 uiNumVerts = 4;
    const efd::UInt32 uiNumIndices = 6;

    NiPoint3 positions[uiNumVerts];
    positions[0] = NiPoint3(0.5f, -0.5f, 0.0f);
    positions[1] = NiPoint3(0.5f, 0.5f, 0.0f);
    positions[2] = NiPoint3(-0.5f, 0.5f, 0.0f);
    positions[3] = NiPoint3(-0.5f, -0.5f, 0.0f);

    NiPoint2 akTexCoords[uiNumVerts];
    akTexCoords[0] = NiPoint2(1.0f, 1.0f);
    akTexCoords[1] = NiPoint2(1.0f, 0.0f);
    akTexCoords[2] = NiPoint2(0.0f, 0.0f);
    akTexCoords[3] = NiPoint2(0.0f, 1.0f);

    // This array will be shared by all three walls since the connectivity
    // is the same for all three walls
    efd::UInt16 indices[uiNumIndices];
    indices[0] = 0;
    indices[1] = 1;
    indices[2] = 3;
    indices[3] = 1;
    indices[4] = 2;
    indices[5] = 3;

    NiBillboardNode* pProxyBillboard = NiNew NiBillboardNode();
    pProxyBillboard->SetMode(NiBillboardNode::RIGID_FACE_CENTER);

    NiMesh* pProxyMesh = NiNew NiMesh();
    pProxyMesh->SetPrimitiveType(NiPrimitiveType::PRIMITIVE_TRIANGLES);
    EE_ASSERT(pProxyMesh);

    pProxyBillboard->AttachChild(pProxyMesh);

    pProxyMesh->AddStream(NiCommonSemantics::POSITION(), 0,
        NiDataStreamElement::F_FLOAT32_3, uiNumVerts,
        NiDataStream::ACCESS_CPU_READ |
        NiDataStream::ACCESS_CPU_WRITE_STATIC |
        NiDataStream::ACCESS_GPU_READ,
        NiDataStream::USAGE_VERTEX,
        positions);

    pProxyMesh->AddStream(NiCommonSemantics::TEXCOORD(), 0,
        NiDataStreamElement::F_FLOAT32_2, uiNumVerts,
        NiDataStream::ACCESS_CPU_READ |
        NiDataStream::ACCESS_CPU_WRITE_STATIC |
        NiDataStream::ACCESS_GPU_READ,
        NiDataStream::USAGE_VERTEX,
        akTexCoords);

    pProxyMesh->AddStream(NiCommonSemantics::INDEX(), 0,
        NiDataStreamElement::F_UINT16_1, uiNumIndices,
        NiDataStream::ACCESS_CPU_READ |
        NiDataStream::ACCESS_CPU_WRITE_STATIC |
        NiDataStream::ACCESS_GPU_READ,
        NiDataStream::USAGE_VERTEX_INDEX,
        indices);

    NiTexturingProperty* pProxyTexProperty = NiNew NiTexturingProperty();
    pProxyTexProperty->SetApplyMode(NiTexturingProperty::APPLY_REPLACE);
    pProxyTexProperty->SetBaseMap(NiNew NiTexturingProperty::Map(pProxyTexture, 0));
    pProxyTexProperty->SetBaseFilterMode(NiTexturingProperty::FILTER_NEAREST);
    pProxyTexProperty->SetBaseClampMode(NiTexturingProperty::CLAMP_S_CLAMP_T);
    pProxyMesh->AttachProperty(pProxyTexProperty);

    NiMaterialProperty* pProxyMaterial = NiNew NiMaterialProperty();
    pProxyMaterial->SetAmbientColor(NiColor::BLACK);
    pProxyMaterial->SetDiffuseColor(NiColor::BLACK);
    pProxyMaterial->SetSpecularColor(NiColor::BLACK);
    pProxyMaterial->SetEmittance(NiColor::WHITE);
    pProxyMaterial->SetShineness(0.0f);
    pProxyMaterial->SetAlpha(1.0f);
    pProxyMesh->AttachProperty(pProxyMaterial);

    NiVertexColorProperty* pProxyVertexColorProp = NiNew NiVertexColorProperty();
    pProxyVertexColorProp->SetLightingMode(NiVertexColorProperty::LIGHTING_E);
    pProxyMesh->AttachProperty(pProxyVertexColorProp);

    pProxyMesh->RecomputeBounds();
    pProxyMesh->UpdateProperties();
    pProxyMesh->UpdateEffects();
    pProxyMesh->Update(0.0f);

    pProxyBillboard->UpdateProperties();
    pProxyBillboard->UpdateEffects();
    pProxyBillboard->Update(0.0f);

    efd::vector<NiObjectPtr> objects;
    objects.push_back(pProxyBillboard);

    UpdateProperties(pEntity, pProxyBillboard);

    SceneGraphService::SceneGraphHandle handle = SceneGraphService::kInvalidHandle;
    handle = m_pSceneGraphService->AddSceneGraph(objects, true, true);
    m_proxySceneGraphs[pEntity->GetEntityID()] = handle;

    return true;
}
//--------------------------------------------------------------------------------------------------
void ProxyService::UpdateProperties(egf::Entity* pEntity, NiAVObject* pNode)
{
    EE_ASSERT(pEntity);

    // Apply the current entity position to the scenegraph node
    efd::Point3 position;
    if (pEntity->GetPropertyValue(kPropertyID_StandardModelLibrary_Position, position) == PropertyResult_OK)
    {
        pNode->SetTranslate(position.x, position.y, position.z);
    }

    // Apply the current entity rotation to the scenegraph node
    efd::Point3 rotation;
    if (pEntity->GetPropertyValue(kPropertyID_StandardModelLibrary_Rotation, rotation) == PropertyResult_OK)
    {
        NiMatrix3 kXRot, kYRot, kZRot;
        kXRot.MakeXRotation(rotation.x * -EE_DEGREES_TO_RADIANS);
        kYRot.MakeYRotation(rotation.y * -EE_DEGREES_TO_RADIANS);
        kZRot.MakeZRotation(rotation.z * -EE_DEGREES_TO_RADIANS);
        pNode->SetRotate(kXRot * kYRot * kZRot);
    }

    // Apply the current entity scale to the scenegraph node
    float scale;
    if (pEntity->GetPropertyValue(kPropertyID_StandardModelLibrary_Scale, scale) == PropertyResult_OK)
    {
        pNode->SetScale(scale);
    }

    // Apply the current entity visibility to the scenegraph node
    bool isVisible;
    if (pEntity->GetPropertyValue(kPropertyID_StandardModelLibrary_IsVisible, isVisible) == PropertyResult_OK)
    {
        pNode->SetAppCulled(!isVisible);
    }
}
//--------------------------------------------------------------------------------------------------
