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

//------------------------------------------------------------------------------------------------
// Precompiled Header
#include "NSBShaderLibPCH.h"

#include "NSBPass.h"
#include "NSBStateGroup.h"
#include "NSBTexture.h"
#include "NSBTextureStage.h"
#include "NSBUtility.h"
#include "NSBShader.h"

//------------------------------------------------------------------------------------------------
NSBPass::NSBPass() :
    m_pcName(0),
    m_pkRenderStateGroup(0),
    m_bSoftwareVP(false),
    m_uiThreadGroupCountX(1),
    m_uiThreadGroupCountY(1),
    m_uiThreadGroupCountZ(1)
{
    for (unsigned int i = 0; i < NSBConstantMap::NSB_SHADER_TYPE_COUNT; i++)
    {
        m_akShaderConstantMaps[i].RemoveAll();
    }

    m_akStages.RemoveAll();
    m_akTextures.RemoveAll();
}

//------------------------------------------------------------------------------------------------
NSBPass::~NSBPass()
{
    NiFree(m_pcName);
    NiDelete m_pkRenderStateGroup;

    // Clean up the shader constant maps.

    for (unsigned int i = 0; i < NSBConstantMap::NSB_SHADER_TYPE_COUNT; i++)
    {
        const unsigned int uiSize = m_akShaderConstantMaps[i].GetSize();
        for (unsigned int j = 0; j < uiSize; j++)
        {
            NSBConstantMap* pkCMap = m_akShaderConstantMaps[i].GetAt(j);
            if (pkCMap)
            {
                NiDelete pkCMap;
                m_akShaderConstantMaps[i].ReplaceAt(j, NULL);
            }
        }
        m_akShaderConstantMaps[i].RemoveAll();
    }

    // Clean up the stages.
    const unsigned int uiStageCount = m_akStages.GetSize();
    for (unsigned int i = 0; i < uiStageCount; i++)
    {
        NSBTextureStage* pkStage = m_akStages.GetAt(i);
        if (pkStage)
        {
            NiDelete pkStage;
            m_akStages.SetAt(i, 0);
        }
    }

    m_akStages.RemoveAll();

    // Clean up the textures.
    const unsigned int uiTextureCount = m_akTextures.GetSize();
    for (unsigned int i = 0; i < uiTextureCount; i++)
    {
        NSBTexture* pkTexture = m_akTextures.GetAt(i);
        if (pkTexture)
        {
            NiDelete pkTexture;
            m_akTextures.SetAt(i, 0);
        }
    }

    m_akTextures.RemoveAll();
}

//------------------------------------------------------------------------------------------------
void NSBPass::SetName(const char* pcName)
{
    NSBUtility::SetString(m_pcName, 0, pcName);
}

//------------------------------------------------------------------------------------------------
NSBStateGroup* NSBPass::GetRenderStateGroup()
{
    if (m_pkRenderStateGroup == 0)
        m_pkRenderStateGroup = NiNew NSBStateGroup();

    return m_pkRenderStateGroup;
}

//------------------------------------------------------------------------------------------------
bool NSBPass::GetShaderPresent(NiGPUProgram::ProgramType eType) const
{
    for (unsigned int i = 0; i < efd::SystemDesc::RENDERER_NUM; i++)
    {
        if (m_akInfo[i][eType].m_pcProgramFile != NULL)
            return true;
    }

    return false;
}

//------------------------------------------------------------------------------------------------
bool NSBPass::GetShaderPresent(
    NiGPUProgram::ProgramType eType, 
    efd::SystemDesc::RendererID eRendererID) const
{
    EE_ASSERT((int)eType < (int)NSBConstantMap::NSB_SHADER_TYPE_COUNT);
    return (m_akInfo[eRendererID][eType].m_pcProgramFile != NULL);
}

//------------------------------------------------------------------------------------------------
NSBConstantMap* NSBPass::GetVertexConstantMap(unsigned int uiIndex) const
{
    return GetConstantMap(NiGPUProgram::PROGRAM_VERTEX, uiIndex);
}

//------------------------------------------------------------------------------------------------
NSBConstantMap* NSBPass::GetGeometryConstantMap(unsigned int uiIndex) const
{
    return GetConstantMap(NiGPUProgram::PROGRAM_GEOMETRY, uiIndex);
}

//------------------------------------------------------------------------------------------------
NSBConstantMap* NSBPass::GetPixelConstantMap(unsigned int uiIndex) const
{
    return GetConstantMap(NiGPUProgram::PROGRAM_PIXEL, uiIndex);
}

