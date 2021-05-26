// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GProject.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "GPDamageAttributeSet.generated.h"

// Uses macros from AttributeSet.h
#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

UCLASS()
class GPROJECT_API UGPDamageAttributeSet : public UAttributeSet
{
	GENERATED_BODY()
	
public:
	
	UGPDamageAttributeSet();
	//virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(BlueprintReadOnly, Category = "Damage")
	FGameplayAttributeData Damage;
	ATTRIBUTE_ACCESSORS(UGPDamageAttributeSet, Damage)

	UPROPERTY(BlueprintReadOnly, Category = "Damage", ReplicatedUsing = OnRep_DamageAmplifier)
	FGameplayAttributeData DamageAmplifier;
	ATTRIBUTE_ACCESSORS(UGPDamageAttributeSet, DamageAmplifier)

	UPROPERTY(BlueprintReadOnly, Category = "Damage", ReplicatedUsing = OnRep_ArmorPierce)
	FGameplayAttributeData ArmorPierce;
	ATTRIBUTE_ACCESSORS(UGPDamageAttributeSet, ArmorPierce)

protected:
	UFUNCTION()
	virtual void OnRep_DamageAmplifier(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_ArmorPierce(const FGameplayAttributeData& OldValue);

};
