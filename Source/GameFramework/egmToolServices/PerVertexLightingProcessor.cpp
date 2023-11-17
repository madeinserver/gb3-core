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
#include <NiMesh.h>
#include <NiCloningProcess.h>
#include <NiRenderObject.h>
#include <NiAlphaProperty.h>
#include "PerVertexLightingProcessor.h"

using namespace egmToolServices;

//--------------------------------------------------------------------------------------------------
PerVertexLightingProcessor::PerVertexLightingProcessor()
{
}

//--------------------------------------------------------------------------------------------------
PerVertexLightingProcessor::~PerVertexLightingProcessor()
{
    m_kTextureProps.RemoveAll();
}

//--------------------------------------------------------------------------------------------------
void PerVertexLightingProcessor::PreRenderProcessList(
    const NiVisibleArray* pkInput, NiVisibleArray& kOutput, void*)
{
    if (!pkInput)
        return;

    // Get renderer pointer
    NiRenderer* pkRenderer = NiRenderer::GetRenderer();
    EE_ASSERT(pkRenderer);

    // Get camera data
    NiPoint3 kWorldLoc, kWorldDir, kWorldUp, kWorldRight;
    NiFrustum kFrustum;
    NiRect<float> kViewport;
    pkRenderer->GetCameraData(kWorldLoc, kWorldDir, kWorldUp, kWorldRight, kFrustum, kViewport);

    const unsigned int uiInputCount = pkInput->GetCount();

    // Initialize size of object depths array
    if (m_uiAllocatedDepths < uiInputCount)
    {
        NiFree(m_pfDepths);
        m_pfDepths = NiAlloc(float, uiInputCount);
        m_uiAllocatedDepths = uiInputCount;
    }

    // Iterator over input geometry array
    unsigned int uiDepthIndex = 0;
    for (unsigned int ui = 0; ui < uiInputCount; ui++)
    {
        NiRenderObject& kMesh = pkInput->GetAt(ui);

        if (IsTransparent(kMesh))
        {
            kOutput.Add(kMesh);

            m_pfDepths[uiDepthIndex++] = ComputeDepth(kMesh, kWorldDir);
        }
        else
        {
            // Render opaque geometry immediately.
            NiAVObject* pkAVObj = &kMesh;
            if (NiIsKindOf(NiRenderObject, pkAVObj))
            {
                NiRenderObject* pkRenderObj = reinterpret_cast<NiRenderObject*>(pkAVObj);
                PreRender(pkRenderObj);
                kMesh.RenderImmediate(pkRenderer);
                PostRender(pkRenderObj);
            }
        }
    }

    // Sort output array by depth
    SortObjectsByDepth(kOutput, 0, kOutput.GetCount() - 1);

    // Render transparent objects
    const unsigned int uiGeometryCount = kOutput.GetCount();
    for (unsigned int ui = uiGeometryCount; ui > 0; ui--)
    {
        NiAVObject* pkAVObj = &kOutput.GetAt(ui - 1);
        EE_ASSERT(NiIsKindOf(NiRenderObject, pkAVObj));
        
        NiRenderObject* pkRenderObj = static_cast<NiRenderObject*>(pkAVObj);
        PreRender(pkRenderObj);
        pkRenderObj->RenderImmediate(pkRenderer);
        PostRender(pkRenderObj);
    }

    kOutput.RemoveAll();
}

//--------------------------------------------------------------------------------------------------
void PerVertexLightingProcessor::PreRender(NiRenderObject* pkGeometry)
{
    NiPropertyState* pkPropertyState = pkGeometry->GetPropertyState();
    EE_ASSERT(pkPropertyState);

    NiTexturingPropertyPtr spTexProp =
        pkPropertyState->GetTexturing();

    // Only replace the texturing property if there's actually something to replace
    if (spTexProp != NiTexturingProperty::GetDefault())
    {
        NiTexturingPropertyPtr spModifiedTexProp = NULL;
        
        if (!m_kTextureProps.GetAt(pkGeometry, spModifiedTexProp))
        {
            // Create normal, parallax, and bump mapless version of the texture property.
            spModifiedTexProp = (NiTexturingProperty*)spTexProp->Clone();
            spModifiedTexProp->SetNormalMap(NULL);
            spModifiedTexProp->SetParallaxMap(NULL);
            spModifiedTexProp->SetBumpMap(NULL);
        }

        // Swap texture properties
        pkGeometry->DetachProperty(spTexProp);
        pkGeometry->AttachProperty(spModifiedTexProp);
        pkGeometry->UpdateProperties();
        m_kTextureProps.SetAt(pkGeometry, spTexProp);
    }
}

//--------------------------------------------------------------------------------------------------
void PerVertexLightingProcessor::PostRender(NiRenderObject* pkGeometry)
{
    NiPropertyState* pkPropertyState = pkGeometry->GetPropertyState();
    EE_ASSERT(pkPropertyState);

    // Get texturing property with out base map
    NiTexturingPropertyPtr spModifiedTexProp =
        pkPropertyState->GetTexturing();

    if (spModifiedTexProp != NiTexturingProperty::GetDefault())
    {
        // Get texturing property with base map
        NiTexturingPropertyPtr spTexProp = NULL;
        m_kTextureProps.GetAt(pkGeometry, spTexProp);

        // Swap texture properties
        pkGeometry->DetachProperty(spModifiedTexProp);
        pkGeometry->AttachProperty(spTexProp);
        pkGeometry->UpdateProperties();
        m_kTextureProps.SetAt(pkGeometry, spModifiedTexProp);
    }
}

//--------------------------------------------------------------------------------------------------
bool PerVertexLightingProcessor::IsTransparent(const NiRenderObject& kObject)
{
    // Get property state.
    const NiPropertyState* pkPropState = kObject.GetPropertyState();
    EE_ASSERT(pkPropState);

    // Get alpha property.
    const NiAlphaProperty* pkAlphaProp = pkPropState->GetAlpha();
    EE_ASSERT(pkAlphaProp);

    if (pkAlphaProp->GetAlphaBlending())
    {
        return true;
    }

    return false;
}

//--------------------------------------------------------------------------------------------------