//------------------------------------------------------------------------------------------------
NSBConstantMap* NSBPass::GetConstantMap(NiGPUProgram::ProgramType eType, unsigned int uiIndex) const
{
    EE_ASSERT((int)eType < (int)NSBConstantMap::NSB_SHADER_TYPE_COUNT);
    if (uiIndex >= m_akShaderConstantMaps[eType].GetSize())
        return NULL;
    return m_akShaderConstantMaps[eType].GetAt(uiIndex);
}

//------------------------------------------------------------------------------------------------
unsigned int NSBPass::AddConstantMap(NiGPUProgram::ProgramType eType)
{
    EE_ASSERT((int)eType < (int)NSBConstantMap::NSB_SHADER_TYPE_COUNT);
    NSBConstantMap* pkConstantMap = NiNew NSBConstantMap();
    pkConstantMap->SetProgramType(eType);
    EE_ASSERT(pkConstantMap);
    return m_akShaderConstantMaps[eType].Add(pkConstantMap);
}

//------------------------------------------------------------------------------------------------
unsigned int NSBPass::GetStageCount() const
{
    return m_akStages.GetSize();
}

//------------------------------------------------------------------------------------------------
NSBTextureStage* NSBPass::GetStage(unsigned int uiIndex, bool bCreate)
{
    NSBTextureStage* pkStage = 0;

    if (m_akStages.GetSize() > uiIndex)
        pkStage = m_akStages.GetAt(uiIndex);
    if (!pkStage)
    {
        if (bCreate)
        {
            // Create all stages up to NiNew one, to ensure
            // stage array is packed
            m_akStages.SetSize(uiIndex + 1);
            for (unsigned int i = m_akStages.GetSize(); i <= uiIndex; i++)
            {
                pkStage = NiNew NSBTextureStage();
                EE_ASSERT(pkStage);

                pkStage->SetStage(i);

                m_akStages.SetAt(i, pkStage);
            }
        }
    }

    return pkStage;
}

//------------------------------------------------------------------------------------------------
unsigned int NSBPass::GetTextureCount() const
{
    return m_akTextures.GetSize();
}

//------------------------------------------------------------------------------------------------
NSBTexture* NSBPass::GetTexture(unsigned int uiIndex, bool bCreate)
{
    NSBTexture* pkTexture = 0;

    if (m_akTextures.GetSize() > uiIndex)
        pkTexture = m_akTextures.GetAt(uiIndex);
    if (!pkTexture)
    {
        if (bCreate)
        {
            // Create all stages up to new one, to ensure
            // stage array is packed
            m_akTextures.SetSize(m_akTextures.GetSize() + 1);
            for (unsigned int i = m_akTextures.GetSize(); i <= uiIndex; i++)
            {
                pkTexture = NiNew NSBTexture();
                EE_ASSERT(pkTexture);

                pkTexture->SetStage(i);

                m_akTextures.SetAt(i, pkTexture);
            }
        }
    }

    return pkTexture;
}

//------------------------------------------------------------------------------------------------
bool NSBPass::SaveBinary(efd::BinaryStream& kStream)
{
    kStream.WriteCString(m_pcName);

    // Version 1.6 added support for user-defined data...
    unsigned int uiUDDSetPresent = 0;
    if (m_spUserDefinedDataSet)
    {
        uiUDDSetPresent = 1;
        NiStreamSaveBinary(kStream, uiUDDSetPresent);
        if (!m_spUserDefinedDataSet->SaveBinary(kStream))
            return false;
    }
    else
    {
        NiStreamSaveBinary(kStream, uiUDDSetPresent);
    }

    if (!NSBUtility::SaveBinaryStateGroup(kStream, m_pkRenderStateGroup))
        return false;

    for (unsigned int i = 0; i < NSBConstantMap::NSB_SHADER_TYPE_COUNT; i++)
    {
        if (!SaveBinaryShaderProgram(kStream, (NiGPUProgram::ProgramType)i))
            return false;
        unsigned int uiConstantMapCount = m_akShaderConstantMaps[i].GetSize();
        NiStreamSaveBinary(kStream, uiConstantMapCount);
        for (unsigned int j = 0; j < uiConstantMapCount; j++)
        {
            if (!NSBUtility::SaveBinaryConstantMap(kStream,
                m_akShaderConstantMaps[i].GetAt(j)))
            {
                return false;
            }
        }
    }

    unsigned int uiValue = m_bSoftwareVP ? 1 : 0;
    NiStreamSaveBinary(kStream, uiValue);

    NiStreamSaveBinary(kStream, m_uiThreadGroupCountX);
    NiStreamSaveBinary(kStream, m_uiThreadGroupCountY);
    NiStreamSaveBinary(kStream, m_uiThreadGroupCountZ);

    if (!SaveBinaryStages(kStream))
        return false;

    if (!SaveBinaryTextures(kStream))
        return false;

    if (!m_kStreamOutSettings.SaveBinary(kStream))
        return false;

    return true;
}

