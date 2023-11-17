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
#include "NiVisualTrackerPCH.h"

#include "NiVisualTracker.h"
#include <NiMaterialProperty.h>
#include <NiZBufferProperty.h>
#include <NiWireframeProperty.h>
#include <NiVertexColorProperty.h>
#include <NiCamera.h>
#include <NiMesh.h>
#include <NiDataStream.h>
#include <NiDataStreamElement.h>
#include <NiSystem.h>
#include <NiRect.h>
#include <NiRenderer.h>
#include <NiMeshScreenElements.h>
#include <NiTexturingProperty.h>
#include <NiSourceTexture.h>
#include <NiDataStreamLock.h>
#include <NiCommonSemantics.h>
#include "Courier16.h"

extern const unsigned char g_SymbolPixels[];

typedef NiTStridedRandomAccessIterator<NiPoint3>    Point3Iter;

//------------------------------------------------------------------------------------------------
//  NiVisualTrackerBase implementation
//------------------------------------------------------------------------------------------------
NiVisualTrackerBase::NiVisualTrackerBase(NiRect<float> kWindowRect,
    bool bShow,
    const char* pcName)
: m_kDimensions(kWindowRect)
, m_bShow(bShow)
, m_kScreenTextures(6, 1)
{
    NiSprintf(m_acName, 255, "%s", pcName);
    m_acName[255] = '\0';
}

//------------------------------------------------------------------------------------------------
NiVisualTrackerBase::~NiVisualTrackerBase()
{
}

//------------------------------------------------------------------------------------------------
void NiVisualTrackerBase::SetShow(bool bShow)
{
    m_bShow = bShow;
}

//------------------------------------------------------------------------------------------------
bool NiVisualTrackerBase::GetShow()
{
    return m_bShow;
}

//------------------------------------------------------------------------------------------------
NiUInt32 NiVisualTrackerBase::Update()
{
    return 0;
}

//------------------------------------------------------------------------------------------------
void NiVisualTrackerBase::Draw()
{
}

//------------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------------
//  NiVisualTracker implementation
//------------------------------------------------------------------------------------------------
NiVisualTracker::NiVisualTracker(float fMaxValue,
    unsigned int uiNumDecimalPlaces,
    NiRect<float> windowRect,
    const char* pcName,
    bool bShow,
    unsigned int uiNumTrackers)
: NiVisualTrackerBase(windowRect, bShow, pcName)
, m_callbackData(uiNumTrackers, 1)
, m_dataRange(0.0f, 1.0f, fMaxValue, 0.0f)
{
    Setup(uiNumTrackers, uiNumDecimalPlaces);
}

//------------------------------------------------------------------------------------------------
NiVisualTracker::NiVisualTracker(
    const char* pcName,
    NiRect<float> dataRange,
    NiRect<float> windowRect,
    efd::UInt32 uiNumDecimalPlaces,
    bool bShow,
    efd::UInt32 uiNumTrackers)
: NiVisualTrackerBase(windowRect, bShow, pcName)
, m_callbackData(uiNumTrackers, 1)
, m_dataRange(dataRange)
{
    Setup(uiNumTrackers, uiNumDecimalPlaces);
}

