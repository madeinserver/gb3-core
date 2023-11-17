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

// Pre-compiled header
#include "egmTerrainPCH.h"

#include <efd/ServiceManager.h>
#include <efd/ecrLogIDs.h>

#include "TerrainService.h"

#include <efd/ConfigManager.h>
#include <efd/IDs.h>
#include <efd/MessageService.h>
#include <efd/PathUtils.h>
#include <efd/TimeType.h>

#include <efd/AssetLocatorService.h>

#include <egf/EntityManager.h>

#include <egf/StandardModelLibraryPropertyIDs.h>
#include <egf/StandardModelLibraryFlatModelIDs.h>

#include <ecr/SceneGraphService.h>
#include <ecr/PickService.h>

#include <egf/egfLogIDs.h>

#include <NiTerrain.h>
#include <NiTerrainCullingProcess.h>
#include <NiTerrainSectorSelectorDefault.h>

using namespace efd;
using namespace egf;
using namespace ecr;
using namespace egmTerrain;

//------------------------------------------------------------------------------------------------
EE_IMPLEMENT_CONCRETE_CLASS_INFO(TerrainService);
EE_IMPLEMENT_CONCRETE_CLASS_INFO(TerrainService::AssetResolver);
//------------------------------------------------------------------------------------------------
// Register for changes to locally owned entities:
EE_HANDLER_WRAP(
    TerrainService,
    HandleEntityDiscoverMessage,
    egf::EntityChangeMessage,
    kMSGID_OwnedEntityAdded);
EE_HANDLER_WRAP(
    TerrainService,
    HandleEntityUpdatedMessage,
    egf::EntityChangeMessage,
    kMSGID_OwnedEntityUpdated);
EE_HANDLER_WRAP(
    TerrainService,
    HandleEntityRemovedMessage,
    egf::EntityChangeMessage,
    kMSGID_OwnedEntityRemoved);
EE_HANDLER_WRAP(
    TerrainService,
    HandleEntityEnterWorldMessage,
    egf::EntityChangeMessage,
    kMSGID_OwnedEntityEnterWorld);
EE_HANDLER_WRAP(
    TerrainService,
    HandleEntityExitWorldMessage,
    egf::EntityChangeMessage,
    kMSGID_OwnedEntityExitWorld);

// Register for asset change related messages:
EE_HANDLER_WRAP(
    TerrainService,
    HandleAssetLocatorResponse,
    efd::AssetLocatorResponse,
    kMSGID_AssetLocatorResponse);

// Register for asset change related messages:
EE_HANDLER_WRAP(
    TerrainService::AssetResolver, 
    HandleAssetLocatorResponse, 
    AssetLocatorResponse, 
    kMSGID_AssetLocatorResponse);

// Register for entity preload messages
EE_HANDLER(TerrainService, HandlePreloadRequest, EntityPreloadRequest);

//------------------------------------------------------------------------------------------------
TerrainService::TerrainService() :
    m_pMessageService(NULL),
    m_pEntityManager(NULL),
    m_pSceneGraphService(NULL),
    m_handleAssetChanges(true)
{
    // Initialize the load categories
    m_assetPreloadRequestCategory = kCAT_INVALID;
    m_assetPreloadResponseCategory = kCAT_INVALID;
    for (efd::UInt32 index = 0; index < AT_NumAssetTypes; ++index)
    {
        m_loadCategory[index] = kCAT_INVALID;
        m_refreshCategory[index] = kCAT_INVALID;
    }

    // Setup the tags that are used for all the asset types
    m_assetTags[AT_Terrain] = "gamebryo-terrain";
    m_assetTags[AT_TerrainSector] = "gamebryo-terrain-sector";
    m_assetTags[AT_MaterialPackage] = "gamebryo-terrain-materialpkg";
    m_assetTags[AT_SurfaceTexture] = "image";

    // If this default priority is changed, also update the service quick reference documentation
    m_defaultPriority = 1700;
}

//------------------------------------------------------------------------------------------------
TerrainService::~TerrainService()
{
}

//------------------------------------------------------------------------------------------------
const char* TerrainService::GetDisplayName() const
{
    return "TerrainService";
}

//------------------------------------------------------------------------------------------------
SyncResult TerrainService::OnPreInit(efd::IDependencyRegistrar* pDependencyRegistrar)
{
    pDependencyRegistrar->AddDependency<SceneGraphService>();

    // Create an asset resolver object
    m_spAssetResolver = NiNew AssetResolver(m_pServiceManager);
    // Create a surface library
    m_spSurfaceLibrary = NiNew NiTerrainSurfaceLibrary(m_spAssetResolver);
    m_spSurfaceLibrary->AttachObserver(this, &TerrainService::OnSurfacePackageUpdated);

    // Fetch the message service and subscribe to required messages
    m_pMessageService = m_pServiceManager->GetSystemServiceAs<efd::MessageService>();
    EE_ASSERT(m_pMessageService);
    m_pMessageService->Subscribe(this, kCAT_LocalMessage);

    // Figure out a category for messages to do with our asset loads to be returned on
    for (efd::UInt32 index = 0; index < AT_NumAssetTypes; ++index)
    {
        m_loadCategory[index] = GenerateAssetResponseCategory();
        m_pMessageService->Subscribe(this, m_loadCategory[index]);

        m_refreshCategory[index] = GenerateAssetResponseCategory();
        m_pMessageService->Subscribe(this, m_refreshCategory[index]);
    }

    // Fetch the frequently used services
    m_pSceneGraphService = m_pServiceManager->GetSystemServiceAs<SceneGraphService>();
    m_pEntityManager = m_pServiceManager->GetSystemServiceAs<egf::EntityManager>();

    // Check for an entity manager
    EE_ASSERT(m_pEntityManager);
    if (!m_pEntityManager)
        return SyncResult_Failure;

    // Setup preload service
    m_assetPreloadRequestCategory = m_pEntityManager->GetEntityPreloadCategory();
    m_pMessageService->Subscribe(this, m_assetPreloadRequestCategory);
    m_assetPreloadResponseCategory = m_pMessageService->GetGloballyUniqueCategory();

    return SyncResult_Success;
}

//------------------------------------------------------------------------------------------------
AsyncResult TerrainService::OnInit()
{
    // Initialize global terrain objects
    m_spAssetResolver->Initialize();

    // Fetch the frequently used services
    m_pEntityManager->RegisterPreloadService(m_assetPreloadResponseCategory);

    // Set up a callback to the reload service so we know when terrain related files are changed on
    // disk. This is to facilitate rapid iteration on Terrain assets.
    ReloadManager* pReloadMgr = m_pServiceManager->GetSystemServiceAs<ReloadManager>();
    if (pReloadMgr && m_handleAssetChanges)
    {
        pReloadMgr->RegisterAssetChangeHandler(m_assetTags[AT_Terrain], this);
        pReloadMgr->RegisterAssetChangeHandler(m_assetTags[AT_TerrainSector], this);
        pReloadMgr->RegisterAssetChangeHandler(m_assetTags[AT_MaterialPackage], this);
    }

    return AsyncResult_Complete;
}

//------------------------------------------------------------------------------------------------
AsyncResult TerrainService::OnTick()
{
    // Update the terrain scene graphs
    UpdateTerrainEntities();

    // Check for recently loaded terrains and send out the preload responses
    CheckPreloadStatus();

    return AsyncResult_Pending;
}

//------------------------------------------------------------------------------------------------
AsyncResult TerrainService::OnShutdown()
{
    // Remove all tasks and terrains from the service
    ClearLoadedData();

    // Remove references to the surface library and asset resolver
    m_spSurfaceLibrary = 0;
    m_spAssetResolver->Shutdown();
    m_spAssetResolver = 0;

    // Unsubscribe from the message service
    if (m_pMessageService != NULL)
    {
        m_pMessageService->Unsubscribe(this, kCAT_LocalMessage);
        m_pMessageService->Unsubscribe(this, m_assetPreloadRequestCategory);

        for (efd::UInt32 index = 0; index < AT_NumAssetTypes; ++index)
        {
            m_pMessageService->Unsubscribe(this, m_loadCategory[index]);
            m_pMessageService->Unsubscribe(this, m_refreshCategory[index]);
        }
    }

    return AsyncResult_Complete;
}

