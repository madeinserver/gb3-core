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
inline NiAccumulatorProcessor::NiAccumulatorProcessor(
    NiAccumulator* pkAccumulator, bool bFinishAccumulating) :
    m_spAccumulator(pkAccumulator), m_bFinishAccumulating(bFinishAccumulating)
{
    m_spDummyCamera = NiNew NiCamera;
}

//--------------------------------------------------------------------------------------------------
inline void NiAccumulatorProcessor::SetAccumulator(
    NiAccumulator* pkAccumulator)
{
    m_spAccumulator = pkAccumulator;
}

//--------------------------------------------------------------------------------------------------
inline NiAccumulator* NiAccumulatorProcessor::GetAccumulator() const
{
    return m_spAccumulator;
}

//--------------------------------------------------------------------------------------------------
inline void NiAccumulatorProcessor::SetFinishAccumulating(
    bool bFinishAccumulating)
{
    m_bFinishAccumulating = bFinishAccumulating;
}

//--------------------------------------------------------------------------------------------------
inline bool NiAccumulatorProcessor::GetFinishAccumulating() const
{
    return m_bFinishAccumulating;
}

//--------------------------------------------------------------------------------------------------