//------------------------------------------------------------------------------------------------
void NiVisualTracker::Setup(efd::UInt32 uiNumTrackers, efd::UInt32 uiNumDecimalPlaces)
{
    Ni2DBuffer* pkBuffer = NiRenderer::GetRenderer()->GetDefaultBackBuffer();
    EE_ASSERT(pkBuffer);

    unsigned int uiBBWidth = pkBuffer->GetWidth();
    unsigned int uiBBHeight = pkBuffer->GetHeight();

    m_pTextTitle = NiNew ScreenText(32, &m_kScreenTextures, NiColorA::WHITE);

    m_spWindowRoot = NiNew NiNode(uiNumTrackers);
    m_spWindowRoot->SetTranslate(NiPoint3(m_kDimensions.m_left, m_kDimensions.m_top, 0.1f));

    // Graph visualization dimensions
    float fOuterWidth = NiAbs(m_kDimensions.m_left - m_kDimensions.m_right);
    float fOuterHeight = NiAbs(m_kDimensions.m_top - m_kDimensions.m_bottom);

    unsigned int uiSpacing = m_pTextTitle->GetCharacterSpacing();
    float fNormTextWidth = ((float)uiSpacing)/((float)uiBBWidth);
    float fNormTextHeight = ((float)m_pTextTitle->GetHeight())/((float)uiBBHeight);

    m_graphLineDimensions.m_left = 0.0f;
    m_graphLineDimensions.m_right = fOuterWidth - 0.10f*fOuterWidth;
    m_graphLineDimensions.m_top = fNormTextHeight;
    m_graphLineDimensions.m_bottom = fOuterHeight - fNormTextHeight;

    float fWidth = NiAbs(m_graphLineDimensions.m_left - m_graphLineDimensions.m_right);
    float fHeight = NiAbs(m_graphLineDimensions.m_top - m_graphLineDimensions.m_bottom);

    // Create the border line segments
    unsigned int uiNumVerts = 11;
    m_spBorders = GraphCallbackObjectData::CreateLines(
        uiNumVerts,
        NiColor::WHITE,
        m_graphLineDimensions);

    // Obtain positions lock (unlocks when goes out of scope)
    NiDataStreamRef* pkStreamRef = m_spBorders->FindStreamRef(
        "POSITION",
        0,
        NiDataStreamElement::F_FLOAT32_3);
    EE_ASSERT(pkStreamRef);

    // Scope the lock appropriately
    {
        NiDataStreamLock kPosLock(pkStreamRef->GetDataStream(), 0);

        Point3Iter kPosIt = kPosLock.begin<NiPoint3>();

        // Build vertex data and region data
        NiDataStream* pkDataStream = kPosLock.GetDataStream();

        // Clear region information
        pkDataStream->RemoveAllRegions();

        // Remember y runs from top (0.0) to bottom (1.0) in Gamebryo screen space.

        // The frame vertices
        *kPosIt++ = NiPoint3(0.0f,   fHeight, 0.0f);
        *kPosIt++ = NiPoint3(fWidth, fHeight, 0.0f);
        *kPosIt++ = NiPoint3(fWidth, 0.0f, 0.0f);
        *kPosIt++ = NiPoint3(0.0f, 0.0f, 0.0f);
        *kPosIt++ = NiPoint3(0.0f, fHeight, 0.0f);
        pkDataStream->AddRegion(NiDataStream::Region(0, 5));
        pkStreamRef->BindRegionToSubmesh(0,0);

        // The 0.25% line
        *kPosIt++ = NiPoint3(0.0f, 0.75f*fHeight, 0.0f);
        *kPosIt++ = NiPoint3(fWidth, 0.75f*fHeight, 0.0f);
        pkDataStream->AddRegion(NiDataStream::Region(5, 2));
        pkStreamRef->BindRegionToSubmesh(1,1);

        // The 0.50% line
        *kPosIt++ = NiPoint3(0.0f, 0.5f*fHeight, 0.0f);
        *kPosIt++ = NiPoint3(fWidth, 0.5f*fHeight, 0.0f);
        pkDataStream->AddRegion(NiDataStream::Region(7, 2));
        pkStreamRef->BindRegionToSubmesh(2,2);

        // The 0.75% line
        *kPosIt++ = NiPoint3(0.0f, 0.25f*fHeight, 0.0f);
        *kPosIt++ = NiPoint3(fWidth, 0.25f*fHeight, 0.0f);
        pkDataStream->AddRegion(NiDataStream::Region(9, 2));
        pkStreamRef->BindRegionToSubmesh(3,3);
    }

    // Obtain positions lock (unlocks when goes out of scope)
    NiMesh* pkMesh = ((NiMesh*)(m_spBorders.data()));
    pkMesh->RecomputeBounds();
    pkMesh->SetSubmeshCount(4);

    // Attach object to scene
    m_spWindowRoot->AttachChild(m_spBorders);

    // Build labels
    size_t stLen = strlen(m_acName);

    float fOffset = fWidth - fNormTextWidth*(float)stLen;
    float fNormX;

    if (fOffset > 1.0f)
        fOffset = 0.0f;

    fNormX = fOffset / 2.0f;

    unsigned int uiXPos, uiYPos;
    uiXPos = (unsigned int)((fNormX + m_kDimensions.m_left) * (float)uiBBWidth);
    uiYPos = (unsigned int)((m_kDimensions.m_top) * (float)uiBBHeight);
    m_pTextTitle->SetTextOrigin(uiXPos, uiYPos);
    m_pTextTitle->SetVisible(true);
    m_pTextTitle->SetString(m_acName);

    char acNumberString[16];
    uiXPos = (NiUInt32)((m_kDimensions.m_left +
        m_graphLineDimensions.m_right + 0.01f) * (float)uiBBWidth);
    uiYPos = (NiUInt32)((m_kDimensions.m_top +
        m_graphLineDimensions.m_top - 0.5f * fNormTextHeight) * (float)uiBBHeight);

    NiUInt32 uiHeightIncrement = (NiUInt32)((0.25f*fHeight)*(float)uiBBHeight);

    char acFormatString[128];
    NiSprintf(acFormatString, 127, "%%.%df", uiNumDecimalPlaces);

    // 100% text
    NiSprintf(acNumberString, 14, acFormatString, m_dataRange.m_top);
    acNumberString[15] = '\0';
    m_pTextOneHundred = NiNew ScreenText(15, &m_kScreenTextures, NiColorA::WHITE);
    m_pTextOneHundred->SetTextOrigin(uiXPos, uiYPos);
    m_pTextOneHundred->SetVisible(true);
    m_pTextOneHundred->SetString(acNumberString);

    // 75% text
    uiYPos += uiHeightIncrement;
    NiSprintf(acNumberString, 14, acFormatString,
        0.75f*(m_dataRange.m_top - m_dataRange.m_bottom) + m_dataRange.m_bottom);
    acNumberString[15] = '\0';
    m_pTextSeventyFive = NiNew ScreenText(15, &m_kScreenTextures, NiColorA::WHITE);
    m_pTextSeventyFive->SetTextOrigin(uiXPos, uiYPos);
    m_pTextSeventyFive->SetVisible(true);
    m_pTextSeventyFive->SetString(acNumberString);

    // 50% text
    uiYPos += uiHeightIncrement;
    NiSprintf(acNumberString, 14, acFormatString,
        0.5f*(m_dataRange.m_top + m_dataRange.m_bottom));
    acNumberString[15] = '\0';
    m_pTextFifty = NiNew ScreenText(15, &m_kScreenTextures, NiColorA::WHITE);
    m_pTextFifty->SetTextOrigin(uiXPos, uiYPos);
    m_pTextFifty->SetVisible(true);
    m_pTextFifty->SetString(acNumberString);

    // 25% text
    uiYPos += uiHeightIncrement;
    NiSprintf(acNumberString, 14, acFormatString,
        0.25f*(m_dataRange.m_top - m_dataRange.m_bottom) + m_dataRange.m_bottom);
    acNumberString[15] = '\0';
    m_pTextTwentyFive = NiNew ScreenText(15, &m_kScreenTextures, NiColorA::WHITE);
    m_pTextTwentyFive->SetTextOrigin(uiXPos, uiYPos);
    m_pTextTwentyFive->SetVisible(true);
    m_pTextTwentyFive->SetString(acNumberString);

    // 0.0% text
    uiYPos += uiHeightIncrement;
    NiSprintf(acNumberString, 14, acFormatString, m_dataRange.m_bottom);
    acNumberString[15] = '\0';
    m_pTextZero = NiNew ScreenText(32, &m_kScreenTextures, NiColorA::WHITE);
    m_pTextZero->SetTextOrigin(uiXPos, uiYPos);
    m_pTextZero->SetVisible(true);
    m_pTextZero->SetString(acNumberString);

    // Define legend location
    m_uiLegendX = (unsigned int)((m_kDimensions.m_left) * (float)uiBBWidth);
    m_uiLegendY = (unsigned int)((m_kDimensions.m_bottom - fNormTextHeight) * (float)uiBBHeight);
}

//------------------------------------------------------------------------------------------------
NiVisualTracker::~NiVisualTracker()
{
    m_spBorders = 0;

    for (unsigned int ui = 0; ui < m_callbackData.GetSize(); ui++)
    {
        NiDelete m_callbackData.GetAt(ui);
    }

    NiDelete m_pTextTitle;
    NiDelete m_pTextZero;
    NiDelete m_pTextTwentyFive;
    NiDelete m_pTextFifty;
    NiDelete m_pTextSeventyFive;
    NiDelete m_pTextOneHundred;

}

//------------------------------------------------------------------------------------------------
unsigned int NiVisualTracker::AddGraph(
    GraphCallbackObject* pCallbackObject,
    const char* pcName,
    const NiColor& color,
    unsigned int uiNumSamplesToKeep,
    float fSamplingTime,
    bool bShow)
{
    LegendEntryPtr spLegend =
        EE_NEW LegendEntry(pcName, color, k_NO_SYMBOL, m_uiLegendX, m_uiLegendY, m_kScreenTextures);

    GraphCallbackObjectData* pkCBData = NiNew GraphCallbackObjectData(
        pCallbackObject,
        spLegend,
        color,
        uiNumSamplesToKeep,
        m_dataRange.m_bottom,
        m_dataRange.m_top,
        fSamplingTime,
        bShow,
        m_graphLineDimensions,
        m_spWindowRoot);

    AddLegend(spLegend);

    return m_callbackData.AddFirstEmpty(pkCBData);
}

//------------------------------------------------------------------------------------------------
unsigned int NiVisualTracker::AddDataPoint(
    DataPointCallbackObject* pObject,
    const NiColor& color,
    efd::UInt32 symbol,
    NiRect<float>& dataRange,
    float fSamplingTime,
    bool bShow)
{
    NiVisualTracker::PointCallbackObjectData* pkCBData = NiNew
        NiVisualTracker::PointCallbackObjectData(
            pObject,
            color,
            symbol,
            fSamplingTime,
            bShow,
            dataRange,
            m_graphLineDimensions,
            m_kScreenTextures);

    return m_callbackData.AddFirstEmpty(pkCBData);
}

//------------------------------------------------------------------------------------------------
bool NiVisualTracker::AddLegend(const char* pName, const NiColor& color, efd::UInt32 symbol)
{
    LegendEntryPtr spLegend =
        EE_NEW LegendEntry(pName, color, symbol, m_uiLegendX, m_uiLegendY, m_kScreenTextures);
    AddLegend(spLegend);
    return true;
}

//------------------------------------------------------------------------------------------------
void NiVisualTracker::RemoveGraph(const char* pcName)
{
    for (unsigned int ui = 0; ui < m_callbackData.GetSize(); ui++)
    {
        NiVisualTracker::CallbackObjectData* pData = m_callbackData.GetAt(ui);
        EE_ASSERT(pData);
        LegendEntry* pLegend = pData->GetLegendEntry();
        if (pLegend && strcmp(pLegend->GetName(), pcName) == 0)
        {
            RemoveLegend(pLegend);

            m_callbackData.RemoveAt(ui);
            NiDelete pData;
            m_callbackData.Compact();
            break;
        }
    }
}

