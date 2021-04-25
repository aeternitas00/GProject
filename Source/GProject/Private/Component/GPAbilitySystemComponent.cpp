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

int32 UGPAbilitySystemComponent::GetDefaultAbilityLevel() const
{
	AGPCharacterBase* OwningCharacter = Cast<AGPCharacterBase>(GetOwnerActor());

	if (OwningCharacter)
	{
		return 1;//OwningCharacter->GetCharacterLevel();
	}
	return 1;
}

UGPAbilitySystemComponent* UGPAbilitySystemComponent::GetAbilitySystemComponentFromActor(const AActor* Actor, bool LookForComponent)
{
	return Cast<UGPAbilitySystemComponent>(UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Actor, LookForComponent));
}

void UGPAbilitySystemComponent::OnTagUpdated(const FGameplayTag& Tag, bool TagExists)
{
	OnTagUpdatedDelegate.Broadcast(Tag, TagExists);
}
