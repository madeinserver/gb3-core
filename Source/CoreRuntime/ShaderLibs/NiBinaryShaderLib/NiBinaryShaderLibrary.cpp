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
//------------------------------------------------------------------------------------------------
// Precompiled Header
#include "NiBinaryShaderLibPCH.h"

#include "NiBinaryShaderLibrary.h"
#include "NSBLoader.h"
#include "NSBShader.h"
#include "NSBUtility.h"
#include "NSBPass.h"
#include "NSBAttributeDesc.h"
#include "NSBTextureStage.h"
#include "NSBTexture.h"

#include <NiShaderLibraryDesc.h>

//------------------------------------------------------------------------------------------------
NiBinaryShaderLibrary* NiBinaryShaderLibrary::ms_pkShaderLibrary = NULL;
NiBinaryShaderLibrary::CREATENIBINARYSHADER NiBinaryShaderLibrary::ms_pfnCreateNiBinaryShader =
    NiBinaryShaderLibrary::DefaultCreateNiBinaryShader;

//------------------------------------------------------------------------------------------------
NiBinaryShaderLibrary::ShaderSet::ShaderSet() :
    m_uiNumImplementations(1)
{
    m_pspImplementationArray = NiNew NiShaderPtr[m_uiNumImplementations];
}

//------------------------------------------------------------------------------------------------
NiBinaryShaderLibrary::ShaderSet::~ShaderSet()
{
    for (unsigned int i = 0; i < m_uiNumImplementations; i++)
    {
        m_pspImplementationArray[i] = 0;
    }
    NiDelete[] m_pspImplementationArray;
}

//------------------------------------------------------------------------------------------------
unsigned int NiBinaryShaderLibrary::ShaderSet::GetImplementationCount() const
{
    return m_uiNumImplementations;
}

//------------------------------------------------------------------------------------------------
void NiBinaryShaderLibrary::ShaderSet::InsertShader(NiShader* pkShader)
{
    EE_ASSERT(pkShader->GetImplementation() != NiShader::DEFAULT_IMPLEMENTATION);
    if (pkShader->GetImplementation() >= m_uiNumImplementations)
    {
        NiShaderPtr* pspNewArray = NiNew NiShaderPtr[pkShader->GetImplementation() + 1];
        for (unsigned int i = 0; i < m_uiNumImplementations; i++)
        {
            pspNewArray[i] = m_pspImplementationArray[i];
            m_pspImplementationArray[i] = 0;
        }
        NiDelete[] m_pspImplementationArray;
        m_pspImplementationArray = pspNewArray;
        m_uiNumImplementations = pkShader->GetImplementation();
    }
    m_pspImplementationArray[pkShader->GetImplementation()] = pkShader;
}

//------------------------------------------------------------------------------------------------
NiShader* NiBinaryShaderLibrary::ShaderSet::GetShader(unsigned int uiImplementation) const
{
    EE_ASSERT(uiImplementation < m_uiNumImplementations);
    return m_pspImplementationArray[uiImplementation];
}

//------------------------------------------------------------------------------------------------
void NiBinaryShaderLibrary::ShaderSet::RemoveShader(unsigned int uiImplementation)
{
    EE_ASSERT(uiImplementation < m_uiNumImplementations);
    m_pspImplementationArray[uiImplementation] = 0;
}

//------------------------------------------------------------------------------------------------
NiBinaryShaderLibrary::NiBinaryShaderLibrary() :
    NiD3DShaderLibrary("NiBinaryShaderLib"),
    m_pkNSBLoader(NULL)
{
    m_pkNSBLoader = NSBLoader::GetInstance();

    EE_ASSERT(m_pkNSBLoader);
}

//------------------------------------------------------------------------------------------------
NiBinaryShaderLibrary::~NiBinaryShaderLibrary()
{
    NiOutputDebugString("Releasing Binary Shaders!\n");

    NiTMapIterator kIter = m_kShaders.GetFirstPos();
    while (kIter)
    {
        ShaderSet* pkShaderSet = NULL;
        const char* pcKey = NULL;
        m_kShaders.GetNext(kIter, pcKey, pkShaderSet);
        EE_ASSERT(pkShaderSet);

        NiDelete pkShaderSet;
    }

    NSBLoader::Release();

    ms_pkShaderLibrary = NULL;
}

//------------------------------------------------------------------------------------------------
NiBinaryShaderLibrary* NiBinaryShaderLibrary::Create(int iDirectoryCount,
    const char* apcDirectories[], bool bRecurseSubFolders)
{
    if (ms_pkShaderLibrary == NULL)
    {
        NiBinaryShaderLibrary* pkShaderLibrary = NiNew NiBinaryShaderLibrary();
        ms_pkShaderLibrary = pkShaderLibrary;
    }

    EE_ASSERT(ms_pkShaderLibrary);

    for (int iCount = 0; iCount < iDirectoryCount; iCount++)
    {
        if (!ms_pkShaderLibrary->LoadNSBShaders(
            apcDirectories[iCount], bRecurseSubFolders))
        {
            NiRenderer::Warning("LoadNSBShaders failed on %s\n",
                apcDirectories[iCount]);
        }
    }

    return ms_pkShaderLibrary;
}

//------------------------------------------------------------------------------------------------
void NiBinaryShaderLibrary::Shutdown()
{
}

//------------------------------------------------------------------------------------------------
NiBinaryShaderLibrary* NiBinaryShaderLibrary::GetLibrary()
{
    EE_FAIL("Who is calling this???");
    return 0;
}

//------------------------------------------------------------------------------------------------
NiShaderLibraryDesc* NiBinaryShaderLibrary::GetShaderLibraryDesc()
{
    return NiD3DShaderLibrary::GetShaderLibraryDesc();
}

//------------------------------------------------------------------------------------------------
void NiBinaryShaderLibrary::SetShaderLibraryDesc(NiShaderLibraryDesc* pkDesc)
{
    NiD3DShaderLibrary::SetShaderLibraryDesc(pkDesc);
}

//------------------------------------------------------------------------------------------------
NiShader* NiBinaryShaderLibrary::GetShader(NiRenderer* pkRenderer,
    const char* pcName, unsigned int uiImplementation)
{
    // D3D10, D3D11 renderers not supported in this library
    if (pkRenderer->GetRendererID() == efd::SystemDesc::RENDERER_D3D10 ||
        pkRenderer->GetRendererID() == efd::SystemDesc::RENDERER_D3D11)
    {
        return NULL;
    }

    // First, load from saved shaders
    ShaderSet* pkShaderSet = NULL;
    if (m_kShaders.GetAt(pcName, pkShaderSet))
    {
        EE_ASSERT(pkShaderSet);
        if (uiImplementation < pkShaderSet->GetImplementationCount())
        {
            NiShader* pkShader = pkShaderSet->GetShader(uiImplementation);
            if (pkShader)
                return pkShader;
        }
    }

    // Then, load from NSB shader
    NSBShader* pkNSBShader = m_pkNSBLoader->GetNSBShader(pcName);
    if (pkNSBShader)
    {
        return CreateNiBinaryShaderFromNSBShader(pkNSBShader, uiImplementation);
    }

    // Didn't exist?
    return NULL;
}

//------------------------------------------------------------------------------------------------
bool NiBinaryShaderLibrary::ReleaseShader(const char* pcName,
    unsigned int uiImplementation)
{
    ShaderSet* pkShaderSet = NULL;
    if (m_kShaders.GetAt(pcName, pkShaderSet))
    {
        EE_ASSERT(pkShaderSet != NULL);

        if (uiImplementation < pkShaderSet->GetImplementationCount())
        {
            bool bReturn = (pkShaderSet->GetShader(uiImplementation) != NULL);
            pkShaderSet->RemoveShader(uiImplementation);
            return bReturn;
        }
    }

    return false;
}

//------------------------------------------------------------------------------------------------
bool NiBinaryShaderLibrary::ReleaseShader(NiShader* pkShader)
{
    if (pkShader == NULL)
        return false;
    return ReleaseShader(pkShader->GetName(), pkShader->GetImplementation());
}

