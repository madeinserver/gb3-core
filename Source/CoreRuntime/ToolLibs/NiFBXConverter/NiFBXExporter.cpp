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


#include "NiFBXExporter.h"
#include "NiMeshFBXExporter.h"
#include <NiTNodeTraversal.h>
#include "NiFBXUtility.h"

//-----------------------------------------------------------------------------------------------
NiFBXExporter::NiFBXExporter() 
    : NiRefObject()
{
    // The first thing to do is to create the FBX SDK manager which is the 
    // object allocator for almost all the classes in the SDK.
    m_pkSdkManager = KFbxSdkManager::Create();

    m_pkFbxScene = KFbxScene::Create(m_pkSdkManager,"");

    if (m_pkFbxScene)
    {
        // create scene info
        KFbxDocumentInfo* sceneInfo = KFbxDocumentInfo::Create(m_pkSdkManager, "SceneInfo");
        sceneInfo->mTitle = "NiFBXExporter-generated scene";
        m_pkFbxScene->SetSceneInfo(sceneInfo);

        KFbxGlobalSettings& kSettings = m_pkFbxScene->GetGlobalSettings();
        kSettings.SetAxisSystem(KFbxAxisSystem(KFbxAxisSystem::eMax));

    }
}
//-----------------------------------------------------------------------------------------------
NiFBXExporter::~NiFBXExporter() 
{
    if (m_pkFbxScene)
        m_pkFbxScene->Destroy();
    if (m_pkSdkManager)
        m_pkSdkManager->Destroy();
}
//-----------------------------------------------------------------------------------------------
bool NiFBXExporter::IsValid() const
{
    return m_pkSdkManager != NULL;
}
//-----------------------------------------------------------------------------------------------
void NiFBXExporter::AddFbxPropertyDouble(
    KFbxObject *object, 
    const NiString& propertyName, 
    double value)
{
    KFbxProperty p = KFbxProperty::Create(object, propertyName, DTDouble);
    p.Set(value);
}
//-----------------------------------------------------------------------------------------------
void NiFBXExporter::AddFbxPropertyInt(
    KFbxObject *object, 
    const NiString& propertyName, 
    int value)
{
    KFbxProperty p = KFbxProperty::Create(object, propertyName, DTInteger);
    p.Set(value);
}
//-----------------------------------------------------------------------------------------------
void NiFBXExporter::AddFbxPropertyBool(
    KFbxObject *object, 
    const NiString& propertyName, 
    bool value)
{
    KFbxProperty p = KFbxProperty::Create(object, propertyName, DTBool);
    p.Set(value);
}
//-----------------------------------------------------------------------------------------------
void NiFBXExporter::AddSceneGraph(NiAVObject* pkObj, NiString kName)
{
    if (pkObj)
    {
        // Create node
        m_pkActiveEntityObjectNode = KFbxNode::Create(m_pkSdkManager, kName + "_EntityRoot");
        m_kActiveName = kName;
        m_uiNodeId = 0;
        NiFBXUtility::ConvertTransform(pkObj, m_pkActiveEntityObjectNode);
        
        // Add to root.
        KFbxNode* pkRootNode = m_pkFbxScene->GetRootNode();
        pkRootNode->AddChild(m_pkActiveEntityObjectNode);

        if (NiIsKindOf(NiNode, pkObj))
        {
            NiNode* pkNode = (NiNode*) pkObj;
            for (efd::UInt32 ui = 0; ui < pkNode->GetArrayCount(); ui++)
            {
                NiAVObject* pkChild = pkNode->GetAt(ui);
                if (pkChild)
                {
                    ConvertSceneGraph(pkChild, m_pkActiveEntityObjectNode);
                }
            }
        }
    }
}
//-----------------------------------------------------------------------------------------------
NiString NiFBXExporter::CreateName(const NiString& kName)
{
    return m_kActiveName + "_" + kName + "_" + NiString::FromUInt(m_uiNodeId++);
}
//-----------------------------------------------------------------------------------------------
bool NiFBXExporter::ConvertSceneGraph(NiAVObject* pkObj, KFbxNode* pkFbxParentNode)
{
    KFbxNode* pkFbxNode = KFbxNode::Create(pkFbxParentNode, CreateName(pkObj->GetName()));
    NiFBXUtility::ConvertTransform(pkObj, pkFbxNode);
    pkFbxNode->SetVisibility(!pkObj->GetAppCulled());
    pkFbxParentNode->AddChild(pkFbxNode);

    if (NiIsKindOf(NiNode, pkObj))
    {
        NiNode* pkNode = (NiNode*) pkObj;
        for (efd::UInt32 ui = 0; ui < pkNode->GetArrayCount(); ui++)
        {
            NiAVObject* pkChild = pkNode->GetAt(ui);
            if (pkChild)
            {
                ConvertSceneGraph(pkChild, pkFbxNode);
            }
        }

        return true;
    }

    if (NiIsExactKindOf(NiMesh, pkObj))
    {
        NiMesh* pkMesh = (NiMesh*) pkObj;
        NiMeshFBXExporter kConverter(m_pkSdkManager, "");
        KFbxMesh* pkFbxMesh = kConverter.ConvertMesh(pkMesh, CreateName((NiString)pkObj->GetName() + 
            "Mesh"));
        if (pkFbxMesh)
        {
            pkFbxNode->SetNodeAttribute(pkFbxMesh);
            pkFbxNode->AddMaterial(kConverter.ConvertMaterial(pkFbxMesh, pkMesh, CreateName("Material")));
        }
        return true;
    }

    return true;
}
//-----------------------------------------------------------------------------------------------
// The code for SaveScene is from the file Common.cxx in the FbxSdk
bool NiFBXExporter::SaveScene(const char* pFilename, int pFileFormat, bool pEmbedMedia)
{	
    int lMajor, lMinor, lRevision;
    bool lStatus = true;

    // Create an exporter.
    KFbxExporter* lExporter = KFbxExporter::Create(m_pkSdkManager, "");

    if( pFileFormat < 0 || pFileFormat >= m_pkSdkManager->GetIOPluginRegistry()->GetWriterFormatCount() )
    {
        // Write in fall back format if pEmbedMedia is true
        pFileFormat = m_pkSdkManager->GetIOPluginRegistry()->GetNativeWriterFormat();

        if (!pEmbedMedia)
        {
            //Try to export in ASCII if possible
            int lFormatIndex, lFormatCount = m_pkSdkManager->GetIOPluginRegistry()->GetWriterFormatCount();

            for (lFormatIndex=0; lFormatIndex<lFormatCount; lFormatIndex++)
            {
                if (m_pkSdkManager->GetIOPluginRegistry()->WriterIsFBX(lFormatIndex))
                {
                    KString lDesc =m_pkSdkManager->GetIOPluginRegistry()->GetWriterFormatDescription(lFormatIndex);
                    char *lASCII = "ascii";
                    if (lDesc.Find(lASCII)>=0)
                    {
                        pFileFormat = lFormatIndex;
                        break;
                    }
                }
            }
        }
    }

    // Set the export states. By default, the export states are always set to 
    // true except for the option eEXPORT_TEXTURE_AS_EMBEDDED. The code below 
    // shows how to change these states.

    IOSREF.SetBoolProp(EXP_FBX_MATERIAL,        true);
    IOSREF.SetBoolProp(EXP_FBX_TEXTURE,         true);
    IOSREF.SetBoolProp(EXP_FBX_EMBEDDED,        pEmbedMedia);
    IOSREF.SetBoolProp(EXP_FBX_SHAPE,           true);
    IOSREF.SetBoolProp(EXP_FBX_GOBO,            true);
    IOSREF.SetBoolProp(EXP_FBX_ANIMATION,       true);
    IOSREF.SetBoolProp(EXP_FBX_GLOBAL_SETTINGS, true);

    // Initialize the exporter by providing a filename.
    if(lExporter->Initialize(pFilename, pFileFormat) == false)
    {
        printf("Call to KFbxExporter::Initialize() failed.\n");
        printf("Error returned: %s\n\n", lExporter->GetLastErrorString());
        return false;
    }

    KFbxSdkManager::GetFileFormatVersion(lMajor, lMinor, lRevision);
    printf("FBX version number for this version of the FBX SDK is %d.%d.%d\n\n", lMajor, lMinor, lRevision);

    // Export the scene.
    lStatus = lExporter->Export(m_pkFbxScene); 

    // Destroy the exporter.
    lExporter->Destroy();
    return lStatus;
}
//-----------------------------------------------------------------------------------------------