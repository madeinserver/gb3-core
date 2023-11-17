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
inline NiLinPosKey::NiLinPosKey()
{
}

//--------------------------------------------------------------------------------------------------
inline NiLinPosKey::NiLinPosKey(float fTime, const NiPoint3& pos)
    :
    NiPosKey(fTime,pos)
{
}

//--------------------------------------------------------------------------------------------------
inline bool NiLinPosKey::Equal(const NiAnimationKey& key0,
    const NiAnimationKey& key1)
{
    return NiPosKey::Equal(key0,key1);
}

//--------------------------------------------------------------------------------------------------
inline void NiLinPosKey::FillDerivedVals(NiAnimationKey*, unsigned int,
    unsigned char)
{
    // no derived values to fill in
}

//--------------------------------------------------------------------------------------------------
