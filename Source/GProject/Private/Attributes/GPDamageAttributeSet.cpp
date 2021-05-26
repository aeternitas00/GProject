// Fill out your copyright notice in the Description page of Project Settings.

#include "Attributes/GPDamageAttributeSet.h"
#include "Component/GPAbilitySystemComponent.h"
#include "GPWeaponActorBase.h"
#include "GameplayEffect.h"
#include "GameplayEffectExtension.h"

UGPDamageAttributeSet::UGPDamageAttributeSet()
	: Damage(0.0f)
	, DamageAmplifier(1.0f)
	, ArmorPierce(0.0f)
	
{}

void UGPDamageAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UGPDamageAttributeSet, DamageAmplifier);
	DOREPLIFETIME(UGPDamageAttributeSet, ArmorPierce);
}

void UGPDamageAttributeSet::OnRep_DamageAmplifier(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGPDamageAttributeSet, DamageAmplifier, OldValue);
}

void UGPDamageAttributeSet::OnRep_ArmorPierce(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGPDamageAttributeSet, ArmorPierce, OldValue);
}
