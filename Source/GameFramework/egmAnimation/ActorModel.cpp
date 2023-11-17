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

// Pre-compiled header
#include "egmAnimationPCH.h"

#include <egmAnimation/AnimationService.h>

#include <egmAnimation/ActorModel.h>

using namespace efd;
using namespace egf;
using namespace ecr;
using namespace egmAnimation;

EE_IMPLEMENT_CONCRETE_CLASS_INFO(ActorModel);

EE_IMPLEMENT_BUILTINMODEL_PROPERTIES(ActorModel);

//------------------------------------------------------------------------------------------------
IBuiltinModel* ActorModel::ActorModelFactory()
{
    return EE_NEW ActorModel();
}

//------------------------------------------------------------------------------------------------
ActorModel::ActorModel()
    : m_kfmAsset()
    , m_isKfmAssetShared(true)
    , m_isNifAssetShared(true)
    , m_accumulatesTransforms(true)
    , m_initialSequence()
{
}

//------------------------------------------------------------------------------------------------
ActorModel::~ActorModel()
{
}

//------------------------------------------------------------------------------------------------
bool ActorModel::Initialize(egf::Entity* pOwner, const egf::PropertyDescriptorList& defaults)
{
    IBuiltinModel::Initialize(pOwner, defaults);

    for (PropertyDescriptorList::const_iterator iter = defaults.begin();
        iter != defaults.end();
        ++iter)
    {
        switch ((*iter)->GetPropertyID())
        {
            case kPropertyID_StandardModelLibrary_KfmAsset:
                (*iter)->GetDefaultProperty()->GetValue(
                    kPropertyID_StandardModelLibrary_KfmAsset,
                    &m_kfmAsset);
                break;

            case kPropertyID_StandardModelLibrary_IsKfmAssetShared:
                (*iter)->GetDefaultProperty()->GetValue(
                    kPropertyID_StandardModelLibrary_IsKfmAssetShared,
                    &m_isKfmAssetShared);
                break;

            case kPropertyID_StandardModelLibrary_IsNifAssetShared:
                (*iter)->GetDefaultProperty()->GetValue(
                    kPropertyID_StandardModelLibrary_IsNifAssetShared,
                    &m_isNifAssetShared);
                break;

            case kPropertyID_StandardModelLibrary_InitialSequence:
                (*iter)->GetDefaultProperty()->GetValue(
                    kPropertyID_StandardModelLibrary_InitialSequence,
                    &m_initialSequence);
                break;

            case kPropertyID_StandardModelLibrary_AccumulatesTransforms:
                (*iter)->GetDefaultProperty()->GetValue(
                    kPropertyID_StandardModelLibrary_AccumulatesTransforms,
                    &m_accumulatesTransforms);
                break;

            case kPropertyID_StandardModelLibrary_AttachedObjects:
                IAttachedObjectsProperty::SetAttachedObjectsProperty((*iter));
                break;

            default:;
        }
    }

    return true;
}

//------------------------------------------------------------------------------------------------
bool ActorModel::ResetProperty(const egf::PropertyDescriptor* pDefault)
{
    switch (pDefault->GetPropertyID())
    {
        case kPropertyID_StandardModelLibrary_KfmAsset:
            pDefault->GetDefaultProperty()->GetValue(
                kPropertyID_StandardModelLibrary_KfmAsset,
                &m_kfmAsset);
            break;

        case kPropertyID_StandardModelLibrary_IsKfmAssetShared:
            pDefault->GetDefaultProperty()->GetValue(
                kPropertyID_StandardModelLibrary_IsKfmAssetShared,
                &m_isKfmAssetShared);
            break;

        case kPropertyID_StandardModelLibrary_IsNifAssetShared:
            pDefault->GetDefaultProperty()->GetValue(
                kPropertyID_StandardModelLibrary_IsNifAssetShared,
                &m_isNifAssetShared);
            break;

        case kPropertyID_StandardModelLibrary_InitialSequence:
            pDefault->GetDefaultProperty()->GetValue(
                kPropertyID_StandardModelLibrary_InitialSequence,
                &m_initialSequence);
            break;

        case kPropertyID_StandardModelLibrary_AccumulatesTransforms:
            pDefault->GetDefaultProperty()->GetValue(
                kPropertyID_StandardModelLibrary_AccumulatesTransforms,
                &m_accumulatesTransforms);
            break;

        case kPropertyID_StandardModelLibrary_AttachedObjects:
            IAttachedObjectsProperty::SetAttachedObjectsProperty(pDefault);
            break;

        default:;
    }

    return true;
}

//------------------------------------------------------------------------------------------------
void ActorModel::OnAdded()
{
    AnimationService* pAnimationService =
        GetServiceManager()->GetSystemServiceAs<AnimationService>();
    EE_ASSERT(pAnimationService);

    pAnimationService->AddEntityWithActorModel(m_pOwningEntity);
}

//------------------------------------------------------------------------------------------------
void ActorModel::OnRemoved()
{
    AnimationService* pAnimationService =
        GetServiceManager()->GetSystemServiceAs<AnimationService>();
    if (pAnimationService)
    {
        pAnimationService->RemoveEntityWithActorModel(m_pOwningEntity);
    }

    IBuiltinModel::OnRemoved();
}

//------------------------------------------------------------------------------------------------
bool ActorModel::operator==(const IProperty& other) const
{
    if (!IBuiltinModel::operator ==(other))
        return false;

    const ActorModel* otherClass = static_cast<const ActorModel *>(&other);

    if (m_kfmAsset != otherClass->m_kfmAsset ||
        m_isKfmAssetShared != otherClass->m_isKfmAssetShared ||
        m_isNifAssetShared != otherClass->m_isNifAssetShared ||
        m_initialSequence != otherClass->m_initialSequence ||
        m_accumulatesTransforms != otherClass->m_accumulatesTransforms)
    {
        return false;
    }

    return IAttachedObjectsProperty::operator ==(*otherClass);
}

//------------------------------------------------------------------------------------------------
