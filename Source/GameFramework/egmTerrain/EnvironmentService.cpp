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

#include "egmTerrainPCH.h"

#include <efd/ServiceManager.h>
#include <efd/ecrLogIDs.h>

#include "EnvironmentService.h"
#include "TimeOfDayFile.h"
#include "TTimeOfDayEntityFunctor.h"

#include <efd/ConfigManager.h>
#include <efd/IDs.h>
#include <efd/MessageService.h>
#include <ecr/LightService.h>
#include <efd/PathUtils.h>
#include <efd/TimeType.h>

#include <efd/AssetLocatorService.h>

#include <egf/EntityManager.h>

#include <egf/StandardModelLibraryPropertyIDs.h>
#include <egf/StandardModelLibraryFlatModelIDs.h>

#include <ecr/SceneGraphService.h>

#include <NiEnvironment.h>
#include <NiTimeOfDay.h>

using namespace efd;
using namespace egf;
using namespace ecr;
using namespace egmTerrain;

EE_IMPLEMENT_CONCRETE_CLASS_INFO(EnvironmentService);

// Register for time of day change related messages:
EE_HANDLER(EnvironmentService, HandleKeyframesChangedMessage,
    ToDKeyframesChangedMessage);
EE_HANDLER(EnvironmentService, HandleTimeChangedMessage,
    ToDTimeChangedMessage);

// Register for changes to locally owned entities:
EE_HANDLER_WRAP(EnvironmentService, HandleEntityDiscoverMessage,
    egf::EntityChangeMessage, kMSGID_OwnedEntityAdded);
EE_HANDLER_WRAP(EnvironmentService, HandleEntityRemovedMessage,
    egf::EntityChangeMessage, kMSGID_OwnedEntityRemoved);
EE_HANDLER_WRAP(EnvironmentService, HandleEntityUpdatedMessage,
    egf::EntityChangeMessage, kMSGID_OwnedEntityUpdated);
EE_HANDLER_WRAP(EnvironmentService, HandleEntityEnterWorldMessage,
    egf::EntityChangeMessage, kMSGID_OwnedEntityEnterWorld);
EE_HANDLER_WRAP(EnvironmentService, HandleEntityExitWorldMessage,
    egf::EntityChangeMessage, kMSGID_OwnedEntityExitWorld);

// Register for asset change related messages:
EE_HANDLER_WRAP(EnvironmentService, HandleAssetLocatorResponse,
    efd::AssetLocatorResponse, kMSGID_AssetLocatorResponse);

NiFixedString EnvironmentService::ms_skyRenderClickName;

//------------------------------------------------------------------------------------------------
EnvironmentService::EnvironmentService() :
    m_sunEntityRef(kENTITY_INVALID),
    m_timeOfDayEntityID(kENTITY_INVALID),
    m_animationSpeed(1),
    m_runAnimation(true),
    m_toDInWorld(false),
    m_environmentInWorld(false),
    m_requireRedraw(false),
    m_dayDuration(86400.0f),
    m_timeScaleMultiplier(60.0f),
    m_animatedToD(true),
    m_environmentLoadCategory(kCAT_INVALID),
    m_toDAssetTag("gamebryo-time-of-day"),
    m_handleAssetChanges(true)
{
    m_spDefaultSun = EE_NEW NiDirectionalLight();
}

//------------------------------------------------------------------------------------------------
EnvironmentService::~EnvironmentService()
{
}

//------------------------------------------------------------------------------------------------
const char* EnvironmentService::GetDisplayName() const
{
    return "Environment Service";
}

//------------------------------------------------------------------------------------------------
efd::SyncResult EnvironmentService::OnPreInit(efd::IDependencyRegistrar* pDependencyRegistrar)
{
    pDependencyRegistrar->AddDependency<RenderService>();
    pDependencyRegistrar->AddDependency<ReloadManager>(sdf_Optional);

    ms_skyRenderClickName = "SkyRenderClick";

    m_spLightService = m_pServiceManager->GetSystemServiceAs<ecr::LightService>();
    EE_ASSERT(m_spLightService);

    m_spMessageService = m_pServiceManager->GetSystemServiceAs<efd::MessageService>();
    EE_ASSERT(m_spMessageService);

    m_spSceneGraphService = m_pServiceManager->GetSystemServiceAs<ecr::SceneGraphService>();
    EE_ASSERT(m_spSceneGraphService);

    m_spEntityManager = m_pServiceManager->GetSystemServiceAs<egf::EntityManager>();
    EE_ASSERT(m_spEntityManager);
    if (!m_spEntityManager)
        return SyncResult_Failure;

    m_spRenderService = m_pServiceManager->GetSystemServiceAs<RenderService>();

    m_environmentLoadCategory = GenerateAssetResponseCategory();
    m_spMessageService->Subscribe(this, m_environmentLoadCategory);
    m_spMessageService->Subscribe(this, kCAT_LocalMessage);

    m_spRenderService->AddDelegate(this);

    return SyncResult_Success;
}

//------------------------------------------------------------------------------------------------
efd::AsyncResult EnvironmentService::OnInit()
{
    m_spMessageService->Subscribe(this, ToDMessagesConstants::ms_timeOfDayMessageCategory);

    // Set up a callback to the reload service so we know when terrain related files are changed on
    // disk. This is to facilitate rapid iteration on Terrain assets.
    ReloadManager* pReloadMgr = m_pServiceManager->GetSystemServiceAs<ReloadManager>();
    if (pReloadMgr && m_handleAssetChanges)
    {
        pReloadMgr->RegisterAssetChangeHandler(m_toDAssetTag, this);
    }

    return AsyncResult_Complete;
}

//------------------------------------------------------------------------------------------------
efd::AsyncResult EnvironmentService::OnTick()
{
    Float32 currTime = GetCurrentTime();

    if (m_spTimeOfDay)
    {
        m_spTimeOfDay->Update(currTime);

        if (m_spTimeOfDay->GetActive())
            m_requireRedraw = true;
    }

    if (m_spEnvironment)
    {
        m_spEnvironment->Update(currTime);
        if (!m_spSunLight && m_sunEntityRef != kENTITY_INVALID)
        {
            m_spSunLight = NiDynamicCast(NiDirectionalLight,
                m_spLightService->GetLightFromEntity(m_sunEntityRef));

            if (m_spSunLight)
                m_spEnvironment->SetSun(m_spSunLight);
        }
    }

    if (m_requireRedraw)
    {
        // Make sure a context we can render exist.
        ecr::RenderContextPtr spContext = m_spRenderService->GetActiveRenderContext();
        if (spContext)
            spContext->Invalidate();
        m_requireRedraw = false;
    }

    return AsyncResult_Pending;
}

//------------------------------------------------------------------------------------------------
efd::Float32 EnvironmentService::GetCurrentTime()
{
    return (Float32)m_pServiceManager->GetTime(kCLASSID_GameTimeClock);
}

//------------------------------------------------------------------------------------------------
efd::AsyncResult EnvironmentService::OnShutdown()
{
    ms_skyRenderClickName = NULL;
    if (m_spMessageService != 0)
    {
        m_spMessageService->Unsubscribe(this, kCAT_LocalMessage);
        m_spMessageService->Unsubscribe(this, m_environmentLoadCategory);
        m_spMessageService->Unsubscribe(this, ToDMessagesConstants::ms_timeOfDayMessageCategory);
    }

    // Unregister from the render service and release service pointers.
    if (m_spRenderService)
        m_spRenderService->RemoveDelegate(this);

    ClearAll();

    return AsyncResult_Complete;
}