//-------------------------------------------------------------------------------------------------
NiTerrain* TerrainService::GetTerrainForEntity(egf::EntityID entityID)
{
    if (m_entityToTerrainMap.find(entityID) == m_entityToTerrainMap.end())
        return NULL;

    return m_entityToTerrainMap[entityID];
}

//------------------------------------------------------------------------------------------------
NiTerrain* TerrainService::GetTerrainForEntity(egf::Entity* pEntity)
{
    EE_ASSERT(pEntity != NULL);

    return GetTerrainForEntity(pEntity->GetEntityID());
}

//------------------------------------------------------------------------------------------------
void TerrainService::ConfigureShadowsForTerrain(egf::Entity* pEntity)
{
    // Configure the shadowing system for this terrain (change the culling process properties)
    efd::UInt32 shadowLOD = 0;
    if (pEntity->GetPropertyValue(kPropertyID_StandardModelLibrary_TerrainShadowLOD, shadowLOD) ==
        PropertyResult_OK)
    {
        if (NiShadowManager::GetShadowManager())
        {
            NiTerrainCullingProcess* pkTerrainCullingProcess =
                NiDynamicCast(NiTerrainCullingProcess, NiShadowManager::GetCullingProcess());
            if (pkTerrainCullingProcess)
            {
                pkTerrainCullingProcess->SetMaxTerrainLOD(shadowLOD);
                pkTerrainCullingProcess->SetMinTerrainLOD(shadowLOD);
            }
        }
    }
}

//------------------------------------------------------------------------------------------------
void TerrainService::ClearLoadedData()
{
    // Remove all terrain entities
    egf::EntityPtr pEntity = 0;
    while (m_terrainEntities.pop_front(pEntity))
    {
        // We are poping the entities and therefore don't need to remove them from the list
        DetachSceneGraph(pEntity);
        RemoveTerrainEntity(pEntity, false);
    }

    // Respond to all the outstanding preload requests
    SkipPreloadingAssets(false);

    // Destroy all the preloaded/unclaimed terrains
    m_preloadingTerrainMap.clear();

    // Unload all the packages
    m_spSurfaceLibrary->FlushLoadedPackages();
}

//------------------------------------------------------------------------------------------------
void TerrainService::UpdateTerrainEntities()
{
    // Update the terrain entities that the service is managing
    for (EntityList::iterator iterator = m_terrainEntities.begin();
        iterator != m_terrainEntities.end();
        iterator++)
    {
        Entity* pEntity = *iterator;

        // Has a terrain object been created for this entity?
        NiTerrain* pTerrain = GetTerrainForEntity(pEntity);

        if (pTerrain)
            UpdateTerrain(pEntity, pTerrain);
    }
}

//------------------------------------------------------------------------------------------------
void TerrainService::UpdateTerrain(egf::Entity* pEntity, NiTerrain* pTerrain)
{
    EE_UNUSED_ARG(pEntity);
    if (pTerrain)
    {
        float currTime = (float)m_pServiceManager->GetServiceManagerTime();
        pTerrain->Update(currTime);
    }
}

//------------------------------------------------------------------------------------------------
efd::Bool TerrainService::LoadTerrainAsset(
    const efd::utf8string& assetID,
    const efd::utf8string& fileName)
{
    // Decode the archive path from the filename
    efd::utf8string archivePath = efd::PathUtils::PathRemoveFileName(fileName);

    // Find the entity that requested this terrain
    Entity* pEntity = FindEntityByAssetLLID(assetID);
    if (!pEntity)
    {
        LOG_TERRAIN_LOAD_ERROR(efd::kGamebryoGeneral0, efd::ILogger::kERR0,
            ("TerrainService::LoadTerrainAsset: "
            "Failed Load - Could not find an entity in the scene for asset (%s).",
            assetID.c_str()), true);

        return false;
    }

    // Have we already created a terrain for this entity?
    NiTerrain* pTerrain = NULL;
    if (m_entityToTerrainMap[pEntity->GetEntityID()] == NULL)
    {
        // Load a terrain from this file
        efd::UInt32 errorCode = 0;
        pTerrain = LoadTerrain(archivePath, &errorCode, pEntity);

        // Decode the error codes:
        if (errorCode & NiTerrain::EC_INVALID_ARCHIVE_PATH)
        {
            LOG_TERRAIN_LOAD_ERROR(efd::kGamebryoGeneral0, efd::ILogger::kERR0,
                ("TerrainService::LoadTerrainAsset: "
                "Failed Load - Invalid terrain asset location on entity (%s).",
                archivePath.c_str()), true);
        }
        if (errorCode & NiTerrain::EC_TERRAIN_FILE_INVALID)
        {
            LOG_TERRAIN_LOAD_ERROR(efd::kGamebryoGeneral0, efd::ILogger::kERR0,
                ("TerrainService::LoadTerrainAsset: "
                "Failed Load - Invalid terrain asset on entity (%s).",
                archivePath.c_str()), true);
        }
        if (errorCode & NiTerrain::EC_TERRAIN_FILE_OUTOFDATE)
        {
            LOG_TERRAIN_LOAD_ERROR(efd::kGamebryoGeneral0, efd::ILogger::kERR1,
                ("TerrainService::LoadTerrainAsset: "
                "Terrain asset is out of date, please resave your scene from the tools(%s).", 
                archivePath.c_str()), false);

            NotifyTerrainOutOfDate(pTerrain);
        }
        if (errorCode & NiTerrain::EC_TERRAIN_MISSING_DATA)
        {
            LOG_TERRAIN_LOAD_ERROR(efd::kGamebryoGeneral0, efd::ILogger::kERR0,
                ("TerrainService::LoadTerrainAsset: "
                "Failed Load - Terrain asset is missing data on entity (%s).",
                archivePath.c_str()), true);
        }
    }
    else
    {
        pTerrain = m_entityToTerrainMap[pEntity->GetEntityID()];
    }

    if (pTerrain)
    {
        // Did a preload request ask for this terrain?
        bool preloading = false;
        PreloadResponseToAssetMap::iterator iter;
        for (iter = m_preloadResponseToAssetMap.begin(); iter != m_preloadResponseToAssetMap.end();
            ++iter)
        {
            if (iter->second == assetID)
            {
                preloading = true;
                m_preloadingTerrainMap[assetID] = pTerrain;
                break;
            }
        }

        // Notify that the terrain is ready for the loading process
        LoadInitialSectors(assetID, pEntity, preloading);
    }

    // Return the result
    return pTerrain != NULL;
}

//------------------------------------------------------------------------------------------------
NiTerrain* TerrainService::LoadTerrain(
    const efd::utf8string& archivePath,
    efd::UInt32* pErrorCode,
    egf::Entity* pEntity)
{
    // Setup the terrain with the archive path for this asset
    NiTerrain* pTerrain = CreateTerrain();
    pTerrain->SetArchivePath(archivePath.c_str());

    // Load the terrain configuration from file
    if (!pTerrain->Load(0, pErrorCode))
    {
        EE_DELETE pTerrain;
        pTerrain = NULL;
    }

    if (pTerrain)
    {
        // Initialize the terrain's configuration
        InitializeTerrain(pTerrain, pEntity);
    }

    return pTerrain;
}

