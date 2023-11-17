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
#ifndef NISTREAM_H
#define NISTREAM_H
#ifndef __SPU__
#include <NiSemaphore.h>
#include <NiThread.h>

#include "NiObject.h"
#include "NiFixedString.h"
#include "NiTLargeArray.h"
#include "NiTStringMap.h"
#include "NiTSet.h"
//#include <NiThreadProcedure.h>
//#include <NiProcessorAffinity.h>

NiSmartPointer(NiTexture);
NiSmartPointer(NiTexturePalette);

class NiSearchPath;
class NiAVObject;

class NIMAIN_ENTRY NiStream : public NiMemObject
{
public:
    NiStream();
    virtual ~NiStream();

    // top-level objects to process
    void InsertObject(NiObject* pkObject);
    void RemoveObject(NiObject* pkObject);
    void RemoveAllObjects();
    inline unsigned int GetObjectCount() const;
    inline NiObject* GetObjectAt(unsigned int i) const;

    // file loads and saves
    virtual bool Load(const char* pcFileName);
    virtual bool Save(const char* pcFileName);

    // character stream loads and save
    virtual bool Load(char* pcBuffer, int iBufferSize);
    virtual bool Save(char*& pcBuffer, int& iBufferSize);

    // Direct stream loads and save
    virtual bool Load(efd::BinaryStream* pkIstr);
    virtual bool Save(efd::BinaryStream* pkOstr);

    // meta-data manipulation flags
    void AddMetaDataBuffer(unsigned int uiTag, unsigned int uiBufferSize,
        const NiUInt8* ucBuffer);

    enum CommonMetaDataTags
    {
        EMBEDDED_THUMBNAIL = 1
    };

    inline unsigned int GetMetaDataBufferCount() const;
    inline unsigned int GetMetaDataTag(unsigned int uiWhichBuffer) const;
    inline const NiUInt8* GetMetaDataBuffer(unsigned int uiWhichBuffer,
        unsigned int& uiBufferSize) const;
    inline bool GetLoadMetaData() const;
    inline void SetLoadMetaData(bool bLoad);
    inline void ClearMetaData();
    virtual bool LoadOnlyMetaData(const char* pcFileName);
    virtual bool LoadOnlyMetaData(char* pcBuffer, int iBufferSize);
    virtual bool LoadOnlyMetaData(efd::BinaryStream* pkIstr);

    // Get/set whether this stream should be saved in little endian.
    // By default, it will save out the same endianness as the current
    // platform.
    bool GetSaveAsLittleEndian();
    void SetSaveAsLittleEndian(bool bLittle);

    // Get/set whether a loaded stream is little endian.  By default,
    // this will return the same endianness as the current platform.
    bool GetSourceIsLittleEndian();

    // background loading
    enum ThreadStatus
    {
        IDLE,
        LOADING,
        CANCELLING,
        PAUSING,
        PAUSED
    };

    class LoadState : public NiMemObject
    {
    public:
        float m_fReadProgress; // [0.0f..1.0f]
        float m_fLinkProgress; // [0.0f..1.0f]
    };

    void BackgroundLoad();
    void BackgroundLoadBegin(const char* pcFileName);
    void BackgroundLoadBegin(efd::BinaryStream* pkIstr);
    ThreadStatus BackgroundLoadPoll(LoadState* pkState);
    void BackgroundLoadPause();
    void BackgroundLoadResume();
    void BackgroundLoadCancel();
    bool BackgroundLoadFinish();
    void BackgroundLoadCleanup();
    inline bool BackgroundLoadGetExitStatus() const;
    virtual void BackgroundLoadOnExit();

    inline const NiProcessorAffinity& GetAffinity() const;
    inline void SetAffinity(const NiProcessorAffinity& kAffinity);
    inline NiThread::Priority GetPriority() const;
    inline void SetPriority(const NiThread::Priority ePriority);

    class BackgroundLoadProcedure : public NiThreadProcedure
    {
    public:
        inline BackgroundLoadProcedure(NiStream* pkStream)
        {
            m_pkStream = pkStream;
        }

        virtual unsigned int ThreadProcedure(NiThread* pkThread);

        NiStream* m_pkStream;
    };

    // version information
    // GetVersionFromString should only be used on strings of the form w.x.y.z where
    // each of w, x, y and z are integers 0-255.
    static unsigned int GetVersionFromString(const char* pcVersionString);
    static unsigned int GetVersion(unsigned int uiMajor, unsigned int uiMinor,
        unsigned int uiPatch, unsigned int uiInternal);
    inline unsigned int GetFileVersion() const;
    inline unsigned int GetFileUserDefinedVersion() const;

    // error messages
    inline unsigned int GetLastError() const;
    inline const char* GetLastErrorMessage() const;
    inline void ResetLastErrorInfo();

    // error messages
    enum
    {
        STREAM_OKAY,
        FILE_NOT_LOADED,
        NOT_NIF_FILE,
        OLDER_VERSION,
        LATER_VERSION,
        NO_CREATE_FUNCTION,
        ENDIAN_MISMATCH
    };