//------------------------------------------------------------------------------------------------
bool NiBinaryShaderLibrary::RegisterNewShader(
    NSBShader* pkNSBShader,
    NiBinaryShader* pkBinaryShader)
{
    EE_ASSERT(pkNSBShader);
    EE_ASSERT(pkBinaryShader);
    EE_UNUSED_ARG(pkNSBShader);

    // Store the shader internally
    ShaderSet* pkShaderSet = NULL;
    const char* pcName = pkBinaryShader->GetName();
    EE_ASSERT(pkBinaryShader->GetImplementation() != NiShader::DEFAULT_IMPLEMENTATION);

    if (m_kShaders.GetAt(pcName, pkShaderSet))
    {
        EE_ASSERT(pkShaderSet != NULL);
    }
    else
    {
        pkShaderSet = NiNew ShaderSet;
        m_kShaders.SetAt(pkBinaryShader->GetName(), pkShaderSet);
    }
    pkShaderSet->InsertShader(pkBinaryShader);

    return true;
}

//------------------------------------------------------------------------------------------------
void NiBinaryShaderLibrary::AddShaderDesc(NSBShader* pkNSBShader)
{
    // Add shader descriptions to library desc
    if (m_spShaderLibraryDesc == 0)
        m_spShaderLibraryDesc = NiNew NiShaderLibraryDesc();
    EE_ASSERT(m_spShaderLibraryDesc);

    NiShaderDesc* pkDesc = pkNSBShader->GetShaderDesc();
    EE_ASSERT(pkDesc);
    m_spShaderLibraryDesc->AddShaderDesc(pkDesc);

    m_spShaderLibraryDesc->AddVertexShaderVersion(
        pkDesc->GetMinVertexShaderVersion());
    m_spShaderLibraryDesc->AddVertexShaderVersion(
        pkDesc->GetMaxVertexShaderVersion());
    m_spShaderLibraryDesc->AddPixelShaderVersion(
        pkDesc->GetMinPixelShaderVersion());
    m_spShaderLibraryDesc->AddPixelShaderVersion(
        pkDesc->GetMaxPixelShaderVersion());
    m_spShaderLibraryDesc->AddUserDefinedVersion(
        pkDesc->GetMinUserDefinedVersion());
    m_spShaderLibraryDesc->AddUserDefinedVersion(
        pkDesc->GetMaxUserDefinedVersion());
    m_spShaderLibraryDesc->AddPlatformFlags(
        pkDesc->GetPlatformFlags());
}

//------------------------------------------------------------------------------------------------
bool NiBinaryShaderLibrary::LoadNSBShaders(const char* pcDirectory,
    bool bRecurseSubFolders)
{
    bool bResult = m_pkNSBLoader->LoadAllNSBFiles(pcDirectory,
        bRecurseSubFolders);

    NiTListIterator kIter = 0;
    NSBShader* pkNSBShader = m_pkNSBLoader->GetFirstNSBShader(kIter);
    while (pkNSBShader)
    {
        AddShaderDesc(pkNSBShader);
        pkNSBShader = m_pkNSBLoader->GetNextNSBShader(kIter);
    }

    return bResult;
}

//------------------------------------------------------------------------------------------------
bool NiBinaryShaderLibrary::LoadShader(const char* pcFile)
{
    bool bResult = m_pkNSBLoader->LoadNSBFile(pcFile);

    if (bResult)
    {
        NiFilename kFilename(pcFile);
        NSBShader* pkNSBShader = m_pkNSBLoader->GetNSBShader(kFilename.GetFilename());
        if (pkNSBShader)
            AddShaderDesc(pkNSBShader);
    }

    return bResult;
}

//------------------------------------------------------------------------------------------------
NiBinaryShaderLibrary::CREATENIBINARYSHADER
NiBinaryShaderLibrary::SetCreateNiBinaryShaderCallback(
    NiBinaryShaderLibrary::CREATENIBINARYSHADER pfnCallback)
{
    CREATENIBINARYSHADER pfnOld = ms_pfnCreateNiBinaryShader;

    if (pfnCallback)
        ms_pfnCreateNiBinaryShader = pfnCallback;
    else
        ms_pfnCreateNiBinaryShader = DefaultCreateNiBinaryShader;

    return pfnOld;
}

//------------------------------------------------------------------------------------------------
NiBinaryShaderLibrary::CREATENIBINARYSHADER
NiBinaryShaderLibrary::GetCreateNiBinaryShaderCallback()
{
    return ms_pfnCreateNiBinaryShader;
}

//------------------------------------------------------------------------------------------------
NiBinaryShader* NiBinaryShaderLibrary::DefaultCreateNiBinaryShader(
    const char* pcClassName)
{
    if (pcClassName && strcmp(pcClassName, ""))
    {
        NiRenderer::Warning("NiBinaryShaderLibrary::DefaultCreateNiBinaryShader "
            "called for class name %s\n"
            "   Did you intend to have a callback set for this???\n",
            pcClassName);
    }

    return NiNew NiBinaryShader;
}

//------------------------------------------------------------------------------------------------
unsigned int NiBinaryShaderLibrary::GetNumSupportedMimeTypes() const
{
    return 1;
}

//------------------------------------------------------------------------------------------------
const char* NiBinaryShaderLibrary::GetSupportedMimeType(unsigned int uiIdx) const
{
    EE_ASSERT_MESSAGE(!uiIdx, ("Invalid index"));
    EE_UNUSED_ARG(uiIdx);
    return "gamebryo-binary-shader";
}

//------------------------------------------------------------------------------------------------
bool NiBinaryShaderLibrary::RegisterTextureStageGlobals(
    NSBShader* pkShader,
    NSBImplementation* pkImplementation)
{
    bool bResult = true;

#if !defined(_PS3)
    // If texture stages use any globals for the transform, we need to
    // register them w/ the renderer so that things line up correctly
    // when we are converting them.
    unsigned int uiPassCount = pkImplementation->GetPassCount();
    for (unsigned int ui = 0; ui < uiPassCount; ui++)
    {
        NSBPass* pkPass = pkImplementation->GetPass(ui, false);
        if (pkPass)
        {
            unsigned int uiStageCount = pkPass->GetStageCount();
            for (unsigned int uj = 0; uj < uiStageCount; uj++)
            {
                NSBTextureStage* pkStage = pkPass->GetStage(uj, false);
                if (pkStage)
                {
                    // See if it have a global transformation
                    unsigned int uiFlags =
                        pkStage->GetTextureTransformFlags();
                    if ((uiFlags & NiTextureStage::TSTTF_SOURCE_MASK) ==
                        NiTextureStage::TSTTF_GLOBAL)
                    {
                        // Grab the entry for the global table
                        NSBAttributeDesc* pkDesc =
                            pkShader->GetGlobalAttributeTable()->GetAttributeByName(
                            (char*)pkStage->GetGlobalName());
                        if (!pkDesc)
                        {
                            // Log this error???
                            bResult = false;
                            continue;
                        }

                        EE_ASSERT(NiShaderConstantMapEntry::IsMatrix4(
                            pkDesc->GetType()));

                        float afData[16];
                        float* pfData = afData;
                        pkDesc->GetValue_Matrix4(pfData, 16 * sizeof(float));
                        // Register it w/ the renderer
                        NiD3DShaderFactory::RegisterGlobalShaderConstant(
                            pkDesc->GetName(), pkDesc->GetType(),
                            sizeof(float) * 16, afData);
                    }
                }
            }
        }
    }
#endif

    return bResult;
}

