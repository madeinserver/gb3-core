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
#include "egmVisualTrackerPCH.h"

#include <efd/MemTracker.h>
#include <efd/IConfigManager.h>
#include <efd/ILogger.h>
#include <efd/ServiceManager.h>

#include <egf/egfLogIDs.h>
#include <egf/egfBaseIDs.h>
#include <egf/EntityChangeMessage.h>

#include <NiCommonGraphCallbackObjects.h>
#include <NiVisualTracker.h>
#include <NiVisualTrackerRenderClick.h>

#include <ecrInput/InputActionMessage.h>

#include <ecr/RenderService.h>
#include <egf/Scheduler.h>
#include <ecr/SceneGraphService.h>

#include "VisualTrackerService.h"

using namespace efd;
using namespace egf;
using namespace ecr;
using namespace egmVisualTracker;


EE_IMPLEMENT_CONCRETE_CLASS_INFO(VisualTrackerService);

EE_HANDLER(VisualTrackerService, HandleMessage, ecrInput::InputActionMessage);
EE_HANDLER_WRAP(VisualTrackerService, HandleEntityMessage, EntityChangeMessage,
                kMSGID_OwnedEntityEnterWorld);
EE_HANDLER_WRAP(VisualTrackerService, HandleEntityMessage, EntityChangeMessage,
                kMSGID_OwnedEntityExitWorld);


static const efd::Category kCAT_VisualTrackerAction(
    efd::UniversalID::ECU_Any,
    kNetID_ISystemService,
    VisualTrackerService::CLASS_ID);

const char* ms_kRenderClickName = "Visual Tracker Click";
const char* ms_kRenderStepName = "Visual Tracker Screen Space Render Step";

//------------------------------------------------------------------------------------------------
VisualTrackerService::VisualTrackerService()
    : m_pages()
    , m_showTrackers(false)
    , m_curPageIndex(0)
    , m_nextClickToAdd(0)
    , m_performanceMax(120.0f)
    , m_memoryMax(100000.0f)
    , m_timeMax(50.0f)
    , m_frameRate(0.0f)
    , m_lastTime(0.0f)
    , m_numObjectsDrawn(0)
    , m_frameTime(0.0f)
    , m_numServices(0)
    , m_serviceIDs(NULL)
    , m_serviceTimings(NULL)
{
    // If this default priority is changed, also update the service quick reference documentation
    m_defaultPriority = 1950;
}

//------------------------------------------------------------------------------------------------
VisualTrackerService::~VisualTrackerService()
{
    // This method intentionally left blank (all shutdown occurs in OnShutdown)
}

//------------------------------------------------------------------------------------------------
const char* VisualTrackerService::GetDisplayName() const
{
    return "VisualTrackerService";
}

//------------------------------------------------------------------------------------------------
SyncResult VisualTrackerService::OnPreInit(efd::IDependencyRegistrar* pDependencyRegistrar)
{
    // If available, we add input actions for displaying the trackers.
    pDependencyRegistrar->AddDependency<ecrInput::InputService>(sdf_Optional);
    // Need the render service so we can add our render steps for displaying the trackers.
    pDependencyRegistrar->AddDependency<ecr::RenderService>();

    return SyncResult_Success;
}

//------------------------------------------------------------------------------------------------
AsyncResult VisualTrackerService::OnInit()
{
    efd::MessageService* pMessageService =
        m_pServiceManager->GetSystemServiceAs<efd::MessageService>();
    EE_ASSERT(pMessageService);
    // Entity Discovery messages are send on the local channel :(
    pMessageService->Subscribe(this, kCAT_LocalMessage);

    m_pRenderService = m_pServiceManager->GetSystemServiceAs<ecr::RenderService>();
    EE_ASSERT(m_pRenderService);
    m_pRenderService->AddDelegate(this);

    // Our dependency on the RenderService should ensure the active service is now setup:
    EE_ASSERT(NULL != m_pRenderService->GetActiveRenderSurface());
    AddSurface(m_pRenderService->GetActiveRenderSurface());

    // If we want to ensure the default page is the first page we need to call this early. In
    // fact we might be able to call this during OnPreInit.
    CreateVisualTrackers();

    ecrInput::InputService* pInputService =
        m_pServiceManager->GetSystemServiceAs<ecrInput::InputService>();
    if (pInputService)
    {
        pInputService->AddAction(
            "VISUAL_TRACKER_ACTION",
            kCAT_INVALID,
            NiAction::KEY_MASK | NiInputKeyboard::KEY_F11,
            ecrInput::InputService::ON_ACTIVATE);
        pInputService->AddAction(
            "VISUAL_TRACKER_ACTION",
            kCAT_INVALID,
            NiAction::GP_BUTTON_B,
            ecrInput::InputService::ON_ACTIVATE);
        pInputService->ListenForInputActionEvent("VISUAL_TRACKER_ACTION",
            kCAT_VisualTrackerAction);

        pMessageService->Subscribe(this, kCAT_VisualTrackerAction);
    }

    return AsyncResult_Complete;
}

