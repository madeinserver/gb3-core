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

#include "NSBUtility.h"
#include "NSBAttributeDesc.h"
#include <NiStream.h>

//--------------------------------------------------------------------------------------------------
NSBAttributeDesc::NSBAttributeDesc() :
    m_pcName(NULL),
    m_pcDesc(NULL),
    m_eType(NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED),
    m_uiFlags(0),
    m_pcValue(NULL),
    m_uiLen(0),
    m_uiLow(0),
    m_uiHigh(0),
    m_pcDefault(NULL),
    m_uiDefaultLen(0)
{
}

//--------------------------------------------------------------------------------------------------
NSBAttributeDesc::~NSBAttributeDesc()
{
    NiFree(m_pcName);
    NiFree(m_pcDesc);

    if (m_eType == NiShaderAttributeDesc::ATTRIB_TYPE_STRING)
    {
        NiFree(m_pcValue);
    }
    else if (m_eType == NiShaderAttributeDesc::ATTRIB_TYPE_ARRAY)
    {
        NiFree(m_kArrayValue.m_pvValue);
        m_kArrayValue.m_pvValue = NULL;
        NiFree(m_pvHigh);
        NiFree(m_pvLow);
    }
    NiFree(m_pcDefault);
}

//--------------------------------------------------------------------------------------------------
NiShaderAttributeDesc* NSBAttributeDesc::GetShaderAttributeDesc()
{
    NiShaderAttributeDesc* pkDesc = NiNew NiShaderAttributeDesc;
    pkDesc->SetName(m_pcName);
    pkDesc->SetDescription(m_pcDesc);

    pkDesc->SetFlags(m_uiFlags);
    pkDesc->SetType(m_eType);

    switch (m_eType)
    {
    case NiShaderAttributeDesc::ATTRIB_TYPE_BOOL:
        pkDesc->SetValue_Bool(m_bValue);
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_STRING:
        pkDesc->SetValue_String(m_pcValue);
        if (IsRanged())
        {
            pkDesc->SetRange_UnsignedInt(m_uiLow, m_uiHigh);
        }
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT:
        pkDesc->SetValue_UnsignedInt(m_uiValue);
        if (IsRanged())
        {
            pkDesc->SetRange_UnsignedInt(m_uiLow, m_uiHigh);
        }
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT:
        pkDesc->SetValue_Float(m_aafValue[0][0]);
        if (IsRanged())
        {
            pkDesc->SetRange_Float(m_aafLow[0][0], m_aafHigh[0][0]);
        }
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_POINT2:
    {
        NiPoint2 kPoint(m_aafValue[0][0], m_aafValue[0][1]);
        pkDesc->SetValue_Point2(kPoint);
        if (IsRanged())
        {
            NiPoint2 kLow(m_aafLow[0][0], m_aafLow[0][1]);
            NiPoint2 kHigh(m_aafHigh[0][0], m_aafHigh[0][1]);
            pkDesc->SetRange_Point2(kLow, kHigh);
        }
        break;
    }
    case NiShaderAttributeDesc::ATTRIB_TYPE_POINT3:
    {
        NiPoint3 kPoint(m_aafValue[0][0], m_aafValue[0][1], m_aafValue[0][2]);
        pkDesc->SetValue_Point3(kPoint);
        if (IsRanged())
        {
            NiPoint3 kLow(m_aafLow[0][0], m_aafLow[0][1], m_aafLow[0][2]);
            NiPoint3 kHigh(m_aafHigh[0][0], m_aafHigh[0][1], m_aafHigh[0][2]);
            pkDesc->SetRange_Point3(kLow, kHigh);
        }
        break;
    }
    case NiShaderAttributeDesc::ATTRIB_TYPE_POINT4:
        pkDesc->SetValue_Point4(m_aafValue[0]);
        if (IsRanged())
        {
            pkDesc->SetRange_Point4(m_aafLow[0], m_aafHigh[0]);
        }
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3:
    {
        NiMatrix3 kMatrix;
        kMatrix.SetEntry(0, 0, m_aafValue[0][0]);
        kMatrix.SetEntry(0, 1, m_aafValue[0][1]);
        kMatrix.SetEntry(0, 2, m_aafValue[0][2]);
        kMatrix.SetEntry(1, 0, m_aafValue[1][0]);
        kMatrix.SetEntry(1, 1, m_aafValue[1][1]);
        kMatrix.SetEntry(1, 2, m_aafValue[1][2]);
        kMatrix.SetEntry(2, 0, m_aafValue[2][0]);
        kMatrix.SetEntry(2, 1, m_aafValue[2][1]);
        kMatrix.SetEntry(2, 2, m_aafValue[2][2]);
        pkDesc->SetValue_Matrix3(kMatrix);
        break;
    }
    case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4:
    {
        float afMatrix[16];
        unsigned int uiDestSize = 16 * sizeof(float); //float [4][4];
        NiMemcpy((void*)m_aafValue, (const void*)afMatrix, uiDestSize);
        pkDesc->SetValue_Matrix4(afMatrix);
        break;
    }
    case NiShaderAttributeDesc::ATTRIB_TYPE_COLOR:
    {
        NiColorA kColor(m_aafValue[0][0], m_aafValue[0][1], m_aafValue[0][2],
            m_aafValue[0][3]);
        pkDesc->SetValue_ColorA(kColor);
        if (IsRanged())
        {
            pkDesc->SetRanged(true);
            NiColorA kLow(m_aafLow[0][0], m_aafLow[0][1], m_aafLow[0][2],
                m_aafLow[0][3]);
            NiColorA kHigh(m_aafHigh[0][0], m_aafHigh[0][1], m_aafHigh[0][2],
                m_aafHigh[0][3]);
            pkDesc->SetRange_ColorA(kLow, kHigh);
        }
        break;
    }
    case NiShaderAttributeDesc::ATTRIB_TYPE_TEXTURE:
        pkDesc->SetValue_Texture(m_uiValue, m_pcDefault);
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_ARRAY:
        pkDesc->SetValue_Array(m_kArrayValue.m_pvValue, m_kArrayValue.m_eType,
            m_kArrayValue.m_uiElementSize, m_uiLen);
        if (IsRanged())
        {
            pkDesc->SetRanged(true);
            pkDesc->SetRange_Array(m_kArrayValue.m_uiElementSize,
                m_uiLen, m_pvLow, m_pvHigh);
        }
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED:
    case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT8:
    case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT12:
    default:
        // Error - not currently supported
        NiDelete pkDesc;
        return NULL;
    }

    return pkDesc;
}

