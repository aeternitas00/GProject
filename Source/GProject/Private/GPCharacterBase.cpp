// Fill out your copyright notice in the Description page of Project Settings.


#include "GPCharacterBase.h"
#include "Interface/GPMagAbilityInterface.h"
#include "Item/GPItem.h"
#include "UObject/Class.h"
#include "AbilitySystemGlobals.h"
#include "Ability/GPGameplayAbility.h"

// Sets default values
AGPCharacterBase::AGPCharacterBase()
{
	// Create ability system component, and set it to be explicitly replicated
	AbilitySystemComponent = CreateDefaultSubobject<UGPAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->OnTagUpdatedDelegate.AddDynamic(this, &AGPCharacterBase::OnTagUpdated);
	// Create the attribute set, this replicates by default
	// 
	// 엔진 버그 때문에 PreInitializeComponents() 로 넘김.
	// 
	//AttributeSet = CreateDefaultSubobject<UGPAttributeSet>(TEXT("AttributeSet"));
	
	bAbilitiesInitialized = false;

	bNetLoadOnClient = false;
}

void AGPCharacterBase::PreInitializeComponents()
{
	Super::PreInitializeComponents();

	// Engine Bug Fix
	AttributeSet = NewObject<UGPAttributeSet>(this,TEXT("AttributeSet"));
}

void AGPCharacterBase::BeginPlay()
{
	GP_LOG(Warning, TEXT("%s"), *GetName());

	Super::BeginPlay();
}

void AGPCharacterBase::Restart()
{
	GP_LOG(Warning, TEXT("%s"), *GetName());

	Super::Restart();

	if (GetController())
	{
		GP_LOG(Warning, TEXT("InitAS: %s"), *GetName());
		// Initialize our abilities
		if (AbilitySystemComponent)
		{
			AbilitySystemComponent->InitAbilityActorInfo(this, this);
			//AddSlottedGameplayAbilities();
			AddStartupGameplayAbilities();
		}
	}
}

void AGPCharacterBase::PossessedBy(AController* NewController)
{
	GP_LOG(Warning, TEXT("%s"), *GetName());

	Super::PossessedBy(NewController);

	// Try setting the inventory source, this will fail for AI
	InventorySource = NewController;
	//GP_LOG(Warning, TEXT("PossessedBy Called"));

	if (InventorySource)
	{
		//GP_LOG(Warning, TEXT("PossessedBy Interface check suc"));
		InventoryUpdateHandle = InventorySource->GetSlottedItemChangedDelegate().AddUObject(this, &AGPCharacterBase::OnItemSlotChanged);
		InventoryLoadedHandle = InventorySource->GetInventoryLoadedDelegate().AddUObject(this, &AGPCharacterBase::RefreshSlottedGameplayAbilities);
	}
}

void AGPCharacterBase::UnPossessed()
{
	// Unmap from inventory source
	if (InventorySource && InventoryUpdateHandle.IsValid())
	{
		InventorySource->GetSlottedItemChangedDelegate().Remove(InventoryUpdateHandle);
		InventoryUpdateHandle.Reset();

		InventorySource->GetInventoryLoadedDelegate().Remove(InventoryLoadedHandle);
		InventoryLoadedHandle.Reset();
	}

	InventorySource = nullptr;
}

void AGPCharacterBase::OnRep_Controller()
{
	Super::OnRep_Controller();

	// Our controller changed, must update ActorInfo on AbilitySystemComponent
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->RefreshAbilityActorInfo();
	}
}

UAbilitySystemComponent* AGPCharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

float AGPCharacterBase::GetHealth() const
{
	if (!AttributeSet)
		return 1.f;

	return AttributeSet->GetHealth();
}

float AGPCharacterBase::GetMaxHealth() const
{
	return AttributeSet->GetMaxHealth();
}

float AGPCharacterBase::GetMana() const
{
	return AttributeSet->GetMana();
}

float AGPCharacterBase::GetMaxMana() const
{
	return AttributeSet->GetMaxMana();
}

float AGPCharacterBase::GetMoveSpeed() const
{
	return AttributeSet->GetMoveSpeed();
}

float AGPCharacterBase::GetCurrentMag() const
{
	return AttributeSet->GetCurrentMag();
}

float AGPCharacterBase::GetMagSize() const
{
	return AttributeSet->GetMagSize();
}