//------------------------------------------------------------------------------------------------
NiTerrain* TerrainService::CreateTerrain()
{
    NiTerrain* pTerrain = EE_NEW NiTerrain();

    // Assign a default storage policy with a custom asset resolver
    NiTerrain::StoragePolicy storagePolicy;
    storagePolicy.SetAssetResolver(m_spAssetResolver);
    pTerrain->SetStoragePolicy(storagePolicy);

    // Setup the terrain surface library
    pTerrain->SetSurfaceLibrary(m_spSurfaceLibrary);
    
    return pTerrain;
}

//------------------------------------------------------------------------------------------------
void TerrainService::InitializeTerrain(NiTerrain* pTerrain, egf::Entity* pEntity)
{
    EE_ASSERT(pTerrain);

    // Tell the terrain to begin loading the sectors it is supposed to have
    pTerrain->SetSectorSelector(EE_NEW SectorSelector(pTerrain));
    
    // Attach the terrain to the entity
    if (pEntity)
    {
        AttachTerrainToEntity(pTerrain, pEntity);

        if (pEntity->IsInWorld())
        {
            // The entity should already be visible so we need to attach the scene graph
            AttachSceneGraph(pEntity);
        }
    }
}

//------------------------------------------------------------------------------------------------
efd::Bool TerrainService::RequestLoadTerrain(egf::Entity* pEntity, efd::utf8string& assetID)
{
    // Fetch the asset ID of the terrain and Insert a load request
    efd::AssetID terrainAssetID;
    if (pEntity->GetPropertyValue(kPropertyID_StandardModelLibrary_TerrainAsset, terrainAssetID)
        != PropertyResult_OK)
    {
        return false;
    }
    else
    {
        // Check if the asset ID is empty
        if (terrainAssetID.GetURN().empty())
        {
            LOG_TERRAIN_LOAD_ERROR(efd::kGamebryoGeneral0, efd::ILogger::kERR1,
                ("Terrain asset id property value empty for entity model: %s. The terrain will "
                "not be visible untill a path has been assigned to the entity.",
                pEntity->GetModelName().c_str()), false);
            return false;
        }
        assetID = terrainAssetID.GetURN();

        egf::Entity* pAssocEntity = FindEntityByAssetLLID(assetID);
        if (pAssocEntity != NULL && pAssocEntity != pEntity)
        {
            LOG_TERRAIN_LOAD_ERROR(efd::kGamebryoGeneral0, efd::ILogger::kERR1,
                ("Terrain asset has already been loaded for a different entity."), false);
            return false;
        }

        // Have we already requested this terrain?
        if (m_terrainEntities.find(pEntity) == m_terrainEntities.end())
        {
            // Request to load this asset
            InsertAssetLocateRequest(AT_Terrain, assetID);
        }

        return true;
    }
}

//------------------------------------------------------------------------------------------------
NiTerrain* TerrainService::CreateBlankTerrain(
    efd::UInt32 terrainSize,
    efd::UInt32 cellSize,
    efd::UInt32 materialMaskSize,
    efd::UInt32 lowDetailTextureSize,
    efd::Float32 minElevation,
    efd::Float32 maxElevation,
    efd::Float32 vertexSpacing)
{
    EE_UNUSED_ARG(vertexSpacing);

    NiTerrain* pResult = CreateTerrain();

    // Set up the block size and the number of levels of detail.
    efd::UInt32 numLOD = (efd::UInt32)(efd::Log((float)terrainSize / (float)cellSize)
        / efd::Log(2.0f));
    pResult->SetCellSize(cellSize);
    pResult->SetNumLOD(numLOD);
    pResult->SetMinHeight(minElevation);
    pResult->SetMaxHeight(maxElevation);
    pResult->SetMaskSize(materialMaskSize);
    pResult->SetLowDetailTextureSize(lowDetailTextureSize);

    InitializeTerrain(pResult);
    pResult->CreateBlankSector(0, 0, true);

    return pResult;
}

//------------------------------------------------------------------------------------------------
efd::Bool TerrainService::InsertAssetLocateRequest(
    AssetType eAssetType,
    const efd::utf8string& assetURN)
{
    // Fetch the asset locater service
    AssetLocatorService* pAssetLocator =
        m_pServiceManager->GetSystemServiceAs<AssetLocatorService>();
    if (!pAssetLocator || assetURN.empty())
        return false;

    // Request to decode the asset URN
    pAssetLocator->AssetLocate(assetURN, m_loadCategory[eAssetType]);
    return true;
}

//------------------------------------------------------------------------------------------------
efd::Bool TerrainService::InsertAssetRefreshRequest(
    AssetType assetType,
    const efd::utf8string& assetURN)
{
    // Fetch the asset locater service
    AssetLocatorService* pAssetLocator =
        m_pServiceManager->GetSystemServiceAs<AssetLocatorService>();
    if (!pAssetLocator || assetURN.empty())
        return false;

    // Request to decode the asset URN
    pAssetLocator->AssetLocate(assetURN, m_refreshCategory[assetType]);
    return true;
}

//------------------------------------------------------------------------------------------------
void TerrainService::AttachTerrainToEntity(NiTerrain* pTerrain, egf::Entity* pEntity)
{
    EE_ASSERT(pTerrain);
    EE_ASSERT(pEntity);

    // Insert the terrain into our entity -> terrain map
    m_entityToTerrainMap[pEntity->GetEntityID()] = pTerrain;

    // Update the transformation on the terrain from the entity
    UpdateTransformation(pTerrain, pEntity);
    
    // Notify that the terrain has changed;
    RaiseTerrainChanged();
}

//------------------------------------------------------------------------------------------------
void TerrainService::UpdateTransformation(NiTerrain* pTerrain, egf::Entity* pEntity)
{
    if (!pTerrain || !pEntity)
        return;

    efd::Point3 position;
    if (pEntity->GetPropertyValue(kPropertyID_StandardModelLibrary_Position, position) ==
        PropertyResult_OK)
    {
        pTerrain->SetTranslate(position.x, position.y, position.z);
    }

    efd::Point3 rotation;
    if (pEntity->GetPropertyValue(kPropertyID_StandardModelLibrary_Rotation, rotation) ==
        PropertyResult_OK)
    {
        NiMatrix3 rotX, rotY, rotZ;
        rotX.MakeXRotation(rotation.x * -EE_DEGREES_TO_RADIANS);
        rotY.MakeYRotation(rotation.y * -EE_DEGREES_TO_RADIANS);
        rotZ.MakeZRotation(rotation.z * -EE_DEGREES_TO_RADIANS);
        pTerrain->SetRotate(rotX * rotY * rotZ);
    }

    float scale;
    if (pEntity->GetPropertyValue(kPropertyID_StandardModelLibrary_Scale, scale) ==
        PropertyResult_OK)
    {
        pTerrain->SetScale(scale);
    }

    pTerrain->Update(0.0f);
}

//------------------------------------------------------------------------------------------------
void TerrainService::RemoveTerrainEntity(egf::Entity* pEntity, efd::Bool removeEntity)
{
    EE_ASSERT(pEntity != NULL);
    
    // Ensure the terrain is removed from our map
    EntityToTerrainMap::iterator iter = m_entityToTerrainMap.find(pEntity->GetEntityID());
    if (iter != m_entityToTerrainMap.end())
    {
        NiTerrain* pTerrain = iter->second;
        if (pTerrain)
            pTerrain->DestroyData();
        m_entityToTerrainMap.erase(iter);
    }

    // Remove our knowledge of this entity altogether?
    if (removeEntity)
        m_terrainEntities.remove(pEntity);

    // Notify anyone who cares that their terrain pointers are no longer valid
    RaiseTerrainChanged();
}

//------------------------------------------------------------------------------------------------
efd::Bool TerrainService::GetTerrainHeightAt(const efd::Point3& pt, float& height,
    efd::Point3& normal)
{
    // Are there any terrains loaded?
    NiTerrain* pTerrain = GetNiTerrain();
    if (!pTerrain)
        return false;

    // Find the height of all terrains at this point
    {
        NiRay ray = NiRay(pt, - efd::Point3::UNIT_Z);
        if (pTerrain->Collide(ray))
        {
            efd::Point3 collisionPt;
            ray.GetIntersection((NiPoint3&)collisionPt, (NiPoint3&)normal);
            height = collisionPt.z;
            return true;
        }
    }

    return false;
}

