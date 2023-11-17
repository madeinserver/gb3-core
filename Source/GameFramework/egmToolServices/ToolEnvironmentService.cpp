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

#include <egf/egfLogIDs.h>
#include <egf/StandardModelLibraryPropertyIDs.h>
#include <egf/StandardModelLibraryFlatModelIDs.h>

#include <egmTerrain/EnvironmentService.h>
#include <egmTerrain/TimeOfDayFile.h>
#include "ToolEnvironmentService.h"
#include "PropertySwapProcessor.h"
#include <NiTimeOfDay.h>

using namespace efd;
using namespace egf;
using namespace ecr;
using namespace egmToolServices;
using namespace egmTerrain;

EE_IMPLEMENT_CONCRETE_CLASS_INFO(ToolEnvironmentService);
EE_HANDLER(ToolEnvironmentService, HandleAnimationChangedMessage,
    ToDAnimationChangedMessage);

//------------------------------------------------------------------------------------------------
ToolEnvironmentService::ToolEnvironmentService()
{
    m_runAnimation = false;
    m_animatedToD = true;

    // Override the handling of asset changes. We don't want to handle them in tool mode
    m_handleAssetChanges = false;
}

//------------------------------------------------------------------------------------------------
void ToolEnvironmentService::OnServiceRegistered(efd::IAliasRegistrar* pAliasRegistrar)
{
    pAliasRegistrar->AddIdentity<EnvironmentService>();
    EnvironmentService::OnServiceRegistered(pAliasRegistrar);
}

//------------------------------------------------------------------------------------------------
efd::AsyncResult ToolEnvironmentService::OnInit()
{
    if (EnvironmentService::OnInit() == efd::AsyncResult_Failure)
        return efd::AsyncResult_Failure;

    return efd::AsyncResult_Complete;
}

//------------------------------------------------------------------------------------------------
efd::AsyncResult ToolEnvironmentService::OnTick()
{
    if (efd::AsyncResult_Failure == EnvironmentService::OnTick())
        return efd::AsyncResult_Failure;

    // We need to advertise to connected application that the time has changed.
    if (m_runAnimation && m_spTimeOfDay)
    {
        RaiseTimeChanged(m_spTimeOfDay->GetTime());
    }

    return efd::AsyncResult_Pending;
}

//------------------------------------------------------------------------------------------------
efd::AsyncResult ToolEnvironmentService::OnShutdown()
{
    return EnvironmentService::OnShutdown();
}

//------------------------------------------------------------------------------------------------
efd::Float32 ToolEnvironmentService::GetCurrentTime()
{
    return (Float32) m_pServiceManager->GetServiceManagerTime();
}

//------------------------------------------------------------------------------------------------
void ToolEnvironmentService::RegisterTimeChangedCallback(TimeChangedCallback callback)
{
    m_registeredTimeChangedCallbacks.push_back(callback);
}

//------------------------------------------------------------------------------------------------
void ToolEnvironmentService::UnregisterTimeChangedCallback(TimeChangedCallback callback)
{
    m_registeredTimeChangedCallbacks.remove(callback);
}

//------------------------------------------------------------------------------------------------
void ToolEnvironmentService::RaiseTimeChanged(float newTime)
{
    efd::list<TimeChangedCallback>::iterator first = m_registeredTimeChangedCallbacks.begin();
    efd::list<TimeChangedCallback>::iterator last = m_registeredTimeChangedCallbacks.end();
    while (first != last)
    {
        (*first)(newTime);
        ++first;
    }
}

//------------------------------------------------------------------------------------------------
void ToolEnvironmentService::RegisterKeyframeLoadedCallback(KeyframeLoadedCallback callback)
{
    m_registeredKeyframeLoadedCallbacks.push_back(callback);
}

//------------------------------------------------------------------------------------------------
void ToolEnvironmentService::UnregisterKeyframeLoadedCallback(KeyframeLoadedCallback callback)
{
    m_registeredKeyframeLoadedCallbacks.remove(callback);
}

