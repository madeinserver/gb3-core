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

#include <NiSystem.h>
#include "NiProcessor.h"
#include "NiTransform.h"

extern "C" void EE_ASM_CALL EE_ASM_DECOR(NiTransformVectorsPentium) (NiUInt32 uiVerts, const float* pModel,
    float* pWorld, NiMatrix3* pMatrix);
extern "C" void EE_ASM_CALL EE_ASM_DECOR(NiTransformPointsPentiumRun)(NiUInt32 uiVerts, const float* pModel,
    float* pWorld, float* pScaledMath, float fTX, float fTY, float fTZ);

void NiTransformPointsPentium (NiUInt32 uiVerts, const float* pModel,
    float* pWorld, const NiTransform *pXform);

NiProcessorSpecificCode::VectorTransformFunc
    NiProcessorSpecificCode::ms_pVectorsTransform
        = EE_ASM_DECOR(NiTransformVectorsPentium);
NiProcessorSpecificCode::PointTransformFunc
    NiProcessorSpecificCode::ms_pPointsTransform
        = NiTransformPointsPentium;

//--------------------------------------------------------------------------------------------------
void NiTransformPointsPentium (NiUInt32 uiVerts, const float* pModel,
    float* pWorld, const NiTransform *pXform)
{
    float fTX = pXform->m_Translate.x;
    float fTY = pXform->m_Translate.y;
    float fTZ = pXform->m_Translate.z;
    float fWS = pXform->m_fScale;

    float pScaledMatData[9];
    float *pScaledMat = pScaledMatData;

    // WARNING.  This typecast is valid because NiMatrix3 on the PC stores
    // the matrix in row-major order as float[3][3].  Any change to the
    // data representation in NiMatrix3 could invalidate this code.
    float* pFMatrix = (float*) &pXform->m_Rotate;

    pScaledMat[0] = fWS * pFMatrix[0];
    pScaledMat[1] = fWS * pFMatrix[1];
    pScaledMat[2] = fWS * pFMatrix[2];
    pScaledMat[3] = fWS * pFMatrix[3];
    pScaledMat[4] = fWS * pFMatrix[4];
    pScaledMat[5] = fWS * pFMatrix[5];
    pScaledMat[6] = fWS * pFMatrix[6];
    pScaledMat[7] = fWS * pFMatrix[7];
    pScaledMat[8] = fWS * pFMatrix[8];

    EE_ASM_DECOR(NiTransformPointsPentiumRun)(uiVerts, pModel, pWorld, pScaledMat, fTX, fTY, fTZ);
}

//--------------------------------------------------------------------------------------------------
void NiProcessorSpecificCode::TransformPointsBasic (NiUInt32 uiVerts,
    const float* pModel, float* pWorld, const NiTransform *pXform)
{
    // WARNING.  This typecast is valid because NiMatrix3 on the PC stores
    // the matrix in row-major order as float[3][3].  Any change to the
    // data representation in NiMatrix3 could invalidate this code.
    float* pFMatrix = (float*) &pXform->m_Rotate;

    for (NiUInt32 i = 0; i < uiVerts; i++, pModel += 3, pWorld += 3)
    {
        // wn = wr*n
        *pWorld = (pFMatrix[0]*(*pModel) +
                  pFMatrix[1]*(*(pModel+1)) +
                  pFMatrix[2]*(*(pModel+2)))*pXform->m_fScale +
                  pXform->m_Translate.x;
        *(pWorld+1) = (pFMatrix[3]*(*pModel) +
                      pFMatrix[4]*(*(pModel+1)) +
                      pFMatrix[5]*(*(pModel+2)))*pXform->m_fScale +
                      pXform->m_Translate.y;
        *(pWorld+2) = (pFMatrix[6]*(*pModel) +
                      pFMatrix[7]*(*(pModel+1)) +
                      pFMatrix[8]*(*(pModel+2)))*pXform->m_fScale +
                      pXform->m_Translate.z;
    }
}

//--------------------------------------------------------------------------------------------------
void NiProcessorSpecificCode::TransformVectorsBasic (NiUInt32 uiVerts,
    const float* pModel, float* pWorld, NiMatrix3* pMatrix)
{
    // WARNING.  This typecast is valid because NiMatrix3 on the PC stores
    // the matrix in row-major order as float[3][3].  Any change to the
    // data representation in NiMatrix3 could invalidate this code.
    float* pFMatrix = (float*) pMatrix;

    for (NiUInt32 i = 0; i < uiVerts; i++, pModel+=3, pWorld+=3)
    {
        // wn = wr*n
        *pWorld = pFMatrix[0]*(*pModel)
                 +pFMatrix[1]*(*(pModel+1))
                 +pFMatrix[2]*(*(pModel+2));
        *(pWorld+1) = pFMatrix[3]*(*pModel)
                     +pFMatrix[4]*(*(pModel+1))
                     +pFMatrix[5]*(*(pModel+2));
        *(pWorld+2) = pFMatrix[6]*(*pModel)
                     +pFMatrix[7]*(*(pModel+1))
                     +pFMatrix[8]*(*(pModel+2));
    }
}

//--------------------------------------------------------------------------------------------------
