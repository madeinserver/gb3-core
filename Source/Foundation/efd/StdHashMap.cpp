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

// Precompiled Header
#include "efdPCH.h"

#include <efd/RTLib.h>
#include <efd/StdHashMap.h>

#ifdef EE_USE_NATIVE_STL

// From STLports/_hash_map.h
inline size_t __stl_hash_string(const char* __s) {
        unsigned long __h = 0;
    for (; *__s; ++__s)
        __h = 5 * __h + *__s;

    return size_t(__h);
}

#define _STLP_PRIV
#endif

/// Define a custom hashing function so we can use a hash_map with utf8string
size_t _STLP_STD_NAME::hash<efd::utf8string>::operator()(efd::utf8string s) const
{
    return _STLP_PRIV __stl_hash_string(s.c_str());
}
