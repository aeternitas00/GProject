// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "GPWeaponAttributeSet.generated.h"

// Uses macros from AttributeSet.h
#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

UCLASS()
class GPROJECT_API UGPWeaponAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	// Constructor and overrides
	UGPWeaponAttributeSet();
	//virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	UPROPERTY(BlueprintReadOnly, Category = "Magazine", ReplicatedUsing = OnRep_CurrentMag)
	FGameplayAttributeData CurrentMag;
	ATTRIBUTE_ACCESSORS(UGPWeaponAttributeSet, CurrentMag)

	UPROPERTY(BlueprintReadOnly, Category = "Magazine", ReplicatedUsing = OnRep_MagSize)
	FGameplayAttributeData MagSize;
	ATTRIBUTE_ACCESSORS(UGPWeaponAttributeSet, MagSize)

protected:
	/** Helper function to proportionally adjust the value of an attribute when it's associated max attribute changes. (i.e. When MaxHealth increases, Health increases by an amount that maintains the same percentage as before) */
	//void AdjustAttributeForMaxChange(FGameplayAttributeData& AffectedAttribute, const FGameplayAttributeData& MaxAttribute, float NewMaxValue, const FGameplayAttribute& AffectedAttributeProperty);

	UFUNCTION()
	virtual void OnRep_CurrentMag(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_MagSize(const FGameplayAttributeData& OldValue);
};
