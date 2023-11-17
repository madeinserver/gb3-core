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

#include "egmVisualizersPCH.h"
#include "AttenuationPropertyVisualizer.h"
#include <egf/ExtraData.h>
#include <egmVisualizers/PropertyVisualizationHelpers.h>
#include <NiMath.h>

using namespace efd;
using namespace egf;
using namespace ecr;
using namespace egmVisualizers;

const efd::Float32 AttenuationPropertyVisualizer::MIN_INTENSITY = 0.0000001f;
const efd::Float32 AttenuationPropertyVisualizer::DEFAULT_ORIGINAL_INTENSITY = 1.0f;
const efd::Float32 AttenuationPropertyVisualizer::DEFAULT_FINAL_INTENSITY = 0.05f;

//--------------------------------------------------------------------------------------------------
AttenuationPropertyVisualizer::AttenuationPropertyVisualizer(ExtraDataPtr spExtraData) :
    RadiusPropertyVisualizer(spExtraData),
    m_originalIntensity(DEFAULT_ORIGINAL_INTENSITY),
    m_finalIntensity(DEFAULT_FINAL_INTENSITY)
{
}
//--------------------------------------------------------------------------------------------------
PropertyChangeAction AttenuationPropertyVisualizer::UpdateFromEntity(Entity* pEntity)
{
    PropertyChangeAction action = RadiusPropertyVisualizer::UpdateFromEntity(pEntity);

    // MaxRange will take the place of the RadisuPropertyVisualizer's Radius property if no
    // attenuation is used.  This is to prevent us from having to rewrite all the functionality
    // RadiusPropertyVisualizer just to use a MaxRange property binding.
    efd::Float32 maxRange = 0.0f;
    if (PropertyVisualizationHelpers::GetDependency(
        m_spExtraData,
        pEntity,
        "MaxRange",
        "No Default",
        maxRange) == PropertyResult_OK && maxRange >= 0.0f)
    {
        m_maxRange = maxRange;
        m_radius = m_maxRange;
        action = PropertyChangeAction_UpdateTransforms;
    }
    else
    {
        m_maxRange = 0.0f;
        m_radius = 0.0f;
    }

    // Get the attenuation values
    efd::Float32 constant = 0.0f, linear = 0.0f, quadratic = 0.0f;
    if (PropertyVisualizationHelpers::GetDependency(
        m_spExtraData,
        pEntity,
        "ConstantAttenuation",
        "ConstantAttenuation",
        constant) == PropertyResult_OK)
    {
        m_constant = constant;
        action = PropertyChangeAction_UpdateTransforms;
    }

    if (PropertyVisualizationHelpers::GetDependency(
        m_spExtraData,
        pEntity,
        "LinearAttenuation",
        "LinearAttenuation",
        linear) == PropertyResult_OK)
    {
        m_linear = linear;
        action = PropertyChangeAction_UpdateTransforms;
    }

    if (PropertyVisualizationHelpers::GetDependency(
        m_spExtraData,
        pEntity,
        "QuadraticAttenuation",
        "QuadraticAttenuation",
        quadratic) == PropertyResult_OK)
    {
        m_quadratic = quadratic;
        action = PropertyChangeAction_UpdateTransforms;
    }

    // Get the original intensity, so generally speaking, the light dimmer value.
    efd::Float32 originalIntensity;
    if (PropertyVisualizationHelpers::GetDependency(
        m_spExtraData,
        pEntity,
        "OriginalIntensity",
        "OriginalIntensity",
        originalIntensity) == PropertyResult_OK)
    {
        m_originalIntensity = efd::Max(MIN_INTENSITY, originalIntensity);
        action = PropertyChangeAction_UpdateTransforms;
    }
    else
    {
        m_originalIntensity = DEFAULT_ORIGINAL_INTENSITY;
    }

    // If the threshold was set, make sure it's above the minimum threshold
    // If it wasn't set, set it to a default threshold
    efd::Float32 finalIntensity;
    if (PropertyVisualizationHelpers::GetDependency(
        m_spExtraData,
        pEntity,
        "FinalIntensity",
        "FinalIntensity",
        finalIntensity) == PropertyResult_OK)
    {
        m_finalIntensity = efd::Max(MIN_INTENSITY, finalIntensity);
        action = PropertyChangeAction_UpdateTransforms;
    }
    else
    {
        m_finalIntensity = DEFAULT_FINAL_INTENSITY;
    }

    // If there is some attenuation, we want to know when that attenuation drops to below 1%.
    // Set this to m_radius, which the RadiusPropertyVisualizer uses to set the size of the radius.

    // Epsilon for floating point comparisons
    float epsilon = 0.00000001f;

    float invIntensity = (m_originalIntensity / m_finalIntensity);

    float dist = 0.0f;

    // Quadratic & Linear Attenuation = 0.  The constant attenuation either makes the
    // visualizer completely on or off.
    if ((m_quadratic < epsilon && m_quadratic > -epsilon) &&
        (m_linear < epsilon && m_linear > -epsilon))
    {
        if (m_constant >= invIntensity)
            dist = 0.0f;
    }
    // Quadratic attenuation = 0.  x = (1/Att - C) / B
    else if (m_quadratic < epsilon && m_quadratic > -epsilon)
    {
        dist = (invIntensity - m_constant) / m_linear;
    }
    // Use the quadratic formula if we have quadratic & linear attenuation set
    else
    {
        float det = m_linear * m_linear - 4 * m_quadratic * (m_constant - invIntensity);
        dist = efd::Abs((-m_linear + NiSqrt(efd::Abs(det))) / (2.0f * m_quadratic));
    }

    if (dist < epsilon)
        dist = 0.0f;
    else if (dist > m_maxRange)
        dist = m_maxRange;

    m_radius = dist;

    return action;
}
//--------------------------------------------------------------------------------------------------