//------------------------------------------------------------------------------------------------
void EnvironmentService::HandleKeyframesChangedMessage(
    const ToDKeyframesChangedMessage* pMessage,
    efd::Category targetChannel)
{
    EE_ASSERT(pMessage);

    // No need to deal with the message if we do not have a time of day object
    if (!m_spTimeOfDay)
    {
        EE_LOG(efd::kGamebryoGeneral0, efd::ILogger::kERR1,
            ("EnvironmentService: Time of day entity does not yet exist"));
        return;
    }

    efd::vector<float> times;
    efd::vector<efd::utf8string> values;
    NiFixedString propertyName = pMessage->GetPropertyName().c_str();

    efd::utf8string tempValue;
    float tempTime;
    for (efd::UInt32 ui = 0; ui < pMessage->GetNumberOfKeyframes(); ui++)
    {
        pMessage->GetKeyframe(ui, tempTime, tempValue);
        times.push_back(tempTime);
        values.push_back(tempValue);
    }

    efd::vector<efd::utf8string> propertyNameSplit;
    if (pMessage->GetPropertyName().split(".",propertyNameSplit) == 2)
    {
        utf8string entityFileID = propertyNameSplit[0];
        efd::ID128 id;
        efd::ParseHelper<efd::ID128>::FromString(entityFileID, id);
        egf::Entity* pEntity = m_spEntityManager->LookupEntityByDataFileID(id);
        if (IsEntityRegistered(pEntity->GetEntityID()))
        {
            m_spTimeOfDay->UpdatePropertySequence(propertyName, pMessage->GetNumberOfKeyframes(),
                times, values);
        }
    }

    RequestRedraw();
}

//------------------------------------------------------------------------------------------------
void EnvironmentService::HandleTimeChangedMessage(const ToDTimeChangedMessage* pMessage,
    efd::Category targetChannel)
{
    EE_ASSERT(pMessage);
    if (!m_spTimeOfDay)
    {
        EE_LOG(efd::kGamebryoGeneral0, efd::ILogger::kERR1,
            ("EnvironmentService: Time of day entity does not yet exist"));
        return;
    }

    // Make sure this message belongs to the correct category
    if (targetChannel != ToDMessagesConstants::ms_timeOfDayMessageCategory)
    {
        return;
    }

    if (pMessage->GetNewTime() == m_spTimeOfDay->GetTime())
        return;

    m_spTimeOfDay->SetTime(pMessage->GetNewTime());

    m_spRenderService->GetActiveRenderContext()->Invalidate();

    RequestRedraw();
}

//------------------------------------------------------------------------------------------------
void EnvironmentService::HandleEntityDiscoverMessage(const egf::EntityChangeMessage* pMessage,
    efd::Category targetChannel)
{
    EE_ASSERT(pMessage);

    Entity* pEntity = pMessage->GetEntity();
    EE_ASSERT(pEntity);
    EntityID entityID = pEntity->GetEntityID();

    if (!m_spSunLight && pEntity->GetEntityID() == m_sunEntityRef &&
        pEntity->GetModel()->ContainsModel(egf::kFlatModelID_StandardModelLibrary_DirectionalLight))
    {
        m_spSunLight = NiDynamicCast(NiDirectionalLight,
            m_spLightService->GetLightFromEntity(m_sunEntityRef));

        if (m_spEnvironment && m_spSunLight)
            m_spEnvironment->SetSun(m_spSunLight);

        return;
    }

    if (pEntity->GetModel()->ContainsModel(egf::kFlatModelID_StandardModelLibrary_Environment))
    {
        if (m_spEnvironment)
        {
            EE_LOG(efd::kGamebryoGeneral0, efd::ILogger::kERR1,
                ("EnvironmentService: Environment entity already exists within the block."));
        }
        else
        {
            CreateOrLoadEnvironment(pEntity);
            m_todAnimatedEntities.push_back(pEntity->GetEntityID());
        }

        return;
    }

    if (pEntity->GetModel()->ContainsModel(
        egf::kFlatModelID_StandardModelLibrary_TimeOfDayEditable))
    {
        if (!EnsureEntityIsUnique(pEntity))
        {
            EE_LOG(efd::kGamebryoGeneral0, efd::ILogger::kERR1,
                ("EnvironmentService: An entity implementing the same model has already been added"
                " to the TimeOfDay Editor. Only one Time Of Day editable entity per model is "
                "currently supported."));
            return;
        }

        m_todAnimatedEntities.push_back(pEntity->GetEntityID());
        RegisterEntity(pEntity);
        return;
    }

    if (pEntity->GetModel()->ContainsModel(
        egf::kFlatModelID_StandardModelLibrary_TimeOfDay))
    {
        if (m_spTimeOfDay)
        {
            EE_LOG(efd::kGamebryoGeneral0, efd::ILogger::kERR1,
                ("EnvironmentService: A TimeOfDay entity already exists within the block."));
        }
        else
        {
            CreateTimeOfDay(pEntity);
        }
        return;
    }

}

//------------------------------------------------------------------------------------------------
void EnvironmentService::HandleEntityRemovedMessage(const egf::EntityChangeMessage* pMessage,
    efd::Category targetChannel)
{
    EE_ASSERT(pMessage);

    Entity* pEntity = pMessage->GetEntity();
    EE_ASSERT(pEntity);

    // Can we remove this entity?
    if (m_todAnimatedEntities.find(pEntity->GetEntityID()) == m_todAnimatedEntities.end() &&
        pEntity->GetEntityID() != m_timeOfDayEntityID &&
        pEntity->GetEntityID() != m_sunEntityRef)
    {
        return;
    }

    if (pEntity->GetModel()->ContainsModel(egf::kFlatModelID_StandardModelLibrary_Environment))
    {
        UnregisterEntity(pEntity);
        RemoveEnvironment();
        m_todAnimatedEntities.remove(pEntity->GetEntityID());
        return;
    }

    if (pEntity->GetModel()->ContainsModel(egf::kFlatModelID_StandardModelLibrary_TimeOfDayEditable))
    {
        m_todAnimatedEntities.remove(pEntity->GetEntityID());
        UnregisterEntity(pEntity);
        return;
    }

    if (pEntity->GetModel()->ContainsModel(egf::kFlatModelID_StandardModelLibrary_TimeOfDay))
    {
        if (m_todAnimatedEntities.size() != 0)
        {
            efd::list<egf::EntityID>::iterator iter = m_todAnimatedEntities.begin();

            while (iter != m_todAnimatedEntities.end())
            {
                egf::Entity* pEnt = m_spEntityManager->LookupEntity(*iter);
                UnregisterEntity(pEnt);
                ++iter;
            }
        }
        RemoveTimeOfDay();
        return;
    }

    if (m_sunEntityRef == pEntity->GetEntityID())
    {
        if (m_spEnvironment)
            m_spEnvironment->SetSun(m_spDefaultSun);

        m_spSunLight = NULL;
    }
}

//------------------------------------------------------------------------------------------------
void EnvironmentService::HandleEntityEnterWorldMessage(
    const egf::EntityChangeMessage* pMessage,
    efd::Category targetChannel)
{
    Entity* pEntity = pMessage->GetEntity();
    EE_ASSERT(pEntity);

    if (pEntity->GetModel()->ContainsModel(egf::kFlatModelID_StandardModelLibrary_Environment))
    {
        if (m_spEnvironment)
        {
            m_environmentInWorld = true;
            InitializeRenderSurfaces();
        }
    }

    if (pEntity->GetModel()->ContainsModel(egf::kFlatModelID_StandardModelLibrary_TimeOfDay))
    {
        if (m_spTimeOfDay)
        {
            m_toDInWorld = true;
            m_spTimeOfDay->SetActive(m_animatedToD && m_runAnimation && m_toDInWorld);
        }
    }
}

//------------------------------------------------------------------------------------------------
void EnvironmentService::HandleEntityExitWorldMessage(
    const egf::EntityChangeMessage* pMessage,
    efd::Category targetChannel)
{
    Entity* pEntity = pMessage->GetEntity();
    EE_ASSERT(pEntity);

    if (pEntity->GetModel()->ContainsModel(egf::kFlatModelID_StandardModelLibrary_Environment))
    {
        if (m_spEnvironment)
        {
            m_environmentInWorld = false;
            InitializeRenderSurfaces();
        }
    }

    if (pEntity->GetModel()->ContainsModel(egf::kFlatModelID_StandardModelLibrary_TimeOfDay))
    {
        if (m_spTimeOfDay)
        {
            m_toDInWorld = false;
            m_spTimeOfDay->SetActive(m_toDInWorld);
        }
    }
}

