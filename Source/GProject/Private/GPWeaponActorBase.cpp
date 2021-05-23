// Fill out your copyright notice in the Description page of Project Settings.


#include "GPWeaponActorBase.h"
//#include "Sound/SoundCue.h"
//#include "Animation/AnimSequence.h"
#include "GPCharacterBase.h"
#include "Item/GPItemWeapon.h"
#include "Item/GPItemToken.h"
#include "Component/GPWAttachmentComponent.h"
#include "AbilitySystemGlobals.h"
#include "Ability/GPGameplayAbility.h"

// Sets default values
AGPWeaponActorBase::AGPWeaponActorBase():CurrentFireMode()
{
	W_AbilitySystemComponent = CreateDefaultSubobject<UGPAbilitySystemComponent>(TEXT("W_AbilitySystemComponent"));
	W_AbilitySystemComponent->SetIsReplicated(true);
	
	
 	//Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
}

UGPAbilitySystemComponent* AGPWeaponActorBase::GetAbilitySystemComponent() const
{
	return W_AbilitySystemComponent;
}

void AGPWeaponActorBase::PreInitializeComponents()
{
	Super::PreInitializeComponents();

	// Engine Bug Fix
	W_AttributeSet = NewObject<UGPWeaponAttributeSet>(this, TEXT("W_AttributeSet"));
}

UGPGameplayAbility* AGPWeaponActorBase::TryActivateWeapon()
{
	const TSubclassOf<UGPGameplayAbility>& AbilityClass = *WeaponAbilities.Find(CurrentFireMode);

	if (!AbilityClass) return nullptr;

	if (W_AbilitySystemComponent->TryActivateAbilityByClass(AbilityClass))
	{
		OnWeaponActivate();
		return Cast<UGPGameplayAbility>(W_AbilitySystemComponent->FindAbilitySpecFromClass(AbilityClass)->Ability);
	}

	return nullptr;
}

bool AGPWeaponActorBase::CanActivateWeapon() const
{
	if ( WeaponAbilities.Num() == 0 ) return false;

	const TSubclassOf<UGPGameplayAbility>& AbilityClass = *WeaponAbilities.Find(CurrentFireMode);

	if (!AbilityClass) return false;

	FGameplayAbilitySpec* Spec = W_AbilitySystemComponent->FindAbilitySpecFromClass(AbilityClass);

	if (!Spec) return false;

	FGameplayAbilityActorInfo* ActorInfo = W_AbilitySystemComponent->AbilityActorInfo.Get();

	if (!ActorInfo) return false; 

	return Spec->Ability->CanActivateAbility(Spec->Handle, ActorInfo);
}


void AGPWeaponActorBase::DeactivateWeapon()
{
	const TSubclassOf<UGPGameplayAbility>& AbilityClass = *WeaponAbilities.Find(CurrentFireMode);

	if (AbilityClass && W_AbilitySystemComponent)
	{
		W_AbilitySystemComponent->CancelAbilityByClass(AbilityClass);
		OnWeaponDeactivate();
	}
}

void AGPWeaponActorBase::ReloadWeapon(float inValue = -1.0f)
{
	AGPCharacterBase* OwningCharacter = GetOwner<AGPCharacterBase>();
	if(!OwningCharacter) return;

	float Delta = GetCurrentMag();
	FGameplayTagContainer TagCon;

	if (inValue < 0.0f)
		W_AttributeSet->SetCurrentMag(W_AttributeSet->GetMagSize());
	else
		W_AttributeSet->SetCurrentMag(inValue);

	Delta = GetCurrentMag() - Delta;
	HandleCurrentMagChanged(Delta, TagCon);

	OnWeaponReloadEnd();
}

bool AGPWeaponActorBase::IsWeaponHasToReload_Implementation() const
{
	return (GetCurrentMag() != GetMagSize()) && (AmmoItem.IsValid());
}

float AGPWeaponActorBase::GetCurrentMag() const
{
	return GetAbilitySystemComponent()->GetNumericAttribute(W_AttributeSet->GetCurrentMagAttribute());
}

float AGPWeaponActorBase::GetMagSize() const
{
	return GetAbilitySystemComponent()->GetNumericAttribute(W_AttributeSet->GetMagSizeAttribute());
}

EWFiringMode AGPWeaponActorBase::GetCurrentFiringMode() const
{
	const EWFiringMode* rv = FiringModeMap.Find(CurrentFireMode);
	if (!rv) return EWFiringMode();
	return *rv;
}


// Called when the game starts or when spawned
void AGPWeaponActorBase::BeginPlay()
{
	Super::BeginPlay();

	GASComponentInitialize();

	if (WeaponAbilities.Num() > 0)
		CurrentFireMode = WeaponAbilities.begin().Key();
}

// Called every frame
void AGPWeaponActorBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AGPWeaponActorBase::GASComponentInitialize()
{
	W_AbilitySystemComponent->ClearAllAbilities();
	W_AbilitySystemComponent->InitAbilityActorInfo(GetOwner(), this);
	
	FGameplayEffectContextHandle EffectContext = W_AbilitySystemComponent->MakeEffectContext();
	EffectContext.AddSourceObject(this);

	for (const TPair<EWFireType, TSubclassOf<UGPGameplayAbility>>& AbilityPair : WeaponAbilities)
	{
		if ( AbilityPair.Value->IsValidLowLevel() ) 
		{
			FGameplayAbilitySpecHandle NewHandle = W_AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(AbilityPair.Value, 1, INDEX_NONE,this));
			//GP_LOG(Warning, TEXT("%s"), *AbilityPair.Value->GetName());
			//AbilityHandles.Add(AbilityPair.Key, NewHandle);
		}
	}
	//GP_LOG(Warning, TEXT("%d"), WeaponAbilities.Num());


	FGameplayEffectSpecHandle NewHandle = W_AbilitySystemComponent->MakeOutgoingSpec(DefaultAttributes, EffectContext.GetAbilityLevel(), EffectContext);
	if (NewHandle.IsValid())
	{
		FActiveGameplayEffectHandle ActiveGEHandle = W_AbilitySystemComponent->ApplyGameplayEffectSpecToTarget(*NewHandle.Data.Get(), W_AbilitySystemComponent);
	}
}

//void AGPWeaponActorBase::UpdateCurrentMag_Implementation(float inValue = -1.0f)
//{
//	UGameplayAbility* Ability = GetSlottedAbilityInstance(CurrentWeaponSlot);
//
//	if (!Ability) return;
//
//	if (Ability->Implements<UGPMagAbilityInterface>())
//	{
//		float Delta = GetCurrentMag();
//		FGameplayTagContainer TagCon;
//
//		if (inValue < 0.0f)
//			AttributeSet->SetCurrentMag(AttributeSet->GetMagSize());
//		else
//			AttributeSet->SetCurrentMag(inValue);
//
//		Delta = GetCurrentMag() - Delta;
//		HandleCurrentMagChanged(Delta, TagCon);
//	}
//}

void AGPWeaponActorBase::HandleCurrentMagChanged(float DeltaValue, const struct FGameplayTagContainer& EventTags)
{
	if (bInitialized)
	{
		OnCurrentMagChanged(DeltaValue, EventTags);
	}
}

void AGPWeaponActorBase::HandleMagSizeChanged(float DeltaValue, const struct FGameplayTagContainer& EventTags)
{
	if (bInitialized)
	{
		OnMagSizeChanged(DeltaValue, EventTags);
	}
}

