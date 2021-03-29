// Fill out your copyright notice in the Description page of Project Settings.


#include "Types/GPBPFuncLibrary.h"
#include "Types/GPTypes.h"

bool UGPBPFuncLibrary::EqualEqual_GPItemSlot(const FGPItemSlot& A, const FGPItemSlot& B)
{
	return A == B;
}

bool UGPBPFuncLibrary::NotEqual_GPItemSlot(const FGPItemSlot& A, const FGPItemSlot& B)
{
	return A != B;
}

bool UGPBPFuncLibrary::IsValidItemSlot(const FGPItemSlot& ItemSlot)
{
	return ItemSlot.IsValid();
}

bool UGPBPFuncLibrary::DoesEffectContainerSpecHaveEffects(const FGPGameplayEffectContainerSpec& ContainerSpec)
{
	return ContainerSpec.HasValidEffects();
}

bool UGPBPFuncLibrary::DoesEffectContainerSpecHaveTargets(const FGPGameplayEffectContainerSpec& ContainerSpec)
{
	return ContainerSpec.HasValidTargets();
}

FGPGameplayEffectContainerSpec UGPBPFuncLibrary::AddTargetsToEffectContainerSpec(const FGPGameplayEffectContainerSpec& ContainerSpec, const TArray<FHitResult>& HitResults, const TArray<AActor*>& TargetActors)
{
	FGPGameplayEffectContainerSpec NewSpec = ContainerSpec;
	NewSpec.AddTargets(HitResults, TargetActors);
	return NewSpec;
}

TArray<FActiveGameplayEffectHandle> UGPBPFuncLibrary::ApplyExternalEffectContainerSpec(const FGPGameplayEffectContainerSpec& ContainerSpec)
{
	TArray<FActiveGameplayEffectHandle> AllEffects;

	// Iterate list of gameplay effects
	for (const FGameplayEffectSpecHandle& SpecHandle : ContainerSpec.TargetGameplayEffectSpecs)
	{
		if (SpecHandle.IsValid())
		{
			// If effect is valid, iterate list of targets and apply to all
			for (TSharedPtr<FGameplayAbilityTargetData> Data : ContainerSpec.TargetData.Data)
			{
				AllEffects.Append(Data->ApplyGameplayEffectSpec(*SpecHandle.Data.Get()));
			}
		}
	}
	return AllEffects;
}