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

#include "NSBImplementation.h"
#include "NSBPass.h"
#include "NSBUtility.h"
#include "NSBShader.h"

//------------------------------------------------------------------------------------------------
NSBImplementation::NSBImplementation() :
    m_pcName(0),
    m_pcDesc(0),
    m_pcClassName(0),
    m_uiIndex(0xffffffff),
    m_pkRequirements(0),
    m_pcPackingDef(0),
    m_pkRenderStateGroup(0),
    m_bSoftwareVP(false)
{
    for (unsigned int i = 0; i < NSBConstantMap::NSB_SHADER_TYPE_COUNT; i++)
        m_akShaderConstantMaps[i].RemoveAll();

    m_kPasses.RemoveAll();
    m_kPasses.SetSize(4);
}

//------------------------------------------------------------------------------------------------
NSBImplementation::~NSBImplementation()
{
    NiDelete m_pkRenderStateGroup;
    NiDelete m_pkRequirements;
    NiFree(m_pcPackingDef);
    NiFree(m_pcClassName);
    NiFree(m_pcDesc);
    NiFree(m_pcName);

    // Clean up the shader constant maps.

    unsigned int i = 0;
    for (; i < NSBConstantMap::NSB_SHADER_TYPE_COUNT; i++)
    {
        const unsigned int uiSize = m_akShaderConstantMaps[i].GetSize();
        for (unsigned int j = 0; j < uiSize; j++)
        {
            NSBConstantMap* pkCMap = m_akShaderConstantMaps[i].GetAt(j);
            if (pkCMap)
            {
                NiDelete pkCMap;
                m_akShaderConstantMaps[i].ReplaceAt(i, NULL);
            }
        }
        m_akShaderConstantMaps[i].RemoveAll();
    }

    // Clean up the passes.
    const unsigned int uiSize = m_kPasses.GetSize();
    for (i = 0; i < uiSize; i++)
    {
        NSBPass* pkPass = m_kPasses.GetAt(i);
        if (pkPass)
        {
            NiDelete pkPass;
            m_kPasses.SetAt(i, NULL);
        }
    }
    m_kPasses.RemoveAll();
}

//------------------------------------------------------------------------------------------------
void NSBImplementation::SetName(const char* pcName)
{
    NSBUtility::SetString(m_pcName, 0, pcName);
}

//------------------------------------------------------------------------------------------------
void NSBImplementation::SetDesc(const char* pcDesc)
{
    NSBUtility::SetString(m_pcDesc, 0, pcDesc);
}

//------------------------------------------------------------------------------------------------
void NSBImplementation::SetClassName(const char* pcClassName)
{
    NSBUtility::SetString(m_pcClassName, 0, pcClassName);
}

//------------------------------------------------------------------------------------------------
void NSBImplementation::SetPackingDef(const char* pcPackingDef)
{
    NSBUtility::SetString(m_pcPackingDef, 0, pcPackingDef);
}

//------------------------------------------------------------------------------------------------
NSBRequirements* NSBImplementation::GetRequirements()
{
    if (m_pkRequirements == 0)
        m_pkRequirements = NiNew NSBRequirements();

    return m_pkRequirements;
}

//------------------------------------------------------------------------------------------------
NSBStateGroup* NSBImplementation::GetRenderStateGroup()
{
    if (m_pkRenderStateGroup == 0)
        m_pkRenderStateGroup = NiNew NSBStateGroup();

    return m_pkRenderStateGroup;
}

//------------------------------------------------------------------------------------------------
NSBConstantMap* NSBImplementation::GetVertexConstantMap(unsigned int uiIndex)
{
    return GetConstantMap(NiGPUProgram::PROGRAM_VERTEX, uiIndex);
}

//------------------------------------------------------------------------------------------------
NSBConstantMap* NSBImplementation::GetGeometryConstantMap(unsigned int uiIndex)
{
    return GetConstantMap(NiGPUProgram::PROGRAM_GEOMETRY, uiIndex);
}

//------------------------------------------------------------------------------------------------
NSBConstantMap* NSBImplementation::GetPixelConstantMap(unsigned int uiIndex)
{
    return GetConstantMap(NiGPUProgram::PROGRAM_PIXEL, uiIndex);
}

//------------------------------------------------------------------------------------------------
NSBConstantMap* NSBImplementation::GetConstantMap(
    NiGPUProgram::ProgramType eType, 
    unsigned int uiIndex)
{
    EE_ASSERT((int)eType < (int)NSBConstantMap::NSB_SHADER_TYPE_COUNT);
    if (uiIndex >= m_akShaderConstantMaps[eType].GetSize())
        return NULL;
    return m_akShaderConstantMaps[eType].GetAt(uiIndex);
}

//------------------------------------------------------------------------------------------------
unsigned int NSBImplementation::AddConstantMap(NiGPUProgram::ProgramType eType)
{
    EE_ASSERT((int)eType < (int)NSBConstantMap::NSB_SHADER_TYPE_COUNT);
    NSBConstantMap* pkConstantMap = NiNew NSBConstantMap();
    pkConstantMap->SetProgramType(eType);
    EE_ASSERT(pkConstantMap);
    return m_akShaderConstantMaps[eType].Add(pkConstantMap);
}

