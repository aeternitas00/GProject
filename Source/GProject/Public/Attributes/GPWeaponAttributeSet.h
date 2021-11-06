// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GProject.h"
#include "GPDamageAttributeSet.h"
#include "GPWeaponAttributeSet.generated.h"

// Uses macros from AttributeSet.h
//#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
//	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
//	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
//	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
//	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

UCLASS()
class GPROJECT_API UGPWeaponAttributeSet : public UGPDamageAttributeSet
{
	GENERATED_BODY()

public:
	// Constructor and overrides
	UGPWeaponAttributeSet();
	//virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	UPROPERTY(BlueprintReadOnly, Category = "Weapon", ReplicatedUsing = OnRep_CurrentMag)
	FGameplayAttributeData CurrentMag;
	ATTRIBUTE_ACCESSORS(UGPWeaponAttributeSet, CurrentMag)

	UPROPERTY(BlueprintReadOnly, Category = "Weapon", ReplicatedUsing = OnRep_MagSize)
	FGameplayAttributeData MagSize;
	ATTRIBUTE_ACCESSORS(UGPWeaponAttributeSet, MagSize)

	UPROPERTY(BlueprintReadOnly, Category = "Weapon", ReplicatedUsing = OnRep_MagSize)
	FGameplayAttributeData FireRate;
	ATTRIBUTE_ACCESSORS(UGPWeaponAttributeSet, FireRate)

	UPROPERTY(BlueprintReadOnly, Category = "Weapon", ReplicatedUsing = OnRep_Accuracy)
	FGameplayAttributeData Accuracy;
	ATTRIBUTE_ACCESSORS(UGPWeaponAttributeSet, Accuracy)
		
	UPROPERTY(BlueprintReadOnly, Category = "Weapon", ReplicatedUsing = OnRep_ReloadSpeed)
	FGameplayAttributeData ReloadSpeed;
	ATTRIBUTE_ACCESSORS(UGPWeaponAttributeSet, ReloadSpeed)

	UPROPERTY(BlueprintReadOnly, Category = "Weapon", ReplicatedUsing = OnRep_RecoilX)
	FGameplayAttributeData RecoilX;
	ATTRIBUTE_ACCESSORS(UGPWeaponAttributeSet, RecoilX)

	UPROPERTY(BlueprintReadOnly, Category = "Weapon", ReplicatedUsing = OnRep_RecoilY)
	FGameplayAttributeData RecoilY;
	ATTRIBUTE_ACCESSORS(UGPWeaponAttributeSet, RecoilY)

protected:
	/** Helper function to proportionally adjust the value of an attribute when it's associated max attribute changes. (i.e. When MaxHealth increases, Health increases by an amount that maintains the same percentage as before) */
	//void AdjustAttributeForMaxChange(FGameplayAttributeData& AffectedAttribute, const FGameplayAttributeData& MaxAttribute, float NewMaxValue, const FGameplayAttribute& AffectedAttributeProperty);

	UFUNCTION()
	virtual void OnRep_CurrentMag(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_MagSize(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_FireRate(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_Accuracy(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_ReloadSpeed(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_RecoilX(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_RecoilY(const FGameplayAttributeData& OldValue);
};
