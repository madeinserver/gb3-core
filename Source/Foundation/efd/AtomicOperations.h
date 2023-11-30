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
#ifndef EE_ATOMICOPERATIONS_H
#define EE_ATOMICOPERATIONS_H

#include <efd/OS.h>
#include <efd/UniversalTypes.h>
#include <efd/Asserts.h>

namespace efd
{

#if (defined(EE_PLATFORM_WIN32) && _WIN32_WINNT >= 0x600) \
    || defined(EE_PLATFORM_PS3) \
    || defined(EE_PLATFORM_XBOX360) \
    || defined(EE_PLATFORM_LINUX)
    typedef efd::UInt64 UAtomic;
    typedef efd::SInt64 SAtomic;
#elif (defined(EE_PLATFORM_WIN32) && _WIN32_WINNT < 0x600)
    typedef efd::UInt32 UAtomic;
    typedef efd::SInt32 SAtomic;
#else
#error "Missing atomics for specified platform"
#endif


/// atomically increment a variable
inline efd::SAtomic AtomicIncrement(efd::SAtomic &value);

/// atomically decrement a variable
inline efd::SAtomic AtomicDecrement(efd::SAtomic &value);

/// atomically increment a variable
inline efd::UAtomic AtomicIncrement(efd::UAtomic &value);

/// atomically decrement a variable
inline efd::UAtomic AtomicDecrement(efd::UAtomic &value);

/// atomically increment a variable
inline efd::SAtomic AtomicIncrement(volatile efd::SAtomic &value);

/// atomically decrement a variable
inline efd::SAtomic AtomicDecrement(volatile efd::SAtomic &value);

/// atomically increment a variable
inline efd::UAtomic AtomicIncrement(volatile efd::UAtomic &value);

/// atomically decrement a variable
inline efd::UAtomic AtomicDecrement(volatile efd::UAtomic &value);

    /// @name Atomic compare-and-swap operations
    //@{

#if defined(EE_PLATFORM_WIN32) || defined(EE_PLATFORM_XBOX360) || defined(EE_PLATFORM_PS3)
    /**
        Performs an atomic compare-and-swap (CAS) operation on the specified pointer values.

        In pseudocode, this function performs the following operation:
        @code
           atomic
           {
               void* pOldValue = *ppDestination;
               if (pOldValue == pComparand)
                   *ppDestination = pExchange;
               return pOldValue;
           }
        @endcode

        This operation includes a memory barrier.

        @param ppDestination The memory location tested and potentially modified
        @param pComparand The value to test against *ppDestination
        @param pExchange The new value written to *ppDestination if the comparison passed
        @return void* The initial value of *ppDestination
    */
    inline void* AtomicCompareAndSwap(
        void* volatile* ppDestination,
        void* pComparand,
        void* pExchange);
#endif

    /**
        Performs an atomic compare-and-swap (CAS) operation on the specified atomic values.

        In pseudocode, this function performs the following operation:
        @code
           atomic
           {
               UAtomic oldValue = *pDestination;
               if (oldValue == comparand)
                   *pDestination = exchange;
               return oldValue;
           }
        @endcode

        This operation includes a memory barrier.

        @param pDestination The memory location tested and potentially modified
        @param comparand The value to test against *pDestination
        @param exchange The new value written to *pDestination if the comparison passed
        @return UAtomic The initial value of *pDestination
    */
    inline efd::UAtomic AtomicCompareAndSwap(
        efd::UAtomic volatile* pDestination,
        efd::UAtomic comparand,
        efd::UAtomic exchange);

    //@}

/**
    This class contains various atomic helper template methods that ease the use of the atomic
    operations.
  */
class Atomic
{
public:
    /**
        Performs an atomic compare-and-swap (CAS) operation on the specified values.

        In pseudocode, this function performs the following operation:
        @code
           atomic
           {
               UInt32 oldValue = *pDestination;
               if (oldValue == comparand)
                   *pDestination = exchange;
               return oldValue;
           }
        @endcode

        This operation includes a memory barrier.

        @param pDestination The memory location tested and potentially modified
        @param comparand The value to test against *pDestination
        @param exchange The new value written to *pDestination if the comparison passed
        @return T The initial value of *pDestination
    */
    template <typename T>
    static inline T CompareAndSwapReturnInit(volatile T* pDestination, T comparand, T exchange);

    /**
        Performs an atomic compare-and-swap (CAS) operation on the specified values.

        In pseudocode, this function performs the following operation:
        @code
           atomic
           {
               UInt32 oldValue = *pDestination;
               if (oldValue == comparand)
               {
                   *pDestination = exchange;
                   return true;
               }
               else
               {
                   return false;
               }
           }
        @endcode

        This operation includes a memory barrier.

        @param pDestination The memory location tested and potentially modified
        @param comparand The value to test against *pDestination
        @param exchange The new value written to *pDestination if the comparison passed
        @return bool True if the exchange took place
    */
    template <typename T>
    static inline bool CompareAndSwap(volatile T* pDestination, T comparand, T exchange);

    /// Sets a memory location to a value atomically (with a memory barrier)
    template <typename T>
    static inline void SetValue(volatile T* pLocation, T newValue);
private:
    template <int Size, typename T>
    struct SelectMatchingUIntBySize;

#if defined(EE_PLATFORM_PS3) || defined(EE_PLATFORM_XBOX360) || defined(EE_ARCH_64)
    template <typename T>
    struct SelectMatchingUIntBySize<8, T>
    {
        typedef UInt64 Result;
    };
#endif

    template <typename T>
    struct SelectMatchingUIntBySize<4, T>
    {
        typedef UInt32 Result;
    };
};

} // end namespace efd

// Include the platform specific inline functions
#include EE_PLATFORM_SPECIFIC_INCLUDE(efd,AtomicOperations,inl)

#endif // EE_ATOMICOPERATIONS_H

