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
#ifndef EE_REMOVE_BACK_COMPAT_STREAMING
#ifndef NIPSYSRESETONLOOPCTLR_H
#define NIPSYSRESETONLOOPCTLR_H

#include <NiParticleLibType.h>
#include <NiTimeController.h>

/// @cond DEPRECATED_CLASS

/**
    This class is deprecated.

    It only exists to support loading old NIF files. It has been replaced by
    NiPSResetOnLoopCtlr.
*/
class NIPARTICLE_ENTRY NiPSysResetOnLoopCtlr : public NiTimeController
{
    NiDeclareRTTI;
    NiDeclareStream;

public:
    // *** begin Emergent internal use only ***
    virtual void Update(float fTime);
    // *** end Emergent internal use only ***

protected:
    // For streaming only.
    NiPSysResetOnLoopCtlr();

    virtual bool TargetIsRequiredType() const;
};

/// @endcond

#endif // #ifndef NIPSYSRESETONLOOPCTLR_H
#endif // #ifndef EE_REMOVE_BACK_COMPAT_STREAMING
