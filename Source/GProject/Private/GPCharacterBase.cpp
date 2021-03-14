// Fill out your copyright notice in the Description page of Project Settings.


#include "GPCharacterBase.h"
#include "GPItem.h"
#include "AbilitySystemGlobals.h"
#include "GPGameplayAbility.h"

// Sets default values
AGPCharacterBase::AGPCharacterBase()
{
	// Create ability system component, and set it to be explicitly replicated
	AbilitySystemComponent = CreateDefaultSubobject<UGPAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);

	// Create the attribute set, this replicates by default
	AttributeSet = CreateDefaultSubobject<UGPAttributeSet>(TEXT("AttributeSet"));

	bAbilitiesInitialized = false;
}

void AGPCharacterBase::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	//// Try setting the inventory source, this will fail for AI
	//InventorySource = NewController;

	//if (InventorySource)
	//{
	//	InventoryUpdateHandle = InventorySource->GetSlottedItemChangedDelegate().AddUObject(this, &ARPGCharacterBase::OnItemSlotChanged);
	//	InventoryLoadedHandle = InventorySource->GetInventoryLoadedDelegate().AddUObject(this, &ARPGCharacterBase::RefreshSlottedGameplayAbilities);
	//}

	// Initialize our abilities
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->InitAbilityActorInfo(this, this);
		AddSlottedGameplayAbilities();
		//AddStartupGameplayAbilities();
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

bool AGPCharacterBase::ActivateAbilitiesWithItemSlot(FGPItemSlot ItemSlot, bool bAllowRemoteActivation)
{
	FGameplayAbilitySpecHandle* FoundHandle = SlottedAbilities.Find(ItemSlot);

	if (FoundHandle && AbilitySystemComponent)
	{
		return AbilitySystemComponent->TryActivateAbility(*FoundHandle, bAllowRemoteActivation);
	}

	return false;
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
	//if (InventorySource)
	//{
	//	const TMap<FGPItemSlot, UGPItem*>& SlottedItemMap = InventorySource->GetSlottedItemMap();

	//	for (const TPair<FGPItemSlot, UGPItem*>& ItemPair : SlottedItemMap)
	//	{
	//		UGPItem* SlottedItem = ItemPair.Value;

	//		// Use the character level as default
	//		int32 AbilityLevel = 1;//GetCharacterLevel();

	//		if (SlottedItem && SlottedItem->ItemType.GetName() == FName(TEXT("Weapon")))
	//		{
	//			// Override the ability level to use the data from the slotted item
	//			AbilityLevel = SlottedItem->AbilityLevel;
	//		}

	//		if (SlottedItem && SlottedItem->GrantedAbility)
	//		{
	//			// This will override anything from default
	//			SlottedAbilitySpecs.Add(ItemPair.Key, FGameplayAbilitySpec(SlottedItem->GrantedAbility, AbilityLevel, INDEX_NONE, SlottedItem));
	//		}
	//	}
	//}
}

void AGPCharacterBase::AddSlottedGameplayAbilities()
{
	TMap<FGPItemSlot, FGameplayAbilitySpec> SlottedAbilitySpecs;
	FillSlottedAbilitySpecs(SlottedAbilitySpecs);

	// Now add abilities if needed
	for (const TPair<FGPItemSlot, FGameplayAbilitySpec>& SpecPair : SlottedAbilitySpecs)
	{
		FGameplayAbilitySpecHandle& SpecHandle = SlottedAbilities.FindOrAdd(SpecPair.Key);

		if (!SpecHandle.IsValid())
		{
			SpecHandle = AbilitySystemComponent->GiveAbility(SpecPair.Value);
		}
	}
}

void AGPCharacterBase::RemoveSlottedGameplayAbilities(bool bRemoveAll)
{
	//TMap<FGPItemSlot, FGameplayAbilitySpec> SlottedAbilitySpecs;

	//if (!bRemoveAll)
	//{
	//	// Fill in map so we can compare
	//	FillSlottedAbilitySpecs(SlottedAbilitySpecs);
	//}

	//for (TPair<FGPItemSlot, FGameplayAbilitySpecHandle>& ExistingPair : SlottedAbilities)
	//{
	//	FGameplayAbilitySpec* FoundSpec = AbilitySystemComponent->FindAbilitySpecFromHandle(ExistingPair.Value);
	//	bool bShouldRemove = bRemoveAll || !FoundSpec;

	//	if (!bShouldRemove)
	//	{
	//		// Need to check desired ability specs, if we got here FoundSpec is valid
	//		FGameplayAbilitySpec* DesiredSpec = SlottedAbilitySpecs.Find(ExistingPair.Key);

	//		if (!DesiredSpec || DesiredSpec->Ability != FoundSpec->Ability || DesiredSpec->SourceObject != FoundSpec->SourceObject)
	//		{
	//			bShouldRemove = true;
	//		}
	//	}

	//	if (bShouldRemove)
	//	{
	//		if (FoundSpec)
	//		{
	//			// Need to remove registered ability
	//			AbilitySystemComponent->ClearAbility(ExistingPair.Value);
	//		}

	//		// Make sure handle is cleared even if ability wasn't found
	//		ExistingPair.Value = FGameplayAbilitySpecHandle();
	//	}
	//}
}