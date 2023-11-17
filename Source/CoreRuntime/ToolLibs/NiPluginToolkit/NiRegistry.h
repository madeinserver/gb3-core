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
#ifndef NIREGISTRY_H
#define NIREGISTRY_H

#if !defined(WIN32)
#error Win32-specific source code!
#endif  //#if !defined(WIN32)

// NiRegistry is intended to provide a simple interface to the Windows
// registry for storage of application information that is intended to
// be shared. Currently, the class is just intended for opening a single
// key, accessing it, then closing it. It will NOT handle looking at
// multiple keys at once.

#include <NiString.h>
#include "NiPluginToolkitLibType.h"

class NIPLUGINTOOLKIT_ENTRY NiRegistry : public NiMemObject
{
protected:
    NiRegistry();
public:
    ~NiRegistry();

    static NiRegistry* Create(const char* pcAppPath);

    bool SetAppPath(NiString strAppPath);
    bool OpenKey();
    bool CloseKey();

    bool QueryBool(const char* pcSubKey, bool& bValue);
    bool QueryDWORD(const char* pcSubKey, unsigned int& uiValue);
    bool QueryString(const char* pcSubKey, NiString& strValue);
    bool QueryGUID(const char* pcSubKey, GUID* pkGUID);

protected:
    NiString m_strAppPath;
    HKEY m_hTopLevelKey;
    HKEY m_hRootKey;
    HKEY m_hAppKey;
    HKEY m_hkey;
};

#endif  //#ifndef NIREGISTRY_H