//------------------------------------------------------------------------------------------------
bool NSBPass::LoadBinary(efd::BinaryStream& kStream)
{
    m_pcName = kStream.ReadCString();

    // Version 1.6 added support for user-defined data...
    if (NSBShader::GetReadVersion() >= 0x00010006)
    {
        unsigned int uiUDDSetPresent;
        NiStreamLoadBinary(kStream, uiUDDSetPresent);
        if (uiUDDSetPresent)
        {
            m_spUserDefinedDataSet = NiNew NSBUserDefinedDataSet();
            EE_ASSERT(m_spUserDefinedDataSet);
            if (!m_spUserDefinedDataSet->LoadBinary(kStream))
                return false;
        }
        else
        {
            m_spUserDefinedDataSet = 0;
        }
    }

    NiDelete m_pkRenderStateGroup;
    m_pkRenderStateGroup = 0;
    if (!NSBUtility::LoadBinaryStateGroup(kStream, m_pkRenderStateGroup))
        return false;


    if (NSBShader::GetReadVersion() < 0x00010013)
    {
        // Before 1.13, there was exactly one constant map for vertex and
        // pixel shaders.
        if (!LoadBinaryPixelShaderProgram(kStream))
            return false;
        m_akShaderConstantMaps[NiGPUProgram::PROGRAM_PIXEL].RemoveAll();
        NSBConstantMap* pkPixelConstantMap = NULL;
        if (!NSBUtility::LoadBinaryConstantMap(kStream,
            pkPixelConstantMap))
        {
            return false;
        }
        m_akShaderConstantMaps[NiGPUProgram::PROGRAM_PIXEL].Add(pkPixelConstantMap);

        if (!LoadBinaryVertexShaderProgram(kStream))
            return false;
        m_akShaderConstantMaps[NiGPUProgram::PROGRAM_VERTEX].RemoveAll();
        NSBConstantMap* pkVertexConstantMap = NULL;
        if (!NSBUtility::LoadBinaryConstantMap(kStream,
            pkVertexConstantMap))
        {
            return false;
        }
        m_akShaderConstantMaps[NiGPUProgram::PROGRAM_VERTEX].Add(pkVertexConstantMap);
    }
    else if (NSBShader::GetReadVersion() < 0x00030001)
    {
        // Version 1.13 added support for multiple constant maps, D3D10, and
        // geometry shaders.

        if (!LoadBinaryPixelShaderProgram(kStream))
            return false;
        m_akShaderConstantMaps[NiGPUProgram::PROGRAM_PIXEL].RemoveAll();
        unsigned int uiConstantMapCount = 0;
        NiStreamLoadBinary(kStream, uiConstantMapCount);
        for (unsigned int i=0; i < uiConstantMapCount; i++)
        {
            NSBConstantMap* pkPixelConstantMap = NULL;
            if (!NSBUtility::LoadBinaryConstantMap(kStream,
                pkPixelConstantMap))
            {
                return false;
            }
            m_akShaderConstantMaps[NiGPUProgram::PROGRAM_PIXEL].Add(pkPixelConstantMap);
        }

        if (!LoadBinaryVertexShaderProgram(kStream))
            return false;
        m_akShaderConstantMaps[NiGPUProgram::PROGRAM_VERTEX].RemoveAll();
        uiConstantMapCount = 0;
        NiStreamLoadBinary(kStream, uiConstantMapCount);
        for (unsigned int i=0; i < uiConstantMapCount; i++)
        {
            NSBConstantMap* pkVertexConstantMap = NULL;
            if (!NSBUtility::LoadBinaryConstantMap(kStream,
                pkVertexConstantMap))
            {
                return false;
            }
            m_akShaderConstantMaps[NiGPUProgram::PROGRAM_VERTEX].Add(pkVertexConstantMap);
        }

        if (!LoadBinaryGeometryShaderProgram(kStream))
            return false;
        m_akShaderConstantMaps[NiGPUProgram::PROGRAM_GEOMETRY].RemoveAll();
        uiConstantMapCount = 0;
        NiStreamLoadBinary(kStream, uiConstantMapCount);
        for (unsigned int i=0; i < uiConstantMapCount; i++)
        {
            NSBConstantMap* pkGeometryConstantMap = NULL;
            if (!NSBUtility::LoadBinaryConstantMap(kStream,
                pkGeometryConstantMap))
            {
                return false;
            }
            m_akShaderConstantMaps[NiGPUProgram::PROGRAM_GEOMETRY].Add(pkGeometryConstantMap);
        }
    }
    else
    {
        // Version 3.1 generalized shaders and constant maps.
        for (unsigned int i = 0; i < NSBConstantMap::NSB_SHADER_TYPE_COUNT; i++)
        {
            if (!LoadBinaryShaderProgram(kStream, (NiGPUProgram::ProgramType)i))
                return false;
            m_akShaderConstantMaps[i].RemoveAll();
            unsigned int uiConstantMapCount = 0;
            NiStreamLoadBinary(kStream, uiConstantMapCount);
            for (unsigned int j = 0; j < uiConstantMapCount; j++)
            {
                NSBConstantMap* pkConstantMap = NULL;
                if (!NSBUtility::LoadBinaryConstantMap(kStream, pkConstantMap))
                {
                    return false;
                }
                m_akShaderConstantMaps[i].Add(pkConstantMap);
            }
        }
    }

    unsigned int uiSoftwareVP = 0;
    NiStreamLoadBinary(kStream, uiSoftwareVP);
    m_bSoftwareVP = (uiSoftwareVP != 0);

    if (NSBShader::GetReadVersion() >= 0x00030001)
    {
        // Version 3.1 added CS thread group count parameters.
        NiStreamLoadBinary(kStream, m_uiThreadGroupCountX);
        NiStreamLoadBinary(kStream, m_uiThreadGroupCountY);
        NiStreamLoadBinary(kStream, m_uiThreadGroupCountZ);
    }

    if (!LoadBinaryStages(kStream))
        return false;

    if (NSBShader::GetReadVersion() >= 0x00010014)
    {
        // Version 1.14 added separate NSBTexture objects.

        if (!LoadBinaryTextures(kStream))
            return false;
    }

    // Version 2.2 added support for stream output
    if (NSBShader::GetReadVersion() >= 0x00020002)
    {
        m_kStreamOutSettings.LoadBinary(kStream);
    }

    return true;
}

