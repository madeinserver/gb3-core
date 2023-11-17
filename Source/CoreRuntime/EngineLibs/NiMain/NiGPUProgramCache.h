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

#pragma once
#ifndef NIGPUPROGRAMCACHE_H
#define NIGPUPROGRAMCACHE_H

#include "NiGPUProgram.h"
#include "NiMaterialResource.h"
#include "NiTFixedStringMap.h"
#include "NiTPtrSet.h"
#include "NiString.h"

class NIMAIN_ENTRY NiGPUProgramCache : public NiRefObject
{
    NiDeclareRootRTTI(NiGPUProgramCache);
public:
    NiGPUProgramCache(unsigned int uiVersion, bool bWriteToDisk,
        bool bLocked);
    virtual ~NiGPUProgramCache();

    virtual NiGPUProgram* FindCachedProgram(const char* pcName,
        NiTObjectPtrSet<NiMaterialResourcePtr>& kUniforms,
        bool& bFailedToCompilePreviously);

    virtual NiGPUProgram* GenerateProgram(const char* pcName,
        const char* pcProgramText,
        NiTObjectPtrSet<NiMaterialResourcePtr>& kUniforms);

    virtual const NiTObjectPtrSet<NiFixedString>*
        GetValidCompileTargetsForPlatform() const;
    virtual const NiFixedString& GetShaderProfile() const;
    virtual const NiFixedString& GetWorkingDirectory() const;
    virtual bool SetWorkingDirectory(const NiFixedString& kPath);

    virtual const char* GetPlatformSpecificCodeID() const = 0;
    virtual const char* GetPlatform() const = 0;

    virtual NiString GetPlatformSpecificInOutSemantic(
        const NiFixedString& kAgnosticSemantic);
    virtual unsigned int GetPlatformSpecificInOutSemanticOrder(
        const NiFixedString& kAgnosticSemantic);
    virtual unsigned int GetPlatformSpecificSemanticCount();

    virtual bool Load();
    virtual bool Save();

    virtual bool Clear();

    virtual bool IsLoaded();
    virtual bool IsLocked();

    virtual void AddFailedProgram(const char* pcProgram);

    static void SetOverwriteDeprecatedCacheFiles(bool bOverwrite);

    // *** begin Emergent internal use only ***
    class NiGPUProgramDesc : public NiRefObject
    {
    public:
        NiGPUProgramPtr m_spProgram;
        NiTObjectPtrSet<NiMaterialResourcePtr> m_kUniforms;
    };
    typedef efd::SmartPointer<NiGPUProgramDesc> NiGPUProgramDescPtr;
    // *** end Emergent internal use only ***

protected:
    virtual bool PruneUnusedConstants(NiGPUProgram* pkProgram,
        NiTObjectPtrSet<NiMaterialResourcePtr>& kUniforms);

    virtual void ComputeFullFilename();
    bool ValidateDirectory();

    virtual bool AppendEntry(NiGPUProgramDesc* pkDesc,
        const NiFixedString& kName);
    virtual bool SaveEntry(efd::BinaryStream& kStream,
        NiGPUProgramDesc* pkDesc,
        const NiFixedString& kName);
    virtual bool LoadEntry(efd::BinaryStream& kStream, bool& bDeprecated);

    bool SaveString(efd::BinaryStream& kStream, const char* pcString);
    bool LoadString(efd::BinaryStream& kStream, char* pcBuffer,
        size_t stBufferSize);

    virtual bool SaveGPUProgram(efd::BinaryStream& kStream,
        NiGPUProgram* pkProgram) = 0;
    virtual bool LoadGPUProgram(efd::BinaryStream& kStream,
        const NiFixedString& kName, NiGPUProgramPtr& spProgram,
        bool bSkip) = 0;

    NiTFixedStringMap<NiGPUProgramDescPtr> m_kCachedPrograms;
    NiTFixedStringMap<bool> m_kFailedPrograms;

    NiFixedString m_kFilename;
    NiFixedString m_kWorkingDir;
    NiFixedString m_kPathAndFilename;
    NiFixedString m_kShaderProfile;
    NiTObjectPtrSet<NiFixedString> m_kValidTargets;
    unsigned int m_uiVersion;

    bool m_bAutoWriteCacheToDisk;
    bool m_bLoaded;
    bool m_bLocked;

    static bool ms_bOverwriteDeprecatedCacheFiles;
};

typedef efd::SmartPointer<NiGPUProgramCache> NiGPUProgramCachePtr;

#endif  //#ifndef NIGPUPROGRAMCACHE_H