//------------------------------------------------------------------------------------------------
bool NiBinaryShaderLibrary::ReleaseTextureStageGlobals(
    NSBShader* pkShader,
    NSBImplementation* pkImplementation)
{
    bool bResult = true;

#if !defined(_PS3)
    // This function will release the globals in texture stages. It is called
    // after the shader is setup, so that the entries will properly be
    // released when the texture stage is released.
    unsigned int uiPassCount = pkImplementation->GetPassCount();
    for (unsigned int ui = 0; ui < uiPassCount; ui++)
    {
        NSBPass* pkPass = pkImplementation->GetPass(ui, false);
        if (pkPass)
        {
            unsigned int uiStageCount = pkPass->GetStageCount();
            for (unsigned int uj = 0; uj < uiStageCount; uj++)
            {
                NSBTextureStage* pkStage = pkPass->GetStage(uj, false);
                if (pkStage)
                {
                    // See if it have a global transformation
                    unsigned int uiFlags =
                        pkStage->GetTextureTransformFlags();
                    if ((uiFlags & NiTextureStage::TSTTF_SOURCE_MASK) ==
                        NiTextureStage::TSTTF_GLOBAL)
                    {
                        // Grab the entry for the global table
                        NSBAttributeDesc* pkDesc =
                            pkShader->GetGlobalAttributeTable()->GetAttributeByName(
                            (char*)pkStage->GetGlobalName());
                        if (!pkDesc)
                        {
                            // Log this error???
                            bResult = false;
                            continue;
                        }

                        EE_ASSERT(NiShaderConstantMapEntry::IsMatrix4(
                            pkDesc->GetType()));

                        // Release it
                        NiFixedString kName = pkDesc->GetName();
                        NiD3DShaderFactory::ReleaseGlobalShaderConstant(
                            kName);
                    }
                }
            }
        }
    }
#endif

    return bResult;
}

//------------------------------------------------------------------------------------------------
NiBinaryShader* NiBinaryShaderLibrary::CreateNiBinaryShaderFromNSBShader(
    NSBShader* pkNSBShader,
    unsigned int uiImplementation)
{
    NSBImplementation* pkImplementation = 0;

    VersionInfo kVersionInfo;
    SetupVersionInfo(kVersionInfo);

    bool bBestImplementation = false;

    if (uiImplementation != NiShader::DEFAULT_IMPLEMENTATION &&
        uiImplementation < pkNSBShader->GetImplementationCount())
    {
        // Grab the proper implementation
        pkImplementation = pkNSBShader->GetImplementationByIndex(uiImplementation);
        if (!IsImplementationValid(pkImplementation, kVersionInfo))
        {
            NiRenderer::Warning("%s - %s - "
                "Requested implementation (%d) invalid on operating "
                "hardware.\n", __FUNCTION__,
                pkNSBShader->GetName(), uiImplementation);
            pkImplementation = 0;
        }
        else
        {
            if (pkImplementation == GetBestImplementation(pkNSBShader))
            {
                bBestImplementation = true;
            }
        }
    }

    if (!pkImplementation)
    {
        // Get the best for the hardware
        pkImplementation = GetBestImplementation(pkNSBShader);
        if (!pkImplementation)
        {
            NiRenderer::Warning("%s - %s - Unable "
                "to find valid implementation for hardware.\n",
                __FUNCTION__, pkNSBShader->GetName());
            return 0;
        }
        bBestImplementation = true;
    }

    // This will ALWAYS be set - at least to the default.
    NiBinaryShaderLibrary::CREATENIBINARYSHADER pfnCallback =
        NiBinaryShaderLibrary::GetCreateNiBinaryShaderCallback();
    EE_ASSERT(pfnCallback);

    NiBinaryShader* pkShader = pfnCallback(pkImplementation->GetClassName());
    if (pkShader == NULL)
        pkShader = DefaultCreateNiBinaryShader(NULL);

    // We better have a shader by now!
    EE_ASSERT(pkShader);

    NiD3DRenderer* pkRenderer =
        NiVerifyStaticCast(NiD3DRenderer, NiRenderer::GetRenderer());
    pkShader->SetD3DRenderer(pkRenderer);
    pkShader->SetName(pkNSBShader->GetName());
    pkShader->SetImplementation(pkImplementation->GetIndex());
    pkShader->SetIsBestImplementation(bBestImplementation);

    pkShader->SetUserDefinedDataSet(pkNSBShader->GetUserDefinedDataSet());
    pkShader->SetImplementationUserDefinedDataSet(
        pkImplementation->GetUserDefinedDataSet());
    NSBPass* pkPass;
    for (unsigned int ui = 0; ui < pkImplementation->GetPassCount(); ui++)
    {
        pkPass = pkImplementation->GetPass(ui, false);
        if (pkPass)
        {
            NSBUserDefinedDataSet* pkUDDSet =
                pkPass->GetUserDefinedDataSet();
            pkShader->SetPassUserDefinedDataSet(ui, pkUDDSet);
        }
    }

    if (!RegisterTextureStageGlobals(pkNSBShader, pkImplementation))
    {
        // Determine how to handle the error case
        NiRenderer::Warning("%s - %s - Failed "
            "to register global texture stage variables.\n",
            __FUNCTION__, pkNSBShader->GetName());
    }

    // Setup the passes and potentially create a shader declaration
    if (!SetupNiBinaryShaderFromNSBImplementation(*pkShader, pkImplementation,
        pkNSBShader, pkNSBShader->GetShaderDesc()))
    {
        NiRenderer::Error("%s - %s - Failed to setup the binary shader.\n",
            __FUNCTION__, pkNSBShader->GetName());
        NiDelete pkShader;
        return 0;
    }

    // If the implementation has both a valid semantic adapter table (SAT)
    // and a packing definition, then the SAT wins, otherwise, the packing
    // def is used to construct a NiShaderDeclaration, which is in turn used
    // to create a SAT.
    if (pkShader->GetSemanticAdapterTable().GetNumFilledTableEntries() > 0)
    {
        // Already found a SAT, do we have a PackingDef too?
        if (pkImplementation->GetPackingDef())
        {
            NiRenderer::Warning("%s - %s - Both a SemanticAdapterTable and "
                "a PackingDef are present in the '%s' implementation. "
                "The PackingDef will be ignored.\n",
                __FUNCTION__, pkImplementation->GetName(), pkNSBShader->GetName());
        }
    }
    else if (pkImplementation->GetPackingDef())
    {
        // No SAT, but we have a PackingDef reference
        // Get the packing def from its name
        NSBPackingDef* pkPackingDef = pkNSBShader->GetPackingDef(
            pkImplementation->GetPackingDef(), false);
        if (!pkPackingDef)
        {
            NiRenderer::Error("%s - %s - Failed "
                "to find packing definition for implementation.\n",
                __FUNCTION__, pkNSBShader->GetName());
            EE_FAIL("Failed to find packing def!");
            NiDelete pkShader;
            return 0;
        }
        else
        {
            // Create the shader declaration from the packing def
            NiShaderDeclarationPtr spShaderDecl =
                GetShaderDeclaration(pkPackingDef);
            if (!spShaderDecl)
            {
                NiRenderer::Error("%s - %s - "
                    "Failed to convert packing definition to "
                    "NiShaderDeclaration.\n", __FUNCTION__,
                    pkNSBShader->GetName());
                EE_FAIL("Failed to convert packing def!");
                NiDelete pkShader;
                return 0;
            }

            // Create the semantic adapter table from the shader
            // declaration
            pkShader->SetSemanticAdapterTableFromShaderDeclaration(
                spShaderDecl);

#if !defined(_PS3)
            // Determine if the shader is requesting BlendIndices as colors.
            // If this is the case, then set the flag on the NiD3DShader
            // to enable this conversion.
            bool bConvert = false;
            unsigned int uiStreamCount = spShaderDecl->GetStreamCount();
            for (unsigned int uiS = 0; uiS < uiStreamCount; uiS++)
            {
                unsigned int uiEntryCount = spShaderDecl->GetEntryCount(uiS);
                for (unsigned int uiE = 0; uiE < uiEntryCount; uiE++)
                {
                    const NiShaderDeclaration::ShaderRegisterEntry* pkEntry =
                        spShaderDecl->GetEntry(uiE, uiS);

                    if (pkEntry->m_eType ==
                        NiShaderDeclaration::SPTYPE_UBYTECOLOR &&
                        NiShaderDeclaration::StringToUsage(
                        pkEntry->m_kUsage) ==
                        NiShaderDeclaration::SPUSAGE_BLENDINDICES)
                    {
                        bConvert = true;
                        break;
                    }
                }
            }

            pkShader->SetConvertBlendIndicesToD3DColor(bConvert);
#endif
        }
    }
    else
    {
        NiRenderer::Warning("%s - %s - Neither a PackingDef or a "
            "SemanticAdapterTable was present, constructing a default "
            "SemanticAdapterTable containing only POSITION0.\n",
            __FUNCTION__, pkNSBShader->GetName());

        // There is no decl, so we rely on the default behavior and
        // create a semantic adapter table that needs only position
        pkShader->SetSemanticAdapterTableFromShaderDeclaration(NULL);
    }

    if (!ReleaseTextureStageGlobals(pkNSBShader, pkImplementation))
    {
        // Determine how to handle the error case
        NiRenderer::Warning("%s - %s - Failed to release global texture "
            "stage variables.\n",
            __FUNCTION__, pkNSBShader->GetName());
    }

    if (!pkShader->Initialize())
    {
        NiRenderer::Warning("%s - %s -  Failed to initialize shader.\n",
            __FUNCTION__, pkNSBShader->GetName());
        NiDelete pkShader;
        return NULL;
    }

    ms_pkShaderLibrary->RegisterNewShader(pkNSBShader, pkShader);

    return pkShader;
}

