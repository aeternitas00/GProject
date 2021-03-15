// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GProject.h"
#include "Types/GPTypes.h" // TODO : REMOVE THIS
#include "GameFramework/Character.h"
#include "UObject/ScriptInterface.h"
#include "GPInventoryInterface.h"
#include "AbilitySystemInterface.h"
#include "Component/GPAbilitySystemComponent.h"
#include "GPAttributeSet.h"
#include "GPCharacterBase.generated.h"

class UGPAbilitySystemComponent;
class UGPGameplayAbility;
class UGameplayEffect;

UCLASS()
class GPROJECT_API AGPCharacterBase : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AGPCharacterBase();
	virtual void PossessedBy(AController* NewController) override;

	// Implement IAbilitySystemInterface
	UAbilitySystemComponent* GetAbilitySystemComponent() const override;

protected:

	/** Remove slotted gameplay abilities, if force is false it only removes invalid ones */
	void RemoveSlottedGameplayAbilities(bool bRemoveAll);

	/** Adds slotted item abilities if needed */
	void AddSlottedGameplayAbilities();

	/** Fills in with ability specs, based on defaults and inventory */
	void FillSlottedAbilitySpecs(TMap<FGPItemSlot, FGameplayAbilitySpec>& SlottedAbilitySpecs);

	/** Called when slotted items change, bound to delegate on interface */
	void OnItemSlotChanged(FGPItemSlot ItemSlot, UGPItem* Item);
	void RefreshSlottedGameplayAbilities();

public:	
	/** Returns current health, will be 0 if dead */
	UFUNCTION(BlueprintCallable)
	virtual float GetHealth() const;

	/** Returns maximum health, health will never be greater than this */
	UFUNCTION(BlueprintCallable)
	virtual float GetMaxHealth() const;

	/** Returns current mana */
	UFUNCTION(BlueprintCallable)
	virtual float GetMana() const;

	/** Returns maximum mana, mana will never be greater than this */
	UFUNCTION(BlueprintCallable)
	virtual float GetMaxMana() const;

	/** Returns current movement speed */
	UFUNCTION(BlueprintCallable)
	virtual float GetMoveSpeed() const;

	/**
	 * Attempts to activate any ability in the specified item slot. Will return false if no activatable ability found or activation fails
	 * Returns true if it thinks it activated, but it may return false positives due to failure later in activation.
	 * If bAllowRemoteActivation is true, it will remotely activate local/server abilities, if false it will only try to locally activate the ability
	 */
	UFUNCTION(BlueprintCallable, Category = "Abilities")
	bool ActivateAbilitiesWithItemSlot(FGPItemSlot ItemSlot, bool bAllowRemoteActivation = true);

	/** Returns a list of active abilities bound to the item slot. This only returns if the ability is currently running */
	UFUNCTION(BlueprintCallable, Category = "Abilities")
	void GetActiveAbilitiesWithItemSlot(FGPItemSlot ItemSlot, TArray<UGPGameplayAbility*>& ActiveAbilities);

	/**
	 * Attempts to activate all abilities that match the specified tags
	 * Returns true if it thinks it activated, but it may return false positives due to failure later in activation.
	 * If bAllowRemoteActivation is true, it will remotely activate local/server abilities, if false it will only try to locally activate the ability
	 */
	UFUNCTION(BlueprintCallable, Category = "Abilities")
	bool ActivateAbilitiesWithTags(FGameplayTagContainer AbilityTags, bool bAllowRemoteActivation);

	/** Returns a list of active abilities matching the specified tags. This only returns if the ability is currently running */
	UFUNCTION(BlueprintCallable, Category = "Abilities")
	void GetActiveAbilitiesWithTags(FGameplayTagContainer AbilityTags, TArray<UGPGameplayAbility*>& ActiveAbilities);

protected:
	UPROPERTY()
	UGPAbilitySystemComponent* AbilitySystemComponent;

	/** Abilities to grant to this character on creation. These will be activated by tag or event and are not bound to specific inputs */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Abilities)
	TArray<TSubclassOf<UGPGameplayAbility>> GameplayAbilities;

	/** Map of item slot to gameplay ability class, these are bound before any abilities added by the inventory */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Abilities)
	TMap<FGPItemSlot, TSubclassOf<UGPGameplayAbility>> DefaultSlottedAbilities;

	/** Passive gameplay effects applied on creation */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Abilities)
	TArray<TSubclassOf<UGameplayEffect>> PassiveGameplayEffects;

	/** Map of slot to ability granted by that slot. I may refactor this later */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Inventory)
	TMap<FGPItemSlot, FGameplayAbilitySpecHandle> SlottedAbilities;

	/** If true we have initialized our abilities */
	UPROPERTY()
	int32 bAbilitiesInitialized;

	/** List of attributes modified by the ability system */
	UPROPERTY()
	UGPAttributeSet* AttributeSet;

	/** Cached pointer to the inventory source for this character, can be null */
	UPROPERTY()
	TScriptInterface<IGPInventoryInterface> InventorySource;

	/** Delegate handles */
	FDelegateHandle InventoryUpdateHandle;
	FDelegateHandle InventoryLoadedHandle;

	// Friended to allow access to handle functions above
	friend UGPAttributeSet;
};
