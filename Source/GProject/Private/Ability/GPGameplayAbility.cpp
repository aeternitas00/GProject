// Copyright Epic Games, Inc. All Rights Reserved.

#include "Ability/GPGameplayAbility.h"
#include "Component/GPAbilitySystemComponent.h"
#include "Types/GPTargetType.h"
#include "Styling/SlateBrush.h"
#include "GPCharacterBase.h"

UGPGameplayAbility::UGPGameplayAbility():SkillIcon() { }

bool UGPGameplayAbility::GetTargetsFromEffectContainer(const FGPGameplayEffectContainer& Container, const FGameplayEventData& EventData, TArray<FHitResult>& outHitResults, TArray<AActor*>& outTargetActors)
{
	AActor* OwningActor = GetOwningActorFromActorInfo();
	AGPCharacterBase* OwningCharacter = Cast<AGPCharacterBase>(OwningActor);
	UGPAbilitySystemComponent* OwningASC = UGPAbilitySystemComponent::GetAbilitySystemComponentFromActor(OwningActor);

	if (OwningASC)
	{
		// If we have a target type, run the targeting logic. This is optional, targets can be added later
		if (Container.TargetType.Get())
		{
			const UGPTargetType* TargetTypeCDO = Container.TargetType.GetDefaultObject();
			AActor* AvatarActor = GetAvatarActorFromActorInfo();
			TargetTypeCDO->GetTargets(OwningCharacter, AvatarActor, EventData, outHitResults, outTargetActors);
			
			return true;
		}
	}
	return false;
}

FGPGameplayEffectContainerSpec UGPGameplayAbility::MakeEffectContainerSpecFromContainer(const FGPGameplayEffectContainer& Container, const FGameplayEventData& EventData, int32 OverrideGameplayLevel)
{
	// First figure out our actor info
	FGPGameplayEffectContainerSpec ReturnSpec;
	UGPAbilitySystemComponent* OwningASC = 
		UGPAbilitySystemComponent::GetAbilitySystemComponentFromActor(GetOwningActorFromActorInfo());

	if (OwningASC)
	{
		// If we have a target type, run the targeting logic. This is optional, targets can be added later
		TArray<FHitResult> HitResults;
		TArray<AActor*> TargetActors;
		if (GetTargetsFromEffectContainer(Container, EventData, HitResults, TargetActors))
		{
			ReturnSpec.AddTargets(HitResults, TargetActors);
		}

		// If we don't have an override level, use the default on the ability itself
		if (OverrideGameplayLevel == INDEX_NONE)
		{
			OverrideGameplayLevel = this->GetAbilityLevel(); //OwningASC->GetDefaultAbilityLevel();
		}

		// Build GameplayEffectSpecs for each applied effect
		for (const TSubclassOf<UGameplayEffect>& EffectClass : Container.TargetGameplayEffectClasses)
		{
			ReturnSpec.TargetGameplayEffectSpecs.Add(MakeOutgoingGameplayEffectSpec(EffectClass, OverrideGameplayLevel));
		}
	}
	return ReturnSpec;
}

FGPGameplayEffectContainerSpec UGPGameplayAbility::MakeEffectContainerSpec(FGameplayTag ContainerTag, const FGameplayEventData& EventData, int32 OverrideGameplayLevel)
{
	FGPGameplayEffectContainer* FoundContainer = EffectContainerMap.Find(ContainerTag);

	if (FoundContainer)
	{
		return MakeEffectContainerSpecFromContainer(*FoundContainer, EventData, OverrideGameplayLevel);
	}
	return FGPGameplayEffectContainerSpec();
}

TArray<FActiveGameplayEffectHandle> UGPGameplayAbility::ApplyEffectContainerSpec(const FGPGameplayEffectContainerSpec& ContainerSpec)
{
	TArray<FActiveGameplayEffectHandle> AllEffects;

	// Iterate list of effect specs and apply them to their target data
	for (const FGameplayEffectSpecHandle& SpecHandle : ContainerSpec.TargetGameplayEffectSpecs)
	{
		AllEffects.Append(K2_ApplyGameplayEffectSpecToTarget(SpecHandle, ContainerSpec.TargetData));
	}
	return AllEffects;
}

TArray<FActiveGameplayEffectHandle> UGPGameplayAbility::ApplyEffectContainer(FGameplayTag ContainerTag, const FGameplayEventData& EventData, int32 OverrideGameplayLevel)
{
	FGPGameplayEffectContainerSpec Spec = MakeEffectContainerSpec(ContainerTag, EventData, OverrideGameplayLevel);
	return ApplyEffectContainerSpec(Spec);
}

void UGPGameplayAbility::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnGiveAbility(ActorInfo, Spec);

	AGPCharacterBase* OwningCharacter = Cast<AGPCharacterBase>(ActorInfo->OwnerActor);
	OwningCharacter->OnGiveAbility(Spec);
}