//------------------------------------------------------------------------------------------------
void EnvironmentService::HandleEntityUpdatedMessage(const egf::EntityChangeMessage* pMessage,
    efd::Category targetChannel)
{
    Entity* pEntity = pMessage->GetEntity();
    EE_ASSERT(pEntity);

    if (pEntity->GetModel()->ContainsModel(egf::kFlatModelID_StandardModelLibrary_Environment))
    {
        UpdateEnvironmentData(pEntity);
    }
    else if (pEntity->GetModel()->ContainsModel(egf::kFlatModelID_StandardModelLibrary_TimeOfDay))
    {
        UpdateTimeOfDayData(pEntity);
    }
    else if (pEntity->GetModel()->ContainsModel(
        egf::kFlatModelID_StandardModelLibrary_TimeOfDayEditable))
    {
        UpdateTimeOfDayEditableEntity(pEntity);
    }
    else
    {
        return;
    }
}

//------------------------------------------------------------------------------------------------
void EnvironmentService::HandleAssetLocatorResponse(const efd::AssetLocatorResponse* pMessage,
    efd::Category targetChannel)
{
    efd::utf8string uri = pMessage->GetResponseURI();
    const AssetLocatorResponse::AssetURLMap& assets = pMessage->GetAssetURLMap();

    if (assets.empty())
    {
        EE_LOG(efd::kGamebryoGeneral0, efd::ILogger::kERR1,
                ("EnvironmentService::HandleAssetLocatorResponse: "
                "Found 0 assets when looking for tag '%s'.", uri.c_str()));

        return;
    }
    
    for (AssetLocatorResponse::AssetURLMap::const_iterator currAsset = assets.begin();
        currAsset != assets.end(); ++currAsset)
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

        efd::utf8string todFile = loc.url;
        if (targetChannel == m_environmentLoadCategory)
        {
            egmTerrain::TimeOfDayFile* pFile =
                egmTerrain::TimeOfDayFile::Open(todFile.c_str(), m_spEntityManager, false);

            if (!pFile)
            {
                EE_LOG(efd::kGamebryoGeneral0, efd::ILogger::kERR1,
                    ("EnvironmentService::HandleAssetLocatorResponse: "
                    "The file associated with the tag '%s' is invalid or corrupt. Please "
                    "load a different file or save to replace this one.", uri.c_str()));

                return;
            }

            if (m_spTimeOfDay)
            {
                AddPropertiesFromFile(pFile, assetLLID);
            }

            m_currentFileLocateRequest.erase(m_currentFileLocateRequest.find(assetLLID));
            EE_DELETE pFile;
        }
    }
}

//------------------------------------------------------------------------------------------------
void EnvironmentService::HandleAssetChange(const efd::utf8string& assetId,
    const efd::utf8string& tag)
{
    if (tag.find(m_toDAssetTag) != efd::utf8string::npos)
    {
        LocateFile(assetId);
    }
}

//------------------------------------------------------------------------------------------------
void EnvironmentService::OnSurfaceAdded(ecr::RenderService* pService, ecr::RenderSurface* pSurface)
{
    EE_UNUSED_ARG(pService);
    EE_ASSERT(pService);
    EE_ASSERT(pSurface);

    UpdateRenderSurface(pSurface);
}

//------------------------------------------------------------------------------------------------
void EnvironmentService::OnSurfaceRecreated(ecr::RenderService* pService,
    ecr::RenderSurface* pSurface)
{
    EE_UNUSED_ARG(pService);
    EE_ASSERT(pService);
    EE_ASSERT(pSurface);

    UpdateRenderSurface(pSurface);
}

//------------------------------------------------------------------------------------------------
void EnvironmentService::LocateFile(efd::utf8string fileURN)
{
    if (m_currentFileLocateRequest.find(fileURN) != m_currentFileLocateRequest.end())
        return;

    // Fetch the asset locater service
    AssetLocatorService* pAssetLocator =
        m_pServiceManager->GetSystemServiceAs<AssetLocatorService>();

    if (!pAssetLocator || fileURN.empty())
        return;

    // Request to decode the asset URN
    m_currentFileLocateRequest.push_back(fileURN);
    pAssetLocator->AssetLocate(fileURN, m_environmentLoadCategory);
}

//------------------------------------------------------------------------------------------------
void EnvironmentService::AddPropertiesFromFile(TimeOfDayFile* pFile, efd::utf8string assetLLID)
{
    efd::list<efd::ID128>::iterator entIter = 
        m_registeredAssetMap[assetLLID].begin();

    if (entIter == m_registeredAssetMap[assetLLID].end())
    {
        EE_LOG(efd::kGamebryoGeneral0, efd::ILogger::kERR1,
        ("EnvironmentService::HandleAssetLocatorResponse: "
        "The file associated with the tag '%s' does not correspond to any entity. "
        "This can be due to loading a file for which no entity has been created or "
        "the file's logical ID was not registered correctly. Resetting the asset path "
        "for the entity will solve the issue.", pFile->GetFilePath().c_str()));
    }

    while (entIter != m_registeredAssetMap[assetLLID].end())
    {
        efd::vector<ToDProperty> properties = pFile->GetProperties(*entIter);
        efd::vector<ToDProperty>::iterator iter = properties.begin();
        
        while (iter != properties.end())
        {
            if (!m_spTimeOfDay->IsPropertyBound((*iter).m_propertyName.c_str()))
            {
                EE_LOG(efd::kGamebryoGeneral0, efd::ILogger::kERR1,
                    ("EnvironmentService::HandleAssetLocatorResponse: "
                    "The file associated with tag '%s' contains data for a property ('%s') that is not "
                    "registered. This happens if the property's animatable trait has been changed. "
                    "Saving the file will resolve the issue.", pFile->GetFilePath().c_str(), 
                    (*iter).m_propertyName.c_str()));
                ++iter;
                continue;
            }

            efd::vector<efd::utf8string> times;
            efd::vector<efd::utf8string> values;
            pFile->GetPropertyKeyframes((*iter).m_propertyName, times, values);
            m_spTimeOfDay->UpdatePropertySequence(iter->m_propertyName.c_str(),
                times.size(), times, values);

            ++iter;
        }
        ++entIter;
    } 
}