//------------------------------------------------------------------------------------------------
bool NiBinaryShaderLibrary::SetupNiBinaryShaderFromNSBImplementation(
    NiBinaryShader& kShader,
    NSBImplementation* pkImplementation,
    NSBShader* pkNSBShader,
    NiShaderDesc* pkShaderDesc)
{
    EE_UNUSED_ARG(pkNSBShader);
    // Requirements

    NSBRequirements* pkRequirements = pkImplementation->GetRequirements();
    if (pkRequirements)
    {
        kShader.SetUsesNiRenderState(
            pkRequirements->UsesNiRenderState());
        kShader.SetUsesNiLightState(
            pkRequirements->UsesNiLightState());
        kShader.SetBonesPerPartition(
            pkRequirements->GetBonesPerPartition());
        kShader.SetBinormalTangentMethod(
            pkRequirements->GetBinormalTangentMethod());
        kShader.SetBinormalTangentUVSource(
            pkRequirements->GetBinormalTangentUVSource());
        kShader.SetBoneMatrixRegisters(
            pkRequirements->GetBoneMatrixRegisters());
        kShader.SetBoneCalcMethod((NiBinaryShader::BoneMatrixCalcMethod)
            pkRequirements->GetBoneCalcMethod());
    }
    else
    {
        kShader.SetUsesNiRenderState(false);
        kShader.SetUsesNiLightState(false);
        kShader.SetBonesPerPartition(0);
        kShader.SetBinormalTangentMethod(
            NiShaderRequirementDesc::NBT_METHOD_NONE);
        kShader.SetBinormalTangentUVSource(
            NiShaderDesc::BINORMALTANGENTUVSOURCEDEFAULT);
        kShader.SetBoneMatrixRegisters(0);
        kShader.SetBoneCalcMethod(NiBinaryShader::BONECALC_SKIN);
    }

    // Setup the 'global' constant mappings

    // Vertex
    if (pkImplementation->GetVertexConstantMapCount() > 0)
    {
        NSBConstantMap* pkCM = pkImplementation->GetVertexConstantMap(0);
        if (pkCM)
        {
            kShader.SetVertexConstantMap(
                CreateConstantMapFromNSBConstantMap(pkCM, pkShaderDesc));
        }
        else
        {
            kShader.SetVertexConstantMap(NULL);
        }
    }
    else
    {
        kShader.SetVertexConstantMap(NULL);
    }

    // Pixel
    if (pkImplementation->GetPixelConstantMapCount() > 0)
    {
        NSBConstantMap* pkCM = pkImplementation->GetPixelConstantMap(0);
        if (pkCM)
        {
            kShader.SetPixelConstantMap(
                CreateConstantMapFromNSBConstantMap(pkCM, pkShaderDesc));
        }
        else
        {
            kShader.SetPixelConstantMap(NULL);
        }
    }
    else
    {
        kShader.SetPixelConstantMap(NULL);
    }

    // 'Global' render states
    NiD3DRenderStateGroup* pkRSGroup = 0;
    NSBStateGroup* pkNSBStateGroup = pkImplementation->GetRenderStateGroup();
    if (pkNSBStateGroup)
    {
        if (pkNSBStateGroup->GetStateCount())
        {
            pkRSGroup = NiNew NiD3DRenderStateGroup();

            EE_ASSERT(pkRSGroup);

            if (!SetupRenderStateGroupFromNSBStateGroup(pkNSBStateGroup, *pkRSGroup))
            {
                NiDelete pkRSGroup;
                pkRSGroup = 0;
                return false;
            }
        }
    }
    kShader.SetRenderStateGroup(pkRSGroup);

    bool bNeedPackingDef = (pkImplementation->GetPackingDef() == NULL);

    // Semantic adapter table support
    NiSemanticAdapterTable& kTable = pkImplementation->GetSemanticAdapterTable();
    if (kTable.GetNumFilledTableEntries())
    {
        // Valid semantic adapter table present, copy it to the shader
        // and ensure that we don't create/require a shader declaration
        kShader.GetSemanticAdapterTable() = kTable;
        bNeedPackingDef = false;
    }

    // Now, create the passes
    NSBPass* pkNSBPass;
    efd::SmartPointer<NiD3DPass> spPass;
    unsigned int uiCurrPass = 0;
    unsigned int uiPassCount = pkImplementation->GetPassCount();
    for (unsigned int ui = 0; ui < uiPassCount; ui++)
    {
        pkNSBPass = pkImplementation->GetPass(ui);
        if (pkNSBPass)
        {

            if (pkImplementation->GetSoftwareVP())
                pkNSBPass->SetSoftwareVertexProcessing(true);

            spPass = NiNew NiD3DPass();
            if (!SetupShaderPassFromNSBPass(pkNSBPass, *spPass, pkShaderDesc))
            {
                // FAILED!
                return false;
            }
            else
            {
                kShader.InsertPass(uiCurrPass, spPass);
                uiCurrPass++;
            }

            // If we have neither a PackingDef, nor a SemanticAdapterTable, the
            // PS3 generates a SAT, while the other platforms error out
            if (bNeedPackingDef &&
                pkNSBPass->GetShaderPresent(
                NiGPUProgram::PROGRAM_VERTEX,
                NiRenderer::GetRenderer()->GetRendererID()))
            {
#if defined(_PS3)
                // Create new packing definition
                EE_ASSERT(pkNSBShader);
                pkImplementation->SetPackingDef("GENERATED_PACKING_DEF");
                NSBPackingDef* pkPackingDef =
                    pkNSBShader->GetPackingDef(
                    pkImplementation->GetPackingDef(), true);

                // Get shader declaration from vertex shader
                // using the Vertex Program's parameters
                NiPS3CgShaderProgram* pkVP = spPass->GetVertexShader();
                if (pkVP)
                {
                    // Autogenerate it
                    GenerateDeclarationFromVertexShader(pkPackingDef, pkVP);

                    NiShaderDeclarationPtr spShaderDecl =
                        GetShaderDeclaration(pkPackingDef);

                    // Convert the declaration to a semantic adapter table
                    kShader.SetSemanticAdapterTableFromShaderDeclaration(
                        spShaderDecl);

                    bNeedPackingDef = false;
                }
#else
                // Confirm that a packing definition is present if any pass
                // has a vertex shader
                NiShaderFactory::ReportError(NISHADERERR_UNKNOWN,
                    false, "* ERROR: %s\n"
                    "    Failed to find packing definition or semantic "
                    "adapter table for an implementation that uses vertex "
                    "shaders.\nShader %s, implementation %s\n",
                    __FUNCTION__, kShader.GetName(),
                    pkImplementation->GetName());
                return false;
#endif
            }
        }
    }

    return true;
}

