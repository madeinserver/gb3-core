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

// Precompiled Header
#include "ecrInputPCH.h"

#include "InputService.h"
#include "InputActionMessage.h"
#include "InputHandler.h"
#include <egf/StandardModelLibraryFlatModelIDs.h>

#include <efd/TinyXML.h>
#include <efd/IConfigManager.h>
#include <efd/MessageService.h>
#if defined (EE_PLATFORM_WIN32)
    #include <efd/Win32/Win32PlatformService.h>
    #include <NiDI8InputSystem.h>
#elif defined (EE_PLATFORM_PS3)
    #include <NiPS3InputSystem.h>
#endif

#include "InputServiceAction.h"
#include "InputServiceDPad.h"
#include "InputServiceStick.h"
#include "InputActionMapsLoaded.h"
#include "InputHandler.h"
#include <egf/StandardModelLibraryFlatModelIDs.h>
#include "InputServiceMouse.h"

#include "KeyboardMessages.h"
#include "MouseMessages.h"
#include "InputStickMessage.h"

#include <efd/ILogger.h>
#include <efd/AssetFactoryManager.h>
#include <efd/ecrLogIDs.h>
#include <efd/ServiceManager.h>
#include <egf/EntityManager.h>
#include <egf/FlatModelManager.h>

using namespace efd;
using namespace ecrInput;

EE_IMPLEMENT_CONCRETE_CLASS_INFO(InputService);

EE_HANDLER(InputService, HandleAssetLoadResponse, AssetLoadResponse);

EE_HANDLER_SUBCLASS(
    InputService,
    HandleAssetLoadResponse,
    AssetLoadResponse,
    GenericAssetLoadResponse);

efd::Category InputService::ms_inputServiceCategory = efd::Category(
    efd::UniversalID::ECU_Any,
    efd::kNetID_Any,
    kCLASSID_InputService);

//------------------------------------------------------------------------------------------------
InputService::InputService(const efd::Bool sendSingleMessages, const efd::Bool legacyMode)
    : m_globalActionMap("Global", NULL)
    , m_instanceRef(0)
    , m_windowRef(0)
    , m_exclusiveKeyboard(false)
    , m_exclusiveMouse(false)
    , m_pAssetFactory(0)
    , m_handlerCount(0)
    , m_bLegacyMode(legacyMode)
    , m_sendSingleMessages(sendSingleMessages)
{
    // If this default priority is changed, also update the service quick reference documentation
    m_defaultPriority = 4500;

#if defined(EE_PLATFORM_XBOX360) || defined(EE_PLATFORM_PS3)
    m_keyW = false;
    m_keyA = false;
    m_keyS = false;
    m_keyD = false;
    m_keyUp = false;
    m_keyLeft = false;
    m_keyRight = false;
#endif
}

//------------------------------------------------------------------------------------------------
InputService::~InputService()
{
    // This method intentionally left blank (all shutdown occurs in OnShutdown)
}

//------------------------------------------------------------------------------------------------
const char* InputService::GetDisplayName() const
{
    return "InputService";
}

//------------------------------------------------------------------------------------------------
#if defined (EE_PLATFORM_XBOX360)
NiInputSystem::CreateParams* InputService::GetInputSystemCreateParams()
{
    NiXenonInputSystem::CreateParams* pParams =
        EE_NEW NiXenonInputSystem::CreateParams();
    EE_ASSERT(pParams);

    pParams->SetGamePadCount(2);
    pParams->SetAxisRange(-EE_NIS_AXIS_RANGE, EE_NIS_AXIS_RANGE);

    return pParams;
}

#elif defined (EE_PLATFORM_WIN32)
NiInputSystem::CreateParams* InputService::GetInputSystemCreateParams()
{
    NiDI8InputSystem::DI8CreateParams* pParams =
        EE_NEW NiDI8InputSystem::DI8CreateParams();
    EE_ASSERT(pParams);

    efd::UInt32 keyboardFlags = NiInputSystem::FOREGROUND;
    if (m_exclusiveKeyboard)
        keyboardFlags |= NiInputSystem::EXCLUSIVE;
    else
        keyboardFlags |= NiInputSystem::NONEXCLUSIVE;

    efd::UInt32 mouseFlags = NiInputSystem::FOREGROUND;
    if (m_exclusiveMouse)
        mouseFlags |= NiInputSystem::EXCLUSIVE;
    else
        mouseFlags |= NiInputSystem::NONEXCLUSIVE;

    pParams->SetKeyboardUsage(keyboardFlags);
    pParams->SetMouseUsage(mouseFlags);
    pParams->SetGamePadCount(2);
    pParams->SetAxisRange(-EE_NIS_AXIS_RANGE, EE_NIS_AXIS_RANGE);
    pParams->SetOwnerInstance(m_instanceRef);
    pParams->SetOwnerWindow(m_windowRef);

    return pParams;
}
#elif defined (EE_PLATFORM_PS3)
NiInputSystem::CreateParams* InputService::GetInputSystemCreateParams()
{
    NiPS3InputSystem::CreateParams* pParams = EE_NEW NiPS3InputSystem::CreateParams();
    EE_ASSERT(pParams);

    pParams->SetGamePadCount(2);
    pParams->SetAxisRange(-EE_NIS_AXIS_RANGE, EE_NIS_AXIS_RANGE);

    return pParams;
}
#endif

