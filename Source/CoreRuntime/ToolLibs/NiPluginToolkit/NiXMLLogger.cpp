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

#include "NiXMLLogger.h"

//--------------------------------------------------------------------------------------------------
NiXMLLogger::NiXMLLogger(): m_pFileHandle(NULL), m_uiTabDepth(0)
{
}
//--------------------------------------------------------------------------------------------------
NiXMLLogger::~NiXMLLogger()
{
    if (m_pFileHandle)
    {
        CloseLog();
    }
}
//--------------------------------------------------------------------------------------------------
bool NiXMLLogger::CreateLog(const NiString& kFilename)
{
    // we only handle one log at a time, so close any existing logs
    if (m_pFileHandle)
    {
        EE_ASSERT(false && !"Attempted to create an XML log with same logger instance when one "
            "is already open.");
        CloseLog();
    }

    // open our target log file for writing
    if (fopen_s(&m_pFileHandle, (const char*)kFilename, "wt") != 0)
    {
        EE_ASSERT(false && !"NiXMLLogger failed to open output log.");
        return false;
    }

    fputs("<?xml version=\"1.0\"?>\n", m_pFileHandle);
    StartElement("LogFile");
    return true;
}
//--------------------------------------------------------------------------------------------------
bool NiXMLLogger::CloseLog()
{
    // close the outer-most element
    EndElement("LogFile");

    // make sure all elements are closed
    EE_ASSERT((m_uiTabDepth == 0) && "XML log closed without closing all elements first. This will "
        "result in invalid XML.");

    // close the file, clear out member data, and return success
    bool succeeded = (fclose(m_pFileHandle) == 0);
    m_pFileHandle = NULL;
    m_uiTabDepth = 0;
    return succeeded;
}
//--------------------------------------------------------------------------------------------------
bool NiXMLLogger::LogElement(const NiString& kElementName,
    const NiString& kData)
{
    if (kElementName.IsEmpty() || kData.IsEmpty() || !m_pFileHandle)
        return false;

    NiString kElementString = "";
    kElementString += "<" + kElementName + ">\n" + kData + "</" + kElementName + ">\n";
    AddTabs(kElementString);
    return (fputs((const char*)kElementString, m_pFileHandle) >= 0);
}
//--------------------------------------------------------------------------------------------------
bool NiXMLLogger::LogData(const NiString& kData)
{
    if (kData.IsEmpty() || !m_pFileHandle)
        return false;

    NiString kTempData = kData;
    AddTabs(kTempData, false);
    return (fputs((const char*)kTempData, m_pFileHandle) >= 0);
}
//--------------------------------------------------------------------------------------------------
bool NiXMLLogger::StartElement(const NiString& kElementName)
{
    if (kElementName.IsEmpty() || !m_pFileHandle)
        return false;

    NiString kTempElement = "";
    kTempElement += "<" + kElementName + ">\n";
    AddTabs(kTempElement);
    m_uiTabDepth++;
    return (fputs((const char*)kTempElement, m_pFileHandle) >= 0);
}
//--------------------------------------------------------------------------------------------------
bool NiXMLLogger::EndElement(const NiString& kElementName)
{
    if (kElementName.IsEmpty() || !m_pFileHandle)
        return false;

    m_uiTabDepth--;
    NiString kTempElement = "";
    kTempElement += "</" + kElementName + ">\n";
    AddTabs(kTempElement);
    return (fputs((const char*)kTempElement, m_pFileHandle) >= 0);
}
//--------------------------------------------------------------------------------------------------
void NiXMLLogger::AddTabs(NiString& kString, bool bPrepend)
{
    NiString kTabString;
    for (NiUInt32 ui = 0; ui < m_uiTabDepth; ui++)
        kTabString += "\t";

    // we will store a copy of the incoming string that contains our inserted tabs
    NiString kTempString = kString;
    // record how many characters we have added so we know where to insert new ones
    NiUInt32 iTempOffset = 0;

    if (bPrepend)
    {
        kTempString = kTabString + kTempString;
        iTempOffset += kTabString.Length();
    }

    NiUInt32 uiStart = kString.Find('\n');
    while (uiStart != NiString::INVALID_INDEX)
    {
        // determine what to do next depending of the character following the carriage return
        if (kString.GetAt(uiStart + 1) == '<')
        {
            // if it preceeds our closing element, don't add an extra tab
            kTempString.Insert((const char*)kTabString, uiStart + iTempOffset + 1);
            iTempOffset += kTabString.Length();
        }
        else if (kString.GetAt(uiStart + 1) != 0)
        {
            // if it's a trailing carriage return, ignore it
            // if it's anything else, add an extra tab
            kTempString.Insert((const char*)kTabString, uiStart + iTempOffset + 1);
            iTempOffset += kTabString.Length();
            kTempString.Insert("\t", uiStart + iTempOffset + 1);
            iTempOffset++;
        }
        uiStart = kString.Find('\n', uiStart + 1);
    }

    // finally, assign our new string back to the passed in one
    kString = kTempString;
}
//--------------------------------------------------------------------------------------------------