//------------------------------------------------------------------------------------------------
bool NiBinaryShaderLibrary::SetupShaderPassFromNSBPass(
    NSBPass* pkNSBPass,
    NiD3DPass& kPass,
    NiShaderDesc* pkShaderDesc)
{
    // Convert the NSB representation to a run-time NiD3DPass

    // RenderStateGroup
    NiD3DRenderStateGroup* pkRSGroup = 0;
    NSBStateGroup* pkNSBStateGroup = pkNSBPass->GetRenderStateGroup();
    {
        if (pkNSBStateGroup->GetStateCount())
        {
            pkRSGroup = NiD3DRenderStateGroup::GetFreeRenderStateGroup();
            EE_ASSERT(pkRSGroup);

            if (!SetupRenderStateGroupFromNSBStateGroup(
                pkNSBStateGroup, *pkRSGroup))
            {
                NiDelete pkRSGroup;
                return false;
            }
        }
    }
    kPass.SetRenderStateGroup(pkRSGroup);

    // Setup the 'local' constant mappings

    // Vertex
    if (pkNSBPass->GetVertexConstantMapCount() > 0)
    {
        NSBConstantMap* pkCM = pkNSBPass->GetVertexConstantMap(0);
        if (pkCM)
        {
            kPass.SetVertexConstantMap(
                CreateConstantMapFromNSBConstantMap(pkCM, pkShaderDesc));
        }
        else
        {
            kPass.SetVertexConstantMap(NULL);
        }
    }
    else
    {
        kPass.SetVertexConstantMap(NULL);
    }

    // Pixel
    if (pkNSBPass->GetPixelConstantMapCount() > 0)
    {
        NSBConstantMap* pkCM = pkNSBPass->GetPixelConstantMap(0);
        if (pkCM)
        {
            kPass.SetPixelConstantMap(
                CreateConstantMapFromNSBConstantMap(pkCM, pkShaderDesc));
        }
        else
        {
            kPass.SetPixelConstantMap(NULL);
        }
    }
    else
    {
        kPass.SetPixelConstantMap(NULL);
    }

    // Shader Programs
#if defined(_XENON) || defined(WIN32)
#if defined(_XENON)
    NiSystemDesc::RendererID eRenderer = NiSystemDesc::RENDERER_XENON;
#elif defined(WIN32)
    NiSystemDesc::RendererID eRenderer = NiSystemDesc::RENDERER_DX9;
#endif

    kPass.SetVertexShaderProgramFileName(pkNSBPass->GetShaderProgramFile(
        eRenderer, NiGPUProgram::PROGRAM_VERTEX));
    kPass.SetPixelShaderProgramFileName(pkNSBPass->GetShaderProgramFile(
        eRenderer, NiGPUProgram::PROGRAM_PIXEL));

    kPass.SetVertexShaderProgramEntryPoint(pkNSBPass->GetShaderProgramEntryPoint(
        eRenderer, NiGPUProgram::PROGRAM_VERTEX));
    kPass.SetVertexShaderProgramShaderTarget(pkNSBPass->GetShaderProgramShaderTarget(
        eRenderer, NiGPUProgram::PROGRAM_VERTEX));
    kPass.SetPixelShaderProgramEntryPoint(pkNSBPass->GetShaderProgramEntryPoint(
        eRenderer, NiGPUProgram::PROGRAM_PIXEL));
    kPass.SetPixelShaderProgramShaderTarget(pkNSBPass->GetShaderProgramShaderTarget(
        eRenderer, NiGPUProgram::PROGRAM_PIXEL));
#elif defined (_PS3)
    // Use the shader program factory which has a cache
    char acFilePath[NI_MAX_PATH];

    const char* pcVertexProgram = pkNSBPass->GetShaderProgramFile(
        NiSystemDesc::RENDERER_PS3, NiGPUProgram::PROGRAM_VERTEX);
    const char* pcVertexEntry = pkNSBPass->GetShaderProgramEntryPoint(
        NiSystemDesc::RENDERER_PS3, NiGPUProgram::PROGRAM_VERTEX);

    // Convert the source shader name into a binary name for the VP
    EE_VERIFY(NiPS3CgShaderProgram::GetCompiledShaderFilename(
        pcVertexProgram, acFilePath, NI_MAX_PATH,
        NiGPUProgram::PROGRAM_VERTEX, pcVertexEntry));

    // The factory will either return the cached binary or load it from disk
    NiPS3CgShaderProgram* pkVP =
        NiPS3ShaderProgramFactory::CreateShaderProgramFromFile(
        NiGPUProgram::PROGRAM_VERTEX, pcVertexProgram,
        acFilePath, pcVertexEntry);
    if (!pkVP)
        return false;

    const char* pcPixelProgram = pkNSBPass->GetShaderProgramFile(
        NiSystemDesc::RENDERER_PS3, NiGPUProgram::PROGRAM_PIXEL);
    const char* pcPixelEntry = pkNSBPass->GetShaderProgramEntryPoint(
        NiSystemDesc::RENDERER_PS3, NiGPUProgram::PROGRAM_PIXEL);

    // Convert the source shader name into a binary name for the FP
    EE_VERIFY(NiPS3CgShaderProgram::GetCompiledShaderFilename(
        pcPixelProgram, acFilePath, NI_MAX_PATH,
        NiGPUProgram::PROGRAM_PIXEL, pcPixelEntry));

    // The factory will either return the cached binary or load it from disk
    NiPS3CgShaderProgram* pkFP =
        NiPS3ShaderProgramFactory::CreateShaderProgramFromFile(
        NiGPUProgram::PROGRAM_PIXEL, pcPixelProgram,
        acFilePath, pcPixelEntry);
    if (!pkFP)
        return false;

    // Look for UserData that controls special per-shader states
    NSBUserDefinedDataSet* pkSet = pkNSBPass->GetUserDefinedDataSet();
    if (pkSet)
    {
        NSBUserDefinedDataBlock* pkUDBlock =
            pkSet->GetBlock("FragmentShaderSettings", false);
        if (pkUDBlock)
        {
            NSBConstantMap::NSBCM_Entry* pkEntry =
                pkUDBlock->GetEntryByKey("RegisterCount");
            if (pkEntry && pkEntry->IsUnsignedInt())
            {
                unsigned int* puiRegisterCount =
                    (unsigned int*)pkEntry->GetDataSource();

                pkFP->SetFragmentProgramRegisterCount(*puiRegisterCount);
            }
        }
    }

    // Add the shaders to the pass
    kPass.SetVertexShader(pkVP);
    kPass.SetPixelShader(pkFP);
#else
#error "Unsupported platform"
#endif

    NSBTextureStage* pkStage;
    const unsigned int uiStageCount = pkNSBPass->GetStageCount();
    if (uiStageCount > NiD3DPass::ms_uiMaxSamplers)
        return false;

    for (unsigned int ui = 0; ui < uiStageCount; ui++)
    {
        pkStage = pkNSBPass->GetStage(ui);
        if (pkStage)
        {
            efd::SmartPointer<NiD3DTextureStage> spPlatformStage =
                kPass.GetStage(ui);
            if (!spPlatformStage)
            {
                spPlatformStage = NiNew NiD3DTextureStage();
                EE_ASSERT(spPlatformStage);
            }

            if (!SetupTextureStageFromNSBTextureStage(pkStage, *spPlatformStage))
            {
                return false;
            }

            kPass.SetStage(ui, spPlatformStage);
        }
    }

    // Textures should only modify existing stages
    NSBTexture* pkTexture;
    unsigned int uiTextureCount = pkNSBPass->GetTextureCount();
    if (uiTextureCount > uiStageCount)
        uiTextureCount = uiStageCount;

    for (unsigned int ui = 0; ui < uiTextureCount; ui++)
    {
        pkTexture = pkNSBPass->GetTexture(ui);

        if (pkTexture)
        {
            efd::SmartPointer<NiD3DTextureStage> spPlatformStage =
                kPass.GetStage(ui);
            if (!spPlatformStage)
            {
                continue;
            }

            if (!ModifyTextureStageFromNSBTexture(pkTexture, *spPlatformStage))
            {
                return false;
            }
        }
    }

#if defined(_PS3)
    for (unsigned int ui = 0; ui < uiStageCount; ui++)
    {
        pkStage = pkNSBPass->GetStage(ui);
        if (pkStage)
        {
            // First, try to match up the sampler by name
            CGparameter pkSamplerParam = 0;

            if (pkStage->GetName())
            {
                pkSamplerParam = cellGcmCgGetNamedParameter(
                    pkFP->GetShaderProgramHandle(), pkStage->GetName());
            }

            if (pkSamplerParam == 0)
            {
                // Match up sampler via resource index, not by name.
                // NSF sampler names currently ignored on other platforms.

                // Iterate though all samplers in the fragment program
                unsigned int uiNumFPSamplers = pkFP->GetSamplerCount();
                for (unsigned int uiFPSampler = 0;
                    uiFPSampler < uiNumFPSamplers;
                    uiFPSampler++)
                {
                    // Match the resource index
                    if (pkFP->GetSamplerParamResourceIndex(uiFPSampler) == ui)
                    {
                        pkSamplerParam = pkFP->GetSamplerParam(uiFPSampler);
                        break;
                    }
                }
            }

            // Record the sampler parameter hookup
            if (pkSamplerParam != 0)
            {
                unsigned int uiSamplerID = cellGcmCgGetParameterResource(
                    pkFP->GetShaderProgramHandle(),
                    pkSamplerParam) - CG_TEXUNIT0;

                efd::SmartPointer<NiD3DTextureStage> spPlatformStage =
                    kPass.GetSampler(ui);
                spPlatformStage->SetSamplerCGParam(pkSamplerParam,
                    uiSamplerID);
            }
            else
            {
                NILOG("Could not bind NSF sampler index %d to shader\n", ui);
            }
        }
    }
#endif

    return true;
}

