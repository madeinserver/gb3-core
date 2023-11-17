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

#pragma once
#ifndef NIBOXSPHEREINTERSECTOR_H
#define NIBOXSPHEREINTERSECTOR_H

#include "NiIntersector.h"
#include "NiBoxBV.h"
#include "NiSphereBV.h"

class NICOLLISION_ENTRY NiBoxSphereIntersector : public NiIntersector
{
public:
    NiBoxSphereIntersector(const NiBoxBV& kBox, const NiSphereBV& kSphere,
        float fMaxTime, float fDerivativeTimeStep = 0.001f,
        float fPseudodistanceThreshold = 0.0001f, int iMaxIterations = 32);

private:
    virtual float Pseudodistance(const NiPoint3& kV0, const NiPoint3& kV1,
        float fTime) const;

    virtual void ComputeContactInformation(const NiPoint3& kV0,
        const NiPoint3& kV1);

    const NiBoxBV& m_kBox;
    const NiSphereBV& m_kSphere;
    float m_fInvRadiusSqr;
    mutable float m_afParameter[3];

private:
    NiBoxSphereIntersector & operator=(const NiBoxSphereIntersector &);
};

#endif
