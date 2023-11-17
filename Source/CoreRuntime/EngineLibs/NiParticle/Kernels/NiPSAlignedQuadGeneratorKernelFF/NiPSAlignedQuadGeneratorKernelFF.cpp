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
#include <NiParticlePCH.h>

#include "NiPSAlignedQuadGeneratorKernelFF.h"
#include <NiUniversalTypes.h>
#include <NiMath.h>

//--------------------------------------------------------------------------------------------------
NiSPBeginKernelImpl(NiPSAlignedQuadGeneratorKernelFF)
{
    // Get input streams.
    const NiPSAlignedQuadGeneratorKernelStruct* pkIStruct =
        kWorkload.GetInput<NiPSAlignedQuadGeneratorKernelStruct>(0);
    const NiPoint3* pkIPositions = kWorkload.GetInput<NiPoint3>(1);
    const float* pfIRadii = kWorkload.GetInput<float>(2);
    const float* pfISizes = kWorkload.GetInput<float>(3);
    const NiRGBA* pkIColors = kWorkload.GetInput<NiRGBA>(4);
    const float* pfIRotAngles = kWorkload.GetInput<float>(5);
    const NiPoint3* pkIVelocities = kWorkload.GetInput<NiPoint3>(6);

    // Get output streams.
    NiPoint3* pkOVertices = kWorkload.GetOutput<NiPoint3>(0);
    NiRGBA* pkOColors = kWorkload.GetOutput<NiRGBA>(1);

    // Get block count.
    NiUInt32 uiBlockCount = kWorkload.GetBlockCount();

    // Compute the right and up vectors.
    NiPoint3 kRight = pkIStruct->m_kUp.UnitCross(pkIStruct->m_kNormal);
    NiPoint3 kUp = pkIStruct->m_kNormal.Cross(kRight);

    // Extract stretch factors
    float fAmountU = pkIStruct->m_fScaleNormalizedU;
    float fLimitU = pkIStruct->m_fScaleLimitU;
    float fRestU = pkIStruct->m_fScaleRestU;
    float fAmountV = pkIStruct->m_fScaleNormalizedV;
    float fLimitV = pkIStruct->m_fScaleLimitV;
    float fRestV = pkIStruct->m_fScaleRestV;
    bool bScaleSpeed = (fAmountU != 0.0f || fAmountV != 0.0f);

    // Extract pivot point center
    float fCenterU = (pkIStruct->m_fCenterU);
    float fCenterV = (pkIStruct->m_fCenterV);

    // Generate quads.
    if (pfIRotAngles)
    {
        for (NiUInt32 ui = 0; ui < uiBlockCount; ++ui)
        {
            float fSize = pfIRadii[ui] * pfISizes[ui];

            float fScaleU = 1.0f;
            float fScaleV = 1.0f;
            if (bScaleSpeed)
            {
                // Scale the up and right vectors according to velocity
                // scale = (ratio * velocity * limit + rest) /
                //         (ratio * velocity + 1)
                const NiPoint3& kVelocity = pkIVelocities[ui];
                float fVelocity = kVelocity.Length();
                fScaleU = (fAmountU * fVelocity * fLimitU + fRestU) /
                    (fAmountU * fVelocity + 1.0f);

                fScaleV = (fAmountV * fVelocity * fLimitV + fRestV) /
                    (fAmountV * fVelocity + 1.0f);
            }

            // Compute the rotated top left and top right center vectors.
            float fSinA, fCosA;
            NiSinCos(pfIRotAngles[ui], fSinA, fCosA);
            float fC1 = fSize * (fCosA + fSinA);
            float fC2 = fSize * (fCosA - fSinA);

            NiPoint3 kV0 =  fC1 * fScaleU * kRight + fC2 * fScaleV * kUp;
            NiPoint3 kV1 = -fC2 * fScaleU * kRight + fC1 * fScaleV * kUp;

            // Compute the rotated pivot point center vector.
            float fCU = fSize * fScaleU * fCenterU;
            float fCV = fSize * fScaleV * fCenterV;

            NiPoint3 kRotatedCenter  =
                (fCV * fSinA + fCU * fCosA) * kRight  +
                (fCV * fCosA - fCU * fSinA) * kUp;

            NiPoint3 kPosition = pkIPositions[ui];

            *pkOVertices = kPosition + kV1 - kRotatedCenter;
            ++pkOVertices;

            *pkOVertices = kPosition - kV0 - kRotatedCenter;
            ++pkOVertices;

            *pkOVertices = kPosition - kV1 - kRotatedCenter;
            ++pkOVertices;

            *pkOVertices = kPosition + kV0 - kRotatedCenter;
            ++pkOVertices;

            if (pkIColors && pkOColors)
            {
                pkOColors[0] = pkIColors[ui];
                pkOColors[1] = pkIColors[ui];
                pkOColors[2] = pkIColors[ui];
                pkOColors[3] = pkIColors[ui];
                pkOColors += 4;
            }
        }
    }
    else
    {
        for (NiUInt32 ui = 0; ui < uiBlockCount; ++ui)
        {
            float fSize = pfIRadii[ui] * pfISizes[ui];

            float fScaleU = 1.0f;
            float fScaleV = 1.0f;
            if (bScaleSpeed)
            {
                // Scale the up and right vectors according to velocity
                // scale = (ratio * velocity * limit + rest) /
                //         (ratio * velocity + 1)
                const NiPoint3& kVelocity = pkIVelocities[ui];
                float fVelocity = kVelocity.Length();
                fScaleU = (fAmountU * fVelocity * fLimitU + fRestU) /
                    (fAmountU * fVelocity + 1.0f);

                fScaleV = (fAmountV * fVelocity * fLimitV + fRestV) /
                    (fAmountV * fVelocity + 1.0f);
            }

            // Compute the top left corner center vector.
            NiPoint3 kTopLeft = kUp * fScaleV - kRight * fScaleU;

            // Compute the top right corner center vector.
            NiPoint3 kTopRight = kUp * fScaleV + kRight * fScaleU;

            NiPoint3 kV0 = fSize * kTopRight;
            NiPoint3 kV1 = fSize * kTopLeft;

            // Compute the pivot point center vector.
            NiPoint3 kCenter = kUp * fScaleV * fCenterV + kRight * fScaleU * fCenterU;
            kCenter *= fSize;

            NiPoint3 kPosition = pkIPositions[ui];

            *pkOVertices = kPosition + kV1 - kCenter;
            ++pkOVertices;

            *pkOVertices = kPosition - kV0 - kCenter;
            ++pkOVertices;

            *pkOVertices = kPosition - kV1 - kCenter;
            ++pkOVertices;

            *pkOVertices = kPosition + kV0 - kCenter;
            ++pkOVertices;

            if (pkIColors && pkOColors)
            {
                pkOColors[0] = pkIColors[ui];
                pkOColors[1] = pkIColors[ui];
                pkOColors[2] = pkIColors[ui];
                pkOColors[3] = pkIColors[ui];
                pkOColors += 4;
            }
        }
    }
}
NiSPEndKernelImpl(NiPSAlignedQuadGeneratorKernelFF)

//--------------------------------------------------------------------------------------------------
