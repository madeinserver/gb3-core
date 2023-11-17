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

#include "egmVisualizersPCH.h"
#include "PropertyVisualizationHelpers.h"
#include <efd/ParseHelper.h>

using namespace efd;
using namespace egf;
using namespace ecr;
using namespace egmVisualizers;

//------------------------------------------------------------------------------------------------
void PropertyVisualizationHelpers::CreateCircle(
    NiMesh* pMesh,
    efd::Float32 radius,
    efd::UInt16 numSlices)
{
    EE_ASSERT(pMesh);
    EE_ASSERT(numSlices > 0);

    pMesh->SetPrimitiveType(NiPrimitiveType::PRIMITIVE_LINESTRIPS);

    NiDataStreamRef* pStreamRef = pMesh->FindStreamRef(NiCommonSemantics::POSITION(), 0);
    if (!pStreamRef)
    {
        pStreamRef = pMesh->AddStream(
            NiCommonSemantics::POSITION(),
            0,
            NiDataStreamElement::F_FLOAT32_3, 17,
            NiDataStream::ACCESS_GPU_READ |
            NiDataStream::ACCESS_CPU_WRITE_STATIC,
            NiDataStream::USAGE_VERTEX);
    }

    EE_ASSERT(pStreamRef);

    NiDataStream* pStream = pStreamRef->GetDataStream();

    NiPoint3* pPoints = (NiPoint3*)pStream->Lock(NiDataStream::LOCK_WRITE);

    efd::Float32 z = 0.0f;
    efd::Float32 stepSize = NI_TWO_PI / (efd::Float32)numSlices;
    for (efd::UInt32 i = 0; i < numSlices; i++)
    {
        efd::Float32 x = radius * NiCos(i * stepSize);
        efd::Float32 y = radius * NiSin(i * stepSize);
        pPoints[i] = NiPoint3(x,y,z);
    }
    pPoints[16] = pPoints[0];

    pStream->Unlock(NiDataStream::LOCK_WRITE);
}

//------------------------------------------------------------------------------------------------
void PropertyVisualizationHelpers::CreateWireSphere(
    NiMesh* pMesh,
    efd::Float32 radius,
    efd::UInt16 numSlices,
    efd::UInt16 numStacks)
{
    EE_ASSERT(pMesh);

    efd::UInt16 numVerts = 2 + (numSlices) * (numStacks-1);
    efd::UInt16 numIndices = 2 + (numStacks-1) * (numSlices+1) +
        (numSlices-1)*(numStacks);

    pMesh->SetPrimitiveType(NiPrimitiveType::PRIMITIVE_LINESTRIPS);

    NiDataStreamRef* pStreamRef = pMesh->FindStreamRef(NiCommonSemantics::POSITION(), 0);
    if (!pStreamRef)
    {
        pStreamRef = pMesh->AddStream(
            NiCommonSemantics::POSITION(),
            0,
            NiDataStreamElement::F_FLOAT32_3,
            numVerts,
            NiDataStream::ACCESS_GPU_READ | NiDataStream::ACCESS_CPU_READ |
            NiDataStream::ACCESS_CPU_WRITE_STATIC,
            NiDataStream::USAGE_VERTEX);
    }

    EE_ASSERT(pStreamRef);
    NiDataStream* pStream = pStreamRef->GetDataStream();

    NiPoint3* pPoints = (NiPoint3*)pStream->Lock(NiDataStream::LOCK_WRITE);
    CreateSphereVerts(pPoints, radius, numSlices, numStacks);
    pStream->Unlock(NiDataStream::LOCK_WRITE);

    pStreamRef = pMesh->FindStreamRef(NiCommonSemantics::INDEX(), 0);
    if (!pStreamRef)
    {
        pStreamRef = pMesh->AddStream(
            NiCommonSemantics::INDEX(),
            0,
            NiDataStreamElement::F_UINT16_1,
            numIndices,
            NiDataStream::ACCESS_GPU_READ |
            NiDataStream::ACCESS_CPU_WRITE_STATIC,
            NiDataStream::USAGE_VERTEX_INDEX);
    }

    EE_ASSERT(pStreamRef);
    pStream = pStreamRef->GetDataStream();

    efd::UInt16* auiIndices = (UInt16*)pStream->Lock(NiDataStream::LOCK_WRITE);
    efd::UInt16* puiIndices = auiIndices;

    // Go around stacks first, while going up slice 0
    {
        // start at the bottom
        *puiIndices++ = 0;

        for (efd::UInt16 st = 0; st < numStacks - 1; st++)
        {
            efd::UInt16 stackOffset = 1 + st * numSlices;
            for (efd::UInt16 sl = 0; sl < numSlices; sl++)
            {
                puiIndices[sl] = sl + stackOffset;
            }
            puiIndices[numSlices] = stackOffset;
            puiIndices += numSlices + 1;
        }

        // top it off
        *puiIndices++ = numVerts - 1;
    }

    // Go down slice 1, up slice 2, down slice 3, etc...
    for (efd::UInt16 sl = 1; sl < numSlices; sl++)
    {
        if (sl % 2 == 1)
        {
            // go down slice
            for (int st = numStacks - 2; st >= 0; st--)
            {
                *puiIndices++ = (efd::UInt16)(sl + 1 + st * numSlices);
            }

            // bottom
            *puiIndices++ = 0;
        }
        else
        {
            // go up slice
            for (efd::UInt16 st = 0; st < numStacks - 1; st++)
            {
                *puiIndices++ = sl + 1 + st * numSlices;
            }

            // top
            *puiIndices++ = numVerts - 1;
        }
    }

    pStream->Unlock(NiDataStream::LOCK_WRITE);
}

