// Fill out your copyright notice in the Description page of Project Settings.


#include "GPWeaponActorBase.h"
//#include "Sound/SoundCue.h"
//#include "Animation/AnimSequence.h"
#include "GPCharacterBase.h"
#include "Item/GPItemWeapon.h"
#include "Item/GPItemAttachment.h"
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

	//FGameplayTagContainer Tagcon;
	//FGameplayTag Tag(FName(TEXT("Ability.Weapon")));
	//Tagcon.AddTag(Tag);

	//W_AbilitySystemComponent->GetActiveEffectsWithAllTags()

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
	//AGPCharacterBase* OwningCharacter = GetOwner<AGPCharacterBase>();
	//if(!OwningCharacter) return;

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

float AGPWeaponActorBase::GetFireRate() const
{
	return GetAbilitySystemComponent()->GetNumericAttribute(W_AttributeSet->GetFireRateAttribute());
}

float AGPWeaponActorBase::GetAccuracy() const
{
	return GetAbilitySystemComponent()->GetNumericAttribute(W_AttributeSet->GetAccuracyAttribute());
}

float AGPWeaponActorBase::GetReloadSpeed() const
{
	return GetAbilitySystemComponent()->GetNumericAttribute(W_AttributeSet->GetReloadSpeedAttribute());
}

FVector2D AGPWeaponActorBase::GetRecoil() const
{
	FVector2D retVal;

	retVal.X = GetAbilitySystemComponent()->GetNumericAttribute(W_AttributeSet->GetRecoilXAttribute());
	retVal.Y = GetAbilitySystemComponent()->GetNumericAttribute(W_AttributeSet->GetRecoilYAttribute());
	return retVal;
}

float AGPWeaponActorBase::GetOpticMagnification() const
{
	return GetAbilitySystemComponent()->GetNumericAttribute(W_AttributeSet->GetOpticMgnfAttribute());
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

	if(HasAuthority())
		GASComponentInitialize();

	for (const EWAttachmentType& Type : AttachableTypes)
	{
		AttachmentSlot.Add(Type, nullptr);
	}

	if (WeaponAbilities.Num() > 0)
		CurrentFireMode = WeaponAbilities.begin().Key();
}

// Called every frame
void AGPWeaponActorBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Returns replaced item. Add to empty slot will return null
UGPItemAttachment* AGPWeaponActorBase::AddAttachment(UGPItemAttachment* inAttachmentItem)
{
	if ( !inAttachmentItem ) return nullptr;
	
	UGPWAttachmentComponent* CompCDO = inAttachmentItem->AttachmentClass->GetDefaultObject<UGPWAttachmentComponent>();
	
	UGPItemAttachment* ReplacedAttachment = RemoveAttachment(CompCDO->Type);

	// Overwrite
	AttachmentSlot.Add(CompCDO->Type, inAttachmentItem);

	UGPWAttachmentComponent* createdComp = NewObject<UGPWAttachmentComponent>(this, inAttachmentItem->AttachmentClass);
	if (createdComp)
	{
		createdComp->RegisterComponent();
		
		createdComp->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform, NAME_None);

		createdComp->CommitEffects();
	}
	
	return ReplacedAttachment;
}

UGPItemAttachment* AGPWeaponActorBase::RemoveAttachment(const EWAttachmentType& inType)
{
	TArray<UGPWAttachmentComponent*> FoundComps;

	GetComponents<UGPWAttachmentComponent>(FoundComps);

	UGPItemAttachment* RemovedAttachment = *(AttachmentSlot.Find(inType));

	for (UActorComponent* FoundComp : FoundComps)
	{
		UGPWAttachmentComponent* CastedComp = Cast<UGPWAttachmentComponent>(FoundComp);

		if (CastedComp->Type == inType)
		{
			//GP_LOG(Warning, TEXT("%s"), *CastedComp->GetName());
			CastedComp->RemoveEffects();
			CastedComp->DestroyComponent();
		}
	}

	AttachmentSlot.Add(inType, nullptr);

	return RemovedAttachment;
}

bool AGPWeaponActorBase::IsAttachableItem(UGPItemAttachment* inAttachmentItem) const
{
	UGPWAttachmentComponent* CompCDO = inAttachmentItem->AttachmentClass->GetDefaultObject<UGPWAttachmentComponent>();
	
	for ( EWeaponType Type : WeaponType )
	{
		if ( CompCDO->AttachableWeaponType.Find(Type) != INDEX_NONE ) return true;
	}

	return false;
}

void AGPWeaponActorBase::GASComponentInitialize()
{
	W_AbilitySystemComponent->ClearAllAbilities();
	W_AbilitySystemComponent->InitAbilityActorInfo(this, GetOwner());
	
	for (const TPair<EWFireType, TSubclassOf<UGPGameplayAbility>>& AbilityPair : WeaponAbilities)
	{
		if ( AbilityPair.Value->IsValidLowLevel() ) 
		{
			FGameplayAbilitySpecHandle NewHandle = W_AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(AbilityPair.Value, 1, INDEX_NONE,this));
		}
	}

	FGameplayEffectContextHandle EffectContext = W_AbilitySystemComponent->MakeEffectContext();
	EffectContext.AddSourceObject(this);

	FGameplayEffectSpecHandle NewHandle = W_AbilitySystemComponent->MakeOutgoingSpec(DefaultAttributes, EffectContext.GetAbilityLevel(), EffectContext);
	if (NewHandle.IsValid())
	{
		FActiveGameplayEffectHandle ActiveGEHandle = W_AbilitySystemComponent->ApplyGameplayEffectSpecToTarget(*NewHandle.Data.Get(), W_AbilitySystemComponent);
	}

	for (const TPair<EWAttachmentType, UGPItemAttachment*>& Attachment : AttachmentSlot)
	{
		if(!Attachment.Value->IsValidLowLevel()) continue;
		UGPWAttachmentComponent* createdComp = NewObject<UGPWAttachmentComponent>(this, Attachment.Value->AttachmentClass);
		if (createdComp)
		{
			if (createdComp->Type == Attachment.Key)
			{
				createdComp->RegisterComponent();
				createdComp->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform,NAME_None);

				createdComp->CommitEffects();
			}
			else
			{	
				createdComp->DestroyComponent();
				AttachmentSlot.Add(Attachment.Key,nullptr);
			}
		}
	}

	W_AttributeSet->SetCurrentMag(W_AttributeSet->GetMagSize());

	bInitialized=true;
}

void AGPWeaponActorBase::HandleCurrentMagChanged(float DeltaValue, const struct FGameplayTagContainer& EventTags)
{
	if (bInitialized)
	{
		OnCurrentMagChanged_D.Broadcast(DeltaValue, EventTags);
		OnCurrentMagChanged(DeltaValue, EventTags);
	}
}

void AGPWeaponActorBase::HandleMagSizeChanged(float DeltaValue, const struct FGameplayTagContainer& EventTags)
{
	if (bInitialized)
	{
		OnMagSizeChanged_D.Broadcast(DeltaValue, EventTags);
		OnMagSizeChanged(DeltaValue, EventTags);
	}
}

