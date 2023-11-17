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

#include "egmToolServicesPCH.h"

#include <efd/ServiceManager.h>
#include <ecrInput/KeyboardMessages.h>
#include <ecrInput/MouseMessages.h>

#include "ToolServicesMessages.h"
#include "InteractionService.h"

using namespace efd;
using namespace egf;
using namespace ecrInput;
using namespace egmToolServices;

EE_IMPLEMENT_CONCRETE_CLASS_INFO(InteractionService);

EE_HANDLER(InteractionService, HandleMouseMoveMessage, MouseMoveMessage);
EE_HANDLER(InteractionService, HandleMouseDownMessage, MouseDownMessage);
EE_HANDLER(InteractionService, HandleMouseUpMessage,   MouseUpMessage);

//------------------------------------------------------------------------------------------------
InteractionService::InteractionService()
{
    // If this default priority is changed, also update the service quick reference documentation
    m_defaultPriority = 2095;
}

//------------------------------------------------------------------------------------------------
InteractionService::~InteractionService()
{
}

//------------------------------------------------------------------------------------------------
const char* InteractionService::GetDisplayName() const
{
    return "InteractionService";
}

//------------------------------------------------------------------------------------------------
SyncResult InteractionService::OnPreInit(efd::IDependencyRegistrar* pDependencyRegistrar)
{
    pDependencyRegistrar->AddDependency<efd::MessageService>();

    m_pMessageService = m_pServiceManager->GetSystemServiceAs<efd::MessageService>();
    EE_ASSERT(m_pMessageService);

    m_pMessageService->Subscribe(this, kCAT_LocalMessage);

    return SyncResult_Success;
}

//------------------------------------------------------------------------------------------------
AsyncResult InteractionService::OnShutdown()
{
    m_pMessageService = m_pServiceManager->GetSystemServiceAs<efd::MessageService>();

    if (m_pMessageService)
    {
        m_pMessageService->Unsubscribe(this, kCAT_LocalMessage);
    }

    m_delegates.clear();

    return AsyncResult_Complete;
}

//-----------------------------------------------------------------------------------------------
void InteractionService::AddInteractionDelegate(IInteractionDelegate* pInteractionDelegate)
{
    for (InteractionDelegates::iterator it = m_delegates.begin(); it != m_delegates.end(); ++it)
    {
        IInteractionDelegate* pDelegate = *it;

        if (pInteractionDelegate->GetInteractionPriority() > pDelegate->GetInteractionPriority())
        {            
            m_delegates.insert(it, pInteractionDelegate);
            return;
        }
    }

    m_delegates.push_back(pInteractionDelegate);
}

//-----------------------------------------------------------------------------------------------
void InteractionService::RemoveInteractionDelegate(IInteractionDelegate* pInteractionDelegate)
{
    InteractionDelegates::iterator it = m_delegates.find(pInteractionDelegate);
    if (it != m_delegates.end())
        m_delegates.erase(it);
}

//-----------------------------------------------------------------------------------------------
void InteractionService::HandleMouseMoveMessage(const MouseMoveMessage* pMsg,
                                                efd::Category targetChannel)
{
    EE_ASSERT(m_pServiceManager);

    int dScroll = pMsg->GetZDelta();

    if (dScroll == 0)
    {
        IInteractionDelegate* pIntercepter = NULL;

        for (SInt32 i = m_delegates.size() - 1; i >= 0; i--)
        {
            IInteractionDelegate* pInteractable = m_delegates[i];

            if (pInteractable->OnPreMouseMove(pMsg->GetX(), pMsg->GetY(), pMsg->GetXDelta(),
                pMsg->GetYDelta()))
            {
                pIntercepter = pInteractable;
                break;
            }
        }

        for (SInt32 i = 0; i < (SInt32)m_delegates.size(); i++)
        {
            IInteractionDelegate* pInteractable = m_delegates[i];

            if (pIntercepter != NULL && pIntercepter != pInteractable)
                continue;

            if (pInteractable->OnMouseMove(false, pMsg->GetX(), pMsg->GetY(), pMsg->GetXDelta(),
                pMsg->GetYDelta()))
            {
                return;
            }
        }
    }
    else
    {
        IInteractionDelegate* pIntercepter = NULL;

        for (SInt32 i = m_delegates.size() - 1; i >= 0; i--)
        {
            IInteractionDelegate* pInteractable = m_delegates[i];

            if (pInteractable->OnPreMouseScroll(pMsg->GetX(), pMsg->GetX(), dScroll))
            {
                pIntercepter = pInteractable;
                break;
            }
        }

        for (SInt32 i = 0; i < (SInt32)m_delegates.size(); i++)
        {
            IInteractionDelegate* pInteractable = m_delegates[i];

            if (pIntercepter != NULL && pIntercepter != pInteractable)
                continue;

            if (pInteractable->OnMouseScroll(false, pMsg->GetX(), pMsg->GetX(), dScroll))
                return;
        }
    }
}
//--------------------------------------------------------------------------------------------------
void InteractionService::HandleMouseDownMessage(const MouseDownMessage* pMsg,
                                                efd::Category targetChannel)
{
    EE_ASSERT(m_pServiceManager);

    IInteractionDelegate* pIntercepter = NULL;

    for (SInt32 i = m_delegates.size() - 1; i >= 0; i--)
    {
        IInteractionDelegate* pInteractable = m_delegates[i];

        if (pInteractable->OnPreMouseDown(pMsg->GetButton(), pMsg->GetX(), pMsg->GetY()))
        {
            pIntercepter = pInteractable;
            break;
        }
    }

    for (SInt32 i = 0; i < (SInt32)m_delegates.size(); i++)
    {
        IInteractionDelegate* pInteractable = m_delegates[i];

        if (pIntercepter != NULL && pIntercepter != pInteractable)
            continue;

        if (pInteractable->OnMouseDown(false, pMsg->GetButton(), pMsg->GetX(), pMsg->GetY()))
            return;
    }
}
//--------------------------------------------------------------------------------------------------
void InteractionService::HandleMouseUpMessage(const MouseUpMessage* pMsg,
                                              efd::Category targetChannel)
{
    EE_ASSERT(m_pServiceManager);

    IInteractionDelegate* pIntercepter = NULL;

    for (SInt32 i = m_delegates.size() - 1; i >= 0; i--)
    {
        IInteractionDelegate* pInteractable = m_delegates[i];

        if (pInteractable->OnPreMouseUp(pMsg->GetButton(), pMsg->GetX(), pMsg->GetY()))
        {
            pIntercepter = pInteractable;
            break;
        }
    }

    for (SInt32 i = 0; i < (SInt32)m_delegates.size(); i++)
    {
        IInteractionDelegate* pInteractable = m_delegates[i];

        if (pIntercepter != NULL && pIntercepter != pInteractable)
            continue;

        if (pInteractable->OnMouseUp(false, pMsg->GetButton(), pMsg->GetX(), pMsg->GetY()))
            return;
    }
}
//--------------------------------------------------------------------------------------------------