bool AGPCharacterBase::ActivateAbilitiesWithItemSlot(FGPItemSlot ItemSlot, bool bAllowRemoteActivation)
{
	FGameplayAbilitySpecHandle* FoundHandle = SlottedAbilities.Find(ItemSlot);
	
	UGameplayAbility* Ability = GetSlottedAbilityInstance(ItemSlot);

	bool rv=false;

	if (FoundHandle && AbilitySystemComponent)
	{
		rv = AbilitySystemComponent->TryActivateAbility(*FoundHandle, bAllowRemoteActivation);
	}

	return rv;
}

void AGPCharacterBase::GetActiveAbilitiesWithItemSlot(FGPItemSlot ItemSlot, TArray<UGPGameplayAbility*>& ActiveAbilities)
{
	FGameplayAbilitySpecHandle* FoundHandle = SlottedAbilities.Find(ItemSlot);

	if (FoundHandle && AbilitySystemComponent)
	{
		FGameplayAbilitySpec* FoundSpec = AbilitySystemComponent->FindAbilitySpecFromHandle(*FoundHandle);

		if (FoundSpec)
		{
			TArray<UGameplayAbility*> AbilityInstances = FoundSpec->GetAbilityInstances();

			// Find all ability instances executed from this slot
			for (UGameplayAbility* ActiveAbility : AbilityInstances)
			{
				ActiveAbilities.Add(Cast<UGPGameplayAbility>(ActiveAbility));
			}
		}
	}
}

UGameplayAbility* AGPCharacterBase::GetSlottedAbilityInstance(FGPItemSlot ItemSlot)
{
	FGameplayAbilitySpecHandle* FoundHandle = SlottedAbilities.Find(ItemSlot);

	if (FoundHandle)
	{
		FGameplayAbilitySpec* FoundSpec = AbilitySystemComponent->FindAbilitySpecFromHandle(*FoundHandle);

		//UGameplayAbility* Ability = FoundSpec->Ability;

		//GP_LOG(Warning,TEXT("%s"),*Ability->GetName());

		if (!FoundSpec) return nullptr;

		if (FoundSpec->Ability->IsValidLowLevel())
			return FoundSpec->Ability;
	}		
	return nullptr;
}


bool AGPCharacterBase::ActivateAbilitiesWithTags(FGameplayTagContainer AbilityTags, bool bAllowRemoteActivation)
{
	if (AbilitySystemComponent)
	{
		return AbilitySystemComponent->TryActivateAbilitiesByTag(AbilityTags, bAllowRemoteActivation);
	}

	return false;
}

void AGPCharacterBase::GetActiveAbilitiesWithTags(FGameplayTagContainer AbilityTags, TArray<UGPGameplayAbility*>& ActiveAbilities)
{
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->GetActiveAbilitiesWithTags(AbilityTags, ActiveAbilities);
	}
}

bool AGPCharacterBase::GetCooldownRemainingForTag(FGameplayTagContainer CooldownTags, float& TimeRemaining, float& CooldownDuration)
{
	if (AbilitySystemComponent && CooldownTags.Num() > 0)
	{
		TimeRemaining = 0.f;
		CooldownDuration = 0.f;

		FGameplayEffectQuery const Query = FGameplayEffectQuery::MakeQuery_MatchAnyOwningTags(CooldownTags);
		TArray< TPair<float, float> > DurationAndTimeRemaining = AbilitySystemComponent->GetActiveEffectsTimeRemainingAndDuration(Query);
		if (DurationAndTimeRemaining.Num() > 0)
		{
			int32 BestIdx = 0;
			float LongestTime = DurationAndTimeRemaining[0].Key;
			for (int32 Idx = 1; Idx < DurationAndTimeRemaining.Num(); ++Idx)
			{
				if (DurationAndTimeRemaining[Idx].Key > LongestTime)
				{
					LongestTime = DurationAndTimeRemaining[Idx].Key;
					BestIdx = Idx;
				}
			}

			TimeRemaining = DurationAndTimeRemaining[BestIdx].Key;
			CooldownDuration = DurationAndTimeRemaining[BestIdx].Value;

			return true;
		}
	}
	return false;
}

void AGPCharacterBase::UpdateCurrentMag_Implementation(float inValue = -1.0f)
{
	UGameplayAbility* Ability = GetSlottedAbilityInstance(CurrentWeaponSlot);
	
	if (!Ability) return;

	if (Ability->Implements<UGPMagAbilityInterface>())
	{
		float Delta = GetCurrentMag();
		FGameplayTagContainer TagCon;

		if( inValue < 0.0f )
			AttributeSet->SetCurrentMag(AttributeSet->GetMagSize());
		else
			AttributeSet->SetCurrentMag(inValue);

		Delta = GetCurrentMag()- Delta;
		HandleCurrentMagChanged(Delta, TagCon);
	}

}