//------------------------------------------------------------------------------------------------
efd::Bool EnvironmentService::CreateOrLoadEnvironment(egf::Entity* pEntity)
{
    EE_ASSERT(pEntity);

    egf::EntityID refID;
    efd::Bool useAutoFog;
    efd::Bool useSunAngles;
    efd::utf8string entityName;
    efd::Float32 azimuth;
    efd::Float32 elevation;
    efd::Float32 blueWaveLength;
    efd::Float32 greenWaveLength;
    efd::Float32 redWaveLength;
    efd::Float32 phaseConstant;
    efd::Float32 mieConstant;
    efd::Float32 rayleighConstant;
    efd::Float32 domeRadius;
    efd::Float32 domeDetail;
    efd::Float32 domeDetailAxisBias;
    efd::Float32 domeDetailHorizonBias;
    efd::Color fogColor;
    efd::Float32 hdrExposure;
    efd::Float32 sunSize;
    efd::Float32 sunIntensity;

    // Get the sunlight entity ref property from the added entity
    if (pEntity->GetPropertyValue(kPropertyID_StandardModelLibrary_EnvironmentSunEntity,
        refID) != PropertyResult_OK)
    {
        EE_LOG(efd::kGamebryoGeneral0, efd::ILogger::kERR1,
            ("EnvironmentService: Error looking up Environment sun entity property."));
        return false;
    }

    // Get the sunlight entity ref property from the added entity
    if (pEntity->GetPropertyValue(kPropertyID_StandardModelLibrary_EnvironmentAutoFogColor,
        useAutoFog) != PropertyResult_OK)
    {
        EE_LOG(efd::kGamebryoGeneral0, efd::ILogger::kERR1,
            ("EnvironmentService: Error looking up Environment auto fog color property."));
        return false;
    }

    // Get the azimuth angle property from the added entity
    if (pEntity->GetPropertyValue(kPropertyID_StandardModelLibrary_EnvironmentSunAzimuthAngle,
        azimuth) != PropertyResult_OK)
    {
        EE_LOG(efd::kGamebryoGeneral0, efd::ILogger::kERR1,
            ("EnvironmentService: Error looking up Environment's azimuth angle property."));
        return false;
    }

    // Get the sunlight entity ref property from the added entity
    if (pEntity->GetPropertyValue(kPropertyID_StandardModelLibrary_EnvironmentSunElevationAngle,
        elevation) != PropertyResult_OK)
    {
        EE_LOG(efd::kGamebryoGeneral0, efd::ILogger::kERR1,
            ("EnvironmentService: Error looking up Environment's bering angle property."));
        return false;
    }

    // Get the sunlight entity ref property from the added entity
    if (pEntity->GetPropertyValue(kPropertyID_StandardModelLibrary_EnvironmentBlueWavelength,
        blueWaveLength) != PropertyResult_OK)
    {
        EE_LOG(efd::kGamebryoGeneral0, efd::ILogger::kERR1,
            ("EnvironmentService: Error looking up Environment's blue wavelength property."));
        return false;
    }

    // Get the sunlight entity ref property from the added entity
    if (pEntity->GetPropertyValue(kPropertyID_StandardModelLibrary_EnvironmentDomeDetail,
        domeDetail) != PropertyResult_OK)
    {
        EE_LOG(efd::kGamebryoGeneral0, efd::ILogger::kERR1,
            ("EnvironmentService: Error looking up Environment dome detail property."));
        return false;
    }

    // Get the sunlight entity ref property from the added entity
    if (pEntity->GetPropertyValue(kPropertyID_StandardModelLibrary_EnvironmentDomeDetailAxisBias,
        domeDetailAxisBias) != PropertyResult_OK)
    {
        EE_LOG(efd::kGamebryoGeneral0, efd::ILogger::kERR1,
            ("EnvironmentService: Error looking up Environment dome detail axis bias property."));
        return false;
    }

    // Get the sunlight entity ref property from the added entity
    if (pEntity->GetPropertyValue(
        kPropertyID_StandardModelLibrary_EnvironmentDomeDetailHorizontalBias,
        domeDetailHorizonBias) != PropertyResult_OK)
    {
        EE_LOG(efd::kGamebryoGeneral0, efd::ILogger::kERR1,
            ("EnvironmentService: Error looking up Environment dome detail horizon bias property."));
        return false;
    }

    // Get the sunlight entity ref property from the added entity
    if (pEntity->GetPropertyValue(kPropertyID_StandardModelLibrary_EnvironmentDomeRadius,
        domeRadius) != PropertyResult_OK)
    {
        EE_LOG(efd::kGamebryoGeneral0, efd::ILogger::kERR1,
            ("EnvironmentService: Error looking up Environment dome radius property."));
        return false;
    }

    // Get the sunlight entity ref property from the added entity
    if (pEntity->GetPropertyValue(kPropertyID_StandardModelLibrary_EnvironmentFogColor,
        fogColor) != PropertyResult_OK)
    {
        EE_LOG(efd::kGamebryoGeneral0, efd::ILogger::kERR1,
            ("EnvironmentService: Error looking up Environment Fog color property."));
        return false;
    }

    // Get the sunlight entity ref property from the added entity
    if (pEntity->GetPropertyValue(kPropertyID_StandardModelLibrary_EnvironmentGreenWavelength,
        greenWaveLength) != PropertyResult_OK)
    {
        EE_LOG(efd::kGamebryoGeneral0, efd::ILogger::kERR1,
            ("EnvironmentService: Error looking up Environment Green Wavelength property."));
        return false;
    }

    // Get the sunlight entity ref property from the added entity
    if (pEntity->GetPropertyValue(kPropertyID_StandardModelLibrary_EnvironmentHDRExposure,
        hdrExposure) != PropertyResult_OK)
    {
        EE_LOG(efd::kGamebryoGeneral0, efd::ILogger::kERR1,
            ("EnvironmentService: Error looking up Environment HDR exporsure property."));
        return false;
    }

    // Get the sunlight entity ref property from the added entity
    if (pEntity->GetPropertyValue(kPropertyID_StandardModelLibrary_EnvironmentMieConstant,
        mieConstant) != PropertyResult_OK)
    {
        EE_LOG(efd::kGamebryoGeneral0, efd::ILogger::kERR1,
            ("EnvironmentService: Error looking up Environment Mie Constant property."));
        return false;
    }

    // Get the sunlight entity ref property from the added entity
    if (pEntity->GetPropertyValue(kPropertyID_StandardModelLibrary_EnvironmentPhaseConstant,
        phaseConstant) != PropertyResult_OK)
    {
        EE_LOG(efd::kGamebryoGeneral0, efd::ILogger::kERR1,
            ("EnvironmentService: Error looking up Environment phase constant property."));
        return false;
    }

    // Get the sunlight entity ref property from the added entity
    if (pEntity->GetPropertyValue(kPropertyID_StandardModelLibrary_EnvironmentRayleighConstant,
        rayleighConstant) != PropertyResult_OK)
    {
        EE_LOG(efd::kGamebryoGeneral0, efd::ILogger::kERR1,
            ("EnvironmentService: Error looking up Environment Rayleigh Constant  property."));
        return false;
    }

    // Get the sunlight entity ref property from the added entity
    if (pEntity->GetPropertyValue(kPropertyID_StandardModelLibrary_EnvironmentRedWavelength,
        redWaveLength) != PropertyResult_OK)
    {
        EE_LOG(efd::kGamebryoGeneral0, efd::ILogger::kERR1,
            ("EnvironmentService: Error looking up Environment Red Wavelength property."));
        return false;
    }

    // Get the sunlight entity ref property from the added entity
    if (pEntity->GetPropertyValue(kPropertyID_StandardModelLibrary_EnvironmentSunIntensity,
        sunIntensity) != PropertyResult_OK)
    {
        EE_LOG(efd::kGamebryoGeneral0, efd::ILogger::kERR1,
            ("EnvironmentService: Error looking up Environment sun intensity property."));
        return false;
    }

    // Get the sunlight entity ref property from the added entity
    if (pEntity->GetPropertyValue(kPropertyID_StandardModelLibrary_EnvironmentSunSize,
        sunSize) != PropertyResult_OK)
    {
        EE_LOG(efd::kGamebryoGeneral0, efd::ILogger::kERR1,
            ("EnvironmentService: Error looking up Environment sun size property."));
        return false;
    }

    if (pEntity->GetPropertyValue(kPropertyID_StandardModelLibrary_EnvironmentUseSunAnlges,
        useSunAngles) != PropertyResult_OK)
    {
        EE_LOG(efd::kGamebryoGeneral0, efd::ILogger::kERR1,
            ("EnvironmentService: Error looking up Environment's use sun angles property."));
        return false;
    }

    // Now that we retrieved all the properties, set the values
    m_sunEntityRef = refID;
    m_environmentEntityName = pEntity->GetModel()->GetName();

    egf::Entity* pSun = m_spEntityManager->LookupEntity(m_sunEntityRef);
    if (pSun && pSun->GetModel()->ContainsModel(
        egf::kFlatModelID_StandardModelLibrary_DirectionalLight))
    {
        m_spSunLight = NiDynamicCast(NiDirectionalLight,
            m_spLightService->GetLightFromEntity(m_sunEntityRef));
    }

    // Create the environment and initialize values
    m_spEnvironment = EE_NEW NiEnvironment();
    m_spEnvironment->SetName(m_environmentEntityName.c_str());
    m_spEnvironment->CreateFogProperty();
    m_spEnvironment->SetAutoCalcFogColor(useAutoFog);
    m_spEnvironment->SetUseSunAngles(useSunAngles);
    m_spEnvironment->SetSunAzimuthAngle(azimuth);
    m_spEnvironment->SetSunElevationAngle(elevation);
    m_spEnvironment->SetFogColor(efd::ColorA(fogColor.r, fogColor.g, fogColor.b,1.0f));

    // Create Atmosphere and initialize values
    NiAtmosphere*  pAtmosphere = m_spEnvironment->CreateAtmosphere();
    pAtmosphere->SetBlueWavelength(blueWaveLength);
    pAtmosphere->SetGreenWavelength(greenWaveLength);
    pAtmosphere->SetHDRExposure(hdrExposure);
    pAtmosphere->SetMieConstant(mieConstant);
    pAtmosphere->SetPhaseConstant(phaseConstant);
    pAtmosphere->SetRayleighConstant(rayleighConstant);
    pAtmosphere->SetRedWavelength(redWaveLength);
    pAtmosphere->SetSunIntensity(sunIntensity);
    pAtmosphere->SetSunSize(sunSize);

    // Create sky and default fog blend stage with initial values
    NiSkyDome* pSky = m_spEnvironment->CreateSkyDome();
    NiSkyFogBlendStage* pFogStage = NiNew NiSkyFogBlendStage();
    pSky->SetBlendStage(3, pFogStage);
    pSky->SetDomeDetail(domeDetail);
    pSky->SetDomeDetailAxisBias(domeDetailAxisBias);
    pSky->SetDomeDetailHorizonBias(domeDetailHorizonBias);
    pSky->SetDomeRadius(domeRadius);

    if (!m_spSunLight)
    {
        if (m_sunEntityRef == kENTITY_INVALID)
        {
            EE_LOG(efd::kGamebryoGeneral0, efd::ILogger::kERR1, ("EnvironmentService: Using "
                "default sun light! Make sure to set the sun entity property to your scene's sun."));
        }
        m_spEnvironment->SetSun(m_spDefaultSun);
    }
    else
    {
        m_spEnvironment->SetSun(m_spSunLight);
    }

    m_spEnvironment->UpdateEffects();
    m_spEnvironment->UpdateProperties();
    m_spEnvironment->Update(0.0f);

    RegisterEntity(pEntity);

    return true;
}