//--------------------------------------------------------------------------------------------------
const char* NSBAttributeDesc::GetName() const
{
    return m_pcName;
}

//--------------------------------------------------------------------------------------------------
const char* NSBAttributeDesc::GetDescription() const
{
    return m_pcDesc;
}

//--------------------------------------------------------------------------------------------------
void NSBAttributeDesc::SetName(const char* pcName)
{
    NiFree(m_pcName);
    m_pcName = 0;

    if (pcName && pcName[0] != '\0')
    {
        size_t stLen = strlen(pcName) + 1;
        m_pcName = NiAlloc(char, stLen);
        EE_ASSERT(m_pcName);
        NiStrcpy(m_pcName, stLen, pcName);
    }
}

//--------------------------------------------------------------------------------------------------
void NSBAttributeDesc::SetDescription(const char* pcDesc)
{
    NiFree(m_pcDesc);
    m_pcDesc = 0;

    if (pcDesc && pcDesc[0] != '\0')
    {
        size_t stLen = strlen(pcDesc) + 1;
        m_pcDesc = NiAlloc(char, stLen);
        EE_ASSERT(m_pcDesc);
        NiStrcpy(m_pcDesc, stLen, pcDesc);
    }
}

//--------------------------------------------------------------------------------------------------
NiShaderAttributeDesc::AttributeType NSBAttributeDesc::GetType() const
{
    return m_eType;
}

//--------------------------------------------------------------------------------------------------
unsigned int NSBAttributeDesc::GetFlags() const
{
    return m_uiFlags;
}

//--------------------------------------------------------------------------------------------------
bool NSBAttributeDesc::GetValue_Bool(bool& bValue) const
{
    if (m_eType != NiShaderAttributeDesc::ATTRIB_TYPE_BOOL)
        return false;

    bValue = m_bValue;
    return true;
}

//--------------------------------------------------------------------------------------------------
bool NSBAttributeDesc::GetValue_String(const char*& pcValue) const
{
    if (m_eType != NiShaderAttributeDesc::ATTRIB_TYPE_STRING)
        return false;

    pcValue = m_pcValue;
    return true;
}

//--------------------------------------------------------------------------------------------------
bool NSBAttributeDesc::GetValue_Texture(unsigned int& uiValue,
    const char*& pcValue) const
{
    if (m_eType != NiShaderAttributeDesc::ATTRIB_TYPE_TEXTURE)
        return false;

    pcValue = m_pcDefault;
    uiValue = m_uiValue;

    return true;
}

//--------------------------------------------------------------------------------------------------
bool NSBAttributeDesc::GetValue_UnsignedInt(unsigned int& uiValue) const
{
    if (m_eType != NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT)
        return false;

    uiValue = m_uiValue;
    return true;
}

//--------------------------------------------------------------------------------------------------
bool NSBAttributeDesc::GetValue_Float(float& fValue) const
{
    if (m_eType != NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT)
        return false;

    fValue = m_aafValue[0][0];
    return true;
}

//--------------------------------------------------------------------------------------------------
bool NSBAttributeDesc::GetValue_Point2(NiPoint2& kPt2Value) const
{
    if (m_eType != NiShaderAttributeDesc::ATTRIB_TYPE_POINT2)
        return false;

    kPt2Value.x = m_aafValue[0][0];
    kPt2Value.y = m_aafValue[0][1];
    return true;
}

//--------------------------------------------------------------------------------------------------
bool NSBAttributeDesc::GetValue_Point3(NiPoint3& kPtValue) const
{
    if (m_eType != NiShaderAttributeDesc::ATTRIB_TYPE_POINT3)
        return false;

    kPtValue.x = m_aafValue[0][0];
    kPtValue.y = m_aafValue[0][1];
    kPtValue.z = m_aafValue[0][2];
    return true;
}

//--------------------------------------------------------------------------------------------------
bool NSBAttributeDesc::GetValue_Point4(float*& pfValue) const
{
    if (m_eType != NiShaderAttributeDesc::ATTRIB_TYPE_POINT4)
        return false;

    pfValue[0] = m_aafValue[0][0];
    pfValue[1] = m_aafValue[0][1];
    pfValue[2] = m_aafValue[0][2];
    pfValue[3] = m_aafValue[0][3];
    return true;
}

//--------------------------------------------------------------------------------------------------
bool NSBAttributeDesc::GetValue_Matrix3(NiMatrix3& kMatValue) const
{
    if (m_eType != NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3)
        return false;

    kMatValue.SetRow(0, m_aafValue[0]);
    kMatValue.SetRow(1, m_aafValue[1]);
    kMatValue.SetRow(2, m_aafValue[2]);
    return true;
}

//--------------------------------------------------------------------------------------------------
bool NSBAttributeDesc::GetValue_Matrix4(float*& pfValue,
    unsigned int uiSizeBytes)
    const
{
    EE_ASSERT(uiSizeBytes == (sizeof(float) * 16));

    if (m_eType != NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4)
        return false;

    NiMemcpy((void*)pfValue, uiSizeBytes, (const void*)m_aafValue,
        sizeof(float) * 16);

    return true;
}

//--------------------------------------------------------------------------------------------------
bool NSBAttributeDesc::GetValue_Color(NiColor& kClrValue) const
{
    if (m_eType != NiShaderAttributeDesc::ATTRIB_TYPE_COLOR)
        return false;

    kClrValue.r = m_aafValue[0][0];
    kClrValue.g = m_aafValue[0][1];
    kClrValue.b = m_aafValue[0][2];

    return true;
}

//--------------------------------------------------------------------------------------------------
bool NSBAttributeDesc::GetValue_ColorA(NiColorA& kClrValue) const
{
    if (m_eType != NiShaderAttributeDesc::ATTRIB_TYPE_COLOR)
        return false;

    kClrValue.r = m_aafValue[0][0];
    kClrValue.g = m_aafValue[0][1];
    kClrValue.b = m_aafValue[0][2];
    kClrValue.a = m_aafValue[0][3];

    return true;
}