//------------------------------------------------------------------------------------------------
SyncResult InputService::OnPreInit(efd::IDependencyRegistrar* pDependencyRegistrar)
{
    // Because we register an IAssetFactory whenever there is an AssetFactoryManager we must
    // depend on the AssetFactoryManager class if it exists.
    pDependencyRegistrar->AddDependency<AssetFactoryManager>(sdf_Optional);

#if defined(EE_PLATFORM_WIN32)
    pDependencyRegistrar->AddDependency<Win32PlatformService>(sdf_Optional);
#endif

    m_pMessageService = m_pServiceManager->GetSystemServiceAs<efd::MessageService>();
    EE_ASSERT(m_pMessageService);
    m_globalActionMap.SetMessageService(m_pMessageService);
    m_pMessageService->Subscribe(this, MessageCategory());

    // Register factory functions for builtin models
    egf::FlatModelManager* pFlatModelManager =
        m_pServiceManager->GetSystemServiceAs<egf::FlatModelManager>();
    EE_ASSERT(pFlatModelManager);

    pFlatModelManager->RegisterBuiltinModelFactory(
        "InputHandler",
        egf::kFlatModelID_StandardModelLibrary_InputHandler,
        ecrInput::InputHandler::Factory);

    return SyncResult_Success;
}

//------------------------------------------------------------------------------------------------
// Service initialization and shutdown
//------------------------------------------------------------------------------------------------
AsyncResult InputService::OnInit()
{
    m_pMessageService = m_pServiceManager->GetSystemServiceAs<efd::MessageService>();
    if (!m_pMessageService)
        return AsyncResult_Failure;

#if defined(EE_PLATFORM_WIN32)
    // If you are using the Win32PlatformService then we can automatically detect your instance
    // and window references, otherwise you must call SetInstanceRef and SetWindowRef before
    // we reach OnInit.
    Win32PlatformService* pWin32 = m_pServiceManager->GetSystemServiceAs<Win32PlatformService>();
    if (pWin32)
    {
        m_instanceRef = pWin32->GetInstanceRef();
        m_windowRef = pWin32->GetWindowRef();
    }

    if (m_instanceRef == 0 || m_windowRef == 0)
    {
        EE_LOG(efd::kGamebryoGeneral0, efd::ILogger::kERR1,
            ("Instance and window refs not set in InputService!"));
        return AsyncResult_Failure;
    }
#endif

    AssetFactoryManager* pAssetFactoryManager =
        m_pServiceManager->GetSystemServiceAs<efd::AssetFactoryManager>();
    if (pAssetFactoryManager)
    {
        m_pAssetFactory = EE_NEW GenericAssetFactory(true);
        pAssetFactoryManager->RegisterAssetFactory(MessageCategory(), m_pAssetFactory);
    }
    else
    {
        EE_LOG(efd::kGamebryoGeneral0, efd::ILogger::kERR1,
            ("InputService cannot find an AssetFactoryManager. "
            "Loading of action maps from file will be unavailable"));
    }

    // Create and initialize parameters for the input system
    NiInputSystem::CreateParams* pParams = GetInputSystemCreateParams();

    // Create the input system
    m_spInput = NiInputSystem::Create(pParams);

    // Delete params.
    EE_DELETE pParams;

    if (!m_spInput)
    {
        EE_LOG(efd::kGamebryoGeneral0, efd::ILogger::kERR1,
            ("NiInputSystem::Create failed"));
        return AsyncResult_Failure;
    }

    // The creation of the input system automatically starts an enumeration of the devices
    NiInputErr err = m_spInput->CheckEnumerationStatus();
    switch (err)
    {
    case NIIERR_ENUM_NOTRUNNING:
        EE_FAIL("NiInputSystem::EnumerateDevices failed.");
        EE_LOG(efd::kGamebryoGeneral0, efd::ILogger::kERR1,
            ("NiInputSystem::EnumerateDevices failed."));
        return AsyncResult_Failure;

    case NIIERR_ENUM_FAILED:
        EE_FAIL("NiInputSystem::CheckEnumerationStatus failed.");
        EE_LOG(efd::kGamebryoGeneral0, efd::ILogger::kERR1,
            ("NiInputSystem::CheckEnumerationStatus failed."));
        return AsyncResult_Failure;

    case NIIERR_ENUM_COMPLETE:
    case NIIERR_ENUM_NOTCOMPLETE:
    default:
        break;
    }

#if defined (EE_PLATFORM_WIN32)
    // On Win32, assume there is a mouse and keyboard
    if (!m_spInput->OpenMouse() || !m_spInput->OpenKeyboard())
    {
        EE_LOG(efd::kGamebryoGeneral0, efd::ILogger::kERR1,
            ("Mouse or keyboard failed to open."));
        return AsyncResult_Failure;
    }
#endif

    // Gamepad may not exist, but attempt to open any
    for (efd::UInt32 padIdx = 0; padIdx < NiInputSystem::MAX_GAMEPADS; padIdx++)
    {
        m_spInput->OpenGamePad(padIdx, 0);
    }

    IConfigManager* pConfigManager = m_pServiceManager->GetSystemServiceAs<IConfigManager>();
    utf8string defaultActionMap = pConfigManager->FindValue("InputService.ActionMapURN");
    if (!defaultActionMap.empty())
    {
        LoadAllActionMaps(defaultActionMap);
    }

    return AsyncResult_Complete;
}

//------------------------------------------------------------------------------------------------
AsyncResult InputService::OnShutdown()
{
    if (m_pAssetFactory)
    {
        AssetFactoryManager* pAssetFactoryManager =
            m_pServiceManager->GetSystemServiceAs<efd::AssetFactoryManager>();
        EE_ASSERT(pAssetFactoryManager);
        pAssetFactoryManager->UnregisterAssetFactory(MessageCategory());

        // The AssetFactoryManager owns the factory, do not delete it.
        m_pAssetFactory = 0;
    }

    m_spInput = NULL;

    DeleteActionMaps();
    m_pMessageService->Unsubscribe(this, MessageCategory());

    if (m_actionMaps.size())
        DeleteActionMaps();

    return AsyncResult_Complete;
}