//------------------------------------------------------------------------------------------------
void PropertyVisualizationHelpers::CreateWireCube(
    NiMesh* pMesh,
    efd::Float32 width,
    efd::Float32 length,
    efd::Float32 height)
{
    EE_ASSERT(pMesh);

    pMesh->SetPrimitiveType(NiPrimitiveType::PRIMITIVE_LINES);

    NiDataStreamRef* pStreamRef = pMesh->FindStreamRef(NiCommonSemantics::POSITION(), 0);
    if (!pStreamRef)
    {
        pStreamRef = pMesh->AddStream(
            NiCommonSemantics::POSITION(),
            0,
            NiDataStreamElement::F_FLOAT32_3, 24,
            NiDataStream::ACCESS_GPU_READ |
            NiDataStream::ACCESS_CPU_WRITE_MUTABLE,
            NiDataStream::USAGE_VERTEX);
    }

    EE_ASSERT(pStreamRef);
    NiDataStream* pStream = pStreamRef->GetDataStream();

    NiPoint3* pPoints = (NiPoint3*)pStream->Lock(NiDataStream::LOCK_WRITE);

    efd::Float32 hx = width / 2.0f;
    efd::Float32 hy = length / 2.0f;
    efd::Float32 hz = height / 2.0f;

    // Front
    pPoints[0] = NiPoint3(-hx, hy, hz);
    pPoints[1] = NiPoint3(hx, hy, hz);
    pPoints[2] = NiPoint3(-hx, hy, -hz);
    pPoints[3] = NiPoint3(hx, hy, -hz);
    pPoints[4] = pPoints[0];
    pPoints[5] = pPoints[2];
    pPoints[6] = pPoints[1];
    pPoints[7] = pPoints[3];

    // Back
    pPoints[8] = NiPoint3(-hx, -hy, hz);
    pPoints[9] = NiPoint3(hx, -hy, hz);
    pPoints[10] = NiPoint3(-hx, -hy, -hz);
    pPoints[11] = NiPoint3(hx, -hy, -hz);
    pPoints[12] = pPoints[8];
    pPoints[13] = pPoints[10];
    pPoints[14] = pPoints[9];
    pPoints[15] = pPoints[11];

    // Side connections
    pPoints[16] = pPoints[0];
    pPoints[17] = pPoints[8];
    pPoints[18] = pPoints[2];
    pPoints[19] = pPoints[10];
    pPoints[20] = pPoints[1];
    pPoints[21] = pPoints[9];
    pPoints[22] = pPoints[3];
    pPoints[23] = pPoints[11];

    pStream->Unlock(NiDataStream::LOCK_WRITE);
}