//--------------------------------------------------------------------------------------------------
 bool NSBAttributeDesc::GetValue_Array(void* pvValue,
     unsigned int uiValueBufferSize) const
{
    if (m_eType != NiShaderAttributeDesc::ATTRIB_TYPE_ARRAY)
        return false;

    if (uiValueBufferSize < m_kArrayValue.m_uiElementSize * m_uiLen)
        return false;

    if (!m_kArrayValue.m_pvValue)
        return false;

    NiMemcpy(pvValue, uiValueBufferSize, m_kArrayValue.m_pvValue,
        m_kArrayValue.m_uiElementSize * m_uiLen);

    return true;
}

//--------------------------------------------------------------------------------------------------
bool NSBAttributeDesc::GetArrayParams(
    NiShaderAttributeDesc::AttributeType& eSubType,
    unsigned int& uiElementSize, unsigned int& uiNumElements) const
{
    if (m_eType != NiShaderAttributeDesc::ATTRIB_TYPE_ARRAY)
        return false;

    eSubType = m_kArrayValue.m_eType;
    uiElementSize = m_kArrayValue.m_uiElementSize;
    uiNumElements = m_uiLen;

    return true;
}

//--------------------------------------------------------------------------------------------------
bool NSBAttributeDesc::IsRanged() const
{
    if (m_uiFlags & NiShaderAttributeDesc::ATTRIB_FLAGS_RANGED)
        return true;

    return false;
}

//--------------------------------------------------------------------------------------------------
bool NSBAttributeDesc::IsHidden() const
{
    if ((m_uiFlags & NiShaderAttributeDesc::ATTRIB_FLAGS_HIDDEN) == 0)
        return false;

    return true;
}

//--------------------------------------------------------------------------------------------------
bool NSBAttributeDesc::GetRange_UnsignedInt(unsigned int& uiLow,
    unsigned int& uiHigh) const
{
    if (!IsRanged() ||
        ((m_eType != NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT) &&
        (m_eType != NiShaderAttributeDesc::ATTRIB_TYPE_STRING)))
    {
        return false;
    }

    uiLow = m_uiLow;
    uiHigh = m_uiHigh;

    return true;
}

//--------------------------------------------------------------------------------------------------
bool NSBAttributeDesc::GetRange_Float(float& fLow, float& fHigh) const
{
    if (m_eType != NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT)
        return false;

    fLow = m_aafLow[0][0];
    fHigh = m_aafHigh[0][0];

    return true;
}

//--------------------------------------------------------------------------------------------------
bool NSBAttributeDesc::GetRange_Point2(NiPoint2& kPt2Low,
    NiPoint2& kPt2High) const
{
    if (m_eType != NiShaderAttributeDesc::ATTRIB_TYPE_POINT2)
        return false;

    kPt2Low.x = m_aafLow[0][0];
    kPt2Low.y = m_aafLow[0][1];
    kPt2High.x = m_aafHigh[0][0];
    kPt2High.y = m_aafHigh[0][1];

    return true;
}

//--------------------------------------------------------------------------------------------------
bool NSBAttributeDesc::GetRange_Point3(NiPoint3& kPtLow,
    NiPoint3& kPtHigh) const
{
    if (m_eType != NiShaderAttributeDesc::ATTRIB_TYPE_POINT3)
        return false;

    kPtLow.x = m_aafLow[0][0];
    kPtLow.y = m_aafLow[0][1];
    kPtLow.z = m_aafLow[0][2];
    kPtHigh.x = m_aafHigh[0][0];
    kPtHigh.y = m_aafHigh[0][1];
    kPtHigh.z = m_aafHigh[0][2];

    return true;
}

//--------------------------------------------------------------------------------------------------
bool NSBAttributeDesc::GetRange_Floats(unsigned int uiCount,
    float* pfLow, float* pfHigh) const
{
    switch (m_eType)
    {
        case NiShaderAttributeDesc::ATTRIB_TYPE_POINT4:
            if (uiCount != 4)
                return false;
            pfLow[0] = m_aafLow[0][0];
            pfLow[1] = m_aafLow[0][1];
            pfLow[2] = m_aafLow[0][2];
            pfLow[3] = m_aafLow[0][3];
            pfHigh[0] = m_aafHigh[0][0];
            pfHigh[1] = m_aafHigh[0][1];
            pfHigh[2] = m_aafHigh[0][2];
            pfHigh[3] = m_aafHigh[0][3];
            break;
        default:
            return false;
    }
    return true;
}

//--------------------------------------------------------------------------------------------------
bool NSBAttributeDesc::GetRange_Color(NiColor& kClrLow,
    NiColor& kClrHigh)  const
{
    if (m_eType != NiShaderAttributeDesc::ATTRIB_TYPE_COLOR)
        return false;

    kClrLow.r = m_aafLow[0][0];
    kClrLow.g = m_aafLow[0][1];
    kClrLow.b = m_aafLow[0][2];

    kClrHigh.r = m_aafHigh[0][0];
    kClrHigh.g = m_aafHigh[0][1];
    kClrHigh.b = m_aafHigh[0][2];

    return true;
}

//--------------------------------------------------------------------------------------------------
bool NSBAttributeDesc::GetRange_ColorA(NiColorA& kClrLow,
    NiColorA& kClrHigh) const
{
    if (m_eType != NiShaderAttributeDesc::ATTRIB_TYPE_COLOR)
        return false;

    kClrLow.r = m_aafLow[0][0];
    kClrLow.g = m_aafLow[0][1];
    kClrLow.b = m_aafLow[0][2];
    kClrLow.a = m_aafLow[0][3];

    kClrHigh.r = m_aafHigh[0][0];
    kClrHigh.g = m_aafHigh[0][1];
    kClrHigh.b = m_aafHigh[0][2];
    kClrHigh.a = m_aafHigh[0][3];

    return true;
}

//--------------------------------------------------------------------------------------------------
bool NSBAttributeDesc::GetRange_Array(void*& pvLow, void*& pvHigh,
    unsigned int uiBufferSize) const
{
    if (m_eType != NiShaderAttributeDesc::ATTRIB_TYPE_ARRAY)
        return false;

    if (uiBufferSize < m_kArrayValue.m_uiElementSize * m_uiLen)
        return false;

    if (!m_pvLow || !m_pvHigh)
        return false;

    unsigned int uiDataSize = m_kArrayValue.m_uiElementSize *
        m_uiLen;
    NiMemcpy(pvLow, m_pvLow, uiDataSize);
    NiMemcpy(pvHigh, m_pvHigh, uiDataSize);
    return true;
}

