// Fill out your copyright notice in the Description page of Project Settings.


#include "Attributes/GPWeaponAttributeSet.h"
#include "Component/GPAbilitySystemComponent.h"
#include "GPWeaponActorBase.h"
#include "GameplayEffect.h"
#include "GameplayEffectExtension.h"

UGPWeaponAttributeSet::UGPWeaponAttributeSet()
	: CurrentMag(0.0f)
	, MagSize(0.0f)
	, FireRate(1.0f)
	, Accuracy(1.0f)
	, ReloadSpeed(1.0f)
	, RecoilX(0.0f)
	, RecoilY(0.0f)
{}

//void UGPWeaponAttributeSet::PreAttributeChange(const FGameplayAttribute & Attribute, float& NewValue)
//{
//}

void UGPWeaponAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	FGameplayEffectContextHandle Context = Data.EffectSpec.GetContext();
	UAbilitySystemComponent* Source = Context.GetOriginalInstigatorAbilitySystemComponent();
	const FGameplayTagContainer& SourceTags = *Data.EffectSpec.CapturedSourceTags.GetAggregatedTags();

	// Compute the delta between old and new, if it is available
	float DeltaValue = 0;
	if (Data.EvaluatedData.ModifierOp == EGameplayModOp::Type::Additive)
	{
		// If this was additive, store the raw delta value to be passed along later
		DeltaValue = Data.EvaluatedData.Magnitude;
	}

	// Get the Target actor, which should be our owner
	AActor* TargetActor = nullptr;
	AController* TargetController = nullptr;
	AGPWeaponActorBase* TargetWeapon = nullptr;
	if (Data.Target.AbilityActorInfo.IsValid() && Data.Target.AbilityActorInfo->OwnerActor.IsValid())
	{
		TargetActor = Data.Target.AbilityActorInfo->OwnerActor.Get();
		TargetController = Data.Target.AbilityActorInfo->PlayerController.Get();
		TargetWeapon = Cast<AGPWeaponActorBase>(TargetActor);
	}

	if (Data.EvaluatedData.Attribute == GetCurrentMagAttribute())
	{
		if (TargetWeapon)
		{
			SetCurrentMag(FMath::Clamp(GetCurrentMag(), 0.0f, GetMagSize()));
			TargetWeapon->HandleCurrentMagChanged(DeltaValue, SourceTags);
		}
	}
	else if (Data.EvaluatedData.Attribute == GetMagSizeAttribute())
	{
		if (TargetWeapon)
		{
			TargetWeapon->HandleMagSizeChanged(DeltaValue, SourceTags);
		}
	}
	else if (Data.EvaluatedData.Attribute == GetFireRateAttribute())
	{
		if (TargetWeapon)
		{
			//TargetWeapon->HandleFireRateChanged(DeltaValue, SourceTags);
		}
	}
	else if (Data.EvaluatedData.Attribute == GetAccuracyAttribute())
	{
		if (TargetWeapon)
		{
			//TargetWeapon->HandleAccuracyChanged(DeltaValue, SourceTags);
		}
	}
	else if (Data.EvaluatedData.Attribute == GetReloadSpeedAttribute())
	{
		if (TargetWeapon)
		{
			//TargetWeapon->HandleReloadSpeedChanged(DeltaValue, SourceTags);
		}
	}
	else if (Data.EvaluatedData.Attribute == GetRecoilXAttribute())
	{
		if (TargetWeapon)
		{
			//TargetWeapon->HandleRecoilChanged(DeltaValue, SourceTags);
		}
	}
	else if (Data.EvaluatedData.Attribute == GetRecoilYAttribute())
	{
		if (TargetWeapon)
		{
			//TargetWeapon->HandleRecoilChanged(DeltaValue, SourceTags);
		}
	}
}

void UGPWeaponAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UGPWeaponAttributeSet, CurrentMag);
	DOREPLIFETIME(UGPWeaponAttributeSet, MagSize);
	DOREPLIFETIME(UGPWeaponAttributeSet, FireRate);
	DOREPLIFETIME(UGPWeaponAttributeSet, Accuracy);
	DOREPLIFETIME(UGPWeaponAttributeSet, ReloadSpeed);
	DOREPLIFETIME(UGPWeaponAttributeSet, RecoilX);
	DOREPLIFETIME(UGPWeaponAttributeSet, RecoilY);
}

void UGPWeaponAttributeSet::OnRep_CurrentMag(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGPWeaponAttributeSet, CurrentMag, OldValue);
}

void UGPWeaponAttributeSet::OnRep_MagSize(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGPWeaponAttributeSet, MagSize, OldValue);
}

void UGPWeaponAttributeSet::OnRep_FireRate(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGPWeaponAttributeSet, FireRate, OldValue);
}

void UGPWeaponAttributeSet::OnRep_Accuracy(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGPWeaponAttributeSet, Accuracy, OldValue);
}

void UGPWeaponAttributeSet::OnRep_ReloadSpeed(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGPWeaponAttributeSet, ReloadSpeed, OldValue);
}

void UGPWeaponAttributeSet::OnRep_RecoilX(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGPWeaponAttributeSet, RecoilX, OldValue);
}


void UGPWeaponAttributeSet::OnRep_RecoilY(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGPWeaponAttributeSet, RecoilY, OldValue);
}