//------------------------------------------------------------------------------------------------
void PropertyVisualizationHelpers::CreateSolidCube(
    NiMesh* pMesh,
    efd::Float32 width,
    efd::Float32 length,
    efd::Float32 height)
{
    EE_ASSERT(pMesh);

    pMesh->SetPrimitiveType(NiPrimitiveType::PRIMITIVE_TRIANGLES);

    NiDataStreamRef* pStreamRef = pMesh->FindStreamRef(NiCommonSemantics::POSITION(), 0);
    if (!pStreamRef)
    {
        pStreamRef = pMesh->AddStream(
            NiCommonSemantics::POSITION(),
            0,
            NiDataStreamElement::F_FLOAT32_3, 36,
            NiDataStream::ACCESS_GPU_READ |
            NiDataStream::ACCESS_CPU_WRITE_MUTABLE,
            NiDataStream::USAGE_VERTEX);
    }

    EE_ASSERT(pStreamRef);
    NiDataStream* pStream = pStreamRef->GetDataStream();

    NiPoint3* pPoints = (NiPoint3*)pStream->Lock(NiDataStream::LOCK_WRITE);

    efd::Float32 hx = width / 2.0f;
    efd::Float32 hy = length / 2.0f;
    efd::Float32 hz = height / 2.0f;

    //  y+                                  //          y-
    //  2---3   x+                          // bottom   1---4 x+
    //  |   | top (looking at)              //          |   |
    //  1---4                               //          2---3
    //
    //  ^                                   //  yx---->
    //  |                                   //  |
    //  yx--->                              //  v

    // Top
    NiPoint3 vTop1(-hx, -hy, hz);
    NiPoint3 vTop2(-hx, hy, hz);
    NiPoint3 vTop3(hx, hy, hz);
    NiPoint3 vTop4(hx, -hy, hz);
    // Bottom
    NiPoint3 vBottom1(-hx, -hy, -hz);
    NiPoint3 vBottom2(-hx, hy, -hz);
    NiPoint3 vBottom3(hx, hy, -hz);
    NiPoint3 vBottom4(hx, -hy, -hz);

    
    // z+
    pPoints[0] = vTop3;
    pPoints[1] = vTop2;
    pPoints[2] = vTop1;
    pPoints[3] = vTop1;
    pPoints[4] = vTop4;
    pPoints[5] = vTop3;

    // x+
    pPoints[6] = vTop3;
    pPoints[7] = vTop4;
    pPoints[8] = vBottom4;
    pPoints[9] = vBottom4;
    pPoints[10] = vBottom3;
    pPoints[11] = vTop3;

    // y+
    pPoints[12] = vTop2;
    pPoints[13] = vTop3;
    pPoints[14] = vBottom3;
    pPoints[15] = vBottom3;
    pPoints[16] = vBottom2;
    pPoints[17] = vTop2;

    // x-
    pPoints[18] = vTop1;
    pPoints[19] = vTop2;
    pPoints[20] = vBottom2;
    pPoints[21] = vBottom2;
    pPoints[22] = vBottom1;
    pPoints[23] = vTop1;

    // y-
    pPoints[24] = vTop4;
    pPoints[25] = vTop1;
    pPoints[26] = vBottom1;
    pPoints[27] = vBottom1;
    pPoints[28] = vBottom4;
    pPoints[29] = vTop4;

    // z-
    pPoints[30] = vBottom1;
    pPoints[31] = vBottom2;
    pPoints[32] = vBottom3;
    pPoints[33] = vBottom3;
    pPoints[34] = vBottom4;
    pPoints[35] = vBottom1;

    pStream->Unlock(NiDataStream::LOCK_WRITE);
}

