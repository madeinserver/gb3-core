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
// NSBImplementation inline functions
//------------------------------------------------------------------------------------------------
inline const char* NSBImplementation::GetName()
{
    return m_pcName;
}

//------------------------------------------------------------------------------------------------
inline const char* NSBImplementation::GetDesc()
{
    return m_pcDesc;
}

//------------------------------------------------------------------------------------------------
inline const char* NSBImplementation::GetClassName()
{
    return m_pcClassName;
}

//------------------------------------------------------------------------------------------------
inline const char* NSBImplementation::GetPackingDef()
{
    return m_pcPackingDef;
}

//------------------------------------------------------------------------------------------------
inline unsigned int NSBImplementation::GetIndex()
{
    return m_uiIndex;
}

//------------------------------------------------------------------------------------------------
inline void NSBImplementation::SetIndex(unsigned int uiIndex)
{
    m_uiIndex = uiIndex;
}

//------------------------------------------------------------------------------------------------
inline NSBUserDefinedDataSet* NSBImplementation::GetUserDefinedDataSet()
{
    return m_spUserDefinedDataSet;
}

//------------------------------------------------------------------------------------------------
inline void NSBImplementation::SetUserDefinedDataSet(
    NSBUserDefinedDataSet* pkUDDSet)
{
    m_spUserDefinedDataSet = pkUDDSet;
}

//------------------------------------------------------------------------------------------------
inline void NSBImplementation::SetSoftwareVP(bool bSoftwareVP)
{
    m_bSoftwareVP = bSoftwareVP;
}

//------------------------------------------------------------------------------------------------
inline bool NSBImplementation::GetSoftwareVP() const
{
    return m_bSoftwareVP;
}

//------------------------------------------------------------------------------------------------
inline unsigned int NSBImplementation::GetVertexConstantMapCount()
{
    return GetConstantMapCount(NiGPUProgram::PROGRAM_VERTEX);
}

//------------------------------------------------------------------------------------------------
inline unsigned int NSBImplementation::GetGeometryConstantMapCount()
{
    return GetConstantMapCount(NiGPUProgram::PROGRAM_GEOMETRY);
}

//------------------------------------------------------------------------------------------------
inline unsigned int NSBImplementation::GetPixelConstantMapCount()
{
    return GetConstantMapCount(NiGPUProgram::PROGRAM_PIXEL);
}

//------------------------------------------------------------------------------------------------
inline unsigned int NSBImplementation::GetConstantMapCount(NiGPUProgram::ProgramType eType)
{
    EE_ASSERT((int)eType < (int)NSBConstantMap::NSB_SHADER_TYPE_COUNT);
    return m_akShaderConstantMaps[eType].GetSize();
}

//------------------------------------------------------------------------------------------------
inline const NiSemanticAdapterTable&
    NSBImplementation::GetSemanticAdapterTable() const
{
    return m_kAdapterTable;
}

//------------------------------------------------------------------------------------------------
inline NiSemanticAdapterTable& NSBImplementation::GetSemanticAdapterTable()
{
    return m_kAdapterTable;
}

//------------------------------------------------------------------------------------------------
