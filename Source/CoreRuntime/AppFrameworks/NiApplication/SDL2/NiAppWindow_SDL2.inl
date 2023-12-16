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

#include <NiMemoryDefines.h>

//--------------------------------------------------------------------------------------------------
inline void NiAppWindow::SetWindowReference(NiWindowRef pWnd)
{
    m_pWnd = pWnd;
}

//--------------------------------------------------------------------------------------------------
inline NiWindowRef NiAppWindow::GetWindowReference()
{
    return m_pWnd;
}

//--------------------------------------------------------------------------------------------------
inline const char* NiAppWindow::GetWindowClassName()
{
    return ms_acWindowClassName;
}

//--------------------------------------------------------------------------------------------------
inline efd::UInt32 NiAppWindow::GetWindowFlags()
{
    return m_unWindowFlags;
}

//--------------------------------------------------------------------------------------------------
inline void NiAppWindow::SetWindowFlags(efd::UInt32 ulWindowFlags)
{
    m_unWindowFlags = ulWindowFlags;
}

//--------------------------------------------------------------------------------------------------
inline const char* NiAppWindow::GetWindowCaption() const
{
    return m_pcWindowCaption;
}

//--------------------------------------------------------------------------------------------------
inline void NiAppWindow::SetWidth(unsigned int uiWidth)
{
    m_uiWidth = uiWidth;
}

//--------------------------------------------------------------------------------------------------
inline unsigned int NiAppWindow::GetWidth() const
{
    return m_uiWidth;
}

//--------------------------------------------------------------------------------------------------
inline void NiAppWindow::SetHeight(unsigned int uiHeight)
{
    m_uiHeight = uiHeight;
}

//--------------------------------------------------------------------------------------------------
inline unsigned int NiAppWindow::GetHeight() const
{
    return m_uiHeight;
}

//--------------------------------------------------------------------------------------------------
inline void NiAppWindow::SetWindowOnTop(bool bWindowOnTop)
{
    m_bOnTop = bWindowOnTop;
}

//--------------------------------------------------------------------------------------------------
inline bool NiAppWindow::GetWindowOnTop() const
{
    return m_bOnTop;
}