//------------------------------------------------------------------------------------------------
AsyncResult VisualTrackerService::OnTick()
{
    if (m_pRenderService->GetRenderContextCount() == 0)
    {
        EE_LOG(efd::kGamebryoGeneral0,
            efd::ILogger::kLVL3,
            ("VisualTrackerService::OnTick: No available RenderContext\n"));
        return AsyncResult_Pending;
    }

    RenderSurface* pRenderSurface = m_pRenderService->GetActiveRenderSurface();

    if (pRenderSurface == NULL)
    {
        EE_LOG(efd::kGamebryoGeneral0,
            efd::ILogger::kERR2,
            ("VisualTrackerService::OnTick: No default Render Surface\n"));
        return AsyncResult_Pending;
    }

    if (m_pages.empty())
    {
        EE_LOG(efd::kGamebryoGeneral0,
            efd::ILogger::kERR2,
            ("VisualTrackerService::OnTick: The default Render Surface has not been registered "
            "with the VIsual Tracker Service. "
            "Call emgVisualTracker::VisualTrackerService::AddSurface.\n"));
        return AsyncResult_Pending;
    }

    NiRenderFrame* pRenderFrame = pRenderSurface->GetRenderFrame();
    if (m_nextClickToAdd < m_pages.size())
    {
        // new pages have been added since we last checked, we need to add their clicks
        NiRenderStep* pRenderStep = pRenderFrame->GetRenderStepByName(ms_kRenderStepName);
        NiDefaultClickRenderStep* pScreenSpaceRenderStep =
            NiDynamicCast(NiDefaultClickRenderStep, pRenderStep);

        for (UInt32 i = m_nextClickToAdd; i < m_pages.size(); ++i)
        {
            // Add all of our clicks to the step
            pScreenSpaceRenderStep->AppendRenderClick(m_pages[i]);
        }
        m_nextClickToAdd = m_pages.size();
    }

    // Measure time
    float currentTime = (float)m_pServiceManager->GetServiceManagerTime();
    float frameTime = currentTime - m_lastTime;
    m_lastTime = currentTime;

    // Compute frame rate
    if (frameTime < 1e-05f)
        m_frameRate = 100.0f;
    else
        m_frameRate = 1.0f / frameTime;

    float cullTime;
    float renderTime;
    pRenderFrame->GatherStatistics(m_numObjectsDrawn, cullTime, renderTime);

    m_frameTime = frameTime;

    for (UInt32 ui = 0; ui < m_numServices; ui++)
    {
        TimeType time = m_pServiceManager->GetServiceLastTickTime(m_serviceIDs[ui]);
        m_serviceTimings[ui] = (float)time / 1000.0f;
    }

    // Update all trackers
    for (UInt32 i = 0; i < m_pages.size(); ++i)
    {
        const NiTPointerList<NiVisualTrackerPtr>& trackers = m_pages[i]->GetVisualTrackers();

        NiTListIterator kIter = trackers.GetHeadPos();
        while (kIter)
        {
            VisualTracker* pVisualTracker = (VisualTracker*)trackers.GetNext(kIter).data();
            EE_ASSERT(pVisualTracker);
            if (!pVisualTracker->m_updateOnlyWhenVisible ||
                pVisualTracker->m_numToRemove > 0 ||
                i == m_curPageIndex)
            {
                pVisualTracker->Update();
            }
        }
    }

    // Return pending to indicate the service should continue to be ticked.
    return AsyncResult_Pending;
}

//------------------------------------------------------------------------------------------------
AsyncResult VisualTrackerService::OnShutdown()
{
    efd::MessageService* pMessageService =
        m_pServiceManager->GetSystemServiceAs<efd::MessageService>();
    if (pMessageService != NULL)
    {
        pMessageService->Unsubscribe(this, kCAT_LocalMessage);
        pMessageService->Unsubscribe(this, kCAT_VisualTrackerAction);
    }

    m_pRenderService = 0;

    m_pages.clear();
    m_curPageIndex = 0;
    m_nextClickToAdd = 0;
    EE_FREE(m_serviceIDs);
    EE_FREE(m_serviceTimings);

    return AsyncResult_Complete;
}