//------------------------------------------------------------------------------------------------
bool NSBPass::LoadBinaryShaderProgram(efd::BinaryStream& kStream,
    NiGPUProgram::ProgramType eType)
{
    const NiSystemDesc::RendererID akStreamedRenderers[] =
    {
        NiSystemDesc::RENDERER_DX9,
        NiSystemDesc::RENDERER_XENON,
        NiSystemDesc::RENDERER_PS3,
        NiSystemDesc::RENDERER_D3D10,
        NiSystemDesc::RENDERER_D3D11
    };
    unsigned int uiNumStreamed = sizeof(akStreamedRenderers) /
        sizeof(NiSystemDesc::RendererID);

    // Version 3.0 added support for D3D11
    if (NSBShader::GetReadVersion() < 0x00030000)
    {
        // Don't include D3D11 if the NSB version is too old.
        uiNumStreamed--;
    }

    for (unsigned int i = 0; i < uiNumStreamed; i++)
    {
        NiSystemDesc::RendererID eRenderer = akStreamedRenderers[i];

        char* pcTemp = kStream.ReadCString();
        SetShaderProgramFile(pcTemp, eRenderer, eType);
        NiFree(pcTemp);
        pcTemp = kStream.ReadCString();
        SetShaderProgramEntryPoint(pcTemp, eRenderer, eType);
        NiFree(pcTemp);
        pcTemp = kStream.ReadCString();
        SetShaderProgramShaderTarget(pcTemp, eRenderer, eType);
        NiFree(pcTemp);
    }

    return true;
}

//------------------------------------------------------------------------------------------------
bool NSBPass::SaveBinaryShaderProgram(efd::BinaryStream& kStream,
    NiGPUProgram::ProgramType eType)
{
    const NiSystemDesc::RendererID akStreamedRenderers[] =
    {
        NiSystemDesc::RENDERER_DX9,
        NiSystemDesc::RENDERER_XENON,
        NiSystemDesc::RENDERER_PS3,
        NiSystemDesc::RENDERER_D3D10,
        NiSystemDesc::RENDERER_D3D11
    };
    const unsigned int uiNumStreamed = sizeof(akStreamedRenderers) /
        sizeof(NiSystemDesc::RendererID);

    for (unsigned int i = 0; i < uiNumStreamed; i++)
    {
        NiSystemDesc::RendererID eRenderer = akStreamedRenderers[i];

        kStream.WriteCString(GetShaderProgramFile(eRenderer, eType));
        kStream.WriteCString(GetShaderProgramEntryPoint(eRenderer, eType));
        kStream.WriteCString(GetShaderProgramShaderTarget(eRenderer, eType));
    }

    return true;
}