//--------------------------------------------------------------------------------------------------
void NSBAttributeDesc::SetType(
    NiShaderAttributeDesc::AttributeType eType)
{
    if (m_eType != eType)
    {
        if (m_eType == NiShaderAttributeDesc::ATTRIB_TYPE_STRING)
        {
            NiFree(m_pcValue);
            m_uiLen = NULL;
        }
        else if (m_eType == NiShaderAttributeDesc::ATTRIB_TYPE_ARRAY)
        {
            NiFree(m_kArrayValue.m_pvValue);
            m_kArrayValue.m_pvValue = NULL;
            NiFree(m_pvHigh);
            NiFree(m_pvLow);
            m_uiLen = NULL;
        }

        m_eType = eType;
        if (eType == NiShaderAttributeDesc::ATTRIB_TYPE_STRING)
        {
            m_pcValue = NULL;
            m_uiLen = NULL;
        }
        else if (eType == NiShaderAttributeDesc::ATTRIB_TYPE_ARRAY)
        {
            m_kArrayValue.m_pvValue = NULL;
            m_pvHigh = NULL;
            m_pvLow = NULL;
            m_uiLen = NULL;
        }
    }
}

//--------------------------------------------------------------------------------------------------
void NSBAttributeDesc::SetFlags(unsigned int uiFlags)
{
    m_uiFlags = uiFlags;
}

//--------------------------------------------------------------------------------------------------
void NSBAttributeDesc::SetValue_Bool(bool bValue)
{
    SetType(NiShaderAttributeDesc::ATTRIB_TYPE_BOOL);
    m_bValue = bValue;
}

//--------------------------------------------------------------------------------------------------
void NSBAttributeDesc::SetValue_String(const char* pcValue)
{
    SetType(NiShaderAttributeDesc::ATTRIB_TYPE_STRING);

    if (pcValue && pcValue[0] != '\0')
    {
        // See if the length is OK
        if (m_pcValue && (m_uiLen <= strlen(pcValue)))
        {
            NiFree(m_pcValue);
            m_pcValue = NULL;
            m_uiLen = 0;
        }

        if (!m_pcValue)
        {
            m_uiLen = (unsigned int)strlen(pcValue) + 1;
            m_pcValue = NiAlloc(char, m_uiLen);
        }

        NiStrcpy(m_pcValue, m_uiLen, pcValue);
    }
    else
    {
        NiFree(m_pcValue);
        m_pcValue = NULL;
        m_uiLen = 0;
    }
}

//--------------------------------------------------------------------------------------------------
void NSBAttributeDesc::SetValue_UnsignedInt(unsigned int uiValue)
{
    SetType(NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT);
    m_uiValue = uiValue;
}

//--------------------------------------------------------------------------------------------------
void NSBAttributeDesc::SetValue_Float(float fValue)
{
    SetType(NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT);
    m_aafValue[0][0] = fValue;
}

//--------------------------------------------------------------------------------------------------
void NSBAttributeDesc::SetValue_Point2(NiPoint2& kPt2Value)
{
    SetType(NiShaderAttributeDesc::ATTRIB_TYPE_POINT2);
    m_aafValue[0][0] = kPt2Value.x;
    m_aafValue[0][1] = kPt2Value.y;
}

//--------------------------------------------------------------------------------------------------
void NSBAttributeDesc::SetValue_Point3(NiPoint3& kPtValue)
{
    SetType(NiShaderAttributeDesc::ATTRIB_TYPE_POINT3);
    m_aafValue[0][0] = kPtValue.x;
    m_aafValue[0][1] = kPtValue.y;
    m_aafValue[0][2] = kPtValue.z;
}

//--------------------------------------------------------------------------------------------------
void NSBAttributeDesc::SetValue_Point4(float* pfValue)
{
    EE_ASSERT(pfValue);

    // Assumes that there are 4 floats!!!
    SetType(NiShaderAttributeDesc::ATTRIB_TYPE_POINT4);
    unsigned int uiDestSize = 16 * sizeof(float); //float [4][4];
    NiMemcpy((void*)m_aafValue, uiDestSize, (const void*)pfValue,
        sizeof(float) * 4);
}

//--------------------------------------------------------------------------------------------------
void NSBAttributeDesc::SetValue_Matrix3(NiMatrix3& kMatValue)
{
    SetType(NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3);
    m_aafValue[0][0] = kMatValue.GetEntry(0,0);
    m_aafValue[0][1] = kMatValue.GetEntry(0,1);
    m_aafValue[0][2] = kMatValue.GetEntry(0,2);
    m_aafValue[1][0] = kMatValue.GetEntry(1,0);
    m_aafValue[1][1] = kMatValue.GetEntry(1,1);
    m_aafValue[1][2] = kMatValue.GetEntry(1,2);
    m_aafValue[2][0] = kMatValue.GetEntry(2,0);
    m_aafValue[2][1] = kMatValue.GetEntry(2,1);
    m_aafValue[2][2] = kMatValue.GetEntry(2,2);
}

//--------------------------------------------------------------------------------------------------
void NSBAttributeDesc::SetValue_Matrix4(float* pfValue)
{
    EE_ASSERT(pfValue);

    // Assumes that there are 16 floats!!!
    SetType(NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4);
    unsigned int uiDestSize = 16 * sizeof(float); //float [4][4];
    NiMemcpy((void*)m_aafValue, (const void*)pfValue, uiDestSize);
}

//--------------------------------------------------------------------------------------------------
void NSBAttributeDesc::SetValue_Color(NiColor& kClrValue)
{
    SetType(NiShaderAttributeDesc::ATTRIB_TYPE_COLOR);
    m_aafValue[0][0] = kClrValue.r;
    m_aafValue[0][1] = kClrValue.g;
    m_aafValue[0][2] = kClrValue.b;
    m_aafValue[0][3] = 1.0f;
}

//--------------------------------------------------------------------------------------------------
void NSBAttributeDesc::SetValue_ColorA(NiColorA& kClrValue)
{
    SetType(NiShaderAttributeDesc::ATTRIB_TYPE_COLOR);
    m_aafValue[0][0] = kClrValue.r;
    m_aafValue[0][1] = kClrValue.g;
    m_aafValue[0][2] = kClrValue.b;
    m_aafValue[0][3] = kClrValue.a;
}