//------------------------------------------------------------------------------------------------
efd::Bool EnvironmentService::CreateTimeOfDay(egf::Entity* pEntity)
{
    efd::Bool animateToD;
    efd::Float32 timeScale;
    efd::Float32 initialTime;
    efd::Float32 dayDuration;

    // Get the environment animation property from the added entity
    if (pEntity->GetPropertyValue(kPropertyID_StandardModelLibrary_TimeOfDayAnimate,
        animateToD) != PropertyResult_OK)
    {
        EE_LOG(efd::kGamebryoGeneral0, efd::ILogger::kERR1,
            ("EnvironmentService: Error looking up Time of day's AnimateEnvironment property."));
        return false;
    }

    // Get the environment fog calculation property from the added entity
    if (pEntity->GetPropertyValue(kPropertyID_StandardModelLibrary_TimeOfDayInitialTime,
        initialTime) != PropertyResult_OK)
    {
        EE_LOG(efd::kGamebryoGeneral0, efd::ILogger::kERR1,
            ("EnvironmentService: Error looking up Time of day initial time property."));
        return false;
    }

    // Get the time scale multiplier property from the added entity
    if (pEntity->GetPropertyValue(kPropertyID_StandardModelLibrary_TimeOfDayTimeScaleMultiplier,
        timeScale) != PropertyResult_OK)
    {
        EE_LOG(efd::kGamebryoGeneral0, efd::ILogger::kERR1,
            ("EnvironmentService: Error looking up Time of day TimeScaleMultiplier property."));
        return false;
    }

    // Get the time scale multiplier property from the added entity
    if (pEntity->GetPropertyValue(kPropertyID_StandardModelLibrary_TimeOfDayDuration,
        dayDuration) != PropertyResult_OK)
    {
        EE_LOG(efd::kGamebryoGeneral0, efd::ILogger::kERR1,
            ("EnvironmentService: Error looking up Time of day duration property."));
        return false;
    }

    // Bind controllers for time of day animation
    // Initialize the time of Day object
    m_spTimeOfDay = NiNew NiTimeOfDay();
    m_dayDuration = dayDuration;
    m_animatedToD = animateToD;
    m_timeScaleMultiplier = timeScale;

    // initialize time of day variables
    m_spTimeOfDay->SetDuration(m_dayDuration);
    m_spTimeOfDay->SetTimeMultiplier(m_timeScaleMultiplier * m_animationSpeed);
    m_spTimeOfDay->SetTime((initialTime / 100.0f) * m_dayDuration);
    m_spTimeOfDay->SetActive(m_animatedToD && m_runAnimation && m_toDInWorld);

    if (m_todAnimatedEntities.size() != 0)
    {
        efd::list<egf::EntityID>::iterator iter = m_todAnimatedEntities.begin();

        while (iter != m_todAnimatedEntities.end())
        {
            egf::Entity* pEnt = m_spEntityManager->LookupEntity(*iter);
            RegisterEntity(pEnt);
            ++iter;
        }
    }

    m_spTimeOfDay->Update(0.0f);
    m_timeOfDayEntityID = pEntity->GetEntityID();

    return true;
}

//------------------------------------------------------------------------------------------------
bool EnvironmentService::RegisterEntity(egf::Entity* pEntity)
{    
    efd::utf8string modelID;
    efd::ParseHelper<efd::ID128>::ToString(pEntity->GetDataFileID(), modelID);
    if (!m_spTimeOfDay || IsEntityRegistered(pEntity->GetEntityID()))
    {
        return false;
    }

    efd::AssetID filename;
    if (pEntity->GetPropertyValue(kPropertyID_StandardModelLibrary_TimeOfDayAssetPath,
        filename) == PropertyResult_OK)
    {
        if (!filename.IsValidURN())
        {
            EE_LOG(efd::kGamebryoGeneral0, efd::ILogger::kERR1,
                ("EnvironmentService: A TimeOfDayEditable entity can only be registered to "
                "the time of day if a valid TimeOfDayAssetPath has been defined."));
            return false;
        }
    }

    bool result = false;
    if (pEntity->GetModel()->ContainsModel(egf::kFlatModelID_StandardModelLibrary_TimeOfDayEditable))
    {
        efd::utf8string nextKey;
        if (pEntity->GetNextPropertyKey(
            kPropertyID_StandardModelLibrary_TimeOfDayAnimatablePropertyList, "", nextKey) !=
            PropertyResult_OK)
        {
            EE_LOG(efd::kGamebryoGeneral0, efd::ILogger::kERR1,
                ("EnvironmentService: Failed to register entity as there are no properties to "
                "register"));
            return false;
        }

        while (!nextKey.empty())
        {
            efd::utf8string propertyName;
            if (pEntity->GetPropertyValue(
                kPropertyID_StandardModelLibrary_TimeOfDayAnimatablePropertyList, nextKey,
                propertyName) != PropertyResult_OK)
            {
                EE_LOG(efd::kGamebryoGeneral0, efd::ILogger::kERR1,
                    ("EnvironmentService: Failed to register entity as service could not retrieve"
                    " a property name in the TimeOfDayAnimatablePropertyList"));
                return false;
            }

            const egf::PropertyDescriptor* pPropDesc = pEntity->GetModel()->GetPropertyDescriptor(
                propertyName);

            efd::utf8string todPropName(efd::Formatted,"%s.%s", modelID.c_str(),
                propertyName.c_str());
            TTimeOfDayEntityFunctor<efd::Float32> floatFunctor;
            TTimeOfDayEntityFunctor<efd::ColorA> colorAFunctor;
            switch (pPropDesc->GetDataClassID())
            {
            case efd::kTypeID_Float32:
                floatFunctor = TTimeOfDayEntityFunctor<efd::Float32>(m_spEntityManager,
                    pEntity->GetEntityID(), pPropDesc->GetPropertyID());
                m_spTimeOfDay->BindProperty<TTimeOfDayEntityFunctor<efd::Float32>,
                    float>(todPropName.c_str(), floatFunctor);
                break;

            case efd::kTypeID_Color:
            case efd::kTypeID_ColorA:
                colorAFunctor = TTimeOfDayEntityFunctor<efd::ColorA>(m_spEntityManager,
                    pEntity->GetEntityID(), pPropDesc->GetPropertyID());
                m_spTimeOfDay->BindProperty<TTimeOfDayEntityFunctor<efd::ColorA>, NiColorA>(
                    todPropName.c_str(), colorAFunctor);
                break;
            default:
                EE_LOG(efd::kGamebryoGeneral0, efd::ILogger::kERR1,
                    ("EnvironmentService: Can not register an unsupported property type. Ensure "
                    "TimeOfDayAnimatable properties are Color , ColorA or Float32."));
                break;
            }

            if (pEntity->GetNextPropertyKey(
                kPropertyID_StandardModelLibrary_TimeOfDayAnimatablePropertyList,
                nextKey, nextKey) == PropertyResult_KeyNotFound)
            {
                EE_LOG(efd::kGamebryoGeneral0, efd::ILogger::kERR1,
                    ("EnvironmentService: Failed to register entity as a property key could not be "
                    "found."));
                return false;
            }
        }

        result = true;
    }

    if (result)
    {
        // We have now registered with the time of day. We need to load the associated tod file
        LocateFile(filename);
        m_registeredAssetMap[filename].push_back(pEntity->GetDataFileID());
        m_registeredEntities.push_back(pEntity->GetEntityID());
    }

    return result;
}