//------------------------------------------------------------------------------------------------
void PropertyVisualizationHelpers::CreateFrustum(
    NiMesh* pMesh,
    efd::Float32 fov,
    efd::Float32 neardist,
    efd::Float32 fardist,
    efd::Float32 aspect)
{
    EE_UNUSED_ARG(pMesh);

    efd::Float32 horizFieldOfViewRad = fov * EE_DEGREES_TO_RADIANS;

    efd::Float32 widthfar = tanf(horizFieldOfViewRad * 0.5f) * fardist;
    efd::Float32 heightfar = widthfar * aspect;

    efd::Float32 widthnear = tanf(horizFieldOfViewRad * 0.5f) * neardist;
    efd::Float32 heightnear = widthnear * aspect;

    NiPoint3 fc(fardist, 0.0f, 0.0f);
    NiPoint3 nc(neardist, 0.0f, 0.0f);
    NiPoint3 right(0.0f, 1.0f, 0.0f);
    NiPoint3 up(0.0f, 0.0f, 1.0f);

    NiPoint3 ftl = fc + (up * heightfar) - (right * widthfar);
    NiPoint3 ftr = fc + (up * heightfar) + (right * widthfar);
    NiPoint3 fbl = fc - (up * heightfar) - (right * widthfar);
    NiPoint3 fbr = fc - (up * heightfar) + (right * widthfar);

    NiPoint3 ntl = nc + (up * heightnear) - (right * widthnear);
    NiPoint3 ntr = nc + (up * heightnear) + (right * widthnear);
    NiPoint3 nbl = nc - (up * heightnear) - (right * widthnear);
    NiPoint3 nbr = nc - (up * heightnear) + (right * widthnear);

    pMesh->SetPrimitiveType(NiPrimitiveType::PRIMITIVE_LINES);

    NiDataStreamRef* pStreamRef = pMesh->FindStreamRef(NiCommonSemantics::POSITION(), 0);
    if (!pStreamRef)
    {
        pStreamRef = pMesh->AddStream(
            NiCommonSemantics::POSITION(),
            0,
            NiDataStreamElement::F_FLOAT32_3,
            24,
            NiDataStream::ACCESS_GPU_READ | NiDataStream::ACCESS_CPU_WRITE_MUTABLE,
            NiDataStream::USAGE_VERTEX);
    }
    EE_ASSERT(pStreamRef);
    NiDataStream* pStream = pStreamRef->GetDataStream();

    NiPoint3* pPoints = (NiPoint3*)pStream->Lock(NiDataStream::LOCK_WRITE);

    // Near
    pPoints[0] = ntl;
    pPoints[1] = ntr;
    pPoints[2] = ntr;
    pPoints[3] = nbr;
    pPoints[4] = nbr;
    pPoints[5] = nbl;
    pPoints[6] = nbl;
    pPoints[7] = ntl;

    // Far
    pPoints[8] = ftl;
    pPoints[9] = ftr;
    pPoints[10] = ftr;
    pPoints[11] = fbr;
    pPoints[12] = fbr;
    pPoints[13] = fbl;
    pPoints[14] = fbl;
    pPoints[15] = ftl;

    // Connections
    pPoints[16] = ntl;
    pPoints[17] = ftl;
    pPoints[18] = ntr;
    pPoints[19] = ftr;
    pPoints[20] = nbl;
    pPoints[21] = fbl;
    pPoints[22] = nbr;
    pPoints[23] = fbr;

    pStream->Unlock(NiDataStream::LOCK_WRITE);
}

//------------------------------------------------------------------------------------------------
void PropertyVisualizationHelpers::CreateAsterisk(NiMesh* pMesh)
{
    EE_ASSERT(pMesh);

    pMesh->SetPrimitiveType(NiPrimitiveType::PRIMITIVE_LINES);

    NiDataStreamRef* pStreamRef = pMesh->FindStreamRef(NiCommonSemantics::POSITION(), 0);
    if (!pStreamRef)
    {
        pStreamRef = pMesh->AddStream(
            NiCommonSemantics::POSITION(),
            0,
            NiDataStreamElement::F_FLOAT32_3,
            6,
            NiDataStream::ACCESS_GPU_READ | NiDataStream::ACCESS_CPU_WRITE_MUTABLE,
            NiDataStream::USAGE_VERTEX,
            NULL);
    }

    EE_ASSERT(pStreamRef);
    NiDataStream* pStream = pStreamRef->GetDataStream();
    NiPoint3* pPoints = (NiPoint3*)pStream->Lock(NiDataStream::LOCK_WRITE);

    pPoints[0] = NiPoint3(-1.0f, 0.0f, -1.0f);
    pPoints[1] = NiPoint3(1.0f, 0.0f, 1.0f);
    pPoints[2] = NiPoint3(1.0f, 0.0f, -1.0f);
    pPoints[3] = NiPoint3(-1.0f, 0.0f, 1.0f);
    pPoints[4] = NiPoint3(0.0f, 1.0f, 0.0f);
    pPoints[5] = NiPoint3(0.0f, -1.0f, 0.0f);

    pStream->Unlock(NiDataStream::LOCK_WRITE);
}

