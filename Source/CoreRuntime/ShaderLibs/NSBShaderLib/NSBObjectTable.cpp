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
#include "NSBShaderLibPCH.h"

#include "NSBObjectTable.h"

//--------------------------------------------------------------------------------------------------
NSBObjectTable::ObjectDesc::ObjectDesc(const char* pcName,
    NiShaderAttributeDesc::ObjectType eType, unsigned int uiIndex) :
    m_pcName(NULL), m_eType(eType), m_uiIndex(uiIndex)
{
    SetName(pcName);
}

//--------------------------------------------------------------------------------------------------
NSBObjectTable::ObjectDesc::~ObjectDesc()
{
    NiFree(m_pcName);
}

//--------------------------------------------------------------------------------------------------
void NSBObjectTable::ObjectDesc::SetName(const char* pcName)
{
    NiFree(m_pcName);
    m_pcName = NULL;
    if (pcName)
    {
        size_t stBufSize = strlen(pcName) + 1;
        m_pcName = NiAlloc(char, stBufSize);
        NiStrcpy(m_pcName, stBufSize, pcName);
    }
}

//--------------------------------------------------------------------------------------------------
NSBObjectTable::~NSBObjectTable()
{
    // Need to walk the list and NiDelete all refs in it.
    NiTListIterator kIter = m_kObjectList.GetHeadPos();
    while (kIter)
    {
        NiDelete m_kObjectList.GetNext(kIter);
    }
    m_kObjectList.RemoveAll();
}

//--------------------------------------------------------------------------------------------------
bool NSBObjectTable::AddObject(const char* pcName,
    NiShaderAttributeDesc::ObjectType eType, unsigned int uiIndex)
{
    if (GetObjectByName(pcName))
    {
        return false;
    }
    m_kObjectList.AddTail(NiNew ObjectDesc(pcName, eType, uiIndex));
    return true;
}

//--------------------------------------------------------------------------------------------------
NSBObjectTable::ObjectDesc* NSBObjectTable::GetObjectByName(
    const char* pcName)
{
    NiTListIterator kIter = 0;
    ObjectDesc* pkDesc = GetFirstObject(kIter);
    while (pkDesc)
    {
        if (NiStricmp(pkDesc->GetName(), pcName) == 0)
        {
            return pkDesc;
        }
        pkDesc = GetNextObject(kIter);
    }

    return NULL;
}

//--------------------------------------------------------------------------------------------------