//------------------------------------------------------------------------------------------------
bool EnvironmentService::UnregisterEntity(egf::Entity* pEntity)
{
    efd::utf8string modelID;
    efd::ParseHelper<efd::ID128>::ToString(pEntity->GetDataFileID(), modelID);
    if (!m_spTimeOfDay || !IsEntityRegistered(pEntity->GetEntityID()))
        return false;

    if (pEntity->GetModel()->ContainsModel(egf::kFlatModelID_StandardModelLibrary_TimeOfDayEditable))
    {
        efd::utf8string nextKey;

        // Clear the asset map from the given entity
        AssetMap::iterator assetIter = m_registeredAssetMap.begin();
        while (assetIter != m_registeredAssetMap.end())
        {
            efd::list< efd::ID128 >::iterator listIter = 
                assetIter->second.find(pEntity->GetDataFileID());
            if (listIter != assetIter->second.end())
            {
                assetIter->second.erase(listIter);
                if (assetIter->second.empty())
                {
                    m_registeredAssetMap.erase(assetIter);
                    continue;
                }
            }
            ++assetIter;
        }

        NiTObjectSet<NiFixedString> todProp;
        m_spTimeOfDay->GetPropertyNames(todProp);

        NiUInt32 size = todProp.GetSize();
        efd::vector<utf8string> toDPropList;
        while (size != 0)
        {
            efd::utf8string propName = (const char*)todProp.GetAt(size - 1);
            efd::vector<efd::utf8string> splittedName;
            propName.split(".", splittedName);

            if (splittedName[0] == modelID)
            {
                toDPropList.push_back(propName);
            }

            --size;
        }

        m_spTimeOfDay->RemoveToDProperties(toDPropList);

        efd::vector<egf::EntityID>::iterator registeredIter = 
            m_registeredEntities.find(pEntity->GetEntityID());
        m_registeredEntities.erase(registeredIter);
        return true;
    }

    return false;
}

//------------------------------------------------------------------------------------------------
bool EnvironmentService::CanRenderEnvironment(ecr::RenderSurface* pSurface)
{
    EE_UNUSED_ARG(pSurface);

    return m_spEnvironment && m_environmentInWorld;
}

//------------------------------------------------------------------------------------------------
NiRenderClick* EnvironmentService::RetrieveSkyRenderClick(ecr::RenderSurface* pSurface)
{
    NiCamera* pkSurfaceCamera = pSurface->GetCamera();
    /*if (pkSurfaceCamera->GetViewFrustum().m_bOrtho)
        return NULL;*/

    NiRenderClick* pSkyRenderClick = m_spEnvironment->CreateSkyRenderClick(pkSurfaceCamera);
    if (!pSkyRenderClick)
        return NULL;

    pSkyRenderClick->SetName(ms_skyRenderClickName);
    return pSkyRenderClick;
}

//------------------------------------------------------------------------------------------------
efd::Bool EnvironmentService::UpdateRenderSurface(ecr::RenderSurface* pSurface)
{
    EE_ASSERT(pSurface);

    // Get the main render step, since we intend to change its render click
    NiDefaultClickRenderStep* pRenderStep =
        (NiDefaultClickRenderStep*)pSurface->GetRenderStep();
    NiTPointerList<NiRenderClickPtr>& clicks = pRenderStep->GetRenderClickList();

    // Remove the sky render click if it was added
    NiRenderClick* pFirstClick = clicks.GetHead();
    if (pFirstClick->GetName() == ms_skyRenderClickName)
    {
        clicks.RemoveHead();
        pFirstClick = clicks.GetHead();
        EE_ASSERT(pFirstClick);
        pFirstClick->SetClearAllBuffers(true);
    }

    if (!CanRenderEnvironment(pSurface))
        return false;

    NiRenderClick* pSkyRenderClick = RetrieveSkyRenderClick(pSurface);
    if (!pSkyRenderClick)
        return false;
    
    // Ensure the click we are adding the sky before does not clear the buffers
    pFirstClick->SetClearAllBuffers(false);

    // Ensure all clicks use the same render target group
    pSkyRenderClick->SetRenderTargetGroup(pFirstClick->GetRenderTargetGroup());
    pRenderStep->PrependRenderClick(pSkyRenderClick);

    return true;
}

//------------------------------------------------------------------------------------------------
efd::Bool EnvironmentService::InitializeRenderSurfaces()
{
    if (!m_spRenderService)
        return false;

    // Parse all the available render contexts and all the render surfaces to add the environment
    // render click
    efd::UInt32 contextIndex = 0;
    for (; contextIndex < m_spRenderService->GetRenderContextCount(); ++contextIndex)
    {
        ecr::RenderContextPtr spContext = m_spRenderService->GetRenderContextAt(contextIndex);
        efd::UInt32 surfaceIndex = 0;
        for (; surfaceIndex < spContext->GetRenderSurfaceCount(); ++surfaceIndex)
        {
            ecr::RenderSurfacePtr spSurface = spContext->GetRenderSurfaceAt(surfaceIndex);

            if (spSurface)
            {
                UpdateRenderSurface(spSurface);
            }
        }
    }

    return true;
}

//------------------------------------------------------------------------------------------------
efd::Bool EnvironmentService::EnsureEntityIsUnique(egf::Entity* pEntity)
{
    efd::list<egf::EntityID>::iterator iter = m_todAnimatedEntities.begin();

    while (iter != m_todAnimatedEntities.end())
    {
        // make sure this entity wasn't added previously
        if (*iter == pEntity->GetEntityID())
            return false;

       ++iter;
    }

    return true;
}

//------------------------------------------------------------------------------------------------
void EnvironmentService::UpdateTimeOfDayEditableEntity(egf::Entity* pEntity)
{
    bool updateRegistration = false;

    // the entity's path was changed
    if (pEntity->IsDirty(kPropertyID_StandardModelLibrary_TimeOfDayAssetPath))
    {
        efd::AssetID filename;
        if (pEntity->GetPropertyValue(kPropertyID_StandardModelLibrary_TimeOfDayAssetPath,
            filename) != PropertyResult_OK)
        {
            EE_LOG(efd::kGamebryoGeneral0, efd::ILogger::kERR1,
                ("EnvironmentService: Error looking up Environment Asset Path property."));
            return;
        }

        updateRegistration = true;
    }

    if (pEntity->IsDirty(kPropertyID_StandardModelLibrary_TimeOfDayAnimatablePropertyList))
    {
        updateRegistration = true;
    }

    if (updateRegistration)
    {
        // First remove the current data for this entity
        UnregisterEntity(pEntity);
        // Now register it again to trigger parse through the entity's property list
        RegisterEntity(pEntity);
    }
}

