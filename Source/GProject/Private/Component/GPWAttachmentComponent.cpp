// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/GPWAttachmentComponent.h"


// Sets default values for this component's properties
UGPWAttachmentComponent::UGPWAttachmentComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UGPWAttachmentComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UGPWAttachmentComponent::CommitEffects_Implementation()
{
	UGPAbilitySystemComponent* ASC = Cast<AGPWeaponActorBase>(GetOwner())->GetAbilitySystemComponent();

	for (TSubclassOf<UGameplayEffect> Effect : PassiveEffects)
	{
		if (!Effect) continue;

		FGameplayEffectContextHandle EffectContext = ASC->MakeEffectContext();
		EffectContext.AddSourceObject(this);

		FGameplayEffectSpecHandle NewHandle = ASC->MakeOutgoingSpec(Effect, EffectContext.GetAbilityLevel(), EffectContext);
		if (NewHandle.IsValid())
		{
			FActiveGameplayEffectHandle ActiveGEHandle = ASC->ApplyGameplayEffectSpecToTarget(*NewHandle.Data.Get(), ASC);
		}

	}
}

void UGPWAttachmentComponent::RemoveEffects_Implementation()
{
	UGPAbilitySystemComponent* ASC = Cast<AGPWeaponActorBase>(GetOwner())->GetAbilitySystemComponent();

	for (FActiveGameplayEffectHandle ActiveEffect : ActiveEffects)
	{
		if (ActiveEffect.IsValid())
		{
			ASC->RemoveActiveGameplayEffect(ActiveEffect);
		}
	}
}

// Called every frame
//void UGPWAttachmentComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
//{
//	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
//
//	// ...
//}

