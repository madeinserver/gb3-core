//-------------------------------------------------------------------------------------
// DirectXPackedVector.inl -- SIMD C++ Math library
//
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
//
// http://go.microsoft.com/fwlink/?LinkID=615560
//-------------------------------------------------------------------------------------
#ifndef _XMFLOAT_H_
#define _XMFLOAT_H_

#include <stdint.h>

inline float XMConvertHalfToFloat(uint16_t Value) noexcept
{
    uint32_t Mantissa = static_cast<uint32_t>(Value & 0x03FF);

    uint32_t Exponent = (Value & 0x7C00);
    if (Exponent == 0x7C00) // INF/NAN
    {
        Exponent = 0x8f;
    }
    else if (Exponent != 0)  // The value is normalized
    {
        Exponent = static_cast<uint32_t>((static_cast<int>(Value) >> 10) & 0x1F);
    }
    else if (Mantissa != 0)     // The value is denormalized
    {
        // Normalize the value in the resulting float
        Exponent = 1;

        do
        {
            Exponent--;
            Mantissa <<= 1;
        } while ((Mantissa & 0x0400) == 0);

        Mantissa &= 0x03FF;
    }
    else                        // The value is zero
    {
        Exponent = static_cast<uint32_t>(-112);
    }

    uint32_t Result =
        ((static_cast<uint32_t>(Value) & 0x8000) << 16) // Sign
        | ((Exponent + 112) << 23)                      // Exponent
        | (Mantissa << 13);                             // Mantissa

    return reinterpret_cast<float*>(&Result)[0];
}

inline uint16_t XMConvertFloatToHalf(float Value) noexcept
{
    uint32_t Result;

    uint32_t IValue = reinterpret_cast<uint32_t*>(&Value)[0];
    uint32_t Sign = (IValue & 0x80000000U) >> 16U;
    IValue = IValue & 0x7FFFFFFFU;      // Hack off the sign
    if (IValue >= 0x47800000 /*e+16*/)
    {
        // The number is too large to be represented as a half. Return infinity or NaN
        Result = 0x7C00U | ((IValue > 0x7F800000) ? (0x200 | ((IValue >> 13U) & 0x3FFU)) : 0U);
    }
    else if (IValue <= 0x33000000U /*e-25*/)
    {
        Result = 0;
    }
    else if (IValue < 0x38800000U /*e-14*/)
    {
        // The number is too small to be represented as a normalized half.
        // Convert it to a denormalized value.
        uint32_t Shift = 125U - (IValue >> 23U);
        IValue = 0x800000U | (IValue & 0x7FFFFFU);
        Result = IValue >> (Shift + 1);
        uint32_t s = (IValue & ((1U << Shift) - 1)) != 0;
        Result += (Result | s) & ((IValue >> Shift) & 1U);
    }
    else
    {
        // Rebias the exponent to represent the value as a normalized half.
        IValue += 0xC8000000U;
        Result = ((IValue + 0x0FFFU + ((IValue >> 13U) & 1U)) >> 13U) & 0x7FFFU;
    }
    return static_cast<uint16_t>(Result | Sign);
}

inline void XMConvertFloatToHalfArray(uint16_t* pDest, const float* pSrc, size_t unCount)
{
    for (size_t i = 0U; i < unCount; i++)
    {
        pDest[i] = XMConvertFloatToHalf(pSrc[i]);
    }
}

inline void XMConvertHalfToFloatArray(float* pDest, const uint16_t* pSrc, size_t unCount)
{
    for (size_t i = 0U; i < unCount; i++)
    {
        pDest[i] = XMConvertHalfToFloat(pSrc[i]);
    }
}

#endif
