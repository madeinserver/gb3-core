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
#include "NiSamplePCH.h"

#include "NiSample.h"

#include <NiApplicationMetrics.h>

#include <NiUIManager.h>
#include <NiSystemCursor.h>
#include <NiNavManager.h>
#include <NiNavFlyController.h>
#include <NiNavOrbitController.h>
#include <NiUIGroup.h>

//------------------------------------------------------------------------------------------------
NiSample::NiSample(const char* pcWindowCaption,
    unsigned int uiWidth, unsigned int uiHeight, bool bUseFrameSystem,
    unsigned int uiMenuID, unsigned int uiNumStatusPanes,
    unsigned int uiBitDepth) : NiApplication(pcWindowCaption, uiWidth,
        uiHeight, bUseFrameSystem, uiMenuID, uiNumStatusPanes, uiBitDepth),
    m_uiMaxOutputLogLines(3),
    m_fUIElementHeight(0.0f),
    m_fUIElementWidth(0.0f),
    m_kUINavHelpStart(0.0f, 0.0f),
    m_kUIElementGroupOffset(0.0f, 0.0f),
    m_uiNumLogChars(0),
    m_uiNavControllerLine(UINT_MAX),
    m_fLastFrameRateTime(0.0f),
    m_bShowNavHelp(false),
    m_kNavUpAxis(0.0f, 0.0f, 1.0f),
    m_fNavDefaultScale(300.0f),
    m_bUseNavSystem(true)
{
    m_kHideAll.Initialize(this, &NiSample::HideAllPressed);
    m_kToggleNavHelp.Initialize(this, &NiSample::ToggleNavHelp);
    m_kChangeController.Initialize(this, &NiSample::ChangeController);
    m_kQuit.Initialize(this, &NiSample::QuitApplication);

    // Initialize frame rendering system object names.
    m_kUIManagerRenderStepName = "NiSample UI Manager Render Step";
    m_kUIManagerRenderClickName = "NiSample UI Manager Render Click";
    m_kCursorRenderStepName = "NiSample Cursor Render Step";
    m_kCursorRenderClickName = "NiSample Cursor Render Click";

#if defined(WIN32)
    // Disable system cursor since we're using NiCursor.
    m_bExclusiveMouse = true;
#endif // #if defined(WIN32)

#if defined(WIN32)
    // Create the cursor render click.
    m_spCursorRenderClick = NiNew NiCursorRenderClick;
    m_spCursorRenderClick->SetName(m_kCursorRenderClickName);
#endif // #if defined(WIN32)

    // Set the default skin filename
#if defined(_XENON)
    SetUISkinFilename("D:\\Data\\UISkinFull.dds");
#elif defined(_PS3)
    EE_VERIFY(FindSampleDataFile("UISkinFull_ps3.dds", m_acSkinPath));
#elif defined(WIN32)
    EE_VERIFY(FindSampleDataFile("UISkinFull.dds", m_acSkinPath));
#else
#error "Unsupported platform"
#endif
}

//------------------------------------------------------------------------------------------------
bool NiSample::Initialize()
{
    NiLogger::CreateDefaultDebugOutDestination();

    // The ShadowManager needs to be initialized before the renderer is
    //created.
    if (m_bUseFrameSystem)
    {
        NiShadowManager::Initialize();
    }

    if (!CreateRenderer())
        return false;

    if (!CreateCamera())
        return false;

    if (!CreateShaderSystem())
       return false;

    if (!CreateInputSystem())
        return false;

    if (!CreateCursor())
        return false;

    NIMETRICS_APPLICATION_TIMER(NiMetricsClockTimer, kTimer,
        CREATE_SCENE_TIME);
    NIMETRICS_APPLICATION_STARTTIMER(kTimer);
    if (!CreateScene())
    {
        NIMETRICS_APPLICATION_ENDTIMER(kTimer);
        return false;
    }
    NIMETRICS_APPLICATION_ENDTIMER(kTimer);

    if (m_bUseFrameSystem && !CreateFrame())
        return false;

    if (m_spScene)
    {
        m_spScene->Update(0.0f);
        m_spScene->UpdateProperties();
        m_spScene->UpdateEffects();
    }

    if (m_spCamera)
    {
        AdjustCameraAspectRatio(m_spCamera);
        m_spCamera->Update(0.0f);
    }

    if (!CreateUISystem())
        return false;

    if (!CreateNavigationControllers())
        return false;

    if (!CreateUIElements())
        return false;

    if (!CompleteUISystem())
        return false;

    if (!CreateVisualTrackers())
        return false;

    NIMETRICS_APPLICATION_EVENT(INITIALIZED, 1.0f);

    return true;
}

//------------------------------------------------------------------------------------------------
#if defined(WIN32)
MessageBoxFunction g_pfnGlobalFunction;

unsigned int NiSample::NiSampleMessageBoxFunc(const char* pcText,
    const char* pcCaption, void* pvExtraData)
{
    bool bExclusiveMouse = NiApplication::ms_pkApplication->IsExclusiveMouse();
    if (bExclusiveMouse)
        ::ShowCursor(TRUE);

    unsigned int uiValue = g_pfnGlobalFunction(pcText, pcCaption, pvExtraData);

    if (bExclusiveMouse)
        ::ShowCursor(FALSE);
    return uiValue;
}
#endif

