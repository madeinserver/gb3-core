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
// Precompiled Header
#include "NSBShaderLibPCH.h"

#include "NSBUtility.h"
#include "NSBStateGroup.h"
#include "NSBConstantMap.h"

#include <NiSystem.h>

//--------------------------------------------------------------------------------------------------
#if defined(NIDEBUG)
unsigned int NSBUtility::ms_uiIndent = 0;
#endif  //#if defined(NIDEBUG)

//--------------------------------------------------------------------------------------------------
bool NSBUtility::SaveBinaryStateGroup(efd::BinaryStream& kStream,
    NSBStateGroup* pkStateGroup)
{
    unsigned int uiValue;

    if (pkStateGroup)
    {
        uiValue = 1;
        NiStreamSaveBinary(kStream, uiValue);
        if (!pkStateGroup->SaveBinary(kStream))
            return false;
    }
    else
    {
        uiValue = 0;
        NiStreamSaveBinary(kStream, uiValue);
    }

    return true;
}

//--------------------------------------------------------------------------------------------------
bool NSBUtility::LoadBinaryStateGroup(efd::BinaryStream& kStream,
    NSBStateGroup*& pkStateGroup)
{
    unsigned int uiValue;
    NiStreamLoadBinary(kStream, uiValue);

    if (uiValue == 0)
    {
        // No render state group was written
        return true;
    }

    pkStateGroup = NiNew NSBStateGroup();
    EE_ASSERT(pkStateGroup);

    if (!pkStateGroup->LoadBinary(kStream))
    {
        NiDelete pkStateGroup;
        pkStateGroup = 0;
        return false;
    }

    return true;
}

//--------------------------------------------------------------------------------------------------
bool NSBUtility::SaveBinaryConstantMap(efd::BinaryStream& kStream,
    NSBConstantMap* pkMap)
{
    unsigned int uiValue;

    if (pkMap)
    {
        uiValue = 1;
        NiStreamSaveBinary(kStream, uiValue);

        if (!pkMap->SaveBinary(kStream))
            return false;
    }
    else
    {
        uiValue = 0;
        NiStreamSaveBinary(kStream, uiValue);
    }

    return true;
}

//--------------------------------------------------------------------------------------------------
bool NSBUtility::LoadBinaryConstantMap(efd::BinaryStream& kStream,
    NSBConstantMap*& pkMap)
{
    unsigned int uiValue;
    NiStreamLoadBinary(kStream, uiValue);

    if (uiValue == 0)
    {
        // No constant map stored
        pkMap = 0;
        return true;
    }

    pkMap = NiNew NSBConstantMap();
    EE_ASSERT(pkMap);

    if (!pkMap->LoadBinary(kStream))
    {
        NiDelete pkMap;
        pkMap = 0;
        return false;
    }

    return true;
}

//--------------------------------------------------------------------------------------------------
unsigned int NSBUtility::SetString(char*& pcDest, unsigned int uiDestSize,
    const char* pcSrc)
{
    if (pcSrc && pcSrc[0] != '\0')
    {
        size_t stSrcSize = strlen(pcSrc) + 1;
        if (pcDest)
        {
            // See if the length is ok
            if (uiDestSize <= stSrcSize)
            {
                NiFree(pcDest);
                pcDest = 0;
            }
        }

        if (!pcDest)
        {
            pcDest = NiAlloc(char, stSrcSize);
            uiDestSize = (unsigned int)stSrcSize;
        }

        NiStrcpy(pcDest, uiDestSize, pcSrc);
        return uiDestSize;
    }
    else
    {
        NiFree(pcDest);
        pcDest = 0;
        return 0;
    }
}

//--------------------------------------------------------------------------------------------------
bool NSBUtility::WriteData(efd::BinaryStream& kStream, void* pvData,
    unsigned int uiSize, unsigned int uiComponentSize)
{
    NiStreamSaveBinary(kStream, uiSize);

    if (uiSize)
    {
        unsigned int uiWrote =
            kStream.BinaryWrite(pvData, uiSize, &uiComponentSize, 1);
        if (uiWrote != uiSize)
            return false;
    }

    return true;
}

//--------------------------------------------------------------------------------------------------
bool NSBUtility::ReadData(efd::BinaryStream& kStream, void*& pvData,
    unsigned int& uiSize)
{
    unsigned int uiReadSize;
    NiStreamLoadBinary(kStream, uiReadSize);

    if (uiReadSize > uiSize)
    {
        uiSize = uiReadSize;
        return false;
    }

    if (uiSize)
    {
        unsigned int uiRead = kStream.Read(pvData, uiSize);
        if (uiRead != uiSize)
            return false;
    }

    return true;
}

//--------------------------------------------------------------------------------------------------
bool NSBUtility::AllocateAndReadData(efd::BinaryStream& kStream, void*& pvData,
    unsigned int& uiDataSize, unsigned int uiComponentSize)
{
    NiStreamLoadBinary(kStream, uiDataSize);

    if (uiDataSize)
    {
        pvData = NiAlloc(unsigned char, uiDataSize);
        EE_ASSERT(pvData);

        unsigned int uiRead =
            kStream.BinaryRead(pvData, uiDataSize, &uiComponentSize, 1);

        if (uiRead != uiDataSize)
            return false;
    }
    else
    {
        pvData = NULL;
    }

    return true;
}

//--------------------------------------------------------------------------------------------------
#if defined(NIDEBUG)

//--------------------------------------------------------------------------------------------------
void NSBUtility::IndentInsert()
{
    ms_uiIndent += 4;
}

//--------------------------------------------------------------------------------------------------
void NSBUtility::IndentRemove()
{
    ms_uiIndent -= 4;
}

//--------------------------------------------------------------------------------------------------
void NSBUtility::Dump(FILE* pf, bool bIndent, const char* pcFmt, ...)
{
    char acTemp[1024];
    char acMessage[1024];

    if (bIndent && (ms_uiIndent != 0))
    {
        acMessage[0] = ' ';
        acMessage[1] = 0;
        for (unsigned int ui = 1; ui < ms_uiIndent; ui++)
            NiStrcat(acMessage, 1024, " ");
    }
    else
    {
        acMessage[0] = 0;
    }

    va_list args;
    va_start(args, pcFmt);
    NiVsprintf(acTemp, 1024, pcFmt, args);
    va_end(args);

    NiStrcat(acMessage, 1024, acTemp);
    if (pf)
        fputs(acMessage, pf);
    else
        NiOutputDebugString(acMessage);
}

//--------------------------------------------------------------------------------------------------

#endif  //#if defined(NIDEBUG)