//------------------------------------------------------------------------------------------------
void NiVisualTracker::RemoveAll()
{
    for (unsigned int ui = 0; ui < m_callbackData.GetSize(); ui++)
    {
        NiVisualTracker::CallbackObjectData* pkData = m_callbackData.GetAt(ui);
        RemoveLegend(pkData->GetLegendEntry());
        NiDelete pkData;
    }

    m_callbackData.RemoveAll();

    // reset legend
    Ni2DBuffer* pkBuffer = NiRenderer::GetRenderer()->GetDefaultBackBuffer();
    EE_ASSERT(pkBuffer);
    unsigned int uiBBWidth = pkBuffer->GetWidth();

    m_uiLegendX = (unsigned int)((m_kDimensions.m_left) * (float)uiBBWidth);
}

//------------------------------------------------------------------------------------------------
const char* NiVisualTracker::GetName()
{
    return m_acName;
}

//------------------------------------------------------------------------------------------------
void NiVisualTracker::ResetTimeCounters()
{
    for (unsigned int ui = 0; ui < m_callbackData.GetSize(); ui++)
    {
        NiVisualTracker::CallbackObjectData* pkData = m_callbackData.GetAt(ui);
        EE_ASSERT(pkData);
        pkData->ResetLastSampleTime();
    }
}

//------------------------------------------------------------------------------------------------
NiUInt32 NiVisualTracker::Update()
{
    float fTime = NiGetCurrentTimeInSec();
    NiUInt32 numRemoved = 0;
    for (unsigned int ui = 0; ui < m_callbackData.GetSize(); ui++)
    {
        NiVisualTracker::CallbackObjectData* pData = m_callbackData.GetAt(ui);
        EE_ASSERT(pData);
        if (!pData->Update(fTime))
        {
            ++numRemoved;
            EE_DELETE pData;
            m_callbackData.RemoveAt(ui);
        }
    }
    if (numRemoved)
    {
        m_callbackData.Compact();
    }
    m_spWindowRoot->Update(fTime);

    return numRemoved;
}

//------------------------------------------------------------------------------------------------
void NiVisualTracker::Draw()
{
    if (!m_bShow)
        return;

    NiRenderer::GetRenderer()->SetScreenSpaceCameraData();

    m_spBorders->RenderImmediate(NiRenderer::GetRenderer());

    for (unsigned int ui = 0; ui < m_callbackData.GetSize(); ui++)
    {
        NiVisualTracker::CallbackObjectData* pData = m_callbackData.GetAt(ui);
        EE_ASSERT(pData);
        pData->Draw();
    }

    for (unsigned int uj = 0; uj < m_kScreenTextures.GetSize(); uj++)
    {
        NiMeshScreenElements* pTexture = m_kScreenTextures.GetAt(uj);
        EE_ASSERT(pTexture);
        pTexture->RenderImmediate(NiRenderer::GetRenderer());
    }
}

//------------------------------------------------------------------------------------------------
unsigned int NiVisualTracker::GetGraphCount()
{
    return m_callbackData.GetSize();
}

//------------------------------------------------------------------------------------------------
bool NiVisualTracker::SetName(const char* pcName, unsigned int uiWhichGraph)
{
    EE_ASSERT(uiWhichGraph < m_callbackData.GetSize());

    NiVisualTracker::CallbackObjectData* pkData = m_callbackData.GetAt(uiWhichGraph);
    EE_ASSERT(pkData);
    LegendEntry* pLegend = pkData->GetLegendEntry();
    if (!pLegend)
        return false;

    AdjustLegend(pLegend, pcName);
    return true;
}

//------------------------------------------------------------------------------------------------
void NiVisualTracker::AddLegend(LegendEntry* pLegend)
{
    EE_ASSERT(pLegend);
    m_legendData.push_back(pLegend);
}

//------------------------------------------------------------------------------------------------
void NiVisualTracker::RemoveLegend(LegendEntry* pLegend)
{
    if (pLegend)
    {
        int iXAdjust = -(int)pLegend->GetLegendWidth();

        // find this legend entry in the list
        LegendList::iterator iter = m_legendData.find(pLegend);
        if (iter != m_legendData.end())
        {
            AdjustRemainingLegends(iter, iXAdjust);
            m_legendData.erase(iter);
        }
    }
}

//------------------------------------------------------------------------------------------------
void NiVisualTracker::AdjustLegend(LegendEntry* pLegend, const char* pcName)
{
    int iXAdjust = -(int)pLegend->GetLegendWidth();
    pLegend->SetName(pcName);
    iXAdjust += pLegend->GetLegendWidth();

    // find this legend entry in the list
    LegendList::iterator iter = m_legendData.find(pLegend);
    if (iter != m_legendData.end())
    {
        AdjustRemainingLegends(iter, iXAdjust);
    }
}

//------------------------------------------------------------------------------------------------
void NiVisualTracker::AdjustRemainingLegends(LegendList::iterator iter, int iXAdjust)
{
    // iterate forward adjusting all the legends that follow the given iter
    for (++iter; iter != m_legendData.end(); ++iter)
    {
        LegendEntry* pLegend = *iter;
        EE_ASSERT(pLegend);
        pLegend->AdjustLegend(iXAdjust, 0);
    }
}

//------------------------------------------------------------------------------------------------
const char* NiVisualTracker::GetName(unsigned int uiWhichGraph)
{
    EE_ASSERT(uiWhichGraph < m_callbackData.GetSize());

    NiVisualTracker::CallbackObjectData* pData = m_callbackData.GetAt(uiWhichGraph);
    EE_ASSERT(pData);
    LegendEntry* pLegend = pData->GetLegendEntry();
    if (pLegend)
        return pLegend->GetName();
    return "";
}

//------------------------------------------------------------------------------------------------
unsigned int NiVisualTracker::GetGraphIndexByName(const char* pcName)
{
    for (unsigned int ui = 0; ui < m_callbackData.GetSize(); ui++)
    {
        NiVisualTracker::CallbackObjectData* pData = m_callbackData.GetAt(ui);
        EE_ASSERT(pData);
        LegendEntry* pLegend = pData->GetLegendEntry();
        if (pLegend)
        {
            if (0 == strcmp(pcName, pLegend->GetName()))
            {
                return ui;
            }
        }
    }

    return (unsigned int) -1;
}

//------------------------------------------------------------------------------------------------
//  NiVisualTracker::CallbackObjectData implementation
//------------------------------------------------------------------------------------------------
NiVisualTracker::CallbackObjectData::CallbackObjectData(
    LegendEntry* legendKey,
    float fSamplingTime,
    bool bShow,
    const NiRect<float>& dimensions)
: m_spLegendEntry(legendKey)
, m_fSamplingTime(fSamplingTime)
, m_bShow(bShow)
, m_kDimensions(dimensions)
, m_fLastTime(-NI_INFINITY)
{
}

//------------------------------------------------------------------------------------------------
NiVisualTracker::CallbackObjectData::~CallbackObjectData()
{
}

//------------------------------------------------------------------------------------------------
void NiVisualTracker::CallbackObjectData::SetShow(bool bShow)
{
    m_bShow = bShow;
}

//------------------------------------------------------------------------------------------------
bool NiVisualTracker::CallbackObjectData::GetShow()
{
    return m_bShow;
}