//------------------------------------------------------------------------------------------------
NiSurfacePackage* TerrainService::FindMaterialPackageAsset(const efd::utf8string& packageAssetID)
{
    return m_spSurfaceLibrary->FindPackageByAsset(packageAssetID);
}

//------------------------------------------------------------------------------------------------
NiSurfacePackage* TerrainService::FindMaterialPackage(const efd::utf8string& packageName)
{
    return m_spSurfaceLibrary->FindPackageByName(packageName);
}


//------------------------------------------------------------------------------------------------
NiSurfacePackage* TerrainService::OpenSurfacePackage(
    const efd::utf8string& packageAssetID,
    const efd::utf8string& packageLocation)
{
    // Generate a resolved reference for the surface library to load
    NiTerrainAssetReference kReference;
    kReference.SetAssetID(packageAssetID);
    kReference.SetResolvedLocation(packageLocation);
    kReference.MarkResolved(true);
    m_spSurfaceLibrary->InjectReference(kReference);

    // Package should have loaded...
    return m_spSurfaceLibrary->FindPackageByAsset(packageAssetID);
}


//------------------------------------------------------------------------------------------------
void TerrainService::ReloadSurfacePackage(NiSurfacePackage* pPackage)
{
    m_spSurfaceLibrary->RequestReload(pPackage);
}

//------------------------------------------------------------------------------------------------
void TerrainService::RequestLoadSurfacePackage(efd::utf8string packageAssetID)
{
    // Is it already completely loaded?
    if (FindMaterialPackageAsset(packageAssetID))
        return;

    // We didn't find it anywhere, insert the request
    InsertAssetLocateRequest(AT_MaterialPackage, packageAssetID);
}

//------------------------------------------------------------------------------------------------
void TerrainService::RequestLoadAllSurfacePackages()
{
    RequestLoadSurfacePackage("urn:" + m_assetTags[AT_MaterialPackage]);
}

//------------------------------------------------------------------------------------------------
void TerrainService::OnSurfacePackageUpdated(NiTerrainSurfaceLibrary* pLibrary, 
    NiSurfacePackage* pPackage)
{
    EE_UNUSED_ARG(pLibrary);
    if (pPackage)
    {
        NotifySurfacePackageLoaded(pPackage->GetAssetID(), pPackage);
    }
}

//------------------------------------------------------------------------------------------------
void TerrainService::NotifySurfacePackageLoaded(
    const efd::utf8string& assetID,
    NiSurfacePackage* pPackage)
{
    // Do nothing, a hook for subclasses
    EE_UNUSED_ARG(assetID);
    EE_UNUSED_ARG(pPackage);
}

//------------------------------------------------------------------------------------------------
void TerrainService::NotifyTerrainOutOfDate(NiTerrain* pTerrain)
{   
    // Do nothing, a hook for subclasses
    EE_UNUSED_ARG(pTerrain);
}

//------------------------------------------------------------------------------------------------
efd::Bool TerrainService::GetPackageAndSurface(
    const efd::utf8string& packageName,
    const efd::utf8string& surfaceName,
    NiSurfacePackage*& pPackage,
    NiSurface*& pSurface)
{
    pPackage = FindMaterialPackage(packageName);
    if (pPackage == NULL)
    {
        EE_LOG(efd::kGamebryoGeneral0, efd::ILogger::kERR1,
            ("ToolTerrainService: Could not locate the currently selected package."));

        pSurface = NULL;
        return false;
    }

    pPackage->GetSurface(surfaceName.c_str(), pSurface);
    if (pSurface == NULL)
    {
        EE_LOG(efd::kGamebryoGeneral0, efd::ILogger::kERR1,
            ("ToolTerrainService: Could not locate the specified material."));

        return false;
    }

    return true;
}

//------------------------------------------------------------------------------------------------
bool TerrainService::DetectSectorFromAsset(
    efd::utf8string assetPath,
    efd::SInt16& sectorX,
    efd::SInt16& sectorY)
{
    efd::utf8string filename = efd::PathUtils::PathGetFileName(assetPath);
    filename = efd::PathUtils::PathRemoveFileExtension(filename);

    // Find the last and second last underscore in the name
    efd::utf8string::size_type lastUnderscore = filename.find_last_of("_");
    efd::utf8string::size_type secondLastUnderscore = filename.find_last_of("_",
                                                                            lastUnderscore);
    if (lastUnderscore == efd::utf8string::npos ||
        secondLastUnderscore == efd::utf8string::npos)
    {
        return false;
    }

    // Extract the values
    efd::utf8string sectorXstr =
        filename.substr(secondLastUnderscore + 1, lastUnderscore - (secondLastUnderscore + 1));
    efd::utf8string sectorYstr = filename.substr(lastUnderscore + 1);

    // Parse the values
    if (!efd::ParseHelper<efd::SInt16>::FromString(sectorXstr, sectorX))
        return false;
    if (!efd::ParseHelper<efd::SInt16>::FromString(sectorYstr, sectorY))
        return false;

    return true;
}

