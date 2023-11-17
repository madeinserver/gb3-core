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
#ifndef NIPSBOXEMITTER_H
#define NIPSBOXEMITTER_H

#include "NiPSVolumeEmitter.h"

/**
    A particle emitter that emits particles from a rectangular volume.

    Particles are emitted from a rectangular box that is centered at the
    world-space translation of the emitter object and with a specified width,
    height, and depth.

    The actual computation work for setting the initial position and velocity
    of new particles is done in the overridden protected function
    ComputeVolumeInitialPositionAndVelocity.
*/
class NIPARTICLE_ENTRY NiPSBoxEmitter : public NiPSVolumeEmitter
{
    /// @cond EMERGENT_INTERNAL

    NiDeclareRTTI;
    NiDeclareClone(NiPSBoxEmitter);
    NiDeclareStream;
    NiDeclareViewerStrings;

    /// @endcond

public:
    /// @name Construction and Destruction
    //@{
    /**
        Main constructor.

        @param kName The name of the emitter.
        @param fEmitterWidth The width (along the emitter object's local-space
            x-axis) of the emitter box.
        @param fEmitterHeight The height (along the emitter object's
            local-space y-axis) of the emitter box.
        @param fEmitterDepth The depth (along the emitter object's local-space
            z-axis) of the emitter box.
        @param pkEmitterObj An object in the scene graph whose world-space
            transformation will be used to represent the position and
            orientation of the emitter.
        @param fSpeed The speed assigned to new particles.
        @param fSpeedVar The speed variation for new particles. The resulting
            speed values assigned to new particles will be evenly
            distributed over the range [fSpeed - fSpeedVar/2,
            fSpeed + fSpeedVar/2].
        @param fSpeedFlipRatio The proportion of particles that have their
            speed negated from the value defined by other parameters. The
            value must be in the range [0.0, 1.0].
        @param fDeclination The declination angle in radians from the positive
            z-axis for the velocity vector of newly created particles. The
            expected range of declination is from 0.0, which will set the
            velocity vector to [0,0,1], to NI_PI, which will set the velocity
            vector to [0,0,-1].
        @param fDeclinationVar The declination variation in radians for new
            particles. The resulting declination values assigned to new
            particles will be evenly distributed over the range
            [fDeclination - fDeclinationVar, fDeclination + fDeclinationVar].
        @param fPlanarAngle The planar angle in radians about the z-axis from
            the positive x-axis that will serve as the plane in which the
            declination will occur. The expected range of the planar angle is
            [0, 2*NI_PI).
        @param fPlanarAngleVar The planar angle variation in radians for new
            particles. The resulting planar angle values assigned to new
            particles will be evenly distributed over the range
            [fPlanarAngle - fPlanarAngleVar, fPlanarAngle + fPlanarAngleVar].
        @param fSize The radius assigned to new particles.
        @param fSizeVar The radius variation for new particles. The resulting
            radius values assigned to new particles will be evenly distributed
            over the range [fSize - fSizeVar, fSize + fSizeVar]. If
            fSizeVar is larger than fSize, it is possible that the radius
            will be set to a negative value. In this case, the particle will
            be flipped, and the bounding volume may be computed incorrectly.
            Thus, it is recommended that fSizeVar be less than or equal to
            fSize.
        @param fLifeSpan The life span in seconds assigned to new particles.
        @param fLifeSpanVar The life span variation for new particles. The
            resulting life span values assigned to new particles will be
            evenly distributed over the range [fLifeSpan - fLifeSpanVar/2,
            fLifeSpan + fLifeSpanVar/2].
        @param fRotAngle The rotation angle in radians assigned to new
            particles.
        @param fRotAngleVar The rotation angle variation in radians for new
            particles. The resulting rotation angle values assigned to new
            particles will be evenly distributed over the range
            [fRotAngle - fRotAngleVar, fRotAngle + fRotAngleVar].
        @param fRotSpeed The rotation speed in radians per second assigned to
            new particles.
        @param fRotSpeedVar The rotation speed variation in radians per second
            for new particles. The resulting rotation speed values assigned to
            new particles will be evenly distributed over the range
            [fRotSpeed - fRotSpeedVar, fRotSpeed + fRotSpeedVar].
        @param bRandomRotSpeedSign Whether or not the rotation speed sign
            should be randomly flipped when being assigned to new particles.
        @param kRotAxis The rotation axis assigned to new particles. This
            value is only used if bRandomRotAxis is false.
        @param bRandomRotAxis Whether or not a random rotation axis will be
            assigned to new particles.
    */
    NiPSBoxEmitter(
        const NiFixedString& kName,
        float fEmitterWidth = 0.0f,
        float fEmitterHeight = 0.0f,
        float fEmitterDepth = 0.0f,
        NiAVObject* pkEmitterObj = NULL,
        float fSpeed = 1.0f,
        float fSpeedVar = 0.0f,
        float fSpeedFlipRatio = 0.0f,
        float fDeclination = 0.0f,
        float fDeclinationVar = 0.0f,
        float fPlanarAngle = 0.0f,
        float fPlanarAngleVar = 0.0f,
        float fSize = 1.0f,
        float fSizeVar = 0.0f,
        float fLifeSpan = 1.0f,
        float fLifeSpanVar = 0.0f,
        float fRotAngle = 0.0f,
        float fRotAngleVar = 0.0f,
        float fRotSpeed = 0.0f,
        float fRotSpeedVar = 0.0f,
        bool bRandomRotSpeedSign = false,
        const NiPoint3& kRotAxis = NiPoint3::UNIT_X,
        bool bRandomRotAxis = true);
    //@}

    //@{
    /// Accesses the width (along the emitter object's local-space x-axis) of
    /// the emitter box.
    inline float GetEmitterWidth() const;
    inline void SetEmitterWidth(float fEmitterWidth);
    //@}

    //@{
    /// Accesses the height (along the emitter object's local-space y-axis) of
    /// the emitter box.
    inline float GetEmitterHeight() const;
    inline void SetEmitterHeight(float fEmitterHeight);
    //@}

    //@{
    /// Accesses the depth (along the emitter object's local-space z-axis) of
    /// the emitter box.
    inline float GetEmitterDepth() const;
    inline void SetEmitterDepth(float fEmitterDepth);
    //@}

protected:
    /// @name Construction and Destruction
    //@{
    /// Protected default constructor for cloning and streaming only.
    NiPSBoxEmitter();
    //@}

    /// @name Base Class Overrides
    //@{
    virtual bool ComputeVolumeInitialPositionAndVelocity(
        NiQuatTransform& kEmitterToPSys,
        NiPoint3& kPosition,
        NiPoint3& kVelocity);
    //@}

    /// The width (along the emitter object's local-space x-axis) of the
    /// emitter box.
    float m_fEmitterWidth;

    /// The height (along the emitter object's local-space y-axis) of the
    /// emitter box.
    float m_fEmitterHeight;

    /// The depth (along the emitter object's local-space z-axis) of the
    /// emitter box.
    float m_fEmitterDepth;
};

NiSmartPointer(NiPSBoxEmitter);

#include "NiPSBoxEmitter.inl"

#endif  // #ifndef NIPSBOXEMITTER_H