//------------------------------------------------------------------------------------------------

void PropertyVisualizationHelpers::CreateLine(
    NiMesh* pMesh,
    const NiPoint3& point1,
    const NiPoint3& point2)
{
    EE_ASSERT(pMesh);

    pMesh->SetPrimitiveType(NiPrimitiveType::PRIMITIVE_LINES);

    NiDataStreamRef* pStreamRef = pMesh->FindStreamRef(NiCommonSemantics::POSITION(), 0);
    if (!pStreamRef)
    {
        pStreamRef = pMesh->AddStream(
            NiCommonSemantics::POSITION(),
            0,
            NiDataStreamElement::F_FLOAT32_3,
            2,
            NiDataStream::ACCESS_GPU_READ |
            NiDataStream::ACCESS_CPU_WRITE_MUTABLE,
            NiDataStream::USAGE_VERTEX,
            NULL);
    }

    EE_ASSERT(pStreamRef);
    NiDataStream* pStream = pStreamRef->GetDataStream();
    NiPoint3* pPoints = (NiPoint3*)pStream->Lock(NiDataStream::LOCK_WRITE);

    pPoints[0] = point1;
    pPoints[1] = point2;

    pStream->Unlock(NiDataStream::LOCK_WRITE);
}

//------------------------------------------------------------------------------------------------
void PropertyVisualizationHelpers::CreateSpline(
    NiMesh* pMesh,
    const NiPoint3& point1,
    const NiPoint3& point2)
{
    EE_ASSERT(pMesh);

    pMesh->SetPrimitiveType(NiPrimitiveType::PRIMITIVE_LINES);

    NiDataStreamRef* pStreamRef = pMesh->FindStreamRef(NiCommonSemantics::POSITION(), 0);
    if (!pStreamRef)
    {
        pStreamRef = pMesh->AddStream(
            NiCommonSemantics::POSITION(),
            0,
            NiDataStreamElement::F_FLOAT32_3,
            2,
            NiDataStream::ACCESS_GPU_READ |
            NiDataStream::ACCESS_CPU_WRITE_MUTABLE,
            NiDataStream::USAGE_VERTEX,
            NULL);
    }

    EE_ASSERT(pStreamRef);
    NiDataStream* pStream = pStreamRef->GetDataStream();
    NiPoint3* pPoints = (NiPoint3*)pStream->Lock(NiDataStream::LOCK_WRITE);

    pPoints[0] = point1;
    pPoints[1] = point2;

    pStream->Unlock(NiDataStream::LOCK_WRITE);
}

//------------------------------------------------------------------------------------------------