//------------------------------------------------------------------------------------------------
//  NiVisualTracker::GraphCallbackObjectData implementation
//------------------------------------------------------------------------------------------------
NiVisualTracker::GraphCallbackObjectData::GraphCallbackObjectData(
    GraphCallbackObject* pObject,
    LegendEntry* pLegend,
    const NiColor& color,
    unsigned int uiNumSamplesToKeep,
    float fMinValue,
    float fMaxValue,
    float fSamplingTime,
    bool bShow,
    const NiRect<float>& dimensions,
    NiNode* pParentNode)
: NiVisualTracker::CallbackObjectData(
    pLegend,
    fSamplingTime,
    bShow,
    dimensions)
, m_pkCallbackObj(pObject)
, m_uiNumSamplesToKeep(uiNumSamplesToKeep)
, m_fMinValue(fMinValue)
, m_fMaxValue(fMaxValue)
, m_uiNextVertex(m_uiNumSamplesToKeep - 1)
, m_uiBufferID(0)
{
    // Create the buffered line meshes that will be rendered and updated in
    // a round robin pattern.
    for (unsigned int ui = 0; ui < BUFFER_COUNT; ui++)
    {
        m_aspLines[ui] = CreateLines(m_uiNumSamplesToKeep, color, dimensions);
        pParentNode->AttachChild(m_aspLines[ui]);
        m_afPrevFramesValues[ui] = 0;
    }
}

//------------------------------------------------------------------------------------------------
NiVisualTracker::GraphCallbackObjectData::~GraphCallbackObjectData()
{
    NiDelete m_pkCallbackObj;

    // Destroy the duplicated line meshes.
    for (unsigned int ui = 0; ui < BUFFER_COUNT; ui++)
    {
        m_aspLines[ui] = 0;
    }
}

//------------------------------------------------------------------------------------------------
void NiVisualTracker::GraphCallbackObjectData::Draw()
{
    if (m_bShow)
    {
        m_aspLines[m_uiBufferID]->RenderImmediate(NiRenderer::GetRenderer());
    }
}

//------------------------------------------------------------------------------------------------
bool NiVisualTracker::GraphCallbackObjectData::Update(float fTime)
{
    if (m_fLastTime != -NI_INFINITY && fTime - m_fLastTime < m_fSamplingTime)
    {
        return true;
    }

    m_uiBufferID++;
    if (m_uiBufferID >= BUFFER_COUNT)
        m_uiBufferID = 0;

    unsigned int uiUpdateBufferID = m_uiBufferID+1;
    if (uiUpdateBufferID >= BUFFER_COUNT)
        uiUpdateBufferID = 0;

    float fDataValue;
    if (!m_pkCallbackObj->TakeSample(fTime, fDataValue))
    {
        return false;
    }

    if (fDataValue > m_fMaxValue)
        fDataValue = m_fMaxValue;

    if (fDataValue < m_fMinValue)
        fDataValue = m_fMinValue;

    // Push the sampled data at the end of the prev frames array.
    for (NiUInt32 ui = 0; ui < BUFFER_COUNT - 1; ui++)
    {
        m_afPrevFramesValues[ui] = m_afPrevFramesValues[ui+1];
    }
    m_afPrevFramesValues[BUFFER_COUNT - 1] = fDataValue;

    float fWidth = NiAbs(m_kDimensions.m_left - m_kDimensions.m_right);
    float fHeight = NiAbs(m_kDimensions.m_top - m_kDimensions.m_bottom);

    // Scope the lock appropriately
    {
        // Build vertex data
        NiDataStreamElementLock kPosLock(
            m_aspLines[uiUpdateBufferID],
            NiCommonSemantics::POSITION(),
            0,
            NiDataStreamElement::F_FLOAT32_3,
            NiDataStream::LOCK_WRITE);
        Point3Iter kPosIt = kPosLock.begin<NiPoint3>();

        for (NiUInt32 ui = 0; ui < m_uiNumSamplesToKeep - BUFFER_COUNT; ++ui)
        {
            kPosIt[ui] = kPosIt[ui+BUFFER_COUNT];
            kPosIt[ui].x = (((float)ui) / (float)m_uiNumSamplesToKeep) * fWidth;
        }

        for (NiUInt32 ui = 0; ui < BUFFER_COUNT; ui++)
        {
            NiPoint3& pt = kPosIt[m_uiNumSamplesToKeep - BUFFER_COUNT + ui];
            pt.x = (float)1.0f * fWidth;

            float fValueScaled = ((float)m_afPrevFramesValues[ui] - m_fMinValue) /
                (m_fMaxValue - m_fMinValue);

            pt.y = fHeight - fValueScaled * fHeight;
            pt.z = 0.0f;
        }

        // Update region to reflect new sample count
        NiDataStream* pkDataStream = kPosLock.GetDataStream();
        NiDataStream::Region& kRegion = pkDataStream->GetRegion(0);
        kRegion.SetRange(m_uiNumSamplesToKeep);
    }

    NiMesh* pkMesh = (NiMesh*)(m_aspLines[uiUpdateBufferID]);
    pkMesh->RecomputeBounds();

    m_fLastTime = fTime;
    return true;
}

//------------------------------------------------------------------------------------------------
NiMeshPtr NiVisualTracker::GraphCallbackObjectData::CreateLines(
    unsigned int uiNumSamplesToKeep, const NiColor& kColor,
    const NiRect<float>& kDims)
{
    // float fWidth = NiAbs(kDimensions.m_left - kDimensions.m_right);
    float fHeight = NiAbs(kDims.m_top - kDims.m_bottom);

    // Build lines mesh and mesh data
    NiMeshPtr spLinesMesh = NiNew NiMesh;
    spLinesMesh->SetPrimitiveType(NiPrimitiveType::PRIMITIVE_LINESTRIPS);
    spLinesMesh->AddStream(
            NiCommonSemantics::POSITION(),
            0,
            NiDataStreamElement::F_FLOAT32_3,
            uiNumSamplesToKeep,
            NiDataStream::ACCESS_CPU_READ | NiDataStream::ACCESS_GPU_READ |
                NiDataStream::ACCESS_CPU_WRITE_MUTABLE,
            NiDataStream::USAGE_VERTEX);

    // Initialize the position data
    NiDataStreamElementLock kPosLock(
        spLinesMesh,
        NiCommonSemantics::POSITION(),
        0,
        NiDataStreamElement::F_FLOAT32_3,
        NiDataStream::LOCK_WRITE);

    NiPoint3 kDefaultValue = NiPoint3(0.0f, fHeight, 0.0f);
    Point3Iter kOutIt = kPosLock.begin<NiPoint3>();
    Point3Iter kEndIt = kPosLock.end<NiPoint3>();
    for (; kOutIt != kEndIt; kOutIt++)
    {
        *kOutIt = kDefaultValue;
    }

    // Set transform
    spLinesMesh->SetTranslate(NiPoint3(kDims.m_left, kDims.m_top, 0.0f));
    NiMatrix3 kRot;
    kRot.MakeIdentity();
    spLinesMesh->SetRotate(kRot);
    spLinesMesh->SetScale(1.0f);

    // Set color property
    NiMaterialProperty* pkMatProp = NiNew NiMaterialProperty();
    pkMatProp->SetEmittance(kColor);
    spLinesMesh->AttachProperty(pkMatProp);

    // Set depth buffer property
    NiZBufferProperty* pkZProp = NiNew NiZBufferProperty();
    pkZProp->SetZBufferTest(false);
    pkZProp->SetZBufferWrite(false);
    spLinesMesh->AttachProperty(pkZProp);

    spLinesMesh->Update(0.0f);
    spLinesMesh->UpdateProperties();
    spLinesMesh->UpdateEffects();
    spLinesMesh->UpdateNodeBound();

    return spLinesMesh;
}

