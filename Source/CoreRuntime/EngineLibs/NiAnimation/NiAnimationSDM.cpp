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

// Precompiled Header
#include "NiAnimationPCH.h"

#include "NiAnimation.h"
#include "NiAnimationSDM.h"

NiImplementSDMConstructor(NiAnimation, "NiMesh NiFloodgate NiMain");

#ifdef NIANIMATION_EXPORT
NiImplementDllMain(NiAnimation);
#endif

//--------------------------------------------------------------------------------------------------
void NiAnimationSDM::Init()
{
    NiImplementSDMInitCheck();

    NiRegisterStream(NiAlphaController);
    NiRegisterStream(NiBlendBoolInterpolator);
    NiRegisterStream(NiBlendColorInterpolator);
    NiRegisterStream(NiBlendFloatInterpolator);
    NiRegisterStream(NiBlendPoint3Interpolator);
    NiRegisterStream(NiBlendQuaternionInterpolator);
    NiRegisterStream(NiBlendTransformInterpolator);
    NiRegisterStream(NiBoolData);
    NiRegisterStream(NiBoolEvaluator);
    NiRegisterStream(NiBoolInterpolator);
    NiRegisterStream(NiBoolTimelineEvaluator);
    NiRegisterStream(NiBoolTimelineInterpolator);
    NiRegisterStream(NiBSplineBasisData);
    NiRegisterStream(NiBSplineColorEvaluator);
    NiRegisterStream(NiBSplineColorInterpolator);
    NiRegisterStream(NiBSplineCompColorEvaluator);
    NiRegisterStream(NiBSplineCompColorInterpolator);
    NiRegisterStream(NiBSplineCompFloatEvaluator);
    NiRegisterStream(NiBSplineCompFloatInterpolator);
    NiRegisterStream(NiBSplineCompPoint3Evaluator);
    NiRegisterStream(NiBSplineCompPoint3Interpolator);
    NiRegisterStream(NiBSplineCompTransformEvaluator);
    NiRegisterStream(NiBSplineCompTransformInterpolator);
    NiRegisterStream(NiBSplineData);
    NiRegisterStream(NiBSplineFloatEvaluator);
    NiRegisterStream(NiBSplineFloatInterpolator);
    NiRegisterStream(NiBSplinePoint3Evaluator);
    NiRegisterStream(NiBSplinePoint3Interpolator);
    NiRegisterStream(NiBSplineTransformEvaluator);
    NiRegisterStream(NiBSplineTransformInterpolator);
    NiRegisterStream(NiColorData);
    NiRegisterStream(NiColorEvaluator);
    NiRegisterStream(NiColorExtraDataController);
    NiRegisterStream(NiColorInterpolator);
    NiRegisterStream(NiConstBoolEvaluator);
    NiRegisterStream(NiConstColorEvaluator);
    NiRegisterStream(NiConstFloatEvaluator);
    NiRegisterStream(NiConstPoint3Evaluator);
    NiRegisterStream(NiConstQuaternionEvaluator);
    NiRegisterStream(NiConstTransformEvaluator);
    NiRegisterStream(NiControllerManager);

    NiRegisterStream(NiFlipController);
    NiRegisterStream(NiFloatData);
    NiRegisterStream(NiFloatEvaluator);
    NiRegisterStream(NiFloatExtraDataController);
    NiRegisterStream(NiFloatInterpolator);
    NiRegisterStream(NiFloatsExtraDataController);
    NiRegisterStream(NiFloatsExtraDataPoint3Controller);

    NiRegisterStream(NiLightColorController);
    NiRegisterStream(NiLightDimmerController);
    NiRegisterStream(NiLookAtEvaluator);
    NiRegisterStream(NiLookAtInterpolator);
    NiRegisterStream(NiMaterialColorController);
    NiRegisterStream(NiMorphWeightsController);
    NiRegisterStream(NiPathEvaluator);
    NiRegisterStream(NiPathInterpolator);
    NiRegisterStream(NiPoint3Evaluator);
    NiRegisterStream(NiPoint3Interpolator);
    NiRegisterStream(NiPosData);
    NiRegisterStream(NiPoseBinding);
    NiRegisterStream(NiQuaternionEvaluator);
    NiRegisterStream(NiQuaternionInterpolator);
    NiRegisterStream(NiRotData);
    NiRegisterStream(NiSkinningLODController);
    NiRegisterStream(NiSequenceData);
    NiRegisterStream(NiStringPalette);
    NiRegisterStream(NiTextKeyExtraData);
    NiRegisterStream(NiTextureTransformController);
    NiRegisterStream(NiTransformController);
    NiRegisterStream(NiTransformData);
    NiRegisterStream(NiTransformEvaluator);
    NiRegisterStream(NiTransformInterpolator);
    NiRegisterStream(NiVisController);

#ifndef EE_REMOVE_BACK_COMPAT_STREAMING
    NiRegisterStream(NiBoneLODController);
    NiRegisterStream(NiGeomMorpherController);
    NiRegisterStream(NiMorphData);
    NiRegisterStream(NiMultiTargetTransformController);

    // In Gamebryo 2.6, NiSequenceData replaced NiControllerSequence
    // as the streamable sequence entity.
    NiStream::RegisterLoader("NiControllerSequence",
        NiSequenceData::CreateObject);

    // In Gamebryo 1.2, NiVisData was deprecated.
    // NiBoolData took its place.
    NiStream::RegisterLoader("NiVisData",
        NiBoolData::CreateObject);

    // In Gamebryo 1.2, NiKeyframeController was deprecated.
    // NiTransformController took its place.
    NiStream::RegisterLoader("NiKeyframeController",
        NiTransformController::CreateObject);

    // In Gamebryo 1.2, NiKeyframeData was renamed NiTransformData.
    NiStream::RegisterLoader("NiKeyframeData",
        NiTransformData::CreateObject);

    NiStream::RegisterPostProcessFunction(NiOldAnimationConverter::Convert);
    NiStream::RegisterPostProcessFunction(NiGeomMorpherConverter::Convert);
    NiStream::RegisterPostProcessFunction(NiBoneLODController::Convert);
#endif

    NiAnimationConstants::_SDMInit();
}

