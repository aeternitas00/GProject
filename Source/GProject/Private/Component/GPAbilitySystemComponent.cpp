// Copyright Epic Games, Inc. All Rights Reserved.

#include "Component/GPAbilitySystemComponent.h"
#include "GPCharacterBase.h"
#include "Ability/GPGameplayAbility.h"
#include "AbilitySystemGlobals.h"

UGPAbilitySystemComponent::UGPAbilitySystemComponent() {}

void UGPAbilitySystemComponent::GetActiveAbilitiesWithTags(const FGameplayTagContainer& GameplayTagContainer, TArray<UGPGameplayAbility*>& ActiveAbilities)
{
	TArray<FGameplayAbilitySpec*> AbilitiesToActivate;
	GetActivatableGameplayAbilitySpecsByAllMatchingTags(GameplayTagContainer, AbilitiesToActivate, false);

	// Iterate the list of all ability specs
	for (FGameplayAbilitySpec* Spec : AbilitiesToActivate)
	{
		// Iterate all instances on this ability spec
		TArray<UGameplayAbility*> AbilityInstances = Spec->GetAbilityInstances();

		for (UGameplayAbility* ActiveAbility : AbilityInstances)
		{
			ActiveAbilities.Add(Cast<UGPGameplayAbility>(ActiveAbility));
		}
	}
}

bool UGPAbilitySystemComponent::GetCooldownRemainingForTag(FGameplayTagContainer CooldownTags, float& TimeRemaining, float& CooldownDuration)
{
	if (CooldownTags.Num() > 0)
	{
		TimeRemaining = 0.f;
		CooldownDuration = 0.f;

		FGameplayEffectQuery const Query = FGameplayEffectQuery::MakeQuery_MatchAnyOwningTags(CooldownTags);
		TArray< TPair<float, float> > DurationAndTimeRemaining = GetActiveEffectsTimeRemainingAndDuration(Query);
		if (DurationAndTimeRemaining.Num() > 0)
		{
			int32 BestIdx = 0;
			float LongestTime = DurationAndTimeRemaining[0].Key;
			for (int32 Idx = 1; Idx < DurationAndTimeRemaining.Num(); ++Idx)
			{
				if (DurationAndTimeRemaining[Idx].Key > LongestTime)
				{
					LongestTime = DurationAndTimeRemaining[Idx].Key;
					BestIdx = Idx;
				}
			}

			TimeRemaining = DurationAndTimeRemaining[BestIdx].Key;
			CooldownDuration = DurationAndTimeRemaining[BestIdx].Value;

			return true;
		}
	}
	return false;
}

int32 UGPAbilitySystemComponent::GetDefaultAbilityLevel() const
{
	AGPCharacterBase* OwningCharacter = Cast<AGPCharacterBase>(GetOwnerActor());

	if (OwningCharacter)
	{
		return 1;//OwningCharacter->GetCharacterLevel();
	}
	return 1;
}

bool UGPAbilitySystemComponent::CancelAbilityByClass(TSubclassOf<UGameplayAbility> AbilityClass)
{
	if (FGameplayAbilitySpec* Spec = FindAbilitySpecFromClass(AbilityClass))
	{
		CancelAbility(Spec->Ability);
		return true;
	}
	return false;
}

UGPAbilitySystemComponent* UGPAbilitySystemComponent::GetAbilitySystemComponentFromActor(const AActor* Actor, bool LookForComponent)
{
	return Cast<UGPAbilitySystemComponent>(UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Actor, LookForComponent));
}

void UGPAbilitySystemComponent::OnTagUpdated(const FGameplayTag& Tag, bool TagExists)
{
	OnTagUpdatedDelegate.Broadcast(Tag, TagExists);
}