//------------------------------------------------------------------------------------------------
void NiBinaryShaderLibrary::GetRequirementValues(
    NSBRequirements* pkReqs,
    unsigned int& uiVSVers,
    unsigned int& uiGSVers,
    unsigned int& uiPSVers,
    unsigned int& uiUser)
{
    EE_ASSERT(pkReqs);

    NiShaderFactory* pkFactory = NiShaderFactory::GetInstance();

    uiVSVers = pkReqs->GetVSVersion();
    if (uiVSVers == 0)
        uiVSVers = pkFactory->CreateVertexShaderVersion(0, 0);
    uiGSVers = pkReqs->GetGSVersion();
    if (uiGSVers == 0)
        uiGSVers = pkFactory->CreateGeometryShaderVersion(0, 0);
    uiPSVers = pkReqs->GetPSVersion();
    if (uiPSVers == 0)
        uiPSVers = pkFactory->CreatePixelShaderVersion(0, 0);
    uiUser = pkReqs->GetUserVersion();
    if (uiUser == 0)
        uiUser = pkFactory->CreateVertexShaderVersion(0, 0);
}

//------------------------------------------------------------------------------------------------
NSBImplementation* NiBinaryShaderLibrary::GetBestImplementation(NSBShader* pkShader)
{
    NSBImplementation* pkImplementation = 0;
    bool bFound = false;

    VersionInfo kVersionInfo;

    // Grab the version info from the renderer, adjusting parameters as
    // necessary to prevent illegal requests, etc. (For example, a request
    // that is higher than the system.)
    SetupVersionInfo(kVersionInfo);

    unsigned int uiVSVers;
    unsigned int uiGSVers;
    unsigned int uiPSVers;
    unsigned int uiUser;
    NSBRequirements* pkReqs;

    NiTPointerList<NSBImplementation*> kValidList;
    kValidList.RemoveAll();

    unsigned int uiCount = pkShader->GetImplementationCount();
    unsigned int ui;

    for (ui = 0; ui < uiCount; ui++)
    {
        pkImplementation = pkShader->GetImplementationByIndex(ui);
        if (pkImplementation)
        {
            // Check the versions vs. the request ones...
            // NOTE: This assumes that implementations are listed
            // in a hardware-need order. ie, the highest hardware
            // requirements will be first in the list!
            if (IsImplementationValid(pkImplementation, kVersionInfo))
            {
                bool bFound = false;

                NiTListIterator pos = kValidList.GetHeadPos();
                while (pos)
                {
                    NSBImplementation* pkCheckImp = kValidList.GetNext(pos);
                    if (pkCheckImp == pkImplementation)
                    {
                        bFound = true;
                        break;
                    }
                }

                if (!bFound)
                    kValidList.AddTail(pkImplementation);
            }
        }
    }

    // Now, check the valid list for the closest match to the requested
    // version.
    if (!bFound)
    {
        NiTListIterator pos;
        // Now, how do we decide on the 'closest' to the requested...
        // We will grab the first one that is <= the requested ones
        pos = kValidList.GetHeadPos();
        while (pos)
        {
            pkImplementation = kValidList.GetNext(pos);
            if (pkImplementation)
            {
                pkReqs = pkImplementation->GetRequirements();
                EE_ASSERT(pkReqs);
                GetRequirementValues(pkReqs, uiVSVers, uiGSVers, uiPSVers,
                    uiUser);

                if ((uiVSVers >= kVersionInfo.m_uiVS_Min) &&
                    (uiGSVers <= kVersionInfo.m_uiGS_Req) &&
                    (uiGSVers >= kVersionInfo.m_uiGS_Min) &&
                    (uiPSVers <= kVersionInfo.m_uiPS_Req) &&
                    (uiPSVers >= kVersionInfo.m_uiPS_Min) &&
                    (uiUser <= kVersionInfo.m_uiUser_Req) &&
                    (uiUser >= kVersionInfo.m_uiUser_Min))
                {
                    if (uiVSVers <= kVersionInfo.m_uiVS_Req ||
                        (kVersionInfo.m_bSoftwareVSCapable_Sys &&
                        pkImplementation->GetSoftwareVP()))
                    {
                        // We have found a workable implementation!
                        bFound = true;
                        break;
                    }
                }
            }
        }
    }

    kValidList.RemoveAll();

    if (!bFound)
    {
        NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, false,
            "* ERROR: Unable to find usable implementation for %s\n",
            pkShader->GetName());
        return 0;
    }
    return pkImplementation;
}

//------------------------------------------------------------------------------------------------
void NiBinaryShaderLibrary::SetupVersionInfo(VersionInfo& kVersionInfo)
{
    NiD3DRenderer* pkRenderer =
        NiVerifyStaticCast(NiD3DRenderer, NiRenderer::GetRenderer());
    const NiD3DShaderLibraryVersion* pkSLVersion =
        pkRenderer->GetShaderLibraryVersion();
    EE_ASSERT(pkSLVersion);

    kVersionInfo.m_uiVS_Sys = pkSLVersion->GetSystemVertexShaderVersion();
    kVersionInfo.m_uiGS_Sys = 0xfffd0400;
    kVersionInfo.m_uiPS_Sys = pkSLVersion->GetSystemPixelShaderVersion();
    kVersionInfo.m_uiUser_Sys = pkSLVersion->GetSystemUserVersion();

    kVersionInfo.m_uiVS_Min = pkSLVersion->GetMinVertexShaderVersion();
    kVersionInfo.m_uiGS_Min = 0xfffd0000;
    kVersionInfo.m_uiPS_Min = pkSLVersion->GetMinPixelShaderVersion();
    kVersionInfo.m_uiUser_Min = pkSLVersion->GetMinUserVersion();

    kVersionInfo.m_uiVS_Req = pkSLVersion->GetVertexShaderVersionRequest();
    kVersionInfo.m_uiGS_Req = 0xfffd0400;
    kVersionInfo.m_uiPS_Req = pkSLVersion->GetPixelShaderVersionRequest();
    kVersionInfo.m_uiUser_Req = pkSLVersion->GetUserVersionRequest();

    kVersionInfo.m_uiPlatform = pkSLVersion->GetPlatform();

    kVersionInfo.m_bSoftwareVSCapable_Sys = false;

    // Do some quick checks to reduce the time spent in this function.

    // If the requested versions are GREATER THAN that the system, then we
    // can bump down the request right now.
    if (kVersionInfo.m_uiVS_Sys < kVersionInfo.m_uiVS_Req)
        kVersionInfo.m_uiVS_Req = kVersionInfo.m_uiVS_Sys;
    if (kVersionInfo.m_uiGS_Sys < kVersionInfo.m_uiGS_Req)
        kVersionInfo.m_uiGS_Req = kVersionInfo.m_uiGS_Sys;
    if (kVersionInfo.m_uiPS_Sys < kVersionInfo.m_uiPS_Req)
        kVersionInfo.m_uiPS_Req = kVersionInfo.m_uiPS_Sys;
    if (kVersionInfo.m_uiUser_Sys < kVersionInfo.m_uiUser_Req)
        kVersionInfo.m_uiUser_Req = kVersionInfo.m_uiUser_Sys;
}