//------------------------------------------------------------------------------------------------
void VisualTrackerService::AddSurface(RenderSurfacePtr spSurface)
{
    // Get the render frame for the surface
    NiRenderFrame* pRenderFrame = spSurface->GetRenderFrame();
    if (!pRenderFrame)
    {
        EE_LOG(efd::kGamebryoGeneral0,
            efd::ILogger::kERR2,
            ("VisualTrackerService::AddSurface: "
            "There is no NiRenderFrame for the added surface.\n"));
        return;
    }

    // Create screen space render step.
    NiDefaultClickRenderStep* pScreenSpaceRenderStep = EE_NEW NiDefaultClickRenderStep;
    pScreenSpaceRenderStep->SetName(ms_kRenderStepName);
    for (UInt32 i = 0; i < m_pages.size(); ++i)
    {
        // Add all of our clicks to the step
        pScreenSpaceRenderStep->AppendRenderClick(m_pages[i]);
    }
    m_nextClickToAdd = m_pages.size();

    // Add the step to the frame
    pRenderFrame->AppendRenderStep(pScreenSpaceRenderStep);

    // When the new surface is first drawn, lets start on the first page.
    m_curPageIndex = 0;
}

//------------------------------------------------------------------------------------------------
void VisualTrackerService::RemoveSurface(RenderSurfacePtr spSurface)
{
    // Get the render frame for the surface
    NiRenderFrame* pRenderFrame = spSurface->GetRenderFrame();
    if (!pRenderFrame)
    {
        EE_LOG(efd::kGamebryoGeneral0,
            efd::ILogger::kERR2,
            ("VisualTrackerService::RemoveSurface: "
            "There is no NiRenderFrame for the removed surface."));
        return;
    }

    NiRenderStep* pRenderStep = pRenderFrame->GetRenderStepByName(ms_kRenderStepName);
    NiDefaultClickRenderStep* pScreenSpaceRenderStep =
        NiDynamicCast(NiDefaultClickRenderStep, pRenderStep);
    if (!pScreenSpaceRenderStep)
    {
        EE_LOG(efd::kGamebryoGeneral0,
            efd::ILogger::kERR2,
            ("VisualTrackerService::RemoveSurface: The given surface does not contain the "
            "visual tracker render step."));
        return;
    }

    // Find the visual tracker default render click
    NiRenderClick* pRenderClick = pScreenSpaceRenderStep->GetRenderClickByName(ms_kRenderClickName);
    if (NULL == pRenderClick)
    {
        EE_LOG(efd::kGamebryoGeneral0,
            efd::ILogger::kERR2,
            ("VisualTrackerService::RemoveSurface: The given surface does not contain the "
            "visual tracker render click."));
        return;
    }
    for (UInt32 i=0; i < m_pages.size(); ++i)
    {
        pScreenSpaceRenderStep->RemoveRenderClick(m_pages[i]);
    }
    m_pages.clear();
    m_curPageIndex = 0;
    m_nextClickToAdd = 0;

    // Remove the step from the frame
    pRenderFrame->RemoveRenderStep(pScreenSpaceRenderStep);
}

//------------------------------------------------------------------------------------------------
void VisualTrackerService::SetShowTrackers(bool show)
{
    if (m_pages.empty())
        return;

    if (show != m_showTrackers)
    {
        SetShowPage(m_curPageIndex, show);
        m_showTrackers = show;
    }
}

//------------------------------------------------------------------------------------------------
void VisualTrackerService::SetShowPage(UInt32 pageNum, bool show)
{
    // Update status of all trackers on the current page
    const NiTPointerList<NiVisualTrackerPtr>& kVisualTrackers =
        m_pages[pageNum]->GetVisualTrackers();

    NiTListIterator kIter = kVisualTrackers.GetHeadPos();
    while (kIter)
    {
        NiVisualTracker* pTracker = kVisualTrackers.GetNext(kIter);
        EE_ASSERT(pTracker);
        pTracker->SetShow(show);
    }
}

//------------------------------------------------------------------------------------------------
void VisualTrackerService::ToggleShowTrackers()
{
    SetShowTrackers(!m_showTrackers);
}