//------------------------------------------------------------------------------------------------
//  NiVisualTracker::SymbolScreenElement implementation
//------------------------------------------------------------------------------------------------
NiPixelDataPtr NiVisualTracker::SymbolScreenElement::ms_spSymbolPixelData;
NiTexturePtr NiVisualTracker::SymbolScreenElement::ms_spSymbolTexture;
efd::UInt32 NiVisualTracker::SymbolScreenElement::ms_textureRefCount;
static const efd::UInt32 k_numSymbolCols = 4;
static const efd::UInt32 k_numSymbolRows = 16;
static const efd::UInt32 k_symbolSize = 8;

//------------------------------------------------------------------------------------------------
NiVisualTracker::SymbolScreenElement::SymbolScreenElement(efd::UInt32 symbol, const NiColor& color)
    : NiMeshScreenElements(false, true, 1)
    , m_symbol(symbol)
{
    if (!ms_textureRefCount)
    {
        ms_spSymbolTexture = NiSourceTexture::Create(CreateSymbolPixelData());
    }
    ++ms_textureRefCount;

    NiTexturingProperty* pkTexProp = NiNew NiTexturingProperty;
    pkTexProp->SetBaseTexture(ms_spSymbolTexture);
    pkTexProp->SetBaseFilterMode(NiTexturingProperty::FILTER_NEAREST);
    pkTexProp->SetApplyMode(NiTexturingProperty::APPLY_MODULATE);
    pkTexProp->SetBaseClampMode(NiTexturingProperty::CLAMP_S_CLAMP_T);

    NiAlphaProperty* pkAlphaProp = NiNew NiAlphaProperty;
    pkAlphaProp->SetAlphaBlending(true);

    NiZBufferProperty* pkZBufProp = NiNew NiZBufferProperty;
    pkZBufProp->SetZBufferTest(false);
    pkZBufProp->SetZBufferWrite(true);

    NiMaterialProperty* pkMatProp = NiNew NiMaterialProperty;
    pkMatProp->SetEmittance(NiColor(color.r, color.g, color.b));

    AttachProperty(pkTexProp);
    AttachProperty(pkAlphaProp);
    AttachProperty(pkZBufProp);
    AttachProperty(pkMatProp);
    UpdateProperties();

    // We need four points in a triangle fan to render our symbol:
    Insert(4);

    // Compute our texture location based on the selected symbol
    SetSymbol(m_symbol);

    // Give use a known initial condition:
    SetRectangle(0, 0.0f, 0.0f, 0.0f, 0.0f);
    UpdateBound();
}

//------------------------------------------------------------------------------------------------
NiVisualTracker::SymbolScreenElement::~SymbolScreenElement()
{
    if (--ms_textureRefCount == 0)
    {
        ms_spSymbolTexture = 0;
        ms_spSymbolPixelData = 0;
    }
}

//------------------------------------------------------------------------------------------------
void NiVisualTracker::SymbolScreenElement::SetSymbol(efd::UInt32 symbol)
{
    m_symbol = symbol;
    if (m_symbol > k_numSymbolCols * k_numSymbolRows)
    {
        m_symbol = 0;
    }

    // compute pixel coordinates:
    efd::UInt32 top = (m_symbol / k_numSymbolCols) * k_symbolSize;
    efd::UInt32 left = (m_symbol % k_numSymbolCols) * k_symbolSize;

    // convert to float:
    float fTexLeft = (float)left / ms_spSymbolTexture->GetWidth();
    float fTexRight = (float)(left + k_symbolSize) / ms_spSymbolTexture->GetWidth();

    float fTexTop = (float)top / ms_spSymbolTexture->GetHeight();
    float fTexBottom = (float)(top + k_symbolSize) / ms_spSymbolTexture->GetHeight();

    SetTextures(0, 0, fTexLeft, fTexTop, fTexRight, fTexBottom);
}

//------------------------------------------------------------------------------------------------
void NiVisualTracker::SymbolScreenElement::SetLocationInWindow(float x, float y)
{
    // convert size into device coordinates:
    NiRenderer* pRenderer = NiRenderer::GetRenderer();
    float fWidth, fHeight;
    pRenderer->ConvertFromPixelsToNDC(k_symbolSize, k_symbolSize, fWidth, fHeight);

    SetRectangle(0, x, y, fWidth, fHeight);
    UpdateBound();
}

//------------------------------------------------------------------------------------------------
void NiVisualTracker::SymbolScreenElement::SetLocationOnScreen(efd::UInt32 x, efd::UInt32 y)
{
    // convert size into device coordinates:
    NiRenderer* pRenderer = NiRenderer::GetRenderer();
    float fTop, fLeft, fWidth, fHeight;
    pRenderer->ConvertFromPixelsToNDC(x, y, fTop, fLeft);
    pRenderer->ConvertFromPixelsToNDC(k_symbolSize, k_symbolSize, fWidth, fHeight);

    SetRectangle(0, fTop, fLeft, fWidth, fHeight);
    UpdateBound();
}

//------------------------------------------------------------------------------------------------
NiPixelData* NiVisualTracker::SymbolScreenElement::CreateSymbolPixelData()
{
    if (ms_spSymbolPixelData == NULL)
    {
        ms_spSymbolPixelData = NiNew NiPixelData(
            k_numSymbolCols * k_symbolSize,
            k_numSymbolRows * k_symbolSize,
            NiPixelFormat::RGBA32);
        EE_ASSERT(ms_spSymbolPixelData);
        unsigned char* pucPixels = ms_spSymbolPixelData->GetPixels();
        for (unsigned int ui = 0; ui < ms_spSymbolPixelData->GetSizeInBytes(); ui++)
        {
            // g_SymbolPixels isn't really pixel data, it is one char used as a boolean per pixel.
            pucPixels[ui] = g_SymbolPixels[ui/4] ? 0xFF : 0x00;
        }
    }
    return ms_spSymbolPixelData;
}

//------------------------------------------------------------------------------------------------
// NiVisualTracker::LegendEntry implementation
//------------------------------------------------------------------------------------------------
NiVisualTracker::LegendEntry::LegendEntry(
    const char* pName,
    const NiColor& color,
    efd::UInt32 symbol,
    unsigned int& io_uiLegendX, unsigned int& io_uiLegendY,
    NiMeshScreenElementsArray& textures)
: m_pkScreenElements(NULL)
{
    NiSprintf(m_acName, 255, "%s%s", symbol == k_NO_SYMBOL ? "" : " ", pName);
    m_acName[255] = '\0';

    NiColorA textColor(color.r, color.g, color.b, 1.0f);
    m_pText = NiNew ScreenText(15, &textures, textColor);
    m_pText->SetTextOrigin(io_uiLegendX, io_uiLegendY);
    m_pText->SetVisible(true);
    m_pText->SetString(m_acName);

    m_pkScreenElements = &textures;
    if (symbol != k_NO_SYMBOL)
    {
        m_spSymbol = EE_NEW SymbolScreenElement(symbol, color);
        efd::UInt32 offset = (m_pText->GetHeight() - k_symbolSize) / 2;
        m_spSymbol->SetLocationOnScreen(io_uiLegendX, io_uiLegendY + offset);
        m_pkScreenElements->AddFirstEmpty(m_spSymbol.data());
    }

    io_uiLegendX += m_pText->GetWidth();
}

