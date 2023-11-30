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

#include <wctype.h>

namespace efd
{

//--------------------------------------------------------------------------------------------------
inline efd::Char* Strcpy(efd::Char* dest, size_t destSize, const efd::Char* src)
{
#ifdef EE_HAVE_SECURE_FUNCTIONS
    strcpy_s(dest, destSize, src);
    return dest;
#else // #if EE_HAVE_SECURE_FUNCTIONS

    EE_ASSERT(destSize != 0);

    size_t stSrcLen = strlen(src);
    EE_ASSERT(destSize > stSrcLen); // > because need null character

    size_t stWrite;

    if (destSize <= stSrcLen) // destSize < stSrcLen + 1
        stWrite = destSize;
    else
        stWrite = stSrcLen + 1;

    efd::Char *pcResult = strncpy(dest, src, stWrite);
    pcResult[destSize - 1] = '\0';
    return pcResult;
#endif // #if EE_HAVE_SECURE_FUNCTIONS
}

//--------------------------------------------------------------------------------------------------
inline efd::Char* Strncpy(efd::Char* dest, size_t destSize, const efd::Char* src, size_t count)
{
#ifdef EE_HAVE_SECURE_FUNCTIONS
    strncpy_s(dest, destSize, src, count);
    return dest;
#else // #if EE_HAVE_SECURE_FUNCTIONS

    EE_ASSERT(dest != 0 && destSize != 0);
    EE_ASSERT(count < destSize || count == EE_TRUNCATE);

    if (count >= destSize)
    {
        if (count != EE_TRUNCATE)
        {
            dest[0] = '\0';
            return dest;
        }
        else
        {
            count = destSize - 1;
        }
    }

    efd::Char* pcResult = strncpy(dest, src, count);
    pcResult[count] = '\0';

    return pcResult;
#endif // #ifdef EE_HAVE_SECURE_FUNCTIONS
}

//--------------------------------------------------------------------------------------------------
inline efd::Char* Strcat(efd::Char* dest, size_t destSize, const efd::Char* src)
{
#ifdef EE_HAVE_SECURE_FUNCTIONS
    strcat_s(dest, destSize, src);
    return dest;
#else // #ifdef EE_HAVE_SECURE_FUNCTIONS

    size_t stSrcLen = strlen(src);
    size_t stDestLen = strlen(dest);
    EE_ASSERT(stSrcLen + stDestLen <= destSize - 1);

    size_t stWrite = destSize - 1 - stDestLen;

    efd::Char* pcResult = strncat(dest, src, stWrite);
    pcResult[destSize - 1] = '\0';
    return pcResult;

#endif // #ifdef EE_HAVE_SECURE_FUNCTIONS
}

//--------------------------------------------------------------------------------------------------
inline efd::Char* Strncat(efd::Char* dest, size_t destSize, const efd::Char* src, size_t count)
{
#ifdef EE_PLATFORM_WIN32
    strncat_s(dest, destSize, src, count);
#else
    strncat(dest, src, count);
#endif
    return dest;
}

//--------------------------------------------------------------------------------------------------
inline efd::Char* Strtok(efd::Char* str, const efd::Char* delimit, efd::Char** ppcContext)
{
#ifdef EE_HAVE_SECURE_FUNCTIONS
    return strtok_s(str, delimit, ppcContext);
#else // #ifdef EE_HAVE_SECURE_FUNCTIONS
    return strtok(str, delimit);
#endif // #ifdef EE_HAVE_SECURE_FUNCTIONS
}

//--------------------------------------------------------------------------------------------------
inline efd::SInt32 Strupr(efd::Char* dest, size_t destSize)
{
#ifndef EE_HAVE_SECURE_FUNCTIONS
    char* tmp = dest;

    for (;*tmp;++tmp) {
        if (destSize == 0)
            return -1;

        *tmp = toupper((unsigned char)*tmp);
        destSize--;
    }

    return 0;
#else
    return _strupr_s(dest, destSize);
#endif
}

//--------------------------------------------------------------------------------------------------
inline efd::SInt32 Strlwr(efd::Char* dest, size_t destSize)
{
#ifndef EE_HAVE_SECURE_FUNCTIONS
    char* tmp = dest;

    for (;*tmp;++tmp) {
        if (destSize == 0)
            return -1;

        *tmp = tolower((unsigned char)*tmp);
        destSize--;
    }
#else
    _strlwr_s(dest, destSize);
#endif

    return 0;
}

//--------------------------------------------------------------------------------------------------
inline efd::SInt32 Memcpy(void* dest, size_t destSize, const void* src,
    size_t count)
{
    efd::SInt32 ret = 0;

#ifdef EE_HAVE_SECURE_FUNCTIONS
    ret = memcpy_s(dest, destSize, src, count);
#else // #ifdef EE_HAVE_SECURE_FUNCTIONS
    if (destSize < count)
        ret = -1;
    else
        memcpy(dest, src, count);
#endif // #ifdef EE_HAVE_SECURE_FUNCTIONS

    EE_ASSERT(ret != -1);
    return ret;
}

//--------------------------------------------------------------------------------------------------
inline efd::SInt32 Memcpy(void* dest, const void* src, size_t count)
{
    // This version of Memcpy is to be used to retrofit existing code,
    // however, the other version of Memcpy should be used whenever
    // possible.
    return Memcpy(dest, count, src, count);
}

//--------------------------------------------------------------------------------------------------
inline efd::SInt32 Memmove(void* dest, size_t destSize, const void* src, size_t count)
{
    efd::SInt32 ret = 0;

#ifdef EE_HAVE_SECURE_FUNCTIONS
    ret = memmove_s(dest, destSize, src, count);
#else // #ifdef EE_HAVE_SECURE_FUNCTIONS
    if (destSize < count)
        ret = -1;
    else
        memmove(dest, src, count);
#endif // #ifdef EE_HAVE_SECURE_FUNCTIONS

    EE_ASSERT(ret != -1);
    return ret;
}

//--------------------------------------------------------------------------------------------------
// WChar version of function to copy one string to another.
inline efd::WChar* WStrcpy(efd::WChar* dest, size_t destSize, const efd::WChar* src)
{
#ifdef EE_HAVE_SECURE_FUNCTIONS
    wcscpy_s((wchar_t *)dest, destSize, (const wchar_t *)src);
    return dest;
#else // #ifdef EE_HAVE_SECURE_FUNCTIONS
    EE_ASSERT(destSize != 0);

    size_t stSrcLen = wcslen((const wchar_t*)src);
    EE_ASSERT(destSize > stSrcLen); // > because need null character

    size_t stWrite;

    if (destSize <= stSrcLen) // destSize < stSrcLen + 1
        stWrite = destSize;
    else
        stWrite = stSrcLen + 1;

    WChar* pkResult = (WChar*)wcsncpy((wchar_t*)dest, (const wchar_t*)src, stWrite);
    pkResult[destSize - 1] = '\0';
    return pkResult;
#endif // #ifdef EE_HAVE_SECURE_FUNCTIONS
}

//--------------------------------------------------------------------------------------------------
// WChar version of function to copy characters of one string to another.
inline efd::WChar* WStrncpy(efd::WChar* dest, size_t destSize, const efd::WChar* src, size_t count)
{
#ifdef EE_HAVE_SECURE_FUNCTIONS
    wcsncpy_s((wchar_t *)dest, destSize, (const wchar_t *)src, count);
    return dest;
#else // #ifdef EE_HAVE_SECURE_FUNCTIONS
    EE_ASSERT(dest != 0 && destSize != 0);
    EE_ASSERT(count < destSize || count == EE_TRUNCATE);

    if (count >= destSize)
    {
        if (count != EE_TRUNCATE)
        {
            dest[0] = '\0';
            return dest;
        }
        else
        {
            count = destSize - 1;
        }
    }

    WChar* pkResult = (WChar*)wcsncpy((wchar_t*)dest, (const wchar_t*)src, count);
    pkResult[count] = '\0';

    return pkResult;
#endif // #ifdef EE_HAVE_SECURE_FUNCTIONS
}

//--------------------------------------------------------------------------------------------------
// WChar version of function to append characters of a string.
inline efd::WChar* WStrcat(efd::WChar* dest, size_t destSize, const efd::WChar* src)
{
#ifdef EE_HAVE_SECURE_FUNCTIONS
    wcscat_s((wchar_t *)dest, destSize, (const wchar_t *)src);
    return dest;
#else // #ifdef EE_HAVE_SECURE_FUNCTIONS
    size_t stSrcLen = wcslen((const wchar_t*)src);
    size_t stDestLen = wcslen((const wchar_t*)dest);
    EE_ASSERT(stSrcLen + stDestLen <= destSize - 1);

    size_t stWrite = destSize - 1 - stDestLen;

    WChar* pkResult = (WChar*)wcsncat((wchar_t*)dest, (const wchar_t*)src, stWrite);
    pkResult[destSize - 1] = '\0';
    return pkResult;

#endif // #ifdef EE_HAVE_SECURE_FUNCTIONS
}

//--------------------------------------------------------------------------------------------------
// WChar version of function to find the next token in a string.
inline efd::WChar* WStrtok(efd::WChar* str, const efd::WChar* delimiters, efd::WChar** ppContext)
{
#ifdef EE_HAVE_SECURE_FUNCTIONS
    return (WChar *)wcstok_s((wchar_t *)str,
        (const wchar_t *)delimiters, (wchar_t **)ppContext);
#else // #ifdef EE_HAVE_SECURE_FUNCTIONS
    return (WChar*)wcstok((wchar_t*)str, (const wchar_t*)delimiters, (wchar_t**)ppContext);
#endif // #ifdef EE_HAVE_SECURE_FUNCTIONS
}

//--------------------------------------------------------------------------------------------------
inline efd::SInt32 WStrupr(efd::WChar* dest, size_t destSize)
{
    EE_COMPILETIME_ASSERT(sizeof(wchar_t) == sizeof(efd::WChar));
#ifdef EE_HAVE_SECURE_FUNCTIONS
    return _wcsupr_s((wchar_t*)(dest), destSize);
#else
    wchar_t* tmp = (wchar_t*)dest;

    for (;*tmp;++tmp) {
        if (destSize == 0)
            return -1;

        *tmp = towupper(*tmp);
        destSize--;
    }

    return 0;
#endif
}

//--------------------------------------------------------------------------------------------------
inline efd::SInt32 WStrlwr(efd::WChar* dest, size_t destSize)
{
    EE_COMPILETIME_ASSERT(sizeof(wchar_t) == sizeof(efd::WChar));
#ifdef EE_HAVE_SECURE_FUNCTIONS
    return _wcslwr_s((wchar_t*)(dest), destSize);
#else
    wchar_t* tmp = (wchar_t*)dest;

    for (;*tmp;++tmp) {
        if (destSize == 0)
            return -1;

        *tmp = towlower(*tmp);
        destSize--;
    }

    return 0;
#endif
}

//--------------------------------------------------------------------------------------------------
inline efd::UInt64 Atoi64(const efd::Char* buffer)
{
#ifdef EE_PLATFORM_WIN32
    return _atoi64(buffer);
#else
    return (efd::UInt64)atoll(buffer);
#endif
}

//--------------------------------------------------------------------------------------------------
inline efd::SInt32 strtol(const efd::Char* buffer,
                           const efd::Char** endPtr,
                           efd::UInt8 base /*= 0 */)
{
    return ::strtol(buffer, const_cast<efd::Char**>(endPtr), base);
}

//--------------------------------------------------------------------------------------------------
inline efd::UInt32 strtoul(const efd::Char* buffer,
                            const efd::Char** endPtr,
                            efd::UInt8 base /*= 0 */)
{
    return ::strtoul(buffer, const_cast<efd::Char**>(endPtr), base);
}

//--------------------------------------------------------------------------------------------------
inline efd::SInt64 strtoll(const efd::Char* buffer,
                            const efd::Char** endPtr,
                            efd::UInt8 base /*= 0 */)
{
#if defined(EE_PLATFORM_WIN32) || defined(EE_PLATFORM_XBOX360)
    return _strtoi64(buffer, const_cast<efd::Char**>(endPtr), base);
#else
    return ::strtoll(buffer, const_cast<efd::Char**>(endPtr), base);
#endif
}

//--------------------------------------------------------------------------------------------------
inline efd::UInt64 strtoull(const efd::Char* buffer,
                             const efd::Char** endPtr,
                             efd::UInt8 base /*= 0 */)
{
#if defined(EE_PLATFORM_WIN32) || defined(EE_PLATFORM_XBOX360)
    return _strtoui64(buffer, const_cast<efd::Char**>(endPtr), base);
#else
    return ::strtoull(buffer, const_cast<efd::Char**>(endPtr), base);
#endif
}

//--------------------------------------------------------------------------------------------------
inline efd::Float32 strtof(const efd::Char* buffer, const efd::Char** endPtr)
{
    return (efd::Float32) ::strtod(buffer, const_cast<efd::Char**>(endPtr));
}

//--------------------------------------------------------------------------------------------------
inline efd::Float64 strtod(const efd::Char* buffer, const efd::Char** endPtr)
{
    return ::strtod(buffer, const_cast<efd::Char**>(endPtr));
}

//--------------------------------------------------------------------------------------------------
inline efd::UInt32 Strlen(const efd::Char* str)
{
    return (efd::UInt32)::strlen(str);
}

//--------------------------------------------------------------------------------------------------
inline efd::UInt32 Strlen(const efd::WChar* str)
{
    EE_COMPILETIME_ASSERT(sizeof(wchar_t) == sizeof(efd::WChar));
    return (efd::UInt32)::wcslen((wchar_t*)str);
}

//--------------------------------------------------------------------------------------------------
inline efd::SInt32 Strcoll(const efd::Char* str1, const efd::Char* str2)
{
    return ::strcoll(str1, str2);
}

//--------------------------------------------------------------------------------------------------
inline efd::SInt32 Strcoll(const efd::WChar* str1, const efd::WChar* str2)
{
    // wchar_t is 16 bit on windows and 32bit on Linux, this wrapper converts from our always
    // 16 bit WChar to call the native API.
    return ::wcscoll(efd::WStrToNative(str1), efd::WStrToNative(str2));
}

//--------------------------------------------------------------------------------------------------
} // end namespace efd