//------------------------------------------------------------------------------------------------
void VisualTrackerService::OnSurfaceRemoved(RenderService* pService, RenderSurface* pSurface)
{
    EE_UNUSED_ARG(pService);

    NiDefaultClickRenderStep* pDefaultRenderStep =
        NiDynamicCast(NiDefaultClickRenderStep, pSurface->GetRenderStep());
    if (!pDefaultRenderStep)
        return;

    // If this Step contains our default Click then this is the surface we are using:
    NiRenderClick* pRenderClick = pDefaultRenderStep->GetRenderClickByName(ms_kRenderClickName);
    if (pRenderClick)
        RemoveSurface(pSurface);
}

//------------------------------------------------------------------------------------------------
void VisualTrackerService::OnRenderServiceShutdown(RenderService* pService)
{
    RemoveSurface(pService->GetActiveRenderSurface());

    m_pages.clear();
    m_curPageIndex = 0;
    m_nextClickToAdd = 0;
}

//------------------------------------------------------------------------------------------------
bool VisualTrackerService::CreateVisualTrackers()
{
    IConfigManager* pConfigManager = m_pServiceManager->GetSystemServiceAs<IConfigManager>();
    NiColor* pServiceColors = NULL;
    bool addEntityPage = false;
    NiRect<float> entityRect(-4000, 4000, 4000, -4000);

    if (pConfigManager)
    {
        const ISection* pSection = pConfigManager->GetConfiguration();

        if (pSection)
        {
            pSection = pSection->FindSection("VisualTracker");
            utf8string value;

            if (pSection)
            {
                if (pSection->FindValue("PerfMaxValue", value))
                    efd::ParseHelper<Float32>::FromString(value, m_performanceMax);

                if (pSection->FindValue("TimingMaxValue", value))
                    efd::ParseHelper<Float32>::FromString(value, m_timeMax);

                if (pSection->FindValue("MemMaxValue", value))
                    efd::ParseHelper<Float32>::FromString(value, m_memoryMax);

                if (pSection->FindValue("ServiceTrackers", value))
                {
                    efd::ParseHelper<UInt32>::FromString(value, m_numServices);
                    m_serviceIDs = EE_ALLOC(ClassID, m_numServices);
                    m_serviceTimings = EE_ALLOC(Float32, m_numServices);
                    pServiceColors = EE_STACK_ALLOC(NiColor, m_numServices);
                }

                for (UInt32 ui = 0; ui < m_numServices; ui++)
                {
                    utf8string key;
                    key.sprintf("Service%d", ui);

                    m_serviceIDs[ui] = 0;
                    if (pSection->FindValue(key, value))
                    {
                        efd::ParseHelper<UInt32>::FromString(value, m_serviceIDs[ui]);
                    }

                    key.sprintf("Service%dColor", ui);
                    float fColor = (float) ui / (float) m_numServices;
                    pServiceColors[ui] = NiColor(fColor, fColor, fColor);
                    if (pSection->FindValue(key, value))
                    {
                        efd::ParseHelper<Color>::FromString(value, pServiceColors[ui]);
                    }
                }

                addEntityPage = pSection->IsTrue("ShowEntityPage", false);
                if (pSection->FindValue("EntityLeft", value))
                {
                    efd::ParseHelper<efd::Float32>::FromString(value, entityRect.m_left);
                }
                if (pSection->FindValue("EntityRight", value))
                {
                    efd::ParseHelper<efd::Float32>::FromString(value, entityRect.m_right);
                }
                if (pSection->FindValue("EntityTop", value))
                {
                    efd::ParseHelper<efd::Float32>::FromString(value, entityRect.m_top);
                }
                if (pSection->FindValue("EntityBottom", value))
                {
                    efd::ParseHelper<efd::Float32>::FromString(value, entityRect.m_bottom);
                }
            }
        }
    }

    if (m_serviceTimings == NULL || m_numServices == 0)
    {
        m_numServices = 3;
        m_serviceIDs = EE_ALLOC(ClassID, m_numServices);
        m_serviceTimings = EE_ALLOC(Float32, m_numServices);
        pServiceColors = EE_STACK_ALLOC(NiColor, m_numServices);

        // Set up the render service
        m_serviceIDs[0] = RenderService::CLASS_ID;
        pServiceColors[0] = NiColor(0.0f, 1.0f, 0.0f);

        // Set up the scene graph service
        m_serviceIDs[1] = SceneGraphService::CLASS_ID;
        pServiceColors[1] = NiColor(0.0f, 0.0f, 1.0f);

        // Set up the scheduler
        m_serviceIDs[2] = egf::Scheduler::CLASS_ID;
        pServiceColors[2] = NiColor(1.0f, 0.0f, 1.0f);
    }

    const float leftBorder = 0.05f;
    const float topBorder = 0.025f;
    const float regionHeight = 0.25f;
    const float regionWidth = 0.90f;

    NiRect<float> windowRect1;
    windowRect1.m_left = leftBorder;
    windowRect1.m_right = windowRect1.m_left + regionWidth;
    windowRect1.m_top = 0.15f;
    windowRect1.m_bottom = windowRect1.m_top + regionHeight;

    NiRect<float> windowRect2;
    windowRect2.m_left = leftBorder;
    windowRect2.m_right = windowRect2.m_left + regionWidth;
    windowRect2.m_top = windowRect1.m_bottom + topBorder;
    windowRect2.m_bottom = windowRect2.m_top + regionHeight;

    NiRect<float> windowRect3;
    windowRect3.m_left = leftBorder;
    windowRect3.m_right = windowRect3.m_left + regionWidth;
    windowRect3.m_top = windowRect2.m_bottom + topBorder;
    windowRect3.m_bottom = windowRect3.m_top + regionHeight;

    NiColor blue(0.0f, 0.0f, 1.0f);
    NiColor yellow(1.0f, 1.0f, 0.0f);
    NiColor red(1.0f, 0.0f, 0.0f);
    NiColor lightBlue(0.5f, 0.5f, 1.0f);
    NiColor lightYellow(1.0f, 1.0f, 0.5f);
    NiColor lightRed(1.0f, 0.5f, 0.5f);


    AddPage(ms_kRenderClickName);
    {
        AddChart(
            "Performance",
            // A width of 100 samples, and a configurable height:
            NiRect<float>(0.0f, 100.0f, m_performanceMax, 0.0f),
            windowRect1,
            0.1f);
        {
            AddLineGraph(
                "Frame-Rate",
                NiNew FrameRateUpdate(&m_frameRate),
                red);

            AddLineGraph(
                "Vis Objects",
                NiNew GenericUnsignedIntUpdate(1.0f, &m_numObjectsDrawn),
                yellow);
        }

        // Second Tracker (Timing)
        AddChart(
            "Time (ms)",
            // A width of 100 samples, and a configurable height:
            NiRect<float>(0.0f, 100.0f, m_timeMax, 0.0f),
            windowRect2,
            0.1f);
        {
            AddLineGraph(
                "Frame Time",
                NiNew GenericFloatUpdate(0.001f, &m_frameTime),
                red);

            for (unsigned int ui = 0; ui < m_numServices; ui++)
            {
                ISystemService* pService = m_pServiceManager->GetSystemService(m_serviceIDs[ui]);
                if (pService)
                {
                    AddLineGraph(
                        pService->GetDisplayName(),
                        NiNew GenericFloatUpdate(0.001f, &m_serviceTimings[ui]),
                        pServiceColors[ui]);
                }
            }
        }

#ifdef EE_USE_MEMORY_MANAGEMENT
        // Third Tracker (Memory)
        efd::MemTracker* pMemTracker = efd::MemTracker::Get();
        if (pMemTracker)
        {
            AddChart(
                "Memory (kb)",
                // A width of 100 samples, and a configurable height:
                NiRect<float>(0.0f, 100.0f, m_memoryMax, 0.0f),
                windowRect3,
                0.1f);
            {
                // Add Gamebryo-tracked memory stats
                AddLineGraph(
                    "High Watermark",
                    NiNew MemHighWaterMarkUpdate(1024.0f, pMemTracker),
                    red);
                AddLineGraph(
                    "Current",
                    NiNew MemCurrentUpdate(1024.0f, pMemTracker),
                    yellow);
                AddLineGraph(
                    "Num Allocs",
                    NiNew MemCurrentAllocCountUpdate(1.0f, pMemTracker),
                    blue);
            }
        }
#endif
    }

    if (addEntityPage)
    {
        AddPage("Entities");
        {
            AddChart(
                "Entities",
                // This should really be the bounding box of the current block file:
                entityRect,
                // Use the entire window:
                NiRect<float>(leftBorder, 1.0f - leftBorder, topBorder, 1.0f - topBorder),
                0.1f,
                0,
                // Only update when visible:
                true);

            // DT32350 we should be able to configure colors and symbols for different models via
            // the config manager. For now just hard code a few common SML and PXML models:
            AddLegend("Actors", Color(0.0f, 0.0f, 1.0f), SGS_stick_man);
            AddLegend("Triggers", Color(0.0f, 1.0f, 0.0f), SGS_circle_large_hollow);
            AddLegend("Camera", Color(1.0f, 1.0f, 1.0f), SGS_camera);
            AddLegend("Lights", Color(1.0f, 1.0f, 0.0f), SGS_lightbulb);
            AddLegend("Active", Color(1.0f, 0.0f, 0.0f), SGS_square);
            AddLegend("Static", Color(0.5f, 0.5f, 0.5f), SGS_square_small);
        }
    }
    EE_STACK_FREE(pServiceColors);

    return true;
}