//--------------------------------------------------------------------------------------------------
void NSBAttributeDesc::SetValue_Texture(unsigned int uiValue,
    const char* pcValue)
{
    SetType(NiShaderAttributeDesc::ATTRIB_TYPE_TEXTURE);

    if (pcValue && pcValue[0] != '\0')
    {
        // See if the length is OK
        if (m_pcDefault && (m_uiDefaultLen <= strlen(pcValue)))
        {
            NiFree(m_pcDefault);
            m_pcDefault = NULL;
            m_uiDefaultLen = 0;
        }

        if (!m_pcDefault)
        {
            m_uiDefaultLen = (unsigned int)strlen(pcValue) + 1;
            m_pcDefault = NiAlloc(char, m_uiDefaultLen);
        }

        NiStrcpy(m_pcDefault, m_uiDefaultLen, pcValue);
    }
    else
    {
        NiFree(m_pcDefault);
        m_pcDefault = NULL;
        m_uiDefaultLen = 0;
    }

    m_uiValue = uiValue;
}

//--------------------------------------------------------------------------------------------------
void NSBAttributeDesc::SetValue_Array(void* pValue,
    NiShaderAttributeDesc::AttributeType eSubType, unsigned int uiElementSize,
    unsigned int uiNumElements)
{
    EE_ASSERT(pValue != NULL && uiNumElements != 0);
    EE_ASSERT(uiElementSize == sizeof(float) ||
           uiElementSize == 2*sizeof(float) ||
           uiElementSize == 3*sizeof(float) ||
           uiElementSize == 4*sizeof(float));
    EE_ASSERT(eSubType == NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT ||
           eSubType == NiShaderAttributeDesc::ATTRIB_TYPE_POINT2 ||
           eSubType == NiShaderAttributeDesc::ATTRIB_TYPE_POINT3 ||
           eSubType == NiShaderAttributeDesc::ATTRIB_TYPE_POINT4 ||
           eSubType == NiShaderAttributeDesc::ATTRIB_TYPE_COLOR);

    SetType(NiShaderAttributeDesc::ATTRIB_TYPE_ARRAY);
    unsigned int uiNewSize = uiNumElements * uiElementSize;
    if (m_kArrayValue.m_pvValue != NULL)
    {
        NiFree(m_kArrayValue.m_pvValue);
    }

    m_kArrayValue.m_pvValue = NiAlloc(char, uiNewSize);
    unsigned int uiDestSize = uiNewSize * sizeof(char);
    NiMemcpy(m_kArrayValue.m_pvValue, pValue, uiDestSize);

    m_kArrayValue.m_eType = eSubType;
    m_kArrayValue.m_uiElementSize = uiElementSize;
    m_uiLen = uiNumElements;
}

//--------------------------------------------------------------------------------------------------
void NSBAttributeDesc::SetHidden(bool bHide)
{
    if (bHide)
        m_uiFlags |= NiShaderAttributeDesc::ATTRIB_FLAGS_HIDDEN;
    else
        m_uiFlags &= ~NiShaderAttributeDesc::ATTRIB_FLAGS_HIDDEN;
}

//--------------------------------------------------------------------------------------------------
void NSBAttributeDesc::SetRanged(bool bRanged)
{
    if (bRanged)
        m_uiFlags |= NiShaderAttributeDesc::ATTRIB_FLAGS_RANGED;
    else
        m_uiFlags &= ~NiShaderAttributeDesc::ATTRIB_FLAGS_RANGED;
}

//--------------------------------------------------------------------------------------------------
bool NSBAttributeDesc::SetRange_UnsignedInt(unsigned int uiLow,
    unsigned int uiHigh)
{
    if ((m_eType != NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT) &&
        (m_eType != NiShaderAttributeDesc::ATTRIB_TYPE_STRING))
    {
        return false;
    }

    m_uiLow = uiLow;
    m_uiHigh = uiHigh;

    return true;
}

//--------------------------------------------------------------------------------------------------
bool NSBAttributeDesc::SetRange_Float(float fLow, float fHigh)
{
    if (m_eType != NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT)
        return false;

    m_aafLow[0][0] = fLow;
    m_aafHigh[0][0] = fHigh;

    return true;
}

//--------------------------------------------------------------------------------------------------
bool NSBAttributeDesc::SetRange_Point2(NiPoint2& kPt2Low,
    NiPoint2& kPt2High)
{
    if (m_eType != NiShaderAttributeDesc::ATTRIB_TYPE_POINT2)
        return false;

    m_aafLow[0][0] = kPt2Low.x;
    m_aafLow[0][1] = kPt2Low.y;
    m_aafHigh[0][0] = kPt2High.x;
    m_aafHigh[0][1] = kPt2High.y;

    return true;
}

//--------------------------------------------------------------------------------------------------
bool NSBAttributeDesc::SetRange_Point3(NiPoint3& kPtLow,
    NiPoint3& kPtHigh)
{
    if (m_eType != NiShaderAttributeDesc::ATTRIB_TYPE_POINT3)
        return false;

    m_aafLow[0][0] = kPtLow.x;
    m_aafLow[0][1] = kPtLow.y;
    m_aafLow[0][2] = kPtLow.z;
    m_aafHigh[0][0] = kPtHigh.x;
    m_aafHigh[0][1] = kPtHigh.y;
    m_aafHigh[0][2] = kPtHigh.z;

    return true;
}

//--------------------------------------------------------------------------------------------------
bool NSBAttributeDesc::SetRange_Point4(float* pfLow, float* pfHigh)
{
    if (m_eType != NiShaderAttributeDesc::ATTRIB_TYPE_POINT4)
        return false;

    m_aafLow[0][0] = pfLow[0];
    m_aafLow[0][1] = pfLow[1];
    m_aafLow[0][2] = pfLow[2];
    m_aafLow[0][3] = pfLow[3];
    m_aafHigh[0][0] = pfHigh[0];
    m_aafHigh[0][1] = pfHigh[1];
    m_aafHigh[0][2] = pfHigh[2];
    m_aafHigh[0][3] = pfHigh[3];

    return true;
}