//------------------------------------------------------------------------------------------------
NiVisualTracker::LegendEntry::~LegendEntry()
{
    NiDelete m_pText;
    if (m_spSymbol)
    {
        m_pkScreenElements->Remove(m_spSymbol.data());
        m_pkScreenElements->Compact();
    }
}

//------------------------------------------------------------------------------------------------
void NiVisualTracker::LegendEntry::SetName(const char* pName)
{
    NiSprintf(m_acName, 255, "%s%s", m_spSymbol ? "" : " ", pName);
    m_acName[255] = '\0';
    m_pText->SetString(m_acName);
}

//------------------------------------------------------------------------------------------------
const char* NiVisualTracker::LegendEntry::GetName()
{
    return m_spSymbol ? m_acName+1 : m_acName;
}

//------------------------------------------------------------------------------------------------
void NiVisualTracker::LegendEntry::AdjustLegend(int iX, int iY)
{
    unsigned int uiXCurrent, uiYCurrent;
    m_pText->GetTextOrigin(uiXCurrent, uiYCurrent);
    uiXCurrent += iX;
    uiYCurrent += iY;
    m_pText->SetTextOrigin(uiXCurrent, uiYCurrent);
    if (m_spSymbol)
    {
        efd::UInt32 offset = (m_pText->GetHeight() - k_symbolSize) / 2;
        m_spSymbol->SetLocationOnScreen(uiXCurrent, uiYCurrent + offset);
    }
}

//------------------------------------------------------------------------------------------------
unsigned int NiVisualTracker::LegendEntry::GetLegendWidth()
{
    return m_pText->GetWidth();
}

//------------------------------------------------------------------------------------------------
//  NiVisualTracker::PointCallbackObjectData implementation
//------------------------------------------------------------------------------------------------
NiVisualTracker::PointCallbackObjectData::PointCallbackObjectData(
    DataPointCallbackObject* pCallbackObject,
    const NiColor& color,
    efd::UInt32 whichSymbol,
    float fSamplingTime,
    bool bShow,
    const NiRect<float>& dataDimensions,
    const NiRect<float>& windowDimensions,
    NiMeshScreenElementsArray& textures)
: NiVisualTracker::CallbackObjectData(
    NULL,
    fSamplingTime,
    bShow,
    windowDimensions)
, m_pCallbackObj(pCallbackObject)
, m_dataRange(dataDimensions)
, m_lastPoint(-NI_INFINITY, -NI_INFINITY)
{
    m_spSymbol = EE_NEW SymbolScreenElement(whichSymbol, color);

    m_pkScreenElements = &textures;
    m_pkScreenElements->AddFirstEmpty(m_spSymbol.data());
}

//------------------------------------------------------------------------------------------------
NiVisualTracker::PointCallbackObjectData::~PointCallbackObjectData()
{
    NiDelete m_pCallbackObj;
    m_pkScreenElements->Remove(m_spSymbol.data());
    m_pkScreenElements->Compact();
}

//------------------------------------------------------------------------------------------------
void ClampToRect(NiRect<float>& rect, efd::Point2& io_pt)
{
    float min = efd::Min(rect.m_left, rect.m_right);
    float max = efd::Max(rect.m_left, rect.m_right);
    io_pt.x = efd::Clamp(io_pt.x, min, max);

    min = efd::Min(rect.m_top, rect.m_bottom);
    max = efd::Max(rect.m_top, rect.m_bottom);
    io_pt.y = efd::Clamp(io_pt.y, min, max);
}

//------------------------------------------------------------------------------------------------
bool NiVisualTracker::PointCallbackObjectData::Update(float fTime)
{
    if (m_fLastTime != -NI_INFINITY && fTime - m_fLastTime < m_fSamplingTime)
    {
        return true;
    }

    // Do something
    efd::Point2 pt;
    efd::UInt32 symbol = m_spSymbol->GetSymbol();
    if (!m_pCallbackObj->TakeSample(fTime, pt, symbol))
    {
        return false;
    }
    if (symbol != m_spSymbol->GetSymbol())
    {
        m_spSymbol->SetSymbol(symbol);
    }
    else if (pt == m_lastPoint)
    {
        return true;
    }
    m_lastPoint = pt;


    // Clamp point to the data range.  Keep in mind the data range can be flipped (in other words,
    // left > right and/or top < bottom) if you want to display the data flipped.
    ClampToRect(m_dataRange, pt);

    // convert into destination window coordinates.  For the destination rect the top,left corner
    // is always less than the bottom right corner (0,0 is the furthest top-left corner and 1,1
    // is the bottom-right corner).  For the data rect 0,0 is the bottom-left so we have to flip
    // the vertical axis when converting.
    efd::Point2 ptInWindow;
    ptInWindow.x =
        (pt.x - m_dataRange.m_left) / m_dataRange.GetWidth() * m_kDimensions.GetWidth();
    if (ptInWindow.x < 0)
        ptInWindow.x = m_kDimensions.m_right + ptInWindow.x;
    else
        ptInWindow.x = m_kDimensions.m_left + ptInWindow.x;

    ptInWindow.y =
        (pt.y - m_dataRange.m_top) / m_dataRange.GetHeight() * m_kDimensions.GetHeight();
    if (ptInWindow.y < 0)
        ptInWindow.y = m_kDimensions.m_top - ptInWindow.y;
    else
        ptInWindow.y = m_kDimensions.m_bottom - ptInWindow.y;

    // and update the element position
    m_spSymbol->SetLocationInWindow(ptInWindow.x, ptInWindow.y);

    m_fLastTime = fTime;
    return true;
}

//------------------------------------------------------------------------------------------------
void NiVisualTracker::PointCallbackObjectData::Draw()
{
}

//------------------------------------------------------------------------------------------------
//  NiVisualTracker::ScreenText implementation
//------------------------------------------------------------------------------------------------
NiPixelDataPtr NiVisualTrackerBase::ScreenText::ms_spFontPixelData = 0;
NiTexturePtr NiVisualTrackerBase::ScreenText::ms_spTextTexture = 0;
unsigned int NiVisualTrackerBase::ScreenText::ms_uiCharWidth = 11;
unsigned int NiVisualTrackerBase::ScreenText::ms_uiCharHeight = 21;
unsigned int NiVisualTrackerBase::ScreenText::ms_uiCharSpacingX = 11;
unsigned int NiVisualTrackerBase::ScreenText::ms_uiCharSpacingY = 21;
const unsigned int NiVisualTrackerBase::ScreenText::ms_uiCharBaseU = 0;
const unsigned int NiVisualTrackerBase::ScreenText::ms_uiCharBaseV = 0;
const unsigned int NiVisualTrackerBase::ScreenText::ms_uiASCIIMin = 33;
const unsigned int NiVisualTrackerBase::ScreenText::ms_uiASCIIMax = 122;
unsigned int NiVisualTrackerBase::ScreenText::ms_uiASCIICols = 23;
int NiVisualTrackerBase::ScreenText::ms_iCount = 0;

//------------------------------------------------------------------------------------------------
void NiVisualTrackerBase::ScreenText::SetString(const char* pcString)
{
    size_t stNewLength = strlen(pcString);

    if (stNewLength >= m_uiMaxChars)
        stNewLength = m_uiMaxChars - 1;
    m_uiNumChars = (unsigned int)stNewLength + 1;

    NiStrncpy(m_pcString, m_uiMaxChars, pcString, stNewLength);
    RecreateText();
}

//------------------------------------------------------------------------------------------------
const char* NiVisualTrackerBase::ScreenText::GetString() const
{
    return m_pcString;
}

