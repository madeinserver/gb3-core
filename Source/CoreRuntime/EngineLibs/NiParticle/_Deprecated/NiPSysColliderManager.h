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
#ifndef NIPSYSCOLLIDERMANAGER_H
#define NIPSYSCOLLIDERMANAGER_H

#include "NiPSysModifier.h"
#include "NiPSysCollider.h"

/// @cond DEPRECATED_CLASS

/**
    This class is deprecated.

    It only exists to support loading old NIF files. It has been replaced by
    NiPSSimulatorCollidersStep.
*/
class NIPARTICLE_ENTRY NiPSysColliderManager : public NiPSysModifier
{
    NiDeclareRTTI;
    NiDeclareStream;

public:
    // *** begin Emergent internal use only ***
    NiPSysCollider* GetColliders();
    // *** end Emergent internal use only ***

protected:
    // For streaming only.
    NiPSysColliderManager();

    NiPSysColliderPtr m_spColliders;
};

NiSmartPointer(NiPSysColliderManager);

/// @endcond

#include "NiPSysColliderManager.inl"

#endif // #ifndef NIPSYSCOLLIDERMANAGER_H
#endif // #ifndef EE_REMOVE_BACK_COMPAT_STREAMING