//--------------------------------------------------------------------------------------------------
void EnvironmentService::UpdateEnvironmentData(egf::Entity* pEntity)
{
    if (!m_spEnvironment)
        return; 

    UpdateTimeOfDayEditableEntity(pEntity);
    
    // The environment's sunlight was changed
    if (pEntity->IsDirty(kPropertyID_StandardModelLibrary_EnvironmentSunEntity))
    {
        egf::EntityID refID;
        if (pEntity->GetPropertyValue(kPropertyID_StandardModelLibrary_EnvironmentSunEntity,
            refID) != PropertyResult_OK)
        {
            EE_LOG(efd::kGamebryoGeneral0, efd::ILogger::kERR1,
                ("EnvironmentService: Error looking up Environment sun entity property."));
            return;
        }

        m_sunEntityRef = refID;
        egf::Entity* pSun = m_spEntityManager->LookupEntity(m_sunEntityRef);
        if (pSun && pSun->GetModel()->ContainsModel(
            egf::kFlatModelID_StandardModelLibrary_DirectionalLight))
        {
            m_spSunLight = NiDynamicCast(NiDirectionalLight,
                m_spLightService->GetLightFromEntity(m_sunEntityRef));
            m_spEnvironment->SetSun(m_spSunLight);
        }
        else
        {
            if (m_sunEntityRef == kENTITY_INVALID)
            {
                EE_LOG(efd::kGamebryoGeneral0, efd::ILogger::kERR1, ("EnvironmentService: Using "
                    "default sun light! Make sure to set the sun entity property to your "
                    "scene's sun."));
            }
            m_spSunLight = NULL;
            m_spEnvironment->SetSun(m_spDefaultSun);
        }
    }

    // Whether we calculate the fog's color or not has changed
    if (pEntity->IsDirty(kPropertyID_StandardModelLibrary_EnvironmentAutoFogColor))
    {
        efd::Bool useFog;
        if (pEntity->GetPropertyValue(kPropertyID_StandardModelLibrary_EnvironmentAutoFogColor,
            useFog) != PropertyResult_OK)
        {
            EE_LOG(efd::kGamebryoGeneral0, efd::ILogger::kERR1,
                ("EnvironmentService: Error looking up Environment AutomaticFogColor property."));
            return;
        }

        m_spEnvironment->SetAutoCalcFogColor(useFog);
    }

    if (pEntity->IsDirty(kPropertyID_StandardModelLibrary_EnvironmentSunAzimuthAngle))
    {
        efd::Float32 value;
        if (pEntity->GetPropertyValue(kPropertyID_StandardModelLibrary_EnvironmentSunAzimuthAngle,
            value) != PropertyResult_OK)
        {
            EE_LOG(efd::kGamebryoGeneral0, efd::ILogger::kERR1,
                ("EnvironmentService: Error looking up Environment's azimuth angle property."));
            return;
        }

        m_spEnvironment->SetSunAzimuthAngle(value);
    }

    if (pEntity->IsDirty(kPropertyID_StandardModelLibrary_EnvironmentSunElevationAngle))
    {
        efd::Float32 value;
        if (pEntity->GetPropertyValue(kPropertyID_StandardModelLibrary_EnvironmentSunElevationAngle,
            value) != PropertyResult_OK)
        {
            EE_LOG(efd::kGamebryoGeneral0, efd::ILogger::kERR1,
                ("EnvironmentService: Error looking up Environment's bering angle property."));
            return;
        }
        m_spEnvironment->SetSunElevationAngle(value);
    }

    if (pEntity->IsDirty(kPropertyID_StandardModelLibrary_EnvironmentUseSunAnlges))
    {
        efd::Bool value;
        if (pEntity->GetPropertyValue(kPropertyID_StandardModelLibrary_EnvironmentUseSunAnlges,
            value) != PropertyResult_OK)
        {
            EE_LOG(efd::kGamebryoGeneral0, efd::ILogger::kERR1,
                ("EnvironmentService: Error looking up Environment's use sun angles property."));
            return;
        }
        m_spEnvironment->SetUseSunAngles(value);
    }

    if (pEntity->IsDirty(kPropertyID_StandardModelLibrary_EnvironmentBlueWavelength))
    {
        efd::Float32 value;
        if (pEntity->GetPropertyValue(kPropertyID_StandardModelLibrary_EnvironmentBlueWavelength,
            value) != PropertyResult_OK)
        {
            EE_LOG(efd::kGamebryoGeneral0, efd::ILogger::kERR1,
                ("EnvironmentService: Error looking up Environment's blue wavelength property."));
            return;
        }
        m_spEnvironment->GetAtmosphere()->SetBlueWavelength(value);
    }

    if (pEntity->IsDirty(kPropertyID_StandardModelLibrary_EnvironmentDomeDetail))
    {
        efd::Float32 value;
        if (pEntity->GetPropertyValue(kPropertyID_StandardModelLibrary_EnvironmentDomeDetail,
            value) != PropertyResult_OK)
        {
            EE_LOG(efd::kGamebryoGeneral0, efd::ILogger::kERR1,
                ("EnvironmentService: Error looking up Environment dome detail property."));
            return;
        }
        NiSkyDome* pSky = (NiSkyDome*)m_spEnvironment->GetSky();
        if (pSky)
            pSky->SetDomeDetail(value);
    }

    if (pEntity->IsDirty(kPropertyID_StandardModelLibrary_EnvironmentDomeDetailAxisBias))
    {
        efd::Float32 value;
        if (pEntity->GetPropertyValue(kPropertyID_StandardModelLibrary_EnvironmentDomeDetailAxisBias,
            value) != PropertyResult_OK)
        {
            EE_LOG(efd::kGamebryoGeneral0, efd::ILogger::kERR1,
                ("EnvironmentService: Error looking up Environment dome detail axis bias property."));
            return;
        }
        NiSkyDome* pSky = (NiSkyDome*)m_spEnvironment->GetSky();
        if (pSky)
            pSky->SetDomeDetailAxisBias(value);
    }

    if (pEntity->IsDirty(kPropertyID_StandardModelLibrary_EnvironmentDomeDetailHorizontalBias))
    {
        efd::Float32 value;
        if (pEntity->GetPropertyValue(
            kPropertyID_StandardModelLibrary_EnvironmentDomeDetailHorizontalBias,
            value) != PropertyResult_OK)
        {
            EE_LOG(efd::kGamebryoGeneral0, efd::ILogger::kERR1,
                ("EnvironmentService: Error looking up Environment dome detail horizon bias property."));
            return;
        }

        NiSkyDome* pSky = (NiSkyDome*)m_spEnvironment->GetSky();
        if (pSky)
            pSky->SetDomeDetailHorizonBias(value);
    }

    if (pEntity->IsDirty(kPropertyID_StandardModelLibrary_EnvironmentDomeRadius))
    {
        efd::Float32 value;
        if (pEntity->GetPropertyValue(kPropertyID_StandardModelLibrary_EnvironmentDomeRadius,
            value) != PropertyResult_OK)
        {
            EE_LOG(efd::kGamebryoGeneral0, efd::ILogger::kERR1,
                ("EnvironmentService: Error looking up Environment dome radius property."));
            return;
        }

        NiSkyDome* pSky = (NiSkyDome*)m_spEnvironment->GetSky();
        if (pSky)
            pSky->SetDomeRadius(value);
    }

    if (pEntity->IsDirty(kPropertyID_StandardModelLibrary_EnvironmentFogColor))
    {
        efd::Color value;
        if (pEntity->GetPropertyValue(kPropertyID_StandardModelLibrary_EnvironmentFogColor,
            value) != PropertyResult_OK)
        {
            EE_LOG(efd::kGamebryoGeneral0, efd::ILogger::kERR1,
                ("EnvironmentService: Error looking up Environment Fog color property."));
            return;
        }
        m_spEnvironment->SetFogColor(efd::ColorA(value.r, value.g, value.b, 1.0f));
    }

    if (pEntity->IsDirty(kPropertyID_StandardModelLibrary_EnvironmentGreenWavelength))
    {
        efd::Float32 value;
        if (pEntity->GetPropertyValue(kPropertyID_StandardModelLibrary_EnvironmentGreenWavelength,
            value) != PropertyResult_OK)
        {
            EE_LOG(efd::kGamebryoGeneral0, efd::ILogger::kERR1,
                ("EnvironmentService: Error looking up Environment Green Wavelength property."));
            return;
        }
        m_spEnvironment->GetAtmosphere()->SetGreenWavelength(value);
    }

    if (pEntity->IsDirty(kPropertyID_StandardModelLibrary_EnvironmentHDRExposure))
    {
        efd::Float32 value;
        if (pEntity->GetPropertyValue(kPropertyID_StandardModelLibrary_EnvironmentHDRExposure,
            value) != PropertyResult_OK)
        {
            EE_LOG(efd::kGamebryoGeneral0, efd::ILogger::kERR1,
                ("EnvironmentService: Error looking up Environment HDR exporsure property."));
            return;
        }
        m_spEnvironment->GetAtmosphere()->SetHDRExposure(value);
    }

    if (pEntity->IsDirty(kPropertyID_StandardModelLibrary_EnvironmentMieConstant))
    {
        efd::Float32 value;
        if (pEntity->GetPropertyValue(kPropertyID_StandardModelLibrary_EnvironmentMieConstant,
            value) != PropertyResult_OK)
        {
            EE_LOG(efd::kGamebryoGeneral0, efd::ILogger::kERR1,
                ("EnvironmentService: Error looking up Environment Mie Constant property."));
            return;
        }
        m_spEnvironment->GetAtmosphere()->SetMieConstant(value);
    }

    if (pEntity->IsDirty(kPropertyID_StandardModelLibrary_EnvironmentPhaseConstant))
    {
        efd::Float32 value;
        if (pEntity->GetPropertyValue(kPropertyID_StandardModelLibrary_EnvironmentPhaseConstant,
            value) != PropertyResult_OK)
        {
            EE_LOG(efd::kGamebryoGeneral0, efd::ILogger::kERR1,
                ("EnvironmentService: Error looking up Environment phase constant property."));
            return;
        }
        m_spEnvironment->GetAtmosphere()->SetPhaseConstant(value);
    }

    if (pEntity->IsDirty(kPropertyID_StandardModelLibrary_EnvironmentRayleighConstant))
    {
        efd::Float32 value;
        if (pEntity->GetPropertyValue(kPropertyID_StandardModelLibrary_EnvironmentRayleighConstant,
            value) != PropertyResult_OK)
        {
            EE_LOG(efd::kGamebryoGeneral0, efd::ILogger::kERR1,
                ("EnvironmentService: Error looking up Environment Rayleigh Constant  property."));
            return;
        }
        m_spEnvironment->GetAtmosphere()->SetRayleighConstant(value);
    }

    if (pEntity->IsDirty(kPropertyID_StandardModelLibrary_EnvironmentRedWavelength))
    {
        efd::Float32 value;
        if (pEntity->GetPropertyValue(kPropertyID_StandardModelLibrary_EnvironmentRedWavelength,
            value) != PropertyResult_OK)
        {
            EE_LOG(efd::kGamebryoGeneral0, efd::ILogger::kERR1,
                ("EnvironmentService: Error looking up Environment Red Wavelength property."));
            return;
        }
        m_spEnvironment->GetAtmosphere()->SetRedWavelength(value);
    }

    if (pEntity->IsDirty(kPropertyID_StandardModelLibrary_EnvironmentSunIntensity))
    {
        efd::Float32 value;
        if (pEntity->GetPropertyValue(kPropertyID_StandardModelLibrary_EnvironmentSunIntensity,
            value) != PropertyResult_OK)
        {
            EE_LOG(efd::kGamebryoGeneral0, efd::ILogger::kERR1,
                ("EnvironmentService: Error looking up Environment sun intensity property."));
            return;
        }
        m_spEnvironment->GetAtmosphere()->SetSunIntensity(value);
    }

    if (pEntity->IsDirty(kPropertyID_StandardModelLibrary_EnvironmentSunSize))
    {
        efd::Float32 value;
        if (pEntity->GetPropertyValue(kPropertyID_StandardModelLibrary_EnvironmentSunSize,
            value) != PropertyResult_OK)
        {
            EE_LOG(efd::kGamebryoGeneral0, efd::ILogger::kERR1,
                ("EnvironmentService: Error looking up Environment sun size property."));
            return;
        }
        m_spEnvironment->GetAtmosphere()->SetSunSize(value);
    }

}