//------------------------------------------------------------------------------------------------
bool NiSample::CreateInputSystem()
{
#if defined(WIN32)
    if (!m_bDumpShotAtFixedInterval)
    {
        g_pfnGlobalFunction = NiMessageBoxUtilities::GetMessageBoxFunction();
        NiMessageBoxUtilities::SetMessageBoxFunction(&NiSampleMessageBoxFunc);
    }
#endif

    bool bResult = NiApplication::CreateInputSystem();

    return bResult;
}

//------------------------------------------------------------------------------------------------
// NiShader Functions
//------------------------------------------------------------------------------------------------
bool NiSample::CreateShaderSystem()
{
    NiShaderFactory::RegisterErrorCallback(ShaderErrorCallback);

#if defined(WIN32)
    NiRenderer* pkRenderer = NiRenderer::GetRenderer();
    EE_ASSERT(pkRenderer);
    const char* pcShaderProgramDir;

    if (pkRenderer->GetRendererID() == efd::SystemDesc::RENDERER_DX9)
        pcShaderProgramDir = "Shaders/DX9";
    else if (pkRenderer->GetRendererID() == efd::SystemDesc::RENDERER_D3D10)
        pcShaderProgramDir = "Shaders/D3D10";
    else if (pkRenderer->GetRendererID() == efd::SystemDesc::RENDERER_D3D11)
        pcShaderProgramDir = "Shaders/D3D11";
    else
        pcShaderProgramDir = NULL;

    EE_ASSERT(pcShaderProgramDir);
#elif defined(_XENON)
    const char* pcShaderProgramDir = "Shaders\\Xbox360";
#elif defined(_PS3)
    const char* pcShaderProgramDir = "Shaders/PS3";
#else
    #error "Unknown platform";
#endif

    const char* pcShaderDir = "Shaders";

    NiShaderFactory::AddShaderProgramFileDirectory(ConvertMediaFilename("Shaders/Common"));
    NiShaderFactory::AddShaderProgramFileDirectory(ConvertMediaFilename(pcShaderProgramDir));

    // First, run the parsers, to make sure that any text-based shader files that have been
    // modified are re-compiled to binary before loading all the binary representations.
    if (!RunShaderParsers(pcShaderDir))
    {
        NiMessageBox("Failed to run shader parsers!", "ERROR");
        return false;
    }
    if (!RunShaderLibraries(pcShaderDir))
    {
        NiMessageBox("Failed to load shader libraries!", "ERROR");
        return false;
    }

    return true;
}

//------------------------------------------------------------------------------------------------
bool NiSample::RunShaderParsers(const char* pcShaderDir)
{
    if (!RegisterShaderParsers())
        return false;

    for (m_uiActiveCallbackIdx = 0; m_uiActiveCallbackIdx < m_kParserLibraries.GetSize();
        m_uiActiveCallbackIdx++)
    {
        const char* pcLibName = m_kParserLibraries.GetAt(m_uiActiveCallbackIdx);
        NiShaderFactory::LoadAndRunParserLibrary(pcLibName, ConvertMediaFilename(pcShaderDir),
            false);
    }

    return true;
}

//------------------------------------------------------------------------------------------------
bool NiSample::RunShaderLibraries(const char* pcShaderDir)
{
    if (!RegisterShaderLibraries())
        return false;

    unsigned int uiCount = 1;
    const char* apcDirectories[1];
    apcDirectories[0] = ConvertMediaFilename(pcShaderDir);

    for (m_uiActiveCallbackIdx = 0; m_uiActiveCallbackIdx < m_kShaderLibraries.GetSize();
        m_uiActiveCallbackIdx++)
    {
        const char* pcLibName = m_kShaderLibraries.GetAt(m_uiActiveCallbackIdx);
        if (!NiShaderFactory::LoadAndRegisterShaderLibrary(pcLibName, uiCount, apcDirectories,
            false))
        {
            NiMessageBox("Failed to load shader library!", "ERROR");
            return false;
        }
    }

    return true;
}

