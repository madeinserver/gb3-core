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

//--------------------------------------------------------------------------------------------------
inline NiRenderedTexture::NiRenderedTexture()
{
    /* */
}

//--------------------------------------------------------------------------------------------------
inline unsigned int NiRenderedTexture::GetWidth() const
{
    if (m_spBuffer)
        return m_spBuffer->GetWidth();
    else
        return 0;
}

//--------------------------------------------------------------------------------------------------
inline unsigned int NiRenderedTexture::GetHeight() const
{
    if (m_spBuffer)
        return m_spBuffer->GetHeight();
    else
        return 0;
}

//--------------------------------------------------------------------------------------------------
