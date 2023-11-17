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
#ifndef NIPSYSMODIFIERFLOATCTLR_H
#define NIPSYSMODIFIERFLOATCTLR_H

#include "NiPSysModifierCtlr.h"

/// @cond DEPRECATED_CLASS

/**
    This class is deprecated.

    It only exists to support loading old NIF files. It has been replaced by
    NiPSEmitterFloatCtlr and NiPSForceFloatCtlr.
*/
class NIPARTICLE_ENTRY NiPSysModifierFloatCtlr : public NiPSysModifierCtlr
{
    NiDeclareRTTI;
    NiDeclareAbstractStream;

public:
    // *** begin Emergent internal use only ***
    virtual void Update(float fTime);
    virtual NiEvaluator* CreatePoseEvaluator(unsigned short usIndex = 0);
    virtual NiInterpolator* CreatePoseInterpolator(unsigned short usIndex = 0);
    virtual void SynchronizePoseInterpolator(NiInterpolator* pkInterp,
        unsigned short usIndex = 0);
    virtual NiBlendInterpolator* CreateBlendInterpolator(
        unsigned short usIndex = 0, bool bManagerControlled = false,
        float fWeightThreshold = 0.0f, unsigned char ucArraySize = 2) const;
    // *** end Emergent internal use only ***

protected:
    // For streaming only.
    NiPSysModifierFloatCtlr();

    // Virtual function overrides from base classes.
    virtual bool InterpolatorIsCorrectType(NiInterpolator* pkInterpolator,
        unsigned short usIndex) const;
};

NiSmartPointer(NiPSysModifierFloatCtlr);

/// @endcond

#endif // #ifndef NIPSYSMODIFIERFLOATCTLR_H
#endif // #ifndef EE_REMOVE_BACK_COMPAT_STREAMING