//------------------------------------------------------------------------------------------------
void VisualTrackerService::HandleMessage(
    const ecrInput::InputActionMessage* pMsg,
    efd::Category targetChannel)
{
    EE_UNUSED_ARG(pMsg);

    EE_ASSERT(pMsg->GetClassID() == kMSGID_CoreInputAction);
    EE_ASSERT(targetChannel == kCAT_VisualTrackerAction);

    // If currently turned off, just turn on the current page:
    if (!m_showTrackers)
    {
        ToggleShowTrackers();
    }
    else
    {
        // Otherwise, hide trackers on the current page:
        SetShowPage(m_curPageIndex, false);

        // Go to the next page:
        ++m_curPageIndex;

        // If we where showing the last page, now we go into the off state:
        if (m_curPageIndex >= m_pages.size())
        {
            m_curPageIndex = 0;
            m_showTrackers = false;
        }
        else
        {
            // otherwise we display the new current page
            SetShowPage(m_curPageIndex, true);
        }
    }
}

//------------------------------------------------------------------------------------------------
//DT32478 The page name should be displayed when that page is active
efd::UInt32 VisualTrackerService::AddPage(const char* pageName)
{
    m_curPageIndex = m_pages.size();

    // Create the visual tracker render click
    NiVisualTrackerRenderClick* pClick = NiNew NiVisualTrackerRenderClick();
    pClick->SetName(pageName);
    m_pages.push_back(pClick);

    EE_ASSERT(m_pages[m_curPageIndex] == pClick);
    return m_curPageIndex;
}