//------------------------------------------------------------------------------------------------
bool NSBPass::SaveBinaryStages(efd::BinaryStream& kStream)
{
    unsigned int uiCount = m_akStages.GetEffectiveSize();

    NiStreamSaveBinary(kStream, uiCount);

    unsigned int uiTestCount = 0;
    NSBTextureStage* pkStage;
    for (unsigned int ui = 0; ui < m_akStages.GetSize(); ui++)
    {
        pkStage = m_akStages.GetAt(ui);
        if (pkStage)
        {
            if (!pkStage->SaveBinary(kStream))
                return false;
            uiTestCount++;
        }
    }

    if (uiTestCount != uiCount)
        return false;

    return true;
}

//------------------------------------------------------------------------------------------------
bool NSBPass::SaveBinaryTextures(efd::BinaryStream& kStream)
{
    unsigned int uiCount = m_akTextures.GetEffectiveSize();

    NiStreamSaveBinary(kStream, uiCount);

    unsigned int uiTestCount = 0;
    NSBTexture* pkTexture;
    for (unsigned int ui = 0; ui < m_akTextures.GetSize(); ui++)
    {
        pkTexture = m_akTextures.GetAt(ui);
        if (pkTexture)
        {
            if (!pkTexture->SaveBinary(kStream))
                return false;
            uiTestCount++;
        }
    }

    if (uiTestCount != uiCount)
        return false;

    return true;
}

//------------------------------------------------------------------------------------------------
bool NSBPass::LoadBinaryVertexShaderProgram(efd::BinaryStream& kStream)
{
    if (NSBShader::GetReadVersion() >= 0x00020001)
        return LoadBinaryShaderProgram(kStream, NiGPUProgram::PROGRAM_VERTEX);

    if (NSBShader::GetReadVersion() == 0x00010001)
    {
        // Version 1.1 only had a single shader file name.  Load first into
        // PS3 name (arbitrary choice - doesn't matter) and then copy into
        // other platform's names.
        char* pcProgramFile = kStream.ReadCString();
        SetShaderProgramFile(pcProgramFile, NiSystemDesc::RENDERER_PS3,
            NiGPUProgram::PROGRAM_VERTEX);
        SetShaderProgramFile(pcProgramFile, NiSystemDesc::RENDERER_DX9,
            NiGPUProgram::PROGRAM_VERTEX);
        SetShaderProgramFile(pcProgramFile, NiSystemDesc::RENDERER_XENON,
            NiGPUProgram::PROGRAM_VERTEX);
        SetShaderProgramFile(pcProgramFile, NiSystemDesc::RENDERER_D3D10,
            NiGPUProgram::PROGRAM_VERTEX);
        SetShaderProgramFile(pcProgramFile, NiSystemDesc::RENDERER_D3D11,
            NiGPUProgram::PROGRAM_VERTEX);
        NiFree(pcProgramFile);
    }
    else
    {
        if (NSBShader::GetReadVersion() < 0x00010008)
        {
            char* pcTemp = 0;

            // Removed Xbox support in version 1.8
            pcTemp = kStream.ReadCString();
            NiFree(pcTemp);

            // Removed DX8 support in version 1.8
            pcTemp = kStream.ReadCString();
            NiFree(pcTemp);
        }

        char* pcTemp = kStream.ReadCString();
        SetShaderProgramFile(pcTemp,
            NiSystemDesc::RENDERER_DX9, NiGPUProgram::PROGRAM_VERTEX);
        NiFree(pcTemp);
        if (NSBShader::GetReadVersion() >= 0x00010008)
        {
            // Added Xenon support in version 1.8
            pcTemp = kStream.ReadCString();
            SetShaderProgramFile(pcTemp,
               NiSystemDesc::RENDERER_XENON, NiGPUProgram::PROGRAM_VERTEX);
            NiFree(pcTemp);
        }
        if (NSBShader::GetReadVersion() >= 0x00010012)
        {
            // Added PS3 support in version 1.12
            pcTemp = kStream.ReadCString();
            SetShaderProgramFile(pcTemp,
               NiSystemDesc::RENDERER_PS3, NiGPUProgram::PROGRAM_VERTEX);
            NiFree(pcTemp);
        }
        if (NSBShader::GetReadVersion() >= 0x00010013)
        {
            // Added D3D10 and geometry shader support in version 1.13.
            pcTemp = kStream.ReadCString();
            SetShaderProgramFile(pcTemp,
               NiSystemDesc::RENDERER_D3D10, NiGPUProgram::PROGRAM_VERTEX);
            NiFree(pcTemp);
        }
        if (NSBShader::GetReadVersion() >= 0x00030000)
        {
            // Added D3D11 support in version 3.0.
            pcTemp = kStream.ReadCString();
            SetShaderProgramFile(pcTemp,
               NiSystemDesc::RENDERER_D3D11, NiGPUProgram::PROGRAM_VERTEX);
            NiFree(pcTemp);
        }
    }

    // Added renderer-specific entry point and targets in version 2.1.
    char* pcProgramEntryPoint = kStream.ReadCString();
    char* pcProgramTarget = kStream.ReadCString();

    for (unsigned int i = 0; i < NiSystemDesc::RENDERER_NUM; i++)
    {
        SetShaderProgramEntryPoint(pcProgramEntryPoint,
            NiSystemDesc::RendererID(i), NiGPUProgram::PROGRAM_VERTEX);
        SetShaderProgramShaderTarget(pcProgramTarget,
            NiSystemDesc::RendererID(i), NiGPUProgram::PROGRAM_VERTEX);
    }
    NiFree(pcProgramEntryPoint);
    NiFree(pcProgramTarget);

    return true;
}

