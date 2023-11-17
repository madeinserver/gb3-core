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

#include <NiDataStreamElement.h>
#include <NiToolDataStream.h>
#include "NiMeshProfileOperations.h"

//--------------------------------------------------------------------------------------------------
NiDataStreamRef NiMeshProfileOperations::Rename(const NiDataStreamRef& kSrcRef,
    NiFixedString& kSemantic, NiUInt8 uiIndex)
{
    if (!IsValidStreamRef(kSrcRef))
        return NiDataStreamRef();

    // Underlying data stream doesn't need to be copied for this operation
    NiDataStreamRef kNewStreamRef = kSrcRef;

    ApplyRenameInPlace(kNewStreamRef, kSemantic, uiIndex);
    return kNewStreamRef;
}

//--------------------------------------------------------------------------------------------------
void NiMeshProfileOperations::ApplyRenameInPlace(NiDataStreamRef& kStreamRef,
    NiFixedString& kGenericSemantic, NiUInt8 uiGenericIndex)
{
    EE_ASSERT(kStreamRef.IsValid());
    EE_ASSERT(kStreamRef.GetElementDescCount() == 1);
    kStreamRef.BindSemanticToElementDescAt(0, kGenericSemantic,
        uiGenericIndex);
}

//--------------------------------------------------------------------------------------------------