//------------------------------------------------------------------------------------------------
void InputService::DeleteActionMaps()
{
    for (efd::UInt32 i = 0; i < m_actionMaps.size(); i++)
        EE_DELETE m_actionMaps[i];

    m_globalActionMap.DeleteAllEvents();
    m_actionMaps.clear();

    while (!m_actionMapStack.empty())
    {
        // Don't delete elements from the actionMapStack. These are the same elements in
        // the m_actionMaps above. Deleting them here results in a double delete.
        m_actionMapStack.pop();
    }
}

//------------------------------------------------------------------------------------------------
// Input processing
//------------------------------------------------------------------------------------------------
AsyncResult InputService::OnTick()
{
    if (!m_spInput)
        return AsyncResult_Failure;

    // Update input
    if (m_spInput->UpdateAllDevices() == NIIERR_DEVICELOST)
        m_spInput->HandleDeviceChanges();

    ProcessInput();
    if (m_sendSingleMessages)
        ProcessSingleMessageInput();

    // Continue ticking the service
    return AsyncResult_Pending;
}

//------------------------------------------------------------------------------------------------
// Single message mode, for use with Tool Services.
//------------------------------------------------------------------------------------------------
void InputService::ProcessSingleMessageInput()
{
    // Process input
    // Translate input events to framework messages
    MessageService* pMessageService = m_pServiceManager->GetSystemServiceAs<MessageService>();

    NiInputKeyboard* pKeyboard = m_spInput->GetKeyboard();
    if (pKeyboard)
    {
        // Check for key events
        for (int key = NiInputKeyboard::KEY_NOKEY;
            key < NiInputKeyboard::KEY_TOTAL_COUNT; key++)
        {
            if (pKeyboard->KeyWasPressed((NiInputKeyboard::KeyCode)key))
            {
                KeyDownMessagePtr spDownMessage =
                    EE_NEW KeyDownMessage((NiInputKeyboard::KeyCode)key);
                if (spDownMessage)
                    pMessageService->SendLocal(spDownMessage);
            }
            if (pKeyboard->KeyWasReleased((NiInputKeyboard::KeyCode)key))
            {
                KeyUpMessagePtr spUpMessage =
                    EE_NEW KeyUpMessage((NiInputKeyboard::KeyCode)key);
                if (spUpMessage)
                    pMessageService->SendLocal(spUpMessage);
            }
        }
    }

    NiInputMouse* pMouse = m_spInput->GetMouse();
    if (pMouse)
    {
        // Check for mouse move events
        int xDelta, yDelta, zDelta;
        if (pMouse->GetPositionDelta(xDelta, yDelta, zDelta))
        {
            if (xDelta != 0 || yDelta != 0 || zDelta != 0)
            {
                MouseMoveMessagePtr spMoveMessage =
                    EE_NEW MouseMoveMessage(xDelta, yDelta, zDelta);
                if (spMoveMessage)
                    pMessageService->SendLocal(spMoveMessage);
            }
        }

        // Check for mouse button down events
        if (pMouse->ButtonWasPressed(NiInputMouse::NIM_LEFT))
        {
            MouseDownMessagePtr spDownMessage =
                EE_NEW MouseDownMessage(MouseDownMessage::MBUTTON_LEFT);
            if (spDownMessage)
                pMessageService->SendLocal(spDownMessage);
        }
        if (pMouse->ButtonWasPressed(NiInputMouse::NIM_RIGHT))
        {
            MouseDownMessagePtr spDownMessage =
                EE_NEW MouseDownMessage(MouseDownMessage::MBUTTON_RIGHT);
            if (spDownMessage)
                pMessageService->SendLocal(spDownMessage);
        }
        if (pMouse->ButtonWasPressed(NiInputMouse::NIM_MIDDLE))
        {
            MouseDownMessagePtr spDownMessage =
                EE_NEW MouseDownMessage(MouseDownMessage::MBUTTON_MIDDLE);
            if (spDownMessage)
                pMessageService->SendLocal(spDownMessage);
        }

        // Check for mouse button up events
        if (pMouse->ButtonWasReleased(NiInputMouse::NIM_LEFT))
        {
            MouseUpMessagePtr spUpMessage =
                EE_NEW MouseUpMessage(MouseUpMessage::MBUTTON_LEFT);
            if (spUpMessage)
                pMessageService->SendLocal(spUpMessage);
        }
        if (pMouse->ButtonWasReleased(NiInputMouse::NIM_RIGHT))
        {
            MouseUpMessagePtr spUpMessage =
                EE_NEW MouseUpMessage(MouseUpMessage::MBUTTON_RIGHT);
            if (spUpMessage)
                pMessageService->SendLocal(spUpMessage);
        }
        if (pMouse->ButtonWasReleased(NiInputMouse::NIM_MIDDLE))
        {
            MouseUpMessagePtr spUpMessage =
                EE_NEW MouseUpMessage(MouseUpMessage::MBUTTON_MIDDLE);
            if (spUpMessage)
                pMessageService->SendLocal(spUpMessage);
        }
    }

#if defined(EE_PLATFORM_XBOX360) || defined(EE_PLATFORM_PS3)
    NiInputGamePad* pGamePad = m_spInput->GetGamePad(0);
    if (pGamePad)
    {
        int horz = 0;
        int vert = 0;
        pGamePad->GetStickValue(NiInputGamePad::NIGP_STICK_RIGHT, horz, vert);

        if (horz != 0 || vert != 0)
        {
            horz /= 5;
            vert /= 5;

            MouseMoveMessagePtr spMoveMessage = EE_NEW MouseMoveMessage(horz, vert, 0);
            if (spMoveMessage)
                pMessageService->SendLocal(spMoveMessage);
        }

        pGamePad->GetStickValue(NiInputGamePad::NIGP_STICK_LEFT, horz, vert);

        if (horz < -15)
        {
            m_keyA = true;
            KeyDownMessagePtr spDownMessage = EE_NEW KeyDownMessage(NiInputKeyboard::KEY_A);
            if (spDownMessage)
                pMessageService->SendLocal(spDownMessage);
        }
        else if (m_keyA)
        {
            m_keyA = false;
            KeyUpMessagePtr spUpMessage = EE_NEW KeyUpMessage(NiInputKeyboard::KEY_A);
            if (spUpMessage)
                pMessageService->SendLocal(spUpMessage);
        }

        if (horz > 15)
        {
            m_keyD = true;
            KeyDownMessagePtr spDownMessage = EE_NEW KeyDownMessage(NiInputKeyboard::KEY_D);
            if (spDownMessage)
                pMessageService->SendLocal(spDownMessage);
        }
        else if (m_keyD)
        {
            m_keyD = false;
            KeyUpMessagePtr spUpMessage = EE_NEW KeyUpMessage(NiInputKeyboard::KEY_D);
            if (spUpMessage)
                pMessageService->SendLocal(spUpMessage);
        }

        if (vert > 15)
        {
            m_keyS = true;
            KeyDownMessagePtr spDownMessage = EE_NEW KeyDownMessage(NiInputKeyboard::KEY_S);
            if (spDownMessage)
                pMessageService->SendLocal(spDownMessage);
        }
        else if (m_keyS)
        {
            m_keyS = false;
            KeyUpMessagePtr spUpMessage = EE_NEW KeyUpMessage(NiInputKeyboard::KEY_S);
            if (spUpMessage)
                pMessageService->SendLocal(spUpMessage);
        }

        if (vert < -15)
        {
            m_keyW = true;
            KeyDownMessagePtr spDownMessage = EE_NEW KeyDownMessage(NiInputKeyboard::KEY_W);
            if (spDownMessage)
                pMessageService->SendLocal(spDownMessage);
        }
        else if (m_keyW)
        {
            m_keyW = false;
            KeyUpMessagePtr spUpMessage = EE_NEW KeyUpMessage(NiInputKeyboard::KEY_W);
            if (spUpMessage)
                pMessageService->SendLocal(spUpMessage);
        }

        if (pGamePad->ButtonIsDown(NiInputGamePad::NIGP_LUP) && !m_keyUp)
        {
            m_keyUp = true;
            KeyDownMessagePtr spDownMessage = EE_NEW KeyDownMessage(NiInputKeyboard::KEY_UP);
            if (spDownMessage)
                pMessageService->SendLocal(spDownMessage);
        }
        else if (!pGamePad->ButtonIsDown(NiInputGamePad::NIGP_LUP) && m_keyUp)
        {
            m_keyUp = false;
            KeyUpMessagePtr spUpMessage = EE_NEW KeyUpMessage(NiInputKeyboard::KEY_UP);
            if (spUpMessage)
                pMessageService->SendLocal(spUpMessage);
        }

        if (pGamePad->ButtonIsDown(NiInputGamePad::NIGP_LLEFT) && !m_keyLeft)
        {
            m_keyLeft = true;
            KeyDownMessagePtr spDownMessage = EE_NEW KeyDownMessage(NiInputKeyboard::KEY_LEFT);
            if (spDownMessage)
                pMessageService->SendLocal(spDownMessage);
        }
        else if (!pGamePad->ButtonIsDown(NiInputGamePad::NIGP_LLEFT) && m_keyLeft)
        {
            m_keyLeft = false;
            KeyUpMessagePtr spUpMessage = EE_NEW KeyUpMessage(NiInputKeyboard::KEY_LEFT);
            if (spUpMessage)
                pMessageService->SendLocal(spUpMessage);
        }

        if (pGamePad->ButtonIsDown(NiInputGamePad::NIGP_LRIGHT) && !m_keyRight)
        {
            m_keyRight = true;
            KeyDownMessagePtr spDownMessage = EE_NEW KeyDownMessage(NiInputKeyboard::KEY_RIGHT);
            if (spDownMessage)
                pMessageService->SendLocal(spDownMessage);
        }
        else if (!pGamePad->ButtonIsDown(NiInputGamePad::NIGP_LRIGHT) && m_keyRight)
        {
            m_keyRight = false;
            KeyUpMessagePtr spUpMessage = EE_NEW KeyUpMessage(NiInputKeyboard::KEY_RIGHT);
            if (spUpMessage)
                pMessageService->SendLocal(spUpMessage);
        }

        struct SimpleBindingEntry
        {
            NiInputGamePad::Button btn;
            NiInputKeyboard::KeyCode key;
        } SimpleBindings[] =
        {
            { NiInputGamePad::NIGP_RDOWN,  NiInputKeyboard::KEY_SPACE },
            { NiInputGamePad::NIGP_RRIGHT,  NiInputKeyboard::KEY_Y },

            { NiInputGamePad::NIGP_L1,  NiInputKeyboard::KEY_U },
            { NiInputGamePad::NIGP_L2,  NiInputKeyboard::KEY_7 },
            { NiInputGamePad::NIGP_RLEFT,  NiInputKeyboard::KEY_I },
            { NiInputGamePad::NIGP_RUP,  NiInputKeyboard::KEY_O },
            { NiInputGamePad::NIGP_R1,  NiInputKeyboard::KEY_P },
            { NiInputGamePad::NIGP_R2,  NiInputKeyboard::KEY_0 },
        };

        for (UInt32 i = 0; i < EE_ARRAYSIZEOF(SimpleBindings); ++i)
        {
            if (pGamePad->ButtonWasPressed(SimpleBindings[i].btn))
            {
                KeyDownMessagePtr spDownMessage = EE_NEW KeyDownMessage(SimpleBindings[i].key);
                pMessageService->SendLocal(spDownMessage);
            }
            else if (pGamePad->ButtonWasReleased(SimpleBindings[i].btn))
            {
                KeyUpMessagePtr spUpMessage = EE_NEW KeyUpMessage(SimpleBindings[i].key);
                pMessageService->SendLocal(spUpMessage);
            }
        }
    }
#endif // (EE_PLATFORM_XBOX360) || defined(EE_PLATFORM_PS3)
}

