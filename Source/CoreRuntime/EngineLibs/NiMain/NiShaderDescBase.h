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
#ifndef NISHADERDESCBASE_H
#define NISHADERDESCBASE_H

// These classes and functions are used for DLL-based shader libraries
#include "NiRefObject.h"
#include "NiSmartPointer.h"

namespace efd
{
    class BinaryStream;
}

// Description of a shader implementation
class NIMAIN_ENTRY NiShaderDescBase : public NiRefObject
{
public:
    NiShaderDescBase();
    virtual ~NiShaderDescBase();

    virtual const char* GetName() const;
    virtual const char* GetDescription() const;

    // *** begin Emergent internal use only ***
    virtual void SetName(const char* pcName);
    virtual void SetDescription(const char* pcDesc);
    // *** end Emergent internal use only ***

protected:
    char* m_pcName;
    char* m_pcDesc;
};

typedef efd::SmartPointer<NiShaderDescBase> NiShaderDescBasePtr;

#endif  //#ifndef NISHADERDESCBASE_H