void AGPCharacterBase::UpdateMagSize()
{
	UGameplayAbility* Ability = GetSlottedAbilityInstance(CurrentWeaponSlot);

	float Delta = GetMagSize();
	FGameplayTagContainer TagCon;

	if (!Ability) return;

	if (Ability->Implements<UGPMagAbilityInterface>())
	{
		AttributeSet->SetMagSize(IGPMagAbilityInterface::Execute_GetMagSize(Ability));
	}
	else
	{
		AttributeSet->SetMagSize(0.0f);
	}

	Delta = GetMagSize() - Delta;
	HandleMagSizeChanged(Delta, TagCon);
}

void AGPCharacterBase::FillSlottedAbilitySpecs(TMap<FGPItemSlot, FGameplayAbilitySpec>& SlottedAbilitySpecs)
{
	// First add default ones
	for (const TPair<FGPItemSlot, TSubclassOf<UGPGameplayAbility>>& DefaultPair : DefaultSlottedAbilities)
	{
		if (DefaultPair.Value.Get())
		{
			SlottedAbilitySpecs.Add(DefaultPair.Key, FGameplayAbilitySpec(DefaultPair.Value, 1/*GetCharacterLevel()*/, INDEX_NONE, this));
		}
	}

	// Now potentially override with inventory
	if (InventorySource)
	{
		const TMap<FGPItemSlot, UGPItem*>& SlottedItemMap = InventorySource->GetSlottedItemMap();

		//GP_LOG(Warning,TEXT("Slotted Item Maps num is %d"), SlottedItemMap.Num());

		for (const TPair<FGPItemSlot, UGPItem*>& ItemPair : SlottedItemMap)
		{
			UGPItem* SlottedItem = ItemPair.Value;

			// Use the character level as default
			
			int32 AbilityLevel = 1;//GetCharacterLevel();

			if (SlottedItem)
			{
				// Override the ability level to use the data from the slotted item
				AbilityLevel = SlottedItem->AbilityLevel;

				if (SlottedItem->GrantedAbility)
				{
					// This will override anything from default
					SlottedAbilitySpecs.Add(ItemPair.Key, FGameplayAbilitySpec(SlottedItem->GrantedAbility, AbilityLevel, INDEX_NONE, SlottedItem));


					//GP_LOG(Warning, TEXT("Slotted Items name is %s"), *SlottedItem->ItemName.ToString());
				}
			}
			
			//int32 AbilityLevel = 1;//GetCharacterLevel();

			//if (SlottedItem && SlottedItem->ItemType.GetName() == FName(TEXT("Weapon")))
			//{
			//	// Override the ability level to use the data from the slotted item
			//	AbilityLevel = SlottedItem->AbilityLevel;
			//}

			//if (SlottedItem && SlottedItem->GrantedAbility)
			//{
			//	// This will override anything from default
			//	SlottedAbilitySpecs.Add(ItemPair.Key, FGameplayAbilitySpec(SlottedItem->GrantedAbility, AbilityLevel, INDEX_NONE, SlottedItem));
			//}
		}
	}
}

void AGPCharacterBase::OnItemSlotChanged(FGPItemSlot ItemSlot, UGPItem* Item)
{
	RefreshSlottedGameplayAbilities();
}

void AGPCharacterBase::RefreshSlottedGameplayAbilities()
{
	if (bAbilitiesInitialized)
	{
		// Refresh any invalid abilities and adds new ones
		RemoveSlottedGameplayAbilities(false);
		AddSlottedGameplayAbilities();
	}
}

void AGPCharacterBase::AddSlottedGameplayAbilities()
{
	TMap<FGPItemSlot, FGameplayAbilitySpec> SlottedAbilitySpecs;
	FillSlottedAbilitySpecs(SlottedAbilitySpecs);

	// Now add abilities if needed
	for (const TPair<FGPItemSlot, FGameplayAbilitySpec>& SpecPair : SlottedAbilitySpecs)
	{
		FGameplayAbilitySpecHandle& SpecHandle = SlottedAbilities.FindOrAdd(SpecPair.Key);

		if (!SpecHandle.IsValid() && GetLocalRole() == ROLE_Authority)
		{
			SpecHandle = AbilitySystemComponent->GiveAbility(SpecPair.Value);
			
		}
	}
}