//------------------------------------------------------------------------------------------------
// Process global action map and action map on the top of a stack
//------------------------------------------------------------------------------------------------
void InputService::ProcessInput()
{
    InputServiceActionMap* pActionMap;
    float currTime = (float)m_pServiceManager->GetServiceManagerTime();

    // If defined, process global action map
    pActionMap = GetGlobalActionMap();
    if (pActionMap)
        pActionMap->ProcessInput(this, currTime);

    // Get current action map (top of stack) and process it
    pActionMap = GetActiveActionMap();
    if (pActionMap)
        pActionMap->ProcessInput(this, currTime);
}

//------------------------------------------------------------------------------------------------
// Events management
//------------------------------------------------------------------------------------------------
InputServiceEvent* InputService::FindEvent(
    const efd::utf8string& eventName) const
{
    InputServiceEvent* pEvent = NULL;

    // Search all action maps for requested event
    for (efd::UInt32 i = 0; i < m_actionMaps.size(); i++)
    {
        pEvent = m_actionMaps[i]->FindEvent(eventName);

        if (pEvent)
            break;
    }

    // If not found, search in global action map
    if (!pEvent)
        pEvent = m_globalActionMap.FindEvent(eventName);

    return pEvent;
}

//------------------------------------------------------------------------------------------------
InputServiceEvent* InputService::FindCreateEvent(
    const efd::utf8string& eventName,
    efd::Category actionCat,
    efd::UInt32 eventFlags,
    efd::Float32 timeout)
{
    InputServiceEvent* pEvent = FindEvent(eventName);

    if (pEvent)
        return pEvent;

    InputServiceActionMap* pActionMap = GetActiveActionMap();

    // If no action map active, add event to global action map
    if (!pActionMap)
        pActionMap = GetGlobalActionMap();

    pEvent = pActionMap->AddEvent(
        eventName,
        actionCat,
        eventFlags,
        timeout);

    return pEvent;
}

