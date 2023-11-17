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

#ifndef EE_REMOVE_BACK_COMPAT_STREAMING

#include <NiLog.h>

//--------------------------------------------------------------------------------------------------
inline void NiPSConverter::ReportSequenceConversionError(
    const char* pcSequenceName,
    const char* pcCtlrType)
{
    EE_UNUSED_ARG(pcSequenceName);
    EE_UNUSED_ARG(pcCtlrType);
    EE_ASSERT(pcCtlrType);
    NILOG("NiSequenceData Conversion Error: %s\n\tUnable to properly "
        "convert a reference to a controller of type %s.\n\tThe interpolator "
        "may not get property attached to that controller.\n\tRe-export the "
        "sequence to fix this problem.\n", pcSequenceName, pcCtlrType);
}

//--------------------------------------------------------------------------------------------------
inline NiAVObject* NiPSConverter::LookupObject(
    NiAVObject* pkOriginalObject,
    const NiTPointerMap<const NiAVObject*, NiAVObject*>& kConversionMap)
{
    NiAVObject* pkNewObject;
    if (kConversionMap.GetAt(pkOriginalObject, pkNewObject))
    {
        return pkNewObject;
    }

    return pkOriginalObject;
}

//--------------------------------------------------------------------------------------------------
#endif // #ifndef EE_REMOVE_BACK_COMPAT_STREAMING