//------------------------------------------------------------------------------------------------
void ToolEnvironmentService::RaiseKeyframeLoaded(ToDProperty propertyToUpdate,
    const efd::vector<efd::utf8string>& times, const efd::vector<efd::utf8string>& values)
{
    efd::list<KeyframeLoadedCallback>::iterator first = m_registeredKeyframeLoadedCallbacks.begin();
    efd::list<KeyframeLoadedCallback>::iterator last = m_registeredKeyframeLoadedCallbacks.end();
    while (first != last)
    {
        (*first)(propertyToUpdate, times, values);
        ++first;
    }
}

//------------------------------------------------------------------------------------------------
void ToolEnvironmentService::RegisterToDPropertiesChangedCallback(
    ToDPropertiesChangedCallback callback)
{
    m_registeredToDPropertiesChangedCallbacks.push_back(callback);
}

//------------------------------------------------------------------------------------------------
void ToolEnvironmentService::UnregisterToDPropertiesChangedCallback(
    ToDPropertiesChangedCallback callback)
{
    m_registeredToDPropertiesChangedCallbacks.remove(callback);
}

//------------------------------------------------------------------------------------------------
void ToolEnvironmentService::RaiseToDPropertiesChanged(
    const efd::vector<ToolEnvironmentService::ToDProperty>& properties)
{
    efd::list<ToDPropertiesChangedCallback>::iterator first =
        m_registeredToDPropertiesChangedCallbacks.begin();
    efd::list<ToDPropertiesChangedCallback>::iterator last =
        m_registeredToDPropertiesChangedCallbacks.end();
    while (first != last)
    {
        (*first)(properties);
        ++first;
    }
}

//------------------------------------------------------------------------------------------------
bool ToolEnvironmentService::SaveTimeOfDay(efd::utf8string fileName, efd::utf8string entityName, 
    efd::ID128 entityFileID)
{
    // First find the asset to save.
    if (fileName.empty())
    {
        if (!m_assetFilePath.find(entityFileID, fileName))
            return false;
    }

    // Open the file
    egmTerrain::TimeOfDayFile* pFile = egmTerrain::TimeOfDayFile::Open(fileName.c_str(),
        m_spEntityManager, true);

    if (!pFile)
    {
        return false;
    }

    efd::utf8string entityID;
    efd::ParseHelper<efd::ID128>::ToString(entityFileID, entityID);

    // We should not save data if there are no time of day objects as there will be no data to be 
    // saved
    if (m_spTimeOfDay)
    {
        // Add the entity to the file
        pFile->AddEntity(entityFileID, entityName);

        efd::map<efd::utf8string, ToDProperty>::iterator iter = m_propertyMap.begin();
        while (iter != m_propertyMap.end())
        {
            // We can now parse through the properties and save them
            NiFixedString currentProperty = iter->second.m_propertyName.c_str();

            efd::utf8string propNameUTF8 = currentProperty;

            if (currentProperty.Contains(entityID.c_str()))
            {
                pFile->AddProperty(entityFileID, iter->second);

                efd::UInt32 numKeyframes = 0;
                efd::vector<efd::utf8string> times;
                efd::vector<efd::utf8string> values;
                m_spTimeOfDay->GetPropertySequence(currentProperty, numKeyframes, times, values);

                for (efd::UInt32 key = 0; key < numKeyframes; ++key)
                {
                    pFile->AddKeyframe(propNameUTF8, times[key], values[key]);
                }
            }

            ++iter;
        }
    }

    pFile->WriteFileContent();
    EE_DELETE pFile;

    return true;
}

//------------------------------------------------------------------------------------------------
void ToolEnvironmentService::HandleAnimationChangedMessage(const ToDAnimationChangedMessage* pMessage,
    efd::Category targetChannel)
{
    EE_ASSERT(pMessage);
    if (!m_spTimeOfDay)
    {
        EE_LOG(efd::kGamebryoGeneral0, efd::ILogger::kERR1,
            ("EnvironmentService: Time of day entity does not yet exist"));
        return;
    }

    m_runAnimation = pMessage->GetAnimationStatus();
    m_animationSpeed = pMessage->GetAnimationSpeedMultiplier();

    if (m_spTimeOfDay)
    {
        // We can only animate if the entity is set to be animated!
        m_spTimeOfDay->SetActive(m_animatedToD && m_runAnimation && m_toDInWorld);
        if (m_runAnimation)
        {
            m_spTimeOfDay->SetTimeMultiplier(m_timeScaleMultiplier * m_animationSpeed);
        }
    }
}

