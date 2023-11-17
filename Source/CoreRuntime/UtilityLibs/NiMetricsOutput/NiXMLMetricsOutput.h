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
#ifndef NIXMLMETRICSOUTPUT_H
#define NIXMLMETRICSOUTPUT_H

// This class provides default XML output for the metrics system.  It outputs
// to a log file at the end of a period of frames (chosen at construction
// time).  It calculates every metric.

#include "NiMetricsOutputLibType.h"
#include "NiCalculatingOutput.h"
#include <NiTArray.h>
#include <efd/File.h>

class NIMETRICSOUTPUT_ENTRY NiXMLMetricsOutput : public NiCalculatingOutput
{
public:
    NiXMLMetricsOutput(unsigned int uiFramePeriod, const char* pcNiFileName,
        unsigned int uiNiFileBufferSize = 32768,
        unsigned int uiHashSize = 127);
    virtual ~NiXMLMetricsOutput();

    bool GetFileValid() const;

    virtual void AddValue(const char* pcName, float fValue);
    inline virtual void StartTimer(NiMetricsTimer&) {};
    virtual void EndTimer(NiMetricsTimer& kTimer);
    virtual void AddImmediateEvent(const char* pcName, float fValue);
    virtual void Update();

protected:

    void WriteHeader();
    void WriteBuffer();
    void WriteFooter();

    efd::File* m_pkFile;
};

NiSmartPointer(NiXMLMetricsOutput);


#endif //#ifndef NIXMLMETRICSOUTPUT_H