//--------------------------------------------------------------------------------------------------
void NiAnimationSDM::Shutdown()
{
    NiImplementSDMShutdownCheck();

    NiUnregisterStream(NiAlphaController);
    NiUnregisterStream(NiBlendBoolInterpolator);
    NiUnregisterStream(NiBlendColorInterpolator);
    NiUnregisterStream(NiBlendFloatInterpolator);
    NiUnregisterStream(NiBlendPoint3Interpolator);
    NiUnregisterStream(NiBlendQuaternionInterpolator);
    NiUnregisterStream(NiBlendTransformInterpolator);
    NiUnregisterStream(NiBoolData);
    NiUnregisterStream(NiBoolEvaluator);
    NiUnregisterStream(NiBoolInterpolator);
    NiUnregisterStream(NiBoolTimelineEvaluator);
    NiUnregisterStream(NiBoolTimelineInterpolator);
    NiUnregisterStream(NiBSplineBasisData);
    NiUnregisterStream(NiBSplineColorEvaluator);
    NiUnregisterStream(NiBSplineColorInterpolator);
    NiUnregisterStream(NiBSplineCompColorEvaluator);
    NiUnregisterStream(NiBSplineCompColorInterpolator);
    NiUnregisterStream(NiBSplineCompFloatEvaluator);
    NiUnregisterStream(NiBSplineCompFloatInterpolator);
    NiUnregisterStream(NiBSplineCompPoint3Evaluator);
    NiUnregisterStream(NiBSplineCompPoint3Interpolator);
    NiUnregisterStream(NiBSplineCompTransformEvaluator);
    NiUnregisterStream(NiBSplineCompTransformInterpolator);
    NiUnregisterStream(NiBSplineData);
    NiUnregisterStream(NiBSplineFloatEvaluator);
    NiUnregisterStream(NiBSplineFloatInterpolator);
    NiUnregisterStream(NiBSplinePoint3Evaluator);
    NiUnregisterStream(NiBSplinePoint3Interpolator);
    NiUnregisterStream(NiBSplineTransformEvaluator);
    NiUnregisterStream(NiBSplineTransformInterpolator);
    NiUnregisterStream(NiColorData);
    NiUnregisterStream(NiColorEvaluator);
    NiUnregisterStream(NiColorExtraDataController);
    NiUnregisterStream(NiColorInterpolator);
    NiUnregisterStream(NiConstBoolEvaluator);
    NiUnregisterStream(NiConstColorEvaluator);
    NiUnregisterStream(NiConstFloatEvaluator);
    NiUnregisterStream(NiConstPoint3Evaluator);
    NiUnregisterStream(NiConstQuaternionEvaluator);
    NiUnregisterStream(NiConstTransformEvaluator);
    NiUnregisterStream(NiControllerManager);

    NiUnregisterStream(NiFlipController);
    NiUnregisterStream(NiFloatData);
    NiUnregisterStream(NiFloatEvaluator);
    NiUnregisterStream(NiFloatExtraDataController);
    NiUnregisterStream(NiFloatInterpolator);
    NiUnregisterStream(NiFloatsExtraDataController);
    NiUnregisterStream(NiFloatsExtraDataPoint3Controller);

    NiUnregisterStream(NiLightColorController);
    NiUnregisterStream(NiLightDimmerController);
    NiUnregisterStream(NiLookAtEvaluator);
    NiUnregisterStream(NiLookAtInterpolator);
    NiUnregisterStream(NiMaterialColorController);
    NiUnregisterStream(NiMorphWeightsController);
    NiUnregisterStream(NiPathEvaluator);
    NiUnregisterStream(NiPathInterpolator);
    NiUnregisterStream(NiPoint3Evaluator);
    NiUnregisterStream(NiPoint3Interpolator);
    NiUnregisterStream(NiPosData);
    NiUnregisterStream(NiPoseBinding);
    NiUnregisterStream(NiQuaternionEvaluator);
    NiUnregisterStream(NiQuaternionInterpolator);
    NiUnregisterStream(NiRotData);
    NiUnregisterStream(NiSkinningLODController);
    NiUnregisterStream(NiSequenceData);
    NiUnregisterStream(NiStringPalette);
    NiUnregisterStream(NiTextKeyExtraData);
    NiUnregisterStream(NiTextureTransformController);
    NiUnregisterStream(NiTransformController);
    NiUnregisterStream(NiTransformData);
    NiUnregisterStream(NiTransformEvaluator);
    NiUnregisterStream(NiTransformInterpolator);
    NiUnregisterStream(NiVisController);

#ifndef EE_REMOVE_BACK_COMPAT_STREAMING
    NiUnregisterStream(NiBoneLODController);
    NiUnregisterStream(NiGeomMorpherController);
    NiUnregisterStream(NiMorphData);
    NiUnregisterStream(NiMultiTargetTransformController);

    // In Gamebryo 2.6, NiSequenceData replaced NiControllerSequence
    // as the streamable sequence entity.
    NiUnregisterStream(NiControllerSequence);

    // In Gamebryo 1.2, NiKeyframeController was deprecated.
    // NiTransformController took its place.
    NiUnregisterStream(NiKeyframeController);

    // In Gamebryo 1.2, NiKeyframeData was renamed NiTransformData.
    NiUnregisterStream(NiKeyframeData);

    // In Gamebryo 1.2, NiVisData was deprecated.
    // NiBoolData took its place.
    NiUnregisterStream(NiVisData);

    NiStream::UnregisterPostProcessFunction(NiOldAnimationConverter::Convert);
    NiStream::UnregisterPostProcessFunction(NiGeomMorpherConverter::Convert);
    NiStream::UnregisterPostProcessFunction(NiBoneLODController::Convert);
#endif

    NiAnimationConstants::_SDMShutdown();
}

//--------------------------------------------------------------------------------------------------