//------------------------------------------------------------------------------------------------
void ToolEnvironmentService::OnSurfaceRemoved(ecr::RenderService* pService,
    ecr::RenderSurface* pSurface)
{
    EE_UNUSED_ARG(pService);
    EE_ASSERT(pSurface);

    m_surfaceRenderOptions.erase(pSurface);
}

//------------------------------------------------------------------------------------------------
void ToolEnvironmentService::SetPropertyConstraint(efd::utf8string propertyName,
    efd::utf8string minValue, efd::utf8string maxValue)
{
    ToDProperty prop;
    if (m_propertyMap.find(propertyName, prop))
    {
        m_propertyMap[propertyName].m_minValue = minValue;
        m_propertyMap[propertyName].m_maxValue = maxValue;
    }
}

//------------------------------------------------------------------------------------------------
void ToolEnvironmentService::UpdateRenderMode(ecr::RenderSurface* pSurface, 
    bool renderEnvironment, bool useWireframe)
{
    if (renderEnvironment)
    {
        m_surfaceRenderOptions[pSurface] |= ENVIRONMENT_RENDER;
    }
    else
    {
        m_surfaceRenderOptions[pSurface] &= ~ENVIRONMENT_RENDER;
    }

    if (useWireframe)
    {
        m_surfaceRenderOptions[pSurface] |= ENVIRONMENT_USE_WIREFRAME;
    }
    else
    {
        m_surfaceRenderOptions[pSurface] &= ~ENVIRONMENT_USE_WIREFRAME;
    }

    UpdateRenderSurface(pSurface);
}

//------------------------------------------------------------------------------------------------
void ToolEnvironmentService::AddPropertiesFromFile(TimeOfDayFile* pFile, efd::utf8string assetLLID)
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
        /// Register the file path with the entity
        m_assetFilePath[*entIter] = pFile->GetFilePath();
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

            ToDProperty prop;
            if (m_propertyMap.find(iter->m_propertyName, prop))
            {
                if (!iter->m_minValue.empty() && !iter->m_maxValue.empty())
                {
                    m_propertyMap[iter->m_propertyName].m_minValue = iter->m_minValue;
                    m_propertyMap[iter->m_propertyName].m_maxValue = iter->m_maxValue;
                }
            }

            efd::vector<efd::utf8string> times;
            efd::vector<efd::utf8string> values;
            pFile->GetPropertyKeyframes((*iter).m_propertyName, times, values);
            m_spTimeOfDay->UpdatePropertySequence(iter->m_propertyName.c_str(),
                times.size(), times, values);

            RaiseKeyframeLoaded((*iter), times, values);

            ++iter;
        }
        ++entIter;
    }
}

//------------------------------------------------------------------------------------------------
bool ToolEnvironmentService::RegisterEntity(egf::Entity* pEntity)
{
    if (!EnvironmentService::RegisterEntity(pEntity))
        return false;

    UpdateToDProperties();

    return true;
}

//------------------------------------------------------------------------------------------------
bool ToolEnvironmentService::UnregisterEntity(egf::Entity* pEntity)
{
    efd::utf8string modelID;
    efd::ParseHelper<efd::ID128>::ToString(pEntity->GetDataFileID(), modelID);
    if (!m_spTimeOfDay || !IsEntityRegistered(pEntity->GetEntityID()))
        return false;

    if (pEntity->GetModel()->ContainsModel(egf::kFlatModelID_StandardModelLibrary_TimeOfDayEditable))
    {
        efd::utf8string nextKey;
        efd::utf8string modelName = pEntity->GetModel()->GetName();

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
            efd::utf8string propName = todProp.GetAt(size - 1);
            efd::vector<efd::utf8string> splittedName;
            propName.split(".", splittedName);

            if (splittedName[0] == modelID)
            {
                toDPropList.push_back(propName);
                m_propertyMap.erase(propName);
            }

            --size;
        }

        m_spTimeOfDay->RemoveToDProperties(toDPropList);

        efd::vector<egf::EntityID>::iterator registeredIter = 
            m_registeredEntities.find(pEntity->GetEntityID());
        m_registeredEntities.erase(registeredIter);
    }

    UpdateToDProperties();

    return true;
}