    // optional image palette (allows image sharing across streams)
    void SetTexturePalette(NiTexturePalette* pkTexturePalette);
    NiTexturePalette* GetTexturePalette() const;

    // *** begin Emergent internal use only ***

    // string loads and saves
    void LoadCString(char*& pcString);
    void SaveCString(const char* pcString);
    void LoadFixedString(NiFixedString& kString);
    void SaveFixedString(const NiFixedString& kString);
    void LoadCStringAsFixedString(NiFixedString& kString);

    static const unsigned int NULL_LINKID; /* = 0xffffffff */

    virtual bool RegisterFixedString(const NiFixedString& kString);
    virtual bool RegisterSaveObject(NiObject* pkObject);
    virtual void ChangeObject(NiObject* pkNewObject,
        bool bLink = true);
    virtual unsigned int GetLinkIDFromObject(const NiObject* pkObject) const;
    virtual void SaveLinkID(const NiObject* pkObject);
    virtual NiObject* ResolveLinkID();
    virtual void ReadLinkID();
    virtual NiObject* GetObjectFromLinkID();

    unsigned int ReadMultipleLinkIDs();
    unsigned int GetNumberOfLinkIDs();
    void SetNumberOfLinkIDs(unsigned int uiLinks);

    NiObjectGroup* GetGroupFromID(unsigned int uiID) const;
    unsigned int GetIDFromGroup(NiObjectGroup* pkGroup) const;

    // for internal read/write of class data members
    inline efd::BinaryStream& Istr();
    inline efd::BinaryStream& Ostr();

    // image palette access
    void SetTexture(const char* pcURL, NiTexture* pkTexture);
    NiTexture* GetTexture(const char* pcURL, NiTexture* pkSelf) const;

    inline NiSearchPath* GetSearchPath() const;
    void SetSearchPath(NiSearchPath* pkSearchPath);

    // last loaded RTTI string
    inline const char* GetLastLoadedRTTI() const;

    // Cached flag values for legacy NIF files.
    inline unsigned short GetLastNiAVObjectFlags() const;
    inline void SetLastNiAVObjectFlags(unsigned short usFlags);

    inline unsigned short GetLastNiTimeControllerFlags() const;
    inline void SetLastNiTimeControllerFlags(unsigned short usFlags);

    inline unsigned short GetLastNiPropertyFlags() const;
    inline void SetLastNiPropertyFlags(unsigned short usFlags);

    // Indicates if mesh modifers should be prepared
    inline bool GetPrepareMeshModifiers();

    // Sets if mesh modifers should be prepared
    inline void SetPrepareMeshModifiers(bool bEnable);

    typedef NiTPointerMap<const NiAVObject*, NiAVObject*> ConversionMap;
    inline NiStream::ConversionMap& GetConversionMap();

    // Locking functions to protect cleanup code for threading issues.
    static void LockCleanupSection();
    static void UnlockCleanupSection();

    // support for object loader functions
    typedef NiObject* (*CreateFunction)(const char**, unsigned int);
    static int RegisterLoader(const char* pcClassName, CreateFunction pfnFunc);
    static void UnregisterLoader(const char* pcClassName);

    // Support for scene graph post-processing functions.
    typedef void (*PostProcessFunction)(NiStream&,
        NiTLargeObjectArray<NiObjectPtr>& kTopObjects);
    typedef void (*PostProcessFinalFunction)(NiStream&,
        NiTLargeObjectArray<NiObjectPtr>& kTopObjects);

    static void SetPostProcessFinalFunction(PostProcessFinalFunction pfnFunc);
    static void RegisterPostProcessFunction(PostProcessFunction pfnFunc);
    static void UnregisterPostProcessFunction(PostProcessFunction pfnFunc);

    static void _SDMInit();
    static void _SDMShutdown();
    static NiObject* CreateObjectByRTTI(const char* pcRTTI,
        const char** pcArgs = 0, unsigned int uiArgCount = 0);

    static const char ms_cRTTIDelimiter = 0x01;

    class NIMAIN_ENTRY StreamObjectArrayElement : public NiMemObject
    {
    public:
        StreamObjectArrayElement(NiObject* pkObject = 0);
        StreamObjectArrayElement(NiObject* pkObject,
            NiObject *pkObjectToLink);
        StreamObjectArrayElement(const StreamObjectArrayElement &kOther);
        ~StreamObjectArrayElement();
        StreamObjectArrayElement & operator=(
            const StreamObjectArrayElement& kOther);
        operator NiObject*() const;
        NiObject* operator->() const;
        NiObject* GetObjectToLink() const;

    private:
        NiObjectPtr m_spObject;
        NiObjectPtr m_spObjectToLink;
    };

    class StreamMetaDataObject : public NiRefObject
    {
    public:
        inline StreamMetaDataObject(unsigned int uiTag,
            unsigned int uiBufferSize,
            const NiUInt8* ucBuffer);
        inline StreamMetaDataObject();
        inline ~StreamMetaDataObject();
        unsigned int m_uiTag;
        unsigned int m_uiBufferSize;
        NiUInt8* m_pucBuffer;
    };
    typedef efd::SmartPointer<NiStream::StreamMetaDataObject>
        StreamMetaDataObjectPtr;


