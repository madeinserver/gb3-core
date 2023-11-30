// EMERGENT GAME TECHNOLOGIES PROPRIETARY INFORMATION
//
// This software is supplied under the terms of a license agreement or
// nondisclosure agreement with Emergent Game Technologies and may not 
// be copied or disclosed except in accordance with the terms of that 
// agreement.
//
//      Copyright (c) 2022-2023 Arves100/Made In Server Developers.
//      Copyright (c) 1996-2009 Emergent Game Technologies.
//      All Rights Reserved.
//
// Emergent Game Technologies, Calabasas, CA 91302
// http://www.emergent.net

namespace efd
{

//-------------------------------------------------------------------------------------------------
efd::UInt32 Socket::getError()
{
#ifdef EE_PLATFORM_WIN32
    return WSAGetLastError();
#else
    return errno;
#endif
}


//-------------------------------------------------------------------------------------------------
efd::utf8string Socket::getErrorMessage()
{
#ifdef EE_PLATFORM_WIN32
    DWORD dw = WSAGetLastError();
#else
    efd::UInt32 dw = (efd::UInt32)errno;
#endif
    return getErrorMessage(dw);
}


//-------------------------------------------------------------------------------------------------
efd::utf8string Socket::getErrorMessage(efd::UInt32 errorNum)
{
#ifdef EE_PLATFORM_WIN32
    LPVOID lpMsgBuf;
    DWORD dw = errorNum;

    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER |
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        dw,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR) &lpMsgBuf,
        0, NULL);

    efd::utf8string returnString = (char*)lpMsgBuf;
    LocalFree(lpMsgBuf);
#else
    efd::utf8string returnString = strerror((int)errorNum);
#endif

    return returnString;
}

//-------------------------------------------------------------------------------------------------
void Socket::setDebug(int debugToggle)
{
    if (SOCKET_ERROR == setsockopt(
        m_socketId,
        SOL_SOCKET,
        SO_DEBUG,
        (char *)&debugToggle,
        sizeof(debugToggle)))
    {
        EE_LOG(efd::kNetMessage,
            efd::ILogger::kERR1,
            ("TCPSocket Error: DEBUG option:%s", getErrorMessage().c_str()));
        EE_LOG_METRIC_COUNT_FMT(kSocket, ("INIT.ERROR.%u", m_qos));
        return;
    }
}

//-------------------------------------------------------------------------------------------------
int Socket::getDebug()
{
    int myOption;
    socklen_t myOptionLen = sizeof(myOption);

    if (getsockopt(m_socketId,SOL_SOCKET,SO_DEBUG,(char*)&myOption,&myOptionLen) == SOCKET_ERROR)
    {
        EE_LOG(efd::kNetMessage,
            efd::ILogger::kERR1,
            ("TCPSocket Error: get DEBUG option: %s", getErrorMessage().c_str()));
        EE_LOG_METRIC_COUNT_FMT(kSocket, ("INIT.ERROR.%u", m_qos));
        return SOCKET_ERROR;
    }

    return myOption;
}

}