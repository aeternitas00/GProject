// Fill out your copyright notice in the Description page of Project Settings.


#include "Types/GPAbilityTypes.h"
#include "Component/GPAbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"

bool FGPGameplayEffectContainerSpec::HasValidEffects() const
{
	return TargetGameplayEffectSpecs.Num() > 0;
}

bool FGPGameplayEffectContainerSpec::HasValidTargets() const
{
	return TargetData.Num() > 0;
}

void FGPGameplayEffectContainerSpec::AddTargets(const TArray<FHitResult>& HitResults, const TArray<AActor*>& TargetActors)
{
	for (const FHitResult& HitResult : HitResults)
	{
		FGameplayAbilityTargetData_SingleTargetHit* NewData = new FGameplayAbilityTargetData_SingleTargetHit(HitResult);
		TargetData.Add(NewData);
	}

	if (TargetActors.Num() > 0)
	{
		FGameplayAbilityTargetData_ActorArray* NewData = new FGameplayAbilityTargetData_ActorArray();
		NewData->TargetActorArray.Append(TargetActors);
		TargetData.Add(NewData);
	}
}

//void FGPGameplayEffectContainerSpec::AddUniqueTargets(const TArray<FHitResult>& HitResults, const TArray<AActor*>& TargetActors)
//{
//	for (const FHitResult& HitResult : HitResults)
//	{
//		FGameplayAbilityTargetData_SingleTargetHit* NewData = new FGameplayAbilityTargetData_SingleTargetHit(HitResult);
//		TargetData.Add(NewData);
//	}
//
//	if (TargetActors.Num() > 0)
//	{
//		FGameplayAbilityTargetData_ActorArray* NewData = new FGameplayAbilityTargetData_ActorArray();
//		NewData->TargetActorArray.Append(TargetActors);
//		TargetData.Add(NewData);
//	}
//}