//------------------------------------------------------------------------------------------------
// Actions adding methods
//------------------------------------------------------------------------------------------------
efd::Bool InputService::AddAction(
    const efd::utf8string& eventName,
    efd::Category msgCat,
    efd::UInt32 semantic,
    efd::UInt32 actionFlags,
    efd::UInt32 appData,
    efd::UInt32 deviceID,
    efd::UInt32 modifiers,
    efd::Float32 minRange,
    efd::Float32 maxRange)
{
    InputServiceEvent* pEvent = FindCreateEvent(eventName, msgCat);

    if (!pEvent)
        return false;

    InputServiceAction* pAction = EE_NEW InputServiceAction(
        semantic,
        actionFlags,
        appData,
        deviceID,
        modifiers,
        minRange,
        maxRange);

    pEvent->AddAction(pAction);

    return true;
}

//------------------------------------------------------------------------------------------------
efd::Bool InputService::AddDPad(
    const efd::utf8string& eventName,
    efd::Category msgCat,
    InputService::DPadType dpadType,
    efd::UInt32 actionFlags,
    efd::UInt32 appData,
    efd::UInt32 deviceID,
    efd::UInt32 modifiers,
    efd::UInt32 upSemantic,
    efd::UInt32 downSemantic,
    efd::UInt32 leftSemantic,
    efd::UInt32 rightSemantic)
{
    InputServiceEvent* pEvent = FindCreateEvent(eventName, msgCat, RETURN_COORD);

    if (!pEvent)
        return false;

    InputServiceDPad* pAction = EE_NEW InputServiceDPad(
        dpadType,
        actionFlags,
        appData,
        deviceID,
        modifiers,
        upSemantic,
        downSemantic,
        leftSemantic,
        rightSemantic);

    pEvent->AddAction(pAction);

    return true;
}

//------------------------------------------------------------------------------------------------
efd::Bool InputService::AddStick(
    const efd::utf8string& eventName,
    efd::Category msgCat,
    InputService::StickType stickType,
    efd::UInt32 actionFlags,
    efd::UInt32 appData,
    efd::UInt32 deviceID,
    efd::UInt32 modifiers,
    efd::Float32 minRangeX,
    efd::Float32 maxRangeX,
    efd::Float32 minRangeY,
    efd::Float32 maxRangeY,
    efd::Float32 minRangeZ,
    efd::Float32 maxRangeZ,
    InputFilteringCallback pFilter,
    efd::UInt32 axisSemanticX,
    efd::UInt32 axisSemanticY,
    efd::UInt32 axisSemanticZ)
{
    InputServiceEvent* pEvent = FindCreateEvent(eventName, msgCat, RETURN_COORD);

    if (!pEvent)
        return false;

    InputServiceStick* pAction = EE_NEW InputServiceStick(
        stickType,
        actionFlags,
        appData,
        deviceID,
        modifiers,
        minRangeX,
        maxRangeX,
        minRangeY,
        maxRangeY,
        minRangeZ,
        maxRangeZ,
        pFilter,
        axisSemanticX,
        axisSemanticY,
        axisSemanticZ);

    pEvent->AddAction(pAction);

    return true;
}