//--------------------------------------------------------------------------------------------------
bool NSBAttributeDesc::SetRange_Floats(unsigned int uiCount,
    float* pfLow, float* pfHigh)
{
    switch (m_eType)
    {
        case NiShaderAttributeDesc::ATTRIB_TYPE_POINT4:
            if (uiCount != 4)
                return false;
            m_aafLow[0][0] = pfLow[0];
            m_aafLow[0][1] = pfLow[1];
            m_aafLow[0][2] = pfLow[2];
            m_aafLow[0][3] = pfLow[3];
            m_aafHigh[0][0] = pfHigh[0];
            m_aafHigh[0][1] = pfHigh[1];
            m_aafHigh[0][2] = pfHigh[2];
            m_aafHigh[0][3] = pfHigh[3];
            break;
        default:
            return false;
    }
    return true;
}

//--------------------------------------------------------------------------------------------------
bool NSBAttributeDesc::SetRange_Color(NiColor& kClrLow,
    NiColor& kClrHigh)
{
    if (m_eType != NiShaderAttributeDesc::ATTRIB_TYPE_COLOR)
        return false;

    m_aafLow[0][0] = kClrLow.r;
    m_aafLow[0][1] = kClrLow.g;
    m_aafLow[0][2] = kClrLow.b;
    m_aafLow[0][3] = 1.0f;
    m_aafHigh[0][0] = kClrHigh.r;
    m_aafHigh[0][1] = kClrHigh.g;
    m_aafHigh[0][2] = kClrHigh.b;
    m_aafHigh[0][3] = 1.0f;

    return true;
}

//--------------------------------------------------------------------------------------------------
bool NSBAttributeDesc::SetRange_ColorA(NiColorA& kClrLow,
    NiColorA& kClrHigh)
{
    if (m_eType != NiShaderAttributeDesc::ATTRIB_TYPE_COLOR)
        return false;

    m_aafLow[0][0] = kClrLow.r;
    m_aafLow[0][1] = kClrLow.g;
    m_aafLow[0][2] = kClrLow.b;
    m_aafLow[0][3] = kClrLow.a;
    m_aafHigh[0][0] = kClrHigh.r;
    m_aafHigh[0][1] = kClrHigh.g;
    m_aafHigh[0][2] = kClrHigh.b;
    m_aafHigh[0][3] = kClrHigh.a;

    return true;
}

//--------------------------------------------------------------------------------------------------
bool NSBAttributeDesc::SetRange_Array(unsigned int uiElementSize,
    unsigned int uiNumElements, void* pLow, void* pHigh)
{
    if (m_eType != NiShaderAttributeDesc::ATTRIB_TYPE_ARRAY)
        return false;

    if (uiElementSize != m_kArrayValue.m_uiElementSize ||
        uiNumElements != m_uiLen)
    {
        return false;
    }

    unsigned int uiNewSize = uiNumElements * uiElementSize;
    if (m_pvLow != NULL || m_pvHigh != NULL)
    {
        if (uiNewSize > (m_kArrayValue.m_uiElementSize * m_uiLen))
        {
            NiFree(m_pvLow);
            NiFree(m_pvHigh);
            m_pvLow = 0;
            m_pvHigh = 0;
        }
    }

    if (pLow != NULL && pHigh != NULL)
    {
        m_pvLow = NiAlloc(float, uiNewSize);
        m_pvHigh = NiAlloc(float, uiNewSize);

        unsigned int uiDestSize = uiNewSize * sizeof(float);

        NiMemcpy(m_pvLow, pLow, uiDestSize);
        NiMemcpy(m_pvHigh, pHigh, uiDestSize);
    }
    else
    {
        m_pvLow = NULL;
        m_pvHigh = NULL;
    }
    return true;
}

//--------------------------------------------------------------------------------------------------
bool NSBAttributeDesc::SaveBinary(efd::BinaryStream& kStream)
{
    kStream.WriteCString(m_pcName);
    kStream.WriteCString(m_pcDesc);

    NiStreamSaveBinary(kStream, m_uiFlags);

    // NiStreamSaveEnum doesn't exist for efd::BinaryStream.
    unsigned int uiType = (unsigned int)m_eType;
    NiStreamSaveBinary(kStream, uiType);

    switch (m_eType)
    {
    case NiShaderAttributeDesc::ATTRIB_TYPE_BOOL:
        if (!SaveBinary_Bool(kStream))
            return false;
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_STRING:
        if (!SaveBinary_String(kStream))
            return false;
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT:
        if (!SaveBinary_UnsignedInt(kStream))
            return false;
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT:
        if (!SaveBinary_Float(kStream))
            return false;
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_POINT2:
        if (!SaveBinary_Point2(kStream))
            return false;
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_POINT3:
        if (!SaveBinary_Point3(kStream))
            return false;
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_POINT4:
        if (!SaveBinary_Point4(kStream))
            return false;
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3:
        if (!SaveBinary_Matrix3(kStream))
            return false;
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4:
        if (!SaveBinary_Matrix4(kStream))
            return false;
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_COLOR:
        if (!SaveBinary_Color(kStream))
            return false;
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_TEXTURE:
        if (!SaveBinary_Texture(kStream))
            return false;
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_ARRAY:
        if (!SaveBinary_Array(kStream))
            return false;
        break;

    case NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED:
    case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT8:
    case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT12:
    case NiShaderAttributeDesc::ATTRIB_TYPE_COUNT:
            // do nothing
        break;
    }

    return true;
}

