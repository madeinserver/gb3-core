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

//--------------------------------------------------------------------------------------------------
inline NiFixedString NiTerrainMaterial::GenerateSamplerName(
    const NiFixedString& kBaseName, NiUInt32 uiIndex)
{
    char acResult[32];
    NiSprintf(acResult, 32, "%s%d", (const char*)kBaseName, uiIndex);
    return acResult;
}

//--------------------------------------------------------------------------------------------------
inline NiMaterialResource* NiTerrainMaterial::AddTextureSampler(
    NiFragmentMaterial::Context& kContext,
    const NiFixedString& kSamplerName, NiUInt32 uiOccurance)
{
    NiMaterialResource* pkSampler =
        kContext.m_spUniforms->GetInputResourceByVariableName(kSamplerName);

    if (pkSampler)
    {
        return pkSampler;
    }
    else
    {
        return kContext.m_spUniforms->AddOutputResource("sampler2D", "Shader",
            "", kSamplerName, 1, NiMaterialResource::SOURCE_PREDEFINED,
            NiShaderAttributeDesc::OT_UNDEFINED, uiOccurance);
    }
}
//---------------------------------------------------------------------------
inline void NiTerrainMaterial::SetForceEnableNormalParallaxMaps(bool bEnable)
{
    m_bEnableNPOnSM2 = bEnable;
}
//---------------------------------------------------------------------------