//------------------------------------------------------------------------------------------------
// Saving / loading in XML
//------------------------------------------------------------------------------------------------
void InputService::LoadTranslationMap()
{
    InputServiceEvent::LoadTranslationMap();
    InputServiceActionBase::LoadTranslationMap();
    InputServiceAction::LoadTranslationMap();
    InputServiceDPad::LoadTranslationMap();
    InputServiceStick::LoadTranslationMap();
}

//------------------------------------------------------------------------------------------------
void InputService::UnloadTranslationMap()
{
    InputServiceEvent::UnloadTranslationMap();
    InputServiceActionBase::UnloadTranslationMap();
    InputServiceAction::UnloadTranslationMap();
    InputServiceDPad::UnloadTranslationMap();
    InputServiceStick::UnloadTranslationMap();
}

//------------------------------------------------------------------------------------------------
efd::Bool InputService::SaveAllActionMaps(const efd::utf8string& fileName)
{
    TiXmlDocument* pDocument = EE_EXTERNAL_NEW TiXmlDocument("Input mapping");

    TiXmlDeclaration* pDecl = EE_EXTERNAL_NEW TiXmlDeclaration(EE_NIS_XML_VERSION, "", "");
    pDocument->LinkEndChild(pDecl);

    TiXmlElement* pRootElement = EE_EXTERNAL_NEW TiXmlElement("InputServiceBindings");
    pDocument->LinkEndChild(pRootElement);

    TiXmlComment* pComment = EE_EXTERNAL_NEW TiXmlComment(
        "Input service bindings: action maps, events and actions");
    pRootElement->LinkEndChild(pComment);

    TiXmlElement* pActionMapsList = EE_EXTERNAL_NEW TiXmlElement("ActionMapsList");
    pRootElement->LinkEndChild(pActionMapsList);

    TiXmlElement* pActionMapElement = EE_EXTERNAL_NEW TiXmlElement("GlobalActionMap");
    pRootElement->LinkEndChild(pActionMapElement);

    LoadTranslationMap();

    m_globalActionMap.SaveXml(pActionMapElement);

    for (efd::UInt32 i = 0; i < m_actionMaps.size(); i++)
    {
        InputServiceActionMap* pActionMap = m_actionMaps[i];

        pActionMapElement = EE_EXTERNAL_NEW TiXmlElement("ActionMap");
        pActionMapsList->LinkEndChild(pActionMapElement);

        pActionMap->SaveXml(pActionMapElement);
    }

    pDocument->SaveFile(fileName.c_str());

    EE_EXTERNAL_DELETE pDocument;

    UnloadTranslationMap();

    return true;
}

//------------------------------------------------------------------------------------------------
bool InputService::LoadAllActionMaps(const efd::utf8string& urn)
{
    if (urn.find("urn:") == utf8string::npos)
    {
        EE_LOG(efd::kGamebryoGeneral0, efd::ILogger::kERR2,
            ("Invalid asset ID: '%s'", urn.c_str()));
        return false;
    }

    if (!m_pAssetFactory)
        return false;

    AssetLoadRequest* pLoadRequest = EE_NEW AssetLoadRequest(urn, MessageCategory());
    m_pMessageService->SendLocal(pLoadRequest, AssetFactoryManager::MessageCategory());

    return true;
}

//------------------------------------------------------------------------------------------------
efd::Bool InputService::ParseAllActionMaps(const char* urn, const char* pXMLString)
{
    TiXmlDocument* pDocument = EE_EXTERNAL_NEW TiXmlDocument(urn);

    // Try to load file
    bool loadResult = pDocument->LoadBuffer(pXMLString);
    if (!loadResult)
    {
        EE_LOG(efd::kGamebryoGeneral0, efd::ILogger::kERR2,
            ("InputService::LoadAllActionMaps(\"%s\"): "
            "Unable to parse xml file",
            urn));
        EE_EXTERNAL_DELETE pDocument;
        return false;
    }

    TiXmlDeclaration* pDeclaration = pDocument->FirstChild()->ToDeclaration();
    if (!pDeclaration)
    {
        EE_LOG(efd::kGamebryoGeneral0, efd::ILogger::kERR2,
            ("InputService::LoadAllActionMaps(\"%s\"): "
            "Unable to find xml declaration",
            urn));
        EE_EXTERNAL_DELETE pDocument;
        return false;
    }

    // Check version. Additional version handling may be added here in future
    if (strcmp(pDeclaration->Version(), EE_NIS_XML_VERSION))
    {
        EE_LOG(efd::kGamebryoGeneral0, efd::ILogger::kERR2,
            ("InputService::LoadAllActionMaps(\"%s\"): "
            "File version mismatch: expected \"%s\", found \"%s\"",
            urn,
            EE_NIS_XML_VERSION,
            pDeclaration->Version()));
        EE_EXTERNAL_DELETE pDocument;
        return false;
    }

    // Check for presence of global action map and list of action maps
    TiXmlElement* pActionMapsList = pDocument->
        FirstChildElement("InputServiceBindings")->
        FirstChildElement("ActionMapsList");
    TiXmlElement* pActionMapElement = pDocument->
        FirstChildElement("InputServiceBindings")->
        FirstChildElement("GlobalActionMap");
    if (!pActionMapElement || !pActionMapsList)
    {
        EE_LOG(efd::kGamebryoGeneral0, efd::ILogger::kERR2,
            ("InputService::LoadAllActionMaps(\"%s\"): "
            "Unable to find global action map or action maps list",
            urn));
        EE_EXTERNAL_DELETE pDocument;
        return false;
    }

    LoadTranslationMap();

    // Clear all action map data and load global action map first
    DeleteActionMaps();
    if (!m_globalActionMap.LoadXml(pActionMapElement))
    {
        UnloadTranslationMap();
        return false;
    }

    // Create all action maps, found in xml and load them
    for (pActionMapElement = pActionMapsList->FirstChildElement();
        pActionMapElement;
        pActionMapElement = pActionMapElement->NextSiblingElement())
    {
        efd::utf8string actionMapName = pActionMapElement->Attribute("ActionMapName");

        InputServiceActionMap* pActionMap = EE_NEW InputServiceActionMap(
            actionMapName,
            m_pMessageService);

        // Delete action map if it has failed to load
        if (!pActionMap->LoadXml(pActionMapElement))
        {
            EE_LOG(efd::kGamebryoGeneral0, efd::ILogger::kERR2,
                ("InputService::LoadAllActionMaps(\"%s\"): "
                "Action map \"%s\" failed to load",
                urn,
                actionMapName.c_str()));
            EE_DELETE pActionMap;
            continue;
        }
        // Since you are not using CreateActionMap,
        // you must push this on to m_actionMaps or it will
        // never be deleted.
        m_actionMaps.push_back(pActionMap);
        m_actionMapStack.push(pActionMap);
    }

    // Remove xml document class after load
    EE_EXTERNAL_DELETE pDocument;

    UnloadTranslationMap();

    return true;
}