//------------------------------------------------------------------------------------------------
#ifdef NIDEBUG
unsigned int NiSample::ShaderErrorCallback(const char* pcError,
#else
unsigned int NiSample::ShaderErrorCallback(const char*,
#endif
    NiShaderError, bool)
{
    NiOutputDebugString("ERROR: ");
    NiOutputDebugString(pcError);
    return 0;
}

//------------------------------------------------------------------------------------------------
#if defined(WIN32) || defined(_PS3)
bool NiSample::FindSampleDataFile(const char* pcFilename, char* pcFullPath)
{
    EE_ASSERT(pcFilename && pcFullPath);

    // Build "primary" relative path:
    //   (up some # of directories) + "Media\Samples\SampleUI\" + filename

#if defined(WIN32)  // Up 5 directories for Win32...
    const char pcRelativePath[] = "..\\..\\..\\..\\..\\Media\\Samples\\SampleUI\\";
#else   // Up 6 directories for PS3...
    const char pcRelativePath[] = "../../../../../../Media/Samples/SampleUI/";
#endif

    char pcRelativeFullPath[NI_MAX_PATH];
    char pcSecondaryRelativeFullPath[NI_MAX_PATH];

    // Combine relative path and filename for "primary" path - e.g., for:
    //   Samples\GraphicsTechDemos\Picking
    NiSprintf(pcRelativeFullPath, NI_MAX_PATH, "%s%s", pcRelativePath, pcFilename);

    // Add another directory step up for "secondary" path - e.g., for:
    //   Samples\Tutorials\Graphics\03-Shaders
#if defined(WIN32)  // Win32
    NiSprintf(pcSecondaryRelativeFullPath, NI_MAX_PATH, "..\\%s", pcRelativeFullPath);
#else   // PS3
    NiSprintf(pcSecondaryRelativeFullPath, NI_MAX_PATH, "../%s", pcRelativeFullPath);
#endif

    // First try to find the file with the "primary" relative path.  If this step fails, try to
    // find the file with the "secondary" relative path.  If that step fails (and if it's Win32),
    // look relative to the path in "EMERGENT_PATH" environment variable.
    if (efd::File::Access(pcRelativeFullPath, efd::File::READ_ONLY))
    {
        NiStrcpy(pcFullPath, NI_MAX_PATH, pcRelativeFullPath);
    }
    else if (efd::File::Access(pcSecondaryRelativeFullPath, efd::File::READ_ONLY))
    {
        NiStrcpy(pcFullPath, NI_MAX_PATH, pcSecondaryRelativeFullPath);
    }
    else
    {

#if defined(WIN32)  // Win32
        // Determine path for the file using environment variable "EMERGENT_PATH" as follows:
        //   EMERGENT_PATH + /Media/Samples/SampleUI/ + filename
        char pcSourcePath[NI_MAX_PATH];
        pcSourcePath[0] = '\0';

#if _MSC_VER >= 1400
        size_t stSize = 0;
        getenv_s(&stSize, pcSourcePath, NI_MAX_PATH, "EMERGENT_PATH");
#else //#if _MSC_VER >= 1400
        char* pcEnvPathTemp = getenv("EMERGENT_PATH");
        if (pcEnvPathTemp)
            NiStrcpy(pcSourcePath, NI_MAX_PATH, pcEnvPathTemp);
#endif //#if _MSC_VER >= 1400

        if (pcSourcePath[0] != '\0')
        {
            // Append data file path
            NiSprintf(pcFullPath, NI_MAX_PATH, "%s\\Media\\Samples\\SampleUI\\%s", pcSourcePath,
                pcFilename);
        }
        else
        {
            NiMessageBox("Environment variable EMERGENT_PATH not found!", "ERROR");
            return false;
        }

        if (!efd::File::Access(pcFullPath, efd::File::READ_ONLY))
        {
            char acErrorMessage[NI_MAX_PATH];
            NiSprintf(acErrorMessage, NI_MAX_PATH, "The file: \n%s \ndoes not exist!",
                pcFullPath);
            NiMessageBox(acErrorMessage, "ERROR");
            return false;
        }
#else   // PS3
        char acErrorMessage[NI_MAX_PATH];
        NiSprintf(acErrorMessage, NI_MAX_PATH, "The file: \n%s \ndoes not exist!", pcFullPath);
        NiMessageBox(acErrorMessage, "ERROR");
        return false;
#endif

    }
    return true;
}
#endif

//------------------------------------------------------------------------------------------------
bool NiSample::CreateCursor()
{
#if defined(WIN32)
    const NiRenderTargetGroup* pkRTGroup = m_spRenderer->
        GetDefaultRenderTargetGroup();
    EE_ASSERT(pkRTGroup);

    NiRect<int> kScreenBounds(0, pkRTGroup->GetWidth(0),
        0, pkRTGroup->GetHeight(0));

    char acCursorPath[NI_MAX_PATH];

    if (!FindSampleDataFile("SystemCursors.tga", acCursorPath))
        return false;

    m_spCursor = NiSystemCursor::Create(m_spRenderer, kScreenBounds,
        NiSystemCursor::STANDARD, acCursorPath, pkRTGroup);

    if (m_spCursor == NULL)
    {
        NiMessageBox("Failed to create a cursor!", "ERROR");
        return false;
    }

    unsigned int uiMouseStartX = (kScreenBounds.m_right + kScreenBounds.m_left) / 2;
    unsigned int uiMouseStartY = (kScreenBounds.m_top + kScreenBounds.m_bottom) / 2;

    m_spCursor->SetPosition(0.0f, uiMouseStartX, uiMouseStartY);
    m_spCursor->Show(true);
    ShowCursor(FALSE);

    m_spCursorRenderClick->AppendCursor(m_spCursor);
#endif

    return true;
}

//------------------------------------------------------------------------------------------------
#if defined(WIN32)
bool NiSample::OnDefault(NiEventRef pEventRecord)
{
    return NiApplication::OnDefault(pEventRecord);
}
#endif //#if defined(WIN32)

//------------------------------------------------------------------------------------------------
const char* NiSample::GetUISkinFilename() const
{
    return m_acSkinPath;
}

//------------------------------------------------------------------------------------------------
void NiSample::SetUISkinFilename(const char* pcFilename)
{
     NiStrcpy(m_acSkinPath, NI_MAX_PATH, pcFilename);
}

//------------------------------------------------------------------------------------------------
bool NiSample::CreateUISystem()
{
    NiUIManager::Create();
    NiUIManager* pkUIManager = NiUIManager::GetUIManager();
    if (pkUIManager == NULL)
    {
        NiMessageBox("Failed to create the user interface system!", "ERROR");
        return false;
    }

    NiCursor* pkCursor = NULL;
#if defined(WIN32)
    pkCursor = m_spCursor;
#endif

    // Initialize the actual UI manager
    const char* pcBaseSkinPath = GetUISkinFilename();
    char acStandardSkinPath[NI_MAX_PATH];
    NiStrcpy(acStandardSkinPath, NI_MAX_PATH, pcBaseSkinPath);
    NiPath::Standardize(acStandardSkinPath);

    if (!pkUIManager->Initialize(GetInputSystem(), acStandardSkinPath, pkCursor))
    {
        NiMessageBox("User interface manager failed to initialize!", "ERROR");
        return false;
    }

    m_fUIElementHeight = pkUIManager->GetMaxCharHeightInNSC() * 3.0f;
    m_fUIElementWidth = NiMin(0.40f,
        pkUIManager->GetMaxCharWidthInNSC() * 25.0f);
    m_fUIGroupHeaderHeight = pkUIManager->GetMaxCharHeightInNSC() * 2.75f;
    m_kUIElementGroupOffset.x = pkUIManager->GetMaxCharWidthInNSC() * 1.5f;
    m_kUIElementGroupOffset.y = pkUIManager->GetMaxCharHeightInNSC() * 0.5f +
        m_fUIGroupHeaderHeight;

    if (m_bUseNavSystem)
    {
        if (!NiNavManager::Create())
            return false;
    }

    NiUIManager::GetUIManager()->ReserveGamePadButton(
        NiInputGamePad::NIGP_A, &m_kHideAll, NiUIManager::WASPRESSED);
    NiUIManager::GetUIManager()->ReserveKeyboardButton(
        NiInputKeyboard::KEY_Z, &m_kHideAll, NiUIManager::WASPRESSED);

    return true;
}

//------------------------------------------------------------------------------------------------
bool NiSample::CreateNavigationControllers()
{
    if (m_bUseNavSystem)
    {
        if (!NiNavManager::GetNavManager())
            return false;

        // These need to be after create scene because there are occasions
        // where the camera (m_spCamera) was not initialized before CreateScene
        m_spFlyController = NiNew NiNavFlyController(
            m_spCamera, m_spCamera, "\"Flying\" Mode", m_fNavDefaultScale,
            m_kNavUpAxis);
        m_spOrbitController = NiNew NiNavOrbitController(
            m_spCamera, "\"Orbiting\" Mode", m_fNavDefaultScale,
            m_kNavUpAxis);

        NiNavManager::GetNavManager()->AddNavigationController(
            m_spFlyController);
        NiNavManager::GetNavManager()->AddNavigationController(
            m_spOrbitController);
    }

    return true;
}

//------------------------------------------------------------------------------------------------
bool NiSample::AdjustNavigationControllers(
    NiAVObject* pkControlledObject,
    NiAVObject* pkReferenceObject,
    const NiPoint3& kUpDir,
    float fScale)
{
    if (m_bUseNavSystem)
    {
        if (m_spFlyController != NULL)
        {
            m_spFlyController->SetUpDir(kUpDir);
            m_spFlyController->SetScale(fScale);
            m_spFlyController->SetControlledObject(pkControlledObject);
            m_spFlyController->SetReferenceObject(pkReferenceObject);
        }
        if (m_spOrbitController != NULL)
        {
            m_spOrbitController->SetRadius(fScale);
            m_spOrbitController->SetObject(pkControlledObject);
            m_spOrbitController->SetUpDir(kUpDir);
        }
    }
    return true;
}

//------------------------------------------------------------------------------------------------
bool NiSample::CompleteUISystem()
{
    NiUIManager* pkUIManager = NiUIManager::GetUIManager();
    pkUIManager->PrepareRenderer();

    if (pkUIManager->GetUIGroupCount() > 1)
    {
        NiUIGroup* pkGroup = pkUIManager->SetSelectedUIGroup(1);
        // if the UI group exists, advance the focus to the first real element
        // if possible.
        if (pkGroup)
            pkGroup->AdvanceFocus();
    }

    return true;
}

//------------------------------------------------------------------------------------------------
bool NiSample::CreateFrame()
{
    if (!NiApplication::CreateFrame())
    {
        return false;
    }

    // Create UI manager render step.
    NiUIManagerRenderClick* pkUIManagerRenderClick =
        NiNew NiUIManagerRenderClick;
    pkUIManagerRenderClick->SetName(m_kUIManagerRenderClickName);
    NiDefaultClickRenderStep* pkUIManagerRenderStep = NiNew
        NiDefaultClickRenderStep;
    pkUIManagerRenderStep->SetName(m_kUIManagerRenderStepName);
    pkUIManagerRenderStep->AppendRenderClick(pkUIManagerRenderClick);

    // Append UI manager render step to frame.
    EE_ASSERT(m_spFrame);
    m_spFrame->AppendRenderStep(pkUIManagerRenderStep);

#if defined(WIN32)
    // Create cursor render step.
    NiDefaultClickRenderStep* pkCursorRenderStep = NiNew
        NiDefaultClickRenderStep;
    pkCursorRenderStep->SetName(m_kCursorRenderStepName);
    pkCursorRenderStep->AppendRenderClick(m_spCursorRenderClick);

    // Append cursor render step to frame.
    m_spFrame->AppendRenderStep(pkCursorRenderStep);
#endif  // #if defined(WIN32)

    return true;
}

//------------------------------------------------------------------------------------------------
void NiSample::RenderFrame()
{
    if (m_bUseFrameSystem)
    {
        // Set renderer's sorter on the accumulator processor if a
        // accumulator processor is used.
        NiAccumulatorProcessor* pkAccumProc =
            NiDynamicCast(NiAccumulatorProcessor, m_spRenderListProcessor);
        if (pkAccumProc)
        {
            pkAccumProc->SetAccumulator(m_spRenderer->GetSorter());
        }

        // Draw the frame.
        m_spFrame->Draw();

        // Gather rendering statistics.
        m_spFrame->GatherStatistics(m_uiNumObjectsDrawn, m_fCullTime,
            m_fRenderTime);
    }
    else
    {
        m_spRenderer->BeginUsingDefaultRenderTargetGroup(
            NiRenderer::CLEAR_ALL);
        CullFrame();

        BeginRender();
        NiDrawVisibleArray(m_spCamera, m_kVisible);

        RenderScreenItems();

        RenderUIElements();

        EndRender();

        RenderVisualTrackers();
        m_spRenderer->EndUsingRenderTargetGroup();
    }
}

//------------------------------------------------------------------------------------------------
void NiSample::RenderUIElements()
{
    NiUIManager::GetUIManager()->Draw(m_spRenderer);

#if defined(WIN32)
    m_spCursor->Draw();
#endif
}

//------------------------------------------------------------------------------------------------
void NiSample::Terminate()
{
    for (unsigned int ui = 0; ui < m_kLogLines.GetSize(); ui++)
        NiDelete m_kLogLines.GetAt(ui);
    m_kLogLines.RemoveAll();

    m_spOrbitController = NULL;
    m_spFlyController = NULL;
    m_spOutputLog = NULL;
    m_spFrameRateLabel = NULL;
    m_kNavHelpRenderGroups.RemoveAll();

#if defined(WIN32)
    m_spCursorRenderClick = NULL;
    m_spCursor = NULL;

    NiSystemCursor::Shutdown();
#endif // #if defined(WIN32)

    // The Navigation Manager must shut down before the UI Manager
    NiNavManager::Shutdown();
    NiUIManager::Shutdown();

    NiShaderFactory::UnregisterAllLibraries();

    NiApplication::Terminate();
}

//------------------------------------------------------------------------------------------------
void NiSample::UpdateFrame()
{
    NiApplication::UpdateFrame();

#if defined(WIN32)
    NiInputMouse* pkMouse = GetInputSystem()->GetMouse();
    if ((pkMouse != NULL) && (m_spCursor != NULL))
    {
        int iX, iY, iZ = 0;
        if (pkMouse->GetPositionDelta(iX, iY, iZ))
        {
            if ((iX != 0) || (iY != 0))
                m_spCursor->Move(0.0f, iX, iY);
        }
    }
#endif
}

//------------------------------------------------------------------------------------------------
void NiSample::EndUpdate()
{
    // UI system updated after application updates scene,
    // allowing cameras to be placed correctly.
    if (NiUIManager::GetUIManager())
    {
        NiUIManager::GetUIManager()->UpdateUI();
        if (m_bUseNavSystem)
        {
            NiNavManager::GetNavManager()->Update(m_fAccumTime);
            if (m_spCamera != NULL)
                m_spCamera->Update(m_fAccumTime);
        }
    }

    NiApplication::EndUpdate();

    const unsigned int uiClicksPerUpdate = 30;
    if (m_spFrameRateLabel && (m_iClicks % uiClicksPerUpdate == 0)
        && m_fAccumTime > m_fLastFrameRateTime)
    {
        float fFrameRate = uiClicksPerUpdate /
                (m_fAccumTime - m_fLastFrameRateTime);

        m_fLastFrameRateTime = m_fAccumTime;

        char acString[32];
        NiSprintf(acString, 32, "Frame Rate: %.1f", fFrameRate);
        m_spFrameRateLabel->SetText(acString);
    }
}

//------------------------------------------------------------------------------------------------
void NiSample::HideAllPressed(unsigned char)
{
    NiUIManager* pkManager = NiUIManager::GetUIManager();
    if (pkManager)
    {
        pkManager->SetVisible(!pkManager->IsVisible());
    }
}

//------------------------------------------------------------------------------------------------
bool NiSample::CreateUIElements()
{
    unsigned int uiWidth, uiHeight;
    NiRenderer::GetRenderer()->ConvertFromNDCToPixels(1.0f, 1.0f,
        uiWidth, uiHeight);

    NiPoint2 kDimensions(0.0f, 0.0f);

    NiUIGroup* pkLogGroup = NiNew NiUIGroup("Output Log",
        m_fUIGroupHeaderHeight, true);

    float fLogWidth = NiMin(0.45f,
        NiUIManager::GetUIManager()->GetMaxCharWidthInNSC() * 35.0f);
    float fCharHeight = NiUIManager::GetUIManager()->GetMaxCharHeightInNSC();
    float fFRHeight = fCharHeight;
    float fLogHeight = fFRHeight * (float)m_uiMaxOutputLogLines;
    kDimensions.x += fLogWidth + 2.0f * m_kUIElementGroupOffset.x;
    kDimensions.y += m_kUIElementGroupOffset.y;

    m_spFrameRateLabel = NiNew NiUILabel("", NiColorA::BLACK, NiColor::WHITE);
    m_spFrameRateLabel->SetOffset(m_kUIElementGroupOffset.x, kDimensions.y);
    m_spFrameRateLabel->SetDimensions(fLogWidth, fFRHeight);
    m_spFrameRateLabel->SetAlignment(NiUILabel::TOP, NiUILabel::LEFT);
    pkLogGroup->AddChild(m_spFrameRateLabel);
    kDimensions.y += fFRHeight + 0.5f * fCharHeight;

    m_spOutputLog = NiNew NiUILabel("", NiColorA::BLACK, NiColor::WHITE);
    m_spOutputLog->SetOffset(m_kUIElementGroupOffset.x, kDimensions.y);
    m_spOutputLog->SetDimensions(fLogWidth, fLogHeight);
    m_spOutputLog->SetAlignment(NiUILabel::TOP, NiUILabel::LEFT);
    pkLogGroup->AddChild(m_spOutputLog);
    kDimensions.y += fLogHeight + fCharHeight;

    AddLogEntry("To hide the log and all UI elements, press \'Z\' on a "
        "keyboard or left analog stick press on a Gamepad.");

    pkLogGroup->SetOffset(0.0f, 0.0f);
    pkLogGroup->SetDimensions(kDimensions.x, kDimensions.y);
    pkLogGroup->UpdateRect();
    NiUIManager::GetUIManager()->AddUIGroup(pkLogGroup);

    m_kUINavHelpStart.x = 0.0f;
    m_kUINavHelpStart.y = kDimensions.y + fCharHeight;

    if (m_bUseNavSystem)
    {
        m_uiNavControllerLine = AddLogEntry("");

        NiNavBaseController* pkController =
            NiNavManager::GetNavManager()->GetCurrentNavController();
        UpdateControllerLog(pkController);
    }

    return true;
}

//------------------------------------------------------------------------------------------------
void NiSample::AddDefaultUIElements(NiUIGroup* pkGroup,
    float& fLeftOffset, float& fTopOffset, float fElementWidth,
    float fElementHeight)
{
    EE_ASSERT(pkGroup);

    NiUIButton* pkButton;
    if (m_bUseNavSystem)
    {
        if (NiNavManager::GetNavManager()->GetNumNavigationControllers() > 1)
        {
            pkButton = NiNew NiUIButton("Change Control Mode");
            pkButton->SetOffset(fLeftOffset, fTopOffset);
            pkButton->SetDimensions(fElementWidth, fElementHeight);
            pkButton->AddKeyboardHotkey(NiInputKeyboard::KEY_C);
            pkButton->AddGamePadHotkey(NiInputGamePad::NIGP_RLEFT);
            pkButton->SubscribeToPressEvent(&m_kChangeController);
            pkGroup->AddChild(pkButton);
            fTopOffset += fElementHeight;
        }
        pkButton = NiNew NiUIButton("Display Control Help");
        pkButton->SetOffset(fLeftOffset, fTopOffset);
        pkButton->SetDimensions(fElementWidth, fElementHeight);
        pkButton->AddKeyboardHotkey(NiInputKeyboard::KEY_V);
        pkButton->SubscribeToPressEvent(&m_kToggleNavHelp);
        pkGroup->AddChild(pkButton);
        fTopOffset += fElementHeight;
    }

    pkButton = NiNew NiUIButton("Quit");
    pkButton->SetOffset(fLeftOffset, fTopOffset);
    pkButton->SetDimensions(fElementWidth, fElementHeight);
    pkButton->AddKeyboardHotkey(NiInputKeyboard::KEY_ESCAPE);
    pkButton->AddGamePadHotkey(NiInputGamePad::NIGP_START,
        NiInputGamePad::NIGP_MASK_SELECT);
    pkButton->SubscribeToPressEvent(&m_kQuit);
    pkGroup->AddChild(pkButton);
    fTopOffset += fElementHeight;

    AddNavigationHelpUIElements(m_kUINavHelpStart.x, m_kUINavHelpStart.y,
        fElementWidth, fElementHeight, 0.1f, 0.4f);
}

//------------------------------------------------------------------------------------------------
void NiSample::AddNavigationHelpUIElements(float fLeftOffset, float fTopOffset,
    float fElementWidth, float fElementHeight, float fImageWidth,
    float fMinLabelWidth)
{
    if (NiUIManager::GetUIManager() == NULL)
        return;

    if (!m_bUseNavSystem)
        return;

    // Note that this code does not attempt to reserve the controls before
    // setting up the images.  This is because navigation controllers need
    // to be swappable.  If they can't swap around, then only one controller
    // could bind to a key or axis (WASD for instance).  However, it is left
    // up to the application programmer to ensure that the Ni*NavController
    // actually gets these controls (i.e. that they aren't bound by something
    // else.  If there are problems, the application programmer can always
    // set the Ni*NavController's bindings to something else.

    NiNavManager* pkNavManager = NiNavManager::GetNavManager();
    if (!pkNavManager)
        return;

    unsigned int uiNumNavControllers =
        pkNavManager->GetNumNavigationControllers();
    unsigned int uiCurrentNavIdx =
        pkNavManager->GetCurrentNavControllerIndex();

    for (unsigned int uiNavIdx = 0; uiNavIdx < uiNumNavControllers; uiNavIdx++)
    {
        const NiNavBaseController* pkNavController =
            pkNavManager->GetNavControllerAt(uiNavIdx);

        if (!pkNavController)
            continue;

        NiUIRenderGroupPtr spRenderGroup = NiNew NiUIRenderGroup();

        unsigned int uiIdx;
        float fCurTop = 0.0f;
        float fRemainingWidth;
        float fWidth = fElementWidth;

        NiUILabelPtr spLabel;
        NiUIHotkeyImagePtr spHotkeyImg;
        unsigned int uiNumControls = pkNavController->GetControlCount();

        for (uiIdx = 0; uiIdx < uiNumControls; ++uiIdx)
        {
            if (!pkNavController->IsControlActive(uiIdx))
                continue;

            // First, we add the hotkey images where appropriate
            spHotkeyImg = NiNew NiUIHotkeyImage();

            const NiNavBaseController::InputBinding& kPosKey =
                pkNavController->GetBinding(uiIdx,
                NiNavBaseController::POSITIVE_AXIS);
            const NiNavBaseController::InputBinding& kNegKey =
                pkNavController->GetBinding(uiIdx,
                NiNavBaseController::NEGATIVE_AXIS);
            if (NiUIManager::GetUIManager()->KeyboardExists())
            {
                if (kPosKey.SupportsKeyboard())
                {
                    spHotkeyImg->SetKeyboardHotkey(kPosKey.eKeyboardKey,
                        kPosKey.eKeyboardModifier);
                }
                if (kNegKey.SupportsKeyboard())
                {
                    spHotkeyImg->SetKeyboardHotkey(kNegKey.eKeyboardKey,
                        kNegKey.eKeyboardModifier);
                }
            }
            if (NiUIManager::GetUIManager()->MouseExists())
            {
                // Note that the assert makes sure that the negative direction
                // also uses a mouse axis and that they both use the same axis
                if (kPosKey.SupportsMouseAxis())
                {
                    EE_ASSERT(kNegKey.SupportsMouseAxis());
                    EE_ASSERT(kNegKey.eMouseAxis == kPosKey.eMouseAxis);

                    if (kPosKey.eMouseAxis == NiInputMouse::NIM_AXIS_Z)
                        spHotkeyImg->SetMouseHotkey(NiInputMouse::NIM_MIDDLE);
                    else
                        spHotkeyImg->SetMouseMotion();
                }
                else
                {
                    if (kPosKey.SupportsMouseButton())
                    {
                        spHotkeyImg->SetMouseHotkey(kPosKey.eMouseButton,
                            kPosKey.eMouseModifier);
                    }
                    if (kNegKey.SupportsMouseButton())
                    {
                        spHotkeyImg->SetMouseHotkey(kNegKey.eMouseButton,
                            kNegKey.eMouseModifier);
                    }
                }
            }
            if (NiUIManager::GetUIManager()->GamePadExists())
            {
                // Note that the assert makes sure that the negative direction
                // also uses the Gamebryo axis and that they use the same axis.
                if (kPosKey.SupportsGamePadAxis())
                {
                    EE_ASSERT(kNegKey.SupportsGamePadAxis());
                    EE_ASSERT(kNegKey.eGamePadAxis == kPosKey.eGamePadAxis);
                    spHotkeyImg->SetGamePadAxis(kPosKey.eGamePadAxis);
                }
                else
                {
                    if (kPosKey.SupportsGamePadButton())
                    {
                        spHotkeyImg->SetGamePadHotkey(kPosKey.eGamePadButton,
                            kPosKey.eGamePadModifier);
                    }
                    if (kNegKey.SupportsGamePadButton())
                    {
                        spHotkeyImg->SetGamePadHotkey(kNegKey.eGamePadButton,
                            kNegKey.eGamePadModifier);
                    }
                }
            }
            // Then we decide how much space is needed for the hotkey images
            fRemainingWidth = fWidth - (spHotkeyImg->GetTotalWidth() *
                fImageWidth * fWidth);
            fRemainingWidth =
                (fRemainingWidth < (fMinLabelWidth * fWidth)) ?
                (fMinLabelWidth * fWidth) : fRemainingWidth;

            // Then we set the label and images where they belong
            spHotkeyImg->SetOffset(fRemainingWidth,
                fCurTop);
            spHotkeyImg->SetDimensions(fWidth - fRemainingWidth,
                fElementHeight);

            spLabel = NiNew NiUILabel(pkNavController->GetControlName(uiIdx),
                NiColorA(0.0f, 0.0f, 0.0f, 0.5f), NiColor::WHITE);
            spLabel->SetOffset(0.0f, fCurTop);
            spLabel->SetDimensions(fRemainingWidth, fElementHeight);
            spLabel->SetAlignment(NiUILabel::VERT_MIDDLE);
            fCurTop += fElementHeight;

            // Add them to the list and then decrement their smart pointer
            // count
            spRenderGroup->AddChild(spLabel);
            spRenderGroup->AddChild(spHotkeyImg);
            spHotkeyImg = NULL;
            spLabel = NULL;
        }

        // If there are no controls, add a simple label
        if (uiNumControls == 0)
        {
            spLabel = NiNew NiUILabel(
                "This navigation mode has no controls.",
                NiColorA(0.0f, 0.0f, 0.0f, 0.5f),
                NiColor::WHITE);
            spLabel->SetOffset(0.0f, fCurTop);
            spLabel->SetDimensions(fElementWidth, fElementHeight);
            spLabel->SetAlignment(NiUILabel::VERT_MIDDLE);
            fCurTop += fElementHeight;

            // Add them to the list and then decrement their smart pointer
            // count
            spRenderGroup->AddChild(spLabel);
            spLabel = NULL;
        }

        m_kNavHelpRenderGroups.SetAtGrow(uiNavIdx, spRenderGroup);
        NiUIManager::GetUIManager()->RegisterUIRenderGroup(spRenderGroup);
        spRenderGroup->SetOffset(fLeftOffset, fTopOffset);
        spRenderGroup->UpdateRect();
        spRenderGroup->InitializeScreenElements();

        if (uiNavIdx == uiCurrentNavIdx)
            spRenderGroup->SetVisible(m_bShowNavHelp);
        else
            spRenderGroup->SetVisible(false);
    }
}

//------------------------------------------------------------------------------------------------
unsigned int NiSample::AddLogEntry(const char* pcNewLine)
{
    // This code adds the new line to my array
    size_t stNewLineLen = strlen(pcNewLine);

    unsigned int uiRetVal = m_kLogLines.Add(NiNew NiString(pcNewLine));
    m_uiNumLogChars =  (unsigned int)(m_uiNumLogChars + stNewLineLen + 2);
    ReinitializeLogText();

    return uiRetVal;
}

//------------------------------------------------------------------------------------------------
bool NiSample::ChangeLogEntry(unsigned int uiLineNum,
    const char* pcNewLine)
{
    if (uiLineNum >= m_kLogLines.GetSize())
        return false;

    unsigned int uiOldLineLen = 0;
    size_t stNewLineLen = strlen(pcNewLine);

    NiString* pkLine = m_kLogLines.GetAt(uiLineNum);
    if (pkLine)
    {
        uiOldLineLen = pkLine->Length();
        *(pkLine) = pcNewLine;
    }
    m_uiNumLogChars =
        (unsigned int)(m_uiNumLogChars + stNewLineLen - uiOldLineLen);
    ReinitializeLogText();

    return true;
}

//------------------------------------------------------------------------------------------------
void NiSample::ReinitializeLogText()
{
    char acLogText[1024];
    EE_ASSERT(m_uiNumLogChars <= 1024);

    char* pcLogText = acLogText;
    unsigned int uiSize, uiIdx, uiCurChar, uiCurLineLen;
    uiSize = m_kLogLines.GetSize();

    uiCurChar = 0;
    for (uiIdx = 0; uiIdx < uiSize; ++uiIdx)
    {
        NiString* pkLine = m_kLogLines.GetAt(uiIdx);
        if (!pkLine)
            continue;

        uiCurLineLen = pkLine->Length();
        const char* pcSrc = *(pkLine);

        memcpy(pcLogText + uiCurChar, pcSrc, uiCurLineLen);
        uiCurChar += uiCurLineLen;
        pcLogText[uiCurChar] = '\n';
        ++uiCurChar;
    }
    if (uiCurChar == 0)
        pcLogText[uiCurChar] = '\0';
    else
        pcLogText[uiCurChar - 1] = '\0';

    unsigned int uiNumLines = 0;
    for (unsigned int ui = 0; ui < uiCurChar; ui++)
    {
        if (pcLogText[ui] == '\n')
            uiNumLines++;
    }
    EE_ASSERT(uiNumLines <= m_uiMaxOutputLogLines);

    EE_ASSERT(m_spOutputLog != NULL);
    m_spOutputLog->SetText(pcLogText);
    EE_ASSERT(uiNumLines <= m_spOutputLog->GetVisualLineCount());
    EE_ASSERT(m_spOutputLog->GetVisualLineCount() <= m_uiMaxOutputLogLines);
}

//------------------------------------------------------------------------------------------------
void NiSample::UpdateControllerLog(NiNavBaseController* pkNewController)
{
    if (pkNewController && UINT_MAX != m_uiNavControllerLine)
    {
        char acString[256];
        NiSprintf(acString, 256, "Navigation Controller Type: %s",
            (const char*) pkNewController->GetName());
        EE_VERIFY(ChangeLogEntry(m_uiNavControllerLine, acString));
    }
}

//------------------------------------------------------------------------------------------------
void NiSample::ToggleNavHelp()
{
    if (m_bUseNavSystem)
    {
        NiNavManager* pkManager = NiNavManager::GetNavManager();
        m_bShowNavHelp = !m_bShowNavHelp;

        if (pkManager)
        {
            unsigned int uiIdx = pkManager->GetCurrentNavControllerIndex();
            NiUIRenderGroup* pkGroup = m_kNavHelpRenderGroups.GetAt(uiIdx);
            if (pkGroup)
                pkGroup->SetVisible(m_bShowNavHelp);
        }
    }
}

//------------------------------------------------------------------------------------------------
void NiSample::ChangeController()
{
    if (!m_bUseNavSystem)
        return;

    NiNavManager* pkManager = NiNavManager::GetNavManager();
    EE_ASSERT(pkManager != NULL);

    unsigned int uiOldIdx = pkManager->GetCurrentNavControllerIndex();
    pkManager->AdvanceController();
    NiNavBaseController* pkNewController =
        pkManager->GetCurrentNavController();
    unsigned int uiNewIdx = pkManager->GetCurrentNavControllerIndex();

    NiUIRenderGroup* pkOldGroup = m_kNavHelpRenderGroups.GetAt(uiOldIdx);
    if (pkOldGroup)
        pkOldGroup->SetVisible(false);

    NiUIRenderGroup* pkNewGroup = m_kNavHelpRenderGroups.GetAt(uiNewIdx);
    if (pkNewGroup)
        pkNewGroup->SetVisible(m_bShowNavHelp);

    UpdateControllerLog(pkNewController);
}

//------------------------------------------------------------------------------------------------
