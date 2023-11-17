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
#ifndef NIDEBUGDEFINES_H
#define NIDEBUGDEFINES_H

#if !defined(_PS3)
#if defined(NIDEBUG)
    #pragma message ("Defined NIDEBUG")
#endif

#if defined(NIRELEASE)
    #pragma message ("Defined NIRELEASE")
#endif

#if defined(NISHIPPING)
    #pragma message ("Defined NISHIPPING")
#endif

#if defined(_DEBUG)
    #pragma message ("Defined _DEBUG")
#endif

#if defined(NDEBUG)
    #pragma message ("Defined NDEBUG")
#endif
#endif //_PS3

#endif //NIDEBUGDEFINES_H