void PropertyVisualizationHelpers::CreateThickLine(
    NiMesh* pMesh,
    const NiPoint3& point1,
    const NiPoint3& point2,
    float thickness)
{
    EE_ASSERT(pMesh);

    pMesh->SetPrimitiveType(NiPrimitiveType::PRIMITIVE_TRIANGLES);

    NiDataStreamRef* pStreamRef = pMesh->FindStreamRef(NiCommonSemantics::POSITION(), 0);
    if (!pStreamRef)
    {
        pStreamRef = pMesh->AddStream(
            NiCommonSemantics::POSITION(),
            0,
            NiDataStreamElement::F_FLOAT32_3,
            8,
            NiDataStream::ACCESS_GPU_READ |
            NiDataStream::ACCESS_CPU_WRITE_MUTABLE,
            NiDataStream::USAGE_VERTEX,
            NULL);
    }

    EE_ASSERT(pStreamRef);
    NiDataStream* pStream = pStreamRef->GetDataStream();
    NiPoint3* pPoints = (NiPoint3*)pStream->Lock(NiDataStream::LOCK_WRITE);

    NiPoint3 dir = point2 - point1;
    NiPoint3 perp = dir.Perpendicular();
    NiPoint3 up = dir.Cross(perp);
    perp.Unitize();
    up.Unitize();

    pPoints[0] = point1 + (up + perp) * thickness;
    pPoints[1] = point1 + (up - perp) * thickness;
    pPoints[2] = point1 + (-up - perp) * thickness;
    pPoints[3] = point1 + (-up + perp) * thickness;

    pPoints[4] = point2 + (up + perp) * thickness;
    pPoints[5] = point2 + (up - perp) * thickness;
    pPoints[6] = point2 + (-up - perp) * thickness;
    pPoints[7] = point2 + (-up + perp) * thickness;

    pStream->Unlock(NiDataStream::LOCK_WRITE);

    pStreamRef = pMesh->FindStreamRef(NiCommonSemantics::INDEX(), 0);
    if (!pStreamRef)
    {
        pStreamRef = pMesh->AddStream(
            NiCommonSemantics::INDEX(),
            0,
            NiDataStreamElement::F_UINT16_1,
            36,
            NiDataStream::ACCESS_GPU_READ |
            NiDataStream::ACCESS_CPU_WRITE_STATIC,
            NiDataStream::USAGE_VERTEX_INDEX);

        EE_ASSERT(pStreamRef);
        pStream = pStreamRef->GetDataStream();

        efd::UInt16* auiIndices = (efd::UInt16*)pStream->Lock(NiDataStream::LOCK_WRITE);
        int i = 0;
        //Front face
        auiIndices[i++] = 2;
        auiIndices[i++] = 1;
        auiIndices[i++] = 0;
        auiIndices[i++] = 3;
        auiIndices[i++] = 2;
        auiIndices[i++] = 0;

        //Back face
        auiIndices[i++] = 4;
        auiIndices[i++] = 5;
        auiIndices[i++] = 6;
        auiIndices[i++] = 7;
        auiIndices[i++] = 4;
        auiIndices[i++] = 6;

        //Top
        auiIndices[i++] = 0;
        auiIndices[i++] = 4;
        auiIndices[i++] = 7;
        auiIndices[i++] = 3;
        auiIndices[i++] = 0;
        auiIndices[i++] = 7;

        //Bottom
        auiIndices[i++] = 2;
        auiIndices[i++] = 6;
        auiIndices[i++] = 5;
        auiIndices[i++] = 2;
        auiIndices[i++] = 5;
        auiIndices[i++] = 1;

        //Right
        auiIndices[i++] = 0;
        auiIndices[i++] = 1;
        auiIndices[i++] = 5;
        auiIndices[i++] = 0;
        auiIndices[i++] = 5;
        auiIndices[i++] = 4;

        //Left
        auiIndices[i++] = 2;
        auiIndices[i++] = 3;
        auiIndices[i++] = 6;
        auiIndices[i++] = 6;
        auiIndices[i++] = 3;
        auiIndices[i++] = 7;
        pStream->Unlock(NiDataStream::LOCK_WRITE);
    }

}

//------------------------------------------------------------------------------------------------
NiMaterialProperty* PropertyVisualizationHelpers::AttachDefaultMaterial(NiMesh* pMesh)
{
    NiMaterialProperty* pMaterial = NiNew NiMaterialProperty();
    pMaterial->SetAmbientColor(NiColor::WHITE);
    pMaterial->SetDiffuseColor(NiColor::WHITE);
    pMaterial->SetSpecularColor(NiColor::WHITE);
    pMaterial->SetEmittance(NiColor::WHITE);
    pMaterial->SetShineness(0.0f);
    pMaterial->SetAlpha(1.0f);
    pMesh->AttachProperty(pMaterial);
    return pMaterial;
}

//------------------------------------------------------------------------------------------------
SceneGraphService::SceneGraphHandle PropertyVisualizationHelpers::AddMeshToSceneGraphService(
    NiMeshPtr spMesh,
    SceneGraphService* pSceneGraphService)
{
    EE_ASSERT(spMesh);
    EE_ASSERT(pSceneGraphService);

    efd::vector<NiObjectPtr> objects;
    objects.push_back(NiDynamicCast(NiObject, spMesh));

    return pSceneGraphService->AddSceneGraph(objects, true, true);
}