//------------------------------------------------------------------------------------------------
efd::Bool VisualTrackerService::SelectPage(efd::UInt32 index)
{
    if (index < m_pages.size())
    {
        if (m_curPageIndex != index)
        {
            if (m_showTrackers)
                SetShowPage(m_curPageIndex, false);
            m_curPageIndex = index;
            if (m_showTrackers)
                SetShowPage(m_curPageIndex, true);
        }
        return true;
    }
    return false;
}

//------------------------------------------------------------------------------------------------
efd::Bool VisualTrackerService::SelectPage(const char* pageName)
{
    for (UInt32 i = 0; i < m_pages.size(); ++i)
    {
        if (m_pages[i]->GetName() == pageName)
        {
            return SelectPage(i);
        }
    }
    return false;
}

//------------------------------------------------------------------------------------------------
efd::Bool VisualTrackerService::AddChart(
    const char* pName,
    NiRect<float> dataRange,
    NiRect<float> windowRect,
    efd::TimeType fSamplingRate,
    efd::UInt32 numDecimalPlaces,
    efd::Bool updateOnlyWhenVisible)
{
    if (m_pages.size() < m_curPageIndex)
    {
        EE_LOG(efd::kGamebryoGeneral0, efd::ILogger::kERR2,
            ("Must create a page prior to adding trackers!"));
        return false;
    }

    VisualTracker* pTracker = EE_NEW VisualTracker(
        pName,
        dataRange,
        windowRect,
        fSamplingRate,
        numDecimalPlaces,
        updateOnlyWhenVisible);
    pTracker->SetShow(false);
    m_pages[m_curPageIndex]->GetVisualTrackers().AddTail(pTracker);

    return true;
}