//------------------------------------------------------------------------------------------------
void TerrainService::RefreshAsset(
    AssetType assetType,
    const efd::utf8string& assetID,
    const efd::utf8string& assetPath)
{
    switch (assetType)
    {
    case AT_Terrain:
        {
            // Find all the terrain entities that are interested in this asset
            efd::set<egf::Entity*> reloadEntitySet;
            for (EntityList::iterator iterator = m_terrainEntities.begin();
                iterator != m_terrainEntities.end();
                iterator++)
            {
                Entity* pEntity = *iterator;

                // Fetch the asset ID of the terrain and Insert a load request
                efd::AssetID terrainAssetID;
                if (pEntity->GetPropertyValue(kPropertyID_StandardModelLibrary_TerrainAsset,
                    terrainAssetID) != PropertyResult_OK)
                {
                    continue;
                }
                else
                {
                    if (terrainAssetID.GetURN() == assetID)
                        reloadEntitySet.insert(pEntity);
                }
            }

            // Reload all the lodged entities
            efd::set<egf::Entity*>::iterator setIter;
            efd::map<egf::EntityID, efd::vector<NiTerrainSector::SectorID> > loadedSectorLists;
            for (setIter = reloadEntitySet.begin(); setIter != reloadEntitySet.end(); ++setIter)
            {
                Entity* pEntity = *setIter;

                // Figure out all the sectors that this entity currently has loaded
                NiTerrain* pTerrain = GetTerrainForEntity(pEntity);
                if (pTerrain)
                {
                    const NiTMap<NiUInt32, NiTerrainSector*>& sectors =
                        pTerrain->GetLoadedSectors();

                    efd::vector<NiTerrainSector::SectorID> sectorList;
                    NiTMapIterator iterator = sectors.GetFirstPos();
                    while (iterator)
                    {
                        NiTerrainSector* pSector = NULL;
                        NiUInt32 index;
                        sectors.GetNext(iterator, index, pSector);

                        sectorList.push_back(pSector->GetSectorID());
                    }

                    loadedSectorLists[pEntity->GetEntityID()] = sectorList;
                }

                // Force the terrain to reload
                DetachSceneGraph(pEntity);
                RemoveTerrainEntity(pEntity, false);
                LoadTerrainAsset(assetID, assetPath);

                // Attempt to load the original set of sectors
                pTerrain = GetTerrainForEntity(pEntity);
                if (pTerrain)
                {
                    // Wait for any streaming on that terrain to complete
                    pTerrain->GetStreamingManager()->WaitForAllTasksCompleted();

                    // Fetch the list of currently loaded sectors
                    const NiTMap<NiUInt32, NiTerrainSector*>& sectors =
                        pTerrain->GetLoadedSectors();
                    // Fetch the list of sectors originally loaded
                    efd::map<egf::EntityID, efd::vector<NiTerrainSector::SectorID> >::iterator iter;
                    iter = loadedSectorLists.find(pEntity->GetEntityID());
                    if (iter != loadedSectorLists.end())
                    {
                        efd::vector<NiTerrainSector::SectorID>& sectorList = iter->second;

                        // Unload sectors that aren't needed
                        NiTMapIterator iterator = sectors.GetFirstPos();
                        while (iterator)
                        {
                            NiTerrainSector* pSector = NULL;
                            NiUInt32 index;
                            sectors.GetNext(iterator, index, pSector);

                            efd::vector<NiTerrainSector::SectorID>::iterator findResult =
                                sectorList.find(pSector->GetSectorID());
                            if (findResult != sectorList.end())
                            {
                                // Remove the sector from the list to load
                                sectorList.erase(findResult);
                            }
                            else
                            {
                                // Unload the sector
                                pTerrain->GetStreamingManager()->RequestPageSector(pSector, -1,
                                    true);
                            }
                        }

                        // Load sectors that are missing
                        efd::vector<NiTerrainSector::SectorID>::iterator idIter;
                        for (idIter = sectorList.begin(); idIter != sectorList.end(); ++idIter)
                        {
                            NiTerrainSector::SectorID sectorID = *idIter;
                            efd::SInt16 indexX, indexY;
                            NiTerrainSector::GenerateSectorIndex(sectorID, indexX, indexY);

                            pTerrain->PageSector(indexX, indexY, pTerrain->GetNumLOD());
                        }
                    }

                    // Wait for these sectors to complete their loading
                    pTerrain->GetStreamingManager()->WaitForAllTasksCompleted();
                }
            }
        }
        break;
    case AT_TerrainSector:
        {
            // Work out the archive path of the relevant terrain
            efd::utf8string archivePath = efd::PathUtils::PathRemoveFileName(assetPath);
            efd::SInt16 sectorX = 0;
            efd::SInt16 sectorY = 0;

            // Work out the relevant sector
            if (assetType == AT_TerrainSector &&
                !DetectSectorFromAsset(assetPath, sectorX, sectorY))
            {
                return;
            }

            // Iterate over all terrains and figure out which terrains are relevant
            efd::set<egf::Entity*> reloadEntitySet;
            EntityToTerrainMap::iterator iter;
            for (iter = m_entityToTerrainMap.begin(); iter != m_entityToTerrainMap.end(); ++iter)
            {
                NiTerrain* pTerrain = iter->second;
                if (pTerrain && (pTerrain->GetArchivePath() == archivePath))
                {
                    // Reload changed sector
                    NiTerrainSector* pSector = pTerrain->GetSector(sectorX, sectorY);
                    if (pSector)
                    {
                        efd::SInt32 loadedLOD =
                            pSector->GetSectorData()->GetHighestLoadedLOD();
                        pTerrain->GetStreamingManager()->RequestPageSector(pSector, -1, true);
                        pTerrain->PageSector(sectorX, sectorY, loadedLOD);
                        pTerrain->GetStreamingManager()->WaitForAllTasksCompleted();
                    }
                }
            }
        }
        break;
    case AT_MaterialPackage:
        {
            // Find the material package that corresponds to this asset
            NiSurfacePackage* pPackage = FindMaterialPackageAsset(assetID);
            if (!pPackage)
                return;

            m_spSurfaceLibrary->RequestReload(pPackage);
        }
        break;
    default:
        // Don't handle any other asset reload requests
        break;
    }
}

//------------------------------------------------------------------------------------------------
void TerrainService::DetachSceneGraph(egf::Entity* pEntity)
{
    // Is the entity relevant to us?
    NiTerrain* pTerrain = GetTerrainForEntity(pEntity);
    if (!pTerrain)
        return;

    EE_ASSERT(m_pSceneGraphService);
    m_pSceneGraphService->RemoveSceneGraph(pEntity, true);
}

//------------------------------------------------------------------------------------------------
void TerrainService::AttachSceneGraph(egf::Entity* pEntity)
{
    // Is the entity relevant to us?
    NiTerrain* pTerrain = GetTerrainForEntity(pEntity);
    if (!pTerrain)
        return;

    // Push the terrain into the scene graph
    efd::vector<NiObjectPtr> objects;
    objects.push_back((NiObject*)pTerrain);
    m_pSceneGraphService->CreateExternalSceneGraph(pEntity, objects, false);
}

//------------------------------------------------------------------------------------------------
void TerrainService::HandleAssetLocatorResponse(
    const efd::AssetLocatorResponse* pMessage,
    efd::Category targetChannel)
{
    // One of the terrain's asset locate requests has come back
    efd::utf8string uri = pMessage->GetResponseURI();
    const AssetLocatorResponse::AssetURLMap& assets = pMessage->GetAssetURLMap();

    // Check if we actually got any assets from the request?
    if (assets.empty())
    {
        // Check if this was an attempt to load all packages (non error condition)
        if (uri.find(m_assetTags[AT_MaterialPackage]) == efd::utf8string::npos)
        {
            LOG_TERRAIN_LOAD_ERROR(efd::kGamebryoGeneral0, efd::ILogger::kERR1,
                ("TerrainService::HandleAssetLocatorResponse: "
                "Found 0 assets when looking for tag '%s'.", uri.c_str()), true);
        }
        return;
    }

    // Loop through each asset response and figure out what to do with the returned files
    for (AssetLocatorResponse::AssetURLMap::const_iterator currAsset = assets.begin();
        currAsset != assets.end();
        ++currAsset)
    {
        efd::AssetLocatorResponse::AssetLoc loc = currAsset->second;
        efd::utf8string assetLLID;

        // Do we need to add the urn:llid prefix?
        if (loc.llid.empty())
        {
            assetLLID = uri;
            EE_ASSERT(assets.size() == 1);
        }
        else if (loc.llid.find("urn:") == assetLLID.npos)
        {
            assetLLID = "urn:llid:" + loc.llid;
        }
        else
        {
            assetLLID = loc.llid;
        }

        efd::utf8string assetFileName = currAsset->second.url;
        if (targetChannel == m_loadCategory[AT_Terrain])
        {
            LoadTerrainAsset(assetLLID, assetFileName);
        }
        else if (targetChannel == m_loadCategory[AT_MaterialPackage])
        {
            OpenSurfacePackage(assetLLID, assetFileName.c_str());
        }
        else if (targetChannel == m_refreshCategory[AT_Terrain])
        {
            RefreshAsset(AT_Terrain, assetLLID, assetFileName.c_str());
        }
        else if (targetChannel == m_refreshCategory[AT_TerrainSector])
        {
            RefreshAsset(AT_TerrainSector, assetLLID, assetFileName.c_str());
        }
        else if (targetChannel == m_refreshCategory[AT_MaterialPackage])
        {
            RefreshAsset(AT_MaterialPackage, assetLLID, assetFileName.c_str());
        }
    }
}

//------------------------------------------------------------------------------------------------
void TerrainService::HandleEntityDiscoverMessage(
    const egf::EntityChangeMessage* pMessage,
    efd::Category targetChannel)
{
    EE_ASSERT(pMessage);
    EE_ASSERT(m_pSceneGraphService);

    // Which entity have we discovered?
    Entity* pEntity = pMessage->GetEntity();
    EE_ASSERT(pEntity);

    // Check if we have already discovered this entity.
    if (m_terrainEntities.find(pEntity) != m_terrainEntities.end())
        return;

    // Is this entity a terrain?
    if (!pEntity->GetModel()->ContainsModel(kFlatModelID_StandardModelLibrary_Terrain))
        return;

    // Build a terrain object for this entity and configure the shadowing system
    efd::utf8string assetID;
    if (RequestLoadTerrain(pEntity, assetID))
    {
        // Add the entity to our list of managed terrain entities
        m_terrainEntities.push_back(pEntity);
    
        ConfigureShadowsForTerrain(pEntity);
    }
}