//------------------------------------------------------------------------------------------------
void NiVisualTrackerBase::ScreenText::SetTextOrigin(unsigned int uiX,
    unsigned int uiY)
{
    if (m_uiTextOriginX != uiX || m_uiTextOriginY != uiY)
    {
        m_uiTextOriginX = uiX;
        m_uiTextOriginY = uiY;
        RecreateText();
    }
}

//------------------------------------------------------------------------------------------------
void NiVisualTrackerBase::ScreenText::GetTextOrigin(unsigned int& uiX,
    unsigned int& uiY) const
{
    uiX = m_uiTextOriginX;
    uiY = m_uiTextOriginY;
}

//------------------------------------------------------------------------------------------------
void NiVisualTrackerBase::ScreenText::SetScrollDown(bool bDown)
{
    if (m_bScrollDown != bDown)
    {
        m_bScrollDown = bDown;
        RecreateText();
    }
}

//------------------------------------------------------------------------------------------------
bool NiVisualTrackerBase::ScreenText::GetScrollDown() const
{
    return m_bScrollDown;
}

//------------------------------------------------------------------------------------------------
const NiColorA& NiVisualTrackerBase::ScreenText::GetColor() const
{
    return m_kColor;
}

//------------------------------------------------------------------------------------------------
short NiVisualTrackerBase::ScreenText::GetHeight() const
{
    return (short)ms_uiCharSpacingY;
}

//------------------------------------------------------------------------------------------------
short NiVisualTrackerBase::ScreenText::GetWidth() const
{
    return (short)(ms_uiCharSpacingX * m_uiNumChars);
}

//------------------------------------------------------------------------------------------------
short NiVisualTrackerBase::ScreenText::GetCharacterSpacing() const
{
    return (short)ms_uiCharSpacingX;
}

//------------------------------------------------------------------------------------------------
void NiVisualTrackerBase::ScreenText::SetMaxLineLength(unsigned int uiColumns)
{
    if (m_uiMaxCols != uiColumns)
    {
        m_uiMaxCols = uiColumns;
        RecreateText();
    }
}

//------------------------------------------------------------------------------------------------
unsigned int NiVisualTrackerBase::ScreenText::GetMaxLineLength() const
{
    return m_uiMaxCols;
}

//------------------------------------------------------------------------------------------------
NiTexture* NiVisualTrackerBase::ScreenText::GetTexture()
{
    return ms_spTextTexture;
}

//------------------------------------------------------------------------------------------------
NiVisualTrackerBase::ScreenText::ScreenText(
    unsigned int uiMaxChars,
    NiMeshScreenElementsArray* pkScreenTextures,
    const NiColorA& kColor)
{
    Init(uiMaxChars, pkScreenTextures, kColor);
}

//------------------------------------------------------------------------------------------------
NiVisualTrackerBase::ScreenText::~ScreenText()
{
    m_pkScreenElements->Remove(m_spScreenElement);
    m_pkScreenElements->Compact();

    if (--ms_iCount < 1)
    {
        ms_spTextTexture = 0;
        ms_spFontPixelData = 0;
    }

    NiFree(m_pcString);
}

//------------------------------------------------------------------------------------------------
void NiVisualTrackerBase::ScreenText::Init(
    unsigned int uiMaxChars,
    NiMeshScreenElementsArray* pkScreenTextures,
    const NiColorA& kColor)
{
    ms_iCount++;

    if (!ms_spTextTexture)
    {
        ms_spTextTexture = NiSourceTexture::Create(CreateCourier16PixelData());
    }

    m_uiMaxChars = uiMaxChars;
    m_kColor = kColor;

    m_spScreenElement = NiNew NiMeshScreenElements(false, true, 1);

    NiTexturingProperty* pkTexProp = NiNew NiTexturingProperty;
    pkTexProp->SetBaseTexture(ms_spTextTexture);
    pkTexProp->SetBaseFilterMode(NiTexturingProperty::FILTER_NEAREST);
    pkTexProp->SetApplyMode(NiTexturingProperty::APPLY_MODULATE);
    pkTexProp->SetBaseClampMode(NiTexturingProperty::CLAMP_S_CLAMP_T);

    NiAlphaProperty* pkAlphaProp = NiNew NiAlphaProperty;
    pkAlphaProp->SetAlphaBlending(true);

    NiZBufferProperty* pkZBufProp = NiNew NiZBufferProperty;
    pkZBufProp->SetZBufferTest(false);
    pkZBufProp->SetZBufferWrite(true);

    NiMaterialProperty* pkMatProp = NiNew NiMaterialProperty;
    pkMatProp->SetEmittance(NiColor(m_kColor.r, m_kColor.g, m_kColor.b));

    m_spScreenElement->AttachProperty(pkTexProp);
    m_spScreenElement->AttachProperty(pkAlphaProp);
    m_spScreenElement->AttachProperty(pkZBufProp);
    m_spScreenElement->AttachProperty(pkMatProp);
    m_spScreenElement->UpdateProperties();

    m_pkScreenElements = pkScreenTextures;

    m_uiTextOriginX = 0;
    m_uiTextOriginY = 0;
    m_bScrollDown = true;

    // String is _not_ NULL terminated
    m_pcString = NiAlloc(char, m_uiMaxChars);
    m_uiNumChars = 0;

    m_uiMaxCols = 40;

    m_uiNumRects = 0;
    m_uiNumCurrentRows = 0;
    m_uiCurrentColumn = 0;
}

//------------------------------------------------------------------------------------------------
NiPixelData* NiVisualTrackerBase::ScreenText::CreateCourier16PixelData()
{
   if (ms_spFontPixelData != NULL)
       return ms_spFontPixelData;

   ms_spFontPixelData = NiNew NiPixelData(256, 128, NiPixelFormat::RGBA32);
   EE_ASSERT(ms_spFontPixelData);
   unsigned char* pucPixels = ms_spFontPixelData->GetPixels();
   for (unsigned int ui = 0; ui < ms_spFontPixelData->GetSizeInBytes(); ui++)
   {
        pucPixels[ui] = g_aucPixels[ui];
   }

   return ms_spFontPixelData;
}

