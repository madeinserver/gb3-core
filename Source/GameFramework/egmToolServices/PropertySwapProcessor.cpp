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

#include <NiRenderObject.h>
#include <NiRenderer.h>

#include "PropertySwapProcessor.h"

using namespace ecr;
using namespace egmToolServices;

NiImplementRTTI(PropertySwapProcessor, NiRenderListProcessor);

//--------------------------------------------------------------------------------------------------
void PropertySwapProcessor::PreRenderProcessList(const NiVisibleArray* pkInput,
                                                 NiVisibleArray& kOutput,
                                                 void* pvExtraData)
{
    EE_UNUSED_ARG(pvExtraData);

    // If the input array pointer is null, do nothing.
    if (!pkInput)
    {
        return;
    }

    const unsigned int uiInputCount = pkInput->GetCount();

    // If the property list pointer is null, defer rendering of all objects.
    if (!m_pkPropertyList)
    {
        for (unsigned int ui = 0; ui < uiInputCount; ui++)
        {
            kOutput.Add(pkInput->GetAt(ui));
        }
        return;
    }

    // Get renderer pointer.
    NiRenderer* pkRenderer = NiRenderer::GetRenderer();
    EE_ASSERT(pkRenderer);

    // Iterate over input array, swapping properties and rendering.
    for (unsigned int ui = 0; ui < uiInputCount; ui++)
    {
        NiRenderObject& kGeometry = pkInput->GetAt(ui);

        NiPropertyState* pkPropertyState = kGeometry.GetPropertyState();
        if (pkPropertyState)
        {
            pkPropertyState->SwapProperties(*m_pkPropertyList);
            kGeometry.SetMaterialNeedsUpdate(true);
            kGeometry.RenderImmediate(pkRenderer);
            pkPropertyState->SwapProperties(*m_pkPropertyList);
            kGeometry.UpdateProperties();
            kGeometry.SetMaterialNeedsUpdate(true);
        }
    }

    // Don't add any geometry objects to the output array.
}
//--------------------------------------------------------------------------------------------------
