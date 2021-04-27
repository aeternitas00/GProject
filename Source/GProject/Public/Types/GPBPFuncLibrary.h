// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GPAbilityTypes.h"
#include "GPBPFuncLibrary.generated.h"

/**
 * 
 */
UCLASS()
class GPROJECT_API UGPBPFuncLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Equal (GPItemSlot)", CompactNodeTitle = "==", Keywords = "== equal"), Category = Inventory)
	static bool EqualEqual_GPItemSlot(const FGPItemSlot& A, const FGPItemSlot& B);

	/** Inequality operator for ItemSlot */
	UFUNCTION(BlueprintPure, meta = (DisplayName = "NotEqualEqual (GPItemSlot)", CompactNodeTitle = "!=", Keywords = "!= not equal"), Category = Inventory)
	static bool NotEqual_GPItemSlot(const FGPItemSlot& A, const FGPItemSlot& B);

	/** Validity check for ItemSlot */
	UFUNCTION(BlueprintPure, Category = Inventory)
	static bool IsValidItemSlot(const FGPItemSlot& ItemSlot);

	/** Checks if spec has any effects */
	UFUNCTION(BlueprintPure, Category = Ability)
	static bool DoesEffectContainerSpecHaveEffects(const FGPGameplayEffectContainerSpec& ContainerSpec);

	/** Checks if spec has any targets */
	UFUNCTION(BlueprintPure, Category = Ability)
	static bool DoesEffectContainerSpecHaveTargets(const FGPGameplayEffectContainerSpec& ContainerSpec);

	/** Adds targets to a copy of the passed in effect container spec and returns it */
	UFUNCTION(BlueprintCallable, Category = Ability, meta = (AutoCreateRefTerm = "HitResults,TargetActors"))
	static FGPGameplayEffectContainerSpec AddTargetsToEffectContainerSpec(const FGPGameplayEffectContainerSpec& ContainerSpec, const TArray<FHitResult>& HitResults, const TArray<AActor*>& TargetActors);

	/** Adds targets to a copy of the passed in effect container spec and returns it */
	//UFUNCTION(BlueprintCallable, Category = Ability, meta = (AutoCreateRefTerm = "HitResults,TargetActors"))
	//static FGPGameplayEffectContainerSpec AddUniqueTargetsToEffectContainerSpec(const FGPGameplayEffectContainerSpec& ContainerSpec, const TArray<FHitResult>& HitResults, const TArray<AActor*>& TargetActors);

	/** Applies container spec that was made from an ability */
	UFUNCTION(BlueprintCallable, Category = Ability)
	static TArray<FActiveGameplayEffectHandle> ApplyExternalEffectContainerSpec(const FGPGameplayEffectContainerSpec& ContainerSpec);

	UFUNCTION(BlueprintCallable, Category = Ability)
	static AGPProjectileBase* SpawnProjectileFromAbility(UGPGameplayAbility* SpawningAbility, TSubclassOf<AGPProjectileBase> ProjectileClass, FGameplayTag GameplayTag, const FGameplayEventData& EventData, const FTransform& OverrideTransform = FTransform());
};
