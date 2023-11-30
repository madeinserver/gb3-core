// EMERGENT GAME TECHNOLOGIES PROPRIETARY INFORMATION
//
// This software is supplied under the terms of a license agreement or
// nondisclosure agreement with Emergent Game Technologies and may not
// be copied or disclosed except in accordance with the terms of that
// agreement.
//
//      Copyright (c) 2023 Arves100/Made In Server Developers.
//      Copyright (c) 1996-2009 Emergent Game Technologies.
//      All Rights Reserved.
//
// Emergent Game Technologies, Calabasas, CA 91302
// http://www.emergent.net

namespace efd
{

    //-------------------------------------------------------------------------------------------------
    inline efd::SAtomic AtomicIncrement(efd::SAtomic& value)
    {
#if (defined(EE_PLATFORM_WIN32) && _WIN32_WINNT >= 0x600) || defined(EE_PLATFORM_XBOX360)
        return InterlockedIncrement64((LONGLONG*)&value);
#elif (defined(EE_PLATFORM_WIN32) && _WIN32_WINNT < 0x600)
        return InterlockedIncrement((LONG*)&value);
#elif defined(EE_PLATFORM_LINUX)
        __sync_add_and_fetch(&value, 1);
        return value;
#endif
    }
    //-------------------------------------------------------------------------------------------------
    inline efd::SAtomic AtomicDecrement(efd::SAtomic& value)
    {
#if (defined(EE_PLATFORM_WIN32) && _WIN32_WINNT >= 0x600) || defined(EE_PLATFORM_XBOX360)
        return InterlockedDecrement64((LONGLONG*)&value);
#elif (defined(EE_PLATFORM_WIN32) && _WIN32_WINNT < 0x600)
        return InterlockedDecrement((LONG*)&value);
#elif defined(EE_PLATFORM_LINUX)
        __sync_sub_and_fetch(&value, 1);
        return value;
#endif
    }
    //-------------------------------------------------------------------------------------------------
    inline efd::SAtomic AtomicIncrement(volatile efd::SAtomic& value)
    {
#if (defined(EE_PLATFORM_WIN32) && _WIN32_WINNT >= 0x600) || defined(EE_PLATFORM_XBOX360)
        return InterlockedIncrement64((LONGLONG*)&value);
#elif (defined(EE_PLATFORM_WIN32) && _WIN32_WINNT < 0x600)
        return InterlockedIncrement((LONG*)&value);
#elif defined(EE_PLATFORM_LINUX)
        __sync_add_and_fetch(&value, 1);
        return value;
#endif
    }
    //-------------------------------------------------------------------------------------------------
    inline efd::SAtomic AtomicDecrement(volatile efd::SAtomic& value)
    {
#if (defined(EE_PLATFORM_WIN32) && _WIN32_WINNT >= 0x600) || defined(EE_PLATFORM_XBOX360)
        return InterlockedDecrement64((LONGLONG*)&value);
#elif (defined(EE_PLATFORM_WIN32) && _WIN32_WINNT < 0x600)
        return InterlockedDecrement((LONG*)&value);
#elif defined(EE_PLATFORM_LINUX)
        __sync_sub_and_fetch(&value, 1);
        return value;
#endif
    }
    //-------------------------------------------------------------------------------------------------
    inline efd::UAtomic AtomicIncrement(efd::UAtomic& value)
    {
#if (defined(EE_PLATFORM_WIN32) && _WIN32_WINNT >= 0x600) || defined(EE_PLATFORM_XBOX360)
        return InterlockedIncrement64((LONGLONG*)&value);
#elif (defined(EE_PLATFORM_WIN32) && _WIN32_WINNT < 0x600)
        return InterlockedIncrement((LONG*)&value);
#elif defined(EE_PLATFORM_LINUX)
        __sync_add_and_fetch(&value, 1);
        return value;
#endif
    }
    //-------------------------------------------------------------------------------------------------
    inline efd::UAtomic AtomicDecrement(efd::UAtomic& value)
    {
#if (defined(EE_PLATFORM_WIN32) && _WIN32_WINNT >= 0x600) || defined(EE_PLATFORM_XBOX360)
        return InterlockedDecrement64((LONGLONG*)&value);
#elif (defined(EE_PLATFORM_WIN32) && _WIN32_WINNT < 0x600)
        return InterlockedDecrement((LONG*)&value);
#elif defined(EE_PLATFORM_LINUX)
        __sync_sub_and_fetch(&value, 1);
        return value;
#endif
    }
    //-------------------------------------------------------------------------------------------------
    inline efd::UAtomic AtomicIncrement(volatile efd::UAtomic& value)
    {
#if (defined(EE_PLATFORM_WIN32) && _WIN32_WINNT >= 0x600) || defined(EE_PLATFORM_XBOX360)
        return InterlockedIncrement64((LONGLONG*)&value);
#elif (defined(EE_PLATFORM_WIN32) && _WIN32_WINNT < 0x600)
        return InterlockedIncrement((LONG*)&value);
#elif defined(EE_PLATFORM_LINUX)
        __sync_add_and_fetch(&value, 1);
        return value;
#endif
    }
    //-------------------------------------------------------------------------------------------------
    inline efd::UAtomic AtomicDecrement(volatile efd::UAtomic& value)
    {
#if (defined(EE_PLATFORM_WIN32) && _WIN32_WINNT >= 0x600) || defined(EE_PLATFORM_XBOX360)
        return InterlockedDecrement64((volatile LONGLONG*)&value);
#elif (defined(EE_PLATFORM_WIN32) && _WIN32_WINNT < 0x600)
        return InterlockedDecrement((volatile LONG*)&value);
#elif defined(EE_PLATFORM_LINUX)
        __sync_sub_and_fetch(&value, 1);
        return value;
#endif
    }

#if defined(EE_PLATFORM_WIN32) || defined(EE_PLATFORM_XBOX360)
    //-------------------------------------------------------------------------------------------------
    inline void* AtomicCompareAndSwap(
        void* volatile* ppDestination,
        void* pComparand,
        void* pExchange)
    {
        return InterlockedCompareExchangePointer(ppDestination, pExchange, pComparand);
    }
#endif

    //-------------------------------------------------------------------------------------------------
    inline efd::UAtomic AtomicCompareAndSwap(
        efd::UAtomic volatile* pDestination,
        efd::UAtomic comparand,
        efd::UAtomic exchange)
    {
#if (defined(EE_PLATFORM_WIN32) && _WIN32_WINNT >= 0x600)
        return InterlockedCompareExchange64(
            reinterpret_cast<LONGLONG volatile*>(pDestination),
            static_cast<LONGLONG>(exchange),
            static_cast<LONGLONG>(comparand));
#elif (defined(EE_PLATFORM_WIN32) && _WIN32_WINNT < 0x600)
        return InterlockedCompareExchange(
            reinterpret_cast<LONG volatile*>(pDestination),
            static_cast<LONG>(exchange),
            static_cast<LONG>(comparand));
#elif defined(EE_PLATFORM_LINUX)
        __sync_val_compare_and_swap(pDestination, comparand, exchange);
        return *pDestination;
#endif
    }

} // end namespace efd