//------------------------------------------------------------------------------------------------
NiBinaryShaderLibrary::ValidityFlag NiBinaryShaderLibrary::IsImplementationValid(
    NSBImplementation* pkImplementation, VersionInfo& kVersionInfo)
{
    if (!pkImplementation)
        return INVALID;

    // Check the versions vs. the request ones...
    // NOTE: This assumes that implementations are listed
    // in a hardware-need order. ie, the highest hardware
    // requirements will be first in the list!
    NSBRequirements* pkReqs = pkImplementation->GetRequirements();
    EE_ASSERT(pkReqs);
    unsigned int uiVSVers;
    unsigned int uiGSVers;
    unsigned int uiPSVers;
    unsigned int uiUser;
    GetRequirementValues(pkReqs, uiVSVers, uiGSVers, uiPSVers, uiUser);

    // Check the platform first.
    if ((pkReqs->GetPlatformFlags() != 0) &&
        ((pkReqs->GetPlatformFlags() & kVersionInfo.m_uiPlatform) == 0))
    {
        return INVALID;
    }

    // Check the system versions to make sure the hardware
    // can handle the implementation
    if (uiPSVers > kVersionInfo.m_uiPS_Sys)
        return INVALID;

    bool bSoftwareVP = false;

    if (pkReqs->GetSoftwareVPRequired())
    {
        if (kVersionInfo.m_bSoftwareVSCapable_Sys)
        {
            pkImplementation->SetSoftwareVP(true);
            bSoftwareVP = true;
        }
        else
        {
            return INVALID;
        }
    }

    if (!bSoftwareVP && uiVSVers > kVersionInfo.m_uiVS_Sys)
    {
        if (pkReqs->GetSoftwareVPAcceptable() &&
            kVersionInfo.m_bSoftwareVSCapable_Sys)
        {
            pkImplementation->SetSoftwareVP(true);
        }
        else
        {
            return INVALID;
        }
    }

    // Check the minimum versions to make sure it is acceptable
    // to the user
    if ((uiVSVers < kVersionInfo.m_uiVS_Min) ||
        (uiGSVers < kVersionInfo.m_uiGS_Min) ||
        (uiPSVers < kVersionInfo.m_uiPS_Min))
    {
        return INVALID;
    }

    // Check the user version.
    if ((uiUser > kVersionInfo.m_uiUser_Sys) ||
        (uiUser < kVersionInfo.m_uiUser_Min))
    {
        return INVALID;
    }

    if ((uiVSVers == kVersionInfo.m_uiVS_Req) &&
        (uiGSVers == kVersionInfo.m_uiGS_Req) &&
        (uiPSVers == kVersionInfo.m_uiPS_Req) &&
        (uiUser == kVersionInfo.m_uiUser_Req))
    {
        return VALID_REQUESTED;
    }

    return VALID;
}

//------------------------------------------------------------------------------------------------
NiD3DShaderConstantMap* NiBinaryShaderLibrary::CreateConstantMapFromNSBConstantMap(
    NSBConstantMap* pkCM,
    NiShaderDesc* pkShaderDesc)
{
    if (pkCM->GetTotalEntryCount() == 0)
        return 0;

    NiD3DShaderConstantMap* pkSCM = NiNew NiD3DShaderConstantMap(
        (NiGPUProgram::ProgramType)pkCM->GetProgramType());
    if (!pkSCM)
        return 0;

    NiTListIterator kIter = 0;
    NSBConstantMap::NSBCM_Entry* pkEntry = pkCM->GetFirstEntry(kIter);
    while (pkEntry)
    {
        if (!ProcessMapEntry(pkShaderDesc, pkEntry, pkSCM))
        {
            NiShaderFactory::ReportError(NISHADERERR_UNKNOWN,
                true, "* ERROR: ConstructConstantMap\n"
                "    Failed processing of entry %s\n",
                (const char*)pkEntry->GetKey());
        }
        pkEntry = pkCM->GetNextEntry(kIter);
    }

    // These sections are compile-time determined, and will only add the
    // shader constants that are specific to the platform the library has
    // be compiled for.
    NiShader::Platform ePlatform;
#if defined(WIN32)
    ePlatform = NiShader::NISHADER_DX9;
#elif defined(_XENON)
    ePlatform = NiShader::NISHADER_XENON;
#elif defined(_PS3)
    ePlatform = NiShader::NISHADER_PS3;
#else
#error "Unsupported platform"
#endif

    kIter = 0;
    pkEntry = pkCM->GetFirstPlatformEntry(ePlatform, kIter);
    while (pkEntry)
    {
        if (!ProcessMapEntry(pkShaderDesc, pkEntry, pkSCM))
        {
            NiShaderFactory::ReportError(NISHADERERR_UNKNOWN,
                true, "* ERROR: ConstructConstantMap\n"
                "    Failed processing of entry %s\n",
                (const char*)pkEntry->GetKey());
        }
        pkEntry = pkCM->GetNextPlatformEntry(ePlatform, kIter);
    }

    // Fix up the CM_Operator extra data
    // The platform SCM can be in a different order than the original NSB, and CM_Operator
    // entries store a pair of indices (to the operands) in extradata
    //
    // This does not have to be repeated for the per-platform entries, as the CM_Operator
    // doesn't support per-platform indices anyways
    kIter = 0;
    pkEntry = pkCM->GetFirstEntry(kIter);
    while (pkEntry)
    {
        if (pkEntry->IsOperator())
        {
            // Get the old indices
            unsigned int uiExtra = pkEntry->GetExtra();

            const unsigned int uiOldEntryIndex1 =
                uiExtra & NiShaderConstantMapEntry::SCME_OPERATOR_ENTRY1_MASK;
            const unsigned int uiOldEntryIndex2 =
                (uiExtra & NiShaderConstantMapEntry::SCME_OPERATOR_ENTRY2_MASK) >>
                NiShaderConstantMapEntry::SCME_OPERATOR_ENTRY2_SHIFT;

            // Remap
            NSBConstantMap::NSBCM_Entry* pkOperator1 = pkCM->GetEntryByIndex(uiOldEntryIndex1);
            NSBConstantMap::NSBCM_Entry* pkOperator2 = pkCM->GetEntryByIndex(uiOldEntryIndex2);
            EE_ASSERT((pkOperator1 != NULL) && (pkOperator2 != NULL));

            unsigned int uiNewEntryIndex1 =
                FindEntryIndexInPlatformSCM(pkSCM, pkOperator1->GetKey());
            unsigned int uiNewEntryIndex2 =
                FindEntryIndexInPlatformSCM(pkSCM, pkOperator2->GetKey());

            // Mask out the old indices and replace them with the new ones
            uiExtra &= ~NiShaderConstantMapEntry::SCME_OPERATOR_ENTRY1_MASK;
            uiExtra &= ~NiShaderConstantMapEntry::SCME_OPERATOR_ENTRY2_MASK;
            uiExtra |= uiNewEntryIndex1;
            uiExtra |= (uiNewEntryIndex2 << NiShaderConstantMapEntry::SCME_OPERATOR_ENTRY2_SHIFT);

            // Find the entry in the SCM and set the extradata there
            int targetIndex = FindEntryIndexInPlatformSCM(pkSCM, pkEntry->GetKey());
            NiShaderConstantMapEntry* pkTarget = pkSCM->GetEntryAtIndex(targetIndex);
            pkTarget->SetExtra(uiExtra);
        }

        pkEntry = pkCM->GetNextEntry(kIter);
    }

    return pkSCM;
}