//------------------------------------------------------------------------------------------------
void TerrainService::HandleEntityRemovedMessage(
    const egf::EntityChangeMessage* pMessage,
    efd::Category targetChannel)
{
    EE_ASSERT(pMessage);

    // Remove this entity from our records
    Entity* pEntity = pMessage->GetEntity();
    RemoveTerrainEntity(pEntity, true);
}

//------------------------------------------------------------------------------------------------
void TerrainService::HandleEntityUpdatedMessage(
    const egf::EntityChangeMessage* pMessage,
    efd::Category targetChannel)
{
    EE_ASSERT(pMessage);
    Entity* pEntity = pMessage->GetEntity();

    // Is the entity relevant to us?
    if (m_entityToTerrainMap.find(pEntity->GetEntityID()) == m_entityToTerrainMap.end())
        return;

    // Check if the entity's asset property has changed
    if (pEntity->IsDirty(kPropertyID_StandardModelLibrary_TerrainAsset))
    {
        DetachSceneGraph(pEntity);
        RemoveTerrainEntity(pEntity, true);
        efd::utf8string assetID;
        if (RequestLoadTerrain(pEntity, assetID))
        {
            // Re-add the entity to our list of managed terrain entities
            m_terrainEntities.push_back(pEntity);
        }
    }

    // Update the shadowing configuration
    if (pEntity->IsDirty(kPropertyID_StandardModelLibrary_TerrainShadowLOD))
    {
        ConfigureShadowsForTerrain(pEntity);
    }

    // Update the terrain's transformation from the entity
    UpdateTransformation(m_entityToTerrainMap[pEntity->GetEntityID()], pEntity);
}

//------------------------------------------------------------------------------------------------
void TerrainService::HandleEntityEnterWorldMessage(
    const egf::EntityChangeMessage* pMessage,
    efd::Category targetChannel)
{
    EE_ASSERT(pMessage);
    Entity* pEntity = pMessage->GetEntity();
        
    AttachSceneGraph(pEntity);
}

//------------------------------------------------------------------------------------------------
void TerrainService::HandleEntityExitWorldMessage(
    const egf::EntityChangeMessage* pMessage,
    efd::Category targetChannel)
{
    EE_ASSERT(pMessage);
    Entity* pEntity = pMessage->GetEntity();

    DetachSceneGraph(pEntity);    
}

//------------------------------------------------------------------------------------------------
void TerrainService::HandleAssetChange(const efd::utf8string& assetId, const efd::utf8string& tag)
{
    // Make sure the tags on the asset contain our Terrain tag indicating the asset that changed
    // on disk is a Terrain-related file we need to track.
    if (tag.find(m_assetTags[AT_MaterialPackage]) != efd::utf8string::npos)
    {
        InsertAssetRefreshRequest(AT_MaterialPackage, assetId);
    }
    else if (tag.find(m_assetTags[AT_TerrainSector]) != efd::utf8string::npos)
    {
        InsertAssetRefreshRequest(AT_TerrainSector, assetId);
    }
    else if (tag.find(m_assetTags[AT_Terrain]) != efd::utf8string::npos)
    {
        // Check this one last because its tag is generally contained in the other tags
        InsertAssetRefreshRequest(AT_Terrain, assetId);
    }
}

//------------------------------------------------------------------------------------------------
void TerrainService::HandlePreloadRequest(
    const egf::EntityPreloadRequest* pRequest,
    efd::Category targetChannel)
{
    EE_UNUSED_ARG(targetChannel);

    // Examine the entity for assets we need to preload
    Entity* pEntity = pRequest->GetEntity();
    EntityPreloadResponse* pResponse = EE_NEW EntityPreloadResponse;
    pResponse->m_pEntity = pEntity;

    // Entity is not terrain or is in the process of being destroyed
    if (!pEntity->GetModel()->ContainsModel(kFlatModelID_StandardModelLibrary_Terrain) ||
        pEntity->IsDestroyInProgress())
    {
        m_pMessageService->SendLocal(pResponse, m_assetPreloadResponseCategory);
        return;
    }

    // Log the preload event
    EE_LOG(efd::kEntity, efd::ILogger::kLVL3,
        ("TerrainService: handling preload of entity %s",
        pEntity->GetModel()->GetName().c_str()));

    // Handle the case where a preload has already started
    if (IsEntityPreloading(pEntity->GetEntityID()))
    {
        EE_LOG(efd::kGamebryoGeneral0, efd::ILogger::kERR1,
            ("TerrainService: Terrain entity already preloading within the block."));
        m_pMessageService->SendLocal(pResponse, m_assetPreloadResponseCategory);
        return;
    }

    // Fetch the asset that this entity will require to be loaded
    efd::utf8string assetID;
    if (RequestLoadTerrain(pEntity, assetID))
    {
        // Add the entity to our list of managed terrain entities
        m_terrainEntities.push_back(pEntity);

        // Set the preload response so TerrainService knows that a preload has started.
        m_preloadResponseToAssetMap[pResponse] = assetID;
    }
    else
    {
        // Didn't end up preloading a terrain, but we still need to send a response
        m_pMessageService->SendLocal(pResponse, m_assetPreloadResponseCategory);
    }
}

//------------------------------------------------------------------------------------------------
void TerrainService::FetchTerrainSet(efd::set<NiTerrain*>& terrains)
{
    // Fetch all the claimed terrains
    EntityToTerrainMap::iterator claimedIter;
    for (claimedIter = m_entityToTerrainMap.begin();
        claimedIter != m_entityToTerrainMap.end();
        ++claimedIter)
    {
        terrains.insert(claimedIter->second);
    }

    // Fetch all the preloaded terrains
    PreloadingTerrainMap::iterator preloadIter;
    for (preloadIter = m_preloadingTerrainMap.begin(); preloadIter != m_preloadingTerrainMap.end();
        ++preloadIter)
    {
        terrains.insert(preloadIter->second);
    }
}

//------------------------------------------------------------------------------------------------
void TerrainService::LoadInitialSectors(
    efd::utf8string assetID, Entity* pEntity,
    bool waitForCompletion)
{
    EE_ASSERT(pEntity);
    if (!pEntity)
        return;

    NiTerrain* pTerrain = m_entityToTerrainMap[pEntity->GetEntityID()];
    if (!pTerrain)
        return;

    // Fetch the entity's list of preload sector ID's
    efd::list<efd::utf8string> preloadKeys;
    if (pEntity->GetPropertyKeys(
        kPropertyID_StandardModelLibrary_TerrainInitialSectors, preloadKeys) ==
        PropertyResult_OK)
    {
        efd::list<efd::utf8string>::iterator iter;
        for (iter = preloadKeys.begin(); iter != preloadKeys.end(); ++iter)
        {
            // Fetch the next property value
            NiTerrainSector::SectorID sectorID;
            if (pEntity->GetPropertyValue(
                kPropertyID_StandardModelLibrary_TerrainInitialSectors, *iter, sectorID) !=
                PropertyResult_OK)
                continue;

            // Work out which sector to load
            efd::SInt16 sectorX, sectorY;
            NiTerrainSector::GenerateSectorIndex(sectorID, sectorX, sectorY);
            LoadSector(pTerrain, sectorX, sectorY);
        }
    }

    // Commence the terrain streaming
    pTerrain->UpdateStreaming();

    if (waitForCompletion)
        pTerrain->GetStreamingManager()->WaitForAllTasksCompleted();
}