//------------------------------------------------------------------------------------------------
bool NSBPass::LoadBinaryGeometryShaderProgram(efd::BinaryStream& kStream)
{
    if (NSBShader::GetReadVersion() >= 0x00020001)
    {
        return LoadBinaryShaderProgram(kStream,
            NiGPUProgram::PROGRAM_GEOMETRY);
    }


    if (NSBShader::GetReadVersion() < 0x00010013)
    {
        // Geometry shaders did not exist before version 1.13.
        return true;
    }

    // Added geometry shader support for DX9, Xenon, PS3, and D3D10 in
    // version 1.13.
    char* pcTemp = kStream.ReadCString();
    SetShaderProgramFile(pcTemp,
       NiSystemDesc::RENDERER_DX9, NiGPUProgram::PROGRAM_GEOMETRY);
    NiFree(pcTemp);
    pcTemp = kStream.ReadCString();
    SetShaderProgramFile(pcTemp,
       NiSystemDesc::RENDERER_XENON, NiGPUProgram::PROGRAM_GEOMETRY);
    NiFree(pcTemp);
    pcTemp = kStream.ReadCString();
    SetShaderProgramFile(pcTemp,
       NiSystemDesc::RENDERER_PS3, NiGPUProgram::PROGRAM_GEOMETRY);
    NiFree(pcTemp);
    pcTemp = kStream.ReadCString();
    SetShaderProgramFile(pcTemp,
       NiSystemDesc::RENDERER_D3D10, NiGPUProgram::PROGRAM_GEOMETRY);
    NiFree(pcTemp);
    if (NSBShader::GetReadVersion() < 0x00030000)
    {
        // Added D3D11 in version 3.0.
        pcTemp = kStream.ReadCString();
        SetShaderProgramFile(pcTemp,
            NiSystemDesc::RENDERER_D3D11, NiGPUProgram::PROGRAM_GEOMETRY);
        NiFree(pcTemp);
    }

    // Added renderer-specific entry point and targets in version 2.1.
    char* pcProgramEntryPoint = kStream.ReadCString();
    char* pcProgramTarget = kStream.ReadCString();

    for (unsigned int i = 0; i < NiSystemDesc::RENDERER_NUM; i++)
    {
        SetShaderProgramEntryPoint(pcProgramEntryPoint,
            NiSystemDesc::RendererID(i), NiGPUProgram::PROGRAM_GEOMETRY);
        SetShaderProgramShaderTarget(pcProgramTarget,
            NiSystemDesc::RendererID(i), NiGPUProgram::PROGRAM_GEOMETRY);
    }
    NiFree(pcProgramEntryPoint);
    NiFree(pcProgramTarget);

    return true;
}

