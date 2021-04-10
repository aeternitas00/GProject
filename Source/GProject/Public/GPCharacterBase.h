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
	virtual void UnPossessed() override;
	virtual void OnRep_Controller() override;
	//virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PreInitializeComponents() override;
	// Implement IAbilitySystemInterface
	UAbilitySystemComponent* GetAbilitySystemComponent() const override;

protected:

	/** Remove slotted gameplay abilities, if force is false it only removes invalid ones */
	void RemoveSlottedGameplayAbilities(bool bRemoveAll);

	/** Adds slotted item abilities if needed */
	void AddSlottedGameplayAbilities();

	/** Fills in with ability specs, based on defaults and inventory */
	void FillSlottedAbilitySpecs(TMap<FGPItemSlot, FGameplayAbilitySpec>& SlottedAbilitySpecs);

	/** Apply the startup gameplay abilities and effects */
	void AddStartupGameplayAbilities();

	/** Called when slotted items change, bound to delegate on interface */
	void OnItemSlotChanged(FGPItemSlot ItemSlot, UGPItem* Item);
	void RefreshSlottedGameplayAbilities();

	/**
		* Called when character takes damage, which may have killed them
		*
		* @param DamageAmount Amount of damage that was done, not clamped based on current health
		* @param HitInfo The hit info that generated this damage
		* @param DamageTags The gameplay tags of the event that did the damage
		* @param InstigatorCharacter The character that initiated this damage
		* @param DamageCauser The actual actor that did the damage, might be a weapon or projectile
	*/
	UFUNCTION(BlueprintImplementableEvent)
	void OnDamaged(float DamageAmount, const FHitResult& HitInfo, const struct FGameplayTagContainer& DamageTags, AGPCharacterBase* InstigatorCharacter, AActor* DamageCauser);
	// Called from GPAttributeSet, these call BP events above
	virtual void HandleDamage(float DamageAmount, const FHitResult& HitInfo, const struct FGameplayTagContainer& DamageTags, AGPCharacterBase* InstigatorCharacter, AActor* DamageCauser);

	/**
		* Called when health is changed, either from healing or from being damaged
		* For damage this is called in addition to OnDamaged/OnKilled
		*
		* @param DeltaValue Change in health value, positive for heal, negative for cost. If 0 the delta is unknown
		* @param EventTags The gameplay tags of the event that changed mana
	*/
	UFUNCTION(BlueprintImplementableEvent)
	void OnHealthChanged(float DeltaValue, const struct FGameplayTagContainer& EventTags);
	
	virtual void HandleHealthChanged(float DeltaValue, const struct FGameplayTagContainer& EventTags);

	/**
		* Called when mana is changed, either from healing or from being used as a cost
		*
		* @param DeltaValue Change in mana value, positive for heal, negative for cost. If 0 the delta is unknown
		* @param EventTags The gameplay tags of the event that changed mana
	*/
	UFUNCTION(BlueprintImplementableEvent)
	void OnManaChanged(float DeltaValue, const struct FGameplayTagContainer& EventTags);

	// Called from RPGAttributeSet, these call BP events above
	virtual void HandleManaChanged(float DeltaValue, const struct FGameplayTagContainer& EventTags);

	/**
		* Called when movement speed is changed
		*
		* @param DeltaValue Change in move speed
		* @param EventTags The gameplay tags of the event that changed mana
	*/
	UFUNCTION(BlueprintImplementableEvent)
	void OnMoveSpeedChanged(float DeltaValue, const struct FGameplayTagContainer& EventTags);

	// Called from RPGAttributeSet, these call BP events above
	virtual void HandleMoveSpeedChanged(float DeltaValue, const struct FGameplayTagContainer& EventTags);
	

	UFUNCTION(BlueprintImplementableEvent)
	void OnCurrentMagChanged(float DeltaValue, const struct FGameplayTagContainer& EventTags);

	virtual void HandleCurrentMagChanged(float DeltaValue, const struct FGameplayTagContainer& EventTags);

	UFUNCTION(BlueprintImplementableEvent)
	void OnMagSizeChanged(float DeltaValue, const struct FGameplayTagContainer& EventTags);

	virtual void HandleMagSizeChanged(float DeltaValue, const struct FGameplayTagContainer& EventTags);

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

	UFUNCTION(BlueprintCallable)
	virtual float GetCurrentMag() const;

	UFUNCTION(BlueprintCallable)
	virtual float GetMagSize() const;

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void UpdateMagSize();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Weapon")
	void UpdateCurrentMag(float inValue = -1.0f);

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

	UFUNCTION(BlueprintCallable, Category = "Abilities")
	UGameplayAbility* GetSlottedAbilityInstance(FGPItemSlot ItemSlot);

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

	/** Returns total time and remaining time for cooldown tags. Returns false if no active cooldowns found */
	UFUNCTION(BlueprintCallable, Category = "Abilities")
	bool GetCooldownRemainingForTag(FGameplayTagContainer CooldownTags, float& TimeRemaining, float& CooldownDuration);

protected:
	UPROPERTY()
	UGPAbilitySystemComponent* AbilitySystemComponent;

	/** Abilities to grant to this character on creation. These will be activated by tag or event and are not bound to specific inputs */
	// NPC 스킬 같은 것들, 바운드 하게 개조 가능
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Abilities)
	TMap<TSubclassOf<UGPGameplayAbility>,int32> GameplayAbilities;
	// TODO : with Ability level data

	/** Map of item slot to gameplay ability class, these are bound before any abilities added by the inventory */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Abilities)
	TMap<FGPItemSlot, TSubclassOf<UGPGameplayAbility>> DefaultSlottedAbilities;

	/** Passive gameplay effects applied on creation */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Abilities)
	TArray<TSubclassOf<UGameplayEffect>> PassiveGameplayEffects;

	/** Map of slot to ability granted by that slot. I may refactor this later */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Inventory)
	TMap<FGPItemSlot, FGameplayAbilitySpecHandle> SlottedAbilities;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Weapon)
	AActor* CurrentWeaponActor;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Weapon)
	FGPItemSlot CurrentWeaponSlot;

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
