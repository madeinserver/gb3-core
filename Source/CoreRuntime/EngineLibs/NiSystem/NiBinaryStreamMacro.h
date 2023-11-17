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

#if !defined(EE_REMOVE_LEGACY_NI)
// This macro should be used within the class declaration of a class
// that derives from binary stream.
#define NiDeclareDerivedBinaryStream() \
    protected: \
    static unsigned int ReadNoSwap(efd::BinaryStream* pkThis, \
        void* pvBuffer, unsigned int uiBytes, \
        unsigned int* puiComponentSizes,unsigned int uiNumComponents); \
    static unsigned int WriteNoSwap(efd::BinaryStream* pkThis, \
        const void* pvBuffer, unsigned int uiBytes, \
        unsigned int* puiComponentSizes,unsigned int uiNumComponents); \
    static unsigned int ReadAndSwap(efd::BinaryStream* pkThis, \
        void* pvBuffer, unsigned int uiBytes, \
        unsigned int* puiComponentSizes,unsigned int uiNumComponents); \
    static unsigned int WriteAndSwap(efd::BinaryStream* pkThis, \
        const void* pvBuffer, unsigned int uiBytes, \
        unsigned int* puiComponentSizes,unsigned int uiNumComponents);

// This macro should be used inside the implementation .cpp file of
// a class that derives from binary stream.
#define NiImplementDerivedBinaryStream(classname, rawread, rawwrite) \
\
unsigned int classname::ReadNoSwap(efd::BinaryStream* pkThis, \
    void* pvBuffer, unsigned int uiBytes, \
    unsigned int*, unsigned int) \
{ \
    return ((classname*)pkThis)->rawread(pvBuffer, uiBytes); \
} \
\
unsigned int classname::WriteNoSwap(efd::BinaryStream* pkThis, \
    const void* pvBuffer, unsigned int uiBytes, \
    unsigned int*, unsigned int) \
{ \
    return ((classname*)pkThis)->rawwrite(pvBuffer, uiBytes); \
} \
\
unsigned int classname::ReadAndSwap(efd::BinaryStream* pkThis, void* pvBuffer, \
    unsigned int uiBytes, unsigned int* puiComponentSizes, \
    unsigned int uiNumComponents) \
{ \
    if (uiBytes <= 0) /* On some platforms, reading 0 bytes is bad */ \
        return 0; \
    unsigned int uiBytesRead = ((classname*)pkThis)->rawread(pvBuffer, \
        uiBytes); \
    DoByteSwap(pvBuffer, uiBytes, puiComponentSizes, uiNumComponents); \
\
    return uiBytesRead; \
} \
\
unsigned int classname::WriteAndSwap(efd::BinaryStream* pkThis, \
    const void* pvBuffer, unsigned int uiBytes, \
    unsigned int* puiComponentSizes, unsigned int uiNumComponents) \
{ \
    if (uiBytes <= 0) /* On some platforms, writing 0 bytes is bad */ \
        return 0; \
\
    /* Swap a temporary array */ \
    unsigned char* pucTmpArray = NiAlloc(unsigned char, uiBytes); \
    NiMemcpy((void*)pucTmpArray, pvBuffer, uiBytes); \
\
    DoByteSwap(pucTmpArray, uiBytes, puiComponentSizes, uiNumComponents); \
    unsigned int uiBytesWritten = ((classname*)pkThis)->rawwrite( \
        pucTmpArray, uiBytes); \
\
    NiFree(pucTmpArray); \
    return uiBytesWritten; \
} \
\
void classname::SetEndianSwap(bool bDoSwap) \
{ \
    if (bDoSwap) \
    { \
        m_pfnRead = &classname::ReadAndSwap; \
        m_pfnWrite = &classname::WriteAndSwap; \
    } \
    else \
    { \
        m_pfnRead = &classname::ReadNoSwap; \
        m_pfnWrite = &classname::WriteNoSwap; \
    } \
}
#endif // !defined(EE_REMOVE_LEGACY_NI)