//------------------------------------------------------------------------------------------------
bool NSBPass::LoadBinaryPixelShaderProgram(efd::BinaryStream& kStream)
{
    if (NSBShader::GetReadVersion() >= 0x00020001)
    {
        return LoadBinaryShaderProgram(kStream,
            NiGPUProgram::PROGRAM_PIXEL);
    }

    if (NSBShader::GetReadVersion() == 0x00010001)
    {
        // Version 1.1 only had a single shader file name
        char* pcProgramFile = kStream.ReadCString();
        SetShaderProgramFile(pcProgramFile, NiSystemDesc::RENDERER_PS3,
            NiGPUProgram::PROGRAM_PIXEL);
        SetShaderProgramFile(pcProgramFile, NiSystemDesc::RENDERER_DX9,
            NiGPUProgram::PROGRAM_PIXEL);
        SetShaderProgramFile(pcProgramFile, NiSystemDesc::RENDERER_XENON,
            NiGPUProgram::PROGRAM_PIXEL);
        SetShaderProgramFile(pcProgramFile, NiSystemDesc::RENDERER_D3D10,
            NiGPUProgram::PROGRAM_PIXEL);
        SetShaderProgramFile(pcProgramFile, NiSystemDesc::RENDERER_D3D11,
            NiGPUProgram::PROGRAM_PIXEL);
        NiFree(pcProgramFile);
    }
    else
    {
        char* pcTemp = 0;
        if (NSBShader::GetReadVersion() < 0x00010008)
        {


            // Removed Xbox support in version 1.8
            pcTemp = kStream.ReadCString();
            NiFree(pcTemp);

            // Removed DX8 support in version 1.8
            pcTemp = kStream.ReadCString();
            NiFree(pcTemp);
        }

        pcTemp = kStream.ReadCString();
        SetShaderProgramFile(pcTemp,
            NiSystemDesc::RENDERER_DX9, NiGPUProgram::PROGRAM_PIXEL);
        NiFree(pcTemp);

        if (NSBShader::GetReadVersion() >= 0x00010008)
        {
            // Added Xenon support in version 1.8
            pcTemp = kStream.ReadCString();
            SetShaderProgramFile(pcTemp,
                NiSystemDesc::RENDERER_XENON, NiGPUProgram::PROGRAM_PIXEL);
            NiFree(pcTemp);
        }
        if (NSBShader::GetReadVersion() >= 0x00010012)
        {
            // Added PS3 support in version 1.12
            pcTemp = kStream.ReadCString();
            SetShaderProgramFile(pcTemp,
                NiSystemDesc::RENDERER_PS3, NiGPUProgram::PROGRAM_PIXEL);
            NiFree(pcTemp);
        }
        if (NSBShader::GetReadVersion() >= 0x00010013)
        {
            // Added D3D10 and geometry shader support in version 1.13.
            pcTemp = kStream.ReadCString();
            SetShaderProgramFile(pcTemp,
                NiSystemDesc::RENDERER_D3D10, NiGPUProgram::PROGRAM_PIXEL);
            NiFree(pcTemp);
        }
        if (NSBShader::GetReadVersion() >= 0x00030000)
        {
            // Added D3D11 support in version 3.0.
            pcTemp = kStream.ReadCString();
            SetShaderProgramFile(pcTemp,
                NiSystemDesc::RENDERER_D3D11, NiGPUProgram::PROGRAM_PIXEL);
            NiFree(pcTemp);
        }
    }

    // Version 1.1 did not have entry points or targets.
    if (NSBShader::GetReadVersion() <= 0x00010001)
        return true;

    // Added renderer-specific entry point and targets in version 2.1.
    char* pcProgramEntryPoint = kStream.ReadCString();
    char* pcProgramTarget = kStream.ReadCString();

    for (unsigned int i = 0; i < NiSystemDesc::RENDERER_NUM; i++)
    {
        SetShaderProgramEntryPoint(pcProgramEntryPoint,
            NiSystemDesc::RendererID(i), NiGPUProgram::PROGRAM_PIXEL);
        SetShaderProgramShaderTarget(pcProgramTarget,
            NiSystemDesc::RendererID(i), NiGPUProgram::PROGRAM_PIXEL);
    }
    NiFree(pcProgramEntryPoint);
    NiFree(pcProgramTarget);

    return true;
}

//------------------------------------------------------------------------------------------------
bool NSBPass::LoadBinaryStages(efd::BinaryStream& kStream)
{
    unsigned int uiCount;

    NiStreamLoadBinary(kStream, uiCount);

    m_akStages.SetSize(uiCount);

    NSBTextureStage* pkStage;
    for (unsigned int ui = 0; ui < uiCount; ui++)
    {
        pkStage = NiNew NSBTextureStage();
        EE_ASSERT(pkStage);

        if (!pkStage->LoadBinary(kStream))
            return false;

        EE_ASSERT(ui == pkStage->GetStage());
        m_akStages.SetAt(ui, pkStage);
    }

    return true;
}

//------------------------------------------------------------------------------------------------
bool NSBPass::LoadBinaryTextures(efd::BinaryStream& kStream)
{
    unsigned int uiCount;

    NiStreamLoadBinary(kStream, uiCount);

    m_akTextures.SetSize(uiCount);

    NSBTexture* pkTexture;
    for (unsigned int ui = 0; ui < uiCount; ui++)
    {
        pkTexture = NiNew NSBTexture();
        EE_ASSERT(pkTexture);

        if (!pkTexture->LoadBinary(kStream))
            return false;

        EE_ASSERT(ui == pkTexture->GetStage());
        m_akTextures.SetAt(ui, pkTexture);
    }

    return true;
}

