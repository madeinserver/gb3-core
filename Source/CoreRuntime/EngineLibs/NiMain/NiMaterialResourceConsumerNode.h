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
#ifndef NIMATERIALRESOURCECONSUMERNODE_H
#define NIMATERIALRESOURCECONSUMERNODE_H

#include "NiMaterialNode.h"

class NIMAIN_ENTRY NiMaterialResourceConsumerNode : public NiMaterialNode
{
    NiDeclareRTTI;
public:
    NiMaterialResourceConsumerNode(const NiFixedString& kName,
       const NiFixedString& kConsumerType);
    const NiFixedString& GetConsumerType();
protected:
    NiFixedString m_kConsumerType;
};

typedef efd::SmartPointer<NiMaterialResourceConsumerNode>
    NiMaterialResourceConsumerNodePtr;

#endif  //#ifndef NIMATERIALRESOURCECONSUMERNODE_H