//------------------------------------------------------------------------------------------------
InputServiceActionBase* InputService::CreateAction(efd::UInt32 actionClsID)
{
    switch (actionClsID)
    {
    case EE_NIS_ACTION_CLSID_ACTION:
        return EE_NEW InputServiceAction();

    case EE_NIS_ACTION_CLSID_DPAD:
        return EE_NEW InputServiceDPad();

    case EE_NIS_ACTION_CLSID_STICK:
        return EE_NEW InputServiceStick();

    case EE_NIS_ACTION_CLSID_MOUSE:
        return EE_NEW InputServiceMouse();

    default:
        return NULL;
    }
}

//------------------------------------------------------------------------------------------------
void InputService::HandleAssetLoadResponse(
    const efd::AssetLoadResponse* pMessage,
    efd::Category targetChannel)
{
    InputActionMapsLoaded* pResponse = EE_NEW InputActionMapsLoaded(pMessage->GetURN());

    if (pMessage->GetResult() != AssetLoadResponse::ALR_Success)
    {
        EE_LOG(efd::kGamebryoGeneral0, efd::Logger::kERR1, ("InputService failed to load action map"
            " \"%s\"", pMessage->GetURN().c_str()));
        pResponse->SetSuccess(false);
        m_pMessageService->SendLocal(pResponse, MessageCategory());
        return;
    }

    const efd::GenericAssetLoadResponse* pGenericResp =
        EE_DYNAMIC_CAST(efd::GenericAssetLoadResponse, pMessage);
    EE_ASSERT(pGenericResp);
    if (!ParseAllActionMaps(pMessage->GetURN().c_str(), pGenericResp->GetAssetData()))
    {
        pResponse->SetSuccess(false);
    }

    m_pMessageService->SendLocal(pResponse, MessageCategory());
}

//------------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------------
// Message sending methods
//------------------------------------------------------------------------------------------------
void InputService::FireButtonAction(
    const efd::utf8string& eventName,
    efd::UInt32 appData,
    efd::Float32 magnitude,
    efd::Category legacyCategory) const
{
    EE_ASSERT(m_pMessageService);

    if (m_bLegacyMode && legacyCategory.IsValid())
    {
        InputActionMessagePtr spMsg = EE_NEW InputActionMessage(eventName, appData, magnitude);
        m_pMessageService->SendLocal(spMsg, legacyCategory);
    }
    CheckRegisteredHandlers(eventName, appData, magnitude);
}

//------------------------------------------------------------------------------------------------
void InputService::FireStickAction(
    const efd::utf8string& eventName,
    efd::UInt32 appData,
    efd::Float32 magnitude,
    efd::Category legacyCategory,
    efd::Float32 x,
    efd::Float32 y,
    efd::Float32 z) const
{
    EE_ASSERT(m_pMessageService);

    if (m_bLegacyMode && legacyCategory.IsValid())
    {
        InputStickMessagePtr spMsg = EE_NEW InputStickMessage(
            eventName,
            appData,
            magnitude,
            x,
            y,
            z);
        m_pMessageService->SendLocal(spMsg, legacyCategory);
    }
    CheckRegisteredHandlers(eventName, appData, magnitude, x, y, z);
}


//------------------------------------------------------------------------------------------------
efd::UInt32 InputService::ListenForInputEvent(
    efd::utf8string inputEvent,
    egf::EntityID entityID,
    efd::utf8string behavior,
    CallbackType type)
{
    if (entityID == egf::kENTITY_INVALID || inputEvent.empty() || behavior.empty())
    {
        return ecrInput::kINVALID_CALLBACK;
    }
    ++m_handlerCount;
    InputType realType = (type == CALLBACK_IMMEDIATE) ? INPUTTYPE_IMMEDIATE_BEHAVIOR :
        INPUTTYPE_NORMAL_BEHAVIOR;
    m_inputEventList.insert(InputEventMapEntry(inputEvent,
        InputEventCallback(realType, m_handlerCount, entityID, behavior)));
    return m_handlerCount;
}

