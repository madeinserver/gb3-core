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

#ifndef EE_REMOVE_BACK_COMPAT_STREAMING

//--------------------------------------------------------------------------------------------------
inline NiTPointerList<NiScreenElementsPtr>& Ni2DRenderView::GetScreenElements()
{
    return m_kScreenElements;
}

//--------------------------------------------------------------------------------------------------
inline void Ni2DRenderView::AppendScreenElement(
    NiScreenElements* pkScreenElement)
{
    EE_ASSERT(pkScreenElement);
    m_kScreenElements.AddTail(pkScreenElement);
}

//--------------------------------------------------------------------------------------------------
inline void Ni2DRenderView::PrependScreenElement(
    NiScreenElements* pkScreenElement)
{
    EE_ASSERT(pkScreenElement);
    m_kScreenElements.AddHead(pkScreenElement);
}

//--------------------------------------------------------------------------------------------------
inline void Ni2DRenderView::RemoveScreenElement(
    NiScreenElements* pkScreenElement)
{
    EE_ASSERT(pkScreenElement);
    m_kScreenElements.Remove(pkScreenElement);
}

//--------------------------------------------------------------------------------------------------
inline void Ni2DRenderView::RemoveAllScreenElements()
{
    m_kScreenElements.RemoveAll();
}

//--------------------------------------------------------------------------------------------------
inline const NiTPointerList<NiScreenElementsPtr>&
    Ni2DRenderView::GetScreenElements() const
{
    return m_kScreenElements;
}

//--------------------------------------------------------------------------------------------------
#endif // #ifndef EE_REMOVE_BACK_COMPAT_STREAMING