//------------------------------------------------------------------------------------------------
efd::Bool VisualTrackerService::AddLineGraph(
    const char* pGraphName,
    NiVisualTracker::GraphCallbackObject* pObject,
    const efd::Color& color)
{
    if (m_pages.size() <= m_curPageIndex)
    {
        EE_LOG(efd::kGamebryoGeneral0, efd::ILogger::kERR2,
            ("Must create a page prior to adding graphs!"));
        return false;
    }
    NiVisualTracker* pNiTracker = m_pages[m_curPageIndex]->GetVisualTrackers().GetTail();
    VisualTracker* pTracker = static_cast<VisualTracker*>(pNiTracker);
    if (!pTracker)
    {
        EE_LOG(efd::kGamebryoGeneral0, efd::ILogger::kERR2,
            ("Must add a tracker to currect page prior to adding graphs!"));
        return false;
    }
    pTracker->AddGraph(
        pGraphName,
        pObject,
        color);

    return true;
}

//------------------------------------------------------------------------------------------------
efd::Bool VisualTrackerService::AddScatterGraphPoint(
    NiVisualTracker::DataPointCallbackObject* pObject,
    const efd::Color& color,
    ScatterGraphSymbols symbol)
{
    if (m_pages.size() <= m_curPageIndex)
    {
        EE_LOG(efd::kGamebryoGeneral0, efd::ILogger::kERR2,
            ("Must create a page prior to adding graphs!"));
        return false;
    }
    NiVisualTracker* pNiTracker = m_pages[m_curPageIndex]->GetVisualTrackers().GetTail();
    VisualTracker* pTracker = static_cast<VisualTracker*>(pNiTracker);
    if (!pTracker)
    {
        EE_LOG(efd::kGamebryoGeneral0, efd::ILogger::kERR2,
            ("Must add a tracker to currect page prior to adding graphs!"));
        return false;
    }
    pTracker->AddDataPoint(pObject, color, symbol);
    return true;
}

//------------------------------------------------------------------------------------------------
efd::Bool VisualTrackerService::AddLegend(
    const char* name,
    const efd::Color& color,
    ScatterGraphSymbols symbol)
{
    if (m_pages.size() <= m_curPageIndex)
    {
        EE_LOG(efd::kGamebryoGeneral0, efd::ILogger::kERR2,
            ("Must create a page prior to adding graphs!"));
        return false;
    }
    NiVisualTracker* pNiTracker = m_pages[m_curPageIndex]->GetVisualTrackers().GetTail();
    VisualTracker* pTracker = static_cast<VisualTracker*>(pNiTracker);
    if (!pTracker)
    {
        EE_LOG(efd::kGamebryoGeneral0, efd::ILogger::kERR2,
            ("Must add a tracker to currect page prior to adding graphs!"));
        return false;
    }
    return pTracker->AddLegend(name, color, (efd::UInt32)symbol);
}

//------------------------------------------------------------------------------------------------
VisualTrackerService::VisualTracker::VisualTracker(
    const char* pName,
    NiRect<float> dataRange,
    NiRect<float> windowRect,
    efd::TimeType fSamplingRate,
    efd::UInt32 numDecimalPlaces,
    efd::Bool updateOnlyWhenVisible)
    : NiVisualTracker(pName, dataRange, windowRect, numDecimalPlaces, false)
    , m_updateRate(fSamplingRate)
    , m_numToRemove(0)
    , m_updateOnlyWhenVisible(updateOnlyWhenVisible)
{
}

//------------------------------------------------------------------------------------------------
efd::UInt32 VisualTrackerService::VisualTracker::AddGraph(
    const char* pName,
    NiVisualTracker::GraphCallbackObject* pObject,
    const efd::Color& color,
    bool bShow)
{
    return NiVisualTracker::AddGraph(
        pObject,
        pName,
        color,
        (UInt32)(m_dataRange.m_right - m_dataRange.m_left),
        (float)m_updateRate,
        bShow);
}

//------------------------------------------------------------------------------------------------
unsigned int VisualTrackerService::VisualTracker::AddDataPoint(
    NiVisualTracker::DataPointCallbackObject* pCallbackObject,
    const efd::Color& color,
    efd::UInt32 symbol,
    bool bShow)
{
    return NiVisualTracker::AddDataPoint(
        pCallbackObject,
        color,
        symbol,
        m_dataRange,
        (float)m_updateRate,
        bShow);
}

//------------------------------------------------------------------------------------------------
efd::UInt32 VisualTrackerService::VisualTracker::IncrementNumToRemove()
{
    return ++m_numToRemove;
}

//------------------------------------------------------------------------------------------------
efd::UInt32 VisualTrackerService::VisualTracker::Update()
{
    UInt32 numRemoved = NiVisualTracker::Update();
    m_numToRemove -= numRemoved;

    return numRemoved;
}