//------------------------------------------------------------------------------------------------
efd::UInt32 InputService::ListenForInputActionEvent(
    efd::utf8string inputEvent,
    efd::Category callback,
    void *userdata)
{
    if (inputEvent.empty() || !callback.IsValid())
    {
        return ecrInput::kINVALID_CALLBACK;
    }
    ++m_handlerCount;
    m_inputEventList.insert(InputEventMapEntry(inputEvent,
        InputEventCallback(INPUTTYPE_ACTION, m_handlerCount, callback, userdata)));
    return m_handlerCount;
}

//------------------------------------------------------------------------------------------------
efd::UInt32 InputService::ListenForInputStickEvent(
    efd::utf8string inputEvent,
    efd::Category callback,
    void *userdata)
{
    if (inputEvent.empty() || !callback.IsValid())
    {
        return ecrInput::kINVALID_CALLBACK;
    }
    ++m_handlerCount;
    m_inputEventList.insert(InputEventMapEntry(inputEvent,
        InputEventCallback(INPUTTYPE_STICK, m_handlerCount, callback, userdata)));
    return m_handlerCount;
}

//------------------------------------------------------------------------------------------------
void InputService::ClearRegisteredInputEvents(egf::EntityID entityID)
{
    for (InputEventMap::iterator iter = m_inputEventList.begin();
        iter != m_inputEventList.end();
        /*do nothing*/)
    {
        if (iter->second.m_entityID == entityID)
        {
            //InputEventMap::iterator itemToErase = iter++;
            //m_inputEventList.erase(itemToErase);
            m_inputEventList.erase(iter++);
        }
        else
        {
            ++iter;
        }
    }
}

//------------------------------------------------------------------------------------------------
void InputService::ClearRegisteredInputEvent(efd::UInt32 callbackID)
{
    for (InputEventMap::iterator iter = m_inputEventList.begin();
        iter != m_inputEventList.end();
        /*do nothing*/)
    {
        if (iter->second.m_callbackID == callbackID)
        {
            m_inputEventList.erase(iter);
            return;
        }
        else
        {
            ++iter;
        }
    }
}

//------------------------------------------------------------------------------------------------
void InputService::CheckRegisteredHandlers(
    const efd::utf8string& eventName,
    efd::UInt32 appData,
    efd::Float32 magnitude,
    efd::Float32 x,
    efd::Float32 y,
    efd::Float32 z) const
{
    egf::EntityManager* pEntityManager =
        m_pServiceManager->GetSystemServiceAs<egf::EntityManager>();
    EE_ASSERT(pEntityManager);

    for (InputEventMap::const_iterator iter = m_inputEventList.find(eventName);
        iter != m_inputEventList.end() && iter->first == eventName;
        ++iter)
    {
        const InputEventCallback& iecb = iter->second;
        switch (iecb.m_type)
        {
        case INPUTTYPE_NORMAL_BEHAVIOR:
        case INPUTTYPE_IMMEDIATE_BEHAVIOR:
            {
                egf::EntityID entityID = iecb.m_entityID;
                egf::Entity* pEntity = pEntityManager->LookupEntity(entityID);
                if (pEntity)
                {
                    efd::ParameterListPtr spStream = EE_NEW efd::ParameterList();
                    spStream->AddParameter("EventName", eventName);
                    spStream->AddParameter("AppData", appData);
                    spStream->AddParameter("Magnitude", magnitude);
                    // For simplicity we always pass the x,y,z even when they are not used; this
                    // way behaviors can treat stick and normal behaviors the same if they really
                    // want to.
                    spStream->AddParameter("X", x);
                    spStream->AddParameter("Y", y);
                    spStream->AddParameter("Z", z);

                    if (iecb.m_type == INPUTTYPE_NORMAL_BEHAVIOR)
                    {
                        pEntity->SendEvent(
                            entityID,
                            pEntity->GetModelName().c_str(),
                            iecb.m_strBehavior.c_str(),
                            spStream);
                    }
                    else
                    {
                        pEntity->CallImmediateBehavior(
                            iecb.m_strBehavior,
                            pEntity->GetModelName(),
                            spStream);
                    }
                }
            }
            break;

        case INPUTTYPE_ACTION:
            {
                InputActionMessagePtr spMsg = EE_NEW InputActionMessage(
                    eventName,
                    appData,
                    magnitude,
                    iecb.m_userdata);
                m_pMessageService->SendLocal(spMsg, iecb.m_callback);
            }
            break;

        case INPUTTYPE_STICK:
            {
                InputStickMessagePtr spMsg = EE_NEW InputStickMessage(
                    eventName,
                    appData,
                    magnitude,
                    x,
                    y,
                    z,
                    iecb.m_userdata);
                m_pMessageService->SendLocal(spMsg, iecb.m_callback);
            }
            break;
        }
    }
}

//------------------------------------------------------------------------------------------------
InputService::InputEventCallback::InputEventCallback(
    InputType type,
    efd::UInt32 callbackID,
    egf::EntityID entityID,
    efd::utf8string strBehavior)
    : m_type(type)
    , m_callbackID(callbackID)
    , m_entityID(entityID)
    , m_strBehavior(strBehavior)
    , m_userdata(NULL)
{
}

//------------------------------------------------------------------------------------------------
InputService::InputEventCallback::InputEventCallback(
    InputType type,
    efd::UInt32 callbackID,
    efd::Category callback,
    void *userdata)
    : m_type(type)
    , m_callbackID(callbackID)
    , m_callback(callback)
    , m_userdata(userdata)
{
}

