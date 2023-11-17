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
#include "NiMainPCH.h"

#include "NiStandardMaterial.h"
#include "NiStandardPixelProgramDescriptor.h"

//--------------------------------------------------------------------------------------------------
NiStandardPixelProgramDescriptor::NiStandardPixelProgramDescriptor() :
    NiGPUProgramDescriptor(
    NiStandardMaterial::PIXEL_PROGRAM_DESCRIPTOR_DWORD_COUNT)
{
}

//--------------------------------------------------------------------------------------------------
unsigned int NiStandardPixelProgramDescriptor::GetUVSetForMap(
    unsigned int uiWhichMap)
{
    switch (uiWhichMap)
    {
    case 0:
        return GetUVSETFORMAP00();
    case 1:
        return GetUVSETFORMAP01();
    case 2:
        return GetUVSETFORMAP02();
    case 3:
        return GetUVSETFORMAP03();
    case 4:
        return GetUVSETFORMAP04();
    case 5:
        return GetUVSETFORMAP05();
    case 6:
        return GetUVSETFORMAP06();
    case 7:
        return GetUVSETFORMAP07();
    case 8:
        return GetUVSETFORMAP08();
    case 9:
        return GetUVSETFORMAP09();
    case 10:
        return GetUVSETFORMAP10();
    case 11:
        return GetUVSETFORMAP11();

    default:
        EE_FAIL("Should never reach here!");
        return UINT_MAX;
        break;
    }

}

//--------------------------------------------------------------------------------------------------
void NiStandardPixelProgramDescriptor::SetUVSetForMap(unsigned int uiWhichMap,
    unsigned int uiUVSet)
{
    switch (uiWhichMap)
    {
    case 0:
        SetUVSETFORMAP00(uiUVSet);
        break;
    case 1:
        SetUVSETFORMAP01(uiUVSet);
        break;
    case 2:
        SetUVSETFORMAP02(uiUVSet);
        break;
    case 3:
        SetUVSETFORMAP03(uiUVSet);
        break;
    case 4:
        SetUVSETFORMAP04(uiUVSet);
        break;
    case 5:
        SetUVSETFORMAP05(uiUVSet);
        break;
    case 6:
        SetUVSETFORMAP06(uiUVSet);
        break;
    case 7:
        SetUVSETFORMAP07(uiUVSet);
        break;
    case 8:
        SetUVSETFORMAP08(uiUVSet);
        break;
    case 9:
        SetUVSETFORMAP09(uiUVSet);
        break;
    case 10:
        SetUVSETFORMAP10(uiUVSet);
        break;
    case 11:
        SetUVSETFORMAP11(uiUVSet);
        break;
    default:
        EE_FAIL("Should never reach here!");
        break;
    }

}

//--------------------------------------------------------------------------------------------------
unsigned int NiStandardPixelProgramDescriptor::GetStandardTextureCount()
{
    unsigned int uiCount =
        GetPARALLAXMAPCOUNT() +
        GetBASEMAPCOUNT() +
        GetNORMALMAPCOUNT() +
        GetDARKMAPCOUNT() +
        GetDETAILMAPCOUNT() +
        GetBUMPMAPCOUNT() +
        GetGLOSSMAPCOUNT() +
        GetGLOWMAPCOUNT() +
        GetCUSTOMMAP00COUNT() +
        GetCUSTOMMAP01COUNT() +
        GetCUSTOMMAP02COUNT() +
        GetCUSTOMMAP03COUNT() +
        GetCUSTOMMAP04COUNT()+
        GetDECALMAPCOUNT();

    return uiCount;
}

//--------------------------------------------------------------------------------------------------
unsigned int NiStandardPixelProgramDescriptor::GetInputUVCount()
{
    int iHighestIndex = -1;
    for (unsigned int ui = 0; ui < GetStandardTextureCount(); ui++)
    {
        int iUVSet = (int)GetUVSetForMap(ui);
        if (iUVSet > iHighestIndex)
            iHighestIndex = iUVSet;
    }

    return iHighestIndex + 1;
}

//--------------------------------------------------------------------------------------------------
NiString NiStandardPixelProgramDescriptor::ToString()
{
    NiString kString;
    ToStringPROJLIGHTMAPTYPES(kString, true);
    ToStringUVSETFORMAP04(kString, true);
    ToStringPOINTLIGHTCOUNT(kString, true);
    ToStringPSSMSLICECOUNT(kString, true);
    ToStringALPHATEST(kString, true);
    return kString;
}

//--------------------------------------------------------------------------------------------------