//------------------------------------------------------------------------------------------------
bool ToolEnvironmentService::CanRenderEnvironment(ecr::RenderSurface* pSurface)
{
    if (!EnvironmentService::CanRenderEnvironment(pSurface))
        return false;

    efd::UInt8 flag;
    if (m_surfaceRenderOptions.find(pSurface, flag))
    {
        if ((flag & ENVIRONMENT_RENDER) != 0)
            return true;
    }
    
    return false;
}

//------------------------------------------------------------------------------------------------
NiRenderClick* ToolEnvironmentService::RetrieveSkyRenderClick(ecr::RenderSurface* pSurface)
{
    NiRenderClick* pSkyRenderClick = EnvironmentService::RetrieveSkyRenderClick(pSurface);
    if (!pSkyRenderClick)
        return NULL;
    
    efd::UInt8 flag;
    if (m_surfaceRenderOptions.find(pSurface, flag))
    {
        if ((flag & ENVIRONMENT_USE_WIREFRAME) != 0)
        {
            NiViewRenderClick* pSkyView = (NiViewRenderClick*)pSkyRenderClick;

            if (pSkyView)
            {
                NiWireframeProperty* pWireframeProperty = NiNew NiWireframeProperty();
                pWireframeProperty->SetWireframe(true);

                NiPropertyList* pWirePropList = NiNew NiPropertyList();
                pWirePropList->AddTail(pWireframeProperty);

                pSkyView->SetProcessor(NiNew PropertySwapProcessor(pWirePropList));
            }
        }
    }

    return pSkyRenderClick;
}

//------------------------------------------------------------------------------------------------
void ToolEnvironmentService::UpdateToDProperties()
{
    if (!m_spTimeOfDay)
        return;

    NiTObjectSet<NiFixedString> propertyNames;
    NiTObjectSet<NiFixedString> propertyTypes;

    m_spTimeOfDay->GetPropertyNamesAndTypes(propertyNames, propertyTypes);
    efd::vector<ToolEnvironmentService::ToDProperty> propertyList;

    EE_VERIFY(propertyNames.GetSize() == propertyTypes.GetSize());

    // first add the properties to the dependant tools
    efd::map<efd::utf8string, ToDProperty>::iterator propIter;
    for (efd::UInt32 ui = 0; ui < propertyNames.GetSize(); ui++)
    {
        ToDProperty prop;
        prop.m_propertyName = propertyNames.GetAt(ui);
        prop.m_propertyType = propertyTypes.GetAt(ui);

        propIter = m_propertyMap.find(prop.m_propertyName);

        // Add the property to the local map
        if (propIter == m_propertyMap.end())
        {
            m_propertyMap[prop.m_propertyName] = prop;
            propertyList.push_back(prop);
        }
        else
        {
            propertyList.push_back(propIter->second);
        }
    }

    RaiseToDPropertiesChanged(propertyList);

    efd::vector<ToolEnvironmentService::ToDProperty>::iterator iter = propertyList.begin();
    while (iter != propertyList.end())
    {
        NiUInt32 uiNumProp = 0;
        efd::vector<efd::utf8string> times;
        efd::vector<efd::utf8string> values;
        efd::utf8string propName = iter->m_propertyName;
        m_spTimeOfDay->GetPropertySequence(propName.c_str(), uiNumProp, times, values);

        RaiseKeyframeLoaded(*iter, times, values);
        ++iter;
    }
}

//------------------------------------------------------------------------------------------------
