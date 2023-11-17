// EMERGENT GAME TECHNOLOGIES PROPRIETARY INFORMATION
//
// This software is supplied under the terms of a license agreement or
// nondisclosure agreement with Emergent Game Technologies and may not 
// be copied or disclosed except in accordance with the terms of that 
// agreement.
//
//      Copyright (c) 1996-2008 Emergent Game Technologies.
//      All Rights Reserved.
//
// Emergent Game Technologies, Chapel Hill, North Carolina 27517
// http://www.emergent.net

#include "NiMeshFBXExporter.h"
#include "NiFBXExporter.h"
#include "NiFBXUtility.h"
#include <NiDevImageConverter.h>


#pragma warning(disable:4996)
#include <fbxsdk.h>

template<typename INDEX_TYPE>
class FbxPrimitiveFunctor
{
public:
    KFbxMesh* m_pkFbxMesh;
    int m_iMaterialIndex;

    FbxPrimitiveFunctor(KFbxMesh* pkFbxMesh, int iMaterialIndex = -1)
    {
        m_pkFbxMesh = pkFbxMesh;
        m_iMaterialIndex = iMaterialIndex;
    }

    bool operator () (const NiUInt32* pIndices,
        NiUInt32 uiIndexCount, NiUInt32 uiPrimitiveIdx, NiUInt16)
    {
        m_pkFbxMesh->BeginPolygon(m_iMaterialIndex); // Material index.
        m_pkFbxMesh->AddPolygon(pIndices[0]); 
        m_pkFbxMesh->AddPolygon(pIndices[1]); 
        m_pkFbxMesh->AddPolygon(pIndices[2]); 
        m_pkFbxMesh->EndPolygon();
        return true;
    }
};
//-----------------------------------------------------------------------------------------------
NiMeshFBXExporter::NiMeshFBXExporter(KFbxSdkManager* pkSdkManager, NiString kTexturePath) 
    : NiRefObject()
{
    m_pkSdkManager = pkSdkManager;
}
//-----------------------------------------------------------------------------------------------
NiMeshFBXExporter::~NiMeshFBXExporter() 
{

}
//-----------------------------------------------------------------------------------------------
KFbxMesh* NiMeshFBXExporter::ConvertMesh(NiMesh* pkMesh, NiString kName)
{
    if (!NiIsExactKindOf(NiMesh, pkMesh))
        return NULL;

    if (pkMesh->GetPrimitiveType() != NiPrimitiveType::PRIMITIVE_TRIANGLES &&
        pkMesh->GetPrimitiveType() != NiPrimitiveType::PRIMITIVE_TRISTRIPS)
    {
        return NULL;
    }

    if (pkMesh->GetSubmeshCount() != 1)
        return NULL;

    if (pkMesh->GetInstanced())
        return NULL;

    KFbxMesh* pkFbxMesh = KFbxMesh::Create(m_pkSdkManager, kName);

    // Create vertex position control points.
    pkFbxMesh->InitControlPoints(pkMesh->GetVertexCount(0));
    KFbxVector4* pkFbxControlPoints = pkFbxMesh->GetControlPoints();
    // Convert position data...
    bool posConverted = false;
    // Try the default conversion first...
    {
        NiDataStreamElementLock kPosLock(pkMesh, NiCommonSemantics::POSITION(), 0,
            NiDataStreamElement::F_FLOAT32_3, NiDataStream::LOCK_TOOL_READ);

        if (kPosLock.DataStreamExists() && kPosLock.IsLocked())
        {
            NiTStridedRandomAccessIterator<NiPoint3> kIter = kPosLock.begin<NiPoint3>();
            for (NiUInt32 ui = 0; ui < pkMesh->GetVertexCount(0); ui++)
            {
                pkFbxControlPoints[ui] = NiFBXUtility::ConvertNiPoint(kIter[ui]);
            }
            posConverted = true;
        }        
    }

    // Now try the terrain conversion
    if (!posConverted)
    {
        NiDataStreamElementLock kPosLock(pkMesh, NiCommonSemantics::POSITION(), 0,
            NiDataStreamElement::F_FLOAT32_4, NiDataStream::LOCK_TOOL_READ);

        if (kPosLock.DataStreamExists() && kPosLock.IsLocked())
        {
            NiTStridedRandomAccessIterator<NiTSimpleArray<float,4>> kIter = kPosLock.begin<NiTSimpleArray<float,4>>();
            for (NiUInt32 ui = 0; ui < pkMesh->GetVertexCount(0); ui++)
            {
                pkFbxControlPoints[ui] = KFbxVector4(kIter[ui][0], kIter[ui][1], kIter[ui][2]);
            }
            posConverted = true;
        }        
    }

    if (posConverted == false)
    {
        pkFbxMesh->Destroy();
        return NULL;
    }


    // specify normals per control point.
    KFbxLayer* pkFbxLayer = pkFbxMesh->GetLayer(0);
    if (pkFbxLayer == NULL)
    {
        pkFbxMesh->CreateLayer();
        pkFbxLayer = pkFbxMesh->GetLayer(0);
    }

    // Convert normal data...
    {
        NiDataStreamElementLock kNormalLock(pkMesh, NiCommonSemantics::NORMAL(), 0,
            NiDataStreamElement::F_FLOAT32_3, NiDataStream::LOCK_TOOL_READ);
        
        if (kNormalLock.DataStreamExists() && kNormalLock.IsLocked())
        {
            KFbxLayerElementNormal* pkFbxNormalLayer = KFbxLayerElementNormal::Create(pkFbxMesh, "");
            pkFbxNormalLayer->SetMappingMode(KFbxLayerElement::eBY_CONTROL_POINT);
            pkFbxNormalLayer->SetReferenceMode(KFbxLayerElement::eDIRECT);
    
            NiTStridedRandomAccessIterator<NiPoint3> kIter = kNormalLock.begin<NiPoint3>();
            for (NiUInt32 ui = 0; ui < pkMesh->GetVertexCount(0); ui++)
            {
                 pkFbxNormalLayer->GetDirectArray().Add(NiFBXUtility::ConvertNiPoint(kIter[ui]));
            }

            pkFbxLayer->SetNormals(pkFbxNormalLayer);
        }
    }

    // Set material mapping.
    KFbxLayerElementMaterial* pkFbxMaterialLayer = KFbxLayerElementMaterial::Create(pkFbxMesh, "");
    pkFbxMaterialLayer->SetMappingMode(KFbxLayerElement::eBY_POLYGON);
    pkFbxMaterialLayer->SetReferenceMode(KFbxLayerElement::eINDEX_TO_DIRECT);
    pkFbxLayer->SetMaterials(pkFbxMaterialLayer);

    // Create polygons. Assign material indices.
    FbxPrimitiveFunctor<NiUInt32> kFunc(pkFbxMesh, 0);
    NiMeshAlgorithms::ForEachPrimitiveOneSubmesh(pkMesh, 0, kFunc, NiDataStream::LOCK_TOOL_READ);

    return pkFbxMesh;
}
//-----------------------------------------------------------------------------------------------
KFbxSurfacePhong* NiMeshFBXExporter::ConvertMaterial(KFbxMesh* pMesh, NiMesh* pkMesh, 
    NiString kName)
{
    KFbxSurfacePhong* pMaterial = KFbxSurfacePhong::Create(m_pkSdkManager, kName);

    NiPropertyState* pkState = pkMesh->GetPropertyState();
    if (pkState)
    {
        NiMaterialProperty* pkMatProp = pkState->GetMaterial();
        pMaterial->GetEmissiveColor().Set(NiFBXUtility::ConvertNiColorToDouble3(
            pkMatProp->GetEmittance()));
        pMaterial->GetAmbientColor().Set(NiFBXUtility::ConvertNiColorToDouble3(
            pkMatProp->GetAmbientColor()));
        pMaterial->GetDiffuseColor().Set(NiFBXUtility::ConvertNiColorToDouble3(
            pkMatProp->GetDiffuseColor()));
        pMaterial->GetTransparencyFactor().Set((double)pkMatProp->GetAlpha());
        pMaterial->GetShininess().Set((double)pkMatProp->GetShineness());
    }
    
    pMaterial->GetShadingModel().Set("Phong");
    return pMaterial;
}
//-----------------------------------------------------------------------------------------------
int NiMeshFBXExporter::GetUVSetIndex(NiMesh* pkMesh, unsigned int uiMap)
{
    NiTexturingProperty* pkTexProp = 
        (NiTexturingProperty*)pkMesh->GetProperty(NiProperty::TEXTURING);
    if (pkTexProp)
    {
        NiTexturingProperty::Map* pkShaderMap = NULL;

        switch (uiMap)
        {
            case NiTexturingProperty::BASE_INDEX:
                pkShaderMap = pkTexProp->GetBaseMap();
                break;
        }
        
        if (pkShaderMap)
            return pkShaderMap->GetTextureIndex();
    }
    return -1;
}
//-----------------------------------------------------------------------------------------------
