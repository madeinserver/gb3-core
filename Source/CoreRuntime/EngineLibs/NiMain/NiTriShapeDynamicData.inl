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
inline void NiTriShapeDynamicData::SetActiveVertexCount(
    unsigned short usActiveVertices)
{
    m_usActiveVertices = usActiveVertices;
}

//--------------------------------------------------------------------------------------------------
inline void NiTriShapeDynamicData::SetActiveTriangleCount(
    unsigned short usActiveTriangles)
{
    m_usActiveTriangles = usActiveTriangles;
}

//--------------------------------------------------------------------------------------------------
inline void NiTriShapeDynamicData::SetTriangleCount(unsigned short usTriangles)
{
    m_usTriangles = usTriangles;
}

//--------------------------------------------------------------------------------------------------
inline void NiTriShapeDynamicData::SetVertexCount(unsigned short usVertices)
{
    m_usVertices = usVertices;
}

//--------------------------------------------------------------------------------------------------
#endif // #ifndef EE_REMOVE_BACK_COMPAT_STREAMING