//------------------------------------------------------------------------------------------------
bool NiBinaryShaderLibrary::ProcessMapEntry(
    NiShaderDesc* pkShaderDesc,
    NSBConstantMap::NSBCM_Entry* pkEntry,
    NiD3DShaderConstantMap* pkSCM)
{
    if (!pkEntry || !pkSCM)
        return false;

    NiShaderError err;

    if (pkEntry->IsGlobal())
    {
        // We have to register the constant with the renderer
        NiShaderAttributeDesc::AttributeType eType =
            NiShaderConstantMapEntry::GetAttributeType(
            pkEntry->GetFlags());
        if (!NiShaderFactory::RegisterGlobalShaderConstant(
            pkEntry->GetKey(), eType, pkEntry->GetDataSize(),
            pkEntry->GetDataSource()))
        {
            EE_FAIL("Failed to add global constant!");
        }

        // Retrieve the global entry
        NiShaderFactory* pkShaderFactory = NiShaderFactory::GetInstance();

        NiGlobalConstantEntry* pkGlobal =
            pkShaderFactory->GetGlobalShaderConstantEntry(
            pkEntry->GetKey());
        EE_ASSERT(pkGlobal);

        err = pkSCM->AddEntry(pkEntry->GetKey(), pkEntry->GetFlags(),
            0, pkEntry->GetShaderRegister(), pkEntry->GetRegisterCount(),
            pkEntry->GetVariableName(), pkEntry->GetDataSize(),
            pkEntry->GetDataStride(), pkGlobal->GetDataSource());

        // Release the entry immediately; the shader constant map should
        // maintain a reference to the entry.
        NiShaderFactory::ReleaseGlobalShaderConstant(pkEntry->GetKey());
    }
    else if (pkEntry->IsAttribute())
    {
        bool bAllocatedMemory = false;

        // Find default value in case attribute is not on geometry.
        const NiShaderAttributeDesc* pkDesc =
            pkShaderDesc->GetAttribute(pkEntry->GetKey());
        if (pkDesc == NULL)
            return false;

        const char* pcValue;
        bool bValue;
        float afValue[16];
        float* pfValue = afValue;
        unsigned int uiValue;

        void* pvDataSource = afValue;
        unsigned int uiDataSize = 0;
        unsigned int uiDataStride = sizeof(afValue[0]);
        switch (pkDesc->GetType())
        {
        case NiShaderAttributeDesc::ATTRIB_TYPE_BOOL:
            pkDesc->GetValue_Bool(bValue);
            pvDataSource = &bValue;
            uiDataSize = sizeof(bValue);
            uiDataStride = sizeof(bValue);
            break;
        case NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT:
            pkDesc->GetValue_UnsignedInt(uiValue);
            pvDataSource = &uiValue;
            uiDataSize = sizeof(uiValue);
            uiDataStride = sizeof(uiValue);
            break;
        case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT:
            pkDesc->GetValue_Float(*afValue);
            uiDataSize = uiDataStride;
            break;
        case NiShaderAttributeDesc::ATTRIB_TYPE_POINT2:
            {
                NiPoint2 kValue;
                pkDesc->GetValue_Point2(kValue);
                afValue[0] = kValue.x;
                afValue[1] = kValue.y;
                uiDataSize = 2 * uiDataStride;
                break;
            }
        case NiShaderAttributeDesc::ATTRIB_TYPE_POINT3:
            {
                NiPoint3 kValue;
                pkDesc->GetValue_Point3(kValue);
                afValue[0] = kValue.x;
                afValue[1] = kValue.y;
                afValue[2] = kValue.z;
                uiDataSize = 3 * uiDataStride;
                break;
            }
        case NiShaderAttributeDesc::ATTRIB_TYPE_POINT4:
            pkDesc->GetValue_Point4(pfValue);
            uiDataSize = 4 * uiDataStride;
            break;
        case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3:
            {
                NiMatrix3 kValue;
                pkDesc->GetValue_Matrix3(kValue);
                afValue[0] = kValue.GetEntry(0, 0);
                afValue[1] = kValue.GetEntry(0, 1);
                afValue[2] = kValue.GetEntry(0, 2);
                afValue[3] = kValue.GetEntry(1, 0);
                afValue[4] = kValue.GetEntry(1, 1);
                afValue[5] = kValue.GetEntry(1, 2);
                afValue[6] = kValue.GetEntry(2, 0);
                afValue[7] = kValue.GetEntry(2, 1);
                afValue[8] = kValue.GetEntry(2, 2);
                uiDataSize = 9 * uiDataStride;
                break;
            }
        case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4:
            pkDesc->GetValue_Matrix4(pfValue, 16 * sizeof(float));
            uiDataSize = 16 * uiDataStride;
            break;
        case NiShaderAttributeDesc::ATTRIB_TYPE_COLOR:
            {
                NiColorA kValue;
                pkDesc->GetValue_ColorA(kValue);
                afValue[0] = kValue.r;
                afValue[1] = kValue.g;
                afValue[2] = kValue.b;
                afValue[3] = kValue.a;
                uiDataSize = 4 * uiDataStride;
                break;
            }
        case NiShaderAttributeDesc::ATTRIB_TYPE_STRING:
        case NiShaderAttributeDesc::ATTRIB_TYPE_TEXTURE:
            pkDesc->GetValue_String(pcValue);
            pvDataSource = (void*)pcValue;
            uiDataSize = (unsigned int)((pcValue == NULL || *pcValue == '\0') ?
                0 : strlen(pcValue) * sizeof(*pcValue));
            uiDataStride = sizeof(*pcValue);
            break;
        case NiShaderAttributeDesc::ATTRIB_TYPE_ARRAY:
            {
                NiShaderAttributeDesc::AttributeType eType;
                unsigned int uiElementSize;
                unsigned int uiNumElements;
                pkDesc->GetArrayParams(
                    eType,
                    uiElementSize,
                    uiNumElements);

                // get copy of data
                uiDataStride = uiElementSize;
                uiDataSize = uiElementSize*uiNumElements;
                pvDataSource = NiAlloc(char, uiDataSize);
                bAllocatedMemory = true;
                pkDesc->GetValue_Array(pvDataSource, uiDataSize);

                break;
            }
        case NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED:
        default:
            uiDataSize = 0;
            uiDataStride = 0;
            pvDataSource = NULL;
            break;
        }

        // Copy the key and truncate it before @@ if @@ exists.
        size_t stBufSize = strlen(pkEntry->GetKey()) + 1;
        char* pcMapping = NiStackAlloc(char, stBufSize);
        NiStrcpy(pcMapping, stBufSize, pkEntry->GetKey());
        char* pcPtr = strstr(pcMapping, "@@");
        if (pcPtr)
            *pcPtr = '\0';

        // Add the NSB SCM entry to the platform SCM
        err = pkSCM->AddEntry(pcMapping, pkEntry->GetFlags(),
            pkEntry->GetExtra(), pkEntry->GetShaderRegister(),
            pkEntry->GetRegisterCount(), pkEntry->GetVariableName(),
            uiDataSize, uiDataStride, pvDataSource, true);

        if (bAllocatedMemory)
        {
            NiFree(pvDataSource);
        }

        NiStackFree(pcMapping);
    }
    else
    {
        // Copy the key and truncate it before @@ if @@ exists.
        size_t stBufSize = strlen(pkEntry->GetKey()) + 1;
        char* pcMapping = NiStackAlloc(char, stBufSize);
        NiStrcpy(pcMapping, stBufSize, pkEntry->GetKey());
        char* pcPtr = strstr(pcMapping, "@@");
        if (pcPtr)
            *pcPtr = '\0';

        // Add the NSB SCM entry to the platform SCM
        err = pkSCM->AddEntry(pcMapping, pkEntry->GetFlags(),
            pkEntry->GetExtra(), pkEntry->GetShaderRegister(),
            pkEntry->GetRegisterCount(), pkEntry->GetVariableName(),
            pkEntry->GetDataSize(), pkEntry->GetDataStride(),
            pkEntry->GetDataSource());

        NiStackFree(pcMapping);
    }

    if (err != NISHADERERR_OK)
        return false;

    return true;
}

//------------------------------------------------------------------------------------------------
unsigned int NiBinaryShaderLibrary::FindEntryIndexInPlatformSCM(
    NiD3DShaderConstantMap* pkSCM,
    const char* pcName)
{
    // Copy the key and truncate it before @@ if @@ exists.
    size_t stBufSize = strlen(pcName) + 1;
    char* pcMapping = NiStackAlloc(char, stBufSize);
    NiStrcpy(pcMapping, stBufSize, pcName);

    char* pcPtr = strstr(pcMapping, "@@");
    if (pcPtr)
    {
        *pcPtr = '\0';
    }

    // Find the key in the map
    unsigned int index = pkSCM->GetEntryIndex(pcMapping);

    NiStackFree(pcMapping);

    return index;
}

//------------------------------------------------------------------------------------------------
