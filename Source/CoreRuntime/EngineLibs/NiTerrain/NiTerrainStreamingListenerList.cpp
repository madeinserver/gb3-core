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

#include "NiTerrainPCH.h"

#include "NiTerrainStreamingListenerList.h"

//--------------------------------------------------------------------------------------------------
NiTerrainStreamingListenerList::NiTerrainStreamingListenerList()
{
}

//--------------------------------------------------------------------------------------------------
NiTerrainStreamingListenerList::~NiTerrainStreamingListenerList()
{
    // For debugging
    EE_ASSERT(m_kMutex.GetCurrentLockCount() == 0);
}

//--------------------------------------------------------------------------------------------------
void NiTerrainStreamingListenerList::AddListener(NiTerrainStreamingManager::Listener* pkListener)
{
    m_kMutex.Lock();
    m_kListenerList.push_back(pkListener);
    m_kMutex.Unlock();
}

//--------------------------------------------------------------------------------------------------
void NiTerrainStreamingListenerList::RemoveListener(NiTerrainStreamingManager::Listener* pkListener)
{
    m_kMutex.Lock();
    m_kListenerList.remove(pkListener);
    m_kMutex.Unlock();
}

//--------------------------------------------------------------------------------------------------
void NiTerrainStreamingListenerList::RemoveAllListeners()
{
    m_kMutex.Lock();
    m_kListenerList.clear();
    m_kMutex.Unlock();
}

//--------------------------------------------------------------------------------------------------
void NiTerrainStreamingListenerList::IncRefCount()
{
    NiRefObject::IncRefCount();
}

//--------------------------------------------------------------------------------------------------
void NiTerrainStreamingListenerList::DecRefCount()
{
    NiRefObject::DecRefCount();
}

//--------------------------------------------------------------------------------------------------
void NiTerrainStreamingListenerList::ReportStartTask(TaskID kTaskID, SectorID kSectorID, 
    TaskType::VALUE eTaskType)
{
    m_kMutex.Lock();

    for (ListenerList::iterator kIter = m_kListenerList.begin(); kIter != m_kListenerList.end(); 
        kIter++)
    {
        (*kIter)->ReportStartTask(kTaskID, kSectorID, eTaskType);
    }

    m_kMutex.Unlock();
}

//--------------------------------------------------------------------------------------------------
void NiTerrainStreamingListenerList::ReportTaskStatus(TaskID kTaskID, TaskType::VALUE eTaskType, 
    Status::VALUE eStatus)
{
    m_kMutex.Lock();

    for (ListenerList::iterator kIter = m_kListenerList.begin(); kIter != m_kListenerList.end(); 
        kIter++)
    {
        (*kIter)->ReportTaskStatus(kTaskID, eTaskType, eStatus);
    }

    m_kMutex.Unlock();
}

//--------------------------------------------------------------------------------------------------
void NiTerrainStreamingListenerList::ReportFinishTask(TaskID kTaskID, TaskType::VALUE eTaskType, 
    efd::UInt32 uiErrorCode)
{
    m_kMutex.Lock();

    for (ListenerList::iterator kIter = m_kListenerList.begin(); kIter != m_kListenerList.end(); 
        kIter++)
    {
        (*kIter)->ReportFinishTask(kTaskID, eTaskType, uiErrorCode);
    }

    m_kMutex.Unlock();
}

//--------------------------------------------------------------------------------------------------