//------------------------------------------------------------------------------------------------
class EntityLocationTracker : public NiVisualTracker::DataPointCallbackObject
{
public:
    EntityLocationTracker(egf::Entity* pEntity);
    virtual bool TakeSample(float fTime, efd::Point2& o_pt, efd::UInt32& io_symbol);

protected:
    egf::EntityPtr m_spEntity;
    float m_destructionTime;
};

//------------------------------------------------------------------------------------------------
EntityLocationTracker::EntityLocationTracker(Entity* pEntity)
: m_spEntity(pEntity)
, m_destructionTime(-1.0f)
{
}

//------------------------------------------------------------------------------------------------
bool EntityLocationTracker::TakeSample(float fTime, efd::Point2& o_pt, efd::UInt32& io_symbol)
{
    EE_UNUSED_ARG(fTime);
    EE_UNUSED_ARG(io_symbol);

    // Auto-remove myself after a timeout if my entity was removed:
    if (m_spEntity->GetEntityManager() == NULL)
    {
        // DT32352 changing symbol is not working, disabling for now:
        //if (m_destructionTime == -1.0f)
        //{
        //    // I was just destroyed, set a timer:
        //    m_destructionTime = fTime;
        //    io_symbol = VisualTrackerService::SGS_x_thin;
        //}
        // Note that the code that forces a single tick when an entity is known to
        // have been removed will need to be modified to tick multiple times until
        // the entity removed count goes to 0.

        //if (fTime > m_destructionTime + 2.0f)
        {
            return false;
        }
    }

    efd::Point3 pos(0.0f, 0.0f, 0.0f);
    m_spEntity->GetPropertyValue("Position", pos);
    o_pt.x = pos.x;
    o_pt.y = pos.y;
    return true;
}

//------------------------------------------------------------------------------------------------
void VisualTrackerService::HandleEntityMessage(const egf::EntityChangeMessage* pMsg, efd::Category)
{
    Entity* pEntity = pMsg->GetEntity();
    if (pEntity->GetModel()->ContainsProperty("Position"))
    {
        UInt32 curPage = GetCurrentPage();
        if (SelectPage("Entities"))
        {
            if (pMsg->GetClassID() == kMSGID_OwnedEntityEnterWorld)
            {
                ScatterGraphSymbols symbol = SGS_square_small;
                efd::Color color(0.5f, 0.5f, 0.5f);

                if (pEntity->GetModel()->ContainsModel("Actor"))
                {
                    symbol = SGS_stick_man;
                    color = Color(0.0f, 0.0f, 1.0f);
                }
                else if (pEntity->GetModel()->ContainsModel("Light"))
                {
                    symbol = SGS_lightbulb;
                    color = Color(1.0f, 1.0f, 0.0f);
                }
                else if (pEntity->GetModel()->ContainsModel("Camera"))
                {
                    symbol = SGS_camera;
                    color = Color(1.0f, 1.0f, 1.0f);
                }
                else if (pEntity->GetModel()->ContainsModel("PhysXTrigger"))
                {
                    symbol = SGS_circle_large_hollow;
                    color = Color(0.0f, 1.0f, 0.0f);
                }
                else
                {
                    bool isStatic = false;
                    pEntity->GetPropertyValue("IsStatic", isStatic);
                    if (!isStatic)
                    {
                        symbol = SGS_square;
                        color = Color(1.0f, 0.0f, 0.0f);
                    }
                }

                if (!pEntity->IsOwned())
                {
                    color *= 0.5f;
                }

                AddScatterGraphPoint(EE_NEW EntityLocationTracker(pEntity), color, symbol);
            }
            else if (pMsg->GetClassID() == kMSGID_OwnedEntityExitWorld)
            {
                NiVisualTracker* pNiTracker = 0;
                if (m_pages.size() <= m_curPageIndex)
                {
                    EE_LOG(efd::kGamebryoGeneral0, efd::ILogger::kERR2,
                        ("Must create a page prior to adding or removing entity data points!"));
                }
                else
                {
                    pNiTracker = m_pages[m_curPageIndex]->GetVisualTrackers().GetTail();
                }
                VisualTracker* pTracker = static_cast<VisualTracker*>(pNiTracker);
                if (!pTracker)
                {
                    EE_LOG(efd::kGamebryoGeneral0, efd::ILogger::kERR2,
                        ("Must add a tracker to currect page prior to adding or removing "
                        "entity data points!"));
                }
                else
                {
                    pTracker->IncrementNumToRemove();
                }
            }

            SelectPage(curPage);
        }
    }
}