//------------------------------------------------------------------------------------------------
bool TerrainService::IsEntityPreloading(egf::EntityID entityID)
{
    PreloadResponseToAssetMap::iterator iter;
    for (iter = m_preloadResponseToAssetMap.begin(); iter != m_preloadResponseToAssetMap.end();
        ++iter)
    {
        if (iter->first->GetEntityID() == entityID)
            return true;
    }

    return false;
}

//------------------------------------------------------------------------------------------------
bool TerrainService::IsLoadComplete(NiTerrain* pTerrain)
{
    EE_ASSERT(pTerrain);

    // Is the terrain streaming data from disk?
    if (pTerrain->GetStreamingManager()->PollStreamingStatus())
        return false;

    // Has the terrain had all it's packages attempted to be resolved?
    if (pTerrain->GetNumUnresolvedSurfaces() != 0)
        return false;

    return true;
}

//------------------------------------------------------------------------------------------------
void TerrainService::CheckPreloadStatus()
{
    // Loop through all the preload requests looking for terrains
    EE_STL_NAMESPACE::set<egf::EntityPreloadResponse*> completedPreloads;
    {
        PreloadResponseToAssetMap::iterator iter;
        for (iter = m_preloadResponseToAssetMap.begin(); iter != m_preloadResponseToAssetMap.end();
            ++iter)
        {
            // Find the terrain object for this request
            NiTerrainPtr spTerrain = NULL;
            if (m_preloadingTerrainMap.find(iter->second, spTerrain))
            {
                if (spTerrain && IsLoadComplete(spTerrain))
                {
                    // Send the preload response
                    m_pMessageService->SendLocal(iter->first, m_assetPreloadResponseCategory);
                    completedPreloads.insert(iter->first);

                    // We have finished preloading now
                    m_preloadingTerrainMap.erase(iter->second);
                }
            }
        }
    }

    // Loop through those completed responses and remove them from the map
    {
        EE_STL_NAMESPACE::set<egf::EntityPreloadResponse*>::iterator iter;
        for (iter = completedPreloads.begin(); iter != completedPreloads.end(); ++iter)
        {
            m_preloadResponseToAssetMap.erase(*iter);
        }
    }
}

//------------------------------------------------------------------------------------------------
void TerrainService::SkipPreloadingAssets(bool bError)
{
    if (bError)
    {
        EE_LOG(efd::kGamebryoGeneral0, efd::ILogger::kERR1,
            ("TerrainService: Skipping preload of assets due to previous errors whilst loading"));
    }

    // Send out the preload responses prematurely. Generally because there was an issue loading
    // one or more of the assets. Check the log files for details on the issues.
    PreloadResponseToAssetMap::iterator iter;
    for (iter = m_preloadResponseToAssetMap.begin(); iter != m_preloadResponseToAssetMap.end();
        ++iter)
    {
        egf::EntityPreloadResponse* pResponse = iter->first;

        m_pMessageService->SendLocal(pResponse, m_assetPreloadResponseCategory);
    }
    m_preloadResponseToAssetMap.clear();
    m_preloadingTerrainMap.clear();
}

//------------------------------------------------------------------------------------------------
TerrainService::TerrainPickPolicy::TerrainPickPolicy(TerrainService* pTerrainService) :
    m_pTerrainService(pTerrainService)
{
    EE_ASSERT(m_pTerrainService);
}

//------------------------------------------------------------------------------------------------
bool TerrainService::TerrainPickPolicy::FindIntersections(
    const NiPoint3& origin,
    const NiPoint3& dir,
    NiPick& pick,
    NiRenderObject* pRenderObj)
{
    EE_UNUSED_ARG(pRenderObj);
    EE_ASSERT(m_pTerrainService);

    if (!m_pTerrainService->GetNiTerrain())
        return false;

    NiTerrain* pTerrain = m_pTerrainService->GetNiTerrain();
    NiRay ray = NiRay(origin, dir);

    EE_ASSERT(pTerrain);
    if (pTerrain->Collide(ray))
    {
        // append the results to the provided NiPick object
        NiPoint3 position;
        NiPoint3 normal;
        ray.GetIntersection(position, normal);

        NiPick::Record* pRecord = pick.Add(m_pTerrainService->GetNiTerrain());
        pRecord->SetIntersection(position);
        pRecord->SetNormal(normal);
        pRecord->SetDistance((position - origin).Length());
        return true;
    }

    return false;
}

//------------------------------------------------------------------------------------------------
Entity* TerrainService::FindEntityByAssetLLID(const AssetID& assetLLID)
{
    for (EntityList::iterator iterator = m_terrainEntities.begin();
        iterator != m_terrainEntities.end();
        iterator++)
    {
        Entity* pEntity = *iterator;

        // Fetch the asset ID of the terrain and Insert a load request
        efd::AssetID terrainAssetID;
        if (pEntity->GetPropertyValue(kPropertyID_StandardModelLibrary_TerrainAsset, terrainAssetID)
            != PropertyResult_OK)
        {
            continue;
        }
        else
        {
            if (terrainAssetID == assetLLID)
                return pEntity;
        }
    }

    return NULL;
}

//------------------------------------------------------------------------------------------------
void TerrainService::LoadSector(NiTerrain* pTerrain, efd::SInt16 sectorX, efd::SInt16 sectorY)
{
    if (!pTerrain)
        return;

    // Fetch the sector selector object from the terrain. If the right type isn't there
    // then this operation isn't supported.
    SectorSelector* pSectorSelector =
        NiDynamicCast(SectorSelector, pTerrain->GetSectorSelector());
    if (!pSectorSelector)
        return;

    // Make request
    pSectorSelector->RequestSectorLOD(sectorX, sectorY, pTerrain->GetNumLOD());
    pSectorSelector->LockSectorLOD(sectorX, sectorY);
}

//------------------------------------------------------------------------------------------------
void TerrainService::UnloadSector(NiTerrain* pTerrain, efd::SInt16 sectorX, efd::SInt16 sectorY)
{
    if (!pTerrain)
        return;

    // Fetch the sector selector object from the terrain. If the right type isn't there
    // then this operation isn't supported.
    SectorSelector* pSectorSelector =
        NiDynamicCast(SectorSelector, pTerrain->GetSectorSelector());
    if (!pSectorSelector)
        return;

    // Make request
    pSectorSelector->UnlockSectorLOD(sectorX, sectorY);
    pSectorSelector->RequestSectorLOD(sectorX, sectorY, -1);
}

//------------------------------------------------------------------------------------------------
NiImplementRTTI(TerrainService::SectorSelector, NiTerrainSectorSelector);

//------------------------------------------------------------------------------------------------
TerrainService::SectorSelector::SectorSelector(NiTerrain* pkTerrain) :
    NiTerrainSectorSelector(pkTerrain),
    m_spSelector(pkTerrain->GetSectorSelector())
{
    // Disable the sector catalogue
    m_bUseSectorCatalogue = false;

    EE_ASSERT(m_spSelector);

    // Set the range of the default sector selector to be 0
    NiTerrainSectorSelectorDefault* pDefaultSelector =
        NiDynamicCast(NiTerrainSectorSelectorDefault, m_spSelector);
    if (pDefaultSelector)
    {
        pDefaultSelector->SetTerrainExtent(0, 0);
    }
}

//------------------------------------------------------------------------------------------------
TerrainService::SectorSelector::SectorSelector(
    NiTerrain* pTerrain,
    NiTerrainSectorSelector* pOrigSelector)
    : NiTerrainSectorSelector(pTerrain)
    , m_spSelector(pOrigSelector)
{
    EE_ASSERT(m_spSelector);
}

//------------------------------------------------------------------------------------------------
TerrainService::SectorSelector::~SectorSelector()
{
}