//------------------------------------------------------------------------------------------------
void NiVisualTrackerBase::ScreenText::RecreateText()
{
    m_spScreenElement->RemoveAll();

    m_uiNumCurrentRows = 0;
    m_uiCurrentColumn = 0;

    NiRendererPtr spRenderer = NiRenderer::GetRenderer();

    for (unsigned int i = 0; i < m_uiNumChars; i++)
    {
        char cChar = m_pcString[i];

        // if we are at the end of a line or if the char is '\n' then move
        // to the start of the next line
        if ((m_uiCurrentColumn >= m_uiMaxCols) || (cChar == '\n'))
        {
            m_uiNumCurrentRows++;
            m_uiCurrentColumn = 0;

            if (!m_bScrollDown)
            {
                // Move all characters up one row
                unsigned int uiNumPolys = m_spScreenElement->GetNumPolygons();
                for (unsigned int j = 0; j < uiNumPolys; j++)
                {
                    float fLeft, fTop, fWidth, fHeight;
                    m_spScreenElement->GetRectangle(j, fLeft, fTop, fWidth, fHeight);
                    fTop -= ms_uiCharSpacingY;
                    m_spScreenElement->SetRectangle(j, fLeft, fTop, fWidth, fHeight);
                }
            }

            // if the extra char is a '\n', skip it
            if (cChar == '\n')
                continue;
        }

        unsigned int uiChar = (unsigned int)cChar;

        float fCharWidth, fCharHeight;
        spRenderer->ConvertFromPixelsToNDC(
            ms_uiCharSpacingX, ms_uiCharSpacingY,
            fCharWidth, fCharHeight);

        // skip whitespace or unprintable character
        if ((uiChar >= ms_uiASCIIMin) && (uiChar <= ms_uiASCIIMax))
        {
            uiChar -= ms_uiASCIIMin;

            unsigned int usPixTop = m_uiTextOriginY;
            if (m_bScrollDown)
            {
                usPixTop += (m_uiNumCurrentRows) * ms_uiCharSpacingY;
            }
            unsigned short usPixLeft =
                (unsigned short)(m_uiTextOriginX + m_uiCurrentColumn * ms_uiCharSpacingX);

            unsigned short usTexTop =
                (unsigned short)(ms_uiCharBaseV + (uiChar / ms_uiASCIICols) * ms_uiCharSpacingY);
            unsigned short usTexLeft =
                (unsigned short)(ms_uiCharBaseU + (uiChar % ms_uiASCIICols) * ms_uiCharSpacingX);

            float fLeft, fTop;
            spRenderer->ConvertFromPixelsToNDC(usPixLeft, usPixTop, fLeft, fTop);

            float fTexLeft = (float)usTexLeft / ms_spTextTexture->GetWidth();
            float fTexTop = (float)usTexTop / ms_spTextTexture->GetHeight();
            float fTexRight = fTexLeft + ((float)ms_uiCharWidth / ms_spTextTexture->GetWidth());
            float fTexBottom = fTexTop + ((float)ms_uiCharHeight / ms_spTextTexture->GetHeight());

            NiInt32 iPolyIndex = m_spScreenElement->Insert(4);
            m_spScreenElement->SetRectangle(iPolyIndex, fLeft, fTop, fCharWidth, fCharHeight);

            m_spScreenElement->SetTextures(iPolyIndex, 0, fTexLeft, fTexTop, fTexRight, fTexBottom);

            m_spScreenElement->UpdateBound();
            m_spScreenElement->SetColors(iPolyIndex, m_kColor);
        }

        m_uiCurrentColumn++;
    }
}

//------------------------------------------------------------------------------------------------
void NiVisualTrackerBase::ScreenText::AppendCharacter(char cChar)
{
    if (m_uiNumChars >= (m_uiMaxChars - 1))
        return;

    m_pcString[m_uiNumChars] = cChar;

    m_uiNumChars++;
    m_pcString[m_uiNumChars] = '\0';

    // if we are at the end of a line or if the char is '\n' then move
    // to the start of the next line
    if ((m_uiCurrentColumn >= m_uiMaxCols) || (cChar == '\n'))
    {
        m_uiNumCurrentRows++;
        m_uiCurrentColumn = 0;

        if (!m_bScrollDown)
        {
            // Move all characters up one row
            unsigned int uiNumPolys = m_spScreenElement->GetNumPolygons();
            for (unsigned int j = 0; j < uiNumPolys; j++)
            {
                float fLeft, fTop, fWidth, fHeight;
                m_spScreenElement->GetRectangle(j, fLeft, fTop, fWidth, fHeight);
                fTop -= ms_uiCharSpacingY;
                m_spScreenElement->SetRectangle(j, fLeft, fTop, fWidth, fHeight);
            }
        }

        // if the extra char is a '\n', skip it
        if (cChar == '\n')
            return;
    }

    unsigned int uiChar = (unsigned int)cChar;

    // skip whitespace or unprintable character
    if ((uiChar >= ms_uiASCIIMin) && (uiChar <= ms_uiASCIIMax))
    {
        uiChar -= ms_uiASCIIMin;

        unsigned int usPixTop = m_uiTextOriginY;
        if (m_bScrollDown)
        {
            usPixTop += (m_uiNumCurrentRows + 1) * ms_uiCharSpacingY;
        }
        unsigned short usPixLeft =
            (unsigned short)(m_uiTextOriginX + m_uiCurrentColumn * ms_uiCharSpacingX);

        unsigned short usTexTop =
            (unsigned short)(ms_uiCharBaseV + (uiChar / ms_uiASCIICols) * ms_uiCharSpacingY);
        unsigned short usTexLeft =
            (unsigned short)(ms_uiCharBaseU + (uiChar % ms_uiASCIICols) * ms_uiCharSpacingX);

        NiRendererPtr spRenderer = NiRenderer::GetRenderer();
        float fCharWidth, fCharHeight;
        spRenderer->ConvertFromPixelsToNDC(
            ms_uiCharSpacingX, ms_uiCharSpacingY,
            fCharWidth, fCharHeight);

        float fLeft, fTop;
        spRenderer->ConvertFromPixelsToNDC(usPixLeft, usPixTop, fLeft, fTop);

        float fTexLeft = (float) usTexLeft / ms_spTextTexture->GetWidth();
        float fTexTop = (float) usTexTop / ms_spTextTexture->GetHeight();
        float fTexRight =
            fTexLeft + ((float) ms_uiCharWidth / ms_spTextTexture->GetWidth());
        float fTexBottom =
            fTexTop + ((float) ms_uiCharHeight / ms_spTextTexture->GetHeight());

        NiInt32 iPolyIndex = m_spScreenElement->Insert(4);
        m_spScreenElement->SetRectangle(iPolyIndex, fLeft, fTop, fCharWidth, fCharHeight);

        m_spScreenElement->SetTextures(iPolyIndex, 0, fTexLeft, fTexTop, fTexRight, fTexBottom);

        m_spScreenElement->UpdateBound();
        m_spScreenElement->SetColors(iPolyIndex, m_kColor);
    }

    m_uiCurrentColumn++;
}

//------------------------------------------------------------------------------------------------
void NiVisualTrackerBase::ScreenText::DeleteLastCharacter()
{
    if (!m_uiNumChars)
        return;

    m_uiNumChars--;
    char cChar = m_pcString[m_uiNumChars];
    m_pcString[m_uiNumChars] = '\0';

    // if we are at the beginning of a line then return to end of the
    // previous line - easiest way to do this is to recreate the text
    if (m_uiCurrentColumn == 0)
    {
        RecreateText();
    }
    else
    {
        unsigned int uiChar = (unsigned int)cChar;

        // skip whitespace or unprintable character
        if ((uiChar >= ms_uiASCIIMin) && (uiChar <= ms_uiASCIIMax))
        {
            // Remove character
            m_spScreenElement->Remove(m_spScreenElement->GetNumPolygons() - 1);
        }
        m_uiCurrentColumn--;
    }
}

//------------------------------------------------------------------------------------------------
void NiVisualTrackerBase::ScreenText::SetColor(NiColorA& kNewColor)
{
    if (kNewColor == m_kColor)
        return;

    m_kColor = kNewColor;

    NiMaterialProperty* pkMatProp = NiDynamicCast(NiMaterialProperty,
        m_spScreenElement->GetProperty(NiProperty::MATERIAL));
    pkMatProp->SetEmittance(NiColor(m_kColor.r, m_kColor.g, m_kColor.b));
    m_spScreenElement->UpdateProperties();
}

//------------------------------------------------------------------------------------------------
void NiVisualTrackerBase::ScreenText::SetVisible(bool bVisible)
{
    if (bVisible)
        m_pkScreenElements->AddFirstEmpty(m_spScreenElement);
    else
        m_pkScreenElements->Remove(m_spScreenElement);
}

//------------------------------------------------------------------------------------------------
// Larger pixel data buffers are included from separate files because they are so large they
// make it hard to edit this file otherwise.
#include "NiCourierPixelData.inc"
#include "NiSymbolPixelData.inc"