void AGPCharacterBase::RemoveSlottedGameplayAbilities(bool bRemoveAll)
{
	TMap<FGPItemSlot, FGameplayAbilitySpec> SlottedAbilitySpecs;

	if (!bRemoveAll)
	{
		// Fill in map so we can compare
		FillSlottedAbilitySpecs(SlottedAbilitySpecs);
	}

	for (TPair<FGPItemSlot, FGameplayAbilitySpecHandle>& ExistingPair : SlottedAbilities)
	{
		FGameplayAbilitySpec* FoundSpec = AbilitySystemComponent->FindAbilitySpecFromHandle(ExistingPair.Value);
		bool bShouldRemove = bRemoveAll || !FoundSpec;

		if (!bShouldRemove)
		{
			// Need to check desired ability specs, if we got here FoundSpec is valid
			FGameplayAbilitySpec* DesiredSpec = SlottedAbilitySpecs.Find(ExistingPair.Key);

			if (!DesiredSpec || DesiredSpec->Ability != FoundSpec->Ability || DesiredSpec->SourceObject != FoundSpec->SourceObject)
			{
				bShouldRemove = true;
			}
		}

		if (bShouldRemove)
		{
			if (FoundSpec)
			{
				// Need to remove registered ability
				AbilitySystemComponent->ClearAbility(ExistingPair.Value);
			}

			// Make sure handle is cleared even if ability wasn't found
			ExistingPair.Value = FGameplayAbilitySpecHandle();
		}
	}
}

void AGPCharacterBase::AddStartupGameplayAbilities()
{
	check(AbilitySystemComponent);

	if (!bAbilitiesInitialized)
	{
		// Grant abilities, but only on the server	
		if (GetLocalRole() == ROLE_Authority)
		{
			for (TPair<TSubclassOf<UGPGameplayAbility>, int32>& StartupAbility : GameplayAbilities)
			{
				AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(StartupAbility.Key, StartupAbility.Value, INDEX_NONE, this));
			}
		}
		
		// Now apply passives
		for (TSubclassOf<UGameplayEffect>& GameplayEffect : PassiveGameplayEffects)
		{
			FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
			EffectContext.AddSourceObject(this);

			FGameplayEffectSpecHandle NewHandle = AbilitySystemComponent->MakeOutgoingSpec(GameplayEffect, EffectContext.GetAbilityLevel(), EffectContext);
			if (NewHandle.IsValid())
			{
				FActiveGameplayEffectHandle ActiveGEHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToTarget(*NewHandle.Data.Get(), AbilitySystemComponent);
			}
		}

		AddSlottedGameplayAbilities();

		bAbilitiesInitialized = true;
	}
}

void AGPCharacterBase::HandleDamage(float DamageAmount, const FHitResult& HitInfo, const struct FGameplayTagContainer& DamageTags, AGPCharacterBase* InstigatorPawn, AActor* DamageCauser)
{
	OnDamaged(DamageAmount, HitInfo, DamageTags, InstigatorPawn, DamageCauser);
}

void AGPCharacterBase::HandleHealthChanged(float DeltaValue, const struct FGameplayTagContainer& EventTags)
{
	// We only call the BP callback if this is not the initial ability setup
	if (bAbilitiesInitialized)
	{
		OnHealthChanged(DeltaValue, EventTags);
	}
}

void AGPCharacterBase::HandleManaChanged(float DeltaValue, const struct FGameplayTagContainer& EventTags)
{
	if (bAbilitiesInitialized)
	{
		OnManaChanged(DeltaValue, EventTags);
	}
}

void AGPCharacterBase::HandleMoveSpeedChanged(float DeltaValue, const struct FGameplayTagContainer& EventTags)
{
	GP_LOG(Warning, TEXT("C: %f, B: %f"), GetMoveSpeed(), AttributeSet->MoveSpeed.GetBaseValue());
	SetMovementSpeed(GetMoveSpeed());

	if (bAbilitiesInitialized)
	{
		OnMoveSpeedChanged(DeltaValue, EventTags);
	}
}

void AGPCharacterBase::SetMovementSpeed(float Speed)
{
	GP_LOG(Warning, TEXT("R: %f, Ch: %f"), Speed, GetMoveSpeed());

	GetCharacterMovement()->MaxWalkSpeed = Speed;
}

void AGPCharacterBase::HandleCurrentMagChanged(float DeltaValue, const struct FGameplayTagContainer& EventTags)
{
	if (bAbilitiesInitialized)
	{
		OnCurrentMagChanged(DeltaValue, EventTags);
	}
}

void AGPCharacterBase::HandleMagSizeChanged(float DeltaValue, const struct FGameplayTagContainer& EventTags)
{
	if (bAbilitiesInitialized)
	{
		OnMagSizeChanged(DeltaValue, EventTags);
	}
}

