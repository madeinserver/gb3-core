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
#include "egmToolServicesPCH.h"

#include "CustomScreenFillingRenderView.h"

using namespace egmToolServices;

NiImplementRTTI(CustomScreenFillingRenderView, NiScreenFillingRenderView);

//--------------------------------------------------------------------------------------------------
CustomScreenFillingRenderView::CustomScreenFillingRenderView() :
    NiScreenFillingRenderView(), m_bPropertiesChanged(false)
{
    // Create the screen-filling quad data object if it doesn't already exist.
    m_spScreenFillingQuad = NiNew NiMeshScreenElements(false, true,
        1, 1, 1, 4, 4, 2, 2);

    int iPolygon = m_spScreenFillingQuad->Insert(4);
    m_spScreenFillingQuad->SetRectangle(iPolygon, 0.0f, 0.0f, 1.0f,
        1.0f);

    NiPoint2 akTexCoords[4] = {NiPoint2(0.0f, 0.0f), NiPoint2(0.0f, 1.0f),
        NiPoint2(1.0f, 1.0f), NiPoint2(1.0f, 0.0f)};
    m_spScreenFillingQuad->SetTextures(iPolygon, 0, akTexCoords);
    m_spScreenFillingQuad->Update(0.0f);

    // Create an effect state to use for dynamic effects.
    m_spEffectState = NiNew NiDynamicEffectState;
}
//--------------------------------------------------------------------------------------------------
CustomScreenFillingRenderView::~CustomScreenFillingRenderView()
{
    m_spScreenFillingQuad = NULL;
}
//--------------------------------------------------------------------------------------------------
void CustomScreenFillingRenderView::CalculatePVGeometry()
{
    EE_ASSERT(m_kCachedPVGeometry.GetCount() == 0);
    EE_ASSERT(m_spScreenFillingQuad);

    // Update properties, if necessary.
    if (m_bPropertiesChanged)
    {
        m_spScreenFillingQuad->UpdateProperties();
        m_bPropertiesChanged = false;
    }

    // Update effects, if necessary.
    if (m_bEffectsChanged)
    {
        NiDynamicEffectState* pkParentState = NULL;
        if (!m_kEffectList.IsEmpty())
        {
            EE_ASSERT(m_spEffectState);
            pkParentState = m_spEffectState;
            NiTListIterator kIter = m_kEffectList.GetHeadPos();
            while (kIter)
            {
                pkParentState->AddEffect(m_kEffectList.GetNext(kIter));
            }
        }
        m_spScreenFillingQuad->UpdateEffectsDownward(pkParentState);
        m_bEffectsChanged = false;
    }

    // Add screen-filling quad to PV geometry array.
    m_kCachedPVGeometry.Add(*m_spScreenFillingQuad);
}
//--------------------------------------------------------------------------------------------------
