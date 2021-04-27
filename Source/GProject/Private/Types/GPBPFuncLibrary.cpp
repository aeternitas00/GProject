// Fill out your copyright notice in the Description page of Project Settings.


#include "Types/GPBPFuncLibrary.h"
#include "Types/GPTypes.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Ability/GPGameplayAbility.h"
#include "Ability/GPProjectileBase.h"
#include "GPCharacterBase.h"

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

//FGPGameplayEffectContainerSpec UGPBPFuncLibrary::AddUniqueTargetsToEffectContainerSpec(const FGPGameplayEffectContainerSpec& ContainerSpec, const TArray<FHitResult>& HitResults, const TArray<AActor*>& TargetActors)
//{
//	FGPGameplayEffectContainerSpec NewSpec = ContainerSpec;
//	NewSpec.AddTargets(HitResults, TargetActors);
//	return NewSpec;
//}

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


AGPProjectileBase* UGPBPFuncLibrary::SpawnProjectileFromAbility(UGPGameplayAbility* SpawningAbility, TSubclassOf<AGPProjectileBase> ProjectileClass, FGameplayTag GameplayTag, const FGameplayEventData& EventData, const FTransform& inTransform = FTransform(),bool bOverrideTransform = false)
{
	FGPGameplayEffectContainerSpec Spec = SpawningAbility->MakeEffectContainerSpec(GameplayTag, EventData, -1);
	AGPCharacterBase* OwningCharacter = Cast<AGPCharacterBase>(SpawningAbility->GetOwningActorFromActorInfo());

	AGPProjectileBase* Projectile = nullptr;

	if (!OwningCharacter->IsValidLowLevel()) return nullptr;

	if (UGPBPFuncLibrary::DoesEffectContainerSpecHaveEffects(Spec) && UKismetSystemLibrary::IsValidClass(ProjectileClass))
	{
		FTransform Trsf = OwningCharacter->GetActorTransform();
		if (bOverrideTransform)
			Trsf = inTransform;

		Trsf.SetScale3D(FVector(1.0f, 1.0f, 1.0f));
		FActorSpawnParameters SpawnParameters;
		SpawnParameters.Instigator = OwningCharacter;
		SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		Projectile = OwningCharacter->GetWorld()->SpawnActor<AGPProjectileBase>(ProjectileClass, Trsf, SpawnParameters);

		Projectile->AddIgnoreActorWhenMoving(OwningCharacter);
	}

	return Projectile;
}
