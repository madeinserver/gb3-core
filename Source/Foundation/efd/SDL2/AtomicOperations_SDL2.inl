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
    inline efd::SInt32 AtomicIncrement(efd::SInt32& value)
    {
        SDL_AtomicIncRef((SDL_atomic_t*)&value);
        return value;
    }
    //-------------------------------------------------------------------------------------------------
    inline efd::SInt32 AtomicDecrement(efd::SInt32& value)
    {
        SDL_AtomicDecRef((SDL_atomic_t*)&value);
        return value;
    }
    //-------------------------------------------------------------------------------------------------
    inline efd::SInt32 AtomicIncrement(volatile efd::SInt32& value)
    {
        SDL_AtomicIncRef((SDL_atomic_t*)&value);
        return value;
    }
    //-------------------------------------------------------------------------------------------------
    inline efd::SInt32 AtomicDecrement(volatile efd::SInt32& value)
    {
        SDL_AtomicDecRef((SDL_atomic_t*)&value);
        return value;
    }
    //-------------------------------------------------------------------------------------------------
    inline efd::UInt32 AtomicIncrement(efd::UInt32& value)
    {
        SDL_AtomicIncRef((SDL_atomic_t*)&value);
        return value;
    }
    //-------------------------------------------------------------------------------------------------
    inline efd::UInt32 AtomicDecrement(efd::UInt32& value)
    {
        EE_ASSERT(value > 0);
        SDL_AtomicDecRef((SDL_atomic_t*)&value);
        return value;
    }
    //-------------------------------------------------------------------------------------------------
    inline efd::UInt32 AtomicIncrement(volatile efd::UInt32& value)
    {
        SDL_AtomicIncRef((SDL_atomic_t*)&value);
        return value;
    }
    //-------------------------------------------------------------------------------------------------
    inline efd::UInt32 AtomicDecrement(volatile efd::UInt32& value)
    {
        EE_ASSERT(value > 0);
        SDL_AtomicDecRef((SDL_atomic_t*)&value);
        return value;
    }
    //-------------------------------------------------------------------------------------------------
    inline void* AtomicCompareAndSwap(
        void* volatile* ppDestination,
        void* pComparand,
        void* pExchange)
    {
#error a
        return InterlockedCompareExchangePointer(ppDestination, pExchange, pComparand);
    }
    //-------------------------------------------------------------------------------------------------
    inline efd::UInt32 AtomicCompareAndSwap(
        efd::UInt32 volatile* pDestination,
        efd::UInt32 comparand,
        efd::UInt32 exchange)
    {
#error a
        return InterlockedCompareExchange(
            reinterpret_cast<LONG volatile*>(pDestination),
            static_cast<LONG>(exchange),
            static_cast<LONG>(comparand));
    }


} // end namespace efd