//------------------------------------------------------------------------------------------------
unsigned int NSBImplementation::GetPassCount()
{
    return m_kPasses.GetEffectiveSize();
}

//------------------------------------------------------------------------------------------------
NSBPass* NSBImplementation::GetPass(unsigned int uiIndex, bool bCreate)
{
    NSBPass* pkPass = 0;

    if (m_kPasses.GetSize() > uiIndex)
        pkPass = m_kPasses.GetAt(uiIndex);
    if (!pkPass)
    {
        if (bCreate)
        {
            pkPass = NiNew NSBPass();
            EE_ASSERT(pkPass);

            m_kPasses.SetAtGrow(uiIndex, pkPass);
        }
    }

    return pkPass;
}

//------------------------------------------------------------------------------------------------
bool NSBImplementation::SaveBinary(efd::BinaryStream& kStream)
{
    kStream.WriteCString(m_pcName);
    kStream.WriteCString(m_pcDesc);

    // Version 1.5 added a user-defined class name to the implementation
    kStream.WriteCString(m_pcClassName);
    NiStreamSaveBinary(kStream, m_uiIndex);
    if (!SaveBinaryRequirements(kStream))
        return false;

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

    if (!SaveBinaryPackingDef(kStream))
        return false;
    if (!NSBUtility::SaveBinaryStateGroup(kStream, m_pkRenderStateGroup))
        return false;

    for (unsigned int i = 0; i < NSBConstantMap::NSB_SHADER_TYPE_COUNT; i++)
    {
        const unsigned int uiConstantMapCount = 
            m_akShaderConstantMaps[i].GetSize();
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

    if (!SaveBinaryPasses(kStream))
        return false;

    unsigned int uiSoftwareVP = m_bSoftwareVP ? 1 : 0;
    NiStreamSaveBinary(kStream, uiSoftwareVP);

    // Version 2.00 added support for semantic adapter tables
    m_kAdapterTable.SaveBinary(kStream);

    return true;
}

//------------------------------------------------------------------------------------------------
bool NSBImplementation::LoadBinary(efd::BinaryStream& kStream)
{
    m_pcName = kStream.ReadCString();
    m_pcDesc = kStream.ReadCString();

    if (NSBShader::GetReadVersion() >= 0x00010005)
    {
        // Version 1.5 added support for user-defined class names
        m_pcClassName = kStream.ReadCString();
    }
    NiStreamLoadBinary(kStream, m_uiIndex);

    if (!LoadBinaryRequirements(kStream))
        return false;

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

    if (!LoadBinaryPackingDef(kStream))
        return false;

    NiDelete m_pkRenderStateGroup;
    m_pkRenderStateGroup = 0;
    if (!NSBUtility::LoadBinaryStateGroup(kStream, m_pkRenderStateGroup))
        return false;

    for (unsigned int i = 0; i < NSBConstantMap::NSB_SHADER_TYPE_COUNT; i++)
    {
        m_akShaderConstantMaps[i].RemoveAll();
    }

    if (NSBShader::GetReadVersion() < 0x00010013)
    {
        // Before 1.13, there was exactly one constant map for vertex and
        // pixel shaders.
        NSBConstantMap* pkPixelConstantMap = NULL;
        if (!NSBUtility::LoadBinaryConstantMap(kStream,
            pkPixelConstantMap))
        {
            return false;
        }
        m_akShaderConstantMaps[NiGPUProgram::PROGRAM_PIXEL].Add(pkPixelConstantMap);

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
        // Version 3.1 generalized constant maps.
        for (unsigned int i = 0; i < NSBConstantMap::NSB_SHADER_TYPE_COUNT; i++)
        {
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

    if (!LoadBinaryPasses(kStream))
        return false;

    // Version 1.7 added better support for software vertex processing.
    if (NSBShader::GetReadVersion() >= 0x00010007)
    {
        unsigned int uiSoftwareVP = 0;
        NiStreamLoadBinary(kStream, uiSoftwareVP);
        m_bSoftwareVP = (uiSoftwareVP != 0);
    }
    else
    {
        m_bSoftwareVP = false;
    }

    // Version 2.00 added support for semantic adapter tables
    if (NSBShader::GetReadVersion() >= 0x00020000)
    {
        m_kAdapterTable.LoadBinary(kStream);
    }

    return true;
}

//------------------------------------------------------------------------------------------------
bool NSBImplementation::SaveBinaryRequirements(efd::BinaryStream& kStream)
{
    unsigned int uiValue;

    if (m_pkRequirements)
    {
        uiValue = 1;
        NiStreamSaveBinary(kStream, uiValue);

        if (!m_pkRequirements->SaveBinary(kStream))
            return false;
    }
    else
    {
        uiValue = 0;
        NiStreamSaveBinary(kStream, uiValue);
    }

    return true;
}

//------------------------------------------------------------------------------------------------
bool NSBImplementation::SaveBinaryPackingDef(efd::BinaryStream& kStream)
{
    kStream.WriteCString(m_pcPackingDef);

    return true;
}

//------------------------------------------------------------------------------------------------
bool NSBImplementation::SaveBinaryPasses(efd::BinaryStream& kStream)
{
    unsigned int uiCount = m_kPasses.GetEffectiveSize();
    NiStreamSaveBinary(kStream, uiCount);

    unsigned int uiTestCount = 0;
    NSBPass* pkPass;
    for (unsigned int ui = 0; ui < m_kPasses.GetSize(); ui++)
    {
        pkPass = m_kPasses.GetAt(ui);
        if (pkPass)
        {
            if (!pkPass->SaveBinary(kStream))
                return false;
            uiTestCount++;
        }
    }

    if (uiTestCount != uiCount)
        return false;

    return true;
}

//------------------------------------------------------------------------------------------------
bool NSBImplementation::LoadBinaryRequirements(efd::BinaryStream& kStream)
{
    unsigned int uiValue;
    NiStreamLoadBinary(kStream, uiValue);

    if (uiValue == 1)
    {
        m_pkRequirements = NiNew NSBRequirements();
        EE_ASSERT(m_pkRequirements);

        if (!m_pkRequirements->LoadBinary(kStream))
            return false;
    }

    return true;
}

//------------------------------------------------------------------------------------------------
bool NSBImplementation::LoadBinaryPackingDef(efd::BinaryStream& kStream)
{
    m_pcPackingDef = kStream.ReadCString();
    return true;
}

//------------------------------------------------------------------------------------------------
bool NSBImplementation::LoadBinaryPasses(efd::BinaryStream& kStream)
{
    unsigned int uiCount;
    NiStreamLoadBinary(kStream, uiCount);

    m_kPasses.SetSize(uiCount);

    NSBPass* pkPass;
    for (unsigned int ui = 0; ui < uiCount; ui++)
    {
        pkPass = NiNew NSBPass();
        EE_ASSERT(pkPass);

        if (!pkPass->LoadBinary(kStream))
            return false;

        m_kPasses.SetAt(ui, pkPass);

        // If any pass requires SW Vertex processing, then the entire
        // implementation does.
        if (pkPass->GetSoftwareVertexProcessing())
            m_pkRequirements->SetSoftwareVPRequired(true);
    }

    return true;
}

//------------------------------------------------------------------------------------------------
#if defined(NIDEBUG)

//------------------------------------------------------------------------------------------------
void NSBImplementation::Dump(FILE* pf)
{
    NSBUtility::Dump(pf, true, "Implementation %2d - %s\n", m_uiIndex,
        m_pcName);

    NSBUtility::IndentInsert();

    NSBUtility::Dump(pf, true, "Desc: %s", m_pcDesc);
    NSBUtility::Dump(pf, false, "\n");

    NSBUtility::Dump(pf, true, "ClassName: %s", m_pcClassName);
    NSBUtility::Dump(pf, false, "\n");

    NSBUtility::Dump(pf, true, "Software Vertex Processing: %s\n",
        m_bSoftwareVP ? "True" : "False");
    NSBUtility::Dump(pf, true, "\n");

    if (m_pkRequirements)
        m_pkRequirements->Dump(pf);
    else
        NSBUtility::Dump(pf, true, "*** NO REQUIREMENTS ***\n");

    NSBUtility::Dump(pf, true, "PackingDef: %s\n", m_pcPackingDef);

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

    const char* apcShaderStrings[NSBConstantMap::NSB_SHADER_TYPE_COUNT] = 
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
        unsigned int uiConstantMapCount = m_akShaderConstantMaps[i].GetSize();
        if (uiConstantMapCount > 0)
        {
            NSBUtility::Dump(pf, true, "ConstantMaps\n");
            NSBUtility::IndentInsert();
            for (unsigned int j=0; j < uiConstantMapCount; j++)
            {
                m_akShaderConstantMaps[i].GetAt(j)->Dump(pf);
            }
            NSBUtility::IndentRemove();
        }
        else
        {
            NSBUtility::Dump(pf, true, "*** NO %s CMAPS ***\n", apcShaderStrings[i]);
        }
    }

    unsigned int uiCount = m_kPasses.GetSize();
    NSBUtility::Dump(pf, true, "%d Passes\n",
        m_kPasses.GetEffectiveSize());
    NSBUtility::IndentInsert();
    for (unsigned int ui = 0; ui < uiCount; ui++)
    {
        NSBPass* pkPass = m_kPasses.GetAt(ui);
        if (pkPass)
            pkPass->Dump(pf);
    }
    NSBUtility::IndentRemove();

    NSBUtility::IndentRemove();
}

//------------------------------------------------------------------------------------------------
#endif  //#if defined(NIDEBUG)

//------------------------------------------------------------------------------------------------