//--------------------------------------------------------------------------------------------------
bool NSBAttributeDesc::LoadBinary(efd::BinaryStream& kStream)
{
    m_pcName = kStream.ReadCString();
    m_pcDesc = kStream.ReadCString();

    NiStreamLoadBinary(kStream, m_uiFlags);

    // NiStreamLoadEnum doesn't exist for efd::BinaryStream.
    unsigned int uiType;
    NiStreamLoadBinary(kStream, uiType);
    m_eType = (NiShaderAttributeDesc::AttributeType)uiType;

    switch (m_eType)
    {
    case NiShaderAttributeDesc::ATTRIB_TYPE_BOOL:
        if (!LoadBinary_Bool(kStream))
            return false;
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_STRING:
        if (!LoadBinary_String(kStream))
            return false;
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT:
        if (!LoadBinary_UnsignedInt(kStream))
            return false;
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT:
        if (!LoadBinary_Float(kStream))
            return false;
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_POINT2:
        if (!LoadBinary_Point2(kStream))
            return false;
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_POINT3:
        if (!LoadBinary_Point3(kStream))
            return false;
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_POINT4:
        if (!LoadBinary_Point4(kStream))
            return false;
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3:
        if (!LoadBinary_Matrix3(kStream))
            return false;
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4:
        if (!LoadBinary_Matrix4(kStream))
            return false;
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_COLOR:
        if (!LoadBinary_Color(kStream))
            return false;
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_TEXTURE:
        if (!LoadBinary_Texture(kStream))
            return false;
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_ARRAY:
        if (!LoadBinary_Array(kStream))
            return false;
        break;

    case NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED:
    case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT8:
    case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT12:
    case NiShaderAttributeDesc::ATTRIB_TYPE_COUNT:
        // do nothing
        break;

    }

    return true;
}

//--------------------------------------------------------------------------------------------------
bool NSBAttributeDesc::SaveBinary_Bool(efd::BinaryStream& kStream)
{
    // Perhaps should use NiBool for this, but it could break streaming.
    unsigned int uiValue = (unsigned int)m_bValue;
    NiStreamSaveBinary(kStream, uiValue);

    return true;
}

//--------------------------------------------------------------------------------------------------
bool NSBAttributeDesc::SaveBinary_String(efd::BinaryStream& kStream)
{
    kStream.WriteCString(m_pcValue);

    if (IsRanged())
    {
        NiStreamSaveBinary(kStream, m_uiHigh);
    }

    return true;
}

//--------------------------------------------------------------------------------------------------
bool NSBAttributeDesc::SaveBinary_UnsignedInt(efd::BinaryStream& kStream)
{
    NiStreamSaveBinary(kStream, m_uiValue);

    if (IsRanged())
    {
        NiStreamSaveBinary(kStream, m_uiLow);
        NiStreamSaveBinary(kStream, m_uiHigh);
    }

    return true;
}

//--------------------------------------------------------------------------------------------------
bool NSBAttributeDesc::SaveBinary_Float(efd::BinaryStream& kStream)
{
    NiStreamSaveBinary(kStream, m_aafValue[0][0]);

    if (IsRanged())
    {
        NiStreamSaveBinary(kStream, m_aafLow[0][0]);
        NiStreamSaveBinary(kStream, m_aafHigh[0][0]);
    }

    return true;
}

//--------------------------------------------------------------------------------------------------
bool NSBAttributeDesc::SaveBinary_Point2(efd::BinaryStream& kStream)
{
    NiStreamSaveBinary(kStream, m_aafValue[0], 2);

    if (IsRanged())
    {
        NiStreamSaveBinary(kStream, m_aafLow[0], 2);
        NiStreamSaveBinary(kStream, m_aafHigh[0], 2);
    }

    return true;
}

//--------------------------------------------------------------------------------------------------
bool NSBAttributeDesc::SaveBinary_Point3(efd::BinaryStream& kStream)
{
    NiStreamSaveBinary(kStream, m_aafValue[0], 3);

    if (IsRanged())
    {
        NiStreamSaveBinary(kStream, m_aafLow[0], 3);
        NiStreamSaveBinary(kStream, m_aafHigh[0], 3);
    }

    return true;
}

//--------------------------------------------------------------------------------------------------
bool NSBAttributeDesc::SaveBinary_Point4(efd::BinaryStream& kStream)
{
    NiStreamSaveBinary(kStream, m_aafValue[0], 4);

    if (IsRanged())
    {
        NiStreamSaveBinary(kStream, m_aafLow[0], 4);
        NiStreamSaveBinary(kStream, m_aafHigh[0], 4);
    }

    return true;
}

//--------------------------------------------------------------------------------------------------
bool NSBAttributeDesc::SaveBinary_Matrix3(efd::BinaryStream& kStream)
{
    NiStreamSaveBinary(kStream, m_aafValue[0], 3);
    NiStreamSaveBinary(kStream, m_aafValue[1], 3);
    NiStreamSaveBinary(kStream, m_aafValue[2], 3);

    return true;
}

//--------------------------------------------------------------------------------------------------
bool NSBAttributeDesc::SaveBinary_Matrix4(efd::BinaryStream& kStream)
{
    NiStreamSaveBinary(kStream, m_aafValue[0], 16);

    return true;
}

//--------------------------------------------------------------------------------------------------
bool NSBAttributeDesc::SaveBinary_Color(efd::BinaryStream& kStream)
{
    NiStreamSaveBinary(kStream, m_aafValue[0], 4);

    if (IsRanged())
    {
        NiStreamSaveBinary(kStream, m_aafLow[0], 4);
        NiStreamSaveBinary(kStream, m_aafHigh[0], 4);
    }

    return true;
}

//--------------------------------------------------------------------------------------------------
bool NSBAttributeDesc::SaveBinary_Texture(efd::BinaryStream& kStream)
{
    NiStreamSaveBinary(kStream, m_uiValue);
    kStream.WriteCString(m_pcDefault);

    return true;
}

//--------------------------------------------------------------------------------------------------
bool NSBAttributeDesc::SaveBinary_Array(efd::BinaryStream& kStream)
{
    NiStreamSaveBinary(kStream, m_uiLen);

    unsigned int uiValue = (unsigned int)m_kArrayValue.m_eType;
    NiStreamSaveBinary(kStream, uiValue);

    NiStreamSaveBinary(kStream, m_kArrayValue.m_uiElementSize);

    switch (m_kArrayValue.m_eType)
    {
        case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT:
        case NiShaderAttributeDesc::ATTRIB_TYPE_POINT2:
        case NiShaderAttributeDesc::ATTRIB_TYPE_POINT3:
        case NiShaderAttributeDesc::ATTRIB_TYPE_POINT4:
            {
                unsigned int uiFloatsPerElement =
                    m_kArrayValue.m_uiElementSize / sizeof(float);

                NiStreamSaveBinary(kStream, (float*)m_kArrayValue.m_pvValue,
                    uiFloatsPerElement * m_uiLen);

                if (IsRanged())
                {
                    NiStreamSaveBinary(kStream, (float*)m_pvHigh,
                        uiFloatsPerElement * m_uiLen);
                    NiStreamSaveBinary(kStream, (float*)m_pvLow,
                        uiFloatsPerElement *  m_uiLen);
                }
            }
            break;
        default:
            EE_FAIL("Trying to save an Unknown Array type");
            return false;
    }

    return true;
}