//------------------------------------------------------------------------------------------------
void EnvironmentService::UpdateTimeOfDayData(egf::Entity* pEntity)
{
    // Whether the environment is animatable or not has changed
    if (pEntity->IsDirty(kPropertyID_StandardModelLibrary_TimeOfDayAnimate))
    {
        efd::Bool animate;
        if (pEntity->GetPropertyValue(kPropertyID_StandardModelLibrary_TimeOfDayAnimate,
            animate) != PropertyResult_OK)
        {
            EE_LOG(efd::kGamebryoGeneral0, efd::ILogger::kERR1,
                ("EnvironmentService: Error looking up Environment AnimateEnvironment property."));
            return;
        }

        m_animatedToD = animate;
        if (m_spTimeOfDay)
        {
            m_spTimeOfDay->SetActive(m_animatedToD && m_runAnimation && m_toDInWorld);
        }
    }

    // The time scale has changed
    if (pEntity->IsDirty(kPropertyID_StandardModelLibrary_TimeOfDayTimeScaleMultiplier))
    {
        efd::Float32 timeScale;
        if (pEntity->GetPropertyValue(kPropertyID_StandardModelLibrary_TimeOfDayTimeScaleMultiplier,
            timeScale) != PropertyResult_OK)
        {
            EE_LOG(efd::kGamebryoGeneral0, efd::ILogger::kERR1,
                ("EnvironmentService: Error looking up Time of day's TimeScaleMultiplier property."));
            return;
        }

        m_timeScaleMultiplier = timeScale;
        if (m_spTimeOfDay)
        {
            m_spTimeOfDay->SetTimeMultiplier(m_timeScaleMultiplier * m_animationSpeed);
        }
    }

    // The day duration has changed
    if (pEntity->IsDirty(kPropertyID_StandardModelLibrary_TimeOfDayDuration))
    {
        efd::Float32 timeScale;
        if (pEntity->GetPropertyValue(kPropertyID_StandardModelLibrary_TimeOfDayDuration,
            timeScale) != PropertyResult_OK)
        {
            EE_LOG(efd::kGamebryoGeneral0, efd::ILogger::kERR1,
                ("EnvironmentService: Error looking up Time of day's day duration property."));
            return;
        }

        m_dayDuration = timeScale;
        if (m_spTimeOfDay)
        {
            m_spTimeOfDay->SetDuration(m_dayDuration);
        }
    }
}

//------------------------------------------------------------------------------------------------
void EnvironmentService::RemoveEnvironment()
{
    if (m_spEnvironment)
    {
        m_spEnvironment = NULL;
    }

    InitializeRenderSurfaces();
}

//------------------------------------------------------------------------------------------------
void EnvironmentService::RemoveTimeOfDay()
{
    if (m_spTimeOfDay)
    {
        m_spTimeOfDay = NULL;
    }
}

//------------------------------------------------------------------------------------------------
void EnvironmentService::ClearAll()
{
    m_spRenderService = NULL;
    m_spMessageService = NULL;

    RemoveTimeOfDay();
    RemoveEnvironment();
    m_spSunLight = NULL;
    m_spDefaultSun = NULL;

    m_spEntityManager = NULL;
    m_spSceneGraphService = NULL;

    m_currentFileLocateRequest.clear();
    m_todAnimatedEntities.clear();
}

//------------------------------------------------------------------------------------------------
bool EnvironmentService::IsEntityRegistered(egf::EntityID entityID)
{
    return m_registeredEntities.find(entityID) != m_registeredEntities.end();
}
