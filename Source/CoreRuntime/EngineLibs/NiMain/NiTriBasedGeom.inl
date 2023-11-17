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
//------------------------------------------------------------------------------------------------
//  NiTriBasedGeom inline functions

#ifndef EE_REMOVE_BACK_COMPAT_STREAMING

//------------------------------------------------------------------------------------------------
inline unsigned short NiTriBasedGeom::GetTriangleCount() const
{
    NiTriBasedGeomData* pkData = ((NiTriBasedGeomData*)(m_spModelData.data()));
    return pkData->GetTriangleCount();
}

//------------------------------------------------------------------------------------------------
inline void NiTriBasedGeom::SetActiveTriangleCount(unsigned short usActive)
{
    ((NiTriBasedGeomData*) GetModelData())->
        SetActiveTriangleCount(usActive);
}

//------------------------------------------------------------------------------------------------
inline unsigned short NiTriBasedGeom::GetActiveTriangleCount() const
{
    return ((NiTriBasedGeomData*) GetModelData())->
        GetActiveTriangleCount();
}

//------------------------------------------------------------------------------------------------
inline void NiTriBasedGeom::GetTriangleIndices(unsigned short i,
    unsigned short& i0, unsigned short& i1, unsigned short& i2) const
{
    ((NiTriBasedGeomData*) GetModelData())->
        GetTriangleIndices(i, i0, i1, i2);
}

//------------------------------------------------------------------------------------------------
#endif // #ifndef EE_REMOVE_BACK_COMPAT_STREAMING