//--------------------------------------------------------------------------------------------------
bool NSBAttributeDesc::LoadBinary_Bool(efd::BinaryStream& kStream)
{
    unsigned int uiValue;
    NiStreamLoadBinary(kStream, uiValue);

    m_bValue = (uiValue != 0);

    return true;
}

//--------------------------------------------------------------------------------------------------
bool NSBAttributeDesc::LoadBinary_String(efd::BinaryStream& kStream)
{
    m_pcValue = kStream.ReadCString();
    m_uiLen = (unsigned int)(m_pcValue ? strlen(m_pcValue) : 0);

    if (IsRanged())
    {
        NiStreamLoadBinary(kStream, m_uiHigh);
    }

    return true;
}

//--------------------------------------------------------------------------------------------------
bool NSBAttributeDesc::LoadBinary_UnsignedInt(efd::BinaryStream& kStream)
{
    NiStreamLoadBinary(kStream, m_uiValue);

    if (IsRanged())
    {
        NiStreamLoadBinary(kStream, m_uiLow);
        NiStreamLoadBinary(kStream, m_uiHigh);
    }

    return true;
}

//--------------------------------------------------------------------------------------------------
bool NSBAttributeDesc::LoadBinary_Float(efd::BinaryStream& kStream)
{
    NiStreamLoadBinary(kStream, m_aafValue[0][0]);

    if (IsRanged())
    {
        NiStreamLoadBinary(kStream, m_aafLow[0][0]);
        NiStreamLoadBinary(kStream, m_aafHigh[0][0]);
    }

    return true;
}

//--------------------------------------------------------------------------------------------------
bool NSBAttributeDesc::LoadBinary_Point2(efd::BinaryStream& kStream)
{
    NiStreamLoadBinary(kStream, m_aafValue[0], 2);

    if (IsRanged())
    {
        NiStreamLoadBinary(kStream, m_aafLow[0], 2);
        NiStreamLoadBinary(kStream, m_aafHigh[0], 2);
    }

    return true;
}

//--------------------------------------------------------------------------------------------------
bool NSBAttributeDesc::LoadBinary_Point3(efd::BinaryStream& kStream)
{
    NiStreamLoadBinary(kStream, m_aafValue[0], 3);

    if (IsRanged())
    {
        NiStreamLoadBinary(kStream, m_aafLow[0], 3);
        NiStreamLoadBinary(kStream, m_aafHigh[0], 3);
    }

    return true;
}

//--------------------------------------------------------------------------------------------------
bool NSBAttributeDesc::LoadBinary_Point4(efd::BinaryStream& kStream)
{
    NiStreamLoadBinary(kStream, m_aafValue[0], 4);

    if (IsRanged())
    {
        NiStreamLoadBinary(kStream, m_aafLow[0], 4);
        NiStreamLoadBinary(kStream, m_aafHigh[0], 4);
    }

    return true;
}

//--------------------------------------------------------------------------------------------------
bool NSBAttributeDesc::LoadBinary_Matrix3(efd::BinaryStream& kStream)
{
    NiStreamLoadBinary(kStream, m_aafValue[0], 3);
    NiStreamLoadBinary(kStream, m_aafValue[1], 3);
    NiStreamLoadBinary(kStream, m_aafValue[2], 3);

    return true;
}

//--------------------------------------------------------------------------------------------------
bool NSBAttributeDesc::LoadBinary_Matrix4(efd::BinaryStream& kStream)
{
    NiStreamLoadBinary(kStream, m_aafValue[0], 16);

    return true;
}

//--------------------------------------------------------------------------------------------------
bool NSBAttributeDesc::LoadBinary_Color(efd::BinaryStream& kStream)
{
    NiStreamLoadBinary(kStream, m_aafValue[0], 4);

    if (IsRanged())
    {
        NiStreamLoadBinary(kStream, m_aafLow[0], 4);
        NiStreamLoadBinary(kStream, m_aafHigh[0], 4);
    }

    return true;
}

//--------------------------------------------------------------------------------------------------
bool NSBAttributeDesc::LoadBinary_Texture(efd::BinaryStream& kStream)
{
    NiStreamLoadBinary(kStream, m_uiValue);

    m_pcDefault = kStream.ReadCString();
    m_uiDefaultLen = (unsigned int)(m_pcDefault ? strlen(m_pcDefault) : 0);
    return true;
}

//--------------------------------------------------------------------------------------------------
bool NSBAttributeDesc::LoadBinary_Array(efd::BinaryStream& kStream)
{
    NiStreamLoadBinary(kStream, m_uiLen);

    unsigned int uiTemp = 0;
    NiStreamLoadBinary(kStream, uiTemp);
    m_kArrayValue.m_eType = (NiShaderAttributeDesc::AttributeType)uiTemp;

    NiStreamLoadBinary(kStream, m_kArrayValue.m_uiElementSize);

    m_kArrayValue.m_pvValue = NiAlloc(char, m_uiLen *
        m_kArrayValue.m_uiElementSize);

    switch (m_kArrayValue.m_eType)
    {
        case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT:
        case NiShaderAttributeDesc::ATTRIB_TYPE_POINT2:
        case NiShaderAttributeDesc::ATTRIB_TYPE_POINT3:
        case NiShaderAttributeDesc::ATTRIB_TYPE_POINT4:
            {
                unsigned int uiFloatsPerElement =
                    m_kArrayValue.m_uiElementSize / sizeof(float);

                NiStreamLoadBinary(kStream, (float*)m_kArrayValue.m_pvValue,
                    uiFloatsPerElement * m_uiLen);

                if (IsRanged())
                {
                    NiStreamLoadBinary(kStream, (float*)m_pvHigh,
                        uiFloatsPerElement * m_uiLen);
                    NiStreamLoadBinary(kStream, (float*)m_pvLow,
                        uiFloatsPerElement *  m_uiLen);
                }
            }
            break;
        default:
            EE_FAIL("Trying to load an Unknown Array type");
            return false;
    }

    return true;
}

//--------------------------------------------------------------------------------------------------