//------------------------------------------------------------------------------------------------
NSBPass::ShaderProgramInfo::ShaderProgramInfo() :
    m_pcProgramFile(NULL),
    m_pcEntryPoint(NULL),
    m_pcTarget(NULL)
{
}

//------------------------------------------------------------------------------------------------
NSBPass::ShaderProgramInfo::~ShaderProgramInfo()
{
    NiFree(m_pcProgramFile);
    NiFree(m_pcEntryPoint);
    NiFree(m_pcTarget);
}

//------------------------------------------------------------------------------------------------
#if defined(NIDEBUG)

//------------------------------------------------------------------------------------------------
void NSBPass::Dump(FILE* pf)
{
    NSBUtility::Dump(pf, true, "Pass %s\n", m_pcName);
    NSBUtility::IndentInsert();

    if (m_pkRenderStateGroup)
    {
        NSBUtility::Dump(pf, true, "RenderStateGroup\n");
        NSBUtility::IndentInsert();
        m_pkRenderStateGroup->Dump(pf, NSBStateGroup::DUMP_RENDERSTATES);
        NSBUtility::IndentRemove();
    }
    else
    {
        NSBUtility::Dump(pf, true, "***   NO RS GROUP   ***\n");
    }

    NSBUtility::Dump(pf, true, "\n");

    const char* apcShaderStrings[][NSBConstantMap::NSB_SHADER_TYPE_COUNT] = 
    {
        "Vertex",
        "Hull",
        "Domain",
        "Geometry",
        "Pixel",
        "Compute"
    };

    for (unsigned int i = 0; i < NSBConstantMap::NSB_SHADER_TYPE_COUNT; i++)
    {
        for (unsigned int j = 0; j < NiSystemDesc::RENDERER_NUM; j++)
        {
            NiSystemDesc::RendererID eRenderer = NiSystemDesc::RendererID(j);
            const char* pcRend = NiSystemDesc::GetRendererString(eRenderer);
            NSBUtility::Dump(pf, true, "%sShader Program File (%s) : %s\n",
                apcShaderStrings[i],
                pcRend, 
                GetShaderProgramFile(eRenderer, (NiGPUProgram::ProgramType)i));
            NSBUtility::Dump(pf, true,
                "%sShader Program Entry Point (%s): %s\n", 
                apcShaderStrings[i],
                pcRend,
                GetShaderProgramEntryPoint(eRenderer,
                (NiGPUProgram::ProgramType)i));
            NSBUtility::Dump(pf, true,
                "%sShader Program Shader Target (%s): %s\n", 
                apcShaderStrings[i],
                pcRend,
                GetShaderProgramShaderTarget(eRenderer,
                (NiGPUProgram::ProgramType)i));
        }
        NSBUtility::Dump(pf, true, "\n");

        const unsigned int uiConstantMapCount = m_akShaderConstantMaps[i].GetSize();
        if (uiConstantMapCount > 0)
        {
            NSBUtility::Dump(pf, true, "%sConstantMaps\n",
                apcShaderStrings[i]);
            NSBUtility::IndentInsert();
            for (unsigned int j = 0; j < uiConstantMapCount; j++)
            {
                m_akShaderConstantMaps[i].GetAt(j)->Dump(pf);
            }
            NSBUtility::IndentRemove();
        }
        else
        {
            NSBUtility::Dump(pf, true, "*** NO %s CMAPS  ***\n",
                apcShaderStrings[i]);
        }

        NSBUtility::Dump(pf, true, "\n");
    }

    NSBUtility::Dump(pf, true, "Software Vertex Processing: %s\n",
        m_bSoftwareVP ? "True" : "False");

    unsigned int uiCount = m_akStages.GetSize();
    NSBUtility::Dump(pf, true, "Stage Count = %d\n",
        m_akStages.GetEffectiveSize());
    NSBUtility::IndentInsert();
    for (unsigned int ui = 0; ui < uiCount; ui++)
    {
        NSBTextureStage* pkStage = m_akStages.GetAt(ui);
        if (pkStage)
            pkStage->Dump(pf);
    }
    NSBUtility::IndentRemove();

    uiCount = m_akTextures.GetSize();
    NSBUtility::Dump(pf, true, "Texture Count = %d\n",
        m_akTextures.GetEffectiveSize());
    NSBUtility::IndentInsert();
    for (unsigned int ui = 0; ui < uiCount; ui++)
    {
        NSBTexture* pkTexture = m_akTextures.GetAt(ui);
        if (pkTexture)
            pkTexture->Dump(pf);
    }
    NSBUtility::IndentRemove();

    NSBUtility::IndentRemove();
}

//------------------------------------------------------------------------------------------------
#endif  //#if defined(NIDEBUG)

//------------------------------------------------------------------------------------------------
