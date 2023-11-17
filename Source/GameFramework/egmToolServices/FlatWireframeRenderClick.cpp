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

#include "FlatWireframeRenderClick.h"

#include <NiMaterialLibrary.h>
#include <NiShaderFactory.h>

using namespace ecr;
using namespace egmToolServices;

//--------------------------------------------------------------------------------------------------
FlatWireframeRenderClick::FlatWireframeRenderClick()
    : m_pkWireMaterial(NULL)
{
    m_pkColor = NiNew NiColor(1.0f, 1.0f, 1.0f);
    m_pkBackupColor = NiNew NiColorA();

    m_pkMaterialSwapProcessor = NiNew ConditionalMaterialSwapProcessor();

    SetProcessor(m_pkMaterialSwapProcessor);
}
//--------------------------------------------------------------------------------------------------
FlatWireframeRenderClick::~FlatWireframeRenderClick()
{
    NiDelete m_pkBackupColor;
    NiDelete m_pkColor;
}
//--------------------------------------------------------------------------------------------------
void FlatWireframeRenderClick::PerformRendering(unsigned int uiFrameID)
{
    if (!m_pkWireMaterial)
    {
        m_pkWireMaterial = NiMaterialLibrary::CreateMaterial("NiFlatWireframeMaterial");
        EE_ASSERT(m_pkWireMaterial);

        if (m_pkWireMaterial)
        {
            m_pkMaterialSwapProcessor->SetMaterial(m_pkWireMaterial,
                (unsigned int)NiMaterialInstance::DEFAULT_EXTRA_DATA);
        }
    }

    //// Set wireframe color, backing up existing color.
    NiColorA kWireframeColor(m_pkColor->r, m_pkColor->g, m_pkColor->b, 1.0f);
    unsigned int uiDataSize = 0;
    const void* pvData = NULL;
    if (NiShaderFactory::RetrieveGlobalShaderConstant("WireframeColor", uiDataSize, pvData))
    {
        EE_ASSERT(uiDataSize == sizeof(NiColorA));
        const float* pfData = (const float*) pvData;
        m_pkBackupColor->r = pfData[0];
        m_pkBackupColor->g = pfData[1];
        m_pkBackupColor->b = pfData[2];
        m_pkBackupColor->a = pfData[3];
    }
    else
    {
        *m_pkBackupColor = kWireframeColor;
    }

    NiShaderFactory::UpdateGlobalShaderConstant("WireframeColor", sizeof(NiColorA),
        &kWireframeColor);

    NiViewRenderClick::PerformRendering(uiFrameID);

    //// Restore wireframe color.
    NiShaderFactory::UpdateGlobalShaderConstant("WireframeColor",
        sizeof(NiColorA), m_pkBackupColor);
}
//--------------------------------------------------------------------------------------------------