    // *** end Emergent internal use only ***

protected:
    // load and save information
    virtual bool LoadHeader();
    virtual void SaveHeader();
    virtual bool LoadStream();
    virtual bool LoadStreamOnlyMetaData();
    virtual bool SaveStream();
    virtual void RegisterObjects();
    virtual void LoadTopLevelObjects();
    virtual void SaveTopLevelObjects();
    virtual bool LoadObject();
    virtual unsigned int PreSaveObjectSizeTable();
    virtual bool SaveObjectSizeTable(unsigned int uiStartOffset);
    virtual bool LoadObjectSizeTable();
    void FreeLoadData();
    void DoThreadPause();

    // automatically set UpdateSelected flags for old NIF files
    // to max functionality
    void SetSelectiveUpdateFlagsForOldVersions();
    void SetSelectiveUpdateFlagsTTTFRecursive(NiAVObject* pkAVObject);

    // RTTI
    void LoadRTTIString(char* pcString);
    virtual bool LoadRTTI();
    void SaveRTTI();
    void RTTIError(const char* pcRTTI);

    // NiFixedString
    unsigned int GetStringID(const NiFixedString& kString);
    void SaveFixedStringTable();
    bool LoadFixedStringTable();

    // MetaData
    bool LoadMetaData(bool bLoadMetaData);
    void SaveMetaData();

    // block allocation
    void LoadObjectGroups();
    void SaveObjectGroups();
    void UpdateObjectGroups();
    typedef NiTPrimitiveArray<NiObjectGroup*> NiObjectGroupArray;
    NiObjectGroupArray m_kGroups;

    unsigned int m_uiNifFileVersion;
    unsigned int m_uiNifFileUserDefinedVersion;
    char m_acFileName[NI_MAX_PATH];// needed to resolve names of external nifs
    bool m_bSaveLittleEndian;
    bool m_bSourceIsLittleEndian;

    NiSearchPath* m_pkSearchPath;

    NiTLargeObjectArray<StreamObjectArrayElement> m_kObjects;
    NiTLargePrimitiveArray<unsigned int> m_kObjectSizes;
    NiTLargeObjectArray<NiObjectPtr> m_kTopObjects;
    NiTLargeObjectArray<NiFixedString> m_kFixedStrings;
    NiTObjectArray<StreamMetaDataObjectPtr> m_kMetaDataObjects;
    efd::BinaryStream* m_pkIstr;
    efd::BinaryStream* m_pkOstr;

    bool m_bLoadMetaData;

    // ordered set of link id's for load-link phase
    NiUnsignedIntSet m_kLinkIDs;
    unsigned int m_uiLinkIndex;
    NiUnsignedIntSet m_kLinkIDBlocks;
    unsigned int m_uiLinkBlockIndex;

    // Hash table to convert registered object pointers to IDs at save time.
    NiTPointerMap<const NiObject*, unsigned int> m_kRegisterMap;

    // Hash table for tranlating converted objects into new objects.
    // Ex. conversion from NiGeometry objects to NiRenderObjectImpl ones..
    ConversionMap m_kConversionMap;

    // image palette for sharing NiTexture objects
    NiTexturePalettePtr m_spTexturePalette;

    // Legacy NIF flag conversion variables
    unsigned short m_usNiAVObjectFlags;
    unsigned short m_usNiTimeControllerFlags;
    unsigned short m_usNiPropertyFlags;

    // background loading
    void BackgroundLoadBegin();
    void BackgroundLoadEstimateProgress(LoadState& kLoadState);

    ThreadStatus m_eBackgroundLoadStatus;
    bool m_bBackgroundLoadExitStatus;
    unsigned int m_uiLoad;
    unsigned int m_uiLink;
    unsigned int m_uiPostLink;

    // NiThread implementation
    NiSemaphore m_kSemaphore;
    NiThread* m_pkThread;
    BackgroundLoadProcedure* m_pkBGLoadProc;
    NiThread::Priority m_ePriority;
    NiProcessorAffinity m_kAffinity;
    bool m_bPrepareModifiers;

    enum
    {
        SKIPPABLE_MASK = 0x8000
    };

    // static data
    static const unsigned int ms_uiNifMinVersion;
    static const unsigned int ms_uiNifMaxVersion;
    static const unsigned int ms_uiNifMinUserDefinedVersion;
    static const unsigned int ms_uiNifMaxUserDefinedVersion;

    char m_acLastLoadedRTTI[NI_MAX_PATH];
    unsigned int m_uiLastError;
    char m_acLastErrorMessage[NI_MAX_PATH];

    // support for object loader functions
    static NiTStringPointerMap<CreateFunction>* ms_pkLoaders;

    // Support for scene graph post-processing functions.
    typedef NiTPrimitiveArray<PostProcessFunction> PostProcessFunctionArray;

    static PostProcessFunctionArray* ms_pkPostProcessFunctions;
    static PostProcessFinalFunction ms_pkPostProcessFinalFunction;

    static efd::CriticalSection ms_kCleanupCriticalSection;
};

#include "NiStream.inl"

#endif // __SPU__
#endif // NISTREAM_H