//------------------------------------------------------------------------------------------------
bool TerrainService::SectorSelector::UpdateSectorSelection()
{
    // Loop through the original selection algorithm and filter it's results
    m_spSelector->UpdateSectorSelection();
    const efd::list<LoadingInfo>& selectedSectors = m_spSelector->GetSelectedSectors();
    efd::list<LoadingInfo>::const_iterator selIter;
    for (selIter = selectedSectors.begin(); selIter != selectedSectors.end(); ++selIter)
    {
        const LoadingInfo& info = *selIter;

        // Check if this sector is locked
        NiTerrainSector::SectorID sectorID;
        NiTerrainSector::GenerateSectorID(info.m_sIndexX, info.m_sIndexY, sectorID);

        // Add to the list if it isn't locked
        if (m_lockedSectors.find(sectorID) == m_lockedSectors.end())
        {
            m_kSelectedSector.push_back(info);
            m_kSectorDetailLevels[sectorID] = info.m_iTargetLoD;
        }
    }
    m_spSelector->ClearSelection();

    // Add all the requests made
    efd::map<NiTerrainSector::SectorID, efd::SInt32>::iterator reqIter;
    for (reqIter = m_pendingRequests.begin(); reqIter != m_pendingRequests.end(); ++reqIter)
    {
        NiTerrainSector::SectorID sectorID = reqIter->first;
        efd::SInt32 lod = reqIter->second;

        efd::SInt16 sectorX, sectorY;
        NiTerrainSector::GenerateSectorIndex(sectorID, sectorX, sectorY);
        AddToSelection(sectorX, sectorY, lod);
    }

    // Clear all the requests that have been made
    m_pendingRequests.clear();

    return m_kSelectedSector.size() > 0;
}

//------------------------------------------------------------------------------------------------
void TerrainService::SectorSelector::NotifySectorLOD(
    efd::SInt16 sectorX,
    efd::SInt16 sectorY,
    efd::SInt32 lod)
{
    NiTerrainSector::SectorID sectorID;
    NiTerrainSector::GenerateSectorID(sectorX, sectorY, sectorID);
    m_kSectorDetailLevels[sectorID] = lod;
}

//------------------------------------------------------------------------------------------------
void TerrainService::SectorSelector::RequestSectorLOD(
    efd::SInt16 sectorX,
    efd::SInt16 sectorY,
    efd::SInt32 lod)
{
    NiTerrainSector::SectorID sectorID;
    NiTerrainSector::GenerateSectorID(sectorX, sectorY, sectorID);
    m_pendingRequests[sectorID] = lod;
}

//------------------------------------------------------------------------------------------------
void TerrainService::SectorSelector::LockSectorLOD(efd::SInt16 sectorX, efd::SInt16 sectorY)
{
    NiTerrainSector::SectorID sectorID;
    NiTerrainSector::GenerateSectorID(sectorX, sectorY, sectorID);
    m_lockedSectors.insert(sectorID);
}

//------------------------------------------------------------------------------------------------
void TerrainService::SectorSelector::UnlockSectorLOD(efd::SInt16 sectorX, efd::SInt16 sectorY)
{
    NiTerrainSector::SectorID sectorID;
    NiTerrainSector::GenerateSectorID(sectorX, sectorY, sectorID);
    m_lockedSectors.erase(sectorID);
}

//------------------------------------------------------------------------------------------------
void TerrainService::SectorSelector::UnlockAllSectors()
{
    m_lockedSectors.clear();
}

//------------------------------------------------------------------------------------------------
TerrainService::AssetResolver::AssetResolver(efd::ServiceManager* pServiceManager)
    : NiTerrainAssetResolverBase()
    , IBase()
{
    m_pMessageService = pServiceManager->GetSystemServiceAs<efd::MessageService>();
    m_pAssetLocatorService = pServiceManager->GetSystemServiceAs<AssetLocatorService>();
}

//------------------------------------------------------------------------------------------------
TerrainService::AssetResolver::~AssetResolver()
{
    m_pMessageService = NULL;
    m_pAssetLocatorService = NULL;
}

//------------------------------------------------------------------------------------------------
void TerrainService::AssetResolver::DeleteThis() const 
{
    (const_cast<AssetResolver*>(this))->NiTerrainAssetResolverBase::DeleteThis();
}

//------------------------------------------------------------------------------------------------
void TerrainService::AssetResolver::IncRefCount() const
{
    (const_cast<AssetResolver*>(this))->NiTerrainAssetResolverBase::IncRefCount();
}

//------------------------------------------------------------------------------------------------
void TerrainService::AssetResolver::DecRefCount() const
{
    (const_cast<AssetResolver*>(this))->NiTerrainAssetResolverBase::DecRefCount();
}               

//------------------------------------------------------------------------------------------------
efd::UInt32 TerrainService::AssetResolver::GetRefCount() const
{
    return (this)->NiTerrainAssetResolverBase::GetRefCount();
}

//------------------------------------------------------------------------------------------------
void TerrainService::AssetResolver::Initialize()
{
    EE_ASSERT(m_pAssetLocatorService);
    EE_ASSERT(m_pMessageService);

    // Subscribe to asset locater responses
    m_locaterCategory = GenerateAssetResponseCategory();
    m_pMessageService->Subscribe(this, m_locaterCategory);
}

//------------------------------------------------------------------------------------------------
void TerrainService::AssetResolver::Shutdown()
{
    // Unsubscribe from messages
    m_pMessageService->Unsubscribe(this, m_locaterCategory);
}

//------------------------------------------------------------------------------------------------
void TerrainService::AssetResolver::InternalResolveReference(NiTerrainAssetReference* pReference)
{
    efd::utf8string assetURN = pReference->GetAssetID();
    if (assetURN.empty())
    {
        pReference->MarkResolved(true);
        return;
    }

    // Request to decode the asset URN
    ContextList& contexts = m_pendingQueries[assetURN];
    contexts.push_back(pReference);

    m_pAssetLocatorService->AssetLocate(assetURN, m_locaterCategory);
}

//------------------------------------------------------------------------------------------------
void TerrainService::AssetResolver::HandleAssetLocatorResponse(
    const efd::AssetLocatorResponse* pMessage, efd::Category targetChannel)
{
    // Is it the correct channel?
    if (targetChannel != m_locaterCategory)
        return;

    const efd::utf8string& assetURN = pMessage->GetResponseURI();

    // Ensure a request was made for this context
    EE_ASSERT (m_pendingQueries.find(assetURN) != m_pendingQueries.end());

    ContextList& contexts = m_pendingQueries[assetURN];

    // There must be at least one entry in the list
    EE_ASSERT(contexts.size() != 0);
    if (contexts.size() == 0)
        return;

    const efd::AssetLocatorResponse::AssetURLMap& urlMap = pMessage->GetAssetURLMap();
    NiTerrainAssetReferencePtr spReference = contexts.front();
    efd::utf8string resolvedLocation = "";
    if (urlMap.size() == 0)
    {
        EE_LOG(efd::kGamebryoGeneral0, efd::ILogger::kERR1,
            ("TerrainService::AssetResolver: Could not resolve asset URN (%s). \n"
            "Referenced by: %s", assetURN.c_str(), 
            spReference->GetReferingAssetLocation().c_str()));
    }
    else 
    {
        if (urlMap.size() > 1)
        {
            EE_LOG(efd::kGamebryoGeneral0, efd::ILogger::kERR1,
                ("TerrainService::AssetResolver: Multiple assets found for asset URN (%s). \n"
                "Referenced by: %s", assetURN.c_str(), 
                spReference->GetReferingAssetLocation().c_str()));
        }

        AssetLocatorResponse::AssetURLMap::const_iterator currAsset = urlMap.begin();
        efd::AssetLocatorResponse::AssetLoc loc = currAsset->second;
        resolvedLocation = loc.url;
    }
    
    // Resolve the reference
    spReference->SetResolvedLocation(resolvedLocation);
    spReference->MarkResolved(true);
    contexts.pop_front();

    // Is this URN all done?
    if (contexts.size() == 0)
        m_pendingQueries.erase(assetURN);
}

//------------------------------------------------------------------------------------------------