//------------------------------------------------------------------------------------------------
template<class F, class T>
static inline egf::PropertyResult GetPropertyAndCast(
    egf::Entity* pEntity,
    const efd::utf8string& propertyName,
    T& value)
{
    F temp;
    egf::PropertyResult result = pEntity->GetPropertyValue(propertyName, temp);
    value = (T)temp;
    return result;
}

//------------------------------------------------------------------------------------------------
template<class T>
static inline egf::PropertyResult GetPropertyAndConvertToString(
    egf::Entity* pEntity,
    const efd::utf8string& propertyName,
    efd::utf8string& value)
{
    T temp;
    egf::PropertyResult result = pEntity->GetPropertyValue(propertyName, temp);

    if (!efd::ParseHelper<T>::ToString(temp, value))
        return egf::PropertyResult_UnknownError;

    return result;
}

//------------------------------------------------------------------------------------------------
PropertyResult PropertyVisualizationHelpers::GetPropertyAsFloat(
    Entity* pEntity,
    const utf8string& propertyName,
    efd::Float32& value)
{
    ClassID type;
    PropertyResult result = pEntity->GetDataStorageType(propertyName, type);
    if (result != PropertyResult_OK)
        return result;

    switch (type)
    {
    case kTypeID_Float32:
        return GetPropertyAndCast<efd::Float32, efd::Float32>(pEntity, propertyName, value);
    case kTypeID_Float64:
        return GetPropertyAndCast<efd::Float64, efd::Float32>(pEntity, propertyName, value);
    case kTypeID_SInt16:
        return GetPropertyAndCast<efd::SInt16, efd::Float32>(pEntity, propertyName, value);
    case kTypeID_SInt32:
        return GetPropertyAndCast<efd::SInt32, efd::Float32>(pEntity, propertyName, value);
    case kTypeID_SInt64:
        return GetPropertyAndCast<efd::SInt64, efd::Float32>(pEntity, propertyName, value);
    case kTypeID_SInt8:
        return GetPropertyAndCast<efd::SInt8, efd::Float32>(pEntity, propertyName, value);
    case kTypeID_UInt16:
        return GetPropertyAndCast<efd::UInt16, efd::Float32>(pEntity, propertyName, value);
    case kTypeID_UInt32:
        return GetPropertyAndCast<efd::UInt32, efd::Float32>(pEntity, propertyName, value);
    case kTypeID_UInt64:
        return GetPropertyAndCast<efd::UInt64, efd::Float32>(pEntity, propertyName, value);
    case kTypeID_UInt8:
        return GetPropertyAndCast<efd::UInt8, efd::Float32>(pEntity, propertyName, value);
    case kTypeID_Point3:
        {
            Point3 point;
            result = pEntity->GetPropertyValue(propertyName, point);
            value = point.x;
            return result;
        }
    default:
        return PropertyResult_TypeMismatch;
    }
}

