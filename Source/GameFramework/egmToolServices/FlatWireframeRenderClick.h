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
#ifndef EE_FLATWIREFRAMERENDERCLICK_H
#define EE_FLATWIREFRAMERENDERCLICK_H

#include "egmToolServicesLibType.h"

#include <NiViewRenderClick.h>
#include "ConditionalMaterialSwapProcessor.h"

namespace egmToolServices
{

class EE_EGMTOOLSERVICES_ENTRY FlatWireframeRenderClick : public NiViewRenderClick
{
public:
    FlatWireframeRenderClick();
    virtual ~FlatWireframeRenderClick();

protected:
    virtual void PerformRendering(unsigned int uiFrameID);

    NiColor* m_pkColor;
    NiColorA* m_pkBackupColor;

    NiMaterial* m_pkWireMaterial;
    ConditionalMaterialSwapProcessor* m_pkMaterialSwapProcessor;
};

NiSmartPointer(FlatWireframeRenderClick);

} // namespace

#endif // EE_FLATWIREFRAMERENDERCLICK_H
