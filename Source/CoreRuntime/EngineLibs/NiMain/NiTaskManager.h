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
#ifndef NITASKMANAGER_H
#define NITASKMANAGER_H

#include "NiObject.h"
#include "NiTQueue.h"

class NiTask;

class NIMAIN_ENTRY NiTaskManager : public NiObject
{
    NiDeclareRTTI;

public:
    enum TaskPriority
    {
        LOW = 0,
        MEDIUM,
        HIGH,
        IMMEDIATE,
        NUM_PRIORITIES
    };

    enum ManagerStatus
    {
        STOPPED,
        STOPPING,
        RUNNING
    };

    NiTaskManager();
    virtual ~NiTaskManager();

    virtual bool AddTask(NiTask* pkTask, TaskPriority ePriority = MEDIUM);

    virtual ManagerStatus DoTasks();
    virtual bool DoSingleTask(TaskPriority ePriority);
    virtual bool Clear();
    virtual ManagerStatus Stop();

    ManagerStatus GetStatus();

    inline int GetTotalNumTasks();
    inline int GetNumTasks(TaskPriority ePriority);

protected:
    NiTPrimitiveQueue<NiTask*> m_akTaskQueues[NUM_PRIORITIES];
    efd::CriticalSection m_kManagerLock;
    ManagerStatus m_eStatus;
};

#include "NiTaskManager.inl"

#endif // NITASKMANAGER_H