//------------------------------------------------------------------------------------------------
PropertyResult PropertyVisualizationHelpers::GetPropertyAsString(
    Entity* pEntity,
    const utf8string& propertyName,
    utf8string& value)
{
    ClassID type;
    PropertyResult result = pEntity->GetDataStorageType(propertyName, type);
    if (result != PropertyResult_OK)
        return result;

    switch (type)
    {
    case kTypeID_Bool:
        return GetPropertyAndConvertToString<efd::Bool>(pEntity, propertyName, value);
    case kTypeID_Utf8Char:
        return GetPropertyAndConvertToString<efd::utf8char_t>(pEntity, propertyName, value);
    case kTypeID_SInt8:
        return GetPropertyAndConvertToString<efd::SInt8>(pEntity, propertyName, value);
    case kTypeID_UInt8:
        return GetPropertyAndConvertToString<efd::UInt8>(pEntity, propertyName, value);
    case kTypeID_SInt16:
        return GetPropertyAndConvertToString<efd::SInt16>(pEntity, propertyName, value);
    case kTypeID_UInt16:
        return GetPropertyAndConvertToString<efd::UInt16>(pEntity, propertyName, value);
    case kTypeID_SInt32:
        return GetPropertyAndConvertToString<efd::SInt32>(pEntity, propertyName, value);
    case kTypeID_UInt32:
        return GetPropertyAndConvertToString<efd::UInt32>(pEntity, propertyName, value);
    case kTypeID_SInt64:
        return GetPropertyAndConvertToString<efd::SInt64>(pEntity, propertyName, value);
    case kTypeID_UInt64:
        return GetPropertyAndConvertToString<efd::UInt64>(pEntity, propertyName, value);
    case kTypeID_Float32:
        return GetPropertyAndConvertToString<efd::Float32>(pEntity, propertyName, value);
    case kTypeID_Float64:
        return GetPropertyAndConvertToString<efd::Float64>(pEntity, propertyName, value);
    case kTypeID_utf8string:
        return GetPropertyAndConvertToString<efd::utf8string>(pEntity, propertyName, value);
    case kTypeID_Point2:
        return GetPropertyAndConvertToString<efd::Point2>(pEntity, propertyName, value);
    case kTypeID_Point3:
        return GetPropertyAndConvertToString<efd::Point3>(pEntity, propertyName, value);
    case kTypeID_Point4:
        return GetPropertyAndConvertToString<efd::Point4>(pEntity, propertyName, value);
    case kTypeID_Matrix3:
        return GetPropertyAndConvertToString<efd::Matrix3>(pEntity, propertyName, value);
    case kTypeID_Color:
        return GetPropertyAndConvertToString<efd::Color>(pEntity, propertyName, value);
    case kTypeID_ColorA:
        return GetPropertyAndConvertToString<efd::ColorA>(pEntity, propertyName, value);
    case kTypeID_AssetID:
        return GetPropertyAndConvertToString<efd::AssetID>(pEntity, propertyName, value);
    default:
        return PropertyResult_TypeMismatch;
    }
}

//------------------------------------------------------------------------------------------------
void PropertyVisualizationHelpers::CreateSphereVerts(
    NiPoint3* pPoints,
    efd::Float32 radius,
    efd::UInt32 numSlices,
    efd::UInt32 numStacks)
{
    EE_ASSERT(pPoints);
    EE_ASSERT(numSlices > 0);
    EE_ASSERT(numStacks > 0);

    *pPoints++ = NiPoint3(0, 0, radius);

    for (efd::UInt32 st = 0; st < numStacks - 1; st++)
    {
        efd::Float32 fAngle = (efd::Float32)(st + 1) / (efd::Float32)(numStacks) * NI_PI;

        efd::Float32 fZ;
        efd::Float32 fStackRadius;
        NiSinCos(fAngle, fStackRadius, fZ);
        fZ *= radius;
        fStackRadius *= radius;

        for (efd::UInt32 sl = 0; sl < numSlices; sl++)
        {
            efd::Float32 fAng = NI_TWO_PI * (efd::Float32)sl / (efd::Float32)numSlices;
            efd::Float32 fX;
            efd::Float32 fY;
            NiSinCos(fAng, fX, fY);

            pPoints[sl] = NiPoint3(fX * fStackRadius, fY * fStackRadius,
                fZ);
        }

        pPoints += numSlices;
    }

    *pPoints++ = NiPoint3(0, 0, -radius);
}

//------------------------------------------------------------------------------------------------
egf::PropertyResult egmVisualizers::PropertyVisualizationHelpers::GetDependency(
    egf::ExtraDataPtr spExtraData,
    egf::Entity* pEntity,
    const efd::utf8string& dependency,
    const efd::utf8string& defaultProperty,
    efd::Float32& value)
{
    egf::ExtraDataEntryPtr entry = spExtraData->GetEntry(dependency);
    efd::utf8string propertyName;

    if (!entry || entry->m_value == "Default")
        propertyName = defaultProperty;
    else
        propertyName = entry->m_value;

    egf::PropertyResult result =
        egmVisualizers::PropertyVisualizationHelpers::GetPropertyAsFloat(
        pEntity,
        propertyName,
        value);

    return result;
}

//------------------------------------------------------------------------------------------------
